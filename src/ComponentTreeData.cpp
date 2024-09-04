/*
 * ComponentTreeData.cpp
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
#include "ComponentTreeData.h"


namespace zsp {
namespace arl {
namespace eval {


ComponentTreeData::ComponentTreeData() {

}

ComponentTreeData::~ComponentTreeData() {

}

ComponentTreeData::ComponentInfo *ComponentTreeData::getCompInfo(
    arl::dm::IDataTypeComponent     *t,
    bool                            create) {
    ComponentInfo *ret = 0;
    std::map<arl::dm::IDataTypeComponent *, ComponentInfo *>::const_iterator it;

    if ((it=m_comp_type_m.find(t)) != m_comp_type_m.end()) {
        ret = it->second;
    } else if (create) {
        ComponentInfo *info = new ComponentInfo(t);
        m_comp_type_l.push_back(ComponentInfoUP(info));
        m_comp_type_m.insert({t, info});
        ret = info;
    }

    return ret;
}

const IComponentTreeData::TypeM &ComponentTreeData::getTypeMap(arl::dm::IDataTypeComponent *t) {
    ComponentInfo *info = getCompInfo(t, false);

    if (info) {
        return info->getTypeM();
    } else {
        return m_empty;
    }
}

std::vector<int32_t> ComponentTreeData::getParents(arl::dm::IDataTypeComponent *t) {
    std::vector<int32_t> ret;

    for (std::vector<ComponentInfoUP>::const_iterator
        it=m_comp_type_l.begin();
        it!=m_comp_type_l.end(); it++) {

    }

    return ret;
}

std::vector<int32_t> ComponentTreeData::getChildren(
        arl::dm::IDataTypeComponent     *t,
        int32_t                         parent_id) {

}

}
}
}
