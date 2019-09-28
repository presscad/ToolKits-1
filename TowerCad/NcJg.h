// NcJg.h: interface for the CNcJg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NCJG_H__C14AAEC8_CE1B_4BF3_8409_4CC48D8AA5D7__INCLUDED_)
#define AFX_NCJG_H__C14AAEC8_CE1B_4BF3_8409_4CC48D8AA5D7__INCLUDED_

#include "f_ent.h"
#include "ProcessPart.h"
#include "JgNcDataManager.h"
#ifdef  __LDS_CONTEXT_
#include "Tower.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CNcJg  
{
	CArray<f3dCircle,f3dCircle>LsCirX;
	CArray<f3dCircle,f3dCircle>LsCirY;
	double wing_wide,wing_thick,length,current_len_pos,current_accurate_len_pos;
	char cMaterial;
	char sPartNo[20];
public:
	char sPrefix[20];
	CJgNcDataManager NcManager;
	void PrintLn(FILE* fp,char *format,f3dCircle *ls_cir=NULL);
	CNcJg();
	virtual ~CNcJg();
#ifdef  __LDS_CONTEXT_
	BOOL InitJg(CLDSPart *pAnglePart);
#endif
	BOOL InitProcessAngle(CProcessAngle *pAngle);
	BOOL GenNCFile(const char *sFileName,const char *sPartNoPrefix=NULL);
	BOOL GetXLsCircle(f3dCircle &Cir,int index);
	BOOL GetYLsCircle(f3dCircle &Cir,int index);
	int GetXLsCount();
	int GetYLsCount();
};
#endif // !defined(AFX_NCJG_H__C14AAEC8_CE1B_4BF3_8409_4CC48D8AA5D7__INCLUDED_)
