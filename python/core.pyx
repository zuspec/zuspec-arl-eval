#****************************************************************************
#* core.pyx
#****************************************************************************
import os
import sys
import ctypes
from libc.stdint cimport intptr_t
from libcpp cimport bool
from libcpp.cast cimport dynamic_cast, static_cast
from libcpp.vector cimport vector as cpp_vector
from enum import IntEnum
from zsp_arl_eval cimport decl
cimport libvsc_dm.core as vsc
cimport libvsc_dm.decl as vsc_decl
cimport cpython.ref as cpy_ref

cdef Factory _inst = None
cdef class Factory(object):

    def __init__(self):
        pass

    cdef init(self, dm_core.Factory f):
        self._hndl.init(f._hndl.getDebugMgr())
        
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

            func = so.zsp_arl_dm_getFactory
            func.restype = ctypes.c_void_p

            hndl = <decl.IFactoryP>(<intptr_t>(func()))
            factory = Factory()
            factory._hndl = hndl
            factory.init(dm_core.Factory.inst())
            _inst = factory
        return _inst


cdef class ModelEvaluator(object):
    pass
