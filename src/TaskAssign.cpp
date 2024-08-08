/*
 * TaskAssign.cpp
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
#include "vsc/dm/impl/TaskIsDataTypeBool.h"
#include "vsc/dm/impl/TaskIsDataTypeEnum.h"
#include "vsc/dm/impl/TaskIsDataTypeInt.h"
#include "vsc/dm/impl/TaskIsDataTypeStr.h"
#include "vsc/dm/impl/TaskIsDataTypeStruct.h"
#include "vsc/dm/impl/ValRefBool.h"
#include "vsc/dm/impl/ValRefInt.h"
#include "zsp/arl/dm/impl/TaskIsDataTypePyObj.h"
#include "TaskAssign.h"


namespace zsp {
namespace arl {
namespace eval {


TaskAssign::TaskAssign(IEvalContext *ctxt) : m_ctxt(ctxt) {
    DEBUG_INIT("zsp::arl::eval::TaskAssign", ctxt->getDebugMgr());
}

TaskAssign::~TaskAssign() {

}

bool TaskAssign::assign(
        vsc::dm::ValRef                 &lval,
        arl::dm::TypeProcStmtAssignOp   op,
        const vsc::dm::ValRef           &rhs) {
    using namespace vsc::dm;
    using namespace arl::dm;
    DEBUG_ENTER("assign");
    bool ret = true;
    IDataTypeEnum *lval_is_enum;
    IDataTypeInt *lval_is_int;
    IDataTypeStruct *lval_is_struct = TaskIsDataTypeStruct().check(lval.type());
    IDataTypeStruct *rval_is_struct = TaskIsDataTypeStruct().check(rhs.type());

    if (TaskIsDataTypeBool().check(lval.type())) {

    } else if ((lval_is_int=TaskIsDataTypeInt().check(lval.type()))) {
        ret = assign_int(lval, lval_is_int, rhs);
    } else if ((lval_is_enum=TaskIsDataTypeEnum().check(lval.type()))) {
    } else if ((lval_is_struct=TaskIsDataTypeStruct().check(lval.type()))) {
        assign_struct(lval, lval_is_struct, rhs);
    } else if (TaskIsDataTypePyObj().check(lval.type())) {
        assign_pyobj(lval, rhs);
    } else {
        DEBUG_ERROR("Unknown type for lval");
        ret = false;
    }

    DEBUG_LEAVE("assign %d", ret);
    return ret;
}

bool TaskAssign::assign_int(
    vsc::dm::ValRef                 &lval,
    vsc::dm::IDataTypeInt           *lval_t,
    const vsc::dm::ValRef           &rhs) {
    using namespace vsc::dm;
    using namespace arl::dm;
    DEBUG_ENTER("assign_int");
    ValRefInt lval_i(lval);
    IDataTypeInt *rval_is_int;
    IDataTypeEnum *rval_is_enum;
    IDataTypeEnum *rval_is_bool;
    bool ret = true;

    if ((rval_is_int=TaskIsDataTypeInt().check(rhs.type()))) {
        ValRefInt rhs_i(rhs);
        DEBUG("rhs is int");
        if (lval_i.is_signed()) {
            lval_i.set_val(rhs_i.get_val_s());
        } else {
            lval_i.set_val(rhs_i.get_val_u());
        }
    } else if ((rval_is_enum=TaskIsDataTypeEnum().check(rhs.type()))) {
        DEBUG("rhs is enum");
    } else if (TaskIsDataTypeBool().check(rhs.type())) {
        ValRefBool val_b(rhs);
        lval_i.set_val(val_b.get_val());
    } else if (TaskIsDataTypePyObj().check(rhs.type())) {
        arl::dm::ValRefPyObj rhs_py(rhs);
        pyapi::IPyEval *py_eval = m_ctxt->getPyEval();
        DEBUG("py_eval: %p", py_eval);
        if (py_eval) {
            PyObject *ptype, *pvalue, *ptraceback;
            DEBUG("vp: 0x%08llx", rhs.vp());
            int64_t val = py_eval->PyLong_AsLongLong(reinterpret_cast<PyObject  *>(rhs.vp()));
            py_eval->PyErr_Fetch(&ptype, &pvalue, &ptraceback);
            if (ptype || pvalue || ptraceback) {
                DEBUG_ERROR("Exception occurred");
            }
            lval_i.set_val(val);
            DEBUG("Python value: %lld", val);
        } else {
            DEBUG_ERROR("TODO: implement py-to-int conversion");
        }
    } else {
        DEBUG_ERROR("Unknown rhs for integer assign");
        ret = false;
    }

    DEBUG_LEAVE("assign_int %d", ret);
    return ret;
}

bool TaskAssign::assign_pyobj(
        vsc::dm::ValRef                 &lval,
        const vsc::dm::ValRef           &rhs) {
    using namespace vsc::dm;
    using namespace arl::dm;
    DEBUG_ENTER("assign_pyobj");
    ValRefPyObj lval_py(lval);
    IDataTypeInt *rval_is_int;
    IDataTypeEnum *rval_is_enum;
    IDataTypeEnum *rval_is_bool;
    bool ret = true;

    if ((rval_is_int=TaskIsDataTypeInt().check(rhs.type()))) {
        ValRefInt rhs_i(rhs);
        DEBUG("rhs is int");
        // if (lval_i.is_signed()) {
        //     lval_i.set_val(rhs_i.get_val_s());
        // } else {
        //     lval_i.set_val(rhs_i.get_val_u());
        // }
    } else if ((rval_is_enum=TaskIsDataTypeEnum().check(rhs.type()))) {
        DEBUG("rhs is enum");
    } else if (TaskIsDataTypeBool().check(rhs.type())) {
        ValRefBool val_b(rhs);
//        lval_i.set_val(val_b.get_val());
    } else if (TaskIsDataTypePyObj().check(rhs.type())) {
        DEBUG("rhs is pyobj");
        arl::dm::ValRefPyObj rhs_py(rhs);
        lval_py.setObj(rhs_py.getObj());
    } else {
        DEBUG_ERROR("Unknown rhs for integer assign");
        ret = false;
    }

    DEBUG_LEAVE("assign_pyobj %d", ret);
    return ret;
}

bool TaskAssign::assign_struct(
    vsc::dm::ValRef                     &lval,
    vsc::dm::IDataTypeStruct            *lval_t,
    const vsc::dm::ValRef               &rhs) {
    bool ret = true;
    DEBUG_ENTER("assign_struct (%s)", lval_t->name().c_str());
    if (dynamic_cast<arl::dm::IDataTypeAddrHandle *>(lval_t)) {
        ret = assign_addr_handle(lval, rhs);
    } else {
        DEBUG("TODO: implement struct assign");
    }

    DEBUG_LEAVE("assign_struct");
    return ret;
}

bool TaskAssign::assign_addr_handle(
    vsc::dm::ValRef                     &lval,
    const vsc::dm::ValRef               &rhs) {
    bool ret = true;
    DEBUG_ENTER("assign_addr_handle lval::flags=0x%08x", lval.flags());
    // addr_handle is a struct with a single field
    vsc::dm::ValRefStruct lval_s(lval);
    vsc::dm::ValRefPtr lval_p(lval_s.getFieldRef(-1));
    DEBUG("lval_p.flags=0x%08x", lval_p.flags());
    vsc::dm::ValRefPtr rhs_p(rhs);
    lval_p.set_val(rhs_p.get_val());
    DEBUG("addr_handle: 0x%08llx (0x%08llx)", rhs_p.get_val(), lval_p.get_val());
    DEBUG_LEAVE("assign_addr_handle");
    return ret;
}

dmgr::IDebug *TaskAssign::m_dbg = 0;

}
}
}
