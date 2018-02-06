LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := lpng

LOCAL_CPPFLAGS := -std=c++14 -frtti -fexceptions

# Add your application source files here...
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/*.c)

LOCAL_SHARED_LIBRARIES := zlib

#include $(BUILD_STATIC_LIBRARY)
include $(BUILD_SHARED_LIBRARY)
