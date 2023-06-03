/*
 * EvalContextFullElab.cpp
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
#include "EvalActivityScopeFullElab.h"
#include "EvalContextFullElab.h"


namespace zsp {
namespace arl {
namespace eval {


EvalContextFullElab::EvalContextFullElab(ElabActivity *activity) :
    m_initial(true) {
}

EvalContextFullElab::~EvalContextFullElab() {

}

bool EvalContextFullElab::eval() {
    bool ret = false;

    if (m_initial) {
        // Add an eval scope for the top-level activity
        EvalActivityScopeFullElab evaluator(
            this,
            this,
            m_activity->activity_s.back().get()
        );

        pushEval(&evaluator);
        if ((ret=evaluator.eval())) {
            suspendEval(&evaluator);
        } else {
            popEval(&evaluator);
        }

        m_initial = false;
    } else {
        while (m_eval_s.size() 
            && !(ret=m_eval_s.back()->eval())) {
            m_eval_s.pop_back();
        }
    }

    return ret;
}

bool EvalContextFullElab::isBlocked() {
    return (m_subthreads.size() 
        && m_subthreads.back()->isBlocked());
}





}
}
}
