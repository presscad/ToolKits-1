#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Wininet.h>
#include ".\XhCharString.h"
#pragma comment (lib, "Wininet.lib")

static int RecogMonthOnYear(const char* itemstr)
{	//识别成功返回月份,否则返回零
	int tm_mon=0;
	if(stricmp(itemstr,"Jan")==0||stricmp(itemstr,"January")==0)
		tm_mon=1;
	else if(stricmp(itemstr,"Feb")==0||stricmp(itemstr,"February")==0)
		tm_mon=2;
	else if(stricmp(itemstr,"Mar")==0||stricmp(itemstr,"March")==0)
		tm_mon=3;
	else if(stricmp(itemstr,"Apr")==0||stricmp(itemstr,"April")==0)
		tm_mon=4;
	else if(stricmp(itemstr,"May")==0)
		tm_mon=5;
	else if(stricmp(itemstr,"Jun")==0||stricmp(itemstr,"June")==0)
		tm_mon=6;
	else if(stricmp(itemstr,"Jul")==0||stricmp(itemstr,"July")==0)
		tm_mon=7;
	else if(stricmp(itemstr,"Aug")==0||stricmp(itemstr,"August")==0)
		tm_mon=8;
	else if(stricmp(itemstr,"Sep")==0||stricmp(itemstr,"September")==0)
		tm_mon=9;
	else if(stricmp(itemstr,"Oct")==0||stricmp(itemstr,"October")==0)
		tm_mon=10;
	else if(stricmp(itemstr,"Nov")==0||stricmp(itemstr,"November")==0)
		tm_mon=11;
	else if(stricmp(itemstr,"Dec")==0||stricmp(itemstr,"December")==0)
		tm_mon=12;
	return tm_mon;
}
static char* _Local_SearchChar(char* srcStr,char ch=':')
{
	int len=strlen(srcStr);
	for(char* iter=srcStr;*iter!=0;iter++)
	{
		if(*iter==ch)
			return iter;
	}
	return NULL;
}
static bool splitGmtTimeStr(char* timestr,tm* pTm)
{
	char* key=strtok(timestr,","),*splitter=NULL,cindex=0;
	bool findGMT=false,findWeek=false;
	CXhChar50 gmtstr;
	while(key!=NULL)
	{
		gmtstr=key;
		key=strtok(NULL,";");
		if(!findWeek&&(strstr(gmtstr,"Mon")!=NULL||
			strstr(gmtstr,"Tue")!=NULL||strstr(gmtstr,"Wed")!=NULL||strstr(gmtstr,"Thu")!=NULL||
			strstr(gmtstr,"Fri")!=NULL||strstr(gmtstr,"Sat")!=NULL||strstr(gmtstr,"Sun")!=NULL))
		{
			findWeek=true;
			continue;	//星期
		}
		if(strstr(gmtstr,"GMT")!=NULL||strstr(gmtstr,"UTC")!=NULL)
		{
			findGMT=true;
			break;
		}
	}
	if(!findGMT)
		return false;
	CXhChar16 items[5];
	key=strtok(gmtstr," ");
	while(key!=NULL&&cindex<5)
	{
		items[cindex].Copy(key);
		if((key=strtok(NULL," "))!=NULL)
			cindex++;
	}
	if(cindex!=4)
		return false;	//格式不对
	tm gmtnow;
	memset(&gmtnow,0,sizeof(tm));
	if((gmtnow.tm_mon=RecogMonthOnYear(items[1]))>0)
	{	//识别成功
		if(items[2].GetLength()==4)
		{	//格式日,月,年
			gmtnow.tm_year=atoi(items[2]);
			gmtnow.tm_mday=atoi(items[0]);
		}
		else if(items[0].GetLength()==4)
		{	//格式年,月,日
			gmtnow.tm_year=atoi(items[0]);
			gmtnow.tm_mday=atoi(items[2]);
		}
	}
	else if((gmtnow.tm_mon=RecogMonthOnYear(items[0]))>0)
	{	//识别成功
		if(items[2].GetLength()==4)
		{	//格式月,日,年
			gmtnow.tm_year=atoi(items[2]);
			gmtnow.tm_mday=atoi(items[1]);
		}
		else if(items[1].GetLength()==4)
		{	//格式月,年,日
			gmtnow.tm_year=atoi(items[1]);
			gmtnow.tm_mday=atoi(items[2]);
		}
	}
	if(gmtnow.tm_year==0)
		return false;
	gmtnow.tm_mon -=1;		//tm结构中mon范围是0~11
	gmtnow.tm_year-=1900;	//GMT时间从1900年开始计时
	key=(char*)items[3];
	CXhChar16 hms;
	//提取小时
	if((splitter=_Local_SearchChar(key,':'))==NULL)
		return false;
	hms.NCopy(&key,(WORD)(splitter-key),true);
	gmtnow.tm_hour=atoi(hms);
	key++;
	//提取分钟
	if((splitter=_Local_SearchChar(key,':'))==NULL)
		return false;
	hms.NCopy(&key,(WORD)(splitter-key),true);
	gmtnow.tm_min=atoi(hms);
	key++;
	//提取秒
	gmtnow.tm_sec=atoi(key);
	*pTm=gmtnow;
	return true;
}
struct INTERNET_HANDLES{
	HINTERNET hInter,hUrl;
	INTERNET_HANDLES(){hInter=hUrl=NULL;}
	~INTERNET_HANDLES()
	{
		if(hInter!=NULL)InternetCloseHandle(hInter);
		if(hUrl!=NULL)	InternetCloseHandle(hUrl);
	}
};
#ifdef __EXPORT_LOGFILE_
#include "LogBinaryFile.h"
#include "TimerCount.h"
extern CLogBinFile loglic;
#endif
static DWORD WINAPI _LocalNetTime(LPVOID lpParam)
{
	char datestr[50]={0},i;
	DWORD bufsize=50;
	LPCSTR ntp_urls[3]={"http://www.fmprc.gov.cn","http://www.baidu.com","http://www.xerofox.com"};
	INTERNET_HANDLES ntp;	//
#if defined(__EXPORT_LOGFILE_)&&defined(_TMP_TEST_LOG_)
	timer.Relay(220);
#endif
	if((ntp.hInter=InternetOpen("HttpGet",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,NULL))==NULL)
		return false;
#if defined(__EXPORT_LOGFILE_)&&defined(_TMP_TEST_LOG_)
	timer.Relay(221);
#endif
	for(i=0;i<3;i++)
	{
#if defined(__EXPORT_LOGFILE_)&&defined(_TMP_TEST_LOG_)
		loglic.Log("tick i=%d started",i);
#endif
		if((ntp.hUrl=InternetOpenUrl(ntp.hInter,ntp_urls[i],NULL,0,INTERNET_FLAG_RAW_DATA,NULL))!=NULL)
			break;
#if defined(__EXPORT_LOGFILE_)&&defined(_TMP_TEST_LOG_)
		loglic.Log("tick i=%d ended",i);
		timer.Relay(222+i);
#endif
	}
#if defined(__EXPORT_LOGFILE_)&&defined(_TMP_TEST_LOG_)
		loglic.Log("iter=%d,hUrl=%X",i,ntp.hUrl);
#endif
	if(i==3)
		return false;
	if(!HttpQueryInfo(ntp.hUrl,HTTP_QUERY_DATE,datestr,&bufsize,NULL))
	{
#if defined(__EXPORT_LOGFILE_)&&defined(_TMP_TEST_LOG_)
		loglic.Log("HttpQuery failed");
#endif
		return false;
	}
#if defined(__EXPORT_LOGFILE_)&&defined(_TMP_TEST_LOG_)
	loglic.Log(datestr);
	timer.Relay(225);
#endif
	tm gmtnow;
	if(!splitGmtTimeStr(datestr,&gmtnow))
		return 0;
#if defined(__EXPORT_LOGFILE_)&&defined(_TMP_TEST_LOG_)
	timer.Relay(226);
#endif
	time_t tm=mktime(&gmtnow) - _timezone;	//相当于VC9之后的_mkgmtime(&gmtnow)函数,出于兼空性考虑暂用mktime函数转换 wjh-2017.11.1
	if(lpParam)
		memcpy(lpParam,&tm,sizeof(time_t));
	return TRUE;
}
time_t NetTime(int niTimeOutMilliseconds=0)
{
	DWORD id;
	int result = -1;
	time_t tm=0;
	DWORD status=WAIT_OBJECT_0;
	if(niTimeOutMilliseconds>0)
	{
		HANDLE hThread = CreateThread(NULL, 0, _LocalNetTime, &tm, 0, &id);
		status=WaitForSingleObject(hThread,niTimeOutMilliseconds);
		if (hThread)
			CloseHandle(hThread);
	}
	else if(!_LocalNetTime(&tm))
		tm=0;
	return (status==WAIT_OBJECT_0)?tm:0;
}
