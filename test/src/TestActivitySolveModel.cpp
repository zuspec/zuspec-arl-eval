/*
 * TestActivitySolveModel.cpp
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
#include "zsp/arl/dm/impl/ModelBuildContext.h"
#include "TestActivitySolveModel.h"
#include "TaskElaborateActivity.h"

namespace zsp {
namespace arl {
namespace eval {


TestActivitySolveModel::TestActivitySolveModel() {

}

TestActivitySolveModel::~TestActivitySolveModel() {

}

TEST_F(TestActivitySolveModel, par_single_resource) {
    vsc::dm::ITypeExprFieldRef *ref;
    dm::IDataTypeFlowObjUP rsrc_t(m_ctxt->mkDataTypeFlowObj("rsrc_t", dm::FlowObjKindE::Resource));

    // component pss_top {
    //   pool [2] rsrc_t   rsrc_p;
    // }
    // 
    dm::IDataTypeComponentUP pss_top_t(m_ctxt->mkDataTypeComponent("pss_top"));
    pss_top_t->addField(m_ctxt->mkTypeFieldPool("rsrc_p", rsrc_t.get(), false, 
        vsc::dm::TypeFieldAttr::NoAttr, 2));
    /* TODO:
    ref = m_ctxt->mkTypeExprFieldRef();
    ref->addIdxRef(1);
    ref->addRootRef();
    pss_top_t->addPoolBindDirective(m_ctxt->mkPoolBindDirective(
        PoolBindKind::All,
        ref,
        0));
     */

    // action_t
    // action action_t {
    //   lock rsrc_t   r1;
    // }
    //
    dm::IDataTypeActionUP action1_t(m_ctxt->mkDataTypeAction("action1_t"));
    action1_t->addField(m_ctxt->mkTypeFieldClaim("l1", rsrc_t.get(), true));
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

    ElabActivityUP activity(TaskElaborateActivity(
        m_solvers_f,
        m_ctxt.get()).elaborate(
        m_randstate->clone(),
        pss_top.get(),
        entry_t.get()));
    ASSERT_TRUE(activity.get());
    {
        dm::IModelActivitySequence *seq = dynamic_cast<dm::IModelActivitySequence *>(activity->root);
        ASSERT_TRUE(seq);
        ASSERT_EQ(seq->activities().size(), 1);
        // TODO: Really shouldn't have two levels of 'sequence' here...
        seq = dynamic_cast<dm::IModelActivitySequence *>(seq->activities().at(0));
        ASSERT_TRUE(seq);
        dm::IModelActivityParallel *par = dynamic_cast<dm::IModelActivityParallel *>(seq->activities().at(0));
//        ASSERT_TRUE(par);
        ASSERT_EQ(par->branches().size(), 2);
    }

}

}
}
}
