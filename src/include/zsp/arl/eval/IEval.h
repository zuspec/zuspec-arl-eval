/**
 * IEval.h
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
#include "vsc/dm/IModelVal.h"
#include "vsc/dm/impl/UP.h"

namespace zsp {
namespace arl {
namespace eval {

class IEval;
using IEvalUP=vsc::dm::UP<IEval>;
class IEval {
public:

    virtual ~IEval() { }

    /**
     * @brief Performs some work. 
     * 
     * @return true - if more work remains
     * @return false - if work is complete
     */
    virtual bool eval() = 0;

    virtual int32_t getIdx() = 0;

    virtual void setIdx(int32_t idx) = 0;

    virtual IEval *clone() = 0;

    /**
     * @brief Indicates if the thread is blocked
     * 
     * @return true - Thread is waiting for an event
     * @return false 
     */
    virtual bool isBlocked() = 0;

    virtual vsc::dm::IModelVal *getResult() = 0;

    virtual vsc::dm::IModelVal *moveResult() = 0;

    virtual void setResult(vsc::dm::IModelVal *val) = 0;

    virtual bool haveResult() const = 0;

};

} /* namespace eval */
} /* namespace arl */
} /* namespace zsp */


