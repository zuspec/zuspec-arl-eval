/**
 * EvalBackendTestFixture.h
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
#pragma once
#include <functional>
#include "zsp/arl/eval/impl/EvalBackendBase.h"

namespace zsp {
namespace arl {
namespace eval {



class EvalBackendTestFixture : public virtual EvalBackendBase {
public:
    EvalBackendTestFixture();

    virtual ~EvalBackendTestFixture();

    virtual void enterThreads(
        const std::vector<IEvalThread *>    &threads) override;
    
    virtual void enterThread(IEvalThread *thread) override;

    virtual void leaveThread(IEvalThread *thread) override;

    virtual void leaveThreads(
        const std::vector<IEvalThread *>    &threads) override;

    virtual void callFuncReq(
            IEvalThread                     *thread,
            dm::IDataTypeFunction           *func_t,
            const std::vector<EvalResult>   &params) override;

    void setCallReq(const std::function<void(IEvalThread*, dm::IDataTypeFunction*,const std::vector<EvalResult> &)> &f) {
        m_call_req = f;
    } 

    const std::vector<std::pair<IEvalThread*,dm::IDataTypeFunction *>> &getFuncCalls() const {
        return m_func_calls;
    }

protected:
    std::function<void(IEvalThread *, dm::IDataTypeFunction *,const std::vector<EvalResult>&)>     m_call_req;
    std::vector<std::pair<IEvalThread*,dm::IDataTypeFunction *>>    m_func_calls;

};

}
}
}


