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
#include "zsp/arl/eval/IEvalContextInt.h"
#include "EvalTypeExpr.h"
#include "EvalTypeProcStmt.h"


namespace zsp {
namespace arl {
namespace eval {


EvalTypeProcStmt::EvalTypeProcStmt(
    IEvalContext            *ctxt,
    IEvalThread             *thread,
    int32_t                 vp_id,
    dm::ITypeProcStmt       *stmt) : EvalBase(ctxt, thread, vp_id),
        m_stmt(stmt), m_idx(0) {
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
            DEBUG("suspendEval");
            m_thread->suspendEval(this);
        } else {
            DEBUG("popEval");
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
    DEBUG_ENTER("visitTypeProcStmtAssign idx=%d", m_idx);
    switch (m_idx) {
        case 0: {
            m_idx = 1;

            clrResult();

            if (EvalTypeExpr(m_ctxt, m_thread, m_vp_id, s->getRhs()).eval()) {
                DEBUG("Suspending due to EvalTypeExpr");
                break;
            }
        }
        case 1: {
            m_idx = 2;

            DEBUG("pre-GetLval: haveResult: %d", haveResult());

            // TODO: Getting the LHS possibly could be time-consuming (?)
            // For now, cheat
            vsc::dm::ValRef lval(TaskEvalGetLval(
                    m_thread->getDebugMgr(),
                    this
                ).eval(s->getLhs()));

            DEBUG("post-GetLval: haveResult: %d", haveResult());

            if (!lval.valid()) {
                setError("null lval");
                break;
            }

            if (!haveResult()) {
                setError("No result");
                break;
            }

            fprintf(stdout, "TODO: Check of result kind\n");

            switch (s->op()) {
                case dm::TypeProcStmtAssignOp::Eq: {
                    vsc::dm::ValRefInt val_i(lval);
                    vsc::dm::ValRefInt val_r(getResult());
                    DEBUG("RHS(int): init=%lld", val_i.get_val_s());
                    val_i.set_val(val_r.get_val_u());
                    DEBUG("RHS(int): %lld (lhs=%lld)", val_r.get_val_s(), val_i.get_val_s());
                    DEBUG("LHS: flags=0x%08x", val_i.flags());
                    /*
                    DEBUG("lval.bits=%d rval.bits=%d", lval->bits(), getResult()->bits());
                    lval->set(getResult());
                     */
                } break;
                default: FATAL("unsupported assign op %d", s->op());
            }
#ifdef UNDEFINED
#endif

            DEBUG("Ready");
        }
        case 2: {
            DEBUG("setVoidResult");
            setVoidResult();
            DEBUG("haveResult: %d", haveResult());
        }
    }

    DEBUG_LEAVE("visitTypeProcStmtAssign");
}

void EvalTypeProcStmt::visitTypeProcStmtExpr(dm::ITypeProcStmtExpr *s) {
    DEBUG_ENTER("visitTypeProcStmtExpr");

    switch (m_idx) {
        case 0: {
            m_idx = 1; // Always move forward
            if (s->getExpr()) {
                EvalTypeExpr evaluator(m_ctxt, m_thread, m_vp_id, s->getExpr());
                if (evaluator.eval()) {
                    // expression evaluation blocked
                    clrResult();
                    break;
                }
            } else {
                ERROR("Null expression for statement");
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

            if (EvalTypeExpr(m_ctxt, m_thread, m_vp_id, s->getCond()).eval()) {
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
            if (s->getExpr()) {
                EvalTypeExpr evaluator(m_ctxt, m_thread, m_vp_id, s->getExpr());

                if (evaluator.eval()) {
                    clrResult();
                    break;
                }
            } else {
                // TODO: need to do something different here?
                DEBUG("Void return statement");
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
