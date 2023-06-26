/**
 * EvalContextFullElab.h
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
#include "zsp/arl/dm/IContext.h"
#include "zsp/arl/eval/IEvalContext.h"
#include "TaskElaborateActivity.h"
#include "EvalResultAlloc.h"
#include "EvalThread.h"

namespace zsp {
namespace arl {
namespace eval {


class EvalContextFullElab :
    public virtual IEvalContext,
    public virtual IEvalThread {
public:
    EvalContextFullElab(
        dmgr::IDebugMgr                                 *dmgr,
        dm::IContext                                    *ctxt,
        ElabActivity                                    *activity,
        IEvalBackend                                    *backend=0);

    virtual ~EvalContextFullElab();

    virtual bool eval() override;

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
        return m_backend;
    }

    virtual void setBackend(IEvalBackend *b) override {
        m_backend = b;
        if (m_backend) {
            m_backend->init(this);
        }
    }

    virtual const std::vector<dm::IDataTypeFunction *> &getFunctions() const override {
        return m_activity->functions;
    }

    virtual const std::vector<dm::IModelFieldExecutor *> &getExecutors() const override {
        return m_activity->executors;
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

    virtual IEval *clone() override { return 0; }

    virtual const IEvalResult *getResult() const override { return m_result.get(); }

    virtual IEvalResult *moveResult() override { return m_result.release(); }

    virtual void clrResult() override { m_result.reset(); }

    virtual void setResult(IEvalResult *r) override;

    virtual bool haveResult() const override { return false; }

    virtual IEvalThreadId *getThreadId() const override { 
        return 0;
    }

    virtual void setThreadId(IEvalThreadId *tid) override { }

    virtual void pushStackFrame(IEvalStackFrame *frame) override {
        m_callstack.push_back(IEvalStackFrameUP(frame));
    }

    virtual IEvalStackFrame *stackFrame() override {
        return (m_callstack.size())?m_callstack.back().get():0;
    }

    virtual void popStackFrame() override {
        m_callstack.pop_back();
    }

    virtual vsc::dm::IModelVal *mkModelValS(int64_t v=0, int32_t w=32) override {
        return m_ctxt->mkModelValS(v, w);
    }

    virtual vsc::dm::IModelVal *mkModelValU(uint64_t v=0, int32_t w=32) override {
        return m_ctxt->mkModelValS(v, w);
    }

    virtual vsc::dm::IModelValOp *getModelValOp() override {
        return m_ctxt->getModelValOp();
    }

    virtual IEvalResult *mkEvalResultVal(const vsc::dm::IModelVal *val) override;

    virtual IEvalResult *mkEvalResultKind(EvalResultKind kind) override;

    virtual IEvalResult *mkEvalResultRef(vsc::dm::IModelField *ref) override;

private:
    static dmgr::IDebug                     *m_dbg;
    dmgr::IDebugMgr                         *m_dmgr;
    dm::IContext                            *m_ctxt;
    IEvalBackend                            *m_backend;
    ElabActivityUP                          m_activity;
    std::vector<dm::IDataTypeFunction *>    m_functions;
    std::vector<dm::IModelFieldExecutor *>  m_executors;
    std::vector<IEvalListener *>            m_listeners;

    bool                                    m_initial;
    std::vector<IEvalUP>                    m_eval_s;
    std::vector<IEvalStackFrameUP>          m_callstack;
    IEvalResultUP                           m_result;
    EvalResultAlloc                         m_result_alloc;

};

}
}
}


