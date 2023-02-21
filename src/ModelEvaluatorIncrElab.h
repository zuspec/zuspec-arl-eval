/**
 * ModelEvaluatorIncrElab.h
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
#include "zsp/arl/eval/IModelEvaluator.h"
#include "zsp/arl/dm/impl/VisitorBase.h"

namespace zsp {
namespace arl {
namespace eval {



class ModelEvaluatorIncrElab : 
    public virtual IModelEvaluator, 
    public virtual dm::VisitorBase {
public:
    ModelEvaluatorIncrElab(dm::IContext *ctxt);

    virtual ~ModelEvaluatorIncrElab();

    virtual dm::IModelEvalIterator *eval(
        const vsc::solvers::IRandState      *randstate,
        dm::IModelFieldComponent            *root_comp,
        dm::IDataTypeAction                 *root_action) override;

	virtual dm::IModelEvalIterator *next();

	virtual void visitModelActivityTraverse(dm::IModelActivityTraverse *a) override;

private:
    static dmgr::IDebug                 *m_dbg;
    dm::IContext                        *m_ctxt;
	vsc::solvers::IRandStateUP		    m_randstate;
	dm::IModelFieldActionUP			    m_action;
	dm::IModelActivityTraverseUP	    m_dummy_activity;
	uint32_t						    m_activity_idx;
	std::vector<dm::IModelActivity *>	m_activities;
	dm::IModelEvalIterator				*m_next;

};

}
}
}


