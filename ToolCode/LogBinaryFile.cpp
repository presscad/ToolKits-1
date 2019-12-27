#include "stdafx.h"
#include <time.h>
#include ".\Buffer.h"
#include ".\XhCharString.h"
#include ".\LogBinaryFile.h"
#include ".\LogFile.h"

CLogBinFile logbin("d:\\warning.dmp");
//////////////////////////////////////////////////////////////////////////
// LOG_METAREC
LOG_METAREC::LOG_METAREC()
{
	memset(datapool, 0, POOL_LEN);
}
void LOG_METAREC::Clear()
{
	snRecBuffLen = 0;
	memset(datapool, 0, POOL_LEN);
}
void LOG_METAREC::ToBuffer(BUFFER_IO* pIO)
{
	pIO->WriteWord(idRecordType);
	pIO->WriteWord(snRecBuffLen);
	int nPoolLen = min(512, snRecBuffLen);
	pIO->WriteInteger(nPoolLen);
	if (nPoolLen > 0)
		pIO->Write(datapool, nPoolLen);
}
void LOG_METAREC::FromBuffer(BUFFER_IO* pIO)
{
	pIO->ReadWord(&idRecordType);
	pIO->ReadWord(&snRecBuffLen);
	int nPoolLen = 0;
	pIO->ReadInteger(&nPoolLen);
	if (nPoolLen > 0)
		pIO->Read(datapool, nPoolLen);
}
//////////////////////////////////////////////////////////////////////////
// LOG_STRING
LOG_STRING::LOG_STRING(const char* szErrLog)
{
	idRecordType = 1;
	long nStrLen = szErrLog == NULL ? 0 : (long)strlen(szErrLog);
	if (nStrLen > 0)
		memcpy(_szInternalStrPool, szErrLog, nStrLen);
	_szInternalStrPool[nStrLen] = 0;
	snRecBuffLen=(short)nStrLen;
}
bool LOG_STRING::InternalFromMetaData(LOG_METAREC* pMetaData)
{
	if (idRecordType != pMetaData->idRecordType)
		return false;
	this->snRecBuffLen = pMetaData->snRecBuffLen;
	memcpy(this->_szInternalStrPool, pMetaData->datapool, pMetaData->snRecBuffLen);
	return true;
}
LOG_METAREC LOG_STRING::InternalMetaRecord()
{
	LOG_METAREC meta;
	meta.idRecordType = this->idRecordType;
	meta.snRecBuffLen = min(512,this->snRecBuffLen);
	memcpy(meta.datapool, this->_szInternalStrPool, meta.snRecBuffLen);
	return meta;
}
void LOG_STRING::ToBuffer(BUFFER_IO* pIO) 
{
	LOG_METAREC meta = InternalMetaRecord();
	meta.ToBuffer(pIO);
}
void LOG_STRING::FromBuffer(BUFFER_IO* pIO) 
{
	LOG_METAREC meta;
	meta.FromBuffer(pIO);
	InternalFromMetaData(&meta);
}
long LOG_STRING::ToStrBuff(char* szLogMessage, long liMaxMsgBuffLen/* = 0*/) 
{
	if (szLogMessage == NULL)
		return this->snRecBuffLen;
	long liMsgBuffLen = (liMaxMsgBuffLen == 0) ? this->snRecBuffLen : min(liMaxMsgBuffLen, this->snRecBuffLen);
	memcpy(szLogMessage, this->_szInternalStrPool, liMsgBuffLen);
	return liMsgBuffLen;
}
//////////////////////////////////////////////////////////////////////////
// LOG_DWORD_ARR_REC
LOG_DWORD_ARR_REC::LOG_DWORD_ARR_REC(DWORD *dwParaArr, int paraCount)
{
	idRecordType = ILogBin2File::TYPE_REC_DWORDS;
	snRecBuffLen = 0;
	_dwInternalDwordPool.Clear();
	if (dwParaArr&&paraCount > 0)
	{
		for (int i = 0; i < paraCount; i++)
		{
			_dwInternalDwordPool.Append(dwParaArr[i]);
			snRecBuffLen += 4;
		}
	}
}
LOG_DWORD_ARR_REC::LOG_DWORD_ARR_REC(DWORD para1 /*= 0xFFFFFFFF*/, int para2 /*= 0xFFFFFFFF*/, int para3 /*= 0xFFFFFFFF*/)
{
	idRecordType = ILogBin2File::TYPE_REC_DWORDS;
	snRecBuffLen = 0;
	_dwInternalDwordPool.Clear();
	if (para1 != 0xFFFFFFFF)
	{
		_dwInternalDwordPool.Append(para1);
		snRecBuffLen += 4;
	}
	if (para2 != 0xFFFFFFFF)
	{
		_dwInternalDwordPool.Append(para2);
		snRecBuffLen += 4;
	}
	if (para3 != 0xFFFFFFFF)
	{
		_dwInternalDwordPool.Append(para3);
		snRecBuffLen += 4;
	}
}
bool LOG_DWORD_ARR_REC::InternalFromMetaData(LOG_METAREC* pMetaData)
{
	if (idRecordType != pMetaData->idRecordType)
		return false;
	if (pMetaData->snRecBuffLen % 4 > 0)
		return false;
	snRecBuffLen = pMetaData->snRecBuffLen;
	int nCount = snRecBuffLen / 4;
	_dwInternalDwordPool.Clear();
	if (nCount > 0)
	{
		CBuffer buffer;
		buffer.AttachMemory(pMetaData->datapool, pMetaData->snRecBuffLen);
		buffer.SeekToBegin();
		for (int i = 0; i < nCount; i++)
		{
			int nValue = buffer.ReadInteger();
			_dwInternalDwordPool.Append(nValue);
		}
	}
	else
		this->snRecBuffLen = 0;
	return true;
}
LOG_METAREC LOG_DWORD_ARR_REC::InternalMetaRecord()
{
	LOG_METAREC meta;
	meta.idRecordType = this->idRecordType;
	CBuffer buffer;
	buffer.SeekToBegin();
	for (UINT i = 0; i < _dwInternalDwordPool.Count; i++)
		buffer.WriteInteger(_dwInternalDwordPool[i]);
	meta.snRecBuffLen = (short)min(512, buffer.Length);
	memcpy(meta.datapool, buffer.GetBufferPtr(), meta.snRecBuffLen);
	return meta;
}
void LOG_DWORD_ARR_REC::ToBuffer(BUFFER_IO* pIO) {
	LOG_METAREC meta=InternalMetaRecord();
	meta.ToBuffer(pIO);
}
void LOG_DWORD_ARR_REC::FromBuffer(BUFFER_IO* pIO) {
	LOG_METAREC meta;
	meta.FromBuffer(pIO);
	InternalFromMetaData(&meta);
}
long LOG_DWORD_ARR_REC::ToString(char* szLogMessage, long liMaxMsgBuffLen/* = 0*/) {
	if (szLogMessage == NULL)
		return this->snRecBuffLen;
	long liMsgBuffLen = (liMaxMsgBuffLen == 0) ? this->snRecBuffLen : min(liMaxMsgBuffLen, this->snRecBuffLen);
	//memcpy(szLogMessage, this->_szInternalStrPool, liMsgBuffLen);
	return liMsgBuffLen;
}

//////////////////////////////////////////////////////////////////////////
// LOG_BYTE_ARR_REC
LOG_BYTE_ARR_REC::LOG_BYTE_ARR_REC(int logId /*= 0*/, BYTE *byteArr/*=NULL*/, int len/*=0*/)
{
	idRecordType = ILogBin2File::TYPE_REC_BYTES;
	_logId = logId;
	_snByteCount = min(len,BYTE_MAX_COUNT);
	memset(_datapool, 0, BYTE_MAX_COUNT);
	if (byteArr&&len > 0)
		memcpy(_datapool, byteArr, _snByteCount);
}
bool LOG_BYTE_ARR_REC::InternalFromMetaData(LOG_METAREC* pMetaData)
{
	if (pMetaData==NULL || idRecordType != pMetaData->idRecordType)
		return false;
	snRecBuffLen = pMetaData->snRecBuffLen;
	_logId = 0;
	_snByteCount = 0;
	memset(_datapool, 0, BYTE_MAX_COUNT);
	if (pMetaData->snRecBuffLen > 0)
	{
		CBuffer buffer;
		buffer.AttachMemory(pMetaData->datapool, pMetaData->snRecBuffLen);
		buffer.SeekToBegin();
		_logId = buffer.ReadInteger();
		buffer.ReadWord(&_snByteCount);
		if(_snByteCount>0)
			buffer.Read(_datapool, _snByteCount);
	}
	return true;
}
LOG_METAREC LOG_BYTE_ARR_REC::InternalMetaRecord()
{
	LOG_METAREC meta;
	meta.idRecordType = this->idRecordType;
	CBuffer buffer;
	buffer.SeekToBegin();
	buffer.WriteInteger(_logId);
	buffer.WriteWord(_snByteCount);
	if (_snByteCount > 0)
		buffer.Write(_datapool, _snByteCount);
	//
	meta.snRecBuffLen = (short)min(LOG_METAREC::POOL_LEN, buffer.Length);
	memcpy(meta.datapool, buffer.GetBufferPtr(), meta.snRecBuffLen);
	return meta;
}
void LOG_BYTE_ARR_REC::ToBuffer(BUFFER_IO* pIO) 
{
	LOG_METAREC meta = InternalMetaRecord();
	meta.ToBuffer(pIO);
}
void LOG_BYTE_ARR_REC::FromBuffer(BUFFER_IO* pIO) {
	LOG_METAREC meta;
	meta.FromBuffer(pIO);
	InternalFromMetaData(&meta);
}
long LOG_BYTE_ARR_REC::ToString(char* szLogMessage, long liMaxMsgBuffLen/* = 0*/) {
	if (szLogMessage == NULL)
		return this->snRecBuffLen;
	if (szLogMessage != NULL)
	{
		CXhChar500 sText("Id=%d 0x",_logId);
		for (int i = 0; i < _snByteCount; i++)
		{
			if (_datapool[i] > 0x0f)
				sText.Append(CXhChar16("%2X", _datapool[i]));
			else
				sText.Append(CXhChar16("0%X", _datapool[i]));
		}
		long liMsgBuffLen = (liMaxMsgBuffLen == 0) ? sText.GetLength() : min(liMaxMsgBuffLen, sText.GetLength());
		memcpy(szLogMessage, sText, liMsgBuffLen);
	}
	return snRecBuffLen;
}
int LOG_BYTE_ARR_REC::ToByteArr(char* data_out, int max_count)
{
	if (data_out == NULL || max_count == 0)
		return 0;
	long liMsgBuffLen = (max_count == 0) ? _snByteCount : min(max_count, _snByteCount);
	memcpy(data_out, _datapool, liMsgBuffLen);
	return liMsgBuffLen;
}
//////////////////////////////////////////////////////////////////////////
// CLogBinFile
bool CLogBinFile::LogObject(LOG_RECORD* pBinLogObj)
{
	if (m_fp == NULL)
		m_fp = fopen(file_name, "wb");
	if (m_fp == NULL)
		return false;
	m_bHasContens = true;
	//写入二进制格式的日志文件头
	fseek(m_fp, 0, SEEK_END);
	long flen = ftell(m_fp);
	UINT uiLogDocType = 857235986, uiLogFileVersion = 0;
	if (flen == 0)
	{
		fwrite(&uiLogDocType, 4, 1, this->SelfFilePointer());
		fwrite(&uiLogFileVersion, 4, 1, this->SelfFilePointer());
	}
	//
	LOG_METAREC meta = pBinLogObj->xMetaRecord;
	fwrite(&meta.idRecordType, 2, 1, m_fp);
	fwrite(&meta.snRecBuffLen, 2, 1, m_fp);
	fwrite(&meta.datapool, meta.snRecBuffLen, 1, m_fp);
	fflush(this->SelfFilePointer());
	return true;
}
CLogBinFile::CLogBinFile(char* fname/*=NULL*/,bool overwriteMode/*=true*/)
{
	m_fp=NULL;
	if(fname==NULL)	//临时抑制日志记录
		m_ciLowestWarningLevel=0;
	else
		InitLogFile(fname);
	m_bInsertBlankRow=false;
	m_bHasContens=false;
	m_bInLifeCycle=false;
	m_bGlobalDisabled=false;
	m_bInsertTitleRow=false;
	m_ciLowestWarningLevel = WARNING_LEVEL2_COMMON;
	m_bAppendTimestamp=!overwriteMode;
	memset(_internal_titlestr,0,201);
	_FireReflectTo=NULL;
	_iTempReflectWarningType=0;							//临时存储变量，常用于将日志信息反射到任务栏中
	_hTempReflectRelaObj[0]=_hTempReflectRelaObj[1]=0;	//临时存储变量，常用于将日志信息反射到任务栏中
}

CLogBinFile::~CLogBinFile(void)
{
	CloseFile();
}
void CLogBinFile::AttachReflector(bool (*_ReflectFunc)(int iWarningType,long hRelaObj[2],const char* errmsg))
{
	_FireReflectTo=_ReflectFunc;
}
void CLogBinFile::UnAttachReflector()
{
	_FireReflectTo=NULL;
}
bool CLogBinFile::InitLogFile(char* fname,bool overwriteMode/*=true*/)
{
	FILE* fp2=fopen(fname,overwriteMode?"wb":"ab");
	if(fp2!=NULL)
	{
		if(m_fp!=NULL)
			fclose(m_fp);
		m_fp=fp2;
		strcpy(file_name,fname);
	}
	if(m_fp==NULL)
		return false;
	m_bHasContens=false;
	m_bGlobalDisabled=false;
	m_bAppendTimestamp=!overwriteMode;
	m_ciLowestWarningLevel=WARNING_LEVEL2_COMMON;	//默认输出普通等级以上的警示信息
	return true;
}
char* CLogBinFile::LogFilePath()
{
	return file_name;
}
void CLogBinFile::InsertTitleRow(const char* title)
{
	if(strlen(title)<201)
		strcpy(_internal_titlestr,title);	//防止lengthOfDestBuf输入错误,实际字符串也并不超过strDest缓存长度时出错 
	else
	{
		strncpy(_internal_titlestr,title,200);
		_internal_titlestr[200]=0;
	}
	m_bInsertTitleRow=true;
}
bool CLogBinFile::ClearContents()
{
	if(m_fp!=NULL)
	{
		fclose(m_fp);
		m_fp=fopen(file_name,"wb");
		if(m_fp==NULL)
			return false;
	}
	m_bHasContens=false;
	return true;
}
void CLogBinFile::ShowToScreen()
{
	if(m_fp)
		fflush(m_fp);
	if(m_bHasContens)
	{
		char cmdstr[200]={0};
		//_snprintf(cmdstr,200,"notepad.exe %s",file_name);
		//WinExec(cmdstr,SW_SHOW);
	}
}

bool CLogBinFile::LevelLog(BYTE ciWarningLevel,const char *format,...)
{
	if(ciWarningLevel>m_ciLowestWarningLevel||m_bGlobalDisabled)
		return false;
	if(m_fp==NULL)
	{
		m_fp=fopen(file_name,"wb");
		m_bHasContens=false;
	}
	if(m_fp==NULL)
		return false;
	CXhChar200 szError;
	if(m_bHasContens&&m_bInsertBlankRow)
		szError.Printf("\n");
	if(m_bInsertTitleRow)
		szError.Append(_internal_titlestr);
	m_bInsertBlankRow=m_bInsertTitleRow=false;
	char timestr[51]={0};
	if(m_bAppendTimestamp)
	{
		time_t nowT=time(NULL);
		tm* _Tm=localtime(&nowT);
		strftime(timestr,50,"%Y-%m-%d %H:%M:%S",_Tm);
		//_snprintf(timestr,50,"%4d-%2d-%2d %2d:%2d:%2d",nowT.GetYear(),nowT.GetMonth(),nowT.GetDay(),
		//	nowT.GetHour(),nowT.GetMinute(),nowT.GetSecond());
	}
	va_list marker;
	va_start(marker, format);
	memset(_internal_tmp_str,0,201);
	_vsnprintf(_internal_tmp_str,200,format,marker);
	size_t str_len=strlen(_internal_tmp_str);
	if(_FireReflectTo)
		_FireReflectTo(_iTempReflectWarningType,_hTempReflectRelaObj,_internal_tmp_str);
	else 
	if(_internal_tmp_str[str_len-1]=='\n')
		szError.Append(CXhChar50("%s %s",_internal_tmp_str,timestr));
	else
		szError.Append(CXhChar50("%s %s\n",_internal_tmp_str,timestr));
	LOG_STRING logstr(szError);
	this->LogObject(&logstr);
	return true;
}
bool CLogBinFile::Log(const char *format,...)
{
	if(WARNING_LEVEL2_COMMON>m_ciLowestWarningLevel||m_bGlobalDisabled)
		return false;
	if(m_fp==NULL)
	{
		m_fp=fopen(file_name,"wb");
		m_bHasContens=false;
	}
	if(m_fp==NULL)
		return false;
	CXhChar200 szError;
	if(m_bHasContens&&m_bInsertBlankRow)
		szError.Printf("\n");
	if(m_bInsertTitleRow)
		szError.Append(_internal_titlestr);
	m_bInsertBlankRow=m_bInsertTitleRow=false;
	char timestr[51]={0};
	if(m_bAppendTimestamp)
	{
		time_t nowT=time(NULL);
		tm* _Tm=localtime(&nowT);
		strftime(timestr,50,"%Y-%m-%d %H:%M:%S",_Tm);
		//_snprintf(timestr,50,"%4d-%2d-%2d %2d:%2d:%2d",nowT.GetYear(),nowT.GetMonth(),nowT.GetDay(),
		//	nowT.GetHour(),nowT.GetMinute(),nowT.GetSecond());
	}
	va_list marker;
	va_start(marker, format);
	memset(_internal_tmp_str,0,201);
	_vsnprintf(_internal_tmp_str,200,format,marker);
	size_t str_len=strlen(_internal_tmp_str);
	if(_FireReflectTo)
		_FireReflectTo(_iTempReflectWarningType,_hTempReflectRelaObj,_internal_tmp_str);
	else 
	if(_internal_tmp_str[str_len-1]=='\n')
		szError.Append(CXhChar50("%s %s",_internal_tmp_str,timestr),' ');
	else
		szError.Append(CXhChar50("%s %s\n",_internal_tmp_str,timestr), ' ');
	LOG_STRING logstr(szError);
	this->LogObject(&logstr);
	return true;
}
bool CLogBinFile::LogString(char *error, bool jumpToNextLine/*=true*/)
{
	if(m_fp==NULL)
	{
		m_fp=fopen(file_name,"wb");
		m_bHasContens=false;
	}
	if(m_fp==NULL)
		return false;
	CXhChar200 szError;
	if(m_bHasContens&&m_bInsertBlankRow)
		szError.Printf("\n");
	if(m_bInsertTitleRow)
		szError.Append(_internal_titlestr);
	m_bInsertBlankRow=m_bInsertTitleRow=false;
	memset(_internal_tmp_str,0,201);
	strncpy(_internal_tmp_str,error,200);
	size_t str_len=strlen(_internal_tmp_str);
	if(_FireReflectTo)
		_FireReflectTo(_iTempReflectWarningType,_hTempReflectRelaObj,_internal_tmp_str);
	else 
	if(_internal_tmp_str[str_len-1]=='\n'||!jumpToNextLine)
		szError.Append(CXhChar50("%s",_internal_tmp_str));
	else
		szError.Append(CXhChar50("%s\n",_internal_tmp_str));
	LOG_STRING logstr(szError);
	this->LogObject(&logstr);
	return true;
}
bool CLogBinFile::LogDwords(int logId, int para1 /*= -1*/, int para2 /*= -1*/, 
							int para3 /*= -1*/, int para4 /*= -1*/, int para5 /*= -1*/)
{
	if (m_fp == NULL)
	{
		m_fp = fopen(file_name, "wb");
		m_bHasContens = false;
	}
	if (m_fp == NULL)
		return false;
	PRESET_UINT16ARR idArr;
	idArr.Append(logId);
	if (para1 != -1)
		idArr.Append(para1);
	if (para2 != -1)
		idArr.Append(para2);
	if (para3 != -1)
		idArr.Append(para3);
	if (para4 != -1)
		idArr.Append(para4);
	if (para5 != -1)
		idArr.Append(para5);
	LOG_DWORD_ARR_REC logDwords((DWORD*)idArr.Data(),(int)idArr.Count);
	//size_t str_len = strlen(_internal_tmp_str);
	//if (_FireReflectTo)
	//	_FireReflectTo(_iTempReflectWarningType, _hTempReflectRelaObj, _internal_tmp_str);
	return this->LogObject(&logDwords);
}
bool CLogBinFile::LogByteArr(int logId,char *byteArr,int count)
{
	return LogByteArr(logId,(BYTE*)byteArr,count);
}
bool CLogBinFile::LogByteArr(int logId, BYTE *byteArr, int count)
{
	if (m_fp == NULL)
	{
		m_fp = fopen(file_name, "wb");
		m_bHasContens = false;
	}
	if (m_fp == NULL)
		return false;
	if (logId == 0 || byteArr == NULL || count <= 0)
		return false;
	int hits=0;
	for (int i=0;i<count;i+=500,hits++)
	{
		int bytescount=500;
		if (i+500>count)
			bytescount=count-i;
		LOG_BYTE_ARR_REC logDwords(logId+hits*100,byteArr+i,bytescount);
		this->LogObject(&logDwords);
	}
	return true;
}
void CLogBinFile::CloseFile()
{
	if(m_fp!=NULL)
		fclose(m_fp);
	m_fp=NULL;
}
bool CLogBinFile::RestoreBinaryLogFile(const char* szBinLogFile)
{
	CXhChar200 fname = szBinLogFile;
	int len = strlen(fname);
	int i = 0;
	for (i = len - 1; i >= 0; i--)
	{
		if (fname[i] == '.')
			break;
	}
	char* pSpliter = i < 0 ? NULL : &fname[i];
	if (stricmp(pSpliter + 1, "dmp") == 0)
		strcpy(pSpliter, ".log");
	CLogFile logtxt(fname);
	FILE* fp = fopen(szBinLogFile, "rb");
	if (fp == NULL)
		return false;
	UINT uiDocType, uiVersion;
	size_t uiReadBytes = 0;
	uiReadBytes = fread(&uiDocType, 4, 1, fp);
	uiReadBytes = fread(&uiVersion, 4, 1, fp);
	LOG_METAREC meta;
	while (uiReadBytes > 0)
	{
		if (fread(&meta.idRecordType, 2, 1, fp) < 2)
			break;
		if (fread(&meta.snRecBuffLen, 2, 1, fp) < 2)
			break;
		if (fread(&meta.datapool, 1, meta.snRecBuffLen, fp) < (UINT)meta.snRecBuffLen)
			break;
		if (meta.idRecordType == 0)
			logtxt.Log("\n");
		else if (meta.idRecordType == CLogBinFile::TYPE_REC_STRMSG)
		{
			meta.datapool[meta.snRecBuffLen] = 0;
			logtxt.Log(meta.datapool);
		}
		else
			continue;
	}
	if (logtxt.IsHasContents())
		logtxt.ShowToScreen();
	return true;
}
