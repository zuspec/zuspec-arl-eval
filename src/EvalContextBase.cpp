/*
 * EvalContextBase.cpp
 *
 * Copyright 2023 Matthew Ballance and Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may 
 * not use this file except in compliance with the License.  
 * You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
 * See the License for the specific language governing permissions and 
 * limitations under the License.
 *
 * Created on:
 *     Author:
 */
#include "dmgr/impl/DebugMacros.h"
#include "EvalContextBase.h"
#include "EvalStackFrame.h"


namespace zsp {
namespace arl {
namespace eval {


EvalContextBase::EvalContextBase(
    dmgr::IDebugMgr                     *dmgr,
    vsc::solvers::IFactory              *solvers_f,
    dm::IContext                        *ctxt,
    const vsc::solvers::IRandState      *randstate,
    IEvalBackend                        *backend) :
        m_dmgr(dmgr), m_solvers_f(solvers_f), m_ctxt(ctxt),
        m_randstate(randstate), m_backend(backend),
        m_initial(true) {
    const std::vector<std::string> functions = {
        "write8", "write16", "write32", "write64",
        "read8", "read16", "read32", "read64",
        "pss::core::reg_write", "pss::core::reg_write_val", 
        "pss::core::reg_read", "pss::core::reg_read_val",
        "pss::core::reg_group::set_handle"
    };

    for (EvalContextFunc f=(EvalContextFunc)0; f!=EvalContextFunc::NumFunctions; 
        f=(EvalContextFunc)((int)f+1)) {
        m_functions[(int)f] = ctxt->findDataTypeFunction(functions[(int)f]);
    }
    m_func_impl[(int)EvalContextFunc::RegGroupSetHandle] = std::bind(
        &EvalContextBase::RegGroupSetHandle, this, 
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
    );
}

EvalContextBase::~EvalContextBase() {

}

void EvalContextBase::pushEval(IEval *e, bool owned) {
    DEBUG_ENTER("pushEval %p", e);
    e->setIdx(m_eval_s.size());
    m_eval_s.push_back(IEvalUP(e, owned));
    DEBUG_LEAVE("pushEval %p", e);
}

void EvalContextBase::suspendEval(IEval *e) {
    DEBUG_ENTER("suspendEval");
    m_eval_s.at(e->getIdx()) = IEvalUP(e->clone(), true);
    m_eval_s.at(e->getIdx())->clrResult();
    DEBUG("haveResult: %d", m_eval_s.at(e->getIdx())->haveResult());
    DEBUG_LEAVE("suspendEval");
}

void EvalContextBase::popEval(IEval *e) {
    DEBUG_ENTER("popEval %p sz=%d", e, m_eval_s.size());
    m_eval_s.pop_back();
    if (e->haveResult()) {
        setResult(e->moveResult());
    } else {
        DEBUG("NOT hasResult");
    }
    DEBUG_LEAVE("popEval");
}

void EvalContextBase::setFunctionData(
        dm::IDataTypeFunction       *func_t,
        IEvalFunctionData           *data) {

}

void EvalContextBase::callListener(
    const std::function<void (IEvalListener *)> &f) {

    for (std::vector<IEvalListener *>::const_iterator
        it=m_listeners.begin();
        it!=m_listeners.end(); it++) {
        f(*it);
    }
}

dm::IDataTypeFunction *EvalContextBase::getFunction(EvalContextFunc func) {
    return m_functions[(int)func];
}

void EvalContextBase::setResult(const vsc::dm::ValRef &r) {
    DEBUG_ENTER("setResult sz=%d", m_eval_s.size());
    if (m_eval_s.size()) {
        m_eval_s.back()->setResult(r);
    } else {
        m_result.set(r);
    }
    DEBUG_LEAVE("setResult");
}

void EvalContextBase::setVoidResult() {
    vsc::dm::IDataTypeInt *i32 = m_ctxt->findDataTypeInt(true, 32);
    if (!i32) {
        i32 = m_ctxt->mkDataTypeInt(true, 32);
        m_ctxt->addDataTypeInt(i32);
    }
    vsc::dm::ValRefInt v(0, i32, vsc::dm::ValRef::Flags::None);
    setResult(v);
}    

IEvalStackFrame *EvalContextBase::stackFrame(int32_t idx) {
    DEBUG_ENTER("stackFrame: idx=%d m_callstack.size=%d", idx, m_callstack.size());
    if (idx < m_callstack.size()) {
        return m_callstack.at(m_callstack.size()-idx-1).get();
    } else {
        return 0;
    }
}

int32_t EvalContextBase::evalMethodCallContext(
        dm::IDataTypeFunction                   *method,
        vsc::dm::IModelField                    *method_ctxt,
        const std::vector<vsc::dm::ITypeExpr *> &params) {
    return -1;
}

vsc::dm::ValRefInt EvalContextBase::mkValRefInt(
        int64_t value, 
        bool is_signed, 
        int32_t width) {
    return m_ctxt->mkValRefInt(value, is_signed, width);
}

IEvalStackFrame *EvalContextBase::mkStackFrame(int32_t n_vars) {
    return new(n_vars) EvalStackFrame(n_vars);
}

void EvalContextBase::callFuncReq(
            IEvalThread                         *thread,
            dm::IDataTypeFunction               *func_t,
            const std::vector<vsc::dm::ValRef>  &params) {
    DEBUG_ENTER("callFuncReq");
    if (func_t->hasFlags(dm::DataTypeFunctionFlags::Core)) {
        for (uint32_t i=0; i<(int)EvalContextFunc::NumFunctions; i++) {
            if (m_functions[i] == func_t) {
                const FuncT &func = m_func_impl[i];
                if (func) {
                    DEBUG("Calling internal function %s", func_t->name().c_str());
                    func(thread, func_t, params);
                } else {
                    DEBUG("No implementation for %s", func_t->name().c_str());
                }
            }
        }
        // Internally-implemented function
    } else {
        // Delegate to backend
        getBackend()->callFuncReq(thread, func_t, params);
    }

    DEBUG_LEAVE("callFuncReq");
}

void EvalContextBase::RegGroupSetHandle(
            IEvalThread                         *thread,
            dm::IDataTypeFunction               *func_t,
            const std::vector<vsc::dm::ValRef>  &params) {
    DEBUG_ENTER("RegGroupSetHandle");
    DEBUG_LEAVE("RegGroupSetHandle");
}

dmgr::IDebug *EvalContextBase::m_dbg = 0;

}
}
}
