/**
 * CollectingEvalListener.h
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
#include "zsp/arl/eval/impl/EvalListenerBase.h"

namespace zsp {
namespace arl {
namespace eval {



class CollectingEvalListener : public virtual EvalListenerBase {
public:
    CollectingEvalListener();

    virtual ~CollectingEvalListener();

    virtual void enterAction(
        IEvalThread             *t,
        dm::IModelFieldAction   *action) override { }

    virtual void leaveAction(
        IEvalThread             *t,
        dm::IModelFieldAction   *action) override { 
        m_actions.push_back(action);
    }
    
    const std::vector<dm::IModelFieldAction *> &getActions() const {
        return m_actions;
    }


protected:
    std::vector<dm::IModelFieldAction *>           m_actions;

};

}
}
}


