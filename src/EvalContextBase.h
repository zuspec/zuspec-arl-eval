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
#include "vsc/solvers/IFactory.h"
#include "zsp/arl/dm/IFactory.h"
#include "zsp/arl/eval/IEvalThread.h"
#include "zsp/arl/eval/IEvalContext.h"
#include "EvalResultAlloc.h"

namespace zsp {
namespace arl {
namespace eval {



class EvalContextBase : 
    public virtual IEvalContext, 
    public virtual IEvalThread {
public:
    EvalContextBase(
        dmgr::IDebugMgr                     *dmgr,
        vsc::solvers::IFactory              *solvers_f,
        dm::IContext                        *ctxt,
        const vsc::solvers::IRandState      *randstate,
        IEvalBackend                        *backend);

    virtual ~EvalContextBase();

    virtual dm::IContext *ctxt() const { return m_ctxt; }

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

    virtual void callListener(const std::function<void (IEvalListener *)> &f) override;

    virtual dmgr::IDebugMgr *getDebugMgr() const override {
        return m_dmgr;
    }

    virtual int32_t getIdx() { return -1; }

    virtual void setIdx(int32_t idx) { }

    virtual const vsc::dm::ValRef &getResult() const override { return m_result; }

    virtual vsc::dm::ValRef &moveResult() override { return m_result; }

    virtual void clrResult() override { m_result.reset(); }

    virtual void setResult(const vsc::dm::ValRef &r) override;
    
    void setVoidResult();

    virtual bool haveResult() const override { return false; }

    virtual IEvalThreadId *getThreadId() const override { 
        return m_thread_id.get();
    }

    virtual void setThreadId(IEvalThreadId *tid) override { 
        m_thread_id = IEvalThreadIdUP(tid);
    }

    virtual void pushStackFrame(IEvalStackFrame *frame) override {
        m_callstack.push_back(IEvalStackFrameUP(frame));
    }

    virtual IEvalStackFrame *stackFrame(int32_t idx=0) override;

    virtual void popStackFrame() override {
        m_callstack.pop_back();
    }

    virtual int32_t evalMethodCallContext(
        dm::IDataTypeFunction                   *method,
        vsc::dm::IModelField                    *method_ctxt,
        const std::vector<vsc::dm::ITypeExpr *> &params) override;

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

protected:
    static dmgr::IDebug                     *m_dbg;
    dmgr::IDebugMgr                         *m_dmgr;
    vsc::solvers::IFactory                  *m_solvers_f;
    dm::IContext                            *m_ctxt;
    const vsc::solvers::IRandState          *m_randstate;
    IEvalBackendUP                          m_backend;
    std::vector<dm::IDataTypeFunction *>    m_solve_functions;
    std::vector<dm::IDataTypeFunction *>    m_target_functions;
    std::vector<IEvalListener *>            m_listeners;

    bool                                    m_initial;
    std::vector<IEvalUP>                    m_eval_s;
    std::vector<IEvalStackFrameUP>          m_callstack;
    vsc::dm::ValRef                         m_result;
    IEvalThreadIdUP                         m_thread_id;

};

}
}
}


