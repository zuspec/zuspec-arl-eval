/*
 * ModelEvaluatorFullElabSequence.cpp
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


ModelEvaluatorFullElabSequence::ModelEvaluatorFullElabSequence(
    dm::IContext                    *ctxt,
    vsc::solvers::IRandState        *randstate,
    dm::IModelActivitySequence      *seq) : m_ctxt(ctxt), m_randstate(randstate), m_seq(seq) {
    DEBUG_INIT("ModelEvaluatorFullElabSequence", ctxt->getDebugMgr());

    m_idx = -1;
    m_action = 0;

}

ModelEvaluatorFullElabSequence::~ModelEvaluatorFullElabSequence() {

}

bool ModelEvaluatorFullElabSequence::next() {
    DEBUG_ENTER("next idx=%d sz=%d", (m_idx+1), m_seq->activities().size());
    m_idx++;

    if (m_idx >= m_seq->activities().size()) {
        delete this;
        DEBUG_LEAVE("next - false");
        return false;
    }

    m_seq->activities().at(m_idx)->accept(m_this);

    DEBUG_LEAVE("next - %d", m_next_type);
    return true;
}

bool ModelEvaluatorFullElabSequence::valid() {
    return (m_idx < m_seq->activities().size());
}

dm::ModelEvalNodeT ModelEvaluatorFullElabSequence::type() const {
    return m_next_type;
}

dm::IModelFieldAction *ModelEvaluatorFullElabSequence::action() {
    return m_action;
}

dm::IModelEvalIterator *ModelEvaluatorFullElabSequence::iterator() {
    return m_iterator;
}

void ModelEvaluatorFullElabSequence::visitModelActivityParallel(dm::IModelActivityParallel *a) {
    DEBUG_ENTER("visitModelActivityParallel");
    ModelEvaluatorFullElabParallel *par = new ModelEvaluatorFullElabParallel(
        m_ctxt, 
        m_randstate->next());

    for (std::vector<dm::IModelActivity *>::const_iterator
        it=a->branches().begin();
        it!=a->branches().end(); it++) {
        par->addBranch(*it);
    }


    m_next_type = dm::ModelEvalNodeT::Parallel;
    m_iterator = par;
    DEBUG_LEAVE("visitModelActivityParallel");
}

void ModelEvaluatorFullElabSequence::visitModelActivitySequence(dm::IModelActivitySequence *a) {
    DEBUG_ENTER("visitModelActivitySequence");
    // Return a
    ModelEvaluatorFullElabSequence *seq = new ModelEvaluatorFullElabSequence(
        m_ctxt,
        m_randstate->clone(), // Not sure about this
        a);
    m_iterator = seq;
    m_next_type = dm::ModelEvalNodeT::Sequence;
    DEBUG_LEAVE("visitModelActivitySequence");
}

void ModelEvaluatorFullElabSequence::visitModelActivityTraverse(dm::IModelActivityTraverse *a) {
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
//    vsc::solvers::ICompoundSolverUP solver(m_ctxt->mkCompoundSolver());
    vsc::solvers::ICompoundSolverUP solver;

    /** TODO:
    bool result = solver->solve(
        m_randstate.get(),
        {a->getTarget()},
        constraints,
        vsc::solvers::SolveFlags::Randomize
            | vsc::solvers::SolveFlags::RandomizeDeclRand
            | vsc::solvers::SolveFlags::RandomizeTopFields);
     */

    m_next_type = dm::ModelEvalNodeT::Action;
    m_action = a->getTarget();

    DEBUG_LEAVE("visitModelActivityTraverse");
}

dmgr::IDebug *ModelEvaluatorFullElabSequence::m_dbg = 0;

}
}
}
