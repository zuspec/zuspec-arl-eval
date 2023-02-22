#****************************************************************************
#* arl_dm_eval core.pxd
#*
#****************************************************************************
import ctypes
cimport debug_mgr.core as dm_core
from zsp_arl_eval cimport decl

from libc.stdint cimport int32_t
from libcpp cimport bool
cimport libvsc_dm.core as vsc
cimport libvsc_dm.decl as vsc_decl

cdef class Factory(object):
    cdef decl.IFactory      *_hndl

    cdef init(self, dm_core.Factory)

cdef class ModelEvaluator(object):
    pass
    
    
