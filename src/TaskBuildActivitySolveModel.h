/**
 * TaskBuildActivitySolveModel.h
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
#include <memory>
#include <vector>
#include "zsp/arl/dm/IContext.h"
#include "zsp/arl/dm/impl/VisitorBase.h"

namespace zsp {
namespace arl {
namespace eval {

struct ResourceClaimData {
    ResourceClaimData(vsc::dm::IDataType *t) : res_t(t) {}

    vsc::dm::IDataType                                  *res_t;
    std::vector<std::pair<int32_t,int32_t>>         comp_sz_l;
    std::vector<vsc::dm::IModelField *>                 resource_l;
};

struct ActivityTraverseData;
using ActivityTraverseDataUP=std::unique_ptr<ActivityTraverseData>;

struct ActivitySolveModel {
    std::vector<vsc::dm::IModelConstraintUP>            constraints;

    using AllCompMapT=std::unordered_map<dm::IModelFieldComponent *, uint32_t>;
    using ResTypeMapT=std::unordered_map<vsc::dm::IDataType *, ResourceClaimData>;
    using TraversalDataMapT=std::unordered_map<dm::IModelActivityTraverse *, ActivityTraverseData *>;

    AllCompMapT                                     all_comp_m;
    std::vector<dm::IModelFieldComponent *>         all_comp_l;

    ResTypeMapT                                     res_type_m;
    std::vector<ResourceClaimData *>                res_type_l;

    TraversalDataMapT                               traversal_m;
    std::vector<ActivityTraverseDataUP>             traversal_l;

    // TODO: need per-buffer-type data

    // TODO: need per-stream-type data

    // TODO: need per-state-type data


    std::vector<vsc::dm::IRefSelectorUP>                ref_l;

    uint32_t getComponentId(dm::IModelFieldComponent *c);
};
using ActivitySolveModelUP=std::unique_ptr<ActivitySolveModel>;

class TaskBuildActivitySolveModel : public dm::VisitorBase {
public:
    TaskBuildActivitySolveModel(dm::IContext *ctxt);

    virtual ~TaskBuildActivitySolveModel();

    ActivitySolveModel *build(
        dm::IModelFieldComponent        *root_comp,
        dm::IModelActivity              *root_activity);

	virtual void visitModelActivityParallel(dm::IModelActivityParallel *a) override;

	virtual void visitModelActivitySchedule(dm::IModelActivitySchedule *a) override;

	virtual void visitModelActivitySequence(dm::IModelActivitySequence *a) override;

	virtual void visitModelActivityTraverse(dm::IModelActivityTraverse *a) override;


private:
    dm::IContext                                *m_ctxt;
    ActivitySolveModelUP                        m_model;
    dm::IModelFieldComponent                    *m_root_comp;
    std::vector<dm::IModelFieldComponent *>     m_component_s;

};

}
}
}

