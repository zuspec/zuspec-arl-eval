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
#include "TaskEvalCheckRegAccess.h"
#include "EvalTypeExpr.h"


namespace zsp {
namespace arl {
namespace eval {


EvalTypeExpr::EvalTypeExpr(
    IEvalContext        *ctxt,
    IEvalThread         *thread,
    IEvalValProvider    *vp,
    vsc::dm::ITypeExpr  *expr) :
        EvalBase(ctxt, thread), m_vp(vp),
        m_expr(expr), m_idx(0), m_subidx(0) {
    DEBUG_INIT("EvalTypeExpr", ctxt->getDebugMgr());
}

EvalTypeExpr::EvalTypeExpr(EvalTypeExpr *o) :
    EvalBase(o), m_vp(o->m_vp), m_expr(o->m_expr), 
    m_val_lhs(std::move(o->m_val_lhs)),
    m_val_rhs(std::move(o->m_val_rhs)), 
    m_idx(o->m_idx), m_subidx(o->m_subidx) {
}

EvalTypeExpr::~EvalTypeExpr() {

}

int32_t EvalTypeExpr::eval() {
    DEBUG_ENTER("[%d] eval", getIdx());
    if (m_initial) {
        m_thread->pushEval(this);

        // Safety
        setVoidResult();
    }

    m_expr->accept(m_this);

    int32_t ret = !haveResult();

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
            EvalTypeExpr evaluator(m_ctxt, m_thread, m_vp, e->lhs());

            if (evaluator.eval()) {
                break;
            }
        }
        case 1: { // Operand 2
            m_idx = 2;
            setVoidResult();

            EvalTypeExpr evaluator(m_ctxt, m_thread, m_vp, e->rhs());

            if (evaluator.eval()) {
                break;
            }

        }
        case 2: { // Perform operation
            m_idx = 3;
            m_val_rhs.set(moveResult()); // Preserve RHS

            vsc::dm::IModelValOp *op = m_ctxt->getModelValOp();
            // Copy size of the RHS for now
            // TODO: need to consider context
#ifdef UNDEFINED
            IEvalResultUP val = IEvalResultUP(m_ctxt->mkEvalResultVal(m_val_rhs.get()));

            switch (e->op()) {
                case vsc::dm::BinOp::Eq: {
                    op->eq(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Ne: {
                    op->ne(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Gt: {
                    // TODO: determine sign of operands
                    op->sgt(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Ge: {
                    // TODO: determine sign of operands
                    op->sge(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Lt: {
                    // TODO: determine sign of operands
                    op->slt(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Le: {
                    // TODO: determine sign of operands
                    op->sle(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Add: {
                    op->add(
                        val.get(), 
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Sub: {
                    op->sub(
                        val.get(), 
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Div: {
                    fprintf(stdout, "TODO: BinOp::Div\n");
                    /*
                    op->div(
                        val.get(), 
                        m_val_lhs.get(),
                        m_val_rhs.get());
                     */
                } break;
                case vsc::dm::BinOp::BinAnd: {
                    op->bin_and(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::BinOr: {
                    op->bin_or(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::BinXor: {
                    op->bin_xor(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::LogAnd: {
                    op->log_and(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::LogOr: {
                    op->log_or(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Sll: {
                    op->sll(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Srl: {
                    op->srl(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Xor: {
                    op->log_xor(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Not: {
                    fprintf(stdout, "TODO: BinOp::Not\n");
                    /*
                    op->log_xor(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                     */
                } break;
                default:
//                    ERROR("")
                    break;
            }

            setResult(val.release());
#endif
        }

        case 3: {
            // Idle
        }
    }
    DEBUG_LEAVE("visitTypeExprBin");
}

void EvalTypeExpr::visitTypeExprFieldRef(vsc::dm::ITypeExprFieldRef *e) { 
    DEBUG_ENTER("visitTypeExprFieldRef");
    switch (e->getRootRefKind()) {
        case vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope: {
            DEBUG("Bottom-up scope");
            IEvalStackFrame *frame = m_thread->stackFrame(e->getPath().at(0));
            vsc::dm::ValRef val(frame->getVariable(e->getPath().at(1)));

            if (e->getPath().size() > 2) {
                DEBUG("TODO: stack-local refernece deeper than 2");
            }

            // TODO:
            fprintf(stdout, "TODO: Set cloned result\n");
//              setResult(dynamic_cast<IEvalResult *>(val->clone()));
        } break;

        case vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope: {
            DEBUG("Top-down scope");
        } break;

        case vsc::dm::ITypeExprFieldRef::RootRefKind::RootExpr: {
            DEBUG("Root expr");
        } break;
    }

    DEBUG_LEAVE("visitTypeExprFieldRef");
}

void EvalTypeExpr::visitTypeExprRange(vsc::dm::ITypeExprRange *e) { 

}

void EvalTypeExpr::visitTypeExprRangelist(vsc::dm::ITypeExprRangelist *e) { 

}

void EvalTypeExpr::visitTypeExprVal(vsc::dm::ITypeExprVal *e) { 
    DEBUG_ENTER("visitTypeExprVal");
    setResult(e->val().toImmutable());
    DEBUG_LEAVE("visitTypeExprVal");
}

void EvalTypeExpr::visitTypeExprMethodCallContext(dm::ITypeExprMethodCallContext *e) {
    DEBUG_ENTER("visitTypeExprMethodCallContext");
    const TaskEvalCheckRegAccess::Result &res = TaskEvalCheckRegAccess(
            m_ctxt,
            m_vp).check(
        e->getContext(),
        e->getTarget());
    
    if (res.root.valid()) {
        DEBUG("Is a register access");
    } else {
        DEBUG("Is NOT a register access");
    }

    // Determine whether this is a call to a register
    // If so: 
    // - What is the base address?
    // - What is the offset expression?
    // Otherwise:
    // - Know this is a model-internal call
    // - TODO: need a Invoke PSS Function stage

    DEBUG_LEAVE("visitTypeExprMethodCallContext");
}

void EvalTypeExpr::visitTypeExprMethodCallStatic(dm::ITypeExprMethodCallStatic *e) {
    DEBUG_ENTER("visitTypeExprMethodCallStatic idx=%d", m_idx);

    switch (m_idx) {
        case 0: {
            if (m_subidx > 0 && haveResult()) {
                m_params.push_back(vsc::dm::ValRef(moveResult()));
            }
            while (m_subidx < e->getParameters().size()) {
                EvalTypeExpr evaluator(
                    m_ctxt, 
                    m_thread, 
                    m_vp,
                    e->getParameters().at(m_subidx).get());

                m_subidx += 1;
                clrResult();
                if (evaluator.eval()) {
                    break;
                } else {
                    if (haveResult()) {
                        fprintf(stdout, "Note: push expr result\n");
                        m_params.push_back(vsc::dm::ValRef(moveResult()));
                    }
                }
            }

            // If we left the loop before achieving enough
            // parameters, suspend...
            if (m_subidx < e->getParameters().size()) {
                break;
            }

            clrResult(); // Clear 'safety' result

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
