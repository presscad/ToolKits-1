#pragma once
#include "stdio.h"
#include "ArrayList.h"

struct BUFFER_IO;
struct LOG_METAREC {
	short idRecordType;	//��������־��¼����
	short snRecBuffLen;	//��������־��¼�ֽڳ���
	static const int POOL_LEN = 512;
	char datapool[512];
	LOG_METAREC();
	void Clear();
	void ToBuffer(BUFFER_IO* pIO);
	void FromBuffer(BUFFER_IO* pIO);
};
struct LOG_RECORD{
public:
	short idRecordType;	//��������־��¼����
	short snRecBuffLen;	//��������־��¼�ֽڳ���
	LOG_RECORD() {
		idRecordType = snRecBuffLen = 0;
	}
	virtual void ToBuffer(BUFFER_IO* pIO)=0;
	virtual long ToStrBuff(char* szLogMessage,long liMaxMsgBuffLen=0) = 0;
protected:
	virtual bool InternalFromMetaData(LOG_METAREC* pMetaData) = 0;
	virtual LOG_METAREC InternalMetaRecord()=0;
	//virtual time_t GetTimeStamp() = 0;
public:
	LOG_METAREC get_xMetaRecord() { return InternalMetaRecord(); }
	void set_xMetaRecord(LOG_METAREC metadata) { InternalFromMetaData(&metadata); }
	__declspec(property(get = get_xMetaRecord)) LOG_METAREC xMetaRecord;
};
struct LOG_STRING : public LOG_RECORD {
	LOG_STRING(const char* szErrLog=NULL);
	virtual void ToBuffer(BUFFER_IO* pIO);
	virtual void FromBuffer(BUFFER_IO* pIO);
	virtual long ToStrBuff(char* szLogMessage, long liMaxMsgBuffLen = 0);
protected:
	char _szInternalStrPool[501];
	virtual LOG_METAREC InternalMetaRecord();
	virtual bool InternalFromMetaData(LOG_METAREC* pMetaData);
};
struct LOG_DWORD_ARR_REC : public LOG_RECORD {
	LOG_DWORD_ARR_REC(DWORD *dwParaArr,int paraCount);
	LOG_DWORD_ARR_REC(DWORD para1 = 0xFFFFFFFF, int para2 = 0xFFFFFFFF, int para3 = 0xFFFFFFFF);
	virtual void ToBuffer(BUFFER_IO* pIO);
	virtual void FromBuffer(BUFFER_IO* pIO);
	virtual long ToString(char* szLogMessage, long liMaxMsgBuffLen = 0);
	virtual long ToStrBuff(char* szLogMessage, long liMaxMsgBuffLen = 0) { return 0; }
	virtual LOG_METAREC InternalMetaRecord();
	virtual bool InternalFromMetaData(LOG_METAREC* pMetaData);
	UINT* GetDwordData() { return (UINT*)_dwInternalDwordPool.Data(); }
	UINT GetDwordCount() { return _dwInternalDwordPool.Count; }
protected:
	PRESET_UINT16ARR _dwInternalDwordPool;
};
struct LOG_BYTE_ARR_REC : public LOG_RECORD {
	LOG_BYTE_ARR_REC(int logId=0, BYTE *byteArr=NULL, int len=0);
	virtual void ToBuffer(BUFFER_IO* pIO);
	virtual void FromBuffer(BUFFER_IO* pIO);
	virtual long ToString(char* szLogMessage, long liMaxMsgBuffLen = 0);
	virtual long ToStrBuff(char* szLogMessage, long liMaxMsgBuffLen = 0) { return 0; }
	virtual LOG_METAREC InternalMetaRecord();
	virtual bool InternalFromMetaData(LOG_METAREC* pMetaData);
	int LogId() { return _logId; }
	short ByteCount() { return _snByteCount; }
	char* Data() { return _datapool; }
	int ToByteArr(char* data_out,int max_count);
protected:
	int _logId;				//id
	short _snByteCount;		//�ֽ���
	static const int BYTE_MAX_COUNT = 504;
	char _datapool[504];	//�ֽ�����
};
struct ILogBin2File{
	//��־��ȫ��ʾ�ȼ�
	static const BYTE WARNING_LEVEL0_NONE		= 0;	//������κξ�ʾ��Ϣ
	static const BYTE WARNING_LEVEL1_IMPORTANT	= 1;	//�������Ҫ��ʾ��Ϣ
	static const BYTE WARNING_LEVEL2_COMMON		= 2;	//���һ�㼶�����ϵľ�ʾ��Ϣ
	static const BYTE WARNING_LEVEL3_UNIMPORTANT= 3;	//���������̫��Ҫ���ϵľ�ʾ��Ϣ
	static const BYTE WARNING_LEVEL_ALL			= 0xff;	//���ȫ����ʾ��Ϣ
	//ciLowestWarningLevel,���Ϊ1������ֵԽ��ʾ�ȼ�Խ�� wjh-2016.6.19
	virtual void SetWarningLevel(BYTE ciLowestWarningLevel=WARNING_LEVEL_ALL)=0;
	virtual BYTE WarningLevel()=0;
	//��־��¼����
	virtual FILE* SelfFilePointer() = 0;
	static const long TYPE_REC_BLANKROW = 0;	//����
	static const long TYPE_REC_STRMSG	= 1;	//�ַ�����ʾ��Ϣ��¼
	static const long TYPE_REC_DWORDS	= 2;	//id���龯ʾ��Ϣ
	static const long TYPE_REC_BYTES	= 3;	//byte���龯ʾ��Ϣ
	virtual void InsertBlankRow()=0;
	virtual char* LatestLog()=0;
	virtual bool LevelLog(BYTE ciWarningLevel,const char *format,...)=0;
	virtual bool Log(const char *format,...)=0;
	//Log(char *format,...)�ὫĳЩ������Ϣ�е�'%'���ַ�ת��Ĩ�� wjh-2014.5.23
	virtual bool LogString(char *error, bool jumpToNextLine=true)=0;
	virtual bool LogObject(LOG_RECORD* pBinLogObj)=0;
	virtual bool ClearContents()=0;
	virtual bool IsHasContents()=0;
};
class CLogBinFile : public ILogBin2File
{
	FILE* m_fp;
	bool m_bHasContens,m_bInsertBlankRow,m_bInsertTitleRow;
	bool m_bAppendTimestamp;	//�Ƿ���ÿ��ĩβ׷��ʱ��� wjh-2017.10.8
	char file_name[200];
	void CloseFile();
	bool m_bInLifeCycle;
	BYTE m_ciLowestWarningLevel;		//��ʱ������־��¼
	bool m_bGlobalDisabled;	//ȫ��������־��¼
	char _internal_tmp_str[201];
	char _internal_titlestr[201];
	bool (*_FireReflectTo)(int iWarningType,long hRelaObj[2],const char* errmsg);
	int _iTempReflectWarningType;	//��ʱ�洢�����������ڽ���־��Ϣ���䵽��������
	long _hTempReflectRelaObj[2];	//��ʱ�洢�����������ڽ���־��Ϣ���䵽��������
	virtual FILE* SelfFilePointer() {return m_fp;}
private:
	friend class CLogErrorLife;
	bool IsInLifeCycle(){return m_bInLifeCycle;}
	void RegisterLifeCycle(){m_bInLifeCycle=true;}
	void UnRegisterLifeCycle(){m_bInLifeCycle=false;}
	void AttachReflector(bool (*_ReflectFunc)(int iWarningType,long hRelaObj[2],const char* errmsg));
	void UnAttachReflector();
public:
	CLogBinFile(char* fname=NULL,bool overwriteMode=true);
	~CLogBinFile(void);
	bool IsEnabled(){return !m_bGlobalDisabled;}
	void GlobalEnabled(bool enabled=true){m_bGlobalDisabled=!enabled;}
	bool IsTimestampEnabled(){return !m_bAppendTimestamp;}
	void EnableTimestamp(bool enabled=true){m_bAppendTimestamp=enabled;}
	//ciLowestWarningLevel,���Ϊ1������ֵԽ��ʾ�ȼ�Խ�� wjh-2016.6.19
	virtual void SetWarningLevel(BYTE ciLowestWarningLevel=WARNING_LEVEL_ALL){m_ciLowestWarningLevel=ciLowestWarningLevel;}
	virtual BYTE WarningLevel(){return m_ciLowestWarningLevel;}
	void InsertBlankRow(){m_bInsertBlankRow=true;}
	void InsertTitleRow(const char* title);
	void CancelSuspendTitleRow(){m_bInsertBlankRow=false;}
	bool InitLogFile(char* fname,bool overwriteMode=true);
	char* LogFilePath();
	char* LatestLog(){return _internal_tmp_str;}
	virtual bool LevelLog(BYTE ciWarningLevel,const char *format,...);
	bool Log(const char *format,...);
	//Log(char *format,...)�ὫĳЩ������Ϣ�е�'%'���ַ�ת��Ĩ�� wjh-2014.5.23
	bool LogString(char *error, bool jumpToNextLine=true);
	bool LogDwords(int logId, int para1 = -1, int para2 = -1, int para3 = -1, int para4 = -1, int para5 = -1);
	bool LogByteArr(int logId, char *byteArr, int count);
	bool LogByteArr(int logId, BYTE *byteArr, int count);
	virtual bool LogObject(LOG_RECORD* pBinLogObj);
	bool ClearContents();
	bool IsHasContents(){return m_bHasContens;}
	void ShowToScreen();
	static bool RestoreBinaryLogFile(const char* szBinLogFile);
};
extern CLogBinFile logbin;
