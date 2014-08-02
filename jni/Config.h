#ifndef CPT_CONFIG_H
#define CPT_CONFIG_H

#include <stddef.h>
#include <sys/types.h>

// 8bit binary type.
typedef unsigned char  byte_t;
typedef unsigned char  byte;

// 8bit character type.
typedef char           char_t;

// 32bit integer types.
typedef signed int     int32_t;
typedef unsigned int   uint32_t;

// float type (32bit IEEE754).
typedef float          real_t;

// double type (64bit IEEE754).
typedef double         double_t;

// charactor-code type (16bit).
typedef  uint16_t      cid_t;
typedef  uint16_t      unicode_t;

// boolean type (0 : false, !0 : true).
typedef int32_t        bool_t;

#define CPT_TRUE 1
#define CPT_FALSE 0

// 单次申请内存不得超过1MB，防止整数溢出攻击。
#define CPT_LIMITMEM (1024 * 1024)

// 禁用复制构造和复制函数，在类中private下使用。
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
    void operator=(const TypeName&)

// That gcc wants both of these prototypes seems mysterious. VC, for
// its part, can't decide which to use (another mystery). Matching of
// template overloads: the final frontier.
#ifndef _MSC_VER
template <typename T, size_t N>
char (&ArraySizeHelper(const T (&array)[N]))[N];
#endif

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

#endif // CPT_CONFIG_H