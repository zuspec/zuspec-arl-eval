/**
 * EvalContextFunctionStatic.h
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
#include "EvalContextBase.h"

namespace zsp {
namespace arl {
namespace eval {



class EvalContextFunctionStatic : public EvalContextBase {
public:
    EvalContextFunctionStatic(
        dmgr::IDebugMgr                             *dmgr,
        vsc::solvers::IFactory                      *solvers_f,
        arl::dm::IContext                           *ctxt,
        const vsc::solvers::IRandState              *randstate,
        IEvalBackend                                *backend,
        dm::IDataTypeFunction                       *func,
        const std::vector<vsc::dm::ITypeExpr *>     &params);

    virtual ~EvalContextFunctionStatic();

    virtual int32_t eval() override;

    virtual IEval *clone() override { return 0; }

protected:
    dm::IDataTypeFunction                           *m_func;
    std::vector<vsc::dm::ITypeExpr *>               m_params;

};

}
}
}


