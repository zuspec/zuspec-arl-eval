/*
 * TaskBindDataTypeValOps.cpp
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
#include "TaskBindDataTypeValOps.h"


namespace zsp {
namespace arl {
namespace eval {


TaskBindDataTypeValOps::TaskBindDataTypeValOps(IEvalContextInt *ctxt) : m_ctxt(ctxt) {
    DEBUG_INIT("zsp::arl::eval::TaskBindDataTypeValOps", ctxt->getDebugMgr());
}

TaskBindDataTypeValOps::~TaskBindDataTypeValOps() {

}

void TaskBindDataTypeValOps::bind(vsc::dm::IDataType *t) {
    DEBUG_ENTER("bind"); 
    t->accept(m_this);
    DEBUG_LEAVE("bind"); 
}

void TaskBindDataTypeValOps::visitDataTypeStruct(vsc::dm::IDataTypeStruct *t) {
    DEBUG("visitDataTypeStruct %s", t->name().c_str());
}

void TaskBindDataTypeValOps::visitDataTypeAddrHandle(dm::IDataTypeAddrHandle *t) {
    DEBUG_ENTER("visitDataTypeAddrHandle %s", t->name().c_str());
    t->setValOps(m_ctxt->getValOps(CoreValOpsE::AddrHandle));
    DEBUG("valOps=%p", t->getValOps());
    DEBUG_LEAVE("visitDataTypeAddrHandle");
}

void TaskBindDataTypeValOps::visitDataTypeAddrSpaceTransparentC(dm::IDataTypeAddrSpaceTransparentC *t) {
    DEBUG_ENTER("visitDataTypeAddrSpaceTransparentC %s", t->name().c_str());
    t->setValOps(m_ctxt->getValOps(CoreValOpsE::AddrSpaceTransparent));
    DEBUG_LEAVE("visitDataTypeAddrSpaceTransparentC");
}

dmgr::IDebug *TaskBindDataTypeValOps::m_dbg = 0;

}
}
}
