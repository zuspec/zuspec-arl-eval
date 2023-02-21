/**
 * TaskElaborateActivitySelectReplicateSizes.h
 *
 * Copyright 2022 Matthew Ballance and Contributors
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
#include <unordered_set>
#include "zsp/arl/dm/IContext.h"
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "dmgr/impl/DebugMacros.h"

namespace zsp {
namespace arl {
namespace eval {



class TaskElaborateActivitySelectReplicateSizes : public dm::VisitorBase {
public:
    TaskElaborateActivitySelectReplicateSizes(dm::IContext *ctxt);

    virtual ~TaskElaborateActivitySelectReplicateSizes();

    bool eval(
        vsc::solvers::IRandState     *randstate,
        IModelActivity               *dm::root);

	virtual void visitModelActivityReplicate(dm::IModelActivityReplicate *a) override;

	virtual void visitModelActivityScope(dm::IModelActivityScope *a) override;

	virtual void visitModelActivityTraverse(dm::IModelActivityTraverse *a) override;

private:
    static dmgr::IDebug                               *m_dbg;
    dm::IContext                                      *m_ctxt;
    std::vector<vsc::dm::IModelField *>               m_count_fields;
    std::unordered_set<vsc::dm::IModelConstraint *>   m_constraint_s;
    std::vector<vsc::dm::IModelConstraint *>          m_constraints;
};

}
}
}

