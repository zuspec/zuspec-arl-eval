/*
 * TaskBuildComponentTreeData.cpp
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
#include "dmgr/impl/DebugMacros.h"
#include "TaskBuildComponentTreeData.h"


namespace zsp {
namespace arl {
namespace eval {


TaskBuildComponentTreeData::TaskBuildComponentTreeData(dmgr::IDebugMgr *dmgr) {
    DEBUG_INIT("zsp::arl::eval::TaskBuildComponentTreeData", dmgr);
}

TaskBuildComponentTreeData::~TaskBuildComponentTreeData() {

}

IComponentTreeData *TaskBuildComponentTreeData::build(arl::dm::IDataTypeComponent *root) {
    DEBUG_ENTER("build");
    m_data = ComponentTreeDataUP(new ComponentTreeData());
    m_comp_id_s.clear();
    m_comp_id_s.push_back({0,0});
    root->accept(m_this);
    DEBUG_LEAVE("build");
    return m_data.release();
}

void TaskBuildComponentTreeData::visitDataTypeComponent(arl::dm::IDataTypeComponent *t) {
    DEBUG_ENTER("visitDataTypeComponent %s", t->name().c_str());
    ComponentTreeData::ComponentInfo *info = m_data->getCompInfo(t, true);

    int32_t parent_id = m_comp_id_s.back().first;
    int32_t comp_id = m_comp_id_s.back().second;

    DEBUG("addRel parent=%d comp=%d", parent_id, comp_id);
    info->addRel(parent_id, comp_id);
    if (parent_id != comp_id) {
        info->addRel(comp_id, comp_id);
    }

    m_comp_id_s.push_back({comp_id, comp_id+1});
    for (std::vector<vsc::dm::ITypeFieldUP>::const_iterator
        it=t->getFields().begin();
        it!=t->getFields().end(); it++) {
        (*it)->accept(m_this);
    }
    comp_id = m_comp_id_s.back().second;
    m_comp_id_s.pop_back();
    m_comp_id_s.back().second = comp_id;

    DEBUG_LEAVE("visitDataTypeComponent %s", t->name().c_str());
}

void TaskBuildComponentTreeData::visitTypeFieldPhy(vsc::dm::ITypeFieldPhy *f) {
    DEBUG_ENTER("visitTypeFieldPhy %s", f->name().c_str());
    m_field = f;
    f->getDataType()->accept(m_this);
    DEBUG_LEAVE("visitTypeFieldPhy %s", f->name().c_str());
}

dmgr::IDebug *TaskBuildComponentTreeData::m_dbg = 0;

}
}
}
