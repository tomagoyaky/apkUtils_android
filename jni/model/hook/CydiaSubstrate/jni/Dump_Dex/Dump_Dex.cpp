#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include "NativeLog.h"
#include "Dump_Dex.H"
#include "Dex_ClassDef.H"
#include "Dex_Header.H"
#include "Dex_Map.H"
#include "Dex_Util.H"
#include "Module_Mem.H"
//
Mod_Mem *mem = NULL;
//__________________________________________________________
/*
*					Dump说明
* 1.传入参数为DexFile*
* 2.通过被解析的DexFile*,得知BassAddr和Dex Length
* 3.将Dex内存连续的数据全部获取
* 4.修复顺序:
*	i、Class_Def修复Class_Def偏移。
*	ii、
*	iii、
* 5.保存文件
*/
//__________________________________________________________
/*		
*				关于修复
* 1.不需要修复Mapitems
* 2.根据DexFile结构体，判断需要的内存是否的Dex所需的内存内。
*	不存在内存中就要进行修复偏移。
* 3.
*/
//__________________________________________________________

/*
 * 日志打印，
 *	这个传入被解析后的DexFile指针
 *	1.从DexFile中可以获取Dex开始地址和长度 -> 连续内存DEX Dump后可以直接使用
 *	2.非连续内存，需要将溢出内存数据放到文件尾端，然后修改偏移
 *	3.程序可能加载了DexFile后后续才修复Dex内容，所以我们先延时10S，再分析
 *
 */
void LOGDexFile(DexFile* DexF)
{
	/*
	*/
	DumpD("++++++++++++++++LOGDexFile+++++++++++++++++++");
	DumpD("[1]Struct DexFile");
	DumpD("LOGDexFile:0x%08x",DexF);
	DumpD("DexOptHeader:0x%08x",DexF->pOptHeader);
	DumpD("DexHeader:0x%08x",DexF->pHeader);
	DumpD("DexStringId:0x%08x",DexF->pStringIds);
	DumpD("DexTypeId:0x%08x",DexF->pTypeIds);
	DumpD("DexFieldId:0x%08x",DexF->pFieldIds);
	DumpD("DexMethodId:0x%08x",DexF->pMethodIds);
	DumpD("DexProtoId:0x%08x",DexF->pProtoIds);
	DumpD("DexClassDef:0x%08x",DexF->pClassDefs);
	DumpD("DexLink:0x%08x",DexF->pLinkData);
	DumpD("DexClassLookup:0x%08x",DexF->pClassLookup);
	DumpD("DexMap:0x%08x",DexF->indexMap);	
		DumpD("DexMap classMap:0x%08x",DexF->indexMap.classMap);	
		DumpD("DexMap classFullCount:0x%08x",DexF->indexMap.classFullCount);	
		DumpD("DexMap classReducedCount:0x%08x",DexF->indexMap.classReducedCount);	

		DumpD("DexMap methodMap:0x%08x",DexF->indexMap.methodMap);	
		DumpD("DexMap methodFullCount:0x%08x",DexF->indexMap.methodFullCount);	
		DumpD("DexMap methodReducedCount:0x%08x",DexF->indexMap.methodReducedCount);	

		DumpD("DexMap fieldMap:0x%08x",DexF->indexMap.fieldMap);	
		DumpD("DexMap fieldFullCount:0x%08x",DexF->indexMap.fieldFullCount);	
		DumpD("DexMap fieldReducedCount:0x%08x",DexF->indexMap.fieldReducedCount);	

		DumpD("DexMap stringMap:0x%08x",DexF->indexMap.stringMap);	
		DumpD("DexMap stringFullCount:0x%08x",DexF->indexMap.stringFullCount);	
		DumpD("DexMap stringReducedCount:0x%08x",DexF->indexMap.stringReducedCount);	
	
	DumpD("RegisterMapPool:0x%08x",DexF->pRegisterMapPool);
	DumpD("baseAddr:0x%08x",DexF->baseAddr);
	DumpD("overhead:0x%08x",DexF->overhead);
	DumpD("[2]Header");
	LOGHeader(DexF->pHeader);	
	DumpD("[3]Maps");
	DexMapList* maplist;
	maplist = (DexMapList*)((u4)(DexF->pHeader->mapOff)+(u4)(DexF->pHeader));
	LOGMAP(maplist);	
	DumpD("++++++++++++++++LOGDexFile+++++++++++++++++++");
}
/*
************************************************************
*			ReDexFile
* @param	inAddr -> 指向DexFile
* @cmt 
*		1.延时10S等待加固程序解密DexFile
*		2.对DexFile进行解析，判断是否是连续内存
*		3.分析ClassDef,查看是否存在错误
*		4.
*		3.保存文件
************************************************************
*/
void Dump_Repair(RepairMem* inRepair,char* inFile){
	FILE* mfd=fopen(inFile,"wb");
	//判断文件是否打开成功
	if(!mfd){
		DumpD("Dump_Repair@文件打开失败!");
		return;
	}
	while(inRepair != NULL){
		fwrite(inRepair->Addr,1,inRepair->Length,mfd);
		inRepair = inRepair->next;
	}
	//写原始数据
	fclose(mfd);
	return ;
}
/*
*/
void* ReDexFile(void* inAddr){
	DexFile* inDex = (DexFile*)inAddr;
	DumpD("Dump@延时15S!");
	sleep(15);
	DumpD("Dump@打印日志!");
	LOGDexFile(inDex);
	DumpD("Dump@获取原本Dex");
	if(memcmp(inDex->pHeader,"dex",3) != 0){
		DumpD("Dump@Save 数据出现问题直接退出!");
		return NULL;
	}
	//获取原本Dex数据函数
	str_ModMem* mMem = mem->newMem(getDumpLen(inDex->pHeader->fileSize));
	memcpy(mMem->Addr,(void*)inDex->pHeader,inDex->pHeader->fileSize);
	DumpD("Dump@修复数据 ClassDef");
	unsigned int DataMinoff = Repair_ClassDef(mem,inDex);
	DumpD("Dump@最小偏移 DataMinoff:0x%08x",DataMinoff);
	DumpD("Dump@更新 Header");
	writeHeader(mem);
	DumpD("Dump@更新 MAPS");
	writeMapClassDef(mem,inDex,DataMinoff);
	DumpD("Dump@Save!");
	mem->SaveFile("/sdcard/DumpDex/Mod_Mem.dump");
	return NULL;
}
/*
************************************************************
*				Dump_DexFile
* @param	inAddr -> 指向DexFile
* @cmt	创建一个线程，然后用线程去执行解码 DexFile
************************************************************
*/
void Dump_DexFile(void* inAddr){
	DumpD("Dump_DexFile@创建子线程!");
	mem = new Mod_Mem();
	pthread_t thread;  
	pthread_create(&thread,NULL,ReDexFile,inAddr);  
}

