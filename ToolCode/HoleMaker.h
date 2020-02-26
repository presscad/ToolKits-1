// HoleMaker.h: interface for the CHoleMaker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOLEMAKER_H__BFDB93F9_2300_4E5B_8976_F251998C3C6D__INCLUDED_)
#define AFX_HOLEMAKER_H__BFDB93F9_2300_4E5B_8976_F251998C3C6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHoleMaker 
{
	double dfHoleD,dfWaistL;
public:
	CHoleMaker(double _fHoleD = 0,double _fWaistL = 0) {
		dfHoleD=_fHoleD;
		dfWaistL=_fWaistL;
	}
	virtual ~CHoleMaker();
	BOOL Create(GECS& hole_ucs,double d,double waist_len,
		f3dPolyFace *pUpFace, f3dPolyFace *pDownFace,
		fBody *pBody,COLORREF crLsHole,int slices=12);
	BOOL Create(GECS hole_ucs,f3dPolyFace *pUpFace,f3dPolyFace *pDownFace,
		fBody *pBody,COLORREF crLsHole,int slices=12) {
		return Create(hole_ucs,dfHoleD,dfWaistL,pUpFace,pDownFace,pBody,crLsHole,slices);
	}
};

#endif // !defined(AFX_HOLEMAKER_H__BFDB93F9_2300_4E5B_8976_F251998C3C6D__INCLUDED_)
