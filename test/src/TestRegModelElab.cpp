/*
 * TestRegModelElab.cpp
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
#include "zsp/arl/dm/IModelFieldComponent.h"
#include "zsp/arl/dm/IModelFieldComponentRoot.h"
#include "zsp/arl/dm/impl/ModelBuildContext.h"
#include "TestRegModelElab.h"


namespace zsp {
namespace arl {
namespace eval {


TestRegModelElab::TestRegModelElab() {

}

TestRegModelElab::~TestRegModelElab() {

}

TEST_F(TestRegModelElab, smoke) {

    vsc::dm::ITypeExprFieldRef *ref;
    m_ctxt->getDebugMgr()->enable(true);

    // component pss_top {
    // }
    // 

    vsc::dm::IDataTypeInt *ui32_t = m_ctxt->mkDataTypeInt(false, 32);
    m_ctxt->addDataTypeInt(ui32_t);
    dm::IDataTypePackedStruct *MyReg_t = m_ctxt->mkDataTypePackedStruct("MyReg");
    vsc::dm::ITypeField *val = m_ctxt->mkTypeFieldPhy("val", ui32_t, false, vsc::dm::TypeFieldAttr::NoAttr, 0);
    MyReg_t->addField(val);
//    MyReg_t->addField(m_ctxt->mkTypeFieldPhy("val", ui32_t, false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    m_ctxt->addDataTypePackedStruct(MyReg_t);

    // struct MyReg : packed_s<> {
    //   bit[32] val;
    // }
    //
    // component MyRegs : reg_group_c {
    //   reg_c<MyReg>     r1;
    //   reg_c<MyReg>     r2;
    // }
    //
    // component pss_top {
    //   MyRegs          regs;
    // }
    //

    dm::IDataTypeComponent *MyRegs_t = m_ctxt->mkDataTypeComponent("MyRegs");
    MyRegs_t->addField(m_ctxt->mkTypeFieldReg("r1", MyReg_t, false));
    MyRegs_t->addField(m_ctxt->mkTypeFieldReg("r2", MyReg_t, false));
    m_ctxt->addDataTypeComponent(MyRegs_t);

    dm::IDataTypeComponent *pss_top_t = m_ctxt->mkDataTypeComponent("pss_top");
    pss_top_t->addField(m_ctxt->mkTypeFieldRegGroup("regs", MyRegs_t, false));

    vsc::dm::ITypeField *val1 = m_ctxt->mkTypeFieldPhy("val", ui32_t, false, vsc::dm::TypeFieldAttr::NoAttr, 0);
    pss_top_t->addField(val1);


    // entry_t
    // action entry_t {
    //  }
    dm::IDataTypeAction *entry_t = m_ctxt->mkDataTypeAction("entry_t");
    entry_t->setComponentType(pss_top_t);
    pss_top_t->addActionType(entry_t);

    vsc::solvers::IRandStateUP randstate(m_solvers_f->mkRandState(""));

    IEvalContextUP eval_ctxt(m_eval_f->mkEvalContextFullElab(
        m_solvers_f,
        m_ctxt.get(),
        randstate.get(),
        pss_top_t,
        entry_t,
        0
    ));

    eval_ctxt->eval();
}

}
}
}
