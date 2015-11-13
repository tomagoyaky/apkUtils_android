LOCAL_PATH := $(call my-dir)
# substrate 环境
include $(CLEAR_VARS)
LOCAL_MODULE:= substrate-dvm
LOCAL_SRC_FILES := so/libsubstrate-dvm.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= substrate
LOCAL_SRC_FILES := so/libsubstrate.so
include $(PREBUILT_SHARED_LIBRARY)

# 通用静态库
include $(CLEAR_VARS)
LOCAL_MODULE    := lib-Common
LOCAL_SRC_FILES := Common/Common.cpp
include $(BUILD_STATIC_LIBRARY)


# Hook静态库
include $(CLEAR_VARS)
LOCAL_MODULE    := lib-Hook
LOCAL_SRC_FILES := Hook/Hook.cpp
include $(BUILD_STATIC_LIBRARY)


#DumpDex静态库
include $(CLEAR_VARS)

LOCAL_MODULE    := lib-Dump_Dex
LOCAL_SRC_FILES := Dump_Dex/Dump_Dex.cpp \
	Dump_Dex/Dex_ClassDef.cpp \
	Dump_Dex/Dex_Util.cpp   \
	Dump_Dex/Dex_Header.cpp  \
	Dump_Dex/Dex_Map.cpp

include $(BUILD_STATIC_LIBRARY)

# MAIN
include $(CLEAR_VARS)
LOCAL_MODULE    := SubstrateHook.cy
LOCAL_SRC_FILES := SubstrateHook.cy.cpp 
LOCAL_LDLIBS := -llog
LOCAL_ARM_MODE := arm
LOCAL_LDLIBS += -L$(LOCAL_PATH) -lsubstrate-dvm -lsubstrate 
LOCAL_STATIC_LIBRARIES := lib-Common lib-Hook lib-Dump_Dex
include $(BUILD_SHARED_LIBRARY)
