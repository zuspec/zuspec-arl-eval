/*
 * TestEvalExec.cpp
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
#include "TestEvalExec.h"


namespace zsp {
namespace arl {
namespace eval {


TestEvalExec::TestEvalExec() {

}

TestEvalExec::~TestEvalExec() {

}

TEST_F(TestEvalExec, single_action_single_fcall) {
    ZSP_DATACLASSES(TestEvalExec_single_action_single_fcall, pss_top, pss_top.Entry, R"(
        @zdc.import_fn
        def func():
            pass

        @zdc.import_fn
        def pre_solve_f():
            pass

        @zdc.component
        class pss_top(object):

            @zdc.action
            class Entry(object):
                @zdc.exec.pre_solve
                def pre_solve(self):
                    pre_solve_f()

                @zdc.exec.body
                def body(self):
                    func()
                pass

    )");
    #include "TestEvalExec_single_action_single_fcall.h"
    enableDebug(true);

    IEvalContext *eval_ctxt = 0;
    IEvalBackend *eval_backend = 0;
    std::vector<std::pair<std::string,int32_t>> calls;
    createBackend(
        &eval_backend,
        [&](
            IEvalThread *thread, 
            dm::IDataTypeFunction *func, 
            const std::vector<vsc::dm::ValRef> &params) {
            fprintf(stdout, "Function %s\n", func->name().c_str());
            // TODO: Need a 'Valid but empty' value
            calls.push_back({func->name(), 1});
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
    ASSERT_EQ(calls.size(), 2);
    ASSERT_EQ(calls.at(0).first, "pre_solve_f");
    ASSERT_EQ(calls.at(0).second, 1);
}

}
}
}
