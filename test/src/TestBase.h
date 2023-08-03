/**
 * TestBase.h
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
#pragma once
#include "gtest/gtest.h"
#include "vsc/solvers/IFactory.h"
#include "vsc/solvers/IRandState.h"
#include "zsp/arl/dm/IContext.h"
#include "zsp/arl/dm/IFactory.h"
#include "zsp/arl/eval/IFactory.h"


namespace zsp {
namespace arl {
namespace eval {


class TestBase : public ::testing::Test {
public:
    TestBase();

    virtual ~TestBase();

    virtual void SetUp() override;

    virtual void TearDown() override;

    void enableDebug(bool en);

protected:

    dm::IDataTypeFunction *mkFunction(
        const std::string       &name,
        vsc::dm::IDataType      *rtype,
        dm::ITypeProcStmt       *stmt);

protected:
    static dmgr::IDebug         *m_dbg;
    dm::IFactory                *m_arl_dm_factory;
    eval::IFactory              *m_eval_f;
    dm::IContextUP              m_ctxt;
    vsc::solvers::IFactory      *m_solvers_f;
    vsc::solvers::IRandStateUP  m_randstate;
};

}
}
}


