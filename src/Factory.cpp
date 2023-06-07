/*
 * Factory.cpp
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
#include "zsp/arl/eval/FactoryExt.h"
#include "EvalContextFullElab.h"
#include "Factory.h"
#include "ModelEvaluatorFullElab.h"
#include "ModelEvaluatorIncrElab.h"
#include "ModelEvaluatorFullElabActivity.h"
#include "TaskElaborateActivity.h"

namespace zsp {
namespace arl {
namespace eval {


Factory::Factory() : m_dmgr(0) {

}

Factory::~Factory() {

}

IModelEvaluator *Factory::mkModelEvaluator(
    ModelEvaluatorKind          kind,
    vsc::solvers::IFactory      *solvers_f,
    arl::dm::IContext           *ctxt) {
    switch (kind) {
        case ModelEvaluatorKind::FullElab:
            return new ModelEvaluatorFullElab(solvers_f, ctxt);
            break;
        case ModelEvaluatorKind::IncrElab:
            return new ModelEvaluatorIncrElab(ctxt);
            break;
        default:
            fprintf(stdout, "Error: unhandled evaluator\n");
    }
    return 0;
}

IEvalContext *Factory::mkEvalContextFullElab(
        vsc::solvers::IFactory          *solvers_f,
        arl::dm::IContext               *ctxt,
        const vsc::solvers::IRandState  *randstate,
        dm::IModelFieldComponent        *root_comp,
        dm::IDataTypeAction             *root_action,
        IEvalBackend                    *backend) {
    vsc::solvers::IRandState *randstate_l = randstate->clone();
    ElabActivity *exec_activity = TaskElaborateActivity(
        solvers_f, 
        ctxt).elaborate(
            randstate_l,
            root_comp,
            root_action);
    return new EvalContextFullElab(
        m_dmgr,
        exec_activity);
}

IFactory *Factory::inst() {
    if (!m_inst.get()) {
        m_inst = FactoryUP(new Factory());
    }
    return m_inst.get();
};

FactoryUP Factory::m_inst;


}
}
}

zsp::arl::eval::IFactory *zsp_arl_eval_getFactory() {
    return zsp::arl::eval::Factory::inst();
}

