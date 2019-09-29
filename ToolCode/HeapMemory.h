#pragma once
#include "Buffer.h"
#include "f_ent_list.h"
class IHeapMemory{
protected:
	//0~3 Bytes��ʾMAT��Memory alloc table)���ĳ���M��,����M����ΪMAT��������Ϊջ�ڴ�������
	char* memory;
	static const BYTE XHHEAP_PACKSIZE=16;
	char m_cnMarIdBitsCount;	//������MAR.xMarInfo���ڴ�����¼Id��ռλ��
	char get_cnMarSizeBitsCount() { return 32-min(31,m_cnMarIdBitsCount); }
	char set_cnMarSizeBitsCount(char cnSizeBitsCount) { return m_cnMarIdBitsCount - min(cnSizeBitsCount,31); }
	//������MAR.xMarInfo���ڴ�����¼�����ڴ�����С��Ϣ��ռλ��
	__declspec(property(put=set_cnMarSizeBitsCount,get=get_cnMarSizeBitsCount)) char m_cnMarSizeBitsCount;
	struct MAR {	//Memory alloc record
		WORD  wAddrOfPackets;	//wAddrOfPackets*PACK_SIZEΪ����ʱ���ϵ�ʵ�����ƫ�Ƶ�ַ
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
