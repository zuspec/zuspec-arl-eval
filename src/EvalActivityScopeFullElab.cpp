/*
 * EvalActivityScopeFullElab.cpp
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
#include "zsp/arl/dm/IModelActivity.h"
#include "zsp/arl/eval/IEvalThread.h"
#include "EvalActivityFullElab.h"
#include "EvalActivityScopeFullElab.h"


namespace zsp {
namespace arl {
namespace eval {


EvalActivityScopeFullElab::EvalActivityScopeFullElab(
    IEvalContext                *ctxt,
    IEvalThread                 *thread,
    dm::IModelActivityScope     *scope,
    int32_t                     idx) : 
        EvalBase(ctxt, thread), m_scope(scope), m_idx(idx) {
    DEBUG_INIT("EvalActivityScopeFullElab", ctxt->getDebugMgr());

}

EvalActivityScopeFullElab::~EvalActivityScopeFullElab() {

}

bool EvalActivityScopeFullElab::eval() {
    bool ret = false;
    DEBUG_ENTER("eval n_activities=%d", m_scope->activities().size());

    if (m_initial) {
        m_thread->pushEval(this);
    }

    while (m_idx < m_scope->activities().size()) {
        EvalActivityFullElab evaluator(
            m_ctxt, 
            m_thread, 
            m_scope->activities().at(m_idx));

        // Only return if one of the tasks indicates that it is
        // suspending with more work to be done
        DEBUG_ENTER("idx %d", m_idx);
        if ((ret=evaluator.eval())) {
            DEBUG("Child statement blocked");
            break;
        }
        DEBUG_LEAVE("idx %d", m_idx);

        m_idx++;
    }

    if (m_initial) {
        if (!ret) {
            m_thread->popEval(this);
        } else {
            // We're suspending due to a sub-frame. Insert ourselves into the queue
            m_thread->suspendEval(this);
        }
    }

    m_initial = false;

    DEBUG_LEAVE("eval (%d)", ret);
    return ret;
}

bool EvalActivityScopeFullElab::isBlocked() {
    return false;
}

IEval *EvalActivityScopeFullElab::clone() {
    return new EvalActivityScopeFullElab(m_ctxt, m_thread, m_scope, m_idx);
}

dmgr::IDebug *EvalActivityScopeFullElab::m_dbg = 0;

}
}
}
