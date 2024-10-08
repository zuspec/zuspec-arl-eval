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
from libcpp.cast cimport dynamic_cast, static_cast, const_cast, reinterpret_cast
from libcpp.vector cimport vector as cpp_vector
from enum import IntEnum, IntFlag
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
        self._pyeval = None
        pass

    cdef init(self, dm_core.Factory f):
        self._hndl.init(f._hndl.getDebugMgr())

    cpdef EvalContext mkEvalContextFullElab(
        self,
        vsc_solvers.Factory           solvers_f,
        arl_dm.Context                ctxt,
        vsc_solvers.RandState         randstate,
        arl_dm.DataTypeComponent      root_comp,
        arl_dm.DataTypeAction         root_action,
        EvalBackend                   backend):
        if self._pyeval is None:
            self._pyeval = pyapi.Factory.inst().getPyEval()

        return EvalContext.mk(self._hndl.mkEvalContextFullElab(
            solvers_f._hndl,
            ctxt.asContext(),
            randstate._hndl,
            self._pyeval._hndl,
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
            build_dir = os.path.abspath(os.path.join(ext_dir, "../../build"))

            for libdir in ("lib", "lib64"):
                core_lib = os.path.join(build_dir, libdir, "libzsp-arl-eval.so")
                if os.path.isfile(core_lib):
                    break
                else:
                    core_lib = None

            if core_lib is None:
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

    def emitMessage(self, str msg):
        pass

class EvalFlags(IntFlag):
    NoFlags = decl.EvalFlags.EvalFlags_NoFlags
    Complete = decl.EvalFlags.EvalFlags_Complete
    Error = decl.EvalFlags.EvalFlags_Error
    Break = decl.EvalFlags.EvalFlags_Break
    Continue = decl.EvalFlags.EvalFlags_Continue
    Return = decl.EvalFlags.EvalFlags_Return

cdef class Eval(object):

    def __dealloc__(self):
        if self._owned:
            del self._hndl

    cpdef bool eval(self):
        return self._hndl.eval()

    cpdef bool hasFlags(self, flags):
        cdef int flags_i = int(flags)
        return self._hndl.hasFlags(<decl.EvalFlags>(flags_i))

    cpdef vsc.ValRef getResult(self):
        cdef vsc.ValRef ret = vsc.ValRef()
        ret.val = self._hndl.getResult()
        return ret

    cpdef void setResult(self, vsc.ValRef r, flags=EvalFlags.Complete):
        cdef int flags_i = int(flags)
        self._hndl.setResult(r.val, <decl.EvalFlags>(flags_i))

    @staticmethod
    cdef Eval mk(decl.IEval *hndl, bool owned=True):
        ret = Eval()
        ret._hndl = hndl
        ret._owned = owned
        return ret

cdef class EvalContext(object):

    cpdef bool eval(self):
        return self._hndl.eval()
        
    cpdef bool hasFlags(self, flags):
        cdef int flags_i = int(flags)
        return self._hndl.hasFlags(<decl.EvalFlags>(flags_i))

    cpdef vsc.ValRef getResult(self):
        cdef vsc.ValRef ret = vsc.ValRef()
        ret.val = self._hndl.getResult()
        return ret

    cdef decl.IEvalContext *asContext(self):
        return dynamic_cast[decl.IEvalContextP](self._hndl)

    cpdef getSolveFunctions(self):
        ret = []
        for i in range(self.asContext().getSolveFunctions().size()):
            ret.append(arl_dm.DataTypeFunction.mk(
                self.asContext().getSolveFunctions().at(i), False))
        return ret

    cpdef getTargetFunctions(self):
        ret = []
        for i in range(self._hndl.getTargetFunctions().size()):
            ret.append(arl_dm.DataTypeFunction.mk(
                self._hndl.getTargetFunctions().at(i), False))
        return ret

    cpdef bool addPyModule(self, str name, object mod):
        self._hndl.addPyModule(
            name.encode(),
            reinterpret_cast[pyapi_decl.PyEvalObjP](<cpy_ref.PyObject *>(mod))
        )
        pass

    cpdef vsc.ValRefInt mkValRefInt(self, int value, bool is_signed, int width):
        cdef vsc.ValRefInt ret = vsc.ValRefInt()
        ret.val = self._hndl.mkValRefInt(value, is_signed, width)
        return ret

    cpdef vsc.ValRefInt getAddrHandleValue(self, vsc.ValRef val):
        cdef vsc.ValRefInt ret = vsc.ValRefInt()
        ret.val = self._hndl.getAddrHandleValue(val.val)
        return ret

    # cpdef EvalResult mkEvalResultVal(self, vsc.ModelVal v):
    #     return EvalResult.mk(self._hndl.mkEvalResultVal(v._hndl), True)

    # cpdef EvalResult mkEvalResultValS(self, int val, int bits=64):
    #     return EvalResult.mk(self._hndl.mkEvalResultValS(val, bits), True)

    # cpdef EvalResult mkEvalResultValU(self, int val, int bits=64):
    #     return EvalResult.mk(self._hndl.mkEvalResultValU(val, bits), True)

    # cpdef EvalResult mkEvalResultKind(self, kind):
    #     cdef int kind_i = int(kind)
    #     return EvalResult.mk(self._hndl.mkEvalResultKind(<decl.EvalResultKind>(kind_i)), True)

    # cpdef EvalResult mkEvalResultRef(self, vsc.ModelField ref):
    #     return EvalResult.mk(self._hndl.mkEvalResultRef(ref.asField()), True)

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
    const cpp_vector[vsc_decl.ValRef]       &params) with gil:
#    cdef decl.IEvalResult *param

    params_l = []
    for i in range(params.size()):
#        param = params.at(i).get()
        params_l.append(vsc.ValRef.mk(params.at(i), False))

    try:
        obj.callFuncReq(
            EvalThread.mk(thread, False),
            arl_dm.DataTypeFunction.mk(func_t, False),
            params_l)
    except Exception as e:
        print("Exception: %s" % str(e))

cdef public void EvalBackendClosure_emitMessage(
    obj,
    const cpp_string                    &msg) with gil:
    obj.emitMessage(msg.decode())

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

    cpdef vsc.ValRefInt mkValRefInt(self, int value, bool is_signed, int width):
        cdef vsc.ValRefInt ret = vsc.ValRefInt()
        ret.val = self.asThread().mkValRefInt(value, is_signed, width)
        return ret

    cpdef vsc.ValRefInt getAddrHandleValue(self, vsc.ValRef val):
        cdef vsc.ValRefInt ret = vsc.ValRefInt()
        ret.val = self.asThread().getAddrHandleValue(val.val)
        return ret

    # cpdef EvalResult mkEvalResultVal(self, vsc.ModelVal v):
    #     return EvalResult.mk(self.asThread().mkEvalResultVal(v._hndl), True)

    # cpdef EvalResult mkEvalResultValS(self, int val, int bits=64):
    #     return EvalResult.mk(self.asThread().mkEvalResultValS(val, bits), True)

    # cpdef EvalResult mkEvalResultValU(self, int val, int bits=64):
    #     return EvalResult.mk(self.asThread().mkEvalResultValU(val, bits), True)

    # cpdef EvalResult mkEvalResultKind(self, kind):
    #     cdef int kind_i = int(kind)
    #     return EvalResult.mk(self.asThread().mkEvalResultKind(<decl.EvalResultKind>(kind_i)), True)

    # cpdef EvalResult mkEvalResultRef(self, vsc.ModelField ref):
    #     return EvalResult.mk(self.asThread().mkEvalResultRef(ref.asField()), True)

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

# if "pss" not in sys.modules.keys():
#     print("load pss")
#     import zsp_arl_eval.impl.pss_module as pss
#     sys.modules["pss"] = pss
