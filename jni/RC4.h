#ifndef CPT_RC4_H
#define CPT_RC4_H

#include <string>

#include "Config.h"
#include "Error.h"

class RC4 
{
public:
    RC4();
    errno_t Init(const byte key[], const size_t len);
    errno_t Init(const char key[], const size_t len);
    int32_t XORStream(const void *src, size_t srcLen, void *dst, size_t dstLen);
    int32_t XORStream(void *data, const size_t len);
    void Reset();

private:
    DISALLOW_COPY_AND_ASSIGN(RC4);
    byte_t s_[256];
    uint8_t i_, j_;
};

#endif // CPT_RC4_H