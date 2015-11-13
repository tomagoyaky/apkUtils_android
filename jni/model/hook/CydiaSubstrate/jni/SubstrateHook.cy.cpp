#include <stdio.h>
#include <unistd.h>
#include "substrate.h"
#include <sys/stat.h>
#include "Common/Common.H"
#include "Hook/Hook.H"
#include "NativeLog.h"

//存放读取的配置文件
char* Config;
//存放全部需要进程Hook的进程名
MSConfig(MSFilterLibrary, "/system/lib/libdvm.so");
//Dvm函数对应表
#define libdvm				"/system/lib/libdvm.so"
#define dvmLoadNativeCode	"_Z17dvmLoadNativeCodePKcP6ObjectPPc"
//Hook dvmLoadNativeCode
bool (*_dvmLoadNativeCode)(const char* pathName, void* classLoader, char** detail);
bool My_dvmLoadNativeCode(const char* pathName, void* classLoader, char** detail){
	LOGD("dvmLoadNativeCode:%s",pathName);
	char* mName = getSelfName();
	LOGD("dvmLoadNativeCode name:%s,tid:%d",mName,gettid());	
	if(Config == NULL)return _dvmLoadNativeCode(pathName,classLoader,detail);
	LOGD("dvmLoadNativeCode Config:%s",Config);	
	if((strstr(Config,mName)!= NULL)){
		LOGD("dvmLoadNativeCode Hook_Main");
		Hook_Main();
	}	
	return _dvmLoadNativeCode(pathName,classLoader,detail);
}
/**
 *			MSInitialize
 *	程序入口点，
 * 		一定是最开始运行，但是不一定是进程中最开始运行
 *
 */
MSInitialize
{
	//获取当前进程号，名称
	char* mName = getSelfName();
	LOGD("MSInitialize name:%s,tid:%d",mName,gettid());	
	//获取配置文件
	Config = getConfig();
	LOGD("MSInitialize Config:%s",Config);
	LOGD("MSInitialize@Hook dvmLoadNativeCode");	
	//开始Hook 一些基本函数
	//
	MSImageRef image = MSGetImageByName(libdvm);
	void* mFun = NULL;
	if(image != NULL){		
		mFun = MSFindSymbol(image,dvmLoadNativeCode);
		if(mFun != NULL){
			MSHookFunction(mFun,(void*)&My_dvmLoadNativeCode,(void**)&_dvmLoadNativeCode);
		}
	}	
}

