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
#include "vsc/impl/DebugMacros.h"
#include "ModelEvaluatorFullElabParallel.h"
#include "ModelEvaluatorFullElabSequence.h"


namespace arl {


ModelEvaluatorFullElabSequence::ModelEvaluatorFullElabSequence(
    IContext                    *ctxt,
    vsc::IRandState             *randstate,
    IModelActivitySequence      *seq) : m_ctxt(ctxt), m_randstate(randstate), m_seq(seq) {
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

bool ModelEvaluatorFullElabSequence::pop() {
    return false;
}

ModelEvalNodeT ModelEvaluatorFullElabSequence::type() const {
    return m_next_type;
}

IModelFieldAction *ModelEvaluatorFullElabSequence::action() {
    return m_action;
}

IModelEvalIterator *ModelEvaluatorFullElabSequence::iterator() {
    return m_iterator;
}

void ModelEvaluatorFullElabSequence::visitModelActivityParallel(IModelActivityParallel *a) {
    DEBUG_ENTER("visitModelActivityParallel");
    ModelEvaluatorFullElabParallel *par = new ModelEvaluatorFullElabParallel(
        m_ctxt, 
        m_randstate->next());

    for (std::vector<IModelActivity *>::const_iterator
        it=a->branches().begin();
        it!=a->branches().end(); it++) {
        par->addBranch(*it);
    }


    m_next_type = ModelEvalNodeT::Parallel;
    m_iterator = par;
    DEBUG_LEAVE("visitModelActivityParallel");
}

void ModelEvaluatorFullElabSequence::visitModelActivitySequence(IModelActivitySequence *a) {
    DEBUG_ENTER("visitModelActivitySequence");
    // Return a
    ModelEvaluatorFullElabSequence *seq = new ModelEvaluatorFullElabSequence(
        m_ctxt,
        m_randstate->clone(), // Not sure about this
        a);
    m_iterator = seq;
    m_next_type = ModelEvalNodeT::Sequence;
    DEBUG_LEAVE("visitModelActivitySequence");
}

void ModelEvaluatorFullElabSequence::visitModelActivityTraverse(IModelActivityTraverse *a) {
    DEBUG_ENTER("visitModelActivityTraverse");
    std::vector<vsc::IModelConstraint *> constraints;

    if (a->getWithC()) {
        constraints.push_back(a->getWithC());
    }

    // Add in the local action constraints
    for (std::vector<vsc::IModelConstraintUP>::const_iterator
        it=a->getTarget()->constraints().begin();
        it!=a->getTarget()->constraints().end(); it++) {
        constraints.push_back(it->get());
    }

    vsc::ICompoundSolverUP solver(m_ctxt->mkCompoundSolver());

    bool result = solver->solve(
        m_randstate.get(),
        {a->getTarget()},
        constraints,
        vsc::SolveFlags::Randomize
            | vsc::SolveFlags::RandomizeDeclRand
            | vsc::SolveFlags::RandomizeTopFields);

    m_next_type = ModelEvalNodeT::Action;
    m_action = a->getTarget();

    DEBUG_LEAVE("visitModelActivityTraverse");
}

vsc::IDebug *ModelEvaluatorFullElabSequence::m_dbg = 0;

}
