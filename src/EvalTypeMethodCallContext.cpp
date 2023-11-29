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
#include "EvalTypeProcStmtScope.h"


namespace zsp {
namespace arl {
namespace eval {


EvalTypeMethodCallContext::EvalTypeMethodCallContext(
    IEvalContext                                *ctxt,
    IEvalThread                                 *thread,
    int32_t                                     vp_id,
    dm::IDataTypeFunction                       *method,
    vsc::dm::IModelField                        *method_ctxt,
    const std::vector<vsc::dm::ITypeExpr *>     &params) :
    EvalBase(ctxt, thread), m_vp_id(vp_id),
    m_method(method), m_method_ctxt(method_ctxt),
    m_method_ctxt_e(0), m_params(params.begin(), params.end()), 
    m_idx(0), m_param_idx(0) {
    DEBUG_INIT("zsp::arl::eval::EvalTypeMethodCallContext", thread->getDebugMgr());

}

EvalTypeMethodCallContext::EvalTypeMethodCallContext(
    IEvalContext                                *ctxt,
    IEvalThread                                 *thread,
    int32_t                                     vp_id,
    dm::IDataTypeFunction                       *method,
    vsc::dm::ITypeExpr                          *method_ctxt,
    const std::vector<vsc::dm::ITypeExpr *>     &params) :
    EvalBase(ctxt, thread), m_vp_id(vp_id), m_method(method), m_method_ctxt(0),
    m_method_ctxt_e(method_ctxt), m_params(params.begin(), params.end()), 
    m_idx(0), m_param_idx(0) {
    DEBUG_INIT("zsp::arl::eval::EvalTypeMethodCallContext", thread->getDebugMgr());

}

EvalTypeMethodCallContext::EvalTypeMethodCallContext(EvalTypeMethodCallContext *o) :
    EvalBase(o), m_vp_id(o->m_vp_id), m_method(o->m_method), 
    m_method_ctxt(o->m_method_ctxt),
    m_params(o->m_params.begin(), o->m_params.end()),
    m_idx(o->m_idx), m_param_idx(o->m_param_idx) {

}

EvalTypeMethodCallContext::~EvalTypeMethodCallContext() {

}

int32_t EvalTypeMethodCallContext::eval() {
    bool intf = m_method->getBody();
    int32_t ret = 0;
    DEBUG_ENTER("eval: m_idx=%d m_param_idx=%d", m_idx, m_param_idx);

    if (m_initial) {
        m_thread->pushEval(this);
        // Safety
        setFlags(EvalFlags::Complete);

        if (intf) {
            m_stack_frame = IEvalStackFrameUP(m_ctxt->mkStackFrame(m_params.size()+1));
            vsc::dm::ValRef method_h(m_ctxt->ctxt()->mkValRefRawPtr(m_method_ctxt));
            m_stack_frame->setVariable(0, method_h);
        } else {
            vsc::dm::ValRef method_h(m_ctxt->ctxt()->mkValRefRawPtr(m_method_ctxt));
            m_pvals.push_back(method_h);
        }
    }

    switch (m_idx) {
        case 0: {
            // If we've been invoked with an expression as the root context,
            // must resolve to a ModelField and determine whether this is
            // a register.
            m_idx = 1;
            if (m_method_ctxt_e) {

            }
        }
        /*
        case 0: { 
            // Determine whether a register-group field is on the context path
            // Check returns:
            // - Register-group handle (if applicable)
            // - Total offset of the register relative to the register-group handle
            // - Size (bits) of the register
            m_idx = 1;


        } 
         */
        case 1: {
            // Compute parameter values to pass to the call
            if (m_param_idx > 0 && hasFlags(EvalFlags::Complete)) {
                /*
                if (intf) {
                    m_stack_frame->setVariable(m_param_idx, moveResult());
                } else {
                    vsc::dm::ValRef pval(moveResult());
                    m_pvals.push_back(pval);
                }
                 */
            }
            while (m_param_idx < m_params.size()) {
                EvalTypeExpr evaluator(
                    m_ctxt, 
                    m_thread, 
                    m_vp_id,
                    m_params.at(m_param_idx));

                m_param_idx += 1;
                clrFlags(EvalFlags::Complete);
                if (evaluator.eval()) {
                    break;
                } else {
                    if (hasFlags(EvalFlags::Complete)) {
                        fprintf(stdout, "Note: push expr result\n");
                        /*
                        if (intf) {
                            m_stack_frame->setVariable(m_param_idx, moveResult());
                        } else {
                            vsc::dm::ValRef pval(moveResult());
                            m_pvals.push_back(pval);
                        }
                         */
                    }
                }
            }

            // If we left the loop before achieving enough
            // parameters, suspend...
            if (m_param_idx < m_params.size()) {
                break;
            }

            clrFlags(EvalFlags::Complete); // Clear 'safety' result

            m_idx = 2;

            // Determine *what* to call
            // - If there is a body, evaluate that

            // If we have register-call info, we need to:
            // - Obtain the current-executor handle to use as a context
            //   - This is where the ability to lookup method types from an object 
            //     handle becomes useful. We're an interpreter, so it would be
            //     useful to be able to check whether the target method is 
            //     internally or externally implemented.
            // - Obtain the appropriate access method based on register size
            // - Invoke the relevant target-context function
            if (m_method->getBody()) {
                DEBUG("Launching proc-body interpreter");

                /*
                // Push parameters
                m_thread->pushStackFrame(m_stack_frame.release());

                // TODO: push local-vars
                m_thread->pushStackFrame(m_ctxt->mkStackFrame(0));
                EvalTypeProcStmtScope(
                    m_ctxt,
                    m_thread,
                    m_vp_id,
                    m_method->getBody()).eval();
                 */
            } else {
                DEBUG("Calling import function");
                m_ctxt->getBackend()->callFuncReq(
                    m_thread,
                    m_method,
                    m_pvals
                );
            }

            if (!hasFlags(EvalFlags::Complete)) {
                break;
            }
        }

        /*
        case 1: {
            // Wait for a response
            if (hasFlags(EvalFlags::Complete)) {
                if (m_method->getBody()) {
                    m_thread->popStackFrame();
                    m_thread->popStackFrame();
                }
            }
        }
         */
    }

    ret = !hasFlags(EvalFlags::Complete);

    if (m_initial) {
        m_initial = false;
        if (hasFlags(EvalFlags::Complete)) {
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

dmgr::IDebug *EvalTypeMethodCallContext::m_dbg = 0;

}
}
}
