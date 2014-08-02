#include "MemoryIO.h"

#include "Config.h"
#include "Debug.h"
#include "Error.h"
#include "Util.h"

MemoryWriter::MemoryWriter() : 
    data_(NULL), capacity_(0), length_(0), pointer_(0)
{
}

ssize_t MemoryWriter::Write(const void *data, const size_t len)
{
    LOGPOS();

    if (pointer_ == capacity_) {
        LOGW("Memory not enough for write!");
        return 0;
    }

    size_t length = len;
    if (pointer_ + len > capacity_) {
        length = capacity_ - length_;
    }

    CopyMemory(data_ + pointer_, data, length);

    pointer_ += length;
    if (pointer_ > length_) {
        length_ = pointer_;
    }

    return length;
}

off_t MemoryWriter::Seek(off_t distanceToMove, int moveMethod)
{
    LOGPOS();

    if (moveMethod == SEEK_CUR) {
        return SetPointer(pointer_ + distanceToMove);
    } else if (moveMethod == SEEK_SET) {
        return SetPointer(distanceToMove);
    } else if (moveMethod == SEEK_END) {
        return SetPointer(length_ + distanceToMove);
    }

    return CPT_ERROR;
}

errno_t MemoryWriter::Reserve(size_t capacity) 
{
    LOGPOS();

    if (capacity <= 0) {
        LOGW("Invalid Arguments!");
        return CPT_ERROR;
    }

    char *tempMemory = new char[capacity];
    if (tempMemory == NULL) {
        LOGW("Allocate memory %d failed!", capacity);
        return CPT_ERROR;
    }

    if (data_ != NULL) {
        CopyMemory(tempMemory, data_, length_);
        delete[] data_;
    }

    data_ = tempMemory;
    tempMemory = NULL;
    capacity_ = capacity;

    return CPT_OK;
}

off_t MemoryWriter::SetPointer(ssize_t pointer)
{
    LOGPOS();

    if (pointer < 0 || pointer > capacity_) {
        LOGW("Invalid Arguments!");
        return pointer_;
    }
    if (pointer > length_) {
        length_ = pointer;
    }
    pointer_ = pointer;
    return pointer_;
}