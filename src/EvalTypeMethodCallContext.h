/**
 * EvalTypeMethodCallContext.h
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

class EvalTypeMethodCallContext : 
    public virtual IEval,
    public virtual EvalBase,
    dm::VisitorBase {
public:
    EvalTypeMethodCallContext(
        IEvalThread                                 *thread,
        dm::IDataTypeFunction                       *method,
        vsc::dm::IModelField                        *method_ctxt,
        const std::vector<vsc::dm::ITypeExpr *>     &params
    );

    EvalTypeMethodCallContext(EvalTypeMethodCallContext *o);

    virtual ~EvalTypeMethodCallContext();

    virtual int32_t eval() override;

    virtual IEval *clone() override {
        return new EvalTypeMethodCallContext(this); 
    }

private:
    dm::IDataTypeFunction                   *m_method;
    vsc::dm::IModelField                    *m_method_ctxt;
    std::vector<vsc::dm::ITypeExpr *>       m_params;
    std::vector<IEvalResultUP>              m_pvals;
    uint32_t                                m_idx;
    uint32_t                                m_subidx;
};

}
}
}


