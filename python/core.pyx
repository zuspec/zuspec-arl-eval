#****************************************************************************
#* core.pyx
#****************************************************************************
import os
import sys
import ctypes
import enum
from libc.stdint cimport intptr_t
from libcpp cimport bool
from libcpp.cast cimport dynamic_cast, static_cast
from libcpp.vector cimport vector as cpp_vector
from enum import IntEnum
cimport zsp_arl_dm.core as arl_dm
from zsp_arl_eval cimport decl
cimport vsc_dm.core as vsc
cimport vsc_dm.decl as vsc_decl
cimport vsc_solvers.core as vsc_solvers
cimport cpython.ref as cpy_ref

class ModelEvaluatorKind(IntEnum):
    FullElab = decl.ModelEvaluatorKind.ModelEvaluatorKind_FullElab
    IncrElab = decl.ModelEvaluatorKind.ModelEvaluatorKind_IncrElab

cdef Factory _inst = None
cdef class Factory(object):

    def __init__(self):
        pass

    cdef init(self, dm_core.Factory f):
        self._hndl.init(f._hndl.getDebugMgr())

    cpdef ModelEvaluator mkModelEvaluator(
        self,
        kind,
        vsc_solvers.Factory solvers_f,
        arl_dm.Context      ctxt):
        cdef int kind_i = int(kind)

        return ModelEvaluator.mk(
            self._hndl.mkModelEvaluator(
                <decl.ModelEvaluatorKind>(kind_i),
                solvers_f._hndl,
                ctxt.asContext()), True)

    @staticmethod
    def inst():
        cdef Factory factory
        global _inst

        if _inst is None:
            ext_dir = os.path.dirname(os.path.abspath(__file__))

            core_lib = os.path.join(ext_dir, "libzsp-arl-eval.so")
            if not os.path.isfile(core_lib):
                raise Exception("Extension library core \"%s\" desn't exist" % core_lib)
            so = ctypes.cdll.LoadLibrary(core_lib)

            func = so.zsp_arl_eval_getFactory
            func.restype = ctypes.c_void_p

            hndl = <decl.IFactoryP>(<intptr_t>(func()))
            factory = Factory()
            factory._hndl = hndl
            factory.init(dm_core.Factory.inst())
            _inst = factory
        return _inst


cdef class ModelEvaluator(object):

    cpdef arl_dm.ModelEvalIterator eval(
        self,
        vsc_solvers.RandState           randstate,
        arl_dm.ModelFieldComponent      root_comp,
        arl_dm.DataTypeAction           root_action):
        return arl_dm.ModelEvalIterator.mk(
            self._hndl.eval(
                randstate._hndl,
                root_comp.asComponent(),
                root_action.asAction()
            ))

    @staticmethod
    cdef ModelEvaluator mk(decl.IModelEvaluator *hndl, bool owned=True):
        ret = ModelEvaluator()
        ret._hndl = hndl
        ret._owned = owned
        return ret

