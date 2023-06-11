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
#include "zsp/arl/eval/EvalResult.h"

namespace zsp {
namespace arl {
namespace eval {

class EvalStackFrame : public virtual IEvalStackFrame {
public:
    EvalStackFrame();

    virtual ~EvalStackFrame();

    virtual vsc::dm::IModelField *getVariable(uint32_t idx) override {
        return m_variables.at(idx).get();
    }

    virtual void addVariable(vsc::dm::IModelField *var) override {
        m_variables.push_back(vsc::dm::IModelFieldUP(var));
    }

    virtual const std::vector<vsc::dm::IModelFieldUP> &getVariables() const override {
        return m_variables;
    }

private:
    std::vector<EvalResult>                     m_parameters;
    std::vector<vsc::dm::IModelFieldUP>         m_variables;

};

}
}
}


