/**
 * TaskAssign.h
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
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "zsp/arl/eval/IEvalContext.h"

namespace zsp {
namespace arl {
namespace eval {



class TaskAssign : public virtual arl::dm::VisitorBase {
public:
    TaskAssign(IEvalContext *ctxt);

    virtual ~TaskAssign();

    bool assign(
        vsc::dm::ValRef                 &lval,
        arl::dm::TypeProcStmtAssignOp   op,
        const vsc::dm::ValRef           &rhs);

protected:
    bool assign_int(
        vsc::dm::ValRef                 &lval,
        vsc::dm::IDataTypeInt           *lval_t,
        const vsc::dm::ValRef           &rhs);

    bool assign_pyobj(
        vsc::dm::ValRef                 &lval,
        const vsc::dm::ValRef           &rhs);

    bool assign_struct(
        vsc::dm::ValRef                 &lval,
        vsc::dm::IDataTypeStruct        *lval_t,
        const vsc::dm::ValRef           &rhs);

    bool assign_addr_handle(
        vsc::dm::ValRef                 &lval,
        const vsc::dm::ValRef           &rhs);

private:
    static dmgr::IDebug             *m_dbg;
    IEvalContext                    *m_ctxt;
};

}
}
}


