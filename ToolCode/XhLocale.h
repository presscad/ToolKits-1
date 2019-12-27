#pragma once
#include <WinDef.h>
#include "HashTable.h"
#include "XhCharString.h"

#ifndef _STRUCT_LOCALE_PROPITEM_DEFINED_
#define _STRUCT_LOCALE_PROPITEM_DEFINED_
struct LOCALE_PROPITEM{
	static const BYTE DTVALTYPE_BOOL	= 0;
	static const BYTE DTVALTYPE_LONG	= 1;
	static const BYTE DTVALTYPE_DOUBLE	= 2;
	static const BYTE DTVALTYPE_STR100	= 3;
public:
	long id;
	CXhChar100 idKeyStr;	//标识字符串
	CXhChar100 name;
	char ciValType;	//0.bool;1.long;2.double;3.短字符串
	union{
		bool bVal;
		long liVal;
		double dfVal;
		char szStrBuff[101];
	};
};
#endif
struct XHLOCALE{
	static const BYTE CID_Xerofox			=	1;	//id=1.信狐天诚
	static const BYTE CID_NanTong_HuaCan	=	2;	//id=2.表示江苏南通华灿
	static const BYTE CID_QingDao_WuXiao	=	3;	//id=3.青岛武晓
	static const BYTE CID_ChongQing_ShunTai	=	4;	//id=4.重庆顺泰
	static const BYTE CID_QingDao_DongFang	=	5;	//id=5.青岛东方
	static const BYTE CID_GuangZhou_ZengLi	=	6;	//id=6.广州增立
	static const BYTE CID_AnHui_HongYuan	=	7;	//id=7.安徽宏源
	static const BYTE CID_YunNan_JingYunXiang=	8;	//id=8.云南景云祥
	static const BYTE CID_ChongQing_JiangDian=	9;	//id=9.重庆江电
	static const BYTE CID_QingDao_HuiJinTong=	10;	//id=10.青岛汇金通
	static const BYTE CID_ChangChun_JuDeLong=	11;	//id=11.长春聚德隆
	static const BYTE CID_ShanXi_BaoJi		=	12;	//id=12.宝鸡塔
	static const BYTE CID_YunNan_JianYuan	=	13;	//id=13.云南建源
	static const BYTE CID_YunNan_DongDian	=	14;	//id=14.云南东电
	static const BYTE CID_YunNan_ZhanYe		=	15;	//id=15.云南展业
	static const BYTE CID_XiNanYuanPower	=	16;	//id=16.西南电力设计院
protected:
	BYTE _ciCustomerSerial;	//客户定制标识序号
public:
	BYTE get_ciCustomerSerial(){return _ciCustomerSerial;}
	__declspec(property(get=get_ciCustomerSerial)) BYTE ciCustomerSerial;	//客户定制标识序号
public:
	virtual void InitCustomerSerial(UINT uiSerial);
public:
	CXhChar100 szAppTitle;			//程序主标题
#ifdef __LDS_CONTEXT_
	CXhChar100 szLicenseCopyright;	//授权与版权
#endif
	CXhChar50 get_szCustomerName();
	__declspec(property(get=get_szCustomerName)) CXhChar50 szCustomerName;
public:
	XHLOCALE();
	//自定义属性存储区
	CHashStrList<LOCALE_PROPITEM> hashLocaleProp;
	LOCALE_PROPITEM* AddLocaleItemLong(const char* keyidstr,long liPropVal,const char* name = NULL);
	LOCALE_PROPITEM* AddLocaleItemDouble(const char* keyidstr,double dfPropVal,const char* name = NULL);
	LOCALE_PROPITEM* AddLocaleItemBool(const char* keyidstr,bool blPropVal,const char* name=NULL);
	LOCALE_PROPITEM* AddLocaleItemStr100(const char* keyidstr, const char* pszPropVal, const char* name = NULL);
	LOCALE_PROPITEM* GetLocaleItem(const char* keyidstr){return hashLocaleProp.GetValue(keyidstr);}
};
