/**
 * TaskBuildComponentTreeData.h
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
#include "dmgr/IDebugMgr.h"
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "zsp/arl/eval/IComponentTreeData.h"
#include "ComponentTreeData.h"

namespace zsp {
namespace arl {
namespace eval {



class TaskBuildComponentTreeData :
    public virtual arl::dm::VisitorBase {
public:
    TaskBuildComponentTreeData(dmgr::IDebugMgr *dmgr);

    virtual ~TaskBuildComponentTreeData();

    IComponentTreeData *build(arl::dm::IDataTypeComponent *root);

    virtual void visitDataTypeComponent(arl::dm::IDataTypeComponent *t) override;

    virtual void visitTypeFieldPhy(vsc::dm::ITypeFieldPhy *f) override;

private:
    static dmgr::IDebug                                 *m_dbg;
    ComponentTreeDataUP                                 m_data;
    vsc::dm::ITypeFieldPhy                              *m_field;
    std::vector<std::pair<int32_t,int32_t>>             m_comp_id_s;
    std::vector<ComponentTreeData::ComponentInfo *>     m_data_s;

};

}
}
}


