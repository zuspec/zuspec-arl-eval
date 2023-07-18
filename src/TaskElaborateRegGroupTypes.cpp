/*
 * TaskElaborateRegGroupTypes.cpp
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
#include "TaskElaborateRegGroupTypes.h"


namespace zsp {
namespace arl {
namespace eval {


TaskElaborateRegGroupTypes::TaskElaborateRegGroupTypes(IEvalContext *ctxt) : 
    m_ctxt(ctxt) {
    DEBUG_INIT("zsp::arl::eval::TaskElaborateRegGroupTypes", ctxt->getDebugMgr());
}

TaskElaborateRegGroupTypes::~TaskElaborateRegGroupTypes() {

}

void TaskElaborateRegGroupTypes::elaborate(
        dm::IModelFieldComponentRoot            *pss_top) {
    DEBUG_ENTER("elaborate");
    m_in_regmodel = 0;
    m_reg_group_s.clear();
    pss_top->accept(m_this);
    DEBUG_LEAVE("elaborate");
}

void TaskElaborateRegGroupTypes::visitModelFieldComponent(dm::IModelFieldComponent *f) {
    DEBUG_ENTER("visitModelFieldComponent");
    for (std::vector<vsc::dm::IModelFieldUP>::const_iterator
        it=f->getFields().begin();
        it!=f->getFields().end(); it++) {
        (*it)->accept(m_this);
    }
    DEBUG_LEAVE("visitModelFieldComponent");
}

void TaskElaborateRegGroupTypes::visitModelFieldRegGroup(dm::IModelFieldRegGroup *f) {
    DEBUG_ENTER("visitModelFieldRegGroup");
    m_in_regmodel++;
    f->getDataType()->accept(m_this);
    m_in_regmodel--;
    DEBUG_LEAVE("visitModelFieldRegGroup");
}

void TaskElaborateRegGroupTypes::visitTypeFieldReg(dm::ITypeFieldReg *f) {
    DEBUG_ENTER("visitTypeFieldReg");

    DEBUG_LEAVE("visitTypeFieldReg");
}

void TaskElaborateRegGroupTypes::visitTypeFieldRegGroup(dm::ITypeFieldRegGroup *f) {
    DEBUG_ENTER("visitTypeFieldRegGroup");
    if (m_in_regmodel && m_reg_group_s.insert(f->getDataTypeT<dm::IDataTypeComponent>()).second) {
        DEBUG("Add register-group field");
//        m_reg_group_l->push_back(f->getDataTypeT<dm::IDataTypeComponent>());

        VisitorBase::visitTypeFieldRegGroup(f);
    }
    DEBUG_LEAVE("visitTypeFieldRegGroup");
}

dmgr::IDebug *TaskElaborateRegGroupTypes::m_dbg = 0;

}
}
}
