/**
 * TaskBuildActivityTraverseData.h
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
#include "zsp/arl/dm/IContext.h"
#include "zsp/arl/dm/IModelActivityTraverse.h"
#include "zsp/arl/dm/IModelFieldComponent.h"

namespace zsp {
namespace arl {
namespace eval {

struct ActivitySolveModel;

struct ActivityTraverseData {
    ActivityTraverseData(dm::IModelActivityTraverse *t) : traversal(t) {}

    dm::IModelActivityTraverse                        *traversal;
    std::vector<uint32_t>                             comp_ctxt_l;

};
using ActivityTraverseDataUP=std::unique_ptr<ActivityTraverseData>;


class TaskBuildActivityTraverseData {
public:
    TaskBuildActivityTraverseData(
        dm::IContext                *ctxt,
        ActivitySolveModel      *solve_model);

    virtual ~TaskBuildActivityTraverseData();

    ActivityTraverseData *build(
        dm::IModelFieldComponent        *root_comp,
        dm::IModelActivityTraverse      *t);

private:
    dm::IContext                            *m_ctxt;
    ActivitySolveModel                  *m_solve_model;
    dm::IModelFieldComponent                *m_root_comp;
    ActivityTraverseDataUP              m_data;
};

}
}
}

