/**
 * TaskConvertImportFuncVal.h
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
#include "zsp/arl/dm/impl/VisitorBase.h"

namespace zsp {
namespace arl {
namespace eval {



class TaskConvertImportFuncVal : public dm::VisitorBase {
public:
    TaskConvertImportFuncVal(IEvalContext *ctxt);

    virtual ~TaskConvertImportFuncVal();

    vsc::dm::ValRef convert(const vsc::dm::ValRef &val);

	virtual void visitDataTypeAddrHandle(dm::IDataTypeAddrHandle *t) override;

	virtual void visitDataTypeStruct(vsc::dm::IDataTypeStruct *t) override;

private:
    vsc::dm::ValRef             m_val;
    IEvalContext                *m_ctxt;

};

}
}
}


