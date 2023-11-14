/**
 * EvalBase.h
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
#include "vsc/dm/impl/ValRef.h"
#include "zsp/arl/eval/IEval.h"
#include "zsp/arl/eval/IEvalContext.h"
#include "zsp/arl/eval/IEvalThread.h"

namespace zsp {
namespace arl {
namespace eval {


class EvalBase : public virtual IEval {
public:

    EvalBase(
        IEvalContext            *ctxt,
        IEvalThread             *thread
    );

    EvalBase(IEvalThread        *thread);

    EvalBase(const EvalBase *o);

    virtual ~EvalBase();

    virtual int32_t eval(const std::function<void()> &body);

    virtual int32_t getIdx() override {
        return m_entry_idx;
    }

    virtual void setIdx(int32_t idx) override {
        m_entry_idx = idx;
    }

    virtual const vsc::dm::ValRef &getResult() const override {
        return m_result;
    }

    virtual vsc::dm::ValRef &moveResult() override {
        return m_result;
    }

    virtual void clrResult(bool clr_err=false) override;

    virtual void setResult(const vsc::dm::ValRef &r) override;

    void setVoidResult();

    virtual void setError(const std::string &msg) override;

    virtual bool haveError() const override;

    virtual const std::string &getError() const override;

    virtual bool haveResult() const override {
        return m_result.valid();
    }

    template <class T> T *ctxtT() const {
        return dynamic_cast<T *>(m_ctxt);
    }

protected:
    bool                        m_initial;
    int32_t                     m_entry_idx;
    IEvalContext                *m_ctxt;
    IEvalThread                 *m_thread;
    vsc::dm::ValRef             m_result;
    bool                        m_error;
    std::string                 m_errMsg;


};

}
}
}


