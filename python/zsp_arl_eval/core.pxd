#****************************************************************************
#* arl_dm_eval core.pxd
#*
#****************************************************************************
import ctypes
cimport debug_mgr.core as dm_core
cimport vsc_solvers.core as vsc_solvers
cimport zsp_arl_dm.core as arl_dm
from zsp_arl_eval cimport decl

from libc.stdint cimport int32_t
from libcpp cimport bool
cimport vsc_dm.core as vsc
cimport vsc_dm.decl as vsc_decl

cdef class Factory(object):
    cdef decl.IFactory      *_hndl

    cdef init(self, dm_core.Factory)

    cpdef ModelEvaluator mkModelEvaluator(
        self,
        kind,
        vsc_solvers.Factory solvers_f,
        arl_dm.Context      ctxt)

cdef class ModelEvaluator(object):
    cdef decl.IModelEvaluator       *_hndl
    cdef bool                       _owned

    cpdef arl_dm.ModelEvalIterator eval(
        self,
        vsc_solvers.RandState           randstate,
        arl_dm.ModelFieldComponent      root_comp,
        arl_dm.DataTypeAction           root_action)

    @staticmethod
    cdef ModelEvaluator mk(decl.IModelEvaluator *hndl, bool owned=*)

    pass
    
    
