/*
 * EvalTypeProcStmtScope.cpp
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
#include "EvalTypeExpr.h"
#include "EvalTypeProcStmt.h"
#include "EvalTypeProcStmtScope.h"


namespace zsp {
namespace arl {
namespace eval {


EvalTypeProcStmtScope::EvalTypeProcStmtScope(
    IEvalContext                *ctxt,
    IEvalThread                 *thread,
    dm::ITypeProcStmtScope      *scope) : 
        EvalBase(ctxt, thread), m_scope(scope), m_idx(0) {
    DEBUG_INIT("EvalTypeProcStmtScope", ctxt->getDebugMgr());
}

EvalTypeProcStmtScope::EvalTypeProcStmtScope(const EvalTypeProcStmtScope *o) : 
    EvalBase(o), m_scope(o->m_scope), m_idx(o->m_idx) {
}

EvalTypeProcStmtScope::~EvalTypeProcStmtScope() {

}

bool EvalTypeProcStmtScope::eval() {
    DEBUG_ENTER("[%d] eval", getIdx());

    if (m_initial) {
        m_thread->pushEval(this);
        // Safety
        setResult(EvalResult::Void());
    }

    bool ret = false;
    if (m_idx < m_scope->getStatements().size()) {
        while (m_idx < m_scope->getStatements().size()) {
            EvalTypeProcStmt evaluator(
                m_ctxt, m_thread, m_scope->getStatements().at(m_idx).get());

            m_idx++;

            if (evaluator.eval()) {
                clrResult();
                ret = true;
                break;
            }
        }
    }

    if (m_initial) {
        m_initial = false;
        if (haveResult()) {
            m_thread->popEval(this);
        } else {
            m_thread->suspendEval(this);
        }
    }

    DEBUG_LEAVE("[%d] eval %d", getIdx(), ret);
    return ret;
}

IEval *EvalTypeProcStmtScope::clone() {
    return new EvalTypeProcStmtScope(this);
}


dmgr::IDebug *EvalTypeProcStmtScope::m_dbg = 0;
 
}
}
}
