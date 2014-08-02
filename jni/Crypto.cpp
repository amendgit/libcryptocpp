#include "Crypto.h"

#include <string>

#include "Config.h"
#include "Debug.h"
#include "FileHeader.h"
#include "FileIO.h"
#include "FileUtil.h"
#include "RC4.h"

using namespace std;

namespace {
    const int32_t kBufferLength = 2048;
    const int32_t kMinHeaderSize = 1*1024*1024;
    const int32_t kMaxHeaderSize = 2*1024*1024;
}

//////////////////////////////
////Encrypt
errno_t Encrypt::SetReader(Reader *reader)
{
    LOGPOS();

    if (reader == NULL) {
        LOGW("The reader is NULL");
        return CPT_ERROR;
    }

    reader_ = reader;
    return CPT_OK;
}

FileHeader *Encrypt::GenrHeader()
{
    LOGPOS();

    fileHeader_.SetFileTag();

    if (fileHeader_.CheckFileTag() != CPT_OK) {
        LOGW("File header is not valid.");
        return NULL;
    }

    return &fileHeader_;
}

errno_t Encrypt::EncryptHeader(const char key[], const size_t len)
{
    LOGPOS();
    RC4 rc4;
    rc4.Init(key, len);
    int32_t length = rc4.XORStream(&fileHeader_, sizeof(FileHeader));
    
    if (length != sizeof(FileHeader)) {
        LOGW("Encrypt file header failed!");
        return CPT_ERROR;
    }

    return CPT_OK;
}

errno_t Encrypt::SetWriter(Writer *writer)
{
    LOGPOS();

    if (writer == NULL) {
        LOGW("The writer is NULL.");
        return CPT_ERROR;
    }

    writer_ = writer;
    return CPT_OK;
}

errno_t Encrypt::WriteHeader()
{
    LOGPOS();

    int32_t length = writer_->Write(&fileHeader_, sizeof(FileHeader));
    if (length != sizeof(FileHeader)) {
        LOGW("Write file header to file failed!");
        return CPT_ERROR;
    }

    return CPT_OK;
}

errno_t Encrypt::PreEncrypt()
{
    LOGPOS();
    return CPT_OK;
}

errno_t Encrypt::DoEncrypt(const char key[], const size_t len)
{
    LOGPOS();

    RC4 rc4;
    rc4.Init(key, len);

    char buf[kBufferLength];
    size_t bufLength = kBufferLength;

    for (;;) {
        bufLength = reader_->Read(buf, kBufferLength);
        // 只考虑了正常读写完成的情况。对于读写错误时，返回值也可能为<=0。
        // 需要额外的处理，对于文件读写故障(如：TF卡被卸载)时进行错误报告。
        if (bufLength <= 0) 
            break;

        rc4.XORStream(buf, bufLength);

        if (writer_->Write(buf, bufLength) != bufLength) {
            LOGW("Write to file failed!");
            return CPT_ERROR;
        }
    }

    return CPT_OK;
}

errno_t Encrypt::PostEncrypt()
{
    LOGPOS();
    return CPT_OK;
}

Encrypt::~Encrypt() 
{
    LOGPOS();
    reader_ = NULL;
    writer_ = NULL;
}

////////////////////////////////
////Decrypt
errno_t Decrypt::SetReader(Reader *reader)
{
    LOGPOS();

    if (reader == NULL) {
        LOGW("The reader is NULL");
        return CPT_ERROR;
    }

    reader_ = reader;
    return CPT_OK;
}

errno_t Decrypt::LoadHeader()
{
    LOGPOS();

    fileHeader_.ReadFrom(reader_);
    return CPT_OK;
}

FileHeader *Decrypt::DecryptHeader(const char key[], const size_t len)
{
    LOGPOS();

    RC4 rc4;
    rc4.Init(key, len);

    int32_t length = rc4.XORStream(&fileHeader_, sizeof(FileHeader));
    if (length != sizeof(FileHeader)) {
        LOGW("Decrypt file header failed!");
        return NULL;
    }

    if (fileHeader_.CheckFileTag() != CPT_OK) {
        LOGW("File format error");
    }

    return &fileHeader_;
}

errno_t Decrypt::SetWriter(Writer *writer)
{
    LOGPOS();

    if (writer == NULL) {
        LOGW("The writer is NULL.");
        return CPT_ERROR;
    }

    writer_ = writer;
    return CPT_OK;
}

errno_t Decrypt::PreDecrypt()
{
    LOGPOS();
    return CPT_OK;
}

errno_t Decrypt::DoDecrypt(const char key[], const size_t len)
{
    LOGPOS();

    RC4 rc4;
    rc4.Init(key, len);

    char buf[kBufferLength];
    size_t bufLength = kBufferLength;
    for (;;) {
        bufLength = reader_->Read(buf, kBufferLength);
        // 只考虑了正常读写完成的情况。对于读写错误时，返回值也可能为<=0。
        // 需要额外的处理，对于文件读写故障(如：TF卡被卸载)时进行错误报告。
        if (bufLength <= 0) 
            break;

        rc4.XORStream(buf, bufLength);
        if (writer_->Write(buf, bufLength) != bufLength) {
            LOGW("Write to file %s failed!");
            return CPT_ERROR;
        }
    }
    return CPT_OK;
}

errno_t Decrypt::PostDecrypt()
{
    LOGPOS();
    return CPT_OK;
}

Decrypt::~Decrypt()
{
    LOGPOS();

    reader_ = NULL;
    writer_ = NULL;
}

// 文件加密函数
// 这个函数有点长，但是基本上都是检错的代码。逻辑还是很清晰的。
errno_t EasyEncrypt(const string& inFilename, const string& outFilename, const string& key)
{
    LOGPOS();

    FileReader fileReader;
    FilePath inFilePath(inFilename);
    if (fileReader.Open(inFilePath.value()) != CPT_OK) {
        LOGW("Open file %s failed!", inFilePath.value().c_str());
        return CPT_ERROR;
    }

    Encrypt encrypt;
    if (encrypt.SetReader(dynamic_cast<Reader*>(&fileReader)) != CPT_OK) {
        LOGW("Encrypt Set Reader failed");
        return CPT_ERROR;
    }

    FileHeader *fileHeader = encrypt.GenrHeader();
    if (fileHeader == NULL) {
        LOGW("Generate file header failed!");
        return CPT_ERROR;
    }

    fileHeader->SetFormat(inFilePath.Extension());

    if (encrypt.EncryptHeader(key.c_str(), key.length()) != CPT_OK) {
        LOGW("Encrypt file header failed");
        return CPT_ERROR;
    }

    FileWriter writer;
    if (writer.Open(outFilename) != CPT_OK) {
        LOGW("Create file %s failed!", outFilename.c_str());
        return CPT_ERROR;
    }

    if (encrypt.SetWriter(dynamic_cast<Writer*>(&writer)) != CPT_OK) {
        LOGW("encrypt set writer failed!");
        return CPT_ERROR;
    }

    if (encrypt.WriteHeader() != CPT_OK) {
        LOGW("Encrypt write header failed!");
        return CPT_ERROR;
    }
    
    int err = encrypt.PreEncrypt();
    ASSERT(err == CPT_OK);

    if (encrypt.DoEncrypt(key.c_str(), key.length()) != CPT_OK) {
        LOGW("Encrypt encrypt failed!");
        return CPT_ERROR;
    }

    err = encrypt.PostEncrypt();
    ASSERT(err == CPT_OK);

    return CPT_OK;
}

errno_t EasyEncrypt(const string& filename, const string& key, string *outFilename)
{
    LOGPOS();

    *outFilename = FilePath(filename).ReplaceExtension(".mxx").value();

    return EasyEncrypt(filename, *outFilename, key);
}

errno_t EasyDecrypt(const string& inFilename, const string& outFilename, const string& key)
{
    LOGPOS();

    FileReader reader;
    FilePath inFilePath(inFilename);
    if (reader.Open(inFilename) != CPT_OK) {
        LOGW("Open file %s failed!", inFilePath.value().c_str());
        return CPT_ERROR;
    }

    Decrypt decrypt;
    if (decrypt.SetReader(&reader) != CPT_OK) {
        LOGW("Decrypt set reader failed!");
        return CPT_ERROR;
    }

    if (decrypt.LoadHeader() != CPT_OK) {
        LOGW("Decrypt load header failed!");
        return CPT_ERROR;
    }

    FileHeader *fileHeader = decrypt.DecryptHeader(key.c_str(), key.length());
    if (fileHeader == NULL) {
        LOGW("LoadHeader error");
        return CPT_ERROR;
    }
    FileWriter writer;
    if (writer.Open(outFilename) != CPT_OK) {
        LOGW("Create file %s failed!", outFilename.c_str());
        return CPT_ERROR;
    }

    if (decrypt.SetWriter(dynamic_cast<Writer*>(&writer)) != CPT_OK) {
        LOGW("Decrypt set writer failed!");
        return CPT_ERROR;
    }

    int err = decrypt.PreDecrypt();
    ASSERT(err == CPT_OK);

    if (decrypt.DoDecrypt(key.c_str(), key.length())) {
        LOGW("Decrypt decrypt file failed!");
        return CPT_ERROR;
    }

    err = decrypt.PostDecrypt();
    ASSERT(err == CPT_OK);

    return CPT_OK;
}

errno_t EasyDecrypt(const string& filename, const string& key, string *outFilename)
{
    LOGPOS();

    FilePath inFilePath(filename);
    FileReader reader;
    if (reader.Open(inFilePath.value()) != CPT_OK) {
        LOGW("Open file %s failed!", inFilePath.value().c_str());
        return CPT_ERROR;
    }

    Decrypt decrypt;
    if (decrypt.SetReader(&reader) != CPT_OK) {
        LOGW("Decrypt set reader failed!");
        return CPT_ERROR;
    }

    if (decrypt.LoadHeader() != CPT_OK) {
        LOGW("Decrypt load header failed!");
        return CPT_ERROR;
    }

    FileHeader *fileHeader = decrypt.DecryptHeader(key.c_str(), key.length());
    if (fileHeader == NULL) {
        LOGW("LoadHeader error");
        return CPT_ERROR;
    }

    FilePath outFilePath = inFilePath.ReplaceExtension(fileHeader->GetFormat());
    *outFilename = outFilePath.value();
    FileWriter writer;
    if (writer.Open(outFilePath.value()) != CPT_OK) {
        LOGW("Create file %s failed!", outFilePath.value().c_str());
        return CPT_ERROR;
    }

    if (decrypt.SetWriter(dynamic_cast<Writer*>(&writer)) != CPT_OK) {
        LOGW("Decrypt set writer failed!");
        return CPT_ERROR;
    }

    int err = decrypt.PreDecrypt();
    ASSERT(err == CPT_OK);

    if (decrypt.DoDecrypt(key.c_str(), key.length())) {
        LOGW("Decrypt decrypt file failed!");
        return CPT_ERROR;
    }

    err = decrypt.PostDecrypt();
    ASSERT(err == CPT_OK);

    return CPT_OK;
}

errno_t EasyDecryptInMemory(const string& filename, const string& key, MemoryWriter *writer)
{
    LOGPOS();

    FilePath inFilePath(filename);
    FileReader reader;
    if (reader.Open(inFilePath.value()) != CPT_OK) {
        LOGW("Open file %s failed!", inFilePath.value().c_str());
        return CPT_ERROR;
    }

    uint64_t u64 = 0;
    reader.GetLength(&u64);

    if (writer->Reserve(u64) != CPT_OK) {
        LOGW("MemoryWriter SetLength failed!");
        return CPT_ERROR;
    }

    Decrypt decrypt;
    if (decrypt.SetReader(&reader) != CPT_OK) {
        LOGW("Decrypt set reader failed!");
        return CPT_ERROR;
    }

    if (decrypt.LoadHeader() != CPT_OK) {
        LOGW("Decrypt LoadHeader error");
        return CPT_ERROR;
    }

    if (decrypt.DecryptHeader(key.c_str(), key.length()) == NULL) {
        LOGW("LoadHeader error");
        return CPT_ERROR;
    }

    if (decrypt.SetWriter(writer) != CPT_OK) {
        LOGW("Decrypt set writer failed!");
        return CPT_ERROR;
    }

    int err = decrypt.PreDecrypt();
    ASSERT(err == CPT_OK);

    if (decrypt.DoDecrypt(key.c_str(), key.length())) {
        LOGW("Decrypt decrypt file failed!");
        return CPT_ERROR;
    }

    err = decrypt.PostDecrypt();
    ASSERT(err == CPT_OK);

    return CPT_OK;
}

/////////////////////////////
////EncryptVideo
FileHeader *EncryptVideo::GenrHeader()
{
    LOGPOS();

    Encrypt::GenrHeader();

    size_t length = reader_->GetLength();

    if (length > kMinHeaderSize) {
        length = length >> 4;
        if (length > kMaxHeaderSize) {
            length = kMaxHeaderSize;
        } else if (length < kMinHeaderSize) {
            length = kMinHeaderSize;
        }
    }

    headerSize_ = length;

    fileHeader_.SetOffset(headerSize_);

    return &fileHeader_;
}

errno_t EncryptVideo::WriteHeader()
{
    LOGPOS();

    off_t curReadPos = reader_->Seek(0, SEEK_SET);
    byte buf[FileHeader::HEADER_SIZE];

    int32_t len = reader_->Read(buf, FileHeader::HEADER_SIZE);
    if (len != FileHeader::HEADER_SIZE) {
        LOGW("File can't be so small: %d bytes", len);
        return CPT_ERROR;
    }

    off_t curWritePos = writer_->Seek(0, SEEK_END);
    len = writer_->Write(buf, FileHeader::HEADER_SIZE);
    if (len != FileHeader::HEADER_SIZE) {
        LOGW("Write failed.");
        return CPT_ERROR;
    }

    writer_->Seek(0, SEEK_SET);

    len = writer_->Write(&fileHeader_, FileHeader::HEADER_SIZE);
    if (len != FileHeader::HEADER_SIZE) {
        LOGW("Write failed.");
        return CPT_ERROR;
    }

    reader_->Seek(curReadPos, SEEK_SET);
    writer_->Seek(curWritePos, SEEK_SET);

    return CPT_OK;
}

errno_t EncryptVideo::PreEncrypt()
{
    LOGPOS();
    LOGW("HeaderSize: %d", headerSize_);
    reader_->Seek(FileHeader::HEADER_SIZE, SEEK_SET);
    writer_->Seek(FileHeader::HEADER_SIZE, SEEK_SET);
    return CPT_OK;
}

errno_t EncryptVideo::DoEncrypt(const char key[], const size_t len)
{
    LOGPOS();
    
    RC4 rc4;
    rc4.Init(key, len);

    char buf[kBufferLength];
    size_t bufLength = kBufferLength;
    size_t all = 0;

    for (;;) {
        int32_t readSize = kBufferLength;
        if (kBufferLength > headerSize_-all)
            readSize = headerSize_-all;

        bufLength = reader_->Read(buf, readSize);
        // 只考虑了正常读写完成的情况。对于读写错误时，返回值也可能为<=0。
        // 需要额外的处理，对于文件读写故障(如：TF卡被卸载)时进行错误报告。
        if (bufLength <= 0) 
            break;

        all += bufLength;
        if (all > headerSize_) 
            break ;

        rc4.XORStream(buf, bufLength);

        if (writer_->Write(buf, bufLength) != bufLength) {
            LOGW("Write to file failed!");
            return CPT_ERROR;
        }
    }

    return CPT_OK;
}

errno_t EncryptVideo::PostEncrypt()
{
    return CPT_OK;
}

errno_t EasyEncryptVideo(const std::string& filename, const std::string& key, string *outFilename)
{
    LOGPOS();

    FileReader fileReader;
    FilePath inFilePath(filename);
    if (fileReader.Open(inFilePath.value()) != CPT_OK) {
        LOGW("Open file %s failed!", inFilePath.value().c_str());
        return CPT_ERROR;
    }

    if (fileReader.GetLength() <= kMinHeaderSize) {
        return EasyEncrypt(filename, key, outFilename);
    }
    
    EncryptVideo encrypt;

    if (encrypt.SetReader(dynamic_cast<Reader*>(&fileReader)) != CPT_OK) {
        LOGW("Encrypt Set Reader failed");
        return CPT_ERROR;
    }

    FileHeader *fileHeader = encrypt.GenrHeader();
    if (fileHeader == NULL) {
        LOGW("Generate file header failed!");
        return CPT_ERROR;
    }

    fileHeader->SetFormat(inFilePath.Extension());

    if (encrypt.EncryptHeader(key.c_str(), key.length()) != CPT_OK) {
        LOGW("Encrypt file header failed");
        return CPT_ERROR;
    }

    FileWriter writer;
    if (writer.OpenExisting(filename) != CPT_OK) {
        LOGW("Create file %s failed!", filename.c_str());
        return CPT_ERROR;
    }

    if (encrypt.SetWriter(dynamic_cast<Writer*>(&writer)) != CPT_OK) {
        LOGW("encrypt set writer failed!");
        return CPT_ERROR;
    }

    if (encrypt.WriteHeader() != CPT_OK) {
        LOGW("Encrypt write header failed!");
        return CPT_ERROR;
    }
    
    if (encrypt.PreEncrypt() != CPT_OK) {
        LOGW("EncryptVideo PreEncrypt error");
        return CPT_ERROR;
    }

    if (encrypt.DoEncrypt(key.c_str(), key.length()) != CPT_OK) {
        LOGW("Encrypt encrypt failed!");
        return CPT_ERROR;
    }

    if (encrypt.PostEncrypt() != CPT_OK) {
        LOGW("Encrypt PostEncrypt error");
        return CPT_ERROR;
    }
    *outFilename = FilePath(filename).ReplaceExtension(".mxx").value();
    RenameFile(filename, *outFilename);

    return CPT_OK;
}

errno_t EasyEncryptVideo(const string& inFilename, const string& outFilename, const string& key)
{
    LOGPOS();

    string tempOutFilename;
    if (EasyEncryptVideo(inFilename, key, &tempOutFilename) != CPT_OK) {
        LOGW("EncryptVideoInternal error");
        return CPT_ERROR;
    }

    if (RenameFile(tempOutFilename, outFilename) != CPT_OK) {
        LOGW("MoveFile error. They are on different volume?");
        return CPT_ERROR;
    }

    return CPT_OK;
}

///////////////////////////////
////\DecryptVideo
errno_t DecryptVideo::PreDecrypt()
{
    LOGPOS();
    headerSize_ = fileHeader_.GetOffset();
    LOGW("HeaderSize: %d", headerSize_);
    reader_->Seek(FileHeader::HEADER_SIZE, SEEK_SET);
    writer_->Seek(FileHeader::HEADER_SIZE, SEEK_SET);
    return CPT_OK;
}

errno_t DecryptVideo::DoDecrypt(const char key[], const size_t len)
{
    LOGPOS();

    RC4 rc4;
    rc4.Init(key, len);

    char buf[kBufferLength];
    size_t bufLength = kBufferLength;
    size_t all = 0;

    for (;;) {
        int32_t readSize = kBufferLength;
        if (kBufferLength > headerSize_-all)
            readSize = headerSize_-all;

        bufLength = reader_->Read(buf, readSize);
        // 只考虑了正常读写完成的情况。对于读写错误时，返回值也可能为<=0。
        // 需要额外的处理，对于文件读写故障(如：TF卡被卸载)时进行错误报告。
        if (bufLength <= 0) 
            break;

        all += bufLength;
        if (all > headerSize_) 
            break ;

        rc4.XORStream(buf, bufLength);

        if (writer_->Write(buf, bufLength) != bufLength) {
            LOGW("Write to file failed!");
            return CPT_ERROR;
        }
    }

    return CPT_OK;
}

errno_t DecryptVideo::PostDecrypt()
{
    LOGPOS();
    reader_->Seek(0-FileHeader::HEADER_SIZE, SEEK_END);
    char buf[FileHeader::HEADER_SIZE];
    int32_t len = FileHeader::HEADER_SIZE;
    len = reader_->Read(buf, FileHeader::HEADER_SIZE);
    if (len != FileHeader::HEADER_SIZE) {
        LOGW("Read failed");
        return CPT_ERROR;
    }

    writer_->Seek(0, SEEK_SET);
    len = writer_->Write(buf, FileHeader::HEADER_SIZE);
    if (len != FileHeader::HEADER_SIZE) {
        LOGW("Write file failed!");
        return CPT_ERROR;
    }
    (dynamic_cast<FileWriter*>(writer_))->Truncate(writer_->GetLength()-FileHeader::HEADER_SIZE);
    return CPT_OK;
}

errno_t EasyDecryptVideo(const string& filename, const string& key, string *outFilename)
{
    LOGPOS();

    FilePath inFilePath(filename);
    FileReader reader;
    if (reader.Open(inFilePath.value()) != CPT_OK) {
        LOGW("Open file %s failed!", inFilePath.value().c_str());
        return CPT_ERROR;
    }

    if (reader.GetLength() <= kMinHeaderSize+FileHeader::HEADER_SIZE) {
        return EasyDecrypt(filename, key, outFilename);
    }

    DecryptVideo decrypt;
    if (decrypt.SetReader(&reader) != CPT_OK) {
        LOGW("Decrypt set reader failed!");
        return CPT_ERROR;
    }

    if (decrypt.LoadHeader() != CPT_OK) {
        LOGW("Decrypt load header failed!");
        return CPT_ERROR;
    }

    FileHeader *fileHeader = decrypt.DecryptHeader(key.c_str(), key.length());
    if (fileHeader == NULL) {
        LOGW("Generate file header failed!");
        return CPT_ERROR;
    }
    
    FileWriter writer;
    if (writer.OpenExisting(filename) != CPT_OK) {
        LOGW("Create file %s failed!", filename.c_str());
        return CPT_ERROR;
    }

    if (decrypt.SetWriter(dynamic_cast<Writer*>(&writer)) != CPT_OK) {
        LOGW("Decrypt set writer failed!");
        return CPT_ERROR;
    }

    if (decrypt.PreDecrypt() != CPT_OK) {
        LOGW("DecryptVideo PreDecrypt error");
        return CPT_ERROR;
    }

    if (decrypt.DoDecrypt(key.c_str(), key.length())) {
        LOGW("Decrypt decrypt file failed!");
        return CPT_ERROR;
    }

    if (decrypt.PostDecrypt() != CPT_OK) {
        LOGW("DecryptVideo PostDecrypt error");
        return CPT_ERROR;
    }

    *outFilename = inFilePath.ReplaceExtension(fileHeader->GetFormat()).value();

    RenameFile(filename, *outFilename);

    return CPT_OK;
}

errno_t EasyDecryptVideo(const string& inFilename, const string& outFilename, const string& key)
{
    LOGPOS();

    string tempOutFilename;
    if (EasyDecryptVideo(inFilename, key, &tempOutFilename) != CPT_OK) {
        LOGW("EncryptVideo error");
        return CPT_ERROR;
    }

    if (RenameFile(tempOutFilename, outFilename) != CPT_OK) {
        LOGW("MoveFile error. from %s to %s", tempOutFilename.c_str(), outFilename.c_str());
        return CPT_ERROR;
    }

    return CPT_OK;
}

errno_t EasyLoadHeaders(const string& filename, FileHeader *fileHeader)
{
    LOGPOS();
    return CPT_ERROR;
}

/*
///////////////////////////////
////CommonEncrypt
errno_t CommonEncrypt::DoEncrypt(const byte_t key[], const size_t len)
{
    LOGPOS();
    
    if (key == NULL || len  <= 0) {
        LOGW("Invalid Arguments!");
        return CPT_ERROR;
    }

    FileHeader fileHeader;
    fileHeader.RandomKey();
    fileHeader.SetFileTag();

    ASSERT(fileHeader.CheckFileTag() == CPT_OK);

    RC4 rc4;
    rc4.Init(key, len);

    char tempBuffer[kBufferLength];
    int32_t tempLength = rc4.XORStream(fileHeader, sizeof(FileHeader), tempBuffer, kBufferLength);

    if (tempLength != sizeof(FileHeader)) {
        LOGW("Encrypt file header failed!");
        return CPT_ERROR;
    }

    OutFile outFile;
    if (outFile.Open(outFilePath_.value()) != CPT_OK) {
        LOGW("Encrypt failed.");
        return CPT_ERROR;
    }

    if (outFile.Write(buf, tempLength) != tempLength) {
        LOGW("Write failed.");
        return CPT_ERROR;
    }

    tempLength = kBufferLength;
    fileHeader.GetKey(tempBuffer, &tempLength);
    
    rc4.Reset();
    rc4.Init(tempBuffer, tempLength);
    
    InFile inFile;
    if (inFile.Open(inFilePath_.value()) != CPT_OK) {
        LOGW("Can't create file %s!", inFilePath_.value());
        return CPT_ERROR;
    }

    for (;;) {
        tempLength = inFile.Read(tempBuffer, kBufferLength);
        // 只考虑了正常读写完成的情况。对于读写错误时，返回值也可能为<=0。
        // 需要额外的处理，对于文件读写故障(如：TF卡被卸载)时进行错误报告。
        if (tempLength <= 0) 
            break;

        rc4.XORStream(tempBuffer, tempLength);

        if (outFile.Write(tempBuffer, tempLength) != tempLength) {
            LOGW("Write to file %s failed!", outFilePath_.value());
            return CPT_ERROR;
        }
    }
}

///////////////////////////
////CommonDecrypt
errno_t CommonDecrypt::DoDecrypt(const byte_t key[], const size_t len) 
{
    LOGPOS();

    if (const byte_t key[], const size_t len) {
        LOGW("Invalid Arguments!");
        return CPT_ERROR;
    }

    InFile inFile;
    if (inFile.Open(inFilePath_.value()) != CPT_OK) {
        LOGW("Open file %s failed!", inFilePath_.value());
        return CPT_ERROR;
    }

    FileHeader fileHeader;
    
    fileHeader.ReadFrom(inFile);
    if (fileHeader.CheckFileTag() != CPT_OK) {
        LOGW("Bad file format %s!", inFilePath_.value());
        return CPT_ERROR;
    }

    RC4 rc4;
    rc4.Init(key);

    int32_t tempLength = rc4.XORStream(&fileHeader, sizeof(FileHeader));
    if (tempLength != sizeof(FileHeader)) {
        LOGW("Decrypt file header failed!");
        return CPT_ERROR;
    }

    char tempBuffer[kBufferLength];
    tempLength = kBufferLength;
    fileHeader.GetKey(tempBuffer, &tempLength);

    rc4.Reset();
    rc4.Init(tempBuffer, tempLength);

    OutFile outFile;
    if (outFile.Open(outFilePath_.value()) != CPT_OK) {
        LOGW("Open file %s failed!", outFilePath_.value());
        return CPT_ERROR;
    } 

    for (;;) {
        tempLength = inFile.Read(tempBuffer, kBufferLength);
        // 只考虑了正常读写完成的情况。对于读写错误时，返回值也可能为<=0。
        // 需要额外的处理，对于文件读写故障(如：TF卡被卸载)时进行错误报告。
        if (tempLength <= 0) 
            break;

        rc4.XORStream(tempBuffer, tempLength);
        if (outFile.Write(tempBuffer, tempLength) != tempLength) {
            LOGW("Write to file %s failed!", outFilePath_.value());
            return CPT_ERROR;
        }
    }
}*/