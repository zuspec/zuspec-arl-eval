/**
 * EvalTypeMethodCallStatic.h
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
#include "vsc/dm/ITypeExpr.h"
#include "zsp/arl/eval/IEval.h"
#include "EvalBase.h"

namespace zsp {
namespace arl {
namespace eval {



class EvalTypeMethodCallStatic :
    public virtual IEval,
    public virtual EvalBase {
public:
    EvalTypeMethodCallStatic(
        IEvalContext                                *ctxt,
        IEvalThread                                 *thread,
        IEvalValProvider                            *vp,
        dm::IDataTypeFunction                       *func,
        const std::vector<vsc::dm::ITypeExpr *>     &params);

    EvalTypeMethodCallStatic(EvalTypeMethodCallStatic *o);

    virtual ~EvalTypeMethodCallStatic();

    virtual int32_t eval() override;

    virtual IEval *clone() override;

private:
    static dmgr::IDebug                             *m_dbg;
    IEvalValProvider                                *m_vp;
    dm::IDataTypeFunction                           *m_func;
    std::vector<vsc::dm::ITypeExpr *>               m_params;
    int32_t                                         m_idx;
    std::vector<vsc::dm::ValRef>                    m_pvals;
    IEvalStackFrameUP                               m_stack_frame;
    int32_t                                         m_param_idx;

};

}
}
}


