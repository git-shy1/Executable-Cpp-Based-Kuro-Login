LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libcurl
LOCAL_SRC_FILES := curl/arm64-v8a/libcurl.a
LOCAL_C_INCLUDES := include/curl
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := LoginWR.sh

LOCAL_CPPFLAGS := -fvisibility=hidden -Wno-narrowing -Wno-error=format-security -fpermissive -w -Werror -s -std=c++17 -fno-rtti -frtti -fno-exceptions -fms-extensions -Wno-error=c++11 -O3

LOCAL_SRC_FILES := Main.cpp \
        frnetlib/Base64.cpp \
        frnetlib/Http.cpp \
        frnetlib/HttpRequest.cpp \
        frnetlib/HttpResponse.cpp \
        frnetlib/Sha1.cpp \
        frnetlib/Socket.cpp \
        frnetlib/SocketSelector.cpp \
        frnetlib/TcpListener.cpp \
        frnetlib/TcpSocket.cpp \
        frnetlib/URL.cpp \
        frnetlib/WebFrame.cpp \
        include/oxorany.cpp \
        zMemory/ZMemory.cpp \
        zMemory/ZUtils.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
                         $(LOCAL_PATH)/include/frnetlib
LOCAL_LDLIBS := -llog -landroid -lz -lEGL -lGLESv2
LOCAL_CPP_FEATURES := exceptions
LOCAL_STATIC_LIBRARIES := libcurl
include $(BUILD_EXECUTABLE)
