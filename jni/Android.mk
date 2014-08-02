LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cryptopg
LOCAL_SRC_FILES := JCrypto.cpp Crypto.cpp FileHeader.cpp JUtil.cpp Util.cpp FilePath.cpp MemoryIO.cpp RC4.cpp FileIO.cpp Debug.cpp FileUtil.cpp RandUtil.cpp
LOCAL_LDLIBS    := -lm -llog

include $(BUILD_SHARED_LIBRARY)
