LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := lua

# Compile as C++
#LOCAL_CFLAGS += -x c++
#LOCAL_CPPFLAGS := -std=c++14 -frtti -fexceptions

# Add your application source files here...
LOCAL_SRC_FILES := $(LOCAL_PATH)/src/lapi.c $(LOCAL_PATH)/src/lcode.c $(LOCAL_PATH)/src/lctype.c $(LOCAL_PATH)/src/ldebug.c $(LOCAL_PATH)/src/ldo.c $(LOCAL_PATH)/src/ldump.c $(LOCAL_PATH)/src/lfunc.c $(LOCAL_PATH)/src/lgc.c $(LOCAL_PATH)/src/llex.c $(LOCAL_PATH)/src/lmem.c $(LOCAL_PATH)/src/lobject.c $(LOCAL_PATH)/src/lopcodes.c $(LOCAL_PATH)/src/lparser.c $(LOCAL_PATH)/src/lstate.c $(LOCAL_PATH)/src/lstring.c $(LOCAL_PATH)/src/ltable.c $(LOCAL_PATH)/src/ltm.c $(LOCAL_PATH)/src/lundump.c $(LOCAL_PATH)/src/lvm.c $(LOCAL_PATH)/src/lzio.c $(LOCAL_PATH)/src/lauxlib.c $(LOCAL_PATH)/src/lbaselib.c $(LOCAL_PATH)/src/lbitlib.c $(LOCAL_PATH)/src/lcorolib.c $(LOCAL_PATH)/src/ldblib.c $(LOCAL_PATH)/src/liolib.c $(LOCAL_PATH)/src/lmathlib.c $(LOCAL_PATH)/src/loslib.c $(LOCAL_PATH)/src/lstrlib.c $(LOCAL_PATH)/src/ltablib.c $(LOCAL_PATH)/src/lutf8lib.c $(LOCAL_PATH)/src/loadlib.c $(LOCAL_PATH)/src/linit.c
#LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/src/*.c)
#LOCAL_SRC_FILES -= $(LOCAL_PATH)/src/lua.c $(LOCAL_PATH)/src/luac.c

LOCAL_CFLAGS += -std=c99 -DLUA_USE_C89
LOCAL_LDLIBS := -llog

#include $(BUILD_STATIC_LIBRARY)
include $(BUILD_SHARED_LIBRARY)
