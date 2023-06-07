/**
 * EvalExecScope.h
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
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "zsp/arl/eval/IEval.h"
#include "EvalBase.h"

namespace zsp {
namespace arl {
namespace eval {



class EvalExecScope : 
    public virtual IEval,
    public virtual EvalBase,
    public virtual dm::VisitorBase {
public:
    EvalExecScope();

    virtual ~EvalExecScope();

	virtual void visitTypeProcStmtAssign(dm::ITypeProcStmtAssign *s) override;

	virtual void visitTypeProcStmtBreak(dm::ITypeProcStmtBreak *s) override;

	virtual void visitTypeProcStmtContinue(dm::ITypeProcStmtContinue *s) override;

	virtual void visitTypeProcStmtExpr(dm::ITypeProcStmtExpr *s) override;

	virtual void visitTypeProcStmtForeach(dm::ITypeProcStmtForeach *s) override;

	virtual void visitTypeProcStmtIfElse(dm::ITypeProcStmtIfElse *s) override;

	virtual void visitTypeProcStmtMatch(dm::ITypeProcStmtMatch *s) override;

	virtual void visitTypeProcStmtRepeat(dm::ITypeProcStmtRepeat *s) override;

	virtual void visitTypeProcStmtRepeatWhile(dm::ITypeProcStmtRepeatWhile *s) override;

	virtual void visitTypeProcStmtReturn(dm::ITypeProcStmtReturn *s) override;

	virtual void visitTypeProcStmtScope(dm::ITypeProcStmtScope *s) override;

	virtual void visitTypeProcStmtVarDecl(dm::ITypeProcStmtVarDecl *s) override;

	virtual void visitTypeProcStmtWhile(dm::ITypeProcStmtWhile *s) override;

};

}
}
}


