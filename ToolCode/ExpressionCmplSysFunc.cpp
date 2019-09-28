//<LOCALE_TRANSLATE BY hxr /> 2015-5-12
// Expression.cpp: implementation of the CExpression class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExpressionCmplSysFunc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CMPL_EXPRESSION_FUNC::CMPL_EXPRESSION_FUNC(UINT _id/*=0*/){
	id=_id;
	memset(name,0,9);
}
bool CMPL_EXPRESSION_FUNC::Calc(EXPRESSION_VALUE* pRsltValue,IXhArray<CMPL_EXPRESSION_WORD>* pExprParams,CMPL_EXPRESSION* pExpr)	//ÔËËã
{
	return false;
}
EXPR_SYSFUNC_ABS::EXPR_SYSFUNC_ABS(UINT _id/*=0*/)
{
	this->id=_id;//1;
	memcpy(name,"Abs",3);
}
bool EXPR_SYSFUNC_ABS::static_calc(EXPRESSION_VALUE* pRsltValue,IXhArray<CMPL_EXPRESSION_WORD>* pExprParams,CMPL_EXPRESSION* pExpr)	//ÔËËã
{	//ÔËËã
	CMPL_EXPRESSION_WORD& exprword=pExprParams->At(0);
	EXPRESSION_VALUE value;
	value.ciValType=exprword.value.ciValType;
	if(exprword.value.ciValType==0)
		value.fValue=exprword.value.fValue<0?-exprword.value.fValue:exprword.value.fValue;
	else if(exprword.value.ciValType==1)
		value.liValue=exprword.value.liValue<0?-exprword.value.liValue:exprword.value.liValue;
	else
		return false;
	if(pRsltValue)
		*pRsltValue=value;
	return true;
}
bool EXPR_SYSFUNC_ABS::Calc(EXPRESSION_VALUE* pRsltValue,IXhArray<CMPL_EXPRESSION_WORD>* pExprParams,CMPL_EXPRESSION* pExpr)
{
	return static_calc(pRsltValue,pExprParams,pExpr);
}
EXPR_SYSFUNC_ABS expr_sysfunc_abs(CExpressionSysFuncLibrary::SYSFUNC_ABS);
static CExpressionSysFuncLibrary _sys_expr_func;
CExpressionSysFuncLibrary::CExpressionSysFuncLibrary()
{
	functions[SYSFUNC_ABS]=&expr_sysfunc_abs;
	hashFuncsByLabel.SetValue("Abs",SYSFUNC_ABS);
}
CMPL_EXPRESSION_FUNC* CExpressionSysFuncLibrary::FromSysFunction(const char* func_label)
{
	UINT* pidSysFunc = _sys_expr_func.hashFuncsByLabel.GetValue(func_label);
	if(pidSysFunc!=NULL)
		return _sys_expr_func.functions[*pidSysFunc];
	else
		return NULL;
}
CMPL_EXPRESSION_FUNC* CExpressionSysFuncLibrary::FromSysFunction(UINT idFunc)
{
	if(idFunc<=SYSFUNC_INTERS)
		return _sys_expr_func.functions[idFunc];
	else
		return NULL;
}
