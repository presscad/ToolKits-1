#include "stdafx.h"
#include "f_alg_fun.h"
#ifdef __DRAWING_CONTEXT_
#include "..\LDS\MapLds\SysPara.h"
#endif
#include "DrawDim.h"
#include "PartLib.h"

const double halfPi=Pi/2;
double SAFE_TEXTSIZE::minFontSize=1;	//文字最小高度
//全局函数
#ifdef __DRAWING_CONTEXT_
CXhChar16 GetPartNoIncMat(const char* sPartNo,char cMat)
{
	CXhChar16 sPartNoIncMat;
	if(sys.general_set.iMatCharPosType==1&&cMat!=CSteelMatLibrary::Q235BriefMark())
	{
		if(sys.general_set.cMatCharSeparator==0)		//无间隔
			sPartNoIncMat.Printf("%c%s",cMat,sPartNo);
		else if(sys.general_set.cMatCharSeparator==' ')	//空格
			sPartNoIncMat.Printf("%c %s",cMat,sPartNo);
		else
			sPartNoIncMat.Printf("%c%c%s",cMat,sys.general_set.cMatCharSeparator,sPartNo);
	}
	else if(sys.general_set.iMatCharPosType==2&&cMat!=CSteelMatLibrary::Q235BriefMark())
	{	
		if(sys.general_set.cMatCharSeparator==0)		//无间隔
			sPartNoIncMat.Printf("%s%c",sPartNo,cMat);
		else if(sys.general_set.cMatCharSeparator==' ')	//空格
			sPartNoIncMat.Printf("%s %c",sPartNo,cMat);
		else
			sPartNoIncMat.Printf("%s%c%c",sPartNo,sys.general_set.cMatCharSeparator,cMat);
	}
	else 
		sPartNoIncMat.Copy(sPartNo);
	return sPartNoIncMat;
}
#else
CXhChar16 GetPartNoIncMat(const char* sPartNo,char cMat,int iMatCharPosType/*=0*/,char cMatSeparator/*=0*/)
{
	CXhChar16 sPartNoIncMat;
	if(iMatCharPosType==1&&cMat!=CSteelMatLibrary::Q235BriefMark())
	{
		if(cMatSeparator==0)		//无间隔
			sPartNoIncMat.Printf("%c%s",cMat,sPartNo);
		else if(cMatSeparator==' ')	//空格
			sPartNoIncMat.Printf("%c %s",cMat,sPartNo);
		else
			sPartNoIncMat.Printf("%c%c%s",cMat,cMatSeparator,sPartNo);
	}
	else if(iMatCharPosType==2&&cMat!=CSteelMatLibrary::Q235BriefMark())
	{	
		if(cMatSeparator==0)		//无间隔
			sPartNoIncMat.Printf("%s%c",sPartNo,cMat);
		else if(cMatSeparator==' ')	//空格
			sPartNoIncMat.Printf("%s %c",sPartNo,cMat);
		else
			sPartNoIncMat.Printf("%s%c%c",sPartNo,cMatSeparator,cMat);
	}
	else 
		sPartNoIncMat.Copy(sPartNo);
	return sPartNoIncMat;
}
#endif
f3dPoint GetRapidVec(double angle)
{
	if(angle==0)
		return f3dPoint(1,0,0);
	else if(angle==halfPi)
	{
		return f3dPoint(0,1,0);
	}
	else
		return f3dPoint(cos(angle),sin(angle));
}

static f3dPoint GetDatumPos(DIMALIGN_TYPE alignType,f3dPoint rgnVertArr[4])
{
	f3dPoint origin;
	switch(alignType)
	{
	case TopLeft:	//左上
		origin=rgnVertArr[3];
		break;	
	case TopCenter:	//中上
		origin=(rgnVertArr[2]+rgnVertArr[3])*0.5;
		break;
	case TopRight:	//右上
		origin=rgnVertArr[2];
		break;
	case MiddleLeft://左中
		origin=(rgnVertArr[0]+rgnVertArr[3])*0.5;
		break;
	case MiddleCenter://中中
		origin=(rgnVertArr[0]+rgnVertArr[1]+rgnVertArr[2]+rgnVertArr[3])*0.25;
		break;
	case MiddleRight://右中
		origin=(rgnVertArr[1]+rgnVertArr[2])*0.5;
		break;
	case BottomLeft://左下
		origin=rgnVertArr[0];
		break;
	case BottomCenter://中下
		origin=(rgnVertArr[0]+rgnVertArr[1])*0.5;
		break;
	case BottomRight:	//右下
		origin=rgnVertArr[1];
		break;
	default:
		break;
	};
	return origin;
}


//////////////////////////////////////////////////////////////////////
// CSizeAngleDim
//////////////////////////////////////////////////////////////////////
CDimStyle CSizeAngleDim::dimStyle;
CSizeAngleDim::CSizeAngleDim()
{
	cFlag=0;
	dimText.Empty();
}

double CSizeAngleDim::GetWidth()
{
	double dim_r=DISTANCE(dimPos,vertex);
	f3dPoint start_vec=dimStartPos-vertex;
	f3dPoint end_vec=dimEndPos-vertex;
	normalize(start_vec);
	normalize(end_vec);
	f3dPoint startPos=vertex+start_vec*dim_r;
	f3dPoint endPos=vertex+end_vec*dim_r;
	return DISTANCE(startPos,endPos);
}
double CSizeAngleDim::GetHeight()
{
	double dim_r=DISTANCE(dimPos,vertex);
	double width=GetWidth();
	double height=dim_r-sqrt(dim_r*dim_r-0.25*width*width);
	double gap=0.5;
#ifdef __DRAWING_CONTEXT_
	gap=CSizeAngleDim::dimStyle.style.Gap;
#endif
	height+=(gap+CSizeAngleDim::dimStyle.textHeight);
	return height;
}

void CSizeAngleDim::Reset()
{
	f3dPoint v1=dimStartPos-vertex;
	f3dPoint v2=dimEndPos-vertex;
	if(v1.x*v2.y-v1.y*v2.x<=0) 
	{	//始终端应调换
		v1=dimStartPos;
		dimStartPos=dimEndPos;
		dimEndPos=v1;
	}
}
//返回标注矩形区(不含标注箭头)左下rgnVertArr[0];右下rgnVertArr[1];右上rgnVertArr[2];左上四个角点rgnVertArr[3]
void CSizeAngleDim::GetDimTextRect(f3dPoint rgnVertArr[4],double draw_scale/*=1.0*/,double text_size/*=0*/) const
{
	f3dPoint vertV=dimPos-vertex;
	normalize(vertV);
	f3dPoint horiV(vertV.y,-vertV.x);
	if(text_size==0)
		text_size=dimStyle.textHeight;
	double text_height=text_size*draw_scale;	//根据输入比例调整高宽 wht 10-09-16
	rgnVertArr[0]=dimPos-vertV-horiV*(text_height*2.1);	//一律按标注角度为6个字符处理
	rgnVertArr[1]=dimPos-vertV+horiV*(text_height*2.1);
	rgnVertArr[2]=dimPos+vertV+horiV*(text_height*2.1);
	rgnVertArr[3]=dimPos+vertV-horiV*(text_height*2.1);
}
void CSizeAngleDim::GetDimRect(f3dPoint rgnVertArr[4],double draw_scale/*=1.0*/) const
{
	f3dPoint v1=dimStartPos-vertex;
	f3dPoint v2=dimEndPos-vertex;
	normalize(v1);
	normalize(v2);
	f3dPoint midV=v1+v2;
	normalize(midV);
	double dimRadius=DISTANCE(dimPos,vertex);
	double cosHalfA=v1*midV;
	double textHeight=dimStyle.textHeight*draw_scale; //根据输入比例调整高宽 wht 10-09-16
	double rectHeight=dimRadius*(1-cosHalfA)+textHeight;
	rgnVertArr[0]=vertex+v2*dimRadius;
	rgnVertArr[1]=vertex+v1*dimRadius;
	rgnVertArr[2]=rgnVertArr[1]+midV*rectHeight;
	rgnVertArr[3]=rgnVertArr[0]+midV*rectHeight;
}
void CSizeAngleDim::GetDimTextRect(f3dPoint rgnVertArr[4],SAFE_TEXTSIZE &safeTextSize) const
{
	double textsize;
	if(safeTextSize.textsize==0)
		textsize=safeTextSize.EnsureSize(dimStyle.textHeight);
	else
		textsize=safeTextSize.EnsureSize();
	return GetDimTextRect(rgnVertArr,1,textsize);
}
f2dArc CSizeAngleDim::GetDimArcLine()
{
	f2dArc arc;
	arc.centre=vertex;
	Reset();
	f3dPoint startVec=dimStartPos-vertex;
	f3dPoint endVec=dimEndPos-vertex;
	f3dPoint dimVec=dimPos-vertex;
	normalize(startVec);
	normalize(endVec);
	arc.startPt=vertex+startVec*dimVec.mod();
	arc.angle=ACOS(startVec,endVec);
	return arc;
}

double CSizeAngleDim::GetDimAngle()
{
	f3dPoint startVec=dimStartPos-vertex;
	f3dPoint endVec=dimEndPos-vertex;
	normalize(startVec);
	normalize(endVec);
	return ACOS(startVec,endVec);
}

//////////////////////////////////////////////////////////////////////
// tagDIM_FRAMETEXT Construction/Destruction
//////////////////////////////////////////////////////////////////////

double CTextOnlyDrawing::PRECISION=1.0;
double CTextOnlyDrawing::Fillet()
{
	if(PRECISION==0.1)
		dist=ftoi(dist*10)/10;
	else if(PRECISION==0.5) 
		dist=fto_halfi(dist);
	else
		dist=ftoi(dist);
	return dist;
}
//precision只能为"0","0.5","0.0"
const char* CTextOnlyDrawing::GetPrecisionDimText()
{
	if(bNeedConvertDistToText)
	{	//绝对尺寸
		if(PRECISION==0.1)
			dimText.Printf("%.1f",dist);
		else if(PRECISION==0.5) 
		{
			double dimdist=fto_halfi(dist);
			if(dimdist==(int)dimdist)
				dimText.Printf("%.0f",dimdist);
			else
				dimText.Printf("%.1f",dimdist);
		}
		else
			dimText.Printf("%.0f",dist);
	}
	return dimText;
}
//返回标注矩形区(不含标注箭头)左下rgnVertArr[0];右下rgnVertArr[1];右上rgnVertArr[2];左上四个角点rgnVertArr[3]
void CTextOnlyDrawing::GetDimRect(f3dPoint rgnVertArr[4],double draw_scale/*=1.0*/,double textsize/*=0*/)
{
	f3dPoint horiVec=GetRapidVec(angle);
	f3dPoint vertVec(-horiVec.y,horiVec.x);
	double text_width=GetWidth()*draw_scale;	//根据输入比例调整高宽 wht 10-09-16
	if(textsize==0)
		textsize=textHeight;
	double text_height=textsize*draw_scale;
	double half_text_height=0.5*text_height;
	double half_text_width=0.5*text_width;
	if(alignType==MiddleLeft)
	{
		rgnVertArr[0]=dimPos-vertVec*half_text_height+origin;//左下
		rgnVertArr[1]=dimPos-vertVec*half_text_height+horiVec*text_width+origin;//右下
		rgnVertArr[2]=dimPos+vertVec*half_text_height+horiVec*text_width+origin;//右上
		rgnVertArr[3]=dimPos+vertVec*half_text_height+origin;//左上
	}
	else if(alignType==MiddleCenter)
	{
		rgnVertArr[0]=dimPos-vertVec*half_text_height-horiVec*half_text_width+origin;//左下
		rgnVertArr[1]=dimPos-vertVec*half_text_height+horiVec*half_text_width+origin;//右下
		rgnVertArr[2]=dimPos+vertVec*half_text_height-horiVec*half_text_width+origin;//右上
		rgnVertArr[3]=dimPos+vertVec*half_text_height-horiVec*half_text_width+origin;//左上
	}
	else if(alignType==TopLeft)
	{
		rgnVertArr[0]=dimPos-vertVec*text_height+origin;//左下
		rgnVertArr[1]=dimPos-vertVec*text_height+horiVec*text_width+origin;//右下
		rgnVertArr[2]=dimPos+horiVec*half_text_width+origin;//右上
		rgnVertArr[3]=dimPos+origin;//左上
	}
	else if(alignType==TopCenter)
	{
		rgnVertArr[0]=dimPos-vertVec*text_height-horiVec*half_text_width+origin;//左下
		rgnVertArr[1]=dimPos-vertVec*text_height+horiVec*half_text_width+origin;//右下
		rgnVertArr[2]=dimPos+horiVec*half_text_width+origin;//右上
		rgnVertArr[3]=dimPos-horiVec*half_text_width+origin;//左上
	}
	else if(alignType==TopRight)
	{
		rgnVertArr[0]=dimPos-vertVec*text_height-horiVec*text_width+origin;//左下
		rgnVertArr[1]=dimPos-vertVec*text_height+origin;//右下
		rgnVertArr[2]=dimPos+origin;//右上
		rgnVertArr[3]=dimPos-horiVec*text_width+origin;//左上
	}
	else if(alignType==BottomLeft)
	{
		rgnVertArr[0]=dimPos+origin;					//左下
		rgnVertArr[1]=dimPos+horiVec*text_width+origin;	//右下
		rgnVertArr[2]=dimPos+vertVec*text_height+horiVec*text_width+origin;//右上
		rgnVertArr[3]=dimPos+vertVec*text_height+origin;//左上
	}
	else if(alignType==BottomCenter)
	{
		rgnVertArr[0]=dimPos-horiVec*half_text_width+origin;	//左下
		rgnVertArr[1]=dimPos+horiVec*half_text_width+origin;	//右下
		rgnVertArr[2]=dimPos+horiVec*half_text_width+vertVec*text_height+origin;//右上
		rgnVertArr[3]=dimPos-horiVec*half_text_width+vertVec*text_height+origin;//左上
	}
	else if(alignType==BottomRight)
	{
		rgnVertArr[0]=dimPos-horiVec*text_width+origin;	//左下
		rgnVertArr[1]=dimPos+origin;	//右下
		rgnVertArr[2]=dimPos+vertVec*text_height+origin;//右上
		rgnVertArr[3]=dimPos+vertVec*text_height-horiVec*text_width+origin;//左上
	}
	else //if(alignType==MiddleRight)
	{
		rgnVertArr[0]=dimPos-vertVec*half_text_height-horiVec*text_width+origin;//左下
		rgnVertArr[1]=dimPos-vertVec*half_text_height+origin;//右下
		rgnVertArr[2]=dimPos+vertVec*half_text_height+origin;//右上
		rgnVertArr[3]=dimPos+vertVec*half_text_height-horiVec*text_width+origin;//左上
	}
}
void CTextOnlyDrawing::GetDimRect(f3dPoint rgnVertArr[4],SAFE_TEXTSIZE& safetextsize)
{
	double textsize;
	if(safetextsize.textsize==0)
		textsize=safetextsize.EnsureSize(textHeight);
	else
		textsize=safetextsize.EnsureSize();
	GetDimRect(rgnVertArr,1,textsize);
}
double CTextOnlyDrawing::GetHeight()
{
	return textHeight;
}
double CTextOnlyDrawing::GetWidth()
{
	int text_len=(int)strlen(GetPrecisionDimText());
	return 0.7*textHeight*text_len;
}
void CTextOnlyDrawing::ResetPosition(DIMALIGN_TYPE align/*=AlignDefault*/,bool bAccumulation/*=false*/)
{	
	double half_text_width=0.5*GetWidth();
	double half_text_height=0.5*GetHeight();
	f3dPoint horiVec=GetRapidVec(angle);
	f3dPoint vertVec(-horiVec.y,horiVec.x);
	if(align==AlignDefault)
	{
		if(dimPos*horiVec>0)
			alignType=MiddleLeft;
		else
			alignType=MiddleRight;
	}
	else
		alignType=align;
	if(bAccumulation)
		origin+=dimPos;
	else
		origin=dimPos;
	dimPos.Set(0,0,0);
}

//////////////////////////////////////////////////////////////////////
// CSizeTextDim
//////////////////////////////////////////////////////////////////////
double CSizeTextDim::PRECISION=1.0;
CDimStyle CSizeTextDim::dimStyle;
CSizeTextDim::CSizeTextDim()
{
	nHits=1;
	iOrder=0;
	iTextDimSylte=0;
	angle=dist=0;
	cFlag=0;
	bShortenDimStartLine=FALSE;
	bShortenDimEndLine=FALSE;
	bAllowRelayout=TRUE;
	bCanTextOutter=FALSE;
}
CSizeTextDim::CSizeTextDim(const double* dimStartCoords,const double* dimEndCoords,const double* dimPosCoords,
	double fDist,double angle,BOOL bAllowRelayout/*=FALSE*/,BOOL bCanTextOutter/*=FALSE*/)
{
	dimStartPos=f3dPoint(dimStartCoords);
	dimEndPos=f3dPoint(dimEndCoords);
	dimPos=f3dPoint(dimPosCoords);
	dist=fDist;
	CSizeTextDim::angle=angle;
	CSizeTextDim::bAllowRelayout=bAllowRelayout;
	CSizeTextDim::bCanTextOutter=bCanTextOutter;
	bShortenDimStartLine=FALSE;
	bShortenDimEndLine=FALSE;
	iOrder = 0;
	nHits=1;
	cFlag=0;
}
double CSizeTextDim::Fillet()	//将标注距离dist按照precision进行圆整化
{
	if(PRECISION==0.1)
		dist=ftoi(dist*10)/10;
	else if(PRECISION==0.5) 
		dist=fto_halfi(dist);
	else
		dist=ftoi(dist);
	return dist;
}

const char* CSizeTextDim::DimText()
{
	if(PRECISION==0.1)
		dimText.Printf("%.1f",dist);
	else if(PRECISION==0.5) 
	{
		double dimdist=fto_halfi(dist);
		if(dimdist==(int)dimdist)
			dimText.Printf("%.0f",dimdist);
		else
			dimText.Printf("%.1f",dimdist);
	}
	else
		dimText.Printf("%.0f",dist);
	if(nHits>1)
	{	//多次连续标注合并
		CXhChar16 temp;
		temp.Printf("X%d",nHits);
		dimText.Append(temp);
	}
	return dimText;
}

double CSizeTextDim::GetWidth()
{
	f3dPoint horiVec(cos(angle),sin(angle));
	f3dPoint start_perp,end_perp;
	SnapPerp(&start_perp,dimPos+horiVec*1000,dimPos-horiVec*1000,dimStartPos);
	SnapPerp(&end_perp,dimPos+horiVec*1000,dimPos-horiVec*1000,dimEndPos);
	return DISTANCE(start_perp,end_perp);
}
double CSizeTextDim::GetHeight(BOOL bIncShortDimLineOnly/*=FALSE*/)
{
	f3dPoint horiVec(cos(angle),sin(angle));
	double start_dist=0,end_dist=0,max_dist=5;	//wbt：这个地方等定义好标注类之后，再做修改。
#ifdef __DRAWING_CONTEXT_
	max_dist=dimStyle.style.arrowSize*0.5;
#endif
	f3dPoint start_perp,end_perp;
	SnapPerp(&start_perp,dimPos+horiVec*1000,dimPos-horiVec*1000,dimStartPos,&start_dist);
	SnapPerp(&end_perp,dimPos+horiVec*1000,dimPos-horiVec*1000,dimEndPos,&end_dist);
	if(start_dist>max_dist)	
		max_dist=start_dist;
	if(end_dist>start_dist)
		max_dist=end_dist;
#ifdef __DRAWING_CONTEXT_
	return (max_dist+dimStyle.style.Gap+dimStyle.textHeight);
#endif
	return (max_dist+0.5+dimStyle.textHeight);
}
//返回标注矩形区(不含标注箭头)左下rgnVertArr[0];右下rgnVertArr[1];右上rgnVertArr[2];左上四个角点rgnVertArr[3]
void CSizeTextDim::GetDimRect(f3dPoint rgnVertArr[4],BOOL bIncShortDimLineOnly/*=FALSE*/,double draw_scale/*=1.0*/,double textsize/*=0*/) const
{
	f3dPoint horiVec=GetRapidVec(angle);
	f3dPoint vertVec(-horiVec.y,horiVec.x);
	f3dPoint startPerp,endPerp,verifyVec;
	SnapPerp(&startPerp,dimStartPos,dimStartPos+vertVec,dimPos);
	SnapPerp(&endPerp,dimEndPos,dimEndPos+vertVec,dimPos);
	verifyVec=dimPos-dimStartPos;
	if(textsize==0)
		textsize=dimStyle.textHeight;
	double text_height=textsize*draw_scale; //根据输入比例调整高宽 wht 10-09-16
	double startDist=fabs((startPerp-dimStartPos)*vertVec);
	double endDist=fabs((endPerp-dimEndPos)*vertVec);
	double maxDist=max(startDist,endDist);
#ifdef  __DRAWING_CONTEXT_
	double upDist=max(maxDist,text_height+dimStyle.style.Gap);
#else
	double upDist=max(maxDist,text_height+0.25*text_height);
#endif
	if(verifyVec*vertVec>0)
	{	//标注线基点在标注点上侧
		if(iTextDimSylte==1)
		{	//字体在标注线下侧
			rgnVertArr[0]=startPerp-vertVec*upDist+origin;
			rgnVertArr[1]=endPerp-vertVec*upDist+origin;
			rgnVertArr[2]=endPerp+vertVec*text_height*0.5+origin;
			rgnVertArr[3]=startPerp+vertVec*text_height*0.5+origin;
		}
		else
		{	//字体在标注线上侧
			rgnVertArr[0]=startPerp-vertVec*maxDist+origin;
			rgnVertArr[1]=endPerp-vertVec*maxDist+origin;
#ifdef  __DRAWING_CONTEXT_
			rgnVertArr[2]=endPerp+vertVec*(text_height+dimStyle.style.Gap)+origin;
			rgnVertArr[3]=startPerp+vertVec*(text_height+dimStyle.style.Gap)+origin;
#else
			rgnVertArr[2]=endPerp+vertVec*(text_height+0.25*text_height)+origin;
			rgnVertArr[3]=startPerp+vertVec*(text_height+0.25*text_height)+origin;
#endif	
		}
	}
	else
	{	//标注线基点在标注点下侧
		if(iTextDimSylte==1)
		{	//字体在标注线下侧
#ifdef  __DRAWING_CONTEXT_
			rgnVertArr[0]=startPerp-vertVec*(text_height+dimStyle.style.Gap)+origin;
			rgnVertArr[1]=endPerp-vertVec*(text_height+dimStyle.style.Gap)+origin;
#else
			rgnVertArr[0]=startPerp-vertVec*(text_height+0.25*text_height)+origin;
			rgnVertArr[1]=endPerp-vertVec*(text_height+0.25*text_height)+origin;
#endif
			rgnVertArr[2]=endPerp+vertVec*maxDist+origin;
			rgnVertArr[3]=startPerp+vertVec*maxDist+origin;
		}
		else
		{	//字体在标注线上册
			rgnVertArr[0]=startPerp-vertVec*text_height*0.5+origin;
			rgnVertArr[1]=endPerp-vertVec*text_height*0.5+origin;
			rgnVertArr[2]=endPerp+vertVec*upDist+origin;
			rgnVertArr[3]=startPerp+vertVec*upDist+origin;
		}
	}
}
void CSizeTextDim::GetDimRect(f3dPoint rgnVertArr[4],BOOL bIncShortDimLineOnly,SAFE_TEXTSIZE& safetextsize) const
{
	double textsize;
	if(safetextsize.textsize==0)
		textsize=safetextsize.EnsureSize(dimStyle.textHeight);
	else
		textsize=safetextsize.EnsureSize();
	GetDimRect(rgnVertArr,bIncShortDimLineOnly,1,textsize);
}
void CSizeTextDim::ResetPosition(DIMALIGN_TYPE align/*=AlignDefault*/,bool bAccumulation/*=false*/)
{
	double half_dim_width=0.5*GetWidth();	//标注区域宽度的一半
	double dim_height=GetHeight();			//标注区域高度
	double up_height=0.5+dimStyle.textHeight;//上侧高度(字体高度与标注内容与标注线之间的间隙值之和)
#ifdef __DRAWING_CONTEXT_
	up_height=dimStyle.style.Gap+dimStyle.textHeight;//上侧高度(字体高度与标注内容与标注线之间的间隙值之和)
#endif
	double down_height=dim_height-up_height;//下侧高度

	f3dPoint horiVec=GetRapidVec(angle);
	f3dPoint vertVec(-horiVec.y,horiVec.x);
	f3dPoint verifyVec=dimPos-dimStartPos;
	double verify=verifyVec*vertVec;
	if(verify>0)	//标注点在标注起始点上端,
	{
		if(iTextDimSylte==1)		//标注文字在标注线下侧
		{
#ifdef __DRAWING_CONTEXT_
			dimPos=dimStartPos+vertVec*(1.5*dimStyle.textHeight+dimStyle.style.Gap)+horiVec;
#else
			dimPos=dimStartPos+vertVec*(1.5*dimStyle.textHeight+0.5)+horiVec;
#endif
		}
		else if(iTextDimSylte==0)	//标注文字在标注线上侧
			dimPos=dimStartPos+vertVec*0.5*dimStyle.textHeight+horiVec;
	}
	else		//标注点在标注起始点下端,
	{
		if(iTextDimSylte==1)	//标注文字在标注线下侧
			dimPos=dimStartPos-vertVec*0.5*dimStyle.textHeight-horiVec;
		else if(iTextDimSylte==0)	//标注文字在标注线上侧
		{
#ifdef __DRAWING_CONTEXT_
			dimPos=dimStartPos-vertVec*(1.5*dimStyle.textHeight+dimStyle.style.Gap)+horiVec;
#else
			dimPos=dimStartPos-vertVec*(1.5*dimStyle.textHeight+0.5)+horiVec;
#endif
		}
	}
	if(align==AlignDefault)
	{
		if(verify>0)
			alignType=BottomCenter;
		else
			alignType=TopCenter;
	}
	else
		alignType=align;
	f3dPoint rgnVertArr[4];
	GetDimRect(rgnVertArr);
	origin=GetDatumPos(alignType,rgnVertArr);
	if(!bAccumulation)
	{
		dimPos-=origin;
		dimStartPos-=origin;
		dimEndPos-=origin;
	}
}
//TODO:为什么要增加ResetPosition2未注明原因和与ResetPosition间的区别 wjh-2017.1.11
//重新计算标注位置：不需要引出线
void CSizeTextDim::ResetPosition2(DIMALIGN_TYPE align/*=AlignDefault*/)
{
	dimPos=(dimStartPos+dimEndPos)*0.5;
	if(align==AlignDefault)
	{
		if(iTextDimSylte==1)	//标注文字在标注线下侧
			alignType=TopCenter;
		else					//标注文字在标注线上侧
			alignType=BottomCenter;
	}
	else
		alignType=align;
	f3dPoint rgnVertArr[4];
	GetDimRect(rgnVertArr);
	origin=GetDatumPos(alignType,rgnVertArr);
	dimPos-=origin;
	dimStartPos-=origin;
	dimEndPos-=origin;
}
//////////////////////////////////////////////////////////////////////
// CKaiHeAngleDim
//////////////////////////////////////////////////////////////////////
double CKaiHeAngleDim::GetHeight()
{
	f3dPoint horiVec(1,0,0);
	double start_dist=0,end_dist=0;
	double max_dist=0.5+dimStyle.textHeight;
#ifdef __DRAWING_CONTEXT_
	max_dist=dimStyle.style.Gap+dimStyle.textHeight;
#endif
	f3dPoint start_perp,end_perp;
	SnapPerp(&start_perp,dimPos+horiVec*1000,dimPos-horiVec*1000,dimStartPos,&start_dist);
	SnapPerp(&end_perp,dimPos+horiVec*1000,dimPos-horiVec*1000,dimEndPos,&end_dist);
	if(start_dist>max_dist)	
		max_dist=start_dist;
	if(end_dist>start_dist)
		max_dist=end_dist;
#ifdef __DRAWING_CONTEXT_
	return (max_dist+dimStyle.style.Gap+dimStyle.textHeight);
#endif
	return (max_dist+0.5+dimStyle.textHeight);
}
//返回标注矩形区(不含标注箭头)左下rgnVertArr[0];右下rgnVertArr[1];右上rgnVertArr[2];左上四个角点rgnVertArr[3]
void CKaiHeAngleDim::GetDimRect(f3dPoint rgnVertArr[4])
{
	f3dPoint horiVec(1,0,0);
	f3dPoint vertVec(0,1,0);
	SCOPE_STRU scope;
	scope.VerifyVertex(dimStartPos+origin);
	scope.VerifyVertex(dimEndPos+origin);
	if(bInWingX)
		scope.fMinY=dimPos.y+origin.y-1.5*dimStyle.textHeight;
	else
		scope.fMaxY=dimPos.y+origin.y+1.5*dimStyle.textHeight;
	rgnVertArr[0].Set(scope.fMinX,scope.fMinY);	//左下
	rgnVertArr[1].Set(scope.fMaxX,scope.fMinY);	//右下
	rgnVertArr[2].Set(scope.fMaxX,scope.fMaxY);	//右上
	rgnVertArr[3].Set(scope.fMinX,scope.fMaxY);	//左上
}

double CKaiHeAngleDim::GetWidth()
{
	f3dPoint horiVec(1,0,0);
	f3dPoint start_perp,end_perp;
	SnapPerp(&start_perp,dimPos+horiVec*1000,dimPos-horiVec*1000,dimStartPos);
	SnapPerp(&end_perp,dimPos+horiVec*1000,dimPos-horiVec*1000,dimEndPos);
	return DISTANCE(start_perp,end_perp);
}
void CKaiHeAngleDim::ResetPosition()
{
	f3dPoint horiVec=GetRapidVec(0);
	f3dPoint vertVec(-horiVec.y,horiVec.x);
	f3dPoint verifyVec=dimPos-dimStartPos;
	if(bInWingX)
	{
		alignType=TopCenter;
#ifdef __DRAWING_CONTEXT_
		dimPos=dimStartPos-vertVec*(2*dimStyle.textHeight+3*dimStyle.style.Gap)+horiVec;
#else
		dimPos=dimStartPos-vertVec*(2*dimStyle.textHeight+1.5)+horiVec;
#endif
	}
	else
	{
		alignType=BottomCenter;
#ifdef __DRAWING_CONTEXT_
		dimPos=dimStartPos+vertVec*(2*dimStyle.textHeight+3*dimStyle.style.Gap)+horiVec;
#else	//TODO:临时代码, 等将来LDS项目下的JgDrawing.cpp与TowerCad中的完全共用时应统一为上面代码 wjh-2015.2.28
		dimPos=dimStartPos+vertVec*(2*dimStyle.textHeight+1.5)+horiVec;
#endif
	}
	f3dPoint rgnVertArr[4];
	GetDimRect(rgnVertArr);
	origin=GetDatumPos(alignType,rgnVertArr);
	dimPos-=origin;
	dimStartPos-=origin;
	dimEndPos-=origin;
}

//////////////////////////////////////////////////////////////////////
// tagDIM_FRAMETEXT Construction/Destruction
//////////////////////////////////////////////////////////////////////
const double CTextFrame::BULGERECT_EXTEND_COEF = 0.1;
CTextFrame::CTextFrame(){
	frameStyle=0;
	margin = 0.2;
	textHeight=2.5;
	WtoH=0.7;
	frameHeight=frameWidth=7;
	dataPos.Set();	//数据位置
	dimPos.Set();	//标注位置
	hPart=0;		//关联构件句柄
	cls_id=0;		//关联构件类型
	iCurLayer=0;	//文本所在图层
	textAlignHori=ACDB::kTextCenter;
	textAlignVert=ACDB::kTextVertMid;
	dwKeyTag=0;
	cFlag=0;
}

//初始化
void CTextFrame::InitTextFrame(double text_height/*=2.5*/,double frame_width/*=0*/,
	int frame_style/*=-1*/,double space/*=0.3*/,double w_to_h/*=0.7*/)
{
	textHeight=text_height;
	frameHeight=frameWidth=frame_width;
	frameStyle=frame_style;
	margin=max(space,0.5);	//对于腰圆最小间隙值为0.5mm比较恰当，太小了上下边会与字分不清 wjh 2016-10-18
	WtoH=w_to_h;
}

bool CTextFrame::BuildFrame(char* dimText,FRAMESIZE& frame)
{
	return BuildFrame(dimText,NULL,frame.fFrameWidth,frame.siFrameStyle,frame.fTextHeight,frame.fMargin);
}
bool CTextFrame::BuildFrame(char* dimText,char* dimText2,FRAMESIZE& frame)
{
	return BuildFrame(dimText,dimText2,frame.fFrameWidth,frame.siFrameStyle,frame.fTextHeight,frame.fMargin);
}
//智能建立边框 frameStyle:-1.自动判断;0.圆圈;1.腰圆矩形框;2.普通矩形框
bool CTextFrame::BuildFrame(char* dim_text,char* dim_text2,double frame_width/*=0*/,int frame_style/*=-1*/,
	double text_height/*=0*/,double _margin/*=0*/)
{
	dimText.Copy(dim_text);
	if(dim_text2!=NULL)
		dimText2.Copy(dim_text2);
	if(text_height>EPS)
		textHeight=text_height;
	if(_margin>EPS)
		margin=_margin;
	double len=dimText.GetLength()*textHeight*WtoH;	//实际需要的文本长度
	if(frame_width>0)	//指定边框宽度
		frameHeight=frameWidth=frame_width;
	//边框宽度小于等于0时自动计算边框宽度
	if(int(frameWidth)<=0)
		frameWidth=len;
	if(frame_style<0||frame_style>2)
	{	//自动判断文本边框的类型
		if(len>frameWidth-margin*2)
		{
			frameStyle=1;	//腰圆矩形边框
			frameWidth=len+margin*2;
		}
		else
			frameStyle=0;	//圆圈边框
	}
	else //指定边框类型
		frameStyle=frame_style;

	//根据边框宽度计算边框高度
	if(frameStyle==0)	//圆圈型边框
		frameHeight=frameWidth;
	else
	{	//指定边框类型为矩形框时自动调整边框宽度
		if(len>frameWidth-margin*2&&frame_style>0)
			frameWidth=len+margin*2;
		frameHeight=textHeight+margin*2;
		if(dimText2.GetLength()>0)
			frameHeight*=2;
	}
	return true;
}
void CTextFrame::ZoomFrameSize(double zoomscale_coef,double fMinFontSize/*=1.0*/)
{
	if(zoomscale_coef<=EPS)
		return;
	if(fMinFontSize>EPS&&textHeight*zoomscale_coef<fMinFontSize)
		zoomscale_coef=fMinFontSize/textHeight;
	textHeight*=zoomscale_coef;
	frameHeight*=zoomscale_coef;
	frameWidth*=zoomscale_coef;
	margin*=zoomscale_coef;
}

void CTextFrame::GetDimRect(f3dPoint rgnVertArr[4],double draw_scale/*=1.0*/) const
{
	double fHalfWidth=0.5*frameWidth*draw_scale;	//根据输入比例调整高宽 wht 10-09-16
	double fHalfHeight=0.5*frameHeight*draw_scale;
	//矩形框
	if(frameStyle==1)	//0.圆圈;1.腰圆矩形框;2.普通矩形框
	{	//根据绘制经验腰圆类型的矩形框范围比普通矩形框要宽一些更合理 wjh-2017.1.11
		rgnVertArr[0]=dimPos+f3dPoint(-fHalfWidth-fHalfHeight*BULGERECT_EXTEND_COEF,-fHalfHeight);
		rgnVertArr[1]=dimPos+f3dPoint( fHalfWidth+fHalfHeight*BULGERECT_EXTEND_COEF,-fHalfHeight);
		rgnVertArr[2]=dimPos+f3dPoint( fHalfWidth+fHalfHeight*BULGERECT_EXTEND_COEF, fHalfHeight);
		rgnVertArr[3]=dimPos+f3dPoint(-fHalfWidth-fHalfHeight*BULGERECT_EXTEND_COEF, fHalfHeight);
	}
	else //if(frameStyle==0||frameStyle==2)
	{
		rgnVertArr[0]=dimPos+f3dPoint(-fHalfWidth,-fHalfHeight);
		rgnVertArr[1]=dimPos+f3dPoint( fHalfWidth,-fHalfHeight);
		rgnVertArr[2]=dimPos+f3dPoint( fHalfWidth, fHalfHeight);
		rgnVertArr[3]=dimPos+f3dPoint(-fHalfWidth, fHalfHeight);
	}
}
//获得引出线与文本框连接端的端点位置
f3dPoint CTextFrame::GetRefLineEndPos(double draw_scale/*=1.0*/)
{
	f3dPoint refVec=dimPos-dataPos;
	normalize(refVec);
	if(refVec.IsZero())
		return dataPos;
	double fram_width=frameWidth*draw_scale;	//根据比例调整宽度
	double fram_height=frameHeight*draw_scale;	//根据比例调整高度
	if(frameStyle==0)	//圆圈型边框
		return dimPos-refVec*(fram_width*0.5);
	else	//矩形或腰圆矩形框
	{
		if(fabs(refVec.x)>EPS&&fabs(refVec.y/refVec.x)<fram_height/fram_width)
		{	//引出线终端连接点在文本框左右侧
			if(refVec.x<0)	//引出线终端在文本框右侧
				return dataPos+refVec*((dimPos.x+0.5*fram_width-dataPos.x)*(1/refVec.x));
			else			//引出线终端在文本框左侧
				return dataPos+refVec*((dimPos.x-0.5*fram_width-dataPos.x)*(1/refVec.x));
		}
		else
		{	//引出线终端在文本框上下侧
			if(refVec.y<0)	//引出线终端在文本框上侧
				return dataPos+refVec*((dimPos.y+0.5*fram_height-dataPos.y)*(1/refVec.y));
			else			//引出线终端在文本框下侧
				return dataPos+refVec*((dimPos.y-0.5*fram_height-dataPos.y)*(1/refVec.y));
		}
	}
}
//更新数据位置
void CTextFrame::UpdateDataPos(CHashList<DATAPOINT_TAG>& xDataPointTagHash)
{
	DATAPOINT_TAG* pTag=xDataPointTagHash.GetValue(dwKeyTag);
	if(pTag==NULL||pTag->vertexArr.GetSize()<=0)
		return;
	f3dPoint* pPointPre=pTag->vertexArr.GetFirst();
	f3dPoint* pPointFir=pPointPre;
	f3dLine minLine(dimPos,*pPointPre),line;//与关联构件最短的线段
	f3dPoint pedalPoint;
	double fDist=0,minLengthSquare=(minLine.startPt.x-minLine.endPt.x)*(minLine.startPt.x-minLine.endPt.x)+(minLine.startPt.y-minLine.endPt.y)*(minLine.startPt.y-minLine.endPt.y);
	for(f3dPoint* pPointCur=pTag->vertexArr.GetNext();pPointCur!=NULL;pPointCur=pTag->vertexArr.GetNext())
	{
		//dim与顶点的较小距离
		double lengthSquare=(dimPos.x-pPointCur->x)*(dimPos.x-pPointCur->x)+(dimPos.y-pPointCur->y)*(dimPos.y-pPointCur->y);
		if(lengthSquare<minLengthSquare)
		{
			minLine.endPt=*pPointCur;
			minLengthSquare=lengthSquare;
		}
		//dimpos与边的垂线是否在直线上
		line.startPt=*pPointPre;
		line.endPt=*pPointCur;
		if(line.startPt.IsEqual(line.endPt))
		{
			//TODO:有重合情况出现
			pedalPoint=dimPos;
		}
		else
			pedalPoint=line.SnapPerp(dimPos,&fDist);
		if(line.PtInLine(pedalPoint)==2&&fDist*fDist<minLengthSquare)
		{
				minLine.endPt=pedalPoint;
				minLengthSquare=fDist*fDist;
		}
		pPointPre=pPointCur;
	}
	if(*pPointPre!=*pPointFir)
	{
		line.startPt=*pPointPre;
		line.endPt=*pPointFir;
		pedalPoint=line.SnapPerp(dimPos,&fDist);
		if(line.PtInLine(pedalPoint)==2&&fDist*fDist<minLengthSquare)
		{
			minLine.endPt=pedalPoint;
			minLengthSquare=fDist*fDist;
		}
	}
	dataPos=minLine.endPt;
}