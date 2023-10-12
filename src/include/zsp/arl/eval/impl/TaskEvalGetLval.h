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
#include "vsc/dm/impl/ValRefStruct.h"
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "zsp/arl/eval/IEvalThread.h"

namespace zsp {
namespace arl {
namespace eval {



class TaskEvalGetLval : 
    public virtual dm::VisitorBase {
public:
    TaskEvalGetLval(
        dmgr::IDebugMgr     *dmgr,
        IEvalValProvider    *vp) : m_vp(vp), m_dbg(0) { 
        DEBUG_INIT("zsp::arl::eval::TaskEvalGetLval", dmgr);

    }

    virtual ~TaskEvalGetLval() { };

    vsc::dm::ValRef eval(vsc::dm::ITypeExpr *expr) {
        DEBUG_ENTER("eval");
        m_val.reset();
        expr->accept(m_this);
        DEBUG_LEAVE("eval %p", m_val);
        return m_val;
    }

	virtual void visitTypeExprFieldRef(vsc::dm::ITypeExprFieldRef *e) override {
        vsc::dm::ValRef var;
        int32_t path_idx = 0;
        DEBUG_ENTER("visitTypeExprFieldRef");

        var = m_vp->getMutVal(
            e->getRootRefKind(),
            0, // TODO:
            e->getPath().at(path_idx++)
        );
        // switch (e->getRootRefKind()) {
        //     case vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope: {
        //         IEvalStackFrame *frame = m_thread->stackFrame(e->getPath().at(path_idx++));
        //         var.setWeakRef(frame->getVariable(e->getPath().at(path_idx++)));
        //     } break;
        //     case vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope: {
        //         vsc::dm::ValRefStruct scope_s(m_scope);
        //         var.setWeakRef(scope_s.getFieldRef(e->getPath().at(path_idx++)));
        //     } break;
        // }

        if (!var.valid()) {
            FATAL("No root var");
        }

        vsc::dm::IModelField *field = 0;

        if (path_idx >= e->getPath().size()) {
            m_val.setWeakRef(var);
        } else {
            while (path_idx < e->getPath().size()) {
                vsc::dm::ValRefStruct val_s(var);
                var = val_s.getFieldRef(path_idx);
            /** TODO:
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
             */
                path_idx++;
            }
            m_val.setWeakRef(var);
        }

        DEBUG_LEAVE("visitTypeExprFieldRef");
    }

private:
    dmgr::IDebug            *m_dbg;
    IEvalValProvider        *m_vp;
    vsc::dm::ValRef         m_val;

};

}
}
}


