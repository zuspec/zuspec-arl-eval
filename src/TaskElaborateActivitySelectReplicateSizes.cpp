/*
 * TaskElaborateActivitySelectReplicateSizes.cpp
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
#include "vsc/dm/impl/PrettyPrinter.h"
#include "TaskElaborateActivitySelectReplicateSizes.h"


namespace zsp {
namespace arl {
namespace eval {


TaskElaborateActivitySelectReplicateSizes::TaskElaborateActivitySelectReplicateSizes(
    vsc::solvers::IFactory  *solvers_f,
    arl::dm::IContext       *ctxt) : m_solvers_f(solvers_f), m_ctxt(ctxt) {
    
    DEBUG_INIT("TaskElaborateActivitySelectReplicateSizes", 
        solvers_f->getDebugMgr());

}

TaskElaborateActivitySelectReplicateSizes::~TaskElaborateActivitySelectReplicateSizes() {

}

bool TaskElaborateActivitySelectReplicateSizes::eval(
    vsc::solvers::IRandState    *randstate,
    dm::IModelActivity          *root) {
    root->accept(m_this);

    // Now that we have the fields, solve all at once to arrive at a
    // consistent result
    // TODO:
    vsc::solvers::ICompoundSolverUP solver(m_solvers_f->mkCompoundSolver());

    DEBUG("Fields: %d ; Constraints: %d", m_count_fields.size(), m_constraints.size());

#ifdef UNDEFINED
    bool ret = solver->solve(
        randstate,
        m_count_fields,
        m_constraints,
        vsc::solvers::SolveFlags::Randomize 
        | vsc::solvers::SolveFlags::RandomizeDeclRand
        | vsc::solvers::SolveFlags::RandomizeTopFields
    );

    return ret;
#endif
}

void TaskElaborateActivitySelectReplicateSizes::visitModelActivityReplicate(
    dm::IModelActivityReplicate *a) {
    m_count_fields.push_back(a->getCountField());
    DEBUG_ENTER("visitModelActivityReplicate");
    for (std::vector<vsc::dm::IModelConstraintUP>::const_iterator
        it=a->getConstraints().begin();
        it!=a->getConstraints().end(); it++) {
        if (m_constraint_s.insert(it->get()).second) {
            DEBUG("Add constraint");
            m_constraints.push_back(it->get());
        }
    }
    DEBUG_LEAVE("visitModelActivityReplicate");
}

void TaskElaborateActivitySelectReplicateSizes::visitModelActivityScope(
    dm::IModelActivityScope *a) {
    DEBUG_ENTER("visitModelActivityScope");
    for (std::vector<vsc::dm::IModelConstraintUP>::const_iterator
        it=a->getConstraints().begin();
        it!=a->getConstraints().end(); it++) {
        if (m_constraint_s.insert(it->get()).second) {
            DEBUG("Add constraint");
            m_constraints.push_back(it->get());
        }
    }

    for (std::vector<dm::IModelActivity *>::const_iterator
        it=a->activities().begin();
        it!=a->activities().end(); it++) {
        (*it)->accept(m_this);
    }
    DEBUG_LEAVE("visitModelActivityScope");
}

void TaskElaborateActivitySelectReplicateSizes::visitModelActivityTraverse(
    dm::IModelActivityTraverse *a) {
    DEBUG_ENTER("visitModelActivityTraverse");

    for (std::vector<vsc::dm::IModelConstraintUP>::const_iterator
        it=a->getTarget()->getConstraints().begin();
        it!=a->getTarget()->getConstraints().end(); it++) {
        if (m_constraint_s.insert(it->get()).second) {
            DEBUG("Add constraint:\n%s", vsc::dm::PrettyPrinter().print(it->get()));
            m_constraints.push_back(it->get());
        }
    }

    if (a->getActivity()) {
        a->getActivity()->accept(m_this);
    }

    DEBUG_LEAVE("visitModelActivityTraverse");
}

dmgr::IDebug *TaskElaborateActivitySelectReplicateSizes::m_dbg = 0;

}
}
}
