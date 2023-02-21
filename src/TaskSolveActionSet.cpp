/*
 * TaskSolveActionSet.cpp
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
#include "zsp/arl/dm/IDataTypeAction.h"
#include "zsp/arl/dm/IDataTypeComponent.h"
#include "zsp/arl/dm/IModelFieldPool.h"
#include "dmgr/impl/DebugMacros.h"
#include "vsc/dm/impl/PrettyPrinter.h"
#include "vsc/dm/impl/TaskUnrollModelFieldRefConstraints.h"
#include "vsc/dm/impl/TaskBuildRefConstraintMap.h"
#include "TaskCollectFlowObjFields.h"
#include "TaskIsDataTypeFlowObj.h"
#include "TaskSolveActionSet.h"



namespace zsp {
namespace arl {
namespace eval {

TaskSolveActionSet::TaskSolveActionSet(
    dm::IContext                *ctxt,
    vsc::solvers::IRandState    *randstate,
    dm::IModelFieldComponent    *comp) : 
    m_ctxt(ctxt), m_randstate(randstate), m_comp(comp) {
    DEBUG_INIT("TaskSolveActionSet", ctxt->getDebugMgr());
    m_action_data = 0;
}

TaskSolveActionSet::~TaskSolveActionSet() {

}

bool TaskSolveActionSet::solve(
    const std::vector<dm::IModelActivityTraverse *> &traversals) {
    DEBUG_ENTER("solve - %d traversals", traversals.size());
    bool ret = true;


    m_all_comp_m.clear();
    m_all_comp_l.clear();

    // Initialize the action-data collection
    m_action_data_l.clear();
    for (std::vector<dm::IModelActivityTraverse *>::const_iterator
        it=traversals.begin(); 
        it!=traversals.end(); it++) {
        m_action_data_l.push_back(ActionData(*it));

        for (std::vector<vsc::dm::IModelConstraintUP>::const_iterator
            c_it=(*it)->getTarget()->getConstraints().begin();
            c_it!=(*it)->getTarget()->getConstraints().end(); c_it++) {
            DEBUG("Action constraint: %s", vsc::dm::PrettyPrinter().print(c_it->get()));
            m_constraints.push_back(c_it->get());
        }

        if ((*it)->getWithC()) {
            m_constraints.push_back((*it)->getWithC());
        }
    }

    build_comp_map(traversals);

    // Collect claims
    // Out goal is to build a per-type map of all resources
    // and their extents (ie what components provide them)
    //
    // comp_id -> [range]
    // 
    for (std::vector<dm::IModelActivityTraverse *>::const_iterator
        it=traversals.begin(); 
        it!=traversals.end(); it++) {
        (*it)->getTarget()->getDataType()->accept(m_this);
    }

    /****************************************************************
     * At this point, we have:
     * - Action/component map information
     * - List of all valid actions
     * - Pool information about all resource types used by the actions
     * - 
     * 
     * We now need to analyze the refs against the constraints to 
     * determine how to unroll the selector constraints
     ****************************************************************/

    // Build out the component constraints for each action
    for (std::vector<ActionData>::iterator
        it=m_action_data_l.begin();
        it!=m_action_data_l.end(); it++) {

        std::vector<vsc::dm::IModelField *> candidates(m_all_comp_l.size());
        for (std::vector<uint32_t>::const_iterator
            comp_it=it->m_comp_ctxt_l.begin();
            comp_it!=it->m_comp_ctxt_l.end(); comp_it++) {
            candidates[*comp_it] = m_all_comp_l[*comp_it];
        }

#ifdef UNDEFINED
        m_ref_l.push_back(vsc::dm::RefSelectorUP(vsc::dm::TaskBuildRefSelector(m_ctxt).build(
                it->m_traversal->getTarget()->getFieldT<vsc::dm::IModelFieldRef>(0),
                candidates)));
#endif
    }

    // Visit each action to build out selectors for the claims and refs
    for (uint32_t i=0; i<traversals.size(); i++) {
        m_action_data = &m_action_data_l.at(i);
        traversals.at(i)->getTarget()->accept(m_this);
    }

    /****************************************************************
     * Okay, we now have all the selectors built. We need to collect
     * just the refs so we can reason about cross-selector dependeices
     ****************************************************************/
    std::vector<vsc::dm::IModelField *>      ref_fields;
    std::vector<vsc::dm::IModelConstraint *> ref_constraints;
#ifdef UNDEFINED
    for (std::vector<vsc::dm::IRefSelectorUP>::const_iterator
        it=m_ref_l.begin();
        it!=m_ref_l.end(); it++) {
        ref_fields.push_back(it->get()->getRef());
    }
#endif

    // Collect the constraints we need to reason about 
    // cross-selector dependencise
    // TODO: this should probably include constraints from above
    for (std::vector<dm::IModelActivityTraverse *>::const_iterator
        it=traversals.begin(); 
        it!=traversals.end(); it++) {
        if ((*it)->getWithC()) {
            ref_constraints.push_back((*it)->getWithC());
        }
        for (std::vector<vsc::dm::IModelConstraintUP>::const_iterator
            c_it=(*it)->getTarget()->getConstraints().begin();
            c_it!=(*it)->getTarget()->getConstraints().end(); c_it++) {
            ref_constraints.push_back(c_it->get());
        }
    }

#ifdef UNDEFINED
    std::vector<vsc::dm::RefConstraintSet> ref_constraint_m = 
        vsc::dm::TaskBuildRefConstraintMap(m_ctxt).build(
            ref_fields,
            m_constraints);

    DEBUG("ref_constraint_m: %d", ref_constraint_m.size());

    std::vector<vsc::dm::IModelConstraintUP> selector_constraints;

    /****************************************************************
     * We now have a list of [ref_idx][constraints] pairs. 
     * 
     * Transform this into a list of [RefSelector],[constraints] pairs
     ****************************************************************/
    for (std::vector<vsc::dm::RefConstraintSet>::const_iterator
        it=ref_constraint_m.begin();
        it!=ref_constraint_m.end(); it++) {
        std::vector<vsc::dm::RefSelector *> selectors;
        for (std::vector<int32_t>::const_iterator
            s_it=it->first.begin();
            s_it!=it->first.end(); s_it++) {
            selectors.push_back(m_ref_l.at(*s_it).get());
        }
        vsc::dm::TaskUnrollModelFieldRefConstraints(m_ctxt).build(
            selector_constraints,
            selectors,
            it->second);
    }

    /****************************************************************
     * We should now have a list of selector constraints that handle
     * all cross-dependencies between the selector variables.
     * 
     * Now, we need to add in the cross constraints from resources
     ****************************************************************/
    build_resource_constraints(selector_constraints);


    // Now, solve out ref assignments
    vsc::dm::ICompoundSolverUP solver(m_ctxt->mkCompoundSolver());

    std::vector<vsc::dm::IModelConstraint *> sel_constraints;
    std::vector<vsc::dm::IModelField *> sel_fields;
    for (std::vector<vsc::dm::IModelConstraintUP>::const_iterator
        it=selector_constraints.begin();
        it!=selector_constraints.end(); it++) {
        DEBUG("Constraint: %s", vsc::dm::PrettyPrinter().print(it->get()));
        sel_constraints.push_back(it->get());
    }
    for (std::vector<vsc::dm::RefSelectorUP>::const_iterator
        it=m_ref_l.begin();
        it!=m_ref_l.end(); it++) {
        sel_fields.push_back(it->get()->m_selector.get());
        for (std::vector<vsc::dm::IModelConstraintUP>::const_iterator
            c_it=it->get()->m_selector.get()->constraints().begin();
            c_it!=it->get()->m_selector.get()->constraints().end(); c_it++) {
            DEBUG("Sel Constraint: %s", vsc::dm::PrettyPrinter().print(c_it->get()));
        }
        DEBUG("Valid C: %s", vsc::dm::PrettyPrinter().print((*it)->m_valid_c.get()));
        sel_constraints.push_back((*it)->m_valid_c.get());
    }

    ret = solver->solve(
            m_randstate,
            sel_fields,
            sel_constraints,
            vsc::dm::SolveFlags::Randomize
                | vsc::dm::SolveFlags::RandomizeDeclRand
                | vsc::dm::SolveFlags::RandomizeTopFields);

    DEBUG("ret=%d", ret);

    // Assign action refs if we were able to solve properly
    if (ret) {
        for (std::vector<vsc::dm::RefSelectorUP>::const_iterator
            it=m_ref_l.begin();
            it!=m_ref_l.end(); it++) {
            vsc::dm::IModelFieldRef *ref = dynamic_cast<vsc::dm::IModelFieldRef *>((*it)->m_ref);
            uint32_t idx = (*it)->m_selector->val()->val_u();
            DEBUG("ref %s ; idx=%d", ref->name().c_str(), idx);
            if (idx < (*it)->m_candidates.size()) {
                ref->setRef((*it)->m_candidates.at(idx));
            } else {
                DEBUG("Error: not setting ref");
            }
        }
    }

    /****************************************************************
     * Once the refs are assigned, solve out the rest of the action
     ****************************************************************/
    std::vector<vsc::dm::IModelField *> actions;
    std::vector<vsc::dm::IModelConstraint *> action_constraints;

    if (ret) {
        for (std::vector<IModelActivityTraverse *>::const_iterator
            it=traversals.begin();
            it!=traversals.end(); it++) {
            actions.push_back((*it)->getTarget());
            if ((*it)->getWithC()) {
                action_constraints.push_back((*it)->getWithC());
            }
        }

        ret = solver->solve(
                m_randstate,
                actions,
                action_constraints,
                vsc::dm::SolveFlags::Randomize
                    | vsc::dm::SolveFlags::RandomizeDeclRand
                    | vsc::dm::SolveFlags::RandomizeTopFields);
        DEBUG("Action Solve: %d", ret);
    }

    DEBUG_LEAVE("solve - ret=%d", ret);
#endif
    return ret;    
}

void TaskSolveActionSet::build_comp_map(
    const std::vector<dm::IModelActivityTraverse *> &traversals) {
    // Build a list of all component contexts and the
    // per-action specifics

    for (uint32_t i=0; i<traversals.size(); i++) {
        dm::IModelActivityTraverse *tr = traversals.at(i);
        // For each action instance, ask where its type can run
        dm::IDataTypeAction *action_t = tr->getTarget()->getDataTypeT<dm::IDataTypeAction>();
        dm::IDataTypeComponent *comp_t = action_t->getComponentType();

#ifdef UNDEFINED
        const std::vector<IModelFieldComponent *> &comp_l = 
            m_comp->getCompMap()->getSubContexts(comp_t);
        
        // Now, we need to merge these with the global list, and 
        // capture which are needed on a per-action basis
        for (std::vector<IModelFieldComponent *>::const_iterator
            c_it=comp_l.begin();
            c_it!=comp_l.end(); c_it++) {
            AllCompMapT::const_iterator it=m_all_comp_m.find(*c_it);
            uint32_t idx;

            if (it == m_all_comp_m.end()) {
                idx = m_all_comp_l.size();
                m_all_comp_l.push_back(*c_it);
                m_all_comp_m.insert({*c_it, idx});
            } else {
                idx = it->second;
            }

            m_action_data_l.at(i).m_comp_ctxt_l.push_back(idx);
        }
#endif
    }
}

void TaskSolveActionSet::build_resource_constraints(
        std::vector<vsc::dm::IModelConstraintUP> &constraints) {
    // Build resource constraints
#ifdef UNDEFINED
    for (std::vector<ResourceClaimData *>::const_iterator
        it=m_res_type_l.begin();
        it!=m_res_type_l.end(); it++) {
        DEBUG("Resource: %s lock=%d share=%d", 
            dynamic_cast<vsc::dm::IDataTypeStruct *>((*it)->m_res_t)->name().c_str(),
            (*it)->m_lock_claims.size(), (*it)->m_share_claims.size());
        for (uint32_t i=0; i<(*it)->m_lock_claims.size(); i++) {
            for (uint32_t j=i+1; j<(*it)->m_lock_claims.size(); j++) {
                vsc::dm::IModelConstraintExpr *distinct_c = m_ctxt->mkModelConstraintExpr(
                    m_ctxt->mkModelExprBin(
                        m_ctxt->mkModelExprFieldRef((*it)->m_lock_claims.at(i)->m_selector.get()),
                        vsc::dm::BinOp::Ne,
                        m_ctxt->mkModelExprFieldRef((*it)->m_lock_claims.at(j)->m_selector.get())
                    ));
                constraints.push_back(vsc::dm::IModelConstraintExprUP(distinct_c));
            }
        }
        for (uint32_t i=0; i<(*it)->m_lock_claims.size(); i++) {
            for (uint32_t j=0; j<(*it)->m_share_claims.size(); j++) {
                vsc::dm::IModelConstraintExpr *distinct_c = m_ctxt->mkModelConstraintExpr(
                    m_ctxt->mkModelExprBin(
                        m_ctxt->mkModelExprFieldRef((*it)->m_lock_claims.at(i)->m_selector.get()),
                        vsc::dm::BinOp::Ne,
                        m_ctxt->mkModelExprFieldRef((*it)->m_lock_claims.at(j)->m_selector.get())
                    ));
                constraints.push_back(vsc::dm::IModelConstraintExprUP(distinct_c));
            }
        } 
    }
#endif
}

void TaskSolveActionSet::visitModelFieldRef(vsc::dm::IModelFieldRef *f) {
    DEBUG_ENTER("visitModelFieldRef %s", f->name().c_str());
    if (dm::TaskIsDataTypeFlowObj::test(f->getDataType())) {
        dm::IDataTypeFlowObj *fo_t = f->getDataTypeT<dm::IDataTypeFlowObj>();
        DEBUG("Is a DataTypeFlowObj %d", fo_t->kind());

        if (fo_t->kind() == dm::FlowObjKindE::Resource) {
            // Build a selector to implement assignment of this resource
            ResTypeMapT::iterator res_t = m_res_type_m.find(f->getDataType());
            std::vector<vsc::dm::IModelField *> candidates;

            uint32_t comp_ctxt_idx = 0;
            uint32_t rsrc_idx = 0;
            for (std::vector<std::pair<int32_t,int32_t>>::const_iterator
                it=res_t->second.m_comp_sz_l.begin();
                it!=res_t->second.m_comp_sz_l.end(); it++) {
                bool valid = (m_action_data->m_comp_ctxt_l.at(comp_ctxt_idx) == it->first);

                DEBUG("Resource: %d %d", it->first, it->second);

                for (uint32_t i=0; i<it->second; i++) {
                    if (valid) {
                        candidates.push_back(res_t->second.m_resource_l.at(rsrc_idx));
                    } else {
                        candidates.push_back(0);
                    }
                    rsrc_idx++;
                }

                if (valid) {
                    comp_ctxt_idx++;
                }
            }

#ifdef UNDEFINED
            vsc::dm::RefSelector *selector = vsc::dm::TaskBuildRefSelector(m_ctxt).build(
                f,
                candidates);
            m_ref_l.push_back(vsc::dm::RefSelectorUP(selector));

            // Store the data on the action as well as in the 
            bool is_lock = true; // TODO:

            if (is_lock) {
                res_t->second.m_lock_claims.push_back(selector);
            } else {
                res_t->second.m_share_claims.push_back(selector);
            }
#endif
        } else {
            // TODO:

        }
    }

    DEBUG_LEAVE("visitModelFieldRef %s", f->name().c_str());
}

void TaskSolveActionSet::visitTypeFieldClaim(dm::ITypeFieldClaim *f) {
    DEBUG_ENTER("visitTypeFieldClaim %s", f->name().c_str());
#ifdef UNDEFINED
    vsc::dm::IDataType *claim_t = f->getDataType();

    // - Process every component instance that we have
    // - If the component instance has a mapping for ResType, add it
    // 
    ResTypeMapT::iterator it = m_res_type_m.find(claim_t);

    if (it == m_res_type_m.end()) {
        it = m_res_type_m.insert({claim_t, ResourceClaimData(claim_t)}).first;
        m_res_type_l.push_back(&it->second);

        for (uint32_t comp_idx=0; comp_idx<m_all_comp_l.size(); comp_idx++) {
            IModelFieldPool *pool = m_all_comp_l.at(comp_idx)->getCompMap()->getPool(f);

            if (pool) {
                it->second.m_comp_sz_l.push_back({comp_idx, pool->getObjects().size()});

                if (pool->getObjects().size() == 0) {
                    DEBUG("Error: zero-sized pool %s", pool->name().c_str());
                }

                for (std::vector<vsc::dm::IModelFieldUP>::const_iterator
                    o_it=pool->getObjects().begin();
                    o_it!=pool->getObjects().end(); o_it++) {
                    it->second.m_resource_l.push_back(o_it->get());
                }
            } else {
                DEBUG("Error: no pool for resource claim %s", f->name().c_str());
            }
        }
    }
#endif
    DEBUG_LEAVE("visitTypeFieldClaim");
}

void TaskSolveActionSet::visitTypeFieldInOut(dm::ITypeFieldInOut *f) {

}

dmgr::IDebug *TaskSolveActionSet::m_dbg = 0;

}
}
}
