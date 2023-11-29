/*
 * EvalTypeFunction.cpp
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
#include "dmgr/impl/DebugMacros.h"
#include "zsp/arl/eval/IEvalContextInt.h"
#include "EvalTypeFunction.h"
#include "EvalTypeProcStmtScope.h"


namespace zsp {
namespace arl {
namespace eval {


EvalTypeFunction::EvalTypeFunction(
    IEvalContext                        *ctxt,
    IEvalThread                         *thread,
    int32_t                             vp_id,
    dm::IDataTypeFunction               *func,
    const std::vector<vsc::dm::ValRef>  &params) :
        EvalBase(ctxt, thread, vp_id), m_func(func),
        m_params(params.begin(), params.end()) {
    DEBUG_INIT("zsp::arl::eval::EvalTypeFunction", ctxt->getDebugMgr());

}

EvalTypeFunction::EvalTypeFunction(const EvalTypeFunction *o) :
    EvalBase(o), m_func(o->m_func),
    m_params(o->m_params.begin(), o->m_params.end()) {

}

EvalTypeFunction::~EvalTypeFunction() {

}

int32_t EvalTypeFunction::eval() {
    DEBUG_ENTER("eval");
    bool ret = false;

    if (m_initial) {
        m_thread->pushEval(this);

        EvalTypeProcStmtScope(
            m_ctxt, m_thread, getIdx(), m_func->getBody()).eval();
    }

    ret = !hasFlags(EvalFlags::Complete);
    
    if (!ret) {
        // The return flag doesn't propagate past the function boundary.
        // Update the flags before the result is moved (via popEval)
        DEBUG("Clear 'return' flag");
        clrFlags(EvalFlags::Return);
        DEBUG("hasFlags(Return): %d", hasFlags(EvalFlags::Return));
    }

    if (m_initial) {
        m_initial = false;
        if (ret) {
            m_thread->suspendEval(this);
        } else {
            m_thread->popEval(this);
        }
    }


    DEBUG_LEAVE("eval %d", ret);
    return ret;
}

IEval *EvalTypeFunction::clone() {
    return new EvalTypeFunction(this);
}

vsc::dm::ValRef EvalTypeFunction::getImmVal(
    vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
    int32_t                                 root_offset,
    int32_t                                 val_offset) {
    DEBUG_ENTER("getImmVal root_kind=%d root_offset=%d val_offset=%d",
        root_kind, root_offset, val_offset);
    if (root_kind == vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope) {
        if (root_offset == 0) {
            // This scope
            if (val_offset < m_params.size()) {
                DEBUG("Get parameter %d (%lld)", val_offset, m_params.at(val_offset).vp());
                return m_params.at(val_offset);
            } else {
                ERROR("out-of-bounds parameter value request");
            }
        } else {
            // Delegate up
            return ctxtT<IEvalContextInt>()->getValProvider(m_vp_id)->getImmVal(
                root_kind,
                root_offset-1,
                val_offset);
        }
    } else {
        return ctxtT<IEvalContextInt>()->getValProvider(m_vp_id)->getImmVal(
            root_kind,
            root_offset,
            val_offset);
    }
    DEBUG_LEAVE("getImmVal root_kind=%d root_offset=%d val_offset=%d",
        root_kind, root_offset, val_offset);
}

vsc::dm::ValRef EvalTypeFunction::getMutVal(
    vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
    int32_t                                 root_offset,
    int32_t                                 val_offset) {
    DEBUG_ENTER("getMutVal root_kind=%d root_offset=%d val_offset=%d",
        root_kind, root_offset, val_offset);
    if (root_kind == vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope) {
        if (root_offset == 0) {
            // This scope
            DEBUG("Get parameter %d (sz=%d)", val_offset, m_params.size());
            if (val_offset < m_params.size()) {
                return m_params.at(val_offset);
            } else {
                ERROR("out-of-bounds parameter value request");
            }
        } else {
            // Delegate up
            return ctxtT<IEvalContextInt>()->getValProvider(getIdx()-1)->getMutVal(
                root_kind,
                root_offset-1,
                val_offset);
        }
    } else {
        return ctxtT<IEvalContextInt>()->getValProvider(m_vp_id)->getMutVal(
            root_kind,
            root_offset,
            val_offset);
    }
    DEBUG_LEAVE("getMutVal root_kind=%d root_offset=%d val_offset=%d",
        root_kind, root_offset, val_offset);
}

}
}
}
