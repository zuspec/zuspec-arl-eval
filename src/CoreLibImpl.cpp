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
#include "CoreLibImpl.h"


namespace zsp {
namespace arl {
namespace eval {


CoreLibImpl::CoreLibImpl(IEvalContext *ctxt) : m_ctxt(ctxt) {
    DEBUG_INIT("zsp::arl::eval::CoreLibImpl", ctxt->getDebugMgr());

}

CoreLibImpl::~CoreLibImpl() {

}

void CoreLibImpl::Print(
        IEvalThread                         *thread,
        dm::IDataTypeFunction               *func_t,
        const std::vector<vsc::dm::ValRef>  &params) {
    DEBUG_ENTER("Print");
    DEBUG_LEAVE("Print");
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
