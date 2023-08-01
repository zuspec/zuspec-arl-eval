/**
 * EvalResult.h
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
#include "vsc/dm/IModelField.h"

namespace zsp {
namespace arl {
namespace eval {

enum class EvalResultKind {
    None,
    Void,
    Val,
    Ref,
    Break,
    Continue
};

union EvalResultVal {
    vsc::dm::IModelValUP    val;
    vsc::dm::IModelField    *ref;
};

class IEvalResult;
using IEvalResultUP=std::unique_ptr<IEvalResult>;
class IEvalResult : public virtual vsc::dm::IModelVal {
public:
    virtual EvalResultKind getKind() const = 0;

    virtual vsc::dm::IModelField *getRef() const = 0;

    EvalResultKind              kind;
    vsc::dm::IModelValUP        val;
    vsc::dm::IModelField        *ref;

};

} /* namespace eval */
} /* namespace arl */
} /* namespace zsp */


