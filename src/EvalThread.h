/**
 * EvalThread.h
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
#include <vector>
#include "zsp/arl/eval/IEvalThread.h"
#include "EvalBase.h"

namespace zsp {
namespace arl {
namespace eval {



class EvalThread : 
    public virtual IEvalThread,
    public virtual EvalBase {
public:
    EvalThread(
        IEvalContext            *ctxt,
        IEvalThread             *thread);

    virtual ~EvalThread();

    virtual bool eval() override;

    virtual IEval *clone() override { return 0; }

    /**
     * @brief Push an eval-stack entry
     */
    virtual void pushEval(IEval *e, bool owned=false) override;

    /**
     * @brief Convert eval-stack entry to a suspended entry
     */
    virtual void suspendEval(IEval *e) override;

    virtual void popEval(IEval *e) override;

    virtual IEvalThreadId *getThreadId() const override {
        return m_thread_id;
    }

    virtual void setThreadId(IEvalThreadId *tid) override {
        m_thread_id = tid;
    }

    virtual void setResult(const EvalResult &r) override;

    virtual void pushStackFrame(IEvalStackFrame *frame) override {
        m_callstack.push_back(IEvalStackFrameUP(frame));
    }

    virtual IEvalStackFrame *stackFrame() override {
        return (m_callstack.size())?m_callstack.back().get():0;
    }

    virtual void popStackFrame() override {
        m_callstack.pop_back();
    }

protected:

protected:
    static dmgr::IDebug                 *m_dbg;
    std::vector<IEvalUP>                m_eval_s;
    IEvalThreadId                       *m_thread_id;
    std::vector<IEvalStackFrameUP>      m_callstack;

};

}
}
}


