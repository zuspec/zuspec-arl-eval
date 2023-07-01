/*
 * EvalResultAlloc.cpp
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


EvalResultAlloc::EvalResultAlloc() {
    memset(m_bins, 0, sizeof(m_bins));
    m_large = 0;
}

EvalResultAlloc::~EvalResultAlloc() {
    for (uint32_t i=0; i<sizeof(m_bins)/sizeof(void *); i++) {
        void *p = m_bins[i];
        while (p) {
            EvalResult *r = reinterpret_cast<EvalResult *>(p);
            void *p2 = reinterpret_cast<void *>(r->next);
//            ::operator delete[](reinterpret_cast<uint32_t *>(p));
            free(p);
            p = p2;
        }
    }
}

void *EvalResultAlloc::alloc(size_t nbytes, uint32_t val_bits) {
    void *p = 0;
    if (val_bits > EvalResultAlloc::MAX_POOLED_BITS) {
        // Too large for us to cache by size
        // Pick something off the 'large' stack
        if (m_large) {
            p = m_large;
            m_large = reinterpret_cast<EvalResult *>(m_large)->next;
        } else {
//            p = ::operator new(nbytes);
            p = malloc(nbytes);
        }
    } else {
        uint32_t bin = (val_bits)?(val_bits-1)/64:0;
        if (m_bins[bin]) {
            // Reuse
            p = m_bins[bin];
            m_bins[bin] = reinterpret_cast<EvalResult *>(p)->next;
        } else {
            uint32_t n_ptrs = ((nbytes-1)/sizeof(void *))+1;
//            p = ::operator new(n_ptrs*sizeof(void *));
            p = malloc(n_ptrs*sizeof(void *));
        }
    }
    return p;
}

void EvalResultAlloc::dealloc(EvalResult *o) {
    if (o->m_bits <= MAX_POOLED_BITS) {
        uint32_t bin = (o->m_bits)?o->m_bits-1/64:0;
        o->next = reinterpret_cast<EvalResult*>(m_bins[bin]);
        m_bins[bin] = o;
    } else {
        o->next = reinterpret_cast<EvalResult *>(m_large);
        m_large = o;
    }
}

}
}
}
