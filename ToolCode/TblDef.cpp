#include "stdafx.h"
#include "TblDef.h"

GRID_DATA_TYPE IntToGridDataType(int nType)
{
	switch(nType)
	{
	case GRID_VT_EMPTY:
		return GRID_VT_EMPTY;
	case GRID_VT_NULL:
		return GRID_VT_NULL;
	case GRID_VT_I2:
		return GRID_VT_I2;
	case GRID_VT_I4:
		return GRID_VT_I4;
	case GRID_VT_R4:
		return GRID_VT_R4;
	case GRID_VT_R8:
		return GRID_VT_R8;
	case GRID_VT_DATE:
		return GRID_VT_DATE;
	case GRID_VT_STRING:
		return GRID_VT_STRING;
	case GRID_VT_POINTER:
		return GRID_VT_POINTER;
	case GRID_VT_BOOL:
		return GRID_VT_BOOL;
	default:
		return GRID_VT_EMPTY;
	}
}
//----------------异构单元格内容类型类-------------------------------
CGridVariant::CGridVariant()
{
	memset(this,0,sizeof(CGridVariant));
	vType = GRID_VT_EMPTY;		//未指定数据类型
}
CGridVariant::CGridVariant(CGridVariant& SourceObj)
{
	*this = SourceObj;
}

CGridVariant::~CGridVariant()
{
	ClearVariant();
}
void CGridVariant::ClearVariant()	//清空异构变量所控内存并清零存储区
{
	GRID_DATA_TYPE old_type = vType;
	if(vType==GRID_VT_STRING&&sVal)
		free(sVal);		//清空字符串空间
	memset(this,0,sizeof(CGridVariant));	//清零存储区
	vType = old_type;	//恢得原数据类型
}
void CGridVariant::ChangeToType(GRID_DATA_TYPE data_type)
{
	if(vType==data_type)
		return;
	else
	{
		ClearVariant();
		vType = data_type;
	}
}
	//重载赋值操作符以防赋值时内存泄漏
CGridVariant& CGridVariant::operator = (const CGridVariant &var)
{
	if(var.vType==GRID_VT_STRING)
		SetGridString(var.sVal);
	else
		memcpy(this,&var,sizeof(CGridVariant));
	return *this;
}

BOOL CGridVariant::SetGridString(char *string, BOOL bForceToString/*=TRUE*/)
{
	if(vType!=GRID_VT_STRING&&!bForceToString||string==NULL)
		return FALSE;	//类型不匹配
	else if(vType!=GRID_VT_STRING)
	{
		ChangeToType(GRID_VT_STRING);
		sVal = (char*)realloc(sVal,strlen(string)+2);
		strcpy(sVal,string);
		return TRUE;
	}
	else
	{
		sVal = (char*)realloc(sVal,strlen(string)+2);
		strcpy(sVal,string);
		return TRUE;
	}
}

//----------------表格单元格类---------------------------------------
#ifdef __DRAWING_CONTEXT_
double CGrid::PREFER_TEXT_SIZE=2;
#endif
CGrid::CGrid()
{
	grid_mode = STRING_GRID;
	feature = iRow = iColumn = 0;
	m_bVGridVirtual = m_bHGridVirtual = FALSE;
	m_bSelectStatus = FALSE;
	data.vType = GRID_VT_STRING;
	//data.SetGridString("我是好人!");
	mark.bTitle = FALSE;
	mark.ext_feature = mark.type_id = 0;
#ifdef __DRAWING_CONTEXT_
	textsize = PREFER_TEXT_SIZE;//字高sys.dim_map.fNumericTextSize;
	text_angle = 0;
#endif
}

CGrid::~CGrid()
{
}
CGrid& CGrid::operator = (CGrid& grid)
{
	Copy(grid);
	return *this;
}

void CGrid::Copy(const CGrid &grid)
{
	m_bHGridVirtual = grid.m_bHGridVirtual;
	m_bVGridVirtual = grid.m_bVGridVirtual;
	data = grid.data;
	feature = grid.feature;
	grid_mode = grid.grid_mode;
#ifndef __DRAWING_CONTEXT_
	font_style = grid.font_style;
#else
	textsize = grid.textsize;
#endif;
	iRow = grid.iRow;
	iColumn = grid.iColumn;
}

void CGrid::Export(FILE *fp)
{
	fprintf(fp,"GridMode=%d\n",grid_mode);
	fprintf(fp,"HorizonVirtual=%d\n",m_bHGridVirtual);
	fprintf(fp,"VerticalVirtual=%d\n",m_bVGridVirtual);
	fprintf(fp,"Feature=%ld\n",feature);
	fprintf(fp,"\n--FontStyleStart\n");
#ifndef __DRAWING_CONTEXT_
	font_style.Export(fp);
#else
	fprintf(fp,"%.1f\n",textsize);
#endif
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

void CGrid::Import(FILE *fp)
{
	char sLineText[200],*sToken=NULL,separator[]=" =,\r\n";
	while(feof(fp)==0)
	{
		fgets(sLineText,200,fp);
		sToken = strtok(sLineText,separator);
		if(sToken&&stricmp(sToken,"GridMode")==0)
		{
			int mode;
			sToken = strtok(NULL,separator);
			if(sToken)
			{
				mode = atoi(sToken);
				if(mode==STRING_GRID)
					grid_mode = STRING_GRID;
				else if(mode==GRAPH_BLOCK_GRID)
					grid_mode = GRAPH_BLOCK_GRID;
				else if(mode==GRAPH_BLOCK_GRID)
					grid_mode = GRAPH_BLOCK_GRID;
			}
		}
		else if(sToken&&stricmp(sToken,"HorizonVirtual")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				m_bHGridVirtual = atoi(sToken);
		}
		else if(sToken&&stricmp(sToken,"VerticalVirtual")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				m_bVGridVirtual = atoi(sToken);
		}
		else if(sToken&&stricmp(sToken,"Feature")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				feature = atoi(sToken);
		}
		else if(sToken&&stricmp(sToken,"--FontStyleStart")==0)
		{
#ifndef __DRAWING_CONTEXT_
			font_style.Import(fp);
#else
			sToken = strtok(NULL,separator);
			if(sToken)
				textsize = atof(sToken);
#endif
		}
		else if(sToken&&stricmp(sToken,"--GridDataStart")==0)
		{
			data.Import(fp);
		}
		else if(sToken&&stricmp(sToken,"IsTitle")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				mark.bTitle = atoi(sToken);
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

CGrid::CFontStyle& CGrid::CFontStyle::operator =(const CGrid::CFontStyle& fontstyle)
{
	CoTaskMemFree(lfFaceName);	//清除以前的字样名称空间
	memcpy(this,&fontstyle,sizeof(CFontStyle));
	lfFaceName=NULL;	//赋为空指针
	SetFaceName(fontstyle.lfFaceName);
	return *this;
}
CGrid::CFontStyle& CGrid::CFontStyle::operator =(const LOGFONT& fontstyle)
{
	CopyFromLOGFONT(fontstyle);
	return *this;
}
CGrid::CFontStyle::operator LOGFONT()const
{
	LOGFONT logfont;
	CopyToLOGFONT(logfont);
	return logfont;
}

CGrid::CFontStyle::CFontStyle()
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
	SetFaceName(_T("宋体"));
	m_nTextAlignFlags = DT_CENTER|DT_VCENTER|DT_SINGLELINE;	//文字对齐方式
	m_fLeftDist=m_fRightDist=0.1;	//左右内边距----cm为单位
	m_fTopDist=m_fBottomDist=0.05;	//上下内边距----cm为单位
}
CGrid::CFontStyle::~CFontStyle()
{
	if(lfFaceName)
		CoTaskMemFree(lfFaceName);
}
int CGrid::CFontStyle::SetFaceName(const TCHAR *face_name)
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
void CGrid::CFontStyle::CopyFromLOGFONT(const LOGFONT &logfont)
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

void CGrid::CFontStyle::CopyToLOGFONT(LOGFONT &logfont)const
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
BOOL CGrid::CFontStyle::operator ==(const CFontStyle& fontstyle)const
{
	if(*this!=fontstyle)
		return FALSE;
	else
		return TRUE;
}
BOOL CGrid::CFontStyle::operator !=(const CFontStyle& fontstyle)const
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
}

void CGrid::CFontStyle::Import(FILE* fp)
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
				m_fLeftDist = atof(sToken);
		}
		else if(sToken&&stricmp(sToken,"TopDist")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				m_fTopDist = atof(sToken);
		fprintf(fp,"TopDist=%.f\n",m_fTopDist);
		}
		else if(sToken&&stricmp(sToken,"RightDist")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				m_fRightDist = atof(sToken);
		}
		else if(sToken&&stricmp(sToken,"BottomDist")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken)
				m_fBottomDist = atof(sToken);
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

void CGrid::CFontStyle::Export(FILE* fp)
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

void CGridVariant::Export(FILE *fp)
{
	CString sLineText;
	COleDateTime datetime;
	fprintf(fp,"DataType=%d\n",vType);
	switch(vType)
	{
	case GRID_VT_EMPTY	:	// 未指定
	case GRID_VT_NULL	:	// 空（暂保留）
		break;
	case GRID_VT_I2		:	// 2字节带符号整数
		sLineText.Format("%d",iVal);
		break;
	case GRID_VT_I4		:	// 4字节带符号整数
		sLineText.Format("%ld",lVal);
		break;
	case GRID_VT_R4		:	// 4字节浮点数
		sLineText.Format("%f",fltVal);
		break;
	case GRID_VT_R8		:	// 8字节浮点数
		sLineText.Format("%lf",dblVal);
		break;
	case GRID_VT_DATE	:	// 日期
		datetime = date;
		sLineText.Format("%d/%d/%d",datetime.GetYear(),datetime.GetMonth(),datetime.GetDay());
		break;
	case GRID_VT_STRING	:	// 字符串
		if(sVal==NULL)
			sLineText.Empty();
		else
			sLineText.Format("%s",sVal);
		break;
	case GRID_VT_POINTER:	// 指针
	case GRID_VT_BOOL	:	// 布尔数; TRUE=1, FALSE=0
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
	fprintf(fp,"DataValue=%s\n",sLineText);
	fprintf(fp,"--GridDataEnd\n\n");
}

void CGridVariant::Import(FILE *fp)
{
	COleDateTime datetime;
	int nYear=2000,nMonth=1,nDay=1;
	char sLineText[500],*sToken=NULL,separator[]=" -/=,";
	while(feof(fp)==0)
	{
		fgets(sLineText,500,fp);
		CString ss = sLineText;
		ss.Replace("`","\n");
		ss.Replace("``","\r\n");
		sprintf(sLineText,"%s",ss);

		sToken = strtok(sLineText,separator);
		if(sToken&&stricmp(sToken,"DataType")==0)
		{
			int iType;
			sToken = strtok(NULL,separator);
			if(sToken)
			{
				iType = atoi(sToken);
				vType = IntToGridDataType(iType);
				sToken = strtok(NULL,separator);
			}
		}
		else if(sToken&&stricmp(sToken,"DataValue")==0)
		{
			sToken = strtok(NULL,separator);
			if(sToken==NULL)
				continue;
			switch(vType)
			{
			case GRID_VT_EMPTY	:	// 未指定
			case GRID_VT_NULL	:	// 空（暂保留）
				break;
			case GRID_VT_I2		:	// 2字节带符号整数
				iVal = atoi(sToken);
				break;
			case GRID_VT_I4		:	// 4字节带符号整数
				lVal = atoi(sToken);
				break;
			case GRID_VT_R4		:	// 4字节浮点数
				fltVal = (float)atof(sToken);
				break;
			case GRID_VT_R8		:	// 8字节浮点数
				dblVal = atof(sToken);
				break;
			case GRID_VT_DATE	:	// 日期
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
				break;
			case GRID_VT_STRING	:	// 字符串
				SetGridString(sToken);
				break;
			case GRID_VT_POINTER:	// 指针
			case GRID_VT_BOOL	:	// 布尔数; TRUE=1, FALSE=0
				bVal = atoi(sToken);
				break;
			default:
				break;
			}
		}
		else if(sToken&&strstr(sToken,"GridDataEnd")!=NULL)
			break;
	}
}

#ifndef __DRAWING_CONTEXT_
HRESULT CGrid::WriteGridToStream(IStream *pStream)
{
	WORD w;
	DWORD dw;
	w = grid_mode;
	pStream->Write(&w,sizeof(WORD),NULL);
	w = (WORD)m_bHGridVirtual;
	pStream->Write(&w,sizeof(WORD),NULL);
	w = (WORD)m_bVGridVirtual;
	pStream->Write(&w,sizeof(WORD),NULL);
	w = (WORD)feature;
	pStream->Write(&w,sizeof(WORD),NULL);
	pStream->Write(&font_style,sizeof(CFontStyle),NULL);
	pStream->Write(font_style.lfFaceName,32,NULL);
	w = (WORD)data.vType;
	pStream->Write(&w,sizeof(WORD),NULL);
	if(w!=GRID_VT_STRING)
		pStream->Write(&data,sizeof(CGridVariant),NULL);
	else
	{
		if(data.sVal)
		{
			w = (WORD)strlen(data.sVal);
			pStream->Write(&w,sizeof(WORD),NULL);
			pStream->Write(data.sVal,w,NULL);
		}
		else
		{
			w = 0;
			pStream->Write(&w,sizeof(WORD),NULL);
		}
	}
	w = (WORD)mark.bTitle;
	pStream->Write(&w,sizeof(WORD),NULL);
	dw = (DWORD)mark.type_id;
	pStream->Write(&dw,sizeof(DWORD),NULL);
	dw = (DWORD)mark.ext_feature;
	pStream->Write(&dw,sizeof(DWORD),NULL);
	return S_OK;
}

HRESULT CGrid::ReadGridFromStream(IStream *pStream)
{
	WORD w;
	DWORD dw;
	pStream->Read(&w,sizeof(WORD),NULL);
	if(w==STRING_GRID)
		grid_mode = STRING_GRID;
	else if(w==GRAPH_BLOCK_GRID)
		grid_mode = GRAPH_BLOCK_GRID;
	else if(w==PIC_BLOCK_GRID)
		grid_mode = PIC_BLOCK_GRID;
	pStream->Read(&w,sizeof(WORD),NULL);
	m_bHGridVirtual = w;
	pStream->Read(&w,sizeof(WORD),NULL);
	m_bVGridVirtual = w;
	pStream->Read(&w,sizeof(WORD),NULL);
	feature = w;
	pStream->Read(&font_style,sizeof(CFontStyle),NULL);
	font_style.lfFaceName = NULL;
	char face_name[33]={'\0'};
	pStream->Read(face_name,32,NULL);
	font_style.SetFaceName(face_name);
	w = (WORD)data.vType;
	pStream->Read(&w,sizeof(WORD),NULL);
	data.vType = IntToGridDataType(w);
	if(w!=GRID_VT_STRING)
		pStream->Read(&data,sizeof(CGridVariant),NULL);
	else
	{
		pStream->Read(&w,sizeof(WORD),NULL);
		if(w>0)
		{
			char *pStr = new char[w+1];
			pStr[w]='\0';
			pStream->Read(pStr,w,NULL);
			data.SetGridString(pStr);
			delete []pStr;
		}
	}
	pStream->Read(&w,sizeof(WORD),NULL);
	mark.bTitle = w;
	pStream->Read(&dw,sizeof(DWORD),NULL);
	mark.type_id = dw;
	pStream->Read(&dw,sizeof(DWORD),NULL);
	mark.ext_feature = dw;
	return S_OK;
}
#endif