#ifndef CPT_MEMORY_IO_H
#define CPT_MEMORY_IO_H

#include "Error.h"
#include "FileHeader.h"
#include "IO.h"

class MemoryWriter : public Writer
{
public:
    MemoryWriter();

    ssize_t Write(const void *data, const size_t len);

    off_t Seek(off_t distanceToMove, int moveMethod);

    errno_t Reserve(size_t capacity);

    size_t GetLength() {
        return length_;
    }

    size_t GetCapacity() const {
        return capacity_;
    }

    const char *GetMemory() const {
        return data_;
    }

    ~MemoryWriter() {
        delete [] data_;
    }

private:
    off_t SetPointer(ssize_t pointer);

private:
    char *data_;
    size_t capacity_;
    size_t length_;
    size_t pointer_;
};

#endif /* CPT_MEMORY_IO_H */