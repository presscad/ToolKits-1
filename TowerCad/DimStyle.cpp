#include "StdAfx.h"
#include "DimStyle.h"

//////////////////////////////////////////////////////////////////////////
//
TMADRAWING_TEXT_STYLE TextStyleTable::hzfs;
void TextStyle(AcDbTextStyleTableRecord *pTextStyleTblRec, char *style_name)
{
#ifdef _ARX_2007
	//设置字体样式表记录名
	pTextStyleTblRec->setName((ACHAR*)_bstr_t(style_name));
	pTextStyleTblRec->setFileName(L"simplex");
#else
	//设置字体样式表记录名
	pTextStyleTblRec->setName(style_name);
	pTextStyleTblRec->setFileName("simplex");
	//设置大字体文件名
#ifndef _MAPTMA_GRX
	pTextStyleTblRec->setBigFontFileName("gbhzfs.shx");
#else
	pTextStyleTblRec->setBigFontFileName("CHINA.shx");
#endif
#endif
	//设置字体的效果：宽度比例
	pTextStyleTblRec->setXScale(0.7);
}
void GetTextStyleId(AcDbObjectId &TextStyleId, char *textStyle/*=NULL*/, bool init_style/*=false*/)
{
	char style[100];
	AcDbTextStyleTable *pTextStyleTbl;//定义字体样式表指针
	AcDbTextStyleTableRecord *pTextStyleTblRec;//定义字体样式记录指针
	//以写方式打开字体样式表，获得指明向字体样式表的指针
	acdbHostApplicationServices()->workingDatabase()->getTextStyleTable(pTextStyleTbl, AcDb::kForWrite);
	//创建字体样式表记录
	//如果字体样式表中还没有指定样式,则创建新字体样式
	//并返回样式ID,如果已有指定样式,则返回已有样式的ID
	if (textStyle == NULL)
		strcpy(style, "Standard");
	else
		strcpy(style, textStyle);
#ifdef _ARX_2007
	if (pTextStyleTbl->getAt((ACHAR*)_bstr_t(style), TextStyleId) != Acad::eOk)
	{
		Acad::ErrorStatus rescode;
		pTextStyleTblRec = new AcDbTextStyleTableRecord;
		TextStyle(pTextStyleTblRec, _bstr_t(style));
		pTextStyleTblRec->setTextSize(0);		//设置字体大小
		rescode = pTextStyleTbl->add(TextStyleId, pTextStyleTblRec);//向字体样式表中写入字体样式表记录
		pTextStyleTbl->close();		//关闭字体样式表记录
		pTextStyleTblRec->close();	//关闭字体样式表
		if (rescode == Acad::eOutOfMemory)
			AfxMessageBox("内存溢出");
		else if (rescode == Acad::eDuplicateRecordName)
			AfxMessageBox("格式重名");
		else if (rescode == Acad::eNoDatabase)
			AfxMessageBox("没有数据库");
	}
	else
	{
		pTextStyleTbl->close();
		acdbOpenObject(pTextStyleTblRec, TextStyleId, AcDb::kForWrite);
		if (init_style)
			TextStyle(pTextStyleTblRec, _bstr_t(style));
		pTextStyleTblRec->setTextSize(0);
		//关闭字体样式表
		pTextStyleTblRec->close();
	}
#else
	if (pTextStyleTbl->getAt(style, TextStyleId) != Acad::eOk)
	{
		Acad::ErrorStatus rescode;
		pTextStyleTblRec = new AcDbTextStyleTableRecord;
		TextStyle(pTextStyleTblRec, style);
		pTextStyleTblRec->setTextSize(0);		//设置字体大小
		rescode = pTextStyleTbl->add(TextStyleId, pTextStyleTblRec);	//向字体样式表中写入字体样式表记录
		pTextStyleTbl->close();		//关闭字体样式表记录
		pTextStyleTblRec->close();	//关闭字体样式表
		if (rescode == Acad::eOutOfMemory)
			AfxMessageBox("内存溢出");
		else if (rescode == Acad::eDuplicateRecordName)
			AfxMessageBox("格式重名");
		else if (rescode == Acad::eNoDatabase)
			AfxMessageBox("没有数据库");
	}
	else
	{
		pTextStyleTbl->close();
		acdbOpenObject(pTextStyleTblRec, TextStyleId, AcDb::kForWrite);
		if (init_style)
			TextStyle(pTextStyleTblRec, style);
		pTextStyleTblRec->setTextSize(0);
		//关闭字体样式表
		pTextStyleTblRec->close();
	}
#endif
	acdbHostApplicationServices()->workingDatabase()->setTextstyle(TextStyleId);
}
void TextStyleTable::InitTextStyleTable()
{
	TextStyleTable::hzfs.bigFontName.Copy("gbhzfs.shx");
	TextStyleTable::hzfs.textStyleName.Copy("standard");
	GetTextStyleId(TextStyleTable::hzfs.textStyleId, "standard", true);	//初始化文本样式
}

//////////////////////////////////////////////////////////////////////////
//
TMADRAWING_DIM_STYLE DimStyleTable::dimStyle;
TMADRAWING_DIM_STYLE DimStyleTable::dimStyle2;
TMADRAWING_DIM_STYLE DimStyleTable::dimStyle3;
TMADRAWING_DIM_STYLE DimStyleTable::dimStyle4;		//文字放在尺寸线外侧 wht 15-04-28
TMADRAWING_DIM_STYLE DimStyleTable::dimAngleStyle;
TMADRAWING_DIM_STYLE DimStyleTable::dimRadiusStyle;
//
double TMADRAWING_DIM_STYLE::s_fDimArrowSize=1.5;
double TMADRAWING_DIM_STYLE::s_fDimTextSize=2.5;
double TMADRAWING_DIM_STYLE::s_fDimGap=0.75;
char TMADRAWING_DIM_STYLE::s_sArrowType[50] = "_OBLIQUE";
double TMADRAWING_DIM_STYLE::s_fDimAngleTextSize = 2;
TMADRAWING_DIM_STYLE::TMADRAWING_DIM_STYLE()
{
	arrowSize = gap = 0;
	m_fInnerSetTextSize = 0;
}
void TMADRAWING_DIM_STYLE::InitDimStyle(double text_height/*=0*/, double arrow_size/*=0*/, double text_gap/*=0*/)
{
	//以写方式打开尺寸样式表
	AcDbDimStyleTableRecord *pDimStyleTblRec = NULL;
	acdbOpenObject(pDimStyleTblRec, dimStyleId, AcDb::kForWrite);
	if (pDimStyleTblRec)
	{
		if (arrow_size == 0)
			arrowSize = s_fDimArrowSize;
		else
			arrowSize = arrow_size;
		m_fInnerSetTextSize = text_height;
		if (m_fInnerSetTextSize == 0)
			m_fInnerSetTextSize = s_fDimTextSize;
		if (text_gap == 0)
			gap = 0.25*m_fInnerSetTextSize;
		else
			gap = text_gap;
		pDimStyleTblRec->setDimasz(arrowSize);	//箭头长度为
		pDimStyleTblRec->setDimtxt(m_fInnerSetTextSize);	//尺寸文字高度为
		pDimStyleTblRec->setDimexe(0.5*m_fInnerSetTextSize);	//尺寸界线在标注上方的延伸量(尺寸界线范围)应设置为标注字高的一半 wht 10-01-10
		pDimStyleTblRec->setDimgap(gap);	//标注线与文字间距应为0.5尺寸线延伸量	wht 10-01-10
		pDimStyleTblRec->close();
	}
}
double TMADRAWING_DIM_STYLE::get_Gap() { return gap; }
double TMADRAWING_DIM_STYLE::set_Gap(double dimgap) { return gap = dimgap; }
double TMADRAWING_DIM_STYLE::CalGap(double text_height)
{
	return max(s_fDimGap, 0.25*text_height);
}

//////////////////////////////////////////////////////////////////////////
//
void DimStyleTable::InitDimStyle()
{
	DimStyleTable::dimStyle = DimStyleTable::GetDimStyle("standard", TMADRAWING_DIM_STYLE::s_fDimTextSize);			//默认标注样式将标注尺寸放置在尺寸线之间
	DimStyleTable::dimStyle2 = DimStyleTable::GetDimStyle2("dimStyle2", TMADRAWING_DIM_STYLE::s_fDimTextSize);		//用于绘制角钢简图等小图
	DimStyleTable::dimStyle3 = DimStyleTable::GetDimStyle("dimStyle3", TMADRAWING_DIM_STYLE::s_fDimTextSize, FALSE);//不将尺寸标注强制放在尺寸线之间，主要用于标注角钢特殊心 wht 11-07-12	
	DimStyleTable::dimStyle4 = DimStyleTable::GetDimStyle("dimStyle4", TMADRAWING_DIM_STYLE::s_fDimTextSize, TRUE, 2);//文字放在尺寸线外侧 wht 15-04-28
	DimStyleTable::dimAngleStyle = DimStyleTable::GetDimAngleStyle("dimAngleStyle", TMADRAWING_DIM_STYLE::s_fDimAngleTextSize);
	DimStyleTable::dimRadiusStyle = DimStyleTable::GetDimRadiusStyle("dimRadiusStyle", TMADRAWING_DIM_STYLE::s_fDimTextSize);
}


/*DIMBLK系统变量
""				实心闭合
"_DOT"			点
"_DOTSMALL"		小点
"_DOTBLANK"		空心点
"_ORIGIN"		原点标记
"_ORIGIN2"		原点标记 2
"_OPEN"			打开
"_OPEN90"		直角
"_OPEN30"		30 度角
"_CLOSED"		闭合
"_SMALL"		空心小点
"_NONE"			无
"_OBLIQUE"		倾斜
"_BOXFILLED"	实心框
"_BOXBLANK"		框
"_CLOSEDBLANK"	空心闭合
"_DATUMFILLED"	实心基准三角形
"_DATUMBLANK"	基准三角形
"_INTEGRAL"		完整标记
"_ARCHTICK"		建筑标记
*/
//设置尺寸变量函数定义
void setDimVar(AcDbDimStyleTableRecord *pDimStyleTblRec, double txt_height/*=2.5*/,
	BOOL bInside/*=TRUE*/, BYTE cVerticalPosType/*=1*/)
{
	pDimStyleTblRec->setDimasz(TMADRAWING_DIM_STYLE::s_fDimArrowSize);	//箭头长度为
#ifdef _ARX_2007
	//pDimStyleTblRec ->setDimblk(L"_ARCHTICK");	//箭头端形式"/"
	//pDimStyleTblRec ->setDimblk1(L"_ARCHTICK");	//箭头端形式"/"
	//pDimStyleTblRec ->setDimblk2(L"_ARCHTICK");	//箭头端形式"/"
	//用户可设置箭头类型 wht 10-08-02
	pDimStyleTblRec->setDimblk((ACHAR*)_bstr_t(TMADRAWING_DIM_STYLE::s_sArrowType));	//箭头端形式"/"
	pDimStyleTblRec->setDimblk1((ACHAR*)_bstr_t(TMADRAWING_DIM_STYLE::s_sArrowType));	//箭头端形式"/"
	pDimStyleTblRec->setDimblk2((ACHAR*)_bstr_t(TMADRAWING_DIM_STYLE::s_sArrowType));	//箭头端形式"/"
#else
	//pDimStyleTblRec ->setDimblk("_ARCHTICK");	//箭头端形式"/"
	//pDimStyleTblRec ->setDimblk1("_ARCHTICK");	//箭头端形式"/"
	//pDimStyleTblRec ->setDimblk2("_ARCHTICK");	//箭头端形式"/"
	pDimStyleTblRec->setDimblk(TMADRAWING_DIM_STYLE::s_sArrowType);	//箭头端形式"/"
	pDimStyleTblRec->setDimblk1(TMADRAWING_DIM_STYLE::s_sArrowType);	//箭头端形式"/"
	pDimStyleTblRec->setDimblk2(TMADRAWING_DIM_STYLE::s_sArrowType);	//箭头端形式"/"
#endif
	//pDimStyleTblRec ->setDimexe(2);	//尺寸界线在标注上方的延伸量
	//pDimStyleTblRec ->setDimgap(1);	//标注线与文字间距为1
	pDimStyleTblRec->setDimtmove(2);	//0.尺寸线随文字移动;1.移动文字时添加引线;2.移动文字时不添加引线
	pDimStyleTblRec->setDimexe(0.5*txt_height);	//尺寸界线在标注上方的延伸量应设置为标注字高的一半 wht 10-01-10
	double fDimGap = max(TMADRAWING_DIM_STYLE::s_fDimGap, 0.25*txt_height); //wht 10-08-02
	pDimStyleTblRec->setDimgap(fDimGap);			//标注线与文字间距应为0.5尺寸线延伸量	wht 10-01-10
	pDimStyleTblRec->setDimexo(0);	//尺寸界线起点偏移量为0
	pDimStyleTblRec->setDimdli(6);	//标注线间距为6
	pDimStyleTblRec->setDimtih(0);	//尺寸界线内侧的的文字水平放置
	pDimStyleTblRec->setDimtoh(0);	//外侧文字水平放置
	pDimStyleTblRec->setDimtxt(txt_height);	//尺寸文字高度为
	pDimStyleTblRec->setDimtxsty(TextStyleTable::hzfs.textStyleId);	//设定字体样式
	pDimStyleTblRec->setDimtad(cVerticalPosType);	//0.居中 1.文字位于标注线上方 2.外部 3.JIS 4.下方
	pDimStyleTblRec->setDimdec(2);	//标注小数位数
	pDimStyleTblRec->setDimtdec(1);//标注公差位数
	if (bInside)	//根据传入参数设置是否将尺寸标注放置在尺寸线之间 wht 11-07-12
		pDimStyleTblRec->setDimtix(Adesk::kTrue);	//强制将尺寸放于尺寸界线内侧
	else
		pDimStyleTblRec->setDimtix(Adesk::kFalse);	//不强制将尺寸放于尺寸界线内侧
	pDimStyleTblRec->setDimtofl(1);//强制在尺寸界限内画标注线
}
void setDimVar2(AcDbDimStyleTableRecord *pDimStyleTblRec, double txt_height)
{
	pDimStyleTblRec->setDimasz(TMADRAWING_DIM_STYLE::s_fDimArrowSize);	//箭头长度为
#ifdef _ARX_2007
	//pDimStyleTblRec ->setDimblk(L"_ARCHTICK");	//箭头端形式"/"
	//pDimStyleTblRec ->setDimblk1(L"_ARCHTICK");	//箭头端形式"/"
	//pDimStyleTblRec ->setDimblk2(L"_ARCHTICK");	//箭头端形式"/"
	//用户可设置箭头类型 wht 10-08-02
	pDimStyleTblRec->setDimblk((ACHAR*)_bstr_t(TMADRAWING_DIM_STYLE::s_sArrowType));	//箭头端形式"/"
	pDimStyleTblRec->setDimblk1((ACHAR*)_bstr_t(TMADRAWING_DIM_STYLE::s_sArrowType));	//箭头端形式"/"
	pDimStyleTblRec->setDimblk2((ACHAR*)_bstr_t(TMADRAWING_DIM_STYLE::s_sArrowType));	//箭头端形式"/"
#else
	//pDimStyleTblRec ->setDimblk("_ARCHTICK");	//箭头端形式"/"
	//pDimStyleTblRec ->setDimblk1("_ARCHTICK");	//箭头端形式"/"
	//pDimStyleTblRec ->setDimblk2("_ARCHTICK");	//箭头端形式"/"
	pDimStyleTblRec->setDimblk(TMADRAWING_DIM_STYLE::s_sArrowType);	//箭头端形式"/"
	pDimStyleTblRec->setDimblk1(TMADRAWING_DIM_STYLE::s_sArrowType);	//箭头端形式"/"
	pDimStyleTblRec->setDimblk2(TMADRAWING_DIM_STYLE::s_sArrowType);	//箭头端形式"/"
#endif
	pDimStyleTblRec->setDimtmove(2);	//0.尺寸线随文字移动;1.移动文字时添加引线;2.移动文字时不添加引线
	pDimStyleTblRec->setDimexe(0);	//尺寸界线在标注上方的延伸量
	pDimStyleTblRec->setDimexo(0);	//尺寸界线起点偏移量为0
	pDimStyleTblRec->setDimdli(2);	//标注线间距为6
	pDimStyleTblRec->setDimtad(1);	//文字位于标注线上方
	pDimStyleTblRec->setDimtih(0);	//尺寸界线内侧的的文字水平放置
	pDimStyleTblRec->setDimtoh(0);	//外侧文字水平放置
	pDimStyleTblRec->setDimtxt(txt_height);	//尺寸文字高度为
	pDimStyleTblRec->setDimtxsty(TextStyleTable::hzfs.textStyleId);	//设定字体样式
	pDimStyleTblRec->setDimgap(1);	//标注线与文字间距为1
	pDimStyleTblRec->setDimdec(2);	//标注小数位数
	pDimStyleTblRec->setDimtdec(1);//标注公差位数
	pDimStyleTblRec->setDimtix(Adesk::kTrue);	//强制将尺寸放于尺寸界线内侧
	pDimStyleTblRec->setDimtofl(1);//强制在尺寸界限内画标注线
}
//设置尺寸变量函数定义
void setDimAngleVar(AcDbDimStyleTableRecord *pDimStyleTblRec, double txt_height)
{
	pDimStyleTblRec->setDimasz(TMADRAWING_DIM_STYLE::s_fDimArrowSize);	//箭头长度为
#ifdef _ARX_2007
	pDimStyleTblRec->setDimblk(L"");	//箭头端形式"/"_OPEN30
	pDimStyleTblRec->setDimblk1(L"");	//箭头端形式"/"
	pDimStyleTblRec->setDimblk2(L"");	//箭头端形式"/"
#else
	pDimStyleTblRec->setDimblk("");	//箭头端形式"/"
	pDimStyleTblRec->setDimblk1("");	//箭头端形式"/"
	pDimStyleTblRec->setDimblk2("");	//箭头端形式"/"
#endif
	//pDimStyleTblRec ->setDimexe(2);	//尺寸界线在标注上方的延伸量
	//pDimStyleTblRec ->setDimgap(1);	//标注线与文字间距为1
	pDimStyleTblRec->setDimexe(0.5*txt_height);	//尺寸界线在标注上方的延伸量应设置为标注字高的一半 wht 10-01-10
	pDimStyleTblRec->setDimgap(0.25*txt_height);	//标注线与文字间距应为0.5尺寸线延伸量	wht 10-01-10
	pDimStyleTblRec->setDimexo(0);	//尺寸界线起点偏移量为0
	pDimStyleTblRec->setDimdli(6);	//标注线间距为6
	pDimStyleTblRec->setDimtad(2);	//文字位于标注线上方0:置中;1:上方;2:外部
	pDimStyleTblRec->setDimtih(0);	//尺寸界线内侧的的文字水平放置
	pDimStyleTblRec->setDimtoh(0);	//外侧文字水平放置
	pDimStyleTblRec->setDimtxt(txt_height);	//尺寸文字高度为
	pDimStyleTblRec->setDimtxsty(TextStyleTable::hzfs.textStyleId);	//设定字体样式
	pDimStyleTblRec->setDimadec(1);	//标注小数位数
	pDimStyleTblRec->setDimtdec(1);//标注公差位数
	pDimStyleTblRec->setDimtix(Adesk::kTrue);	//强制将尺寸放于尺寸界线内侧
	pDimStyleTblRec->setDimtofl(1);//强制在尺寸界限内画标注线
}
TMADRAWING_DIM_STYLE DimStyleTable::GetDimStyle(char *dimstyle/*=NULL*/, double txt_height/*=2.0*/, BOOL bInside/*=TRUE*/, BYTE cVerticalPosType/*=1*/)
{
	TMADRAWING_DIM_STYLE dimStyle;
	dimStyle.dimStyleName.Copy(dimstyle);
	dimStyle.arrowSize = TMADRAWING_DIM_STYLE::s_fDimArrowSize;
	dimStyle.Gap = 0.25*txt_height;
	AcDbDimStyleTable *pDimStyleTbl;//定义尺寸标注指针
	AcDbDimStyleTableRecord *pDimStyleTblRec;
	if (dimstyle == NULL)
		dimStyle.dimStyleName.Copy("TOWERBZ");
	else
		dimStyle.dimStyleName.Copy(dimstyle);
	//以写方式打开尺寸样式表
	acdbHostApplicationServices()->workingDatabase()->getDimStyleTable(pDimStyleTbl, AcDb::kForWrite);
#ifdef _ARX_2007
	if (pDimStyleTbl->getAt((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName), dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName((ACHAR*)_bstr_t(dimStyle.dimStyleName));
		//向尺寸标注样式表中写入尺寸标注样式标记录
		pDimStyleTbl->add(dimStyle.dimStyleId, pDimStyleTblRec);
#else
	if (pDimStyleTbl->getAt((char*)dimStyle.dimStyleName, dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName(dimStyle.dimStyleName);
		//向尺寸标注样式表中写入尺寸标注样式标记录
		pDimStyleTbl->add(dimStyle.dimStyleId, pDimStyleTblRec);
#endif
	}
	else
		acdbOpenObject(pDimStyleTblRec, dimStyle.dimStyleId, AcDb::kForWrite);
	pDimStyleTbl->close();
	setDimVar(pDimStyleTblRec, txt_height, bInside, cVerticalPosType);
	pDimStyleTblRec->close();
	return dimStyle;
	}
TMADRAWING_DIM_STYLE DimStyleTable::GetDimStyle2(char *dimstyle/*=NULL*/, double txt_height/*=2.0*/)
{
	TMADRAWING_DIM_STYLE dimStyle;
	dimStyle.arrowSize = TMADRAWING_DIM_STYLE::s_fDimArrowSize;
	dimStyle.Gap = 0.25*txt_height;
	if (dimstyle == NULL)
		dimStyle.dimStyleName.Copy("TOWERBZ2");
	else
		dimStyle.dimStyleName.Copy(dimstyle);
	//以写方式打开尺寸样式表
	AcDbDimStyleTable *pDimStyleTbl;//定义尺寸标注指针
	AcDbDimStyleTableRecord *pDimStyleTblRec;
	acdbHostApplicationServices()->workingDatabase()->getDimStyleTable(pDimStyleTbl, AcDb::kForWrite);
#ifdef _ARX_2007
	if (pDimStyleTbl->getAt((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName), dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName((ACHAR*)_bstr_t(dimStyle.dimStyleName));
#else
	if (pDimStyleTbl->getAt((char*)dimStyle.dimStyleName, dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName(dimStyle.dimStyleName);
		//向尺寸标注样式表中写入尺寸标注样式标记录
		pDimStyleTbl->add(dimStyle.dimStyleId, pDimStyleTblRec);
#endif
	}
	else
		acdbOpenObject(pDimStyleTblRec, dimStyle.dimStyleId, AcDb::kForWrite);
	setDimVar2(pDimStyleTblRec, txt_height);
	pDimStyleTbl->close();
	pDimStyleTblRec->close();
	return dimStyle;
}
TMADRAWING_DIM_STYLE DimStyleTable::GetDimAngleStyle(char *dimstyle/*=NULL*/, double txt_height/*=2.0*/)
{
	TMADRAWING_DIM_STYLE dimStyle;
	dimStyle.arrowSize = TMADRAWING_DIM_STYLE::s_fDimArrowSize;
	dimStyle.Gap = 0.25*txt_height;
	if (dimstyle == NULL)
		dimStyle.dimStyleName.Copy("TOWERBZ3");
	else
		dimStyle.dimStyleName.Copy(dimstyle);
	//以写方式打开尺寸样式表
	AcDbDimStyleTable *pDimStyleTbl;//定义尺寸标注指针
	AcDbDimStyleTableRecord *pDimStyleTblRec;
	acdbHostApplicationServices()->workingDatabase()->getDimStyleTable(pDimStyleTbl, AcDb::kForWrite);
#ifdef _ARX_2007
	if (pDimStyleTbl->getAt((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName), dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName));
#else
	if (pDimStyleTbl->getAt(dimStyle.dimStyleName, dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName(dimStyle.dimStyleName);
		//向尺寸标注样式表中写入尺寸标注样式标记录
		pDimStyleTbl->add(dimStyle.dimStyleId, pDimStyleTblRec);
#endif
	}
	else
		acdbOpenObject(pDimStyleTblRec, dimStyle.dimStyleId, AcDb::kForWrite);
	setDimAngleVar(pDimStyleTblRec, txt_height);
	//pDimStyleTblRec->setDimfit(1);	//设置半径标注方式
	pDimStyleTbl->close();
	pDimStyleTblRec->close();
	return dimStyle;
}

TMADRAWING_DIM_STYLE DimStyleTable::GetDimRadiusStyle(char *dimstyle/*=NULL*/, double txt_height/*=2.0*/)
{
	TMADRAWING_DIM_STYLE dimStyle;
	dimStyle.arrowSize = TMADRAWING_DIM_STYLE::s_fDimArrowSize;
	dimStyle.Gap = 0.25*txt_height;
	if (dimstyle == NULL)
		dimStyle.dimStyleName.Copy("TOWERBZ4");
	else
		dimStyle.dimStyleName.Copy(dimstyle);
	//以写方式打开尺寸样式表
	AcDbDimStyleTable *pDimStyleTbl;//定义尺寸标注指针
	AcDbDimStyleTableRecord *pDimStyleTblRec;
	acdbHostApplicationServices()->workingDatabase()->getDimStyleTable(pDimStyleTbl, AcDb::kForWrite);
#ifdef _ARX_2007
	if (pDimStyleTbl->getAt((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName), dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName((ACHAR*)_bstr_t((char*)dimStyle.dimStyleName));
#else
	if (pDimStyleTbl->getAt(dimStyle.dimStyleName, dimStyle.dimStyleId) != Acad::eOk)
	{
		pDimStyleTblRec = new AcDbDimStyleTableRecord;
		pDimStyleTblRec->setName(dimStyle.dimStyleName);
		//向尺寸标注样式表中写入尺寸标注样式标记录
		pDimStyleTbl->add(dimStyle.dimStyleId, pDimStyleTblRec);
#endif
	}
	else
		acdbOpenObject(pDimStyleTblRec, dimStyle.dimStyleId, AcDb::kForWrite);
	setDimAngleVar(pDimStyleTblRec, txt_height);
	pDimStyleTbl->close();
	pDimStyleTblRec->close();
	return dimStyle;
}