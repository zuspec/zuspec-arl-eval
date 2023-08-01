/**
 * EvalStackFrame.h
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
#include "zsp/arl/eval/IEvalStackFrame.h"
#include "zsp/arl/eval/IEvalResult.h"

namespace zsp {
namespace arl {
namespace eval {

struct EvalStackFrame : public virtual IEvalStackFrame {
public:
    EvalStackFrame(int32_t num_variables);

    virtual ~EvalStackFrame();

    void *operator new(size_t size, int32_t n_vars);

    virtual IEvalResult *getVariable(uint32_t idx) override {
        return m_variables[idx].get();
    }

    virtual void setVariable(uint32_t idx, IEvalResult *var) override {
        m_variables[idx] = IEvalResultUP(var);
    }

    virtual int32_t getNumVariables() override {
        return m_num_variables;
    }

private:
    int32_t                                     m_num_variables;
    IEvalResultUP                               m_variables[1];

};

}
}
}


