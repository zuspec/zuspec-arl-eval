/*
 * EvalTypeExprRegOffset.cpp
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
#include "zsp/arl/dm/impl/TaskIsRefGroupRef.h"
#include "zsp/arl/eval/IEvalContextInt.h"
#include "EvalTypeExprRegOffset.h"
#include "TaskGetSubField.h"
#include "TaskGetRegBaseAddr.h"


namespace zsp {
namespace arl {
namespace eval {


EvalTypeExprRegOffset::EvalTypeExprRegOffset(
        IEvalContext        *ctxt,
        IEvalThread         *thread,
        int32_t             vp_id,
        vsc::dm::ITypeExpr  *expr) : EvalTypeExpr(ctxt, thread, vp_id, expr) {

}

EvalTypeExprRegOffset::~EvalTypeExprRegOffset() {

}

EvalTypeExprRegOffset::EvalTypeExprRegOffset(EvalTypeExprRegOffset *o) : 
    EvalTypeExpr(o) {

}

int32_t EvalTypeExprRegOffset::eval() {
    int32_t ret = EvalTypeExpr::eval();

    if (!ret) {

    }

    return ret;
}

IEval *EvalTypeExprRegOffset::clone() {
    return new EvalTypeExprRegOffset(this);
}

void EvalTypeExprRegOffset::visitTypeExprFieldRef(vsc::dm::ITypeExprFieldRef *e) {
    DEBUG_ENTER("visitTypeExprFieldRef (Offset)");
    bool have_base = false;

    switch (e->getRootRefKind()) {
        case vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope: {
            DEBUG("Bottom-up scope");
            // TODO: Should we get a mutable reference instead?
            vsc::dm::ValRef val(getImmVal(
                vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope,
                e->getRootRefOffset(),
                e->getPath().at(0)));

            if (e->getPath().size() > 1) {
                DEBUG("TODO: stack-local refernece deeper than 2");
            }

            // Are there any cases in which we need to clone?
            setResult(val);

            // TODO:
            fprintf(stdout, "TODO: Set cloned result\n");
//              setResult(dynamic_cast<IEvalResult *>(val->clone()));
        } break;

        case vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope: {
            DEBUG("Top-down scope");
            vsc::dm::ValRef root = ctxtT<IEvalContextInt>()->getValProvider(
                m_vp_id)->getMutVal(
                    e->getRootRefKind(), 
                    e->getRootRefOffset(), e->getPath().at(0));
            
            if (dm::TaskIsRefGroupRef().check(root.field())) {
                DEBUG("Found register base");
                have_base = true;
                TaskGetRegBaseAddr(m_ctxt->getDebugMgr()).get(root);
            }
            
            DEBUG("  ref type=%p", root.type());
            for (uint32_t i=0; i<e->getPath().size(); i++) {
                root = TaskGetSubField(m_ctxt->getDebugMgr()).getMutVal(
                    root,
                    e->getPath().at(i));
                if (!have_base && dm::TaskIsRefGroupRef().check(root.field())) {
                    DEBUG("Found register base @ %d", i);
                    have_base = true;
                    TaskGetRegBaseAddr(m_ctxt->getDebugMgr()).get(root);
                }
            }
            setResult(root);
        } break;

        case vsc::dm::ITypeExprFieldRef::RootRefKind::RootExpr: {
            DEBUG("Root expr");
        } break;
    }

    if (getResult().field()) {
        DEBUG("Is a field");
    }

    DEBUG_LEAVE("visitTypeExprFieldRef (Offset)");
}

}
}
}
