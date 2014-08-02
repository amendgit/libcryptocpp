#include "JUtil.h"

#include <string>

#include "Config.h"
#include "Debug.h"
#include "Error.h"


using namespace std;

// Return NULL if failed.
jfieldID GetClassField(JNIEnv *env, jobject obj, const char *method, 
    const char *signture)
{
    LOGPOS();

    jclass clazz = (jclass) env->GetObjectClass(obj);
    if (clazz == NULL) {
        return NULL;
    }

    return static_cast<jfieldID>(env->GetFieldID(clazz, method, signture));
}

void *GetClassInstance(JNIEnv *env, jobject obj)
{
    LOGPOS();
    jfieldID instanceFieldID = GetClassField(env, obj, "instance", "J");
    if (instanceFieldID == NULL) {
        return NULL;
    }

    int64_t i64 = env->GetLongField(obj, instanceFieldID);
    if (i64 <= 0) {
        return NULL;
    }

    return reinterpret_cast<void*>(i64 & 0xffffffff);
}

errno_t SetClassInstance(JNIEnv *env, jobject obj, void *instance)
{
    LOGPOS();

    int64_t i64 = reinterpret_cast<uintptr_t>(instance) & 0x00000000ffffffff;

    jfieldID instanceFieldID = GetClassField(env, obj, "instance", "J");
    if (instanceFieldID == NULL) {
        return CPT_ERROR;
    }

    env->SetLongField(obj, instanceFieldID, i64);

    return CPT_OK;
}

void JByteArrayCopyToCString(JNIEnv *env, jbyteArray byteArray, std::string *str)
{
    LOGPOS();

    if (byteArray == NULL || str == NULL) {
        LOGW("Invalid Arguments!");
        return ;
    }

    size_t len = env->GetArrayLength(byteArray);
    str->resize(len);
    if (str->size() < len) {
        LOGW("Allocate memory failed!");
    }
    env->GetByteArrayRegion(byteArray, 0, len, 
        reinterpret_cast<signed char*>(const_cast<char*>(str->data())));;
}

void CStringCopyToJByteString(JNIEnv *env, const char *data, const size_t len, jbyteArray byteArray)
{
    LOGPOS();
    env->SetByteArrayRegion(byteArray, 0, len, reinterpret_cast<const signed char*>(data));
}

void RegisterNativeMethods(JNIEnv *env, const char *clazzName, 
    const JNINativeMethod *methods, uint32_t n)
{
    LOGPOS();

    jclass clazz = env->FindClass(clazzName);
    ASSERT(clazz != NULL);

    if (env->RegisterNatives(clazz, methods, n) != JNI_OK) {
        LOGE("Register methods for %s", clazzName);
    }
}