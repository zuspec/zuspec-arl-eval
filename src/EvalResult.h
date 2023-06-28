/**
 * EvalResult.h
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
#include <stdint.h>
#include <memory>
#include "zsp/arl/eval/IEvalResult.h"

namespace zsp {
namespace arl {
namespace eval {

class EvalResultAlloc;

class EvalResult : public virtual IEvalResult {
    friend class EvalResultAlloc;
public:

    // Kind result
    EvalResult(
        EvalResultAlloc         *alloc,
        EvalResultKind          kind
    );

    // Ref result
    EvalResult(
        EvalResultAlloc         *alloc,
        vsc::dm::IModelField    *ref
    );

    // Val result
    EvalResult(
        EvalResultAlloc          *alloc,
        const vsc::dm::IModelVal *val
    );

    EvalResult(
        EvalResultAlloc          *alloc,
        int32_t                  bits,
        int64_t                  val
    );

    EvalResult(
        EvalResultAlloc          *alloc,
        int32_t                  bits,
        uint64_t                 val
    );

    virtual ~EvalResult();

    void operator delete(void *p);

    void *operator new(size_t nbytes, EvalResultAlloc *alloc, uint32_t val_bits);

    virtual EvalResultKind getKind() const override {
        return m_kind;
    }

    virtual vsc::dm::IModelField *getRef() const override {
        return reinterpret_cast<vsc::dm::IModelField *>(m_val.vp);
    }

	virtual void set(const IModelVal *v) override;

	virtual uint32_t bits() const override {
		return m_bits;
	}

	virtual void bits(uint32_t b) override;

	virtual void setBits(uint32_t b) override;

	virtual void to_bits(char *bits) const override;

	virtual void from_bits(const char *bits, int32_t width=-1) override;

	virtual void val_u(uint64_t v, int32_t width=-1) override;

	virtual void set_val_u(uint64_t v, int32_t width=-1) override;

	virtual void set_val_i(int64_t v, int32_t width=-1) override;

	inline uint64_t val_u() const { return (bits()<=64)?val().v:val().vp[0]; }

	virtual const val_t &val() const override { return m_val; }

	virtual val_t &val() override { return m_val; }

	virtual IEvalResult *clone(vsc::dm::IContext *ctxt=0) const override;

    virtual void slice(
        IModelVal           *target,
        int32_t             upper,
        int32_t             lower) override;


public:
    EvalResult          *next;

private:
    EvalResultKind                  m_kind;
    vsc::dm::IModelVal::val_t       m_val;
    uint32_t                        m_bits;
    bool                            m_own_vp;
    EvalResultAlloc                 *m_alloc;

};

}
}
}


