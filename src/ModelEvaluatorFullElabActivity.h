/**
 * ModelEvaluatorFullElabActivity.h
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
#include "zsp/arl/dm/IModelEvalIterator.h"
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "vsc/solvers/IRandState.h"

namespace zsp {
namespace arl {
namespace eval {



class ModelEvaluatorFullElabActivity : 
    public virtual dm::IModelEvalIterator,
    public virtual dm::VisitorBase {
public:
    ModelEvaluatorFullElabActivity(
        dm::IContext                    *ctxt,
        vsc::solvers::IRandState             *randstate,
        dm::IModelActivity              *activity);

    virtual ~ModelEvaluatorFullElabActivity();

	virtual bool next() override;

	virtual bool valid() override { return !m_taken; }

	virtual dm::ModelEvalNodeT type() const override {
        return m_type;
    }

	virtual dm::IModelFieldAction *action() override {
        return m_action;
    }

	virtual dm::IModelEvalIterator *iterator() override {
        return m_iterator;
    }

    virtual void visitModelActivityParallel(dm::IModelActivityParallel *a) override;

	virtual void visitModelActivitySequence(dm::IModelActivitySequence *a) override;

	virtual void visitModelActivityTraverse(dm::IModelActivityTraverse *a) override;

private:
    static dmgr::IDebug              *m_dbg;
    dm::IContext                     *m_ctxt;
    vsc::solvers::IRandStateUP       m_randstate;
    dm::IModelActivity               *m_activity;
    bool                             m_taken;

    dm::ModelEvalNodeT               m_type;
    dm::IModelFieldAction            *m_action;
    dm::IModelEvalIterator           *m_iterator;

};

}
}
}

