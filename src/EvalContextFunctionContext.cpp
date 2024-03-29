/*
 * EvalContextFunctionContext.cpp
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
#include "zsp/arl/eval/impl/EvalBackendBase.h"
#include "EvalContextFunctionContext.h"
#include "EvalTypeMethodCallContext.h"


namespace zsp {
namespace arl {
namespace eval {


EvalContextFunctionContext::EvalContextFunctionContext(
    dmgr::IDebugMgr                             *dmgr,
    vsc::solvers::IFactory                      *solvers_f,
    arl::dm::IContext                           *ctxt,
    const vsc::solvers::IRandState              *randstate,
    IEvalBackend                                *backend,
    dm::IDataTypeFunction                       *func,
    vsc::dm::IModelField                        *func_ctxt,
    const std::vector<vsc::dm::ITypeExpr *>     &params) :
        EvalContextBase(dmgr, solvers_f, ctxt, randstate, 0, backend),
        m_func(func), m_func_ctxt(func_ctxt),
        m_params(params.begin(), params.end()) {
    DEBUG_INIT("zsp::arl::eval::EvalContextFunctionContext", dmgr);

    // TODO: Collect functions

}



EvalContextFunctionContext::~EvalContextFunctionContext() {

}

int32_t EvalContextFunctionContext::eval() {
    int32_t ret = 0;

    if (m_initial) {
//        pushEval(this);

        if (!getBackend()) {
            setBackend(new EvalBackendBase());
        }

        setFlags(EvalFlags::Complete);

        ret = EvalTypeMethodCallContext(
                this, 
                this,
                0, // TODO: ValProvider
                m_func,
                m_func_ctxt,
                m_params).eval();
    }

    DEBUG("haveResult: %d", hasFlags(EvalFlags::Complete));

    if (m_initial) {
        m_initial = false;

/*
        if (!hasFlags(EvalFlags::Complete)) {
            suspendEval(this);
        } else {
            popEval(this);
        }
 */
    }

    return ret;
}

}
}
}
