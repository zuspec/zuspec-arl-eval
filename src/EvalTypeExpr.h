/**
 * EvalTypeExpr.h
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
#pragma once
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "zsp/arl/eval/IEval.h"
#include "EvalBase.h"

namespace zsp {
namespace arl {
namespace eval {



class EvalTypeExpr :
    public virtual IEval,
    public virtual EvalBase,
    public virtual dm::VisitorBase {
public:
    EvalTypeExpr(
        IEvalContext        *ctxt,
        IEvalThread         *thread,
        vsc::dm::ITypeExpr  *expr);

    EvalTypeExpr(EvalTypeExpr *o);

    virtual ~EvalTypeExpr();

    virtual int32_t eval() override;

    virtual IEval *clone() override;

	virtual void visitTypeExprBin(vsc::dm::ITypeExprBin *e) override;

	virtual void visitTypeExprFieldRef(vsc::dm::ITypeExprFieldRef *e) override;

	virtual void visitTypeExprRange(vsc::dm::ITypeExprRange *e) override;

	virtual void visitTypeExprRangelist(vsc::dm::ITypeExprRangelist *e) override;

	virtual void visitTypeExprVal(vsc::dm::ITypeExprVal *e) override;

    virtual void visitTypeExprMethodCallContext(dm::ITypeExprMethodCallContext *e) override;

    virtual void visitTypeExprMethodCallStatic(dm::ITypeExprMethodCallStatic *e) override;

protected:
    static dmgr::IDebug         *m_dbg;
    vsc::dm::ITypeExpr          *m_expr;
    IEvalResultUP               m_val_lhs;
    IEvalResultUP               m_val_rhs;
    std::vector<IEvalResultUP>  m_params;
    uint32_t                    m_idx;
    uint32_t                    m_subidx;

};

}
}
}


