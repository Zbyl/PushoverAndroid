LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := lua

# Add your application source files here...
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/src/*.c)

include $(BUILD_STATIC_LIBRARY)
