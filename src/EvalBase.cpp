/*
 * EvalBase.cpp
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
#include "vsc/dm/impl/ValRefInt.h"
#include "zsp/arl/eval/IEvalContextInt.h"
#include "EvalBase.h"


namespace zsp {
namespace arl {
namespace eval {

EvalBase::EvalBase(
    IEvalContext            *ctxt,
    IEvalThread             *thread,
    int32_t                 vp_id) :
        m_dbg(0),
        m_initial(true), m_entry_idx(-1), m_ctxt(ctxt), m_thread(thread),
        m_vp_id(vp_id), m_error(false) {

}

EvalBase::EvalBase(IEvalThread *thread) :
        m_dbg(0), m_initial(true), m_entry_idx(-1), m_ctxt(0), m_thread(thread),
        m_vp_id(-1), m_error(false) {
}

EvalBase::EvalBase(const EvalBase *o) :
    m_dbg(o->m_dbg),
    m_initial(false), m_entry_idx(o->m_entry_idx), m_ctxt(o->m_ctxt), 
    m_thread(o->m_thread), m_vp_id(o->m_vp_id),
    m_error(o->m_error), m_errMsg(o->m_errMsg) {

}

EvalBase::~EvalBase() {

}

int32_t EvalBase::eval(const std::function<void()> &body) {
    if (m_initial) {
        m_thread->pushEval(this);
    }

    body();

    if (m_initial) {
        if (!haveResult()) {
            m_thread->suspendEval(this);
        }
        m_initial = false;
    }

    return !haveResult();
}

vsc::dm::ValRef EvalBase::getImmVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) {
    if (m_vp_id != -1) {
        return ctxtT<IEvalContextInt>()->getValProvider(m_vp_id)->getImmVal(
            root_kind,
            root_offset-1,
            val_offset);
    } else {
        vsc::dm::ValRef ret;
        switch (root_kind) {
            case vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope: {
                if (root_offset > 0) {
                    // Go up a level and look for the value there
                    ret = ctxtT<IEvalContextInt>()->getValProvider(getIdx()-1)->getImmVal(
                        root_kind,
                        root_offset-1,
                        val_offset);
                } else {
                    ERROR("no value provider for bottom-up");
                }
            } break;
            case vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope: {
                ERROR("no value provider for top-down");
            } break;
        }
        return ret;
    }
}

vsc::dm::ValRef EvalBase::getMutVal(
        vsc::dm::ITypeExprFieldRef::RootRefKind root_kind,
        int32_t                                 root_offset,
        int32_t                                 val_offset) {
    if (m_vp_id != -1) {
        return ctxtT<IEvalContextInt>()->getValProvider(m_vp_id)->getMutVal(
            root_kind,
            root_offset-1,
            val_offset);
    } else {
        vsc::dm::ValRef ret;
        switch (root_kind) {
            case vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope: {
                if (root_offset > 0) {
                    // Go up a level and look for the value there
                    ret = ctxtT<IEvalContextInt>()->getValProvider(getIdx()-1)->getImmVal(
                        root_kind,
                        root_offset-1,
                        val_offset);
                } else {
                    fprintf(stdout, "Error: no value provider for bottom-up\n");
                }
            } break;
            case vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope: {
                fprintf(stdout, "Error: no value provider for top-down\n");
            } break;
        }
        return ret;
    }
}

void EvalBase::clrResult(bool clr_err) {
    m_result.reset();
    if (clr_err) {
//        m_error = false;
//        m_errMsg.clear();
    }
}

void EvalBase::setResult(const vsc::dm::ValRef &r) {
    DEBUG("setResult (valid=%d)", r.valid());
    m_result.set(r);
}

void EvalBase::setVoidResult() {
    vsc::dm::IDataTypeInt *i32 = m_ctxt->ctxt()->findDataTypeInt(true, 32);
    if (!i32) {
        i32 = m_ctxt->ctxt()->mkDataTypeInt(true, 32);
        m_ctxt->ctxt()->addDataTypeInt(i32);
    }
    vsc::dm::ValRefInt v(0, i32, vsc::dm::ValRef::Flags::None);
    setResult(v);
}

void EvalBase::setError(const std::string &msg) {
    m_error = true;
    m_errMsg = msg;
}

bool EvalBase::haveError() const {
    return m_error;
}

const std::string &EvalBase::getError() const {
    return m_errMsg;
}

bool EvalBase::haveResult() const {
    DEBUG("haveResult: %d", m_result.valid());
    return m_result.valid();
}

}
}
}
