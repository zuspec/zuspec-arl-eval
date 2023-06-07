/*
 * EvalTypeExecList.cpp
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
#include "EvalTypeExecList.h"
#include "EvalTypeProcStmtScope.h"


namespace zsp {
namespace arl {
namespace eval {


EvalTypeExecList::EvalTypeExecList(
    IEvalContext                        *ctxt,
    IEvalThread                         *thread,
    const std::vector<dm::ITypeExecUP>  &execs) :
        EvalBase(ctxt, thread), m_execs(execs), m_idx(0) {
    DEBUG_INIT("EvalTypeExecList", ctxt->getDebugMgr());
}

EvalTypeExecList::EvalTypeExecList(const EvalTypeExecList *o) :
    EvalBase(o), m_execs(o->m_execs), m_idx(o->m_idx) {

}

EvalTypeExecList::~EvalTypeExecList() {

}

bool EvalTypeExecList::eval() {
    DEBUG_ENTER("eval");

    if (m_initial) {
        m_thread->pushEval(this);
    }

    while (m_idx < m_execs.size() && !haveResult()) {

        clrResult();
        m_execs.at(m_idx)->accept(m_this);
        m_idx++;

    }

    bool ret = !haveResult();

    if (m_initial) {
        if (!haveResult()) {
            m_thread->suspendEval(this);
        } else {
            m_thread->popEval(this);
        }
    }

    DEBUG_LEAVE("eval (%d)", ret);
    return ret;
}

IEval *EvalTypeExecList::clone() {
    return new EvalTypeExecList(this);
}

void EvalTypeExecList::visitTypeExecProc(dm::ITypeExecProc *e) {
    DEBUG_ENTER("visitTypeExecProc");

    EvalTypeProcStmtScope evaluator(m_ctxt, m_thread, e->getBody());
    evaluator.eval();

    DEBUG_LEAVE("visitTypeExecProc");
}

dmgr::IDebug *EvalTypeExecList::m_dbg = 0;

}
}
}
