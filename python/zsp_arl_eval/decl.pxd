#****************************************************************************
#* arl_dm_eval decl.pxd
#*
#****************************************************************************
cimport vsc_dm.decl as vsc
cimport vsc_solvers.decl as vsc_solvers
cimport zsp_arl_dm.decl as arl_dm
cimport debug_mgr.decl as dm

from libcpp.string cimport string as cpp_string
from libcpp.vector cimport vector as cpp_vector
from libcpp.memory cimport unique_ptr
from libcpp cimport bool
from libc.stdint cimport int32_t
cimport cpython.ref as cpy_ref

ctypedef EvalThreadData *EvalThreadDataP
ctypedef IFactory *IFactoryP
ctypedef IEvalThread *IEvalThreadP
ctypedef IEvalThreadId *IEvalThreadIdP

cdef extern from "zsp/arl/eval/IFactory.h" namespace "zsp::arl::eval":
    cdef cppclass IFactory:
        void init(dm.IDebugMgr *)

        IModelEvaluator *mkModelEvaluator(
            ModelEvaluatorKind      kind,
            vsc_solvers.IFactory    *solvers_f,
            arl_dm.IContext         *ctxt)

        IEvalContext *mkEvalContextFullElab(
            vsc_solvers.IFactory            *solvers_f,
            arl_dm.IContext                 *ctxt,
            const vsc_solvers.IRandState    *randstate,
            arl_dm.IModelFieldComponent     *root_comp,
            arl_dm.IDataTypeAction          *root_action,
            IEvalBackend                    *backend)

cdef extern from "zsp/arl/eval/EvalResult.h" namespace "zsp::arl::eval":
    cdef cppclass EvalResult:
        pass

cdef extern from "zsp/arl/eval/IEval.h" namespace "zsp::arl::eval":
    cdef cppclass IEval:
        bool eval()
        void setResult(const EvalResult &)


cdef extern from "zsp/arl/eval/IEvalBackend.h" namespace "zsp::arl::eval":
    cdef cppclass IEvalBackend:
        void enterThreads(const cpp_vector[IEvalThreadP] &)

        void enterThread(IEvalThread *)

        void leaveThread(IEvalThread *)

        void leaveThreads(const cpp_vector[IEvalThreadP] &)

        void callFuncReq(
            IEvalThread                     *thread,
            arl_dm.IDataTypeFunction        *func_t,
            const cpp_vector[EvalResult]    &params)

cdef extern from "zsp/arl/eval/IEvalContext.h" namespace "zsp::arl::eval":
    cdef cppclass IEvalContext(IEval):
        IEvalBackend *getBackend() const
        void setBackend(IEvalBackend *b)
        const cpp_vector[arl_dm.IDataTypeFunctionP] &getFunctions() const
#        const cpp_vector[arl_dm.IModelFieldExecutor] &getFunctions() const

cdef extern from "EvalBackendClosure.h" namespace "zsp::arl::eval":
    cdef cppclass EvalBackendClosure(IEvalBackend):
        EvalBackendClosure(cpy_ref.PyObject *)

cdef extern from "zsp/arl/eval/IEvalThreadId.h" namespace "zsp::arl::eval":
    cdef cppclass IEvalThreadId:
        pass

cdef extern from "zsp/arl/eval/IEvalThread.h" namespace "zsp::arl::eval":
    cdef cppclass IEvalThread:
        void setThreadId(IEvalThreadId *)
        IEvalThreadId *getThreadId()
        pass

cdef extern from "EvalThreadData.h" namespace "zsp::arl::eval":
    cdef cppclass EvalThreadData(IEvalThreadId):
        EvalThreadData(cpy_ref.PyObject *)
        object getData() const

cdef extern from "zsp/arl/eval/IModelEvaluator.h" namespace "zsp::arl::eval":
    cdef enum ModelEvaluatorKind "zsp::arl::eval::ModelEvaluatorKind" :
        ModelEvaluatorKind_FullElab "zsp::arl::eval::ModelEvaluatorKind::FullElab"
        ModelEvaluatorKind_IncrElab "zsp::arl::eval::ModelEvaluatorKind::IncrElab"

    cdef cppclass IModelEvaluator:
        arl_dm.IModelEvalIterator *eval(
            const vsc_solvers.IRandState    *randstate,
            arl_dm.IModelFieldComponent     *root_comp,
            arl_dm.IDataTypeAction          *root_action)


