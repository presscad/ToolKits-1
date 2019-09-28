#pragma once
#include ".\Buffer.h"
#include ".\ArrayList.h"

class CTempFileBuffer : public BUFFER_IO
{
	long m_nInitBuffPoolSize;
	bool m_bInitBuffPool;	//是否已为临时文件分配好了缓存内存区
	bool InitBuffPool();
protected:
	FILE *m_fp;
	ULONG m_uFilePosition;
	char m_filename[201];
	ULONG m_uBufferLength;		//当前文件缓存大小
	bool m_bEnableAutoDelete;
	bool m_bExternalFile;
	DYN_ARRAY<char>BUFF_POOL;
	friend class CTempFilePositionStack;
public:	//父类继承函数
	virtual DWORD Read(void *pch, DWORD size);
	virtual DWORD Write(const void *pch,DWORD size);
	virtual bool SeekToBegin();
	virtual bool SeekToEnd();
	virtual bool SeekPosition(DWORD pos);
	virtual bool SeekOffset(DWORD offset);
	virtual DWORD GetCursorPosition(){return m_uFilePosition;}
	virtual DWORD GetLength(){return m_uBufferLength;}
	void ClearContents();	//只清空内容，不清除缓存，即只将file_len置0
public:
	CTempFileBuffer(FILE* fp=NULL);
	CTempFileBuffer(const char* fname);
	virtual ~CTempFileBuffer();
	bool CreateFile(const char *filename,int nBufSize=0);
	bool InitFile(FILE* fp);
	long GetInitBuffSize(){return m_nInitBuffPoolSize;}
	long GetActualBuffSize();	//当前文件实际缓存大小
	BOOL IsValidFile(){return m_fp!=NULL;}
	int GetFileName(char* filename);
	void CloseFile();
	bool EnableAutoDelete(bool enabled=true){return m_bEnableAutoDelete=enabled;}
	BOOL DeleteFile();	//删除临时文件
	bool FlushTmpFile();
};
