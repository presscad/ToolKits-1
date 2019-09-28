// DxfFile.cpp: implementation of the CDxfFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DxfFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDxfFile::CDxfFile()
{
	handle=0x20;
	m_fp = NULL;
}

CDxfFile::~CDxfFile()
{
	extmin.Set(-1000, -1000);
	extmax.Set(1000, 1000);
}

bool CDxfFile::OpenFile(const char *file_name)
{
	m_fp=fopen(file_name,"wt");
	if(m_fp==NULL)
		return false;
	InitFileHeader();
	return true;
}

void CDxfFile::InitFileHeader()
{
	handle=0x20;
    fprintf(m_fp,"  0\nSECTION\n  2\nHEADER\n");
	fprintf(m_fp,"9\n$EXTMIN\n10\n%lf\n20\n%lf\n30\n%lf\n",extmin.x,extmin.y,extmin.z);
	fprintf(m_fp,"9\n$EXTMAX\n10\n%lf\n20\n%lf\n30\n%lf\n",extmax.x,extmax.y,extmax.z);
	fprintf(m_fp, " 9\n$TEXTSIZE\n 40\n2.5\n");
	fprintf(m_fp, " 9\n$TEXTSTYLE\n 7\nstandard\n");
    fprintf(m_fp,"  0\nENDSEC\n");
    fprintf(m_fp,"  0\nSECTION\n  2\nTABLES\n");
	fprintf(m_fp,"0\nTABLE\n2\nVPORT\n70\n3\n0\nVPORT\n2\n*ACTIVE\n70\n0\n10\n0.0\n20\n0.0\n11\n1.0\n21\n1.0\n");
	fprintf(m_fp,"12\n%lf\n22\n%lf\n13\n0.0\n23\n0.0\n14\n1.0\n24\n1.0\n 15\n0.0\n25\n0.0\n",
		(extmin.x+extmax.x)/2,(extmin.y+extmax.y)/2);
	fprintf(m_fp,"16\n0.0\n26\n0.0\n36\n1.0\n17\n0.0\n27\n0.0\n37\n0.0\n40\n%lf\n41\n%lf\n42\n50\n43\n0.0\n44\n0.0\n",
		fabs(extmax.y-extmin.y)*2,1.2);
	fprintf(m_fp,"50\n0.0\n51\n0.0\n71\n0.0\n72\n100\n73\n0.0\n74\n3\n75\n0\n76\n0\n77\n0\n78\n0\n0\nENDTAB\n");
	//字样表
	fprintf(m_fp, " 0\nTABLE\n2\nSTYLE\n 5\n3\n70\n1\n");
	fprintf(m_fp, " 0\nSTYLE\n 5\n11\n 2\nstandard\n 70\n0\n 40\n0.0\n 41\n0.7\n 50\n0.0\n 71\n0\n 42\n2.5\n 3\nsimplex\n 4\ngbhzfs.shx\n");
	fprintf(m_fp, " 0\nENDTAB\n");
	fprintf(m_fp,"  0\nENDSEC\n");
    fprintf(m_fp,"  0\nSECTION\n  2\nENTITIES\n");
}

void CDxfFile::CloseFile()
{
	if(m_fp)
	{
	    fprintf(m_fp,"  0\nENDSEC\n  0\nEOF\n");
		fclose(m_fp);
	}
}

void CDxfFile::NewLine(f3dPoint start, f3dPoint end, int clrIndex/*=-1*/)
{
	fprintf(m_fp,"  0\nLINE\n  5\n%x\n  8\n0\n",handle);
	if (clrIndex >= 0 && clrIndex <= 255)
		fprintf(m_fp, "  62\n %d\n", clrIndex);
	//为了纠正Y轴方向与ACAD一至(自下至上)Y坐标值全部加了'-'号
	fprintf(m_fp," 10\n%.14f\n 20\n%.14f\n 30\n0.0\n",start.x,start.y);
	fprintf(m_fp," 11\n%.14f\n 21\n%.14f\n 31\n0.0\n",end.x,end.y);
	handle++;
}

void CDxfFile::New3dLine(f3dPoint ptS, f3dPoint ptE, int clrIndex/*=-1*/)
{
	fprintf(m_fp,"  0\nLINE\n  5\n%x\n  8\n0\n",handle);
	if (clrIndex >= 0 && clrIndex <= 255)
		fprintf(m_fp, "  62\n %d\n", clrIndex);
	fprintf(m_fp," 10\n%.14f\n 20\n%.14f\n 30\n%.14f\n",ptS.x,ptS.y,ptS.z);
	fprintf(m_fp," 11\n%.14f\n 21\n%.14f\n 31\n%.14f\n",ptE.x,ptE.y,ptE.z);
	handle++;
}

void CDxfFile::NewCircle(f3dPoint centre, double radius)
{
	fprintf(m_fp,"  0\nCIRCLE\n  5\n%x\n  8\n0\n",handle);
	fprintf(m_fp," 10\n%f\n 20\n%f\n 30\n0.0\n",centre.x,centre.y);
	fprintf(m_fp," 40\n%f\n",radius);
	handle++;
}

void CDxfFile::NewArc(double xo, double yo, double radius, double start_ang, double end_ang)
{
	fprintf(m_fp,"  0\nARC\n  5\n%x\n  8\n0\n",handle);
	fprintf(m_fp," 10\n%.14f\n 20\n%.14f\n 30\n0.0\n",xo,yo);
	fprintf(m_fp," 40\n%f\n 50\n%.14f\n 51\n%.14f\n",radius,start_ang,end_ang);
	handle++;
}

void CDxfFile::NewText(const char* sText, f3dPoint pt, double fTextH, double fRotAnge/*=0*/,int clrIndex/*=1*/)
{
	fprintf(m_fp, "  0\nTEXT\n  5\n%x\n  8\n0\n", handle);
	if(clrIndex>=0&&clrIndex<=255)
		fprintf(m_fp, "  62\n %d\n", clrIndex);
	fprintf(m_fp, " 10\n%.14f\n 20\n%.14f\n 30\n0.0\n", pt.x, pt.y);
	fprintf(m_fp, " 40\n%f\n 1\n%s\n 50\n%.14f\n 41\n0.7\n", fTextH, sText, fRotAnge);
	fprintf(m_fp, " 11\n%.14f\n 21\n%.14f\n 31\n0.0\n 73\n1\n ", pt.x, pt.y);
	handle++;
}

void CDxfFile::NewEllipse(f3dPoint &centre,f3dPoint &long_axis_end_pt,double scale_short_2_long,double start_angle,double end_angle)
{
	fprintf(m_fp,"  0\nELLIPSE\n  5\nF1\n300\n1F\n100 AcDbEntity\n  8\n0\n100\nAcDbEllipse\n");	//椭圆基本信息
	fprintf(m_fp," 10\n%.14f\n 20\n%.14f\n 30\n0.0\n",centre.x,centre.y);	//圆心
	fprintf(m_fp," 11\n%.14f\n 22\n%.14f\n 31\n0.0\n",long_axis_end_pt.x,long_axis_end_pt.y);	//长轴端点（相对于圆心的相对值）
	fprintf(m_fp," 210\n0.0\n 220\n0.0\n 230\n1.0\n");	//拉伸方向默认（0，0，1）
	fprintf(m_fp," 40\n%.14f\n 41\n%.14f\n 42\n%.14f\n",scale_short_2_long,start_angle,end_angle);	//长轴与短轴的比例、起始角、终止角
}