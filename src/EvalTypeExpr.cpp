/*
 * EvalTypeExpr.cpp
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
#include "vsc/dm/ITypeExprVal.h"
#include "EvalTypeExpr.h"


namespace zsp {
namespace arl {
namespace eval {


EvalTypeExpr::EvalTypeExpr(
    IEvalContext        *ctxt,
    IEvalThread         *thread,
    vsc::dm::ITypeExpr  *expr,
    vsc::dm::IModelVal  *lhs,
    vsc::dm::IModelVal  *rhs,
    uint32_t            idx) :
        EvalBase(ctxt, thread), 
        m_expr(expr),
        m_val_lhs(lhs), m_val_rhs(rhs), m_idx(idx), m_subidx(0) {
    DEBUG_INIT("EvalTypeExpr", ctxt->getDebugMgr());
}

EvalTypeExpr::EvalTypeExpr(EvalTypeExpr *o) :
    EvalBase(o), m_expr(o->m_expr), m_val_lhs(o->m_val_lhs),
    m_val_rhs(o->m_val_rhs), m_idx(o->m_idx), m_subidx(o->m_subidx) {
}

EvalTypeExpr::~EvalTypeExpr() {

}

bool EvalTypeExpr::eval() {
    DEBUG_ENTER("[%d] eval", getIdx());
    if (m_initial) {
        m_thread->pushEval(this);

        // Safety
        setResult(EvalResult::Void());
    }


    m_expr->accept(m_this);

    bool ret = !haveResult();

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

IEval *EvalTypeExpr::clone() {
    return new EvalTypeExpr(this);
}

void EvalTypeExpr::visitTypeExprBin(vsc::dm::ITypeExprBin *e) { 
    DEBUG_ENTER("visitTypeExprBin");
    switch (m_idx) {
        case 0: { // Operand 1
            m_idx = 1;
            EvalTypeExpr evaluator(m_ctxt, m_thread, e->lhs());

            if (evaluator.eval()) {
                break;
            }
        }
        case 1: { // Operand 2
            m_idx = 2;
            m_val_lhs = moveResult(); // Preserve LHS

            EvalTypeExpr evaluator(m_ctxt, m_thread, e->rhs());

            if (evaluator.eval()) {
                break;
            }

        }
        case 2: { // Perform operation
            m_idx = 3;
            m_val_rhs = moveResult(); // Preserve RHS

            vsc::dm::IModelValOp *op = m_ctxt->getModelValOp();
            vsc::dm::IModelVal *val = m_ctxt->mkModelValU();

            switch (e->op()) {
                case vsc::dm::BinOp::Add: {
                    op->add(
                        val, 
                        m_val_lhs.val.get(),
                        m_val_rhs.val.get());
                } break;
                case vsc::dm::BinOp::BinAnd: {
                    op->bin_and(
                        val,
                        m_val_lhs.val.get(),
                        m_val_rhs.val.get());
                } break;
                case vsc::dm::BinOp::BinOr: {
                    op->bin_or(
                        val,
                        m_val_lhs.val.get(),
                        m_val_rhs.val.get());
                } break;
                case vsc::dm::BinOp::BinXor: {
                    op->bin_xor(
                        val,
                        m_val_lhs.val.get(),
                        m_val_rhs.val.get());
                } break;
            }

            EvalResult res(val);
            moveResult(res);
        }

        case 3: {
            // Idle
        }
    }
    DEBUG_LEAVE("visitTypeExprBin");
}

void EvalTypeExpr::visitTypeExprFieldRef(vsc::dm::ITypeExprFieldRef *e) { 

}

void EvalTypeExpr::visitTypeExprRange(vsc::dm::ITypeExprRange *e) { 

}

void EvalTypeExpr::visitTypeExprRangelist(vsc::dm::ITypeExprRangelist *e) { 

}

void EvalTypeExpr::visitTypeExprVal(vsc::dm::ITypeExprVal *e) { 
    DEBUG_ENTER("visitTypeExprVal");
    EvalResult res(e->val()->clone());
    moveResult(res);
    DEBUG("haveResult: %d ; val: %p", haveResult(), getResult().val.get());
    DEBUG_LEAVE("visitTypeExprVal");
}

void EvalTypeExpr::visitTypeExprMethodCallContext(dm::ITypeExprMethodCallContext *e) {

}

void EvalTypeExpr::visitTypeExprMethodCallStatic(dm::ITypeExprMethodCallStatic *e) {
    DEBUG_ENTER("visitTypeExprMethodCallStatic idx=%d", m_idx);

    switch (m_idx) {
        case 0: {
            if (m_subidx > 0 && haveResult()) {
                EvalResult r = moveResult();
                m_params.push_back(r);
            }
            while (m_subidx < e->getParameters().size()) {
                EvalTypeExpr evaluator(m_ctxt, m_thread, e->getParameters().at(m_subidx).get());

                m_subidx += 1;
                clrResult();
                if (evaluator.eval()) {
                    break;
                } else {
                    if (haveResult()) {
                        fprintf(stdout, "Note: push expr result\n");
                        EvalResult r = moveResult();
                        m_params.push_back(r);
                    }
                }
            }

            // If we left the loop before achieving enough
            // parameters, suspend...
            if (m_subidx < e->getParameters().size()) {
                break;
            }

            m_idx = 1;
            m_ctxt->getBackend()->callFuncReq(
                m_thread,
                e->getTarget(),
                m_params
            );

            if (!haveResult()) {
                break;
            }
        }

        case 1: {
            // Wait for a response

        }
    }

    DEBUG_LEAVE("visitTypeExprMethodCallStatic idx=%d haveResult=%d", m_idx, haveResult());
}

dmgr::IDebug *EvalTypeExpr::m_dbg = 0;

}
}
}
