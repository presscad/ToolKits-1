#include <afxwin.h>
#include "ServerObject.h"
#include "ArrayList.h"
#include "LogFile.h"

//////////////////////////////////////////////////////////////////////////
// CServerObject
BOOL CServerObject::ParseSaveOrUpdateServerRetStr(char *sResult)
{
	return TRUE;
}
bool CServerObject::ExtractFromXmlStr(CMarkup& xml)
{
	CString attribstr=xml.GetAttrib("id");
	if(attribstr.GetLength()==0)
		return false;
	_id=atoi(attribstr);
	return true;
}
bool CServerObject::ExtractObjArrFromXmlStr(const char *sRsltXmlStr,CServerObject** ppObjArr,int count/*=1*/)
{
	CMarkup xml(sRsltXmlStr);
	xml.ResetMainPos();
	int i=0;
	CXhChar100 objname=count>0?(*ppObjArr)[0].GetServerClassName():"";

	CXhChar100 objsetname = objname;
	objsetname.Append("Set");
	while(i<count&&xml.FindChildElem())   
	{
		xml.IntoElem();
		CString elemname=xml.GetTagName();
		if(elemname.Compare(objsetname)==0)
		{
			while(i<count&&xml.FindChildElem())   
			{
				xml.IntoElem();
				if(ppObjArr[i]->ExtractFromXmlStr(xml))
					i++;
				xml.OutOfElem();
			}
		}
		else if(elemname.Compare(objname)==0)
		{
			ppObjArr[i]->ExtractFromXmlStr(xml);
			i++;
		}
		xml.OutOfElem();
	}
	return i>0;
}
