/**
 * TaskElaborateActivity.h
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
#include "vsc/solvers/ICompoundSolver.h"
#include "vsc/solvers/IFactory.h"
#include "zsp/arl/dm/IContext.h"
#include "zsp/arl/dm/impl/VisitorBase.h"

namespace zsp {
namespace arl {
namespace eval {

struct ElabActivity {
    std::vector<dm::IModelActivityScopeUP>      activity_s;
    dm::IModelActivityScope                     *root;
};
using ElabActivityUP=std::unique_ptr<ElabActivity>;

class TaskElaborateActivity : public dm::VisitorBase {
public:
    TaskElaborateActivity(
        vsc::solvers::IFactory  *solvers_f,
        dm::IContext            *ctxt);

    virtual ~TaskElaborateActivity();

    /**
     * @brief Builds an activity from a component tree and action type
     * 
     * @param root_comp 
     * @param root_action 
     * @return IModelActivity* 
     */
    ElabActivity *elaborate(
        vsc::solvers::IRandState             *randstate,
        dm::IModelFieldComponent        *root_comp,
        dm::IDataTypeAction             *root_action);

	virtual void visitModelActivityTraverse(dm::IModelActivityTraverse *a) override;

private:
    enum class StepT {

    };

private:

    void process_scope(dm::IModelActivityScope *s);

    void process_traversal(dm::IModelActivityTraverse *t);

private:
    static dmgr::IDebug             *m_dbg;
    vsc::solvers::IFactory          *m_solvers_f;
    dm::IContext                    *m_ctxt;
    ElabActivityUP                  m_activity;
    int32_t                         m_scope_search_depth;
    int32_t                         m_action_depth;
    int32_t                         m_action_target_depth;
    bool                            m_more_work;
    bool                            m_changed;

};

}
}
}

