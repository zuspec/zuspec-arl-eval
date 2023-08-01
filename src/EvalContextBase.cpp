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
#include "EvalResult.h"
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
    m_eval_s.at(e->getIdx()) = IEvalUP(e->clone(), true);
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

void EvalContextBase::setResult(IEvalResult *r) {
    DEBUG_ENTER("setResult sz=%d", m_eval_s.size());
    if (m_eval_s.size()) {
        m_eval_s.back()->setResult(r);
    } else {
        m_result = IEvalResultUP(r);
    }
    DEBUG_LEAVE("setResult");
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

IEvalResult *EvalContextBase::mkEvalResultVal(const vsc::dm::IModelVal *val) {
    return new(&m_result_alloc, (val)?val->bits():0) EvalResult(
        &m_result_alloc,
        val);
}

IEvalResult *EvalContextBase::mkEvalResultKind(EvalResultKind kind) {
    return new(&m_result_alloc, 0) EvalResult(
        &m_result_alloc,
        kind);
}

IEvalResult *EvalContextBase::mkEvalResultRef(vsc::dm::IModelField *ref) {
    return new(&m_result_alloc, 0) EvalResult(
        &m_result_alloc,
        ref);
}

IEvalResult *EvalContextBase::mkEvalResultValS(int64_t val, int32_t bits) {
    return new(&m_result_alloc, bits) EvalResult(
        &m_result_alloc,
        bits,
        val);
}

IEvalResult *EvalContextBase::mkEvalResultValU(uint64_t val, int32_t bits) {
    return new(&m_result_alloc, bits) EvalResult(
        &m_result_alloc,
        bits,
        val);
}

IEvalStackFrame *EvalContextBase::mkStackFrame(int32_t n_vars) {
    return new(n_vars) EvalStackFrame(n_vars);
}

dmgr::IDebug *EvalContextBase::m_dbg = 0;

}
}
}
