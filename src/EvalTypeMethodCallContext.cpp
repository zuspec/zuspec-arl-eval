/*
 * EvalTypeMethodCallContext.cpp
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
#include "EvalTypeExpr.h"
#include "EvalTypeMethodCallContext.h"


namespace zsp {
namespace arl {
namespace eval {


EvalTypeMethodCallContext::EvalTypeMethodCallContext(
    IEvalThread                                 *thread,
    dm::IDataTypeFunction                       *method,
    vsc::dm::IModelField                        *method_ctxt,
    const std::vector<vsc::dm::ITypeExpr *>     &params) :
    EvalBase(thread), m_method(method), m_method_ctxt(method_ctxt),
    m_params(params.begin(), params.end()), m_idx(0), m_subidx(0) {
    DEBUG_INIT("zsp::arl::eval::EvalTypeMethodCallContext", thread->getDebugMgr());

}

EvalTypeMethodCallContext::EvalTypeMethodCallContext(EvalTypeMethodCallContext *o) :
    EvalBase(o), m_method(o->m_method), m_method_ctxt(o->m_method_ctxt),
    m_params(o->m_params.begin(), o->m_params.end()),
    m_idx(o->m_idx), m_subidx(o->m_subidx) {

}

EvalTypeMethodCallContext::~EvalTypeMethodCallContext() {

}

int32_t EvalTypeMethodCallContext::eval() {
    DEBUG_ENTER("eval");

    if (m_initial) {
        m_thread->pushEval(this);

        // Safety
        setResult(m_thread->mkEvalResultKind(EvalResultKind::Void));
    }

    switch (m_idx) {
        case 0: {
            if (m_subidx > 0 && haveResult()) {
                m_pvals.push_back(IEvalResultUP(moveResult()));
            }
            while (m_subidx < m_params.size()) {
                EvalTypeExpr evaluator(m_ctxt, m_thread, m_params.at(m_subidx));

                m_subidx += 1;
                clrResult();
                if (evaluator.eval()) {
                    break;
                } else {
                    if (haveResult()) {
                        fprintf(stdout, "Note: push expr result\n");
                        m_pvals.push_back(IEvalResultUP(moveResult()));
                    }
                }
            }

            // If we left the loop before achieving enough
            // parameters, suspend...
            if (m_subidx < m_params.size()) {
                break;
            }

            clrResult(); // Clear 'safety' result

            m_idx = 1;
            m_thread->getBackend()->callFuncReq(
                m_thread,
                m_method,
                m_pvals
            );

            if (!haveResult()) {
                break;
            }
        }

        case 1: {
            // Wait for a response

        }

    }

    int32_t ret = !haveResult();

    if (m_initial) {
        m_initial = false;
        if (haveResult()) {
            m_thread->popEval(this);
        } else {
            m_thread->suspendEval(this);
        }
    }

    DEBUG_LEAVE("eval m_idx=%d %d", m_idx, ret);

    return ret;
}

dmgr::IDebug *EvalTypeMethodCallContext::m_dbg = 0;

}
}
}
