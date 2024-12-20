/*
 * ValOpsDataTypeAddrHandle.cpp
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
#include "ValOpsDataTypeAddrHandle.h"


namespace zsp {
namespace arl {
namespace eval {


ValOpsDataTypeAddrHandle::ValOpsDataTypeAddrHandle(IEvalContextInt *ctxt) : m_ctxt(ctxt) {
    DEBUG_INIT("zsp::arl::eval::ValOpsDataTypeAddrHandle", ctxt->getDebugMgr());
}

ValOpsDataTypeAddrHandle::~ValOpsDataTypeAddrHandle() {

}

void ValOpsDataTypeAddrHandle::initVal(vsc::dm::ValRef &v) {
    DEBUG_ENTER("initVal");
    DEBUG("vp: 0x%llx", v.vp());
    DEBUG_LEAVE("initVal");
}

void ValOpsDataTypeAddrHandle::finiVal(vsc::dm::ValRef &v) {
    DEBUG_ENTER("finiVal");
    DEBUG_LEAVE("finiVal");
}

vsc::dm::ValRef ValOpsDataTypeAddrHandle::copyVal(const vsc::dm::ValRef &src) {
    DEBUG_ENTER("copyVal");

    DEBUG_LEAVE("copyVal");
}

vsc::dm::IValIterator *ValOpsDataTypeAddrHandle::mkValIterator(const vsc::dm::ValRef &src) {
    DEBUG_ENTER("mkValIterator");
    DEBUG("TODO: mkValIterator");
    DEBUG_LEAVE("mkValIterator");
    return 0;
}

vsc::dm::IValMutIterator *ValOpsDataTypeAddrHandle::mkValMutIterator(const vsc::dm::ValRef &src) {
    DEBUG_ENTER("mkValMutIterator");
    DEBUG("TODO: mkValMutIterator");
    DEBUG_LEAVE("mkValMutIterator");
    return 0;
}

dmgr::IDebug *ValOpsDataTypeAddrHandle::m_dbg = 0;

}
}
}
