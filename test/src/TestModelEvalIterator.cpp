/*
 * TestModelEvalIterator.cpp
 *
 * Copyright 2022 Matthew Ballance and Contributors
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
#include "zsp/arl/dm/impl/ModelBuildContext.h"
#include "TestModelEvalIterator.h"

#include "ModelEvaluatorIncrElabSequence.h"
#include "ModelEvaluatorThread.h"


namespace zsp {
namespace arl {
namespace eval {

TestModelEvalIterator::TestModelEvalIterator() {

}

TestModelEvalIterator::~TestModelEvalIterator() {

}

TEST_F(TestModelEvalIterator, two_branch_parallel) {

    // ModelAction
    // - Activity
    //   - parallel
    //     - traverse
    //     - traverse

    dm::IDataTypeActionUP action_t(m_ctxt->mkDataTypeAction("action_t"));
    dm::IDataTypeComponentUP comp_t(m_ctxt->mkDataTypeComponent("comp_t"));
    action_t->setComponentType(comp_t.get());
    comp_t->addActionType(action_t.get());

    dm::IDataTypeActionUP entry_t(m_ctxt->mkDataTypeAction("entry_t"));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a1", action_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, vsc::dm::ValRef()));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a2", action_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, vsc::dm::ValRef()));
    entry_t->setComponentType(comp_t.get());
    comp_t->addActionType(entry_t.get());

    dm::ModelBuildContext build_ctxt(dm::ModelBuildContext(m_ctxt.get()));
    dm::IModelFieldComponentRootUP comp(comp_t->mkRootFieldT<dm::IModelFieldComponentRoot>(
        &build_ctxt,
        "pss_top",
        false));


    dm::IDataTypeActivitySequence *activity = m_ctxt->mkDataTypeActivitySequence();
    vsc::dm::ITypeExprFieldRef *ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(1);
    activity->addActivity(m_ctxt->mkTypeFieldActivity(
        "",
        m_ctxt->mkDataTypeActivityTraverse(
            ref,
            0),
        true));

    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(2);
    activity->addActivity(m_ctxt->mkTypeFieldActivity(
        "",
        m_ctxt->mkDataTypeActivityTraverse(
            ref,
            0),
        true));

    entry_t->addActivity(m_ctxt->mkTypeFieldActivity("activity", activity, true));

    dm::IModelFieldActionUP entry(entry_t->mkRootFieldT<dm::IModelFieldAction>(
        &build_ctxt,
        "entry",
        false));

// TODO:
    vsc::solvers::IRandStateUP rs(m_solvers_f->mkRandState("0"));

    dm::IModelFieldComponentRootUP pss_top(comp_t->mkRootFieldT<dm::IModelFieldComponentRoot>(
        &build_ctxt,
        "pss_top", 
        false));
    pss_top->initCompTree();

    // IModelActivitySequence *activity = new ModelActivitySequence("", 0);
    // IModelActivityParallel *parallel = new ModelActivityParallel();
    // parallel->addBranch(new ModelActivityTraverse(traverse_1.get(), 0));
    // parallel->addBranch(new ModelActivityTraverse(traverse_2.get(), 0));
    // activity->addActivity(parallel);
    // root->addActivity(activity);

    ModelEvaluatorThread *root_thread = new ModelEvaluatorThread(
        m_ctxt.get(), rs->next());
    root_thread->pushComponent(comp.get());
    ModelEvaluatorIncrElabSequence *root_seq = new ModelEvaluatorIncrElabSequence(root_thread);
    root_seq->addActivity(m_ctxt->mkModelActivityTraverse(
        entry.get(), 
        0,
        false,
        0,
        false), true);
    root_thread->pushIterator(root_seq);

    ASSERT_TRUE(root_thread->next());
    ASSERT_EQ(root_thread->type(), dm::ModelEvalNodeT::Parallel);
    dm::IModelEvalIterator *parallel_it = root_thread->iterator();
    ASSERT_TRUE(parallel_it);

    // Before evaluating the branches, click the parent iterator
    // to remove clear the parallel iterator
    ASSERT_FALSE(root_thread->next());


    ASSERT_TRUE(parallel_it->next());
    ASSERT_EQ(parallel_it->type(), dm::ModelEvalNodeT::Sequence);

    dm::IModelEvalIterator *branch_it = parallel_it->iterator();
    ASSERT_TRUE(branch_it);
    ASSERT_TRUE(branch_it->next());
    ASSERT_EQ(branch_it->type(), dm::ModelEvalNodeT::Action);
    ASSERT_FALSE(branch_it->next());

    ASSERT_TRUE(parallel_it->next());
    ASSERT_EQ(parallel_it->type(), dm::ModelEvalNodeT::Sequence);
    branch_it = parallel_it->iterator();
    ASSERT_TRUE(branch_it);
    ASSERT_TRUE(branch_it->next());
    ASSERT_EQ(branch_it->type(), dm::ModelEvalNodeT::Action);
    ASSERT_FALSE(branch_it->next());

    ASSERT_FALSE(parallel_it->next());
}

TEST_F(TestModelEvalIterator, par_single_resource) {
    vsc::dm::ITypeExprFieldRef *ref;
    m_ctxt->getDebugMgr()->enable(true);

    dm::IDataTypeFlowObjUP rsrc_t(m_ctxt->mkDataTypeFlowObj("rsrc_t", dm::FlowObjKindE::Resource));

    // component pss_top {
    //   pool [2] rsrc_t   rsrc_p;
    // }
    // 
    dm::IDataTypeComponentUP pss_top_t(m_ctxt->mkDataTypeComponent("pss_top"));
    pss_top_t->addField(m_ctxt->mkTypeFieldPool("rsrc_p", rsrc_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 2));
    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(1);
    pss_top_t->addPoolBindDirective(m_ctxt->mkPoolBindDirective(
        dm::PoolBindKind::All,
        ref,
        0));

    // action_t
    // action action_t {
    //   lock rsrc_t   r1;
    // }
    //
    vsc::dm::IDataTypeInt *ui4 = m_ctxt->mkDataTypeInt(false, 4);
    m_ctxt->addDataTypeInt(ui4);
    dm::IDataTypeActionUP action1_t(m_ctxt->mkDataTypeAction("action1_t"));
    action1_t->addField(m_ctxt->mkTypeFieldClaim("l1", rsrc_t.get(), true));
    action1_t->addField(m_ctxt->mkTypeFieldPhy("v1", ui4, false, vsc::dm::TypeFieldAttr::Rand, vsc::dm::ValRef()));
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
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a1", action1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, vsc::dm::ValRef()));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("a2", action1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, vsc::dm::ValRef()));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("v1", ui4, false, vsc::dm::TypeFieldAttr::Rand, vsc::dm::ValRef()));
    entry_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(entry_t.get());

    dm::IDataTypeActivityParallel *par = m_ctxt->mkDataTypeActivityParallel();
    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(1);
    par->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(2);
    par->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    // Root activity is always expected to be a scope
    dm::IDataTypeActivitySequence *activity_root = m_ctxt->mkDataTypeActivitySequence();
    activity_root->addActivity(m_ctxt->mkTypeFieldActivity(
        "", 
        par,
        true));

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
    IModelEvaluatorUP evaluator(m_eval_f->mkModelEvaluator(
        ModelEvaluatorKind::FullElab, m_solvers_f, m_ctxt.get()));

    dm::IModelEvalIterator *it = evaluator->eval(
        randstate->next(),
        pss_top.get(),
        entry_t.get());
    ASSERT_TRUE(it);
    ASSERT_TRUE(it->next());
    ASSERT_EQ(it->type(), dm::ModelEvalNodeT::Sequence);
    dm::IModelEvalIterator *it1 = it->iterator();
    ASSERT_TRUE(it1->next());
    ASSERT_EQ(it1->type(), dm::ModelEvalNodeT::Action);
    vsc::dm::IModelField *e1 = it1->action();
    vsc::dm::IModelField *ef1 = e1->getField(3);
    ASSERT_TRUE(ef1);
//    fprintf(stdout, "ef1=%lld\n", ef1->val()->val_u());

    ASSERT_TRUE(it1->next());
    ASSERT_EQ(it1->type(), dm::ModelEvalNodeT::Sequence);
    dm::IModelEvalIterator *it2 = it1->iterator();

    ASSERT_TRUE(it2->next());
    ASSERT_EQ(it2->type(), dm::ModelEvalNodeT::Parallel);

    dm::IModelEvalIterator *it_p = it2->iterator();
    ASSERT_TRUE(it_p->next());
    ASSERT_EQ(it_p->type(), dm::ModelEvalNodeT::Action);
    dm::IModelFieldAction *br1 = it_p->action();
    ASSERT_TRUE(br1);
    vsc::dm::IModelField *f1 = br1->getField(2); 
    ASSERT_TRUE(f1);
//    fprintf(stdout, "f1: %lld\n", f1->val()->val_u());

    ASSERT_TRUE(it_p->next());
    ASSERT_EQ(it_p->type(), dm::ModelEvalNodeT::Action);
    dm::IModelFieldAction *br2 = it_p->action();
    ASSERT_TRUE(br2);
    vsc::dm::IModelField *f2 = br2->getField(2); 
    ASSERT_TRUE(f2);
//    fprintf(stdout, "f2: %lld\n", f2->val()->val_u());

    ASSERT_FALSE(it_p->next());

    ASSERT_FALSE(it1->next());
    ASSERT_FALSE(it->next());
}

// TEST_F(TestModelEvalIterator, two_branch_parallel_branch_seq) {

//     // ModelAction
//     // - Activity
//     //   - parallel
//     //     - traverse
//     //     - traverse

//     ArlImpl::inst()->init(vsc::DebugMgr::inst());
//     vsc::DebugMgr::inst()->enable(true);

//     IModelFieldActionUP traverse_1(new ModelFieldActionRoot("traverse_1", 0));
//     IModelFieldActionUP traverse_2(new ModelFieldActionRoot("traverse_2", 0));
//     IModelFieldActionUP root(new ModelFieldActionRoot("root", 0));

//     vsc::IRandStateUP rs(new vsc::RandState("0"));

//     IModelActivitySequence *activity = new ModelActivitySequence("", 0);
//     IModelActivityParallel *parallel = new ModelActivityParallel();
//     IModelActivitySequence *seq_1 = new ModelActivitySequence("XX", 0);
//     IModelActivitySequence *seq_2 = new ModelActivitySequence("XX", 0);

//     seq_1->addActivity(new ModelActivityTraverse(traverse_1.get(), 0));
//     seq_2->addActivity(new ModelActivityTraverse(traverse_2.get(), 0));

//     parallel->addBranch(seq_1);
//     parallel->addBranch(seq_2);
//     activity->addActivity(parallel);
//     root->addActivity(activity);

//     ModelEvaluatorThread *root_thread = new ModelEvaluatorThread(0, rs->next());
//     ModelEvaluatorSequence *root_seq = new ModelEvaluatorSequence(root_thread);
//     root_seq->addActivity(new ModelActivityTraverse(root.get(), 0), true);
//     root_thread->pushIterator(root_seq);

//     ASSERT_TRUE(root_thread->next());
//     ASSERT_EQ(root_thread->type(), ModelEvalNodeT::Parallel);
//     IModelEvalIterator *parallel_it = root_thread->iterator();
//     ASSERT_TRUE(parallel_it);

//     // Before evaluating the branches, click the parent iterator
//     // to remove clear the parallel iterator
//     ASSERT_FALSE(root_thread->next());


//     ASSERT_TRUE(parallel_it->next());
//     ASSERT_EQ(parallel_it->type(), ModelEvalNodeT::Sequence);

//     IModelEvalIterator *branch_it = parallel_it->iterator();
//     ASSERT_TRUE(branch_it);
//     ASSERT_TRUE(branch_it->next());
//     ASSERT_EQ(branch_it->type(), ModelEvalNodeT::Action);
//     ASSERT_FALSE(branch_it->next());

//     ASSERT_TRUE(parallel_it->next());
//     ASSERT_EQ(parallel_it->type(), ModelEvalNodeT::Sequence);
//     branch_it = parallel_it->iterator();
//     ASSERT_TRUE(branch_it);
//     ASSERT_TRUE(branch_it->next());
//     ASSERT_EQ(branch_it->type(), ModelEvalNodeT::Action);
//     ASSERT_FALSE(branch_it->next());

//     ASSERT_FALSE(parallel_it->next());
// }

}
}
}
