#include "FileIO.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>

#include "Config.h"
#include "Debug.h"
#include "Error.h"

using namespace std;

namespace {
    errno_t Close(int *handle)
    {
        LOGPOS();

        if (*handle != -1) 
            return CPT_OK;
        if (::close(*handle) != 0) 
            return CPT_ERROR;
        *handle = -1;
        return CPT_OK;
    }

    errno_t OpenBinary(const std::string filename, int flags, int *handle)
    {
        LOGPOS();

        #ifdef O_BINARY
        flags |= O_BINARY;
        #endif
        Close(handle);
        *handle = ::open(filename.c_str(), flags, 0666);
        if (*handle == -1) {
            LOGW("Open file %s failed!, %d", filename.c_str(), errno);
            return CPT_ERROR;
        }
        return CPT_OK;
    }

    off_t Seek(int handle, off_t distanceToMove, int moveMethod)
    {
        return ::lseek(handle, distanceToMove, moveMethod);
    }

    errno_t GetLength(int handle, uint64_t *length)
    {
        off_t curPos = Seek(handle, 0, SEEK_CUR);
        off_t lengthTemp = Seek(handle, 0, SEEK_END);
        Seek(handle, curPos, SEEK_SET);
        *length = (uint64_t)lengthTemp;
        return CPT_OK;
    }
}

/////////////////////////////
////FileReader

errno_t FileReader::Open(const string &filename)
{
    LOGPOS();
    return ::OpenBinary(filename, O_RDONLY, &handle_);
}

errno_t FileReader::Close()
{
    LOGPOS();
    return ::Close(&handle_);
}

off_t FileReader::Seek(off_t distanceToMove, int moveMethod)
{
    LOGPOS();
    return ::Seek(handle_, distanceToMove, moveMethod);
}

size_t FileReader::GetLength()
{
    LOGPOS();
    uint64_t length = 0;
    ::GetLength(handle_, &length);
    return static_cast<size_t>(length);
}

errno_t FileReader::GetLength(uint64_t *length) const
{
    return ::GetLength(handle_, length);
}

ssize_t FileReader::Read(void *data, size_t size)
{
    return ::read(handle_, data, size);
}

/////////////////////////////
////FileWriter
errno_t FileWriter::Create(const string &filename, bool createAlways)
{
    LOGPOS();
    if (createAlways) {
        Close();
        handle_ = ::creat(filename.c_str(), 0666);
        if (handle_ == -1) {
            LOGW("Create file %s failed!, %d", filename.c_str(), errno);
            return CPT_ERROR;
        }
        return CPT_OK;
    }
    return ::OpenBinary(filename, O_CREAT|O_EXCL|O_WRONLY, &handle_);
}

errno_t FileWriter::Open(const string &filename, bool creationDisposition)
{
    LOGPOS();
    return Create(filename, false);
}

errno_t FileWriter::OpenExisting(const string& filename)
{
    LOGPOS();
    return ::OpenBinary(filename, O_WRONLY, &handle_);
}

off_t FileWriter::Seek(off_t distanceToMove, int moveMethod)
{
    LOGPOS();
    return ::Seek(handle_, distanceToMove, moveMethod);
}

size_t FileWriter::GetLength()
{
    LOGPOS();
    uint64_t length = 0;
    ::GetLength(handle_, &length);
    return static_cast<size_t>(length);
}

errno_t FileWriter::GetLength(uint64_t *length) const
{
    return ::GetLength(handle_, length);
}

errno_t FileWriter::Truncate(const size_t size)
{
    LOGPOS();
    int err = ftruncate(handle_, size);
    if (err == 0) {
        return CPT_OK;
    }
    return CPT_OK;
}

errno_t FileWriter::Close()
{
    LOGPOS();
    return ::Close(&handle_);
}

ssize_t FileWriter::Write(const void *data, size_t size)
{
    return ::write(handle_, data, size);
}