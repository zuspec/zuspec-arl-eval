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
        IEvalThread             *thread,
        int32_t                 vp_id=-1,
        const std::string       &logid="zsp::arl::eval::EvalBase"
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
    
    virtual vsc::dm::ValRef getImmVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) override;

    virtual vsc::dm::ValRef getMutVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) override;

    virtual EvalFlags getFlags() const override;

    virtual bool hasFlags(EvalFlags flags) const override;

    virtual void setFlags(EvalFlags flags) override;

    virtual void clrFlags(EvalFlags flags=EvalFlags::AllFlags) override;

    virtual const vsc::dm::ValRef &getResult() const override;

    virtual void setResult(
        const vsc::dm::ValRef   &r,
        EvalFlags               flags=EvalFlags::Complete) override;

    virtual void setError(const char *fmt, ...) override;

    template <class T> T *ctxtT() const {
        return dynamic_cast<T *>(m_ctxt);
    }

protected:
    dmgr::IDebug                *m_dbg;
    bool                        m_initial;
    int32_t                     m_entry_idx;
    IEvalContext                *m_ctxt;
    IEvalThread                 *m_thread;
    int32_t                     m_vp_id;
    vsc::dm::ValRef             m_result;
    EvalFlags                   m_flags;


};

}
}
}


