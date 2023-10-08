/*
 * TestEvalContextFuncCall.cpp
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
#include "TestEvalContextFuncCall.h"


namespace zsp {
namespace arl {
namespace eval {


TestEvalContextFuncCall::TestEvalContextFuncCall() {

}

TestEvalContextFuncCall::~TestEvalContextFuncCall() {

}

TEST_F(TestEvalContextFuncCall, func_identified) {
    vsc::dm::ITypeExprFieldRef *ref;
    m_ctxt->getDebugMgr()->enable(true);

    // Import function
    dm::IDataTypeFunctionUP doit(m_ctxt->mkDataTypeFunction(
        "doit",
        0,
        false));
    doit->addImportSpec(m_ctxt->mkDataTypeFunctionImport("", false, false));

    // component pss_top {
    // }
    // 
    dm::IDataTypeComponentUP pss_top_t(m_ctxt->mkDataTypeComponent("pss_top"));

    // action_t
    // action action_t {
    //
    //   exec body {
    //     doit();
    //   }
    // }
    //

    dm::IDataTypeActionUP action1_t(m_ctxt->mkDataTypeAction("action1_t"));
    action1_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(action1_t.get());

    // entry_t
    // action entry_t {
    //   action_t a1, a2;
    //   activity {
    //     a1;
    //     a2;
    //  }
    dm::IDataTypeActionUP entry_t(m_ctxt->mkDataTypeAction("entry_t"));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a1", action1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, vsc::dm::ValRef()));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a2", action1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, vsc::dm::ValRef()));
    entry_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(entry_t.get());

    dm::IDataTypeActivitySequence *activity_root = m_ctxt->mkDataTypeActivitySequence();
    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(1);
    activity_root->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(2);
    activity_root->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    entry_t->addActivity(m_ctxt->mkTypeFieldActivity(
        "activity", 
        activity_root,
        true));

    vsc::solvers::IRandStateUP randstate(m_solvers_f->mkRandState(""));

    IEvalContextUP eval_ctxt(m_eval_f->mkEvalContextFullElab(
        m_solvers_f,
        m_ctxt.get(),
        randstate.get(),
        pss_top_t.get(),
        entry_t.get(),
        0
    ));
}

TEST_F(TestEvalContextFuncCall, stmt_incr_field) {
    enableDebug(true);

    vsc::dm::IDataTypeIntUP i32_t(m_ctxt->mkDataTypeInt(true, 32));
    vsc::dm::ValRef val(m_ctxt->mkValRefInt(15, true, 32));

    vsc::dm::IDataTypeStruct *dt = m_ctxt->mkDataTypeStruct("dt");
    dt->addField(m_ctxt->mkTypeFieldPhy("a", i32_t.get(), false, 
        vsc::dm::TypeFieldAttr::NoAttr, vsc::dm::ValRef()));
    m_ctxt->addDataTypeStruct(dt);

    dm::IDataTypeFunctionUP func(mkFunction(
        "func",
        i32_t.get(),
        m_ctxt->mkTypeProcStmtAssign(
            m_ctxt->mkTypeExprFieldRef(
                vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope,
                1,
                {0, 0}),
            dm::TypeProcStmtAssignOp::Eq,
            m_ctxt->mkTypeExprVal(i32_t.get(), val.vp())
        ))
    );

    vsc::dm::ModelBuildContext build_ctxt(m_ctxt.get());
    vsc::dm::IModelFieldUP obj(dt->mkRootField(&build_ctxt, "obj", false));

    IEvalContextUP eval_c(m_eval_f->mkEvalContextFunctionContext(
        m_solvers_f,
        m_ctxt.get(),
        m_randstate.get(),
        0, // &backend,
        func.get(),
        obj.get(),
        {}
    ));

    ASSERT_EQ(eval_c->eval(), 0);
//    ASSERT_EQ(eval_c->haveResult());
    vsc::dm::ValRef result(eval_c->getResult());
//    ASSERT_EQ(result->getKind(), EvalResultKind::Void);
//    ASSERT_EQ(obj->getField(0)->val()->val_i(), 15);
}

TEST_F(TestEvalContextFuncCall, eval_check_reg_access) {
    enableDebug(true);

    vsc::dm::ValRef val(m_ctxt->mkValRefInt(15, true, 32));
    vsc::dm::IDataTypeIntUP i32_t(m_ctxt->mkDataTypeInt(true, 32));

    // Need a register group containing a register field
    dm::IDataTypeComponent *reg_group_t = m_ctxt->mkDataTypeComponent("reg_group_t");

    // Build placeholder read/write functions
    reg_group_t->addFunction(
        m_ctxt->mkDataTypeFunction("read32", i32_t.get(), false, 
        dm::DataTypeFunctionFlags::Target));
    reg_group_t->addFunction(
        m_ctxt->mkDataTypeFunction("write32", i32_t.get(), false, 
        dm::DataTypeFunctionFlags::Target));

    reg_group_t->addField(
        m_ctxt->mkTypeFieldReg("reg1", i32_t.get(), false));

//    dm::ITypeFieldRegGroup *reg_group_t = m_ctxt->mkTypeFieldRegGroup(
    vsc::dm::ModelBuildContext build_ctxt(m_ctxt.get());
    vsc::dm::IModelField *reg_group_f = reg_group_t->mkRootField(
        &build_ctxt,
        "root",
        false
    );

#ifdef UNDEFINED
    m_eval_f->mkEvalContextFunctionContext(
        m_solvers_f,
        m_ctxt,
        m_randstate.get(),
        0,
        {}
    )
    m_ctxt->mkTypeExprMethodCallContext(

    )
#endif

    vsc::dm::IDataTypeStruct *dt = m_ctxt->mkDataTypeStruct("dt");
    dt->addField(m_ctxt->mkTypeFieldPhy("a", i32_t.get(), false, 
        vsc::dm::TypeFieldAttr::NoAttr, vsc::dm::ValRef()));
    m_ctxt->addDataTypeStruct(dt);

    dm::IDataTypeFunctionUP func(mkFunction(
        "func",
        i32_t.get(),
        m_ctxt->mkTypeProcStmtAssign(
            m_ctxt->mkTypeExprFieldRef(
                vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope,
                1,
                {0, 0}),
            dm::TypeProcStmtAssignOp::Eq,
            m_ctxt->mkTypeExprVal(i32_t.get(), val.vp())
        ))
    );

//    vsc::dm::ModelBuildContext build_ctxt(m_ctxt.get());
    vsc::dm::IModelFieldUP obj(dt->mkRootField(&build_ctxt, "obj", false));

    IEvalContextUP eval_c(m_eval_f->mkEvalContextFunctionContext(
        m_solvers_f,
        m_ctxt.get(),
        m_randstate.get(),
        0, // &backend,
        func.get(),
        obj.get(),
        {

        }
    ));

    ASSERT_EQ(eval_c->eval(), 0);
//    ASSERT_EQ(eval_c->haveResult());
    vsc::dm::ValRef result(eval_c->getResult());
    fprintf(stdout, "result: %p\n", result);
//    ASSERT_EQ(result->getKind(), EvalResultKind::Void);
//    ASSERT_EQ(obj->getField(0)->val()->val_i(), 15);
}


}
}
}
