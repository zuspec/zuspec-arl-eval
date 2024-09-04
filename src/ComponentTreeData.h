/**
 * ComponentTreeData.h
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
#include <map>
#include "zsp/arl/dm/IDataTypeComponent.h"
#include "zsp/arl/eval/IComponentTreeData.h"

namespace zsp {
namespace arl {
namespace eval {

class ComponentTreeData;
using ComponentTreeDataUP=vsc::dm::UP<ComponentTreeData>;
class ComponentTreeData :
    public virtual IComponentTreeData {
public:
    class ComponentInfo;
    using ComponentInfoUP=vsc::dm::UP<ComponentInfo>;
    class ComponentInfo {
    public:
        ComponentInfo(arl::dm::IDataTypeComponent *comp_t) :
            m_comp_t(comp_t) {
        }

        void addRootId(int32_t id) {
            m_root_id_l.push_back(id);
        }

        const TypeM &getTypeM() const {
            return m_type_m;
        }

        TypeM &getTypeM() {
            return m_type_m;
        }

        void addRel(int32_t p_id, int32_t c_id) {
            int32_t p_idx=-1, c_idx=-1;

            for (TypeM::const_iterator 
                it=m_type_m.begin();
                it!=m_type_m.end(); it++) {
                for (int32_t p_it=0; p_it<it->parent_id.size(); p_it++) {
                    if (it->parent_id.at(p_it) == p_id) {
                        p_idx = p_it;
                        break;
                    }
                }
            }

            if (p_idx == -1) {
                p_idx = m_type_m.size();
                m_type_m.push_back(TypeMapE());
                m_type_m.back().parent_id.push_back(p_id);
            }

            m_type_m.at(p_idx).child_id.push_back(c_id);
        }

        int32_t                                                         m_comp_id;
        arl::dm::IDataTypeComponent                                     *m_comp_t;
        std::vector<int32_t>                                            m_root_id_l;
        std::map<arl::dm::IDataTypeComponent *, std::vector<int32_t>>   m_type_offset_m;
        TypeM                                                           m_type_m;
    };

public:
    ComponentTreeData();

    virtual ~ComponentTreeData();

    ComponentInfo *getCompInfo(
        arl::dm::IDataTypeComponent     *t,
        bool                            create);

    virtual const TypeM &getTypeMap(arl::dm::IDataTypeComponent *t) override;

    std::vector<int32_t> getParents(arl::dm::IDataTypeComponent *t);

    std::vector<int32_t> getChildren(
        arl::dm::IDataTypeComponent     *t,
        int32_t                         parent_id);

private:
    std::vector<ComponentInfoUP>                                m_comp_type_l;
    std::map<arl::dm::IDataTypeComponent *, ComponentInfo *>    m_comp_type_m;
    TypeM                                                       m_empty;


};

}
}
}


