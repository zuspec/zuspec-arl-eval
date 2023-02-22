/**
 * TaskSolveActionSet.h
 *
 * Copyright 2022 Matthew Ballance and Contributors
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
#include <unordered_map>
#include "vsc/solvers/ICompoundSolver.h"
#include "vsc/solvers/IRandState.h"
#include "zsp/arl/dm/IContext.h"
#include "zsp/arl/dm/IModelActivityTraverse.h"
#include "zsp/arl/dm/IModelEvalIterator.h"
#include "zsp/arl/dm/IModelFieldComponent.h"
#include "zsp/arl/dm/impl/VisitorBase.h"

namespace vsc {
    class ModelFieldRefConstraintData;
    using ModelFieldRefConstraintDataUP=std::unique_ptr<ModelFieldRefConstraintData>;
}

namespace zsp {
namespace arl {
namespace eval {


class TaskSolveActionSet : public dm::VisitorBase {
public:
    TaskSolveActionSet(
        dm::IContext                *ctxt,
        vsc::solvers::IRandState    *randstate,
        dm::IModelFieldComponent    *comp);

    virtual ~TaskSolveActionSet();

    // TODO: need to pass in bind info?
    bool solve(const std::vector<dm::IModelActivityTraverse *> &traversals);

	virtual void visitModelFieldRef(vsc::dm::IModelFieldRef *f) override;

	virtual void visitTypeFieldClaim(dm::ITypeFieldClaim *f) override;

	virtual void visitTypeFieldInOut(dm::ITypeFieldInOut *f) override;

private:
    void build_comp_map(const std::vector<dm::IModelActivityTraverse *> &traversals);

    void build_resource_constraints(
        std::vector<vsc::dm::IModelConstraintUP> &constraints);

private:

    using RefSelPairT=std::pair<vsc::dm::IModelFieldRef *, vsc::dm::ModelFieldRefConstraintDataUP>;

    struct ActionData {
        ActionData(dm::IModelActivityTraverse *t) : m_traversal(t) {} 
        dm::IModelActivityTraverse                              *m_traversal;
        std::vector<uint32_t>                               m_comp_ctxt_l;
    };

    struct ResourceClaimData {

        ResourceClaimData(vsc::dm::IDataType *t) : m_res_t(t) {}
        vsc::dm::IDataType                              *m_res_t;
        std::vector<std::pair<int32_t,int32_t>>     m_comp_sz_l;
        std::vector<vsc::dm::IModelField *>             m_resource_l;
        std::vector<vsc::dm::IRefSelector *>            m_lock_claims;
        std::vector<vsc::dm::IRefSelector *>            m_share_claims;
    };

    using AllCompMapT=std::unordered_map<dm::IModelFieldComponent *,uint32_t>;
    using ResTypeMapT=std::unordered_map<vsc::dm::IDataType *, ResourceClaimData>;


private:
    static dmgr::IDebug                                     *m_dbg;
    dm::IContext                                            *m_ctxt;
    vsc::solvers::IRandState                                *m_randstate;
    dm::IModelFieldComponent                                *m_comp;
    AllCompMapT                                             m_all_comp_m;
    std::vector<dm::IModelFieldComponent *>                 m_all_comp_l;
    ActionData                                              *m_action_data;
    std::vector<ActionData>                                  m_action_data_l;
    ResTypeMapT                                             m_res_type_m;
    std::vector<ResourceClaimData *>                        m_res_type_l;
    std::vector<vsc::dm::IRefSelectorUP>                        m_ref_l;

//    std::unordered_map<vsc::dm::IDataTy
    std::vector<dm::IModelEvalIteratorUP>                       m_result;
    std::vector<vsc::dm::IModelConstraint *>                    m_constraints;

};

}
}
}
