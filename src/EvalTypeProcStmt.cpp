/*
 * EvalTypeProcStmt.cpp
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


namespace zsp {
namespace arl {
namespace eval {


EvalTypeProcStmt::EvalTypeProcStmt(
    IEvalContext            *ctxt,
    IEvalThread             *thread,
    dm::ITypeProcStmt       *stmt) : EvalBase(ctxt, thread),
        m_stmt(stmt), m_idx(0) {
    DEBUG_INIT("EvalTypeProcStmt", ctxt->getDebugMgr());

}

EvalTypeProcStmt::EvalTypeProcStmt(const EvalTypeProcStmt *o) :
    EvalBase(o), m_stmt(o->m_stmt), m_idx(o->m_idx) {

}

EvalTypeProcStmt::~EvalTypeProcStmt() {

}

bool EvalTypeProcStmt::eval() {
    DEBUG_ENTER("[%d] eval", getIdx());
    // We need to preserve this evaluator, since it contains 
    // the "stitching" logic across evaluation parts
    if (m_initial) {
        m_thread->pushEval(this);

        // Safety
        setResult(0, EvalResultKind::Default);
    }


    // In this case, we delegate to the individual visit methods
    m_stmt->accept(m_this);

    if (m_initial) {
        m_initial = false;
        if (haveResult()) {
            m_thread->popEval(this);
        } else {
            m_thread->suspendEval(this);
        }
    }

    DEBUG_LEAVE("[%d] eval %d", getIdx(), !haveResult());
    return !haveResult();
}

IEval *EvalTypeProcStmt::clone() {
    return new EvalTypeProcStmt(this);
}

void EvalTypeProcStmt::visitTypeProcStmtExpr(dm::ITypeProcStmtExpr *s) {
    DEBUG_ENTER("visitTypeProcStmtExpr");

    switch (m_idx) {
        case 0: {
            m_idx = 1; // Always move forward
            EvalTypeExpr evaluator(m_ctxt, m_thread, s->getExpr());
            if (evaluator.eval()) {
                // expression evaluation blocked
                clrResult();
                break;
            }
        }
        case 1: {
            // Result from the previous has been reflected up. 
            setResult(0, EvalResultKind::Default);
        }
    }

    DEBUG_LEAVE("visitTypeProcStmtExpr");
}

dmgr::IDebug *EvalTypeProcStmt::m_dbg = 0;

}
}
}
