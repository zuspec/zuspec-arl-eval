/**
 * TaskInitComponentTree.h
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
#pragma once
#include "dmgr/IDebugMgr.h"
#include "zsp/arl/eval/IEvalContext.h"

namespace zsp {
namespace arl {
namespace eval {



class TaskInitComponentTree {
public:
    TaskInitComponentTree(
        IEvalContext        *ctxt,
        IEvalThread         *thread);

    virtual ~TaskInitComponentTree();

    void init(
        dm::IDataTypeComponent      *root_comp_t,
        const vsc::dm::ValRef       &root_comp_v);

private:
    static dmgr::IDebug                 *m_dbg;
    IEvalContext                        *m_ctxt;
    IEvalThread                         *m_thread;

};

}
}
}


