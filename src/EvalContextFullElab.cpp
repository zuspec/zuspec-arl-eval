/*
 * EvalContextFullElab.cpp
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
#include "EvalActivityScopeFullElab.h"
#include "EvalContextFullElab.h"
#include "EvalResult.h"
#include "TaskElaborateRegGroupTypes.h"


namespace zsp {
namespace arl {
namespace eval {


EvalContextFullElab::EvalContextFullElab(
    dmgr::IDebugMgr                                 *dmgr,
    vsc::solvers::IFactory                          *solvers_f,
    dm::IContext                                    *ctxt,
    const vsc::solvers::IRandState                  *randstate,
    dm::IDataTypeComponent                          *root_comp,
    dm::IDataTypeAction                             *root_action,
    IEvalBackend                                    *backend) : 
        m_dmgr(dmgr), m_solvers_f(solvers_f), m_ctxt(ctxt), 
        m_randstate(randstate), m_backend(backend), 
        m_initial(true), m_pss_top(0), m_root_comp(root_comp), 
        m_pss_top_is_init(false), m_root_action(root_action) {
    DEBUG_INIT("EvalContextFullElab", dmgr);

    if (backend) {
        backend->init(this);
    }
}

EvalContextFullElab::~EvalContextFullElab() {

}

int32_t EvalContextFullElab::buildCompTree() {
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
        bool is_solve = false;
        for (std::vector<dm::IDataTypeFunctionImportUP>::const_iterator
            ii=(*it)->getImportSpecs().begin();
            ii!=(*it)->getImportSpecs().end(); ii++) {
            is_solve |= (*ii)->isSolve();
        }
        if (is_solve) {
            m_solve_functions.push_back(*it);
        }
    }

    return 0;
}

int32_t EvalContextFullElab::initCompTree() {

    m_pss_top->initCompTree();

    // TODO: Process InitUp/InitDown exec blocks
    
    TaskElaborateRegGroupTypes(this).elaborate(m_pss_top.get());

    m_pss_top_is_init = true;

    return 0;
}

int32_t EvalContextFullElab::eval() {
    bool ret = false;
    DEBUG_ENTER("eval");

    if (m_initial) {
        if (!m_pss_top) {
            buildCompTree();
        }

        if (!m_pss_top_is_init) {
            initCompTree();
        }

        if (!getBackend()) {
            DEBUG("Note: using stub implementation of backend");
            setBackend(new EvalBackendBase());
        }

        m_activity = ElabActivityUP(TaskElaborateActivity(
            m_solvers_f, 
            m_ctxt).elaborate(
                m_randstate->clone(),
                m_pss_top.get(),
                m_root_action));

        // Add an eval scope for the top-level activity
        getBackend()->enterThread(this);

        EvalActivityScopeFullElab evaluator(
            this,
            this,
            m_activity->activity_s.back().get()
        );

        ret = evaluator.eval();

        m_initial = false;
    } else {
        while (m_eval_s.size()) {
            DEBUG_ENTER("sub-eval %d", m_eval_s.back()->getIdx());
            if (!(ret=m_eval_s.back()->eval())) {
                DEBUG_LEAVE("sub-eval %d -- done", m_eval_s.back()->getIdx());
                m_eval_s.pop_back();
            } else {
                DEBUG_LEAVE("sub-eval %d -- more work", m_eval_s.back()->getIdx());
                break;
            }
        }

        if (m_eval_s.size() == 0) {
            getBackend()->leaveThread(this);
        }
    }

    DEBUG_LEAVE("eval (%d)", ret);
    return ret;
}

void EvalContextFullElab::pushEval(IEval *e, bool owned) {
    e->setIdx(m_eval_s.size());
    m_eval_s.push_back(IEvalUP(e, owned));
}

void EvalContextFullElab::suspendEval(IEval *e) {
    m_eval_s.at(e->getIdx()) = IEvalUP(e->clone(), true);
}

void EvalContextFullElab::popEval(IEval *e) {
    if (e->haveResult()) {
        DEBUG("hasResult (%d)", m_eval_s.size());
        if (m_eval_s.size() > 1) {
            m_eval_s.at(m_eval_s.size()-2)->setResult(e->moveResult());
        } else {
            setResult(e->moveResult());
        }
    } else {
        DEBUG("NOT hasResult");
    }
    m_eval_s.pop_back();
}

void EvalContextFullElab::setFunctionData(
        dm::IDataTypeFunction       *func_t,
        IEvalFunctionData           *data) {

}

void EvalContextFullElab::callListener(
    const std::function<void (IEvalListener *)> &f) {

    for (std::vector<IEvalListener *>::const_iterator
        it=m_listeners.begin();
        it!=m_listeners.end(); it++) {
        f(*it);
    }
}

void EvalContextFullElab::setResult(IEvalResult *r) {
    DEBUG_ENTER("setResult sz=%d", m_eval_s.size());
    if (m_eval_s.size()) {
        m_eval_s.back()->setResult(r);
    }
    DEBUG_LEAVE("setResult");
}

int32_t EvalContextFullElab::evalMethodCallContext(
        dm::IDataTypeFunction                   *method,
        vsc::dm::IModelField                    *method_ctxt,
        const std::vector<vsc::dm::ITypeExpr *> &params) {
    return -1;
}

IEvalResult *EvalContextFullElab::mkEvalResultVal(const vsc::dm::IModelVal *val) {
    return new(&m_result_alloc, (val)?val->bits():0) EvalResult(
        &m_result_alloc,
        val);
}

IEvalResult *EvalContextFullElab::mkEvalResultKind(EvalResultKind kind) {
    return new(&m_result_alloc, 0) EvalResult(
        &m_result_alloc,
        kind);
}

IEvalResult *EvalContextFullElab::mkEvalResultRef(vsc::dm::IModelField *ref) {
    return new(&m_result_alloc, 0) EvalResult(
        &m_result_alloc,
        ref);
}

IEvalResult *EvalContextFullElab::mkEvalResultValS(int64_t val, int32_t bits) {
    return new(&m_result_alloc, bits) EvalResult(
        &m_result_alloc,
        bits,
        val);
}

IEvalResult *EvalContextFullElab::mkEvalResultValU(uint64_t val, int32_t bits) {
    return new(&m_result_alloc, bits) EvalResult(
        &m_result_alloc,
        bits,
        val);
}

void EvalContextFullElab::finalizeComponentTree() {

}

dmgr::IDebug *EvalContextFullElab::m_dbg = 0;


}
}
}
