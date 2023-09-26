/*
 * ModelEvaluatorFullElabParallel.cpp
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
#include "ModelEvaluatorFullElabParallel.h"
#include "ModelEvaluatorFullElabSequence.h"


namespace zsp {
namespace arl {
namespace eval {


ModelEvaluatorFullElabParallel::ModelEvaluatorFullElabParallel(
    dm::IContext                *ctxt,
    vsc::solvers::IRandState    *randstate) : m_ctxt(ctxt), m_randstate(randstate) {
    DEBUG_INIT("ModelEvaluatorFullElabParallel", ctxt->getDebugMgr())
    m_idx = -1;
}

ModelEvaluatorFullElabParallel::~ModelEvaluatorFullElabParallel() {

}

void ModelEvaluatorFullElabParallel::addBranch(dm::IModelActivity *branch) {
    m_branches.push_back(branch);
}

bool ModelEvaluatorFullElabParallel::next() {
    DEBUG_ENTER("next - idx=%d size=%d", (m_idx+1), m_branches.size());
    m_idx++;

    if (m_idx >= m_branches.size()) {
        delete this;
        DEBUG_LEAVE("next - false");
        return false;
    }

    m_type = dm::ModelEvalNodeT::Action;
    m_action = 0;
    m_iterator = 0;
    m_branches.at(m_idx)->accept(m_this);

    DEBUG_LEAVE("next - %d", m_type);
    return true;
}

void ModelEvaluatorFullElabParallel::visitModelActivityParallel(
    dm::IModelActivityParallel *a) {
    DEBUG_ENTER("visitModelActivityParallel");
    ModelEvaluatorFullElabParallel *par = new ModelEvaluatorFullElabParallel(
        m_ctxt, 
        m_randstate->next());

    m_type = dm::ModelEvalNodeT::Parallel;
    m_iterator = par;
    DEBUG_LEAVE("visitModelActivityParallel");
}

void ModelEvaluatorFullElabParallel::visitModelActivitySequence(
    dm::IModelActivitySequence *a) {
    DEBUG_ENTER("visitModelActivitySequence");
    ModelEvaluatorFullElabSequence *seq = new ModelEvaluatorFullElabSequence(
        m_ctxt,
        m_randstate->next(), // Not sure about this
        a);
    m_iterator = seq;
    m_type = dm::ModelEvalNodeT::Sequence;
    DEBUG_LEAVE("visitModelActivitySequence");
}

void ModelEvaluatorFullElabParallel::visitModelActivityTraverse(
    dm::IModelActivityTraverse *a) {
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
//    vsc::dm::ICompoundSolverUP solver(m_ctxt->mkCompoundSolver());
    vsc::solvers::ICompoundSolverUP solver;

    vsc::solvers::IRandStateUP state(m_randstate->next());
    /**
     * TODO:
    bool result = solver->solve(
        state.get(),
        {a->getTarget()},
        constraints,
        vsc::solvers::SolveFlags::Randomize
            | vsc::solvers::SolveFlags::RandomizeDeclRand
            | vsc::solvers::SolveFlags::RandomizeTopFields);
     */

    m_type = dm::ModelEvalNodeT::Action;
    m_action = a->getTarget();
    DEBUG_LEAVE("visitModelActivityTraverse");
}

dmgr::IDebug *ModelEvaluatorFullElabParallel::m_dbg = 0;

}
}
}
