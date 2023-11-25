/*
 * StringFormatter.cpp
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
 * Note: this file makes use of code from https://github.com/mpaland/printf,
 * which is released under the MIT license. The license header from that
 * work is included here:
// \author (c) Marco Paland (info@paland.com)
//             2014-2019, PALANDesign Hannover, Germany
//
// \license The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
 * 
 * 
 * Created on:
 *     Author:
 */
#include "dmgr/impl/DebugMacros.h"
#include "vsc/dm/impl/ValRefInt.h"
#include "vsc/dm/impl/ValRefStr.h"
#include "vsc/dm/impl/ValRefStruct.h"
#include "StringFormatter.h"

// internal flag definitions
#define FLAGS_ZEROPAD   (1U <<  0U)
#define FLAGS_LEFT      (1U <<  1U)
#define FLAGS_PLUS      (1U <<  2U)
#define FLAGS_SPACE     (1U <<  3U)
#define FLAGS_HASH      (1U <<  4U)
#define FLAGS_UPPERCASE (1U <<  5U)
#define FLAGS_CHAR      (1U <<  6U)
#define FLAGS_SHORT     (1U <<  7U)
#define FLAGS_LONG      (1U <<  8U)
#define FLAGS_LONG_LONG (1U <<  9U)
#define FLAGS_PRECISION (1U << 10U)
#define FLAGS_ADAPT_EXP (1U << 11U)


#ifndef PRINTF_NTOA_BUFFER_SIZE
#define PRINTF_NTOA_BUFFER_SIZE    32U
#endif

// 'ftoa' conversion buffer size, this must be big enough to hold one converted
// float number including padded zeros (dynamically created on stack)
// default: 32 byte
#ifndef PRINTF_FTOA_BUFFER_SIZE
#define PRINTF_FTOA_BUFFER_SIZE    32U
#endif

// support for the floating point type (%f)
// default: activated
#ifndef PRINTF_DISABLE_SUPPORT_FLOAT
#define PRINTF_SUPPORT_FLOAT
#endif

// support for exponential floating point notation (%e/%g)
// default: activated
#ifndef PRINTF_DISABLE_SUPPORT_EXPONENTIAL
#define PRINTF_SUPPORT_EXPONENTIAL
#endif

// define the default floating point precision
// default: 6 digits
#ifndef PRINTF_DEFAULT_FLOAT_PRECISION
#define PRINTF_DEFAULT_FLOAT_PRECISION  6U
#endif

// define the largest float suitable to print with %f
// default: 1e9
#ifndef PRINTF_MAX_FLOAT
#define PRINTF_MAX_FLOAT  1e9
#endif

// support for the long long types (%llu or %p)
// default: activated
#ifndef PRINTF_DISABLE_SUPPORT_LONG_LONG
#define PRINTF_SUPPORT_LONG_LONG
#endif

// support for the ptrdiff_t type (%t)
// ptrdiff_t is normally defined in <stddef.h> as long or long long type
// default: activated
#ifndef PRINTF_DISABLE_SUPPORT_PTRDIFF_T
#define PRINTF_SUPPORT_PTRDIFF_T
#endif

// import float.h for DBL_MAX
#if defined(PRINTF_SUPPORT_FLOAT)
#include <float.h>
#endif

namespace zsp {
namespace arl {
namespace eval {


StringFormatter::StringFormatter(dmgr::IDebugMgr *dmgr) {
    DEBUG_INIT("zsp::arl::eval::StringFormatter", dmgr);
}

StringFormatter::~StringFormatter() {

}

// internal test if char is a digit (0-9)
// \return true if char is a digit
static inline bool _is_digit(char ch)
{
  return (ch >= '0') && (ch <= '9');
}

// internal ASCII string to unsigned int conversion
static unsigned int _atoi(const std::string &str, uint32_t &idx) {
  unsigned int i = 0U;
  while (_is_digit(str.at(idx))) {
    i = i * 10U + (unsigned int)(str.at(idx) - '0');
    idx++;
  }
  return i;
}

// // internal secure strlen
// // \return The length of the string (excluding the terminating 0) limited by 'maxsize'
// static inline unsigned int _strnlen_s(const std::string &str, uint32_t idx, size_t maxsize) {
//   const char* s;
//   for (s = str; *s && maxsize--; ++s);
//   return (unsigned int)(s - str);
// }

const std::string &StringFormatter::format(
        const std::string                   &fmt,
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             idx_a) {
    m_result.clear();

    unsigned int flags, width, precision, n; 
    uint32_t idx_f=0;
//  size_t idx = 0U;

    while (idx_f < fmt.size()) {
    // format specifier?  %[flags][width][.precision][length]
    if (fmt.at(idx_f) != '%') {
      // no
      out(fmt.at(idx_f));
      idx_f++;
      continue;
    }
    else {
      // yes, evaluate it
      idx_f++;
    }

    // evaluate flags
    flags = 0U;
    do {
      switch (fmt.at(idx_f)) {
        case '0': flags |= FLAGS_ZEROPAD; idx_f++; n = 1U; break;
        case '-': flags |= FLAGS_LEFT;    idx_f++; n = 1U; break;
        case '+': flags |= FLAGS_PLUS;    idx_f++; n = 1U; break;
        case ' ': flags |= FLAGS_SPACE;   idx_f++; n = 1U; break;
        case '#': flags |= FLAGS_HASH;    idx_f++; n = 1U; break;
        default :                                   n = 0U; break;
      }
    } while (n);

    // evaluate width field
    width = 0U;
    if (_is_digit(fmt.at(idx_f))) {
      width = _atoi(fmt, idx_f);
    } else if (fmt.at(idx_f) == '*') {
      const int w = getI32Arg(args, idx_a);
      if (w < 0) {
        flags |= FLAGS_LEFT;    // reverse padding
        width = (unsigned int)-w;
      } else {
        width = (unsigned int)w;
      }
      idx_f++;
    }

    // evaluate precision field
    precision = 0U;
    if (fmt.at(idx_f) == '.') {
      flags |= FLAGS_PRECISION;
      idx_f++;
      if (_is_digit(fmt.at(idx_f))) {
        precision = _atoi(fmt, idx_f);
      }
      else if (fmt.at(idx_f) == '*') {
        const int prec = getI32Arg(args, idx_a);
        precision = prec > 0 ? (unsigned int)prec : 0U;
        idx_f++;
      }
    }

    // evaluate length field
    switch (fmt.at(idx_f)) {
      case 'l' :
        flags |= FLAGS_LONG;
        idx_f++;
        if (fmt.at(idx_f) == 'l') {
          flags |= FLAGS_LONG_LONG;
          idx_f++;
        }
        break;
      case 'h' :
        flags |= FLAGS_SHORT;
        idx_f++;
        if (fmt.at(idx_f) == 'h') {
          flags |= FLAGS_CHAR;
          idx_f++;
        }
        break;
#if defined(PRINTF_SUPPORT_PTRDIFF_T)
      case 't' :
        flags |= (sizeof(ptrdiff_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
        idx_f++;
        break;
#endif
      case 'j' :
        flags |= (sizeof(intmax_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
        idx_f++;
        break;
      case 'z' :
        flags |= (sizeof(size_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
        idx_f++;
        break;
      default :
        break;
    }

    // evaluate specifier
    switch (fmt.at(idx_f)) {
      case 'd' :
      case 'i' :
      case 'u' :
      case 'x' :
      case 'X' :
      case 'o' :
      case 'b' : {
        // set the base
        unsigned int base;
        if (fmt.at(idx_f) == 'x' || fmt.at(idx_f) == 'X') {
          base = 16U;
        }
        else if (fmt.at(idx_f) == 'o') {
          base =  8U;
        }
        else if (fmt.at(idx_f) == 'b') {
          base =  2U;
        }
        else {
          base = 10U;
          flags &= ~FLAGS_HASH;   // no hash for dec format
        }
        // uppercase
        if (fmt.at(idx_f) == 'X') {
          flags |= FLAGS_UPPERCASE;
        }

        // no plus or space flag for u, x, X, o, b
        if ((fmt.at(idx_f) != 'i') && (fmt.at(idx_f) != 'd')) {
          flags &= ~(FLAGS_PLUS | FLAGS_SPACE);
        }

        // ignore '0' flag when precision is given
        if (flags & FLAGS_PRECISION) {
          flags &= ~FLAGS_ZEROPAD;
        }

        // convert the integer
        if ((fmt.at(idx_f) == 'i') || (fmt.at(idx_f) == 'd')) {
          // signed
          if (flags & FLAGS_LONG_LONG) {
#if defined(PRINTF_SUPPORT_LONG_LONG)
            const long long value = getI64Arg(args, idx_a);
            _ntoa_long_long((unsigned long long)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
#endif
          } else if (flags & FLAGS_LONG) {
            const long value = getI32Arg(args, idx_a);
            _ntoa_long((unsigned long)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
          } else {
            const int value = (flags & FLAGS_CHAR) ? (char)getI32Arg(args, idx_a) : (flags & FLAGS_SHORT) ? (short int)getI32Arg(args, idx_a) : getI32Arg(args, idx_a);
            _ntoa_long((unsigned int)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
          }
        } else {
          // unsigned
          if (flags & FLAGS_LONG_LONG) {
#if defined(PRINTF_SUPPORT_LONG_LONG)
            _ntoa_long_long(getU64Arg(args, idx_a), false, base, precision, width, flags);
#endif
          } else if (flags & FLAGS_LONG) {
            _ntoa_long(getU32Arg(args, idx_a), false, base, precision, width, flags);
          }
          else {
            const unsigned int value = (flags & FLAGS_CHAR) ? (unsigned char)getU32Arg(args, idx_a) : (flags & FLAGS_SHORT) ? (unsigned short int)getU32Arg(args, idx_a) : getU32Arg(args, idx_a);
            _ntoa_long(value, false, base, precision, width, flags);
          }
        }
        idx_f++;
        break;
      }
#if defined(PRINTF_SUPPORT_FLOAT)
      case 'f' :
      case 'F' :
        if (fmt.at(idx_f) == 'F') flags |= FLAGS_UPPERCASE;
        _ftoa(getDoubleArg(args, idx_a), precision, width, flags);
        idx_f++;
        break;
#if defined(PRINTF_SUPPORT_EXPONENTIAL)
      case 'e':
      case 'E':
      case 'g':
      case 'G':
        if ((fmt.at(idx_f) == 'g')||(fmt.at(idx_f) == 'G')) flags |= FLAGS_ADAPT_EXP;
        if ((fmt.at(idx_f) == 'E')||(fmt.at(idx_f) == 'G')) flags |= FLAGS_UPPERCASE;
        _etoa(getDoubleArg(args, idx_a), precision, width, flags);
        idx_f++;
        break;
#endif  // PRINTF_SUPPORT_EXPONENTIAL
#endif  // PRINTF_SUPPORT_FLOAT
      case 'c' : {
        unsigned int l = 1U;
        // pre padding
        if (!(flags & FLAGS_LEFT)) {
          while (l++ < width) {
            out(' ');
          }
        }
        // char output
        out((char)getI32Arg(args, idx_a));
        // post padding
        if (flags & FLAGS_LEFT) {
          while (l++ < width) {
            out(' ');
          }
        }
        idx_f++;
        break;
      }

      case 's' : {
        uint32_t idx_s = 0;
        const std::string p = getStrArg(args, idx_a);
        unsigned int l = p.size(); // _strnlen_s(p, precision ? precision : (size_t)-1);
        // pre padding
        if (flags & FLAGS_PRECISION) {
          l = (l < precision ? l : precision);
        }
        if (!(flags & FLAGS_LEFT)) {
          while (l++ < width) {
            out(' ');
          }
        }
        // string output
        while (idx_s < l && (!(flags & FLAGS_PRECISION) || precision--)) {
          out(p.at(idx_s));
        }
        // post padding
        if (flags & FLAGS_LEFT) {
          while (l++ < width) {
            out(' ');
          }
        }
        idx_f++;
        break;
      }

      case 'p' : {
        width = sizeof(void*) * 2U;
        flags |= FLAGS_ZEROPAD | FLAGS_UPPERCASE;
#if defined(PRINTF_SUPPORT_LONG_LONG)
        const bool is_ll = sizeof(uintptr_t) == sizeof(long long);
        if (is_ll) {
          _ntoa_long_long((uintptr_t)getPtrArg(args, idx_a), false, 16U, precision, width, flags);
        }
        else {
#endif
          _ntoa_long((unsigned long)((uintptr_t)getPtrArg(args, idx_a)), false, 16U, precision, width, flags);
#if defined(PRINTF_SUPPORT_LONG_LONG)
        }
#endif
        idx_f++;
        break;
      }

      case '%' :
        out('%');
        idx_f++;
        break;

      default :
        out(fmt.at(idx_f));
        idx_f++;
        break;
    }
  }

  // termination

    return m_result;
}

double StringFormatter::getDoubleArg(
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             &idx) {
    double ret = 0;

    if (idx >= 0 && idx < args.size()) {
        // TODO:
//        vsc::dm::ValRefInt val_i(args.at(idx));
        idx++;
//        ret = val_i.get_val_s();
    }
    return ret;
}

uintptr_t StringFormatter::getPtrArg(
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             &idx) {
    uintptr_t ret = -1;

    if (idx >= 0 && idx < args.size()) {
        vsc::dm::ValRefInt val_i(args.at(idx));
        idx++;
        ret = val_i.get_val_u();
    }
    return ret;
}

std::string StringFormatter::getStrArg(
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             &idx) {
    if (idx >= 0 && idx < args.size()) {
        vsc::dm::ValRefStr val_s(args.at(idx));
        idx++;
        return val_s.val_s();
    } else {
        return "";
    }
}

int32_t StringFormatter::getI32Arg(
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             &idx) {
    int32_t ret = -1;

    if (idx >= 0 && idx < args.size()) {
        vsc::dm::ValRefInt val_i(args.at(idx));
        idx++;
        ret = val_i.get_val_s();
    }
    return ret;
}

uint32_t StringFormatter::getU32Arg(
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             &idx) {
    uint32_t ret = -1;

    if (idx >= 0 && idx < args.size()) {
        vsc::dm::ValRefInt val_i(args.at(idx));
        idx++;
        ret = val_i.get_val_u();
    }
    return ret;
}

int64_t StringFormatter::getI64Arg(
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             &idx) {
    int64_t ret = -1;

    if (idx >= 0 && idx < args.size()) {
        vsc::dm::ValRefInt val_i(args.at(idx));
        idx++;
        ret = val_i.get_val_s();
    }
    return ret;
}

uint64_t StringFormatter::getU64Arg(
        const std::vector<vsc::dm::ValRef>  &args,
        int32_t                             &idx) {
    uint64_t ret = 0;

    if (idx >= 0 && idx < args.size()) {
        vsc::dm::ValRefInt val_i(args.at(idx));
        idx++;
        ret = val_i.get_val_u();
    }
    return ret;
}

void StringFormatter::out(char c) {
    m_result.push_back(c);
}

void StringFormatter::_ntoa_long(unsigned long value, bool negative, unsigned long base, unsigned int prec, unsigned int width, unsigned int flags) {
  char buf[PRINTF_NTOA_BUFFER_SIZE];
  size_t len = 0U;

  // no hash for 0 values
  if (!value) {
    flags &= ~FLAGS_HASH;
  }

  // write if precision != 0 and value is != 0
  if (!(flags & FLAGS_PRECISION) || value) {
    do {
      const char digit = (char)(value % base);
      buf[len++] = digit < 10 ? '0' + digit : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10;
      value /= base;
    } while (value && (len < PRINTF_NTOA_BUFFER_SIZE));
  }

  return _ntoa_format(buf, len, negative, (unsigned int)base, prec, width, flags);
}

// internal itoa format
void StringFormatter::_ntoa_format(char* buf, size_t len, bool negative, unsigned int base, unsigned int prec, unsigned int width, unsigned int flags) {
  // pad leading zeros
  if (!(flags & FLAGS_LEFT)) {
    if (width && (flags & FLAGS_ZEROPAD) && (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
      width--;
    }
    while ((len < prec) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
      buf[len++] = '0';
    }
    while ((flags & FLAGS_ZEROPAD) && (len < width) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
      buf[len++] = '0';
    }
  }

  // handle hash
  if (flags & FLAGS_HASH) {
    if (!(flags & FLAGS_PRECISION) && len && ((len == prec) || (len == width))) {
      len--;
      if (len && (base == 16U)) {
        len--;
      }
    }
    if ((base == 16U) && !(flags & FLAGS_UPPERCASE) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
      buf[len++] = 'x';
    }
    else if ((base == 16U) && (flags & FLAGS_UPPERCASE) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
      buf[len++] = 'X';
    }
    else if ((base == 2U) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
      buf[len++] = 'b';
    }
    if (len < PRINTF_NTOA_BUFFER_SIZE) {
      buf[len++] = '0';
    }
  }

  if (len < PRINTF_NTOA_BUFFER_SIZE) {
    if (negative) {
      buf[len++] = '-';
    }
    else if (flags & FLAGS_PLUS) {
      buf[len++] = '+';  // ignore the space if the '+' exists
    }
    else if (flags & FLAGS_SPACE) {
      buf[len++] = ' ';
    }
  }

  return _out_rev(buf, len, width, flags);
}

void StringFormatter::_ntoa_long_long(unsigned long long value, bool negative, unsigned long long base, unsigned int prec, unsigned int width, unsigned int flags) {
  char buf[PRINTF_NTOA_BUFFER_SIZE];
  size_t len = 0U;

  // no hash for 0 values
  if (!value) {
    flags &= ~FLAGS_HASH;
  }

  // write if precision != 0 and value is != 0
  if (!(flags & FLAGS_PRECISION) || value) {
    do {
      const char digit = (char)(value % base);
      buf[len++] = digit < 10 ? '0' + digit : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10;
      value /= base;
    } while (value && (len < PRINTF_NTOA_BUFFER_SIZE));
  }

  return _ntoa_format(buf, len, negative, (unsigned int)base, prec, width, flags);
}

// internal ftoa for fixed decimal floating point
void StringFormatter::_ftoa(double value, unsigned int prec, unsigned int width, unsigned int flags) {
  char buf[PRINTF_FTOA_BUFFER_SIZE];
  size_t len  = 0U;
  double diff = 0.0;

  // powers of 10
  static const double pow10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };

  // test for special values
  if (value != value)
    return _out_rev("nan", 3, width, flags);
  if (value < -DBL_MAX)
    return _out_rev("fni-", 4, width, flags);
  if (value > DBL_MAX)
    return _out_rev((flags & FLAGS_PLUS) ? "fni+" : "fni", (flags & FLAGS_PLUS) ? 4U : 3U, width, flags);

  // test for very large values
  // standard printf behavior is to print EVERY whole number digit -- which could be 100s of characters overflowing your buffers == bad
  if ((value > PRINTF_MAX_FLOAT) || (value < -PRINTF_MAX_FLOAT)) {
#if defined(PRINTF_SUPPORT_EXPONENTIAL)
    return _etoa(value, prec, width, flags);
#else
    return 0U;
#endif
  }

  // test for negative
  bool negative = false;
  if (value < 0) {
    negative = true;
    value = 0 - value;
  }

  // set default precision, if not set explicitly
  if (!(flags & FLAGS_PRECISION)) {
    prec = PRINTF_DEFAULT_FLOAT_PRECISION;
  }
  // limit precision to 9, cause a prec >= 10 can lead to overflow errors
  while ((len < PRINTF_FTOA_BUFFER_SIZE) && (prec > 9U)) {
    buf[len++] = '0';
    prec--;
  }

  int whole = (int)value;
  double tmp = (value - whole) * pow10[prec];
  unsigned long frac = (unsigned long)tmp;
  diff = tmp - frac;

  if (diff > 0.5) {
    ++frac;
    // handle rollover, e.g. case 0.99 with prec 1 is 1.0
    if (frac >= pow10[prec]) {
      frac = 0;
      ++whole;
    }
  }
  else if (diff < 0.5) {
  }
  else if ((frac == 0U) || (frac & 1U)) {
    // if halfway, round up if odd OR if last digit is 0
    ++frac;
  }

  if (prec == 0U) {
    diff = value - (double)whole;
    if ((!(diff < 0.5) || (diff > 0.5)) && (whole & 1)) {
      // exactly 0.5 and ODD, then round up
      // 1.5 -> 2, but 2.5 -> 2
      ++whole;
    }
  }
  else {
    unsigned int count = prec;
    // now do fractional part, as an unsigned number
    while (len < PRINTF_FTOA_BUFFER_SIZE) {
      --count;
      buf[len++] = (char)(48U + (frac % 10U));
      if (!(frac /= 10U)) {
        break;
      }
    }
    // add extra 0s
    while ((len < PRINTF_FTOA_BUFFER_SIZE) && (count-- > 0U)) {
      buf[len++] = '0';
    }
    if (len < PRINTF_FTOA_BUFFER_SIZE) {
      // add decimal
      buf[len++] = '.';
    }
  }

  // do whole part, number is reversed
  while (len < PRINTF_FTOA_BUFFER_SIZE) {
    buf[len++] = (char)(48 + (whole % 10));
    if (!(whole /= 10)) {
      break;
    }
  }

  // pad leading zeros
  if (!(flags & FLAGS_LEFT) && (flags & FLAGS_ZEROPAD)) {
    if (width && (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
      width--;
    }
    while ((len < width) && (len < PRINTF_FTOA_BUFFER_SIZE)) {
      buf[len++] = '0';
    }
  }

  if (len < PRINTF_FTOA_BUFFER_SIZE) {
    if (negative) {
      buf[len++] = '-';
    }
    else if (flags & FLAGS_PLUS) {
      buf[len++] = '+';  // ignore the space if the '+' exists
    }
    else if (flags & FLAGS_SPACE) {
      buf[len++] = ' ';
    }
  }

  _out_rev(buf, len, width, flags);
}

void StringFormatter::_etoa(double value, unsigned int prec, unsigned int width, unsigned int flags) {
  // check for NaN and special values
  if ((value != value) || (value > DBL_MAX) || (value < -DBL_MAX)) {
    return _ftoa(value, prec, width, flags);
  }

  // determine the sign
  const bool negative = value < 0;
  if (negative) {
    value = -value;
  }

  // default precision
  if (!(flags & FLAGS_PRECISION)) {
    prec = PRINTF_DEFAULT_FLOAT_PRECISION;
  }

  // determine the decimal exponent
  // based on the algorithm by David Gay (https://www.ampl.com/netlib/fp/dtoa.c)
  union {
    uint64_t U;
    double   F;
  } conv;

  conv.F = value;
  int exp2 = (int)((conv.U >> 52U) & 0x07FFU) - 1023;           // effectively log2
  conv.U = (conv.U & ((1ULL << 52U) - 1U)) | (1023ULL << 52U);  // drop the exponent so conv.F is now in [1,2)
  // now approximate log10 from the log2 integer part and an expansion of ln around 1.5
  int expval = (int)(0.1760912590558 + exp2 * 0.301029995663981 + (conv.F - 1.5) * 0.289529654602168);
  // now we want to compute 10^expval but we want to be sure it won't overflow
  exp2 = (int)(expval * 3.321928094887362 + 0.5);
  const double z  = expval * 2.302585092994046 - exp2 * 0.6931471805599453;
  const double z2 = z * z;
  conv.U = (uint64_t)(exp2 + 1023) << 52U;
  // compute exp(z) using continued fractions, see https://en.wikipedia.org/wiki/Exponential_function#Continued_fractions_for_ex
  conv.F *= 1 + 2 * z / (2 - z + (z2 / (6 + (z2 / (10 + z2 / 14)))));
  // correct for rounding errors
  if (value < conv.F) {
    expval--;
    conv.F /= 10;
  }

  // the exponent format is "%+03d" and largest value is "307", so set aside 4-5 characters
  unsigned int minwidth = ((expval < 100) && (expval > -100)) ? 4U : 5U;

  // in "%g" mode, "prec" is the number of *significant figures* not decimals
  if (flags & FLAGS_ADAPT_EXP) {
    // do we want to fall-back to "%f" mode?
    if ((value >= 1e-4) && (value < 1e6)) {
      if ((int)prec > expval) {
        prec = (unsigned)((int)prec - expval - 1);
      }
      else {
        prec = 0;
      }
      flags |= FLAGS_PRECISION;   // make sure _ftoa respects precision
      // no characters in exponent
      minwidth = 0U;
      expval   = 0;
    }
    else {
      // we use one sigfig for the whole part
      if ((prec > 0) && (flags & FLAGS_PRECISION)) {
        --prec;
      }
    }
  }

  // will everything fit?
  unsigned int fwidth = width;
  if (width > minwidth) {
    // we didn't fall-back so subtract the characters required for the exponent
    fwidth -= minwidth;
  } else {
    // not enough characters, so go back to default sizing
    fwidth = 0U;
  }
  if ((flags & FLAGS_LEFT) && minwidth) {
    // if we're padding on the right, DON'T pad the floating part
    fwidth = 0U;
  }

  // rescale the float value
  if (expval) {
    value /= conv.F;
  }

  // output the floating part
  const size_t start_len = m_result.size();
  _ftoa(negative ? -value : value, prec, fwidth, flags & ~FLAGS_ADAPT_EXP);

  // output the exponent part
  if (minwidth) {
    // output the exponential symbol
    out((flags & FLAGS_UPPERCASE) ? 'E' : 'e');
    // output the exponent value
    _ntoa_long((expval < 0) ? -expval : expval, expval < 0, 10, 0, minwidth-1, FLAGS_ZEROPAD | FLAGS_PLUS);
    // might need to right-pad spaces
    if (flags & FLAGS_LEFT) {
        while ((m_result.size()-start_len) < width) out(' ');
    }
  }
}

// output the specified string in reverse, taking care of any zero-padding
void StringFormatter::_out_rev(const char* buf, size_t len, unsigned int width, unsigned int flags) {
  const size_t start_sz = m_result.size();

  // pad spaces up to given width
  if (!(flags & FLAGS_LEFT) && !(flags & FLAGS_ZEROPAD)) {
    for (size_t i = len; i < width; i++) {
      out(' ');
    }
  }

  // reverse string
  while (len) {
    out(buf[--len]);
  }

  // append pad spaces up to given width
  if (flags & FLAGS_LEFT) {
    while ((m_result.size() - start_sz) < width) {
      out(' ');
    }
  }
}

dmgr::IDebug *StringFormatter::m_dbg = 0;

}
}
}
