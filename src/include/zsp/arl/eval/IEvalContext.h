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
#include "pyapi-compat-if/IPyEval.h"
#include "vsc/dm/IModelValOp.h"
#include "zsp/arl/eval/IEval.h"
#include "zsp/arl/eval/IEvalBackend.h"
#include "zsp/arl/eval/IEvalListener.h"
#include "zsp/arl/eval/IEvalFunctionData.h"
#include "zsp/arl/eval/IEvalValProvider.h"
#include "zsp/arl/dm/IContext.h"
#include "zsp/arl/dm/IDataTypeFunction.h"
#include "zsp/arl/dm/IDataTypeFunctionImport.h"
#include "zsp/arl/dm/IModelFieldExecutor.h"

namespace zsp {
namespace arl {
namespace eval {

class IEvalResult;

enum class EvalContextFunc {
    Write8,
    Write16,
    Write32,
    Write64,
    Read8,
    Read16,
    Read32,
    Read64,
    RegWrite,
    RegWriteMasked,
    RegWriteVal,
    RegWriteValMasked,
    RegRead,
    RegReadVal,
    RegGroupSetHandle,
    Print,
    NumFunctions
};

class IEvalContext;
using IEvalContextUP=vsc::dm::UP<IEvalContext>;
class IEvalContext {
public:

    virtual ~IEvalContext() { }

    virtual int32_t eval() = 0;

    virtual dm::IContext *ctxt() const = 0;

    virtual const vsc::dm::ValRef &getResult() const = 0;

    virtual bool haveError() const = 0;

    virtual const std::string &getError() const = 0;

    virtual vsc::dm::ValRef &moveResult() = 0;

    virtual IEvalBackend *getBackend() const = 0;

    virtual void setBackend(IEvalBackend *b, bool owned=false) = 0;

    virtual dm::IModelFieldComponentRoot *getRootComponent() = 0;

    /**
     * Return just the global solve functions
     * 
     * Valid after construction
     */
    virtual const std::vector<dm::IDataTypeFunction *> &getSolveFunctions() const = 0;

    /**
     * Return just the target functions
     * 
     * Valid after construction
     */
    virtual const std::vector<dm::IDataTypeFunction *> &getTargetFunctions() const = 0;

    virtual void setFunctionData(
        dm::IDataTypeFunction       *func_t,
        IEvalFunctionData           *data
    ) = 0;

    virtual void addListener(IEvalListener *l) = 0;

    virtual void callListener(const std::function<void (IEvalListener *)> &f) = 0;

    virtual dm::IDataTypeFunction *getFunction(EvalContextFunc func) = 0;

    virtual vsc::dm::ValRefInt mkValRefInt(
        int64_t value, 
        bool is_signed, 
        int32_t width) = 0;

    virtual vsc::dm::IModelVal *mkModelValS(int64_t v=0, int32_t w=32) = 0;

    virtual vsc::dm::IModelVal *mkModelValU(uint64_t v=0, int32_t w=32) = 0;

    virtual vsc::dm::IModelValOp *getModelValOp() = 0;

    virtual IEvalStackFrame *mkStackFrame(int32_t n_vars) = 0;

    virtual pyapi::PyEvalObj *getPyModule(dm::IPyImport *imp) = 0;

    virtual pyapi::IPyEval *getPyEval() = 0;

    virtual dmgr::IDebugMgr *getDebugMgr() const = 0;

};

} /* namespace eval */
} /* namespace arl */
} /* namespace zsp */


