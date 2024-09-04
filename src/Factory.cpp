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
#include "zsp/arl/dm/impl/ModelBuildContext.h"
#include "zsp/arl/eval/FactoryExt.h"
#include "zsp/arl/eval/impl/EvalBackendBase.h"
#include "EvalContextFullElab.h"
#include "EvalContextIncrElab.h"
#include "EvalStackFrame.h"
#include "EvalThread.h"
#include "EvalContextFunctionContext.h"
#include "EvalContextFunctionStatic.h"
#include "Factory.h"
#include "ModelEvaluatorFullElab.h"
#include "ModelEvaluatorIncrElab.h"
#include "ModelEvaluatorFullElabActivity.h"
#include "TaskBuildComponentTreeData.h"
#include "TaskElaborateActivity.h"

namespace zsp {
namespace arl {
namespace eval {


Factory::Factory() : m_dmgr(0) {

}

Factory::~Factory() {

}

IComponentTreeData *Factory::mkComponentTreeData(
        arl::dm::IDataTypeComponent     *root_t) {
    return TaskBuildComponentTreeData(m_dmgr).build(root_t);
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
        pyapi::IPyEval                  *pyeval,
        dm::IDataTypeComponent          *root_comp,
        dm::IDataTypeAction             *root_action,
        IEvalBackend                    *backend) {


    IEvalContext *ret = 0;
#ifdef UNDEFINED
    ret = new EvalContextFullElab(
        m_dmgr,
        solvers_f,
        ctxt,
        randstate,
        root_comp,
        root_action,
        backend);
#else
    ret = new EvalContextIncrElab(
        m_dmgr,
        solvers_f,
        ctxt,
        randstate,
        pyeval,
        root_comp,
        root_action,
        backend);
#endif

    return ret;
}

IEvalThread *Factory::mkEvalThread(
        IEvalBackend                    *backend,
        IEvalThread                     *parent) {
    EvalThread *ret;
    if (parent) {
        ret = new EvalThread(parent);
    } else {
        ret = new EvalThread(m_dmgr);
    }
    if (!backend) {
        backend = new EvalBackendBase();
    }
    ret->setBackend(backend);
    return ret;
}

IEvalContext *Factory::mkEvalContextFunctionStatic(
    vsc::solvers::IFactory                      *solvers_f,
    arl::dm::IContext                           *ctxt,
    const vsc::solvers::IRandState              *randstate,
    IEvalBackend                                *backend,
    dm::IDataTypeFunction                       *func,
    const std::vector<vsc::dm::ITypeExpr *>     &params) {
    return new EvalContextFunctionStatic(
        m_dmgr,
        solvers_f,
        ctxt,
        randstate,
        backend,
        func,
        params);
}

IEvalContext *Factory::mkEvalContextFunctionContext(
    vsc::solvers::IFactory                      *solvers_f,
    arl::dm::IContext                           *ctxt,
    const vsc::solvers::IRandState              *randstate,
    IEvalBackend                                *backend,
    dm::IDataTypeFunction                       *func,
    vsc::dm::IModelField                        *func_ctxt,
    const std::vector<vsc::dm::ITypeExpr *>     &params) {
    return new EvalContextFunctionContext(
        m_dmgr,
        solvers_f,
        ctxt,
        randstate,
        backend,
        func,
        func_ctxt,
        params
    );
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

