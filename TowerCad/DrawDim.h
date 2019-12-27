#pragma once
//#include "stdafx.h"
#include "f_ent.h"
#include "f_ent_list.h"
#include "f_alg_fun.h"
#include "XhCharString.h"
#include "HashTable.h"
#include "ArrayList.h"
#ifdef __DRAWING_CONTEXT_
#include "../LDS/MapLds/RxTools.h"
#endif

enum DIMALIGN_TYPE{AlignDefault=1,TopLeft,TopCenter,TopRight,MiddleLeft,MiddleCenter,MiddleRight,BottomLeft,BottomCenter,BottomRight};

class CDimStyle{
public:
#ifdef __DRAWING_CONTEXT_
	TMADRAWING_DIM_STYLE style;
#else
	struct DIM_STYLE
	{
	protected:
		double gap;				//文字与基准线间隙
		double m_fInnerSetTextSize;
	public:
		CXhChar16 dimStyleName;	//文字样式名称
		double arrowSize;		//箭头大小
		//AcDbObjectId dimStyleId;	//文字样式
	public:
		DIM_STYLE(){m_fInnerSetTextSize=arrowSize=gap=0;}
		void InitDimStyle(double textHeight=0,double arrowSize=0,double gap=0);
		double get_Gap(){return gap;}
		double set_Gap(double dimgap){return gap=dimgap;}
		__declspec(property(get=get_Gap,put=set_Gap)) double Gap;
		//static double CalGap(double text_height);
	} style;
#endif
	double textHeight;
	CDimStyle(){textHeight=2.0;}
};
struct SAFE_TEXTSIZE{
	double textsize;	//=0表示遵照系统环境默认大小
	double fontscale;	//文字高度缩放系数
	static double minFontSize;	//文字最小高度
	SAFE_TEXTSIZE(){
		textsize=0;
		fontscale=1.0;
	}
	double EnsureSize(double textheight=0){
		double th=textheight>0?textheight:textsize;
		th*=fontscale;
		return th<minFontSize?minFontSize:th;
	}
};
//角度标注
class CSizeAngleDim
{
public:
	static CDimStyle dimStyle;
	f3dPoint vertex;		//角度标注顶点
	f3dPoint dimStartPos;	//角度标注起始点
	f3dPoint dimEndPos;		//角度标注终止点
	f3dPoint dimPos;		//角度标注文字位置
	CXhChar16 dimText;		//某些情况需要指定标注内容
	BYTE cFlag;				//角度标注缩放状态标注
public:
	CSizeAngleDim();
	void Reset();
	double GetWidth();
	double GetHeight();
	double GetDimAngle();
	//返回标注矩形区(不含标注箭头)左下rgnVertArr[0];右下rgnVertArr[1];右上rgnVertArr[2];左上四个角点rgnVertArr[3]
	void GetDimTextRect(f3dPoint rgnVertArr[4],double draw_scale=1.0,double text_size=0) const; //可根据比例得到矩形框大小，默认为1 wht 10-09-16
	void GetDimRect(f3dPoint rgnVertArr[4],double draw_scale=1.0) const;
	void GetDimTextRect(f3dPoint rgnVertArr[4],SAFE_TEXTSIZE &safeTextSize) const;
	f2dArc GetDimArcLine();
};
//带标注线的文本尺寸标注
class CSizeTextDim
{
	CXhChar50 dimText;
	DIMALIGN_TYPE alignType;
public:
	static double PRECISION;		//生成文字距离标注时的精度,只取1.0,0.5,0.1三种值
	static CDimStyle dimStyle;
	f3dPoint origin;		//对齐定位基点位置
	f3dPoint dimStartPos;	//标注线起始点(相对于对齐定位基点)
	f3dPoint dimEndPos;		//标注线终止点(相对于对齐定位基点)
	f3dPoint dimPos;		//标注文字位置(相对于对齐定位基点)
	f3dPoint initDimPos;	//记录初始标注位置,每次调整标注位置时以initDimPos为基准 wht 18-09-17
	double angle;			//弧度
	double dist;			//标注距离
	int nHits;				//连续的等距为S的标注，可以合并为一个S X nHits的标注
	//0.装配检测尺寸 1.局部偏移尺寸 2.脚钉间距 
	//3(-3).构件定位尺寸 (+-表上下) 4.外端面到外端面距离标注
	int iOrder;				//分层标注时用来确定尺寸定位顺序
	int iTextDimSylte;		//文字标注样式 0.标注线上册 1.标注线下侧
	BYTE cFlag;				//文字标注缩放状态标记
public:
	BOOL bCanTextOutter;	//支持文字在尺寸线外侧标注 wht 15-04-27
	BOOL bAllowRelayout;	//是否允许自动调整标注位置,某些特殊情况不用自动调整位置 wht 10-08-15
	BOOL bShortenDimStartLine,bShortenDimEndLine;
	CSizeTextDim();
	CSizeTextDim(const double* dimStartCoords,const double* dimEndCoords,const double* dimPosCoords,
		double fDist,double angle,BOOL bAllowRelayout=FALSE,BOOL bCanTextOutter=FALSE);
	//precision只能为"0","0.5","0.0"
	double Fillet();	//将标注距离dist按照precision进行圆整化
	const char* DimText();
	double GetWidth();
	double GetHeight(BOOL bIncShortDimLineOnly=FALSE);
	//可根据比例得到矩形框大小，默认为1 wht 10-09-16;
	//返回标注矩形区(不含标注箭头)左下rgnVertArr[0];右下rgnVertArr[1];右上rgnVertArr[2];左上四个角点rgnVertArr[3]
	void GetDimRect(f3dPoint rgnVertArr[4],BOOL bIncShortDimLineOnly=FALSE,double draw_scale=1.0,double text_height=0) const;
	void GetDimRect(f3dPoint rgnVertArr[4],BOOL bIncShortDimLineOnly,SAFE_TEXTSIZE& safetextsize) const;
	DIMALIGN_TYPE AlignType(){return alignType;}
	void ResetPosition(DIMALIGN_TYPE align=AlignDefault,bool bAccumulation=false);
	void ResetPosition2(DIMALIGN_TYPE align=AlignDefault);
};

class CTextOnlyDrawing{
public:
	static double PRECISION;
	DIMALIGN_TYPE alignType;
	CXhChar50 dimText;		//标注的文本内容
	f3dPoint origin;		//对齐定位基点位置
	double textHeight;
	f3dPoint dimPos;		//标注文字位置(相对于对齐定位基点)
	bool bNeedConvertDistToText;	//需要把dist在GetPrecisionDimText()中变为标注字符串
	double angle;			//弧度
	double dist;			//一般是与dimText相对应的双精度数（不是必须）
	BOOL bInXWing;
	//0.焊接方向文字 1.绝对尺寸 +-2(表上下).视图名称  3(-3).剖视图符 +表示沿X轴正方向 -表示沿X轴负方向  4.特殊标记文字
	int iOrder;				//分层标注时用来确定尺寸定位顺序
	BYTE cFlag;				//文本标注缩放状态标记
	BYTE m_nResetHits;
public:
	CTextOnlyDrawing(){iOrder=0;angle=dist=0;textHeight=2.0;alignType=MiddleLeft;bNeedConvertDistToText=true;cFlag=0;m_nResetHits=0;}
	double Fillet();
	//precision只能为"0","0.5","0.0"
	const char* GetPrecisionDimText();
	//返回标注矩形区(不含标注箭头)左下rgnVertArr[0];右下rgnVertArr[1];右上rgnVertArr[2];左上四个角点rgnVertArr[3]
	void GetDimRect(f3dPoint rgnVertArr[4],double draw_scale=1.0,double textsize=0); //可根据比例得到矩形框大小，默认为1 wht 10-09-16
	void GetDimRect(f3dPoint rgnVertArr[4],SAFE_TEXTSIZE& safetextsize);
	double GetHeight();
	double GetWidth();
	DIMALIGN_TYPE AlignType(){return alignType;}
	void ResetPosition(DIMALIGN_TYPE align=AlignDefault,bool bAccumulation=false);
};

class CZhunXianTextDim : public CSizeTextDim
{
public:
	f3dPoint lineStart;
	f3dPoint lineEnd;
	long hRelaBolt;		//关联螺栓句柄,智能绘图时使用 wht 11-08-10
	bool bDimZhunJu;	//标注特殊准距
	bool bDimOnLeftSide;
	CZhunXianTextDim() {hRelaBolt=0;bDimZhunJu=bDimOnLeftSide=true;}
};

class ACDB
{
public:
	enum TextHorzMode      
	{	kTextLeft    = 0,   // TH_LEFT,
		kTextCenter  = 1,   // TH_CENT,
		kTextRight   = 2,   // TH_RIGHT,
		kTextAlign   = 3,   // THV_ALIGN,
		kTextMid     = 4,   // THV_MID,
		kTextFit     = 5 }; // THV_FIT

	enum TextVertMode      
	{	kTextBase    = 0,   // TV_BASE,
		kTextBottom  = 1,   // TV_BOT,
		kTextVertMid = 2,   // TV_MID,
		kTextTop     = 3 }; // TV_TOP
};

class CKaiHeAngleDim
{
	DIMALIGN_TYPE alignType;
public:
	CDimStyle dimStyle;
	CXhChar100 dimText;		//开合角标注内容(包括角钢上的相对位置及开合角度)
	f3dPoint origin;		//对齐定位基点位置
	//real_XXX表示不考虑比例尺的尺寸
	f3dPoint dimPos,real_dim;			//开合角标注位置(相对于对齐定位基点)
	f3dPoint dimStartPos,start_ls_pos;	//开合角标注起始点(相对于对齐定位基点)
	f3dPoint real_start,real_start_ls_pos;
	f3dPoint dimEndPos,end_ls_pos;		//开合角标注终止点
	f3dPoint real_end,real_end_ls_pos;
	double dist;		//开合范围
	BOOL bInWingX;
	int start0_end1_else2;	//表示
public:
	CKaiHeAngleDim(){dist=0;bInWingX=0;start0_end1_else2=2;}
	//返回标注矩形区(不含标注箭头)左下rgnVertArr[0];右下rgnVertArr[1];右上rgnVertArr[2];左上四个角点rgnVertArr[3]
	void GetDimRect(f3dPoint rgnVertArr[4]);
	double GetHeight();
	double GetWidth();
	DIMALIGN_TYPE AlignType(){return alignType;}
	void ResetPosition();
};
struct DATAPOINT_TAG
{
	DWORD key_tag;
	ARRAY_LIST<f3dPoint> vertexArr;
	DATAPOINT_TAG(){
		key_tag=0;
		vertexArr.Empty();
	}
};
#ifndef __TEXTFRAMDE_SIZE_DEF_
#define __TEXTFRAMDE_SIZE_DEF_
struct FRAMESIZE{
	short siFrameStyle;	//-1.自动判断编号标注圈样式;0.圈圈;1.腰圆矩形;2.普通矩形框
	double fMargin;		//文本距边框的间隙
	double fFrameWidth;	//边框的宽度
	double fTextHeight;	//字体高度
	//double fFrameHeight;	//边框的高度一般由边框样式、边距及边框宽度决定
};
#endif

//带边框的文本
class CTextFrame{
	double margin;		//文本距边框的间隙
	double frameWidth;	//边框的长度
	double frameHeight;	//边框的高度
	double textHeight;	//字体高度
	double WtoH;		//字体宽高比
	CXhChar16 dimText,dimText2;	//边框中标注的文本
public:
	ACDB::TextHorzMode textAlignHori;
	ACDB::TextVertMode textAlignVert;
	static const double BULGERECT_EXTEND_COEF;	// = 0.1;腰圆矩形相较普通矩形宽度单侧延长系数
	int frameStyle;		//-1.自动判断;0.圆圈;1.腰圆矩形框;2.普通矩形框
	f3dPoint dataPos;	//数据位置
	f3dPoint dimPos;	//标注位置
	long hPart;			//关联构件句柄
	int	 cls_id;		//关联构件类型
	int	 iCurLayer;		//文本所在图层
	DWORD dwKeyTag;		//数据点的标签键值
	BYTE cFlag;			//数据点缩放状态标记
public:
	f3dPoint center,widthVec;	//仅用于绘图时临时存取数据用，只影响边框位置不影响边框大小
	CTextFrame();
	//初始化
	void InitTextFrame(double text_height=2.5,double frame_width=0,
		int frame_style=-1,double space=0.2,double w_to_h=0.7);
	//智能建立边框 frameStyle:-1.自动判断;0.圆圈;1.腰圆矩形框;2.普通矩形框
	bool BuildFrame(char* dimText,char* dimText2=NULL,double frameWidth=0,int frameStyle=-1,double text_height=0,double _margin=0);
	bool BuildFrame(char* dimText,FRAMESIZE& frame);
	bool BuildFrame(char* dimText,char* dimText2,FRAMESIZE& frame);
	void ZoomFrameSize(double zoomscale_coef,double fMinFontSize=1.0);
	//计算结果属性获取函数
	char*  Text(){return dimText;}
	char*  Text2(){return dimText2;}
	__declspec( property (get=Text)) char* szText;
	__declspec( property (get=Text2)) char* szText2;
	//边框类型:0.圆圈;1.腰圆矩形框;2.普通矩形框
	int    Style(){return frameStyle;}
	//边框宽度(直径)
	double Width(){return frameWidth;}
	//边框高度
	double Height(){return frameHeight;}
	//字体高度
	double TextHeight(){return textHeight;}
	//边框距文本的间隙
	double BoundarySpace(){return margin;}
	//得到编号矩形框 //可根据比例得到矩形框大小，默认为1 wht 10-09-16
	void GetDimRect(f3dPoint rgnVertArr[4],double draw_scale=1.0) const;
	//获得引出线与文本框连接端的端点位置 
	//根据比例得到引出线端节点位置 绘制1:1组焊图时需要 wht 10-09-17
	f3dPoint GetRefLineEndPos(double draw_scale=1.0);
	//更新数据位置
	void UpdateDataPos(CHashList<DATAPOINT_TAG>& xDataPointTagHash);
};
#ifdef __DRAWING_CONTEXT_
CXhChar16 GetPartNoIncMat(const char* sPartNo,char cMat);
#else
CXhChar16 GetPartNoIncMat(const char* sPartNo,char cMat,int iMatCharPosType=0,char cMatSeparator=0);
#endif
