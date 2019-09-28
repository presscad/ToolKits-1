// ExpressionCmpl.h: interface for the CExpression class.
//
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ArrayList.h"
#include "Hashtable.h"
#include "ExpressionCmpl.h"

class EXPR_SYSFUNC_ABS : public CMPL_EXPRESSION_FUNC{
public:
	EXPR_SYSFUNC_ABS(UINT _id=0);
	static bool static_calc(EXPRESSION_VALUE* pRsltValue,IXhArray<CMPL_EXPRESSION_WORD>* pExprParams,CMPL_EXPRESSION* pExpr);	//‘ÀÀ„
	virtual bool Calc(EXPRESSION_VALUE* pRsltValue,IXhArray<CMPL_EXPRESSION_WORD>* pExprParams,CMPL_EXPRESSION* pExpr);	//‘ÀÀ„
};
class CExpressionSysFuncLibrary{
public:
	static const WORD SYSFUNC_ABS		= 1;
	static const WORD SYSFUNC_INTERS	= 2;
	static const UINT MAX_SYSFUNCID		= 999;
public:
	EXPRESSION_FUNCPTR functions[2];
	CHashStrList<UINT> hashFuncsByLabel;
	CExpressionSysFuncLibrary();
	static CMPL_EXPRESSION_FUNC* FromSysFunction(const char* func_label);
	static CMPL_EXPRESSION_FUNC* FromSysFunction(UINT idFunc);
};
