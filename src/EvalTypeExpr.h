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
#include "zsp/arl/eval/IEvalValProvider.h"
#include "EvalBase.h"
#include "TaskEvalCheckRegAccess.h"

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
        int32_t             vp_id,
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

    virtual void visitTypeExprPyImportRef(dm::ITypeExprPyImportRef *t) override;

    virtual void visitTypeExprPythonFieldRef(dm::ITypeExprPythonFieldRef *t) override;

    virtual void visitTypeExprPythonMethodCall(dm::ITypeExprPythonMethodCall *t) override;

    virtual void visitTypeExprPythonModuleRef(dm::ITypeExprPythonModuleRef *t) override;

protected:
    vsc::dm::ValRef                 m_val;
    vsc::dm::ITypeExpr              *m_expr;
    vsc::dm::ValRef                 m_val_lhs;
    vsc::dm::ValRef                 m_val_rhs;
    IBuiltinFuncInfo                *m_builtin_i;
    std::vector<vsc::dm::ValRef>    m_params;
    vsc::dm::ValRefStruct           m_params_f;
    dm::IDataTypeFunction           *m_func;
    TaskEvalCheckRegAccess::Result  m_isreg_res;
    uint32_t                        m_idx;
    uint32_t                        m_subidx;

};

}
}
}


