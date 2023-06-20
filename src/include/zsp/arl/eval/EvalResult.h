/**
 * EvalResult.h
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
#pragma once
#include "vsc/dm/IModelVal.h"
#include "vsc/dm/IModelField.h"

namespace zsp {
namespace arl {
namespace eval {

enum class EvalResultKind {
    None,
    Void,
    Val,
    Ref,
    Break,
    Continue
};

union EvalResultVal {
    vsc::dm::IModelValUP    val;
    vsc::dm::IModelField    *ref;
};

struct EvalResult {

    EvalResult() : kind(EvalResultKind::None), ref(0) { }

    EvalResult(const EvalResult &o) : 
        kind(o.kind), ref(o.ref) {
        if (kind == EvalResultKind::Val) {
            if (o.val.get()) {
                val = vsc::dm::IModelValUP(o.val->clone());
            } else {
                val = vsc::dm::IModelValUP();
            }
//            throw "Error: cannot use copy ctor with Val\n";
        }
    }

    EvalResult(EvalResult &o) : 
        kind(o.kind), val(o.val.release()), ref(o.ref) {
    }

    void operator =(EvalResult &o) {
        kind = o.kind;
        val = vsc::dm::IModelValUP(o.val.release());
        ref = o.ref;
    }

    void operator =(EvalResult &&o) {
        kind = o.kind;
        val = vsc::dm::IModelValUP(o.val.release());
        ref = o.ref;
    }

/*
    void operator =(const EvalResult &o) {
        kind = o.kind;
        ref = o.ref;
        val = vsc::dm::IModelValUP(o.val.get());
    }
 */

    EvalResult(EvalResultKind k) : kind(k), ref(0) { }

    EvalResult(vsc::dm::IModelVal *v) : 
        kind(EvalResultKind::Val), val(v), ref(0) {
    }

    EvalResult(EvalResultKind k, vsc::dm::IModelField *r) : 
        kind(k), ref(r) {
    }

    void clear() {
        kind = EvalResultKind::None;
        ref = 0;
        val.reset();
    }

    static EvalResult None() { return EvalResult(EvalResultKind::None); }

    static EvalResult Void() { return EvalResult(EvalResultKind::Void); }

    static EvalResult Break() { return EvalResult(EvalResultKind::Break); }

    static EvalResult Continue() { return EvalResult(EvalResultKind::Continue); }

    EvalResultKind              kind;
    vsc::dm::IModelValUP        val;
    vsc::dm::IModelField        *ref;

};

} /* namespace eval */
} /* namespace arl */
} /* namespace zsp */


