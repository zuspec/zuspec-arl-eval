/**
 * Factory.h
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
#include <memory>
#include "dmgr/IDebugMgr.h"
#include "zsp/arl/eval/IFactory.h"

namespace zsp {
namespace arl {
namespace eval {


class Factory;
using FactoryUP=std::unique_ptr<Factory>;
class Factory : public virtual IFactory {
public:
    Factory();

    virtual ~Factory();

    virtual void init(dmgr::IDebugMgr *dmgr) override {
        m_dmgr = dmgr;
    }

    virtual IModelEvaluator *mkModelEvaluator(
        ModelEvaluatorKind          kind,
        vsc::solvers::IFactory      *solvers_f,
        arl::dm::IContext           *ctxt) override;

    virtual IEvalContext *mkEvalContextFullElab(
        vsc::solvers::IFactory          *solvers_f,
        arl::dm::IContext               *ctxt,
        const vsc::solvers::IRandState  *randstate,
        dm::IModelFieldComponent        *root_comp,
        dm::IDataTypeAction             *root_action,
        IEvalBackend                    *backend) override;

    static IFactory *inst();

private:
    static FactoryUP            m_inst;
    dmgr::IDebugMgr             *m_dmgr;

};

}
}
}

