#pragma once
#include "XhCharString.h"
#ifndef	___DIM_STYLE_H_
#define ___DIM_STYLE_H_

typedef struct tagTMADRAWING_TEXT_STYLE
{
	CXhChar16 textStyleName;	//文字样式名称
	CXhChar16 bigFontName;		//汉字字体文件
	AcDbObjectId textStyleId;	//文字样式
}TMADRAWING_TEXT_STYLE;
class TextStyleTable
{
public:
	static TMADRAWING_TEXT_STYLE hzfs;
	static void InitTextStyleTable();
};
//字体设置
void TextStyle(AcDbTextStyleTableRecord *pTextStyleTblRec);
void GetTextStyleId(AcDbObjectId &TextStyleId, char *textStyle = NULL, bool init_style = false);
double DrawTextLength(const char* dimtext, double height, AcDbObjectId textStyleId);

//
struct TMADRAWING_DIM_STYLE
{
	CXhChar16 dimStyleName;	//文字样式名称
	double arrowSize;		//箭头大小
	AcDbObjectId dimStyleId;	//文字样式
	TMADRAWING_DIM_STYLE();
	void InitDimStyle(double textHeight = 0, double arrowSize = 0, double gap = 0);
	double get_Gap();
	double set_Gap(double dimgap);
	__declspec(property(get = get_Gap, put = set_Gap)) double Gap;
	static double CalGap(double text_height);
protected:
	double m_fInnerSetTextSize;
	double gap;				//文字与基准线间隙

public:
	static double s_fDimAngleTextSize;
	static double s_fDimArrowSize;
	static double s_fDimTextSize;
	static double s_fDimGap;
	static char s_sArrowType[50];
};

class DimStyleTable
{
public:
	static TMADRAWING_DIM_STYLE dimStyle;
	static TMADRAWING_DIM_STYLE dimStyle2;
	static TMADRAWING_DIM_STYLE dimStyle3;	//不强制将尺寸放于尺寸界线内侧
	static TMADRAWING_DIM_STYLE dimStyle4;	//文字放在尺寸线外侧 wht 15-04-28
	static TMADRAWING_DIM_STYLE dimAngleStyle;
	static TMADRAWING_DIM_STYLE dimRadiusStyle;

	static void InitDimStyle();
	static TMADRAWING_DIM_STYLE GetDimStyle(char *dimstyle = NULL, double txt_height = 2.0, BOOL bInside = TRUE, BYTE cVerticalPosType = 1);
	static TMADRAWING_DIM_STYLE GetDimStyle2(char *dimstyle = NULL, double txt_height = 2.0);
	static TMADRAWING_DIM_STYLE GetDimAngleStyle(char *dimstyle = NULL, double txt_height = 2.0);
	static TMADRAWING_DIM_STYLE GetDimRadiusStyle(char *dimstyle = NULL, double txt_height = 2.0);
};

#endif
