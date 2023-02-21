/**
 * TaskBuildActivityComponentMap.h
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
#include <unordered_map>
#include <vector>
#include "zsp/arl/dm/IModelActivity.h"
#include "zsp/arl/dm/IModelFieldAction.h"
#include "zsp/arl/dm/IModelFieldComponent.h"
#include "zsp/arl/dm/impl/VisitorBase.h"

namespace zsp {
namespace arl {
namespace eval {

struct ActivityComponentMap {

    using AllCompMapT=std::unordered_map<dm::IModelFieldComponent *, uint32_t>;

    AllCompMapT                             all_comp_m;
    std::vector<dm::IModelFieldComponent *> all_comp_l;
};
using ActivityComponentMapUP=std::unique_ptr<ActivityComponentMap>;


class TaskBuildActivityComponentMap : public dm::VisitorBase {
public:
    TaskBuildActivityComponentMap(dm::IModelFieldComponent *root_comp);

    virtual ~TaskBuildActivityComponentMap();

    ActivityComponentMap *build(dm::IModelActivity *root_activity);

private:
    dm::IModelFieldComponent                *m_root_comp;

};

}
}
}

