#ifndef __DUMP_CLASSDEF_H_
#define __DUMP_CLASSDEF_H_
#include "Module_Mem.H"
 /* expanded form of a class_data_item header */
typedef struct DexClassDataHeader {
    u4 staticFieldsSize;
    u4 instanceFieldsSize;
    u4 directMethodsSize;
    u4 virtualMethodsSize;
} DexClassDataHeader;

/* expanded form of encoded_field */
typedef struct DexField {
    u4 fieldIdx;    /* index to a field_id_item */
    u4 accessFlags;
    struct DexField* next;
} DexField;

/* expanded form of encoded_method */
typedef struct DexMethod {
    u4 methodIdx;    /* index to a method_id_item */
    u4 accessFlags;
    u4 codeOff;      /* file offset to a code_item */\
    struct DexMethod* next;
} DexMethod;

/* expanded form of class_data_item. Note: If a particular item is
 * absent (e.g., no static fields), then the corresponding pointer
 * is set to NULL. */
typedef struct DexClassData {
    DexClassDataHeader header;
    DexField*          staticFields;
    DexField*          instanceFields;
    DexMethod*         directMethods;
    DexMethod*         virtualMethods;
} DexClassData;
//链表保存ClassDef 数据以及item数据
typedef struct LinkClassDef{
	struct DexClassDef* info;
	struct LinkClassDef* next;
	struct DexClassData* item;
}Link_ClassDef;
//














//----------------------------------------------------------
extern void LOG_ClassDef(const DexFile* in_Dex);
//
extern unsigned int Repair_ClassDef(Mod_Mem* inMem,DexFile* inDex);
//





#endif