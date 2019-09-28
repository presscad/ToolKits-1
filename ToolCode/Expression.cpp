//<LOCALE_TRANSLATE BY hxr /> 2015-5-12
// Expression.cpp: implementation of the CExpression class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\Expression.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

struct EXPRESSION_WORD
{
	int word_type;	//0:操作符1:操作数常量2:操作数变量
	union ITEM{
		char chOperator;
		double fConst;
		EXPRESSION_VAR *pVar;
		char valueStr[16];
		bool SafeSetChar(int i,char ch)
		{
			if(i<15)
			{
				valueStr[i]=ch;
				return true;
			}
			return false;
		}
	}value;
	EXPRESSION_WORD(){
		word_type=1;
		memset(&value.valueStr,0,16);
	}
};

static bool IsNumeric(char ch){return (ch>='0'&&ch<='9')||ch=='.';}
static bool IsAlphabet(char ch){return (ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||ch=='_';}
bool IsOperator(char ch)
{
	if(ch=='+'||ch=='-'||ch=='*'||ch=='/'||ch=='('||ch==')'||ch=='['||ch==']')
		return true;
	else
		return false;
}
int CompareOperatorPriority(char chOper1,char chOper2)
{
	int priority1=0,priority2=0;
	if(chOper1=='('||chOper1==')')
		priority1=1;
	else if(chOper1=='+'||chOper1=='-')
		priority1=2;
	else if(chOper1=='*'||chOper1=='/')
		priority1=3;
	if(chOper2=='('||chOper2==')')
		priority2=1;
	else if(chOper2=='+'||chOper2=='-')
		priority2=2;
	else if(chOper2=='*'||chOper2=='/')
		priority2=3;
	return priority1-priority2;
}

EXPRESSION_WORD ExecuteOneOperation(EXPRESSION_WORD before,char chOperator,EXPRESSION_WORD after)
{
	if(before.word_type!=0&&after.word_type!=0)
	{
		double before_value,after_value;
		if(before.word_type==1)
			before_value=before.value.fConst;
		else
			before_value=before.value.pVar->fValue;
		if(after.word_type==1)
			after_value=after.value.fConst;
		else
			after_value=after.value.pVar->fValue;
		EXPRESSION_WORD w;
		w.word_type=1;
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
		return w;
	}
	throw "Error expression format";
#else
			throw "表达式格式错误";
		return w;
	}
	throw "表达式格式错误";
#endif
}

bool InterpretBeforeOperWord(CStack<EXPRESSION_WORD> &wordStack, char currentOperator)
{
	CStack<EXPRESSION_WORD> temStack(20);
	EXPRESSION_WORD before,oper,after,w;
	while(wordStack.GetPushSize()>0)
	{
		wordStack.pop(oper);
		if(oper.word_type!=0)
			temStack.push(oper);
		else
		{
			if(CompareOperatorPriority(oper.value.chOperator,currentOperator)>=0)
			{	//当前操作符优先级低于前一操作符优先级，原堆栈应弹出一组运算符和运算量予以运算
				if(oper.value.chOperator=='('&&currentOperator==')')
				{
					temStack.pop(oper);
					wordStack.push(oper);
					return false;
				}
				else if(wordStack.GetPushSize()>0&&temStack.GetPushSize()>0)
				{
					wordStack.pop(before);
					temStack.pop(after);
					w=ExecuteOneOperation(before,oper.value.chOperator,after);
					wordStack.push(w);
					return true;
				}
			}
			else
			{	//当前操作符优先级高于前一操作符优先级，原堆栈应保持不变，予以恢复
				wordStack.push(oper);
				while(temStack.GetPushSize()>0)
				{
					temStack.pop(oper);
					wordStack.push(oper);
				}
				return false;
			}
		}
	}
	while(temStack.GetPushSize()>0)
	{
		temStack.pop(oper);
		wordStack.push(oper);
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExpression::CExpression()
{

}

CExpression::~CExpression()
{

}
EXPRESSION_VAR* CExpression::VarAt(int index)
{
	EXPRESSION_VAR* pVar=varList.EnumObjectFirst();
	for(int i=0;pVar!=NULL&&i<index;i++)
		pVar=varList.EnumObjectNext();
	return pVar;
}

static double Int2dLineXY(const char* param_str,CExpression* pContextExpr,int x0_y1)
{
	char paraStr[200];
	strncpy(paraStr,param_str,200);
	f2dPoint pt_arr[4];
	int i=0;
	char* key=strtok(paraStr,",");
	while(key)
	{
		if(i==8)
#ifdef	AFX_TARG_ENU_ENGLISH
			throw "Parameter mismatched in reference function of expression,error expression!";
#else
			throw "表达式中引用函数中，参数不匹配，表达式错误！";
#endif
		if(i%2==0)
			pt_arr[i/2].x=pContextExpr->SolveExpression(key);
		else
			pt_arr[i/2].y=pContextExpr->SolveExpression(key);
		key=strtok(NULL,",");
		i++;
	};
	if(i!=8)
#ifdef	AFX_TARG_ENU_ENGLISH
		throw "Parameter mismatched in reference function of expression,error expression!";
#else
		throw "表达式中引用函数中，参数不匹配，表达式错误！";
#endif
	double px,py;
	Int2dpl(f2dLine(pt_arr[0],pt_arr[1]),f2dLine(pt_arr[2],pt_arr[3]),px,py);
	return (x0_y1==0)?px:py;
}
struct EXPRESSION_FUNC_INT_X : public EXPRESSION_FUNC{
	EXPRESSION_FUNC_INT_X(){
		id=1;
		memcpy(name,"INT_X",6);
	}
	//virtual long GetRsltValType(){return 0;}
	bool Calc(EXPRESSION_VALUE* pRsltValue,const char* param_str,CExpression* pContextExpr)
	{
		if(pRsltValue)
		{
			pRsltValue->ciValType=0;
			pRsltValue->fValue=Int2dLineXY(param_str,pContextExpr,0);
		}
		return true;
	}
};
struct EXPRESSION_FUNC_INT_Y : public EXPRESSION_FUNC{
	EXPRESSION_FUNC_INT_Y(){
		id=1;
		memcpy(name,"INT_Y",6);
	}
	//virtual long GetRsltValType(){return 0;}
	bool Calc(EXPRESSION_VALUE* pRsltValue,const char* param_str,CExpression* pContextExpr)
	{
		if(pRsltValue)
		{
			pRsltValue->ciValType=0;
			pRsltValue->fValue=Int2dLineXY(param_str,pContextExpr,1);
		}
		return true;
	}
};
///////////////////////////////////////////////////////////////
static EXPRESSION_FUNC_INT_X _locale_expression_funcobj_inters_x;
static EXPRESSION_FUNC_INT_Y _locale_expression_funcobj_inters_y;
EXPRESSION_FUNC::EXPRESSION_FUNC(UINT _id/*=0*/)
{
	id=_id;
	memset(name,0,19);
}
long EXPRESSION_FUNC::GetRsltValType(){
	return 0;
}
bool EXPRESSION_FUNC::Calc(EXPRESSION_VALUE* pRsltValue,const char* param_str,CExpression* pContextExpr)
{	//运算
	return false;
};
EXPRESSION_FUNC* CExpression::FromFunctionName(const char* func_label)
{
	if(stricmp(func_label,"INT_X")==0)
		return &_locale_expression_funcobj_inters_x;
	else if(stricmp(func_label,"INT_T")==0)
		return &_locale_expression_funcobj_inters_y;
	return NULL;
}

EXPRESSION_VAR *CExpression::FindExpressionVariable(char* var)
{
	for(EXPRESSION_VAR *pVar=varList.EnumObjectFirst();pVar;pVar=varList.EnumObjectNext())
	{
		if(stricmp(pVar->variableStr,var)==0)
			return pVar;
	}
	return NULL;
}

double CExpression::Int2dLine(char* paraStr,int x0_y1)
{
	f2dPoint pt_arr[4];
	int i=0;
	char* key=strtok(paraStr,",");
	while(key)
	{
		if(i==8)
#ifdef	AFX_TARG_ENU_ENGLISH
			throw "Parameter mismatched in reference function of expression,error expression!";
#else
			throw "表达式中引用函数中，参数不匹配，表达式错误！";
#endif
		if(i%2==0)
			pt_arr[i/2].x=SolveExpression(key);
		else
			pt_arr[i/2].y=SolveExpression(key);
		key=strtok(NULL,",");
		i++;
	};
	if(i!=8)
#ifdef	AFX_TARG_ENU_ENGLISH
		throw "Parameter mismatched in reference function of expression,error expression!";
#else
		throw "表达式中引用函数中，参数不匹配，表达式错误！";
#endif
	double px,py;
	Int2dpl(f2dLine(pt_arr[0],pt_arr[1]),f2dLine(pt_arr[2],pt_arr[3]),px,py);
	if(x0_y1==0)
		return px;
	else
		return py;
}
/*void Test_ToExpression()
{
	CXhChar16 destStr;
	CExpression::ToExpression(0,2,destStr,16);
	CExpression::ToExpression(0.1,2,destStr,16);
	CExpression::ToExpression(0.2,2,destStr,16);
	CExpression::ToExpression(0.3,2,destStr,16);
	CExpression::ToExpression(0.4,2,destStr,16);
	CExpression::ToExpression(0.5,2,destStr,16);
	CExpression::ToExpression(0.6,2,destStr,16);
	CExpression::ToExpression(0.7,2,destStr,16);
	CExpression::ToExpression(0.8,2,destStr,16);
	CExpression::ToExpression(0.9,2,destStr,16);

	CExpression::ToExpression(0.12,2,destStr,16);
	CExpression::ToExpression(0.24,2,destStr,16);
	CExpression::ToExpression(0.33,2,destStr,16);
	CExpression::ToExpression(0.16,2,destStr,16);
	CExpression::ToExpression(0.15,2,destStr,16);
	CExpression::ToExpression(0.18,2,destStr,16);
	CExpression::ToExpression(0.32,2,destStr,16);
	CExpression::ToExpression(0.66,2,destStr,16);
	CExpression::ToExpression(0.14,2,destStr,16);

	CExpression::ToExpression(0.124,3,destStr,16);
	CExpression::ToExpression(0.25,3,destStr,16);
	CExpression::ToExpression(0.33,3,destStr,16);
	CExpression::ToExpression(0.166,3,destStr,16);
	CExpression::ToExpression(0.15,3,destStr,16);
	CExpression::ToExpression(0.18,3,destStr,16);
	CExpression::ToExpression(0.333,3,destStr,16);
	CExpression::ToExpression(0.667,3,destStr,16);
	CExpression::ToExpression(0.14,3,destStr,16);
}*/
void CExpression::ToExpression(double f,int precision,char* destStr,int maxCount)
{
	if(precision<2)
		precision=2;
	if(f<1)
	{
		double tolerance=pow(0.1,precision);
		if(1-f<tolerance)
			strncpy(destStr,"1",maxCount);
		else if(fabs(0.5-f)<tolerance)
		{
			strncpy(destStr,"1/2",maxCount);
			return;
		}
		else
		{
			for(int divn=3;divn<=8;divn++)
			{
				for(int j=1;j<divn;j++)
				{
					double div=j/(double)divn;
					if(fabs(div-f)<tolerance)
					{
						_snprintf(destStr,maxCount,"%d/%d",j,divn);
						return;
					}
				}
			}
		}
	}
	//默认直接打印浮点数
	char format[10];
	sprintf(format,"%%.%df",precision);
	_snprintf(destStr,maxCount,format,f);
	//去除小数部分末尾无效零字符
	char* split=strchr(destStr,'.');
	if(split==NULL)
		return;
	int len=strlen(split);
	for(int i=len-1;i>0;i--)
	{
		if(split[i]=='0')
		{
			split[i]=0;
			if(i==1)	//无小数部分
				*split=0;
		}
		else
			break;
	}
}
double CExpression::SolveExpression(const char* expression)
{
	CStack<EXPRESSION_WORD>wordStack(20);
	int i,j=0,strLen=strlen(expression);
	if(strLen==0)
		return 0.0;
	wordStack.SetBlockSize(10);
	//扫描表达式，并进行换码转换为单词链表
	EXPRESSION_WORD w;
	bool wordBegin=false,funcBegin=false;
	ATOM_LIST<EXPRESSION_WORD>wordList;
	for(i=0;i<=strLen;i++)
	{
		if(i<strLen&&expression[i]==' ')
			continue;
		else if(!wordBegin&&i<strLen)
		{	//目前并不是一个单词的开始状态
			j=0;
			if(IsOperator(expression[i]))
			{	//操作符
				w.word_type=0;
				w.value.chOperator=expression[i];
				wordList.append(w);
				continue;
			}
			//一个单词（操作常数或操作变量）的开始
			wordBegin=true;
			w.value.SafeSetChar(j,expression[i]);
			if(IsNumeric(expression[i]))
				w.word_type=1;	//操作数常量
			else if(IsAlphabet(expression[i]))
				w.word_type=2;	//操作数变量
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
			if(w.word_type==1&&IsNumeric(expression[i]))
				w.value.SafeSetChar(j,expression[i]);	//一个操作常数的继续
			else if(w.word_type==2&&(IsNumeric(expression[i])||IsAlphabet(expression[i])))
				w.value.SafeSetChar(j,expression[i]);	//一个操作变量的继续
			else if(w.word_type==1&&IsAlphabet(expression[i]))
#ifdef	AFX_TARG_ENU_ENGLISH
				throw "Contains letter in operating constant,error expression!";
#else
				throw "操作常数中含有字母，表达式错误！";
#endif
			else if(IsOperator(expression[i])||i==strLen)
			{	//一个单词需要结束
				wordBegin=false;	//词结束
				if(i<strLen&&(expression[i]=='('||expression[i]=='['))	//函数或索引开始
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
					EXPRESSION_FUNC *pExprFunc=NULL;
					if(cLeftBracketType=='[')
					{
						UINT uiIndex=(UINT)SolveExpression(paraStr);
						EXPRESSION_VAR *pIndexVar=FindExpressionVariable(w.value.valueStr);
						if(pIndexVar==NULL)
							throw "表达式中含有未定义的数组变量";
						else if(pIndexVar->uiUpperBound<uiIndex)
							throw "表达式中数组变量超出索引上限";
						w.value.fConst=pIndexVar->arrValues[uiIndex];
					}
					else if(stricmp(w.value.valueStr,"INT_X")==0)
						w.value.fConst=Int2dLine(paraStr,0);
					else if(stricmp(w.value.valueStr,"INT_Y")==0)
						w.value.fConst=Int2dLine(paraStr,1);
					else if((pExprFunc=FromFunctionName(w.value.valueStr))!=NULL)
					{
						EXPRESSION_VALUE rsltval;
						pExprFunc->Calc(&rsltval,paraStr,this);
						w.value.fConst=rsltval.liSafeVal;
					}
					else
#ifdef	AFX_TARG_ENU_ENGLISH
						throw "Contains undefined function in expression,error exrpession!";
#else
						throw "表达式中含有未定义的函数，表达式错误！";
#endif
					w.word_type=1;
					wordList.append(w);
				}
				else if(w.word_type==1)
				{
					w.value.SafeSetChar(j,'\0');
					w.value.fConst=atof(w.value.valueStr);
					wordList.append(w);
				}
				else if(w.word_type==2)
				{
					w.value.SafeSetChar(j,'\0');
					w.value.pVar=FindExpressionVariable(w.value.valueStr);
					if(w.value.pVar==NULL)
#ifdef	AFX_TARG_ENU_ENGLISH
						throw "Contains undefined value in expression";
#else
						throw "表达式中存在未定义的变量";
#endif
					wordList.append(w);
				}
				if(i!=strLen)
				{
					w.word_type=0;
					w.value.chOperator=expression[i];
					wordList.append(w);
				}
			}
		}
	}
	//解释执行表达式
	EXPRESSION_WORD *pWord=NULL,*pFinalWord=wordList.GetTail();
	for(pWord=wordList.GetFirst();pWord;pWord=wordList.GetNext())
	{
		if(pWord->word_type==0)	//操作符
		{
			if(pWord->value.chOperator=='(')
			{
				if( wordStack.GetPushSize()==0||
					(wordStack.TopAtom()->word_type==0&&wordStack.TopAtom()->value.chOperator!=')'))
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
			else if(pWord->value.chOperator=='-' &&(wordStack.GetPushSize()==0||
				(wordStack.TopAtom()->word_type==0&&wordStack.TopAtom()->value.chOperator=='(')))
			{	//负号
				w.word_type=1;
				w.value.fConst=0;
				wordStack.push(w);
				w.word_type=0;
				w.value.chOperator='-';
				wordStack.push(w);
				continue;
			}
			else if(pWord->value.chOperator=='+'&&(wordStack.GetPushSize()==0||
				(wordStack.TopAtom()->word_type==0&&wordStack.TopAtom()->value.chOperator=='(')))
				continue;	//正号忽略
		}
		if(pWord->word_type==0)
		{
			bool bInterpret=false;
			do
			{
				bInterpret=InterpretBeforeOperWord(wordStack,pWord->value.chOperator);
			}while(bInterpret);
			if(pWord->value.chOperator==')'||pWord->value.chOperator==']')
				continue;
		}
		wordStack.push(*pWord);
	}
	EXPRESSION_WORD before,oper,after;
	if(wordStack.GetPushSize()>=3)
		wordStack.pop(after);
	else if(wordStack.GetPushSize()==1&&wordStack.TopAtom()->word_type!=0)
	{
		if(wordStack.TopAtom()->word_type==1)
			return wordStack.TopAtom()->value.fConst;
		else
			return wordStack.TopAtom()->value.pVar->fValue;
	}
	else
#ifdef	AFX_TARG_ENU_ENGLISH
		throw "Error expression format";
#else
		throw "表达式格式错误";
#endif
	while(wordStack.GetPushSize()>=2)
	{
		wordStack.pop(oper);
		wordStack.pop(before);
		if(before.word_type!=0&&after.word_type!=0)
			after=ExecuteOneOperation(before,oper.value.chOperator,after);
	}
	return after.value.fConst;
}
bool LogicalJustify(double leftValue,int operType,double rightValue)
{
	switch(operType)
	{
	case CExpression::OPER_TYPE_NOTLESS:
		if(leftValue>=rightValue)
			return true;
		else
			return false;
	case CExpression::OPER_TYPE_GREATER:
		if(leftValue>rightValue)
			return true;
		else
			return false;
	case CExpression::OPER_TYPE_LESS:
		if(leftValue<rightValue)
			return true;
		else
			return false;
	case CExpression::OPER_TYPE_NOTGREATER:
		if(leftValue<=rightValue)
			return true;
		else
			return false;
	case CExpression::OPER_TYPE_EQUAL:
		if(leftValue==rightValue)
			return true;
		else
			return false;
	case CExpression::OPER_TYPE_NOTEQUAL:
		if(leftValue!=rightValue)
			return true;
		else
			return false;
	default:
		return false;
	}
}
bool CExpression::SolveLogicalExpression(const char* expression)
{
	char native_expr[201]={0};
	strncpy(native_expr,expression,200);
	int operType=0;
	char* forePart=native_expr;
	char* hindPart=NULL;
	if((hindPart=strstr(native_expr,">="))!=NULL)
	{
		operType=OPER_TYPE_NOTLESS;
		memset(hindPart,0,2);
		hindPart+=2;
	}
	else if((hindPart=strstr(native_expr,"<="))!=NULL)
	{
		operType=OPER_TYPE_NOTGREATER;
		memset(hindPart,0,2);
		hindPart+=2;
	}
	else if((hindPart=strstr(native_expr,"!="))!=NULL)
	{
		operType=OPER_TYPE_NOTEQUAL;
		memset(hindPart,0,2);
		hindPart+=2;
	}
	else if((hindPart=strchr(native_expr,'>'))!=NULL)
	{
		operType=OPER_TYPE_GREATER;
		*hindPart=0;
		hindPart++;
	}
	else if((hindPart=strchr(native_expr,'<'))!=NULL)
	{
		operType=OPER_TYPE_LESS;
		*hindPart=0;
		hindPart++;
	}
	else if((hindPart=strchr(native_expr,'='))!=NULL)
	{
		operType=OPER_TYPE_EQUAL;
		*hindPart=0;
		hindPart++;
	}
	else	//不支持的逻辑表达式
		return false;

	double foreValue=this->SolveExpression(forePart);
	double hindValue=SolveExpression(hindPart);
	return LogicalJustify(foreValue,operType,hindValue);
}