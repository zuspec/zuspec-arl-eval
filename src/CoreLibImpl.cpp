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
#include "vsc/dm/impl/ValRefStr.h"
#include "BuiltinFuncInfo.h"
#include "CoreLibImpl.h"
#include "StringFormatter.h"


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
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
            } else if (fname == "write") {
               ret = new BuiltinFuncInfo(std::bind(
                    &CoreLibImpl::RegWrite,
                    this,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
            } else if (fname == "read_val") {
               ret = new BuiltinFuncInfo(std::bind(
                    &CoreLibImpl::RegReadVal,
                    this,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
            } else if (fname == "write_val") {
               ret = new BuiltinFuncInfo(std::bind(
                    &CoreLibImpl::RegWriteVal,
                    this,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
            }
        } else if (leaf.substr(0, 11) == "reg_group_c") {
            DEBUG("TODO: handle reg_group_c functions");
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
    DEBUG_ENTER("Print");
    const std::string &msg = StringFormatter(m_ctxt->getDebugMgr()).format(
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

    DEBUG_LEAVE("RegRead");
}

void CoreLibImpl::RegWrite(
        IEvalThread                         *thread,
        dm::IDataTypeFunction               *func_t,
        const std::vector<vsc::dm::ValRef>  &params) {
    DEBUG_ENTER("RegWrite");

    DEBUG_LEAVE("RegWrite");
}

void CoreLibImpl::RegReadVal(
        IEvalThread                         *thread,
        dm::IDataTypeFunction               *func_t,
        const std::vector<vsc::dm::ValRef>  &params) {
    DEBUG_ENTER("RegReadVal");

    DEBUG_LEAVE("RegReadVal");
}

void CoreLibImpl::RegWriteVal(
        IEvalThread                         *thread,
        dm::IDataTypeFunction               *func_t,
        const std::vector<vsc::dm::ValRef>  &params) {
    DEBUG_ENTER("RegWriteVal");
    
    DEBUG_LEAVE("RegWriteVal");
}

void CoreLibImpl::RegGroupSetHandle(
            IEvalThread                         *thread,
            dm::IDataTypeFunction               *func_t,
            const std::vector<vsc::dm::ValRef>  &params) {
    DEBUG_ENTER("RegGroupSetHandle");
    DEBUG_LEAVE("RegGroupSetHandle");
}

dmgr::IDebug *CoreLibImpl::m_dbg = 0;

}
}
}
