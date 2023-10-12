/*
 * EvalValProviderStructThread.cpp
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
#include "EvalValProviderStructThread.h"


namespace zsp {
namespace arl {
namespace eval {


EvalValProviderStructThread::EvalValProviderStructThread(IEvalThread *thread) 
    : m_thread(thread), m_scope(vsc::dm::ValRef()) {

}

EvalValProviderStructThread::EvalValProviderStructThread(
    IEvalThread                 *thread,
    const vsc::dm::ValRef       &scope) : m_thread(thread), m_scope(scope) {

}

EvalValProviderStructThread::EvalValProviderStructThread(
    const EvalValProviderStructThread &o) : 
        m_thread(o.m_thread), m_scope(o.m_scope) { }

EvalValProviderStructThread::~EvalValProviderStructThread() {

}

void EvalValProviderStructThread::setScope(const vsc::dm::ValRefStruct &scope) {
    m_scope = scope;
};

vsc::dm::ValRef EvalValProviderStructThread::getImmVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) const {
    vsc::dm::ValRef var;

    switch (root_kind) {
        case vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope: {
            IEvalStackFrame *frame = m_thread->stackFrame(root_offset);
            var.setWeakRef(frame->getVariable(val_offset));
        } break;
        case vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope: {
            vsc::dm::ValRefStruct scope_s(m_scope);
            var.setWeakRef(scope_s.getFieldRef(val_offset));
        } break;
    }

    return var.toImmutable();
}

vsc::dm::ValRef EvalValProviderStructThread::getMutVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) {
    vsc::dm::ValRef var;

    switch (root_kind) {
        case vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope: {
            IEvalStackFrame *frame = m_thread->stackFrame(root_offset);
            var.setWeakRef(frame->getVariable(val_offset));
        } break;
        case vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope: {
            vsc::dm::ValRefStruct scope_s(m_scope);
            var.setWeakRef(scope_s.getFieldRef(val_offset));
        } break;
    }

    return var.toMutable();
}

}
}
}
