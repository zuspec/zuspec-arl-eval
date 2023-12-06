/**
 * TaskGetSubField.h
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
#include "dmgr/IDebugMgr.h"
#include "zsp/arl/dm/impl/VisitorBase.h"

namespace zsp {
namespace arl {
namespace eval {



class TaskGetSubField : public virtual dm::VisitorBase {
public:
    TaskGetSubField(dmgr::IDebugMgr *dmgr);

    virtual ~TaskGetSubField();

    vsc::dm::ValRef getMutVal(
        vsc::dm::ValRef     &root,
        int32_t             offset);

	virtual void visitDataTypeArlStruct(dm::IDataTypeArlStruct *t) override;

    virtual void visitDataTypeStruct(vsc::dm::IDataTypeStruct *i);

	virtual void visitTypeFieldPhy(vsc::dm::ITypeFieldPhy *f) override;

	virtual void visitTypeFieldRef(vsc::dm::ITypeFieldRef *f) override;

private:
    static dmgr::IDebug         *m_dbg;
    vsc::dm::ValRef             m_root;
    int32_t                     m_offset;

};

}
}
}


