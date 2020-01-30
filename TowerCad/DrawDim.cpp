#include "stdafx.h"
#include "f_alg_fun.h"
#ifdef __DRAWING_CONTEXT_
//#include "..\LDS\RxTools.h"
#include "..\LDS\MapLds\SysPara.h"
#endif
#include "PartLib.h"
#include "DrawDim.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const double halfPi=Pi/2;
double SAFE_TEXTSIZE::minFontSize=1;	//������С�߶�
//ȫ�ֺ���
#ifdef __DRAWING_CONTEXT_
CXhChar16 GetPartNoIncMat(const char* sPartNo,char cMat)
{
	CXhChar16 sPartNoIncMat;
	if(sys.general_set.iMatCharPosType==1&&cMat!=CSteelMatLibrary::Q235BriefMark())
	{
		if(sys.general_set.cMatCharSeparator==0)		//�޼��
			sPartNoIncMat.Printf("%c%s",cMat,sPartNo);
		else if(sys.general_set.cMatCharSeparator==' ')	//�ո�
			sPartNoIncMat.Printf("%c %s",cMat,sPartNo);
		else
			sPartNoIncMat.Printf("%c%c%s",cMat,sys.general_set.cMatCharSeparator,sPartNo);
	}
	else if(sys.general_set.iMatCharPosType==2&&cMat!=CSteelMatLibrary::Q235BriefMark())
	{	
		if(sys.general_set.cMatCharSeparator==0)		//�޼��
			sPartNoIncMat.Printf("%s%c",sPartNo,cMat);
		else if(sys.general_set.cMatCharSeparator==' ')	//�ո�
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
		if(cMatSeparator==0)		//�޼��
			sPartNoIncMat.Printf("%c%s",cMat,sPartNo);
		else if(cMatSeparator==' ')	//�ո�
			sPartNoIncMat.Printf("%c %s",cMat,sPartNo);
		else
			sPartNoIncMat.Printf("%c%c%s",cMat,cMatSeparator,sPartNo);
	}
	else if(iMatCharPosType==2&&cMat!=CSteelMatLibrary::Q235BriefMark())
	{	
		if(cMatSeparator==0)		//�޼��
			sPartNoIncMat.Printf("%s%c",sPartNo,cMat);
		else if(cMatSeparator==' ')	//�ո�
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
	case TopLeft:	//����
		origin=rgnVertArr[3];
		break;	
	case TopCenter:	//����
		origin=(rgnVertArr[2]+rgnVertArr[3])*0.5;
		break;
	case TopRight:	//����
		origin=rgnVertArr[2];
		break;
	case MiddleLeft://����
		origin=(rgnVertArr[0]+rgnVertArr[3])*0.5;
		break;
	case MiddleCenter://����
		origin=(rgnVertArr[0]+rgnVertArr[1]+rgnVertArr[2]+rgnVertArr[3])*0.25;
		break;
	case MiddleRight://����
		origin=(rgnVertArr[1]+rgnVertArr[2])*0.5;
		break;
	case BottomLeft://����
		origin=rgnVertArr[0];
		break;
	case BottomCenter://����
		origin=(rgnVertArr[0]+rgnVertArr[1])*0.5;
		break;
	case BottomRight:	//����
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
	{	//ʼ�ն�Ӧ����
		v1=dimStartPos;
		dimStartPos=dimEndPos;
		dimEndPos=v1;
	}
}
//���ر�ע������(������ע��ͷ)����rgnVertArr[0];����rgnVertArr[1];����rgnVertArr[2];�����ĸ��ǵ�rgnVertArr[3]
void CSizeAngleDim::GetDimTextRect(f3dPoint rgnVertArr[4],double draw_scale/*=1.0*/,double text_size/*=0*/) const
{
	f3dPoint vertV=dimPos-vertex;
	normalize(vertV);
	f3dPoint horiV(vertV.y,-vertV.x);
	if(text_size==0)
		text_size=dimStyle.textHeight;
	double text_height=text_size*draw_scale;	//����������������߿� wht 10-09-16
	rgnVertArr[0]=dimPos-vertV-horiV*(text_height*2.1);	//һ�ɰ���ע�Ƕ�Ϊ6���ַ�����
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
	double textHeight=dimStyle.textHeight*draw_scale; //����������������߿� wht 10-09-16
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
//precisionֻ��Ϊ"0","0.5","0.0"
const char* CTextOnlyDrawing::GetPrecisionDimText()
{
	if(bNeedConvertDistToText)
	{	//���Գߴ�
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
//���ر�ע������(������ע��ͷ)����rgnVertArr[0];����rgnVertArr[1];����rgnVertArr[2];�����ĸ��ǵ�rgnVertArr[3]
void CTextOnlyDrawing::GetDimRect(f3dPoint rgnVertArr[4],double draw_scale/*=1.0*/,double textsize/*=0*/)
{
	f3dPoint horiVec=GetRapidVec(angle);
	f3dPoint vertVec(-horiVec.y,horiVec.x);
	double text_width=GetWidth()*draw_scale;	//����������������߿� wht 10-09-16
	if(textsize==0)
		textsize=textHeight;
	double text_height=textsize*draw_scale;
	double half_text_height=0.5*text_height;
	double half_text_width=0.5*text_width;
	if(alignType==MiddleLeft)
	{
		rgnVertArr[0]=dimPos-vertVec*half_text_height+origin;//����
		rgnVertArr[1]=dimPos-vertVec*half_text_height+horiVec*text_width+origin;//����
		rgnVertArr[2]=dimPos+vertVec*half_text_height+horiVec*text_width+origin;//����
		rgnVertArr[3]=dimPos+vertVec*half_text_height+origin;//����
	}
	else if(alignType==MiddleCenter)
	{
		rgnVertArr[0]=dimPos-vertVec*half_text_height-horiVec*half_text_width+origin;//����
		rgnVertArr[1]=dimPos-vertVec*half_text_height+horiVec*half_text_width+origin;//����
		rgnVertArr[2]=dimPos+vertVec*half_text_height-horiVec*half_text_width+origin;//����
		rgnVertArr[3]=dimPos+vertVec*half_text_height-horiVec*half_text_width+origin;//����
	}
	else if(alignType==TopLeft)
	{
		rgnVertArr[0]=dimPos-vertVec*text_height+origin;//����
		rgnVertArr[1]=dimPos-vertVec*text_height+horiVec*text_width+origin;//����
		rgnVertArr[2]=dimPos+horiVec*half_text_width+origin;//����
		rgnVertArr[3]=dimPos+origin;//����
	}
	else if(alignType==TopCenter)
	{
		rgnVertArr[0]=dimPos-vertVec*text_height-horiVec*half_text_width+origin;//����
		rgnVertArr[1]=dimPos-vertVec*text_height+horiVec*half_text_width+origin;//����
		rgnVertArr[2]=dimPos+horiVec*half_text_width+origin;//����
		rgnVertArr[3]=dimPos-horiVec*half_text_width+origin;//����
	}
	else if(alignType==TopRight)
	{
		rgnVertArr[0]=dimPos-vertVec*text_height-horiVec*text_width+origin;//����
		rgnVertArr[1]=dimPos-vertVec*text_height+origin;//����
		rgnVertArr[2]=dimPos+origin;//����
		rgnVertArr[3]=dimPos-horiVec*text_width+origin;//����
	}
	else if(alignType==BottomLeft)
	{
		rgnVertArr[0]=dimPos+origin;					//����
		rgnVertArr[1]=dimPos+horiVec*text_width+origin;	//����
		rgnVertArr[2]=dimPos+vertVec*text_height+horiVec*text_width+origin;//����
		rgnVertArr[3]=dimPos+vertVec*text_height+origin;//����
	}
	else if(alignType==BottomCenter)
	{
		rgnVertArr[0]=dimPos-horiVec*half_text_width+origin;	//����
		rgnVertArr[1]=dimPos+horiVec*half_text_width+origin;	//����
		rgnVertArr[2]=dimPos+horiVec*half_text_width+vertVec*text_height+origin;//����
		rgnVertArr[3]=dimPos-horiVec*half_text_width+vertVec*text_height+origin;//����
	}
	else if(alignType==BottomRight)
	{
		rgnVertArr[0]=dimPos-horiVec*text_width+origin;	//����
		rgnVertArr[1]=dimPos+origin;	//����
		rgnVertArr[2]=dimPos+vertVec*text_height+origin;//����
		rgnVertArr[3]=dimPos+vertVec*text_height-horiVec*text_width+origin;//����
	}
	else //if(alignType==MiddleRight)
	{
		rgnVertArr[0]=dimPos-vertVec*half_text_height-horiVec*text_width+origin;//����
		rgnVertArr[1]=dimPos-vertVec*half_text_height+origin;//����
		rgnVertArr[2]=dimPos+vertVec*half_text_height+origin;//����
		rgnVertArr[3]=dimPos+vertVec*half_text_height-horiVec*text_width+origin;//����
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
double CSizeTextDim::Fillet()	//����ע����dist����precision����Բ����
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
	{	//���������ע�ϲ�
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
	double start_dist=0,end_dist=0,max_dist=5;	//wbt������ط��ȶ���ñ�ע��֮�������޸ġ�
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
//���ر�ע������(������ע��ͷ)����rgnVertArr[0];����rgnVertArr[1];����rgnVertArr[2];�����ĸ��ǵ�rgnVertArr[3]
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
	double text_height=textsize*draw_scale; //����������������߿� wht 10-09-16
	double startDist=fabs((startPerp-dimStartPos)*vertVec);
	double endDist=fabs((endPerp-dimEndPos)*vertVec);
	double maxDist=max(startDist,endDist);
#ifdef  __DRAWING_CONTEXT_
	double upDist=max(maxDist,text_height+dimStyle.style.Gap);
#else
	double upDist=max(maxDist,text_height+0.25*text_height);
#endif
	if(verifyVec*vertVec>0)
	{	//��ע�߻����ڱ�ע���ϲ�
		if(iTextDimSylte==1)
		{	//�����ڱ�ע���²�
			rgnVertArr[0]=startPerp-vertVec*upDist+origin;
			rgnVertArr[1]=endPerp-vertVec*upDist+origin;
			rgnVertArr[2]=endPerp+vertVec*text_height*0.5+origin;
			rgnVertArr[3]=startPerp+vertVec*text_height*0.5+origin;
		}
		else
		{	//�����ڱ�ע���ϲ�
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
	{	//��ע�߻����ڱ�ע���²�
		if(iTextDimSylte==1)
		{	//�����ڱ�ע���²�
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
		{	//�����ڱ�ע���ϲ�
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
	double half_dim_width=0.5*GetWidth();	//��ע�����ȵ�һ��
	double dim_height=GetHeight();			//��ע����߶�
	double up_height=0.5+dimStyle.textHeight;//�ϲ�߶�(����߶����ע�������ע��֮��ļ�϶ֵ֮��)
#ifdef __DRAWING_CONTEXT_
	up_height=dimStyle.style.Gap+dimStyle.textHeight;//�ϲ�߶�(����߶����ע�������ע��֮��ļ�϶ֵ֮��)
#endif
	double down_height=dim_height-up_height;//�²�߶�

	f3dPoint horiVec=GetRapidVec(angle);
	f3dPoint vertVec(-horiVec.y,horiVec.x);
	f3dPoint verifyVec=dimPos-dimStartPos;
	double verify=verifyVec*vertVec;
	if(verify>0)	//��ע���ڱ�ע��ʼ���϶�,
	{
		if(iTextDimSylte==1)		//��ע�����ڱ�ע���²�
		{
#ifdef __DRAWING_CONTEXT_
			dimPos=dimStartPos+vertVec*(1.5*dimStyle.textHeight+dimStyle.style.Gap)+horiVec;
#else
			dimPos=dimStartPos+vertVec*(1.5*dimStyle.textHeight+0.5)+horiVec;
#endif
		}
		else if(iTextDimSylte==0)	//��ע�����ڱ�ע���ϲ�
			dimPos=dimStartPos+vertVec*0.5*dimStyle.textHeight+horiVec;
	}
	else		//��ע���ڱ�ע��ʼ���¶�,
	{
		if(iTextDimSylte==1)	//��ע�����ڱ�ע���²�
			dimPos=dimStartPos-vertVec*0.5*dimStyle.textHeight-horiVec;
		else if(iTextDimSylte==0)	//��ע�����ڱ�ע���ϲ�
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
//TODO:ΪʲôҪ����ResetPosition2δע��ԭ�����ResetPosition������� wjh-2017.1.11
//���¼����עλ�ã�����Ҫ������
void CSizeTextDim::ResetPosition2(DIMALIGN_TYPE align/*=AlignDefault*/)
{
	dimPos=(dimStartPos+dimEndPos)*0.5;
	if(align==AlignDefault)
	{
		if(iTextDimSylte==1)	//��ע�����ڱ�ע���²�
			alignType=TopCenter;
		else					//��ע�����ڱ�ע���ϲ�
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
//���ر�ע������(������ע��ͷ)����rgnVertArr[0];����rgnVertArr[1];����rgnVertArr[2];�����ĸ��ǵ�rgnVertArr[3]
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
	rgnVertArr[0].Set(scope.fMinX,scope.fMinY);	//����
	rgnVertArr[1].Set(scope.fMaxX,scope.fMinY);	//����
	rgnVertArr[2].Set(scope.fMaxX,scope.fMaxY);	//����
	rgnVertArr[3].Set(scope.fMinX,scope.fMaxY);	//����
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
#else	//TODO:��ʱ����, �Ƚ���LDS��Ŀ�µ�JgDrawing.cpp��TowerCad�е���ȫ����ʱӦͳһΪ������� wjh-2015.2.28
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
	dataPos.Set();	//����λ��
	dimPos.Set();	//��עλ��
	hPart=0;		//�����������
	cls_id=0;		//������������
	iCurLayer=0;	//�ı�����ͼ��
	textAlignHori=ACDB::kTextCenter;
	textAlignVert=ACDB::kTextVertMid;
	dwKeyTag=0;
	cFlag=0;
}

//��ʼ��
void CTextFrame::InitTextFrame(double text_height/*=2.5*/,double frame_width/*=0*/,
	int frame_style/*=-1*/,double space/*=0.3*/,double w_to_h/*=0.7*/)
{
	textHeight=text_height;
	frameHeight=frameWidth=frame_width;
	frameStyle=frame_style;
	margin=max(space,0.5);	//������Բ��С��϶ֵΪ0.5mm�Ƚ�ǡ����̫С�����±߻����ֲַ��� wjh 2016-10-18
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
//���ܽ����߿� frameStyle:-1.�Զ��ж�;0.ԲȦ;1.��Բ���ο�;2.��ͨ���ο�
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
	double len=dimText.GetLength()*textHeight*WtoH;	//ʵ����Ҫ���ı�����
	if(frame_width>0)	//ָ���߿���
		frameHeight=frameWidth=frame_width;
	//�߿���С�ڵ���0ʱ�Զ�����߿���
	if(int(frameWidth)<=0)
		frameWidth=len;
	if(frame_style<0||frame_style>2)
	{	//�Զ��ж��ı��߿������
		if(len>frameWidth-margin*2)
		{
			frameStyle=1;	//��Բ���α߿�
			frameWidth=len+margin*2;
		}
		else
			frameStyle=0;	//ԲȦ�߿�
	}
	else //ָ���߿�����
		frameStyle=frame_style;

	//���ݱ߿��ȼ���߿�߶�
	if(frameStyle==0)	//ԲȦ�ͱ߿�
		frameHeight=frameWidth;
	else
	{	//ָ���߿�����Ϊ���ο�ʱ�Զ������߿���
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
	double fHalfWidth=0.5*frameWidth*draw_scale;	//����������������߿� wht 10-09-16
	double fHalfHeight=0.5*frameHeight*draw_scale;
	//���ο�
	if(frameStyle==1)	//0.ԲȦ;1.��Բ���ο�;2.��ͨ���ο�
	{	//���ݻ��ƾ�����Բ���͵ľ��ο�Χ����ͨ���ο�Ҫ��һЩ������ wjh-2017.1.11
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
//������������ı������Ӷ˵Ķ˵�λ��
f3dPoint CTextFrame::GetRefLineEndPos(double draw_scale/*=1.0*/)
{
	f3dPoint refVec=dimPos-dataPos;
	normalize(refVec);
	if(refVec.IsZero())
		return dataPos;
	double fram_width=frameWidth*draw_scale;	//���ݱ����������
	double fram_height=frameHeight*draw_scale;	//���ݱ��������߶�
	if(frameStyle==0)	//ԲȦ�ͱ߿�
		return dimPos-refVec*(fram_width*0.5);
	else	//���λ���Բ���ο�
	{
		if(fabs(refVec.x)>EPS&&fabs(refVec.y/refVec.x)<fram_height/fram_width)
		{	//�������ն����ӵ����ı������Ҳ�
			if(refVec.x<0)	//�������ն����ı����Ҳ�
				return dataPos+refVec*((dimPos.x+0.5*fram_width-dataPos.x)*(1/refVec.x));
			else			//�������ն����ı������
				return dataPos+refVec*((dimPos.x-0.5*fram_width-dataPos.x)*(1/refVec.x));
		}
		else
		{	//�������ն����ı������²�
			if(refVec.y<0)	//�������ն����ı����ϲ�
				return dataPos+refVec*((dimPos.y+0.5*fram_height-dataPos.y)*(1/refVec.y));
			else			//�������ն����ı����²�
				return dataPos+refVec*((dimPos.y-0.5*fram_height-dataPos.y)*(1/refVec.y));
		}
	}
}
//��������λ��
void CTextFrame::UpdateDataPos(CHashList<DATAPOINT_TAG>& xDataPointTagHash)
{
	DATAPOINT_TAG* pTag=xDataPointTagHash.GetValue(dwKeyTag);
	if(pTag==NULL||pTag->vertexArr.GetSize()<=0)
		return;
	f3dPoint* pPointPre=pTag->vertexArr.GetFirst();
	f3dPoint* pPointFir=pPointPre;
	f3dLine minLine(dimPos,*pPointPre),line;//�����������̵��߶�
	f3dPoint pedalPoint;
	double fDist=0,minLengthSquare=(minLine.startPt.x-minLine.endPt.x)*(minLine.startPt.x-minLine.endPt.x)+(minLine.startPt.y-minLine.endPt.y)*(minLine.startPt.y-minLine.endPt.y);
	for(f3dPoint* pPointCur=pTag->vertexArr.GetNext();pPointCur!=NULL;pPointCur=pTag->vertexArr.GetNext())
	{
		//dim�붥��Ľ�С����
		double lengthSquare=(dimPos.x-pPointCur->x)*(dimPos.x-pPointCur->x)+(dimPos.y-pPointCur->y)*(dimPos.y-pPointCur->y);
		if(lengthSquare<minLengthSquare)
		{
			minLine.endPt=*pPointCur;
			minLengthSquare=lengthSquare;
		}
		//dimpos��ߵĴ����Ƿ���ֱ����
		line.startPt=*pPointPre;
		line.endPt=*pPointCur;
		if(line.startPt.IsEqual(line.endPt))
		{
			//TODO:���غ��������
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
