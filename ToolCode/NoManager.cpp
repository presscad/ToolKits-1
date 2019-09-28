// NoManager.cpp: implementation of the CObjNoManager class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "NoManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CObjNoGroup::CObjNoGroup(int nMaxNo/*=0*/)
{
	m_idInit=1;
	m_nMaxNo=nMaxNo;
}
void CObjNoGroup::InitMaxNo(unsigned int nMaxNo)
{
	m_nMaxNo=nMaxNo;
	for(long *pNo=list.GetFirst();pNo;pNo=list.GetNext())
	{
		if(list.GetCursorKey()>nMaxNo)
			list.DeleteCursor();
	}
	list.Clean();
}
long CObjNoGroup::GetUsedNoCount()
{
	long nHits=0;
	for(long iNo=GetFirstUsedNo();iNo>0;iNo=GetNextUsedNo())
		nHits++;
	return nHits;
}
long CObjNoGroup::GetFirstUsedNo()
{
	long *pNo=list.GetFirst();
	if(pNo)
		return list.GetCursorKey();
	else
		return 0;
}
long CObjNoGroup::GetNextUsedNo()
{
	long *pNo=list.GetNext();
	if(pNo)
		return list.GetCursorKey();
	else
		return 0;
}
int CObjNoGroup::EnumIdleNo()
{
	for(int i=m_idInit;i<=m_nMaxNo;i++)
	{
		long *pNo=list.GetValue(i);
		if(pNo)
			continue;
		else
			return i;
	}
	return 0;
}
void CObjNoGroup::SetNoState(int iNo,BOOL bInUse/*=TRUE*/)
{
	if(bInUse)
		list.SetValue(iNo,iNo);
	else
	{
		list.DeleteNode(iNo);
		list.Clean();
	}
}

void CObjNoGroup::EmptyNo()
{
	list.Empty();
}

CObjNoManager::CObjNoManager()
{
	/*AppendGroup(MODEL_DATUM_POINT_GROUP,1000);		//基准节点最大编号1000
	AppendGroup(MODEL_DATUM_LINE_GROUP,1000);		//基准线点最大编号1000
	AppendGroup(MODEL_DATUM_PLANE_GROUP,1000);		//基准面点最大编号1000
	AppendGroup(MODEL_ARCLIFT_GROUP,1000);			//预拱项最大编号1000
	AppendGroup(MODEL_GROUP,1000);					//计算模型最大编号1000
	AppendGroup(WIRELAYOUT_GROUP,100);				//架线方案最大编号100
	AppendGroup(WORKSTATUS_GROUP,10000);			//计算工况最大编号10000
	AppendGroup(WINDSEG_GROUP,1000);				//最多风压段编号1000
	AppendGroup(WORKSTATUS_ATTACH_WINDLOAD,1000);	//最多风压段编号1000
	AppendGroup(WIRELOAD_MODEL_GROUP,1000);			//最多线条荷载编号1000
	AppendGroup(FL_OPTIMIZOR_GROUP,10000);			//最多优化选材法兰编号10000
	CObjNoGroup* pParaCSGroup=AppendGroup(PARAMETRIC_CS_GROUP,100000);			//最多装备坐标系编号100000
	pParaCSGroup->m_idInit=4;	//1~3保留给固定X/Y/Z轴对称变换*/
}
CObjNoGroup* CObjNoManager::AppendGroup(long ID,long nMaxNo)
{
	CObjNoGroup* pGroup=hashNoGroup.Add(ID);
	pGroup->m_idGroup=ID;
	pGroup->m_nMaxNo=nMaxNo;
	return pGroup;
}

CObjNoGroup* CObjNoManager::FromGroupID(long ID)
{
	return hashNoGroup.GetValue(ID);
}
void CObjNoManager::EmptyGroupNo()
{
	for(CObjNoGroup*pGroup=hashNoGroup.GetFirst();pGroup;pGroup=hashNoGroup.GetNext())
		pGroup->EmptyNo();
}
void CObjNoManager::EmptyGroup()
{
	hashNoGroup.Empty();
}

