/**
 * StringFormatter.h
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
#include <string>
#include <vector>
#include "dmgr/IDebugMgr.h"
#include "vsc/dm/impl/ValRef.h"

namespace zsp {
namespace arl {
namespace eval {



class StringFormatter {
public:
    StringFormatter(dmgr::IDebugMgr *dmgr);

    virtual ~StringFormatter();

    const std::string &format(
        const std::string                   &fmt,
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             idx_a=0);

private:
    double getDoubleArg(
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             &idx);

    uintptr_t getPtrArg(
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             &idx);

    std::string getStrArg(
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             &idx);

    int32_t getI32Arg(
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             &idx);

    uint32_t getU32Arg(
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             &idx);

    int64_t getI64Arg(
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             &idx);

    uint64_t getU64Arg(
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             &idx);


    void out(char c);
    
    void _ntoa_long(unsigned long value, bool negative, unsigned long base, unsigned int prec, unsigned int width, unsigned int flags);

    void _ntoa_format(char* buf, size_t len, bool negative, unsigned int base, unsigned int prec, unsigned int width, unsigned int flags);

    void _ntoa_long_long(unsigned long long value, bool negative, unsigned long long base, unsigned int prec, unsigned int width, unsigned int flags);

    void _ftoa(double value, unsigned int prec, unsigned int width, unsigned int flags);

    void _etoa(double value, unsigned int prec, unsigned int width, unsigned int flags);

    void _out_rev(const char* buf, size_t len, unsigned int width, unsigned int flags);

private:
    static dmgr::IDebug             *m_dbg;
    std::string                     m_tmp;
    std::string                     m_result;


};

}
}
}


