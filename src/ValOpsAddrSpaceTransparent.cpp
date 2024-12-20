/*
 * ValOpsAddrSpaceTransparent.cpp
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
#include "vsc/dm/impl/ValRefPtr.h"
#include "vsc/dm/impl/ValRefStruct.h"
#include "ModelAddrSpaceTransparent.h"
#include "ValOpsAddrSpaceTransparent.h"


namespace zsp {
namespace arl {
namespace eval {


ValOpsAddrSpaceTransparent::ValOpsAddrSpaceTransparent(IEvalContextInt *ctxt) : m_ctxt(ctxt) {
    DEBUG_INIT("zsp::arl::eval::ValOpsAddrSpaceTransparent", ctxt->getDebugMgr());

}

ValOpsAddrSpaceTransparent::~ValOpsAddrSpaceTransparent() {

}

void ValOpsAddrSpaceTransparent::initVal(vsc::dm::ValRef &v) {
    DEBUG_ENTER("initVal flags=0x%08x", v.flags());
    vsc::dm::ValRefStruct val_s(v);
    vsc::dm::ValRefPtr self_p(val_s.getFieldRef(-1));
    IModelAddressSpace *aspace = new ModelAddrSpaceTransparent(m_ctxt);
    DEBUG("val_s: 0x%08x", val_s.vp());
    DEBUG("self_p: 0x%08x", self_p.vp());
    DEBUG("aspace: 0x%p", aspace);
    self_p.set_val(reinterpret_cast<uintptr_t>(aspace));
    DEBUG("self_p (get): 0x%p", self_p.get_val());
    self_p = val_s.getFieldRef(-1);
    DEBUG("self_p (get2): 0x%p", self_p.get_val());
    DEBUG_LEAVE("initVal");
}

void ValOpsAddrSpaceTransparent::finiVal(vsc::dm::ValRef &v) {
    DEBUG_ENTER("finiVal");
    DEBUG_LEAVE("finiVal");
}

vsc::dm::ValRef ValOpsAddrSpaceTransparent::copyVal(const vsc::dm::ValRef &src) {
    DEBUG_ENTER("copyVal");

    DEBUG_LEAVE("copyVal");
    return vsc::dm::ValRef();
}

vsc::dm::IValIterator *ValOpsAddrSpaceTransparent::mkValIterator(const vsc::dm::ValRef &src) {
    DEBUG_ENTER("mkValIterator");
    DEBUG("TODO: mkValIterator");
    DEBUG_LEAVE("mkValIterator");
    return 0;
}

vsc::dm::IValMutIterator *ValOpsAddrSpaceTransparent::mkValMutIterator(const vsc::dm::ValRef &src) {
    DEBUG_ENTER("mkValMutIterator");
    DEBUG("TODO: mkValMutIterator");
    DEBUG_LEAVE("mkValMutIterator");
    return 0;
}

dmgr::IDebug *ValOpsAddrSpaceTransparent::m_dbg = 0;

}
}
}
