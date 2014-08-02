#ifndef CPT_UTIL_H
#define CPT_UTIL_H

#include <memory.h>  
#include <stdlib.h>   

#include "Config.h"
#include "Error.h"

bool Int32ToOctets(uint32_t u32, byte_t arrOctets[4]);

uint32_t OctetsToInt32(byte_t arrOctets[4]);

inline void ZeroMemory(void *data, const size_t len)
{
    unsigned char *p = static_cast<unsigned char*>(data);
    
    for (size_t i=0; i<len; ++i) 
        *(p+i) = 0;
}

inline void CopyMemory(void *dst, const void *src, const size_t n)
{
    memcpy(dst, src, n);
}

inline int CompareMemory(const void *arg1, const void *arg2, const size_t n)
{
    return memcmp(arg1, arg2, n);
}

inline char *CopyString(char *dst, const char *src, const size_t len)
{
    if (dst == NULL || src == NULL || len  == 0) 
        return NULL;

    for (size_t i=0; src[i]!='\0' && i<len; ++i) 
        dst[i] = src[i];
}

#endif /* CPT_UTIL_H */