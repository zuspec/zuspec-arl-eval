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
#include "EvalTypeExpr.h"


namespace zsp {
namespace arl {
namespace eval {


EvalTypeExpr::EvalTypeExpr(
    IEvalContext        *ctxt,
    IEvalThread         *thread,
    vsc::dm::ITypeExpr  *expr,
    vsc::dm::IModelVal  *lhs,
    vsc::dm::IModelVal  *rhs) :
        EvalBase(ctxt, thread), 
        m_expr(expr),
        m_val_lhs(lhs), m_val_rhs(rhs) {

}

EvalTypeExpr::~EvalTypeExpr() {

}

bool EvalTypeExpr::eval() {
    if (m_initial) {
        m_thread->pushEval(this);
    }

    m_expr->accept(m_this);

    if (!haveResult() && m_initial) {
        m_thread->suspendEval(this);
    }

    m_initial = false;

    return !haveResult();
}

IEval *EvalTypeExpr::clone() {
    return new EvalTypeExpr(m_ctxt, m_thread, m_expr,
        m_val_lhs.release(), m_val_rhs.release());
}

}
}
}
