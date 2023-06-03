/*
 * EvalActivityFullElab.cpp
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
#include "EvalActivityFullElab.h"


namespace zsp {
namespace arl {
namespace eval {


EvalActivityFullElab::EvalActivityFullElab(
    IEvalContext        *ctxt,
    IEvalThread         *thread,
    dm::IModelActivity  *activity) : 
        EvalBase(ctxt, thread), m_activity(activity) {

}

EvalActivityFullElab::~EvalActivityFullElab() {

}

bool EvalActivityFullElab::eval() {

}

IEval *EvalActivityFullElab::clone() {
    return new EvalActivityFullElab(m_ctxt, m_thread, m_activity);
}

bool EvalActivityFullElab::isBlocked() {

}

}
}
}
