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
        IEvalContext                                *ctxt,
        IEvalThread                                 *thread,
        int32_t                                     vp_id,
        dm::IDataTypeFunction                       *method,
        vsc::dm::IModelField                        *method_ctxt,
        const std::vector<vsc::dm::ITypeExpr *>     &params
    );

    EvalTypeMethodCallContext(
        IEvalContext                                *ctxt,
        IEvalThread                                 *thread,
        int32_t                                     vp_id,
        dm::IDataTypeFunction                       *method,
        vsc::dm::ITypeExpr                          *method_ctxt,
        const std::vector<vsc::dm::ITypeExpr *>     &params
    );

    EvalTypeMethodCallContext(EvalTypeMethodCallContext *o);

    virtual ~EvalTypeMethodCallContext();

    virtual int32_t eval() override;

    virtual IEval *clone() override {
        return new EvalTypeMethodCallContext(this); 
    }

private:
    static dmgr::IDebug                     *m_dbg;
    int32_t                                 m_vp_id;
    dm::IDataTypeFunction                   *m_method;
    vsc::dm::ITypeExpr                      *m_method_ctxt_e;
    vsc::dm::IModelField                    *m_method_ctxt;
    std::vector<vsc::dm::ITypeExpr *>       m_params;
    uint32_t                                m_idx;
    std::vector<vsc::dm::ValRef>            m_pvals;
    IEvalStackFrameUP                       m_stack_frame;
    uint32_t                                m_param_idx;
};

}
}
}


