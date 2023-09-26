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

    vsc::dm::ValRef val(m_ctxt->mkValRefInt(15, true, 32));
    vsc::dm::IDataTypeIntUP i32_t(m_ctxt->mkDataTypeInt(true, 32));

    dm::IDataTypeFunctionUP func(mkFunction(
        "func",
        i32_t.get(),
        m_ctxt->mkTypeProcStmtReturn(
            m_ctxt->mkTypeExprVal(i32_t.get(), val.vp())
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
    vsc::dm::ValRefInt result(eval_c->getResult());
    fprintf(stdout, "result: %p\n", result);
//    ASSERT_EQ(result->getKind(), EvalResultKind::Val);
    ASSERT_EQ(result.get_val_s(), 15);
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

    vsc::dm::ValRef val(m_ctxt->mkValRefInt(15, true, 32));
    IEvalContextUP eval_c(m_eval_f->mkEvalContextFunctionStatic(
        m_solvers_f,
        m_ctxt.get(),
        m_randstate.get(),
        0, // &backend,
        func.get(),
        {
            m_ctxt->mkTypeExprVal(val.type(), val.vp())
        }
    ));

    ASSERT_EQ(eval_c->eval(), 0);
//    ASSERT_EQ(eval_c->haveResult());
    vsc::dm::ValRefInt result(eval_c->getResult());
//    fprintf(stdout, "result: %p\n", result);
//    ASSERT_EQ(result->getKind(), EvalResultKind::Val);
    ASSERT_EQ(result.get_val_s(), 15);
}

TEST_F(TestEvalProcStmts, stmt_return_ite_param_gt_true) {
    enableDebug(true);

    vsc::dm::ValRef val_true(m_ctxt->mkValRefInt(15, true, 32));
    vsc::dm::ValRef val_false(m_ctxt->mkValRefInt(20, true, 32));
    vsc::dm::IDataTypeIntUP i32_t(m_ctxt->mkDataTypeInt(true, 32));

    dm::IDataTypeFunctionUP func(mkFunction(
        "func",
        i32_t.get(),
        m_ctxt->mkTypeProcStmtIfElse(
            m_ctxt->mkTypeExprBin(
                m_ctxt->mkTypeExprFieldRef(
                    vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope,
                    1,
                    {0}
                ),
                vsc::dm::BinOp::Gt,
                m_ctxt->mkTypeExprFieldRef(
                    vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope,
                    1,
                    {1}
                )
            ),
            m_ctxt->mkTypeProcStmtReturn(m_ctxt->mkTypeExprVal(
                val_true.type(), val_true.vp())),
            m_ctxt->mkTypeProcStmtReturn(m_ctxt->mkTypeExprVal(
                val_false.type(), val_false.vp()))
        ))
    );

    vsc::dm::ValRef val_1(m_ctxt->mkValRefInt(15, true, 32));
    vsc::dm::ValRef val_2(m_ctxt->mkValRefInt(10, true, 32));
    IEvalContextUP eval_c(m_eval_f->mkEvalContextFunctionStatic(
        m_solvers_f,
        m_ctxt.get(),
        m_randstate.get(),
        0, // &backend,
        func.get(),
        {
            m_ctxt->mkTypeExprVal(val_1),
            m_ctxt->mkTypeExprVal(val_2)
        }
    ));

    ASSERT_EQ(eval_c->eval(), 0);
//    ASSERT_EQ(eval_c->haveResult());
    vsc::dm::ValRefInt result(eval_c->getResult());
//    fprintf(stdout, "result: %p\n", result);
//    ASSERT_EQ(result->getKind(), EvalResultKind::Val);
    ASSERT_EQ(result.get_val_s(), 15);
}

TEST_F(TestEvalProcStmts, stmt_return_ite_param_gt_false) {
    enableDebug(true);

    vsc::dm::ValRef val_true(m_ctxt->mkValRefInt(15, true, 32));
    vsc::dm::ValRef val_false(m_ctxt->mkValRefInt(20, true, 32));
    vsc::dm::IDataTypeIntUP i32_t(m_ctxt->mkDataTypeInt(true, 32));

    dm::IDataTypeFunctionUP func(mkFunction(
        "func",
        i32_t.get(),
        m_ctxt->mkTypeProcStmtIfElse(
            m_ctxt->mkTypeExprBin(
                m_ctxt->mkTypeExprFieldRef(
                    vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope,
                    1,
                    {0}
                ),
                vsc::dm::BinOp::Gt,
                m_ctxt->mkTypeExprFieldRef(
                    vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope,
                    1,
                    {1}
                )
            ),
            m_ctxt->mkTypeProcStmtReturn(m_ctxt->mkTypeExprVal(val_true)),
            m_ctxt->mkTypeProcStmtReturn(m_ctxt->mkTypeExprVal(val_false))
        ))
    );

    vsc::dm::ValRef val_1(m_ctxt->mkValRefInt(15, true, 32));
    vsc::dm::ValRef val_2(m_ctxt->mkValRefInt(10, true, 32));
    IEvalContextUP eval_c(m_eval_f->mkEvalContextFunctionStatic(
        m_solvers_f,
        m_ctxt.get(),
        m_randstate.get(),
        0, // &backend,
        func.get(),
        {
            m_ctxt->mkTypeExprVal(val_2),
            m_ctxt->mkTypeExprVal(val_1)
        }
    ));

    ASSERT_EQ(eval_c->eval(), 0);
//    ASSERT_EQ(eval_c->haveResult());
    vsc::dm::ValRefInt result(eval_c->getResult());
//    fprintf(stdout, "result: %p\n", result);
//    ASSERT_EQ(result->getKind(), EvalResultKind::Val);
    ASSERT_EQ(result.get_val_s(), 20);
}



}
}
}
