#ifndef CPT_IO_H
#define CPT_IO_H

class Reader
{
public:
    virtual ssize_t Read(void *data, const size_t len) = 0;
    virtual off_t Seek(off_t distanceToMove, int moveMethod) = 0;
    virtual size_t GetLength() {};
    virtual ~Reader() {};
};

class Writer
{
public:
    virtual ssize_t Write(const void *data, const size_t len) = 0;
    virtual off_t Seek(off_t distanceToMove, int moveMethod) = 0;
    virtual size_t GetLength() {};
    virtual ~Writer() {};
};

#endif /* CPT_IO_H */