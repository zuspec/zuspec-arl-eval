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
#include "vsc/dm/IDataTypeWrapper.h"
#include "zsp/arl/eval/IEvalContextInt.h"
#include "TaskElaborateRegisterOffsets.h"
#include "EvalTypeFunction.h"


namespace zsp {
namespace arl {
namespace eval {


TaskElaborateRegisterOffsets::TaskElaborateRegisterOffsets(
    IEvalContext    *ctxt,
    IEvalThread     *thread) : m_ctxt(ctxt), m_thread(thread) {
    DEBUG_INIT("zsp::arl::eval::TaskElaborateRegisterOffsets", ctxt->getDebugMgr());
}

TaskElaborateRegisterOffsets::~TaskElaborateRegisterOffsets() {

}

void TaskElaborateRegisterOffsets::elaborate(dm::IDataTypeComponent *root_comp) {
    DEBUG_ENTER("elaborate");
    root_comp->accept(m_this);
    DEBUG_LEAVE("elaborate");
}

void TaskElaborateRegisterOffsets::visitDataTypeComponent(dm::IDataTypeComponent *i) {
    DEBUG_ENTER("visitDataTypeComponent");
    if (m_processed_comp_s.find(i) == m_processed_comp_s.end()) {
        m_processed_comp_s.insert(i);

        for (std::vector<vsc::dm::ITypeFieldUP>::const_iterator
            it=i->getFields().begin();
            it!=i->getFields().end(); it++) {
            (*it)->accept(m_this);
        }
    }
    DEBUG_LEAVE("visitDataTypeComponent");
}

void TaskElaborateRegisterOffsets::visitTypeField(vsc::dm::ITypeField *f) {
    DEBUG_ENTER("visitTypeField %s", f->name().c_str());
    f->getDataType()->accept(m_this);
    DEBUG_LEAVE("visitTypeField");
}

void TaskElaborateRegisterOffsets::visitTypeFieldReg(dm::ITypeFieldReg *f) {
    DEBUG_ENTER("visitTypeFieldRegField %s", f->name().c_str());
    IEvalContextInt *ctxt_i = dynamic_cast<IEvalContextInt *>(m_ctxt);

    OffsetFuncInfo offset_f = m_offset_func_s.back();
    std::vector<vsc::dm::ValRef> params;
    params.push_back(vsc::dm::ValRef()); // illegal
    params.push_back(m_ctxt->ctxt()->mkValRefStr(f->name()));

    m_thread->clrFlags(EvalFlags::Complete);
    EvalTypeFunction(
        m_ctxt, 
        m_thread, 
        -1, 
        offset_f.instance_off_f,
        params, true).eval();

    vsc::dm::ValRefInt result_i = m_thread->getResult();

    // TODO: call offset function
    // TODO: set field offset
    DEBUG("Set register offset to %d", result_i.get_val_u());
    f->setAddrOffset(result_i.get_val_u());

    DEBUG_LEAVE("visitTypeFieldRegField");
}

void TaskElaborateRegisterOffsets::visitTypeFieldRegGroup(dm::ITypeFieldRegGroup *f) {
    DEBUG_ENTER("visitTypeFieldRegGroup %s", f->name().c_str());
    vsc::dm::IDataTypeWrapper *wrapper_t = f->getDataTypeT<vsc::dm::IDataTypeWrapper>();
    dm::IDataTypeComponent *group_t = dynamic_cast<dm::IDataTypeComponent *>(wrapper_t->getDataTypeVirt());

    if (m_processed_comp_s.find(group_t) == m_processed_comp_s.end()) {
        DEBUG("Processing register-group type %s", group_t->name().c_str());
        dm::IDataTypeFunction *instance_off = 0;
        dm::IDataTypeFunction *instance_array_off = 0;

        std::string instance_off_n = group_t->name() + "::get_offset_of_instance";
        std::string instance_array_off_n = group_t->name() + "::get_offset_of_instance_array";

        for (std::vector<dm::IDataTypeFunction *>::const_iterator
            it=m_ctxt->ctxt()->getDataTypeFunctions().begin();
            it!=m_ctxt->ctxt()->getDataTypeFunctions().end(); it++) {
            DEBUG("Function: %s (%s %s)", (*it)->name().c_str(), instance_off_n.c_str(), instance_array_off_n.c_str());
            if ((*it)->name() == instance_off_n) {
                instance_off = (*it);
            } else if ((*it)->name() == instance_array_off_n) {
                instance_array_off = (*it);
            }

            if (instance_off && instance_array_off) {
                break;
            }
        }

        if (!instance_off || !instance_array_off) {
            ERROR("failed to find offset-calculation functions (instance_off=%p instance_array_off=%p)");
            DEBUG_LEAVE("visitTypeFiedRegGroup");
            return;
        } else {
            DEBUG("Found offset-calculation functions");
        }

        // Save the offset functions for later...
        m_offset_func_s.push_back({
            .instance_off_f = instance_off,
            .instance_array_off_f = instance_array_off
        });

        // First, find offset functions
        m_offset_s.push_back(0);
        for (std::vector<vsc::dm::ITypeFieldUP>::const_iterator
            it=group_t->getFields().begin();
            it!=group_t->getFields().end(); it++) {
            (*it)->accept(m_this);
        }
        m_offset_s.pop_back();
        m_offset_func_s.pop_back();
    } else {
        DEBUG("Register-group type %s has already been processed", group_t->name().c_str());
    }
    DEBUG_LEAVE("visitTypeFieldRegGroup");
}

dmgr::IDebug *TaskElaborateRegisterOffsets::m_dbg = 0;

}
}
}
