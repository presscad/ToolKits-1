#include "stdafx.h"
#include "PlateDrawer.h"
#include "SortFunc.h"

//////////////////////////////////////////////////////////////////////////
//CTubeEndPlateDrawing
CTubeEndPlateDrawing::CTubeEndPlateDrawing(void)
{
	m_dwClassType=TYPE_TUBE_END;
	m_dwDrawType=TYPE_DETAIL;
	L=M=N=S=W=H=m_fThick=0;
	m_pHoleBottomRight=NULL;
	m_fTubeD=m_fTubeThick=m_fHf=0;
	m_fTubeLengthExposed=100;
}
CTubeEndPlateDrawing::~CTubeEndPlateDrawing(void)
{
}
void CTubeEndPlateDrawing::InitBottomRightHole()
{
	m_pHoleBottomRight=NULL;
	for(HOLE_INFO *pHoleInfo=boltHoleList.GetFirst();pHoleInfo;pHoleInfo=boltHoleList.GetNext())
	{
		if(m_pHoleBottomRight==NULL)
			m_pHoleBottomRight=pHoleInfo;
		else if(pHoleInfo->fPosX>=m_pHoleBottomRight->fPosX&&pHoleInfo->fPosY<=m_pHoleBottomRight->fPosY)
			m_pHoleBottomRight=pHoleInfo;
	}
}
void CTubeEndPlateDrawing::Init(CProcessPlate *pProcessPlate)
{
	CMaxD maxX,maxY;
	CMinD minX,minY;//计算钢板的各螺栓坐标的位移
	for (PROFILE_VER *pVertex=pProcessPlate->vertex_list.GetFirst();pVertex;pVertex=pProcessPlate->vertex_list.GetNext())
	{
		minX.Update(pVertex->vertex.x,pVertex);
		maxX.Update(pVertex->vertex.x,pVertex);
		minY.Update(pVertex->vertex.y,pVertex);
		maxY.Update(pVertex->vertex.y,pVertex);
		if(pVertex->m_bRollEdge)
			W=pVertex->manu_space;
	}
	PROFILE_VER *pVertexN=NULL,*pVertexS=NULL;
	for (PROFILE_VER *pVertex=pProcessPlate->vertex_list.GetFirst();pVertex;pVertex=pProcessPlate->vertex_list.GetNext())
	{
		if(pVertex->vertex.x>minX.number&&pVertex->vertex.x<maxX.number)
			pVertexN=pVertex;
		if(pVertex->vertex.y>minY.number&&pVertex->vertex.y<maxY.number)
			pVertexS=pVertex;
	}
	L=maxX.number-minX.number;
	H=maxY.number-minY.number;
	if(pVertexN)
		N=maxX.number-fabs(pVertexN->vertex.x);
	if(pVertexS)
		S=maxY.number-fabs(pVertexS->vertex.y);
	m_fThick=pProcessPlate->m_fThick;
	m_cMaterial=pProcessPlate->cMaterial;
	m_sPartLabel=pProcessPlate->GetPartNo();
	for (BOLT_INFO *pBolt=pProcessPlate->m_xBoltInfoList.GetFirst();pBolt;pBolt=pProcessPlate->m_xBoltInfoList.GetNext())
	{
		HOLE_INFO holeInfo;
		holeInfo.d=pBolt->bolt_d;
		holeInfo.fIncrement=pBolt->hole_d_increment;
		holeInfo.fPosX=pBolt->posX;
		holeInfo.fPosY=pBolt->posY;
		boltHoleList.append(holeInfo);
	}
	M=L/2;
	m_fTubeD=H/2;
	m_fTubeThick=8;
	m_fTubeLengthExposed=100;
	m_fHf=6;
	fTextHeight=10;
	InitBottomRightHole();
}
void CTubeEndPlateDrawing::Init(CLDSGeneralPlate *pPlate)
{
	if(pPlate==NULL||pPlate->GetClassTypeId()!=CLS_PARAMPLATE)
		return;
	CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPlate;
	if(pParamPlate->m_iParamType!=TYPE_ROLLEND)
		return;
	CLDSLineTube *pLineTube=(CLDSLineTube*)pParamPlate->BelongModel()->FromPartHandle(pParamPlate->m_hPartWeldParent,CLS_LINETUBE);
	if(pLineTube==NULL)
		return;
	m_fTubeD=pLineTube->GetDiameter();
	m_fTubeThick=pLineTube->GetThick();
	m_fTubeLengthExposed=160;
	m_fHf=6;
	fTextHeight=10;
	m_pPlate=pPlate;
	//
	m_fThick=pPlate->GetThick();
	m_cMaterial=pParamPlate->cMaterial;
	m_sPartLabel.Copy(pParamPlate->GetPartNo());
	pParamPlate->GetDesignItemValue('L',&L);
	pParamPlate->GetDesignItemValue('M',&M);
	pParamPlate->GetDesignItemValue('N',&N);
	pParamPlate->GetDesignItemValue('S',&S);
	pParamPlate->GetDesignItemValue('H',&H);
	pParamPlate->GetDesignItemValue('W',&W);
	//
	for(CLsRef *pLsRef=pParamPlate->GetFirstLsRef();pLsRef;pLsRef=pParamPlate->GetNextLsRef())
	{
		CLDSBolt *pBolt=pLsRef->GetLsPtr();
		GEPOINT pos=pBolt->ucs.origin;
		coord_trans(pos,pParamPlate->ucs,FALSE);
		HOLE_INFO holeInfo;
		holeInfo.d=pBolt->d;
		holeInfo.fIncrement=pBolt->hole_d_increment;
		holeInfo.fPosX=pos.x;
		holeInfo.fPosY=pos.y;
		boltHoleList.append(holeInfo);
	}
	InitBottomRightHole();
}
static CXhChar16 QuerySteelMatMark(char cMat)
{
	CXhChar16 sMatMark;
	if('H'==cMat)
		sMatMark.Copy("Q345");
	else if('G'==cMat)
		sMatMark.Copy("Q390");
	else if('P'==cMat)
		sMatMark.Copy("Q420");
	else if('T'==cMat)
		sMatMark.Copy("Q460");
	else if('S'==cMat)
		sMatMark.Copy("Q235");
	return sMatMark;
}
void CTubeEndPlateDrawing::AppendPartLabelAndMaterial(f3dPoint dim_pos)
{
	CTextOnlyDrawing dim_text;
	dim_text.dimPos=dim_pos;
	dim_text.dimText=m_sPartLabel;
	dim_text.alignType=MiddleCenter;
	dim_text.textHeight=fTextHeight;
	double fRadius=5*fTextHeight;
	m_xEntsContext.AppendTextDim(dim_text);
	f3dCircle circle;
	circle.centre=dim_text.dimPos;
	circle.radius=fRadius;
	circle.norm=f3dPoint(0,0,1);
	m_xEntsContext.AppendCircle(circle);
	//材质
	f3dPoint dim_ent_datapos;
	dim_text.dimText.Printf("%s-%.f",(char*)QuerySteelMatMark(m_cMaterial),m_fThick);
	dim_ent_datapos.Set(dim_pos.x,dim_pos.y+fRadius,0);
	dim_text.dimPos=dim_ent_datapos;
	dim_text.alignType=BottomCenter;
	m_xEntsContext.AppendTextDim(dim_text);
}
static int CompareHoleGroupPtr(const CTubeEndPlateDrawing::HOLE_GROUP_PTR &ls1,const CTubeEndPlateDrawing::HOLE_GROUP_PTR &ls2)
{
	if(ls1->fCoord>ls2->fCoord)
		return 1;
	else if(ls1->fCoord<ls2->fCoord)
		return -1;
	else
		return 0;
}
void CTubeEndPlateDrawing::DrawSketch()
{	//轮廓点
	f3dPoint pointArr[6];
	pointArr[0].Set(L,H/2-S,0);
	pointArr[1].Set(L-N,H/2,0);
	pointArr[2].Set(0,H/2,0);
	pointArr[3].Set(0,-H/2,0);
	pointArr[4].Set(L-N,-H/2,0);
	pointArr[5].Set(L,-(H/2-S),0);
	f3dLine line;
	for(int i=0;i<6;i++)
	{
		line.startPt=pointArr[i];
		line.endPt=pointArr[(i+1)%6];
		m_xEntsContext.AppendLine(line);
	}
	//////////////////////////////////////////////////////////////////////////
	//卷边线
	if(W<0)
	{
		line.pen.style=PS_DASHDOTDOT;
		line.pen.crColor=RGB(255,0,0);
	}
	else
	{
		line.pen.style=PS_SOLID;
		line.pen.crColor=RGB(0,0,0);
	}
	// 求偏移值
	double fOffsetX=(1-m_fThick/S)*N;
	line.startPt.Set(0,H/2-m_fThick,0);
	line.endPt.Set(L-fOffsetX,H/2-m_fThick,0);
	m_xEntsContext.AppendLine(line);
	line.startPt.Set(0,-(H/2-m_fThick),0);
	line.endPt.Set(L-fOffsetX,-(H/2-m_fThick),0);
	m_xEntsContext.AppendLine(line);
	//////////////////////////////////////////////////////////////////////////
	//螺栓
	for(HOLE_INFO *pBoltInfo=boltHoleList.GetFirst();pBoltInfo;pBoltInfo=boltHoleList.GetNext())
	{	//生成螺栓
		f3dCircle circle;
		circle.norm.Set(0,0,1);
		circle.centre.Set(pBoltInfo->fPosX,pBoltInfo->fPosY,0);
		circle.radius = (pBoltInfo->d+pBoltInfo->fIncrement)/2;
		m_xEntsContext.AppendCircle(circle);
	}
};
void CTubeEndPlateDrawing::DrawTube()
{
	//钢管外部
	f3dLine tubeLine;
	tubeLine.startPt.Set(M,m_fTubeD/2,0);
	tubeLine.endPt.Set(M,-m_fTubeD/2,0);
	m_xEntsContext.AppendLine(tubeLine);
	tubeLine.startPt.Set(M,m_fTubeD/2,0);
	tubeLine.endPt.Set(-m_fTubeLengthExposed,m_fTubeD/2,0);
	m_xEntsContext.AppendLine(tubeLine);
	tubeLine.startPt.Set(M,-m_fTubeD/2,0);
	tubeLine.endPt.Set(-m_fTubeLengthExposed,-m_fTubeD/2,0);
	m_xEntsContext.AppendLine(tubeLine);
	//钢管内部
	tubeLine.startPt.Set(M,m_fTubeD/2-m_fTubeThick,0);
	tubeLine.endPt.Set(-m_fTubeLengthExposed,m_fTubeD/2-m_fTubeThick,0);
	m_xEntsContext.AppendLine(tubeLine);
	tubeLine.startPt.Set(M,-m_fTubeD/2+m_fTubeThick,0);
	tubeLine.endPt.Set(-m_fTubeLengthExposed,-m_fTubeD/2+m_fTubeThick,0);
	m_xEntsContext.AppendLine(tubeLine);
	//钢管轴线
	f3dLine line_x;
	line_x.startPt.Set(-m_fTubeLengthExposed,0,0);
	line_x.endPt.Set(L,0,0);
	m_xEntsContext.AppendLine(line_x);
	//钢管后的直线
	tubeLine.startPt.Set(M+5,H/2,0);
	tubeLine.endPt.Set(M+5,-H/2,0);
	m_xEntsContext.AppendLine(tubeLine);
}
void CTubeEndPlateDrawing::DrawDimSize()
{	
	ATOM_LIST<HOLE_GROUP> rowGroupList,colGroupList;
	//获取螺栓之间的间距
	//1.根据相应的坐标值进行分组，并将坐标点添加
	for(HOLE_INFO *pBoltInfo=boltHoleList.GetFirst();pBoltInfo;pBoltInfo=boltHoleList.GetNext())
	{	//行分组
		HOLE_GROUP *pRow=NULL;
		for(pRow=rowGroupList.GetFirst();pRow;pRow=rowGroupList.GetNext())
		{
			if(fabs(pBoltInfo->fPosY-pRow->fCoord)<EPS2)
				break;
		}
		if(pRow==NULL)
		{
			pRow=rowGroupList.append();
			pRow->fCoord=pBoltInfo->fPosY;
		}
		pRow->Add(pBoltInfo);
		//列分组
		HOLE_GROUP *pCol=NULL;
		for(pCol=colGroupList.GetFirst();pCol;pCol=colGroupList.GetNext())
		{
			if(fabs(pBoltInfo->fPosX-pCol->fCoord)<EPS2)
				break;
		}
		if(pCol==NULL)
		{
			pCol=colGroupList.append();
			pCol->fCoord=pBoltInfo->fPosX;
		}
		pCol->Add(pBoltInfo);
	}
	//2.根据坐标值进行排序
	//2.1.将列表内数据指针保存到新的列表
	ARRAY_LIST<HOLE_GROUP*> rowGroup,colGroup;
	for (HOLE_GROUP *pHole=rowGroupList.GetFirst();pHole;pHole=rowGroupList.GetNext())
		rowGroup.append(pHole);
	for (HOLE_GROUP *pHole=colGroupList.GetFirst();pHole;pHole=colGroupList.GetNext())
		colGroup.append(pHole);
	//2.2.对指针进行排序
	CHeapSort<HOLE_GROUP*>::HeapSort(rowGroup.m_pData,rowGroup.GetSize(),CompareHoleGroupPtr);
	CHeapSort<HOLE_GROUP*>::HeapSort(colGroup.m_pData,colGroup.GetSize(),CompareHoleGroupPtr);
	//////////////////////////////////////////////////////////////////////////
	//外层
	//L
	double fUnitIncrease=fTextHeight*2;
	CSizeTextDim dimSize;
	dimSize.dimStartPos.Set(0,H/2+3*fUnitIncrease,0);
	dimSize.dimEndPos.Set(L,H/2+3*fUnitIncrease,0);
	dimSize.dimPos.Set(L/2,H/2+3*fUnitIncrease,0);
	dimSize.dist=L;
	dimSize.dimStyle.textHeight=fTextHeight;
	m_xEntsContext.AppendSizeTextDim(dimSize);	
	//H
	dimSize.dimStartPos.Set(L+3*fUnitIncrease,H/2,0);
	dimSize.dimEndPos.Set(L+3*fUnitIncrease,-H/2,0);
	dimSize.dimPos.Set(L+3*fUnitIncrease,0,0);
	dimSize.angle=Pi/2*3;
	dimSize.dist=H;
	m_xEntsContext.AppendSizeTextDim(dimSize);	
	//////////////////////////////////////////////////////////////////////////
	//内层
	for(int i=0;i<2;i++)
	{	//1.统计生成标注点(列标注从左到右，行标注从下到上)
		ARRAY_LIST<GEPOINT> dimPointArr;
		ARRAY_LIST<HOLE_GROUP*> *pGroupList=&colGroup;
		if(i==0)
		{	//列标注从左到右（最小X点，M点、螺栓点、N点、L的最右侧点）
			dimPointArr.append(GEPOINT(0,H/2,0));
			dimPointArr.append(GEPOINT(M,H/2,0));
		}
		else
			pGroupList=&rowGroup;
		for(HOLE_GROUP **ppGroup=pGroupList->GetFirst();ppGroup;ppGroup=pGroupList->GetNext())
		{
			HOLE_GROUP *pGroup=*ppGroup;
			HOLE_INFO *pHole=(HOLE_INFO *)pGroup->boltInfoPtrList.GetTail();
			if(pHole)
				dimPointArr.append(GEPOINT(pHole->fPosX,pHole->fPosY));
		}
		if(i==0)
		{
			dimPointArr.append(GEPOINT(L-N,H/2,0));
			dimPointArr.append(GEPOINT(L,H/2,0));
		}
		//else
		//{
			//dimPointArr.append(GEPOINT(L,H/2-S,0));
			//dimPointArr.append(GEPOINT(L,H/2,0));
		//}
		//2.添加标注
		for(int j=0;j<dimPointArr.GetSize()-1;j++)
		{
			dimSize.dimStartPos=dimPointArr[j];
			dimSize.dimEndPos=dimPointArr[j+1];
			if(i==0)
			{
				if(fabs(dimPointArr[j+1].x-dimPointArr[j].x)<EPS2)
					continue;
				dimSize.dimPos.Set((dimPointArr[j].x+dimPointArr[j+1].x)/2,(H/2+fUnitIncrease),0);
				dimSize.dist=dimPointArr[j+1].x-dimPointArr[j].x;
				dimSize.angle=0;
			}
			else
			{
				if(fabs(dimPointArr[j+1].y-dimPointArr[j].y)<EPS2)
					continue;
				dimSize.dimPos.Set(L+1.5*fUnitIncrease,(dimPointArr[j].y+dimPointArr[j+1].y)/2,0);
				dimSize.dist=dimPointArr[j+1].y-dimPointArr[j].y;
				dimSize.angle=Pi/2*3;
			}
			m_xEntsContext.AppendSizeTextDim(dimSize);
		}
	}
}
void CTubeEndPlateDrawing::DrawPartDetail()
{
	CTextOnlyDrawing dim_text;
	double fUnitIncrement=fTextHeight;
	double fUnderLineLength=100;
	double fPosY=-(H/2+3*fUnitIncrement);
	//钢管参数
	dim_text.dimPos.Set(-m_fTubeLengthExposed/2,m_fTubeD/2+2,0);
	//取值小数点后一位或整数
	CXhChar16 sTubeD("%f",m_fTubeD);
	SimplifiedNumString((char*)sTubeD);
	CXhChar16 sTubeThick("%f",m_fTubeThick);
	SimplifiedNumString((char*)sTubeThick);
	CXhChar50 str_tube("φ%sX%s%C",(char*)sTubeD,(char*)sTubeThick,m_cMaterial);
	dim_text.dimText=str_tube;
	dim_text.alignType=BottomRight;
	dim_text.textHeight=fTextHeight;
	m_xEntsContext.AppendTextDim(dim_text);
	//指向焊缝的参数线
	f3dLine tubeLine;
	f3dPoint hf_dim_max(40,-m_fTubeD/2,0);
	if(M<hf_dim_max.x)
		tubeLine.startPt.Set(M/2,-m_fTubeD/2,0);
	else
		tubeLine.startPt=hf_dim_max;
	tubeLine.endPt.Set(0,fPosY,0);
	m_xEntsContext.AppendLine(tubeLine);
	//焊缝
	f3dLine dim_text_line;
	dim_text_line.startPt.Set(0,fPosY,0);
	dim_text_line.endPt.Set(-fUnderLineLength,fPosY,0);
	m_xEntsContext.AppendLine(dim_text_line);
	CXhChar50 cHf("hf=%u",(int)m_fHf);
	dim_text.dimPos=dim_text_line.startPt;
	dim_text.dimPos.y+=1;
	dim_text.dimText=cHf;
	dim_text.alignType=BottomRight;
	m_xEntsContext.AppendTextDim(dim_text);
	//三角形
	f3dPoint vertexlist[3];
	vertexlist[0]=dim_text_line.endPt;
	vertexlist[1].Set(vertexlist[0].x+fUnitIncrement,vertexlist[0].y+fUnitIncrement,0);
	vertexlist[2].Set(vertexlist[0].x+fUnitIncrement,vertexlist[0].y-fUnitIncrement,0);
	for(int i=0;i<3;i++)
	{
		f3dLine line;
		line.startPt=vertexlist[i];
		line.endPt=vertexlist[(i+1)%3];
		m_xEntsContext.AppendLine(line);
	}
	//螺栓参数
	if(m_pHoleBottomRight)
	{
		DWORD dwHoleCount=boltHoleList.GetSize();
		CXhChar50 sBoltDim("%uφ%.1f,%uM%d",dwHoleCount,(m_pHoleBottomRight->d+m_pHoleBottomRight->fIncrement),dwHoleCount,m_pHoleBottomRight->d);
		//
		f3dLine line_bolt;
		line_bolt.startPt.Set(m_pHoleBottomRight->fPosX,m_pHoleBottomRight->fPosY,0);
		line_bolt.endPt.Set(L+fUnitIncrement,-(H/2+5*fUnitIncrement),0);
		m_xEntsContext.AppendLine(line_bolt);
		line_bolt.startPt=line_bolt.endPt;
		line_bolt.endPt.Set(line_bolt.startPt.x+fUnderLineLength,line_bolt.startPt.y,0);
		m_xEntsContext.AppendLine(line_bolt);
		//
		dim_text.dimText=sBoltDim;
		dim_text.dimPos=line_bolt.startPt;
		dim_text.dimPos.x+=1;
		dim_text.dimPos.y+=1;
		dim_text.alignType=BottomLeft;
		m_xEntsContext.AppendTextDim(dim_text);
	}
	//钢板类型
	f3dPoint dim_pos(L/2,-(H/2+8*fUnitIncrement),0);
	AppendPartLabelAndMaterial(dim_pos);
}
void CTubeEndPlateDrawing::SideTubeDrawing()
{
	f3dPoint origin;
	double fUnitIncrement=fTextHeight;
	double fRadiusOut=m_fTubeD/2+m_fTubeThick;
	double fRadiusIn=m_fTubeD/2;
	origin.Set(L+fRadiusOut+8*fUnitIncrement,0,0);
	//钢管
	f3dCircle circle;
	circle.norm.Set(0,0,1);
	circle.centre=origin;
	circle.radius = fRadiusIn;
	m_xEntsContext.AppendCircle(circle);
	circle.radius=fRadiusOut;
	m_xEntsContext.AppendCircle(circle);
	//轴线
	f3dLine line;
	line.startPt.Set(origin.x-fRadiusOut-fUnitIncrement,0,0);
	line.endPt.Set(origin.x+fRadiusOut+fUnitIncrement,0,0);
	m_xEntsContext.AppendLine(line);
	line.startPt.Set(origin.x,fRadiusOut+fUnitIncrement,0);
	line.endPt.Set(origin.x,-(fRadiusOut+fUnitIncrement),0);
	m_xEntsContext.AppendLine(line);
	double fExposedW=fabs(W)-m_fTubeThick;
	f3dPoint vertexList[10];
	int size=0;
	if (W!=0)
	{
		vertexList[0].Set(origin.x+m_fTubeThick/2+fExposedW,origin.y+H/2-m_fTubeThick,0);
		vertexList[1].Set(origin.x+m_fTubeThick/2+fExposedW,origin.y+H/2,0);
		vertexList[2].Set(origin.x+m_fTubeThick/2,origin.y+H/2,0);
		vertexList[3].Set(origin.x-m_fTubeThick/2,origin.y+H/2-m_fTubeThick,0);
		vertexList[4].Set(origin.x-m_fTubeThick/2,origin.y-H/2+m_fTubeThick,0);
		vertexList[5].Set(origin.x+m_fTubeThick/2,origin.y-H/2,0);
		vertexList[6].Set(origin.x+m_fTubeThick/2+fExposedW,origin.y-H/2,0);
		vertexList[7].Set(origin.x+m_fTubeThick/2+fExposedW,origin.y-H/2+m_fTubeThick,0);
		vertexList[8].Set(origin.x+m_fTubeThick/2,origin.y-H/2+m_fTubeThick,0);
		vertexList[9].Set(origin.x+m_fTubeThick/2,origin.y+H/2-m_fTubeThick,0);
		size=10;
	}
	else
	{
		vertexList[0].Set(origin.x-m_fTubeThick/2,origin.y+H/2,0);
		vertexList[1].Set(origin.x-m_fTubeThick/2,origin.y-H/2,0);
		vertexList[2].Set(origin.x+m_fTubeThick/2,origin.y-H/2,0);
		vertexList[3].Set(origin.x+m_fTubeThick/2,origin.y+H/2,0);
		size=4;
	}
	for (int i=0;i<size;i++)
	{
		f3dLine line;
		line.startPt=vertexList[i];
		line.endPt=vertexList[(i+1)%size];
		m_xEntsContext.AppendLine(line);
	}
	//W
	CSizeTextDim dimSize;
	dimSize.dimStartPos.Set(origin.x-m_fTubeThick/2,H/2+fUnitIncrement,0);
	dimSize.dimEndPos.Set(origin.x+m_fTubeThick/2+fExposedW,H/2+fUnitIncrement,0);
	dimSize.dimPos.Set(origin.x-m_fTubeThick/2+fabs(W/2),H/2+fUnitIncrement,0);
	dimSize.dist=W;
	dimSize.dimStyle.textHeight=fTextHeight;
	m_xEntsContext.AppendSizeTextDim(dimSize);
	//
	line.startPt.Set(origin.x+fExposedW,H/2-m_fTubeThick,0);
	line.endPt.Set(origin.x+fabs(W)+6*fUnitIncrement,H/2-6*fUnitIncrement,0);
	m_xEntsContext.AppendLine(line);
	f3dPoint dim_pos(origin.x+fabs(W)+8*fUnitIncrement,H/2-6*fUnitIncrement,0);
	AppendPartLabelAndMaterial(dim_pos);
}
void CTubeEndPlateDrawing::CreatePatternDrawing()
{

}
void CTubeEndPlateDrawing::CreateDetailDrawing()
{
	DrawSketch();
	DrawTube();
	DrawDimSize();
	DrawPartDetail();
	SideTubeDrawing();
}
void CTubeEndPlateDrawing::Draw(IDrawing *pDrawing)
{
	m_xEntsContext.InitContext(pDrawing,STORE_XERO);
	if (m_dwDrawType==TYPE_DETAIL)
		CreateDetailDrawing();
	else
		CreatePatternDrawing();
}

void CTubeEndPlateDrawing::Draw(IDrawingComponent *pComponet)
{
	m_xEntsContext.InitContext(pComponet,STORE_XERO);
	if (m_dwDrawType==TYPE_DETAIL)
		CreateDetailDrawing();
	else
		CreatePatternDrawing();
}
void CTubeEndPlateDrawing::Draw(IDrawingCommand *pCommand)
{
	m_xEntsContext.InitContext(pCommand,STORE_XERO);
	if (m_dwDrawType==TYPE_DETAIL)
		CreateDetailDrawing();
	else
		CreatePatternDrawing();
}
void CTubeEndPlateDrawing::Draw(AcDbBlockTableRecord *pBlockTbl)
{
	m_xEntsContext.InitContext(pBlockTbl);
	if (m_dwDrawType==TYPE_DETAIL)
		CreateDetailDrawing();
	else
		CreatePatternDrawing();
}
//////////////////////////////////////////////////////////////////////////
//CPlateDrawer
IPlateDrawing* CPlateDrawer::CreatePlateDrawing(DWORD cls_type_id)
{
	IPlateDrawing *pPlateDrawing=NULL;
	switch(cls_type_id)
	{
	case IPlateDrawing::TYPE_COMMON :
		pPlateDrawing=new IPlateDrawing();
		break;
	case IPlateDrawing::TYPE_TUBE_END:
		pPlateDrawing=new CTubeEndPlateDrawing();
		break;
	}
	return pPlateDrawing;
}
bool CPlateDrawer::DestroyPlateDrawing(IPlateDrawing *pPlateDrawing)
{
	if(pPlateDrawing==NULL)
		return false;
	switch (pPlateDrawing->GetClassType())
	{
	case IPlateDrawing::TYPE_COMMON :
		delete (IPlateDrawing*)pPlateDrawing;
		break;
	case IPlateDrawing::TYPE_TUBE_END:
		delete (CTubeEndPlateDrawing*)pPlateDrawing;
		break;
	}
	return true;
}
CPlateDrawer::CPlateDrawer()
{
	m_pPlateDrawing=NULL;
}
void CPlateDrawer::Init(CLDSGeneralPlate *pPlate)
{
	if(pPlate==NULL||pPlate->GetClassTypeId()!=CLS_PARAMPLATE)
		return;
	CLDSParamPlate *pParamPlate=(CLDSParamPlate*)pPlate;
	if(pParamPlate->m_iParamType!=TYPE_ROLLEND)
		return;
	m_pPlateDrawing=CreatePlateDrawing(IPlateDrawing::TYPE_TUBE_END);
	if(m_pPlateDrawing)
		m_pPlateDrawing->Init(pPlate);
}
void CPlateDrawer::Init(CProcessPlate *pProcessPlate)
{
	m_pPlateDrawing=CreatePlateDrawing(IPlateDrawing::TYPE_TUBE_END);
	if(m_pPlateDrawing)
		m_pPlateDrawing->Init(pProcessPlate);
}
void CPlateDrawer::Draw(IDrawing *pDrawing)
{
	if(m_pPlateDrawing)
		m_pPlateDrawing->Draw(pDrawing);
}
void CPlateDrawer::Draw(IDrawingComponent *pComponet)
{
	if(m_pPlateDrawing)
		m_pPlateDrawing->Draw(pComponet);
}
void CPlateDrawer::Draw(IDrawingCommand *pCommand)
{
	if(m_pPlateDrawing)
		m_pPlateDrawing->Draw(pCommand);
}
void CPlateDrawer::Draw(AcDbBlockTableRecord *pBlockTbl)
{
	if(m_pPlateDrawing)
		m_pPlateDrawing->Draw(pBlockTbl);
}
CPlateDrawer::~CPlateDrawer(void)
{
	DestroyPlateDrawing(m_pPlateDrawing);
}
