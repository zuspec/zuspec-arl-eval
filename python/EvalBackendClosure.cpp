/*
 * EvalBackendClosure.cpp
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
#include <memory>
#include <vector>
#include "zsp/arl/dm/IDataTypeFunction.h"
#include "zsp/arl/dm/IModelFieldAction.h"
#include "zsp/arl/eval/IEvalThread.h"
typedef zsp::arl::eval::IEvalThread *__pyx_t_12zsp_arl_eval_4decl_IEvalThreadP;
//typedef std::unique_ptr<zsp::arl::eval::IEvalResult> __pyx_t_12zsp_arl_eval_4decl_IEvalResultUP;
#include "core.h"
#include "EvalBackendClosure.h"


namespace zsp {
namespace arl {
namespace eval {


EvalBackendClosure::EvalBackendClosure(PyObject *peer) : m_peer(peer) {
    Py_INCREF(m_peer);
}

EvalBackendClosure::~EvalBackendClosure() {
    Py_DECREF(m_peer);
}

void EvalBackendClosure::enterThreads(
        const std::vector<IEvalThread *>    &threads) { 
    EvalBackendClosure_enterThreads(m_peer, threads);
}
    
void EvalBackendClosure::enterThread(IEvalThread *thread) { 
    EvalBackendClosure_enterThread(m_peer, thread);
}

void EvalBackendClosure::leaveThread(IEvalThread *thread) { 
    EvalBackendClosure_leaveThread(m_peer, thread);
}

void EvalBackendClosure::leaveThreads(
        const std::vector<IEvalThread *>    &threads) { 
    EvalBackendClosure_leaveThreads(m_peer, threads);
}

void EvalBackendClosure::enterAction(
        IEvalThread                         *thread,
        dm::IModelFieldAction               *action) {
    EvalBackendClosure_enterAction(m_peer, thread, action);
}

void EvalBackendClosure::leaveAction(
        IEvalThread                         *thread,
        dm::IModelFieldAction               *action) {
    EvalBackendClosure_leaveAction(m_peer, thread, action);
}

void EvalBackendClosure::callFuncReq(
            IEvalThread                         *thread,
            dm::IDataTypeFunction               *func_t,
            const std::vector<vsc::dm::ValRef>  &params) { 
    EvalBackendClosure_callFuncReq(m_peer, thread, func_t, params);
}

}
}
}
