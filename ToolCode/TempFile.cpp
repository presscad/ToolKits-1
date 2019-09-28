#include "stdafx.h"
#include <stdio.h>
#include ".\Tempfile.h"

CTempFileBuffer::CTempFileBuffer(FILE* fp/*=NULL*/)
{
	m_fp=fp;
	m_uBufferLength=m_uFilePosition=0;
	m_nInitBuffPoolSize=0;
	memset(m_filename,0,201);
	if(fp!=NULL)
		m_bExternalFile=true;
	else
		m_bExternalFile=false;
	m_bInitBuffPool=false;
	m_bEnableAutoDelete=!m_bExternalFile;
}
CTempFileBuffer::CTempFileBuffer(const char* fname)
{
	m_fp=NULL;
	m_uBufferLength=m_uFilePosition=0;
	memset(m_filename,0,201);
	m_bExternalFile=false;
	m_bEnableAutoDelete=true;
	m_bInitBuffPool=false;
	m_nInitBuffPoolSize=0;
	CreateFile(fname);
}

CTempFileBuffer::~CTempFileBuffer(){CloseFile();}
void CTempFileBuffer::CloseFile()
{
	if(m_bEnableAutoDelete&&!m_bExternalFile&&m_fp!=NULL)
	{
		fclose(m_fp);
		DeleteFile();
		m_fp=NULL;
	}
}
bool CTempFileBuffer::InitFile(FILE* fp)
{
	CloseFile();
	m_fp=fp;
	m_uBufferLength=m_uFilePosition=0;
	if(fp!=NULL)
		m_bExternalFile=true;
	else
		m_bExternalFile=false;
	return true;
}
bool CTempFileBuffer::InitBuffPool()
{
	if(m_bInitBuffPool)
		return true;
	long nBufSize=0;
	do{
		nBufSize=nBufSize==0?m_nInitBuffPoolSize:nBufSize/2;
		m_bInitBuffPool=BUFF_POOL.Resize(nBufSize);
	}while(!m_bInitBuffPool&&nBufSize>0xA00000);
	if(m_bInitBuffPool&&nBufSize>0)
		setvbuf(m_fp,BUFF_POOL,_IOFBF,BUFF_POOL.Size());
	return m_bInitBuffPool;
}
long CTempFileBuffer::GetActualBuffSize()	//当前文件实际缓存大小
{
	return (long)BUFF_POOL.Size();
}
bool CTempFileBuffer::CreateFile(const char *filename,int nBufSize/*=0*/)
{
	if(m_bExternalFile)
	{	//外部文件
		m_fp=NULL;
		m_bExternalFile=false;
	}
	if(m_fp)
		fclose(m_fp);
	if(_stricmp(m_filename,filename)!=0)
		DeleteFile();
	strncpy(m_filename,filename,200);
	int hits=0;
	do{
		if(hits>0)
			strcat(m_filename,"~");
		m_fp=fopen(m_filename,"w+b");	//打开一个空文件用于读写
		hits++;
	}while(m_fp==NULL&&hits<5);
	if(nBufSize>0)	//设定临时文件缓存大小
		m_nInitBuffPoolSize=nBufSize;
	/* 为节省内存在初次进行读写操作时再设定缓存 wjh-2016.12.22
	{
		BUFF_POOL.Resize(nBufSize);
		setvbuf(m_fp,BUFF_POOL,_IOFBF,nBufSize);
		m_bInitBuffPool=true;
	}*/
	m_uBufferLength=m_uFilePosition=0;
	if(m_fp)
		return true;
	else
		return false;
}
int CTempFileBuffer::GetFileName(char* filename)
{
	if(filename)
		strcpy(filename,m_filename);
	return strlen(m_filename);
}
bool CTempFileBuffer::FlushTmpFile()
{
	if(m_fp!=NULL)
		return fflush(m_fp)==0;
	else
		return false;
}

static char zeropool[256]={0};
DWORD CTempFileBuffer::Write(const void* pch,DWORD size)
{
	if(m_fp==NULL)
		return 0;
	InitBuffPool();
	ULONG write_bytes=size;
	if(pch==NULL)	//写入零数据
	{
		int count=size>>8;
		int odd=size&0x000000ff;
		for(int i=0;i<count;i++)
			fwrite(zeropool,1,256,m_fp);
		if(odd>0)
			fwrite(zeropool,1,odd,m_fp);
	}
	else
		write_bytes=fwrite(pch,1,size,m_fp);
	m_uFilePosition+=write_bytes;
	if(m_uFilePosition>m_uBufferLength)
		m_uBufferLength=m_uFilePosition;
	return write_bytes;
}
DWORD CTempFileBuffer::Read(void* pch,DWORD size)
{
	if(m_fp==NULL)
		return 0;
	InitBuffPool();
	ULONG read_bytes=fread(pch,1,size,m_fp);
	m_uFilePosition+=read_bytes;
	return read_bytes;
}
BOOL CTempFileBuffer::DeleteFile()	//删除临时文件
{
	return ::DeleteFile(m_filename);
}
void CTempFileBuffer::ClearContents()	//只清空内容，不清除缓存，即只将file_len置0
{
	if(m_fp==NULL)
		return;
	fseek(m_fp,0,SEEK_SET);
	char final='\0';
	fwrite(&final,1,1,m_fp);
	fflush(m_fp);
	fseek(m_fp,0,SEEK_SET);
	m_uFilePosition=0;
	m_uBufferLength=0;
}
bool CTempFileBuffer::SeekToBegin()
{
	if(m_fp!=NULL&&fseek(m_fp,0,SEEK_SET)==0)	//执行成功
	{
		m_uFilePosition=0;
		return true;
	}
	else
		return false;
}
bool CTempFileBuffer::SeekToEnd()
{
	if(m_fp!=NULL)
	{
		fseek(m_fp,0,SEEK_END);	//执行成功
		m_uFilePosition=ftell(m_fp);
	}
	else
		return false;
	return true;
}
bool CTempFileBuffer::SeekPosition(DWORD pos)
{
	if(m_fp!=NULL&&fseek(m_fp,pos,SEEK_SET)==0)	//执行成功
	{
		m_uFilePosition=pos;
		return true;
	}
	else
		return false;
}
bool CTempFileBuffer::SeekOffset(DWORD offset)
{
	if(m_fp!=NULL&&fseek(m_fp,offset,SEEK_CUR)==0)	//执行成功
	{
		m_uFilePosition+=offset;
		return true;
	}
	else
		return false;
}
