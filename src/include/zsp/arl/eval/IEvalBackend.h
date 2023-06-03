/**
 * IEvalBackend.h
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
#include <vector>
#include "zsp/arl/eval/IEvalThread.h"
#include "zsp/arl/eval/IEvalThreadId.h"
#include "zsp/arl/dm/IDataTypeFunction.h"

namespace zsp {
namespace arl {
namespace eval {



class IEvalBackend {
public:

    virtual ~IEvalBackend() { }

    /**
     * @brief Start
     * 
     * @param threads 
     */
    virtual void startThreads(
        const std::vector<IEvalThread *>    &threads) = 0;
    
    virtual void startThread(IEvalThread *thread) = 0;

    virtual void endThread(IEvalThread *thread) = 0;

    virtual void endThreads(
        const std::vector<IEvalThread *>    &threads) = 0;

    virtual void callFuncReq(
            IEvalThread                 *thread,
            dm::IDataTypeFunction       *func_t
    ) = 0;

};

} /* namespace eval */
} /* namespace arl */
} /* namespace zsp */


