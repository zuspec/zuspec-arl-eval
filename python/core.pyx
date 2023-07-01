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
from libcpp.cast cimport dynamic_cast, static_cast, const_cast
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

    def __dealloc__(self):
        if self._owned:
            del self._hndl

    cpdef bool eval(self):
        return self._hndl.eval()

    cpdef EvalResult getResult(self):
        ret = EvalResult()
        ret._hndl = const_cast[decl.IEvalResultP](self._hndl.getResult())
        ret._owned = False
        return ret

    cpdef void setResult(self, EvalResult r):
        cdef decl.IEvalResult *rp = NULL
        if r is not None:
            if not r._owned:
                raise Exception("Attempting to set result from a non-owned Result object")
            r._owned = False
            rp = r.asResult()
        self._hndl.setResult(rp)

    cpdef EvalResult moveResult(self):
        self._owned = False
        ret = EvalResult.mk(self._hndl.moveResult(), True)
        self._hndl = NULL
        return ret

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

    cpdef EvalResult mkEvalResultVal(self, vsc.ModelVal v):
        return EvalResult.mk(self._hndl.mkEvalResultVal(v._hndl), True)

    cpdef EvalResult mkEvalResultValS(self, int val, int bits=64):
        return EvalResult.mk(self._hndl.mkEvalResultValS(val, bits), True)

    cpdef EvalResult mkEvalResultValU(self, int val, int bits=64):
        return EvalResult.mk(self._hndl.mkEvalResultValU(val, bits), True)

    cpdef EvalResult mkEvalResultKind(self, kind):
        cdef int kind_i = int(kind)
        return EvalResult.mk(self._hndl.mkEvalResultKind(<decl.EvalResultKind>(kind_i)), True)

    cpdef EvalResult mkEvalResultRef(self, vsc.ModelField ref):
        return EvalResult.mk(self._hndl.mkEvalResultRef(ref.asField()), True)

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
        threads_l.append(EvalThread.mk(threads.at(i), False))
    try:
        obj.enterThreads(threads_l)
    except Exception as e:
        print("Exception: %s" % str(e))

cdef public void EvalBackendClosure_enterThread(
    obj,
    decl.IEvalThread *thread) with gil:
    try:
        obj.enterThread(EvalThread.mk(thread, False))
    except Exception as e:
        print("Exception: %s" % str(e))
    
cdef public void EvalBackendClosure_leaveThread(
    obj,
    decl.IEvalThread *thread) with gil:
    try:
        obj.leaveThread(EvalThread.mk(thread, False))
    except Exception as e:
        print("Exception: %s" % str(e))


cdef public void EvalBackendClosure_leaveThreads(
    obj,
    const cpp_vector[decl.IEvalThreadP] &threads) with gil:
    threads_l = []
    for i in range(threads.size()):
        threads_l.append(EvalThread.mk(threads.at(i), False))
    try:
        obj.leaveThreads(threads_l)
    except Exception as e:
        print("Exception: %s" % str(e))

cdef public void EvalBackendClosure_enterAction(
    obj,
    decl.IEvalThread                    *thread,
    arl_dm_decl.IModelFieldAction       *action) with gil:
    obj.enterAction(
        EvalThread.mk(thread, False),
        arl_dm.ModelFieldAction.mk(action, False))

cdef public void EvalBackendClosure_leaveAction(
    obj,
    decl.IEvalThread                    *thread,
    arl_dm_decl.IModelFieldAction       *action) with gil:
    obj.leaveAction(
        EvalThread.mk(thread, False),
        arl_dm.ModelFieldAction.mk(action, False))

cdef public void EvalBackendClosure_callFuncReq(
    obj,
    decl.IEvalThread                        *thread,
    arl_dm_decl.IDataTypeFunction           *func_t,
    const cpp_vector[decl.IEvalResultUP]    &params) with gil:
    cdef decl.IEvalResult *param

    params_l = []
#    for i in range(params.size()):
#        param = params.at(i).get()
#        params_l.append(EvalResult.mk(param, False))

    try:
        obj.callFuncReq(
            EvalThread.mk(thread, False),
            arl_dm.DataTypeFunction.mk(func_t, False),
            params_l)
    except Exception as e:
        print("Exception: %s" % str(e))

class EvalResultKind(IntEnum):
    Void = decl.EvalResultKind.ResultKind_Void
    Val = decl.EvalResultKind.ResultKind_Val
    Ref = decl.EvalResultKind.ResultKind_Ref
    Break = decl.EvalResultKind.ResultKind_Break
    Continue = decl.EvalResultKind.ResultKind_Continue

cdef class EvalResult(vsc.ModelVal):

    cdef decl.IEvalResult *asResult(self):
        return dynamic_cast[decl.IEvalResultP](self._hndl)

    @staticmethod
    cdef EvalResult mk(decl.IEvalResult *hndl, bool owned=True):
        ret = EvalResult()
        ret._hndl = hndl
        ret._owned = owned
        return ret

cdef class EvalThread(Eval):

    cpdef void setThreadId(self, obj):
        cdef decl.EvalThreadData *data;
        data = new decl.EvalThreadData(<cpy_ref.PyObject *>(obj))
        self.asThread().setThreadId(data);
    
    cpdef object getThreadId(self):
        cdef decl.EvalThreadData *data
        data = dynamic_cast[decl.EvalThreadDataP](self.asThread().getThreadId())

        if data == NULL:
            return None
        else:
            return data.getData()

    cpdef EvalResult mkEvalResultVal(self, vsc.ModelVal v):
        return EvalResult.mk(self.asThread().mkEvalResultVal(v._hndl), True)

    cpdef EvalResult mkEvalResultValS(self, int val, int bits=64):
        return EvalResult.mk(self.asThread().mkEvalResultValS(val, bits), True)

    cpdef EvalResult mkEvalResultValU(self, int val, int bits=64):
        return EvalResult.mk(self.asThread().mkEvalResultValU(val, bits), True)

    cpdef EvalResult mkEvalResultKind(self, kind):
        cdef int kind_i = int(kind)
        return EvalResult.mk(self.asThread().mkEvalResultKind(<decl.EvalResultKind>(kind_i)), True)

    cpdef EvalResult mkEvalResultRef(self, vsc.ModelField ref):
        return EvalResult.mk(self.asThread().mkEvalResultRef(ref.asField()), True)

    cdef decl.IEvalThread *asThread(self):
        return dynamic_cast[decl.IEvalThreadP](self._hndl)

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

