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
#include "zsp/arl/eval/IEvalContext.h"
#include "TaskElaborateActivity.h"
#include "EvalThread.h"

namespace zsp {
namespace arl {
namespace eval {


class EvalContextFullElab :
    public virtual IEvalContext,
    public virtual IEvalThread {
public:
    EvalContextFullElab(
        dmgr::IDebugMgr     *dmgr,
        ElabActivity        *activity,
        IEvalBackend        *backend=0);

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

    virtual bool isBlocked() override;

    virtual IEvalBackend *getBackend() const override {
        return m_backend;
    }

    virtual void setBackend(IEvalBackend *b) override {
        m_backend = b;
    }
    
    virtual const std::vector<dm::IDataTypeFunction *> &getFunctions() const override {
        return m_functions;
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

    virtual vsc::dm::IModelVal *getResult() override { return 0; }

    virtual EvalResultKind getResultKind() override { return EvalResultKind::Default; }

    virtual vsc::dm::IModelVal *moveResult() override { return 0; }

    virtual void clrResult() override { }

    virtual void setResult(
        vsc::dm::IModelVal      *val,
        EvalResultKind          kind) override;

    virtual bool haveResult() const override { return false; }

    virtual IEvalThreadId *getThreadId() const override { 
        return 0;
    }

    virtual void setThreadId(IEvalThreadId *tid) override { }

private:
    static dmgr::IDebug                     *m_dbg;
    dmgr::IDebugMgr                         *m_dmgr;
    IEvalBackend                            *m_backend;
    ElabActivityUP                          m_activity;
    std::vector<dm::IDataTypeFunction *>    m_functions;
    std::vector<IEvalListener *>            m_listeners;

    bool                                    m_initial;
    std::vector<IEvalUP>                    m_eval_s;

};

}
}
}


