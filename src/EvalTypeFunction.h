/**
 * EvalTypeFunction.h
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
#include "zsp/arl/eval/IEval.h"
#include "zsp/arl/eval/IEvalValProvider.h"
#include "EvalBase.h"

namespace zsp {
namespace arl {
namespace eval {



class EvalTypeFunction : public virtual EvalBase {
public:
    EvalTypeFunction(
        IEvalContext                        *ctxt,
        IEvalThread                         *thread,
        int32_t                             vp_id,
        dm::IDataTypeFunction               *func,
        const std::vector<vsc::dm::ValRef>  &params,
        bool                                is_ctxt);

    EvalTypeFunction(const EvalTypeFunction *o);

    virtual ~EvalTypeFunction();

    virtual int32_t eval() override;

    virtual IEval *clone() override;

    virtual vsc::dm::ValRef getImmVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) override;

    virtual vsc::dm::ValRef getMutVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) override;

protected:
    dm::IDataTypeFunction               *m_func;
    std::vector<vsc::dm::ValRef>        m_params;
    bool                                m_is_ctxt;

};

}
}
}


