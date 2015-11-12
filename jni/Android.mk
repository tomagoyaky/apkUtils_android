LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := ApkUtils
LOCAL_SRC_FILES := ApkUtils.cpp\
	Bridge.c\
	model/monitor/NetWorkRequest/libsocketMonitor.c\
	model/dex/libdex.cpp\

LOCAL_C_INCLUDES :=	$(LOCAL_PATH)\
	$(LOCAL_PATH)/model/dex\
	$(LOCAL_PATH)/model/elf\
	$(LOCAL_PATH)/model/hook\
	$(LOCAL_PATH)/model/monitor/NetWorkRequest\

LOCAL_LDFLAGS := -lm -llog -landroid
include $(BUILD_SHARED_LIBRARY)