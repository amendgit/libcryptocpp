#ifndef CPT_JUTIL_H
#define CPT_JUTIL_H 

#include <jni.h>
#include <string>

#include "Config.h"
#include "Error.h"

jfieldID GetClassField(JNIEnv *env, jobject obj, const char *method, const char *signture);

void *GetClassInstance(JNIEnv *env, jobject obj);

errno_t SetClassInstance(JNIEnv *env, jobject obj, void *instance);

void JByteArrayCopyToCString(JNIEnv *env, jbyteArray byteArray, std::string *str);

void CStringCopyToJByteString(JNIEnv *env, const char *data, const size_t len, jbyteArray byteArray);

void RegisterNativeMethods(JNIEnv *env, const char *clazz, const JNINativeMethod *methods, uint32_t n);

#endif /* CPT_JUTIL_H */