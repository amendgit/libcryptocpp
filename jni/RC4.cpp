#include "RC4.h"

#include <string>

#include "Debug.h"
#include "Error.h"

using namespace std;

RC4::RC4() : i_(0), j_(0) {}

errno_t RC4::Init(const byte key[], const size_t len) 
{
    LOGPOS();

    byte_t *tempKey = (byte_t*)key;
    if (len < 1 || len > 256) {
        LOGW("Error key size %d.", len);
        return CPT_ERROR;
    }

    for (int32_t i=0; i<256; ++i) {
        s_[i] = static_cast<byte_t>(i);
    }

    uint8_t j = 0;
    for (int32_t i=0; i<256; ++i) {
        j += s_[i] + tempKey[i%len]; // j是uint8_t，超过255后会从0开始增长。
        swap(s_[i], s_[j]);
    }
    return CPT_OK;
}

errno_t RC4::Init(const char key[], const size_t len)
{
    return Init(reinterpret_cast<const byte*>(key), len);
}

int32_t RC4::XORStream(const void *src, size_t srcLen, void *dst, size_t dstLen)
{
    LOGPOS();

    if (src == NULL || srcLen <= 0 || dst == NULL || dstLen <= 0) {
        LOGW("Invalid Arguments!");
        return 0;
    }

    if (srcLen < dstLen) {
        LOGW("Error source length %d is lower than destination length %d!", srcLen, dstLen);
        return 0;
    }

    const unsigned char *srcbuf = static_cast<const unsigned char*>(src);
    unsigned char *dstbuf = static_cast<unsigned char*>(dst);

    for (int32_t i=0; i<srcLen; ++i) {
        i_ += 1;
        j_ += s_[i_];
        swap(s_[i_], s_[j_]);
        dstbuf[i] = srcbuf[i] ^ s_[static_cast<uint8_t>(s_[i_] + s_[j_])];
    }

    return srcLen;
}

int32_t RC4::XORStream(void *data, const size_t len)
{
    LOGPOS();

    if (len <= 0) {
        LOGW("Invalid Arguments!");
        return 0;
    }

    return XORStream(data, len, data, len);
}

void RC4::Reset() 
{
    LOGPOS();

    memset(s_, 0, 256 * sizeof(byte_t));
    i_ = j_ = 0;
}