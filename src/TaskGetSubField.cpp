/*
 * TaskGetSubField.cpp
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
#include "vsc/dm/impl/ValRefPtr.h"
#include "vsc/dm/impl/ValRefStruct.h"
#include "TaskGetSubField.h"


namespace zsp {
namespace arl {
namespace eval {


TaskGetSubField::TaskGetSubField(dmgr::IDebugMgr *dmgr) {
    DEBUG_INIT("zsp::arl::eval::TaskGetSubField", dmgr);
}

TaskGetSubField::~TaskGetSubField() {

}

vsc::dm::ValRef TaskGetSubField::getMutVal(
        vsc::dm::ValRef     &root,
        int32_t             offset) {
    DEBUG_ENTER("getMutVal");
    m_root = root;
    m_offset = offset;
    m_root.field()->accept(m_this);
    DEBUG_LEAVE("getMutVal");
    return m_root;
}

void TaskGetSubField::visitDataTypeArlStruct(dm::IDataTypeArlStruct *t) {
    DEBUG_ENTER("visitDataTypeArlStruct");
    DEBUG_LEAVE("visitDataTypeArlStruct");
}

void TaskGetSubField::visitDataTypeStruct(vsc::dm::IDataTypeStruct *i) {
    DEBUG_ENTER("visitDataTypeStruct");
    vsc::dm::ValRefStruct val_s(m_root);
    m_root = val_s.getFieldRef(m_offset);
    DEBUG_LEAVE("visitDataTypeStruct");
}

void TaskGetSubField::visitTypeFieldPhy(vsc::dm::ITypeFieldPhy *f) {
    DEBUG_ENTER("visitTypeFieldPhy");
    f->getDataType()->accept(m_this);
    DEBUG_LEAVE("visitTypeFieldPhy");
}

void TaskGetSubField::visitTypeFieldRef(vsc::dm::ITypeFieldRef *f) {
    DEBUG_ENTER("visitTypeFieldRef");
    vsc::dm::ValRefPtr val_p(m_root);
    m_root = val_p.deref();
    f->getDataType()->accept(m_this);
    DEBUG_LEAVE("visitTypeFieldRef");
}

dmgr::IDebug *TaskGetSubField::m_dbg = 0;

}
}
}
