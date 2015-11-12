#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "HFile/NativeLog.h"
#include "Dump_Dex.H"
#include "Dex_Util.H" 
#include "Module_Mem.H"
//__________________________________________________________
/*
 * 日志打印DexHeader,
 *	Dex文件头主要包括校验和以及其他结构的偏移地址和长度信息。
 */
void LOGHeader(const DexHeader* pHeader)
{
	DumpD("DEX file header: addr:0x%08x\n",pHeader);
	DumpD("magic               : '%.3s'\n", pHeader->magic);
	DumpD("checksum            : %08x\n", pHeader->checksum);
	DumpD("signature           : %02x%02x...%02x%02x\n",
		pHeader->signature[0], pHeader->signature[1],
		pHeader->signature[kSHA1DigestLen-2],
		pHeader->signature[kSHA1DigestLen-1]);
	DumpD("file_size           : %d\n", pHeader->fileSize);
	DumpD("header_size         : %d\n", pHeader->headerSize);
	DumpD("link_size           : %d\n", pHeader->linkSize);
	DumpD("link_off            : %d (0x%08x)\n",
		pHeader->linkOff, pHeader->linkOff);
	DumpD("mapOff              : %d (0x%08x)\n",
		pHeader->mapOff,  pHeader->mapOff);	
	DumpD("string_ids_size     : %d\n", pHeader->stringIdsSize);
	DumpD("string_ids_off      : %d (0x%08x)\n",
		pHeader->stringIdsOff, pHeader->stringIdsOff);
	DumpD("type_ids_size       : %d\n", pHeader->typeIdsSize);
	DumpD("type_ids_off        : %d (0x%08x)\n",
		pHeader->typeIdsOff, pHeader->typeIdsOff);
	DumpD("field_ids_size      : %d\n", pHeader->fieldIdsSize);
	DumpD("field_ids_off       : %d (0x%08x)\n",
		pHeader->fieldIdsOff, pHeader->fieldIdsOff);
	DumpD("method_ids_size     : %d\n", pHeader->methodIdsSize);
	DumpD("method_ids_off      : %d (0x%08x)\n",
		pHeader->methodIdsOff, pHeader->methodIdsOff);
	DumpD("class_defs_size     : %d\n", pHeader->classDefsSize);
	DumpD("class_defs_off      : %d (0x%08x)\n",
		pHeader->classDefsOff, pHeader->classDefsOff);
	DumpD("data_size           : %d\n", pHeader->dataSize);
	DumpD("data_off            : %d (0x%08x)\n",
		pHeader->dataOff, pHeader->dataOff);
}
//
void writeHeader(Mod_Mem* inMem)
{
	//设置文件和Code大小
	*inMem->getOffsetAddr_INT(0x20) = inMem->getMemLength();
	*inMem->getOffsetAddr_INT(0x68) = inMem->getMemLength() - inMem->getOffsetData_INT(0x6C);
}
//


//__________________________________________________________
