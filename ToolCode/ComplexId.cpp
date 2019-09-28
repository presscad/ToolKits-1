#include "stdafx.h"
#include "ComplexId.h"
#include "LogFile.h"

DWORD INT4_ARR[32]={ 0X00000001,0X00000002,0X00000004,0X00000008,0X00000010,0X00000020,0X00000040,0X00000080,
					 0X00000100,0X00000200,0X00000400,0X00000800,0X00001000,0X00002000,0X00004000,0X00008000,
					 0X00010000,0X00020000,0X00040000,0X00080000,0X00100000,0X00200000,0X00400000,0X00800000,
					 0X01000000,0X02000000,0X04000000,0X08000000,0X10000000,0X20000000,0X40000000,0X80000000};
//////////////////////////////////////////////////////////////////////
// KEY2C,KEY4C Construction/Destruction
//////////////////////////////////////////////////////////////////////
static char _LocalKEY2C_pack_str[3];
char* KEY2C::Label()
{
	_LocalKEY2C_pack_str[0]=_label[0];
	_LocalKEY2C_pack_str[1]=_label[1];
	_LocalKEY2C_pack_str[2]=0;
	return _LocalKEY2C_pack_str;
}
static char pack_str[5];
char* KEY4C::Label()//返回指针是临时字符串指针
{
	strncpy(pack_str,_label,4);
	pack_str[4]=0;
	return pack_str;
}
bool KEY4C::IsAlphabetLabel()//返回指针是临时字符串指针, 到下一次调用KEY4C::Label()前有效
{
	for(int i=0;i<4;i++)
	{
		if(!isdigit(_label[i])&&!isalpha(_label[i])&&_label[i]!='-'&&_label[i]!='+'&&_label[i]!='_'&&_label[i]!='@'&&_label[i]!='#')
			return false;
	}
	return true;
}
KEY4C::KEY4C(const char* keyStr){
	dwItem=0;
	if(keyStr==NULL)
		return;
	else //if(keyStr!=NULL)
	{
		for(size_t i=0;i<strlen(keyStr)&&i<4;i++)
			_label[i]=keyStr[i];
	}
}
KEY4C::KEY4C(WORD wItem,WORD wIndex){
	pair.wItem=wItem;
	if(wIndex==0)	//0会与字符串结尾符'\0'混淆，故用0xffff代替
		pair.index=0xffff;
	else
		pair.index=wIndex;
}
//克隆前半部(wItem)，后半部取值输入index值
KEY4C KEY4C::ClonePrior(WORD index) const
{
	return KEY4C(pair.wItem,index);
}
