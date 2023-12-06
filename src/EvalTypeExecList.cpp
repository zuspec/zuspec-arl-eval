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
    int32_t                             vp_id,
    const std::vector<dm::ITypeExecUP>  &execs) :
        EvalBase(ctxt, thread, vp_id), m_execs(execs), m_idx(0) {
    DEBUG_INIT("EvalTypeExecList", ctxt->getDebugMgr());
}

EvalTypeExecList::EvalTypeExecList(const EvalTypeExecList *o) :
    EvalBase(o), m_execs(o->m_execs), m_idx(o->m_idx) {

}

EvalTypeExecList::~EvalTypeExecList() {

}

int32_t EvalTypeExecList::eval() {
    DEBUG_ENTER("[%d] eval size=%d idx=%d", getIdx(), m_execs.size(), m_idx);

    if (m_initial) {
        m_thread->pushEval(this);

        // Safety...
        setFlags(EvalFlags::Complete);
    }

    int32_t ret = 0;

    if (m_idx < m_execs.size()) {
        while (m_idx < m_execs.size()) {
            clrFlags(EvalFlags::Complete);
            m_execs.at(m_idx)->accept(m_this);
            m_idx++;

            if (!hasFlags(EvalFlags::Complete)) {
                ret = true;
                break;
            }
        }
    }

    if (m_initial) {
        m_initial = false;
        if (!hasFlags(EvalFlags::Complete)) {
            m_thread->suspendEval(this);
        } else {
            m_thread->popEval(this);
        }
    }

    DEBUG_LEAVE("[%d] eval (%d)", getIdx(), ret);
    return ret;
}

IEval *EvalTypeExecList::clone() {
    return new EvalTypeExecList(this);
}

void EvalTypeExecList::visitTypeExecProc(dm::ITypeExecProc *e) {
    DEBUG_ENTER("visitTypeExecProc vp_id=%d", m_vp_id);

    EvalTypeProcStmtScope evaluator(
        m_ctxt, 
        m_thread, 
        m_vp_id,
        e->getBody());

    if (evaluator.eval()) {
        clrFlags(EvalFlags::Complete);
    }

    DEBUG_LEAVE("visitTypeExecProc");
}

}
}
}
