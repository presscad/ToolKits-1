// DxfFile.h: interface for the CDxfFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXFFILE_H__D4A1B4C4_6C10_4F24_B932_56AB5CB66C21__INCLUDED_)
#define AFX_DXFFILE_H__D4A1B4C4_6C10_4F24_B932_56AB5CB66C21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "f_ent.h"
class CDxfFile  
{
	FILE* m_fp;
public:
	void NewArc(double xo,double yo,double radi,double start_ang,double end_ang);
	void NewCircle(f3dPoint centre, double radius);
	void NewLine(f3dPoint start, f3dPoint end, int clrIndex=-1);
	void New3dLine(f3dPoint ptS, f3dPoint ptE, int clrIndex=-1);
	void NewText(const char*sText, f3dPoint pt, double fTextH, double fRotAnge = 0, int clrIndex=1);
	void NewEllipse(f3dPoint &centre,f3dPoint &long_axis_end_pt,double scale_short_2_long,double start_angle,double end_angle);
	void CloseFile();
	bool OpenFile(const char *file_name);
	f3dPoint extmin;
	f3dPoint extmax;
	CDxfFile();
	virtual ~CDxfFile();

private:
	long handle;
	void InitFileHeader();
};

#endif // !defined(AFX_DXFFILE_H__D4A1B4C4_6C10_4F24_B932_56AB5CB66C21__INCLUDED_)
