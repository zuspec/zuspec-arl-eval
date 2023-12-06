/**
 * TaskElaborateRegisterOffsets.h
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
#include <unordered_set>
#include <vector>
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "zsp/arl/eval/IEvalContext.h"

namespace zsp {
namespace arl {
namespace eval {

/**
 * - Collect all reg_group_c types used in the component tree
 * - Walk through each and assign offsets to reg_c and reg_group_c fields
 * 
 */

class TaskElaborateRegisterOffsets : public virtual dm::VisitorBase {
public:
    TaskElaborateRegisterOffsets(
        IEvalContext    *ctxt,
        IEvalThread     *thread);

    virtual ~TaskElaborateRegisterOffsets();

    void elaborate(dm::IDataTypeComponent *root_comp);

	virtual void visitDataTypeComponent(dm::IDataTypeComponent *i) override;

	virtual void visitTypeField(vsc::dm::ITypeField *f) override;

	virtual void visitTypeFieldReg(dm::ITypeFieldReg *f) override;

	virtual void visitTypeFieldRegGroup(dm::ITypeFieldRegGroup *f) override;

private:
    struct OffsetFuncInfo {
        dm::IDataTypeFunction   *instance_off_f;
        dm::IDataTypeFunction   *instance_array_off_f;
    };


private:
    static dmgr::IDebug                             *m_dbg;
    IEvalContext                                    *m_ctxt;
    IEvalThread                                     *m_thread;
    std::unordered_set<dm::IDataTypeComponent *>    m_processed_comp_s;
    std::vector<uint32_t>                           m_offset_s;
    std::vector<dm::ITypeFieldRegGroup *>           m_field_s;
    std::vector<OffsetFuncInfo>                     m_offset_func_s;
    uint32_t                                        m_depth;
};

}
}
}


