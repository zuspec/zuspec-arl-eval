/**
 * EvalTypeActionIncrElab.h
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
#include "zsp/arl/dm/impl/ValRefAction.h"
#include "zsp/arl/eval/IEval.h"
#include "EvalBase.h"
#include "EvalContextIncrElab.h"
#include "EvalValProviderStructThread.h"

namespace zsp {
namespace arl {
namespace eval {



class EvalTypeActionIncrElab :
    public virtual IEval,
    public virtual EvalBase {
public:
    EvalTypeActionIncrElab(
        EvalContextIncrElab             *ctxt,
        IEvalThread                     *thread,
        const vsc::dm::ValRef           &comp,
        const vsc::dm::ValRef           &action
    );

    EvalTypeActionIncrElab(const EvalTypeActionIncrElab *o);

    virtual ~EvalTypeActionIncrElab();

    virtual int32_t eval() override;

    virtual IEval *clone() override;

    virtual IEvalValProvider *getValProvider() override;

private:
    static dmgr::IDebug                 *m_dbg;
    int32_t                             m_idx;
    vsc::dm::ValRefStruct               m_comp;
    dm::ValRefAction                    m_action;
    EvalValProviderStructThread         m_vp;

};

}
}
}


