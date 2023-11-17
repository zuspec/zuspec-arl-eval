/*
 * EvalActivityFullElab.cpp
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
#include "EvalActivityFullElab.h"
#include "EvalActivityScopeFullElab.h"
#include "EvalTypeExecList.h"


namespace zsp {
namespace arl {
namespace eval {


EvalActivityFullElab::EvalActivityFullElab(
    IEvalContext        *ctxt,
    IEvalThread         *thread,
    dm::IModelActivity  *activity) : 
        EvalBase(ctxt, thread), m_vp(thread), m_activity(activity), m_idx(0) {
    
    DEBUG_INIT("EvalActivityFullElab", ctxt->getDebugMgr());
}

EvalActivityFullElab::EvalActivityFullElab(const EvalActivityFullElab *o) :
    EvalBase(o), m_vp(o->m_vp), m_activity(o->m_activity), m_idx(o->m_idx) {

}

EvalActivityFullElab::~EvalActivityFullElab() {

}

int32_t EvalActivityFullElab::eval() {
    DEBUG_ENTER("[%d] eval", getIdx());
    if (m_initial) {
        m_thread->pushEval(this);
    }

    m_activity->accept(m_this);

    int32_t ret = !haveResult();

    if (m_initial) {
        m_initial = false;

        if (!haveResult()) {
            m_thread->suspendEval(this);
        } else {
            m_thread->popEval(this);
        }
    }

    DEBUG_LEAVE("[%d] eval (%d)", getIdx(), ret);
    return ret;
}

IEval *EvalActivityFullElab::clone() {
    return new EvalActivityFullElab(this);
}

void EvalActivityFullElab::visitModelActivityScope(dm::IModelActivityScope *a) {
    DEBUG_ENTER("visitModelActivityScope");

    switch (m_idx) {
        case 0: {
            EvalActivityScopeFullElab evaluator(m_ctxt, m_thread, a);

            m_idx++;
            if (evaluator.eval()) {
                clrResult();
                break;
            }
        }

        case 1: {
            setVoidResult();
        }
    }


    DEBUG_LEAVE("visitModelActivityScope (%d)", haveResult());
}

void EvalActivityFullElab::visitModelActivityTraverse(dm::IModelActivityTraverse *a) {
    bool ret = false;
    DEBUG_ENTER("visitModelActivityTraverse %s idx=%d",
        a->getTarget()->getDataTypeT<vsc::dm::IDataTypeStruct>()->name().c_str(), m_idx);

    switch (m_idx) {
        case 0: {
            m_idx = 1; // Always move forward

            // After solving?
            m_ctxt->callListener([&](IEvalListener *l) { 
                l->enterAction(m_thread, a->getTarget());});

            // TODO: Evaluate non-blocking 'pre_solve' if it exists

            // TODO: Perform solving first

            m_ctxt->getBackend()->enterAction(
                m_thread,
                a->getTarget());

            // TODO: Evaluate non-blocking 'post_solve' if it exists

            if (a->getTarget()->getActivity()) {
                // Compound activity
                DEBUG("Note: Have an activity");
                EvalActivityScopeFullElab evaluator(
                    m_ctxt, 
                    m_thread, 
                    a->getTarget()->getActivity());

                if (evaluator.eval()) {
                    break;
                }
            } else {
                // Need to evaluate exec blocks
                dm::IDataTypeAction *action_t = a->getTarget()->getDataTypeT<dm::IDataTypeAction>();

                if (action_t->getExecs(dm::ExecKindT::Body).size()) {
                    m_vp.setScope(a->getTarget()->getImmVal());
                    DEBUG("Note: Have an exec body block");
                    EvalTypeExecList evaluator(
                        m_ctxt, 
                        m_thread,
                        getIdx(),
                        action_t->getExecs(dm::ExecKindT::Body));
                    
                    if (evaluator.eval()) {
                        break;
                    }
                } else {
                    DEBUG("Note: No exec body block");
                }
            }
        }
        
        case 1: {
            DEBUG("case 1: setResult");
            m_idx = 2;
            m_ctxt->getBackend()->leaveAction(
                m_thread, 
                a->getTarget());

            setVoidResult();
        } break;
    }

    if (haveResult()) {
        m_ctxt->callListener([&](IEvalListener *l) { 
            l->leaveAction(m_thread, a->getTarget());});
    }

    DEBUG_LEAVE("visitModelActivityTraverse %s (%d)",
        a->getTarget()->getDataTypeT<vsc::dm::IDataTypeStruct>()->name().c_str(),
        haveResult());
}

dmgr::IDebug *EvalActivityFullElab::m_dbg = 0;

}
}
}
