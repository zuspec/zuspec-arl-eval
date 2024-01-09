/*
 * ModelAddrSpaceTransparent.cpp
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
#include "vsc/dm/impl/ValRefInt.h"
#include "vsc/dm/impl/ValRefStruct.h"
#include "ModelAddrSpaceTransparent.h"


namespace zsp {
namespace arl {
namespace eval {


ModelAddrSpaceTransparent::ModelAddrSpaceTransparent(IEvalContextInt *ctxt) {
    DEBUG_INIT("zsp::arl::eval::ModelAddrSpaceTransparent", ctxt->getDebugMgr());

}

ModelAddrSpaceTransparent::~ModelAddrSpaceTransparent() {

}

void ModelAddrSpaceTransparent::addNonallocatableRegion(
        IEvalThread                     *thread,
        const vsc::dm::ValRefStruct     &region) {
    DEBUG_ENTER("addNonallocatableRegion fields=%d", region.getNumFields());
    vsc::dm::IDataTypeStruct *dt_s = dynamic_cast<vsc::dm::IDataTypeStruct *>(region.type());
    for (uint32_t i=0; i<dt_s->getFields().size(); i++) {
        DEBUG("Field[%d]: %s %d", i, dt_s->getField(i)->name().c_str(), dt_s->getField(i)->getOffset());
    }
    vsc::dm::ValRefInt size_i(region.getFieldRef(0));
    vsc::dm::ValRefStruct trait_s(region.getFieldRef(1));
    vsc::dm::ValRefInt addr_i(region.getFieldRef(2));

    DEBUG("addNonallocatableRegion: 0x%08llx @ 0x%08llx",
        size_i.get_val_u(), addr_i.get_val_u());

    DEBUG_LEAVE("addNonallocatableRegion");
}

dmgr::IDebug *ModelAddrSpaceTransparent::m_dbg = 0;

}
}
}
