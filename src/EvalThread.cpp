/*
 * EvalThread.cpp
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
#include "EvalThread.h"


namespace zsp {
namespace arl {
namespace eval {


EvalThread::EvalThread() {

}

EvalThread::~EvalThread() {

}

bool EvalThread::eval() {
    bool ret = false;

    while (m_eval_s.size() 
        && !(ret=m_eval_s.back()->eval())) {

        // Propagate the result up the stack
        if (m_eval_s.size() > 1) {
            m_eval_s.at(m_eval_s.size()-2)->setResult(
                m_eval_s.back()->moveResult());
        } else {
            setResult(m_eval_s.back()->moveResult());
        }
        m_eval_s.pop_back();
    }

    if (m_eval_s.size() && !m_eval_s.back()->isBlocked()) {
        m_eval_s.back()->eval();
    }

    return ret;
}

bool EvalThread::isBlocked() {
    return (m_eval_s.size() && m_eval_s.back()->isBlocked());
}

void EvalThread::pushEval(IEval *e, bool owned) {
    e->setIdx(m_eval_s.size());
    m_eval_s.push_back(IEvalUP(e, owned));
}

void EvalThread::suspendEval(IEval *e) {
    if (!m_eval_s.at(e->getIdx()).owned()) {
        m_eval_s.at(e->getIdx()) = IEvalUP(e->clone(), true);
    }
}

void EvalThread::popEval(IEval *e) {
    m_eval_s.pop_back();
}

/*
void EvalThread::sendEvalEvent(
    const std::function<void (IEvalListener *)> &f) {
    IEvalThread *t = this;
    while (t) {
        for (std::vector<IEvalListener *>::const_iterator
            it=t->getListeners().begin();
            it!=t->getListeners().end(); it++) {
            f(*it);
        }
        t = t->getParent();
    }
}
*/

}
}
}
