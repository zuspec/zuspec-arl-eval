/*
 * ValOpsDataTypeAddrSpaceC.cpp
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
#include "ValOpsDataTypeAddrSpaceC.h"


namespace zsp {
namespace arl {
namespace eval {


ValOpsDataTypeAddrSpaceC::ValOpsDataTypeAddrSpaceC(IEvalContext *ctxt) : m_ctxt(ctxt) {
    DEBUG_INIT("zsp::arl::eval::ValOpsDataTypeAddrSpaceC", ctxt->getDebugMgr());
}

ValOpsDataTypeAddrSpaceC::~ValOpsDataTypeAddrSpaceC() {

}

void ValOpsDataTypeAddrSpaceC::initVal(vsc::dm::ValRef &v) {
    DEBUG_ENTER("initVal");
    DEBUG_LEAVE("initVal");
}

void ValOpsDataTypeAddrSpaceC::finiVal(vsc::dm::ValRef &v) {
    DEBUG_ENTER("finiVal");
    DEBUG_LEAVE("finiVal");
}

vsc::dm::ValRef ValOpsDataTypeAddrSpaceC::copyVal(const vsc::dm::ValRef &src) {
    DEBUG_ENTER("copyVal");
    DEBUG_LEAVE("copyVal");
}

dmgr::IDebug *ValOpsDataTypeAddrSpaceC::m_dbg = 0;

}
}
}
