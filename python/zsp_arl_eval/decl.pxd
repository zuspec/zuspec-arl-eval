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

ctypedef IFactory *IFactoryP

cdef extern from "zsp/arl/eval/IFactory.h" namespace "zsp::arl::eval":
    cdef cppclass IFactory:
        void init(dm.IDebugMgr *)

        IModelEvaluator *mkModelEvaluator(
            ModelEvaluatorKind      kind,
            vsc_solvers.IFactory    *solvers_f,
            arl_dm.IContext         *ctxt)

cdef extern from "zsp/arl/eval/IModelEvaluator.h" namespace "zsp::arl::eval":
    cdef enum ModelEvaluatorKind "zsp::arl::eval::ModelEvaluatorKind" :
        ModelEvaluatorKind_FullElab "zsp::arl::eval::ModelEvaluatorKind::FullElab"
        ModelEvaluatorKind_IncrElab "zsp::arl::eval::ModelEvaluatorKind::IncrElab"

    cdef cppclass IModelEvaluator:
        arl_dm.IModelEvalIterator *eval(
            const vsc_solvers.IRandState    *randstate,
            arl_dm.IModelFieldComponent     *root_comp,
            arl_dm.IDataTypeAction          *root_action)


