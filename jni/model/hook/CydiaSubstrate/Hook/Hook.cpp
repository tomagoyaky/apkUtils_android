#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "substrate.h"
#include "HFile/NativeLog.h"
#include "Dump_Dex/DumpDex.H"

//Dvm函数对应表
#define libdvm			"/system/lib/libdvm.so"
#define dexFileParse	"_Z12dexFileParsePKhji"
//这里是解析DexFile
void* (*_dexFileParse)(int *, unsigned int, int);
void* My_dexFileParse(int *inDex, unsigned int length, int parseFlags){
	LOGD("My_dexFileParse:inDex:0x%08X,length:%d",inDex,length);
	void* Out_DexFile = _dexFileParse(inDex,length,parseFlags);
	LOGD("My_dexFileParse:DexFile:0x%08X",Out_DexFile);
	//将获取的DexFile Dump出文件
	Dump_DexFile(Out_DexFile);
	return Out_DexFile;
}

/**
 *			Hook_Main
 *align_Len
 */
 static bool first = true;
int Hook_Main(){
	LOGD("Hook_Main");
	if(first != true)return -1;
	first = false;
	LOGD("Hook_Main StartHook");
	MSImageRef image = MSGetImageByName(libdvm);
	void* mFun = NULL;
	if(image != NULL){		
		mFun = MSFindSymbol(image,dexFileParse);
		if(mFun != NULL){
			MSHookFunction(mFun,(void*)&My_dexFileParse,(void**)&_dexFileParse);
		}

		
	}
	
	return 0;
}

