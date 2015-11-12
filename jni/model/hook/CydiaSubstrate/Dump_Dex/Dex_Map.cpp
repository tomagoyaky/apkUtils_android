#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "HFile/NativeLog.h"
#include "Dump_Dex.H"
#include "Dex_Util.H" 
#include "Module_Mem.H"
//__________________________________________________________
/*
 * 日志打印LOGMAP，
*/
void LOGMAP(DexMapList* inmapList){
	DumpD("LOGMAP@addr:0x%08x\n",inmapList);
	DumpD("DEX Map Number of sections:0x%08x\n",inmapList->size);
	for(int m_i = 0;m_i < inmapList->size;m_i++){
		DexMapItem* item= (DexMapItem*)((u4)(&inmapList->list) + (u4)(sizeof(DexMapItem)*m_i));
		DumpD("LOGMAP@item[%d]",m_i);
		DumpD("LOGMAP@type:0x%08x",item->type);
		DumpD("LOGMAP@unused:0x%08x",item->unused);
		DumpD("LOGMAP@size:0x%08x",item->size);
		DumpD("LOGMAP@offset:0x%08x",item->offset);		
	}
}
//__________________________________________________________
//
void writeMapClassDef(Mod_Mem* in_Mem,DexFile* in_Dex,unsigned int in_Off){
	//修复ClassDef数据
	DexMapList* mapList = (DexMapList*)((u4)in_Dex->pHeader->mapOff+(u4)in_Dex->pHeader);
	for(int m_i = 0;m_i < mapList->size;m_i++){
		DexMapItem* item= (DexMapItem*)((u4)(&mapList->list) + (u4)(sizeof(DexMapItem)*m_i));
		//判断是否是Class Data
		if(item->type == 0x2001){
			unsigned int mOff = (unsigned int)((u4)&item->offset - (u4)&in_Dex->pHeader->magic[0]);
			*in_Mem->getOffsetAddr_INT(mOff) = in_Off;		
			DumpD("mOff:0x%08x,in_Off:0x%08x",mOff,in_Off);
		}
	}	
}