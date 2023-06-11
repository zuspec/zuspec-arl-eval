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

    EvalBase(const EvalBase *o);

    virtual ~EvalBase();

    virtual bool eval(const std::function<void()> &body);

    virtual int32_t getIdx() override {
        return m_entry_idx;
    }

    virtual void setIdx(int32_t idx) override {
        m_entry_idx = idx;
    }

    virtual const EvalResult &getResult() const override {
        return m_result;
    }

    virtual EvalResult moveResult() override {
        EvalResult ret = m_result;
        m_result.clear();
        return ret;
    }

    virtual void clrResult() override {
        m_result.clear();
    }

    virtual void setResult(const EvalResult &r) override {
        if (r.kind == EvalResultKind::Val) {
            throw "Error: cannot call setResult(const) with Val\n";
        } else {
            m_result.ref = r.ref;
            m_result.kind = r.kind;
            m_result.val.reset();
        }
    }

    virtual void moveResult(EvalResult &r) override {
        m_result = r;
    }

    virtual void moveResult(EvalResult &&r) override {
        m_result = r;
    }

    virtual bool haveResult() const override {
        return m_result.kind != EvalResultKind::None;
    }

protected:
    bool                        m_initial;
    int32_t                     m_entry_idx;
    IEvalContext                *m_ctxt;
    IEvalThread                 *m_thread;
    EvalResult                  m_result;

};

}
}
}


