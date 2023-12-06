/*
 * TaskInitComponentTree.cpp
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
#include "TaskInitComponentTree.h"
#include "TaskEvalComponentExecInit.h"
#include "TaskElaborateRegisterOffsets.h"


namespace zsp {
namespace arl {
namespace eval {


TaskInitComponentTree::TaskInitComponentTree(
    IEvalContext        *ctxt,
    IEvalThread         *thread) : m_ctxt(ctxt), m_thread(thread) {
    DEBUG_INIT("zsp::arl::eval::TaskInitComponentTree", ctxt->getDebugMgr());

}

TaskInitComponentTree::~TaskInitComponentTree() {

}

void TaskInitComponentTree::init(
        dm::IDataTypeComponent      *root_comp_t,
        const vsc::dm::ValRef       &root_comp_v) {
    DEBUG_ENTER("init");

    // Process InitUp/InitDown exec blocks
    TaskEvalComponentExecInit(m_ctxt, m_thread).eval(
        root_comp_t,
        root_comp_v);
    
    TaskElaborateRegisterOffsets(m_ctxt, m_thread).elaborate(root_comp_t);

    DEBUG_LEAVE("init");
}

dmgr::IDebug *TaskInitComponentTree::m_dbg = 0;

}
}
}
