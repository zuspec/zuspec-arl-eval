/*
 * ModelAddrSpaceRegion.cpp
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
#include "ModelAddrSpaceRegion.h"
#include "ModelAddrSpace.h"


namespace zsp {
namespace arl {
namespace eval {


ModelAddrSpaceRegion::ModelAddrSpaceRegion(
    ModelAddrSpace          *aspace,
    uint64_t                addr,
    uint32_t                size,
    vsc::dm::ValRef         trait) : m_aspace(aspace), 
        m_addr(addr), m_size(size), m_allocatable(true),
        m_trait(trait) {

}

ModelAddrSpaceRegion::ModelAddrSpaceRegion(
    ModelAddrSpace            *aspace,
    uint64_t                  addr,
    uint32_t                  size) : m_aspace(aspace),
        m_addr(addr), m_size(size), m_allocatable(false) {

}

ModelAddrSpaceRegion::~ModelAddrSpaceRegion() {

}

void ModelAddrSpaceRegion::releaseHandle(ModelAddrHandle *h) {
    if (m_allocatable) {

    }
    // TODO: pool somewhere?
    delete h;
}

}
}
}
