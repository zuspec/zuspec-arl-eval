/*
 * TaskEvalComponentExecInit.cpp
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
#include "vsc/dm/impl/ValRefStruct.h"
#include "TaskEvalComponentExecInit.h"
#include "EvalTypeExecList.h"
#include "EvalValProviderStructThread.h"


namespace zsp {
namespace arl {
namespace eval {


TaskEvalComponentExecInit::TaskEvalComponentExecInit(
    IEvalContext    *ctxt,
    IEvalThread     *thread) : EvalBase(ctxt, thread), m_vp(thread) {
    DEBUG_INIT("zsp::arl::eval::TaskEvalComponentExecInit", ctxt->getDebugMgr());
}

TaskEvalComponentExecInit::TaskEvalComponentExecInit(
    const TaskEvalComponentExecInit *o) :
    EvalBase(o), m_val_s(o->m_val_s), m_vp(o->m_vp) {

}

TaskEvalComponentExecInit::~TaskEvalComponentExecInit() {

}

void TaskEvalComponentExecInit::eval(
        dm::IDataTypeComponent      *comp_t,
        const vsc::dm::ValRef       &val) {
    DEBUG_ENTER("eval");
    m_thread->pushEval(this);
    m_val_s.push_back(val);
    comp_t->accept(m_this);
    m_val_s.pop_back();
    m_thread->popEval(this);
    DEBUG_LEAVE("eval");
}

IEval *TaskEvalComponentExecInit::clone() {
    return new TaskEvalComponentExecInit(*this);
}

vsc::dm::ValRef TaskEvalComponentExecInit::getImmVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) {
    return m_vp.getImmVal(root_kind, root_offset, val_offset);
}

vsc::dm::ValRef TaskEvalComponentExecInit::getMutVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) {
    return m_vp.getMutVal(root_kind, root_offset, val_offset);
}

int32_t TaskEvalComponentExecInit::eval() {
    DEBUG_FATAL("TaskEvalComponentExecInit not intended to use eval interface");
}

void TaskEvalComponentExecInit::visitDataTypeComponent(
    dm::IDataTypeComponent *t) {
    m_vp.setScope(m_val_s.back());

    vsc::dm::ValRefStruct val_s(m_val_s.back());
    DEBUG_ENTER("visitDataTypeComponent");

    // Execute init-down
    const std::vector<dm::ITypeExecUP> &execs = t->getExecs(dm::ExecKindT::InitDown);
    if (execs.size() > 0) {
        DEBUG("Evaluating %d init-down execs", execs.size());
        EvalTypeExecList(m_ctxt, m_thread, getIdx(), execs).eval();
    } else {
        DEBUG("No init-down execs to evaluate");
    }

    // Process component-type fields
    for (uint32_t i=0; i<t->getFields().size(); i++) {
        m_val_s.push_back(val_s.getFieldRef(i));
        t->getField(i)->accept(m_this);
        m_val_s.pop_back();
    }

    // Execute init-up
    const std::vector<dm::ITypeExecUP> &execs_u = t->getExecs(dm::ExecKindT::InitUp);
    if (execs_u.size() > 0) {
        DEBUG("Evaluating %d init-up execs", execs_u.size());
        EvalTypeExecList(m_ctxt, m_thread, getIdx(), execs).eval();
    } else {
        DEBUG("No init-up execs to evaluate");
    }

    DEBUG_LEAVE("visitDataTypeComponent");
}

dmgr::IDebug *TaskEvalComponentExecInit::m_dbg = 0;

}
}
}
