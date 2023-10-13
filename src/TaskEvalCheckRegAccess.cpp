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
#include "vsc/dm/impl/ValRefPtr.h"
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
    DEBUG_ENTER("check %s", func->name().c_str());
    m_res.root = vsc::dm::ValRef();
    m_res.offset = 0;
    m_res.is_write = (func->name() == "pss::core::reg_write");
    m_res.access_sz = 0;

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

    m_is_reg_ref = false;
    if (m_val.field()) {
        m_val.field()->accept(m_this);
    }

    if (m_is_reg_ref) {
        // Root is the ref
        m_res.root = m_val;
    } else {
        vsc::dm::ValRefStruct val_s(m_val);
        for (uint32_t i=1; i<e->getPath().size(); i++) {
            vsc::dm::ValRef val = val_s.getFieldRef(e->getPath().at(i));

            if (val.field()) {
                DEBUG_ENTER("val.field()->accept");
                if (m_is_reg_ref) {
                    val.field()->accept(m_this);
                } else {
                    val.field()->accept(m_this);
                    if (m_is_reg_ref) {
                        // Capture the origin
                        m_res.root = val;
                    }
                }
                DEBUG_LEAVE("val.field()->accept");
            } else {
                DEBUG("Null field");
                break;
            }
            val_s = vsc::dm::ValRefStruct(val);
        }
    }

    if (m_is_reg_ref) {
        DEBUG("Accessing offset %d", m_res.offset);
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

void TaskEvalCheckRegAccess::visitTypeFieldReg(dm::ITypeFieldReg *f) {
    DEBUG_ENTER("visitTypeFieldReg offset=%d", f->getOffset());
    m_res.access_sz = f->getDataType()->getByteSize();
    m_res.offset += f->getOffset();
    DEBUG("AccessSzBytes: %d", f->getDataType()->getByteSize());
    DEBUG_LEAVE("visitTypeFieldReg");
}

void TaskEvalCheckRegAccess::visitTypeFieldRegGroup(dm::ITypeFieldRegGroup *f) {
    DEBUG_ENTER("visitTypeFieldRegGroup");
    if (m_is_reg_ref) {
        // Collecting offsets
        m_res.offset += f->getOffset();
    } else {
        m_is_reg_ref = true;
    }
    DEBUG_LEAVE("visitTypeFieldRegGroup");
}

void TaskEvalCheckRegAccess::visitTypeFieldRef(vsc::dm::ITypeFieldRef *f) {
    DEBUG_ENTER("visitTypeFieldRef %s", f->name().c_str());
    vsc::dm::ValRefPtr vp(m_val);
    DEBUG("Target: %p", vp.get_val());
    // Dereference
    m_val = vsc::dm::ValRef(vp.get_val(), f->getDataType(), vp.flags());
//    f->getDataType()->accept(m_this);
    DEBUG_LEAVE("visitTypeFieldRef");
}

dmgr::IDebug *TaskEvalCheckRegAccess::m_dbg = 0;

}
}
}
