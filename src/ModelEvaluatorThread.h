/**
 * ModelEvaluatorThread.h
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
#include "zsp/arl/dm/IModelEvalIterator.h"
#include "vsc/solvers/IRandState.h"

namespace zsp {
namespace arl {
namespace eval {

class ModelEvaluatorThread : public virtual dm::IModelEvalIterator {
public:
    ModelEvaluatorThread(
        dm::IContext            *m_ctxt,
        vsc::solvers::IRandState     *randstate);

    virtual ~ModelEvaluatorThread();

	virtual bool next() override;

    virtual bool valid() override;

	virtual dm::ModelEvalNodeT type() const override;

	virtual dm::IModelFieldAction *action() override;

	virtual IModelEvalIterator *iterator() override;

    virtual void addListener(dm::IModelEvalIteratorListener *l) override { }

    virtual void remListener(dm::IModelEvalIteratorListener *l) override { }

    dm::IContext *ctxt() const { return m_ctxt; }

    vsc::solvers::IRandState *randstate() { return m_randstate.get(); }

    void pushIterator(IModelEvalIterator *it);

    void pushComponent(dm::IModelFieldComponent *comp);

    void popComponent();

    dm::IModelFieldComponent *component() const {
        return m_component_s.back();
    }

private:
    static dmgr::IDebug                         *m_dbg;
    dm::IContext                                *m_ctxt;
    std::vector<dm::IModelFieldComponent *>     m_component_s;
    vsc::solvers::IRandStateUP                  m_randstate;
    std::vector<IModelEvalIterator *>           m_iter_s;

};

}
}
}