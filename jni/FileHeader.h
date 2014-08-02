#ifndef FILE_FORMAT_H
#define FILE_FORMAT_H

#include "Config.h"
#include "Error.h"
#include "FilePath.h"
#include "IO.h"

#pragma pack(1)
class FileHeader 
{
public:
    FileHeader();

    errno_t LoadHeader(const FilePath& filePath);

    errno_t ReadFrom(Reader *in);
    errno_t WriteTo(Writer *out);

    void Reset();

    void SetFileTag();
    errno_t CheckFileTag();
    
    void RandomKey();
    errno_t SetKey(const char key[], const size_t len);
    errno_t GetKey(char *key, size_t *len);

    void RandomOffset();
    void SetOffset(size_t offset);
    uint32_t GetOffset();

    void RandomSample();
    uint32_t GetSample(); // 获取样本的长度.

    errno_t SetFormat(const std::string& format);
    const char *GetFormat();

    static const char kFileTag[];
    static const char kEncryptedFileTag[];

public:
    enum {
        TAG_SIZE    = 4,
        OFFSET_SIZE = 4,
        SAMPLE_SIZE = 4,
        FORMAT_SIZE = 20,
        UNUSED_SIZE = 136,
        KEY_SIZE    = 256,
        USERS_SIZE  = 600,
        HEADER_SIZE = 1024
    };

private:
    DISALLOW_COPY_AND_ASSIGN(FileHeader);

private:
    byte tag_[TAG_SIZE];      // 文件标识4BYTE
    byte key_[KEY_SIZE];    // 文件密钥256BYTE
    byte users_[USERS_SIZE];  // 可打开用户列表600BYTE
    /* used only for video */
    byte offset_[OFFSET_SIZE];   // 偏移4BYTE
    byte sample_[SAMPLE_SIZE];   // 取样4BYTE
    byte format_[FORMAT_SIZE];  // 原文件格式20BYTE
    byte unused_[UNUSED_SIZE]; // 保留
};
#pragma pop


#endif /* FILE_FORMAT_H */