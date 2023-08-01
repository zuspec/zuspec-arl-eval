/*
 * EvalStackFrame.cpp
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
#include "EvalStackFrame.h"


namespace zsp {
namespace arl {
namespace eval {


EvalStackFrame::EvalStackFrame(int32_t num_variables) : m_num_variables(num_variables) {

}

EvalStackFrame::~EvalStackFrame() {

}

void *EvalStackFrame::operator new(size_t size, int32_t n_vars) {
    if (n_vars > 1) {
        size += (sizeof(IEvalResultUP)*(n_vars-1));
    }
    return ::operator new(size);
}

}
}
}
