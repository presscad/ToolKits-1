#pragma once
#include "Buffer.h"
#include "f_ent_list.h"
class CStackMemory{
protected:
	//0~3 Bytes表示MAT（Memory alloc table)区的长度M，,后续M长度为MAT，再往后为栈内存数据区
	char* memory;
public:
	char* AllocMemoryBlock(int nAllocSize) { return NULL; }
	bool  FreeMemoryBlock(void* pMemAddr, int nMemSize) { return false; }
	bool  CheckMemoryInStack(void* pMemAddr, int nMemSize) { return false; }
};
