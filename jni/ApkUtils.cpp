#include <jni.h>
#include <stddef.h>
#include <dlfcn.h>

#include <libdex.h>
#include <libsocketMonitor.h>
#include <Bridge.h>
#include <support/Mcro_Common.h>

static JavaVM *GetJavaVM();
static JNIEnv *GetJNIEnv();

typedef struct _GlobalVar{
	JavaVM *g_JavaVM;
	Dex *dex;
	Bridge *bridge;
	SocketMonitor *socketMonitor;
	int g_isAttated;
	JavaVM *(*GetJavaVM)();
	JNIEnv *(*GetJNIEnv)();
}GlobalVar;

static GlobalVar globalVarObj = {NULL};
static GlobalVar *global = &globalVarObj;

static JavaVM *GetJavaVM(){

	_FUNC_START
	JavaVM *javaVM = NULL;
	void *handle = NULL;
	do{
		handle = dlopen("libandroid_runtime.so", RTLD_NOW);
		if(handle == NULL){
			LOGD("[-]Load libandroid_runtime.so fail: %s", dlerror());
			_BREAK_DOWHILE
		}

		LOGI("handle \t 0x%08X", (unsigned int)(handle));

		void * AndroidRuntime_mJavaVM = dlsym(handle, "_ZN7android14AndroidRuntime7mJavaVME");
		if(AndroidRuntime_mJavaVM == NULL)
		{
			LOGD("[-]get AndroidRuntime_mJavaVM fail: %s", dlerror());
			_BREAK_DOWHILE
		}
		javaVM = (JavaVM *)AndroidRuntime_mJavaVM;
	} while (0);
	_JUDGE_RETFLAG_VALUE
	if(handle != NULL)
		dlclose(handle);
	_FUNC_END_OBJ(javaVM)
}

static JNIEnv *GetJNIEnv(){

	_FUNC_START
	JNIEnv *env = NULL;
	do{
		if(global->g_JavaVM == NULL)										{_BREAK_DOWHILE}
		LOGI("global->g_JavaVM \t 0x%08X", (unsigned int)(global->g_JavaVM));
//		retFlag = global->g_JavaVM->GetEnv((void **)&env, JNI_VERSION_1_4);
//		if(retFlag != JNI_OK){
			if(global->g_JavaVM->AttachCurrentThread(&env, NULL) < 0)		{_BREAK_DOWHILE}
			global->g_isAttated = 1;
//		}
	} while (0);
	_JUDGE_RETFLAG_VALUE
	_FUNC_END_OBJ(env)
}

jint JNI_OnLoad(JavaVM* vm, void* reserved){
     JNIEnv* env = NULL;
     jint result = -1;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
          LOGE("ERROR: GetEnv failed\n");
          return -1;
    }

	_FUNC_START
	do {
		if(global == NULL)	{_BREAK_DOWHILE}
		global->GetJNIEnv = GetJNIEnv;
		global->GetJavaVM = GetJavaVM;
		global->bridge = NULL;
		global->socketMonitor = NULL;
		global->g_isAttated = 0;
		global->g_JavaVM = vm;
		global->dex = NULL;

	//		if((global->g_JavaVM = global->GetJavaVM()) == NULL)		{_BREAK_DOWHILE}
	//		if((env = global->GetJNIEnv()) == NULL)						{_BREAK_DOWHILE}
	//		if((global->bridge->registerNativeMethod()) == -1)			{_BREAK_DOWHILE}
		if((global->socketMonitor->registerMonitor()) == -1)		{_BREAK_DOWHILE}
		if((global->socketMonitor->start()) == -1)					{_BREAK_DOWHILE}

	} while (0);
	_JUDGE_RETFLAG_VALUE
	_FUNC_END_OBJ(JNI_VERSION_1_4)
}

static void DetachCurrent(){

	_FUNC_START
	do {
	    if(global->g_isAttated){
	    	global->g_JavaVM->DetachCurrentThread();
	    }
	} while (0);
	_JUDGE_RETFLAG_VALUE
	_FUNC_END
}

void init_register() __attribute__((constructor));
void init_register(){
	_FUNC_START
    JNIEnv *env = NULL;
	do {
		LOGD("[+] %s", "init_register");
	} while (0);
	_JUDGE_RETFLAG_VALUE
	_FUNC_END
}
