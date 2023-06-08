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
        m_val_lhs(lhs), m_val_rhs(rhs), m_idx(idx) {
    DEBUG_INIT("EvalTypeExpr", ctxt->getDebugMgr());
}

EvalTypeExpr::EvalTypeExpr(EvalTypeExpr *o) :
    EvalBase(o), m_expr(o->m_expr), m_val_lhs(o->m_val_lhs.release()),
    m_val_rhs(o->m_val_rhs.release()), m_idx(o->m_idx) {
}

EvalTypeExpr::~EvalTypeExpr() {

}

bool EvalTypeExpr::eval() {
    DEBUG_ENTER("[%d] eval", getIdx());
    if (m_initial) {
        m_thread->pushEval(this);

        // Safety
        setResult(0, EvalResultKind::Default);
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

void EvalTypeExpr::visitTypeExprMethodCallContext(dm::ITypeExprMethodCallContext *e) {

}

void EvalTypeExpr::visitTypeExprMethodCallStatic(dm::ITypeExprMethodCallStatic *e) {
    DEBUG_ENTER("visitTypeExprMethodCallStatic idx=%d", m_idx);

    switch (m_idx) {
        case 0: {
            clrResult();

            m_ctxt->getBackend()->callFuncReq(
                m_thread,
                e->getTarget()
            );

            m_idx = 1;
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
