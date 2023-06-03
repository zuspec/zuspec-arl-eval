/**
 * EvalContextFullElab.h
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
#include "zsp/arl/eval/IEvalContext.h"
#include "TaskElaborateActivity.h"
#include "EvalThread.h"

namespace zsp {
namespace arl {
namespace eval {


class EvalContextFullElab :
    public virtual IEvalContext,
    public virtual EvalThread {
public:
    EvalContextFullElab(ElabActivity *activity);

    virtual ~EvalContextFullElab();

    virtual bool eval() override;

    virtual bool isBlocked() override;

private:
    bool                            m_initial;
    ElabActivityUP                  m_activity;
    std::vector<IEvalThreadUP>      m_subthreads;

};

}
}
}


