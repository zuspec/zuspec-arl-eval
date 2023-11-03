/*
 * TaskEvalCheckRegAccess.cpp
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
#include "dmgr/impl/DebugMacros.h"
#include "vsc/dm/impl/ValRefPtr.h"
#include "vsc/dm/impl/ValRefWrapper.h"
#include "TaskEvalCheckRegAccess.h"


namespace zsp {
namespace arl {
namespace eval {

static const EvalContextFunc RegAccessFuncs[] = {
        EvalContextFunc::RegWrite,
        EvalContextFunc::RegWriteMasked,
        EvalContextFunc::RegWriteVal,
        EvalContextFunc::RegWriteValMasked,
        EvalContextFunc::RegRead,
        EvalContextFunc::RegReadVal};

bool TaskEvalCheckRegAccess::m_func_is_write[N_REG_FUNCS] = {
    true,
    true,
    true,
    true,
    false,
    false
};

bool TaskEvalCheckRegAccess::m_func_is_masked[N_REG_FUNCS] = {
    false,
    true,
    false,
    true,
    false,
    false
};

bool TaskEvalCheckRegAccess::m_func_is_struct[N_REG_FUNCS] = {
    true, // write
    true, // write_masked
    false,
    false,
    true, // read
    false
};

TaskEvalCheckRegAccess::TaskEvalCheckRegAccess(
    IEvalContext        *ctxt,
    IEvalValProvider    *vp) : m_ctxt(ctxt), m_vp(vp) {
    DEBUG_INIT("zsp::arl::eval::TaskEvalCheckRegAccess", ctxt->getDebugMgr());

    for (uint32_t i=0; i<sizeof(RegAccessFuncs)/sizeof(EvalContextFunc); i++) {
        m_functions[i] = ctxt->getFunction(RegAccessFuncs[i]);
    }
}

TaskEvalCheckRegAccess::~TaskEvalCheckRegAccess() {

}

const TaskEvalCheckRegAccess::Result &TaskEvalCheckRegAccess::check(
        vsc::dm::ITypeExpr          *func_ctxt, 
        dm::IDataTypeFunction       *func) {
    DEBUG_ENTER("check %s", func->name().c_str());

    memset(&m_res, 0, sizeof(Result));
    m_res.root = vsc::dm::ValRef();

    bool is_reg_access = false;
    for (uint32_t i=0; i<sizeof(m_functions)/sizeof(dm::IDataTypeFunction *); i++) {
        if (m_functions[i] == func) {
            is_reg_access = true;
            m_res.is_write = m_func_is_write[i];
            m_res.is_masked = m_func_is_masked[i];
            m_res.is_struct = m_func_is_struct[i];
            break;
        }
    }

    if (is_reg_access) {
        func_ctxt->accept(m_this);
    }

    DEBUG_LEAVE("check");
    return m_res;
}

void TaskEvalCheckRegAccess::visitDataTypeStruct(vsc::dm::IDataTypeStruct *t) {
    // Confirm that we have a struct access
    m_res.is_struct = true;
}

void TaskEvalCheckRegAccess::visitTypeExprFieldRef(vsc::dm::ITypeExprFieldRef *e) {
    DEBUG_ENTER("visitTypeExprFieldRef");

    m_val = m_vp->getImmVal(
        e->getRootRefKind(),
        e->getRootRefOffset(),
        e->getPath().at(0)
    );

    m_is_reg_ref = false;
    if (m_val.field()) {
        m_val.field()->accept(m_this);
    }

    // First, determine where the root is
    uint32_t i=0;
    vsc::dm::IDataTypeStruct *field_t = 0;
    if (m_is_reg_ref) {
        // Root is the ref
        vsc::dm::ValRefWrapper val_w(m_val);
        vsc::dm::IDataTypeWrapper *dt_w = m_val.field()->getDataTypeT<vsc::dm::IDataTypeWrapper>();

        m_res.root = val_w.get_val();

        field_t = dynamic_cast<vsc::dm::IDataTypeStruct *>(dt_w->getDataTypeVirt());
        i++;
    } else {
        vsc::dm::ValRefStruct val_s(m_val);
        for (; i<e->getPath().size(); i++) {
            vsc::dm::ValRef val = val_s.getFieldRef(e->getPath().at(i));

            if (val.field()) {
                DEBUG_ENTER("val.field()->accept");
                val.field()->accept(m_this);
                if (m_is_reg_ref) {
                    DEBUG("Found root @ %d", i);
                    vsc::dm::ValRefWrapper val_w(val);
                    vsc::dm::IDataTypeWrapper *dt_w = val.field()->getDataTypeT<vsc::dm::IDataTypeWrapper>();
                    field_t = dynamic_cast<vsc::dm::IDataTypeStruct *>(dt_w->getDataTypeVirt());
                    m_res.root = val_w.get_val();
                    i++;
                    break;
                }
            } else {
                DEBUG("Null field");
                break;
            }
            val_s = vsc::dm::ValRefStruct(val);
        }
    }

    if (field_t) {
        // Now, compute the office with the type model
        for (i; i<e->getPath().size(); i++) {
            vsc::dm::ITypeField *field = field_t->getField(e->getPath().at(i));
            field->accept(m_this);

            if (i+2 < e->getPath().size()) {
                field_t = field->getDataTypeT<vsc::dm::IDataTypeStruct>();
                if (!field_t) {
                    DEBUG("Field type is not struct");
                    break;
                } 
            } else {
                // Looking at the 
                DEBUG("Last field: %d", field_t->getByteSize());
            }
        }
    }

    if (m_is_reg_ref) {
        DEBUG("Accessing offset %d", m_res.offset);
    }

    DEBUG_LEAVE("visitTypeExprFieldRef");
}

void TaskEvalCheckRegAccess::visitTypeFieldReg(dm::ITypeFieldReg *f) {
    DEBUG_ENTER("visitTypeFieldReg offset=%d", f->getOffset());

    int32_t width = f->getWidth(); 
    DEBUG("Register width is %d", width);

    m_res.type = f->getDataType();

    // If the function supports packed structs, confirm that
    // the field type is a packed struct
    if (m_res.is_struct) {
        // If the type is a struct, visiting the type
        // will set the 'is_struct' flag. Otherwise,
        // we don't need to bother with type conversion
        m_res.is_struct = false;
        f->getDataType()->accept(m_this);
    }

    // uint32_t sz = f->getDataType()->getByteSize();

    if (width > 32) {
        if (m_res.is_write) {
            m_res.func = m_ctxt->getFunction(EvalContextFunc::Write64);
        } else {
            m_res.func = m_ctxt->getFunction(EvalContextFunc::Read64);
        }
    } else if (width > 16) {
        if (m_res.is_write) {
            m_res.func = m_ctxt->getFunction(EvalContextFunc::Write32);
        } else {
            m_res.func = m_ctxt->getFunction(EvalContextFunc::Read32);
        }
    } else if (width > 8) {
        if (m_res.is_write) {
            m_res.func = m_ctxt->getFunction(EvalContextFunc::Write16);
        } else {
            m_res.func = m_ctxt->getFunction(EvalContextFunc::Read16);
        }
    } else {
        if (m_res.is_write) {
            m_res.func = m_ctxt->getFunction(EvalContextFunc::Write8);
        } else {
            m_res.func = m_ctxt->getFunction(EvalContextFunc::Read8);
        }
    }

    m_res.offset += f->getOffset();

    DEBUG("AccessSzBytes: %d", f->getDataType()->getByteSize());
    DEBUG_LEAVE("visitTypeFieldReg");
}

void TaskEvalCheckRegAccess::visitTypeFieldRegGroup(dm::ITypeFieldRegGroup *f) {
    DEBUG_ENTER("visitTypeFieldRegGroup");
    if (m_is_reg_ref) {
        // Collecting offsets
        m_res.offset += f->getOffset();
    } else {
        m_is_reg_ref = true;
    }
    DEBUG_LEAVE("visitTypeFieldRegGroup");
}

void TaskEvalCheckRegAccess::visitTypeFieldRef(vsc::dm::ITypeFieldRef *f) {
    DEBUG_ENTER("visitTypeFieldRef %s", f->name().c_str());
    vsc::dm::ValRefPtr vp(m_val);
    DEBUG("Target: %p", vp.get_val());
    // Dereference
    m_val = vsc::dm::ValRef(vp.get_val(), f->getDataType(), vp.flags());
//    f->getDataType()->accept(m_this);
    DEBUG_LEAVE("visitTypeFieldRef");
}

dmgr::IDebug *TaskEvalCheckRegAccess::m_dbg = 0;

}
}
}
