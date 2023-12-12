/*
 * CoreLibImpl.cpp
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
#include "vsc/dm/IDataTypeWrapper.h"
#include "vsc/dm/impl/TaskComputeTypePackedSize.h"
#include "vsc/dm/impl/ValRefStr.h"
#include "zsp/arl/eval/IEvalContextInt.h"
#include "BuiltinFuncInfo.h"
#include "CoreLibImpl.h"
#include "StringFormatter.h"
#include "EvalTypeFunction.h"


namespace zsp {
namespace arl {
namespace eval {


CoreLibImpl::CoreLibImpl(IEvalContext *ctxt) : m_ctxt(ctxt) {
    DEBUG_INIT("zsp::arl::eval::CoreLibImpl", ctxt->getDebugMgr());

}

CoreLibImpl::~CoreLibImpl() {

}

IBuiltinFuncInfo *CoreLibImpl::findBuiltin(
        const std::string           &name) {
    IBuiltinFuncInfo *ret = 0;

    if (name.substr(0,9) == "std_pkg::") {
        std::string leaf = name.substr(9);
        if (leaf == "print") {
            ret = new BuiltinFuncInfo(std::bind(
                &CoreLibImpl::Print, 
                this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        }
    } else if (name.substr(0,14) == "addr_reg_pkg::") {
        std::string leaf = name.substr(14);
        if (leaf == "addr_value") {
            ret = new BuiltinFuncInfo(std::bind(
                &CoreLibImpl::AddrValue, 
                this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        } else if (leaf == "make_handle_from_claim") {
            ret = new BuiltinFuncInfo(std::bind(
                &CoreLibImpl::MakeHandleFromClaim, 
                this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        } else if (leaf == "make_handle_from_handle") {
            ret = new BuiltinFuncInfo(std::bind(
                &CoreLibImpl::MakeHandleFromHandle, 
                this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        } else if (leaf.substr(0,5) == "reg_c") {
            int32_t leaf_idx = leaf.rfind("::");
            std::string fname = leaf.substr(leaf_idx+2);
            DEBUG("TODO: handle reg_c functions %s", fname.c_str());
            if (fname == "read") {
               ret = new BuiltinFuncInfo(std::bind(
                    &CoreLibImpl::RegRead,
                    this,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                    BuiltinFuncFlags::RegFunc);
            } else if (fname == "write") {
               ret = new BuiltinFuncInfo(std::bind(
                    &CoreLibImpl::RegWrite,
                    this,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                    BuiltinFuncFlags::RegFunc);
            } else if (fname == "read_val") {
               ret = new BuiltinFuncInfo(std::bind(
                    &CoreLibImpl::RegReadVal,
                    this,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                    BuiltinFuncFlags::RegFunc);
            } else if (fname == "write_val") {
               ret = new BuiltinFuncInfo(std::bind(
                    &CoreLibImpl::RegWriteVal,
                    this,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                    BuiltinFuncFlags::RegFunc);
            }
        } else if (leaf.substr(0, 11) == "reg_group_c") {
            int32_t leaf_idx = leaf.rfind("::");
            std::string fname = leaf.substr(leaf_idx+2);

            DEBUG("reg_group_c: fname=\"%s\"", fname.c_str());
            if (fname == "set_handle") {
                DEBUG("Returning RegGroupSetHandle");
                ret = new BuiltinFuncInfo(std::bind(
                    &CoreLibImpl::RegGroupSetHandle,
                    this,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
            }
        }
    } else if (name.substr(0,14) == "executor_pkg::") {

    }

    return ret;
}

void CoreLibImpl::Print(
        IEvalThread                         *thread,
            dm::IDataTypeFunction           *func_t,
        const std::vector<vsc::dm::ValRef>  &params) {
    std::string fmt = vsc::dm::ValRefStr(params.at(0)).val_s();
    DEBUG_ENTER("Print %s", fmt.c_str());
    const std::string msg = StringFormatter(m_ctxt->getDebugMgr()).format(
        fmt,
        params,
        1);
    m_ctxt->getBackend()->emitMessage(msg);
    thread->setFlags(EvalFlags::Complete);
    DEBUG_LEAVE("Print");
}

void CoreLibImpl::AddrValue(
        IEvalThread                         *thread,
        dm::IDataTypeFunction               *func_t,
        const std::vector<vsc::dm::ValRef>  &params) {
    DEBUG_ENTER("AddrValue");
    DEBUG_LEAVE("AddrValue");
}

void CoreLibImpl::MakeHandleFromClaim(
        IEvalThread                         *thread,
        dm::IDataTypeFunction               *func_t,
        const std::vector<vsc::dm::ValRef>  &params) {
    DEBUG_ENTER("MakeHandleFromClaim");
    DEBUG_LEAVE("MakeHandleFromClaim");
}

void CoreLibImpl::MakeHandleFromHandle(
        IEvalThread                         *thread,
        dm::IDataTypeFunction               *func_t,
        const std::vector<vsc::dm::ValRef>  &params) {
    DEBUG_ENTER("MakeHandleFromHandle");
    DEBUG_LEAVE("MakeHandleFromHandle");
}

void CoreLibImpl::RegRead(
        IEvalThread                         *thread,
        dm::IDataTypeFunction               *func_t,
        const std::vector<vsc::dm::ValRef>  &params) {
    DEBUG_ENTER("RegRead");
    thread->setFlags(EvalFlags::Complete);
    DEBUG_LEAVE("RegRead");
}

void CoreLibImpl::RegWrite(
        IEvalThread                         *thread,
        dm::IDataTypeFunction               *func_t,
        const std::vector<vsc::dm::ValRef>  &params) {
    DEBUG_ENTER("RegWrite");
    // TODO: identify proper-size mem function
    thread->setFlags(EvalFlags::Complete);
    DEBUG_LEAVE("RegWrite");
}

void CoreLibImpl::RegReadVal(
        IEvalThread                         *thread,
        dm::IDataTypeFunction               *func_t,
        const std::vector<vsc::dm::ValRef>  &params) {
    DEBUG_ENTER("RegReadVal");
    IEvalContextInt *ctxt_i = dynamic_cast<IEvalContextInt *>(m_ctxt);
    uint32_t sz = vsc::dm::TaskComputeTypePackedSize().compute(
        func_t->getReturnType());
    dm::IDataTypeFunction *func = 0;

    DEBUG("width=%d", sz);
    for (uint32_t i=0; i<params.size(); i++) {
        DEBUG("Param[%d] valid=%d", i, params.at(i).valid());
    }

    if (sz > 32) {
        func = ctxt_i->getFunction(EvalContextFunc::Read64);
    } else if (sz > 16) {
        func = ctxt_i->getFunction(EvalContextFunc::Read32);
    } else if (sz > 8) {
        func = ctxt_i->getFunction(EvalContextFunc::Read16);
    } else {
        func = ctxt_i->getFunction(EvalContextFunc::Read8);
    }
    
    ctxt_i->callFuncReq(thread, func, params);
    DEBUG_LEAVE("RegReadVal");
}

void CoreLibImpl::RegWriteVal(
        IEvalThread                         *thread,
        dm::IDataTypeFunction               *func_t,
        const std::vector<vsc::dm::ValRef>  &params) {
    DEBUG_ENTER("RegWriteVal");
    IEvalContextInt *ctxt_i = dynamic_cast<IEvalContextInt *>(m_ctxt);
    uint32_t sz = vsc::dm::TaskComputeTypePackedSize().compute(
        func_t->getParameters().at(0)->getDataType());
    dm::IDataTypeFunction *func = 0;

    for (uint32_t i=0; i<params.size(); i++) {
        DEBUG("Param[%d] valid=%d", i, params.at(i).valid());
    }

    if (sz > 32) {
        func = ctxt_i->getFunction(EvalContextFunc::Write64);
    } else if (sz > 16) {
        func = ctxt_i->getFunction(EvalContextFunc::Write32);
    } else if (sz > 8) {
        func = ctxt_i->getFunction(EvalContextFunc::Write16);
    } else {
        func = ctxt_i->getFunction(EvalContextFunc::Write8);
    }
    
    DEBUG("width=%d", sz);
    ctxt_i->callFuncReq(thread, func, params);
    DEBUG_LEAVE("RegWriteVal");
}

void CoreLibImpl::RegGroupSetHandle(
            IEvalThread                         *thread,
            dm::IDataTypeFunction               *func_t,
            const std::vector<vsc::dm::ValRef>  &params) {
    DEBUG_ENTER("RegGroupSetHandle");
    vsc::dm::IDataTypeWrapper *dt = dynamic_cast<vsc::dm::IDataTypeWrapper *>(params.at(0).type());
    vsc::dm::ValRefPtr hndl_p(params.at(0));

    // RHS will be an integer (for now), and later an address handle
    vsc::dm::ValRefInt val_i(params.at(1));
    DEBUG("val_i: 0x%llx", val_i.get_val_u());
    hndl_p.set_val(val_i.get_val_u());
    DEBUG("hndl_p: 0x%0llx", hndl_p.get_val());

    // Context is of Wrapper type
    // Must set to specified value
    thread->setFlags(EvalFlags::Complete);
    DEBUG_LEAVE("RegGroupSetHandle");
}

dmgr::IDebug *CoreLibImpl::m_dbg = 0;

}
}
}
