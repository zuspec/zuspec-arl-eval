/*
 * EvalContextIncrElab.cpp
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
#include "dmgr/impl/DebugMacros.h"
#include "zsp/arl/dm/impl/ModelBuildContext.h"
#include "zsp/arl/eval/impl/EvalBackendBase.h"
#include "EvalContextIncrElab.h"
#include "EvalTypeActionIncrElab.h"
#include "TaskElaborateRegGroupTypes.h"
#include "TaskEvalComponentExecInit.h"


namespace zsp {
namespace arl {
namespace eval {


EvalContextIncrElab::EvalContextIncrElab(
    dmgr::IDebugMgr                                 *dmgr,
    vsc::solvers::IFactory                          *solvers_f,
    dm::IContext                                    *ctxt,
    const vsc::solvers::IRandState                  *randstate,
    pyapi::IPyEval                                  *pyeval,
    dm::IDataTypeComponent                          *root_comp,
    dm::IDataTypeAction                             *root_action,
    IEvalBackend                                    *backend) : 
        EvalContextBase(dmgr, solvers_f, ctxt, randstate, pyeval, backend),
        m_pss_top(0), m_root_comp(root_comp), 
        m_pss_top_is_init(false), m_root_action(root_action) {
    DEBUG_INIT("zsp::arl::eval::EvalContextIncrElab", dmgr);

    if (!m_pss_top) {
        buildCompTree();
    }

    if (backend) {
        backend->init(this);
    }

}

EvalContextIncrElab::~EvalContextIncrElab() {

}

int32_t EvalContextIncrElab::buildCompTree() {
    DEBUG_ENTER("buildCompTree");
    std::set<std::string> ignore = {
        "make_handle_from_claim",
        "make_handle_from_handle",
        "addr_value"
    };
    dm::ModelBuildContext build_ctxt(m_ctxt);

    // Build component tree
    m_pss_top = dm::IModelFieldComponentRootUP(
        m_root_comp->mkRootFieldT<dm::IModelFieldComponentRoot>(
            &build_ctxt,
            "pss_top",
            false));

    // Collect all solve import functions
    for (std::vector<dm::IDataTypeFunction *>::const_iterator
        it=m_ctxt->getDataTypeFunctions().begin();
        it!=m_ctxt->getDataTypeFunctions().end(); it++) {
        if (ignore.find((*it)->name()) == ignore.end()) {
            bool is_solve = false;
            DEBUG("Process Function %s", (*it)->name().c_str());
            for (std::vector<dm::IDataTypeFunctionImportUP>::const_iterator
                ii=(*it)->getImportSpecs().begin();
                ii!=(*it)->getImportSpecs().end(); ii++) {
                is_solve |= (*ii)->isSolve();
            }
            if (is_solve) {
                m_solve_functions.push_back(*it);
            } else {
                m_target_functions.push_back(*it);
            }
        }
    }

    DEBUG_LEAVE("buildCompTree %d %d",
        m_solve_functions.size(),
        m_target_functions.size());
    return 0;
}

int32_t EvalContextIncrElab::initCompTree() {

    m_pss_top->initCompTree();

    // Process InitUp/InitDown exec blocks
    TaskEvalComponentExecInit(this, this).eval(
        m_pss_top->getDataTypeT<dm::IDataTypeComponent>(),
        m_pss_top->getMutVal());
    
    TaskElaborateRegGroupTypes(this).elaborate(m_pss_top.get());

    m_pss_top_is_init = true;

    return 0;
}

int32_t EvalContextIncrElab::eval() {
    bool ret = false;
    DEBUG_ENTER("[%d] eval", m_eval_s.size());

    if (m_initial) {
        dm::ModelBuildContext build_ctxt(m_ctxt);

        initPython();

        if (haveError()) {
            ret = true;
            goto leaving;
        }

        if (!m_pss_top_is_init) {
            initCompTree();
        }

        if (!getBackend()) {
            DEBUG("Note: using stub implementation of backend");
            setBackend(new EvalBackendBase());
        }

        m_root_action_f = m_root_action->mkRootField(
            &build_ctxt,
            m_root_action->name(),
            false);

        /*
        m_activity = ElabActivityUP(TaskElaborateActivity(
            m_solvers_f, 
            m_ctxt).elaborate(
                m_randstate->clone(),
                m_pss_top.get(),
                m_root_action));
         */

        // Add an eval scope for the top-level activity
        getBackend()->enterThread(this);

        /*
        EvalActivityScopeFullElab evaluator(
            this,
            this,
            m_activity->activity_s.back().get()
        );

        ret = evaluator.eval();
         */
        ret = EvalTypeActionIncrElab(
                this,
                this, 
                m_pss_top->getImmVal(),
                m_root_action_f->getMutVal()).eval();

        m_initial = false;
    } else {
        while (m_eval_s.size()) {
            DEBUG("m_eval_s size=%d", m_eval_s.size());
            DEBUG_ENTER("sub-eval %d", m_eval_s.back()->getIdx());
            if (!(ret=m_eval_s.back()->eval())) {
                DEBUG_LEAVE("sub-eval %d -- done", m_eval_s.back()->getIdx());
                // Propagate result
                if (m_eval_s.size() > 1) {
                    m_eval_s.at(m_eval_s.size()-2)->setResult(
                        m_eval_s.back()->getResult());
                }
                m_eval_s.pop_back();
            } else {
                DEBUG_LEAVE("sub-eval %d -- more work", m_eval_s.back()->getIdx());
                ret = (haveError())?-1:ret;
                break;
            }
        }

        if (m_eval_s.size() == 0) {
            getBackend()->leaveThread(this);
        }
    }

leaving:
    if (m_pyeval) {
        DEBUG("Flush");
        m_pyeval->flush();
    }

    if (ret) {
        DEBUG("Suspending...");
    }

    DEBUG_LEAVE("[%d] eval (%d)", m_eval_s.size(), ret);
    return ret;
}

void EvalContextIncrElab::evalExecInit(
        dm::IDataTypeComponent      *comp_t,
        const vsc::dm::ValRef       &val) {
    // TODO: Create a value provider

    // Call 

}


void EvalContextIncrElab::finalizeComponentTree() {

}

}
}
}
