// JNI的主要实现
#include <jni.h>
#include <iostream>
#include <string>

#include "Config.h"
#include "Crypto.h"
#include "Debug.h"
#include "Error.h"
#include "FileHeader.h"
#include "JUtil.h"
#include "RC4.h"

using namespace std;

namespace {
    /////////////////////////////////////
    ////RawRC4 
    jint 
    RawRC4New(JNIEnv *env, jobject thiz, jbyteArray jkey)
    {
        LOGPOS();

        string key;
        JByteArrayCopyToCString(env, jkey, &key);
        
        RC4 *rc4 = new RC4();
        if (rc4->Init(key.c_str(), key.length()) != CPT_OK) {
            LOGW("Error during init key for rc4!");
            return CPT_ERROR;
        }

        if (rc4 == NULL) {
            LOGE("Construct RC4 Failed with %s", key.c_str());
            return CPT_ERROR;
        }

        SetClassInstance(env, thiz, static_cast<void*>(rc4));

        return CPT_OK;
    }

    jint 
    RawRC4XORStream(JNIEnv *env, jobject thiz, jbyteArray jbuf) 
    {
        LOGPOS();
        RC4 *rc4 = static_cast<RC4*>(GetClassInstance(env, thiz));
        ASSERT(rc4 != NULL);

        signed char *data = env->GetByteArrayElements(jbuf, 0);
        const size_t len = env->GetArrayLength(jbuf);
        
        rc4->XORStream(data, len);
        
        env->ReleaseByteArrayElements(jbuf, data, JNI_COMMIT);

        return CPT_OK;
    }

    void 
    RawRC4Reset(JNIEnv *env, jobject thiz)
    {
        LOGPOS();

        RC4 *rc4 = static_cast<RC4*>(GetClassInstance(env, thiz));
        ASSERT(rc4 != NULL);

        rc4->Reset();

        return ;
    }

    void 
    RawRC4Destroy(JNIEnv *env, jobject thiz)
    {
        LOGPOS();

        RC4 *rc4 = static_cast<RC4*>(GetClassInstance(env, thiz));
        if (rc4 != NULL) {
            delete rc4;
            rc4 = NULL;
        }

        return ;
    }

    void
    RegisterRawRC4(JNIEnv *env)
    {
        LOGPOS();

        static JNINativeMethod methods[] = {
            { "RawNew",       "([B)I", (void*)RawRC4New},
            { "RawXORStream", "([B)I", (void*)RawRC4XORStream},
            { "RawReset",     "()V",   (void*)RawRC4Reset },
            { "RawDestroy",   "()V",   (void*)RawRC4Destroy}
        };
        RegisterNativeMethods(env, "com/mcs/crypto/RawRC4", methods, 
            sizeof(methods)/sizeof(methods[0]));
    }

    /////////////////////////////
    ////\RawCipherFormat
    jint
    RawCipherFormatNew(JNIEnv *env, jobject thiz) 
    {
        LOGPOS();

        FileHeader *fileHeader = new FileHeader();
        if (fileHeader == NULL) {
            LOGW("Construct FileHeader Failed!");
            return CPT_ERROR; 
        }

        SetClassInstance(env, thiz, fileHeader);
        return CPT_OK;
    }

    // 加密指定的文件
    // 文件名使用UTF-8编码，密钥使用ANSII编码。
    jint
    RawCipherFormatEncrypt(JNIEnv *env, jobject thiz, jbyteArray jplainFilename,
        jbyteArray jcipherFilename, jbyteArray jkey) 
    {
        LOGPOS();

        FileHeader *CipherFormat = static_cast<FileHeader*>(GetClassInstance(env, thiz));
        ASSERT(CipherFormat != NULL);

        string plainFilename;
        JByteArrayCopyToCString(env, jplainFilename, &plainFilename);

        string cipherFilename;
        JByteArrayCopyToCString(env, jcipherFilename, &cipherFilename);

        string key;
        JByteArrayCopyToCString(env, jkey, &key);

        // return EasyEncryptFile(plainFilename, cipherFilename, key);
        string tempOutFilename;
        return ::EasyEncrypt(plainFilename, key, &tempOutFilename);
    }

    jint 
    RawCipherFormatDecrypt(JNIEnv *env, jobject thiz, jbyteArray jcipherFilename,
        jbyteArray jplainFilename, jbyteArray jkey)
    {
        LOGPOS();

        FileHeader *fileHeader = static_cast<FileHeader*>(GetClassInstance(env, thiz));
        ASSERT(fileHeader != NULL);

        string cipherFilename;
        JByteArrayCopyToCString(env, jcipherFilename, &cipherFilename);

        string plainFilename;
        JByteArrayCopyToCString(env, jplainFilename, &plainFilename);

        string key;
        JByteArrayCopyToCString(env, jkey, &key);

        // return EasyDecryptFile(cipherFilename, plainFilename, key);
        string outFilename;
        return EasyDecrypt(cipherFilename, key, &outFilename);
    }

    jint
    RawCipherFormatLoadHeaders(JNIEnv *env, jobject thiz, jbyteArray jfilename)
    {
        LOGPOS();

        FileHeader *fileHeader = static_cast<FileHeader*>(GetClassInstance(env, thiz));
        ASSERT(fileHeader != NULL);

        string filename;
        JByteArrayCopyToCString(env, jfilename, &filename);

        return EasyLoadHeaders(filename, fileHeader);
    }

    void
    RawCipherFormatDestroy(JNIEnv *env, jobject thiz)
    {
        LOGPOS();

        FileHeader *fileHeader = static_cast<FileHeader*>(GetClassInstance(env, thiz));
        
        if (fileHeader != NULL) {
            delete fileHeader;
            fileHeader = NULL;
            SetClassInstance(env, thiz, NULL);
        }

        return ;
    }

    void 
    RegisterCipherFormat(JNIEnv *env)
    {
        LOGPOS();

        static JNINativeMethod methods[] = {
            {"RawNew",         "()I",       (void*)RawCipherFormatNew },
            {"RawEncrypt",     "([B[B[B)I", (void*)RawCipherFormatEncrypt },
            {"RawDecrypt",     "([B)[B",    (void*)RawCipherFormatDecrypt },
            //{"RawSetHeader",   "(I[B)I",    (void*)RawCipherFormatSetHeader },
            //{"RawGetHeader",   "(I)[B",     (void*)RawCipherFormatGetHeader },
            //{"RawSetFileKey",  "([B)I",     (void*)RawCipherFormatSetFileKey },
            //{"RawGetFileKey",  "()[B",      (void*)RawCipherFormatGetFileKey },
            {"RawLoadHeaders", "([B)I",     (void*)RawCipherFormatLoadHeaders },
            {"RawDestroy",     "()V",       (void*)RawCipherFormatDestroy }
        };
        RegisterNativeMethods(env, "com/mcs/crypto/CipherFormat", methods, 
            sizeof(methods)/sizeof(methods[0]));
    }

    //////////////////////////////
    ////\Crypto
    jbyteArray
    EasyEncrypt(JNIEnv *env, jclass clazz, jbyteArray jfilename, jbyteArray jkey)
    {
        LOGPOS();

        string filename;
        JByteArrayCopyToCString(env, jfilename, &filename);

        string key;
        JByteArrayCopyToCString(env, jkey, &key);

        string outFilename;
        if (::EasyEncrypt(filename, key, &outFilename) != CPT_OK) {
            return NULL;
        }

        jbyteArray joutFilename = env->NewByteArray(outFilename.length());
        CStringCopyToJByteString(env, outFilename.data(), outFilename.length(), joutFilename);

        return joutFilename;
    }

    jint
    EasyEncrypt2(JNIEnv *env, jclass clazz, jbyteArray jinFilename, jbyteArray joutFilename, jbyteArray jkey)
    {
        LOGPOS();

        string inFilename;
        JByteArrayCopyToCString(env, jinFilename, &inFilename);

        string outFilename;
        JByteArrayCopyToCString(env, joutFilename, &outFilename);

        string key;
        JByteArrayCopyToCString(env, jkey, &key);

        return ::EasyEncrypt(inFilename, outFilename, key);
    }

    jbyteArray 
    EasyDecrypt(JNIEnv *env, jclass clazz, jbyteArray jfilename, jbyteArray jkey)
    {
        LOGPOS();

        string filename;
        JByteArrayCopyToCString(env, jfilename, &filename);

        string key;
        JByteArrayCopyToCString(env, jkey, &key);

        string outFilename;
        if (::EasyDecrypt(filename, key, &outFilename) == CPT_OK) {
            return NULL;
        }

        jbyteArray joutFilename = env->NewByteArray(outFilename.length());
        CStringCopyToJByteString(env, outFilename.data(), outFilename.length(), joutFilename);

        return joutFilename;
    }

    jint
    EasyDecrypt2(JNIEnv *env, jclass clazz, jbyteArray jinFilename, jbyteArray joutFilename, jbyteArray jkey)
    {
        LOGPOS();

        string inFilename;
        JByteArrayCopyToCString(env, jinFilename, &inFilename);

        string outFilename;
        JByteArrayCopyToCString(env, joutFilename, &outFilename);

        string key;
        JByteArrayCopyToCString(env, jkey, &key);

        return ::EasyDecrypt(inFilename, outFilename, key);
    }

    jbyteArray 
    EasyDecryptInMemory(JNIEnv *env, jclass clazz, jbyteArray jfilename, jbyteArray jkey)
    {
        LOGPOS();

        string filename;
        JByteArrayCopyToCString(env, jfilename, &filename);

        string key;
        JByteArrayCopyToCString(env, jkey, &key);

        MemoryWriter writer;
        EasyDecryptInMemory(filename, key, &writer);
        
        const char *buf = writer.GetMemory();
        size_t len = writer.GetLength();
        LOGI("len: %d", len);
        jbyteArray ret = env->NewByteArray(len);
        CStringCopyToJByteString(env, buf, len, ret);

        return ret;
    }

    jbyteArray
    EasyEncryptVideo(JNIEnv *env, jclass clazz, jbyteArray jfilename, jbyteArray jkey)
    {
        LOGPOS();

        string filename;
        JByteArrayCopyToCString(env, jfilename, &filename);

        string key;
        JByteArrayCopyToCString(env, jkey, &key);

        string outFilename;
        if (::EasyEncryptVideo(filename, key, &outFilename) != CPT_OK) {
            return NULL;
        }

        jbyteArray joutFilename = env->NewByteArray(outFilename.length());
        CStringCopyToJByteString(env, outFilename.data(), outFilename.length(), joutFilename);

        return joutFilename;
    }

    jint
    EasyEncryptVideo2(JNIEnv *env, jclass clazz, jbyteArray jinFilename, jbyteArray joutFilename, jbyteArray jkey)
    {
        LOGPOS();

        string inFilename;
        JByteArrayCopyToCString(env, jinFilename, &inFilename);

        string outFilename;
        JByteArrayCopyToCString(env, joutFilename, &outFilename);

        string key;
        JByteArrayCopyToCString(env, jkey, &key);

        return ::EasyEncryptVideo(inFilename, outFilename, key);
    }

    jbyteArray
    EasyDecryptVideo(JNIEnv *env, jclass clazz, jbyteArray jfilename, jbyteArray jkey)
    {
        LOGPOS();

        string filename;
        JByteArrayCopyToCString(env, jfilename, &filename);

        string key;
        JByteArrayCopyToCString(env, jkey, &key);

        string outFilename;
        if (::EasyDecryptVideo(filename, key, &outFilename) != CPT_OK) {
            return NULL;
        }

        jbyteArray joutFilename = env->NewByteArray(outFilename.length());
        CStringCopyToJByteString(env, outFilename.data(), outFilename.length(), joutFilename);

        return joutFilename;
    }

    jint
    EasyDecryptVideo2(JNIEnv *env, jclass clazz, jbyteArray jinFilename, jbyteArray joutFilename, jbyteArray jkey)
    {
        LOGPOS();

        string inFilename;
        JByteArrayCopyToCString(env, jinFilename, &inFilename);

        string outFilename;
        JByteArrayCopyToCString(env, joutFilename, &outFilename);

        string key;
        JByteArrayCopyToCString(env, jkey, &key);

        return ::EasyDecryptVideo(inFilename, outFilename, key);
    }

    void 
    RegisterCrypto(JNIEnv *env)
    {
        LOGPOS();

        static JNINativeMethod methods[] = {
            {"easyEncrypt",         "([B[B)[B",  (void*)EasyEncrypt },
            {"easyEncrypt",         "([B[B[B)I", (void*)EasyEncrypt2 },
            {"easyDecrypt",         "([B[B)[B",  (void*)EasyDecrypt },
            {"easyDecrypt",         "([B[B[B)I", (void*)EasyDecrypt2 },
            {"easyDecryptInMemory", "([B[B)[B",  (void*)EasyDecryptInMemory },
            {"easyEncryptVideo",    "([B[B)[B",  (void*)EasyEncryptVideo },
            {"easyEncryptVideo",    "([B[B[B)I", (void*)EasyEncryptVideo2 },
            {"easyDecryptVideo",    "([B[B)[B",  (void*)EasyDecryptVideo },
            {"easyDecryptVideo",    "([B[B[B)I", (void*)EasyDecryptVideo2 },
        };

        RegisterNativeMethods(env, "com/mcs/crypto/Crypto", methods, 
            sizeof(methods)/sizeof(methods[0]));
    }
}

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void* /*reserved*/)
{
    LOGPOS();

    JNIEnv *env;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("vm->GetEnv((void**)&env, JNI_VERSION_1_4) Failed!");
        return JNI_ERR;
    }

    RegisterRawRC4(env);
    RegisterCipherFormat(env);
    RegisterCrypto(env);

    return JNI_VERSION_1_4;
}
