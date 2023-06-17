/*
 * EvalBackendTestFixture.cpp
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
#include "EvalBackendTestFixture.h"


namespace zsp {
namespace arl {
namespace eval {


EvalBackendTestFixture::EvalBackendTestFixture() {

}

EvalBackendTestFixture::~EvalBackendTestFixture() {

}

void EvalBackendTestFixture::enterThreads(
    const std::vector<IEvalThread *>    &threads) {

}
    
void EvalBackendTestFixture::enterThread(IEvalThread *thread) {

}

void EvalBackendTestFixture::leaveThread(IEvalThread *thread) {

}

void EvalBackendTestFixture::leaveThreads(
    const std::vector<IEvalThread *>    &threads) {

}

void EvalBackendTestFixture::callFuncReq(
    IEvalThread                     *thread,
    dm::IDataTypeFunction           *func_t,
    const std::vector<EvalResult>   &params) {
    m_func_calls.push_back({thread,func_t});
    if (m_call_req) {
        m_call_req(thread, func_t, params);
    } else {
        // In the absence of a handler, respond as if it is a void non-blocking return
        thread->setResult(EvalResult::Void());
    }
}

}
}
}
