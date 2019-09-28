#include "stdafx.h"
#include "FileIO.h"

CFileBuffer::CFileBuffer(FILE *fp,BOOL bWrite)
{
	m_fp=fp;
	m_bWrite=bWrite;
}
CFileBuffer::CFileBuffer(const char *filename,BOOL bWrite)
{
	m_fp=NULL;
	m_bWrite=bWrite;
	if(bWrite)
		m_fp=fopen(filename,"wb");
	else
		m_fp=fopen(filename,"rb");
}
CFileBuffer::~CFileBuffer()
{
	if(m_fp)
		fclose(m_fp);
}
BOOL CFileBuffer::CreateFile(const char *filename,BOOL bWrite)
{
	if(m_fp)
		fclose(m_fp);
	if(bWrite)
		m_fp=fopen(filename,"wb");
	else
		m_fp=fopen(filename,"rb");
	if(m_fp)
		return TRUE;
	else
		return FALSE;
}

// return string length or -1 if UNICODE string is found in the archive
UINT CFileBuffer::ReadStringLength()
{
	DWORD nNewLen;

	// attempt BYTE length first
	BYTE bLen;
	fread(&bLen,1,1,m_fp);

	if (bLen < 0xff)
		return bLen;

	// attempt WORD length
	WORD wLen;
	fread(&wLen,2,1,m_fp);
	if (wLen == 0xfffe)
	{
		// UNICODE string prefix (length will follow)
		return (UINT)-1;
	}
	else if (wLen == 0xffff)
	{
		// read DWORD of length
		fread(&nNewLen,4,1,m_fp);
		return (UINT)nNewLen;
	}
	else
		return wLen;
}
UINT CFileBuffer::ReadString(char *sReadString)
{
	UINT len=ReadStringLength();
	if(sReadString==NULL)
		return len;
	else
		Read(sReadString,len);
	sReadString[len]='\0';
	return len;
}
// String format:
//      UNICODE strings are always prefixed by 0xff, 0xfffe
//      if < 0xff chars: len:BYTE, TCHAR chars
//      if >= 0xff characters: 0xff, len:WORD, TCHAR chars
//      if >= 0xfffe characters: 0xff, 0xffff, len:DWORD, TCHARs
void CFileBuffer::WriteString(char *ss)
{
	BYTE b;
	WORD w;
	DWORD dw,str_len;
	// special signature to recognize unicode strings
#ifdef _UNICODE
	b=0xff;
	w=0xfffe;
	fwrite(&b,1,1,m_fp);
	fwrite(&w,2,1,m_fp);
#endif
	if(ss)
		str_len=strlen(ss);
	else
		str_len=0;

	if (strlen(ss)< 255)
	{
		b=(BYTE)strlen(ss);
		fwrite(&b,1,1,m_fp);
	}
	else if (strlen(ss) < 0xfffe)
	{
		b=0xff;
		w=(WORD)strlen(ss);
		fwrite(&b,1,1,m_fp);
		fwrite(&w,2,1,m_fp);
	}
	else
	{
		b=0xff;
		w=0xffff;
		dw=strlen(ss);
		fwrite(&b,1,1,m_fp);
		fwrite(&w,2,1,m_fp);
		fwrite(&dw,4,1,m_fp);
	}
	if(str_len>0)
		fwrite(ss,str_len,1,m_fp);
}
void CFileBuffer::Seek(long lMove,int iOrigin)
{
	fseek(m_fp,lMove,iOrigin);
}
UINT CFileBuffer::Read(void *dest_ptr,UINT size)
{
	return fread(dest_ptr,size,1,m_fp);
}
UINT CFileBuffer::Write(const void *src_ptr,UINT size)
{
	return fwrite(src_ptr,size,1,m_fp);
}
BOOL CFileBuffer::IsStoring()
{
	return m_bWrite;
}
//read operation
CFileBuffer & CFileBuffer::operator>>(char &ch)
{
	if(!m_bWrite)
		Read(&ch,1);
	return *this;
}
CFileBuffer & CFileBuffer::operator>>(WORD &w)
{
	if(!m_bWrite)
		Read(&w,sizeof(WORD));
	return *this;
}
CFileBuffer & CFileBuffer::operator>>(DWORD &dw)
{
	if(!m_bWrite)
		Read(&dw,sizeof(DWORD));
	return *this;
}
CFileBuffer & CFileBuffer::operator>>(UINT &u)
{
	if(!m_bWrite)
		Read(&u,sizeof(UINT));
	return *this;
}
CFileBuffer & CFileBuffer::operator>>(short &sn)
{
	if(!m_bWrite)
		Read(&sn,sizeof(short));
	return *this;
}
CFileBuffer & CFileBuffer::operator>>(int &ln)
{
	if(!m_bWrite)
		Read(&ln,sizeof(int));
	return *this;
}
CFileBuffer & CFileBuffer::operator>>(long &ln)
{
	if(!m_bWrite)
		Read(&ln,sizeof(long));
	return *this;
}
CFileBuffer & CFileBuffer::operator>>(double &ff)
{
	if(!m_bWrite)
		Read(&ff,sizeof(double));
	return *this;
}
//write operation
CFileBuffer & CFileBuffer::operator<<(const char ch)
{
	if(m_bWrite)
		Write(&ch,1);
	return *this;
}
CFileBuffer & CFileBuffer::operator<<(const WORD w)
{
	if(m_bWrite)
		Write(&w,sizeof(WORD));
	return *this;
}
CFileBuffer & CFileBuffer::operator<<(const DWORD dw)
{
	if(m_bWrite)
		Write(&dw,sizeof(DWORD));
	return *this;
}
CFileBuffer & CFileBuffer::operator<<(const UINT u)
{
	if(m_bWrite)
		Write(&u,sizeof(UINT));
	return *this;
}
CFileBuffer & CFileBuffer::operator<<(const short sn)
{
	if(m_bWrite)
		Write(&sn,sizeof(short));
	return *this;
}
CFileBuffer & CFileBuffer::operator<<(const int ln)
{
	if(m_bWrite)
		Write(&ln,sizeof(int));
	return *this;
}
CFileBuffer & CFileBuffer::operator<<(const long ln)
{
	if(m_bWrite)
		Write(&ln,sizeof(long));
	return *this;
}
CFileBuffer & CFileBuffer::operator<<(const double ff)
{
	if(m_bWrite)
		Write(&ff,sizeof(double));
	return *this;
}

BOOL CFileBuffer::IsValidFile()
{
	if(m_fp)
		return TRUE;
	else
		return FALSE;
}
