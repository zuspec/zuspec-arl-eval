/*
 * EvalMultiStepClosure.cpp
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
#include "EvalMultiStepClosure.h"


namespace zsp {
namespace arl {
namespace eval {


EvalMultiStepClosure::EvalMultiStepClosure(
    IEvalContext                            *ctxt,
    IEvalThread                             *thread,
    const std::function<bool(EvalMultiStepClosure *, uint32_t &)>     &func,
    uint32_t                                idx) :
        EvalBase(ctxt, thread), m_func(func), m_idx(idx) {
    m_initial = (idx == 0);
}

EvalMultiStepClosure::~EvalMultiStepClosure() {

}

int32_t EvalMultiStepClosure::eval() {
    if (m_initial) {
        m_thread->pushEval(this);
    }

    int32_t ret = m_func(this, m_idx);

    if (m_initial) {
        m_thread->suspendEval(this);
    }

    return ret;
}

IEval *EvalMultiStepClosure::clone() {
    return new EvalMultiStepClosure(m_ctxt, m_thread, m_func, m_idx);
}

}
}
}
