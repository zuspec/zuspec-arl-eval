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
#include <functional>
#include <vector>
#include "dmgr/IDebugMgr.h"
#include "vsc/dm/IModelValOp.h"
#include "zsp/arl/eval/IEval.h"
#include "zsp/arl/eval/IEvalBackend.h"
#include "zsp/arl/eval/IEvalListener.h"
#include "zsp/arl/eval/IEvalFunctionData.h"
#include "zsp/arl/eval/IEvalResult.h"
#include "zsp/arl/dm/IDataTypeFunction.h"
#include "zsp/arl/dm/IDataTypeFunctionImport.h"
#include "zsp/arl/dm/IModelFieldExecutor.h"

namespace zsp {
namespace arl {
namespace eval {

class IEvalResult;

class IEvalContext;
using IEvalContextUP=vsc::dm::UP<IEvalContext>;
class IEvalContext {
public:

    virtual ~IEvalContext() { }

    virtual bool eval() = 0;

    virtual IEvalBackend *getBackend() const = 0;

    virtual void setBackend(IEvalBackend *b) = 0;

    /**
     * @brief Return functions
     * 
     * @return const std::vector<dm::IDataTypeFunction *>& 
     */
    virtual const std::vector<dm::IDataTypeFunction *> &getFunctions() const = 0;

    virtual const std::vector<dm::IModelFieldExecutor *> &getExecutors() const = 0;

    virtual void setFunctionData(
        dm::IDataTypeFunction       *func_t,
        IEvalFunctionData           *data
    ) = 0;

    virtual void addListener(IEvalListener *l) = 0;

    virtual void callListener(const std::function<void (IEvalListener *)> &f) = 0;

    virtual vsc::dm::IModelVal *mkModelValS(int64_t v=0, int32_t w=32) = 0;

    virtual vsc::dm::IModelVal *mkModelValU(uint64_t v=0, int32_t w=32) = 0;

    virtual vsc::dm::IModelValOp *getModelValOp() = 0;

    virtual IEvalResult *mkEvalResultVal(const vsc::dm::IModelVal *val) = 0;

    virtual IEvalResult *mkEvalResultValS(int64_t val, int32_t bits) = 0;

    virtual IEvalResult *mkEvalResultValU(uint64_t val, int32_t bits) = 0;

    virtual IEvalResult *mkEvalResultKind(EvalResultKind kind) = 0;

    virtual IEvalResult *mkEvalResultRef(vsc::dm::IModelField *ref) = 0;

    virtual dmgr::IDebugMgr *getDebugMgr() const = 0;

};

} /* namespace eval */
} /* namespace arl */
} /* namespace zsp */


