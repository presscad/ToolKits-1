// Variant.h: interface for the Variant class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VARIANT_H__F567840B_34DD_42E4_A5F9_989E254D7268__INCLUDED_)
#define AFX_VARIANT_H__F567840B_34DD_42E4_A5F9_989E254D7268__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
	//表格中所填充内容的数据类型
#ifndef _DEF_GRID_DATA_V_TYPE
#define _DEF_GRID_DATA_V_TYPE
enum GRID_DATA_TYPE{
	GRID_VT_EMPTY	= 0,	// 未指定
	GRID_VT_NULL	= 1,	// 空（暂保留）
	GRID_VT_I2		= 2,	// 2字节带符号整数
	GRID_VT_I4		= 3,	// 4字节带符号整数
	GRID_VT_R4		= 4,	// 4字节浮点数
	GRID_VT_R8		= 5,	// 8字节浮点数
	GRID_VT_DATE	= 6,	// 日期
	GRID_VT_STRING	= 7,	// 字符串
	GRID_VT_POINTER = 8,	// 指针
	GRID_VT_BOOL	= 9,	// 布尔数; TRUE=1, FALSE=0
	// 其余枚举常数在此无效
};
#endif
class CVariant{
public:
	long handle;
	GRID_DATA_TYPE vType;
	union{
		bool					bVal;     // GRID_VT_BOOL
		__int16					siVal;     // GRID_VT_I2
		__int32					lVal;     // GRID_VT_I4
		float					fltVal;   // GRID_VT_R4
		double					dblVal;   // GRID_VT_R8
		char*					sVal;     // GRID_VT_STRING
		void*					pVal;// GRID_VT_POINTER
	};
public:
	void Import(FILE* fp);
	void Export(FILE* fp);
	CVariant();
	CVariant(CVariant& SourceObj);
	~CVariant();
	CVariant& operator = (const CVariant &var);
	void ChangeToType(GRID_DATA_TYPE data_type);
	void ClearVariant();
	bool SetGridString(const char *string,bool bForceToString=true);
};

class CVariant2dArray  
{
public:
	VARIANT var;
	CVariant2dArray(VARIANT var);
	CVariant2dArray(int rows,int cols);
	virtual ~CVariant2dArray();
	bool GetValueAt(int i,int j,VARIANT& value);
	bool SetValueAt(int i,int j,VARIANT value);
	int RowsCount();
	int ColsCount();
	int ValidColsCount();
};

#endif // !defined(AFX_VARIANT_H__F567840B_34DD_42E4_A5F9_989E254D7268__INCLUDED_)
