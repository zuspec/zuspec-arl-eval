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
#include "vsc/dm/impl/UP.h"
#include "zsp/arl/eval/IEval.h"
#include "zsp/arl/eval/IEvalStackFrame.h"
#include "zsp/arl/eval/IEvalThreadId.h"

namespace zsp {
namespace arl {
namespace eval {

class IEvalThread;
using IEvalThreadUP=vsc::dm::UP<IEvalThread>;
class IEvalThread : public virtual IEval {
public:

    virtual ~IEvalThread() { }

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

    virtual IEvalStackFrame *stackFrame() = 0;

    virtual void popStackFrame() = 0;

};

} /* namespace eval */
} /* namespace arl */
} /* namespace zsp */


