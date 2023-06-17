/*
 * EvalContextFullElab.cpp
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
#include "EvalActivityScopeFullElab.h"
#include "EvalContextFullElab.h"


namespace zsp {
namespace arl {
namespace eval {


EvalContextFullElab::EvalContextFullElab(
    dmgr::IDebugMgr                                 *dmgr,
    dm::IContext                                    *ctxt,
    ElabActivity                                    *activity,
    IEvalBackend                                    *backend) : 
        m_dmgr(dmgr), m_ctxt(ctxt), m_backend(backend), 
        m_initial(true), m_activity(activity) {
    DEBUG_INIT("EvalContextFullElab", dmgr);
}

EvalContextFullElab::~EvalContextFullElab() {

}

bool EvalContextFullElab::eval() {
    bool ret = false;
    DEBUG_ENTER("eval");

    if (m_initial) {
        // Add an eval scope for the top-level activity
        EvalActivityScopeFullElab evaluator(
            this,
            this,
            m_activity->activity_s.back().get()
        );

        ret = evaluator.eval();

        /*
        pushEval(&evaluator);
        if ((ret=evaluator.eval())) {
            suspendEval(&evaluator);
        } else {
            popEval(&evaluator);
        }
         */

        m_initial = false;
    } else {
        while (m_eval_s.size()) {
            DEBUG_ENTER("sub-eval %d", m_eval_s.back()->getIdx());
            if (!(ret=m_eval_s.back()->eval())) {
                DEBUG_LEAVE("sub-eval %d -- done", m_eval_s.back()->getIdx());
                m_eval_s.pop_back();
            } else {
                DEBUG_LEAVE("sub-eval %d -- more work", m_eval_s.back()->getIdx());
                break;
            }
        }
    }

    DEBUG_LEAVE("eval (%d)", ret);
    return ret;
}

void EvalContextFullElab::pushEval(IEval *e, bool owned) {
    e->setIdx(m_eval_s.size());
    m_eval_s.push_back(IEvalUP(e, owned));
}

void EvalContextFullElab::suspendEval(IEval *e) {
    m_eval_s.at(e->getIdx()) = IEvalUP(e->clone(), true);
}

void EvalContextFullElab::popEval(IEval *e) {
    if (e->haveResult()) {
        DEBUG("hasResult (%d)", m_eval_s.size());
        if (m_eval_s.size() > 1) {
            m_eval_s.at(m_eval_s.size()-2)->moveResult(e->moveResult());
        } else {
            moveResult(e->moveResult());
        }
    } else {
        DEBUG("NOT hasResult");
    }
    m_eval_s.pop_back();
}

void EvalContextFullElab::setFunctionData(
        dm::IDataTypeFunction       *func_t,
        IEvalFunctionData           *data) {

}

void EvalContextFullElab::callListener(
    const std::function<void (IEvalListener *)> &f) {

    for (std::vector<IEvalListener *>::const_iterator
        it=m_listeners.begin();
        it!=m_listeners.end(); it++) {
        f(*it);
    }
}

void EvalContextFullElab::setResult(const EvalResult &r) {
    DEBUG_ENTER("setResult sz=%d", m_eval_s.size());
    if (m_eval_s.size()) {
        m_eval_s.back()->setResult(r);
    }
    DEBUG_LEAVE("setResult");
}

void EvalContextFullElab::moveResult(EvalResult &r) {
    DEBUG_ENTER("setResult sz=%d", m_eval_s.size());
    if (m_eval_s.size()) {
        m_eval_s.back()->moveResult(r);
    }
    DEBUG_LEAVE("setResult");
}

void EvalContextFullElab::moveResult(EvalResult &&r) {
    DEBUG_ENTER("setResult sz=%d", m_eval_s.size());
    if (m_eval_s.size()) {
        m_eval_s.back()->moveResult(r);
    }
    DEBUG_LEAVE("setResult");
}

dmgr::IDebug *EvalContextFullElab::m_dbg = 0;


}
}
}
