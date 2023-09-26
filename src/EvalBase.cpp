/*
 * EvalBase.cpp
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
#include "vsc/dm/impl/ValRefInt.h"
#include "EvalBase.h"


namespace zsp {
namespace arl {
namespace eval {

EvalBase::EvalBase(
    IEvalContext            *ctxt,
    IEvalThread             *thread) :
        m_initial(true), m_entry_idx(-1), m_ctxt(ctxt), m_thread(thread) {

}

EvalBase::EvalBase(IEvalThread *thread) :
        m_initial(true), m_entry_idx(-1), m_ctxt(0), m_thread(thread) {
}

EvalBase::EvalBase(const EvalBase *o) :
    m_initial(false), m_entry_idx(o->m_entry_idx), m_ctxt(o->m_ctxt), 
    m_thread(o->m_thread) {

}

EvalBase::~EvalBase() {

}

int32_t EvalBase::eval(const std::function<void()> &body) {
    if (m_initial) {
        m_thread->pushEval(this);
    }

    body();

    if (m_initial) {
        if (!haveResult()) {
            m_thread->suspendEval(this);
        }
        m_initial = false;
    }

    return !haveResult();
}

void EvalBase::setVoidResult() {
    vsc::dm::IDataTypeInt *i32 = m_ctxt->ctxt()->findDataTypeInt(true, 32);
    if (!i32) {
        i32 = m_ctxt->ctxt()->mkDataTypeInt(true, 32);
        m_ctxt->ctxt()->addDataTypeInt(i32);
    }
    vsc::dm::ValRefInt v(0, i32, vsc::dm::ValRef::Flags::None);
    setResult(v);
}

}
}
}
