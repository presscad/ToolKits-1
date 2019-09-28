#include "stdafx.h"
#include "SquareTree.h"
#include "f_alg_fun.h"
#include "ArrayList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

POLYGONEX::POLYGONEX()
{

}
POLYGONEX::~POLYGONEX()
{

}
void POLYGONEX::CalLocalCS()
{
	if(!lcs.axis_z.IsZero())
		return;
	lcs.axis_z=zAxis;
	lcs.origin=vertexes[0];
	for(int i=1;lcs.axis_x.IsZero()&&i<count;i++)
		lcs.axis_x=vertexes[i]-vertexes[i-1];
	normalize(lcs.axis_x);
	lcs.axis_y=lcs.axis_z^lcs.axis_x;
}
double POLYGONEX::CalPerimeter()
{
	double fPerimeter=0;
	for(int i=0;i<count;i++)
		fPerimeter+=DISTANCE(vertexes[i],vertexes[(i+1)%count]);
	return fPerimeter;
}
bool IsLineIntersInternal(const double* line1Start,const double* line1End,const double* line2Start,const double* line2End)
{
	GEPOINT v1(line1End[0]-line1Start[0],line1End[1]-line1Start[1]);
	GEPOINT v2(line2End[0]-line2Start[0],line2End[1]-line2Start[1]);
	GEPOINT v(line2Start[0]-line1Start[0],line2Start[1]-line1Start[1]);
	bool startOnRight,endOnRight;
	if(v1.x*v.y-v1.y*v.x<=0) 
		startOnRight=true;	//line2.Start点在1边的右侧
	else
		startOnRight=false;	//点在某条边的左侧
	v.Set(line2End[0]-line1Start[0],line2End[1]-line1Start[1]);
	if(v1.x*v.y-v1.y*v.x<=0) 
		endOnRight=true;	//line2.End点在1边的右侧
	else
		endOnRight=false;	//点在某条边的左侧
	if(startOnRight==endOnRight)
		return false;	//2号直线段位于1号直线段的同一侧,无有效交点
	v.Set(line1Start[0]-line2Start[0],line1Start[1]-line2Start[1]);
	if(v2.x*v.y-v2.y*v.x<=0) 
		startOnRight=true;	//line1.Start点在2边的右侧
	else
		startOnRight=false;	//点在某条边的左侧
	v.Set(line1End[0]-line2Start[0],line1End[1]-line2Start[1]);
	if(v2.x*v.y-v2.y*v.x<=0) 
		endOnRight=true;	//line1.End点在2边的右侧
	else
		endOnRight=false;	//点在某条边的左侧
	if(startOnRight==endOnRight)
		return false;	//1号直线段位于2号直线段的同一侧,无有效交点
	return true;
}
//-1:方格不在多边形面域上;1.方格与多边形面域无重叠;2.方格与多边形域面有重叠(包括多边形在方格内);3.方格在多边形域面内
int POLYGONEX::IncludeState(GEPOINT *xVertexes, int count,const double* x_y,double sqare_width)
{	
	int i;
	DYN_ARRAY<f3dPoint> vertexArr(count);
	for(i=0;i<count;i++)
		vertexArr[i].Set(xVertexes[i].x,xVertexes[i].y);
	f3dPoint sqareArr[4]={GEPOINT(x_y[0],x_y[1]),
						 GEPOINT(x_y[0]+sqare_width,x_y[1]),
						 GEPOINT(x_y[0]+sqare_width,x_y[1]+sqare_width),
						 GEPOINT(x_y[0],x_y[1]+sqare_width)};
	POLYGONEX polygon1,polygon2;
	polygon1.CreatePolygonRgn(vertexArr,count);	//轮廓点的多边形区域
	polygon2.CreatePolygonRgn(sqareArr,4);		//方格的多边形区域
	int ret1=polygon1.PtInRgn2(sqareArr[0]);
	int ret2=polygon1.PtInRgn2(sqareArr[1]);
	int ret3=polygon1.PtInRgn2(sqareArr[2]);
	int ret4=polygon1.PtInRgn2(sqareArr[3]);
	if(ret1==0||ret2==0||ret3==0||ret4==0)
		return -1;	//方格不在多面形面域上
	else if(ret1==3&&ret2==3&&ret3==3&&ret4==3)
	{	//方格在多边形面域外(方格与多边形有重叠|方格与多边形无重叠)
		//1.排除多边形位于方格任一边的外侧
		bool outside=true,inside=true;
		for(i=0;i<count&&outside;i++)	//判断点是否全在方格左边的左侧
			outside=(xVertexes[i].x<=x_y[0]-EPS);
		if(outside)
			return 1;	//SQUARENODE::COVER_NONE
		for(i=0,outside=true;i<count&&outside;i++)	//判断点是否全在方格右边的右侧
			outside=(xVertexes[i].x>=x_y[0]+sqare_width+EPS);
		if(outside)
			return 1;	//SQUARENODE::COVER_NONE
		for(i=0,outside=true;i<count&&outside;i++)	//判断点是否全在方格底边的下侧
			outside=(xVertexes[i].y<=x_y[1]-EPS);
		if(outside)
			return 1;	//SQUARENODE::COVER_NONE
		for(i=0,outside=true;i<count&&outside;i++)	//判断点是否全在方格顶边的上侧
			outside=(xVertexes[i].y>=x_y[1]+sqare_width+EPS);
		if(outside)
			return 1;	//SQUARENODE::COVER_NONE
		//2.排除多边形位于方格内情况
		for(i=0,inside=true;i<count&&inside;i++)	//判断点是否全在方格顶边的上侧
		{
			double deitax=xVertexes[i].x-x_y[0];
			double deitay=xVertexes[i].y-x_y[1];
			if(deitax<-EPS||deitax>sqare_width+EPS || deitay<-EPS||deitay>sqare_width+EPS)
				inside=false;
		}
		if(inside)	//方格全包括了多边形
			return 2;	//SQUARENODE::COVER_WITH
		//3.通过多边形任一边与方格的四边存在有效内交点,判断为有覆盖 wjh-2017.11.23
		bool findInters=false;
		for(i=0;i<count&&!findInters;i++)	//判断点是否全在方格顶边的上侧
		{
			for(int j=0;j<4&&!findInters;j++)
				findInters=IsLineIntersInternal(xVertexes[i],xVertexes[(i+1)%count],sqareArr[j],sqareArr[(j+1)%4]);
		}
		if(!findInters)	//未找到任何边相交情况
			return 1;	//SQUARENODE::COVER_NONE
		else //if(findInters)
			return 2;	//SQUARENODE::COVER_WITH
	}
	else if((ret1==1||ret1==2)&&(ret2==1||ret2==2)&&(ret3==1||ret3==2)&&(ret4==1||ret4==2))
		return 3;		//SQUARENODE::COVER_IN
	else
		return 2;		//SQUARENODE::COVER_WITH
}
#ifdef __ALFA_TEST_VERSION_
#include "DxfFile.h"
void AddSquareNodeGezi(SQUARENODE* pSquareNode,CDxfFile* pDxfile)
{
	for(int i=0;i<4;i++)
	{
		SQUARENODE* pSonNode=(SQUARENODE*)pSquareNode->GetSquareNode(i);
		bool* pbLeafNode=(bool*)pSonNode+4;
		if(*pbLeafNode)
		{
			GEPOINT start(pSonNode->square.X,pSonNode->square.Y);
			double width=pSonNode->square.fHalfWidth*2;
			if(pSonNode->square.state==0||pSonNode->square.state==SQUARENODE::COVER_IN||pSonNode->square.state==SQUARENODE::COVER_WITH)
			{
				pDxfile->NewLine(start,GEPOINT(start.x+width,start.y));
				pDxfile->NewLine(GEPOINT(start.x+width,start.y),GEPOINT(start.x+width,start.y+width));
				pDxfile->NewLine(GEPOINT(start.x+width,start.y+width),GEPOINT(start.x,start.y+width));
				pDxfile->NewLine(GEPOINT(start.x,start.y+width),GEPOINT(start.x,start.y));
			}
			if(pSonNode->square.state==SQUARENODE::COVER_IN)
				pDxfile->NewCircle(GEPOINT(start.x+width*0.5,start.y+width*0.5),width*0.5);
		}
		else
			AddSquareNodeGezi(pSonNode,pDxfile);
	}
}
#endif
double POLYGONEX::OverlappedWith(GEPOINT *vertexes2, int count2,SQUARENODE* pRootNode/*=NULL*/,double DECISION/*=2*/)
{
	int i;
	SCOPE_STRU scope;
	for(i=0;i<count;i++)
		scope.VerifyVertex(vertexes[i]);
	DYN_ARRAY<GEPOINT> planarVertexs(0),planarVertexs2(count2);
	GEPOINT* currVertexesArr=vertexes;
	CalLocalCS();	//计算当前多边形的局部坐标系，并以此坐标为系为参照计算两个多边形的重叠
	bool needTransCS=false;
	if(scope.depth()>EPS)
	{
		//建立局部坐标系，将坐标系统一到局部平面坐标系下
		scope.ClearScope();
		planarVertexs.Resize(count);
		for(i=0;i<count;i++)
		{
			planarVertexs[i]=lcs.TransPToCS(vertexes[i]);
			scope.VerifyVertex(planarVertexs[i]);
		}
		currVertexesArr=planarVertexs;
		needTransCS=true;
	}
	//将传入的vertexes2也转换到当前多边形域平面内
	for(i=0;i<count2;i++)
	{
		if(needTransCS)
			planarVertexs2[i]=lcs.TransPToCS(vertexes2[i]);
		else
			planarVertexs2[i]=vertexes2[i];
		scope.VerifyVertex(planarVertexs2[i]);
	}
	//2.在scope域的X－Y平面内进行四分格扫描重叠区
	//2.1 建立根节点（将全部覆区域分为四个方格）
	GEPOINT center;
	SQUARENODE root;
	double width=max(scope.wide(),scope.high());
	scope.centre(&center.x,&center.y,&center.z);
	SQUARERGN square(center.x-width/2,center.y-width/2,width);
	SQUARENODE* pRoot=(pRootNode)?pRootNode:(&root);
	pRoot->AddSonNode(square);
	DetectSquareCoverState(currVertexesArr,planarVertexs2,count2,pRoot,DECISION);
#ifdef __ALFA_TEST_VERSION_	//测试代码
	double intersarea=pRoot->SumStateWeight(SQUARENODE::COVER_WITH);
	double coverarea=pRoot->SumStateWeight(SQUARENODE::COVER_IN);
	bool create=false;
	if(create)
	{
		CDxfFile dxfile;
		dxfile.OpenFile("d:\\test.dxf");
		for(i=0;i<count;i++)
			dxfile.NewLine(currVertexesArr[i],currVertexesArr[(i+1)%count]);
		AddSquareNodeGezi(pRoot,&dxfile);
		dxfile.CloseFile();
	}
#endif
	return pRoot->SumStateWeight(SQUARENODE::COVER_IN);
}
int POLYGONEX::DetectSquareCoverState(GEPOINT *vertexes1,GEPOINT *vertexes2, int count2,SQUARENODE* pSquareNode,double DECISION/*=1*/)
{
	int rslt1,rslt2;
	double XY[2]={pSquareNode->square.X,pSquareNode->square.Y};
	rslt1=IncludeState(vertexes1,count ,XY,pSquareNode->square.fHalfWidth*2);
	rslt2=IncludeState(vertexes2,count2,XY,pSquareNode->square.fHalfWidth*2);
	if(rslt1==rslt2)
	{
		if(rslt1==SQUARENODE::COVER_IN)
			return pSquareNode->square.state=SQUARENODE::COVER_IN;
		else if(rslt1==SQUARENODE::COVER_NONE)
			return pSquareNode->square.state=SQUARENODE::COVER_NONE;
		else if(pSquareNode->square.fHalfWidth<DECISION)
			return pSquareNode->square.state=SQUARENODE::COVER_WITH;
		else //if(rslt1==SQUARENODE::COVER_WITH)
		{
			pSquareNode->square.state=SQUARENODE::COVER_WITH;
			pSquareNode->AddSonNode(pSquareNode->square);
		}
	}
	else if(rslt1==SQUARENODE::COVER_NONE||rslt2==SQUARENODE::COVER_NONE)
		return pSquareNode->square.state=SQUARENODE::COVER_NONE;
	else if(pSquareNode->square.fHalfWidth<DECISION)
		return pSquareNode->square.state=SQUARENODE::COVER_WITH;	//不再细分
	else
	{
		pSquareNode->square.state=SQUARENODE::COVER_WITH;
		pSquareNode->AddSonNode(pSquareNode->square);
	}
	//继续对方格进行细分
	int state=SQUARENODE::COVER_NONE;
	for(int i=0;i<4;i++)
	{
		if(i==0 || i==2)
			XY[0]=pSquareNode->square.X;
		else
			XY[0]=pSquareNode->square.X+pSquareNode->square.fHalfWidth;
		if(i==0 || i==1)
			XY[1]=pSquareNode->square.Y;
		else
			XY[1]=pSquareNode->square.Y+pSquareNode->square.fHalfWidth;
		SQUARERGN square(XY[0],XY[1],pSquareNode->square.fHalfWidth);
		SQUARENODE* pSonNode=(SQUARENODE*)pSquareNode->GetSquareNode(i);
		pSonNode->square=square;
		int rslt=DetectSquareCoverState(vertexes1,vertexes2,count2,pSonNode,DECISION);
		if(i==0)
			state=rslt;
		else if(state!=rslt)//不完全相同，则视为部分覆盖，需要细分
			state=SQUARENODE::COVER_WITH;
	}
	return state;
}
