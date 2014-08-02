#ifndef CPT_FILE_IO_H
#define CPT_FILE_IO_H

#include <string>

#include "Error.h"
#include "IO.h"

class FileReader : public Reader
{
public:
    FileReader() : handle_(-1) {}

    ~FileReader() {
        Close();
    }

    errno_t Open(const std::string &filename);

    errno_t Close();

    off_t Seek(off_t distanceToMove, int moveMethod);

    size_t GetLength();
    
    errno_t GetLength(uint64_t *length) const;

    ssize_t Read(void *data, size_t size);

protected:
    int handle_;
};

class FileWriter : public Writer
{
public:
    FileWriter() : handle_(-1) {}

    ~FileWriter() {
        Close();
    }

    errno_t Create(const std::string &filename, bool createAlways = false);

    errno_t Open(const std::string &filename, bool creationDisposition = false);

    errno_t OpenExisting(const std::string& filename);

    off_t Seek(off_t distanceToMove, int moveMethod);

    size_t GetLength();

    errno_t GetLength(uint64_t *length) const;

    errno_t Truncate(const size_t size);

    errno_t Close();

    ssize_t Write(const void *data, size_t size);
    
protected:
    int handle_;
};

#endif // CPT_FILE_IO_H