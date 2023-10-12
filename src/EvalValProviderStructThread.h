/**
 * EvalValProviderStructThread.h
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
#include "vsc/dm/impl/ValRefStruct.h"
#include "zsp/arl/eval/IEvalThread.h"
#include "zsp/arl/eval/IEvalValProvider.h"

namespace zsp {
namespace arl {
namespace eval {



class EvalValProviderStructThread : public virtual IEvalValProvider {
public:
    EvalValProviderStructThread(IEvalThread *thread);

    EvalValProviderStructThread(
        IEvalThread                     *thread,
        const vsc::dm::ValRef           &scope);

    EvalValProviderStructThread(const EvalValProviderStructThread &o);

    virtual ~EvalValProviderStructThread();

    void setScope(const vsc::dm::ValRefStruct &scope);

    virtual vsc::dm::ValRef getImmVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) const override;

    virtual vsc::dm::ValRef getMutVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) override;

private:
    IEvalThread                     *m_thread;
    vsc::dm::ValRef                 m_scope;

};

}
}
}


