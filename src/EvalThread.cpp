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
#include "EvalResult.h"
#include "EvalThread.h"
#include "EvalTypeMethodCallContext.h"


namespace zsp {
namespace arl {
namespace eval {


EvalThread::EvalThread(
    dmgr::IDebugMgr     *dmgr,
    IEvalBackend        *backend,
    IEvalThread         *thread) : EvalBase(thread), 
        m_dmgr(dmgr), m_backend(backend), m_thread_id(0) {
    DEBUG_INIT("EvalThread", dmgr);

}

EvalThread::EvalThread(dmgr::IDebugMgr *dmgr) :
    EvalBase((IEvalThread *)0), m_dmgr(dmgr),
    m_backend(0), m_thread_id(0) {

}

EvalThread::EvalThread(IEvalThread *thread) :
    EvalBase(thread), m_dmgr(thread->getDebugMgr()),
    m_backend(thread->getBackend()), m_thread_id(0) {

}

EvalThread::~EvalThread() {

}

int32_t EvalThread::eval() {
    DEBUG_ENTER("eval");
    int32_t ret = 0;

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

int32_t EvalThread::evalMethodCallContext(
        dm::IDataTypeFunction                   *method,
        vsc::dm::IModelField                    *method_ctxt,
        const std::vector<vsc::dm::ITypeExpr *> &params) {
    EvalTypeMethodCallContext invoke(this, method, method_ctxt, params);

    int32_t ret = invoke.eval();

    return ret;
}

IEvalResult *EvalThread::mkEvalResultVal(const vsc::dm::IModelVal *val) {
    if (m_ctxt) {
        return m_ctxt->mkEvalResultVal(val);
    } else {
        return new(&m_result_alloc, (val)?val->bits():0) EvalResult(
            &m_result_alloc,
            val);
    }
}

IEvalResult *EvalThread::mkEvalResultValS(int64_t val, int32_t bits) {
    if (m_ctxt) {
        return m_ctxt->mkEvalResultValS(val, bits);
    } else {
        return new(&m_result_alloc, bits) EvalResult(
            &m_result_alloc,
            bits,
            val);
    }
}

IEvalResult *EvalThread::mkEvalResultValU(uint64_t val, int32_t bits) {
    if (m_ctxt) {
        return m_ctxt->mkEvalResultValU(val, bits);
    } else {
        return new(&m_result_alloc, bits) EvalResult(
            &m_result_alloc,
            bits,
            val);
    }
}

IEvalResult *EvalThread::mkEvalResultKind(EvalResultKind kind) {
    if (m_ctxt) {
        return m_ctxt->mkEvalResultKind(kind);
    } else {
        return new(&m_result_alloc, 0) EvalResult(
            &m_result_alloc,
            kind);
    }
}

IEvalResult *EvalThread::mkEvalResultRef(vsc::dm::IModelField *ref) {
    if (m_ctxt) {
        return m_ctxt->mkEvalResultRef(ref);
    } else {
        return new(&m_result_alloc, 0) EvalResult(
            &m_result_alloc,
            ref);
    }
}

dmgr::IDebug *EvalThread::m_dbg = 0;

}
}
}
