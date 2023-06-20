#****************************************************************************
#* core.pyx
#*
#* Copyright 2023 Matthew Ballance and Contributors
#*
#* Licensed under the Apache License, Version 2.0 (the "License"); you may 
#* not use this file except in compliance with the License.  
#* You may obtain a copy of the License at:
#*
#*   http://www.apache.org/licenses/LICENSE-2.0
#*
#* Unless required by applicable law or agreed to in writing, software 
#* distributed under the License is distributed on an "AS IS" BASIS, 
#* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
#* See the License for the specific language governing permissions and 
#* limitations under the License.
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
cimport zsp_arl_dm.decl as arl_dm_decl
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

    cpdef EvalContext mkEvalContextFullElab(
        self,
        vsc_solvers.Factory           solvers_f,
        arl_dm.Context                ctxt,
        vsc_solvers.RandState         randstate,
        arl_dm.ModelFieldComponent    root_comp,
        arl_dm.DataTypeAction         root_action,
        EvalBackend                   backend):
        return EvalContext.mk(self._hndl.mkEvalContextFullElab(
            solvers_f._hndl,
            ctxt.asContext(),
            randstate._hndl,
            root_comp.asComponent(),
            root_action.asAction(),
            backend._hndl
        ), False)

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

cdef class EvalBackend(object):

    def __init__(self):
        self._hndl = new decl.EvalBackendClosure(<cpy_ref.PyObject *>(self))
        pass

    def enterThreads(self, threads):
        pass

    def enterThread(self, thread):
        pass

    def leaveThread(self, thread):
        pass

    def leaveThreads(self, threads):
        pass

    def enterAction(self, thread, action):
        pass

    def leaveAction(self, thread, action):
        pass

    def callFuncReq(self, thread, func_t, params):
        pass

cdef class Eval(object):

    cpdef bool eval(self):
        return self._hndl.eval()

    @staticmethod
    cdef Eval mk(decl.IEval *hndl, bool owned=True):
        ret = Eval()
        ret._hndl = hndl
        ret._owned = owned
        return ret

cdef class EvalContext(object):

    cpdef bool eval(self):
        return self._hndl.eval()

    cpdef getFunctions(self):
        ret = []
        for i in range(self._hndl.getFunctions().size()):
            ret.append(arl_dm.DataTypeFunction.mk(
                self._hndl.getFunctions().at(i), False))
        return ret

    @staticmethod
    cdef EvalContext mk(decl.IEvalContext *hndl, bool owned=True):
        ret = EvalContext()
        ret._hndl = hndl
        ret._owned = owned
        return ret

#********************************************************************
#* Export methods for EvalBackendClosure
#********************************************************************
cdef public void EvalBackendClosure_enterThreads(
    obj,
    const cpp_vector[decl.IEvalThreadP] &threads) with gil:
    threads_l = []
    for i in range(threads.size()):
        threads_l.append(EvalThread.mk(threads.at(i)))
    obj.startThreads(threads_l)

cdef public void EvalBackendClosure_enterThread(
    obj,
    decl.IEvalThread *thread) with gil:
    obj.enterThread(EvalThread.mk(thread, False))
    
cdef public void EvalBackendClosure_leaveThread(
    obj,
    decl.IEvalThread *thread) with gil:
    obj.leaveThread(EvalThread.mk(thread, False))

cdef public void EvalBackendClosure_leaveThreads(
    obj,
    const cpp_vector[decl.IEvalThreadP] &threads) with gil:
    threads_l = []
    for i in range(threads.size()):
        threads_l.append(EvalThread.mk(threads.at(i)))
    obj.leaveThreads(threads_l)

cdef public void EvalBackendClosure_enterAction(
    obj,
    decl.IEvalThread                    *thread,
    arl_dm_decl.IModelFieldAction       *action):
    obj.enterAction(
        EvalThread.mk(thread, False),
        arl_dm.ModelFieldAction.mk(action, False))

cdef public void EvalBackendClosure_leaveAction(
    obj,
    decl.IEvalThread                    *thread,
    arl_dm_decl.IModelFieldAction       *action):
    obj.leaveAction(
        EvalThread.mk(thread, False),
        arl_dm.ModelFieldAction.mk(action, False))

cdef public void EvalBackendClosure_callFuncReq(
    obj,
    decl.IEvalThread                     *thread,
    arl_dm_decl.IDataTypeFunction        *func_t,
    const cpp_vector[decl.EvalResult]    &params):
    cdef decl.EvalResult *param

    params_l = []
    for i in range(params.size()):
        param = &params.at(i)
        params_l.append(EvalResult.mk(param, False))
    
    obj.callFuncReq(
        EvalThread.mk(thread, False),
        arl_dm.DataTypeFunction.mk(func_t, False),
        params_l)
    pass

cdef class EvalResult(object):

    @staticmethod
    cdef EvalResult mk(decl.EvalResult *hndl, bool owned=True):
        ret = EvalResult()
        ret._hndl = hndl
        ret._owned = owned
        return ret

cdef class EvalThread(object):

    def __dealloc__(self):
        if self._owned:
            del self._hndl

    cpdef void setThreadId(self, obj):
        cdef decl.EvalThreadData *data;
        data = new decl.EvalThreadData(<cpy_ref.PyObject *>(obj))
        self._hndl.setThreadId(data);
    
    cpdef getThreadId(self):
        cdef decl.EvalThreadData *data
        data = dynamic_cast[decl.EvalThreadDataP](self._hndl.getThreadId())

        if data == NULL:
            return None
        else:
            return data.getData()

    @staticmethod
    cdef EvalThread mk(decl.IEvalThread *hndl, bool owned=True):
        ret = EvalThread()
        ret._hndl = hndl
        ret._owned = owned
        return ret

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

