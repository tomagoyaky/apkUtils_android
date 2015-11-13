#ifndef __DEX_UTIL_H_
#define __DEX_UTIL_H_
//__________________________________________________________
//读取一个Leb128数据
extern u4 readLeb128(u1* inAddr);
//读取下一个Leb128数据地址
extern u1* readNEXTLeb128(u1* inAddr);
//读取Leb128数据宽度
extern u4 readLimitLeb128(u1* inAddr);
//
extern int writeAddLeb128(u1* inAddr,u4 Add_Off);
extern int writeLeb128(u1* inAddr,u4 inData);
//将长度4K对齐后加上4K
extern u4 getDumpLen(u4 inLen);
//
extern u4 getFileLength(RepairMem* inRep_Start);
#endif