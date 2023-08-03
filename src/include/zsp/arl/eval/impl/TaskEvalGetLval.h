/**
 * TaskEvalGetLval.h
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
#include "dmgr/impl/DebugMacros.h"
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "zsp/arl/eval/IEvalThread.h"

namespace zsp {
namespace arl {
namespace eval {



class TaskEvalGetLval : 
    public virtual dm::VisitorBase {
public:
    TaskEvalGetLval(IEvalThread *thread) : m_thread(thread), m_dbg(0) { 
        DEBUG_INIT("zsp::arl::eval::TaskEvalGetLval", thread->getDebugMgr());

    }

    virtual ~TaskEvalGetLval() { };

    vsc::dm::IModelVal *eval(vsc::dm::ITypeExpr *expr) {
        DEBUG_ENTER("eval");
        m_val = 0;
        expr->accept(m_this);
        DEBUG_LEAVE("eval %p", m_val);
        return m_val;
    }

	virtual void visitTypeExprFieldRef(vsc::dm::ITypeExprFieldRef *e) override {
        IEvalResult *var = 0;
        int32_t path_idx = 0;
        DEBUG_ENTER("visitTypeExprFieldRef");

        switch (e->getRootRefKind()) {
            case vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope: {
                IEvalStackFrame *frame = m_thread->stackFrame(e->getPath().at(path_idx++));
                var = frame->getVariable(e->getPath().at(path_idx++));
            } break;
        }

        if (!var) {
            FATAL("No root var");
        }

        vsc::dm::IModelField *field = 0;

        if (path_idx >= e->getPath().size()) {
            m_val = var;
        } else {
        while (path_idx < e->getPath().size()) {
            switch (var->getKind()) {
                case EvalResultKind::Val: {
                    FATAL("TODO: support val-kind intermediate var");
                } break;
                case EvalResultKind::Ref: {
                    if (!field) {
                        field = var->getRef()->getField(e->getPath().at(path_idx++));
                    } else {
                        field = field->getField(e->getPath().at(path_idx++));
                    }

                    DEBUG("Get field @%d: %s", path_idx-1, field->name().c_str());

                    if (path_idx >= e->getPath().size()) {
                        m_val = field->val();
                    }
                } break;
                default: FATAL("Unsupported var-result kind %d", var->getKind());
            }
        }
        }

        DEBUG_LEAVE("visitTypeExprFieldRef");
    }

private:
    dmgr::IDebug            *m_dbg;
    IEvalThread             *m_thread;
    vsc::dm::IModelVal      *m_val;

};

}
}
}


