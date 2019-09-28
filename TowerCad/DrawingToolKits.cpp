#include "StdAfx.h"
#include "DrawingToolKits.h"
#include "f_alg_fun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IDbPoint *AppendDbPoint(IDrawing *pDrawing,GEPOINT pos,
						HIBERID hiberId/*=0*/,short style/*=PS_SOLID*/,
						COLORREF crColor/*=RGB(0,0,0)*/,BYTE width/*=5*/)
{
	IDbPoint *pPoint=(IDbPoint*)pDrawing->AppendDbEntity(IDbEntity::DbPoint);
	pPoint->SetPosition(pos);
	PEN_STRU pen;
	pen.crColor=crColor;
	pen.style=style;
	pen.width=width;
	pPoint->SetPen(pen);
	pPoint->SetHiberId(hiberId);
	return pPoint;
}

IDbLine *AppendDbLine(IDrawing *pDrawing,GEPOINT start,GEPOINT end,
					  HIBERID hiberId/*=0*/,short style/*=PS_SOLID*/,
					  COLORREF crColor/*=RGB(0,0,0)*/,BYTE width/*=1*/)
{
	IDbLine *pLine=(IDbLine*)pDrawing->AppendDbEntity(IDbEntity::DbLine);
	pLine->SetStartPosition(start);
	pLine->SetEndPosition(end);
	PEN_STRU pen;
	pen.crColor=crColor;
	pen.style=style;
	pen.width=width;
	pLine->SetPen(pen);
	pLine->SetHiberId(hiberId);
	return pLine;
}
IDbArcline *AppendDbArcLine(IDrawing *pDrawing,double* center,double r,
							double startAngle,double endAngle,double* work_norm,
							HIBERID hiberId/*=0*/,short style/*=PS_SOLID*/,
							COLORREF crColor/*=RGB(0,0,0)*/,BYTE width/*=1*/)
{
	IDbArcline *pArcLine=(IDbArcline*)pDrawing->AppendDbEntity(IDbEntity::DbArcline);
	f3dPoint startPt(center[0]+r,center[1]);
	rotate_point_around_axis(startPt,startAngle,f3dPoint(center),f3dPoint(center)+f3dPoint(work_norm)*100);
	pArcLine->CreateMethod1(center,startPt,work_norm,endAngle-startAngle);
	PEN_STRU pen;
	pen.crColor=crColor;
	pen.style=style;
	pen.width=width;
	pArcLine->SetPen(pen);
	pArcLine->SetHiberId(hiberId);
	return pArcLine;
}
IDbArcline *AppendDbArcLine(IDrawing *pDrawing,
							HIBERID hiberId/*=0*/,short style/*=PS_SOLID*/,
							COLORREF crColor/*=RGB(0,0,0)*/,BYTE width/*=1*/)
{
	IDbArcline *pArcLine=(IDbArcline*)pDrawing->AppendDbEntity(IDbEntity::DbArcline);
	PEN_STRU pen;
	pen.crColor=crColor;
	pen.style=style;
	pen.width=width;
	pArcLine->SetPen(pen);
	pArcLine->SetHiberId(hiberId);
	return pArcLine;
}
IDbPolyline *AppendDbPolyline(IDrawing *pDrawing,GEPOINT *ptArr,int nArrSize,
							  HIBERID hiberId/*=0*/,short style/*=PS_SOLID*/,
							  COLORREF crColor/*=RGB(0,0,0)*/,BYTE width/*=1*/)
{
	IDbPolyline *pPolyline=(IDbPolyline*)pDrawing->AppendDbEntity(IDbEntity::DbPolyline);
	for(int i=0;i<nArrSize;i++)
	{
		pPolyline->AddVertexAt(i,ptArr[i]);
		int nCount=pPolyline->numVerts();
	}
	PEN_STRU pen;
	pen.crColor=crColor;
	pen.style=style;
	pen.width=width;
	pPolyline->SetPen(pen);
	pPolyline->SetHiberId(hiberId);
	return pPolyline;
}


IDbCircle *AppendDbCircle(IDrawing *pDrawing,f3dPoint center,f3dPoint norm,
						  double radius,HIBERID hiberId/*=0*/,short style/*=PS_SOLID*/,
						  COLORREF crColor/*=RGB(0,0,0)*/,BYTE width/*=1*/)
{
	IDbCircle *pCir=(IDbCircle*)pDrawing->AppendDbEntity(IDbEntity::DbCircle);
	pCir->SetCenter(center);
	pCir->SetNormal(norm);
	pCir->SetRadius(radius);
	PEN_STRU pen;
	pen.crColor=crColor;
	pen.style=style;
	pen.width=width;
	pCir->SetPen(pen);
	pCir->SetHiberId(hiberId);
	return pCir;
}
IDbText *AppendDbText(IDrawing *pDrawing,f3dPoint pos,const char* text,
					  double rotation,double height,int aligment/*=IDbText::AlignMiddleLeft*/,
					  HIBERID hiberId/*=0*/,short style/*=PS_SOLID*/,
					  COLORREF crColor/*=RGB(0,0,0)*/,BYTE width/*=1*/,GEPOINT norm/*=GEPOINT(0,0,1)*/)
{
	IDbText *pText=(IDbText*)pDrawing->AppendDbEntity(IDbEntity::DbText);
	pText->SetHeight(height);
	pText->SetAlignment(aligment);
	pText->SetPosition(pos);
	pText->SetRotation(rotation);
	pText->SetTextString(text);
	pText->SetNormal(norm);
	PEN_STRU pen;
	pen.crColor=crColor;
	pen.style=style;
	pen.width=width;
	pText->SetPen(pen);
	pText->SetHiberId(hiberId);
	return pText;
}
IDbMText *AppendDbMText(IDrawing *pDrawing,f3dPoint pos,const char* text,
						double rotation,double height,double width,
						int aligment/*=IDbText::AlignMiddleLeft*/,short flowDirection/*=IDbMText::kLtoR*/,
						HIBERID hiberId/*=0*/,short style/*=PS_SOLID*/,
						COLORREF crColor/*=RGB(0,0,0)*/,BYTE pen_width/*=1*/)
{
	IDbMText *pText=(IDbMText*)pDrawing->AppendDbEntity(IDbEntity::DbMText);
	pText->SetHeight(height);
	pText->SetAlignment(aligment);
	pText->SetPosition(pos);
	pText->SetRotation(rotation);
	pText->SetTextString(text);
	pText->SetWidth(width);
	pText->SetFlowDirection(flowDirection);
	PEN_STRU pen;
	pen.crColor=crColor;
	pen.style=style;
	pen.width=pen_width;
	pText->SetPen(pen);
	pText->SetHiberId(hiberId);
	return pText;
}
IDbRect *AppendDbRect(IDrawing *pDrawing,f3dPoint topLeft,f3dPoint btmRight,
					  HIBERID hiberId/*=0*/,short style/*=PS_SOLID*/,
					  COLORREF crColor/*=RGB(0,0,0)*/,BYTE width/*=1*/)
{
	IDbRect *pRect=(IDbRect*)pDrawing->AppendDbEntity(IDbEntity::DbRect);
	pRect->SetTopLeft(topLeft);
	pRect->SetBottomRight(btmRight);
	PEN_STRU pen;
	pen.crColor=crColor;
	pen.style=style;
	pen.width=width;
	pRect->SetPen(pen);
	pRect->SetHiberId(hiberId);
	return pRect;
}
IDbAlignedDimension* AppendDbAlignedDim(IDrawing *pDrawing,f3dPoint start,f3dPoint end,
										f3dPoint dimPos,const char* dimText,long dimStyleId/*=0*/,double fTextH/*=2*/,
										GEPOINT upperDirection/*=GEPOINT(0,0,-1)*/,HIBERID hiberId/*=0*/,short style/*=PS_SOLID*/,
										COLORREF crColor/*=RGB(0,0,0)*/,BYTE width/*=1*/)
{
	IDbAlignedDimension *pDim=(IDbAlignedDimension*)pDrawing->AppendDbEntity(IDbEntity::DbAlignedDimension);
	pDim->SetLine1Point(start);
	pDim->SetLine2Point(end);
	pDim->SetDimLinePoint(dimPos);
	pDim->SetDimText(dimText);
	pDim->SetUpperDirection(upperDirection);
	pDim->SetDimStyleId(dimStyleId);
	if(fTextH>0)
		pDim->SetTextHeight(fTextH);
	PEN_STRU pen;
	pen.crColor=crColor;
	pen.style=style;
	pen.width=width;
	pDim->SetPen(pen);
	pDim->SetHiberId(hiberId);
	return pDim;
}
IDbRotatedDimension* AppendDbRotatedDim(IDrawing *pDrawing,f3dPoint start,f3dPoint end,
										f3dPoint dimPos,const char* dimText,double rotation,long dimStyleId/*=0*/,
										GEPOINT upperDirection/*=GEPOINT(0,0,-1)*/,HIBERID hiberId/*=0*/,short style/*=PS_SOLID*/,
										COLORREF crColor/*=RGB(0,0,0)*/,BYTE width/*=1*/)
{
	IDbRotatedDimension *pDim=(IDbRotatedDimension*)pDrawing->AppendDbEntity(IDbEntity::DbRotatedDimension);
	pDim->SetLine1Point(start);
	pDim->SetLine2Point(end);
	pDim->SetDimLinePoint(dimPos);
	//pDim->SetOblique(rotation);
	pDim->SetRotationRadian(rotation);
	pDim->SetDimText(dimText);
	pDim->SetUpperDirection(upperDirection);
	pDim->SetDimStyleId(dimStyleId);
	PEN_STRU pen;
	pen.crColor=crColor;
	pen.style=style;
	pen.width=width;
	pDim->SetPen(pen);
	pDim->SetHiberId(hiberId);
	return pDim;
}
IDbArrow* AppendDbArrow(IDrawing *pDrawing,f3dPoint origin,
						f3dPoint direct,double len,double hederLen,
						HIBERID hiberId/*=0*/,short style/*=PS_SOLID*/,
						COLORREF crColor/*=RGB(0,0,0)*/,BYTE width/*=1*/)
{
	IDbArrow *pArrow=(IDbArrow*)pDrawing->AppendDbEntity(IDbEntity::DbArrow);
	pArrow->SetArrowOrigin(origin);
	pArrow->SetArrowDirect(direct);
	pArrow->SetArrowLength(len);
	pArrow->SetArrowHeaderLength(hederLen);
	PEN_STRU pen;
	pen.crColor=crColor;
	pen.style=style;
	pen.width=width;
	pArrow->SetPen(pen);
	pArrow->SetHiberId(hiberId);
	return pArrow;
}

IDbDiametricDimension *AppendDbDiaDim(IDrawing *pDrawing,f3dPoint chordPoint,
									  f3dPoint farChordPoint,double leaderLength,
									  HIBERID hiberId/*=0*/,short style/*=PS_SOLID*/,
									  COLORREF crColor/*=RGB(0,0,0)*/,BYTE width/*=1*/)
{
	IDbDiametricDimension *pDiaDim=(IDbDiametricDimension*)pDrawing->AppendDbEntity(IDbEntity::DbDiametricDimension);
	pDiaDim->SetChordPoint(chordPoint);
	pDiaDim->SetFarChordPoint(farChordPoint);
	pDiaDim->SetLeaderLength(leaderLength);
	PEN_STRU pen;
	pen.crColor=crColor;
	pen.style=style;
	pen.width=width;
	pDiaDim->SetPen(pen);
	pDiaDim->SetHiberId(hiberId);
	return pDiaDim;
}

//////////////////////////////////////////////////////////////////////////
//
void DRAWING_DIM_STYLE::UpdateDimStyleTextHeight(IEntityDatabase* pDataBase,double fTextHeight)
{
	if(pDataBase==NULL||m_idDimStyle==0)
		return;
	pDataBase->GetDimStyle(m_idDimStyle,&dimStyleVar);
	dimStyleVar.m_fTextHeight=fTextHeight;
	pDataBase->SetDimStyle(m_idDimStyle,dimStyleVar);
}
void DRAWING_DIM_STYLE::UpdateDimStyleDimGap(IEntityDatabase* pDataBase,double fDimGap)
{
	if(pDataBase==NULL||m_idDimStyle==0)
		return;
	pDataBase->GetDimStyle(m_idDimStyle,&dimStyleVar);
	dimStyleVar.m_fDimGap=fDimGap;
	pDataBase->SetDimStyle(m_idDimStyle,dimStyleVar);
}
void DRAWING_DIM_STYLE::UpdateDimStyleDimExtend(IEntityDatabase* pDataBase,double fDimExtend)
{
	if(pDataBase==NULL||m_idDimStyle==0)
		return;
	pDataBase->GetDimStyle(m_idDimStyle,&dimStyleVar);
	dimStyleVar.m_fDimExtend=fDimExtend;
	pDataBase->SetDimStyle(m_idDimStyle,dimStyleVar);
}
void DRAWING_DIM_STYLE::UpdateDimStyleDimBlk(IEntityDatabase* pDataBase,char ciDimBlk)
{
	if(pDataBase==NULL||m_idDimStyle==0)
		return;
	pDataBase->GetDimStyle(m_idDimStyle,&dimStyleVar);
	dimStyleVar.m_ciDimBlk=ciDimBlk;
	pDataBase->SetDimStyle(m_idDimStyle,dimStyleVar);
}
void DRAWING_DIM_STYLE::UpdateDimStyleArrowSize(IEntityDatabase* pDataBase,double fArrowSize)
{
	if(pDataBase==NULL||m_idDimStyle==0)
		return;
	pDataBase->GetDimStyle(m_idDimStyle,&dimStyleVar);
	dimStyleVar.m_fArrowSize=fArrowSize;
	pDataBase->SetDimStyle(m_idDimStyle,dimStyleVar);
}
void DRAWING_DIM_STYLE::UpdateDimStyleTextStyleId(IEntityDatabase* pDataBase,long idTextStyle)
{
	if(pDataBase==NULL||m_idDimStyle==0)
		return;
	pDataBase->GetDimStyle(m_idDimStyle,&dimStyleVar);
	dimStyleVar.m_idTextStyle=idTextStyle;
	pDataBase->SetDimStyle(m_idDimStyle,dimStyleVar);
}
void DRAWING_DIM_STYLE::UpdateDimStyle(IEntityDatabase* pDataBase,XERO_DIMSTYLEVAR var)
{
	if(pDataBase==NULL||m_idDimStyle==0)
		return;
	dimStyleVar=var;
	pDataBase->SetDimStyle(m_idDimStyle,dimStyleVar);
}
//////////////////////////////////////////////////////////////////////////
//
DRAWING_DIM_STYLE XeroDimStyleTable::dimStyle;
DRAWING_DIM_STYLE XeroDimStyleTable::dimStyleHoleSpace;
DRAWING_DIM_STYLE XeroDimStyleTable::dimStyleHoleSpace2;
void XeroDimStyleTable::InitDimStyle(IEntityDatabase* pDataBase)
{	//standard
	dimStyle.dimStyleName.Copy("standard");
	dimStyle.dimStyleVar.m_ciDimBlk=0;
	dimStyle.dimStyleVar.m_fArrowSize=100;
	dimStyle.dimStyleVar.m_fDimExtend=100;
	dimStyle.dimStyleVar.m_fTextHeight=200;
	dimStyle.dimStyleVar.m_fDimGap=5;
	dimStyle.m_idDimStyle=pDataBase->SetDimStyle(dimStyle.dimStyleName,dimStyle.dimStyleVar);
	//
	dimStyleHoleSpace.dimStyleName.Copy("HoleSpace");
	dimStyleHoleSpace.dimStyleVar.m_ciDimBlk=0;
	dimStyleHoleSpace.dimStyleVar.m_fArrowSize=5;
	dimStyleHoleSpace.dimStyleVar.m_fDimExtend=5;
	dimStyleHoleSpace.dimStyleVar.m_fTextHeight=30;
	dimStyleHoleSpace.dimStyleVar.m_fDimGap=3;
	dimStyleHoleSpace.dimStyleVar.m_ciVerticalPosType=1;//默认文字位于标注线上方 
	dimStyleHoleSpace.m_idDimStyle=pDataBase->SetDimStyle(dimStyleHoleSpace.dimStyleName,dimStyleHoleSpace.dimStyleVar);
	dimStyleHoleSpace2=dimStyleHoleSpace;
	dimStyleHoleSpace2.dimStyleName.Copy("HoleSpace2");
	dimStyleHoleSpace2.dimStyleVar.m_ciVerticalPosType=2;//默认文字位于标注线外部(主要用于角钢下侧X肢上的螺栓间距横向标注) wjh-2017.11.28
	dimStyleHoleSpace2.m_idDimStyle=pDataBase->SetDimStyle(dimStyleHoleSpace2.dimStyleName,dimStyleHoleSpace2.dimStyleVar);
}

void XeroDimStyleTable::InitDimStyle2(IEntityDatabase* pDataBase)
{	//standard
	dimStyle.dimStyleName.Copy("standard");
	dimStyle.dimStyleVar.m_ciDimBlk=0;
	dimStyle.dimStyleVar.m_fArrowSize=1;
	dimStyle.dimStyleVar.m_fDimExtend=1;
	dimStyle.dimStyleVar.m_fTextHeight=2;
	dimStyle.dimStyleVar.m_fDimGap=0.5;
	dimStyle.m_idDimStyle=pDataBase->SetDimStyle(dimStyle.dimStyleName,dimStyle.dimStyleVar);
	//
	dimStyleHoleSpace.dimStyleName.Copy("HoleSpace");
	dimStyleHoleSpace.dimStyleVar.m_ciDimBlk=0;
	dimStyleHoleSpace.dimStyleVar.m_fArrowSize=1;
	dimStyleHoleSpace.dimStyleVar.m_fDimExtend=1;
	dimStyleHoleSpace.dimStyleVar.m_fTextHeight=2;
	dimStyleHoleSpace.dimStyleVar.m_fDimGap=0.5;
	dimStyleHoleSpace.m_idDimStyle=pDataBase->SetDimStyle(dimStyleHoleSpace.dimStyleName,dimStyleHoleSpace.dimStyleVar);
	dimStyleHoleSpace2=dimStyleHoleSpace;
	dimStyleHoleSpace2.dimStyleName.Copy("HoleSpace2");
	dimStyleHoleSpace2.dimStyleVar.m_ciVerticalPosType=2;//默认文字位于标注线外部(主要用于角钢下侧X肢上的螺栓间距横向标注) wjh-2017.11.28
	dimStyleHoleSpace2.m_idDimStyle=pDataBase->SetDimStyle(dimStyleHoleSpace2.dimStyleName,dimStyleHoleSpace2.dimStyleVar);
}
void XeroDimStyleTable::UpdateDimStyle(IEntityDatabase* pDataBase,double text_size,double arrow_size/*=1*/,
								   double dim_extend/*=1*/,double dim_gap/*=0.5*/,char ciDimBlk/*=0*/)
{
	dimStyle.dimStyleName.Copy("standard");
	dimStyle.dimStyleVar.m_ciDimBlk=ciDimBlk;
	dimStyle.dimStyleVar.m_fArrowSize=arrow_size;
	dimStyle.dimStyleVar.m_fDimExtend=dim_extend;
	dimStyle.dimStyleVar.m_fTextHeight=text_size;
	dimStyle.dimStyleVar.m_fDimGap=dim_gap;
	dimStyle.m_idDimStyle=pDataBase->SetDimStyle(dimStyle.dimStyleName,dimStyle.dimStyleVar);
}