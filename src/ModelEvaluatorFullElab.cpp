/*
 * ModelEvaluatorFullElab.cpp
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
#include "dmgr/impl/DebugMacros.h"
#include "ModelEvaluatorFullElab.h"
#include "ModelEvaluatorFullElabActivity.h"
#include "ModelEvaluatorFullElabScope.h"
#include "TaskElaborateActivity.h"


namespace zsp {
namespace arl {
namespace eval {


ModelEvaluatorFullElab::ModelEvaluatorFullElab(
    vsc::solvers::IFactory      *solvers_f,
    dm::IContext                *ctxt) : m_solvers_f(solvers_f), m_ctxt(ctxt) {
    DEBUG_INIT("ModelEvaluatorFullElab", ctxt->getDebugMgr());

}

ModelEvaluatorFullElab::~ModelEvaluatorFullElab() {

}

dm::IModelEvalIterator *ModelEvaluatorFullElab::eval(
        const vsc::solvers::IRandState      *randstate,
        dm::IModelFieldComponent            *root_comp,
        dm::IDataTypeAction                 *root_action) {
    vsc::solvers::IRandState *randstate_l = randstate->clone();
    ElabActivity *exec_activity = TaskElaborateActivity(
        m_solvers_f, m_ctxt).elaborate(
            randstate_l,
            root_comp,
            root_action);

    return new ModelEvaluatorFullElabScope(
        m_solvers_f,
        m_ctxt, 
        randstate_l,
        exec_activity->root);
}

dmgr::IDebug *ModelEvaluatorFullElab::m_dbg = 0;

}
}
}
