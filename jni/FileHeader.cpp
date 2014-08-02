#include "FileHeader.h"

#include "Config.h"
#include "Debug.h"
#include "Error.h"
#include "FileIO.h"
#include "Util.h"

const char FileHeader::kFileTag[] = "XXXX";
const char FileHeader::kEncryptedFileTag[] = "\x39\x05\xFE\x52";

namespace {
    const size_t kBufferLength = 2048;
}

/////////////////////////////
////FileHeader
FileHeader::FileHeader() 
{
    Reset();
}

errno_t FileHeader::ReadFrom(Reader *in)
{
    LOGPOS();
    int len = in->Read(this, sizeof(FileHeader));
    if (len != sizeof(FileHeader))
        return CPT_ERROR;
    return CheckFileTag();
}

errno_t FileHeader::WriteTo(Writer *out)
{
    LOGPOS();
    int lengthTemp = out->Write(this, sizeof(FileHeader));
    if (lengthTemp != sizeof(FileHeader)) 
        return CPT_ERROR;
    return CPT_OK;
    // return CPT_ERROR;
}

void FileHeader::Reset()
{
    LOGPOS();
    ZeroMemory(this, sizeof(FileHeader));
    RandomOffset();
    RandomSample();
}

errno_t FileHeader::CheckFileTag()
{
    LOGPOS();

    if (CompareMemory(kFileTag, tag_, TAG_SIZE) == 0) 
        return CPT_OK;

    //if (CompareMemory(kEncryptedFileTag, tag_, TAG_SIZE) == 0)
    //    return CPT_OK;
    
    return CPT_ERROR;
}

void FileHeader::SetFileTag()
{
    LOGPOS();
    CopyMemory(tag_, kFileTag, TAG_SIZE);
}

void FileHeader::RandomKey()
{
    LOGPOS();
    RandBytes(key_, KEY_SIZE);
    return ;
}

errno_t FileHeader::SetKey(const char key[], const size_t len)
{
    LOGPOS();

    if (key == NULL) {
        LOGW("Key is NULL!");
        return CPT_ERROR;
    }

    if (len <= 0 || len >= KEY_SIZE) {
        LOGW("The length of the key %d is not valid!", len);
        return CPT_ERROR;
    }

    ZeroMemory(key_, KEY_SIZE);
    CopyMemory(key_, key, len);

    return CPT_OK;
}

errno_t FileHeader::GetKey(char *key, size_t *len)
{
    LOGPOS();
    if (key == NULL || len == NULL) {
        LOGW("The args is NOT valid!");
        return CPT_ERROR;
    }
    if (*len <= KEY_SIZE) {
        LOGW("Not enough buf.");
        return CPT_ERROR;
    }
    CopyMemory(key, key_, KEY_SIZE);
    *len = KEY_SIZE;
}

void FileHeader::RandomOffset()
{
    LOGPOS();
    Int32ToOctets(1024, offset_);
}

void FileHeader::SetOffset(size_t offset)
{   
    LOGPOS();
    Int32ToOctets(offset, offset_);
}

uint32_t FileHeader::GetOffset()
{
    LOGPOS();
    return OctetsToInt32(offset_);
}

void FileHeader::RandomSample()
{
    LOGPOS();
    Int32ToOctets(256, sample_);
}

uint32_t FileHeader::GetSample()
{
    return OctetsToInt32(sample_);
}

errno_t FileHeader::SetFormat(const std::string& format)
{
    LOGPOS();

    if (format.length() > FORMAT_SIZE) {
        LOGW("Extension is too long!");
        return CPT_ERROR;
    }
    ZeroMemory(format_, FORMAT_SIZE);
    CopyString(reinterpret_cast<char*>(format_), format.data(), format.length());
    return CPT_OK;
}

const char *FileHeader::GetFormat()
{
    LOGPOS();
    return reinterpret_cast<char*>(format_);
}

errno_t FileHeader::LoadHeader(const FilePath& filePath)
{
    LOGPOS();
    FileReader in; // Will be autoclose.

    if (!in.Open(filePath.value())) {
        LOGW("Open File %s for Read failed.", filePath.value().c_str());
        return CPT_ERROR;
    }

    if (!ReadFrom(&in)) {
        LOGW("Load header from %s failed", filePath.value().c_str());
        return CPT_ERROR;
    }

    return CPT_OK;
}

