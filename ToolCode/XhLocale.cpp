#include "StdAfx.h"
#include ".\XhLocale.h"
#include ".\ClientId.h"

XHLOCALE::XHLOCALE()
{
	_ciCustomerSerial=0;
	szAppTitle="";			//程序主标题
#ifdef __LDS_CONTEXT_
	szLicenseCopyright="";	//授权与版权
#endif
}
CXhChar50 XHLOCALE::get_szCustomerName()
{
	return GetClientNameById(_ciCustomerSerial);
}
void XHLOCALE::InitCustomerSerial(UINT uiSerial)
{
	if(uiSerial==CID_XiNanYuanPower)
		szAppTitle.Printf("西南电力设计院\n北京信狐天诚软件科技有限公司版权所有 (C) 2008-2019");
	_ciCustomerSerial=uiSerial;
}
LOCALE_PROPITEM* XHLOCALE::AddLocaleItemLong(const char* keyidstr,long liPropVal,const char* name/*=NULL*/)
{
	if(keyidstr==NULL||keyidstr[0]=='0')
		return NULL;
	LOCALE_PROPITEM* pItem=this->hashLocaleProp.Add(keyidstr);
	pItem->idKeyStr=keyidstr;
	pItem->name=name;
	pItem->ciValType=LOCALE_PROPITEM::DTVALTYPE_LONG;
	pItem->liVal=liPropVal;
	return pItem;
}
LOCALE_PROPITEM* XHLOCALE::AddLocaleItemDouble(const char* keyidstr,double dfPropVal,const char* name/*=NULL*/)
{
	if(keyidstr==NULL||keyidstr[0]=='0')
		return NULL;
	LOCALE_PROPITEM* pItem=this->hashLocaleProp.Add(keyidstr);
	pItem->idKeyStr=keyidstr;
	pItem->name=name;
	pItem->ciValType=LOCALE_PROPITEM::DTVALTYPE_LONG;
	pItem->dfVal=dfPropVal;
	return pItem;
}

LOCALE_PROPITEM* XHLOCALE::AddLocaleItemBool(const char* keyidstr,bool blPropVal,const char* name/*=NULL*/)
{
	if(keyidstr==NULL||keyidstr[0]=='0')
		return NULL;
	LOCALE_PROPITEM* pItem=this->hashLocaleProp.Add(keyidstr);
	pItem->idKeyStr=keyidstr;
	pItem->name=name;
	pItem->ciValType=LOCALE_PROPITEM::DTVALTYPE_BOOL;
	pItem->bVal=blPropVal;
	return pItem;
}
LOCALE_PROPITEM* XHLOCALE::AddLocaleItemStr100(const char* keyidstr,const char* pszPropVal,const char* name/*=NULL*/)
{
	if(keyidstr==NULL||keyidstr[0]=='0')
		return NULL;
	LOCALE_PROPITEM* pItem=this->hashLocaleProp.Add(keyidstr);
	pItem->idKeyStr=keyidstr;
	pItem->name=name;
	pItem->ciValType=LOCALE_PROPITEM::DTVALTYPE_STR100;
	CXhString szValStr100(pItem->szStrBuff,101);
	szValStr100.Copy(pszPropVal);
	return pItem;
}

void TestCustomizeConfig()
{
	XHLOCALE gxLocaleCustomize;
	//内部
	gxLocaleCustomize.InitCustomerSerial(2);
	//外围程序初始化
	if(gxLocaleCustomize.ciCustomerSerial==2)
	{
		gxLocaleCustomize.AddLocaleItemDouble("KaiheAngle.Threshold",2.5,"");
		gxLocaleCustomize.AddLocaleItemDouble("KaiheAngle.Deita",2.5,"");
	}
	//外围使用环境
	/*if(gxLocaleCustomize.GetLocaleItem("KaiheAngleThreshold"))
		;
	else
		;*/
}
