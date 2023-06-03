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
        vsc::dm::ITypeExpr  *expr,
        vsc::dm::IModelVal  *lhs=0,
        vsc::dm::IModelVal  *rhs=0
    );

    virtual ~EvalTypeExpr();

    virtual bool eval() override;

    virtual IEval *clone() override;

    virtual bool isBlocked() override {
        return !haveResult();
    }


protected:
    vsc::dm::ITypeExpr      *m_expr;
    vsc::dm::IModelValUP    m_val_lhs;
    vsc::dm::IModelValUP    m_val_rhs;

};

}
}
}


