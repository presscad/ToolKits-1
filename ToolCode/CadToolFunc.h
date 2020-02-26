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
// 文档锁定
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
//CAD的实体声明周期控制
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
//屏蔽不需要的图层
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
int GetNearestACI(COLORREF color);	//从RGB得到cad颜色索引值
COLORREF GetColorFromIndex(int color_index);//从CAD的颜色得到RGB
int GetEntColorIndex(AcDbEntity *pEnt);		//获取CAD实体颜色索引

//添加CAD实体图元
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
//根据圆弧起点，终点以及半径生成圆弧 wht 10-08-18
AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint center, f3dPoint startPoint, f3dPoint endPoint, THANDLE handle = NULL);
//根据圆心，起始点及扇形角生成一段圆弧
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
//添加CAD标注
AcDbObjectId 
	DimText(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint base,//标注起始/终止点
	const char *dimText,//尺寸线位置及内容
	AcDbObjectId textStyleId,	//=NULL标注样式记录ID号
	double height,double rotation,
	AcDb::TextHorzMode hMode=AcDb::kTextLeft,AcDb::TextVertMode vMode=AcDb::kTextBase,
	AcDbObjectId layerId=AcDbObjectId::kNull,COLORREF clr=-1);
AcDbObjectId
DimMText(AcDbBlockTableRecord *pBlockTableRecord,//块表记录指针
	char *dimText,//尺寸线位置及内容
	f3dPoint base,//标注起始/终止点
	double width,	//每行的最大宽
	double texheight,//文字高
	AcDbObjectId textStyleId,	//=NULL标注样式记录ID号
	AcDbMText::AttachmentPoint align_type,	//文字对齐方式
	AcDbMText::FlowDirection flowDirection, double angle = 0);
//生成转角标注
AcDbObjectId DimAngleSize(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint start, f3dPoint end,//标注起始/终止点
	f3dPoint dimpos, const char *dimText,//尺寸线位置及内容
	AcDbObjectId dimStyleId,	//标注样式记录ID号
	double angle, double txt_height = 0);	//, BOOL bInside
AcDbObjectId
DimSize(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint start, f3dPoint end,//标注起始/终止点
	f3dPoint dimpos, const char *dimText,//尺寸线位置及内容
	AcDbObjectId dimStyleId,	//=NULL标注样式记录ID号
	int dimtype, double txt_height = 0/*,BOOL bInside*/);	//=0 水平 =1 垂直 =2倾斜

//视图区域操作函数
double TestDrawTextLength(const char* dimtext, double height, AcDbObjectId textStyleId, double widthFactor=0);
BOOL GetAcDbTextRect(AcDbText *pText, f3dPoint rgnVertArr[4]);
BOOL GetCadTextEntPos(AcDbText *pText, GEPOINT &pos, bool bCorrectPos = false);
BOOL VerifyVertexByCADEnt(SCOPE_STRU &scope, AcDbEntity *pEnt);
BOOL VerifyVertexByCADEntId(SCOPE_STRU &scope, AcDbObjectId entId);
f2dRect GetCadEntRect(ARRAY_LIST<ULONG> &idList, double extendLen = 0);
f2dRect GetCadEntRect(ARRAY_LIST<AcDbObjectId> &entIdList, double extendLen = 0);
f2dRect GetCadEntRect(CHashSet<AcDbObjectId> &screenEntSet, double extendLen = 0);
extern int ZOOM_LEFT_FIXED_WIDTH;	//缩放时左侧预留的固定值，以模态对话框形式显示，需预留宽度 wht 19-06-28
extern int ZOOM_RIGHT_FIXED_WIDTH;	//缩放时右侧预留的固定值
void ZoomAcadView(AcGePoint2d Pt1, AcGePoint2d Pt2, double scale = 1.0);
void ZoomAcadView(f2dRect rect, double extendLen, double scale = 1.0);
void ZoomAcadView(SCOPE_STRU scope, double extendLen, double scale = 1.0);
void ZoomAcadView(CHashSet<AcDbObjectId> &screenEntSet,double extendLen=5,double scale=1.0);
void ZoomAcadView(ARRAY_LIST<AcDbObjectId> &entIdList, double extendLen = 5, double scale = 1.0);
//获得表格单元数据
BOOL GetGridKey(AcDbEntity* pPoint, GRID_DATA_STRU *grid_data);
//拖拽图形集合
#ifdef __DRAG_ENT_
#include "DragEntSet.h"
int DragEntSet(ads_point base,char *prompt);
#endif

void MoveEntIds(ARRAY_LIST<ULONG> &idList, GEPOINT &fromPt, GEPOINT &toPoint, bool bUpdateDisplay);
void MoveEntIds(ARRAY_LIST<AcDbObjectId> &entIdList, GEPOINT &fromPt, GEPOINT &toPoint, bool bUpdateDisplay);
//另存选中实体为dxf文件
void SaveAsDxf(const char* sDxfFilePath, ARRAY_LIST<AcDbObjectId> &entIdList, bool bMoveToOrigin);
//
CXhChar50 GetLineTypeName(AcDbObjectId lineTypeId);
AcDbObjectId GetLineTypeId(AcDbEntity *pEnt);
#endif