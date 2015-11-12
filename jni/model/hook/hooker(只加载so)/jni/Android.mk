LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := hookclient
LOCAL_SRC_FILES := hookclient.cpp \
	hook.cpp \
	shellcode.s
LOCAL_LDLIBS:= -llog

LOCAL_CFLAGS += -pie -fPIE
LOCAL_LDFLAGS += -pie -fPIE
include $(BUILD_EXECUTABLE)

#include $(BUILD_SHARED_LIBRARY)
