/**
 * ModelEvaluatorIncrElabParallel.h
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

namespace zsp {
namespace arl {
namespace eval {

class ModelEvaluatorThread;

class ModelEvaluatorIncrElabParallel : public virtual dm::IModelEvalIterator {
public:
    ModelEvaluatorIncrElabParallel(const std::vector<ModelEvaluatorThread *> &branches);

    virtual ~ModelEvaluatorIncrElabParallel();

	virtual bool next() override;

	virtual bool valid() override;

	virtual dm::ModelEvalNodeT type() const override;

	virtual dm::IModelFieldAction *action() override;

	virtual dm::IModelEvalIterator *iterator() override;

    virtual void addListener(dm::IModelEvalIteratorListener *l) override { }

    virtual void remListener(dm::IModelEvalIteratorListener *l) override { }

private:
    static dmgr::IDebug                         *m_dbg;
    int32_t                                     m_idx;
    std::vector<ModelEvaluatorThread *>         m_branches;


};

}
}
}
