#pragma once
#ifndef __CAD_TOOL_FUNC_H_
#define __CAD_TOOL_FUNC_H_

#include "f_ent.h"
#include "f_ent_list.h"
#include "f_alg_fun.h"
#include "XhCharString.h"
#include "HashTable.h"
#include "ArrayList.h"
#include "TblDef.h"
#include "LayerTable.h"
#include "DimStyle.h"
#include "PathManager.h"

//////////////////////////////////////////////////////////////////////////
// �ĵ�����
class CLockDocumentLife
{
public:
	CLockDocumentLife() {
		acDocManager->lockDocument(acDocManager->curDocument());
	}
	~CLockDocumentLife() {
		acDocManager->unlockDocument(acDocManager->curDocument());
	}
};
#ifdef _ARX_2007
void SendCommandToCad(ACHAR* sCmd);
#else
void SendCommandToCad(ACHAR* sCmd);
#endif
//////////////////////////////////////////////////////////////////////////
//CAD��ʵ���������ڿ���
class AcDbObject;
class CAcDbObjLife
{
	AcDbObject *pCadDbObj;
public:
	CAcDbObjLife(AcDbObject* pDbObj){pCadDbObj=pDbObj;}
	~CAcDbObjLife(){
		if(pCadDbObj)
			pCadDbObj->close();
	}
};
//���β���Ҫ��ͼ��
class CShieldCadLayer
{
private:
	BOOL m_bSendCommand;
public:
	CXhChar50 sCurLayer;
public:
	void InitCurLayer();
	CShieldCadLayer(const char* sReservedLayerName=NULL,BOOL bFreeze=FALSE,BOOL bSendCommand=FALSE);
	~CShieldCadLayer();
};
//
class CNewDatabaseLife
{
	AcDbDatabase newDb;
	AcDbDatabase* pOldDb;
public:
	CNewDatabaseLife();
	~CNewDatabaseLife();
	//
	BOOL SaveAs(const char* sFullPath);
};
AcDbObjectId AddEntityToDbs(AcDbEntity *pEntity,AcDbBlockTableRecord *pTemRecord=NULL);
class CBlockTableRecordLife
{
	AcDbBlockTableRecord *m_pBlockTblRec;
public:
	CBlockTableRecordLife(AcDbBlockTableRecord *pBlockTblRec) {
		m_pBlockTblRec = pBlockTblRec;
	}
	~CBlockTableRecordLife() {
		if (m_pBlockTblRec)
			m_pBlockTblRec->close();
	}
	AcDbBlockTableRecord *get_BlockTblRec() { return m_pBlockTblRec; }
	__declspec(property(get = get_BlockTblRec))	AcDbBlockTableRecord *pBlockTblRec;
};
//
AcDbDatabase *GetCurDwg();
void InitDrawingEnv();
AcDbBlockTableRecord *GetBlockTableRecord();
AcDbObjectId SearchBlock(const char *blkname);
int GetNearestACI(COLORREF color);	//��RGB�õ�cad��ɫ����ֵ
COLORREF GetColorFromIndex(int color_index);//��CAD����ɫ�õ�RGB
int GetEntColorIndex(AcDbEntity *pEnt);		//��ȡCADʵ����ɫ����

//���CADʵ��ͼԪ
AcDbObjectId CreateAcadLine(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint start, f3dPoint end, long handle = NULL, long style = 0, COLORREF clr=-1);
AcDbObjectId CreateAcadRect(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint topLeft,double rect_width,
	double rect_height,BOOL bHasBulge=FALSE,THANDLE handle=NULL,
	f3dPoint waist_vec=f3dPoint(0,0,0),double line_width=0);
AcDbObjectId CreateAcadPolyLineArrow(AcDbBlockTableRecord *pBlockTableRecord, f3dPoint start,
	f3dPoint end, double line_width, double arrow_width, COLORREF clr, THANDLE handle = NULL);
AcDbObjectId CreateAcadPolyLine(AcDbBlockTableRecord *pBlockTableRecord, f3dPoint* vertex_arr,
	int vertex_num, double line_width, THANDLE handle=NULL);
AcDbObjectId CreateAcadHatch(AcDbBlockTableRecord *pBlockTableRecord, fPtList &vertexList, char *sHatchStyle,
	double scale, double angle = 0, COLORREF color = RGB(255, 255, 255));
AcDbObjectId CreateAcadCircle(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint centre, double radius, THANDLE handle = NULL,COLORREF clr=-1);
//����Բ����㣬�յ��Լ��뾶����Բ�� wht 10-08-18
AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint center, f3dPoint startPoint, f3dPoint endPoint, THANDLE handle = NULL);
//����Բ�ģ���ʼ�㼰���ν�����һ��Բ��
AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint center, f3dPoint startPoint, double sectorAngle, THANDLE handle = NULL);
AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint center, f3dPoint startPoint, double sectorAngle, f3dPoint worknorm, THANDLE handle = NULL);
AcDbObjectId CreateAcadEllipseLine(AcDbBlockTableRecord *pBlockTableRecord, f3dPoint center,
	f3dPoint normal, f3dPoint majorAxis, double radiusRatio, double startAngle, double endAngle, THANDLE handle = NULL);
AcDbObjectId CreateAcadEllipseLine(AcDbBlockTableRecord *pBlockTableRecord, f3dArcLine arcline, THANDLE handle = NULL);
BOOL InsertBlock(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint insert_pos, char *blkname,
	double scale = 1, double angle = 0,
	long handle = 0, AcDbObjectId *pOutEntId = NULL);
//���CAD��ע
AcDbObjectId 
	DimText(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint base,//��ע��ʼ/��ֹ��
	const char *dimText,//�ߴ���λ�ü�����
	AcDbObjectId textStyleId,	//=NULL��ע��ʽ��¼ID��
	double height,double rotation,
	AcDb::TextHorzMode hMode=AcDb::kTextLeft,AcDb::TextVertMode vMode=AcDb::kTextBase,
	AcDbObjectId layerId=AcDbObjectId::kNull,COLORREF clr=-1);
AcDbObjectId
DimMText(AcDbBlockTableRecord *pBlockTableRecord,//����¼ָ��
	char *dimText,//�ߴ���λ�ü�����
	f3dPoint base,//��ע��ʼ/��ֹ��
	double width,	//ÿ�е�����
	double texheight,//���ָ�
	AcDbObjectId textStyleId,	//=NULL��ע��ʽ��¼ID��
	AcDbMText::AttachmentPoint align_type,	//���ֶ��뷽ʽ
	AcDbMText::FlowDirection flowDirection, double angle = 0);
//����ת�Ǳ�ע
AcDbObjectId DimAngleSize(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint start, f3dPoint end,//��ע��ʼ/��ֹ��
	f3dPoint dimpos, const char *dimText,//�ߴ���λ�ü�����
	AcDbObjectId dimStyleId,	//��ע��ʽ��¼ID��
	double angle, double txt_height = 0);	//, BOOL bInside
AcDbObjectId
DimSize(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint start, f3dPoint end,//��ע��ʼ/��ֹ��
	f3dPoint dimpos, const char *dimText,//�ߴ���λ�ü�����
	AcDbObjectId dimStyleId,	//=NULL��ע��ʽ��¼ID��
	int dimtype, double txt_height = 0/*,BOOL bInside*/);	//=0 ˮƽ =1 ��ֱ =2��б

//��ͼ�����������
double TestDrawTextLength(const char* dimtext, double height, AcDbObjectId textStyleId, double widthFactor=0);
BOOL GetAcDbTextRect(AcDbText *pText, f3dPoint rgnVertArr[4]);
BOOL GetCadTextEntPos(AcDbText *pText, GEPOINT &pos, bool bCorrectPos = false);
BOOL VerifyVertexByCADEnt(SCOPE_STRU &scope, AcDbEntity *pEnt);
BOOL VerifyVertexByCADEntId(SCOPE_STRU &scope, AcDbObjectId entId);
f2dRect GetCadEntRect(ARRAY_LIST<ULONG> &idList, double extendLen = 0);
f2dRect GetCadEntRect(ARRAY_LIST<AcDbObjectId> &entIdList, double extendLen = 0);
f2dRect GetCadEntRect(CHashSet<AcDbObjectId> &screenEntSet, double extendLen = 0);
extern int ZOOM_LEFT_FIXED_WIDTH;	//����ʱ���Ԥ���Ĺ̶�ֵ����ģ̬�Ի�����ʽ��ʾ����Ԥ����� wht 19-06-28
extern int ZOOM_RIGHT_FIXED_WIDTH;	//����ʱ�Ҳ�Ԥ���Ĺ̶�ֵ
void ZoomAcadView(AcGePoint2d Pt1, AcGePoint2d Pt2, double scale = 1.0);
void ZoomAcadView(f2dRect rect, double extendLen, double scale = 1.0);
void ZoomAcadView(SCOPE_STRU scope, double extendLen, double scale = 1.0);
void ZoomAcadView(CHashSet<AcDbObjectId> &screenEntSet,double extendLen=5,double scale=1.0);
void ZoomAcadView(ARRAY_LIST<AcDbObjectId> &entIdList, double extendLen = 5, double scale = 1.0);
//��ñ��Ԫ����
BOOL GetGridKey(AcDbEntity* pPoint, GRID_DATA_STRU *grid_data);
//��קͼ�μ���
#ifdef __DRAG_ENT_
#include "DragEntSet.h"
int DragEntSet(ads_point base,char *prompt);
#endif

void MoveEntIds(ARRAY_LIST<ULONG> &idList, GEPOINT &fromPt, GEPOINT &toPoint, bool bUpdateDisplay);
void MoveEntIds(ARRAY_LIST<AcDbObjectId> &entIdList, GEPOINT &fromPt, GEPOINT &toPoint, bool bUpdateDisplay);
//���ѡ��ʵ��Ϊdxf�ļ�
void SaveAsDxf(const char* sDxfFilePath, ARRAY_LIST<AcDbObjectId> &entIdList, bool bMoveToOrigin);
//
CXhChar50 GetLineTypeName(AcDbObjectId lineTypeId);
AcDbObjectId GetLineTypeId(AcDbEntity *pEnt);
#endif