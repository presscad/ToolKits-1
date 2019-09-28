// Variant.cpp: implementation of the Variant class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Variant.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//----------------异构单元格内容类型类-------------------------------
CVariant::CVariant()
{
	memset(this,0,sizeof(CVariant));
	vType = GRID_VT_EMPTY;		//未指定数据类型
}
CVariant::CVariant(CVariant& SourceObj)
{
	*this = SourceObj;
}

CVariant::~CVariant()
{
	ClearVariant();
}
void CVariant::ClearVariant()	//清空异构变量所控内存并清零存储区
{
	GRID_DATA_TYPE old_type = vType;
	if(vType==GRID_VT_STRING&&sVal)
		free(sVal);		//清空字符串空间
	memset(this,0,sizeof(CVariant));	//清零存储区
	vType = old_type;	//恢得原数据类型
}
void CVariant::ChangeToType(GRID_DATA_TYPE data_type)
{
	if(vType==data_type)
		return;
	else
	{
		ClearVariant();
		vType = data_type;
	}
}
	//重载赋值操作符以防赋值时内存泄漏
CVariant& CVariant::operator = (const CVariant &var)
{
	if(var.vType==GRID_VT_STRING)
		SetGridString(var.sVal);
	else
		memcpy(this,&var,sizeof(CVariant));
	return *this;
}

bool CVariant::SetGridString(const char *string, bool bForceToString/*=true*/)
{
	if(vType!=GRID_VT_STRING&&!bForceToString||string==NULL)
		return false;	//类型不匹配
	else if(vType!=GRID_VT_STRING)
	{
		ChangeToType(GRID_VT_STRING);
		sVal = (char*)realloc(sVal,strlen(string)+2);
		strcpy(sVal,string);
		return true;
	}
	else
	{
		sVal = (char*)realloc(sVal,strlen(string)+2);
		strcpy(sVal,string);
		return true;
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVariant2dArray::CVariant2dArray(VARIANT _var)
{
	var=_var;
}
CVariant2dArray::CVariant2dArray(int rows,int cols)
{
	var.vt=VT_VARIANT|VT_ARRAY;
	SAFEARRAYBOUND rgsabound[2];
	rgsabound[0].lLbound=1;
	rgsabound[0].cElements=rows;
	rgsabound[1].lLbound=1;
	rgsabound[1].cElements=cols;
	var.parray = SafeArrayCreate(VT_VARIANT,2,rgsabound);
}

CVariant2dArray::~CVariant2dArray()
{
}
int CVariant2dArray::RowsCount()
{
	return var.parray->rgsabound[1].cElements;
}
int CVariant2dArray::ColsCount()
{
	return var.parray->rgsabound[0].cElements;
}
int CVariant2dArray::ValidColsCount()
{	//以前十行为样本探出有效列出
	int nRowsCount=min(10,RowsCount());
	int nColCount=ColsCount();
	VARIANT var;
	int nValidColCount=0;
	for(int i=0;i<nColCount;i++)
	{
		BOOL bHasContent=FALSE;
		for(int j=0;j<nRowsCount;j++)
		{
			if(GetValueAt(j,i,var)&&var.vt!=VT_EMPTY)
			{
				bHasContent=TRUE;
				break;
			}
		}
		if(!bHasContent)
			break;
		nValidColCount++;
	}
	return nValidColCount;
}
bool CVariant2dArray::GetValueAt(int i,int j,VARIANT& value)
{
	value.vt=VT_EMPTY;
	i+=var.parray->rgsabound[1].lLbound;
	j+=var.parray->rgsabound[0].lLbound;
	LONG indices[2]={i,j};
	bool result=FAILED(SafeArrayGetElement(var.parray,&indices[0],&value));
	return !result;
}
bool CVariant2dArray::SetValueAt(int i,int j,VARIANT value)
{
	i+=var.parray->rgsabound[1].lLbound;
	j+=var.parray->rgsabound[0].lLbound;
	LONG indices[2]={i,j};
	return !FAILED(SafeArrayPutElement(var.parray,indices,&value));
}
