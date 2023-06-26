/*
 * EvalResult.cpp
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
#include <string.h>
#include "EvalResult.h"
#include "EvalResultAlloc.h"


namespace zsp {
namespace arl {
namespace eval {


EvalResult::EvalResult(
    EvalResultAlloc     *alloc,
    EvalResultKind      kind) : next(0), m_alloc(alloc), 
        m_kind(kind), m_bits(0) {

}

EvalResult::EvalResult(
    EvalResultAlloc         *alloc,
    vsc::dm::IModelField    *ref) : next(0), m_alloc(alloc), 
        m_kind(EvalResultKind::Ref), m_bits(0) {
    m_val.vp = reinterpret_cast<uint64_t *>(ref);
}

EvalResult::EvalResult(
    EvalResultAlloc          *alloc,
    const vsc::dm::IModelVal *val) : next(0), m_alloc(alloc),
        m_kind(EvalResultKind::Val), m_bits(val->bits()) {

	if (m_bits <= 64) {
		m_val.v = val->val().v;

		if (m_bits < 64) {
			m_val.v &= (1ULL << m_bits)-1;
		}
	} else {
        // Storage is located beyond the end of the class
        /* TODO:
		m_val.vp = new uint64_t[(m_bits-1)/64+1];
		memcpy(m_val.vp, rhs.m_val.vp, sizeof(uint64_t)*((m_bits-1)/64+1));
         */
	}
}

EvalResult::~EvalResult() {

}

void EvalResult::operator delete(void *p) {
    EvalResult *r = reinterpret_cast<EvalResult *>(p);

    if (r->m_alloc) {
        r->m_alloc->dealloc(r);
    } else {
        free(p);
    }
}

void *EvalResult::operator new(size_t nbytes, EvalResultAlloc *alloc, uint32_t val_bits) {
    return alloc->alloc(nbytes, val_bits);
}

void EvalResult::set(const IModelVal *rhs) {
	if (m_bits > 64 && m_val.vp) {
		delete [] m_val.vp;
	}

	m_bits = rhs->bits();

	if (m_bits <= 64) {
		m_val.v = rhs->val().v;

		if (m_bits < 64) {
			m_val.v &= ((1ULL << m_bits)-1);
		}
	} else {
		m_val.vp = new uint64_t[(m_bits-1)/64+1];
		memcpy(m_val.vp, rhs->val().vp, sizeof(uint64_t)*((m_bits-1)/64+1));
	}
}

void EvalResult::bits(uint32_t b) {
	// TODO: handle transitions between val and vec
	if ((m_bits <= 64) != (b <= 64)) {
		// I

	}
	m_bits = b;
}

void EvalResult::setBits(uint32_t b) {
	// TODO: handle transitions between val and vec
	if ((m_bits <= 64) != (b <= 64)) {
		// I

	}
	m_bits = b;
}

void EvalResult::to_bits(char *bits) const {
	if (m_bits <= 64) {
		char *ep = bits+m_bits;
		*ep-- = 0;
		uint64_t v = m_val.v;
		for (uint32_t i=0; i<m_bits; i++) {
			*ep = '0'+(v&1);
			ep--;
			v >>= 1;
		}
	} else {
		// TODO:
	}
#ifdef UNDEFINED
	if (m_bits <= 64) {
	} else {
	}
	char *ep = bits+m_bits;
	*ep-- = 0;

	uint32_t words=(m_bits-1)/32+1;
	for (uint32_t wi=0; wi<words; wi++) {
		uint32_t word = m_val[wi];
		for (uint32_t bi=0; bi<32 && ep >= bits; bi++) {
			*ep = '0'+(word&1);
			ep--;
			word >>= 1;
		}
	}
#endif
}

void EvalResult::from_bits(const char *bits, int32_t width) {
	// Width is for the incoming bits. It often happens
	// that the width of the literal is wider than
	if (width == -1) {
		width = strlen(bits);
	}
	const char *ep = bits+width-1;
	uint32_t n_words = ((width-1)/32)+1;

	if (m_bits <= 64) {
		const char *cp;
		m_val.v = 0;
		if (m_bits < width) {
			cp = ep-m_bits+1;
		} else {
			cp = ep-width+1;
		}
		for (uint32_t i=0; i<m_bits; i++) {
			m_val.v <<= 1;
			m_val.v |= (*cp - '0');
			cp++;
		}
	} else {
		// TODO:
	}
}

void EvalResult::val_u(uint64_t v, int32_t width) {
	if (width == -1) {
		width = m_bits;
	}
	if (m_bits > 64) {
		m_val.vp[0] = v;
		if (width < 64) {
			m_val.vp[0] &= (1ULL << width)-1;
		}
	} else {
		m_val.v = v;
		if (width < 64) {
			m_val.v &= (1ULL << width)-1;
		}
	}
}

void EvalResult::set_val_u(
		uint64_t v,
		int32_t width) {
	if (width == -1) {
		width = m_bits;
	} else {
		m_bits = width;
	}
	if (m_bits > 64) {
		m_val.vp[0] = v;
		if (width < 64) {
			m_val.vp[0] &= (1ULL << width)-1;
		}
	} else {
		m_val.v = v;
		if (width < 64) {
			m_val.v &= (1ULL << width)-1;
		}
	}
}

void EvalResult::set_val_i(
		int64_t v,
		int32_t width) {
	if (width == -1) {
		width = m_bits;
	} else {
		m_bits = width;
	}
	if (m_bits > 64) {
		m_val.vp[0] = v;
		if (width < 64) {
			m_val.vp[0] &= (1ULL << width)-1;
		}
	} else {
		m_val.v = v;
		if (width < 64) {
			m_val.v &= (1ULL << width)-1;
		}
	}
}

void EvalResult::slice(
        IModelVal           *target,
        int32_t             upper,
        int32_t             lower) {
	if (m_bits <= 64) {
		target->setBits(upper-lower+1);
        	target->set_val_u(m_val.v >> lower);
	} else {
		// TODO:
	}
#ifdef UNDEFINED
	ModelVal ret((upper-lower+1));
	for (uint32_t bit=lower; bit<=upper; bit++) {
		ret.set_bit(bit-lower, get_bit(bit));
	}
	return ret;
#endif
}

IEvalResult *EvalResult::clone(vsc::dm::IContext *ctxt) const {
/*
	IModelVal *ret = (ctxt)?ctxt->mkModelVal():new ModelVal(this);

	if (!ctxt) {
		ret->set(this);
	}

	return ret;
 */
    return 0;
}

}
}
}
