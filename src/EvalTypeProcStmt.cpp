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
#include "zsp/arl/eval/impl/TaskEvalGetLval.h"
#include "EvalTypeExpr.h"
#include "EvalTypeProcStmt.h"


namespace zsp {
namespace arl {
namespace eval {


EvalTypeProcStmt::EvalTypeProcStmt(
    IEvalContext            *ctxt,
    IEvalThread             *thread,
    IEvalValProvider        *vp,
    dm::ITypeProcStmt       *stmt) : EvalBase(ctxt, thread),
        m_vp(vp), m_stmt(stmt), m_idx(0) {
    DEBUG_INIT("EvalTypeProcStmt", ctxt->getDebugMgr());

}

EvalTypeProcStmt::EvalTypeProcStmt(const EvalTypeProcStmt *o) :
    EvalBase(o), m_stmt(o->m_stmt), m_idx(o->m_idx) {

}

EvalTypeProcStmt::~EvalTypeProcStmt() {

}

int32_t EvalTypeProcStmt::eval() {
    DEBUG_ENTER("[%d] eval", getIdx());
    // We need to preserve this evaluator, since it contains 
    // the "stitching" logic across evaluation parts
    if (m_initial) {
        m_thread->pushEval(this);

        // Safety
        setVoidResult();
    }


    // In this case, we delegate to the individual visit methods
    m_stmt->accept(m_this);

    int32_t ret = !haveResult();

    if (m_initial) {
        m_initial = false;
        if (ret) {
            m_thread->suspendEval(this);
        } else {
            m_thread->popEval(this);
        }
    }

    DEBUG_LEAVE("[%d] eval %d", getIdx(), ret);
    return ret;
}

IEval *EvalTypeProcStmt::clone() {
    return new EvalTypeProcStmt(this);
}

void EvalTypeProcStmt::visitTypeProcStmtAssign(dm::ITypeProcStmtAssign *s) {
    DEBUG_ENTER("visitTYpeProcStmtAssign");
    switch (m_idx) {
        case 0: {
            m_idx = 1;

            if (EvalTypeExpr(m_ctxt, m_thread, m_vp, s->getRhs()).eval()) {
                break;
            }
        }
        case 1: {
            // TODO: Getting the LHS possibly could be time-consuming (?)
            // For now, cheat
            vsc::dm::ValRef lval(TaskEvalGetLval(m_thread).eval(s->getLhs()));
            if (!lval.valid()) {
                FATAL("null lval");
            }

            if (!haveResult()) {
                FATAL("No result");
            }

            fprintf(stdout, "TODO: Check of result kind\n");

            switch (s->op()) {
                case dm::TypeProcStmtAssignOp::Eq: {
                    /*
                    DEBUG("lval.bits=%d rval.bits=%d", lval->bits(), getResult()->bits());
                    lval->set(getResult());
                     */
                } break;
                default: FATAL("unsupported assign op %d", s->op());
            }
        }
        case 2: {
            setVoidResult();
        }
    }

    DEBUG_LEAVE("visitTYpeProcStmtAssign");
}

void EvalTypeProcStmt::visitTypeProcStmtExpr(dm::ITypeProcStmtExpr *s) {
    DEBUG_ENTER("visitTypeProcStmtExpr");

    switch (m_idx) {
        case 0: {
            m_idx = 1; // Always move forward
            EvalTypeExpr evaluator(m_ctxt, m_thread, m_vp, s->getExpr());
            if (evaluator.eval()) {
                // expression evaluation blocked
                clrResult();
                break;
            }
        }
        case 1: {
            // Result from the previous has been reflected up. 
            setVoidResult();
        }
    }

    DEBUG_LEAVE("visitTypeProcStmtExpr");
}

void EvalTypeProcStmt::visitTypeProcStmtIfElse(dm::ITypeProcStmtIfElse *s) {
    DEBUG_ENTER("visitTYpeProcStmtIfElse");
    
    switch (m_idx) {
        case 0: {
            // Evaluate condition
            m_idx = 1;

            if (EvalTypeExpr(m_ctxt, m_thread, m_vp, s->getCond()).eval()) {
                clrResult();
                break;
            }
        }
        case 1: {
            m_idx = 2;
            // Have the condition result
            fprintf(stdout, "TODO: process condition result\n");
            /*
            DEBUG("Result: kind=%d val_u=%lld", getResult()->getKind(), getResult()->val_u());
            if (getResult()->val_u() != 0) {
                DEBUG("True branch");
                if (EvalTypeProcStmt(m_ctxt, m_thread, s->getTrue()).eval()) {
                    clrResult();
                    break;
                }
            } else if (s->getFalse()) {
                DEBUG("False branch");
                if (EvalTypeProcStmt(m_ctxt, m_thread, s->getFalse()).eval()) {
                    clrResult();
                    break;
                }
            }
             */
        }
        case 2: {
            // 
        }
    }
    DEBUG_LEAVE("visitTYpeProcStmtIfElse");
}

void EvalTypeProcStmt::visitTypeProcStmtReturn(dm::ITypeProcStmtReturn *s) {
    DEBUG_ENTER("visitTypeProcStmtReturn");

    switch (m_idx) {
        case 0: {
            m_idx = 1;
            EvalTypeExpr evaluator(m_ctxt, m_thread, m_vp, s->getExpr());

            if (evaluator.eval()) {
                clrResult();
                break;
            }
        }

        case 1: 
            DEBUG("case 1 (result: %p)", getResult()); 
            break;
    }

    DEBUG_LEAVE("visitTypeProcStmtReturn");
}

dmgr::IDebug *EvalTypeProcStmt::m_dbg = 0;

}
}
}
