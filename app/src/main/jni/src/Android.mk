LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/../SDL2_ttf $(LOCAL_PATH)/../SDL2_mixer $(LOCAL_PATH)/../zlib $(LOCAL_PATH)/../lpng $(LOCAL_PATH)/../lua/src
LOCAL_CPPFLAGS := -std=c++14 -frtti -fexceptions

# Add your application source files here...
LOCAL_SRC_FILES := android_support.cpp screen.cpp main_mobile.cpp ant.cpp graphics.cpp graphicsn.cpp leveldata.cpp leveldisplay.cpp levelplayer.cpp levelset.cpp luaclass.cpp main.cpp pngloader.cpp recorder.cpp sha1.cpp solvedmap.cpp soundsys.cpp textsections.cpp tools.cpp window.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf SDL2_mixer lpng zlib lua
#LOCAL_STATIC_LIBRARIES := lpng zlib lua

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
