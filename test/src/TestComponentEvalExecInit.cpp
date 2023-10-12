/*
 * TestComponentEvalExecInit.cpp
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
#include "TestComponentEvalExecInit.h"
#include "vsc/dm/impl/ValRefInt.h"
#include "zsp/arl/dm/IModelFieldComponentRoot.h"


namespace zsp {
namespace arl {
namespace eval {


TestComponentEvalExecInit::TestComponentEvalExecInit() {

}

TestComponentEvalExecInit::~TestComponentEvalExecInit() {

}

TEST_F(TestComponentEvalExecInit, smoke) {
    ZSP_DATACLASSES(TestComponentEvalExecInit_smoke, pss_top, pss_top.Entry, R"(
        @zdc.import_fn
        def ext_f():
            pass 

        @zdc.component
        class pss_top(object):

            @zdc.exec.init_down
            def init_down(self):
                pass

            @zdc.exec.init_up
            def init_up(self):
                pass

            @zdc.action
            class Entry(object):
                pass

    )");
    #include "TestComponentEvalExecInit_smoke.h"
    enableDebug(true);

    IEvalContext *eval_ctxt = 0;
    IEvalBackend *eval_backend = 0;
    createBackend(
        &eval_backend,
        [&](
            IEvalThread *thread, 
            dm::IDataTypeFunction *func, 
            const std::vector<vsc::dm::ValRef> &params) {
            fprintf(stdout, "Function\n");
            // TODO: Need a 'Valid but empty' value
            thread->setResult(m_ctxt->mkValRefInt(0, false, 1));
        }
    );
    createEvalContext(
        &eval_ctxt,
        pss_top_t,
        Entry_t,
        eval_backend);
    IEvalContextUP eval_ctxt_o(eval_ctxt);
    ASSERT_TRUE(eval_ctxt_o.get());

    ASSERT_FALSE(eval_ctxt_o->eval());
}

TEST_F(TestComponentEvalExecInit, nested_2) {
    ZSP_DATACLASSES(TestComponentEvalExecInit_nested_2, pss_top, pss_top.Entry, R"(
        @zdc.import_fn
        def ext_f():
            pass 

        @zdc.import_fn
        def sub_f(v : zdc.uint32_t) -> zdc.uint32_t:
            pass 

        @zdc.import_fn
        def sub_f2(v : zdc.uint32_t):
            pass 

        @zdc.component
        class SubC(object):
            f1 : zdc.uint64_t
            f2 : zdc.uint64_t
            @zdc.exec.init_down
            def init_down(self):
                self.f1 == 2
                self.f2 == 3
                sub_f(self.f1)
                pass

            @zdc.exec.init_up
            def init_up(self):
                sub_f()
                pass


        @zdc.component
        class pss_top(object):
            c1 : SubC
            c2 : SubC

            @zdc.exec.init_down
            def init_down(self):
                ext_f()
                pass

            @zdc.exec.init_up
            def init_up(self):
                ext_f()
                pass

            @zdc.action
            class Entry(object):
                pass
    )");
    #include "TestComponentEvalExecInit_nested_2.h"
    enableDebug(true);


    IEvalContext *eval_ctxt = 0;
    IEvalBackend *eval_backend = 0;
    createBackend(
        &eval_backend,
        [&](
            IEvalThread *thread, 
            dm::IDataTypeFunction *func, 
            const std::vector<vsc::dm::ValRef> &params) {
            fprintf(stdout, "Function %s (%d)\n", func->name().c_str(), params.size());
            // TODO: Need a 'Valid but empty' value
            thread->setResult(m_ctxt->mkValRefInt(0, false, 1));
        }
    );
    createEvalContext(
        &eval_ctxt,
        pss_top_t,
        Entry_t,
        eval_backend);
    IEvalContextUP eval_ctxt_o(eval_ctxt);
    ASSERT_TRUE(eval_ctxt_o.get());

    ASSERT_FALSE(eval_ctxt_o->eval());

    dm::IModelFieldComponentRoot *pss_top = eval_ctxt->getRootComponent();
    ASSERT_EQ(pss_top->getDataTypeT<vsc::dm::IDataTypeStruct>()->getField(0)->getOffset(), 0);
//    ASSERT_EQ(pss_top->getDataTypeT<vsc::dm::IDataTypeStruct>()->getField(1)->getOffset(), 0);
    vsc::dm::ValRefStruct val(pss_top->getImmVal());
    ASSERT_EQ(val.getNumFields(), 2);
    vsc::dm::ValRefStruct c1(val.getFieldRef(1));
    ASSERT_EQ(c1.getNumFields(), 2);
    vsc::dm::ValRefInt f1(c1.getFieldRef(0));
    ASSERT_EQ(f1.get_val_u(), 2);
    vsc::dm::ValRefInt f2(c1.getFieldRef(1));
    ASSERT_EQ(f2.get_val_u(), 3);
}

}
}
}
