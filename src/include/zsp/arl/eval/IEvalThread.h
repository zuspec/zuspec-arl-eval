/**
 * IEvalThread.h
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
#include "dmgr/IDebugMgr.h"
#include "vsc/dm/impl/UP.h"
#include "zsp/arl/dm/IDataTypeFunction.h"
#include "zsp/arl/eval/IEval.h"
#include "zsp/arl/eval/IEvalStackFrame.h"
#include "zsp/arl/eval/IEvalThreadId.h"

namespace zsp {
namespace arl {
namespace eval {

class IEvalBackend;

class IEvalThread;
using IEvalThreadUP=vsc::dm::UP<IEvalThread>;
class IEvalThread : public virtual IEval {
public:

    virtual ~IEvalThread() { }

    virtual IEvalBackend *getBackend() const = 0;

    /**
     * @brief Push an eval-stack entry
     */
    virtual void pushEval(IEval *e, bool owned=false) = 0;

    /**
     * @brief Convert eval-stack entry to a suspended entry
     */
    virtual void suspendEval(IEval *e) = 0;

    virtual void popEval(IEval *e) = 0;

    virtual IEvalThreadId *getThreadId() const = 0;

    virtual void setThreadId(IEvalThreadId *tid) = 0;

    virtual void pushStackFrame(IEvalStackFrame *frame) = 0;

    virtual IEvalStackFrame *stackFrame(int32_t idx=0) = 0;

    virtual void popStackFrame() = 0;

    /**
     * @brief Initiates a method call using this thread as context.
     *        If the return is '0', then the result is immediately
     *        available. Otherwise, subsequent calls to eval must
     *        be made.
     * 
     * @param method 
     * @param method_ctxt 
     * @param params 
     * @return int32_t 
     */
    virtual int32_t evalMethodCallContext(
        dm::IDataTypeFunction                   *method,
        vsc::dm::IModelField                    *method_ctxt,
        const std::vector<vsc::dm::ITypeExpr *> &params) =  0;

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


