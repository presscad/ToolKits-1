#ifndef __BINARY_FILE_IO_H_
#define __BINARY_FILE_IO_H_
class CFileBuffer
{
	BOOL m_bWrite;
	FILE *m_fp;
public:
	BOOL IsValidFile();
	CFileBuffer(FILE *fp,BOOL bWrite);
	CFileBuffer(const char *filename,BOOL bWrite);
	~CFileBuffer();
	BOOL CreateFile(const char *filename,BOOL bWrite);
	void Seek(long lMove,int iOrigin);
	long CurrentPosition(){return ftell(m_fp);}
	UINT Read(void *dest_ptr,UINT size);
	UINT Write(const void *src_ptr,UINT size);
	BOOL IsStoring();
	//read operation
	CFileBuffer & operator>>(char &ch);
	CFileBuffer & operator>>(WORD &w);
	CFileBuffer & operator>>(DWORD &dw);
	CFileBuffer & operator>>(UINT &u);
	CFileBuffer & operator>>(short &sn);
	CFileBuffer & operator>>(long &ln);
	CFileBuffer & operator>>(int &ln);
	CFileBuffer & operator>>(double &ff);
	//write operation
	CFileBuffer & operator<<(const char ch);
	CFileBuffer & operator<<(const WORD w);
	CFileBuffer & operator<<(const DWORD dw);
	CFileBuffer & operator<<(const UINT u);
	CFileBuffer & operator<<(const short sn);
	CFileBuffer & operator<<(const long ln);
	CFileBuffer & operator<<(const int ln);
	CFileBuffer & operator<<(const double ff);
	// return string length or -1 if UNICODE string is found in the archive
	UINT ReadStringLength();
	UINT ReadString(char *sReadString);
	// String format:
	//      UNICODE strings are always prefixed by 0xff, 0xfffe
	//      if < 0xff chars: len:BYTE, TCHAR chars
	//      if >= 0xff characters: 0xff, len:WORD, TCHAR chars
	//      if >= 0xfffe characters: 0xff, 0xffff, len:DWORD, TCHARs
	void WriteString(char *ss);
};
#endif