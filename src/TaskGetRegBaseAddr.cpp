/*
 * TaskGetRegBaseAddr.cpp
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
#include "vsc/dm/impl/ValRefInt.h"
#include "vsc/dm/impl/ValRefPtr.h"
#include "TaskGetRegBaseAddr.h"


namespace zsp {
namespace arl {
namespace eval {


TaskGetRegBaseAddr::TaskGetRegBaseAddr(IEvalContext *ctxt) : m_ctxt(ctxt) {
    DEBUG_INIT("zsp::arl::eval::TaskGetRegBaseAddr", ctxt->getDebugMgr());

}

TaskGetRegBaseAddr::~TaskGetRegBaseAddr() {

}

vsc::dm::ValRef TaskGetRegBaseAddr::get(const vsc::dm::ValRef &field_val) {
    DEBUG_ENTER("get");
    m_val = field_val;
    field_val.field()->accept(m_this);
    DEBUG_LEAVE("get");
    return m_val;
}

void TaskGetRegBaseAddr::visitDataTypeWrapper(vsc::dm::IDataTypeWrapper *t) {
    DEBUG_ENTER("visitDataTypeWrapper");
    vsc::dm::ValRefPtr val_p(m_val);

    DEBUG("base=%lld type=%p", val_p.get_val(), t->getDataTypeVirt());
    m_val = m_ctxt->ctxt()->mkValRefInt(
        val_p.get_val(),
        false,
        64);

    DEBUG_LEAVE("visitDataTypeWrapper");
}

void TaskGetRegBaseAddr::visitTypeFieldPhy(vsc::dm::ITypeFieldPhy *f) {
    DEBUG_ENTER("visitTypeFieldPhy");
    DEBUG_LEAVE("visitTypeFieldPhy");
}

void TaskGetRegBaseAddr::visitTypeFieldRef(vsc::dm::ITypeFieldRef *f) {
    DEBUG_ENTER("visitTypeFieldRef %s", f->name().c_str());
    DEBUG_LEAVE("visitTypeFieldRef");
}

void TaskGetRegBaseAddr::visitTypeFieldRegGroup(dm::ITypeFieldRegGroup *f) {
    DEBUG_ENTER("visitTypeFieldRegGroup");
    f->getDataType()->accept(m_this);
    DEBUG_LEAVE("visitTypeFieldRegGroup");

}

dmgr::IDebug *TaskGetRegBaseAddr::m_dbg = 0;

}
}
}
