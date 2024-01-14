/*
 * ModelAddrHandle.cpp
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
#include "ModelAddrHandle.h"
#include "ModelAddrSpaceRegion.h"


namespace zsp {
namespace arl {
namespace eval {


ModelAddrHandle::ModelAddrHandle(
    ModelAddrSpaceRegion        *region,
    uint64_t                    addr,
    uint32_t                    size) : 
    m_region(region), m_refcnt(1), m_addr(addr), m_size(size) {

}

ModelAddrHandle::~ModelAddrHandle() {

}

void ModelAddrHandle::incRef() {
    m_refcnt++;
}

void ModelAddrHandle::decRef() {
    if (m_refcnt) {
        m_refcnt--;
        if (!m_refcnt) {
            m_region->releaseHandle(this);
        }
    }
}

}
}
}
