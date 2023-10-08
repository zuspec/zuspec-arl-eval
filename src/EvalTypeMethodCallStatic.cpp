/*
 * EvalTypeMethodCallStatic.cpp
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
#include "EvalTypeMethodCallStatic.h"
#include "EvalTypeExpr.h"
#include "EvalTypeProcStmtScope.h"

namespace zsp {
namespace arl {
namespace eval {


EvalTypeMethodCallStatic::EvalTypeMethodCallStatic(
    IEvalContext                                *ctxt,
    IEvalThread                                 *thread,
    IEvalValProvider                            *vp,
    dm::IDataTypeFunction                       *func,
    const std::vector<vsc::dm::ITypeExpr *>     &params) :
    EvalBase(ctxt, thread), m_vp(vp), m_func(func),
    m_params(params.begin(), params.end()),
    m_idx(0), m_param_idx(0) {

    DEBUG_INIT("zsp::arl::eval::EvalTypeMethodCallStatic", ctxt->getDebugMgr());
}

EvalTypeMethodCallStatic::EvalTypeMethodCallStatic(EvalTypeMethodCallStatic *o) :
    EvalBase(o), m_func(o->m_func), m_params(o->m_params.begin(), o->m_params.end()),
    m_idx(o->m_idx), m_param_idx(o->m_param_idx) {
    for (std::vector<vsc::dm::ValRef>::iterator
        it=o->m_pvals.begin();
        it!=o->m_pvals.end(); it++) {
        m_pvals.push_back(*it);
    }
}

EvalTypeMethodCallStatic::~EvalTypeMethodCallStatic() {

}

int32_t EvalTypeMethodCallStatic::eval() {
    bool intf = m_func->getBody();
    int32_t ret = 0;
    DEBUG_ENTER("eval: m_idx=%d m_param_idx=%d", m_idx, m_param_idx);

    if (m_initial) {
        m_thread->pushEval(this);
        // Safety
        setVoidResult();

        if (intf) {
            m_stack_frame = IEvalStackFrameUP(m_ctxt->mkStackFrame(m_params.size()));
        }
    }

    switch (m_idx) {
        case 0: {
            if (m_param_idx > 0 && haveResult()) {
                if (intf) {
                    m_stack_frame->setVariable(m_param_idx-1, moveResult());
                } else {
                    vsc::dm::ValRef pval(moveResult());
                    m_pvals.push_back(pval);
                }
            }
            while (m_param_idx < m_params.size()) {
                EvalTypeExpr evaluator(
                    m_ctxt, 
                    m_thread, 
                    m_vp,
                    m_params.at(m_param_idx));

                m_param_idx += 1;
                clrResult();
                if (evaluator.eval()) {
                    break;
                } else {
                    if (haveResult()) {
                        fprintf(stdout, "Note: push expr result\n");
                        if (intf) {
                            m_stack_frame->setVariable(m_param_idx-1, moveResult());
                        } else {
                            vsc::dm::ValRef pval(moveResult());
                            m_pvals.push_back(pval);
                        }
                    }
                }
            }

            // If we left the loop before achieving enough
            // parameters, suspend...
            if (m_param_idx < m_params.size()) {
                break;
            }

            clrResult(); // Clear 'safety' result

            m_idx = 1;

            // Determine *what* to call
            // - If there is a body, evaluate that

            if (m_func->getBody()) {
                DEBUG("Launching proc-body interpreter");

                // Push parameters
                m_thread->pushStackFrame(m_stack_frame.release());

                // TODO: push local-vars
                m_thread->pushStackFrame(m_ctxt->mkStackFrame(0));
                EvalTypeProcStmtScope(
                    m_ctxt,
                    m_thread,
                    m_vp,
                    m_func->getBody()).eval();
            } else {
                DEBUG("Calling import function");
                m_ctxt->getBackend()->callFuncReq(
                    m_thread,
                    m_func,
                    m_pvals
                );
            }

            if (!haveResult()) {
                break;
            }
        }

        case 1: {
            // Wait for a response
            if (haveResult()) {
                if (m_func->getBody()) {
                    m_thread->popStackFrame();
                    m_thread->popStackFrame();
                }
            }
        }
    }

    ret = !haveResult();

    if (m_initial) {
        m_initial = false;
        if (haveResult()) {
            DEBUG("popEval");
            m_thread->popEval(this);
        } else {
            DEBUG("suspendEval");
            m_thread->suspendEval(this);
        }
    }

    DEBUG_LEAVE("eval: m_idx=%d m_param_idx=%d %d", m_idx, m_param_idx, ret);

    return ret;
}

IEval *EvalTypeMethodCallStatic::clone() {
    return new EvalTypeMethodCallStatic(this);
}

dmgr::IDebug *EvalTypeMethodCallStatic::m_dbg = 0;

}
}
}
