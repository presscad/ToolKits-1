#pragma once
#include "Buffer.h"
#include "XhCharString.h"
#include "MemberProperty.h"
#include "Markup.h"

class CServerObject
{
protected:
	int _id;
	CXhChar50 m_sServerClassName;
public:
	DECLARE_READONLY_VIRTUAL_PROPERTY(int ,Id){return _id;}
	DECLARE_READONLY_VIRTUAL_PROPERTY(int ,m_id){return _id;}
public:
	CServerObject(){_id=0;m_sServerClassName="CServerObject";}
	virtual CXhChar50 GetServerClassName(){return m_sServerClassName;}
	//
	virtual void ToBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0)=0;
	virtual void FromBuffer(CBuffer &buffer,bool bIncludeId=true,DWORD dwSerialzeFlag=0)=0;
	//
	void SetKey(DWORD id){_id=id;}
	BOOL IsEqual(int id){return _id==id;}
	virtual BOOL ParseSaveOrUpdateServerRetStr(char *sResult);
	virtual bool ExtractFromXmlStr(CMarkup& xml);
	static bool ExtractObjArrFromXmlStr(const char *sRsltXmlStr,CServerObject** ppObjArr,int count=1);
};

