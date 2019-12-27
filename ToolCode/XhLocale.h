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
	CXhChar100 idKeyStr;	//��ʶ�ַ���
	CXhChar100 name;
	char ciValType;	//0.bool;1.long;2.double;3.���ַ���
	union{
		bool bVal;
		long liVal;
		double dfVal;
		char szStrBuff[101];
	};
};
#endif
struct XHLOCALE{
	static const BYTE CID_Xerofox			=	1;	//id=1.�ź����
	static const BYTE CID_NanTong_HuaCan	=	2;	//id=2.��ʾ������ͨ����
	static const BYTE CID_QingDao_WuXiao	=	3;	//id=3.�ൺ����
	static const BYTE CID_ChongQing_ShunTai	=	4;	//id=4.����˳̩
	static const BYTE CID_QingDao_DongFang	=	5;	//id=5.�ൺ����
	static const BYTE CID_GuangZhou_ZengLi	=	6;	//id=6.��������
	static const BYTE CID_AnHui_HongYuan	=	7;	//id=7.���պ�Դ
	static const BYTE CID_YunNan_JingYunXiang=	8;	//id=8.���Ͼ�����
	static const BYTE CID_ChongQing_JiangDian=	9;	//id=9.���콭��
	static const BYTE CID_QingDao_HuiJinTong=	10;	//id=10.�ൺ���ͨ
	static const BYTE CID_ChangChun_JuDeLong=	11;	//id=11.�����۵�¡
	static const BYTE CID_ShanXi_BaoJi		=	12;	//id=12.������
	static const BYTE CID_YunNan_JianYuan	=	13;	//id=13.���Ͻ�Դ
	static const BYTE CID_YunNan_DongDian	=	14;	//id=14.���϶���
	static const BYTE CID_YunNan_ZhanYe		=	15;	//id=15.����չҵ
	static const BYTE CID_XiNanYuanPower	=	16;	//id=16.���ϵ������Ժ
protected:
	BYTE _ciCustomerSerial;	//�ͻ����Ʊ�ʶ���
public:
	BYTE get_ciCustomerSerial(){return _ciCustomerSerial;}
	__declspec(property(get=get_ciCustomerSerial)) BYTE ciCustomerSerial;	//�ͻ����Ʊ�ʶ���
public:
	virtual void InitCustomerSerial(UINT uiSerial);
public:
	CXhChar100 szAppTitle;			//����������
#ifdef __LDS_CONTEXT_
	CXhChar100 szLicenseCopyright;	//��Ȩ���Ȩ
#endif
	CXhChar50 get_szCustomerName();
	__declspec(property(get=get_szCustomerName)) CXhChar50 szCustomerName;
public:
	XHLOCALE();
	//�Զ������Դ洢��
	CHashStrList<LOCALE_PROPITEM> hashLocaleProp;
	LOCALE_PROPITEM* AddLocaleItemLong(const char* keyidstr,long liPropVal,const char* name = NULL);
	LOCALE_PROPITEM* AddLocaleItemDouble(const char* keyidstr,double dfPropVal,const char* name = NULL);
	LOCALE_PROPITEM* AddLocaleItemBool(const char* keyidstr,bool blPropVal,const char* name=NULL);
	LOCALE_PROPITEM* AddLocaleItemStr100(const char* keyidstr, const char* pszPropVal, const char* name = NULL);
	LOCALE_PROPITEM* GetLocaleItem(const char* keyidstr){return hashLocaleProp.GetValue(keyidstr);}
};
