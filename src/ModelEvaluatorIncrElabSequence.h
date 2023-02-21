/**
 * ModelEvaluatorIncrElabSequence.h
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
#include "zsp/arl/dm/IModelEvalIterator.h"
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "ModelEvaluatorThread.h"

namespace zsp {
namespace arl {
namespace eval {

class ModelEvaluatorIncrElabSequence : 
    public virtual dm::IModelEvalIterator,
    public virtual dm::VisitorBase {
public:
    ModelEvaluatorIncrElabSequence(ModelEvaluatorThread *thread);

    ModelEvaluatorIncrElabSequence(
        ModelEvaluatorThread                *thread,
        const std::vector<dm::IModelActivity *> &activities,
        bool                                owned=false);

    virtual ~ModelEvaluatorIncrElabSequence();

    void addActivity(dm::IModelActivity *activity, bool owned);

	virtual bool next() override;

    virtual bool valid() override;

	virtual dm::ModelEvalNodeT type() const override;

	virtual dm::IModelFieldAction *action() override;

	virtual dm::IModelEvalIterator *iterator() override;

    virtual void addListener(dm::IModelEvalIteratorListener *l) override { }

    virtual void remListener(dm::IModelEvalIteratorListener *l) override { }

	virtual void visitModelActivityParallel(dm::IModelActivityParallel *a) override;

	virtual void visitModelActivitySchedule(dm::IModelActivitySchedule *a) override;

	virtual void visitModelActivitySequence(dm::IModelActivitySequence *a) override;

	virtual void visitModelActivityTraverse(dm::IModelActivityTraverse *a) override;

private:
    static dmgr::IDebug                  *m_dbg;
    ModelEvaluatorThread                *m_thread;
    int32_t                             m_idx;
    std::vector<dm::IModelActivity *>       m_activities;
    std::vector<dm::IModelActivityUP>       m_activities_up;
    dm::IModelFieldAction                   *m_action;
    dm::IModelEvalIterator                  *m_next_it;
    dm::ModelEvalNodeT                      m_type;
};

}
}
}
