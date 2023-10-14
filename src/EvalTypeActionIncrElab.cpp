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
        setVoidResult();
    }

    switch (m_idx) {
        case 0: {
            dm::IDataTypeAction *action_t = m_action.typeT<dm::IDataTypeAction>();
            m_idx = 1;

            // TODO: assign component
            vsc::dm::ValRefPtr comp_p(m_action.getFieldRef(0));
//            DEBUG("Initial Comp: %p", comp_p.get_val());
            DEBUG("  %p", m_comp.vp());
            DEBUG("Initial Comp: %p", comp_p.get_val());
            comp_p.set_val(m_comp.vp());
            DEBUG("FieldVal: %p", m_action.getFieldRef(0).vp());
            fflush(stdout);

            EvalTypeExecList(m_ctxt, m_thread, &m_vp,
                action_t->getExecs(dm::ExecKindT::PreSolve)).eval();

            // TODO: randomize action

            EvalTypeExecList(m_ctxt, m_thread, &m_vp,
                action_t->getExecs(dm::ExecKindT::PostSolve)).eval();
        }
        case 1: {
            dm::IDataTypeAction *action_t = m_action.typeT<dm::IDataTypeAction>();
            m_idx = 2;

            if (action_t->activities().size()) {
                // TODO: determine if we have an activity
            } else if (action_t->getExecs(dm::ExecKindT::Body).size()) {
                if (EvalTypeExecList(m_ctxt, m_thread, &m_vp, 
                    action_t->getExecs(dm::ExecKindT::Body)).eval()) {
                    DEBUG("Suspend due to execs");
                    clrResult();
                    break;
                }
            }
        }
        case 2: {
            // End
        }
    }

    int32_t ret = !haveResult();

    if (m_initial) {
        m_initial = false;
        if (haveResult()) {
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

dmgr::IDebug *EvalTypeActionIncrElab::m_dbg = 0;

}
}
}
