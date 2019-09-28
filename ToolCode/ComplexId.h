#pragma once
#include "XhCharString.h"
#include "LogFile.h"

extern DWORD INT4_ARR[32];
struct TEMPL_NUMBER5 {static const int CONST_INT =  5;};
struct TEMPL_NUMBER16{static const int CONST_INT = 16;};

template<class TYPE> struct COMPLEXID{
private:
	long wholeId;
	long GroupMaskFlag() const{return (long)INT4_ARR[TYPE::CONST_INT-1];}
	long MaxGroupNo() const{return GroupMaskFlag()-1;}
	DWORD GroupMask() const{
		if(TYPE::CONST_INT==5)
			return 0x78000000;	//组号占5位
		else if(TYPE::CONST_INT==16)
			return 0x7FFF0000;	//组号占16位
		DWORD mask=0;
		for(int i=1;i<TYPE::CONST_INT;i++)
			mask|=INT4_ARR[31-i];
		return mask;
	}
	DWORD IdMask() const{
		if(TYPE::CONST_INT==5)
			return 0x07FFFFFF;	//组号占5位
		else if(TYPE::CONST_INT==16)
			return 0x0000FFFF;	//组号占16位
		DWORD mask=0;
		for(int i=0;i<32-TYPE::CONST_INT;i++)
			mask|=INT4_ARR[i];
		return mask;
	}
public:
	static const BYTE GROUP_DEFAULT	= 0;//默认模式
	static const BYTE GROUP_BOUNDARY= 1;//构件的外形边框
public:
	COMPLEXID(long id=0,long group=0){Set(group,id);}	//将组号与标识合并为组标识
	bool Set(long group=0,long id=0)
	{
		long validId=id;
		if(group!=0&&group!=GroupMaskFlag())//0x10)
		{	//组号不为零
			long validGroup=group|GroupMaskFlag();
			validId=(validGroup<<(32-TYPE::CONST_INT))|(id&IdMask());
		}
		wholeId=validId;
		if(group>=GroupMaskFlag())
		{
			logerr.Log("%d组号已超出标识分组极限%d，请核实!",group,GroupMaskFlag()-1);
			return false;
		}
		if(id>(long)IdMask())
		{
			logerr.Log("0x%X句柄号已超出标识句柄极限0x%X，请核实!",id,IdMask());
			return false;
		}
		else
			return true;
	}
	long Group() const
	{
		if((wholeId&0x80000000)==0)
			return 0;
		DWORD group=(wholeId&GroupMask());
		group=group>>(32-TYPE::CONST_INT);
		return (long)group;
	}
	long Id() const
	{
		if((wholeId&GroupMask())==0||(wholeId&0x80000000)==0)
			return wholeId&0x7fffffff;
		else
			return wholeId&IdMask();
	}
	long FromString(const char* subidStr)
	{
		if(subidStr==NULL)
			return false;
		char buf_str[65]={0};
		strncpy(buf_str,subidStr,64);
		char* subid=strchr(buf_str,'-');
		long group=0,id=0;
		if(subid!=NULL)
		{
			*subid=0;
			group=atoi(subidStr);
			id=atoi(subid+1);
		}
		else
			id=atoi(buf_str);
		Set(group,id);
		return wholeId;
	}
	//cJunction='@'时表示直接将组号转换为字母(1对应'A')
	CXhChar16 ToString(char cJunction='-',bool bIdOnAfter=true,int alignSize=0) const
	{
		long group=Group();
		long id=Id();
		CXhChar16 str("%d",id);
		if(group>0)
		{
			CXhChar16 groupStr;
			if(cJunction=='@')
				groupStr.Printf("%C",group+'@');
			else //if(cJunction!='@')	//if(cJunction=='-'||cJunction=='#')
				groupStr.Printf("%d",group);
			if(bIdOnAfter)
			{
				if(cJunction!=0&&cJunction!='@')
					str.InsertBefore(cJunction);
				str.InsertBefore(groupStr);
			}
			else
			{
				if(cJunction!=0&&cJunction!='@')
					str.Append(cJunction);
				str.Append(groupStr);
			}
		}
		if(alignSize>0)
			str.ResizeLength(alignSize,' ',true);
		return str;
	}
	operator DWORD(){return (DWORD)wholeId;}
	operator long(){return wholeId;}
	bool operator !=(long id){return wholeId!=id;}
};
typedef COMPLEXID<TEMPL_NUMBER5 > SUBID;	//组标识信息部分占5个比特位
typedef COMPLEXID<TEMPL_NUMBER16> SUBID16;	//组标识信息部分占16个比特位

union KEY2C{
private:
	char _label[2];	//首字母加下划线'_'是为了写代码时与Label()函数智能拼写混 wjh-2014.8.19
public:
	WORD wItem;
	operator WORD(){return wItem;}
	KEY2C(WORD wKey=0){wItem=wKey;}
	KEY2C(const char* keyStr){
		wItem=0;
		if(keyStr!=NULL)
		{
			for(size_t i=0;i<strlen(keyStr)&&i<2;i++)
				_label[i]=keyStr[i];
		}
	}
	char* Label();//返回指针是临时字符串指针
	bool IsEqual(KEY2C key){return wItem==key.wItem;}
};
union KEY4C{
private:
	char _label[4];	//首字母加下划线'_'是为了写代码时与Label()函数智能拼写混 wjh-2014.8.19
public:
	DWORD dwItem;
	struct{
		WORD wItem;	//key中前半部，2字节一般作为循环参数的主标识
		WORD index;	//key中后半部，2字节一般作为循环参数的索引标识
	}pair;
public:
	operator DWORD(){return dwItem;}
	KEY4C(DWORD dwKey=0){dwItem=dwKey;}
	KEY4C(WORD wItem,WORD wIndex);
	KEY4C(const char* keyStr);
	KEY4C(char kc1,char kc2,char kc3,char kc4=0){
		_label[0]=kc1;
		_label[1]=kc2;
		_label[2]=kc3;
		_label[3]=kc4;
	}
	char* Label();//返回指针是临时字符串指针, 到下一次调用KEY4C::Label()前有效
	char CharAt(int i){
		if(i<4)
			return _label[i];
		else
			return 0;
	}
	bool IsAlphabetLabel();//返回指针是临时字符串指针, 到下一次调用KEY4C::Label()前有效
	KEY4C ClonePrior(WORD index) const;	//克隆前半部(wItem)，后半部取值输入index值
	bool IsEqual(KEY4C key){return dwItem==key.dwItem;}
};
