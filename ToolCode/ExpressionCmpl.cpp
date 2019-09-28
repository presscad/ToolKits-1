// ExpressionCmpl.cpp: implementation of the CExpression class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include <stdlib.h>
#include "list.h"
#include "XhCharString.h"
#include "ExpressionCmpl.h"
//#include "f_alg_fun.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

EXPRESSION_OPERATION::EXPRESSION_OPERATION()
{
	m_cnExprParamValCount=0;
	memset(m_arrExprVal,0,sizeof(UINT)*8);
}
UINT EXPRESSION_OPERATION::SetExprVal(int index,UINT uiVal)
{
	if(index>=8)
		return 0;	//超限
	if(index+1>m_cnExprParamValCount)
		m_cnExprParamValCount=index+1;
	m_arrExprVal[index]=uiVal;
	return uiVal;
}
bool EXPRESSION_OPERATION::Calc(CMPL_EXPRESSION* pExprContext,EXPRESSION_VALUE *pRsltValue)
{
	if(this->ciType==0)
		return true;
	else if(this->ciType==1)
	{	//四则运算
		if(pRsltValue->ciValType<=1)
		{
			CMPL_EXPRESSION_WORD* pLeftWord =pExprContext->FromExprWord(idLeftExprVal);
			CMPL_EXPRESSION_WORD* pRightWord=pExprContext->FromExprWord(idRightExprVal);
			pLeftWord->Calc(pExprContext);
			pRightWord->Calc(pExprContext);
			if(ciOperator=='+')
			{
				if(pRsltValue->ciValType==0)
					pRsltValue->fValue=pLeftWord->value.fValue+pRightWord->value.fValue;
				else
					pRsltValue->fValue=pLeftWord->value.liValue+pRightWord->value.liValue;
			}
			else if(ciOperator=='-')
			{
				if(pRsltValue->ciValType==0)
					pRsltValue->fValue=pLeftWord->value.fValue-pRightWord->value.fValue;
				else
					pRsltValue->fValue=pLeftWord->value.liValue-pRightWord->value.liValue;
			}
			else if(ciOperator=='*')
			{
				if(pRsltValue->ciValType==0)
					pRsltValue->fValue=pLeftWord->value.fValue*pRightWord->value.fValue;
				else
					pRsltValue->fValue=pLeftWord->value.liValue*pRightWord->value.liValue;
			}
			else if(ciOperator=='/')
			{
				if(pRsltValue->ciValType==0)
					pRsltValue->fValue=pLeftWord->value.fValue/pRightWord->value.fValue;
				else
					pRsltValue->fValue=pLeftWord->value.liValue/pRightWord->value.liValue;
			}
		}
	}
	else if(this->ciType==2)
	{	//2.逻辑运算符;
	}
	else if(this->ciType==3)
	{	//3.函数运算
	}
	else if(this->ciType==4)
	{	//4.数组元素取值
	}
	//else
		return false;
}
CMPL_EXPRESSION_WORD* CMPL_EXPRESSION::AppendExprWord(BYTE ciValType/*=0*/,BYTE ciOperType/*=0*/)
{
	CMPL_EXPRESSION_WORD word(ciValType,ciOperType);
	CMPL_EXPRESSION_WORD *pWord=words.Append(word);
	pWord->id=m_uiMinExprWordId;
	m_uiMinExprWordId++;
	return pWord;
}
CMPL_EXPRESSION_WORD* CMPL_EXPRESSION::AppendExprWord(CMPL_EXPRESSION_WORD xExprWord)
{
	CMPL_EXPRESSION_WORD *pWord;
	if(xExprWord.Id==0)
	{
		pWord=words.Append(xExprWord);
		pWord->id=m_uiMinExprWordId;
		m_uiMinExprWordId++;
	}
	else if(xExprWord.Id<m_uiMinExprWordId)
		return SetExprWord(xExprWord.Id,xExprWord);
	else // if(xExprWord.Id>=m_uiMinExprWordId)
	{
		for(UINT indexId=m_uiMinExprWordId;indexId<=xExprWord.Id;indexId++)
		{
			pWord=words.Append(xExprWord);
			pWord->id=indexId;
			if(indexId<xExprWord.Id)	//清空计算过程
				pWord->calcproc.ciType=0;
			m_uiMinExprWordId=indexId+1;
		}
	}
	return pWord;
}
static const BYTE WORDTYPE_OPERATOR		= 0;
static const BYTE WORDTYPE_CONST_FLOAT	= 1;
static const BYTE WORDTYPE_CONST_INTEGER= 3;
static const BYTE WORDTYPE_VARIABLE		= 2;
static const BYTE WORDTYPE_CMPLWORD		= 4;
CMPL_EXPRESSION_WORD* CMPL_EXPRESSION::AppendExprWord(INTERPRET_WORD* pInterpretWord)
{
	CMPL_EXPRESSION_WORD* pExprWord=NULL;
	if(pInterpretWord->word_type==WORDTYPE_CONST_FLOAT)//1)
	{
		pExprWord=AppendExprWord(EXPRESSION_VALUE::VALUETYPE_DOUBLE,EXPRESSION_OPERATION::OPER_SELFVALUE);
		pExprWord->value.fValue=pInterpretWord->value.fConst;
	}
	else if(pInterpretWord->word_type==WORDTYPE_CONST_INTEGER)
	{
		pExprWord=AppendExprWord(EXPRESSION_VALUE::VALUETYPE_LONG,EXPRESSION_OPERATION::OPER_SELFVALUE);
		pExprWord->value.liValue=pInterpretWord->value.liConst;
	}
	else if(pInterpretWord->word_type==WORDTYPE_VARIABLE)
	{
		pExprWord=AppendExprWord(0,EXPRESSION_OPERATION::OPER_SELFVALUE);
		pExprWord->value=pInterpretWord->value.pExprWord->value;
	}
	else if(pInterpretWord->word_type==WORDTYPE_CMPLWORD)
	{
		pExprWord=AppendExprWord(*pInterpretWord->value.pExprWord);
	}
	return pExprWord; 
}
bool CMPL_EXPRESSION::UpdateExprByInterpretWord(INTERPRET_WORD* pInterpretWord)
{
	if(pInterpretWord->word_type==WORDTYPE_CONST_FLOAT)//1)
	{
		value.ciValType=EXPRESSION_VALUE::VALUETYPE_DOUBLE;
		value.fValue=pInterpretWord->value.fConst;
		calcproc.ciType=EXPRESSION_OPERATION::OPER_SELFVALUE;
		return true; 
	}
	else if(pInterpretWord->word_type==WORDTYPE_CONST_INTEGER)
	{
		value.ciValType=EXPRESSION_VALUE::VALUETYPE_LONG;
		value.liValue=pInterpretWord->value.liConst;
		calcproc.ciType=EXPRESSION_OPERATION::OPER_SELFVALUE;
		return true; 
	}
	else if(pInterpretWord->word_type==WORDTYPE_VARIABLE)
	{
		value=pInterpretWord->value.pExprWord->value;
		calcproc.ciType=EXPRESSION_OPERATION::OPER_SELFVALUE;
		return true;
	}
	else if(pInterpretWord->word_type==WORDTYPE_CMPLWORD)
	{
		value=pInterpretWord->value.pExprWord->value;
		calcproc=pInterpretWord->value.pExprWord->calcproc;
		return true;
	}
	else
		return false;
}
#include "ExpressionCmplSysFunc.h"
CMPL_EXPRESSION_FUNC* CMPL_EXPRESSION::FromSysFunction(const char* func_label)
{
	return CExpressionSysFuncLibrary::FromSysFunction(func_label);
}
CMPL_EXPRESSION_FUNC* CMPL_EXPRESSION::FromSysFunction(UINT idFunc)
{
	return CExpressionSysFuncLibrary::FromSysFunction(idFunc);
}
CMPL_EXPRESSION_OBJ* CMPL_EXPRESSION::FromExpressionObj(int idObjType){
	if(m_pReflectExpr!=NULL)
		return m_pReflectExpr->FromExpressionObj(idObjType);
	return NULL;
}
UINT CMPL_EXPRESSION::FromFunctionName(const char* func_label)
{
	if(m_pReflectExpr!=NULL)
		return m_pReflectExpr->FromFunctionName(func_label);
	CMPL_EXPRESSION_FUNC* pSysFunc=FromSysFunction(func_label);
	if(pSysFunc!=NULL)
		return pSysFunc->Id;
	UINT *pidFunc=localefunctions.hashFuncsByLabel.GetValue(func_label);
	if(pidFunc!=NULL)
		return (pidFunc!=NULL)?*pidFunc:0;
	else
		return 0;
}
CMPL_EXPRESSION_FUNC* CMPL_EXPRESSION::FromFunction(UINT idFunc)
{
	if(m_pReflectExpr!=NULL)
		return m_pReflectExpr->FromFunction(idFunc);
	return NULL;
}
CMPL_EXPRESSION::CMPL_EXPRESSION(CMPL_EXPRESSION* pReflectExpr/*=NULL*/,UINT uidMinExprWord/*=1*/)
{
	m_pReflectExpr=pReflectExpr;
	m_uiMinExprWordId=m_uiBaseIndexExprWord=uidMinExprWord;
	words.ZeroPresetMemory();
}

CMPL_EXPRESSION_WORD* CMPL_EXPRESSION::SetExprWord(UINT idExprWord,CMPL_EXPRESSION_WORD xExprWord)
{
	CMPL_EXPRESSION_WORD* pExprWord=NULL;
	if(idExprWord-m_uiBaseIndexExprWord<words.Count)
	{
		pExprWord=words.GetAt(idExprWord-m_uiBaseIndexExprWord);
		pExprWord->value=xExprWord.value;
		pExprWord->calcproc=xExprWord.calcproc;
		memcpy(pExprWord->name,xExprWord.name,9);
		if(pExprWord->Id!=idExprWord)
			pExprWord->id=idExprWord;
		//TODO:检查calcproc.xExprValArr是否超出4个
	}
	else
	{
		int index=words.Set(idExprWord-m_uiBaseIndexExprWord,xExprWord);
		pExprWord=words.GetAt(index);
		if(pExprWord->Id!=idExprWord)
			pExprWord->id=idExprWord;
		m_uiMinExprWordId=max(m_uiMinExprWordId,idExprWord);
	}
	return pExprWord;
}
CMPL_EXPRESSION_WORD* CMPL_EXPRESSION::FromExprWord(UINT idExprWord)
{
	if(idExprWord>=words.Count+m_uiBaseIndexExprWord)
		return NULL;
	return words.GetAt(idExprWord-m_uiBaseIndexExprWord);
}
CMPL_EXPRESSION_WORD* CMPL_EXPRESSION::FindNamedExprWord(const char* varname)
{
	if(m_pReflectExpr!=NULL)
		return m_pReflectExpr->FindNamedExprWord(varname);
	UINT *puidVar=this->hashNamedWordIds.GetValue(varname);
	if(puidVar==NULL)
		return NULL;
	return FromExprWord(*puidVar);
}
bool LogicalJustify(double leftValue,int operType,double rightValue);	//defined in Expression.cpp
bool CMPL_EXPRESSION::CompileLogicExprStr(const char* exprstr)
{
	char native_expr[201]={0};
	strncpy(native_expr,exprstr,200);
	int operType=0;
	char* forePart=native_expr;
	char* hindPart=NULL;
	if((hindPart=strstr(native_expr,">="))!=NULL)
	{
		operType=OPERATOR_LOGIC_EnGT;//OPER_TYPE_NOTLESS;
		memset(hindPart,0,2);
		hindPart+=2;
	}
	else if((hindPart=strstr(native_expr,"<="))!=NULL)
	{
		operType=OPERATOR_LOGIC_EnLT;//OPER_TYPE_NOTGREATER;
		memset(hindPart,0,2);
		hindPart+=2;
	}
	else if((hindPart=strstr(native_expr,"!="))!=NULL)
	{
		operType=OPERATOR_LOGIC_NEQUAL;//OPER_TYPE_NOTEQUAL;
		memset(hindPart,0,2);
		hindPart+=2;
	}
	else if((hindPart=strchr(native_expr,'>'))!=NULL)
	{
		operType=OPERATOR_LOGIC_GT;//OPER_TYPE_GREATER;
		*hindPart=0;
		hindPart++;
	}
	else if((hindPart=strchr(native_expr,'<'))!=NULL)
	{
		operType=OPERATOR_LOGIC_LT;//OPER_TYPE_LESS;
		*hindPart=0;
		hindPart++;
	}
	else if((hindPart=strchr(native_expr,'='))!=NULL)
	{
		operType=OPERATOR_LOGIC_EQUAL;//OPER_TYPE_EQUAL;
		*hindPart=0;
		hindPart++;
	}
	else	//不支持的逻辑表达式
		return false;

	double foreValue=CompileExprStr(forePart);	//this->SolveExpression(forePart);
	double hindValue=CompileExprStr(hindPart);	//SolveExpression(hindPart);
	return LogicalJustify(foreValue,operType,hindValue);
}
static bool IsNumeric(char ch){return (ch>='0'&&ch<='9')||ch=='.';}
static bool IsAlphabet(char ch){return (ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||ch=='_';}
bool IsOperator(char ch);	//defined in Expression.cpp
int CompareOperatorPriority(char chOper1,char chOper2);	//defined in Expression.cpp
bool CMPL_EXPRESSION::InterpretBeforeOperWord(IStack<INTERPRET_WORD> *pWordStack, char currentOperator)
{
	PRESET_STACK32<INTERPRET_WORD> temStack;
	INTERPRET_WORD before,oper,after,w;
	while(pWordStack->GetPushSize()>0)
	{
		pWordStack->pop(oper);
		if(oper.word_type!=0)
			temStack.push(oper);
		else
		{
			if(CompareOperatorPriority(oper.value.chOperator,currentOperator)>=0)
			{	//当前操作符优先级低于前一操作符优先级，原堆栈应弹出一组运算符和运算量予以运算
				if(oper.value.chOperator=='('&&currentOperator==')')
				{
					temStack.pop(oper);
					pWordStack->push(oper);
					return false;
				}
				else if(pWordStack->GetPushSize()>0&&temStack.GetPushSize()>0)
				{
					pWordStack->pop(before);
					temStack.pop(after);
					w=CompileOneOperation(before,oper.value.chOperator,after);
					pWordStack->push(w);
					return true;
				}
			}
			else
			{	//当前操作符优先级高于前一操作符优先级，原堆栈应保持不变，予以恢复
				pWordStack->push(oper);
				while(temStack.GetPushSize()>0)
				{
					temStack.pop(oper);
					pWordStack->push(oper);
				}
				return false;
			}
		}
	}
	while(temStack.GetPushSize()>0)
	{
		temStack.pop(oper);
		pWordStack->push(oper);
	}
	return false;
}
INTERPRET_WORD CMPL_EXPRESSION::CompileOneOperation(INTERPRET_WORD before,char chOperator,INTERPRET_WORD after)
{
	if(before.word_type!=WORDTYPE_OPERATOR&&after.word_type!=WORDTYPE_OPERATOR)
	{
		double before_value,after_value;
		INTERPRET_WORD w;
		if(before.word_type!=WORDTYPE_CONST_FLOAT||after.word_type!=WORDTYPE_CONST_FLOAT)
		{
			CMPL_EXPRESSION_WORD* pExprWord=this->AppendExprWord(0,EXPRESSION_OPERATION::OPER_CALC_BASIC);
			pExprWord->calcproc.ciOperator=chOperator;
			CMPL_EXPRESSION_WORD* pLeftExprWord=NULL,*pRightExprWord=NULL;
			if(before.word_type==WORDTYPE_CMPLWORD)
				pLeftExprWord=before.value.pExprWord;
			else
			{
				pLeftExprWord=this->AppendExprWord(0,EXPRESSION_OPERATION::OPER_CALC_BASIC);
				if(before.word_type==WORDTYPE_CONST_FLOAT)
				{
					pLeftExprWord->value.ciValType=0;
					pLeftExprWord->value.fValue=before.value.fConst;
				}
				else if(before.word_type==WORDTYPE_CONST_INTEGER)
				{
					pLeftExprWord->value.ciValType=EXPRESSION_VALUE::VALUETYPE_LONG;
					pLeftExprWord->value.liValue=before.value.liConst;
				}
				//else
				//	logerr.Log("error");
			}
			if(after.word_type==WORDTYPE_CMPLWORD)
				pRightExprWord=after.value.pExprWord;
			else
			{
				pRightExprWord=this->AppendExprWord(0,EXPRESSION_OPERATION::OPER_CALC_BASIC);
				if(after.word_type==WORDTYPE_CONST_FLOAT)
				{
					pRightExprWord->value.ciValType=0;
					pRightExprWord->value.fValue=after.value.fConst;
				}
				else if(after.word_type==WORDTYPE_CONST_INTEGER)
				{
					pRightExprWord->value.ciValType=EXPRESSION_VALUE::VALUETYPE_LONG;
					pRightExprWord->value.liValue=after.value.liConst;
				}
				//else
				//	logerr.Log("error");
			}
			pExprWord->calcproc.idLeftExprVal =pLeftExprWord->Id;
			pExprWord->calcproc.idRightExprVal=pRightExprWord->Id;
			w.word_type=WORDTYPE_CMPLWORD;
			w.value.pExprWord=pExprWord;
			return w;
		}
		else
		{
			if(before.word_type==WORDTYPE_CONST_FLOAT)
				before_value=before.value.fConst;
			else
				before_value=before.value.pExprWord->value.fValue;//pVar->fValue;
			if(after.word_type==WORDTYPE_CONST_FLOAT)
				after_value=after.value.fConst;
			else
				after_value=after.value.pExprWord->value.fValue;//pVar->fValue;
			w.word_type=WORDTYPE_CONST_FLOAT;	//1
			if(chOperator=='+')
				w.value.fConst=before_value+after_value;
			else if(chOperator=='-')
				w.value.fConst=before_value-after_value;
			else if(chOperator=='*')
				w.value.fConst=before_value*after_value;
			else if(chOperator=='/')
				w.value.fConst=before_value/after_value;
			else
#ifdef	AFX_TARG_ENU_ENGLISH
				throw "Error expression format";
#else
				throw "表达式格式错误";
#endif
			return w;
		}
	}
#ifdef	AFX_TARG_ENU_ENGLISH
	throw "Error expression format";
#else
	throw "表达式格式错误";
#endif
}
bool CMPL_EXPRESSION::CompileExprStr(const char* expression,BYTE ciExprValType/*=0xff*/)
{
	if(ciExprValType<3)
		value.ciValType=ciExprValType;
	if(ciExprValType==2)	//逻辑表达式
		return CompileLogicExprStr(expression);

	PRESET_STACK32<INTERPRET_WORD>wordStack;
	int i,j=0,strLen=strlen(expression);
	if(strLen==0)
	{
		calcproc.ciType=0;
		return 0.0;
	}
	//扫描表达式，并进行换码转换为单词链表
	INTERPRET_WORD w;
	CMPL_EXPRESSION_WORD* pExprWord;
	bool wordBegin=false,funcBegin=false;
	CXhSimpleList<INTERPRET_WORD>listWords;
	for(i=0;i<=strLen;i++)
	{
		if(i<strLen&&expression[i]==' ')
			continue;
		else if(!wordBegin&&i<strLen)
		{	//目前并不是一个单词的开始状态
			j=0;
			if(IsOperator(expression[i]))
			{	//操作符
				w.word_type=WORDTYPE_OPERATOR;
				w.value.chOperator=expression[i];
				listWords.Append(w);
				continue;
			}
			//一个单词（操作常数或操作变量）的开始
			wordBegin=true;
			w.value.SafeSetChar(j,expression[i]);
			if(IsNumeric(expression[i]))
				w.word_type=WORDTYPE_CONST_FLOAT;	//操作数常量
			else if(IsAlphabet(expression[i]))
				w.word_type=WORDTYPE_VARIABLE;	//操作数变量//、函数或系统关键词
			else
#ifdef	AFX_TARG_ENU_ENGLISH
				throw "Error expression";
#else
				throw "表达式错误！";
#endif
		}
		else
		{
			j++;
			if(w.word_type==WORDTYPE_CONST_FLOAT&&IsNumeric(expression[i]))
				w.value.SafeSetChar(j,expression[i]);	//一个操作常数的继续
			else if(w.word_type==WORDTYPE_VARIABLE&&(IsNumeric(expression[i])||IsAlphabet(expression[i])))
				w.value.SafeSetChar(j,expression[i]);	//一个操作变量的继续
			else if(w.word_type==WORDTYPE_CONST_FLOAT&&IsAlphabet(expression[i]))
#ifdef	AFX_TARG_ENU_ENGLISH
				throw "Contains letter in operating constant,error expression!";
#else
				throw "操作常数中含有字母，表达式错误！";
#endif
			else if(IsOperator(expression[i])||i==strLen)
			{	//一个单词需要结束
				wordBegin=false;	//词结束
				if(i<strLen&&expression[i]=='#')	//单整数常量为参数的函数开始
				{
					char paraStr[100]="";
					i++;
					for(int jj=0;jj<100&&i+jj+1<strLen;jj++)
					{
						if(expression[i+jj]>='0'&&expression[i+jj]<='9')
							paraStr[jj]=expression[i+jj];
						else
						{
							i+=jj;
							break;
						}
					}
					w.value.SafeSetChar(j,'\0');
					CMPL_EXPRESSION_WORD* pFuncCmplWord=this->AppendExprWord(EXPRESSION_VALUE::VALUETYPE_OBJPROP,EXPRESSION_OPERATION::OPER_CALC_FUNCION);
					pFuncCmplWord->calcproc.idFunction=this->FromFunctionName(w.value.valueStr);
					w.word_type=WORDTYPE_CMPLWORD;
					w.value.pExprWord=pFuncCmplWord;
					listWords.Append(w);
					CMPL_EXPRESSION_WORD* pIndexWord=this->AppendExprWord(EXPRESSION_VALUE::VALUETYPE_LONG);
					pIndexWord->value.liValue=atoi(paraStr);
					pFuncCmplWord->calcproc.idLeftExprVal=pIndexWord->Id;
				}
				else if(i<strLen&&(expression[i]=='('||expression[i]=='['))	//函数或索引开始
				{
					char paraStr[100]="";
					char cLeftBracketType=expression[i];
					char cRightBracketType=cLeftBracketType=='(' ? ')' : ']';
					int leftBracketHits=1;
					for(int jj=0;jj<100&&i+jj+1<strLen;jj++)
					{
						if(expression[i+jj+1]==cLeftBracketType)//'(')
							leftBracketHits++;
						if(expression[i+jj+1]==cRightBracketType)//')')
							leftBracketHits--;
						if(leftBracketHits>0)
							paraStr[jj]=expression[i+jj+1];
						else
						{
							i+=jj+2;
							paraStr[jj]='\0';
							break;
						}
					}
					if(leftBracketHits>0)
#ifdef	AFX_TARG_ENU_ENGLISH
						throw "The reference function in expression didn't close,error expression!";
#else
						throw "表达式中引用的函数未封闭，表达式错误！";
#endif
					w.value.SafeSetChar(j,'\0');
					UINT idFunction=0;
					if(cLeftBracketType=='[')
					{
						pExprWord=FindNamedExprWord(w.value.valueStr);//
						//UINT uiIndex=(UINT)SolveExpression(paraStr);
						//EXPRESSION_VAR *pIndexVar=FindExpressionVariable(w.value.valueStr);
						CMPL_EXPRESSION_WORD* pIndexWord=CompileToExprWord(paraStr,this,NULL);
						if(pIndexWord==NULL)
							throw "表达式中含有未定义的数组变量";
						//else if(pIndexVar->uiUpperBound<uiIndex)
						//	throw "表达式中数组变量超出索引上限";
						//w.value.fConst=pIndexVar->arrValues[uiIndex];
						CMPL_EXPRESSION_WORD* pArrElemExprWord=this->AppendExprWord(EXPRESSION_VALUE::VALUETYPE_DOUBLE,EXPRESSION_OPERATION::OPER_CALC_ARRELEM);
						pArrElemExprWord->calcproc.idLeftExprVal =pExprWord->Id;
						pArrElemExprWord->calcproc.idRightExprVal=pIndexWord->Id;
						w.word_type=WORDTYPE_CMPLWORD;
						w.value.pExprWord=pArrElemExprWord;
						listWords.Append(w);
					}
					else if((idFunction=FromFunctionName(w.value.valueStr))>0)
					{
						CMPL_EXPRESSION_FUNC* pExprFunc=FromFunction(idFunction);
						CMPL_EXPRESSION_WORD* pFuncCmplWord=this->AppendExprWord(0,EXPRESSION_OPERATION::OPER_CALC_FUNCION);
						pFuncCmplWord->calcproc.idFunction=idFunction;
						CXhSimpleList<CMPL_EXPRESSION_WORD*> listParamWords;
						CMPL_EXPRESSION_WORD* pWord=CompileToExprWord(paraStr,this,&listParamWords);
						if(pWord!=NULL)
						{
							int index=0;
							for(CMPL_EXPRESSION_WORDPTR* ppExprWord=listParamWords.EnumObjectFirst();ppExprWord;
								ppExprWord=listParamWords.EnumObjectNext(),index++)
							{
								pExprWord=AppendExprWord(*(*ppExprWord));
								pFuncCmplWord->calcproc.SetExprVal(index,pExprWord->Id);	//目前暂仅支持单参数函数调用
							}
						}
						w.word_type=WORDTYPE_CMPLWORD;
						w.value.pExprWord=pFuncCmplWord;
						long liRsltValType=pExprFunc->GetRsltValType();
						if(liRsltValType/100!=4)
							pFuncCmplWord->value.ciValType=liRsltValType%100;
						else if(expression[i]=='.')
						{	//对象属性
							pFuncCmplWord->value.ciValType=EXPRESSION_VALUE::VALUETYPE_OBJECT;
							CMPL_EXPRESSION_WORD* pExprObjProp=this->AppendExprWord(EXPRESSION_VALUE::VALUETYPE_OBJPROP,EXPRESSION_OPERATION::OPER_SELFVALUE);
							pExprObjProp->value.objtype.ciObjType=liRsltValType%100;
							CMPL_EXPRESSION_OBJ* pExprObj=this->FromExpressionObj(pExprObjProp->value.objtype.ciObjType);
							int j;
							CXhChar16 szPropKey;
							for(j=i+1;j<strLen;j++)
							{
								if(IsAlphabet(expression[j]))
									szPropKey.Append(expression[j]);
								else
									break;
							}
							pExprObjProp->value.objtype.wiPropValue=pExprObj->GetObjPropId(szPropKey);
							i=j;
							w.value.pExprWord=pExprObjProp;
						}
						listWords.Append(w);
						/*
						else if(stricmp(w.value.valueStr,"INT_X")==0)
							w.value.fConst=Int2dLine(paraStr,0);
						else if(stricmp(w.value.valueStr,"INT_Y")==0)
							w.value.fConst=Int2dLine(paraStr,1);
						*/
					}
					else
#ifdef	AFX_TARG_ENU_ENGLISH
						throw "Contains undefined function in expression,error exrpession!";
#else
						throw "表达式中含有未定义的函数，表达式错误！";
#endif
					//w.word_type=WORDTYPE_CONST_FLOAT;//1;
					//listWords.Append(w);
				}
				else if(w.word_type==WORDTYPE_CONST_FLOAT)
				{
					w.value.SafeSetChar(j,'\0');
					w.value.fConst=atof(w.value.valueStr);
					listWords.Append(w);
				}
				else if(w.word_type==WORDTYPE_VARIABLE)
				{
					w.value.SafeSetChar(j,'\0');
					w.word_type=WORDTYPE_CMPLWORD;
					//w.value.pVar=FindExpressionVariable(w.value.valueStr);
					w.value.pExprWord=FindNamedExprWord(w.value.valueStr);//
					if(w.value.pExprWord==NULL)
#ifdef	AFX_TARG_ENU_ENGLISH
						throw "Contains undefined value in expression";
#else
						throw "表达式中存在未定义的变量";
#endif
					listWords.Append(w);
				}
				if(i!=strLen)
				{
					w.word_type=WORDTYPE_OPERATOR;
					w.value.chOperator=expression[i];
					listWords.Append(w);
				}
			}
		}
	}
	//解释编译(执行)表达式
	CMPL_EXPRESSION_WORD* pCmplWord=NULL;
	INTERPRET_WORD *pWord=NULL,*pFinalWord=listWords.CurrentObject;//wordList.GetTail();
	for(pWord=listWords.EnumObjectFirst();pWord;pWord=listWords.EnumObjectNext())
	{
		if(pWord->word_type==WORDTYPE_OPERATOR)	//操作符
		{
			if(pWord->value.chOperator=='(')
			{
				if( wordStack.GetPushCount()==0||
					(wordStack.TopAtom()->word_type==WORDTYPE_OPERATOR&&wordStack.TopAtom()->value.chOperator!=')'))
				{
					w.word_type=0;
					w.value.chOperator='(';
					wordStack.push(w);
					continue;
				}
				else
#ifdef	AFX_TARG_ENU_ENGLISH
					throw "Error exrpession!";
#else
					throw "表达式错误!";
#endif
			}
			else if(pWord->value.chOperator=='-' &&(wordStack.GetPushCount()==0||
				(wordStack.TopAtom()->word_type==WORDTYPE_OPERATOR&&wordStack.TopAtom()->value.chOperator=='(')))
			{	//负号
				w.word_type=WORDTYPE_CONST_FLOAT;	//1
				w.value.fConst=0;
				wordStack.push(w);
				w.word_type=0;
				w.value.chOperator='-';
				wordStack.push(w);
				continue;
			}
			else if(pWord->value.chOperator=='+'&&(wordStack.GetPushCount()==0||
				(wordStack.TopAtom()->word_type==WORDTYPE_OPERATOR&&wordStack.TopAtom()->value.chOperator=='(')))
				continue;	//正号忽略
		}
		if(pWord->word_type==WORDTYPE_OPERATOR)
		{
			bool bInterpret=false;
			do
			{
				bInterpret=InterpretBeforeOperWord(&wordStack,pWord->value.chOperator);
			}while(bInterpret);
			if(pWord->value.chOperator==')'||pWord->value.chOperator==']')
				continue;
		}
		wordStack.push(*pWord);
	}
	INTERPRET_WORD before,oper,after;
	if(wordStack.GetPushCount()>=3)
		wordStack.pop(after);
	else if(wordStack.GetPushCount()==1&&wordStack.TopAtom()->word_type!=WORDTYPE_OPERATOR)
		return UpdateExprByInterpretWord(wordStack.TopAtom());
	else
#ifdef	AFX_TARG_ENU_ENGLISH
		throw "Error expression format";
#else
		throw "表达式格式错误";
#endif
	pCmplWord=this->AppendExprWord(&after);
	calcproc.idRightExprVal=pCmplWord->Id;
	while(wordStack.GetPushCount()>=2)
	{
		wordStack.pop(oper);
		wordStack.pop(before);
		calcproc.ciOperator=oper.value.chOperator;
		pCmplWord=this->AppendExprWord(&before);
		calcproc.idLeftExprVal=pCmplWord->Id;
		if(before.word_type!=0&&after.word_type!=0&&wordStack.GetPushCount()>=2)
		{
			CMPL_EXPRESSION_WORD* pRightWord=this->AppendExprWord(0,1);
			pRightWord->calcproc=calcproc;
			value=pRightWord->value;
			calcproc.idRightExprVal=pRightWord->Id;
			calcproc.ciType=0;
		}
		//if(before.word_type!=0&&after.word_type!=0)
		//	after=CompileOneOperation(before,oper.value.chOperator,after);
	}
	//return after.value.fConst;
	return true;
}
CMPL_EXPRESSION_WORD* CMPL_EXPRESSION::CompileToExprWord(const char* exprstr,CMPL_EXPRESSION* pContainerExpr,IXhList<CMPL_EXPRESSION_WORDPTR>* pListParamWords)
{
	CXhChar200 _exprstr=exprstr;
	CXhSimpleList<CXhChar200> listParamKeys;
	char* pszKey=strtok(_exprstr,",");
	while(pszKey!=NULL)
	{
		listParamKeys.Append(pszKey);
		pszKey=strtok(NULL,",");
	}
	CMPL_EXPRESSION_WORD* pHeadExprWord=NULL,*pExprWord=NULL;
	if(listParamKeys.Count==0)
		return NULL;
	else if(listParamKeys.Count>1)
	{
		for(CXhChar200* pszParamKey=listParamKeys.EnumObjectFirst();pszParamKey;pszParamKey=listParamKeys.EnumObjectNext())
		{
			pExprWord=CompileToExprWord(*pszParamKey,pContainerExpr);
			if(pHeadExprWord==NULL)
				pHeadExprWord=pExprWord;
			if(pListParamWords!=NULL)
				pListParamWords->Append(pExprWord);
		}
		return pHeadExprWord;
	}
	else
	{	//解析单参数表达式
		CMPL_EXPRESSION tempory(pContainerExpr,pContainerExpr->words.Count+1);
		tempory.CompileExprStr(_exprstr);
		for(UINT i=0;i<tempory.words.Count;i++)
			pContainerExpr->AppendExprWord(tempory.words[i]);
		pHeadExprWord=pContainerExpr->AppendExprWord(tempory.value.ciValType,tempory.calcproc.ciType);
		pHeadExprWord->value=tempory.value;
		pHeadExprWord->calcproc=tempory.calcproc;
		if(pListParamWords!=NULL)
			pListParamWords->Append(pHeadExprWord);
	}
	return pHeadExprWord;
}
bool CMPL_EXPRESSION::Calc()	//求解
{
	return calcproc.Calc(this,&value);
}

void TestExpression()
{
	CMPL_EXPRESSION expr;
	expr.value.ciValType=EXPRESSION_VALUE::VALUETYPE_BOOL;	//布尔值
	CMPL_EXPRESSION_WORD* pExprWord,*pExprVal;
	pExprWord=expr.AppendExprWord();
	expr.calcproc.idLeftExprVal=pExprWord->Id;
	pExprWord->value.ciValType=4;	//对象属性值
	pExprWord->value.objtype.ciObjType=1;	//节点
	pExprWord->value.objtype.wiPropValue=1; //节点X坐标
	pExprVal=expr.AppendExprWord();
	pExprVal->value.ciValType=1;
	pExprVal->value.liValue=3;
	pExprVal->calcproc.ciType=0;
	pExprWord->calcproc.SetExprVal(0,pExprVal->Id);

	pExprWord=expr.AppendExprWord();
	expr.calcproc.idRightExprVal=pExprWord->Id;
	pExprWord->value.ciValType=4;	//对象属性值
	pExprWord->value.objtype.ciObjType=1;	//节点
	pExprWord->value.objtype.wiPropValue=1; //节点X坐标
	pExprVal=expr.AppendExprWord();
	pExprVal->value.ciValType=1;
	pExprVal->value.liValue=9;
	pExprVal->calcproc.ciType=0;
	pExprWord->calcproc.SetExprVal(0,pExprVal->Id);
	expr.calcproc.ciOperator=2;	//'<'
	bool successed=expr.Calc();
	int wait=3;
}