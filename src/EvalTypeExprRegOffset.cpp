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
        vsc::dm::ITypeExpr  *expr,
        const std::string   &logid) : EvalTypeExpr(ctxt, thread, vp_id, expr, logid),
        m_have_base(0) {

}

EvalTypeExprRegOffset::~EvalTypeExprRegOffset() {

}

EvalTypeExprRegOffset::EvalTypeExprRegOffset(EvalTypeExprRegOffset *o) : 
    EvalTypeExpr(o), m_have_base(o->m_have_base) {

}

int32_t EvalTypeExprRegOffset::eval() {
    DEBUG_ENTER("eval");
    if (m_initial) {
        m_thread->pushEval(this);

        setFlags(EvalFlags::Complete);
        m_have_base = false;
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

void EvalTypeExprRegOffset::visitTypeExprArrIndex(vsc::dm::ITypeExprArrIndex *e) {
    DEBUG_ENTER("visitTypeExprArrIndex");
    e->getRootExpr()->accept(m_this);

    if (!m_have_base) {
        DEBUG("TODO: array index prior to reg path");
        /*
        m_root = TaskGetSubField(m_ctxt->getDebugMgr()).getMutVal(
            m_root,
            e->getSubFieldIndex());
         */
//        m_dt = dynamic_cast<vsc::dm::IDataTypeStruct *>(
//            m_root.field()->getDataTypeT<vsc::dm::IDataTypeWrapper>()->getDataTypeVirt());
        /*
        */
        if (!m_have_base && dm::TaskIsRefGroupRef().check(m_root.field())) {
            DEBUG("Found register base");
            m_have_base = true;
            m_addr = TaskGetRegBaseAddr(m_ctxt).get(m_root);
            setResult(m_addr);
            m_dt = dynamic_cast<vsc::dm::IDataTypeStruct *>(
               m_root.field()->getDataTypeT<vsc::dm::IDataTypeWrapper>()->getDataTypeVirt());
            DEBUG("Base: 0x%08llx", m_addr.get_val_u());
        }
    } else {
        // Calculate the offset
        int32_t offset = 0;
        int32_t elem_sz = 0;

        m_addr.set_val(m_addr.get_val_u() + offset*elem_sz);
        /*
        dm::ITypeFieldReg *tf = m_dt->getFieldT<dm::ITypeFieldReg>(e->getSubFieldIndex());
        DEBUG("Offset %d: %s -- %lld", 
            e->getSubFieldIndex(), 
            tf->name().c_str(), 
            tf->getAddrOffset());
        m_addr.set_val(m_addr.get_val_u() + tf->getAddrOffset());
        m_dt = tf->getDataTypeT<vsc::dm::IDataTypeStruct>();
         */
        setResult(m_addr);
    }
    DEBUG_LEAVE("visitTypeExprArrIndex");
}

void EvalTypeExprRegOffset::visitTypeExprFieldRef(vsc::dm::ITypeExprFieldRef *e) {
    DEBUG_ENTER("visitTypeExprFieldRef (Offset)");

#ifdef UNDEFINED
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
            m_root = ctxtT<IEvalContextInt>()->getValProvider(
                m_vp_id)->getMutVal(
                    e->getRootRefKind(), 
                    e->getRootRefOffset(), e->getPath().at(0));
            
            if (dm::TaskIsRefGroupRef().check(m_root.field())) {
                DEBUG("Found register base");
                m_have_base = true;
//                TaskGetRegBaseAddr(m_ctxt->getDebugMgr()).get(root);
            }
            
            DEBUG("  ref type=%p", m_root.type());
            vsc::dm::IDataTypeStruct *dt = 0;
            /*
            vsc::dm::ValRefInt addr;
            for (uint32_t i=1; i<e->getPath().size(); i++) {
                if (!m_have_base) {
                    m_root = TaskGetSubField(m_ctxt->getDebugMgr()).getMutVal(
                        m_root,
                        e->getPath().at(i));
                    dt = dynamic_cast<vsc::dm::IDataTypeStruct *>(
                        m_root.field()->getDataTypeT<vsc::dm::IDataTypeWrapper>()->getDataTypeVirt());
                    if (!m_have_base && dm::TaskIsRefGroupRef().check(m_root.field())) {
                        DEBUG("Found register base @ %d", i);
                        m_have_base = true;
                        addr = TaskGetRegBaseAddr(m_ctxt).get(m_root);
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
             */
        } break;

        case vsc::dm::ITypeExprFieldRef::RootRefKind::RootExpr: {
            DEBUG("Root expr");
        } break;
    }

    if (getResult().field()) {
        DEBUG("Is a field");
    }
#endif /* UNDEFINED */

    DEBUG_LEAVE("visitTypeExprFieldRef (Offset)");
}

void EvalTypeExprRegOffset::visitTypeExprRefBottomUp(vsc::dm::ITypeExprRefBottomUp *e) {
    DEBUG_ENTER("visitTypeExprRefBottomUp");

    DEBUG_LEAVE("visitTypeExprRefBottomUp");
}

void EvalTypeExprRegOffset::visitTypeExprRefTopDown(vsc::dm::ITypeExprRefTopDown *e) {
    DEBUG_ENTER("visitTypeExprRefTopDown");
        m_root = ctxtT<IEvalContextInt>()->getValProvider(
            m_vp_id)->getMutVal(
                vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
                -1,
                -1);
    DEBUG_LEAVE("visitTypeExprRefTopDown");
}

void EvalTypeExprRegOffset::visitTypeExprSubField(vsc::dm::ITypeExprSubField *e) {
    DEBUG_ENTER("visitTypeExprSubField");
    e->getRootExpr()->accept(m_this);

    if (!m_have_base) {
        m_root = TaskGetSubField(m_ctxt->getDebugMgr()).getMutVal(
            m_root,
            e->getSubFieldIndex());
//        m_dt = dynamic_cast<vsc::dm::IDataTypeStruct *>(
//            m_root.field()->getDataTypeT<vsc::dm::IDataTypeWrapper>()->getDataTypeVirt());
        /*
        */
        if (!m_have_base && dm::TaskIsRefGroupRef().check(m_root.field())) {
            DEBUG("Found register base");
            m_have_base = true;
            m_addr = TaskGetRegBaseAddr(m_ctxt).get(m_root);
            setResult(m_addr);
            m_dt = m_root.field()->getDataTypeT<vsc::dm::IDataTypeWrapper>()->getDataTypeVirt();
            DEBUG("Base: 0x%08llx dt=%s", m_addr.get_val_u(), 
                dynamic_cast<vsc::dm::IDataTypeStruct *>(m_dt)?
                    dynamic_cast<vsc::dm::IDataTypeStruct *>(m_dt)->name().c_str():"<unknown>");
        }
    } else {
        if (dynamic_cast<vsc::dm::IDataTypeStruct *>(m_dt)) {
            vsc::dm::IDataTypeStruct *dt_s = dynamic_cast<vsc::dm::IDataTypeStruct *>(m_dt);
            vsc::dm::ITypeField *tf = dt_s->getField(e->getSubFieldIndex());
            DEBUG("Offset %d: %s", e->getSubFieldIndex(), tf->name().c_str());

            tf->accept(m_this);

            // DEBUG("  dt: %s -> %s", 
            //     m_dt->name().c_str(),
            //     tf->getDataTypeT<vsc::dm::IDataTypeStruct>()->name().c_str());
    //        m_dt = tf->getDataTypeT<vsc::dm::IDataTypeStruct>();
            setResult(m_addr);
        } else {
            ERROR("Datatype is not composite");
        }
    }
    DEBUG_LEAVE("visitTypeExprSubField");
}

void EvalTypeExprRegOffset::visitTypeFieldReg(arl::dm::ITypeFieldReg *f) {
    DEBUG_ENTER("visitTypeFieldReg");
    m_addr.set_val(m_addr.get_val_u() + f->getAddrOffset());
    m_dt = f->getDataType();
    DEBUG_LEAVE("visitTypeFieldReg");
}

void EvalTypeExprRegOffset::visitTypeFieldRegGroup(arl::dm::ITypeFieldRegGroup *f) {
    DEBUG_ENTER("visitTypeFieldRegGroup");
    m_addr.set_val(m_addr.get_val_u() + f->getAddrOffset());
    m_dt = f->getDataType();
    DEBUG_LEAVE("visitTypeFieldRegGroup");
}


void EvalTypeExprRegOffset::visitTypeFieldRegGroupArr(arl::dm::ITypeFieldRegGroupArr *f) {
    DEBUG_ENTER("visitTypeFieldRegGroupArr %s", f->name().c_str());
    m_dt = f->getElemType();
    DEBUG_LEAVE("visitTypeFieldRegGroupArr %s", f->name().c_str());
}

}
}
}
