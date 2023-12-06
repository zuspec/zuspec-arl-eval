/**
 * EvalTypeExprRegOffset.h
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
#include "EvalTypeExpr.h"

namespace zsp {
namespace arl {
namespace eval {



class EvalTypeExprRegOffset : public virtual EvalTypeExpr {
public:
    EvalTypeExprRegOffset(
        IEvalContext        *ctxt,
        IEvalThread         *thread,
        int32_t             vp_id,
        vsc::dm::ITypeExpr  *expr);

    EvalTypeExprRegOffset(EvalTypeExprRegOffset *o);

    virtual ~EvalTypeExprRegOffset();

    virtual int32_t eval() override;

    virtual IEval *clone() override;

	virtual void visitTypeExprFieldRef(vsc::dm::ITypeExprFieldRef *e) override;

protected:
    uint64_t                m_value;
    bool                    m_offset_mode;

};

}
}
}


