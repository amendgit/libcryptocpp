#ifndef CPT_CRYPTO_H
#define CPT_CRYPTO_H

#include <string>

#include "Config.h"
#include "Error.h"
#include "MemoryIO.h"

class Encrypt
{
public:
    // 输入文件
    virtual errno_t SetReader(Reader *reader);
    // 根据输入文件的相关信息，填充文件头部。
    virtual FileHeader *GenrHeader();
    // 加密文件头
    virtual errno_t EncryptHeader(const char key[], const size_t len);
    // 输出文件
    virtual errno_t SetWriter(Writer *writer);
    // 写文件头
    virtual errno_t WriteHeader();
    // 初始化加密上下文
    virtual errno_t PreEncrypt();
    // 加密
    virtual errno_t DoEncrypt(const char key[], const size_t len);
    // 加密后相关操作
    virtual errno_t PostEncrypt();

    virtual ~Encrypt();

protected:
    FileHeader fileHeader_;
    Reader *reader_;
    Writer *writer_;
};

class Decrypt
{
public:
    virtual errno_t SetReader(Reader *reader);
    virtual errno_t LoadHeader();
    virtual FileHeader *DecryptHeader(const char key[], const size_t len);
    virtual errno_t SetWriter(Writer *writer);
    virtual errno_t PreDecrypt();
    virtual errno_t DoDecrypt(const char key[], const size_t len);
    virtual errno_t PostDecrypt();
    virtual ~Decrypt();

protected:
    FileHeader fileHeader_;
    Reader *reader_;
    Writer *writer_;
};

errno_t EasyEncrypt(const std::string& filename, const std::string& key, std::string *outFilename);

errno_t EasyEncrypt(const std::string& inFilename, const std::string& outFilename, const std::string& key);

errno_t EasyDecrypt(const std::string& filename, const std::string& key, std::string *outFilename);

errno_t EasyDecrypt(const std::string& inFilename, const std::string& outFilename, const std::string& key);

errno_t EasyDecryptInMemory(const std::string& filename, const std::string& key, MemoryWriter *writer);

///////////////

class EncryptVideo : public Encrypt 
{
public:
    FileHeader *GenrHeader();

    errno_t WriteHeader();

    errno_t PreEncrypt();

    errno_t DoEncrypt(const char key[], const size_t len);

    errno_t PostEncrypt();

private:
    size_t headerSize_;
};

errno_t EasyEncryptVideo(const std::string& filename, const std::string& key, std::string *outFilename);

errno_t EasyEncryptVideo(const std::string& inFilename, const std::string& outFilename, const std::string& key);

class DecryptVideo : public Decrypt
{
public:
    errno_t PreDecrypt();

    errno_t DoDecrypt(const char key[], const size_t len);

    errno_t PostDecrypt();

private:
    size_t headerSize_;
};

errno_t EasyDecryptVideo(const std::string& filename, const std::string& key, std::string *outFilename);

errno_t EasyDecryptVideo(const std::string& inFilename, const std::string& outFilename, const std::string& key);

//////////////

errno_t EasyLoadHeaders(const std::string& filename, FileHeader *fileHeader);

#endif /* CPT_CRYPTO_H */