#include "stdio.h"
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include "Dump_Dex.H"
/*
************************************************************
*			readLeb128
* @param	inAddr -> 读取地址
* @return	result -> 读取的Leb128数据
************************************************************
*/
u4 readLeb128(u1* inAddr){
	u4 result = 0;
	for(int m_i = 0;m_i < 5;m_i++){
		u1 m_byte = *(inAddr + m_i);
		m_byte &= 0x7F;
		result |= m_byte<<(m_i*7);
		if(*(inAddr+m_i) < 0x7f){
			return result;
		}
	}
	return result;
}
/*
************************************************************
*			readNEXTLeb128
* @param	inAddr -> 读取地址
* @return	result ->	下一个Leb128数据地址
************************************************************
*/
u1* readNEXTLeb128(u1* inAddr){
	int m_i;
	for(m_i = 0;m_i < 5;m_i++){
		if(*(inAddr+m_i) < 0x7f){
			return inAddr+m_i+1;
		}
	}
	return inAddr+m_i+1;
}
/*
************************************************************
*			readLimitLeb128
* @param	inAddr -> 读取地址
* @return	result ->	读取这个数据宽度
************************************************************
*/
u4 readLimitLeb128(u1* inAddr){
	int m_i = 0;
	for(m_i = 0;m_i < 5;m_i++){
		if(*(inAddr+m_i) < 0x7f){
			return  m_i+1;
		}
	}
	return  m_i+1;
}
/*
************************************************************
*这个写Leb128受原先Leb128宽度限制，
*宽度出现问题时，写入数据可能会出错
*@param	inAddr -> 数据地址
*@param	Add_Off -> 增加数据量
************************************************************
*/
int writeAddLeb128(u1* inAddr,u4 Add_Off){
	u4 mOld = readLeb128(inAddr);
	u4 mLength = readLimitLeb128(inAddr);
	u4 writeData = mOld+Add_Off;
	for(int m_i = 0;m_i< mLength;m_i++){
		u1 mByte = (writeData>>(m_i*7))&0x7F;
		if((m_i+1)!= mLength){
			*(inAddr+m_i) = mByte|0x80;
		}else{
			*(inAddr+m_i) = mByte;
		}		
	}
	return 0;
}
int writeLeb128(u1* inAddr,u4 inData){
	u4 mLength = readLimitLeb128(inAddr);
	for(int m_i = 0;m_i< mLength;m_i++){
		u1 mByte = (inData>>(m_i*7))&0x7F;
		if((m_i+1)!= mLength){
			*(inAddr+m_i) = mByte|0x80;
		}else{
			*(inAddr+m_i) = mByte;
		}		
	}
	return 0;
}
/*
************************************************************
*			getDumpLen			
*1.将长度4K对齐后加上4K
************************************************************
*/
u4 getDumpLen(u4 inLen){
	u4 Length = (inLen&0xFFFFF000) + 0x1000;
	return Length;
}
/*
************************************************************
*获取文件长度
************************************************************
*/
u4 getFileLength(RepairMem* inRep_Start){
	u4 mLength = 0;
	while(inRep_Start != NULL){
		mLength += inRep_Start->Length;
		inRep_Start = inRep_Start->next;
	}
	return mLength;
}


