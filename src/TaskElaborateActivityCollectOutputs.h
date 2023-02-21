/**
 * TaskElaborateActivityCollectOutputs.h
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


class TaskElaborateActivityCollectOutputs : public dm::VisitorBase {
public:
    TaskElaborateActivityCollectOutputs(
        dm::IContext                *ctxt,
        ActivityScheduleData     *sched_data
    );

    virtual ~TaskElaborateActivityCollectOutputs();

    void collect(dm::IModelFieldAction *action);

	virtual void visitModelFieldInOut(dm::IModelFieldInOut *f) override;

private:
    dm::IContext                *m_ctxt;
    ActivityScheduleData        *m_sched_data;

};

}
}
}

