/*
 * EvalTypeExpr.cpp
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
#include "vsc/dm/ITypeExprVal.h"
#include "vsc/dm/impl/TaskIsDataTypeInt.h"
#include "vsc/dm/impl/TaskIsDataTypeStr.h"
#include "zsp/arl/dm/impl/TaskPackStruct2Int.h"
#include "zsp/arl/dm/impl/TaskUnpackInt2Struct.h"
#include "zsp/arl/dm/impl/ValRefPyObj.h"
#include "zsp/arl/eval/IEvalContextInt.h"
#include "zsp/arl/eval/impl/TaskEvalGetLval.h"
#include "TaskEvalCheckRegAccess.h"
#include "EvalTypeExpr.h"
#include "EvalTypeExprRegOffset.h"
#include "EvalTypeFunction.h"
#include "TaskConvertImportFuncVal.h"


namespace zsp {
namespace arl {
namespace eval {


EvalTypeExpr::EvalTypeExpr(
    IEvalContext            *ctxt,
    IEvalThread             *thread,
    int32_t                 vp_id,
    vsc::dm::ITypeExpr      *expr,
    const std::string       &logid) :
        EvalBase(ctxt, thread, vp_id, logid),
        m_expr(expr), 
        m_builtin_i(0),
        m_idx(0), m_subidx(0) {
    DEBUG_INIT("zsp::arl::eval::EvalTypeExpr", ctxt->getDebugMgr());
}

EvalTypeExpr::EvalTypeExpr(EvalTypeExpr *o) :
    EvalBase(o), m_expr(o->m_expr), 
    m_val_lhs(std::move(o->m_val_lhs)),
    m_val_rhs(std::move(o->m_val_rhs)), 
    m_builtin_i(o->m_builtin_i),
    m_params(o->m_params.begin(), o->m_params.end()),
    m_idx(o->m_idx), m_subidx(o->m_subidx) {
}

EvalTypeExpr::~EvalTypeExpr() {

}

int32_t EvalTypeExpr::eval() {
    DEBUG_ENTER("[%d] eval (initial=%d)", getIdx(), m_initial);
    if (m_initial) {
        m_thread->pushEval(this);

        // Safety
        setFlags(EvalFlags::Complete);
    }

    if (m_expr) {
        m_expr->accept(m_this);
    } else {
        DEBUG_ERROR("attempting to evaluate a null expression");
    }

    int32_t ret = !hasFlags(EvalFlags::Complete);

    if (m_initial) {
        m_initial = false;
        if (!ret) {
            m_thread->popEval(this);
        } else {
            m_thread->suspendEval(this);
        }
    }

    DEBUG_LEAVE("[%d] eval %d", getIdx(), ret);
    return ret;
}

IEval *EvalTypeExpr::clone() {
    return new EvalTypeExpr(this);
}

void EvalTypeExpr::visitTypeExprArrIndex(vsc::dm::ITypeExprArrIndex *e) {
    DEBUG_ENTER("visitTypeExprArrIndex");
    DEBUG_ERROR("Unimplemented");
    DEBUG_LEAVE("visitTypeExprArrIndex");
}

void EvalTypeExpr::visitTypeExprBin(vsc::dm::ITypeExprBin *e) { 
    DEBUG_ENTER("visitTypeExprBin");
    switch (m_idx) {
        case 0: { // Operand 1
            m_idx = 1;
            EvalTypeExpr evaluator(m_ctxt, m_thread, m_vp_id, e->lhs());

            if (evaluator.eval()) {
                break;
            }
        }
        case 1: { // Operand 2
            m_idx = 2;
            m_val_lhs.set(getResult()); // Preserve LHS

            setFlags(EvalFlags::Complete);

            EvalTypeExpr evaluator(m_ctxt, m_thread, m_vp_id, e->rhs());

            if (evaluator.eval()) {
                break;
            }

        }
        case 2: { // Perform operation
            m_idx = 3;

            m_val_rhs.set(getResult()); // Preserve RHS

            vsc::dm::IDataTypeInt *lhs_int;
            vsc::dm::IDataTypeInt *rhs_int;
            vsc::dm::IDataTypeString *lhs_str, *rhs_str;

            if ((lhs_int=vsc::dm::TaskIsDataTypeInt().check(m_val_lhs.type()))
                && (rhs_int=vsc::dm::TaskIsDataTypeInt().check(m_val_rhs.type()))) {
                DEBUG("TODO: integer operation %d,%d %d,%d",
                    lhs_int->isSigned(), lhs_int->getWidth(),
                    rhs_int->isSigned(), rhs_int->getWidth());
                setResult(m_ctxt->ctxt()->evalBinOpInt(
                    vsc::dm::ValRefInt(m_val_lhs),
                    e->op(),
                    vsc::dm::ValRefInt(m_val_rhs)));
            } else if (vsc::dm::TaskIsDataTypeStr().check(m_val_lhs.type())
                && vsc::dm::TaskIsDataTypeStr().check(m_val_rhs.type())) {
                DEBUG("String evaluation");
                setResult(m_ctxt->ctxt()->evalBinOpStr(
                    vsc::dm::ValRefStr(m_val_lhs),
                    e->op(),
                    vsc::dm::ValRefStr(m_val_rhs)));
            }

            vsc::dm::IModelValOp *op = m_ctxt->getModelValOp();
            // Copy size of the RHS for now
            // TODO: need to consider context
#ifdef UNDEFINED
            IEvalResultUP val = IEvalResultUP(m_ctxt->mkEvalResultVal(m_val_rhs.get()));

            switch (e->op()) {
                case vsc::dm::BinOp::Eq: {
                    op->eq(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Ne: {
                    op->ne(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Gt: {
                    // TODO: determine sign of operands
                    op->sgt(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Ge: {
                    // TODO: determine sign of operands
                    op->sge(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Lt: {
                    // TODO: determine sign of operands
                    op->slt(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Le: {
                    // TODO: determine sign of operands
                    op->sle(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Add: {
                    op->add(
                        val.get(), 
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Sub: {
                    op->sub(
                        val.get(), 
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Div: {
                    fprintf(stdout, "TODO: BinOp::Div\n");
                    /*
                    op->div(
                        val.get(), 
                        m_val_lhs.get(),
                        m_val_rhs.get());
                     */
                } break;
                case vsc::dm::BinOp::BinAnd: {
                    op->bin_and(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::BinOr: {
                    op->bin_or(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::BinXor: {
                    op->bin_xor(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::LogAnd: {
                    op->log_and(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::LogOr: {
                    op->log_or(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Sll: {
                    op->sll(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Srl: {
                    op->srl(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Xor: {
                    op->log_xor(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                } break;
                case vsc::dm::BinOp::Not: {
                    fprintf(stdout, "TODO: BinOp::Not\n");
                    /*
                    op->log_xor(
                        val.get(),
                        m_val_lhs.get(),
                        m_val_rhs.get());
                     */
                } break;
                default:
//                    DEBUG_ERROR("")
                    break;
            }

            setResult(val.release());
#endif
        }

        case 3: {
            // Idle
        }
    }
    DEBUG_LEAVE("visitTypeExprBin");
}

void EvalTypeExpr::visitTypeExprFieldRef(vsc::dm::ITypeExprFieldRef *e) { 
    vsc::dm::ValRef val;

    DEBUG_ENTER("visitTypeExprFieldRef");
#ifdef UNDEFINED
    switch (e->getRootRefKind()) {
        case vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope: {
            DEBUG("Bottom-up scope");
            // TODO: Should we get a mutable reference instead?
            val = getImmVal(
                vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope,
                e->getRootRefOffset(),
                e->getPath().at(0));


            // TODO:
            fprintf(stdout, "TODO: Set cloned result\n");
//              setResult(dynamic_cast<IEvalResult *>(val->clone()));
        } break;

        case vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope: {
            DEBUG("Top-down scope");
            val = ctxtT<IEvalContextInt>()->getValProvider(m_vp_id)->getMutVal(
                    e->getRootRefKind(), 
                    e->getRootRefOffset(), e->getPath().at(0));
        } break;

        case vsc::dm::ITypeExprFieldRef::RootRefKind::RootExpr: {
            DEBUG_ERROR("Root expr");
        } break;
    }

    DEBUG("  ref type=%p", val.type());
    
    if (!val.valid()) {
        setError("Failed to obtain a valid root handle");
        DEBUG_LEAVE("visitTypeExprFieldRef");
        return;
    }

    DEBUG("%d sub-elements on this path", (e->getPath().size()-1));

    for (uint32_t i=1; i<e->getPath().size(); i++) {
        vsc::dm::ValRefStruct val_s(val);
        val = val_s.getFieldRef(e->getPath().at(i));
    }

    // Are there any cases in which we need to clone?
    setResult(val);

#endif /* UNDEFINED */
    DEBUG_LEAVE("visitTypeExprFieldRef");
}

void EvalTypeExpr::visitTypeExprRange(vsc::dm::ITypeExprRange *e) { 

}

void EvalTypeExpr::visitTypeExprRangelist(vsc::dm::ITypeExprRangelist *e) { 

}

void EvalTypeExpr::visitTypeExprRefBottomUp(vsc::dm::ITypeExprRefBottomUp *e) {
    DEBUG_ENTER("visitTypeExprRefBottomUp");
    vsc::dm::ValRef val(getImmVal(
            vsc::dm::ITypeExprFieldRef::RootRefKind::BottomUpScope,
            e->getScopeOffset(),
            e->getSubFieldIndex()));
    setResult(val);
    DEBUG_LEAVE("visitTypeExprRefBottomUp");
}

void EvalTypeExpr::visitTypeExprRefTopDown(vsc::dm::ITypeExprRefTopDown *e) {
    DEBUG_ENTER("visitTypeExprRefTopDown");
    setResult(getImmVal(
            vsc::dm::ITypeExprFieldRef::RootRefKind::TopDownScope,
            -1,
            -1));
    DEBUG_LEAVE("visitTypeExprRefTopDown");
}

void EvalTypeExpr::visitTypeExprSubField(vsc::dm::ITypeExprSubField *e) {
    DEBUG_ENTER("visitTypeExprSubField");
    e->getRootExpr()->accept(m_this);
    vsc::dm::ValRefStruct val_s(getResult());
    setResult(val_s.getFieldRef(e->getSubFieldIndex()));
    DEBUG_LEAVE("visitTypeExprSubField");
}

void EvalTypeExpr::visitTypeExprVal(vsc::dm::ITypeExprVal *e) { 
    DEBUG_ENTER("visitTypeExprVal");
    setResult(e->val().toImmutable());
    DEBUG_LEAVE("visitTypeExprVal");
}

void EvalTypeExpr::visitTypeExprMethodCallContext(dm::ITypeExprMethodCallContext *e) {
    DEBUG_ENTER("visitTypeExprMethodCallContext");
    switch (m_idx) {
        case 0: {
            // Determine if we need to rewrite a register access 
            m_idx = 1;
            m_builtin_i = ctxtT<IEvalContextInt>()->getBuiltinFuncInfo(e->getTarget());

#ifdef UNDEFINED
            m_isreg_res = TaskEvalCheckRegAccess(
                m_ctxt,
                m_vp_id).check(
                    e->getContext(),
                    e->getTarget());
#endif

            if (m_builtin_i && m_builtin_i->hasFlags(BuiltinFuncFlags::RegFunc)) {
                DEBUG("Is a register access");

                DEBUG_ENTER("GetRegOffset");
                if (EvalTypeExprRegOffset(m_ctxt, m_thread, m_vp_id, e->getContext()).eval()) {
                    DEBUG_LEAVE("GetRegOffset -- blocked");
                    break;
                }
                DEBUG_LEAVE("GetRegOffset");

                /**
                 * Calculate the address using the address associated with
                 * the base handle and offets along the path
                 */

                /*
                m_func = m_isreg_res.func;

                // Set the base+offset as the result
                vsc::dm::ValRefInt base(m_isreg_res.root);
                setResult(m_ctxt->mkValRefInt(
                    base.get_val_u()+m_isreg_res.offset,
                    false,
                    64));
                 */
            } else {
                DEBUG("Is NOT a register access");

                // Push the context handle 
                vsc::dm::ValRef ctxt(TaskEvalGetLval(m_ctxt->getDebugMgr(), this).eval(e->getContext()));

                setFlags(EvalFlags::Complete);
                setResult(ctxt);
/*
                if (EvalTypeExpr(m_ctxt, m_thread, m_vp_id, e->getContext()).eval()) {
                    break;
                }
 */
            }
        }

        // Grab the context handle
        case 1: {

            if (m_subidx == 0) {
                // Perform first-time checks
                DEBUG("Context flags=0x%08x", getResult().flags());
                m_params.push_back(getResult());
                clrFlags(EvalFlags::Complete);
            } else if (hasFlags(EvalFlags::Complete)) {
                // Perform some light type conversion if we'll be calling
                // an import function
                if (e->getTarget()->hasFlags(dm::DataTypeFunctionFlags::Import)) {
                    m_params.push_back(
                        TaskConvertImportFuncVal(m_ctxt).convert(getResult()));
                } else {
                    m_params.push_back(getResult());
                }
            } else {
                break;
            }

            while (m_subidx < e->getParameters().size()) {
                EvalTypeExpr evaluator(
                    m_ctxt, 
                    m_thread, 
                    m_vp_id,
                    e->getParameters().at(m_subidx).get());

                m_subidx += 1;
                clrFlags(EvalFlags::Complete);
                if (evaluator.eval()) {
                    break;
                } else if (hasFlags(EvalFlags::Complete)) {
                    DEBUG("push param[%d] (valid=%d)", m_params.size(), getResult().valid());

                    // Perform some light type conversion if we'll be calling
                    // an import function
                    if (e->getTarget()->hasFlags(dm::DataTypeFunctionFlags::Import)) {
                        m_params.push_back(
                            TaskConvertImportFuncVal(m_ctxt).convert(getResult()));
                    } else {
                        m_params.push_back(getResult());
                    }
                }
            }

            // If we left the loop before achieving enough
            // parameters, suspend...
            if (m_subidx < e->getParameters().size()) {
                break;
            }

            clrFlags(EvalFlags::Complete); // Clear 'safety' result

            m_idx = 2;

            if (m_builtin_i
                && m_builtin_i->hasFlags(BuiltinFuncFlags::RegFunc)
                && m_builtin_i->hasFlags(BuiltinFuncFlags::RegFuncWrite)
                && m_builtin_i->hasFlags(BuiltinFuncFlags::RegFuncStruct)) {
                // Must convert second parameter (data) from struct
                // to integer
                DEBUG("Convert struct-type parameter to integer");
                vsc::dm::ValRef val_s = dm::TaskPackStruct2Int(
                    m_ctxt->ctxt()).pack(m_params.at(1));
                m_params.at(1) = val_s;
            }

            DEBUG_ENTER("callFuncReq");
            if (m_builtin_i) {
                m_builtin_i->getImpl()(m_thread, e->getTarget(), m_params);
            } else if (!e->getTarget()->hasFlags(dm::DataTypeFunctionFlags::Import) &&
                !e->getTarget()->hasFlags(dm::DataTypeFunctionFlags::Core)) {
                DEBUG("TODO: Running function locally");
                EvalTypeFunction(
                    m_ctxt, 
                    m_thread, 
                    m_vp_id, e->getTarget(), 
                    m_params, true).eval();
            } else {
                ctxtT<IEvalContextInt>()->callFuncReq(
                    m_thread,
                    e->getTarget(),
                    m_params
                );
            }

            if (!hasFlags(EvalFlags::Complete)) {
                break;
            }
        }

        case 2: {
            // Wait for a response
            DEBUG_LEAVE("callFuncReq haveResult=%d", hasFlags(EvalFlags::Complete));
//            setResult(m_thread->getResult());

#ifdef UNDEFINED
            if (m_isreg_res.func
                && !m_isreg_res.is_write
                && m_isreg_res.is_struct) {
                // Must convert the response from integer to struct
                DEBUG("Convert integer return to struct-type value");
                vsc::dm::ValRef val = getResult();
                setResult(dm::TaskUnpackInt2Struct(
                    m_ctxt->getDebugMgr(),
                    m_ctxt->ctxt()).unpack(
                        val, 
                        dynamic_cast<vsc::dm::IDataTypeStruct *>(m_isreg_res.type)));
                vsc::dm::ValRefStruct res(getResult());
                DEBUG("res[0]=0x%08x", vsc::dm::ValRefInt(res.getFieldRef(0)).get_val_u());
            }
#endif
        }
    }

    DEBUG_LEAVE("visitTypeExprMethodCallContext");
}

void EvalTypeExpr::visitTypeExprMethodCallStatic(dm::ITypeExprMethodCallStatic *e) {
    DEBUG_ENTER("visitTypeExprMethodCallStatic idx=%d", m_idx);

    switch (m_idx) {
        case 0: {
            if (m_subidx == 0) {
                m_builtin_i = ctxtT<IEvalContextInt>()->getBuiltinFuncInfo(e->getTarget());
            }

            if (m_subidx > 0 && hasFlags(EvalFlags::Complete)) {
                // Perform some light type conversion if we'll be calling
                // an import function
                if (e->getTarget()->hasFlags(dm::DataTypeFunctionFlags::Import)) {
                    m_params.push_back(
                        TaskConvertImportFuncVal(m_ctxt).convert(getResult()));
                } else {
                    m_params.push_back(getResult());
                }
            }
            while (m_subidx < e->getParameters().size()) {
                EvalTypeExpr evaluator(
                    m_ctxt, 
                    m_thread, 
                    m_vp_id,
                    e->getParameters().at(m_subidx).get());

                m_subidx += 1;
                clrFlags(EvalFlags::Complete);
                if (evaluator.eval()) {
                    break;
                } else if (hasFlags(EvalFlags::Complete)) {
                    // Perform some light type conversion if we'll be calling
                    // an import function
                    if (e->getTarget()->hasFlags(dm::DataTypeFunctionFlags::Import)) {
                        m_params.push_back(
                            TaskConvertImportFuncVal(m_ctxt).convert(getResult()));
                    } else {
                        m_params.push_back(getResult());
                    }
                }
            }

            // If we left the loop before achieving enough
            // parameters, suspend...
            if (m_subidx < e->getParameters().size()) {
                break;
            }

            clrFlags(EvalFlags::Complete); // Clear 'safety' result

            m_idx = 1;
            DEBUG("[%d] Function %s: m_builtin_i=%p", 
                getIdx(), e->getTarget()->name().c_str(), m_builtin_i);
            if (m_builtin_i) {
                m_builtin_i->getImpl()(m_thread, e->getTarget(), m_params);
            } else if (!e->getTarget()->hasFlags(dm::DataTypeFunctionFlags::Import) &&
                !e->getTarget()->hasFlags(dm::DataTypeFunctionFlags::Core)) {
                DEBUG("TODO: Running function locally");
                EvalTypeFunction(
                    m_ctxt, 
                    m_thread, 
                    m_vp_id, 
                    e->getTarget(), 
                    m_params, false).eval();
            } else {
                DEBUG_ENTER("Context: callFuncReq");
                ctxtT<IEvalContextInt>()->callFuncReq(
                    m_thread,
                    e->getTarget(),
                    m_params
                );
            }

            if (!hasFlags(EvalFlags::Complete)) {
                DEBUG("[%d] No result yet ... suspend", getIdx());
                break;
            } else {
                DEBUG("Have a result");
            }
        }

        case 1: {
            // Wait for a response
            DEBUG_LEAVE("callFuncReq hasFlags(Return): %d", hasFlags(EvalFlags::Return));

        }
    }

    DEBUG_LEAVE("visitTypeExprMethodCallStatic idx=%d haveResult=%d", m_idx, hasFlags(EvalFlags::Complete));
}

void EvalTypeExpr::visitTypeExprPyImportRef(dm::ITypeExprPyImportRef *t) {
    DEBUG_ENTER("visitTypeExprPyImportRef");
    pyapi::PyEvalObj *obj = m_ctxt->getPyModule(t->getImport());
    setResult(m_ctxt->ctxt()->mkValPyObj(obj));
    DEBUG_LEAVE("visitTypeExprPyImportRef");
}

void EvalTypeExpr::visitTypeExprPythonFieldRef(dm::ITypeExprPythonFieldRef *t) {
    DEBUG_ENTER("visitTypeExprPythonFieldRef %s", t->getName().c_str());
    t->getBase()->accept(m_this);

    dm::ValRefPyObj base(getResult());

    if (!m_ctxt->getPyEval()->hasAttr(base.getObj(), t->getName())) {
        DEBUG("Error: no attribute in object");
        clrFlags(EvalFlags::Complete);
        setError("Object does not contain name <>");
    } else {
        pyapi::PyEvalObj *obj = m_ctxt->getPyEval()->getAttr(base.getObj(), t->getName());
        DEBUG("obj=%p", obj);

        if (!obj) {
            setError("Failed to get %s attribute", t->getName().c_str());
        } else {
//                    m_ctxt->getPyEval()->Py_IncRef(ret);
            setResult(m_ctxt->ctxt()->mkValPyObj(obj));
        }
    }
    DEBUG_LEAVE("visitTypeExprPythonFieldRef");
}

void EvalTypeExpr::visitTypeExprPythonMethodCall(dm::ITypeExprPythonMethodCall *t) {
    DEBUG_ENTER("visitTypeExprPythonMethodCall");
    switch (m_idx) {
        case 0: {
            m_idx = 1;

            // Obtain the base handle
            if (EvalTypeExpr(m_ctxt, m_thread, m_vp_id, t->getBase()).eval()) {
                break;
            }
        }

        case 1: {
            dm::ValRefPyObj base(getResult());

            if (!base.getObj()) {
                DEBUG("Error: failed to get object");
                clrFlags(EvalFlags::Complete);
                setError("Attempting to invoke <> on null handle" /*, t->getName() */);
                break;
            /*
            } else if (!m_ctxt->getPyEval()->hasAttr(base.getObj(), t->getName())) {
                DEBUG("Error: no attribute in object");
                clrFlags(EvalFlags::Complete);
                setError("Object does not contain name <>");
                break;
             */
            } else {
                pyapi::IPyEval *py_eval = m_ctxt->getPyEval();
                pyapi::PyEvalObj *obj = base.getObj() /*m_ctxt->getPyEval()->getAttr(
                    base.getObj(), t->getName())*/;
                DEBUG("obj=%p", obj);
                pyapi::PyEvalObj *args = m_ctxt->getPyEval()->mkTuple(0);
                pyapi::PyEvalObj *ret = m_ctxt->getPyEval()->call(obj, args, 0);
                DEBUG("ret=%p", ret);

                PyObject *ptype, *pvalue, *ptraceback;
                py_eval->PyErr_Fetch(&ptype, &pvalue, &ptraceback);
                if (ptype || pvalue || ptraceback) {
                    py_eval->PyErr_Display(ptype, pvalue, ptraceback);
                    DEBUG_ERROR("Exception occurred");
                }

                if (!ret) {
                    setError("Failed to evaluate Python method");
                } else {
//                    m_ctxt->getPyEval()->Py_IncRef(ret);
                    setResult(m_ctxt->ctxt()->mkValPyObj(ret));
                }
            } 
        }

        case 2: {
            setFlags(EvalFlags::Complete);
        }
    }

    DEBUG_LEAVE("visitTypeExprPythonMethodCall");
}

void EvalTypeExpr::visitTypeExprPythonModuleRef(dm::ITypeExprPythonModuleRef *t) {
    DEBUG_ENTER("visitTypeExprPythonModuleRef");
    pyapi::PyEvalObj *mod = m_ctxt->getPyModule(t->getImport());
    DEBUG("mod: %p", mod);
    dm::ValRefPyObj val(m_ctxt->ctxt()->mkValPyObj(mod));
    setResult(val);
    DEBUG_LEAVE("visitTypeExprPythonModuleRef");
}


}
}
}
