#include "StdAfx.h"
#include "CadToolFunc.h"
#include "ArrayList.h"
#include "DimStyle.h"
#include "LayerTable.h"
#include "PathManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

enum LDSPART_TYPE {
	TA_NODE = 0,		// �ڵ�
	STEELTYPE_ANGLE = 1,		// �Ǹ�
	STEELTYPE_BOLT = 2,		// ��˨
	STEELTYPE_PLATE = 3,		// �ְ�
	STEELTYPE_TUBE = 4,		// �ֹ�
	STEELTYPE_FLAT = 5,		// ����
	STEELTYPE_SLOT = 6,		// �۸�
	TA_OTHER = 100,
};

//////////////////////////////////////////////////////////////////////////
//
#ifdef _ARX_2007
void SendCommandToCad(ACHAR *sCmd)
#else
void SendCommandToCad(ACHAR *sCmd)
#endif
#endif
{
	size_t len = _tcslen(sCmd) + 1;
	if (len <= 0)
		return;
	COPYDATASTRUCT cmd_msg;
	cmd_msg.dwData = (DWORD)1;
	cmd_msg.cbData = len * sizeof(ACHAR);
	cmd_msg.lpData = sCmd;
#ifdef _ARX_2007
	SendMessageW(adsw_acadMainWnd(), WM_COPYDATA, (WPARAM)adsw_acadMainWnd(), (LPARAM)&cmd_msg);
#else
	SendMessage(adsw_acadMainWnd(), WM_COPYDATA, (WPARAM)adsw_acadMainWnd(), (LPARAM)&cmd_msg);
#endif
}

//////////////////////////////////////////////////////////////////////////
//ACAD RGB��ACI��Ӧ���� ��ʱʹ������ acdbGetRGB ������
typedef struct tagAcadRGB
{
	BYTE R,G,B;
}ACAD_RGB;
static ACAD_RGB AcadRGB[256]=
{
	0,0,0,
	255,0,0,
	255,255,0,
	0,255,0,
	0,255,255,
	0,0,255,
	255,0,255,
	255,255,255,
	128,128,128,
	192,192,192,
	255,0,0,
	255,127,127,
	165,0,0,
	165,82,82,
	127,0,0,
	127,63,63,
	76,0,0,
	76,38,38,
	38,0,0,
	38,19,19,
	255,63,0,
	255,159,127,
	165,41,0,
	165,103,82,
	127,31,0,
	127,79,63,
	76,19,0,
	76,47,38,
	38,9,0,
	38,23,19,
	255,127,0,
	255,191,127,
	165,82,0,
	165,124,82,
	127,63,0,
	127,95,63,
	76,38,0,
	76,57,38,
	38,19,0,
	38,28,19,
	255,191,0,
	255,223,127,
	165,124,0,
	165,145,82,
	127,95,0,
	127,111,63,
	76,57,0,
	76,66,38,
	38,28,0,
	38,33,19,
	255,255,0,
	255,255,127,
	165,165,0,
	165,165,82,
	127,127,0,
	127,127,63,
	76,76,0,
	76,76,38,
	38,38,0,
	38,38,19,
	191,255,0,
	223,255,127,
	124,165,0,
	145,165,82,
	95,127,0,
	111,127,63,
	57,76,0,
	66,76,38,
	28,38,0,
	33,38,19,
	127,255,0,
	191,255,127,
	82,165,0,
	124,165,82,
	63,127,0,
	95,127,63,
	38,76,0,
	57,76,38,
	19,38,0,
	28,38,19,
	63,255,0,
	159,255,127,
	41,165,0,
	103,165,82,
	31,127,0,
	79,127,63,
	19,76,0,
	47,76,38,
	9,38,0,
	23,38,19,
	0,255,0,
	127,255,127,
	0,165,0,
	82,165,82,
	0,127,0,
	63,127,63,
	0,76,0,
	38,76,38,
	0,38,0,
	19,38,19,
	0,255,63,
	127,255,159,
	0,165,41,
	82,165,103,
	0,127,31,
	63,127,79,
	0,76,19,
	38,76,47,
	0,38,9,
	19,38,23,
	0,255,127,
	127,255,191,
	0,165,82,
	82,165,124,
	0,127,63,
	63,127,95,
	0,76,38,
	38,76,57,
	0,38,19,
	19,38,28,
	0,255,191,
	127,255,223,
	0,165,124,
	82,165,145,
	0,127,95,
	63,127,111,
	0,76,57,
	38,76,66,
	0,38,28,
	19,38,33,
	0,255,255,
	127,255,255,
	0,165,165,
	82,165,165,
	0,127,127,
	63,127,127,
	0,76,76,
	38,76,76,
	0,38,38,
	19,38,38,
	0,191,255,
	127,223,255,
	0,124,165,
	82,145,165,
	0,95,127,
	63,111,127,
	0,57,76,
	38,66,76,
	0,28,38,
	19,33,38,
	0,127,255,
	127,191,255,
	0,82,165,
	82,124,165,
	0,63,127,
	63,95,127,
	0,38,76,
	38,57,76,
	0,19,38,
	19,28,38,
	0,63,255,
	127,159,255,
	0,41,165,
	82,103,165,
	0,31,127,
	63,79,127,
	0,19,76,
	38,47,76,
	0,9,38,
	19,23,38,
	0,0,255,
	127,127,255,
	0,0,165,
	82,82,165,
	0,0,127,
	63,63,127,
	0,0,76,
	38,38,76,
	0,0,38,
	19,19,38,
	63,0,255,
	159,127,255,
	41,0,165,
	103,82,165,
	31,0,127,
	79,63,127,
	19,0,76,
	47,38,76,
	9,0,38,
	23,19,38,
	127,0,255,
	191,127,255,
	82,0,165,
	124,82,165,
	63,0,127,
	95,63,127,
	38,0,76,
	57,38,76,
	19,0,38,
	28,19,38,
	191,0,255,
	223,127,255,
	124,0,165,
	145,82,165,
	95,0,127,
	111,63,127,
	57,0,76,
	66,38,76,
	28,0,38,
	33,19,38,
	255,0,255,
	255,127,255,
	165,0,165,
	165,82,165,
	127,0,127,
	127,63,127,
	76,0,76,
	6,38,76,
	38,0,38,
	38,19,38,
	255,0,191,
	255,127,223,
	165,0,124,
	165,82,145,
	127,0,95,
	127,63,111,
	76,0,57,
	76,38,66,
	38,0,28,
	38,19,33,
	255,0,127,
	255,127,191,
	165,0,82,
	165,82,124,
	127,0,63,
	127,63,95,
	76,0,38,
	76,38,57,
	38,0,19,
	38,19,28,
	255,0,63,
	255,127,159,
	165,0,41,
	165,82,103,
	127,0,31,
	127,63,79,
	76,0,19,
	76,38,47,
	38,0,9,
	38,19,23,
	0,0,0,
	51,51,51,
	91,91,91,
	132,132,132,
	173,173,173,
	214,214,214,
};
//��RGB�õ�cad��ɫ����ֵ 
int GetNearestACI(COLORREF color)
{
	long min_dist = 2147483647L;
	long dist = 0;
	int min_index = 0;
	long red=GetRValue(color);
	long green=GetGValue(color);
	long blue=GetBValue(color);
	for(int i=1;i<256;i++)
	{
		/*long aci_rgb,r,g,b;
		aci_rgb=acdbGetRGB(i);
		r=GetRValue(aci_rgb);
		g=GetGValue(aci_rgb);
		b=GetBValue(aci_rgb);
		dist=abs(r-red)+abs(g-green)+abs(b-blue);*/
		dist=abs(AcadRGB[i].R-red)+abs(AcadRGB[i].G-green)+abs(AcadRGB[i].B-blue);
		if(dist<min_dist) 
		{
			min_index = i;
			min_dist = dist;
		}
	}
	return min_index;
}
//��CAD����ɫ�õ�RGB
COLORREF GetColorFromIndex(int color_index)
{
	/*if(colorIndex < 0 || colorIndex > 255)
	{
		ads_alert("�������ɫ�Ų���0~255֮��!");
		return 0;
	}
	BYTE R, G, B;
#ifdef ARX_2002_dll
	R = lpszRGBData[colorIndex*3+0];
	G = lpszRGBData[colorIndex*3+1];
	B = lpszRGBData[colorIndex*3+2];
#else
	long zhi = acdbGetRGB(colorIndex);
	WORD LOW = LOWORD(zhi);
	WORD HIG = HIWORD(zhi);
	R = LOBYTE(LOW);
	G = HIBYTE(LOW);
	B = LOBYTE(HIG);
#endif
	return RGB(R,G,B);*/
	if(color_index<1||color_index>255)
		return RGB(255,0,0);
	else
		return RGB(AcadRGB[color_index].R,AcadRGB[color_index].G,AcadRGB[color_index].B);
}
//��ȡCADʵ����ɫ����
int GetEntColorIndex(AcDbEntity *pEnt)
{
	if (pEnt == NULL)
		return 0;
	// Color (0=BYBLOCK, 256=BYLAYER, negative=layer is turned off)
	int iColor = pEnt->colorIndex();
	if (0 == iColor)
	{	//���
		//AcDbBlockTableRecord *pBlockTblRec=NULL;
		//acdbOpenObject(pBlockTblRec,pEnt->blockId(),AcDb::kForRead);
	}
	else if (256 == iColor)
	{	//���
		AcDbLayerTableRecord *pLayerTableRecord = NULL;
		acdbOpenObject(pLayerTableRecord, pEnt->layerId(), AcDb::kForRead);
		if (pLayerTableRecord)
		{
			pLayerTableRecord->close();
			iColor = pLayerTableRecord->color().colorIndex();
		}
	}
	return iColor;
}
//////////////////////////////////////////////////////////////////////////
//CAD�����Ĳ���������ͼ�����ʽ
//////////////////////////////////////////////////////////////////////////
//CShieldCadLayer
void CShieldCadLayer::InitCurLayer()
{
	struct resbuf result;
#ifdef _ARX_2007
	acedGetVar(_T(L"CLAYER"),&result);	//ȡ�õ�ǰͼ��
	if(strlen(_bstr_t(result.resval.rstring))>0)
		sCurLayer.Copy(_bstr_t(result.resval.rstring));
#else
	acedGetVar(_T("CLAYER"),&result);	//ȡ�õ�ǰͼ��
	if(strlen(result.resval.rstring)>0)
		sCurLayer.Copy(result.resval.rstring);
#endif
	else
		sCurLayer.Copy(LayerTable::VisibleProfileLayer.layerName);
}
//δָ������ͼ��ʱ��������ǰͼ��,δ�ҵ���ǰͼ��ʱ�����ɼ�������ͼ��
CShieldCadLayer::CShieldCadLayer(const char* sReservedLayerName/*=NULL*/,BOOL bFreeze/*=FALSE*/,BOOL bSendCommand/*=FALSE*/)
{
	CLockDocumentLife lockLife;
	m_bSendCommand = bSendCommand;
	CXhChar50 sLayerName(sReservedLayerName);
	if (sCurLayer.GetLength() <= 0)
		InitCurLayer();
	if(sLayerName.GetLength()<=0)
		sLayerName.Copy(sCurLayer);
	CXhChar50 sCmd1("-layer s %s\n ", (char*)sCurLayer);
	CXhChar50 sCmd2("-layer F %s\n ", (char*)sLayerName);
	CXhChar50 sCmd3("-layer s %s\n ", (char*)sLayerName);
	CXhChar50 sCmd4("-layer F *\n ");
#ifdef _ARX_2007
	if (m_bSendCommand)
	{
		SendCommandToCad(L"undo be \n");
		if (bFreeze)
		{
			SendCommandToCad((ACHAR*)_bstr_t(sCmd1));
			SendCommandToCad((ACHAR*)_bstr_t(sCmd2));
		}
		else
		{
			SendCommandToCad((ACHAR*)_bstr_t(sCmd3));
			SendCommandToCad((ACHAR*)_bstr_t(sCmd4));
		}
		SendCommandToCad(L"undo e ");
	}
	else
	{
		acedCommand(RTSTR,L"undo",RTSTR,L"be",RTNONE);
		if(bFreeze)
		{	//����ָ��ͼ��
			acedCommand(RTSTR,L"layer",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)sCurLayer),RTSTR,"",RTNONE);
			acedCommand(RTSTR,L"layer",RTSTR,L"F",RTSTR,(ACHAR*)_bstr_t((char*)sLayerName),RTSTR,L"",RTNONE);
		}
		else
		{
			acedCommand(RTSTR,L"layer",RTSTR,L"s",RTSTR,(ACHAR*)_bstr_t((char*)sLayerName),RTSTR,"",RTNONE);
			acedCommand(RTSTR,L"layer",RTSTR,L"F",RTSTR,L"*",RTSTR,L"",RTNONE);
		}
		acedCommand(RTSTR,L"undo",RTSTR,L"e",RTNONE);
	}
#else
	if (bSendCommand)
	{
		SendCommandToCad("undo be ");
		if (bFreeze)
		{
			SendCommandToCad(sCmd1);
			SendCommandToCad(sCmd2);
		}
		else
		{
			SendCommandToCad(sCmd3);
			SendCommandToCad(sCmd4);
		}
		SendCommandToCad("undo e ");
	}
	else
	{
		int retCode = acedCommand(RTSTR, "undo", RTSTR, "be", RTNONE);
		if (bFreeze)
		{	//����ָ��ͼ��
			acedCommand(RTSTR, "layer", RTSTR, "s", RTSTR, (char*)sCurLayer, RTSTR, "", RTNONE);
			acedCommand(RTSTR, "layer", RTSTR, "F", RTSTR, (char*)sLayerName, RTSTR, "", RTNONE);
		}
		else
		{
			acedCommand(RTSTR, "layer", RTSTR, "s", RTSTR, (char*)sLayerName, RTSTR, "", RTNONE);
			acedCommand(RTSTR, "layer", RTSTR, "F", RTSTR, "*", RTSTR, "", RTNONE);
		}
		acedCommand(RTSTR, "undo", RTSTR, "e", RTNONE);
	}
#endif
}
CShieldCadLayer::~CShieldCadLayer()
{
	if (m_bSendCommand)
	{	//�ⶳ����ͼ��
#ifdef _ARX_2007
		SendCommandToCad(L"undo be ");
		SendCommandToCad(L"-layer T *\n ");
		SendCommandToCad(L"undo e ");
#else
		SendCommandToCad("undo be ");
		SendCommandToCad("-layer T *\n ");
		SendCommandToCad("undo e ");
#endif
	}
	else
	{
		//�ⶳ����ͼ��
#ifdef _ARX_2007
		acedCommand(RTSTR, L"undo", RTSTR, L"be", RTNONE);
		acedCommand(RTSTR, L"layer", RTSTR, L"T", RTSTR, L"*", RTSTR, L"", RTNONE);
		acedCommand(RTSTR, L"undo", RTSTR, L"e", RTNONE);
#else
		acedCommand(RTSTR, "undo", RTSTR, "be", RTNONE);
		acedCommand(RTSTR, "layer", RTSTR, "T", RTSTR, "*", RTSTR, "", RTNONE);
		acedCommand(RTSTR, "undo", RTSTR, "e", RTNONE);
#endif
	}
}

//��ʼ����ͼ����
void InitDrawingEnv()
{
	AcDbObjectId layerId;
	GetCurDwg()->setLtscale(20);
	LayerTable::InitLayerTable();
	TextStyleTable::InitTextStyleTable();
	DimStyleTable::InitDimStyle();
}
AcDbDatabase *GetCurDwg()
{
	return acdbHostApplicationServices()->workingDatabase();
}
CNewDatabaseLife::CNewDatabaseLife()
{
	pOldDb=GetCurDwg();
	newDb.setLtscale(20);
	newDb.setExtmin(AcGePoint3d(-300,600,0));
	newDb.setExtmax(AcGePoint3d(300,-600,0));
	newDb.setWorldUcsBaseOrigin(AcGePoint3d(0,0,0),AcDb::OrthographicView::kNonOrthoView);
	acdbHostApplicationServices()->setWorkingDatabase(&newDb);
}
CNewDatabaseLife::~CNewDatabaseLife()
{
	newDb.closeInput();
	acdbHostApplicationServices()->setWorkingDatabase(pOldDb);
}
BOOL CNewDatabaseLife::SaveAs(const char* sFullPath)
{
#ifdef _ARX_2007
	if(newDb.saveAs((ACHAR*)_bstr_t(sFullPath))!=Acad::eOk)
#else
	if(newDb.saveAs(sFullPath)!=Acad::eOk)
#endif
		return FALSE;
	else
		return TRUE;
}
AcDbObjectId AddEntityToDbs(AcDbEntity *pEntity,AcDbBlockTableRecord *pTemRecord/*=NULL*/)
{
	AcDbObjectId entityId;
	if(pTemRecord!=NULL)
#ifdef __DRAG_ENT_
		DRAGSET.AppendAcDbEntity(pTemRecord, entityId, pEntity);
#else
		pTemRecord->appendAcDbEntity(entityId, pEntity);
#endif
	else
	{
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
#ifdef __DRAG_ENT_
		DRAGSET.AppendAcDbEntity(pBlockTableRecord, entityId, pEntity);
#else
		pBlockTableRecord->appendAcDbEntity(entityId, pEntity);
#endif
		pBlockTableRecord->appendAcDbEntity(entityId, pEntity);
		pBlockTableRecord->close();
	}
	pEntity->close();
	return entityId;
}

static AcDbBlockTableRecord *fpBlockTableRecord = NULL;
AcDbBlockTableRecord *GetBlockTableRecord()
{
	//��õ�ǰͼ�εĿ��ָ��
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable, AcDb::kForRead);
	//��õ�ǰͼ�ο���¼ָ��
	AcDbBlockTableRecord *pBlockTableRecord;//�������¼ָ��
	//��д��ʽ��ģ�Ϳռ䣬��ÿ���¼ָ��
	Acad::ErrorStatus es = pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
	if (es != Acad::eOk)
	{
		pBlockTableRecord = fpBlockTableRecord;
		AfxMessageBox(CXhChar50("%d", es));
	}
	else
		fpBlockTableRecord = pBlockTableRecord;
	pBlockTable->close();//�رտ��
	return pBlockTableRecord;
}
AcDbObjectId SearchBlock(const char *blkname)
{
	AcDbBlockTable *pBT;
	AcDbObjectId blockId;
	AcDbBlockTableRecord *pBTRecord;
	GetCurDwg()->getBlockTable(pBT,AcDb::kForRead);
#ifdef _ARX_2007
	if(pBT->getAt((ACHAR*)_bstr_t(blkname),pBTRecord,AcDb::kForRead)!=Acad::eOk)
#else
	if(pBT->getAt(blkname,pBTRecord,AcDb::kForRead)!=Acad::eOk)
#endif
	{
		pBT->close();
		return NULL;
	}
	else
	{
		pBT->close();
		blockId = pBTRecord->objectId();
		pBTRecord->close();
		return blockId;
	}
}
//��ȡ���峤��
double DrawTextLength(const char* dimtext,double height,AcDbObjectId textStyleId)
{
	AcDbMText mtxt;
#ifdef _ARX_2007
	mtxt.setContents((ACHAR*)_bstr_t(dimtext));				//�������ֱ�ע����
#else
	mtxt.setContents(dimtext);				//�������ֱ�ע����
#endif
	mtxt.setWidth(strlen(dimtext)*height);					//ÿ�����ֵ������
	mtxt.setTextHeight(height);
	mtxt.setTextStyle(textStyleId);		//���ֲ����
	return mtxt.actualWidth();
}
//////////////////////////////////////////////////////////////////////////
//���CADͼԪʵ��
//////////////////////////////////////////////////////////////////////////
//���ֱ��ͼԪ
AcDbObjectId CreateAcadLine(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint start, f3dPoint end,long handle/*=NULL*/,long style/*=0*/,
	COLORREF clr/*=-1*/)
{
	AcDbObjectId LineId=0;//�����ʶ��
	AcGePoint3d acad_start,acad_end;
	start.z = end.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	if(start==end)
		return LineId;	//ʼ�յ��غ�
	Cpy_Pnt(acad_start,start);
	Cpy_Pnt(acad_end,end);
	AcDbLine *pLine=new AcDbLine(acad_start,acad_end);//����LINE����
	if (clr != -1)
	{	//������ɫ����
		int color_index = GetNearestACI(clr);
		pLine->setColorIndex(color_index);
	}
#ifdef __DRAG_ENT_
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,LineId,pLine))	//��ʵ��д�����¼
#else
	if(pBlockTableRecord->appendAcDbEntity(LineId,pLine)==Acad::eOk)
#endif
	{
		if(handle!=NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId,xrecObjId;
			pLine->createExtensionDictionary();
			dictObjId = pLine->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();

			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle,AcDb::kDxfInt32, style,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pLine->close();//���ùرն���ĳ�Ա����
	}
	ads_name ent_name;
	acdbGetAdsName(ent_name,LineId);
	acdbEntUpd(ent_name);
	return LineId;
}
AcDbObjectId CreateAcadRect(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint topLeft,double rect_width,
					double rect_height,BOOL bHasBulge/*=FALSE*/,THANDLE handle/*=NULL*/
					,f3dPoint waist_vec/*=f3dPoint(0,0,0)*/,double line_width/*=0*/)
{
	int i=0;
	AcDbObjectId LineId;//�����ʶ��
	f3dPoint rectVertex[4];
	AcGePoint2d acad_vertex[4];
	double offset_dist=0;
	if(bHasBulge) //���Ǿ���
		offset_dist=0.4*rect_height;
	if(waist_vec.IsZero())
	{
		rectVertex[0].Set(topLeft.x+offset_dist,topLeft.y,0);
		rectVertex[1].Set(topLeft.x+offset_dist,topLeft.y-rect_height);
		rectVertex[2].Set(topLeft.x+rect_width-offset_dist,topLeft.y-rect_height);
		rectVertex[3].Set(topLeft.x+rect_width-offset_dist,topLeft.y);
	}
	else
	{	//��Բ����
		f3dPoint vec(-waist_vec.y,waist_vec.x);	//vec��ֱ����Բ����
		rectVertex[0]=topLeft+waist_vec*offset_dist;
		rectVertex[1]=topLeft+waist_vec*offset_dist-vec*rect_height;
		rectVertex[2]=topLeft+waist_vec*(rect_width-offset_dist)-vec*rect_height;
		rectVertex[3]=topLeft+waist_vec*(rect_width-offset_dist);
	}
	AcDbPolyline *pPolyLine=new AcDbPolyline(4);//����LINE����
	pPolyLine->setClosed(true);
	for(i=0;i<4;i++)
	{
		acad_vertex[i].x=rectVertex[i].x;
		acad_vertex[i].y=rectVertex[i].y;
		if(bHasBulge&&(i==1||i==3))
			pPolyLine->addVertexAt(0,acad_vertex[i],-1,line_width,line_width);
		else
			pPolyLine->addVertexAt(0,acad_vertex[i],0,line_width,line_width);
	}
#ifdef __DRAG_ENT_
	if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,LineId,pPolyLine))//��ʵ��д�����¼
#else
	if(pBlockTableRecord->appendAcDbEntity(LineId,pPolyLine)==Acad::eOk)
#endif
	{
		if(handle!=NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId,xrecObjId;
			pPolyLine->createExtensionDictionary();
			dictObjId = pPolyLine->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();
			
			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle,0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pPolyLine->close();//���ùرն���ĳ�Ա����
	}
	ads_name ent_name;
	acdbGetAdsName(ent_name,LineId);
	acdbEntUpd(ent_name);
	return LineId;
}

AcDbObjectId CreateAcadPolyLineArrow(AcDbBlockTableRecord *pBlockTableRecord, f3dPoint start,
									 f3dPoint end, double line_width, double arrow_width, COLORREF clr, THANDLE handle/*=NULL*/)
{
	AcDbObjectId LineId = 0;//�����ʶ��
	AcDbPolyline *pPolyLine = new AcDbPolyline(3);//����LINE����
	
	f3dPoint line_vec = start - end;
	normalize(line_vec);
	double len = DISTANCE(start, end);
	double arrow_len = (arrow_width*1.5);
	if (arrow_len > len)
		arrow_len = 0.5*len;
	f3dPoint arrow_pt = end + line_vec * arrow_len;
	AcGePoint2d acad_pnt;
	acad_pnt.x = start.x;
	acad_pnt.y = start.y;
	pPolyLine->addVertexAt(0, acad_pnt, 0, line_width, line_width);
	acad_pnt.x = arrow_pt.x;
	acad_pnt.y = arrow_pt.y;
	pPolyLine->addVertexAt(1, acad_pnt, 0, arrow_width, 0);
	acad_pnt.x = end.x;
	acad_pnt.y = end.y;
	pPolyLine->addVertexAt(2, acad_pnt, 0, 0, 0);
	int color_index = GetNearestACI(clr);
	pPolyLine->setColorIndex(color_index);	//������ɫ����

#ifdef __DRAG_ENT_
	if (DRAGSET.AppendAcDbEntity(pBlockTableRecord, LineId, pPolyLine))//��ʵ��д�����¼
#else
	if (pBlockTableRecord->appendAcDbEntity(LineId, pPolyLine) == Acad::eOk)
#endif

	{
		if (handle != NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId, xrecObjId;
			pPolyLine->createExtensionDictionary();
			dictObjId = pPolyLine->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();

			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle, 0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pPolyLine->close();//���ùرն���ĳ�Ա����
	}
	ads_name ent_name;
	acdbGetAdsName(ent_name, LineId);
	acdbEntUpd(ent_name);
	return LineId;
}

AcDbObjectId CreateAcadPolyLine(AcDbBlockTableRecord *pBlockTableRecord, f3dPoint* vertex_arr,
	int vertex_num, double line_width, THANDLE handle/*=NULL*/)
{
	AcDbObjectId LineId = 0;//�����ʶ��
	AcDbPolyline *pPolyLine = new AcDbPolyline(vertex_num);//����LINE����
	AcGePoint2d acad_pnt;
	for (int i = 0; i < vertex_num; i++)
	{
		acad_pnt.x = vertex_arr[i].x;
		acad_pnt.y = vertex_arr[i].y;
		pPolyLine->addVertexAt(i, acad_pnt, 0, line_width, line_width);
	}
#ifdef __DRAG_ENT_
	if (DRAGSET.AppendAcDbEntity(pBlockTableRecord, LineId, pPolyLine))//��ʵ��д�����¼
#else
	if (pBlockTableRecord->appendAcDbEntity(LineId, pPolyLine) == Acad::eOk)
#endif
	
	{
		if (handle != NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId, xrecObjId;
			pPolyLine->createExtensionDictionary();
			dictObjId = pPolyLine->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();

			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle, 0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pPolyLine->close();//���ùرն���ĳ�Ա����
	}
	ads_name ent_name;
	acdbGetAdsName(ent_name, LineId);
	acdbEntUpd(ent_name);
	return LineId;
}
//ͼ�����,�������谴˳ʱ�����ʱ�뷽������ wht 11-07-21
AcDbObjectId CreateAcadHatch(AcDbBlockTableRecord *pBlockTableRecord, fPtList &vertexList, char *sHatchStyle,
	double scale, double angle/*=0*/, COLORREF color/*=RGB(255,255,255)*/)
{
	if (vertexList.GetNodeNum() <= 0)
		return 0;
	AcDbHatch *pHatch = new AcDbHatch();
	pHatch->setNormal(AcGeVector3d(0, 0, 1));	//���÷���
	pHatch->setElevation(0.0);				//���û�ͼ���
	pHatch->setAssociative(Adesk::kFalse);
	pHatch->setPatternScale(scale);			//�������ű���
#ifdef _ARX_2007
	pHatch->setPattern(AcDbHatch::kPreDefined, (ACHAR*)_bstr_t(sHatchStyle));	//�������ͼ��
#else
	pHatch->setPattern(AcDbHatch::kPreDefined, sHatchStyle);				//�������ͼ��
#endif
	pHatch->setHatchStyle(AcDbHatch::kNormal);	//���������ʽ
	//
	AcGePoint2dArray ptArr;
	AcGeDoubleArray ptBulgesArr;
	ptArr.setLogicalLength(vertexList.GetNodeNum() + 1);
	ptBulgesArr.setLogicalLength(vertexList.GetNodeNum() + 1);
	int i = 0;
	f3dPoint *pPoint = NULL, *pFirstPt = NULL;
	for (pPoint = vertexList.GetFirst(); pPoint; pPoint = vertexList.GetNext())
	{
		if (pFirstPt == NULL)
			pFirstPt = pPoint;
		ptBulgesArr[i] = 0;
		ptArr[i].set(pPoint->x, pPoint->y);
		i++;
	}
	if (pFirstPt)
	{	//������ĩβ�ٴ���ӵ�һ���������γɷ������
		ptBulgesArr[i] = 0;
		ptArr[i].set(pFirstPt->x, pFirstPt->y);
	}
	pHatch->appendLoop(AcDbHatch::kDefault, ptArr, ptBulgesArr);
	pHatch->evaluateHatch();	//���趨����������ʽ���
	pHatch->setPatternAngle(angle);			//�������ͼ����ת�Ƕ�
	int color_index = GetNearestACI(color);
	pHatch->setColorIndex(color_index);	//������������ɫ����
	AcDbObjectId HatchId;//�����ʶ��
#ifdef __DRAG_ENT_
	if (DRAGSET.AppendAcDbEntity(pBlockTableRecord, HatchId, pHatch))//��ʵ��д�����¼
#else
	if (pBlockTableRecord->appendAcDbEntity(HatchId, pHatch) == Acad::eOk)
#endif
	{
		pHatch->close();//�رն���
		ads_name ent_name;
		acdbGetAdsName(ent_name, HatchId);
		acdbEntUpd(ent_name);
	}
	return HatchId;
}
AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint center, f3dPoint startPoint, f3dPoint endPoint, THANDLE handle/*=NULL*/)
{	//����Բ����㣬�յ��Լ��뾶����Բ��
	center.z = startPoint.z = endPoint.z = 0;
	f3dPoint start_line_vec = startPoint - center;
	f3dPoint end_line_vec = endPoint - center;
	double startAngle = Cal2dLineAng(f2dPoint(0, 0), f2dPoint(start_line_vec.x, start_line_vec.y));
	double endAngle = Cal2dLineAng(f2dPoint(0, 0), f2dPoint(end_line_vec.x, end_line_vec.y));
	double sectorAngle = endAngle - startAngle;
	return CreateAcadArcLine(pBlockTableRecord, center, startPoint, sectorAngle, f3dPoint(0, 0, 1), handle);
}

AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint center, f3dPoint startPoint, double sectorAngle, THANDLE handle/*=NULL*/)
{
	return CreateAcadArcLine(pBlockTableRecord, center, startPoint, sectorAngle, f3dPoint(0, 0, 1), handle);
}

AcDbObjectId CreateAcadArcLine(AcDbBlockTableRecord *pBlockTableRecord, f3dPoint center,
	f3dPoint startPoint, double sectorAngle, f3dPoint worknorm, THANDLE handle/*=NULL*/)
{
	//���������3������Բ����Բ�ģ��뾶����ʼ�Ƕȣ���ֹ�Ƕ�
	double radius, startAngle, endAngle;
	f2dLine lineStart, lineEnd;
	center.z = startPoint.z = 0;
	//��ʼ�Ƕȣ���ֹ�Ƕ�
	f3dPoint start_line_vec = startPoint - center;
	radius = start_line_vec.mod();
	startAngle = Cal2dLineAng(f2dPoint(0, 0), f2dPoint(start_line_vec.x, start_line_vec.y));
	if (worknorm.z < 0)
		startAngle = Pi - startAngle;
	if (startAngle < 0)
		startAngle += 2 * Pi;
	endAngle = startAngle + sectorAngle;

	AcDbObjectId ArcId;//�����ʶ��
	AcGePoint3d acad_center;
	AcGeVector3d normal;
	normal.set(worknorm.x, worknorm.y, worknorm.z);
	Cpy_Pnt(acad_center, center);
	AcDbArc *pArc = new AcDbArc(acad_center, normal, radius, startAngle, endAngle);//����ARC����
#ifdef __DRAG_ENT_
	if (DRAGSET.AppendAcDbEntity(pBlockTableRecord, ArcId, pArc))//��ʵ��д�����¼
#else
	if (pBlockTableRecord->appendAcDbEntity(ArcId, pArc) == Acad::eOk)
#endif
	{
		if (handle != NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId, xrecObjId;
			pArc->createExtensionDictionary();
			dictObjId = pArc->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();

			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle, 0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pArc->close();//���ùرն���ĳ�Ա����
	}
	/*ads_name ent_name;
	acdbGetAdsName(ent_name,ArcId);
	acdbEntUpd(ent_name);
	ads_name arc_name;
	acdbEntLast(arc_name);
	acdbEntUpd(arc_name);
	struct resbuf *rb = acdbEntGet(arc_name);
	acdbEntMod(rb);
	acutRelRb(rb);*/
	return ArcId;
}

AcDbObjectId CreateAcadEllipseLine(AcDbBlockTableRecord *pBlockTableRecord, f3dArcLine arcline, THANDLE handle/*=NULL*/)
{
	f3dPoint majorAxis, minorAxis = arcline.ColumnNorm() ^ arcline.WorkNorm();
	majorAxis.Set(-minorAxis.y, minorAxis.x, minorAxis.z);
	normalize(majorAxis);
	double radiusRatio = fabs(arcline.ColumnNorm()*arcline.WorkNorm());
	f3dPoint start_vec, end_vec;
	double start_angle = 0, end_angle = 0;
	start_vec = arcline.Start() - arcline.Center();
	normalize(start_vec);
	if (arcline.SectorAngle() < Pi + Pi - EPS)
	{
		end_vec = arcline.End() - arcline.Center();
		normalize(end_vec);
		double start_cosa = start_vec * majorAxis;
		double end_cosa = end_vec * majorAxis;
		//������ʼ�������᷽����ͬ����� wht 10-08-31
		start_angle = ACOS(start_cosa);
		end_angle = ACOS(end_cosa);
		start_vec = majorAxis ^ start_vec;
		end_vec = majorAxis ^ end_vec;
		if (start_vec*arcline.WorkNorm() < 0)
			start_angle = 2 * Pi - start_angle;
		if (end_vec*arcline.WorkNorm() < 0)
			end_angle = 2 * Pi - end_angle;
	}
	else
	{
		end_vec = start_vec;
		start_angle = 0;
		end_angle = Pi + Pi;
	}
	majorAxis *= arcline.Radius() / radiusRatio;
	return CreateAcadEllipseLine(pBlockTableRecord, arcline.Center(), arcline.WorkNorm(), majorAxis, radiusRatio, start_angle, end_angle, handle);
}
AcDbObjectId CreateAcadEllipseLine(AcDbBlockTableRecord *pBlockTableRecord, f3dPoint center,
	f3dPoint normal, f3dPoint majorAxis, double radiusRatio, double startAngle, double endAngle, THANDLE handle/*=NULL*/)
{
	//���������3������Բ����Բ�ģ��뾶����ʼ�Ƕȣ���ֹ�Ƕ�
	center.z = 0;

	AcDbObjectId ArcId;//�����ʶ��
	AcGePoint3d acad_center;
	AcGeVector3d acad_normal, acad_majorAxis;
	acad_normal.set(normal.x, normal.y, normal.z);
	acad_majorAxis.set(majorAxis.x, majorAxis.y, majorAxis.z);
	Cpy_Pnt(acad_center, center);
	AcDbEllipse *pEllipse = new AcDbEllipse(acad_center, acad_normal, acad_majorAxis, radiusRatio, startAngle, endAngle);//����ARC����
#ifdef __DRAG_ENT_	
		if (DRAGSET.AppendAcDbEntity(pBlockTableRecord, ArcId, pEllipse))//��ʵ��д�����¼
#else
		if (pBlockTableRecord->appendAcDbEntity(ArcId, pEllipse) == Acad::eOk)
#endif
	{
		if (handle != NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId, xrecObjId;
			pEllipse->createExtensionDictionary();
			dictObjId = pEllipse->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();

			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle, 0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pEllipse->close();//���ùرն���ĳ�Ա����
	}
	/*ads_name ent_name;
	acdbGetAdsName(ent_name,ArcId);
	acdbEntUpd(ent_name);
	ads_name arc_name;
	acdbEntLast(arc_name);
	acdbEntUpd(arc_name);
	struct resbuf *rb = acdbEntGet(arc_name);
	acdbEntMod(rb);
	acutRelRb(rb);*/
	return ArcId;
}
//////////////////////////////////////////////////////////////////////////
//���CAD��ע��
//////////////////////////////////////////////////////////////////////////
//��������ע
AcDbObjectId 
	DimText(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint base,//��ע��ʼ/��ֹ��
	const char *dimText,//�ߴ���λ�ü�����
	AcDbObjectId textStyleId,	//=NULL��ע��ʽ��¼ID��
	double height,double rotation,
	AcDb::TextHorzMode hMode,AcDb::TextVertMode vMode,
	AcDbObjectId layerId/*=AcDbObjectId::kNull*/,//=NULLͼ��ID��
	COLORREF clr/*=-1*/)
{
	AcGePoint3d acad_base;
	AcDbText *pText;
	base.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	if(layerId.isValid())
		GetCurDwg()->setClayer(layerId);
	GetCurDwg()->setTextstyle(textStyleId);
	Cpy_Pnt(acad_base,base);
	//����ע�Ͷ���
#ifdef _ARX_2007
	pText=new AcDbText(acad_base,(ACHAR*)_bstr_t(dimText),textStyleId,height,rotation);
#else
	pText=new AcDbText(acad_base,dimText,textStyleId,height,rotation);
#endif
	if (clr != -1)
	{	//������ɫ����
		int color_index = GetNearestACI(clr);
		pText->setColorIndex(color_index);
	}
	pText->setHorizontalMode(hMode);
	pText->setVerticalMode(vMode);
	if(hMode!=AcDb::kTextLeft||vMode!=AcDb::kTextBase)
		pText->setAlignmentPoint(acad_base);
	pText->setHeight(height);
	pText->setWidthFactor(0.7);
	AcDbObjectId TextId;//�����ʶ��
#ifdef __DRAG_ENT_
	if (DRAGSET.AppendAcDbEntity(pBlockTableRecord, TextId, pText))
#else
	if (pBlockTableRecord->appendAcDbEntity(TextId, pText) == Acad::eOk)	//��ʵ��д�����¼
#endif
		pText->close();//�رն���
	ads_name ent_name;
	acdbGetAdsName(ent_name,TextId);
	acdbEntUpd(ent_name);
	return TextId;
}
////////////////////////////////////////////////////////////////////////////
//��ͼ�����������
//////////////////////////////////////////////////////////////////////////
double TestDrawTextLength(const char* dimtext, double height, AcDbObjectId textStyleId, double widthFactor/*=0*/)
{
	AcDbMText mtxt;
#ifdef _ARX_2007
	mtxt.setContents((ACHAR*)_bstr_t(dimtext));				//�������ֱ�ע����
#else
	mtxt.setContents(dimtext);				//�������ֱ�ע����
#endif
	mtxt.setWidth(strlen(dimtext)*height);					//ÿ�����ֵ������
	mtxt.setTextHeight(height);
	mtxt.setTextStyle(textStyleId);		//���ֲ����
	double width = mtxt.actualWidth();
	if (widthFactor > 0)
	{
		double fTextStyleXScale = 0.7;
		AcDbTextStyleTableRecord *pTextStyleTblRec = NULL;
		acdbOpenObject(pTextStyleTblRec, textStyleId, AcDb::kForRead);
		if (pTextStyleTblRec)
		{
			fTextStyleXScale=pTextStyleTblRec->xScale();
			pTextStyleTblRec->close();//�ر�������ʽ��
			//���������ע��ʽ��߱��������߱ȼ�����ʵ���� wht 19-12-09
			if (fTextStyleXScale > 0)
				width = (width / fTextStyleXScale)*widthFactor;
		}
	}
	return width;
}
int LocalCalArcResolution(double radius, double sector_angle, double scale_of_user2scr, double sample_len, int smoothness/*=36*/)
{
	double length = sector_angle * radius;
	smoothness = max(6, smoothness);
	int n = ftoi((length / scale_of_user2scr) / sample_len);
	int min_n = ftoi(smoothness*sector_angle / Pi) / 2;
	if (min_n == 0)
		min_n = 1;
	int max_n = ftoi(72 * sector_angle / Pi);
	n = max(n, min_n);
	n = min(n, max_n);
	return n;
}
//���ر�ע������(������ע��ͷ)����rgnVertArr[0];����rgnVertArr[1];����rgnVertArr[2];�����ĸ��ǵ�rgnVertArr[3]
BOOL GetAcDbTextRect(AcDbText *pText, f3dPoint rgnVertArr[4])
{
	if (pText == NULL)
		return FALSE;
	CXhChar100 sText;
#ifdef _ARX_2007
	sText.Copy(_bstr_t(pText->textString()));
#else
	sText.Copy(pText->textString());
#endif
	AcGePoint3d cad_pos;
	f3dPoint text_pos, left_btm, text_vec, text_top_vec;
	double fTestH = pText->height();
	double fWidth = TestDrawTextLength(sText, fTestH, pText->textStyle());
	AcDb::TextHorzMode  horzMode = pText->horizontalMode();
	AcDb::TextVertMode  vertMode = pText->verticalMode();
	//��ȡAcDbText����� wht 18-12-20
	//If vertical mode is AcDb::kTextBase and horizontal mode is either AcDb::kTextLeft, AcDb::kTextAlign, or AcDb::kTextFit,
	//then the position point is the insertion point for the text object and, for AcDb::kTextLeft, 
	//the alignment point (DXF group code 11) is automatically calculated based on the other parameters in the text object.
	if (vertMode == AcDb::kTextBase)
	{
		if (horzMode == AcDb::kTextLeft || horzMode == AcDb::kTextAlign || horzMode == AcDb::kTextFit)
			cad_pos = pText->position();
		else
			cad_pos = pText->alignmentPoint();
	}
	else
		cad_pos = pText->alignmentPoint();
	text_pos.Set(cad_pos.x, cad_pos.y, 0);
	text_vec.Set(cos(pText->rotation()), sin(pText->rotation()));
	text_top_vec.Set(-text_vec.y, text_vec.x);
	left_btm.Set(text_pos.x, text_pos.y, text_pos.z);
	//
	if (horzMode == AcDb::kTextLeft)
		left_btm = text_pos;
	else if (horzMode == AcDb::kTextRight)
		left_btm = text_pos - text_vec * fWidth;
	else if (horzMode == AcDb::kTextMid)
		left_btm = text_pos - text_vec * (fWidth*0.5);
	if (vertMode == AcDb::kTextTop)
		left_btm -= text_top_vec * fTestH;
	else if (vertMode == AcDb::kTextVertMid)
		left_btm -= text_top_vec * (fTestH*0.5);
	//
	rgnVertArr[0] = left_btm;
	rgnVertArr[1] = left_btm + text_vec * fWidth;
	rgnVertArr[2] = left_btm + text_vec * fWidth + text_top_vec * fTestH;
	rgnVertArr[3] = left_btm + text_top_vec * fTestH;
	return TRUE;
}
BOOL GetCadTextEntPos(AcDbText *pText, GEPOINT &pos, bool bCorrectPos /*= false*/)
{
	if (pText == NULL)
		return FALSE;
	CXhChar200 sText;
#ifdef _ARX_2007
	sText.Copy(_bstr_t(pText->textString()));
#else
	sText.Copy(pText->textString());
#endif
	AcGePoint3d org_txt_pos = pText->position();//alignmentPoint();
	AcGePoint3d org_align_pos = pText->alignmentPoint();
	AcGePoint3d txt_pos = org_txt_pos;
	if (bCorrectPos)
	{
		//TMA���ɸְ����ͼ֮�󣬿�ѡ��һ���ļ����Ƶ�����һ���ļ�ʱ���ᵼ��position��alignmentPoint��һ��
		//λ�ò�һ�£��޸����Ժ�ᴥ��adjustAlignment,������������λ��ƫ�ƣ��˴�����adjustAlignment����posλ�ã�
		//�ٸ���ԭʼ��positionλ������alignmentPoint wht 19-01-12
		pText->adjustAlignment();	//����adjustAlignment����pos��alignmentPointһ��
		AcGePoint3d txt_pos = pText->position();//alignmentPoint();
		AcGePoint3d align_pos = pText->alignmentPoint();
		AcGePoint3d offset_pos;
		Sub_Pnt(offset_pos, align_pos, txt_pos);
		pText->setPosition(org_txt_pos);
		Add_Pnt(align_pos, org_txt_pos, offset_pos);
		pText->setAlignmentPoint(align_pos);
	}
	
	//
	double fTestH = pText->height();
	double fWidth = TestDrawTextLength(sText, fTestH, pText->textStyle());
	AcDb::TextHorzMode  horzMode = pText->horizontalMode();
	AcDb::TextVertMode  vertMode = pText->verticalMode();
	//��ȡAcDbText����� wht 18-12-20
	//If vertical mode is AcDb::kTextBase and horizontal mode is either AcDb::kTextLeft, AcDb::kTextAlign, or AcDb::kTextFit,
	//then the position point is the insertion point for the text object and, for AcDb::kTextLeft, 
	//the alignment point (DXF group code 11) is automatically calculated based on the other parameters in the text object.
	if (vertMode == AcDb::kTextBase)
	{
		if (horzMode == AcDb::kTextLeft || horzMode == AcDb::kTextAlign || horzMode == AcDb::kTextFit)
			txt_pos = pText->position();
		else
			txt_pos = pText->alignmentPoint();
	}
	else
		txt_pos = pText->alignmentPoint();
	f3dPoint dim_pos, dim_vec;
	dim_vec.Set(cos(pText->rotation()), sin(pText->rotation()));
	dim_pos.Set(txt_pos.x, txt_pos.y, txt_pos.z);
	//
	if (horzMode == AcDb::kTextLeft)
		dim_pos += dim_vec * (fWidth*0.5);
	else if (horzMode == AcDb::kTextRight)
		dim_pos -= dim_vec * (fWidth*0.5);
	/*if(vertMode==AcDb::kTextTop)
		dim_pos-=fTestH*0.5;
	else if(vertMode==AcDb::kTextBottom)
		dim_pos+=fTestH*0.5;*/
	pos.Set(dim_pos.x,dim_pos.y,dim_pos.z);
	return true;
}
//����CADʵ��ID����scope wht 11-08-01
BOOL VerifyVertexByCADEntId(SCOPE_STRU &scope, AcDbObjectId entId)
{
	AcDbEntity *pEnt = NULL;
	acdbOpenObject(pEnt, entId, AcDb::kForRead);
	if (pEnt == NULL)
		return FALSE;
	BOOL bRetCode = VerifyVertexByCADEnt(scope, pEnt);
	pEnt->close();
	return bRetCode;
}
BOOL VerifyVertexByCADEnt(SCOPE_STRU &scope, AcDbEntity *pEnt)
{
	if(pEnt==NULL)
		return FALSE;
	if(pEnt->isKindOf(AcDbLine::desc()))
	{
		AcDbLine *pLine=(AcDbLine*)pEnt;
		scope.VerifyVertex(f3dPoint(pLine->startPoint().x,pLine->startPoint().y,0));
		scope.VerifyVertex(f3dPoint(pLine->endPoint().x,pLine->endPoint().y,0));
	}
	else if(pEnt->isKindOf(AcDbPoint::desc()))
	{	//��һ���������£�Ӧ������������չ����Χ wht 11-06-16
		AcDbPoint *pPoint=(AcDbPoint*)pEnt;
		f3dPoint base_pt;
		Cpy_Pnt(base_pt,pPoint->position());
		scope.VerifyVertex(f3dPoint(base_pt.x-5,base_pt.y,0));
		scope.VerifyVertex(f3dPoint(base_pt.x+5,base_pt.y,0));
		scope.VerifyVertex(f3dPoint(base_pt.x,base_pt.y-5,0));
		scope.VerifyVertex(f3dPoint(base_pt.x,base_pt.y+5,0));
	}
	else if (pEnt->isKindOf(AcDbArc::desc()))
	{	//���Բ��������
		AcDbArc* pArc = (AcDbArc*)pEnt;
		AcGePoint3d pt;
		f3dPoint startPt, endPt, center, norm;
		pArc->getStartPoint(pt);
		Cpy_Pnt(startPt, pt);
		pArc->getEndPoint(pt);
		Cpy_Pnt(endPt, pt);
		Cpy_Pnt(center, pArc->center());
		Cpy_Pnt(norm, pArc->normal());
		double radius = pArc->radius();
		double angle = (pArc->endAngle() - pArc->startAngle());
		if (radius > 0 && fabs(angle) > 0)
		{	//���˴����Բ��(���磺��ʱpEnt��һ����,����������ʾΪԲ��)
			f3dArcLine arcLine;
			arcLine.CreateMethod3(startPt, endPt, norm, radius, center);
			int nSlices = LocalCalArcResolution(arcLine.Radius(), arcLine.SectorAngle(), 1.0, 3.0, 10);
			double slice_angle = arcLine.SectorAngle() / nSlices;
			for (int i = 0; i <= nSlices; i++)
			{
				f3dPoint pt = arcLine.PositionInAngle(i*slice_angle);
				scope.VerifyVertex(pt);
			}
		}
	}
	else if (pEnt->isKindOf(AcDbEllipse::desc()))
	{
		AcDbEllipse *pEllipse = (AcDbEllipse*)pEnt;
		AcGePoint3d pt;
		AcGeVector3d minorAxis;
		f3dPoint startPt, endPt, center, min_vec, maj_vec, column_norm, work_norm;
		pEllipse->getStartPoint(pt);
		Cpy_Pnt(startPt, pt);
		pEllipse->getEndPoint(pt);
		Cpy_Pnt(endPt, pt);
		Cpy_Pnt(center, pEllipse->center());
		Cpy_Pnt(min_vec, pEllipse->minorAxis());
		Cpy_Pnt(maj_vec, pEllipse->majorAxis());
		Cpy_Pnt(work_norm, pEllipse->normal());
		double min_R = min_vec.mod();
		double maj_R = maj_vec.mod();
		double cosa = min_R / maj_R;
		double sina = SQRT(1 - cosa * cosa);
		column_norm = work_norm;
		RotateVectorAroundVector(column_norm, sina, cosa, min_vec);
		f3dArcLine arcLine;
		arcLine.CreateEllipse(center, startPt, endPt, column_norm, work_norm, min_R);
		int nSlices = LocalCalArcResolution(arcLine.Radius(), arcLine.SectorAngle(), 1.0, 3.0, 10);
		double slice_angle = arcLine.SectorAngle() / nSlices;
		for (int i = 0; i <= nSlices; i++)
		{
			f3dPoint pt = arcLine.PositionInAngle(i*slice_angle);
			scope.VerifyVertex(pt);
		}
	}
	else if(pEnt->isKindOf(AcDbCircle::desc()))
	{
		AcDbCircle *pCir=(AcDbCircle*)pEnt;
		AcGePoint3d point=pCir->center();
		scope.VerifyVertex(f3dPoint(point.x+pCir->radius(),point.y,0));
		scope.VerifyVertex(f3dPoint(point.x-pCir->radius(),point.y,0));
		scope.VerifyVertex(f3dPoint(point.x,point.y+pCir->radius(),0));
		scope.VerifyVertex(f3dPoint(point.x,point.y-pCir->radius(),0));
	}
	else if(pEnt->isKindOf(AcDbPolyline::desc()))
	{
		AcDbPolyline *pPolyLine=(AcDbPolyline*)pEnt;
		int i=0,nVertex=pPolyLine->numVerts();
		for(i=0;i<nVertex;i++)
		{
			AcGePoint3d point;
			pPolyLine->getPointAt(i,point);
			scope.VerifyVertex(f3dPoint(point.x,point.y,0));
		}
	}
	else if(pEnt->isKindOf(AcDbText::desc()))
	{	//��һ���������£�Ӧ������������չ����Χ wht 11-06-16
		AcDbText *pText=(AcDbText*)pEnt;
		f3dPoint base_pt; 
		Cpy_Pnt(base_pt,pText->position());
		f3dPoint rgnVertArr[4];
		if (GetAcDbTextRect(pText, rgnVertArr))
		{
			scope.VerifyVertex(rgnVertArr[0]);
			scope.VerifyVertex(rgnVertArr[1]);
			scope.VerifyVertex(rgnVertArr[2]);
			scope.VerifyVertex(rgnVertArr[3]);
		}
		else
		{
			scope.VerifyVertex(f3dPoint(base_pt.x - 5, base_pt.y, 0));
			scope.VerifyVertex(f3dPoint(base_pt.x + 5, base_pt.y, 0));
			scope.VerifyVertex(f3dPoint(base_pt.x, base_pt.y - 5, 0));
			scope.VerifyVertex(f3dPoint(base_pt.x, base_pt.y + 5, 0));
		}
	}
	else if(pEnt->isKindOf(AcDbRotatedDimension::desc()))
	{
		AcDbRotatedDimension *pDim=(AcDbRotatedDimension*)pEnt;
		AcGePoint3d point=pDim->xLine1Point();
		scope.VerifyVertex(f3dPoint(point.x,point.y,0));
		point=pDim->xLine2Point();
		scope.VerifyVertex(f3dPoint(point.x,point.y,0));
		point=pDim->dimLinePoint();
		scope.VerifyVertex(f3dPoint(point.x,point.y,0));
	}
	else if(pEnt->isKindOf(AcDbAlignedDimension::desc()))
	{
		AcDbAlignedDimension *pDim=(AcDbAlignedDimension*)pEnt;
		AcGePoint3d point=pDim->xLine1Point();
		scope.VerifyVertex(f3dPoint(point.x,point.y,0));
		point=pDim->xLine2Point();
		scope.VerifyVertex(f3dPoint(point.x,point.y,0));
		point=pDim->dimLinePoint();
		scope.VerifyVertex(f3dPoint(point.x,point.y,0));
	}
	return TRUE;
}
f2dRect GetCadEntRect(ARRAY_LIST<ULONG> &idList, double extendLen/*=0*/)
{
	ARRAY_LIST<AcDbObjectId> entIdList;
	for (ULONG *pId = idList.GetFirst(); pId; pId = idList.GetNext())
		entIdList.append(AcDbObjectId((AcDbStub*)*pId));
	return GetCadEntRect(entIdList, extendLen);
}
f2dRect GetCadEntRect(ARRAY_LIST<AcDbObjectId> &entIdList, double extendLen/*=0*/)
{
	SCOPE_STRU scope;
	scope.ClearScope();
	for (AcDbObjectId *pObjId = entIdList.GetFirst(); pObjId; pObjId = entIdList.GetNext())
		VerifyVertexByCADEntId(scope, *pObjId);
	f2dRect rc;
	rc.topLeft.Set(scope.fMinX - extendLen, scope.fMaxY + extendLen);
	rc.bottomRight.Set(scope.fMaxX + extendLen, scope.fMinY - extendLen);
	return rc;
}
f2dRect GetCadEntRect(CHashSet<AcDbObjectId> &screenEntSet, double extendLen/*=0*/)
{
	SCOPE_STRU scope;
	scope.ClearScope();
	AcDbObjectId objId;
	for (objId = screenEntSet.GetFirst(); objId; objId = screenEntSet.GetNext())
		VerifyVertexByCADEntId(scope, objId);
	f2dRect rc;
	rc.topLeft.Set(scope.fMinX - extendLen, scope.fMaxY + extendLen);
	rc.bottomRight.Set(scope.fMaxX + extendLen, scope.fMinY - extendLen);
	return rc;
}
void ZoomAcadView(SCOPE_STRU scope,double extendLen/*=100*/,double scale/*=1.0*/)
{
	if((scope.fMaxX-scope.fMinX)<=0||(scope.fMaxY-scope.fMinY)<=0)
		return;
	ads_point L_T,R_B;
	L_T[X]=scope.fMinX-extendLen;
	L_T[Y]=scope.fMaxY+extendLen;
	L_T[Z]=0;
	R_B[X]=scope.fMaxX+extendLen;
	R_B[Y]=scope.fMinY-extendLen;
	R_B[Z]=0;
	ZoomAcadView(AcGePoint2d(L_T[X],L_T[Y]),AcGePoint2d(R_B[X],R_B[Y]),scale);
}
void ZoomAcadView(f2dRect rect,double extendLen/*=100*/,double scale/*=1.0*/)
{
	if(rect.Width()<=0||rect.Height()<=0)
		return;
	ads_point L_T,R_B;
	L_T[X]=rect.topLeft.x-extendLen;
	L_T[Y]=rect.topLeft.y+extendLen;
	L_T[Z]=0;
	R_B[X]=rect.bottomRight.x+extendLen;
	R_B[Y]=rect.bottomRight.y-extendLen;
	R_B[Z]=0;
	ZoomAcadView(AcGePoint2d(L_T[X],L_T[Y]),AcGePoint2d(R_B[X],R_B[Y]),scale);
}
int ZOOM_LEFT_FIXED_WIDTH=0;		//����ʱ���Ԥ���Ĺ̶�ֵ����ģ̬�Ի�����ʽ��ʾ����Ԥ����� wht 19-06-28
int ZOOM_RIGHT_FIXED_WIDTH=0;	//����ʱ�Ҳ�Ԥ���Ĺ̶�ֵ
void ZoomAcadView(AcGePoint2d Pt1,AcGePoint2d Pt2,double scale/*=1.0*/)
{
	AcGePoint2d CenterPt;
	//��X�����Y�����غϣ���Ϊ���⣬������SetAcadView����
	if ((fabs(Pt1.x-Pt2.x)<EPS)||(fabs(Pt1.y-Pt2.y)<EPS))
		return;
	//ȷ�����������ֱ�Ϊ���ϽǺ����½�
	if (Pt1.x>Pt2.x) 
	{
		double tmp;
		tmp = Pt1.x;
		Pt1.x = Pt2.x;
		Pt2.x = tmp;
	}
	if (Pt2.y>Pt1.y) 
	{
		double tmp;
		tmp = Pt1.y;
		Pt1.y = Pt2.y;
		Pt2.y = tmp;
	}
	//��ȡ��ǰDwgView�ĳߴ�
	CRect CADrect;
	acedGetAcadDwgView()->GetClientRect(&CADrect);
	double width,height,ratio;
	LONG right = CADrect.right;
	LONG left = CADrect.left;
	if (ZOOM_LEFT_FIXED_WIDTH > 0)
		left += ZOOM_LEFT_FIXED_WIDTH;
	if (ZOOM_RIGHT_FIXED_WIDTH > 0)
		right -= ZOOM_RIGHT_FIXED_WIDTH;
	ratio = (double)(right-left)/(double)(CADrect.bottom-CADrect.top);
	if (fabs(ratio)<EPS)
		return;
	if ((Pt2.x-Pt1.x)/(Pt1.y-Pt2.y) > ratio) 
	{
		width = Pt2.x-Pt1.x;
		height = width/ratio;
	}
	else
	{
		height = Pt1.y-Pt2.y;
		width = height * ratio;
	}
	//���õ�ǰ��ͼ���ĵ�
	CenterPt.x = (Pt1.x+Pt2.x)/2;
	CenterPt.y = (Pt1.y+Pt2.y)/2;
	//�ı䵱ǰ��ͼ
	AcDbViewTableRecord pVwRec;
	pVwRec.setCenterPoint(CenterPt);
	pVwRec.setWidth(width * scale);
	pVwRec.setHeight(height * scale);
	AcDbViewport *pViewPort=NULL,viewPort;
	if (ZOOM_RIGHT_FIXED_WIDTH > 0 || ZOOM_LEFT_FIXED_WIDTH > 0)
	{
		AcDbObjectId viewportId = acedGetCurViewportObjectId();
		AcDbViewport *pCurViewPort = NULL;
		acdbOpenObject(pCurViewPort, viewportId, AcDb::kForRead);
		CAcDbObjLife dictLife(pCurViewPort);
		if (pCurViewPort)
		{
			double fViewWidth=pCurViewPort->width() - ZOOM_LEFT_FIXED_WIDTH - ZOOM_RIGHT_FIXED_WIDTH;
			viewPort.setWidth(fViewWidth);
			viewPort.setHeight(pCurViewPort->height());
			AcGePoint2d viewCenter=pCurViewPort->viewCenter();
			viewCenter.x -= ZOOM_RIGHT_FIXED_WIDTH;
			viewCenter.x += ZOOM_LEFT_FIXED_WIDTH;
			viewPort.setViewCenter(viewCenter);
			pViewPort = &viewPort;
		}
	}
	acedSetCurrentView( &pVwRec, pViewPort );
}
void ZoomAcadView(ARRAY_LIST<AcDbObjectId> &entIdList, double extendLen/*=5*/, double scale/*=1.0*/)
{
	CHashSet<AcDbObjectId> screenEntSet;
	for (AcDbObjectId *pEntId = entIdList.GetFirst(); pEntId; pEntId = entIdList.GetNext())
		screenEntSet.SetValue(pEntId->asOldId(), *pEntId);
	ZoomAcadView(screenEntSet, extendLen, scale);
}
void ZoomAcadView(CHashSet<AcDbObjectId> &screenEntSet,double extendLen/*=5*/,double scale/*=1.0*/)
{
	SCOPE_STRU scope;
	scope.ClearScope();
	AcDbObjectId objId;
	for(objId=screenEntSet.GetFirst();objId;objId=screenEntSet.GetNext())
		VerifyVertexByCADEntId(scope,objId);
	f2dRect rect;
	if(scope.wide()>0&&scope.high()>0)
	{
		rect.topLeft.Set(scope.fMinX,scope.fMaxY);
		rect.bottomRight.Set(scope.fMaxX,scope.fMinY);
	}
	else
	{
		rect.topLeft.Set(0,0);
		rect.bottomRight.Set(0,0);
	}
	ZoomAcadView(rect,extendLen,scale);
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
}
//��ñ��Ԫ����
BOOL GetGridKey(AcDbEntity* pPoint, GRID_DATA_STRU *grid_data)
{
	THANDLE handle = 0;
	resbuf *pRb, *pNextRb;
	AcDbXrecord *pXrec;
	AcDbDictionary *pDict;
	AcDbObjectId dictObjId, xrecObjId;
	f2dRect rect;

	dictObjId = pPoint->extensionDictionary();
	if (dictObjId != AcDbObjectId::kNull)
	{
		acdbOpenObject(pDict, dictObjId, AcDb::kForRead);
		CAcDbObjLife dictLife(pDict);
#ifdef _ARX_2007
		if (pDict->getAt(L"TOWER_XREC", (AcDbObject* &)pXrec, AcDb::kForRead) == Acad::eOk)
#else
		if (pDict->getAt("TOWER_XREC", (AcDbObject* &)pXrec, AcDb::kForRead) == Acad::eOk)
#endif
		{
			pXrec->rbChain(&pRb);
			pXrec->close();
			if (pRb == NULL)
				return FALSE;
			grid_data->data_type = pRb->resval.rlong;	//��������
			pNextRb = pRb->rbnext;
			if (pNextRb == NULL)
				return FALSE;
			grid_data->type_id = pNextRb->resval.rlong;	//����ID
			pNextRb = pNextRb->rbnext;
			if (pNextRb == NULL)
				return FALSE;
			grid_data->fTextHigh = pNextRb->resval.rreal;//���ָ߶�
			pNextRb = pNextRb->rbnext;
			if (pNextRb == NULL)
				return FALSE;
			grid_data->fHighToWide = pNextRb->resval.rreal;//���ֿ�߱�
			pNextRb = pNextRb->rbnext;
			if (pNextRb == NULL)
				return FALSE;
			grid_data->min_x = pNextRb->resval.rreal;
			pNextRb = pNextRb->rbnext;
			if (pNextRb == NULL)
				return FALSE;
			grid_data->min_y = pNextRb->resval.rreal;
			pNextRb = pNextRb->rbnext;
			if (pNextRb == NULL)
				return FALSE;
			grid_data->max_x = pNextRb->resval.rreal;
			pNextRb = pNextRb->rbnext;
			if (pNextRb == NULL)
				return FALSE;
			grid_data->max_y = pNextRb->resval.rreal;

			ads_relrb(pRb);
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
//��קͼ�μ���
//////////////////////////////////////////////////////////////////////////
ads_matrix     CTM;		      // Current Transformation Matrix 
ads_point Base; 		      /* Base point for xform */
void ads_subvec(ads_point ap, ads_point bp,ads_point dp)
{
	dp[X] = ap[X] - bp[X];
	dp[Y] = ap[Y] - bp[Y];
	dp[Z] = ap[Z] - bp[Z];
}
void ads_mat_x_vec(ads_matrix mat,ads_point  pin,ads_point  pout)
{
	int i;
	ads_point temp;

	for (i = X; i <= Z; i++)
		temp[i] = mat[i][X] * pin[X] +
		mat[i][Y] * pin[Y] +
		mat[i][Z] * pin[Z];
	memcpy(pout, temp, sizeof(ads_point));
}
int dragsampler(ads_point pt,ads_matrix mat)
{
	ads_point	    tp;
	// generate translation matrix
	ads_subvec(pt, Base, tp);
	ads_mat_x_vec(mat, tp, tp);
	mat[X][T] = tp[X];//X����ƫ�ƾ���
	mat[Y][T] = tp[Y];//Y����ƫ�ƾ���
	mat[Z][T] = tp[Z];//Z����ƫ�ƾ���

	memcpy(CTM, mat, sizeof(ads_matrix));
	return RTNORM;
}
#ifdef __DRAG_ENT_
int DragEntSet(ads_point base,char *prompt)
{
	ads_point result;
	Base[X] = base[X];
	Base[Y] = base[Y];
	Base[Z] = base[Z]=0;
	int nRetCode=0;
#ifdef _ARX_2007
	nRetCode=ads_draggen(DRAGSET.drag_ent_set,(ACHAR*)_bstr_t(prompt),0,dragsampler,result);
#else
	nRetCode=ads_draggen(DRAGSET.drag_ent_set,prompt,0,dragsampler,result);
#endif
	if(nRetCode==RTNORM)
		nRetCode=ads_xformss(DRAGSET.drag_ent_set, CTM);
	else
		result[X]=result[Y]=result[Z]=0.0;
	base[X] = result[X];
	base[Y] = result[Y];
	base[Z] = result[Z];
	//�޸���ͼλ�ú���Ҫ��ʱ���½���,����acedSSGet()���ܲ��ܻ�ȡ��ȷ��ʵ�弯 wht 11-06-25
	actrTransactionManager->flushGraphics();
	acedUpdateDisplay();
	return  nRetCode;
}
#endif

void MoveEntIds(ARRAY_LIST<ULONG> &idList, GEPOINT &fromPt, GEPOINT &toPoint, bool bUpdateDisplay)
{
	ARRAY_LIST<AcDbObjectId> entIdList;
	for (ULONG *pId = idList.GetFirst(); pId; pId = idList.GetNext())
		entIdList.append(AcDbObjectId((AcDbStub*)*pId));
	MoveEntIds(entIdList, fromPt, toPoint, bUpdateDisplay);
}

void MoveEntIds(ARRAY_LIST<AcDbObjectId> &entIdList, GEPOINT &fromPt, GEPOINT &toPoint, bool bUpdateDisplay)
{
	AcGeMatrix3d moveMat;
	ads_point ptFrom, ptTo;
	ptFrom[X] = fromPt.x;
	ptFrom[Y] = fromPt.y;
	ptFrom[Z] = 0;
	ptTo[X] = toPoint.x;
	ptTo[Y] = toPoint.y;
	ptTo[Z] = 0;
	moveMat.setToTranslation(AcGeVector3d(ptTo[X] - ptFrom[X], ptTo[Y] - ptFrom[Y], ptTo[Z] - ptFrom[Z]));
	AcDbEntity *pEnt = NULL;
	CLockDocumentLife lockDocLife;
	for (AcDbObjectId *pId = entIdList.GetFirst(); pId; pId = entIdList.GetNext())
	{
		acdbOpenAcDbEntity(pEnt, *pId, AcDb::kForWrite);
		CAcDbObjLife entLife(pEnt);
		if (pEnt == NULL)
			continue;
		pEnt->transformBy(moveMat);
	}
	//3.ˢ�½���
	if (bUpdateDisplay)
	{
		actrTransactionManager->flushGraphics();
		acedUpdateDisplay();
	}
}

void SaveAsDxf(const char* sDxfFilePath, ARRAY_LIST<AcDbObjectId> &entIdList, bool bMoveToOrigin)
{
	CLockDocumentLife lock;
	AcDbDatabase* pCur = acdbHostApplicationServices()->workingDatabase();
	AcDbDatabase* pOut = NULL;
	AcDbObjectIdArray outObjIds;
	for (AcDbObjectId *pId = entIdList.GetFirst(); pId; pId = entIdList.GetNext())
		outObjIds.append(*pId);
	f2dRect rect = GetCadEntRect(entIdList);
	if (bMoveToOrigin)
		MoveEntIds(entIdList, GEPOINT(rect.topLeft.x, rect.bottomRight.y), GEPOINT(0, 0), false);
	//ʹ��wblock���dxf�ļ�ʱ���Ƚ���ͼ��������Ҫ�������ٽ��д洢�����������ͼ����Ϊdxf�ļ���Ĭ����ͼ wht 19-03-02
	ZoomAcadView(entIdList);
	Acad::ErrorStatus es = pCur->wblock(pOut, outObjIds, AcGePoint3d(0, 0, 0));
	if (pOut)
	{
		pOut->setWorldview(true);
#ifdef _ARX_2007
		pOut->dxfOut(_bstr_t(sDxfFilePath));
#else
		es = pOut->dxfOut(sDxfFilePath);
#endif
		delete pOut;
	}
	//else 
	//	AfxMessageBox(CXhChar16("%d",es));
	if (bMoveToOrigin)
		MoveEntIds(entIdList, GEPOINT(0, 0), GEPOINT(rect.topLeft.x, rect.bottomRight.y), false);
}
AcDbObjectId GetLineTypeId(AcDbEntity *pEnt)
{
	//����ѡ��ʵ�壺���Ϲ��������Ƶ��ض�ͼ��
	CXhChar50 sLineTypeName;
#ifdef _ARX_2007
	ACHAR* sValue = new ACHAR[50];
	sValue = pEnt->linetype();
	sLineTypeName.Copy((char*)_bstr_t(sValue));
	delete[] sValue;
#else
	char *sValue = new char[50];
	sValue = pEnt->linetype();
	sLineTypeName.Copy(sValue);
	delete[] sValue;
#endif
	AcDbObjectId curLineId;
	if (stricmp(sLineTypeName, "ByLayer") == 0)
	{	//�������
		AcDbLayerTableRecord *pLayerTableRecord;
		acdbOpenObject(pLayerTableRecord, pEnt->layerId(), AcDb::kForRead);
		pLayerTableRecord->close();
		curLineId = pLayerTableRecord->linetypeObjectId();
	}
	else if (stricmp(sLineTypeName, "ByBlock") == 0)
		curLineId = 0;//lineTypeId;		//���ͼԪ����������ΪByBlock,�����;���ʵ��
	else
		curLineId = pEnt->linetypeId();
	return curLineId;
}
CXhChar50 GetLineTypeName(AcDbObjectId lineTypeId)
{
	AcDbLinetypeTableRecord *pLineTypeTblRec = NULL;
	acdbOpenObject(pLineTypeTblRec, lineTypeId, AcDb::kForRead);
	if (pLineTypeTblRec == NULL)
		return CXhChar50();
	pLineTypeTblRec->close();
	CXhChar50 sLineTypeName;
#ifdef _ARX_2007
	ACHAR* sValue = new ACHAR[50];
	pLineTypeTblRec->getName(sValue);
	sLineTypeName.Copy((char*)_bstr_t(sValue));
	delete[] sValue;
#else
	char *sValue = new char[50];
	pLineTypeTblRec->getName(sValue);
	sLineTypeName.Copy(sValue);
	delete[] sValue;
#endif
	return sLineTypeName;
}

BOOL InsertBlock(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint insert_pos, char *blkname,
	double scale, double angle,
	long handle/*=0*/, AcDbObjectId *pOutEntId/*=NULL*/)
{
	AcDbDatabase blkDb(Adesk::kFalse);//����յ����ݿ�
	AcDbObjectId newEntId, blockId;
	AcGePoint3d acad_insert_pos;
	AcDbBlockReference *pBlkRef = new AcDbBlockReference;
	blockId = SearchBlock(blkname);
	Cpy_Pnt(acad_insert_pos, insert_pos);
	if (blockId == NULL)
	{
		char filename[MAX_PATH] = {0};
		char temstr[MAX_PATH] = {0}, sys_path[MAX_PATH] = { 0 }, app_path[MAX_PATH] = { 0 };
		GetSysPath(sys_path);
		GetAppPath(app_path);
#ifdef __CNC_
		strcpy(filename, app_path);
		sprintf(temstr, "%s\\ProcessCardMap\\%s.dwg", blkname);
#else
		strcpy(filename, sys_path);
		sprintf(temstr, "%s.dwg", blkname);
#endif
		//TODO:��������ȷ�Ϻ����Ƿ���ڵĴ��룬FindFirstFile,FindNextFile wjh-2014.5.13
		long ll = (int)strlen(temstr);
		strcat(filename, temstr);
#ifdef _ARX_2007
		if (blkDb.readDwgFile((ACHAR*)_bstr_t(filename)) == Acad::eOk)
#else
		if (blkDb.readDwgFile(filename) == Acad::eOk)
#endif
		{
			AcDbDatabase *pTempDb;
			blkDb.wblock(pTempDb);
#ifdef _ARX_2007
			GetCurDwg()->insert(blockId, _bstr_t(blkname), pTempDb);
#else
			GetCurDwg()->insert(blockId, blkname, pTempDb);
#endif
			delete pTempDb;
		}
		else
		{
			pBlkRef->close();
			return FALSE;
		}
	}
	AcGeScale3d scaleXYZ(scale, scale, scale);
	pBlkRef->setBlockTableRecord(blockId);
	pBlkRef->setPosition(acad_insert_pos);
	pBlkRef->setRotation(angle);
	pBlkRef->setScaleFactors(scaleXYZ);
#ifdef __DRAG_ENT_
	if (DRAGSET.AppendAcDbEntity(pBlockTableRecord, newEntId, pBlkRef))
#else
	if (pBlockTableRecord->appendAcDbEntity(newEntId, pBlkRef) == Acad::eOk)
#endif
	{
		if (handle != NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId, xrecObjId;
			pBlkRef->createExtensionDictionary();
			dictObjId = pBlkRef->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();

			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle, AcDb::kDxfInt32, STEELTYPE_BOLT, 0);
			pXrec->setFromRbChain(*head);
			acutRelRb(head);
			pXrec->close();
		}
		pBlkRef->close();
	}
	ads_name ent_name;
	acdbGetAdsName(ent_name, newEntId);
	acdbEntUpd(ent_name);
	if (pOutEntId)	//��˨��Ӧ��ʵ��ID wht 11-06-20
		*pOutEntId = newEntId;
	return TRUE;
}


AcDbObjectId CreateAcadCircle(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint centre, double radius, THANDLE handle/*=NULL*/, COLORREF clr /*= -1*/)
{
	AcDbObjectId CircleId;//�����ʶ��
	AcGePoint3d acad_centre;
	AcGeVector3d norm(0, 0, 1);
	centre.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	Cpy_Pnt(acad_centre, centre);
	AcDbCircle *pCircle = new AcDbCircle(acad_centre, norm, radius);//����CIRCLE����
#ifdef __DRAG_ENT_
	if (DRAGSET.AppendAcDbEntity(pBlockTableRecord, CircleId, pCircle))//��ʵ��д�����¼
#else
	if (pBlockTableRecord->appendAcDbEntity(CircleId, pCircle) == Acad::eOk)
#endif
	
	{
		if (handle != NULL)
		{
			AcDbDictionary *pDict;
			AcDbObjectId dictObjId, xrecObjId;
			pCircle->createExtensionDictionary();
			dictObjId = pCircle->extensionDictionary();
			acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
			AcDbXrecord* pXrec = new AcDbXrecord;
#ifdef _ARX_2007
			pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
			pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
			pDict->close();

			struct resbuf *head;
			head = acutBuildList(AcDb::kDxfInt32, handle, AcDb::kDxfInt32, STEELTYPE_BOLT, 0);
			pXrec->setFromRbChain(*head);
			ads_relrb(head);
			pXrec->close();
		}
		if (clr != -1)
		{	//������ɫ����
			int color_index = GetNearestACI(clr);
			pCircle->setColorIndex(color_index);
		}
		pCircle->close();//���ùرն���ĳ�Ա����
	}
	ads_name ent_name;
	acdbGetAdsName(ent_name, CircleId);
	acdbEntUpd(ent_name);
	/*ads_name circle_name;
	acdbEntLast(circle_name);
	acdbEntUpd(circle_name);
	struct resbuf *rb = acdbEntGet(circle_name);
	acdbEntMod(rb);
	acutRelRb(rb);*/
	return CircleId;
}

AcDbObjectId
DimMText(AcDbBlockTableRecord *pBlockTableRecord,//����¼ָ��
	char *dimText,//�ߴ���λ�ü�����
	f3dPoint base,//��ע��ʼ/��ֹ��
	double width,	//ÿ�е�����
	double textheight,//���ָ�
	AcDbObjectId textStyleId,	//=NULL��ע��ʽ��¼ID��
	AcDbMText::AttachmentPoint align_type,	//���ֶ��뷽ʽ
	AcDbMText::FlowDirection flowDirection, double angle)
{
	AcDbMText *pText;
	AcGePoint3d acad_base;
	//����ע�Ͷ���
	base.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	Cpy_Pnt(acad_base, base);
	pText = new AcDbMText();
#ifdef _ARX_2007
	pText->setContents((ACHAR*)_bstr_t(dimText));				//�������ֱ�ע����
#else
	pText->setContents(dimText);				//�������ֱ�ע����
#endif
	pText->setLocation(acad_base);				//���ò����
	pText->setWidth(width);					//ÿ�����ֵ������
	pText->setTextHeight(textheight);
	pText->setTextStyle(textStyleId);		//���ֲ����
	pText->setAttachment(align_type);		//���ֶ��뷽ʽ
	pText->setFlowDirection(flowDirection);	//������������
	pText->setRotation(angle);	//������������

	AcDbObjectId TextId;//�����ʶ��
#ifdef __DRAG_ENT_
	if (DRAGSET.AppendAcDbEntity(pBlockTableRecord, TextId, pText))//��ʵ��д�����¼
#else
	if (pBlockTableRecord->appendAcDbEntity(TextId, pText) == Acad::eOk)
#endif
	{
		pText->close();//�رն���
		ads_name ent_name;
		acdbGetAdsName(ent_name, TextId);
		acdbEntUpd(ent_name);
	}

	return TextId;
}
//����ת�Ǳ�ע
AcDbObjectId DimAngleSize(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint start, f3dPoint end,//��ע��ʼ/��ֹ��
	f3dPoint dimpos, const char *dimText,//�ߴ���λ�ü�����
	AcDbObjectId dimStyleId,	//��ע��ʽ��¼ID��
	double angle, double txt_height/*=0,BOOL bInside*/)
{
	AcGePoint3d acad_start, acad_end, acad_dimpos;
	//double dd = DISTANCE(start,end);
	//ȥ��bInside����,dimStyle3ר������[��ǿ�ƽ��ߴ���ڳߴ���֮��]�ı�ע
	//���ʹ��ͬһ��ע��ʽ�����޸ĸ�����ʱ��Ӱ������ʹ�ø���ʽ�ı�ע  wht 11-07-12
	/*
	if(!bInside)
	{	//ֻ����Ҫ���ñ�ע��ʽʱ����Ҫ����dimStyleId������Ӧ�ı�ע��ʽ��ÿ�ζ��򿪻�Ӱ������ٶ� wht 11-07-07
		AcDbDimStyleTableRecord *pDimStyleRec=NULL;
		acdbOpenObject(pDimStyleRec,dimStyleId,AcDb::kForWrite);
		if(pDimStyleRec)
		{
			pDimStyleRec->setDimtix(Adesk::kFalse);	//ǿ�ƽ��ߴ���ڳߴ�����ڲ�
			pDimStyleRec->close();
		}
	}*/
	start.z = end.z = dimpos.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	Cpy_Pnt(acad_start, start);
	Cpy_Pnt(acad_end, end);
	Cpy_Pnt(acad_dimpos, dimpos);
	//GetCurDwg()->setDimstyle(dimStyleId);
	//����ע�Ͷ���
#ifdef _ARX_2007
	AcDbRotatedDimension *pDim = new AcDbRotatedDimension(angle, acad_start, acad_end, acad_dimpos, (ACHAR*)_bstr_t(dimText), dimStyleId);
#else
	AcDbRotatedDimension *pDim = new AcDbRotatedDimension(angle, acad_start, acad_end, acad_dimpos, dimText, dimStyleId);
#endif
	if (txt_height > 0)
		pDim->setDimtxt(txt_height);
	AcDbObjectId DimId;//�����ʶ��
#ifdef __DRAG_ENT_
	if (DRAGSET.AppendAcDbEntity(pBlockTableRecord, DimId, pDim))//��ʵ��д�����¼
#else
	if (pBlockTableRecord->appendAcDbEntity(DimId, pDim) == Acad::eOk)
#endif
	{
		pDim->close();//�رն���
		ads_name ent_name;
		acdbGetAdsName(ent_name, DimId);
		acdbEntUpd(ent_name);
	}
	return DimId;
}
AcDbObjectId
DimSize(AcDbBlockTableRecord *pBlockTableRecord,
	f3dPoint start, f3dPoint end,//��ע��ʼ/��ֹ��
	f3dPoint dimpos, const char *dimText,//�ߴ���λ�ü�����
	AcDbObjectId dimStyleId,	//=NULL��ע��ʽ��¼ID��
	int dimtype, double txt_height/*=0*//*,BOOL bInside*/)	//=0 ˮƽ =1 ��ֱ =2��б
{
	AcDbEntity *pDim;
	AcDbRotatedDimension* pRotateDim = NULL;
	AcDbAlignedDimension* pAlignedDim = NULL;
	//double dd = DISTANCE(start,end);
	//ȥ��bInside����,dimStyle3ר������[��ǿ�ƽ��ߴ���ڳߴ���֮��]�ı�ע
	//���ʹ��ͬһ��ע��ʽ�����޸ĸ�����ʱ��Ӱ������ʹ�ø���ʽ�ı�ע  wht 11-07-12
	/*
	if(!bInside)
	{	//ֻ����Ҫ���ñ�ע��ʽʱ����Ҫ����dimStyleId������Ӧ�ı�ע��ʽ��ÿ�ζ��򿪻�Ӱ������ٶ� wht 11-07-07
		AcDbDimStyleTableRecord *pDimStyleRec=NULL;
		acdbOpenObject(pDimStyleRec,dimStyleId,AcDb::kForWrite);
		if(pDimStyleRec)
		{
			double fTxtSize=pDimStyleRec->dimtxt();
			pDimStyleRec->setDimtix(Adesk::kFalse);	//ǿ�ƽ��ߴ���ڳߴ�����ڲ�
			pDimStyleRec->close();
		}
	}
	*/
	GetCurDwg()->setDimstyle(dimStyleId);
	AcGePoint3d acad_start, acad_end, acad_dimpos;
	start.z = end.z = dimpos.z = 0;	//Z������㣬����ƽ��ͼ����ͬһZƽ���ϡ�WJH-2006/06/27
	Cpy_Pnt(acad_start, start);
	Cpy_Pnt(acad_end, end);
	Cpy_Pnt(acad_dimpos, dimpos);

	//����ע�Ͷ���
#ifdef _ARX_2007
	if (dimtype == 0)	//ˮƽ��ע
		pDim = pRotateDim = new AcDbRotatedDimension(0, acad_start, acad_end, acad_dimpos, (ACHAR*)_bstr_t(dimText), dimStyleId);
	else if (dimtype == 1)	//��ֱ��ע
		pDim = pRotateDim = new AcDbRotatedDimension(Pi / 2, acad_start, acad_end, acad_dimpos, (ACHAR*)_bstr_t(dimText), dimStyleId);
	else// if (dimtype==2)
		pDim = pAlignedDim = new AcDbAlignedDimension(acad_start, acad_end, acad_dimpos, (ACHAR*)_bstr_t(dimText), dimStyleId);
#else
	if (dimtype == 0)	//ˮƽ��ע
		pDim = pRotateDim = new AcDbRotatedDimension(0, acad_start, acad_end, acad_dimpos, dimText, dimStyleId);
	else if (dimtype == 1)	//��ֱ��ע
		pDim = pRotateDim = new AcDbRotatedDimension(Pi / 2, acad_start, acad_end, acad_dimpos, dimText, dimStyleId);
	else// if (dimtype==2)
		pDim = pAlignedDim = new AcDbAlignedDimension(acad_start, acad_end, acad_dimpos, dimText, dimStyleId);
#endif
	if (pRotateDim&&txt_height > 0)
		pRotateDim->setDimtxt(txt_height);
	else if (pAlignedDim&&txt_height > 0)
		pAlignedDim->setDimtxt(txt_height);

	AcDbObjectId DimId;//�����ʶ��
#ifdef __DRAG_ENT_
	if (DRAGSET.AppendAcDbEntity(pBlockTableRecord, DimId, pDim))//��ʵ��д�����¼
#else
	if (pBlockTableRecord->appendAcDbEntity(DimId, pDim) == Acad::eOk)
#endif
		pDim->close();//�رն���
	ads_name ent_name;
	acdbGetAdsName(ent_name, DimId);
	acdbEntUpd(ent_name);
	return DimId;
}