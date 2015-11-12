LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := ptraceAttach
LOCAL_SRC_FILES := ptraceAttach.cpp
LOCAL_LDLIBS:= -llog

LOCAL_CFLAGS += -pie -fPIE
LOCAL_LDFLAGS += -pie -fPIE
include $(BUILD_EXECUTABLE)

#include $(BUILD_SHARED_LIBRARY)
