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
#include "zsp/arl/dm/ITypeFieldReg.h"
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
    DEBUG_ENTER("eval");
    if (m_initial) {
        m_thread->pushEval(this);

        setFlags(EvalFlags::Complete);
    }

    m_expr->accept(m_this);

//    setResult(m_ctxt->ctxt()->mkValRefInt(20, false, 32));

    int32_t ret = !hasFlags(EvalFlags::Complete);

    if (m_initial) {
        m_initial = false;
        if (!ret) {
            m_thread->popEval(this);
        } else {
            m_thread->suspendEval(this);
        }
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
//                TaskGetRegBaseAddr(m_ctxt->getDebugMgr()).get(root);
            }
            
            DEBUG("  ref type=%p", root.type());
            vsc::dm::IDataTypeStruct *dt = 0;
            vsc::dm::ValRefInt addr;
            for (uint32_t i=1; i<e->getPath().size(); i++) {
                if (!have_base) {
                    root = TaskGetSubField(m_ctxt->getDebugMgr()).getMutVal(
                        root,
                        e->getPath().at(i));
                    dt = dynamic_cast<vsc::dm::IDataTypeStruct *>(
                        root.field()->getDataTypeT<vsc::dm::IDataTypeWrapper>()->getDataTypeVirt());
                    if (!have_base && dm::TaskIsRefGroupRef().check(root.field())) {
                        DEBUG("Found register base @ %d", i);
                        have_base = true;
                        addr = TaskGetRegBaseAddr(m_ctxt).get(root);
                        DEBUG("Base: 0x%08llx", addr.get_val_u());
                    }
                } else {
                    dm::ITypeFieldReg *tf = dt->getFieldT<dm::ITypeFieldReg>(e->getPath().at(i));
                    DEBUG("Offset %d: %s -- %lld", e->getPath().at(i), tf->name().c_str(), tf->getAddrOffset());
                    addr.set_val(addr.get_val_u() + tf->getAddrOffset());
                    dt = tf->getDataTypeT<vsc::dm::IDataTypeStruct>();
                }
            }

            DEBUG("Final Address: 0x%llx", addr.get_val_u());
            setResult(addr);
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
