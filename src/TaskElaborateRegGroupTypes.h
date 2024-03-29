/**
 * TaskElaborateRegGroupTypes.h
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
#include <vector>
#include <set>
#include "dmgr/IDebugMgr.h"
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "zsp/arl/eval/IEvalContext.h"

namespace zsp {
namespace arl {
namespace eval {



class TaskElaborateRegGroupTypes : public dm::VisitorBase {
public:
    TaskElaborateRegGroupTypes(IEvalContext *ctxt);

    virtual ~TaskElaborateRegGroupTypes();

    void elaborate(
        dm::IModelFieldComponentRoot            *pss_top);

	virtual void visitModelFieldComponent(dm::IModelFieldComponent *f) override;

	virtual void visitModelFieldRegGroup(dm::IModelFieldRegGroup *f) override;

	virtual void visitTypeFieldReg(dm::ITypeFieldReg *f) override;

	virtual void visitTypeFieldRegGroup(dm::ITypeFieldRegGroup *f) override;

private:
    static dmgr::IDebug                     *m_dbg;
    IEvalContext                            *m_ctxt;
    uint32_t                                m_in_regmodel;
    std::set<dm::IDataTypeComponent *>      m_reg_group_s;
};

}
}
}


