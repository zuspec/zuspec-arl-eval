/**
 * EvalContextIncrElab.h
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
#pragma once
#include "zsp/arl/dm/IContext.h"
#include "zsp/arl/eval/IEvalContextActivity.h"
#include "EvalContextBase.h"
#include "TaskElaborateActivity.h"
#include "EvalThread.h"

namespace zsp {
namespace arl {
namespace eval {



class EvalContextIncrElab :
    public virtual IEvalContextActivity,
    public virtual EvalContextBase {
public:
    EvalContextIncrElab(
        dmgr::IDebugMgr                                 *dmgr,
        vsc::solvers::IFactory                          *solvers_f,
        dm::IContext                                    *ctxt,
        const vsc::solvers::IRandState                  *randstate,
        dm::IDataTypeComponent                          *root_comp,
        dm::IDataTypeAction                             *root_action,
        IEvalBackend                                    *backend=0);

    virtual ~EvalContextIncrElab();

    virtual int32_t buildCompTree() override;
    
    virtual int32_t initCompTree() override;

    virtual int32_t eval() override;

    virtual IEval *clone() override { return 0; }

    virtual dm::IModelFieldComponentRoot *getRootComponent() override {
        return m_pss_top.get();
    }

    virtual const std::vector<dm::IModelFieldExecutor *> &getExecutors() const override {
        return m_activity->executors;
    }

protected:

    void evalExecInit(
        dm::IDataTypeComponent      *comp_t,
        const vsc::dm::ValRef       &val);

    void finalizeComponentTree();

private:
    static dmgr::IDebug                     *m_dbg;
    dm::IModelFieldComponentRootUP          m_pss_top;
    bool                                    m_pss_top_is_init;
    dm::IDataTypeComponent                  *m_root_comp;
    dm::IDataTypeAction                     *m_root_action;
    vsc::dm::IModelField                    *m_root_action_f;
    ElabActivityUP                          m_activity;
    std::vector<dm::IModelFieldExecutor *>  m_executors;

};

}
}
}


