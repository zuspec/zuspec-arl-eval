/**
 * BuiltinFuncInfo.h
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
#include "zsp/arl/eval/IBuiltinFuncInfo.h"

namespace zsp {
namespace arl {
namespace eval {



class BuiltinFuncInfo : public virtual IBuiltinFuncInfo {
public:
    BuiltinFuncInfo(
        const FuncT             &impl,
        BuiltinFuncFlags        flags=BuiltinFuncFlags::NoFlags);

    virtual ~BuiltinFuncInfo();

    virtual const FuncT &getImpl() const override {
        return m_impl;
    }

    virtual BuiltinFuncFlags getFlags() const override {
        return m_flags;
    }

    virtual bool hasFlags(BuiltinFuncFlags flags) const override {
        return ((m_flags & flags) != BuiltinFuncFlags::NoFlags);
    }


private:
    FuncT                       m_impl;
    BuiltinFuncFlags            m_flags;

};

}
}
}


