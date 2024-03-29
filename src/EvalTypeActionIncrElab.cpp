/*
 * EvalTypeActionIncrElab.cpp
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
#include "vsc/dm/impl/ValRefPtr.h"
#include "EvalTypeActionIncrElab.h"
#include "EvalTypeExecList.h"


namespace zsp {
namespace arl {
namespace eval {


EvalTypeActionIncrElab::EvalTypeActionIncrElab(
    EvalContextIncrElab             *ctxt,
    IEvalThread                     *thread,
    const vsc::dm::ValRef           &comp,
    const vsc::dm::ValRef           &action) : 
        EvalBase(ctxt, thread), m_idx(0), m_comp(comp), m_action(action),
        m_vp(thread, action) {
    DEBUG_INIT("zsp::arl::eval::EvalTypeActionIncrElab", ctxt->getDebugMgr());

}

EvalTypeActionIncrElab::EvalTypeActionIncrElab(const EvalTypeActionIncrElab *o) :
    EvalBase(o), m_idx(o->m_idx), m_comp(o->m_comp),
    m_action(o->m_action), m_vp(o->m_thread, o->m_action) { }

EvalTypeActionIncrElab::~EvalTypeActionIncrElab() {

}

int32_t EvalTypeActionIncrElab::eval() {
    DEBUG_ENTER("eval");
    if (m_initial) {
        m_thread->pushEval(this);
        setFlags(EvalFlags::Complete);
    }

    switch (m_idx) {
        case 0: {
            dm::IDataTypeAction *action_t = m_action.typeT<dm::IDataTypeAction>();
            m_idx = 1;

            // TODO: assign component
            vsc::dm::ValRefPtr comp_p(m_action.getFieldRef(0));
//            DEBUG("Initial Comp: %p", comp_p.get_val());
            DEBUG("  %p", m_comp.vp());
//            DEBUG("Initial Comp: %p", comp_p.get_val());
            comp_p.set_val(m_comp.vp());
            DEBUG("FieldVal: %p idx=%d", m_action.getFieldRef(0).vp(), getIdx());

            if (EvalTypeExecList(m_ctxt, m_thread, getIdx(),
                action_t->getExecs(dm::ExecKindT::PreSolve)).eval()) {
                DEBUG("Exec pre-solve suspended");
                clrFlags(EvalFlags::Complete);
                break;
            }

            // TODO: randomize action

            if (EvalTypeExecList(m_ctxt, m_thread, getIdx(),
                action_t->getExecs(dm::ExecKindT::PostSolve)).eval()) {
                DEBUG("Exec post-solve suspended");
                clrFlags(EvalFlags::Complete);
                break;
            }

            m_ctxt->getBackend()->enterAction(
                m_thread,
                dynamic_cast<arl::dm::IDataTypeAction *>(m_action.type()),
                m_action);
        }
        case 1: {
            dm::IDataTypeAction *action_t = m_action.typeT<dm::IDataTypeAction>();
            m_idx = 2;

            if (action_t->activities().size()) {
                // TODO: determine if we have an activity
            } else if (action_t->getExecs(dm::ExecKindT::Body).size()) {
                if (!m_ctxt->hasContextFlags(EvalContextFlags::SkipExecBody)) {
                    if (EvalTypeExecList(m_ctxt, m_thread, getIdx(), 
                        action_t->getExecs(dm::ExecKindT::Body)).eval()) {
                        DEBUG("Suspend due to execs");
                        clrFlags(EvalFlags::Complete);
                        break;
                    } else {
                        DEBUG("EvalTypeExecList completed");
                    }
                }
            }
        }
        case 2: {
            // End
            m_ctxt->getBackend()->leaveAction(
                m_thread,
                dynamic_cast<arl::dm::IDataTypeAction *>(m_action.type()),
                m_action);
            setFlags(EvalFlags::Complete);
        }
    }

    int32_t ret = !hasFlags(EvalFlags::Complete);

    if (m_initial) {
        m_initial = false;
        if (!ret) {
            m_thread->popEval(this);
        } else {
            m_thread->suspendEval(this);
        }
    }

    DEBUG_LEAVE("eval %d", ret);
    return ret;
}

IEval *EvalTypeActionIncrElab::clone() {
    return new EvalTypeActionIncrElab(this);
}

vsc::dm::ValRef EvalTypeActionIncrElab::getImmVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) {
    return m_vp.getImmVal(root_kind, root_offset, val_offset);
}

vsc::dm::ValRef EvalTypeActionIncrElab::getMutVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) {
    return m_vp.getMutVal(root_kind, root_offset, val_offset);
}

dmgr::IDebug *EvalTypeActionIncrElab::m_dbg = 0;

}
}
}
