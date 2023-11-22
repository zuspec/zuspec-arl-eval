/*
 * EvalBaseLocals.cpp
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
#include "zsp/arl/eval/IEvalContextInt.h"
#include "EvalBaseLocals.h"


namespace zsp {
namespace arl {
namespace eval {


EvalBaseLocals::EvalBaseLocals(
        IEvalContext                    *ctxt,
        IEvalThread                     *thread,
        int32_t                         vp_id,
        const vsc::dm::ValRefStruct     &locals) :
    EvalBase(ctxt, thread, vp_id), m_locals(locals) {

}

EvalBaseLocals::EvalBaseLocals(const EvalBaseLocals *o) :
    EvalBase(o), m_locals(o->m_locals) {

}

EvalBaseLocals::~EvalBaseLocals() {

}

vsc::dm::ValRef EvalBaseLocals::getImmVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) {
    if (root_kind == vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope
        && m_vp_id == -1 && root_offset == 0) {
        return m_locals.getFieldRef(val_offset).toImmutable();
    } else {
        return EvalBase::getImmVal(root_kind, root_offset, val_offset);
    }
}

vsc::dm::ValRef EvalBaseLocals::getMutVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) {
    if (root_kind == vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope
        && m_vp_id == -1 && root_offset == 0) {
        return m_locals.getFieldRef(val_offset).toMutable();
    } else {
        return EvalBase::getMutVal(root_kind, root_offset, val_offset);
    }
}

}
}
}
