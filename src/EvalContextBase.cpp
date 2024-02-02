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
#include <algorithm>
#include "dmgr/impl/DebugMacros.h"
#include "BuiltinFuncInfo.h"
#include "EvalContextBase.h"
#include "EvalStackFrame.h"
#include "ModelAddrHandle.h"
#include "TaskBindDataTypeValOps.h"
#include "ValOpsAddrSpaceTransparent.h"
#include "ValOpsDataTypeAddrHandle.h"


namespace zsp {
namespace arl {
namespace eval {


EvalContextBase::EvalContextBase(
    dmgr::IDebugMgr                     *dmgr,
    vsc::solvers::IFactory              *solvers_f,
    dm::IContext                        *ctxt,
    const vsc::solvers::IRandState      *randstate,
    pyapi::IPyEval                      *pyeval,
    IEvalBackend                        *backend) :
        m_dmgr(dmgr), m_corelib(this),
         m_solvers_f(solvers_f), m_ctxt(ctxt),
        m_randstate(randstate), m_pyeval(pyeval),
        m_ctxt_flags(EvalContextFlags::NoFlags),
        m_flags(EvalFlags::NoFlags), m_backend(backend), m_initial(true) {
        
    // m_func_impl.insert({m_functions[(int)EvalContextFunc::RegGroupSetHandle], std::bind(
    //     &CoreLibImpl::RegGroupSetHandle, &m_corelib, 
    //     std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
}

EvalContextBase::~EvalContextBase() {

}

void EvalContextBase::init() {
    /**
     * Built-in method handling. Built-in methods are 'core' methods that
     * are prototyped in PSS -- generally by the standard.
     * 
     * There are two categories/classes of built-in methods:
     * - Methods that are called by user code, but not by the tool
     * - Methods that need to be called by the tool
     * 
     */
    const std::vector<std::string> tool_call_funcs = {
        "addr_reg_pkg::write8", "addr_reg_pkg::write16", 
        "addr_reg_pkg::write32", "addr_reg_pkg::write64",
        "addr_reg_pkg::read8", "addr_reg_pkg::read16", 
        "addr_reg_pkg::read32", "addr_reg_pkg::read64"
    };
    memset(m_functions, 0, sizeof(m_functions));

    for (std::vector<dm::IDataTypeFunction *>::const_iterator
        it=m_ctxt->getDataTypeFunctions().begin();
        it!=m_ctxt->getDataTypeFunctions().end(); it++) {
        IBuiltinFuncInfo *info;
        std::vector<std::string>::const_iterator tool_it;

        if ((info=m_corelib.findBuiltin((*it)->name()))) {
            // Has a builtin implementation
            DEBUG("Found built-in %s", (*it)->name().c_str());
            m_func_info_m.insert({*it, info});
            m_func_info_l.push_back(IBuiltinFuncInfoUP(info));
        } else if ((tool_it=std::find(
            tool_call_funcs.begin(), 
            tool_call_funcs.end(), (*it)->name())) != tool_call_funcs.end()) {
            // Need to store this function for later retrieval
            DEBUG("Found tool-called %s", (*it)->name().c_str());
            m_functions[(tool_it-tool_call_funcs.begin())] = *it;
        }
    }

    for (uint32_t i=0; i<tool_call_funcs.size(); i++) {
        if (!m_functions[i]) {
            ERROR("Failed to find tool-called function %s", tool_call_funcs.at(i).c_str());
        }
    }

    m_valops[(int)CoreValOpsE::AddrSpaceTransparent] = 
        vsc::dm::IValOpsUP(new ValOpsAddrSpaceTransparent(this));
    m_valops[(int)CoreValOpsE::AddrHandle] = 
        vsc::dm::IValOpsUP(new ValOpsDataTypeAddrHandle(this));

    TaskBindDataTypeValOps binder(this);
    for (std::vector<vsc::dm::IDataTypeStructUP>::const_iterator
        it=m_ctxt->getDataTypeStructs().begin();
        it!=m_ctxt->getDataTypeStructs().end(); it++) {
        binder.bind(it->get());
    }
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
    m_eval_s.at(e->getIdx())->clrFlags(EvalFlags::Complete);
    DEBUG("haveResult: %d", m_eval_s.at(e->getIdx())->hasFlags(EvalFlags::Complete));
    DEBUG_LEAVE("suspendEval");
}

void EvalContextBase::popEval(IEval *e) {
    DEBUG_ENTER("popEval %p sz=%d", e, m_eval_s.size());
    m_eval_s.pop_back();
    if (e->hasFlags(EvalFlags::Error)) {
        setResult(e->getResult(), e->getFlags());
    } else if (e->hasFlags(EvalFlags::Complete)) {
        setResult(
            e->getResult(),
            e->getFlags());
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

pyapi::PyEvalObj *EvalContextBase::getPyModule(dm::IPyImport *imp) {
    std::unordered_map<dm::IPyImport *, pyapi::PyEvalObj *>::const_iterator it;

    if ((it=m_module_m.find(imp)) != m_module_m.end()) {
        return it->second;
    } else {
        return 0;
    }
}

pyapi::IPyEval *EvalContextBase::getPyEval() {
    return m_pyeval;
}

bool EvalContextBase::addPyModule(
        const std::string       &name,
        pyapi::PyEvalObj        *mod) {
    std::unordered_map<dm::IPyImport *, pyapi::PyEvalObj *>::const_iterator it;

    arl::dm::IPyImport *imp = m_ctxt->findPyImport(name);

    if (m_module_m.find(imp) == m_module_m.end()) {
        m_pyeval->INCREF(mod);
        m_module_m.insert({imp, mod});
        return true;
    } else {
        return false;
    }
}

dm::IDataTypeFunction *EvalContextBase::getFunction(EvalContextFunc func) {
    return m_functions[(int)func];
}

EvalFlags EvalContextBase::getFlags() const {
    if (m_eval_s.size()) {
        return m_eval_s.back()->getFlags();
    } else {
        return m_flags;
    }
}

bool EvalContextBase::hasFlags(EvalFlags flags) const {
    return ((getFlags() & flags) != EvalFlags::NoFlags);
}

void EvalContextBase::setFlags(EvalFlags flags) {
    if (m_eval_s.size()) {
        m_eval_s.back()->setFlags(flags);
    } else {
        m_flags = flags;
    }
}

void EvalContextBase::clrFlags(EvalFlags flags) {
    if (m_eval_s.size()) {
        m_eval_s.back()->clrFlags(flags);
    } else {
        m_flags = (m_flags & ~flags);
    }
}

void EvalContextBase::setResult(const vsc::dm::ValRef &r, EvalFlags flags) {
    DEBUG_ENTER("setResult sz=%d", m_eval_s.size());
    if (m_eval_s.size()) {
        m_eval_s.back()->setResult(r, flags);
    } else {
        m_result.set(r);
        m_flags = flags;
    }
    DEBUG_LEAVE("setResult have=%d", hasFlags(EvalFlags::Complete));
}

void EvalContextBase::setError(const char *fmt, ...) {
    DEBUG_ENTER("setError");
    char tmp[1024];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(tmp, sizeof(tmp), fmt, ap);
    DEBUG("Message: %s", tmp);
    setResult(
        m_ctxt->mkValRefStr(tmp),
        EvalFlags::Error);
    va_end(ap);
    DEBUG_LEAVE("setError");
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
    BuiltinFuncInfoM::const_iterator it = m_func_info_m.find(func_t);

    if (it != m_func_info_m.end()) {
        // Invoke
        it->second->getImpl()(thread, func_t, params);
    } else if (func_t->hasFlags(dm::DataTypeFunctionFlags::Import)) {
        // Delegate to backend
        getBackend()->callFuncReq(thread, func_t, params);
        // 
    } else {
        ERROR("Implement internal function evaluation for %s (flags=0x%08x). Should not have made it here",
            func_t->name().c_str(), func_t->getFlags());
        thread->setFlags(EvalFlags::Complete);
    }

    DEBUG_LEAVE("callFuncReq");
}

IEvalValProvider *EvalContextBase::getValProvider(int32_t id) {
    if (id >= 0 && id < m_eval_s.size()) {
        return m_eval_s.at(id).get();
    } else if (id < 0) {
        return this;
    } else {
        FATAL("Out-of-bounds value request %d", id);
        return 0;
    }
}

IBuiltinFuncInfo *EvalContextBase::getBuiltinFuncInfo(
        dm::IDataTypeFunction *func) {
    BuiltinFuncInfoM::const_iterator it = m_func_info_m.find(func);
    if (it != m_func_info_m.end()) {
        return it->second;
    } else {
        return 0;
    }
}

vsc::dm::IValOps *EvalContextBase::getValOps(CoreValOpsE kind) {
    return m_valops[(int)kind].get();
}

vsc::dm::ValRef EvalContextBase::getImmVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) {
    FATAL("Root doesn't provide value");
}

vsc::dm::ValRef EvalContextBase::getMutVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) {
    FATAL("Root doesn't provide value");
}

bool EvalContextBase::initPython() {
    DEBUG_ENTER("initPython");
    for (std::vector<dm::IPyImportUP>::const_iterator
        it=m_ctxt->getPyImports().begin();
        it!=m_ctxt->getPyImports().end(); it++) {
        if (m_module_m.find(it->get()) != m_module_m.end()) {
            DEBUG("Module %s has already been loaded", (*it)->path().c_str());
        } else {
            DEBUG("Loading Python module: %s", (*it)->path().c_str());
            pyapi::PyEvalObj *m = m_pyeval->importModule((*it)->path());
            DEBUG("Result: %p", m);

            if (m) {
                m_module_m.insert({it->get(), m});
            } else {
                char tmp[1024];
                snprintf(tmp, sizeof(tmp), "Failed to load Python module %s",
                    (*it)->path().c_str());
                setError(tmp);
                break;
            }
        }
    }
    DEBUG_LEAVE("initPython");
    return true;
}

vsc::dm::ValRefInt EvalContextBase::getAddrHandleValue(const vsc::dm::ValRef &addr_h) {
    vsc::dm::ValRefStruct addr_h_s(addr_h);
    vsc::dm::ValRefPtr addr_h_p(addr_h_s.getFieldRef(-1));
    ModelAddrHandle *addr_hndl = addr_h_p.get_valT<ModelAddrHandle>();

    return m_ctxt->mkValRefInt(
        addr_hndl->getAddr(),
        false,
        64);
}


dmgr::IDebug *EvalContextBase::m_dbg = 0;

}
}
}
