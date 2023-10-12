/*
 * TaskEvalCheckRegAccess.cpp
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
#include "dmgr/impl/DebugMacros.h"
#include "TaskEvalCheckRegAccess.h"


namespace zsp {
namespace arl {
namespace eval {


TaskEvalCheckRegAccess::TaskEvalCheckRegAccess(
    IEvalContext        *ctxt,
    IEvalValProvider    *vp) : m_ctxt(ctxt), m_vp(vp) {
    DEBUG_INIT("zsp::arl::eval::TaskEvalCheckRegAccess", ctxt->getDebugMgr());

}

TaskEvalCheckRegAccess::~TaskEvalCheckRegAccess() {

}

const TaskEvalCheckRegAccess::Result &TaskEvalCheckRegAccess::check(
        vsc::dm::ITypeExpr          *func_ctxt, 
        dm::IDataTypeFunction       *func) {
    DEBUG_ENTER("check");
    m_res.root = vsc::dm::ValRef();

    func_ctxt->accept(m_this);

    DEBUG_LEAVE("check");
    return m_res;
}

void TaskEvalCheckRegAccess::visitTypeExprFieldRef(vsc::dm::ITypeExprFieldRef *e) {
    DEBUG_ENTER("visitTypeExprFieldRef");

    m_val = m_vp->getImmVal(
        e->getRootRefKind(),
        e->getRootRefOffset(),
        e->getPath().at(0)
    );

    if (m_val.field()) {
        m_val.field()->accept(m_this);
    }
//    val.type()->accept(m_this);

    vsc::dm::IModelField *field = 0;

    // switch (e->getRootRefKind()) {
    //     case vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope:
	// 		field = m_ctxt->getTopDownScope();
    //         break;
    //     case ITypeExprFieldRef::RootRefKind::BottomUpScope:
	// 		m_field = m_ctxt->getScope();
    //         break;
    // }
	// for (std::vector<int32_t>::const_iterator
	// 	it=e->getPath().begin(); 
    //     it!=e->getPath().end(); it++) {
	// 	m_field = m_field->getField(*it);
	// }

    DEBUG_LEAVE("visitTypeExprFieldRef");
}

void TaskEvalCheckRegAccess::visitTypeFieldRegGroup(dm::ITypeFieldRegGroup *f) {
    DEBUG_ENTER("visitTypeFieldRegGroup");

    DEBUG_LEAVE("visitTypeFieldRegGroup");
}

void TaskEvalCheckRegAccess::visitTypeFieldRef(vsc::dm::ITypeFieldRef *f) {
    DEBUG_ENTER("visitTypeFieldRef");
    DEBUG_LEAVE("visitTypeFieldRef");
}

dmgr::IDebug *TaskEvalCheckRegAccess::m_dbg = 0;

}
}
}
