// Expression.h: interface for the CExpression class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPRESSION_H__01230132_F1FB_485D_9A6C_B0BFAAB4C1CA__INCLUDED_)
#define AFX_EXPRESSION_H__01230132_F1FB_485D_9A6C_B0BFAAB4C1CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "list.h"
#include "ExpressionValue.h"

//－－－－－－－－－－四则运算表达式求值类定义－－－－－－－－
struct EXPRESSION_VAR
{
	union{
		double fValue;			//变量数值
		struct{	//数组变量
			UINT uiUpperBound;	//数组变量的索引上限(最大值, 0 based index)
			double* arrValues;	//数组变量时，指向数组的索引（可能指向临时内存）
		};
	};
	char variableStr[9];	//变量名称
	EXPRESSION_VAR(){memset(this,0,sizeof(EXPRESSION_VAR));}
};
class CExpression;
struct EXPRESSION_FUNC{
protected:
	UINT id;
public:
	char name[19];
	EXPRESSION_FUNC(UINT _id=0);
	UINT get_Id(){return id;}
	__declspec(property(get=get_Id)) UINT Id;
	virtual long GetRsltValType();
	virtual bool Calc(EXPRESSION_VALUE* pRsltValue,const char* param_str,CExpression* pContextExpr);	//运算
};
/*
	求解表达式值的类。
	此类暂只进行加(+)减(-)乘(*)除(/(四种浮点运算。
	运算之前需要先把表达式中引用的变量对象添加到varList中，然后把表达式以字符串
	参数形式输入到SolveExpression(...)中，最终返回双精度型计算结果。
	计算过程中如果遇到不合理的表达式，系统会抛出char* 类型字符串异常
	内置平直线求交函数
	INT_X(line1_startx,line1_starty,line1_endx,line1_endy,
	      line2_start_x,line2_start_y,line2_end_x,line2_end_y)	返回line1^line2交点的X坐标分量
	INT_Y(line1_startx,line1_starty,line1_endx,line1_endy,
		  line2_start_x,line2_start_y,line2_end_x,line2_end_y)	返回line1^line2交点的Y坐标分量
*/
class CExpression  
{
protected:
	virtual EXPRESSION_VAR * FindExpressionVariable(char* var);
	virtual EXPRESSION_FUNC* FromFunctionName(const char* func_label);

	double Int2dLine(char* paraStr,int x0_y1);
public:
	CExpression();
	virtual ~CExpression();
	CXhSimpleList<EXPRESSION_VAR> varList;
	EXPRESSION_VAR* VarAt(int index);
	static void ToExpression(double f,int precision,char* destStr,int maxCount);
	double SolveExpression(const char* expression);
	bool SolveLogicalExpression(const char* expression);//目前该表达式中只能出现一次'<'、'>'、'='、'<='、'>='、'!='逻辑操作符
public:	//逻辑运算符
	static const int OPER_TYPE_GREATER		= 1;	//>
	static const int OPER_TYPE_LESS			= 2;	//<
	static const int OPER_TYPE_EQUAL		= 3;	//=
	static const int OPER_TYPE_NOTEQUAL		= 4;	//!=
	static const int OPER_TYPE_NOTGREATER	= 5;	//<=
	static const int OPER_TYPE_NOTLESS		= 6;	//>=
};

#endif // !defined(AFX_EXPRESSION_H__01230132_F1FB_485D_9A6C_B0BFAAB4C1CA__INCLUDED_)
