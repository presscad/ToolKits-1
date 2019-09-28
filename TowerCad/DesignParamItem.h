#pragma once
#include "ComplexId.h"
#include "XhCharString.h"

struct CATEGORY{
	long id;
	long idUpper;
	CXhChar16 name;
	CATEGORY(){id=idUpper=0;}
};
struct DESIGN_PARAM_ITEM
{
	KEY4C key;			//该参数的标识键值（或不超过4个长的字符串）
	UINT uiCategoryId;	//首位为1即uiCategoryId&0x80000000>0表示该参数本身即为目录项
	CXhChar50 sLabel;	//属性栏中显示的短名称标签
	CXhChar100 sNotes;	//界面中对该项属性的解释性描述
	union{
		double fValue;
		long hPart;
		long iType;
		struct{	//数组变量
			UINT uiUpperBound;	//数组变量的索引上限(最大值, 0 based index)
			double* arrValues;	//数组变量时，指向数组的索引（可能指向临时内存）
		};
	}value;
	BYTE dataType;	//0.double数值 1.句柄 2. 整数 3. 浮点数组
	BOOL bModify;
	void SetKey(DWORD hashkey){key=hashkey;}
	DESIGN_PARAM_ITEM(){uiCategoryId=dataType=0; bModify=FALSE;}
public:	//dataType的数据类型
	static const BYTE TYPE_DOUBLE	= 0;
	static const BYTE TYPE_HANDLE	= 1;
	static const BYTE TYPE_INTEGER	= 2;
	static const BYTE TYPE_DOUBLE_ARR	= 3;
};
//参数示意关联项
struct PARA_SKETCH_ITEM
{
	KEY4C key;
	BYTE m_ciSketchVec;	//1.X方向|2.Y方向|3.Z方向
	long m_hNode1;
	long m_hNode2;
	//
	void SetKey(DWORD hashkey){key=hashkey;}
	PARA_SKETCH_ITEM(){m_ciSketchVec=1,m_hNode1=m_hNode2=0;}
};
