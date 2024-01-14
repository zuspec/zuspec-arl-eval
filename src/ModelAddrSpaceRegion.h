/**
 * ModelAddrSpaceRegion.h
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
#pragma once
#include <stdint.h>
#include "vsc/dm/impl/UP.h"
#include "vsc/dm/impl/ValRef.h"

namespace zsp {
namespace arl {
namespace eval {

class ModelAddrSpace;
class ModelAddrHandle;

class ModelAddrSpaceRegion;
using ModelAddrSpaceRegionUP=vsc::dm::UP<ModelAddrSpaceRegion>;
class ModelAddrSpaceRegion {
public:
    ModelAddrSpaceRegion(
        ModelAddrSpace            *aspace,
        uint64_t                  addr,
        uint32_t                  size,
        const vsc::dm::ValRef     trait
    );

    ModelAddrSpaceRegion(
        ModelAddrSpace            *aspace,
        uint64_t                  addr,
        uint32_t                  size
    );

    virtual ~ModelAddrSpaceRegion();

    void releaseHandle(ModelAddrHandle *h);

private:
    ModelAddrSpace              *m_aspace;
    uint64_t                    m_addr;
    uint32_t                    m_size;
    bool                        m_allocatable;
    vsc::dm::ValRef             m_trait;

};

}
}
}


