/*
 * TaskElaborateRegisterOffsets.cpp
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
#include "TaskElaborateRegisterOffsets.h"


namespace zsp {
namespace arl {
namespace eval {


TaskElaborateRegisterOffsets::TaskElaborateRegisterOffsets(IEvalContext *ctxt) :
    m_ctxt(ctxt) {
    DEBUG_INIT("zsp::arl::eval::TaskElaborateRegisterOffsets", ctxt->getDebugMgr());
}

TaskElaborateRegisterOffsets::~TaskElaborateRegisterOffsets() {

}

void TaskElaborateRegisterOffsets::elaborate(dm::IDataTypeComponent *reg_group) {
    DEBUG_ENTER("elaborate");
    reg_group->accept(m_this);
    DEBUG_LEAVE("elaborate");
}

void TaskElaborateRegisterOffsets::visitTypeFieldReg(dm::ITypeFieldReg *f) {
    DEBUG_ENTER("visitTypeFieldRegField");

    DEBUG_LEAVE("visitTypeFieldRegField");
}

void TaskElaborateRegisterOffsets::visitTypeFieldRegGroup(dm::ITypeFieldRegGroup *f) {
    DEBUG_ENTER("visitTypeFieldRegGroup");
    if (m_field_s.size()) {
        DEBUG("TODO: obtain offset of field %s", f->name().c_str());
    } else {
        m_field_s.push_back(f);
        VisitorBase::visitTypeFieldRegGroup(f);
        m_field_s.pop_back();
    }

    DEBUG_LEAVE("visitTypeFieldRegGroup");
}

dmgr::IDebug *TaskElaborateRegisterOffsets::m_dbg = 0;

}
}
}
