// EntityDataBase.h: interface for the CEntityDataBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENTITYDATABASE_H__D8EF80D6_6C10_4C71_A2E2_EDDE2BAB943E__INCLUDED_)
#define AFX_ENTITYDATABASE_H__D8EF80D6_6C10_4C71_A2E2_EDDE2BAB943E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "f_ent.h"
#include "f_alg_fun.h"
#include "f_ent_list.h"
class CEntityDataBase  
{
public:
	void Empty();
	void GetScopeSize(SCOPE_STRU *scope);
	CEntityDataBase();
	~CEntityDataBase();
	ATOM_LIST<f3dPoint> m_arrPoint3d;
	ATOM_LIST<f3dLine> m_arrLine3d;
	ATOM_LIST<f3dArcLine> m_arrArcLine3d;
	ATOM_LIST<f3dRect> m_arrRect3d;
	ATOM_LIST<f3dCircle> m_arrCircle3d;
	ATOM_LIST<CMark>m_arrMark;			//系统标志
};

#endif // !defined(AFX_ENTITYDATABASE_H__D8EF80D6_6C10_4C71_A2E2_EDDE2BAB943E__INCLUDED_)
