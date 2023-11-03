/**
 * TaskEvalCheckRegAccess.h
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
#pragma once
#include "vsc/dm/impl/ValRefInt.h"
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "zsp/arl/eval/IEvalContext.h"
#include "zsp/arl/eval/IEvalValProvider.h"

namespace zsp {
namespace arl {
namespace eval {



class TaskEvalCheckRegAccess : public dm::VisitorBase {
public:
    struct Result {
        dm::IDataTypeFunction       *func;
        vsc::dm::ValRef             root;
        int64_t                     offset;
        vsc::dm::IDataType          *type;

        // read vs write
        // masked vs unmasked
        // struct vs integral
        bool                        is_write;
        bool                        is_masked;
        bool                        is_struct;
    };

public:
    TaskEvalCheckRegAccess(
        IEvalContext            *ctxt,
        IEvalValProvider        *vp);

    virtual ~TaskEvalCheckRegAccess();

    const Result &check(
        vsc::dm::ITypeExpr          *func_ctxt, 
        dm::IDataTypeFunction       *func);

	virtual void visitDataTypeStruct(vsc::dm::IDataTypeStruct *t) override;

	virtual void visitTypeExprFieldRef(vsc::dm::ITypeExprFieldRef *e) override;

	virtual void visitTypeFieldReg(dm::ITypeFieldReg *f) override;

	virtual void visitTypeFieldRegGroup(dm::ITypeFieldRegGroup *f) override;

	virtual void visitTypeFieldRef(vsc::dm::ITypeFieldRef *f) override;

private:
    static const uint32_t           N_REG_FUNCS =
        ((int)EvalContextFunc::RegReadVal-(int)EvalContextFunc::RegWrite)+1;
    static dmgr::IDebug             *m_dbg;
    IEvalContext                    *m_ctxt;
    IEvalValProvider                *m_vp;
    vsc::dm::ValRef                 m_val;
    bool                            m_is_reg_ref;
    dm::IDataTypeFunction           *m_functions[N_REG_FUNCS];
    static bool                     m_func_is_write[N_REG_FUNCS];
    static bool                     m_func_is_masked[N_REG_FUNCS];
    static bool                     m_func_is_struct[N_REG_FUNCS];
    Result                          m_res;

};

}
}
}


