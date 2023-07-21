/*
 * TestEvalContextFullElab.cpp
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
#include "dmgr/impl/DebugMacros.h"
#include "TestEvalContextFullElab.h"
#include "CollectingEvalListener.h"
#include "EvalBackendTestFixture.h"
#include "zsp/arl/dm/IModelFieldComponent.h"
#include "zsp/arl/dm/IModelFieldComponentRoot.h"
#include "zsp/arl/dm/impl/ModelBuildContext.h"
#include "zsp/arl/eval/impl/EvalBackendBase.h"

namespace zsp {
namespace arl {
namespace eval {


TestEvalContextFullElab::TestEvalContextFullElab() {

}

TestEvalContextFullElab::~TestEvalContextFullElab() {

}

TEST_F(TestEvalContextFullElab, two_action_seq) {
    vsc::dm::ITypeExprFieldRef *ref;
    m_ctxt->getDebugMgr()->enable(true);

    // component pss_top {
    // }
    // 
    dm::IDataTypeComponentUP pss_top_t(m_ctxt->mkDataTypeComponent("pss_top"));

    // action_t
    // action action_t {
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

    vsc::solvers::IRandStateUP randstate(m_solvers_f->mkRandState(""));

    EvalBackendTestFixture backend;
    IEvalContextUP eval_ctxt(m_eval_f->mkEvalContextFullElab(
        m_solvers_f,
        m_ctxt.get(),
        randstate.get(),
        pss_top_t.get(),
        entry_t.get(),
        &backend
    ));

    CollectingEvalListener listener;
    eval_ctxt->addListener(&listener);

    while (eval_ctxt->eval()) {
        ;
    }

    ASSERT_EQ(listener.getActions().size(), 3);
}



TEST_F(TestEvalContextFullElab, two_action_seq_exec_func_b) {
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
    //   exec body {
    //     doit();
    //   }
    // }
    //

    dm::IDataTypeActionUP action1_t(m_ctxt->mkDataTypeAction("action1_t"));
    dm::ITypeProcStmtScopeUP exec_body_b(m_ctxt->mkTypeProcStmtScope());
    exec_body_b->addStatement(m_ctxt->mkTypeProcStmtExpr(
        m_ctxt->mkTypeExprMethodCallStatic(doit.get(), {})
    ));
    dm::ITypeExecUP exec_body(m_ctxt->mkTypeExecProc(
        dm::ExecKindT::Body,
        exec_body_b.release()));
    action1_t->addExec(exec_body.release());

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

    EvalBackendTestFixture backend;

    // Don't respond to call request
    backend.setCallReq([](IEvalThread*,dm::IDataTypeFunction*,const std::vector<IEvalResultUP>&) {});

    IEvalContextUP eval_ctxt(m_eval_f->mkEvalContextFullElab(
        m_solvers_f,
        m_ctxt.get(),
        randstate.get(),
        pss_top_t.get(),
        entry_t.get(),
        &backend
    ));
    CollectingEvalListener listener;
    eval_ctxt->addListener(&listener);

    // First call should block
    ASSERT_TRUE(eval_ctxt->eval());

    // Should have just one call request
    ASSERT_EQ(backend.getFuncCalls().size(), 1);

    // Send a response
    DEBUG_ENTER("setResult");
    backend.getFuncCalls().back().first->setResult(
        eval_ctxt->mkEvalResultKind(EvalResultKind::Void));
    DEBUG_LEAVE("setResult");

    // Should terminate this time
    ASSERT_TRUE(eval_ctxt->eval());

    ASSERT_EQ(backend.getFuncCalls().size(), 2);

    DEBUG_ENTER("setResult");
    backend.getFuncCalls().back().first->setResult(
        eval_ctxt->mkEvalResultKind(EvalResultKind::Void));
    DEBUG_LEAVE("setResult");

    ASSERT_FALSE(eval_ctxt->eval());

    ASSERT_EQ(listener.getActions().size(), 3);
}

TEST_F(TestEvalContextFullElab, two_action_par_exec_func_nb) {
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
    //   exec body {
    //     doit();
    //   }
    // }
    //

    dm::IDataTypeActionUP action1_t(m_ctxt->mkDataTypeAction("action1_t"));
    dm::ITypeProcStmtScopeUP exec_body_b(m_ctxt->mkTypeProcStmtScope());
    exec_body_b->addStatement(m_ctxt->mkTypeProcStmtExpr(
        m_ctxt->mkTypeExprMethodCallStatic(doit.get(), {})
    ));
    dm::ITypeExecUP exec_body(m_ctxt->mkTypeExecProc(
        dm::ExecKindT::Body,
        exec_body_b.release()));
    action1_t->addExec(exec_body.release());

    action1_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(action1_t.get());

    // entry_t
    // action entry_t {
    //   action_t a1, a2;
    //   activity {
    //     parallel {
    //       a1;
    //       a2;
    //     }
    //  }
    dm::IDataTypeActionUP entry_t(m_ctxt->mkDataTypeAction("entry_t"));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a1", action1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a2", action1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    entry_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(entry_t.get());

    dm::IDataTypeActivitySequence *activity_root = m_ctxt->mkDataTypeActivitySequence();
    dm::IDataTypeActivityParallel *activity_par = m_ctxt->mkDataTypeActivityParallel();
    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(1);
    activity_par->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(2);
    activity_par->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    entry_t->addActivity(m_ctxt->mkTypeFieldActivity(
        "activity", 
        activity_par,
        true));

    vsc::solvers::IRandStateUP randstate(m_solvers_f->mkRandState(""));

    EvalBackendTestFixture backend;
    IEvalContextUP eval_ctxt(m_eval_f->mkEvalContextFullElab(
        m_solvers_f,
        m_ctxt.get(),
        randstate.get(),
        pss_top_t.get(),
        entry_t.get(),
        &backend
    ));
    CollectingEvalListener listener;
    eval_ctxt->addListener(&listener);

    // Expect no remaining work
    ASSERT_FALSE(eval_ctxt->eval());

    ASSERT_EQ(listener.getActions().size(), 3);
}

TEST_F(TestEvalContextFullElab, two_action_par_exec_func_nb_b) {
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
    //   exec body {
    //     doit();
    //   }
    // }
    //

    dm::IDataTypeActionUP action1_t(m_ctxt->mkDataTypeAction("action1_t"));
    dm::ITypeProcStmtScopeUP exec_body_b(m_ctxt->mkTypeProcStmtScope());
    exec_body_b->addStatement(m_ctxt->mkTypeProcStmtExpr(
        m_ctxt->mkTypeExprMethodCallStatic(doit.get(), {})
    ));
    dm::ITypeExecUP exec_body(m_ctxt->mkTypeExecProc(
        dm::ExecKindT::Body,
        exec_body_b.release()));
    action1_t->addExec(exec_body.release());

    action1_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(action1_t.get());

    // entry_t
    // action entry_t {
    //   action_t a1, a2;
    //   activity {
    //     parallel {
    //       a1;
    //       a2;
    //     }
    //  }
    dm::IDataTypeActionUP entry_t(m_ctxt->mkDataTypeAction("entry_t"));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a1", action1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a2", action1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    entry_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(entry_t.get());

    dm::IDataTypeActivitySequence *activity_root = m_ctxt->mkDataTypeActivitySequence();
    dm::IDataTypeActivityParallel *activity_par = m_ctxt->mkDataTypeActivityParallel();
    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(1);
    activity_par->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(2);
    activity_par->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    entry_t->addActivity(m_ctxt->mkTypeFieldActivity(
        "activity", 
        activity_par,
        true));

    vsc::solvers::IRandStateUP randstate(m_solvers_f->mkRandState(""));

    EvalBackendTestFixture backend;

    IEvalContextUP eval_ctxt(m_eval_f->mkEvalContextFullElab(
        m_solvers_f,
        m_ctxt.get(),
        randstate.get(),
        pss_top_t.get(),
        entry_t.get(),
        &backend
    ));

    backend.setCallReq([&](IEvalThread *t, dm::IDataTypeFunction *f, const std::vector<IEvalResultUP> &p) {
        if (backend.getFuncCalls().size() == 1) {
            // Immediately respond to this
            t->setResult(eval_ctxt->mkEvalResultKind(EvalResultKind::Void));
        }
    });

    CollectingEvalListener listener;
    eval_ctxt->addListener(&listener);

    ASSERT_TRUE(eval_ctxt->eval());
    ASSERT_EQ(backend.getFuncCalls().size(), 2);
    ASSERT_EQ(listener.getActions().size(), 1);

    // Now, release the second function
    backend.getFuncCalls().back().first->setResult(
        eval_ctxt->mkEvalResultKind(EvalResultKind::Void));
    ASSERT_FALSE(eval_ctxt->eval());
    ASSERT_EQ(backend.getFuncCalls().size(), 2);
    ASSERT_EQ(listener.getActions().size(), 3);
}

TEST_F(TestEvalContextFullElab, two_action_par_exec_func_b_nb) {
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
    //   exec body {
    //     doit();
    //   }
    // }
    //

    dm::IDataTypeActionUP action1_t(m_ctxt->mkDataTypeAction("action1_t"));
    dm::ITypeProcStmtScopeUP exec_body_b(m_ctxt->mkTypeProcStmtScope());
    exec_body_b->addStatement(m_ctxt->mkTypeProcStmtExpr(
        m_ctxt->mkTypeExprMethodCallStatic(doit.get(), {})
    ));
    dm::ITypeExecUP exec_body(m_ctxt->mkTypeExecProc(
        dm::ExecKindT::Body,
        exec_body_b.release()));
    action1_t->addExec(exec_body.release());

    action1_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(action1_t.get());

    // entry_t
    // action entry_t {
    //   action_t a1, a2;
    //   activity {
    //     parallel {
    //       a1;
    //       a2;
    //     }
    //  }
    dm::IDataTypeActionUP entry_t(m_ctxt->mkDataTypeAction("entry_t"));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a1", action1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a2", action1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    entry_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(entry_t.get());

    dm::IDataTypeActivitySequence *activity_root = m_ctxt->mkDataTypeActivitySequence();
    dm::IDataTypeActivityParallel *activity_par = m_ctxt->mkDataTypeActivityParallel();
    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(1);
    activity_par->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(2);
    activity_par->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    entry_t->addActivity(m_ctxt->mkTypeFieldActivity(
        "activity", 
        activity_par,
        true));

    vsc::solvers::IRandStateUP randstate(m_solvers_f->mkRandState(""));

    EvalBackendTestFixture backend;

    IEvalContextUP eval_ctxt(m_eval_f->mkEvalContextFullElab(
        m_solvers_f,
        m_ctxt.get(),
        randstate.get(),
        pss_top_t.get(),
        entry_t.get(),
        &backend
    ));

    backend.setCallReq([&](IEvalThread *t, dm::IDataTypeFunction *f, const std::vector<IEvalResultUP> &p) {
        if (backend.getFuncCalls().size() == 2) {
            // Immediately respond to this
            t->setResult(eval_ctxt->mkEvalResultKind(EvalResultKind::Void));
        }
    });

    CollectingEvalListener listener;
    eval_ctxt->addListener(&listener);

    ASSERT_TRUE(eval_ctxt->eval());
    ASSERT_EQ(backend.getFuncCalls().size(), 2);
    ASSERT_EQ(listener.getActions().size(), 1);

    // Now, release the first function
    backend.getFuncCalls().at(0).first->setResult(
            eval_ctxt->mkEvalResultKind(EvalResultKind::Void));
    ASSERT_FALSE(eval_ctxt->eval());
    ASSERT_EQ(backend.getFuncCalls().size(), 2);
    for (std::vector<dm::IModelFieldAction *>::const_iterator
        it=listener.getActions().begin();
        it!=listener.getActions().end(); it++) {
        DEBUG("Action: %s", (*it)->name().c_str());
    }
    ASSERT_EQ(listener.getActions().size(), 3);
}

TEST_F(TestEvalContextFullElab, call_func_param_val) {
    vsc::dm::ITypeExprFieldRef *ref;
    m_ctxt->getDebugMgr()->enable(true);

    // Import function
    dm::IDataTypeFunctionUP doit(m_ctxt->mkDataTypeFunction(
        "doit",
        0,
        false));
    vsc::dm::IDataTypeIntUP uint32(m_ctxt->mkDataTypeInt(32, false));

    doit->addParameter(
        m_ctxt->mkDataTypeFunctionParamDecl(
            "p1",
            dm::ParamDir::In,
            uint32.get(),
            false,
            0
        ));
    doit->addImportSpec(m_ctxt->mkDataTypeFunctionImport("", false, false));

    // component pss_top {
    // }
    // 
    dm::IDataTypeComponentUP pss_top_t(m_ctxt->mkDataTypeComponent("pss_top"));

    // action_t
    // action action_t {
    //   exec body {
    //     doit();
    //   }
    // }
    //

    dm::IDataTypeActionUP action1_t(m_ctxt->mkDataTypeAction("action1_t"));
    dm::ITypeProcStmtScopeUP exec_body_b(m_ctxt->mkTypeProcStmtScope());
    vsc::dm::IModelValUP val(m_ctxt->mkModelValS(6, 32));
    exec_body_b->addStatement(m_ctxt->mkTypeProcStmtExpr(
        m_ctxt->mkTypeExprMethodCallStatic(doit.get(), {
            m_ctxt->mkTypeExprVal(val.get())
        }
    )));
    dm::ITypeExecUP exec_body(m_ctxt->mkTypeExecProc(
        dm::ExecKindT::Body,
        exec_body_b.release()));
    action1_t->addExec(exec_body.release());

    action1_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(action1_t.get());

    // entry_t
    // action entry_t {
    //   action_t a1;
    //   activity {
    //     a1;
    //  }
    dm::IDataTypeActionUP entry_t(m_ctxt->mkDataTypeAction("entry_t"));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a1", action1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
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

    entry_t->addActivity(m_ctxt->mkTypeFieldActivity(
        "activity", 
        activity_root,
        true));

    vsc::solvers::IRandStateUP randstate(m_solvers_f->mkRandState(""));

    EvalBackendTestFixture backend;
    std::vector<uint64_t> pvals;

    IEvalContextUP eval_ctxt(m_eval_f->mkEvalContextFullElab(
        m_solvers_f,
        m_ctxt.get(),
        randstate.get(),
        pss_top_t.get(),
        entry_t.get(),
        &backend
    ));

    backend.setCallReq([&](
        IEvalThread                     *t,
        dm::IDataTypeFunction           *f,
        const std::vector<IEvalResultUP>   &params) {
        for (uint32_t i=0; i<params.size(); i++) {
            pvals.push_back(params.at(i)->val_u());
        }
        t->setResult(eval_ctxt->mkEvalResultKind(EvalResultKind::Void));
    });

    CollectingEvalListener listener;
    eval_ctxt->addListener(&listener);

    ASSERT_FALSE(eval_ctxt->eval());

    ASSERT_EQ(listener.getActions().size(), 2);
    ASSERT_EQ(pvals.size(), 1);
    ASSERT_EQ(pvals.at(0), 6);
}

TEST_F(TestEvalContextFullElab, call_func_param_expr_val) {
    vsc::dm::ITypeExprFieldRef *ref;
    m_ctxt->getDebugMgr()->enable(true);

    // Import function
    dm::IDataTypeFunctionUP doit(m_ctxt->mkDataTypeFunction(
        "doit",
        0,
        false));
    vsc::dm::IDataTypeIntUP uint32(m_ctxt->mkDataTypeInt(32, false));

    doit->addParameter(
        m_ctxt->mkDataTypeFunctionParamDecl(
            "p1",
            dm::ParamDir::In,
            uint32.get(),
            false,
            0
        ));
    doit->addImportSpec(m_ctxt->mkDataTypeFunctionImport("", false, false));

    // component pss_top {
    // }
    // 
    dm::IDataTypeComponentUP pss_top_t(m_ctxt->mkDataTypeComponent("pss_top"));

    // action_t
    // action action_t {
    //   exec body {
    //     doit();
    //   }
    // }
    //

    dm::IDataTypeActionUP action1_t(m_ctxt->mkDataTypeAction("action1_t"));
    dm::ITypeProcStmtScopeUP exec_body_b(m_ctxt->mkTypeProcStmtScope());
    vsc::dm::IModelValUP val1(m_ctxt->mkModelValS(6, 32));
    vsc::dm::IModelValUP val2(m_ctxt->mkModelValS(5, 32));
    exec_body_b->addStatement(m_ctxt->mkTypeProcStmtExpr(
        m_ctxt->mkTypeExprMethodCallStatic(doit.get(), {
            m_ctxt->mkTypeExprBin(
                m_ctxt->mkTypeExprVal(val1.get()),
                vsc::dm::BinOp::Add,
                m_ctxt->mkTypeExprVal(val2.get()))
        }
    )));
    dm::ITypeExecUP exec_body(m_ctxt->mkTypeExecProc(
        dm::ExecKindT::Body,
        exec_body_b.release()));
    action1_t->addExec(exec_body.release());

    action1_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(action1_t.get());

    // entry_t
    // action entry_t {
    //   action_t a1;
    //   activity {
    //     a1;
    //  }
    dm::IDataTypeActionUP entry_t(m_ctxt->mkDataTypeAction("entry_t"));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a1", action1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
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

    entry_t->addActivity(m_ctxt->mkTypeFieldActivity(
        "activity", 
        activity_root,
        true));

    vsc::solvers::IRandStateUP randstate(m_solvers_f->mkRandState(""));

    EvalBackendTestFixture backend;
    std::vector<uint64_t> pvals;

    IEvalContextUP eval_ctxt(m_eval_f->mkEvalContextFullElab(
        m_solvers_f,
        m_ctxt.get(),
        randstate.get(),
        pss_top_t.get(),
        entry_t.get(),
        &backend
    ));

    backend.setCallReq([&](
        IEvalThread                     *t,
        dm::IDataTypeFunction           *f,
        const std::vector<IEvalResultUP>   &params) {
        for (uint32_t i=0; i<params.size(); i++) {
            pvals.push_back(params.at(i)->val_u());
        }
        t->setResult(eval_ctxt->mkEvalResultKind(EvalResultKind::Void));
    });

    CollectingEvalListener listener;
    eval_ctxt->addListener(&listener);

    ASSERT_FALSE(eval_ctxt->eval());

    ASSERT_EQ(listener.getActions().size(), 2);
    ASSERT_EQ(pvals.size(), 1);
    ASSERT_EQ(pvals.at(0), 11);
}

}
}
}
