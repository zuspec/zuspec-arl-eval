/**
 * TaskElaborateActivityPopluateOutRefs.h
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
#include <vector>
#include "zsp/arl/dm/IContext.h"
#include "zsp/arl/dm/impl/VisitorBase.h"

namespace zsp {
namespace arl {
namespace eval {

class TaskElaborateActivityPopluateOutRefs : public dm::VisitorBase {
public:
    TaskElaborateActivityPopluateOutRefs(
        vsc::dm::IModelBuildContext *ctxt);

    virtual ~TaskElaborateActivityPopluateOutRefs();

    void populate(
        std::vector<vsc::dm::IModelFieldUP>     &objs,
        dm::IModelActivity                      *root);

	virtual void visitModelActivityTraverse(dm::IModelActivityTraverse *a) override;

	virtual void visitModelFieldRef(vsc::dm::IModelFieldRef *f) override;

	virtual void visitModelFieldRefType(vsc::dm::IModelFieldTypeRef *f) override;

	virtual void visitTypeFieldClaim(dm::ITypeFieldClaim *f) override;

	virtual void visitTypeFieldInOut(dm::ITypeFieldInOut *f) override;

private:
    vsc::dm::IModelBuildContext                 *m_ctxt;
    vsc::dm::IModelFieldRef                     *m_active_ref;
    vsc::dm::IModelField                        *m_obj;
    std::vector<vsc::dm::IModelFieldUP>         *m_objs;
};

}
}
}
