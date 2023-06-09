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
#include "EvalThread.h"


namespace zsp {
namespace arl {
namespace eval {


EvalActivityScopeFullElab::EvalActivityScopeFullElab(
    IEvalContext                *ctxt,
    IEvalThread                 *thread,
    dm::IModelActivityScope     *scope) :
        EvalBase(ctxt, thread), m_scope(scope), m_idx(0) {
    DEBUG_INIT("EvalActivityScopeFullElab", ctxt->getDebugMgr());

}

EvalActivityScopeFullElab::EvalActivityScopeFullElab(EvalActivityScopeFullElab *o) :
    EvalBase(o), m_scope(o->m_scope), m_idx(o->m_idx) {

    for (uint32_t i=0; i<o->m_threads.size(); i++) {
        if (o->m_threads.at(i)) {
            m_threads.push_back(o->m_threads.at(i));
            o->m_threads.at(i) = 0;
        } else {
            m_threads.push_back(0);
        }
    }
}

EvalActivityScopeFullElab::~EvalActivityScopeFullElab() {
    for (uint32_t i=0; i<m_threads.size(); i++) {
        if (m_threads.at(i)) {
            delete m_threads.at(i);
        }
    }
}

bool EvalActivityScopeFullElab::eval() {
    bool ret = false;
    DEBUG_ENTER("eval type=%d", m_scope->getType());

    switch (m_scope->getType()) {
        case dm::ModelActivityScopeT::Sequence:
            ret = eval_sequence();
            break;
        case dm::ModelActivityScopeT::Parallel:
            ret = eval_parallel();
            break;
        default:
            DEBUG("ERROR: Unhandled branch");
            break;
    }

    DEBUG_LEAVE("eval type=%d ret=%d", m_scope->getType(), ret);
    return ret;
}

bool EvalActivityScopeFullElab::eval_parallel() {
    bool ret = false;
    DEBUG_ENTER("[%d] eval_parallel n_branches=%d", getIdx(), m_scope->activities().size());

    if (m_initial) {
        m_thread->pushEval(this);

        for (uint32_t i=0; i<m_scope->activities().size(); i++) {
            m_threads.push_back(new EvalThread(m_ctxt, m_thread));
        }

        m_ctxt->getBackend()->startThreads(m_threads);
    }

    for (uint32_t i=0; i<m_threads.size(); i++) {
        // Skip threads that are already complete
        if (!m_threads.at(i)->haveResult()) {
            if (m_initial) {
                DEBUG("Creating thread %d", i);
                EvalActivityFullElab evaluator(
                    m_ctxt, 
                    m_threads.at(i),
                    m_scope->activities().at(i));

                // Only return if one of the tasks indicates that it is
                // suspending with more work to be done
                ret |= evaluator.eval();
            } else {
                DEBUG("Evaluating thread %d", i);
                ret |= m_threads.at(i)->eval();
            }
        } else {
            DEBUG("Thread %d is complete", i);
        }
    }

    if (m_initial) {
        m_initial = false;
        if (ret) {
            m_thread->suspendEval(this);
        } else {
            m_thread->popEval(this);
        }
    }

    // All done
    if (!ret) {
        m_ctxt->getBackend()->endThreads(m_threads);
    }
    DEBUG_LEAVE("[%d] eval_parallel n_branches=%d", getIdx(), m_scope->activities().size());
    return ret;
}

bool EvalActivityScopeFullElab::eval_sequence() {
    bool ret = false;
    DEBUG_ENTER("[%d] eval_sequence n_activities=%d", getIdx(), m_scope->activities().size());

    if (m_initial) {
        m_thread->pushEval(this);
    }

    while (m_idx < m_scope->activities().size()) {
        EvalActivityFullElab evaluator(
            m_ctxt, 
            m_thread, 
            m_scope->activities().at(m_idx));

        m_idx++; // Always advance

        // Only return if one of the tasks indicates that it is
        // suspending with more work to be done
        DEBUG_ENTER("idx %d", m_idx);
        if ((ret=evaluator.eval())) {
            DEBUG("Child statement blocked");
            break;
        }
        DEBUG_LEAVE("idx %d", m_idx);
    }

    if (m_initial) {
        m_initial = false;
        if (!ret) {
            m_thread->popEval(this);
        } else {
            // We're suspending due to a sub-frame. Insert ourselves into the queue
            m_thread->suspendEval(this);
        }
    }

    DEBUG_LEAVE("[%d] eval_sequence (%d)", getIdx(), ret);
    return ret;
}

bool EvalActivityScopeFullElab::isBlocked() {
    return false;
}

IEval *EvalActivityScopeFullElab::clone() {
    return new EvalActivityScopeFullElab(this);
}

dmgr::IDebug *EvalActivityScopeFullElab::m_dbg = 0;

}
}
}
