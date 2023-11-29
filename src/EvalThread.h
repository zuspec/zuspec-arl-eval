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
        dmgr::IDebugMgr         *dmgr,
        IEvalBackend            *backend,
        IEvalThread             *thread);

    EvalThread(dmgr::IDebugMgr  *dmgr);

    EvalThread(IEvalThread      *thread);

    virtual ~EvalThread();

    virtual int32_t eval() override;

    virtual IEval *clone() override { return 0; }

    virtual IEvalBackend *getBackend() const override {
        return m_backend;
    }

    void setBackend(IEvalBackend *b) {
        m_backend = b;
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

    virtual IEvalThreadId *getThreadId() const override {
        return m_thread_id.get();
    }

    virtual void setThreadId(IEvalThreadId *tid) override {
        m_thread_id = IEvalThreadIdUP(tid);
    }

    virtual void setResult(
        const vsc::dm::ValRef   &r,
        EvalFlags               flags=EvalFlags::Complete) override;

    virtual int32_t evalMethodCallContext(
        dm::IDataTypeFunction                   *method,
        vsc::dm::IModelField                    *method_ctxt,
        const std::vector<vsc::dm::ITypeExpr *> &params) override;

    virtual dmgr::IDebugMgr *getDebugMgr() const override {
        return m_dmgr;
    }

    virtual vsc::dm::ValRefInt mkValRefInt(
        int64_t value, 
        bool is_signed, 
        int32_t width) override;

protected:

protected:
    static dmgr::IDebug                 *m_dbg;
    IEvalBackend                        *m_backend;
    dmgr::IDebugMgr                     *m_dmgr;
    std::vector<IEvalUP>                m_eval_s;
    IEvalThreadIdUP                     m_thread_id;

};

}
}
}


