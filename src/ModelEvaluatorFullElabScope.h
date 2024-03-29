/**
 * ModelEvaluatorFullElabScope.h
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
#include "zsp/arl/dm/IModelActivityScope.h"
#include "zsp/arl/dm/IModelEvalIterator.h"
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "vsc/solvers/IFactory.h"
#include "vsc/solvers/IRandState.h"

namespace zsp {
namespace arl {
namespace eval {


class ModelEvaluatorFullElabScope :
    public virtual dm::IModelEvalIterator,
    public virtual dm::VisitorBase {
public:
    ModelEvaluatorFullElabScope(
        vsc::solvers::IFactory      *solvers_f,
        dm::IContext                *ctxt,
        vsc::solvers::IRandState    *randstat,
        dm::IModelActivityScope     *scope
    );

    virtual ~ModelEvaluatorFullElabScope();

	virtual bool next() override;

	virtual bool valid() override;

	virtual dm::ModelEvalNodeT type() const override;

	virtual dm::IModelFieldAction *action() override;

	virtual dm::IModelEvalIterator *iterator() override;

    virtual void addListener(dm::IModelEvalIteratorListener *l) override { }

    virtual void remListener(dm::IModelEvalIteratorListener *l) override { }

	virtual void visitModelActivityScope(dm::IModelActivityScope *a) override;

	virtual void visitModelActivityTraverse(dm::IModelActivityTraverse *a) override;

private:
    static dmgr::IDebug              *m_dbg;
    vsc::solvers::IFactory           *m_solvers_f;
    dm::IContext                     *m_ctxt;
    vsc::solvers::IRandStateUP       m_randstate;
    dm::IModelActivityScope          *m_scope;
    int32_t                          m_idx;

    dm::ModelEvalNodeT               m_type;
    dm::IModelFieldAction            *m_action;
    dm::IModelEvalIterator           *m_iterator;

};

}
}
}

