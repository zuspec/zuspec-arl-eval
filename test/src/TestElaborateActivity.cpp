/*
 * TestElaborateActivity.cpp
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
#include "TestElaborateActivity.h"
#include "zsp/arl/dm/impl/ModelBuildContext.h"
#include "TaskElaborateActivity.h"

namespace zsp {
namespace arl {
namespace eval {

TestElaborateActivity::TestElaborateActivity() {

}

TestElaborateActivity::~TestElaborateActivity() {

}

TEST_F(TestElaborateActivity, resource_wildcard_1t_1p) {
    enableDebug(true);

    vsc::dm::ITypeExprFieldRef *ref;
    dm::IDataTypeFlowObjUP rsrc_t(m_ctxt->mkDataTypeFlowObj("rsrc_t", dm::FlowObjKindE::Resource));

    // component pss_top {
    //   pool [2] rsrc_t   rsrc_p;
    // }
    // 
    dm::IDataTypeComponentUP pss_top_t(m_ctxt->mkDataTypeComponent("pss_top"));
    pss_top_t->addField(m_ctxt->mkTypeFieldPool("rsrc_p", rsrc_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 2));
    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        -1
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

    ASSERT_EQ(pss_top->getPools(rsrc_t.get()).size(), 1);
    ASSERT_EQ(pss_top->getResObjects(dynamic_cast<dm::IDataTypeResource *>(rsrc_t.get())).size(), 2);
    std::pair<int32_t,int32_t> pool_r = pss_top->getResPoolObjRange(pss_top->getPools(rsrc_t.get()).at(0));
    ASSERT_EQ(pool_r.first, 0);
    ASSERT_EQ(pool_r.second, 1);
    ASSERT_EQ(pool_r.second, 1);
    const std::vector<std::pair<int32_t,dm::IModelFieldPool *>> &claim_m = pss_top->getClaimBoundCompPool(
        action1_t->getFieldT<dm::ITypeFieldClaim>(1));
    ASSERT_EQ(claim_m.size(), 1);
    ASSERT_EQ(claim_m.at(0).first, 0);

    TaskElaborateActivity(m_ctxt.get()).elaborate(
        m_randstate.get(),
        pss_top.get(),
        entry_t.get()
    );
}

TEST_F(TestElaborateActivity, buffer_wildcard_2p_1c) {
    enableDebug(true);

    vsc::dm::ITypeExprFieldRef *ref;
    dm::IDataTypeFlowObjUP buf_t(m_ctxt->mkDataTypeFlowObj("buf_t", dm::FlowObjKindE::Buffer));

    // component pss_top {
    //   pool [2] buf_t   buf_p;
    // }
    // 
    dm::IDataTypeComponentUP pss_top_t(m_ctxt->mkDataTypeComponent("pss_top"));
    pss_top_t->addField(m_ctxt->mkTypeFieldPool("buf_p", buf_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(1);
    pss_top_t->addPoolBindDirective(m_ctxt->mkPoolBindDirective(
        dm::PoolBindKind::All,
        ref,
        0));

    // prod1_t
    // action prod1_t {
    //   output buf_t   dat_o;
    // }
    //
    dm::IDataTypeActionUP prod1_t(m_ctxt->mkDataTypeAction("prod1_t"));
    prod1_t->addField(m_ctxt->mkTypeFieldInOut("dat_o", buf_t.get(), false));
    prod1_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(prod1_t.get());

    // prod2_t
    // action prod2_t {
    //   output buf_t   dat_o;
    // }
    //
    dm::IDataTypeActionUP prod2_t(m_ctxt->mkDataTypeAction("prod2_t"));
    prod2_t->addField(m_ctxt->mkTypeFieldInOut("dat_o", buf_t.get(), false));
    prod2_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(prod2_t.get());

    // cons1_t
    // action cons1_t {
    //   input buf_t   dat_i;
    // }
    //
    dm::IDataTypeActionUP cons1_t(m_ctxt->mkDataTypeAction("cons1_t"));
    cons1_t->addField(m_ctxt->mkTypeFieldInOut("dat_i", buf_t.get(), true));
    cons1_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(cons1_t.get());

    // entry_t
    // action entry_t {
    //   prod1_t p1;
    //   prod2_t p2;
    //   cons1_t c1;
    //   activity {
    //     p1;
    //     p2;
    //     c1;
    //  }
    dm::IDataTypeActionUP entry_t(m_ctxt->mkDataTypeAction("entry_t"));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("p1", prod1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("p2", prod2_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("c1", cons1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    entry_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(entry_t.get());

    dm::IDataTypeActivitySequence *seq = m_ctxt->mkDataTypeActivitySequence();
    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(1);
    seq->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(2);
    seq->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(3);
    seq->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    // Root activity is always expected to be a scope
    dm::IDataTypeActivitySequence *activity_root = m_ctxt->mkDataTypeActivitySequence();
    activity_root->addActivity(m_ctxt->mkTypeFieldActivity(
        "", 
        seq,
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

    TaskElaborateActivity(m_ctxt.get()).elaborate(
        m_randstate.get(),
        pss_top.get(),
        entry_t.get()
    );
}

TEST_F(TestElaborateActivity, buffer_wildcard_2p_2c) {
    enableDebug(true);

    vsc::dm::ITypeExprFieldRef *ref;
    dm::IDataTypeFlowObjUP buf_t(m_ctxt->mkDataTypeFlowObj("buf_t", dm::FlowObjKindE::Buffer));

    // component pss_top {
    //   pool [2] buf_t   buf_p;
    // }
    // 
    dm::IDataTypeComponentUP pss_top_t(m_ctxt->mkDataTypeComponent("pss_top"));
    pss_top_t->addField(m_ctxt->mkTypeFieldPool("buf_p", buf_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0
    );
    ref->addPathElem(1);
    pss_top_t->addPoolBindDirective(m_ctxt->mkPoolBindDirective(
        dm::PoolBindKind::All,
        ref,
        0));

    // prod1_t
    // action prod1_t {
    //   output buf_t   dat_o;
    // }
    //
    dm::IDataTypeActionUP prod1_t(m_ctxt->mkDataTypeAction("prod1_t"));
    prod1_t->addField(m_ctxt->mkTypeFieldInOut("dat_o", buf_t.get(), false));
    prod1_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(prod1_t.get());

    // prod2_t
    // action prod2_t {
    //   output buf_t   dat_o;
    // }
    //
    dm::IDataTypeActionUP prod2_t(m_ctxt->mkDataTypeAction("prod2_t"));
    prod2_t->addField(m_ctxt->mkTypeFieldInOut("dat_o", buf_t.get(), false));
    prod2_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(prod2_t.get());

    // cons1_t
    // action cons1_t {
    //   input buf_t   dat_i;
    // }
    //
    dm::IDataTypeActionUP cons1_t(m_ctxt->mkDataTypeAction("cons1_t"));
    cons1_t->addField(m_ctxt->mkTypeFieldInOut("dat_i", buf_t.get(), true));
    cons1_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(cons1_t.get());

    // entry_t
    // action entry_t {
    //   prod1_t p1;
    //   prod2_t p2;
    //   cons1_t c1;
    //   cons1_t c2;
    //   activity {
    //     p1;
    //     p2;
    //     c1;
    //     c2;
    //  }
    dm::IDataTypeActionUP entry_t(m_ctxt->mkDataTypeAction("entry_t"));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("p1", prod1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("p2", prod2_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("c1", cons1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    entry_t->addField(m_ctxt->mkTypeFieldPhy("c2", cons1_t.get(), false, vsc::dm::TypeFieldAttr::NoAttr, 0));
    entry_t->setComponentType(pss_top_t.get());
    pss_top_t->addActionType(entry_t.get());

    dm::IDataTypeActivitySequence *seq = m_ctxt->mkDataTypeActivitySequence();
    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0  
    );
    ref->addPathElem(1);
    seq->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0);
    ref->addPathElem(2);
    seq->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0);
    ref->addPathElem(2);
    seq->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    ref = m_ctxt->mkTypeExprFieldRef(
        vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
        0);
    ref->addPathElem(4);
    seq->addActivity(m_ctxt->mkTypeFieldActivity(
        "", m_ctxt->mkDataTypeActivityTraverse(ref, 0), true));

    // Root activity is always expected to be a scope
    dm::IDataTypeActivitySequence *activity_root = m_ctxt->mkDataTypeActivitySequence();
    activity_root->addActivity(m_ctxt->mkTypeFieldActivity(
        "", 
        seq,
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

    TaskElaborateActivity(m_ctxt.get()).elaborate(
        m_randstate.get(),
        pss_top.get(),
        entry_t.get()
    );
}

}
}
}
