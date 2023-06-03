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
    doit->addImportSpec(m_ctxt->mkDataTypeFunctionImport(""));

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
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a1", action1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a2", action1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
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

    dm::ModelBuildContext build_ctxt(m_ctxt.get());
    dm::IModelFieldComponentRootUP pss_top(pss_top_t->mkRootFieldT<dm::IModelFieldComponentRoot>(
        &build_ctxt,
        "pss_top", 
        false));

    pss_top->initCompTree();

    vsc::solvers::IRandStateUP randstate(m_solvers_f->mkRandState(""));

    IEvalContextUP eval_ctxt(m_eval_f->mkEvalContextFullElab(
        m_solvers_f,
        m_ctxt.get(),
        randstate.get(),
        pss_top.get(),
        entry_t.get(),
        0
    ));
}

}
}
}
