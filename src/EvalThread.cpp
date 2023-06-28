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
#include "dmgr/impl/DebugMacros.h"
#include "EvalThread.h"


namespace zsp {
namespace arl {
namespace eval {


EvalThread::EvalThread(
    IEvalContext        *ctxt,
    IEvalThread         *thread) : EvalBase(ctxt, thread), m_thread_id(0) {
    DEBUG_INIT("EvalThread", ctxt->getDebugMgr());

}

EvalThread::~EvalThread() {

}

bool EvalThread::eval() {
    DEBUG_ENTER("eval");
    bool ret = false;

    while (m_eval_s.size() && !(ret=m_eval_s.back()->eval())) {

        // Propagate the result up the stack
        if (m_eval_s.size() > 1) {
            m_eval_s.at(m_eval_s.size()-2)->setResult(
                m_eval_s.back()->moveResult());
        } else {
            EvalBase::setResult(m_eval_s.back()->moveResult());
        }
        m_eval_s.pop_back();
    }

    DEBUG_LEAVE("eval %d", ret);
    return ret;
}

void EvalThread::pushEval(IEval *e, bool owned) {
    DEBUG_ENTER("pushEval %d", m_eval_s.size());
    e->setIdx(m_eval_s.size());
    m_eval_s.push_back(IEvalUP(e, owned));
    DEBUG_LEAVE("pushEval");
}

void EvalThread::suspendEval(IEval *e) {
    DEBUG_ENTER("suspendEval %d", m_eval_s.size());
    if (!m_eval_s.at(e->getIdx()).owned()) {
        DEBUG("Swap with clone");
        m_eval_s.at(e->getIdx()) = IEvalUP(e->clone(), true);
    } else {
        DEBUG("Already owned");
    }
    DEBUG_LEAVE("suspendEval %d", m_eval_s.size());
}

void EvalThread::popEval(IEval *e) {
    DEBUG_ENTER("popEval");
    if (e->haveResult()) {
        DEBUG("hasResult");
        if (m_eval_s.size() > 1) {
            m_eval_s.at(m_eval_s.size()-2)->setResult(e->moveResult());
        } else {
            setResult(e->moveResult());
        }
    } else {
        DEBUG("NOT hasResult");
    }
    m_eval_s.pop_back();
    DEBUG_LEAVE("popEval");
}

void EvalThread::setResult(IEvalResult *r) {
    DEBUG_ENTER("setResult sz=%d", m_eval_s.size());
    if (m_eval_s.size()) {
        m_eval_s.back()->setResult(r);
    } else {
        EvalBase::setResult(r);
    }
    DEBUG_LEAVE("setResult sz=%d", m_eval_s.size());
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

IEvalResult *EvalThread::mkEvalResultVal(const vsc::dm::IModelVal *val) {
    return m_ctxt->mkEvalResultVal(val);
}

IEvalResult *EvalThread::mkEvalResultValS(int64_t val, int32_t bits) {
    return m_ctxt->mkEvalResultValS(val, bits);
}

IEvalResult *EvalThread::mkEvalResultValU(uint64_t val, int32_t bits) {
    return m_ctxt->mkEvalResultValU(val, bits);
}

IEvalResult *EvalThread::mkEvalResultKind(EvalResultKind kind) {
    return m_ctxt->mkEvalResultKind(kind);
}

IEvalResult *EvalThread::mkEvalResultRef(vsc::dm::IModelField *ref) {
    return m_ctxt->mkEvalResultRef(ref);
}

dmgr::IDebug *EvalThread::m_dbg = 0;

}
}
}
