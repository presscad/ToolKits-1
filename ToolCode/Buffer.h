// Buffer.h: interface for the CBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFFER_H__10945DBC_F91B_433A_97DD_482C7BC01747__INCLUDED_)
#define AFX_BUFFER_H__10945DBC_F91B_433A_97DD_482C7BC01747__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <stdio.h> //����FILE����

struct IBufferStack;
struct BUFFER_IO{
public:	//����������������Ϊ��ʵ��property,property�в���ֱ��ָ���麯��
	DWORD InternalCursorPosition(){return GetCursorPosition();}
	DWORD InternalLength(){return GetLength();}
public:
	virtual DWORD Read(void *pch, DWORD size)=0;
	virtual DWORD Write(const void *pch,DWORD size)=0;

	virtual DWORD GetCursorPosition()=0;
	__declspec( property(get=InternalCursorPosition)) DWORD CursorPosition;//��ȡ���浱ǰ��ȡλ��

	virtual DWORD GetLength()=0;
	__declspec( property(get=InternalLength)) DWORD Length;//��ȡ�Ѷ�(д)�������Ч����
	virtual DWORD ReadBuffer(BUFFER_IO *buffer_io,DWORD size,char* pool=NULL,DWORD pool_size=0);
	virtual DWORD WriteBuffer(BUFFER_IO *buffer_io,DWORD size,char* pool=NULL,DWORD pool_size=0);
	virtual bool SeekPosition(DWORD pos)=0;
	virtual bool SeekOffset(DWORD offset){return false;}
	virtual bool SeekToBegin()=0;
	virtual bool SeekToEnd()=0;
	//���浱ǰλ��ѹջ��ջ����
	virtual IBufferStack* BufferStack(){return NULL;}
	virtual bool AttachStack(IBufferStack* pPosStack){return false;}
	virtual void DetachStack(){;}
	virtual int PushPositionStack();				//����ǰ����λ��ѹջ
	virtual bool PopPositionStack(int push_pos=-1);	//����������Ե�PushStackʱ����λ��
	virtual bool RestoreNowPosition();		//�ָ�PopStack֮ǰ�Ļ���λ��
	virtual bool ClearLevelCount(int iLevel);
	virtual bool IncrementLevelCount(int iLevel);
	virtual long LevelCount(int iLevel);
	//�����������͵Ŀ�����д
	virtual DWORD ReadByte(BYTE *byte){return Read(byte,1);}
	virtual DWORD ReadByte(char *ch){return Read(ch,1);}
	virtual BYTE ReadByte(){
		BYTE cbValue=0;
		Read(&cbValue,1);
		return cbValue;
	}
	virtual void ReadBoolean(bool *b);	//Ϊ������Ĵ���ȫ�ֱ��룬ʵ�ִ�������Buffer.cpp wjh-2014.5.15
	virtual void ReadBooleanThin(bool *b){Read(b,1);}
	virtual void ReadInteger(int *ii){Read(ii,4);}
	virtual void ReadInteger(long *ii){Read(ii,4);}
	virtual void ReadInteger(UINT *u){Read(u,4);}
	virtual void ReadInteger(DWORD *u){Read(u,4);}
	virtual long ReadInteger(){
		long liValue=0;
		Read(&liValue,4);
		return liValue;
	}
	virtual void ReadWord(WORD *w){Read(w,2);}
	virtual void ReadWord(short *w){Read(w,2);}
	virtual WORD ReadWord(){
		WORD wiValue=0;
		Read(&wiValue,2);
		return wiValue;
	}
	virtual void ReadDword(DWORD *dw){Read(dw,4);}
	virtual DWORD ReadDword(){
		DWORD dwValue=0;
		Read(&dwValue,4);
		return dwValue;
	}
	virtual void ReadFloat(float *f){Read(f,4);}
	virtual void ReadFloat(double *ff){float f=0; Read(&f,4); *ff=f;}
	virtual void ReadDouble(double *d){Read(d,8);}
	virtual double ReadDouble(){
		double dfValue=0;
		Read(&dfValue,8);
		return dfValue;
	}
	virtual void ReadPoint(double *pt_arr){Read(pt_arr,24);}	//��ά˫���ȸ�������
	virtual void ReadThinPoint(double *pt_arr);		//��ά�����ȸ�������
	virtual void WriteByte(BYTE byte){Write(&byte,1);}
	virtual void WriteBoolean(bool b);	//Ϊ������Ĵ���ȫ�ֱ��룬ʵ�ִ�������Buffer.cpp wjh-2014.5.15
	virtual void WriteBooleanThin(bool b){Write(&b,1);}
	virtual void WriteInteger(long ii){Write(&ii,4);}
	virtual void WriteWord(WORD w){Write(&w,2);}
	virtual void WriteWord(short w){Write(&w,2);}
	virtual void WriteDword(DWORD dw){Write(&dw,4);}
	virtual void WriteFloat(float f){Write(&f,4);}
	virtual void WriteFloat(double ff){float f=(float)ff; Write(&f,4);}
	virtual void WriteDouble(double d){Write(&d,8);}
	virtual void WritePoint(double *pt_arr){Write(pt_arr,24);}	//��ά˫���ȸ�������
	virtual void WriteThinPoint(double *pt_arr);		//��ά�����ȸ�������
	// return string length or -1 if UNICODE string is found in the archive
	UINT ReadStringLength();
	UINT ReadString(char *sReadString,UINT maxBufLength=0);
	// String format:
	//      UNICODE strings are always prefixed by 0xff, 0xfffe
	//      if < 0xff chars: len:BYTE, TCHAR chars
	//      if >= 0xff characters: 0xff, len:WORD, TCHAR chars
	//      if >= 0xfffe characters: 0xff, 0xffff, len:DWORD, TCHARs
	//maxWriteLength��ֹд������ַ������½�����ReadString�趨����ȡ�����ٴ򲻿��ļ�
	//maxWriteLength��0ʱ��ʾ���������д�볤�� wjh-2013.11.08
	void WriteString(const char *ss,UINT maxWriteLength=0);
};
typedef struct IBufferStack
{
	virtual void AttachBuffer(BUFFER_IO* pBuffIO)=0;
	virtual int PushPosition()=0;
	virtual bool PopPosition(int pos=-1)=0;
	virtual bool RestoreNowPosition()=0;
	virtual int GetStackRemnantSize()=0;
	virtual bool ClearLevelCount(int iLevel)=0;
	virtual bool IncrementLevelCount(int iLevel)=0;
	virtual long LevelCount(int iLevel)=0;
}*IBufferStackPtr;
class BUFFERPOP{
	UINT count,m_nPlanningCount;
	DWORD logPosition,currPosition;
	BUFFER_IO* m_pIO;
public:
	BUFFERPOP(BUFFER_IO* pIO,UINT nHits){
		Initialize(pIO,nHits);
	}
	void Initialize(BUFFER_IO* pIO,UINT nHits){
		count=0;
		m_nPlanningCount=nHits;
		m_pIO=pIO;
		currPosition=logPosition=pIO->GetCursorPosition();
	}
	void Increment(){count++;}
	UINT Count(){return count;}
	bool VerifyAndRestore(bool bUpdate=true,BYTE cDataTypeLen=4)
	{
		if(m_nPlanningCount==count)
			return true;
		currPosition=m_pIO->GetCursorPosition();
		if(bUpdate)
		{
			m_pIO->SeekPosition(logPosition);
			cDataTypeLen=min(cDataTypeLen,4);
			m_pIO->Write(&count,cDataTypeLen);
			m_pIO->SeekPosition(currPosition);
		}
		return false;
	}
};
struct BUFF_POSITION_ITEM{
	long lPushPosition;	//ѹջʱ����λ��
	long lNowPosition;	//��ջǰ���浱ǰλ��
	int level;
	long counts;
	BUFF_POSITION_ITEM(long push=0,long popNow=0){
		lPushPosition=push;
		lNowPosition=popNow;
		level=counts=0;
	};
};
class STD_BUFFER : public BUFFER_IO{
	DWORD cursor,buf_size;
	BYTE* data;
	STD_BUFFER(void* buff,DWORD size)
	{
		cursor=0;
		buf_size=size;
		data=(BYTE*)buff;
	}
	virtual bool SeekPosition(DWORD pos)
	{
		if(pos<0||pos>=buf_size)
			return false;
		else
			cursor=pos;
		return true;
	}
	virtual DWORD Read(void* pch,DWORD size)
	{
		DWORD dwRead=min(size,buf_size-cursor);
		memcpy(pch,data+cursor,dwRead);
		cursor+=dwRead;
		return dwRead;
	}
	virtual DWORD Write(const void* pch,DWORD size)
	{
		DWORD dwWrite=min(size,buf_size-cursor);
		memcpy(data+cursor,pch,dwWrite);
		cursor+=dwWrite;
		return dwWrite;
	}
};

class CBuffer : public BUFFER_IO
{
protected:
	bool m_bExternalBuf;
	DWORD file_len,buffer_len; 
	DWORD mem_cursor;		//��ǰָ��λ��
	DWORD log_mem_position;	//���ָ��λ��
	BUFF_POSITION_ITEM temporary;	//���ָ��λ��
	char *buffer;
	DWORD m_nBlockSize;
	bool m_bExternalPosStack;
	IBufferStack* m_pPosStack;
	//��������ļ���д���Լ��������壬ר���ڽ��д����ļ�ʱ�ڴ����ʧ����� wjh-2019.8.15
	FILE* m_fpOverflowBuffFile;		//FILE*���ͣ�ֻ��Ϊ�˼���ͷ�ļ����������Դ˴�����Ϊvoid*
	char* _OverFlowBuffPool;		//��������ļ���дʱ���ļ�����
	DWORD m_dwOverFlowBuffPoolSize;	//_OverFlowBuffPool��С
	DWORD m_dwMaxAllocMemBuffSize;	//ֱ�Ӵӳ�����Ϸ����ڴ�Ĵ�С��������ֵ��ָ���������ļ�ʱ��ȡ(��д��)����ļ�
	DWORD m_dwOverflowBuffFileLength;
	//ʵ��֤��ftell�������ļ���д�ٶ�Ӱ��޴��б�Ҫ�ڴ��¼��ǰ�α�λ�� wjh-2019.8.15
	long  m_liOverflowFileCurrPos;	//����ļ���ǰ�Ķ�дλ��
	DWORD ReadFromFileAt(DWORD posBeginFromFile,void *pch,DWORD size);
	DWORD WriteToFileAt(DWORD posBeginFromFile,const void *pch,DWORD size);
public:
	static IBufferStackPtr (*CreateBufferStackFunc)(BUFFER_IO* pBuffIO);	//ֻ�Ǵ���IBufferStack���͵Ļ���λ��ջ����Ҫ�û�����delete
public:
	virtual DWORD GetLength();
	DWORD SetBlockSize(DWORD nBlockSize = 1024 );
	bool  InitOverflowBuffFile(FILE* fp,DWORD dwMaxAllocMemBuffSize=0x40000000,char* buff_pool=NULL,UINT buff_pool_size=0);	//Ĭ�ϴӳ����һ���Է����ڴ����Ϊ1G
	UINT  DetachOverflowBuffFile();	//���ص�ǰ��������ļ���С
	DWORD GetOverflowBuffFileLength();
	virtual void ClearBuffer();		//������棬�ͷ��ڴ棬����Ҫʱ��Ҫ���·���
	void ClearContents();	//ֻ������ݣ���������棬��ֻ��file_len��0
	char* GetBufferPtr();
	virtual bool SeekToBegin();
	virtual bool SeekToEnd();
	DWORD Offset(int offset);//�Ե�ǰλ����λoffset���ֽ�
	DWORD LogPosition();		//���ָ��λ��
	char* GetCursorBuffer();////���ص�ǰָ��ָ�򻺴�
	virtual DWORD GetCursorPosition();//���ص�ǰָ��λ��
	DWORD RecallPosition();	//ѭ���ٻ�ָ��λ��
	UINT GetRemnantSize();
	virtual DWORD Read(void *pch,DWORD size);
	virtual DWORD Write(const void *pch,DWORD size);
	DWORD ReadAt(DWORD pos,void *pch,DWORD size,bool moveCursorPosition=false);
	void WriteAt(DWORD pos,void *pch,DWORD size,bool moveCursorPosition=true);
	virtual bool SeekPosition(DWORD pos);
	virtual bool SeekOffset(DWORD offset){Offset(offset);return true;}
	virtual IBufferStack* BufferStack(){return m_pPosStack;}
	virtual bool AttachStack(IBufferStack* pPosStack);
	virtual void DetachStack();
	virtual int PushPositionStack();				//����ǰ����λ��ѹջ
	virtual bool PopPositionStack(int push_pos=-1);	//����������Ե�PushStackʱ����λ��
	virtual bool RestoreNowPosition();		//�ָ�PopStack֮ǰ�Ļ���λ��
	virtual bool ClearLevelCount(int iLevel);
	virtual bool IncrementLevelCount(int iLevel);
	virtual long LevelCount(int iLevel);
	//����һ����Դ���浱ǰλ�ö�ȡָ�������ֽ���,ͬʱ�������Դ���������λ����
	DWORD Write(CBuffer& buffer,DWORD size);
	DWORD ReadByte(BYTE *byte);
	DWORD ReadByte(char *ch);
	//virtual void ReadBoolean(bool *b);
	void ReadBooleanThin(bool *b);
	void ReadInteger(int *ii);
	void ReadInteger(long *ii);
	void ReadInteger(UINT *u);
	void ReadInteger(DWORD *u);
	long ReadInteger();
	DWORD ReadDword();
	void ReadWord(WORD *w);
	void ReadWord(short *w);
	void ReadDword(DWORD *dw);
	//virtual void ReadFloat(float *f);
	double ReadDouble();
	void ReadDouble(double *d);
	//void ReadPoint(double *pt_arr);	//��ά˫���ȸ�������
	void WriteByte(BYTE byte);
	//virtual void WriteBoolean(bool b);
	void WriteBooleanThin(bool b);
	void WriteInteger(long ii);
	void WriteWord(WORD w);
	void WriteWord(short w);
	void WriteDword(DWORD dw);
	//void WriteFloat(float f);
	void WriteDouble(double d);
	//void WritePoint(double *pt_arr);	//��ά˫���ȸ�������
	BOOL ReadFromPipe(HANDLE hPipeRead,DWORD pack_size=1024);
	BOOL WriteToPipe(HANDLE hPipeWrite,DWORD pack_size=1024);
	UINT ReadStringFromUTF8(char *sReadString,UINT maxLength=0);
	void WriteStringToUTF8(const char *ss,UINT maxWriteLength=0);
	void operator<<(bool &var);
	void operator>>(bool &var);
	void operator<<(WORD &var);
	void operator>>(WORD &var);
	void operator<<(DWORD &var);
	void operator>>(DWORD &var);
	void operator<<(short &var);
	void operator>>(short &var);
	void operator<<(int &var);
	void operator>>(int &var);
	void operator<<(long &var);
	void operator>>(long &var);
	void operator<<(double &var);
	void operator>>(double &var);
	bool AttachMemory(char* srcBuf, DWORD buf_size);
	void DetachMemory();
	CBuffer(int nBlockSize = 1024 );
	CBuffer(char* srcBuf, DWORD buf_size);
	CBuffer(BYTE* srcBuf, DWORD buf_size);
	~CBuffer();
};
class CAttachBuffer : public CBuffer
{
protected:
	long m_idKey;
public:
	char name[17];
	CAttachBuffer(long key){m_idKey=key;}
	virtual ~CAttachBuffer(void){;}
	long Key(){return m_idKey;}
};

#endif // !defined(AFX_BUFFER_H__10945DBC_F91B_433A_97DD_482C7BC01747__INCLUDED_)
