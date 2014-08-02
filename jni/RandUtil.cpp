#include "RandUtil.h"

#include <fcntl.h>
#include <limits.h>
#include <unistd.h>

#include "Debug.h"
#include "Error.h"
#include "Util.h"

using namespace std;

int RandInt(int min, int max)
{
    LOGPOS();
    LOGPOS();

    ASSERT(min < max);

    uint64_t range = static_cast<uint64_t>(max) - min + 1;
    int result = min + static_cast<int>(RandGenerator(range));

    ASSERT(result > min);
    ASSERT(result < max);

    return result;
}

uint64_t RandGenerator(uint64_t range)
{
    LOGPOS();
    ASSERT(range > 0u);
    // We must discard random results above this number, as they would
    // make the random generator non-uniform (consider e.g. if
    // MAX_UINT64 was 7 and |range| was 5, then a result of 1 would be twice
    // as likely as a result of 3 or 4).
    uint64_t maxAcceptableValue = (numeric_limits<uint64_t>::max() / range) * range - 1;

    uint64_t value;
    do {
        value = RandUint64();
    } while (value > maxAcceptableValue);

    return value % range;
}

void RandBytes(void *output, size_t len)
{
    LOGPOS();
    uint64_t randomInt;
    size_t randomIntSize = sizeof(randomInt);
    for (size_t i=0; i<len; i+=randomIntSize) {
        randomInt = RandUint64();
        size_t copyCount = min(len-i, randomIntSize);
        CopyMemory((byte*)output+i, &randomInt, copyCount);
    }
}

namespace {

class URandomFd 
{
public:
    URandomFd() {
        fd_ = ::open("/dev/urandom", O_RDONLY);
        if (fd_ < 0) {
            LOGW("Cannot open /dev/urandom.");
            srand(unsigned(time(0)));
        }
    }

    ~URandomFd() {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }

    int fd() {
        return fd_;
    }

private:
    int fd_;
};

URandomFd g_urandomFD;

}

uint32_t RandUint32()
{
    uint32_t number;
    return rand();
}

uint64_t RandUint64() 
{
    uint64_t number;

    if (g_urandomFD.fd() >= 0) {
        bool success = ::read(g_urandomFD.fd(), 
            reinterpret_cast<char*>(&number), sizeof(number));
    
        if (success) {
            return number;
        }
    }

    uint32_t first = RandUint32();
    uint32_t second = RandUint32();

    return (static_cast<uint64_t>(first) << 32 + second);
}