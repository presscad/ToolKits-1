// XhRptTbl.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <objbase.h>
#include "XhRptTbl.h"
#include "HashTable.h"
#include "ExcelOper.h"
/////////////////////////////////////////////////////////////////////////////////
//XHVAR
XHVAR::XHVAR()
{
	memset(this,0,sizeof(XHVAR));
	ciType = XHTBLDEF::GRID_VT_EMPTY;		//未指定数据类型
}
XHVAR::XHVAR(const XHVAR& srcObj)
{
	*this = srcObj;
	if (ciType==XHTBLDEF::GRID_VT_VARSTR&&sVal)
	{
		sVal=new char[strlen(srcObj.sVal)+1];
		strcpy(sVal,srcObj.sVal);
	}
}

XHVAR::~XHVAR()
{
	ClearVariant();
}
void XHVAR::ClearVariant()	//清空异构变量所控内存并清零存储区
{
	BYTE old_type = ciType;
	if(ciType==XHTBLDEF::GRID_VT_VARSTR&&sVal)
		free(sVal);		//清空字符串空间
	memset(this,0,sizeof(XHVAR));	//清零存储区
	ciType = old_type;	//恢得原数据类型
}

bool XHVAR::SetGridString(const char *string, BOOL bForceToString/*=TRUE*/)
{
	if(ciType!=XHTBLDEF::GRID_VT_VARSTR&&!bForceToString||string==NULL)
		return false;	//类型不匹配
	else if(ciType!=XHTBLDEF::GRID_VT_VARSTR)
	{
		ClearVariant();
		ciType = XHTBLDEF::GRID_VT_VARSTR;
		sVal = (char*)realloc(sVal,strlen(string)+2);
		strcpy(sVal,string);
		return true;
	}
	else
	{
		sVal = (char*)realloc(sVal,strlen(string)+2);
		strcpy(sVal,string);
		return TRUE;
	}
}
void XHVAR::Export(FILE *fp)
{
	CXhChar200 sLineText;
	//COleDateTime datetime;
	fprintf(fp,"DataType=%d\n",ciType);
	switch(ciType)
	{
	case XHTBLDEF::GRID_VT_EMPTY	:	// 未指定
	case XHTBLDEF::GRID_VT_NULL	:	// 空（暂保留）
		break;
	case XHTBLDEF::GRID_VT_I2		:	// 2字节带符号整数
		sLineText.Printf("%d",iVal);
		break;
	case XHTBLDEF::GRID_VT_I4		:	// 4字节带符号整数
		sLineText.Printf("%ld",lVal);
		break;
	case XHTBLDEF::GRID_VT_R4		:	// 4字节浮点数
		sLineText.Printf("%f",fltVal);
		break;
	case XHTBLDEF::GRID_VT_R8		:	// 8字节浮点数
		sLineText.Printf("%lf",dblVal);
		break;
	//case GRID_VT_DATE	:	// 日期
	//	datetime = date;
	//	sLineText.Format("%d/%d/%d",datetime.GetYear(),datetime.GetMonth(),datetime.GetDay());
	//	break;
	case XHTBLDEF::GRID_VT_VARSTR:	// 字符串
		if(sVal==NULL)
			sLineText.Empty();
		else
			sLineText.Printf("%s",sVal);
		break;
	case XHTBLDEF::GRID_VT_POINTER:	// 指针
	case XHTBLDEF::GRID_VT_BOOL	:	// 布尔数; TRUE=1, FALSE=0
		if(bVal)
			sLineText = "1";
		else
			sLineText = "0";
		break;
	default:
		break;
	}
	sLineText.Replace("\r","`");
	sLineText.Replace("\n","`");
	fprintf(fp,"DataValue=%s\n",(char*)sLineText);
	fprintf(fp,"--GridDataEnd\n\n");
}

void XHVAR::Import(FILE *fp)
{
	//COleDateTime datetime;
	int nYear=2000,nMonth=1,nDay=1;
	char sLineText[500],*sToken=NULL,separator[]=" /=,";
	while(feof(fp)==0)
	{
		fgets(sLineText,500,fp);
		CXhChar500 ss = sLineText;
		ss.Replace("`","\n");
		ss.Replace("``","\r\n");
		sprintf(sLineText,"%s",(char*)ss);

		sToken = strtok(sLineText,separator);
		if(sToken&&stricmp(sToken,"DataType")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
			{
				ciType = (BYTE)atoi(sToken);
				sToken = strtok(NULL,separator);
			}
		}
		else if(sToken&&stricmp(sToken,"DataValue")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken==NULL)
				continue;
			CXhChar500 sValue(sToken);
			sValue.Replace("\n", "");
			switch(ciType)
			{
			case XHTBLDEF::GRID_VT_EMPTY	:	// 未指定
			case XHTBLDEF::GRID_VT_NULL	:	// 空（暂保留）
				break;
			case XHTBLDEF::GRID_VT_I2		:	// 2字节带符号整数
				iVal = atoi(sValue);
				break;
			case XHTBLDEF::GRID_VT_I4		:	// 4字节带符号整数
				lVal = atoi(sValue);
				break;
			case XHTBLDEF::GRID_VT_R4		:	// 4字节浮点数
				fltVal = (float)atof(sValue);
				break;
			case XHTBLDEF::GRID_VT_R8		:	// 8字节浮点数
				dblVal = atof(sValue);
				break;
			/*case XHTBLDEF::GRID_VT_DATE	:	// 日期
				nYear = atoi(sToken);
				sToken = strtok(NULL,separator);
				if(sToken)
				{
					nMonth = atoi(sToken);
					sToken = strtok(NULL,separator);
					if(sToken)
						nDay = atoi(sToken);
				}
				datetime.SetDate(nYear,nMonth,nDay);
				date = datetime;
				break;*/
			case XHTBLDEF::GRID_VT_VARSTR:	// 字符串
				SetGridString(sValue);
				break;
			case XHTBLDEF::GRID_VT_POINTER:	// 指针
			case XHTBLDEF::GRID_VT_BOOL	:	// 布尔数; TRUE=1, FALSE=0
				bVal = atoi(sValue);
				break;
			default:
				break;
			}
		}
		else if(sToken&&strstr(sToken,"GridDataEnd")!=NULL)
			break;
	}
}
/////////////////////////////////////////////////////////////////////////////////
CXhGrid::FONTSTYLE::operator LOGFONT()const
{
	LOGFONT logfont;
	CopyToLOGFONT(logfont);
	return logfont;
}

CXhGrid::FONTSTYLE::FONTSTYLE()
{
	lfHeight		=-13;
	lfWidth			= 0; 
	lfEscapement	= 0; 
	lfOrientation	= 0; 
	lfWeight		= 400; 
	lfItalic		= 0; 
	lfUnderline		= 0; 
	lfStrikeOut		= 0; 
	lfCharSet		= 134;	//CHARSET_GB132
	lfOutPrecision	= 3; 
	lfClipPrecision	= 2; 
	lfQuality		= 1; 
	lfPitchAndFamily= 2;
	lfFaceName = NULL;
	crTextColor = 0;		//文本颜色为黑色
	crBackground = RGB(255, 255, 255);
	SetFaceName(_T("宋体"));
	m_nTextAlignFlags = XHTBLDEF::TEXTALIGN_MIDDLE_CENTER;
	//m_nTextAlignFlags = DT_CENTER|DT_VCENTER|DT_SINGLELINE;	//文字对齐方式
	m_fLeftDist=m_fRightDist=1;		//左右内边距----mm为单位
	m_fTopDist=m_fBottomDist=0.5;	//上下内边距----mm为单位
}
CXhGrid::FONTSTYLE::FONTSTYLE(const CXhGrid::FONTSTYLE& fontstyle)
{
	CoTaskMemFree(lfFaceName);	//清除以前的字样名称空间
	memcpy(this,&fontstyle,sizeof(FONTSTYLE));
	lfFaceName=NULL;	//赋为空指针
	SetFaceName(fontstyle.lfFaceName);
}
CXhGrid::FONTSTYLE::FONTSTYLE(const LOGFONT& fontstyle)
{
	CopyFromLOGFONT(fontstyle);
}
CXhGrid::FONTSTYLE::~FONTSTYLE()
{
	if(lfFaceName)
		CoTaskMemFree(lfFaceName);
}
int CXhGrid::FONTSTYLE::SetFaceName(const TCHAR *face_name)
{
	if(face_name==NULL)
		return 0;
	int n = (int)_tcslen(face_name);
	if(lfFaceName)
	{
		if(32>=n)
		{
			for(int i=0;i<32;i++)
				lfFaceName[i] = face_name[i];
		}
		else
		{
			CoTaskMemFree(lfFaceName);
			lfFaceName = (TCHAR*)CoTaskMemAlloc(sizeof(TCHAR)*n);
			for(int i=0;i<32;i++)
				lfFaceName[i] = face_name[i];
		}
	}
	else
	{
		lfFaceName = (TCHAR*)CoTaskMemAlloc(sizeof(TCHAR)*128);	//最小是64，否则跳错
		for(int i=0;i<32;i++)
			lfFaceName[i] = face_name[i];
	}
	return n;
}
void CXhGrid::FONTSTYLE::CopyFromLOGFONT(const LOGFONT &logfont)
{
  lfHeight			=logfont.lfHeight;
  lfWidth			=logfont.lfWidth;
  lfEscapement		=logfont.lfEscapement;
  lfOrientation		=logfont.lfOrientation;
  lfWeight			=logfont.lfWeight;
  lfItalic			=logfont.lfItalic;
  lfUnderline		=logfont.lfUnderline;
  lfStrikeOut		=logfont.lfStrikeOut;
  lfCharSet			=logfont.lfCharSet;
  lfOutPrecision	=logfont.lfOutPrecision;
  lfClipPrecision	=logfont.lfClipPrecision;
  lfQuality			=logfont.lfQuality;
  lfPitchAndFamily	=logfont.lfPitchAndFamily;
  SetFaceName(logfont.lfFaceName); 
}

void CXhGrid::FONTSTYLE::CopyToLOGFONT(LOGFONT &logfont)const
{
	logfont.lfHeight		=lfHeight;
	logfont.lfWidth			=lfWidth;
	logfont.lfEscapement	=lfEscapement;
	logfont.lfOrientation	=lfOrientation;
	logfont.lfWeight		=lfWeight;
	logfont.lfItalic		=lfItalic;
	logfont.lfUnderline		=lfUnderline;
	logfont.lfStrikeOut		=lfStrikeOut;
	logfont.lfCharSet		=lfCharSet;
	logfont.lfOutPrecision	=lfOutPrecision;
	logfont.lfClipPrecision	=lfClipPrecision;
	logfont.lfQuality		=lfQuality;
	logfont.lfPitchAndFamily=lfPitchAndFamily;
	if(lfFaceName==NULL)
		strcpy(logfont.lfFaceName,"");
	else
		strcpy(logfont.lfFaceName, lfFaceName); 
}
/*BOOL CXhGrid::FONTSTYLE::operator ==(const FONTSTYLE& fontstyle)const
{
	if(*this!=fontstyle)
		return FALSE;
	else
		return TRUE;
}
BOOL CXhGrid::FONTSTYLE::operator !=(const FONTSTYLE& fontstyle)const
{
	if(lfHeight!=fontstyle.lfHeight)
		return TRUE;
	else if(lfWidth!=fontstyle.lfWidth)
		return TRUE;
	else if(lfEscapement!=fontstyle.lfEscapement)
		return TRUE;
	else if(lfOrientation!=fontstyle.lfOrientation)
		return TRUE;
	else if(lfWeight!=fontstyle.lfWeight)
		return TRUE;
	else if(lfItalic!=fontstyle.lfItalic)
		return TRUE;
	else if(lfUnderline!=fontstyle.lfUnderline)
		return TRUE;
	else if(lfStrikeOut!=fontstyle.lfStrikeOut)
		return TRUE;
	else if(lfCharSet!=fontstyle.lfCharSet)
		return TRUE;
	else if(lfOutPrecision!=fontstyle.lfOutPrecision)
		return TRUE;
	else if(lfClipPrecision!=fontstyle.lfClipPrecision)
		return TRUE;
	else if(lfQuality!=fontstyle.lfQuality)
		return TRUE;
	else if(lfPitchAndFamily!=fontstyle.lfPitchAndFamily)
		return TRUE;
	else if(crTextColor!=fontstyle.crTextColor)
		return TRUE;
	else if(lfFaceName&&fontstyle.lfFaceName)
	{
		if(_tcsicmp(lfFaceName,fontstyle.lfFaceName)!=0)
			return TRUE;
		else
			return FALSE;
	}
	else if((lfFaceName!=NULL&&_tcslen(lfFaceName)>0)||(fontstyle.lfFaceName!=NULL&&_tcslen(fontstyle.lfFaceName)>0))
		return TRUE;
	else
		return FALSE;
}*/

void CXhGrid::FONTSTYLE::Import(FILE* fp)
{
	char sLineText[200],*sToken=NULL,separator[]=" =,\r\n";
	while(feof(fp)==0)
	{
		fgets(sLineText,200,fp);
		sToken = strtok(sLineText,separator);
		if(sToken&&stricmp(sToken,"TextColor")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				crTextColor = atoi(sToken);
		}
		else if(sToken&&stricmp(sToken,"CharSet")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				lfCharSet = atoi(sToken);
		}
		else if(sToken&&stricmp(sToken,"ClipPrecision")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				lfClipPrecision = atoi(sToken);
		}
		else if(sToken&&stricmp(sToken,"OutPrecision")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				lfOutPrecision = atoi(sToken);
		}
		else if(sToken&&stricmp(sToken,"Escapement")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				lfEscapement = atoi(sToken);
		}
		else if(sToken&&stricmp(sToken,"Orientation")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				lfOrientation = atoi(sToken);
		}
		else if(sToken&&stricmp(sToken,"FaceName")==0)
		{
			size_t n=32;
			sToken = strtok(NULL,separator);
			n = min(32,strlen(sToken));
			if(sToken)
				SetFaceName(sToken);
		}
		else if(sToken&&stricmp(sToken,"Height")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				lfHeight = atoi(sToken);
		}
		else if(sToken&&stricmp(sToken,"Width")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				lfWidth = atoi(sToken);
		fprintf(fp,"Width=%d\n",lfWidth);
		}
		else if(sToken&&stricmp(sToken,"Weight")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				lfWeight = atoi(sToken);
		}
		else if(sToken&&stricmp(sToken,"Italic")==0)
		{
			int key;
			sToken = strtok(NULL,separator);
			if(sToken)
				key = atoi(sToken);
			if(key)
				lfItalic = 1;
			else
				lfItalic = 0;
		}
		else if(sToken&&stricmp(sToken,"StrikeOut")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				lfCharSet = atoi(sToken);
		if(lfStrikeOut)
			fprintf(fp,"StrikeOut=1\n");
		else
			fprintf(fp,"StrikeOut=0\n");
		}
		else if(sToken&&stricmp(sToken,"Underline")==0)
		{
			int key;
			sToken = strtok(NULL,separator);
			if(sToken)
				key = atoi(sToken);
			if(key)
				lfUnderline=1;
			else
				lfUnderline=0;
		}
		else if(sToken&&stricmp(sToken,"LeftDist")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				m_fLeftDist = (float)atof(sToken);
		}
		else if(sToken&&stricmp(sToken,"TopDist")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				m_fTopDist = (float)atof(sToken);
		fprintf(fp,"TopDist=%.f\n",m_fTopDist);
		}
		else if(sToken&&stricmp(sToken,"RightDist")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				m_fRightDist = (float)atof(sToken);
		}
		else if(sToken&&stricmp(sToken,"BottomDist")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				m_fBottomDist = (float)atof(sToken);
		}
		else if(sToken&&stricmp(sToken,"TextAlignFlag")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				m_nTextAlignFlags = atoi(sToken);
		}
		else if(sToken&&strstr(sToken,"FontStyleEnd")==0)
			break;
	}
}

void CXhGrid::FONTSTYLE::Export(FILE* fp)
{
	fprintf(fp,"TextColor=%ld\n",crTextColor);
	fprintf(fp,"CharSet=%d\n",lfCharSet);
	fprintf(fp,"ClipPrecision=%d\n",lfClipPrecision);
	fprintf(fp,"OutPrecision=%d\n",lfOutPrecision);
	fprintf(fp,"Escapement=%d\n",lfEscapement);
	fprintf(fp,"Orientation=%d\n",lfOrientation);
	fprintf(fp,"FaceName=%s\n",lfFaceName);
	fprintf(fp,"Height=%d\n",lfHeight);
	fprintf(fp,"Width=%d\n",lfWidth);
	fprintf(fp,"Weight=%d\n",lfWeight);
	if(lfItalic)
		fprintf(fp,"Italic=1\n");
	else
		fprintf(fp,"Italic=0\n");
	if(lfStrikeOut)
		fprintf(fp,"StrikeOut=1\n");
	else
		fprintf(fp,"StrikeOut=0\n");
	if(lfUnderline)
		fprintf(fp,"Underline=1\n");
	else
		fprintf(fp,"Underline=0\n");
	fprintf(fp,"LeftDist=%.f\n",m_fLeftDist);
	fprintf(fp,"TopDist=%.f\n",m_fTopDist);
	fprintf(fp,"RightDist=%.f\n",m_fRightDist);
	fprintf(fp,"BottomDist=%.f\n",m_fBottomDist);
	fprintf(fp,"TextAlignFlag=%d\n",m_nTextAlignFlags);
	fprintf(fp,"--FontStyleEnd\n\n");
}
/////////////////////////////////////////////////////////////////////////////////
float CXhGrid::PREFER_TEXT_SIZE = 2.0f;
CXhGrid::CXhGrid()
{
	grid_mode = XHTBLDEF::STRING_GRID;
	uiTagFeature = iRow = iColumn = 0;
	m_bVGridVirtual = m_bHGridVirtual = FALSE;
	m_bSelectStatus = false;
	data.ciType = XHTBLDEF::GRID_VT_VARSTR;
	//data.SetGridString("我是好人!");
	mark.bTitle = FALSE;
	mark.ext_feature = mark.type_id = 0;
	simfont.bBold = false;
	simfont.sfTextSize = PREFER_TEXT_SIZE;//字高sys.dim_map.fNumericTextSize;
	simfont.sfTextAngle = 0;
	uidCellValSource=0;
}

CXhGrid::~CXhGrid()
{
}
CXhGrid::CXhGrid(const CXhGrid& grid)
{
	Copy(grid);
}
bool CXhGrid::get_blStateSelected()
{
	return this->m_bSelectStatus;
}
bool CXhGrid::set_blStateSelected(bool blSelected)
{
	return m_bSelectStatus=blSelected;
}

void CXhGrid::Copy(const CXhGrid &grid)
{
	m_bHGridVirtual = grid.m_bHGridVirtual;
	m_bVGridVirtual = grid.m_bVGridVirtual;
	data = grid.data;
	uiTagFeature = grid.uiTagFeature;
	grid_mode = grid.grid_mode;
	ciFontFormat=grid.ciFontFormat;
	xFontStyle = grid.xFontStyle;
	simfont=grid.simfont;
	iRow = grid.iRow;
	iColumn = grid.iColumn;
}

void CXhGrid::Export(FILE *fp)
{
	fprintf(fp,"GridMode=%d\n",grid_mode);
	fprintf(fp,"HorizonVirtual=%d\n",m_bHGridVirtual);
	fprintf(fp,"VerticalVirtual=%d\n",m_bVGridVirtual);
	fprintf(fp,"Feature=%ld\n",uiTagFeature);
	fprintf(fp,"\n--FontStyleStart\n");
	xFontStyle.Export(fp);
	fprintf(fp,"TextSize=%.1f\n",simfont.sfTextSize);
	fprintf(fp,"TextAngle=%.1f\n",simfont.sfTextAngle);
	fprintf(fp,"BoldText=%d\n", simfont.bBold);
	fprintf(fp,"\n--GridDataStart\n");
	data.Export(fp);
	if(mark.bTitle)
		fprintf(fp,"IsTitle=1\n");
	else
		fprintf(fp,"IsTitle=0\n");
	fprintf(fp,"TypeId=%ld\n",mark.type_id);
	fprintf(fp,"ExtFeature=%ld\n",mark.ext_feature);
	fprintf(fp,"--NewGridEnd\n");
}

void CXhGrid::Import(FILE *fp)
{
	char sLineText[200],*sToken=NULL,separator[]=" =,\r\n";
	while(feof(fp)==0)
	{
		fgets(sLineText,200,fp);
		sToken = strtok(sLineText,separator);
		if(sToken&&stricmp(sToken,"GridMode")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				grid_mode = (BYTE)atoi(sToken);
		}
		else if(sToken&&stricmp(sToken,"HorizonVirtual")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				m_bHGridVirtual = atoi(sToken)!=0;
		}
		else if(sToken&&stricmp(sToken,"VerticalVirtual")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				m_bVGridVirtual = atoi(sToken)!=0;
		}
		else if(sToken&&stricmp(sToken,"Feature")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				uiTagFeature = atoi(sToken);
		}
		else if(sToken&&stricmp(sToken,"--FontStyleStart")==0)
		{	//TODO:未完成xFontstyle与simfont间的协调
			xFontStyle.Import(fp);
			sToken = strtok(NULL,separator);
			if(sToken)
			{
				simfont.sfTextSize = (float)atof(sToken);
			}
		}
		else if(sToken&&stricmp(sToken,"--GridDataStart")==0)
		{
			data.Import(fp);
		}
		else if (sToken&&stricmp(sToken, "TextSize") == 0)
		{
			sToken = strtok(NULL, separator);
			if (sToken)
				simfont.sfTextSize = (short)atof(sToken);
		}
		else if (sToken&&stricmp(sToken, "TextAngle") == 0)
		{
			sToken = strtok(NULL, separator);
			if (sToken)
				simfont.sfTextAngle = (short)atof(sToken);
		}
		else if (sToken&&stricmp(sToken, "BoldText") == 0)
		{
			sToken = strtok(NULL, separator);
			if (sToken)
				simfont.bBold = atoi(sToken);
		}
		else if(sToken&&stricmp(sToken,"IsTitle")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				mark.bTitle = atoi(sToken)!=0;
		}
		else if(sToken&&stricmp(sToken,"TypeId")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				mark.type_id = atoi(sToken);
		}
		else if(sToken&&stricmp(sToken,"ExtFeature")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				mark.ext_feature = atoi(sToken);
			break;
		}
		else if(sToken&&stricmp(sToken,"--NewGridEnd")==0)
			break;
	}
}
bool CXhGrid::WriteGridToStream(BUFFER_IO *pIO)
{
	pIO->WriteByte(grid_mode);
	pIO->WriteBooleanThin(m_bHGridVirtual);
	pIO->WriteBooleanThin(m_bVGridVirtual);
	pIO->WriteDword(this->uiTagFeature);
	pIO->Write(&xFontStyle,sizeof(FONTSTYLE));
	pIO->Write(xFontStyle.lfFaceName,32);
	pIO->WriteFloat(simfont.sfTextSize);
	pIO->WriteFloat(simfont.sfTextAngle);
	pIO->WriteInteger(simfont.bBold);
	pIO->WriteByte(data.ciType);
	if(data.ciType!=XHTBLDEF::GRID_VT_VARSTR)
		pIO->Write(&data,sizeof(XHVAR));
	else
	{
		if(data.sVal)
			pIO->WriteString(data.sVal);
		else
			pIO->WriteByte(0);
	}
	pIO->WriteBooleanThin(mark.bTitle);
	pIO->WriteInteger(mark.type_id);
	pIO->WriteInteger(mark.ext_feature);
	return S_OK;
}

bool CXhGrid::ReadGridFromStream(BUFFER_IO *pIO)
{
	pIO->ReadByte(&grid_mode);
	pIO->ReadBooleanThin(&m_bHGridVirtual);
	pIO->ReadBooleanThin(&m_bVGridVirtual);
	pIO->ReadDword(&uiTagFeature);
	pIO->Read(&xFontStyle,sizeof(FONTSTYLE));
	xFontStyle.lfFaceName = NULL;
	char face_name[33]={'\0'};
	pIO->Read(face_name,32);
	xFontStyle.SetFaceName(face_name);
	pIO->ReadFloat(&simfont.sfTextSize);
	pIO->ReadFloat(&simfont.sfTextAngle);
	pIO->ReadInteger(&simfont.bBold);
	pIO->ReadByte(&data.ciType);
	if(data.ciType!=XHTBLDEF::GRID_VT_VARSTR)
		pIO->Read(&data,sizeof(XHVAR));
	else
	{
		CXhChar200 szCellText;
		pIO->ReadString(szCellText,201);
		data.SetGridString(szCellText);
	}
	pIO->ReadBooleanThin(&mark.bTitle);
	pIO->ReadInteger(&mark.type_id);
	pIO->ReadInteger(&mark.ext_feature);
	return true;
}
/////////////////////////////////////////////////////////////////////////////////
//CGridMap------------------单元格存储二维表类--------------
CXhGridMap::CXhGridMap()
{
	m_nRow = m_nColumn = 0;
	grid_map = NULL;	//清空单元格存储区指针
}
CXhGridMap::~CXhGridMap()
{
	delete []grid_map;	//清空单元格存储区所占内存
}

void CXhGridMap::Destroy()
{
	m_nRow = m_nColumn = 0;
	delete []grid_map;
	grid_map=NULL;
}
BOOL CXhGridMap::Create(int nRow,int nColumn)
{
	if(grid_map==NULL)
		grid_map = new CXhGrid[nRow*nColumn];
	else
		return FALSE;
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nColumn;j++)
		{
			grid_map[i*nColumn+j].iRow		= i;
			grid_map[i*nColumn+j].iColumn	= j;
		}
	}
	m_nRow = nRow;			//赋值行总数
	m_nColumn = nColumn;	//赋值列总数
	return TRUE;
}

BOOL CXhGridMap::InsertRow(int iRow)
{
	int i,j;
	if(iRow<0||iRow>=m_nRow)
		return FALSE;
	else
	{
		CXhGrid *new_map = new CXhGrid[m_nRow*m_nColumn+m_nColumn];
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				if(i<iRow)
					new_map[i*m_nColumn+j] = grid_map[i*m_nColumn+j];
				else
					new_map[(i+1)*m_nColumn+j] = grid_map[i*m_nColumn+j];
			}
		}
		delete []grid_map;
		m_nRow++;
		grid_map = new_map;
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				grid_map[i*m_nColumn+j].iRow	= i;
				grid_map[i*m_nColumn+j].iColumn	= j;
			}
		}
	}
	return TRUE;
}
BOOL CXhGridMap::DelRow(int iRow)
{
	int i,j;
	if(iRow<0||iRow>=m_nRow)
		return FALSE;
	else
	{
		CXhGrid *new_map = new CXhGrid[m_nRow*m_nColumn-m_nColumn];
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				if(i<iRow)
					new_map[i*m_nColumn+j] = grid_map[i*m_nColumn+j];
				else if(i==iRow)
					continue;
				else
					new_map[(i-1)*m_nColumn+j] = grid_map[i*m_nColumn+j];
			}
		}
		delete []grid_map;
		m_nRow--;
		grid_map = new_map;
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				grid_map[i*m_nColumn+j].iRow	= i;
				grid_map[i*m_nColumn+j].iColumn	= j;
			}
		}
	}
	return TRUE;
}
BOOL CXhGridMap::InsertColumn(int iColumn)
{
	int i,j;
	if(iColumn<0||iColumn>=m_nColumn)
		return FALSE;
	else
	{
		CXhGrid *new_map = new CXhGrid[m_nRow*(m_nColumn+1)];
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				if(j<iColumn)
					new_map[i*(m_nColumn+1)+j] = grid_map[i*m_nColumn+j];
				else if(j==iColumn)
					continue;
				else
					new_map[i*(m_nColumn+1)+j+1] = grid_map[i*m_nColumn+j];
			}
		}
		delete []grid_map;
		grid_map = new_map;
		m_nColumn++;
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				grid_map[i*m_nColumn+j].iRow	= i;
				grid_map[i*m_nColumn+j].iColumn	= j;
			}
		}
	}
	return TRUE;
}
BOOL CXhGridMap::DelColumn(int iColumn)
{
	int i,j;
	if(iColumn<0||iColumn>=m_nColumn)
		return FALSE;
	else
	{
		CXhGrid *new_map = new CXhGrid[m_nRow*(m_nColumn-1)];
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				if(j<iColumn)
					new_map[i*(m_nColumn-1)+j] = grid_map[i*m_nColumn+j];
				else if(j==iColumn)
					continue;
				else
					new_map[i*(m_nColumn-1)+j-1] = grid_map[i*m_nColumn+j];
			}
		}
		delete []grid_map;
		grid_map = new_map;
		m_nColumn--;
		for(i=0;i<m_nRow;i++)
		{
			for(j=0;j<m_nColumn;j++)
			{
				grid_map[i*m_nColumn+j].iRow	= i;
				grid_map[i*m_nColumn+j].iColumn	= j;
			}
		}
	}
	return TRUE;
}
	//根据行列号获得指定单元格指针
CXhGrid* CXhGridMap::GetGridAt(int iRow,int iColumn)
{
	int i = iRow*(m_nColumn)+iColumn;
	if(i<0||i>=m_nRow*m_nColumn)
		return NULL;
	else
		return &grid_map[iRow*(m_nColumn)+iColumn];
}
	//填充单元格内容
BOOL CXhGridMap::SetGrid(int iRow, int iColumn,const CXhGrid &grid)
{
	if(iRow*m_nColumn+iColumn<0||iRow*m_nColumn+iColumn>=m_nRow*m_nColumn)
		return FALSE;
	CXhGrid *pGrid = GetGridAt(iRow,iColumn);
	if(pGrid)
	{
		pGrid->Copy(grid);	//全部赋值给指定单元格
		return TRUE;
	}
	else
		return FALSE;
}
////////////////////////////////////////////////////////////////////////
//CXhTable
CXhTable::CXhTable(long iSerial /*= 0*/)
{
	m_iSerial = iSerial;//标识
	ciLocaStyle = XHTBLDEF::TOP_LEFT;
	rows_arr_high = columns_arr_wide = NULL;
}
CXhTable::~CXhTable()
{
	Destroy();
}
BOOL CXhTable::Create(int nRow,int nCol, double high /*= 6*/, double width /*= 16*/)
{
	if(nRow<1|| nCol <1)
		return FALSE;
	if(!grid_map.Create(nRow, nCol))
		return FALSE;
	rows_arr_high = new double[nRow];
	columns_arr_wide = new double[nCol];
	for (int i = 0; i < nRow; i++)
		rows_arr_high[i] = high;
	for (int i = 0; i < nCol; i++)
		columns_arr_wide[i] = width;
	return TRUE;
}
//iRow和iColumn必须大于等于1并分别小于规格表的行总数和列总数
BOOL CXhTable::SetGrid(int iRow, int iColumn,char *string,BOOL bForceToString/*=TRUE*/)
{
	if(!IsValidGrid(iRow,iColumn))
		return FALSE;
	return grid_map.GetGridAt(iRow,iColumn)->data.SetGridString(string,bForceToString);
}
//清空指定行的单元格内容
BOOL CXhTable::CleanGrid(int iStartRow,int iEndRow)
{
	for(int i=iStartRow;i<iEndRow;i++)
	{
		for(int j=0;j< nColsCount;j++)
		{
			if(!IsValidGrid(i,j))
				continue;
			grid_map.GetGridAt(i,j)->data.ClearVariant();
		}
	}
	return TRUE;
}
BOOL CXhTable::SetRowFontSize(int iRow, float fontSize,bool bBold /*= false*/)
{
	if (iRow >= nRowsCount || fontSize <= 0)
		return FALSE;
	for (int iCol = 0; iCol < nColsCount; iCol++)
	{
		GetGridAt(iRow, iCol)->simfont.sfTextSize = fontSize;
		GetGridAt(iRow, iCol)->simfont.bBold = bBold;
	}
	return TRUE;
}
BOOL CXhTable::SetRowColor(int iRow, COLORREF crFontColor, COLORREF crBackColor /*= RGB(255, 255, 255)*/)
{
	if (iRow >= nRowsCount)
		return FALSE;
	for (int iCol = 0; iCol < nColsCount; iCol++)
	{
		GetGridAt(iRow, iCol)->xFontStyle.crTextColor = crFontColor;
		GetGridAt(iRow, iCol)->xFontStyle.crBackground = crBackColor;
	}
	return TRUE;
}
BOOL CXhTable::SetRowHigh(int iRow,double high)
{
	if(iRow<0||iRow>=grid_map.GetRowsCount())
		return FALSE;
	rows_arr_high[iRow] = high;
	return TRUE;
}
BOOL CXhTable::SetColWide(int iColumn,double wide)
{
	if(iColumn<0||iColumn>=grid_map.GetColumnsCount())
		return FALSE;
	columns_arr_wide[iColumn] = wide;
	return TRUE;
}
void CXhTable::GetBasePos(double* pos2d)
{
	pos2d[0]=xLocation.x;
	pos2d[1]=xLocation.y;
}
void CXhTable::SetBasePos(const double* pos2d)
{
	xLocation.x = pos2d[0];
	xLocation.y = pos2d[1];
}
void CXhTable::SetLocateStyle(BYTE ciStyle)
{
	ciLocaStyle = ciStyle;
}
	//iRow和iColumn必须大于等于1并分别小于规格表的行总数和列总数
BOOL CXhTable::GetGridString(int iRow, int iColumn,char *string)
{
	if(!IsValidGrid(iRow,iColumn))
		return FALSE;
	CXhGrid *pGrid = grid_map.GetGridAt(iRow,iColumn);
	if(pGrid==NULL||pGrid->data.sVal==NULL||pGrid->data.ciType!=XHTBLDEF::GRID_VT_VARSTR)
		return FALSE;
	else 
		strcpy(string,pGrid->data.sVal);
	return TRUE;
}
	//获得指定行线距离格式表相应外缘的位置距离
double CXhTable::getRowLinePos(BOOL bUpToDown,int iRow)
{
	double dist = 0;
	if(bUpToDown)	//从上顶端到指定行距离
	{
		for(int i=0;i<iRow;i++)
			dist += rows_arr_high[i];
	}
	else			//从下底端到指定行距离
	{
		for(int i=grid_map.GetRowsCount()-1;i>=iRow;i--)
			dist += rows_arr_high[i];
	}
	return dist;
}
	//获得指定列线距离格式表相应外缘的位置距离
double CXhTable::getColumnLinePos(BOOL bLToR,int iColumn)
{
	double dist = 0;
	if(bLToR)	//从左端到指定列距离
	{
		for(int i=0;i<iColumn;i++)
			dist += columns_arr_wide[i];
	}
	else		//从右端到指定列距离
	{
		for(int i=grid_map.GetColumnsCount()-1;i>=iColumn;i--)
			dist += columns_arr_wide[i];
	}
	return dist;
}
BOOL CXhTable::GetRowLineStart(int iRow,double* pStartPos2d)
{
	if(iRow<0||iRow>grid_map.GetRowsCount())
		return FALSE;
	GEPOINT2D start;
	switch(ciLocaStyle)
	{
	case XHTBLDEF::TOP_LEFT:		//左上角为二维表基点
		start.x = xLocation.x;
		start.y = xLocation.y-getRowLinePos(TRUE,iRow);
		break;
	case XHTBLDEF::TOP_RIGHT:		//右上角为基点
		start.x = xLocation.x - getColumnLinePos(FALSE,0);
		start.y = xLocation.y-getRowLinePos(TRUE,iRow);
		break;
	case XHTBLDEF::BOTTOM_LEFT:	//左下角为基点
		start.x = xLocation.x;
		start.y = xLocation.y+getRowLinePos(FALSE,iRow);
		break;
	case XHTBLDEF::BOTTOM_RIGHT:	//右下角为基点
		start.x = xLocation.x - getColumnLinePos(FALSE,0);
		start.y = xLocation.y+getRowLinePos(FALSE,iRow);
		break;
	default:
		return FALSE;
	}
	if (pStartPos2d!=NULL)
	{
		pStartPos2d[0]=start.x;
		pStartPos2d[1]=start.y;
	}
	return TRUE;
}
BOOL CXhTable::GetRowLineEnd(int iRow,double* pEndPos2d)
{
	if(iRow<0||iRow>grid_map.GetRowsCount())
		return FALSE;
	GEPOINT2D end;
	switch(ciLocaStyle)
	{
	case XHTBLDEF::TOP_LEFT:
		end.x = xLocation.x+getColumnLinePos(TRUE,grid_map.GetColumnsCount());
		end.y = xLocation.y-getRowLinePos(TRUE,iRow);
		break;
	case XHTBLDEF::TOP_RIGHT:
		end.x = xLocation.x;
		end.y = xLocation.y-getRowLinePos(TRUE,iRow);
		break;
	case XHTBLDEF::BOTTOM_LEFT:
		end.x = xLocation.x+getColumnLinePos(TRUE,grid_map.GetColumnsCount());
		end.y = xLocation.y+getRowLinePos(FALSE,iRow);
		break;
	case XHTBLDEF::BOTTOM_RIGHT:
		end.x = xLocation.x;
		end.y = xLocation.y+getRowLinePos(FALSE,iRow);
		break;
	default:
		return FALSE;
	}
	if (pEndPos2d!=NULL)
	{
		pEndPos2d[0]=end.x;
		pEndPos2d[1]=end.y;
	}
	return TRUE;
}
BOOL CXhTable::GetColLineStart(int iColumn,double* pStartPos2d)
{
	if(iColumn<0||iColumn>grid_map.GetColumnsCount())
		return FALSE;
	GEPOINT2D start;
	switch(ciLocaStyle)
	{
	case XHTBLDEF::TOP_LEFT:
		start.x = xLocation.x+getColumnLinePos(TRUE,iColumn);
		start.y = xLocation.y;
		break;
	case XHTBLDEF::TOP_RIGHT:
		start.x = xLocation.x - getColumnLinePos(FALSE,iColumn);
		start.y = xLocation.y;
		break;
	case XHTBLDEF::BOTTOM_LEFT:
		start.x = xLocation.x+getColumnLinePos(TRUE,iColumn);
		start.y = xLocation.y+getRowLinePos(FALSE,0);
		break;
	case XHTBLDEF::BOTTOM_RIGHT:
		start.x = xLocation.x - getColumnLinePos(FALSE,iColumn);
		start.y = xLocation.y+getRowLinePos(FALSE,0);
		break;
	default:
		return FALSE;
	}
	if (pStartPos2d!=NULL)
	{
		pStartPos2d[0]=start.x;
		pStartPos2d[1]=start.y;
	}
	return TRUE;
}
BOOL CXhTable::GetColLineEnd(int iColumn,double* pEndPos2d)
{
	if(iColumn<0||iColumn>grid_map.GetColumnsCount())
		return FALSE;
	GEPOINT2D end;
	switch(ciLocaStyle)
	{
	case XHTBLDEF::TOP_LEFT:
		end.x = xLocation.x+getColumnLinePos(TRUE,iColumn);
		end.y = xLocation.y-getRowLinePos(TRUE,grid_map.GetRowsCount());
		break;
	case XHTBLDEF::TOP_RIGHT:
		end.x = xLocation.x-getColumnLinePos(FALSE,iColumn);
		end.y = xLocation.y-getRowLinePos(TRUE,grid_map.GetRowsCount());
		break;
	case XHTBLDEF::BOTTOM_LEFT:
		end.x = xLocation.x+getColumnLinePos(TRUE,iColumn);
		end.y = xLocation.y;
		break;
	case XHTBLDEF::BOTTOM_RIGHT:
		end.x = xLocation.x-getColumnLinePos(FALSE,iColumn);
		end.y = xLocation.y;
		break;
	default:
		return FALSE;
	}
	if (pEndPos2d!=NULL)
	{
		pEndPos2d[0]=end.x;
		pEndPos2d[1]=end.y;
	}
	return TRUE;
}
BOOL CXhTable::GetRowHigh(int iRow, double *high)
{
	if(!IsValidGrid(iRow,1))
		return FALSE;
	*high = rows_arr_high[iRow];
	return TRUE;
}
BOOL CXhTable::GetColWide(int iColumn, double *wide)
{
	if(!IsValidGrid(1,iColumn))
		return FALSE;
	*wide = columns_arr_wide[iColumn];
	return TRUE;
}
BYTE CXhTable::GetLocateStyle()
{
	return ciLocaStyle;
}
void CXhTable::SetFontSize(float fontSize /*= 2*/)
{
	for (int iRow = 0; iRow < nRowsCount; iRow++)
	{
		for (int iCol = 0; iCol < nColsCount; iCol++)
		{
			GetGridAt(iRow, iCol)->simfont.sfTextSize = fontSize;
		}
	}
}
	//获得指定单元格大小(高宽)
BOOL CXhTable::GetGridRange(int &start_i,int &start_j,int *end_i, int *end_j)
{
	int i,j,tem_end_i=start_i,tem_end_j=start_j;
	f2dPoint pos;
	//判断索引号是否合法
	if(!IsValidGrid(start_i,start_j))
		return FALSE;
	CXhGrid *pGrid = grid_map.GetGridAt(start_i,start_j);
	//判断指定表格单元是否为虚单元(不画出)
	if(pGrid->m_bVGridVirtual||pGrid->m_bHGridVirtual)
		return FALSE;
	//定位表格单元左上角的坐标位置
	if(!GetColLineStart(start_j,pos))
		return FALSE;
	if(!GetRowLineStart(start_i,pos))
		return FALSE;
	//定位表格单元右下角的坐标位置
	for(i=start_i+1;i<grid_map.GetRowsCount();i++)		//计算单元格高度
	{
		if(!GetGridAt(i,start_j)->m_bVGridVirtual)//是否与同列下一单元格合并
			break;
		tem_end_i = i;
	}
	for(j=start_j+1;j<grid_map.GetColumnsCount();j++)	//计算单元格宽度
	{
		if(!GetGridAt(start_i,j)->m_bHGridVirtual)	//是否与同行下一单元格合并
			break;
		tem_end_j = j;
	}
	if(tem_end_i>start_i||tem_end_j>start_j)
	{
		if(end_i)
			*end_i = tem_end_i;
		if(end_j)
			*end_j = tem_end_j;
		return TRUE;
	}
	else
		return FALSE;
}
//表格单元索引号以0为起始值
BOOL CXhTable::GetGridRect(int iRow, int iColumn, f2dRect &rect)
{
	int i,j;
	f2dPoint pos;
	//判断索引号是否合法
	if(!IsValidGrid(iRow,iColumn))
		return FALSE;
	CXhGrid *pGrid = grid_map.GetGridAt(iRow,iColumn);
	//判断指定表格单元是否为虚单元(不画出)
	if(pGrid->m_bVGridVirtual||pGrid->m_bHGridVirtual)
		return FALSE;
	//定位表格单元左上角的坐标位置
	if(!GetColLineStart(iColumn,pos))
		return FALSE;
	rect.topLeft.x = pos.x;
	if(!GetRowLineStart(iRow,pos))
		return FALSE;
	rect.topLeft.y = pos.y;
	rect.bottomRight.x = rect.topLeft.x+columns_arr_wide[iColumn];
	rect.bottomRight.y = rect.topLeft.y-rows_arr_high[iRow];
	//定位表格单元右下角的坐标位置
	for(i=iRow+1;i<grid_map.GetRowsCount();i++)		//计算单元格高度
	{
		if(!GetGridAt(i,iColumn)->m_bVGridVirtual)//是否与同列下一单元格合并
			break;
		rect.bottomRight.y -= rows_arr_high[i];
	}
	for(j=iColumn+1;j<grid_map.GetColumnsCount();j++)	//计算单元格宽度
	{
		if(!GetGridAt(iRow,j)->m_bHGridVirtual)	//是否与同行下一单元格合并
			break;
		rect.bottomRight.x += columns_arr_wide[j];
	}
	return TRUE;
}
BOOL CXhTable::GetGridSize(int iRow,int iColumn,
							 double *height, double *wide)
{
	f2dRect rect;
	GetGridRect(iRow,iColumn,rect);
	*height = fabs(rect.topLeft.y-rect.bottomRight.y);
	*wide = fabs(rect.bottomRight.x-rect.topLeft.x);
	return FALSE;
}

//表格单元索引号以0为起始值
CXhGrid* CXhTable::GetGridAt(int iRow, int iColumn)
{
	return grid_map.GetGridAt(iRow,iColumn);
}
//表格单元索引号以0为起始值
BOOL CXhTable::IsValidGrid(int iRow, int iColumn)
{
	if(iRow<0||iRow>=grid_map.GetRowsCount()||iColumn<0||iColumn>=grid_map.GetColumnsCount())
		return FALSE;
	else
		return TRUE;
}
double CXhTable::GetTableWide()
{
	double wide=0;
	for(int i=0;i<grid_map.GetColumnsCount();i++)
		wide+=columns_arr_wide[i];
	return wide;
}
double CXhTable::GetTableHeight()
{
	double height=0;
	for(int i=0;i<grid_map.GetRowsCount();i++)
		height+=rows_arr_high[i];
	return height;
}

void CXhTable::Destroy()
{
	grid_map.Destroy();
	ciLocaStyle = XHTBLDEF::TOP_LEFT;
	if(columns_arr_wide!=NULL)
		delete []columns_arr_wide;
	if(rows_arr_high!=NULL)
		delete []rows_arr_high;
	rows_arr_high = columns_arr_wide = NULL;
}

BOOL CXhTable::InsertRow(int iRow,double high)
{
	int i,n = grid_map.GetRowsCount();
	if(iRow<0||iRow>=n)
		return FALSE;
	else
	{
		double *rows_high = new double[n];
		memcpy(rows_high,rows_arr_high,n*sizeof(double));
		delete[]rows_arr_high;
		rows_arr_high = new double[n+1];
		for(i=0;i<iRow;i++)
			rows_arr_high[i] = rows_high[i];
		rows_arr_high[iRow] = high;
		for(i=iRow;i<n;i++)
			rows_arr_high[i+1] = rows_high[i];
		delete []rows_high;
		return grid_map.InsertRow(iRow);
	}
}
BOOL CXhTable::DelRow(int iRow)
{
	int i,n = grid_map.GetRowsCount();
	if(iRow<0||iRow>=n||iRow>=n)
		return FALSE;
	else
	{
		double *rows_high = new double[n];
		memcpy(rows_high,rows_arr_high,n*sizeof(double));
		delete[]rows_arr_high;
		rows_arr_high = new double[n-1];
		for(i=0;i<iRow;i++)
			rows_arr_high[i] = rows_high[i];
		for(i=iRow+1;i<n;i++)
			rows_arr_high[i-1] = rows_high[i];
		delete []rows_high;
		return grid_map.DelRow(iRow);
	}
}
BOOL CXhTable::InsertCol(int iColumn,double wide)
{
	int i,n = grid_map.GetColumnsCount();
	if(iColumn<0||iColumn>=n)
		return FALSE;
	else
	{
		double *columns_wide = new double[n];
		memcpy(columns_wide,columns_arr_wide,n*sizeof(double));
		delete[]columns_arr_wide;
		columns_arr_wide = new double[n+1];
		for(i=0;i<iColumn;i++)
			columns_arr_wide[i] = columns_wide[i];
		columns_arr_wide[iColumn] = wide;
		for(i=iColumn;i<n;i++)
			columns_arr_wide[i+1] = columns_wide[i];
		delete []columns_wide;
		return grid_map.InsertColumn(iColumn);
	}
}
BOOL CXhTable::DelCol(int iColumn)
{
	int i,n = grid_map.GetColumnsCount();
	if(iColumn<0||iColumn>=n||iColumn>=n)
		return FALSE;
	else
	{
		double *columns_wide = new double[n];
		memcpy(columns_wide,columns_arr_wide,n*sizeof(double));
		delete[]columns_arr_wide;
		columns_arr_wide = new double[n-1];
		for(i=0;i<iColumn;i++)
			columns_arr_wide[i] = columns_wide[i];
		for(i=iColumn+1;i<n;i++)
			columns_arr_wide[i-1] = columns_wide[i];
		delete []columns_wide;
		return grid_map.DelColumn(iColumn);
	}
}
//从文本文件导入格式表
BOOL CXhTable::ImportTxt(FILE *fp)
{
	char sLineText[200],*sTokey=NULL,separator[]=" =,\r\n";
	while(feof(fp)==0)
	{
		fgets(sLineText,200,fp);
		sTokey = strtok(sLineText,separator);
		if(sTokey)
		{
			if(stricmp(sTokey,"FormatTableID")==0)
			{
				sTokey = strtok(NULL,separator);
				//if(sTokey)
					//ID = atoi(sTokey);
			}
			else if(stricmp(sTokey,"LocateStyle")==0)
			{
				sTokey = strtok(NULL,separator);
				if(sTokey)
					ciLocaStyle = (BYTE)atoi(sTokey);
			}
			else if(stricmp(sTokey,"Rows")==0)
			{
				int nRow,nCol;
				sTokey = strtok(NULL,separator);
				if(sTokey)
				{
					nRow = atoi(sTokey);
					sTokey = strtok(NULL,separator);
					if(sTokey)
					{
						if(stricmp(sTokey,"Columns")==0)
							sTokey = strtok(NULL,separator);
						if(sTokey)
							nCol = atoi(sTokey);
						else
							nCol = 1;
					}
					else 
						nCol = 1;
					Create(nRow,nCol);
				}
			}
			else if(stricmp(sTokey,"BasePosX")==0)
			{
				sTokey = strtok(NULL,separator);
				if(sTokey)
					xLocation.x = atof(sTokey);
			}
			else if(stricmp(sTokey,"BasePosY")==0)
			{
				sTokey = strtok(NULL,separator);
				if(sTokey)
					xLocation.y = atof(sTokey);
			}
			else if(sLineText[0]=='#')
				continue;
			else if(sLineText[0]=='\n')
				continue;
			else if(stricmp(sTokey,"Row_i")==0)
			{
				int ii;
				sTokey = strtok(NULL,separator);
				if(sTokey)
				{
					ii = atoi(sTokey);
					sTokey = strtok(NULL,separator);
					if(sTokey&&stricmp(sTokey,"High")==0)
					{
						sTokey = strtok(NULL,separator);
						double high = atof(sTokey);
						SetRowHigh(ii,high);
					}
				}
			}
			else if(stricmp(sTokey,"Column_j")==0)
			{
				int ii;
				sTokey = strtok(NULL,separator);
				if(sTokey)
				{
					ii = atoi(sTokey);
					sTokey = strtok(NULL,separator);
					if(sTokey&&stricmp(sTokey,"Wide")==0)
					{
						sTokey = strtok(NULL,separator);
						double wide = atof(sTokey);
						SetColWide(ii,wide);
					}
				}
			}
			else if(stricmp(sTokey,"--NewGridStart")==0)
			{
				fgets(sLineText,200,fp);
				sTokey = strtok(sLineText,separator);
				int i=0,j=0;
				if(sTokey&&strcmp(sTokey,"i")==0)
				{
					sTokey = strtok(NULL,separator);
					if(sTokey)
					{
						i = atoi(sTokey);
						sTokey = strtok(NULL,separator);
					}
				}
				if(sTokey&&strcmp(sTokey,"j")==0)
				{
					sTokey = strtok(NULL,separator);
					j = atoi(sTokey);
				}
				grid_map.GetGridAt(i,j)->Import(fp);
			}
			else if(stricmp(sTokey,"--EndFormatTbl")==0)
				break;
		}
	}
	return TRUE;
}
BOOL CXhTable::ImportTxt(char *filename)
{
	FILE* fp = fopen(filename, "rt");
	if(fp==NULL)
		return FALSE;
	ImportTxt(fp);
	fclose(fp);
	return TRUE;
}
//向文本文件导出格式表
BOOL CXhTable::ExportTxt(FILE *fp)
{
	fprintf(fp,"FormatTableID=%d\n",ID);
	fprintf(fp,"LocateStyle=%d\n",ciLocaStyle);
	fprintf(fp,"Rows=%d,Columns=%d\n", nRowsCount, nColsCount);
	fprintf(fp,"BasePosX=%.f\n",xLocation.x);
	fprintf(fp,"BasePosY=%.f\n",xLocation.y);
	fprintf(fp,"\n#----------Row High Section-----------\n\n");
	for(int i=0;i< nRowsCount;i++)
		fprintf(fp,"Row_i=%d,High=%.f\n",i,rows_arr_high[i]);
	fprintf(fp,"\n#----------Column Wide Section-----------\n\n");
	for(int i=0;i< nColsCount;i++)
		fprintf(fp,"Column_j=%d,Wide=%.f\n",i,columns_arr_wide[i]);
	fprintf(fp,"\n#----------Grid Content Section-----------\n\n");
	for(int i=0;i< nRowsCount;i++)
	{
		for(int j=0;j< nColsCount;j++)
		{
			fprintf(fp,"--NewGridStart\n");
			fprintf(fp,"i=%d,j=%d\n",i,j);
			grid_map.GetGridAt(i,j)->Export(fp);
			fprintf(fp,"\n");
		}
	}

	return TRUE;
}
BOOL CXhTable::ExportTxt(char *filename)
{
	FILE* fp = fopen(filename, "wt");
	if(fp==NULL)
		return FALSE;
	ExportTxt(fp);
	fclose(fp);
	return TRUE;
}

int CXhTable::ReleaseSnapStatus()
{
	int nHits=0;
	long nRows = grid_map.GetRowsCount();
	long nColumns = grid_map.GetColumnsCount();
	for(int i=0;i<nRows;i++)
	{
		for(int j=0;j<nColumns;j++)
		{
			CXhGrid *pGrid = grid_map.GetGridAt(i,j);
			if(pGrid->blStateSelected)
			{
				nHits++;
				pGrid->blStateSelected=FALSE;
			}
		}
	}
	return nHits;
}

int CXhTable::GetSelectedCounts()
{
	int nHits=0;
	long nRows = grid_map.GetRowsCount();
	long nColumns = grid_map.GetColumnsCount();
	for(int i=0;i<nRows;i++)
	{
		for(int j=0;j<nColumns;j++)
		{
			CXhGrid *pGrid = grid_map.GetGridAt(i,j);
			if(pGrid->blStateSelected)
			{
				nHits++;
			}
		}
	}
	return nHits;
}
CXhGrid *CXhTable::GetFirstSelectedGrid(CXhTable **pFormatTbl/*=NULL*/,int *pos_i/*=NULL*/,int *pos_j/*=NULL*/)
{
	long nRows = grid_map.GetRowsCount();
	long nColumns = grid_map.GetColumnsCount();
	for(int i=0;i<nRows;i++)
	{
		for(int j=0;j<nColumns;j++)
		{
			CXhGrid *pGrid = grid_map.GetGridAt(i,j);
			if(pGrid->blStateSelected)
			{
				if(pFormatTbl)
					*pFormatTbl = this;
				if(pos_i)
					*pos_i = i;
				if(pos_j)
					*pos_j = j;
				return pGrid;
			}
		}
	}
	return NULL;
}

void CXhTable::SelectAll()
{
	long nRows = grid_map.GetRowsCount();
	long nColumns = grid_map.GetColumnsCount();
	for(int i=0;i<nRows;i++)
		for(int j=0;j<nColumns;j++)
			grid_map.GetGridAt(i,j)->blStateSelected=TRUE;
}

CXhGrid * CXhTable::SnapGrid(double x, double y, int *iRow, int *iCol)
{
	f2dRect rect;
	for(int i=0;i< nRowsCount;i++)
	{
		for(int j=0;j< nColsCount;j++)
		{
			GetGridRect(i,j,rect);
			if(rect.PtInRect(f2dPoint(x,y)))
			{
				if(iRow)
					*iRow = i;
				if(iCol)
					*iCol = j;
				return GetGridAt(i,j);
			}
		}
	}
	return NULL;
}

BOOL CXhTable::GetTblRect(f2dRect *tbl_rect)
{
	if (nRowsCount <= 0 || nColsCount <= 0)
		return FALSE;
	double tbl_wide,tbl_high;
	tbl_wide = GetTableWide();
	tbl_high = GetTableHeight();
	switch(ciLocaStyle)
	{
	case XHTBLDEF::TOP_LEFT:		//左上角
		tbl_rect->topLeft.Set(xLocation.x,xLocation.y);
		tbl_rect->bottomRight.Set(xLocation.x+tbl_wide,xLocation.y-tbl_high);
		break;
	case XHTBLDEF::TOP_RIGHT:		//右上角
		tbl_rect->topLeft.Set(xLocation.x-tbl_wide,xLocation.y);
		tbl_rect->bottomRight.Set(xLocation.x,xLocation.y-tbl_high);
		break;
	case XHTBLDEF::BOTTOM_LEFT:	//左下角
		tbl_rect->topLeft.Set(xLocation.x,xLocation.y+tbl_high);
		tbl_rect->bottomRight.Set(xLocation.x+tbl_wide,xLocation.y);
		break;
	case XHTBLDEF::BOTTOM_RIGHT:	//右下角
		tbl_rect->topLeft.Set(xLocation.x-tbl_wide,xLocation.y+tbl_high);
		tbl_rect->bottomRight.Set(xLocation.x,xLocation.y);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
//合并单元格：cbMergeFlag:0.自动合并;0x01横向合并;0x02竖向合并
BOOL CXhTable::MergeGrid(int iCellStartRow,int iCellStartCol,int iCellEndRow,int iCellEndCol,BYTE cbMergeFlag/*=0*/)
{
	if (iCellStartRow<0||iCellStartCol<0||iCellStartRow<iCellEndRow||iCellStartCol<iCellEndCol||
		iCellEndRow>=this->nRowsCount||iCellEndCol>=nColsCount)
		return FALSE;
	if ((iCellEndRow-iCellStartRow)+(iCellEndCol-iCellStartCol)==0)
		return FALSE;
	int rowI=0,colJ=0;
	if (iCellEndRow-iCellStartRow==0)
	{	//单行合并
		rowI=iCellStartRow;
		for (colJ=iCellStartCol+1;colJ<=iCellEndCol;colJ++)
		{
			CXhGrid* pGrid=this->GetGridAt(rowI,colJ);
			pGrid->m_bVGridVirtual=true;
		}
	}
	else if (iCellEndCol-iCellStartCol==0)
	{	//单列合并
		colJ=iCellStartCol;
		for (rowI=iCellStartRow+1;rowI<=iCellEndRow;rowI++)
		{
			CXhGrid* pGrid=this->GetGridAt(rowI,colJ);
			pGrid->m_bHGridVirtual=true;
		}
	}
	else
	{	//二维网格区域合并
		bool blMergeHori=(cbMergeFlag&0x03)==0||(cbMergeFlag&0x01)>0;
		bool blMergeVert=(cbMergeFlag&0x03)==0||(cbMergeFlag&0x02)>0;
		for (rowI=iCellStartRow;rowI<=iCellEndRow;rowI++)
		{
			for (colJ=iCellStartCol;colJ<=iCellEndCol;colJ++)
			{
				CXhGrid* pGrid=this->GetGridAt(rowI,colJ);
				pGrid->m_bHGridVirtual=blMergeHori&&(colJ>iCellStartCol);
				pGrid->m_bVGridVirtual=blMergeVert&&(rowI>iCellStartRow);
			}
		}
	}
	return TRUE;
}

BOOL CXhTable::SnapGridPos(double x, double y, double *pSnapPos2d,double scope/*=1.0*/)
{
	f2dPoint start,tem_snap_pos;
	BOOL bSnaped = FALSE;
	double dist = scope;
	GEPOINT2D snappoint(x,y);
	for(int i=0;i<= nRowsCount;i++)
	{
		GetRowLineStart(i,start);
		if(fabs(start.y-y)<dist)
		{
			snappoint.y = start.y;
			dist = fabs(start.y-y);
			bSnaped = TRUE;
		}
	}
	dist = scope;
	for(int i=0;i<= nColsCount;i++)
	{
		GetColLineStart(i,start);
		if(fabs(start.x-x)<dist)
		{
			snappoint.x = start.x;
			dist = fabs(start.x-x);
			bSnaped = TRUE;
		}
	}
	if (pSnapPos2d)
	{
		pSnapPos2d[0]=snappoint.x;
		pSnapPos2d[1]=snappoint.y;
	}
	return bSnaped;
}

void CXhTable::CopyFrom(CXhTable *pFormatTbl)
{
	pFormatTbl->GetBasePos(xLocation);
	ciLocaStyle = pFormatTbl->GetLocateStyle();
	Create(pFormatTbl->nRowsCount, pFormatTbl->nColsCount);
	for(int i=0;i< pFormatTbl->nRowsCount;i++)
	{
		double high = 0;
		pFormatTbl->GetRowHigh(i,&high);
		SetRowHigh(i,high);
	}
	for(int j=0;j< pFormatTbl->nColsCount;j++)
	{
		double wide = 0;
		pFormatTbl->GetColWide(j,&wide);
		SetColWide(j,wide);
	}
	for(int i=0;i< pFormatTbl->nRowsCount;i++)
	{
		for(int j=0;j< pFormatTbl->nColsCount;j++)
		{
			CXhGrid *pGrid = pFormatTbl->GetGridAt(i,j);
			GetGridAt(i,j)->Copy(*pGrid);
		}
	}
}


//复制表格但不复制iRemoveStart至iRemoveEnd之间的行 wht 11-03-13
void CXhTable::CopyFrom(CXhTable *pFormatTbl,int iRemoveStart,int iRemoveEnd)
{
	if(iRemoveStart<0||iRemoveEnd<0||iRemoveEnd<=iRemoveStart)
	{	//复制完整的表格数据
		CopyFrom(pFormatTbl);
		return;
	}
	pFormatTbl->GetBasePos(xLocation);
	ciLocaStyle = pFormatTbl->GetLocateStyle();
	int nRealRows= pFormatTbl->nRowsCount -(iRemoveEnd-iRemoveStart+1);	//真实行数
	Create(nRealRows, pFormatTbl->nColsCount);
	int iRowIndex=0;
	for(int i=0;i< pFormatTbl->nRowsCount;i++)
	{	//设置行高
		if(i>=iRemoveStart&&i<=iRemoveEnd)
			continue;	//跳过指定行
		double high = 0;
		pFormatTbl->GetRowHigh(i,&high);
		SetRowHigh(iRowIndex,high);
		iRowIndex++;
	}
	for(int j=0;j< pFormatTbl->nColsCount;j++)
	{	//设置列宽
		double wide = 0;
		pFormatTbl->GetColWide(j,&wide);
		SetColWide(j,wide);
	}
	iRowIndex=0;
	for(int i=0;i< pFormatTbl->nRowsCount;i++)
	{	//设置单元格数据
		if(i>=iRemoveStart&&i<=iRemoveEnd)
			continue;	//跳过指定行
		for(int j=0;j< pFormatTbl->nColsCount;j++)
		{
			CXhGrid *pGrid = pFormatTbl->GetGridAt(i,j);
			GetGridAt(iRowIndex,j)->Copy(*pGrid);
		}
		iRowIndex++;
	}
}

BOOL CXhTable::ReadBasicTbl(BUFFER_IO *pIO)
{
	int i,j,nRow,nCol;
	pIO->ReadByte(&ciLocaStyle);
	pIO->ReadInteger(&nRow);
	pIO->ReadInteger(&nCol);
	pIO->ReadDouble(&xLocation.x);
	pIO->ReadDouble(&xLocation.y);
	Create(nRow,nCol);
	for(i=0;i<nRow;i++)
		pIO->ReadDouble(&rows_arr_high[i]);
	for(i=0;i<nCol;i++)
		pIO->ReadDouble(&columns_arr_wide[i]);
	for(i=0;i<nRow;i++)
	{
		for(j=0;j<nCol;j++)
			grid_map.GetGridAt(i,j)->ReadGridFromStream(pIO);
	}
	return TRUE;
}

BOOL CXhTable::WriteBasicTbl(BUFFER_IO *pIO)
{
	pIO->WriteByte(ciLocaStyle);
	pIO->WriteInteger(nRowsCount);
	pIO->WriteInteger(nColsCount);
	pIO->WriteDouble(xLocation.x);
	pIO->WriteDouble(xLocation.y);
	for(int i=0;i< nRowsCount;i++)
		pIO->WriteDouble(rows_arr_high[i]);
	for(int i=0;i< nColsCount;i++)
		pIO->WriteDouble(columns_arr_wide[i]);
	for(int i=0;i< nRowsCount;i++)
	{
		for(int j=0;j< nColsCount;j++)
			grid_map.GetGridAt(i,j)->WriteGridToStream(pIO);
	}
	return TRUE;
}
//行高单位转化，Excel行高单位为磅，CAD行高单位默认位mm
double CXhTable::TransRowHightToCad(double fRowHight)
{	//1个单位＝0.3612毫米
	return fRowHight * 0.3612;
}
double CXhTable::TransRowHightToXls(double fRowHight)
{	//1毫米＝2.7682个单位
	return fRowHight * 2.7682;
}
//列宽单位转化，Excel列宽单位为1/10英寸，CAD列宽单位默认为mm
double CXhTable::TransColWidthToCad(double fColWidth)
{	//1个单位＝2.2862毫米
	return fColWidth * 2.2862;
}
double CXhTable::TransColWidthToXls(double fColWidth)
{	//1毫米＝0.4374个单位；
	return fColWidth * 0.4374;
}
