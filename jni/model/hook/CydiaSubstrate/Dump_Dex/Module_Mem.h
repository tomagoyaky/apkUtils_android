#ifndef __Module_Mem_H_
#define __Module_Mem_H_
#include "stdio.h"
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
//__________________________________________________________
typedef struct str_ModMem{
	unsigned char*	Addr;
	unsigned char*	LoadBase;
	unsigned int	Length;
	unsigned int	Offset;
	unsigned int	Base;
	struct str_ModMem* next;
}str_ModMem;
//__________________________________________________________
class Mod_Mem{
public:
str_ModMem* ModMem = NULL;
/*
*	获取模块分区内存大小
*/
	 unsigned int getMemLength()
	 {
		unsigned int mMemLength = 0;
		str_ModMem* mMod = ModMem;
		while(mMod != NULL){
			mMemLength += mMod->Length;
			mMod = mMod->next;
		}
		return mMemLength;
	}
/*
*获取最后一个内存模块
*/
	str_ModMem* Last()
	{
		str_ModMem* mMod = ModMem;
		if(mMod == NULL)return NULL;

		while(mMod->next != NULL){
			mMod = mMod->next;
		}
		return mMod;
	}
/*
*	添加一块新内存到整个模块中
*/	
	str_ModMem* newMem(size_t in_Length)
	{	
		str_ModMem* mMem = Last();
		if(mMem == NULL){//没内存模块
			ModMem = (str_ModMem*)malloc(sizeof(str_ModMem)+1);
			memset(ModMem,0,sizeof(str_ModMem)+1);
			ModMem->Addr = (unsigned char*)malloc(in_Length);
			memset(ModMem->Addr,0,in_Length);
			ModMem->Length = in_Length;
			return ModMem;
		} 
		mMem->next = (str_ModMem*)malloc(sizeof(str_ModMem)+1);
		mMem = mMem->next;
		memset(mMem,0,sizeof(str_ModMem)+1);
		mMem->Addr = (unsigned char*)malloc(in_Length);
		memset(mMem->Addr,0,in_Length);
		mMem->Length = in_Length;
		return mMem;
	}
/*
*根据偏移获取具体地址
*/
	unsigned char* getOffsetAddr(size_t in_Off){
		str_ModMem* mMod = ModMem;
		unsigned int mMemLength = 0;
		while(mMod != NULL){
			if(in_Off < (mMemLength + mMod->Length)){
				return mMod->Addr+in_Off-mMemLength;				
			}
			mMemLength += mMod->Length;
			mMod = mMod->next;
		}
		return NULL;
	}
/*
*根据偏移获取具体地址
*/
	unsigned int* getOffsetAddr_INT(size_t in_Off){
		str_ModMem* mMod = ModMem;
		unsigned int mMemLength = 0;
		while(mMod != NULL){
			if(in_Off < (mMemLength + mMod->Length)){
				return (unsigned int*)(mMod->Addr+in_Off-mMemLength);				
			}
			mMemLength += mMod->Length;
			mMod = mMod->next;
		}
		return NULL;
	}
/*
*获取数据
*/
	unsigned int getOffsetData_INT(size_t in_Off){
		str_ModMem* mMod = ModMem;
		unsigned int mMemLength = 0;
		while(mMod != NULL){
			if(in_Off < (mMemLength + mMod->Length)){
				unsigned int* m_data;
				m_data = (unsigned int*)((unsigned int)mMod->Addr+in_Off-mMemLength);
				return *m_data;				
			}
			mMemLength += mMod->Length;
			mMod = mMod->next;
		}
		return NULL;
	}
/*
*保存成文件
*/
	void SaveFile(char* inFile){
		FILE* mfd=fopen(inFile,"wb");
		//判断文件是否打开成功
		if(!mfd){
			return;
		}
		str_ModMem* mMod = ModMem;
		while(mMod != NULL){
			fwrite(mMod->Addr,1,mMod->Length,mfd);
			mMod = mMod->next;
		}
		//写原始数据
		fclose(mfd);
	}
};
//
#endif