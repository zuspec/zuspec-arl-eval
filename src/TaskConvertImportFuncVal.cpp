/*
 * TaskConvertImportFuncVal.cpp
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
#include "TaskConvertImportFuncVal.h"
#include "ModelAddrHandle.h"


namespace zsp {
namespace arl {
namespace eval {


TaskConvertImportFuncVal::TaskConvertImportFuncVal(IEvalContext *ctxt) :
    m_ctxt(ctxt) {

}

TaskConvertImportFuncVal::~TaskConvertImportFuncVal() {

}

vsc::dm::ValRef TaskConvertImportFuncVal::convert(const vsc::dm::ValRef &val) {
    m_val = val;
    val.type()->accept(m_this);
    return m_val;
}

void TaskConvertImportFuncVal::visitDataTypeAddrHandle(dm::IDataTypeAddrHandle *t) {
    vsc::dm::ValRefStruct addr_h_s(m_val);
    vsc::dm::ValRefPtr addr_h_p(addr_h_s.getFieldRef(-1));
    ModelAddrHandle *addr_hndl = addr_h_p.get_valT<ModelAddrHandle>();

    m_val = m_ctxt->ctxt()->mkValRefInt(
        addr_hndl->getAddr(),
        false,
        64);
}

void TaskConvertImportFuncVal::visitDataTypeStruct(vsc::dm::IDataTypeStruct *t) {
    // No conversion
}

}
}
}
