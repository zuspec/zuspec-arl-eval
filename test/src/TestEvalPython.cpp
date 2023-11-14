/*
 * TestEvalPython.cpp
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
#include "TestEvalPython.h"


namespace zsp {
namespace arl {
namespace eval {


TestEvalPython::TestEvalPython() {

}

TestEvalPython::~TestEvalPython() {

}

TEST_F(TestEvalPython, smoke) {
    ZSP_DATACLASSES(TestEvalPython_smoke, pss_top, pss_top.Entry, R"(
        @zdc.component
        class pss_top(object):
            @zdc.action
            class Entry(object):
                @zdc.exec.post_solve
                def post_solve(self):
                    pass
    )");
    #include "TestEvalPython_smoke.h"

    enableDebug(true);

    dm::IPyImport *imp = m_ctxt->mkPyImport("test_eval_python.smoke");
    m_ctxt->addPyImport(imp);

    dm::ITypeExecProc *exec = dynamic_cast<dm::ITypeExecProc *>(
        Entry_t->getExecs(dm::ExecKindT::PostSolve).at(0).get());
    ASSERT_TRUE(exec);
    dm::ITypeProcStmtScope *body = exec->getBody();
    body->addStatement(
        m_ctxt->mkTypeProcStmtExpr(
            m_ctxt->mkTypeExprPythonMethodCall(
                m_ctxt->mkTypeExprPythonModuleRef(imp),
                true,
                "doit",
                {}
            )
        )
    );

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

    int32_t ret = eval_ctxt_o->eval();
    ASSERT_FALSE(ret) << eval_ctxt_o->getError();
/*
    ASSERT_EQ(calls.size(), 2);
    ASSERT_EQ(calls.at(0).first, "pre_solve_f");
    ASSERT_EQ(calls.at(0).second, 1);
 */

}

TEST_F(TestEvalPython, smoke2) {
    ZSP_DATACLASSES(TestEvalPython_smoke2, pss_top, pss_top.Entry, R"(
        @zdc.component
        class pss_top(object):
            @zdc.action
            class Entry(object):
                @zdc.exec.post_solve
                def post_solve(self):
                    pass
    )");
    #include "TestEvalPython_smoke2.h"

    enableDebug(true);

    dm::IPyImport *imp = m_ctxt->mkPyImport("test_eval_python.smoke");
    m_ctxt->addPyImport(imp);

    dm::ITypeExecProc *exec = dynamic_cast<dm::ITypeExecProc *>(
        Entry_t->getExecs(dm::ExecKindT::PostSolve).at(0).get());
    ASSERT_TRUE(exec);
    dm::ITypeProcStmtScope *body = exec->getBody();
    body->addStatement(
        m_ctxt->mkTypeProcStmtExpr(
            m_ctxt->mkTypeExprPythonMethodCall(
                m_ctxt->mkTypeExprPythonModuleRef(imp),
                true,
                "doit",
                {}
            )
        )
    );

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

    int32_t ret = eval_ctxt_o->eval();
    ASSERT_FALSE(ret) << eval_ctxt_o->getError();
/*
    ASSERT_EQ(calls.size(), 2);
    ASSERT_EQ(calls.at(0).first, "pre_solve_f");
    ASSERT_EQ(calls.at(0).second, 1);
 */

}

}
}
}
