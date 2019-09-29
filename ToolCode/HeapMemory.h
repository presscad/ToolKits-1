#pragma once
#include "Buffer.h"
#include "f_ent_list.h"
class IHeapMemory{
protected:
	//0~3 Bytes表示MAT（Memory alloc table)区的长度M，,后续M长度为MAT，再往后为栈内存数据区
	char* memory;
	static const BYTE XHHEAP_PACKSIZE=16;
	char m_cnMarIdBitsCount;	//下文中MAR.xMarInfo中内存分配记录Id所占位数
	char get_cnMarSizeBitsCount() { return 32-min(31,m_cnMarIdBitsCount); }
	char set_cnMarSizeBitsCount(char cnSizeBitsCount) { return m_cnMarIdBitsCount - min(cnSizeBitsCount,31); }
	//下文中MAR.xMarInfo中内存分配记录描述内存分配大小信息所占位数
	__declspec(property(put=set_cnMarSizeBitsCount,get=get_cnMarSizeBitsCount)) char m_cnMarSizeBitsCount;
	struct MAR {	//Memory alloc record
		WORD  wAddrOfPackets;	//wAddrOfPackets*PACK_SIZE为在临时堆上的实际相对偏移地址
		DWORD xMarInfo;
		UINT get_hHeapMemory();
		__declspec(property(get=get_hHeapMemory)) UINT hHeapMemory;
		UINT get_uiMemoryAllocSize();
		__declspec(property(get=get_uiMemoryAllocSize)) UINT uiMemoryAllocSize;
	};
public:
	char* AllocMemoryBlock(int nAllocSize) { return NULL; }
	bool  FreeMemoryBlock(void* pMemAddr, int nMemSize) { return false; }
	bool  CheckMemoryInStack(void* pMemAddr, int nMemSize) { return false; }
};
