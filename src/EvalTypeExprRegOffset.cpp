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

    // Expression consists of:
    // - 0-M elements that are *not* of TypeFieldReg kind
    //   - Must perform derferencing as normal, starting with the root value
    // - 1-N elements that are TypeFieldReg kind
    //   - On first TypeFieldReg reference, must compute base address
    //   - On subsequent dereferences, use pre-computed offset information 
    //     to offset the base address
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
    vsc::dm::ValRef base(getResult());

    if (!m_have_base) {
        DEBUG_ERROR("reg-path index of non-reg-path not implemented");
        /*
        if (dm::TaskIsRefGroupRef().check(m_root.field())) {
            DEBUG("TODO: array index prior to reg path");
        m_root = TaskGetSubField(m_ctxt->getDebugMgr()).getMutVal(
            m_root,
            e->getSubFieldIndex());
         */
//        m_dt = dynamic_cast<vsc::dm::IDataTypeStruct *>(
//            m_root.field()->getDataTypeT<vsc::dm::IDataTypeWrapper>()->getDataTypeVirt());
        /*
            DEBUG("Found register base");
            m_have_base = true;
            setResult(TaskGetRegBaseAddr(m_ctxt).get(m_root));
            m_dt = dynamic_cast<vsc::dm::IDataTypeStruct *>(
                m_root.field()->getDataTypeT<vsc::dm::IDataTypeWrapper>()->getDataTypeVirt());
            DEBUG("Base: 0x%08llx", vsc::dm::ValRefInt(getResult()).get_val_u());
        }
        */
    } else {
        // Calculate the offset

        // Save the address while we get the index
        vsc::dm::ValRefInt addr(getResult());

        EvalTypeExpr(
            m_ctxt, 
            m_thread,
            m_vp_id,
            e->getIndexExpr()).eval();
        vsc::dm::ValRefInt index(getResult());

        // Now, determine scale
        findFieldOffsetScale();

        DEBUG("Index: addr=0x%08llx offset=%lld scale=%lld index=%lld",
            addr.get_val_u(),
            m_offset,
            m_scale,
            index.get_val_s());
        setResult(m_ctxt->mkValRefInt(
            addr.get_val_u() + (m_scale * index.get_val_s()) + m_offset,
            false,
            64
        ));
        DEBUG("Result: %08llx", vsc::dm::ValRefInt(getResult()).get_val_u());
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

        findBase(m_root);
    } else {
        // m_root points to first element that is a TypeFieldReg
        // <result> holds the base address
        // 
        // This expression is just a sub-element reference
        // - Get field offset by visiting the field
        // Result has

        // Now, setup for next
        vsc::dm::IDataTypeStruct *dt_s = dynamic_cast<vsc::dm::IDataTypeStruct *>(m_root_dt);
        vsc::dm::ITypeField *field = dt_s->getField(e->getSubFieldIndex());
        DEBUG("Moving from field %s to %s", m_root_field->name().c_str(), field->name().c_str());
        m_root_field = field;

        findFieldOffsetScale();

        vsc::dm::ValRefInt addr(getResult());
        DEBUG("Update Addr: addr=0x%08llx offset=%0d", addr.get_val_u(), m_offset);
        setResult(m_ctxt->mkValRefInt(
            addr.get_val_u() + m_offset,
            false, 64));
    }


    DEBUG_LEAVE("visitTypeExprSubField");
}

void EvalTypeExprRegOffset::visitTypeFieldReg(arl::dm::ITypeFieldReg *f) {
    DEBUG_ENTER("visitTypeFieldReg %s", f->name().c_str());
    DEBUG("set m_offset=%lld", f->getAddrOffset());
    m_offset = f->getAddrOffset();
    DEBUG_LEAVE("visitTypeFieldReg");
}

void EvalTypeExprRegOffset::visitTypeFieldRegGroup(arl::dm::ITypeFieldRegGroup *f) {
    DEBUG_ENTER("visitTypeFieldRegGroup");
    DEBUG("set m_offset=%lld", f->getAddrOffset());
    m_offset = f->getAddrOffset();
    if (dynamic_cast<vsc::dm::IDataTypeWrapper *>(f->getDataType())) {
        DEBUG("wrapper-dt");
        m_root_dt = dynamic_cast<vsc::dm::IDataTypeWrapper *>(f->getDataType())->getDataTypeVirt();
    } else {
        m_root_dt = f->getDataType();
    }
    DEBUG_LEAVE("visitTypeFieldRegGroup");
}


void EvalTypeExprRegOffset::visitTypeFieldRegGroupArr(arl::dm::ITypeFieldRegGroupArr *f) {
    DEBUG_ENTER("visitTypeFieldRegGroupArr %s", f->name().c_str());
    DEBUG("set m_offset=%lld m_scale=%d", f->getAddrOffset(), f->getStride());
    m_offset = f->getAddrOffset();
    m_scale = f->getStride();
    m_root_field = f;

//    m_root_field = 0; // Upper-level logic needs to reset this
    if (dynamic_cast<vsc::dm::IDataTypeWrapper *>(f->getElemType())) {
        DEBUG("elem is wrapper-dt");
        m_root_dt = dynamic_cast<vsc::dm::IDataTypeWrapper *>(f->getElemType())->getDataTypeVirt();
    } else {
        m_root_dt = f->getElemType();
    }
    DEBUG_LEAVE("visitTypeFieldRegGroupArr %s", f->name().c_str());
}

bool EvalTypeExprRegOffset::findBase(const vsc::dm::ValRef &field) {
    bool ret = false;
    DEBUG_ENTER("findBase");

    m_root_field = m_root.field();
    if ((ret=dm::TaskIsRefGroupRef().check(m_root.field()))) {
        DEBUG("Found register base");
        m_have_base = true;
        vsc::dm::ValRefInt addr(TaskGetRegBaseAddr(m_ctxt).get(m_root));

        setResult(addr);

        vsc::dm::IDataTypeWrapper *dt = m_root_field->getDataTypeT<vsc::dm::IDataTypeWrapper>();
        m_root_dt = dt->getDataTypeVirt();

/*
        DEBUG("Base: 0x%08llx dt=%s", m_addr.get_val_u(), 
            dynamic_cast<vsc::dm::IDataTypeStruct *>(m_dt)?
                dynamic_cast<vsc::dm::IDataTypeStruct *>(m_dt)->name().c_str():"<unknown>");
 */
    } else {
    }
    DEBUG_LEAVE("findBase %d", ret);
    return ret;
}

void EvalTypeExprRegOffset::findFieldOffsetScale() {
    m_offset = 0;
    m_scale = 1;

    m_root_field->accept(m_this);

/*
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
            DEBUG_ERROR("Datatype is not composite");
        }
    }
 */
}

}
}
}
