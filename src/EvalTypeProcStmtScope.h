/**
 * EvalTypeProcStmtScope.h
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
#include "zsp/arl/dm/impl/ModelBuildContext.h"
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "zsp/arl/eval/IEval.h"
#include "EvalBase.h"

namespace zsp {
namespace arl {
namespace eval {



class EvalTypeProcStmtScope :
    public virtual IEval,
    public virtual EvalBase {
public:
    EvalTypeProcStmtScope(
        IEvalContext                *ctxt,
        IEvalThread                 *thread,
        int32_t                     vp_id,
        dm::ITypeProcStmtScope      *scope
    );

    EvalTypeProcStmtScope(EvalTypeProcStmtScope *other);

    virtual ~EvalTypeProcStmtScope();

    virtual int32_t eval() override;

    virtual vsc::dm::ValRef getImmVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) override;

    virtual vsc::dm::ValRef getMutVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) override;

    virtual IEval *clone() override;


private:
    static dmgr::IDebug                     *m_dbg;
    dm::ITypeProcStmtScope                  *m_scope;
    std::vector<vsc::dm::IModelFieldUP>     m_locals;
    uint32_t                                m_idx;
    uint32_t                                m_subidx;
    dm::IModelBuildContextUP                m_build_ctxt;

};

}
}
}


