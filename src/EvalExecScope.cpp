/*
 * EvalExecScope.cpp
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
#include "EvalExecScope.h"


namespace zsp {
namespace arl {
namespace eval {


EvalExecScope::EvalExecScope() {

}

EvalExecScope::~EvalExecScope() {

}

void EvalExecScope::visitTypeProcStmtAssign(dm::ITypeProcStmtAssign *s) { }

void EvalExecScope::visitTypeProcStmtBreak(dm::ITypeProcStmtBreak *s) { }

void EvalExecScope::visitTypeProcStmtContinue(dm::ITypeProcStmtContinue *s) { }

void EvalExecScope::visitTypeProcStmtExpr(dm::ITypeProcStmtExpr *s) { }

void EvalExecScope::visitTypeProcStmtForeach(dm::ITypeProcStmtForeach *s) { }

void EvalExecScope::visitTypeProcStmtIfElse(dm::ITypeProcStmtIfElse *s) { }

void EvalExecScope::visitTypeProcStmtMatch(dm::ITypeProcStmtMatch *s) { }

void EvalExecScope::visitTypeProcStmtRepeat(dm::ITypeProcStmtRepeat *s) { }

void EvalExecScope::visitTypeProcStmtRepeatWhile(dm::ITypeProcStmtRepeatWhile *s) { }

void EvalExecScope::visitTypeProcStmtReturn(dm::ITypeProcStmtReturn *s) { }

void EvalExecScope::visitTypeProcStmtScope(dm::ITypeProcStmtScope *s) { }

void EvalExecScope::visitTypeProcStmtVarDecl(dm::ITypeProcStmtVarDecl *s) { }

void EvalExecScope::visitTypeProcStmtWhile(dm::ITypeProcStmtWhile *s) { }

}
}
}
