/*
 * ModelEvaluatorFullElabActivity.cpp
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
#include "vsc/solvers/ICompoundSolver.h"
#include "ModelEvaluatorFullElabActivity.h"
#include "ModelEvaluatorFullElabParallel.h"
#include "ModelEvaluatorFullElabScope.h"
#include "ModelEvaluatorFullElabSequence.h"


namespace zsp {
namespace arl {
namespace eval {


ModelEvaluatorFullElabActivity::ModelEvaluatorFullElabActivity(
    dm::IContext                *ctxt,
    vsc::solvers::IRandState    *randstate,
    dm::IModelActivity          *activity) : m_ctxt(ctxt), m_randstate(randstate) {
    DEBUG_INIT("ModelEvaluatorFullElabActivity", m_ctxt->getDebugMgr());
    m_activity = activity;
    m_taken = false;

}

ModelEvaluatorFullElabActivity::~ModelEvaluatorFullElabActivity() {

}

bool ModelEvaluatorFullElabActivity::next() {
    DEBUG_ENTER("next");

    if (m_taken) {
        DEBUG_LEAVE("next - done");
        delete this;
        return false;
    }

    m_type = dm::ModelEvalNodeT::Action;
    m_action = 0;
    m_iterator = 0;

    m_activity->accept(m_this);

    m_taken = true;

    DEBUG_LEAVE("next");
    return true;
}

void ModelEvaluatorFullElabActivity::visitModelActivityParallel(dm::IModelActivityParallel *a) {
    DEBUG_ENTER("visitModelActivityParallel");
    ModelEvaluatorFullElabParallel *par = new ModelEvaluatorFullElabParallel(
        m_ctxt, 
        m_randstate->next());

    for (std::vector<dm::IModelActivity *>::const_iterator
        it=a->branches().begin();
        it!=a->branches().end(); it++) {
        par->addBranch(*it);
    }

    m_type = dm::ModelEvalNodeT::Parallel;
    m_iterator = par;

    DEBUG_LEAVE("visitModelActivityParallel");
}


void ModelEvaluatorFullElabActivity::visitModelActivitySequence(dm::IModelActivitySequence *a) {
    DEBUG_ENTER("visitModelActivitySequence");

    // Return a
    ModelEvaluatorFullElabSequence *seq = new ModelEvaluatorFullElabSequence(
        m_ctxt,
        m_randstate->clone(), // Not sure about this
        a);
    m_iterator = seq;
    m_type = dm::ModelEvalNodeT::Sequence;

    DEBUG_LEAVE("visitModelActivitySequence");
}

void ModelEvaluatorFullElabActivity::visitModelActivityTraverse(dm::IModelActivityTraverse *a) {
    DEBUG_ENTER("visitModelActivityTraverse");

    std::vector<vsc::dm::IModelConstraint *> constraints;

    if (a->getWithC()) {
        constraints.push_back(a->getWithC());
    }

    // Add in the local action constraints
    for (std::vector<vsc::dm::IModelConstraintUP>::const_iterator
        it=a->getTarget()->getConstraints().begin();
        it!=a->getTarget()->getConstraints().end(); it++) {
        constraints.push_back(it->get());
    }

    // TODO:
//    vsc::solvers::ICompoundSolverUP solver(/*m_ctxt->mkCompoundSolver()*/);
    vsc::solvers::ICompoundSolverUP solver;

    bool result = solver->solve(
        m_randstate.get(),
        {a->getTarget()},
        constraints,
        vsc::solvers::SolveFlags::Randomize
            | vsc::solvers::SolveFlags::RandomizeDeclRand
            | vsc::solvers::SolveFlags::RandomizeTopFields);

    m_type = dm::ModelEvalNodeT::Action;
    m_action = a->getTarget();

    if (a->getActivity()) {
        m_iterator = new ModelEvaluatorFullElabScope(
            m_ctxt, 
            m_randstate->next(),
            dynamic_cast<dm::IModelActivityScope *>(a->getActivity()));
    }

    DEBUG_LEAVE("visitModelActivityTraverse");
}

dmgr::IDebug *ModelEvaluatorFullElabActivity::m_dbg = 0;

}
}
}
