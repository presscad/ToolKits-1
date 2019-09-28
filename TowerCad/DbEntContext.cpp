#include "stdafx.h"
#include "DbEntContext.h"

/////////////////////////////////////////////////////////////////////////////////
//CDrawingEntsContext
/////////////////////////////////////////////////////////////////////////////////
void CDrawingEntsContext::InitContext(IDrawingCommand* pXeroCmd,STORE_MODE cStoreMode/*=STORE_ACAD*/)
{
	//m_pRootPart=pRootPart;
	cStoreType=XERO_COMMAND;
	context.pXeroCmd=pXeroCmd;
	m_cStoreMode=cStoreMode;
	m_fUser2ScrDrawScale=1.0;
	ProcessHideLineFunc=NULL;
}
void CDrawingEntsContext::InitContext(IDrawingComponent* pXeroComponent,STORE_MODE cStoreMode/*=STORE_ACAD*/)
{
	//m_pRootPart=pRootPart;
	cStoreType=XERO_COMPONENT;
	context.pSectDrawing=pXeroComponent;
	m_cStoreMode=cStoreMode;
	m_fUser2ScrDrawScale=1.0;
	ProcessHideLineFunc=NULL;
}
void CDrawingEntsContext::InitContext(IDrawing* pXeroComponent,STORE_MODE cStoreMode/*=STORE_ACAD*/)
{
	//m_pRootPart=pRootPart;
	cStoreType=XERO_DRAWING;
	context.pXeroDrawing=pXeroComponent;
	m_cStoreMode=cStoreMode!=STORE_ACAD ? cStoreMode : STORE_XERO;
	m_fUser2ScrDrawScale=1.0;
	ProcessHideLineFunc=NULL;
}
void CDrawingEntsContext::InitContext(AcDbBlockTableRecord *pBlockTableRecord)
{
	//m_pRootPart=pRootPart;
	cStoreType=XERO_OTHER;
	context.pBlockTableRecord=pBlockTableRecord;
	m_cStoreMode=STORE_ACADINSTANT;
	ProcessHideLineFunc=NULL;
}
//设置消隐处理函数
void CDrawingEntsContext::SetHideFunc(void (*ProcessHideLine)(AcDbBlockTableRecord *pBlockTableRecord,fPtList& vertex_list,char *layer))
{
	ProcessHideLineFunc=ProcessHideLine;
}
void CDrawingEntsContext::HideLegacyLines(fPtList& vertex_list,char* layer)
{
	if(ProcessHideLineFunc&&m_cStoreMode==STORE_ACADINSTANT)
		ProcessHideLineFunc(context.pBlockTableRecord,vertex_list,layer);
}
//fSysFontSize是系统设定的字体高度，返回值是考虑到绘图比例后的模型尺寸范畴的字体高度　wjh-2017.11.9
double CDrawingEntsContext::ZoomToModelTextSize(double fSysFontSize)
{
	return fSysFontSize;
}
FRAMESIZE CDrawingEntsContext::ZoomToModelTextSize(FRAMESIZE& framesize)
{
	if(cStoreType==XERO_COMPONENT&&context.pSectDrawing!=NULL)
	{
		FRAMESIZE frame=framesize;
		frame.fTextHeight=ZoomToModelTextSize(framesize.fTextHeight);
		frame.fFrameWidth=ZoomToModelTextSize(framesize.fFrameWidth);
		frame.fMargin    =ZoomToModelTextSize(framesize.fMargin);
		return frame;
	}
	else if(cStoreType==XERO_COMMAND&&context.pXeroCmd!=NULL)
	{
		FRAMESIZE frame=framesize;
		frame.fTextHeight=ZoomToModelTextSize(framesize.fTextHeight);
		frame.fFrameWidth=ZoomToModelTextSize(framesize.fFrameWidth);
		frame.fMargin    =ZoomToModelTextSize(framesize.fMargin);
		return frame;
	}
	else
		return framesize;
}

IDbEntity* CDrawingEntsContext::AppendDbEntity(int idDbEntType)
{
	if(cStoreType==XERO_COMMAND)
		return context.pXeroCmd->AppendDbEntity(idDbEntType);
	else if(cStoreType==XERO_COMPONENT)
		return context.pSectDrawing->AppendDbEntity(idDbEntType);
	else if(cStoreType==XERO_DRAWING)
		return context.pXeroDrawing->AppendDbEntity(idDbEntType);
	else
		return NULL;
}
void CDrawingEntsContext::AppendAngularDim(f3dPoint vertex,f3dPoint start_vec,f3dPoint end_vec,
										   double fDimR/*=0*/,char *dimText/*=NULL*/,double fDimAngleTextSize/*=3.0*/)
{
	CSizeAngleDim angularDim;
	CSizeAngleDim *pAngularDim=&angularDim;
	double cosa=start_vec*end_vec;
	double angle=ACOS(start_vec*end_vec);
	if(fabs(angle)<2*RADTODEG_COEF)
		return;	//夹角度数过小
	if(fDimR<=0)	//Pi=3.14
	{
		double half_angle=0.5*ACOS(start_vec*end_vec);
		if(half_angle<1.57)
			fDimR = TransToModelLength(fDimAngleTextSize*0.7*4)/tan(half_angle);
	}
	//角顶点标记点
	pAngularDim->vertex=vertex;
	//角起始边标记点
	pAngularDim->dimStartPos = vertex+start_vec*fDimR*0.4;
	//终止边标记点
	pAngularDim->dimEndPos = vertex+end_vec*fDimR*0.4;
	//标注位置圆弧标记点
	f3dPoint startPos=vertex+start_vec*fDimR;
	f3dPoint endPos=vertex+end_vec*fDimR;
	f3dPoint dim_vec=(startPos+endPos)/2.0-vertex;
	normalize(dim_vec);
	pAngularDim->dimPos=vertex+dim_vec*fDimR;
	
	if(dimText)//指定标注内容
		pAngularDim->dimText.Copy(dimText);
	if(m_cStoreMode==STORE_ACAD)
	{
		
	}
	else if(m_cStoreMode==STORE_ACADINSTANT)
	{
		//UNDONE#完成直接添加到ACAD块表记录里的代码
	}
	else //if(m_cStoreMode==STORE_XERO)
	{
		IDbAngularDimension* pAngularEnt=(IDbAngularDimension*)AppendDbEntity(IDbEntity::DbAngularDimension);
		pAngularEnt->SetArcPoint(angularDim.dimPos);
		pAngularEnt->SetLine1Start(angularDim.vertex);	//line1Start
		pAngularEnt->SetLine1End(angularDim.dimStartPos);	//line1End
		pAngularEnt->SetLine2Start(angularDim.vertex);	//line2Start
		pAngularEnt->SetLine2End(angularDim.dimEndPos);	//line2End
		pAngularEnt->SetDimText(dimText);
	}
}
void CDrawingEntsContext::AppendAngularDim(CSizeAngleDim& angularDim)
{
	if (m_cStoreMode == STORE_ACAD)
	{

	}
	else if (m_cStoreMode == STORE_ACADINSTANT)
	{
		//UNDONE#完成直接添加到ACAD块表记录里的代码
	}
	else if (m_cStoreMode == STORE_XERO)
	{
		IDbAngularDimension* pAngularEnt = (IDbAngularDimension*)AppendDbEntity(IDbEntity::DbAngularDimension);
		pAngularEnt->SetArcPoint(angularDim.dimPos);
		pAngularEnt->SetLine1Start(angularDim.vertex);	//line1Start
		pAngularEnt->SetLine1End(angularDim.dimStartPos);	//line1End
		pAngularEnt->SetLine2Start(angularDim.vertex);	//line2Start
		pAngularEnt->SetLine2End(angularDim.dimEndPos);	//line2End
	}
}
void CDrawingEntsContext::AppendLine(f3dLine& line)
{
	if(m_cStoreMode==STORE_ACAD)
	{

	}
	else if(m_cStoreMode==STORE_ACADINSTANT)
	{
		//UNDONE#完成直接添加到ACAD块表记录里的代码
	}
	else if(m_cStoreMode==STORE_XERO)
	{
		IDbLine* pLine=(IDbLine*)AppendDbEntity(IDbEntity::DbLine);
		pLine->SetPen(line.pen);
		pLine->SetStartPosition(line.startPt);
		pLine->SetEndPosition(line.endPt);
	}
}
void CDrawingEntsContext::AppendLine(f3dArcLine& arcline)
{
	if(m_cStoreMode==STORE_ACAD)
	{

	}
	else if(m_cStoreMode==STORE_ACADINSTANT)
	{
		//UNDONE#完成直接添加到ACAD块表记录里的代码
	}
	else if(m_cStoreMode==STORE_XERO)
	{
		if(arcline.pen.style==PS_USERSTYLE)	//样条曲线
		{
			IDbSpline* pSpline=(IDbSpline*)AppendDbEntity(IDbEntity::DbSpline);
			pSpline->SetPen(arcline.pen);
			pSpline->InsertFitPointAt(0,arcline.Start());
			pSpline->InsertFitPointAt(1,arcline.Center());
			pSpline->InsertFitPointAt(2,arcline.End());
		}
		else
		{
			IDbArcline* pArcLine=(IDbArcline*)AppendDbEntity(IDbEntity::DbArcline);
			pArcLine->SetPen(arcline.pen);
			if(fabs(arcline.SectorAngle()-2*Pi)<EPS)			//
			{
				pArcLine->SetCenter(arcline.Center());
				pArcLine->SetRadius(arcline.Radius());
			}
			else if(arcline.SectorAngle()>0)	//指定圆弧半径
			{
				pArcLine->SetCenter(arcline.Center());
				if(arcline.ColumnNorm()==arcline.WorkNorm())	//圆弧
				{
					pArcLine->SetStartPosition(arcline.Start());
					pArcLine->SetSectorAngle(arcline.SectorAngle());
				}
				else		//椭圆弧
				{
					pArcLine->SetStartPosition(arcline.Start());
					pArcLine->SetEndPosition(arcline.End());
					pArcLine->SetWorkNorm(arcline.WorkNorm());
					pArcLine->SetColumnNorm(arcline.ColumnNorm());
				}
			}
			else	//直线
			{
				pArcLine->SetStartPosition(arcline.Start());
				pArcLine->SetEndPosition(arcline.End());
			}
		}
	}
}
void CDrawingEntsContext::AppendSpLine(fPtList& vertex_list)
{
	if(m_cStoreMode==STORE_ACAD)
	{
		return;
	}
	else if(m_cStoreMode==STORE_ACADINSTANT)
	{
		//UNDONE#完成直接添加到ACAD块表记录里的代码
	}
	else if(m_cStoreMode==STORE_XERO)
	{
		IDbSpline* pSpline=(IDbSpline*)AppendDbEntity(IDbEntity::DbSpline);
		pSpline->SetPen(vertex_list[0].pen);
		for(int i=0;i<vertex_list.GetNodeNum();i++)
			pSpline->InsertFitPointAt(i,vertex_list[i]);
	}
}
void CDrawingEntsContext::AppendCircle(f3dCircle& circle)
{
	if(m_cStoreMode==STORE_ACAD)
	{

	}
	else if(m_cStoreMode==STORE_ACADINSTANT)
	{
		//UNDONE#完成直接添加到ACAD块表记录里的代码
	}
	else if(m_cStoreMode==STORE_XERO)
	{
		IDbCircle* pCircle=(IDbCircle*)AppendDbEntity(IDbEntity::DbCircle);
		pCircle->SetCenter(circle.centre);
		pCircle->SetRadius(circle.radius);
		pCircle->SetNormal(circle.norm);
		pCircle->SetPen(circle.pen);
	}
}
void CDrawingEntsContext::AppendSizeTextDim(CSizeTextDim& sizeTextDim)
{
	if(m_cStoreMode==STORE_ACAD)
	{

	}
	else if(m_cStoreMode==STORE_ACADINSTANT)
	{
		//UNDONE#完成直接添加到ACAD块表记录里的代码
	}
	else if(m_cStoreMode==STORE_XERO)
	{
		IDbRotatedDimension* pRotateDim=(IDbRotatedDimension*)AppendDbEntity(IDbEntity::DbRotatedDimension);
		pRotateDim->SetLine1Point(sizeTextDim.dimStartPos);
		pRotateDim->SetLine2Point(sizeTextDim.dimEndPos);
		pRotateDim->SetDimLinePoint(sizeTextDim.dimPos);
		pRotateDim->SetRotationRadian(sizeTextDim.angle);
		pRotateDim->SetTextHeight(sizeTextDim.dimStyle.textHeight);
		pRotateDim->SetDimText(sizeTextDim.DimText());
	}
}
void CDrawingEntsContext::AppendTextDim(CTextOnlyDrawing& pureTextDim)
{
	if(m_cStoreMode==STORE_ACAD)
	{

	}
	else if(m_cStoreMode==STORE_ACADINSTANT)
	{
		//UNDONE#完成直接添加到ACAD块表记录里的代码
	}
	else if(m_cStoreMode==STORE_XERO)
	{
		IDbText* pTextDim=(IDbText*)AppendDbEntity(IDbEntity::DbText);
		pTextDim->SetHeight(pureTextDim.textHeight);
		pTextDim->SetAlignment(pureTextDim.alignType);
		pTextDim->SetPosition(pureTextDim.dimPos);
		pTextDim->SetTextString(pureTextDim.dimText);
	}
}
//构件数据标识点
void CDrawingEntsContext::AppendDataPoint(CTextFrame& dataPoint)
{
	if(m_cStoreMode==STORE_ACAD)
	{

	}
	else if(m_cStoreMode==STORE_ACADINSTANT)
	{
		//UNDONE#完成直接添加到ACAD块表记录里的代码
	}
}
DATAPOINT_TAG* CDrawingEntsContext::AppendDataPoineTag()
{
	if(m_cStoreMode==STORE_ACAD)
	{
		return NULL;
	}
	else if(m_cStoreMode==STORE_ACADINSTANT)
	{
		//UNDONE#完成直接添加到ACAD块表记录里的代码
		return NULL;
	}
	return NULL;
}
//钢管端面图的环向角标注基准线
void CDrawingEntsContext::AppendTubeBaseLine(BYTE iDrawMode,CLDSLineTube* pBaseTube,f3dPoint arrowVec,f3dPoint arrowBtm,f3dPoint arrowHead)
{
	
}
void CDrawingEntsContext::AppendTubeTaCoreVecLine(f3dPoint vec,f3dPoint xBtm,f3dPoint xHead)
{
	
}
//绘制焊缝线
void CDrawingEntsContext::DrawWeldLineSymbol(f3dPoint start, f3dPoint end,double width/*=0*/,double height/*=0*/)	
{	
	DrawWeldLineSymbol(f3dLine(start,end),start,width,height);
}
void CDrawingEntsContext::DrawWeldLineSymbol(f3dLine line,f3dPoint pickPt,double width/*=0*/,double height/*=0*/)
{
	if(width<=0)
		width=0.8;//sys.general_set.fWeldLineSymbolSpace;
	if(height<=0)
		height=0.8;//sys.general_set.fWeldLineSymbolSpace;
	width=TransToModelLength(width);
	height=TransToModelLength(height);
	f3dPoint line_vec = line.endPt-line.startPt;
	double length = line_vec.mod();
	normalize(line_vec);
	f3dPoint dest_vec(-line_vec.y,line_vec.x);
	f3dPoint verify_vec=pickPt-line.startPt;
	normalize(verify_vec);
	if(verify_vec*dest_vec<0)
		dest_vec *=-1.0;
	f3dPoint weldEnd, weldStart = line.startPt+line_vec*width;
	f3dLine tem_line;	//临时绘制线
	for(int i=0; i<ftoi(length/width)-1;i++)
	{
		weldEnd = weldStart+dest_vec*height;
		//绘制焊缝线
		tem_line.startPt=weldStart;
		tem_line.endPt=weldEnd;
		tem_line.ID=0;
		tem_line.pen.style=PS_INSIDEFRAME; //图层6
		AppendLine(tem_line);
		weldStart += line_vec*width;
	}
}
void CDrawingEntsContext::DrawWeldLineSymbol(f3dArcLine arcline,double width/*=0*/,double height/*=0*/)
{	
	//if(sys.m_iCurMapType==5&&!sys.part_group_map.bDrawWeldLineSymbol)
	//	return;	//不在组焊图中绘制焊缝线标记
	if(width<=0)
		width=0.8;//sys.general_set.fWeldLineSymbolSpace;
	if(height<=0)
		height=0.8;//sys.general_set.fWeldLineSymbolSpace;
	width=TransToModelLength(width);
	height=TransToModelLength(height);
	double radius=fabs(arcline.Radius());
	double slice_angle=width/radius;
	f3dPoint center = arcline.Center();
	f3dPoint dest_vec;
	f3dPoint weldStart,weldEnd;
	f3dLine line;	//临时绘制线
	double angle=slice_angle;
	for(;;)
	{
		if(angle>arcline.SectorAngle())
			break;
		weldStart = arcline.PositionInAngle(angle);
		dest_vec=weldStart-center;
		normalize(dest_vec);
		weldEnd=weldStart+dest_vec*height;
		//绘制焊缝线
		line.startPt=weldStart;
		line.endPt=weldEnd;
		line.ID=0;
		line.pen.style=PS_INSIDEFRAME;
		AppendLine(line);
		//edgeList.append(LineToArc(line));
		angle+=slice_angle;
		line.feature=-1;	//检测碰撞时为了提升速度可不检测焊缝线
	}
}
double CDrawingEntsContext::TransToModelLength(double scr_draw_length)
{
	if(m_cStoreMode==STORE_ACAD)
		return 1.0;
	else
		return scr_draw_length*m_fUser2ScrDrawScale;
}