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
#include "vsc/dm/impl/ValRef.h"
#include "vsc/dm/impl/UP.h"
#include "zsp/arl/eval/IEvalResult.h"

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
    virtual int32_t eval() = 0;

    virtual int32_t getIdx() = 0;

    virtual void setIdx(int32_t idx) = 0;

    virtual IEval *clone() = 0;

    virtual const vsc::dm::ValRef &getResult() const = 0;

    /**
     * @brief Moves the result, clearing the result here
     * 
     * @return EvalResult& 
     */
    virtual vsc::dm::ValRef &moveResult() = 0;

    /**
     * @brief Sets the result, taking ownership of value data
     * 
     * @param r 
     */
    virtual void setResult(vsc::dm::ValRef &r) = 0;

    virtual void setVoidResult() = 0;

    virtual void clrResult() = 0;

    virtual bool haveResult() const = 0;

};

} /* namespace eval */
} /* namespace arl */
} /* namespace zsp */


