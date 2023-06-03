/**
 * IEvalContext.h
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
#include "zsp/arl/eval/IEvalBackend.h"
#include "zsp/arl/eval/IEvalThread.h"
#include "zsp/arl/eval/IEvalFunctionData.h"
#include "zsp/arl/dm/IDataTypeFunction.h"
#include "zsp/arl/dm/IDataTypeFunctionImport.h"

namespace zsp {
namespace arl {
namespace eval {

class IEvalContext;
using IEvalContextUP=vsc::dm::UP<IEvalContext>;
class IEvalContext : public virtual IEvalThread {
public:

    virtual ~IEvalContext() { }

    virtual IEvalBackend *getBackend() const = 0;

    virtual void setBackend(IEvalBackend *b) = 0;

    /**
     * @brief Return functions
     * 
     * @return const std::vector<dm::IDataTypeFunction *>& 
     */
    virtual const std::vector<dm::IDataTypeFunction *> &getFunctions() const = 0;

    virtual void setFunctionData(
        dm::IDataTypeFunction       *func_t,
        IEvalFunctionData           *data
    ) = 0;

};

} /* namespace eval */
} /* namespace arl */
} /* namespace zsp */


