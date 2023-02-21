/**
 * TaskElaborateActivityBinds.h
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
#include "zsp/arl/dm/IContext.h"
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "ActivityScheduleData.h"

namespace zsp {
namespace arl {
namespace eval {

class TaskElaborateActivityBinds : public dm::VisitorBase {
public:
    TaskElaborateActivityBinds(dm::IContext *ctxt);

    virtual ~TaskElaborateActivityBinds();

    bool elab(
        ActivityScheduleData        *sched_data,
        dm::IModelActivity          *activity);

	virtual void visitModelActivityBind(dm::IModelActivityBind *a) override;

	virtual void visitModelActivityParallel(dm::IModelActivityParallel *a) override;

	virtual void visitModelActivitySchedule(dm::IModelActivitySchedule *a) override;

	virtual void visitModelActivitySequence(dm::IModelActivitySequence *a) override;

	virtual void visitModelActivityScope(dm::IModelActivityScope *a) override;

	virtual void visitModelActivityTraverse(dm::IModelActivityTraverse *a) override;

protected:
    void processClaim(dm::IModelFieldClaim *f);

    void processRefInput(dm::IModelFieldInOut *f);

    void processRefOutput(dm::IModelFieldInOut *f);

    void processActivityParallel(dm::IModelActivityScope *a);

    void processActivitySchedule(dm::IModelActivityScope *a);

    void processActivitySequence(dm::IModelActivityScope *a);

private:
    struct ResourceClaimType {
        std::vector<vsc::dm::IRefSelector *>        lock;
        std::vector<vsc::dm::IRefSelector *>        share;
    };

    struct ResourceClaimM {
        std::unordered_map<vsc::dm::IDataType *, ResourceClaimType>     claims;
        std::vector<vsc::dm::IDataType *>                               types;
    };
    using ResourceClaimMUP=std::unique_ptr<ResourceClaimM>;
    using ResourceContentionSet=std::vector<ResourceClaimM>;

    using FlowObjType2ObjM=std::unordered_map<vsc::dm::IDataType *, std::vector<int32_t>>;

private:
    void propagateResources(
        ResourceClaimM              &dst,
        const ResourceClaimM        &src);

private:
    static dmgr::IDebug                                              *m_dbg;
    dm::IContext                                                     *m_ctxt;
    ActivityScheduleData                                             *m_sched_data;

//    std::vector<IModelActivityTraverse *>                           m_traverse_s;

    std::vector<ActivityScheduleData::ActionTraversalData *>        m_traverse_s;
    std::vector<ActivityScheduleData::ScheduleRegionData *>         m_schedule_s;
    std::vector<ResourceClaimM>                                     m_resource_s;
    std::vector<FlowObjType2ObjM>                                   m_buffer_s;



    // TODO: must capture condition(s) (if any) for current actions

    // TODO: must capture binds and any conditions on those binds


};

}
}
}

