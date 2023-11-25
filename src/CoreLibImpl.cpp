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

IBuiltinFuncInfo::FuncT CoreLibImpl::findBuiltin(
        const std::string           &name) {
    IBuiltinFuncInfo::FuncT ret;

    if (name.substr(0,9) == "std_pkg::") {
        std::string leaf = name.substr(9);
        if (leaf == "print") {
            ret = std::bind(
                &CoreLibImpl::Print, 
                this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        }
    } else if (name.substr(0,14) == "addr_reg_pkg::") {
        std::string leaf = name.substr(14);
        if (leaf == "addr_value") {
            ret = std::bind(
                &CoreLibImpl::AddrValue, 
                this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        } else if (leaf == "make_handle_from_claim") {
            ret = std::bind(
                &CoreLibImpl::MakeHandleFromClaim, 
                this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        } else if (leaf == "make_handle_from_handle") {
            ret = std::bind(
                &CoreLibImpl::MakeHandleFromHandle, 
                this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
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
    thread->setVoidResult();
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
