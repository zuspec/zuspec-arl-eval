/*
 * TestBase.cpp
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
#include "dmgr/FactoryExt.h"
#include "dmgr/impl/DebugMacros.h"
#include "pyapi-compat-if/FactoryExt.h"
#include "zsp/arl/dm/FactoryExt.h"
#include "zsp/arl/eval/FactoryExt.h"
#include "vsc/dm/FactoryExt.h"
#include "vsc/solvers/FactoryExt.h"
#include "EvalBackendTestFixture.h"
#include "TestBase.h"


namespace zsp {
namespace arl {
namespace eval {


TestBase::TestBase() {

}

TestBase::~TestBase() {

}

void TestBase::SetUp() {

    std::vector<std::string> args = ::testing::internal::GetArgvs();

    char tmp[1024];

    fprintf(stdout, "argv[0]=%s\n", args.at(0).c_str());

    realpath(args.at(0).c_str(), tmp);
    fprintf(stdout, "realpath=%s\n", tmp);
    ::testing::internal::FilePath project_dir = ::testing::internal::FilePath(tmp);
    for (uint32_t i=0; i<3; i++) {
        project_dir = project_dir.RemoveFileName();
        project_dir = project_dir.RemoveTrailingPathSeparator();
    }
    fprintf(stdout, "project_dir=%s\n", project_dir.c_str());

    const char *pythonpath = getenv("PYTHONPATH");

    if (!pythonpath || !strstr(pythonpath, project_dir.c_str())) {
        fprintf(stdout, "Need to prepend\n");
        char *pythonpath_env = (char *)malloc(
            ((pythonpath)?strlen(pythonpath)+1:0)
            + project_dir.string().size()
            + strlen("test/src/python")
            + 1);
        strcpy(pythonpath_env, project_dir.c_str());
        strcat(pythonpath_env, "/test/src/python");
        if (pythonpath && pythonpath[0]) {
            strcat(pythonpath_env, ":");
            strcat(pythonpath_env, pythonpath);
        }
        setenv("PYTHONPATH", pythonpath_env, 1);
    }

    const char *path = getenv("PATH");
    if (!path || !strstr(path, project_dir.c_str())) {
        // Add 
        fprintf(stdout, "Need to prepend\n");
        char *path_env = (char *)malloc(
            ((path)?strlen(path)+1:0)
            + project_dir.string().size()
            + strlen("/packages/python/bin")
            + 1);
        strcpy(path_env, project_dir.c_str());
        strcat(path_env, "/packages/python/bin");
        if (path && path[0]) {
            strcat(path_env, ":");
            strcat(path_env, path);
        }
        setenv("PATH", path_env, 1);
    }

    dmgr::IFactory *dmgr_factory = dmgr_getFactory();
    vsc::dm::IFactory *vsc_dm_factory = vsc_dm_getFactory();
    m_arl_dm_factory = zsp_arl_dm_getFactory();
    vsc_dm_factory->init(dmgr_factory->getDebugMgr());
    m_arl_dm_factory->init(dmgr_factory->getDebugMgr());

    m_eval_f = zsp_arl_eval_getFactory();
    m_eval_f->init(dmgr_factory->getDebugMgr());

    pyapi::IFactory *pyapi_f = pyapi_compat_if_getFactory();
    pyapi_f->init(dmgr_factory->getDebugMgr());

    m_ctxt = dm::IContextUP(m_arl_dm_factory->mkContext(
        vsc_dm_factory->mkContext()));
    m_solvers_f = vsc_solvers_getFactory();

    m_randstate = vsc::solvers::IRandStateUP(
        m_solvers_f->mkRandState(""));

//    ArlImpl::inst()->init(vsc::VscImpl::inst()->getDebugMgr());
//    m_ctxt = IContextUP(
//        ArlImpl::inst()->mkContext(
//            vsc::VscImpl::inst()->mkContext()));
//    m_randstate = vsc::IRandStateUP(m_ctxt->mkRandState(""));

    DEBUG_INIT("Test", dmgr_factory->getDebugMgr());
    enableDebug(false);
}

void TestBase::TearDown() {
    m_ctxt.reset();
}

void TestBase::enableDebug(bool en) {
    m_ctxt->getDebugMgr()->enable(en);
}

dm::IDataTypeFunction *TestBase::mkFunction(
        const std::string       &name,
        vsc::dm::IDataType      *rtype,
        dm::ITypeProcStmt       *stmt) {
    dm::IDataTypeFunction *ret = m_ctxt->mkDataTypeFunction(
        name,
        rtype,
        false,
        dm::DataTypeFunctionFlags::Target);
    ret->getBody()->addStatement(stmt);

    return ret;
}

void TestBase::createEvalContext(
        IEvalContext                **ctxt,
        dm::IDataTypeComponent      *root_comp,
        dm::IDataTypeAction         *root_action,
        IEvalBackend                *backend) {
    pyapi::IFactory *pyapi_f = pyapi_compat_if_getFactory();
        
    ASSERT_TRUE(root_comp);
    ASSERT_TRUE(root_action);

    std::string err;
    pyapi::IPyEval *pyeval = pyapi_f->getPyEval(err);

    ASSERT_TRUE(pyeval) << err;
    
    IEvalContext *eval_ctxt = m_eval_f->mkEvalContextFullElab(
        m_solvers_f,
        m_ctxt.get(),
        m_randstate.get(),
        pyeval,
        root_comp,
        root_action,
        backend);
    ASSERT_TRUE(eval_ctxt);
    *ctxt = eval_ctxt;
}

void TestBase::createBackend(
        IEvalBackend               **backend,
        const std::function<void(IEvalThread*, dm::IDataTypeFunction*,const std::vector<vsc::dm::ValRef> &)> &f) {
    EvalBackendTestFixture *b = new EvalBackendTestFixture();
    b->setCallReq(f);
    *backend = b;
}

dmgr::IDebug *TestBase::m_dbg = 0;

}
}
}
