/*
 * TestEvalProcStmts.cpp
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
#include "TestEvalProcStmts.h"
#include "EvalBackendTestFixture.h"


namespace zsp {
namespace arl {
namespace eval {


TestEvalProcStmts::TestEvalProcStmts() {

}

TestEvalProcStmts::~TestEvalProcStmts() {

}

TEST_F(TestEvalProcStmts, stmt_return_i) {
    enableDebug(true);

    vsc::dm::IModelValUP val(m_ctxt->mkModelValS(15, 32));
    vsc::dm::IDataTypeIntUP i32_t(m_ctxt->mkDataTypeInt(true, 32));

    dm::IDataTypeFunctionUP func(mkFunction(
        "func",
        i32_t.get(),
        m_ctxt->mkTypeProcStmtReturn(
            m_ctxt->mkTypeExprVal(val.get())
        ))
    );


    IEvalContextUP eval_c(m_eval_f->mkEvalContextFunctionStatic(
        m_solvers_f,
        m_ctxt.get(),
        m_randstate.get(),
        0, // &backend,
        func.get(),
        {}
    ));

    ASSERT_EQ(eval_c->eval(), 0);
//    ASSERT_EQ(eval_c->haveResult());
    IEvalResult *result = eval_c->getResult();
    fprintf(stdout, "result: %p\n", result);
    ASSERT_EQ(result->getKind(), EvalResultKind::Val);
    ASSERT_EQ(result->val_i(), 15);
}

TEST_F(TestEvalProcStmts, stmt_return_param_i) {
    enableDebug(true);

    vsc::dm::IDataTypeIntUP i32_t(m_ctxt->mkDataTypeInt(true, 32));

    dm::IDataTypeFunctionUP func(mkFunction(
        "func",
        i32_t.get(),
        m_ctxt->mkTypeProcStmtReturn(
            m_ctxt->mkTypeExprFieldRef(
                vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope,
                1,
                {0}
            )
        ))
    );

    vsc::dm::IModelValUP val(m_ctxt->mkModelValS(15, 32));
    IEvalContextUP eval_c(m_eval_f->mkEvalContextFunctionStatic(
        m_solvers_f,
        m_ctxt.get(),
        m_randstate.get(),
        0, // &backend,
        func.get(),
        {
            m_ctxt->mkTypeExprVal(val.get())
        }
    ));

    ASSERT_EQ(eval_c->eval(), 0);
//    ASSERT_EQ(eval_c->haveResult());
    IEvalResult *result = eval_c->getResult();
    fprintf(stdout, "result: %p\n", result);
    ASSERT_EQ(result->getKind(), EvalResultKind::Val);
    ASSERT_EQ(result->val_i(), 15);
}

dm::IDataTypeFunction *TestEvalProcStmts::mkFunction(
        const std::string       &name,
        vsc::dm::IDataType      *rtype,
        dm::ITypeProcStmt       *stmt) {
    dm::IDataTypeFunction *ret = m_ctxt->mkDataTypeFunction(
        name,
        rtype,
        false,
        false,
        true);
    ret->getBody()->addStatement(stmt);

    return ret;
}

}
}
}
