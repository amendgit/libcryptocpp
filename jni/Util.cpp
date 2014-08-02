#include "Util.h"

#include "Debug.h"

bool Int32ToOctets(uint32_t u32, byte_t arrOctets[4])
{
    LOGPOS();
    for (size_t i=0; i<sizeof(uint32_t); ++i) {
        arrOctets[i] = (byte_t)u32 & 0xff;
        u32 >>= 8;
    }
}

uint32_t OctetsToInt32(byte_t arrOctets[4]) 
{
    LOGPOS();
    uint32_t u32 = 0;
    for (ssize_t i=sizeof(uint32_t); i>=0; --i) {
        u32 = (u32 << 8) | arrOctets[i];
    }
    return u32;
}
/*
void ZeroMemory(void *data, const size_t len)
{
    LOGPOS();

    unsigned char *p = static_cast<unsigned char*>(data);
    
    for (size_t i=0; i<len; ++i) 
        *(p+i) = 0;
}

void CopyMemory(void *dst, const void *src, const size_t n)
{
    LOGPOS();
    memcpy(dst, src, n);
}

int CompareMemory(const void *arg1, const void *arg2, const size_t n)
{
    LOGPOS();
    return memcmp(arg1, arg2, n);
}


inline char *CopyString(char *dst, const char *src, const size_t len)
{
    if (dst == NULL || src == NULL || len  == 0) 
        return NULL;

    for (size_t i=0; src[i]!='\0' && i<len; ++i) 
        dst[i] = src[i];
}*/