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


ModelAddrSpaceTransparent::ModelAddrSpaceTransparent(IEvalContextInt *ctxt) :
    ModelAddrSpaceContiguous(ctxt) {
    DEBUG_INIT("zsp::arl::eval::ModelAddrSpaceTransparent", ctxt->getDebugMgr());

}

ModelAddrSpaceTransparent::~ModelAddrSpaceTransparent() {

}



dmgr::IDebug *ModelAddrSpaceTransparent::m_dbg = 0;

}
}
}
