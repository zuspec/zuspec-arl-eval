/*
 * EvalTypeProcStmtScope.cpp
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
#include "dmgr/impl/DebugMacros.h"
#include "zsp/arl/eval/IEvalContextInt.h"
#include "EvalTypeExpr.h"
#include "EvalTypeProcStmt.h"
#include "EvalTypeProcStmtScope.h"


namespace zsp {
namespace arl {
namespace eval {


EvalTypeProcStmtScope::EvalTypeProcStmtScope(
    IEvalContext                *ctxt,
    IEvalThread                 *thread,
    int32_t                     vp_id,
    dm::ITypeProcStmtScope      *scope) : 
        EvalBase(ctxt, thread), m_vp_id(vp_id), m_scope(scope), 
        m_idx(0), m_subidx(0) {
    DEBUG_INIT("EvalTypeProcStmtScope", ctxt->getDebugMgr());
}

EvalTypeProcStmtScope::EvalTypeProcStmtScope(EvalTypeProcStmtScope *o) : 
    EvalBase(o), m_vp_id(o->m_vp_id), m_scope(o->m_scope), 
    m_idx(o->m_idx), m_subidx(o->m_subidx), 
    m_build_ctxt(std::move(o->m_build_ctxt)) {
    for (std::vector<vsc::dm::IModelFieldUP>::iterator
        it=o->m_locals.begin();
        it!=o->m_locals.end(); it++) {
        m_locals.push_back(std::move(*it));
    }
    o->m_locals.clear();
}

EvalTypeProcStmtScope::~EvalTypeProcStmtScope() {

}

int32_t EvalTypeProcStmtScope::eval() {
    DEBUG_ENTER("[%d] eval", getIdx());

    if (m_initial) {
        m_thread->pushEval(this);
        // Safety
    }
    setVoidResult();

    switch (m_idx) {
        case 0: {
            if (m_subidx > 0) {
                // Re-entering after suspending during init-expr evaluation
                m_locals.at(m_subidx-1)->getMutVal().set(getResult());
            }

            while (m_subidx < m_scope->getVariables().size()) {
                dm::ITypeProcStmtVarDecl *var = m_scope->getVariables().at(m_subidx).get();

                // Create and optionally initialize variable
                if (!m_build_ctxt) {
                    m_build_ctxt = dm::IModelBuildContextUP(new dm::ModelBuildContext(m_ctxt->ctxt()));
                }
                m_locals.push_back(vsc::dm::IModelFieldUP(
                    var->getDataType()->mkRootField(
                        m_build_ctxt.get(),
                        var->name(),
                        false)));
                if (var->getInit()) {
                    if (EvalTypeExpr(m_ctxt, m_thread, getIdx(), var->getInit()).eval()) {
                        m_subidx++;
                        break;
                    }
                    m_locals.back()->getMutVal().set(getResult());
                } else {
                    m_subidx++;
                }
            }

            m_idx = 1;
            m_subidx = 0;
        }

        case 1: {
            while (m_subidx < m_scope->getStatements().size()) {
                EvalTypeProcStmt evaluator(
                    m_ctxt, 
                    m_thread, 
                    getIdx(), // Direct the evaluator here
                    m_scope->getStatements().at(m_subidx).get());

                m_subidx++;

                if (evaluator.eval()) {
                    clrResult();
                    break;
                }
            }
        }
    }


    // TODO: during first phase, must initialize any local variables 
    // with initial values

    int32_t ret = !haveResult();

    if (m_initial) {
        m_initial = false;
        if (!ret) {
            DEBUG("popEval");
            m_thread->popEval(this);
        } else {
            DEBUG("suspendEval");
            m_thread->suspendEval(this);
        }
    }

    DEBUG_LEAVE("[%d] eval %d", getIdx(), ret);
    return ret;
}

vsc::dm::ValRef EvalTypeProcStmtScope::getImmVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) {
    DEBUG_ENTER("getImmVal kind=%d root_offset=%d val_offset=%d",
        root_kind, root_offset, val_offset);
    vsc::dm::ValRef ret;
    if (root_kind == vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope) {
        if (root_offset == 0) {
            // This scope
            if (val_offset < m_locals.size()) {
                DEBUG("Get parameter %d", val_offset);
                ret = m_locals.at(val_offset)->getImmVal();
            } else {
                ERROR("out-of-bounds parameter value request");
            }
        } else if (m_vp_id != -1) {
            // Delegate up
            DEBUG("Delegate up to @ m_vp_id", m_vp_id);
            return ctxtT<IEvalContextInt>()->getValProvider(m_vp_id)->getImmVal(
                root_kind,
                root_offset-1,
                val_offset);
        } else {
            ERROR("Invalid vp_id with root_offset=%d", root_offset);
        }
    } else {
        return ctxtT<IEvalContextInt>()->getValProvider(m_vp_id)->getImmVal(
            root_kind,
            root_offset,
            val_offset);
    }
    DEBUG_LEAVE("getImmVal");
}

vsc::dm::ValRef EvalTypeProcStmtScope::getMutVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) {
    DEBUG_ENTER("getMutVal kind=%d root_offset=%d val_offset=%d",
        root_kind, root_offset, val_offset);
    vsc::dm::ValRef ret;
    if (root_kind == vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope) {
        if (root_offset == 0) {
            // This scope
            if (val_offset < m_locals.size()) {
                DEBUG("Get parameter %d", val_offset);
                ret = m_locals.at(val_offset)->getMutVal();
            } else {
                ERROR("out-of-bounds parameter value request");
            }
        } else if (m_vp_id != -1) {
            // Delegate up
            DEBUG("Delegate up to @ m_vp_id", m_vp_id);
            return ctxtT<IEvalContextInt>()->getValProvider(m_vp_id)->getMutVal(
                root_kind,
                root_offset-1,
                val_offset);
        } else {
            ERROR("Invalid vp_id with root_offset=%d", root_offset);
        }
    } else {
        return ctxtT<IEvalContextInt>()->getValProvider(m_vp_id)->getMutVal(
            root_kind,
            root_offset,
            val_offset);
    }
    DEBUG_LEAVE("getMutVal");
    return ret;
}

IEval *EvalTypeProcStmtScope::clone() {
    return new EvalTypeProcStmtScope(this);
}


dmgr::IDebug *EvalTypeProcStmtScope::m_dbg = 0;
 
}
}
}
