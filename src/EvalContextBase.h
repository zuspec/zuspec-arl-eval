/**
 * EvalContextBase.h
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
#pragma once
#include <functional>
#include "pyapi-compat-if/IPyEval.h"
#include "vsc/solvers/IFactory.h"
#include "zsp/arl/dm/IFactory.h"
#include "zsp/arl/eval/IEvalThread.h"
#include "zsp/arl/eval/IEvalContextInt.h"
#include "CoreLibImpl.h"

namespace zsp {
namespace arl {
namespace eval {



class EvalContextBase : 
    public virtual IEvalContextInt, 
    public virtual IEvalThread {
public:
    EvalContextBase(
        dmgr::IDebugMgr                     *dmgr,
        vsc::solvers::IFactory              *solvers_f,
        dm::IContext                        *ctxt,
        const vsc::solvers::IRandState      *randstate,
        pyapi::IPyEval                      *pyeval,
        IEvalBackend                        *backend);

    virtual ~EvalContextBase();

    virtual void init();

    virtual dm::IContext *ctxt() const { return m_ctxt; }

    virtual dm::IModelFieldComponentRoot *getRootComponent() override {
        return 0;
    }

    virtual void setContextFlags(EvalContextFlags flags) override {
        m_ctxt_flags = static_cast<EvalContextFlags>(
            static_cast<uint32_t>(m_ctxt_flags) | 
            static_cast<uint32_t>(flags));
    }

    virtual bool hasContextFlags(EvalContextFlags flags) override {
        return (static_cast<uint32_t>(m_ctxt_flags) & static_cast<uint32_t>(flags));
    }

    /**
     * @brief Push an eval-stack entry
     */
    virtual void pushEval(IEval *e, bool owned=false) override;

    /**
     * @brief Convert eval-stack entry to a suspended entry
     */
    virtual void suspendEval(IEval *e) override;

    virtual void popEval(IEval *e) override;

    virtual IEvalBackend *getBackend() const override {
        return m_backend.get();
    }

    virtual void setBackend(IEvalBackend *b, bool owned=false) override {
        m_backend = IEvalBackendUP(b, owned);
        if (m_backend) {
            m_backend->init(this);
        }
    }

    virtual const std::vector<dm::IDataTypeFunction *> &getSolveFunctions() const override {
        return m_solve_functions;
    }

    virtual const std::vector<dm::IDataTypeFunction *> &getTargetFunctions() const override {
        return m_target_functions;
    }

    virtual void setFunctionData(
        dm::IDataTypeFunction       *func_t,
        IEvalFunctionData           *data) override;

    virtual void addListener(IEvalListener *l) override {
        m_listeners.push_back(l);
    }

    virtual dm::IDataTypeFunction *getFunction(EvalContextFunc func) override;

    virtual void callListener(const std::function<void (IEvalListener *)> &f) override;

    virtual pyapi::PyEvalObj *getPyModule(dm::IPyImport *imp) override;

    virtual pyapi::IPyEval *getPyEval() override;

    virtual bool addPyModule(
        const std::string       &name,
        pyapi::PyEvalObj        *mod) override;

    virtual dmgr::IDebugMgr *getDebugMgr() const override {
        return m_dmgr;
    }

    virtual int32_t getIdx() { return -1; }

    virtual void setIdx(int32_t idx) { }


    virtual EvalFlags getFlags() const override;

    virtual bool hasFlags(EvalFlags flags) const override;

    virtual void setFlags(EvalFlags flags) override;

    virtual void clrFlags(EvalFlags flags=EvalFlags::AllFlags) override;

    virtual const vsc::dm::ValRef &getResult() const override { return m_result; }

    virtual void setResult(
        const vsc::dm::ValRef   &r,
        EvalFlags               flags=EvalFlags::Complete) override;

    virtual void setError(const char *fmt, ...) override;

    virtual IEvalThreadId *getThreadId() const override { 
        return m_thread_id.get();
    }

    virtual void setThreadId(IEvalThreadId *tid) override { 
        m_thread_id = IEvalThreadIdUP(tid);
    }

    virtual int32_t evalMethodCallContext(
        dm::IDataTypeFunction                   *method,
        vsc::dm::IModelField                    *method_ctxt,
        const std::vector<vsc::dm::ITypeExpr *> &params) override;

    virtual vsc::dm::ValRefInt mkValRefInt(
        int64_t value, 
        bool is_signed, 
        int32_t width) override;

    virtual vsc::dm::IModelVal *mkModelValS(int64_t v=0, int32_t w=32) override {
        return m_ctxt->mkModelValS(v, w);
    }

    virtual vsc::dm::IModelVal *mkModelValU(uint64_t v=0, int32_t w=32) override {
        return m_ctxt->mkModelValS(v, w);
    }

    virtual vsc::dm::IModelValOp *getModelValOp() override {
        return m_ctxt->getModelValOp();
    }

    virtual IEvalStackFrame *mkStackFrame(int32_t n_vars) override;
    
    virtual void callFuncReq(
            IEvalThread                         *thread,
            dm::IDataTypeFunction               *func_t,
            const std::vector<vsc::dm::ValRef>  &params
    ) override;

    virtual IEvalValProvider *getValProvider(int32_t id) override;

    virtual IBuiltinFuncInfo *getBuiltinFuncInfo(
        dm::IDataTypeFunction *func) override;

    virtual vsc::dm::IValOps *getValOps(CoreValOpsE kind) override;

    virtual vsc::dm::ValRef getImmVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) override;

    virtual vsc::dm::ValRef getMutVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) override;

protected:

    virtual bool initPython();

protected:
    using FuncT=std::function<void(
        IEvalThread *,
        dm::IDataTypeFunction *,
        const std::vector<vsc::dm::ValRef> &)>;

    using BuiltinFuncInfoM=std::unordered_map<dm::IDataTypeFunction *, IBuiltinFuncInfo *>;

protected:
    static dmgr::IDebug                     *m_dbg;
    dmgr::IDebugMgr                         *m_dmgr;
    CoreLibImpl                             m_corelib;
    vsc::solvers::IFactory                  *m_solvers_f;
    dm::IContext                            *m_ctxt;
    const vsc::solvers::IRandState          *m_randstate;
    pyapi::IPyEval                          *m_pyeval;
    IEvalBackendUP                          m_backend;
    std::vector<dm::IDataTypeFunction *>    m_solve_functions;
    std::vector<dm::IDataTypeFunction *>    m_target_functions;
    std::vector<IEvalListener *>            m_listeners;
    dm::IDataTypeFunction                               *m_functions[(int)EvalContextFunc::NumFunctions];
    vsc::dm::IValOpsUP                      m_valops[(int)CoreValOpsE::Number];
    std::unordered_map<dm::IDataTypeFunction *, FuncT>  m_func_impl;

    EvalContextFlags                        m_ctxt_flags;

    bool                                    m_initial;
    std::vector<IEvalUP>                    m_eval_s;
    vsc::dm::ValRef                         m_result;
    EvalFlags                               m_flags;
    IEvalThreadIdUP                         m_thread_id;
    std::unordered_map<dm::IPyImport *, pyapi::PyEvalObj *>     m_module_m;
    BuiltinFuncInfoM                        m_func_info_m;
    std::vector<IBuiltinFuncInfoUP>         m_func_info_l;

};

}
}
}


