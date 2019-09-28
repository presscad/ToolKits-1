#include "XhCharString.h"
#include "f_ent.h"
#include "f_ent_list.h"
#include "f_alg_fun.h"

enum DIMALIGN_TYPE{AlignDefault=1,TopLeft,TopCenter,TopRight,MiddleLeft,MiddleCenter,MiddleRight,BottomLeft,BottomCenter,BottomRight};
typedef struct tagTMADRAWING_DIM_STYLE
{
	CXhChar16 dimStyleName;	//文字样式名称
	double arrowSize;		//箭头大小
	double gap;				//文字与基准线间隙
	unsigned long dimStyleId;	//文字样式
	tagTMADRAWING_DIM_STYLE(){arrowSize=gap=0;}
	void InitDimStyle(double textHeight=0,double arrowSize=0,double gap=0);
}TMADRAWING_DIM_STYLE;
class CDimStyle{
public:
	TMADRAWING_DIM_STYLE style;
	double textHeight;
	CDimStyle(){textHeight=2.0;}
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
	double angle;			//弧度
	double dist;			//标注距离
	int nHits;				//连续的等距为S的标注，可以合并为一个S X nHits的标注
	//0.装配检测尺寸 1.局部偏移尺寸 2.脚钉间距 
	//3(-3).构件定位尺寸 (+-表上下) 4.外端面到外端面距离标注
	int iOrder;				//分层标注时用来确定尺寸定位顺序
public:
	BOOL bAllowRelayout;	//是否允许自动调整标注位置,某些特殊情况不用自动调整位置 wht 10-08-15
	BOOL bShortenDimStartLine,bShortenDimEndLine;
	CSizeTextDim();
	//precision只能为"0","0.5","0.0"
	double Fillet();	//将标注距离dist按照precision进行圆整化
	const char* DimText();
	double GetWidth();
	double GetHeight(BOOL bIncShortDimLineOnly=FALSE);
	//可根据比例得到矩形框大小，默认为1 wht 10-09-16;
	//返回标注矩形区(不含标注箭头)左下rgnVertArr[0];右下rgnVertArr[1];右上rgnVertArr[2];左上四个角点rgnVertArr[3]
	void GetDimRect(f3dPoint rgnVertArr[4],BOOL bIncShortDimLineOnly=FALSE,double draw_scale=1.0) const;
	DIMALIGN_TYPE AlignType(){return alignType;}
	void ResetPosition(DIMALIGN_TYPE align=AlignDefault);
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
	//0.焊接方向文字 1.绝对尺寸 +-2(表上下).视图名称  3(-3).剖视图符 +表示沿X轴正方向 -表示沿X轴负方向
	int iOrder;				//分层标注时用来确定尺寸定位顺序
public:
	CTextOnlyDrawing(){iOrder=0;angle=dist=0;textHeight=2.0;alignType=MiddleLeft;bNeedConvertDistToText=true;}
	double Fillet();
	//precision只能为"0","0.5","0.0"
	const char* GetPrecisionDimText();
	//返回标注矩形区(不含标注箭头)左下rgnVertArr[0];右下rgnVertArr[1];右上rgnVertArr[2];左上四个角点rgnVertArr[3]
	void GetDimRect(f3dPoint rgnVertArr[4],double draw_scale=1.0); //可根据比例得到矩形框大小，默认为1 wht 10-09-16
	double GetHeight();
	double GetWidth();
	DIMALIGN_TYPE AlignType(){return alignType;}
	void ResetPosition(DIMALIGN_TYPE align=AlignDefault);
};

class CZhunXianTextDim : public CSizeTextDim
{
public:
	f3dPoint lineStart;
	f3dPoint lineEnd;
	long hRelaBolt;		//关联螺栓句柄,智能绘图时使用 wht 11-08-10
	bool bDimZhunJu;	//标注特殊准距
	CZhunXianTextDim() {hRelaBolt=0;bDimZhunJu=true;}
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
	int frameStyle;		//-1.自动判断;0.圆圈;1.腰圆矩形框;2.普通矩形框
	f3dPoint dataPos;	//数据位置
	f3dPoint dimPos;	//标注位置
	long hPart;			//关联构件句柄
	int	 cls_id;		//关联构件类型
	int	 iCurLayer;		//文本所在图层
public:
	f3dPoint center,widthVec;	//仅用于绘图时临时存取数据用，只影响边框位置不影响边框大小
	CTextFrame();
	//初始化
	void InitTextFrame(double text_height=2.5,double frame_width=0,
		int frame_style=-1,double space=0.2,double w_to_h=0.7);
	//智能建立边框 frameStyle:-1.自动判断;0.圆圈;1.腰圆矩形框;2.普通矩形框
	bool BuildFrame(char* dimText,char* dimText2=NULL,double frameWidth=0,int frameStyle=-1);
	//计算结果属性获取函数
	char*  Text(){return dimText;}
	char*  Text2(){return dimText2;}
	//边框类型:0.圆圈;1.腰圆矩形框;2.普通矩形框
	int    Style(){return frameStyle;}
	//边框宽度(直径)
	double Width(){return frameWidth;}
	//边框高度
	double Height(){return frameHeight;}
	//边框距文本的间隙
	double BoundarySpace(){return margin;}
	//得到编号矩形框 //可根据比例得到矩形框大小，默认为1 wht 10-09-16
	void GetDimRect(f3dPoint rgnVertArr[4],double draw_scale=1.0) const;
	//获得引出线与文本框连接端的端点位置 
	//根据比例得到引出线端节点位置 绘制1:1组焊图时需要 wht 10-09-17
	f3dPoint GetRefLineEndPos(double draw_scale=1.0);
};

class DimStyleTable
{
public:
	static TMADRAWING_DIM_STYLE dimStyle;
	static TMADRAWING_DIM_STYLE dimStyle2;
	static TMADRAWING_DIM_STYLE dimStyle3;	//不强制将尺寸放于尺寸界线内侧
	static TMADRAWING_DIM_STYLE dimAngleStyle;
	static TMADRAWING_DIM_STYLE dimRadiusStyle;
};

class CDrawNewSysPara
{
public:
	int  iLsSpaceDimStyle;		//0.X轴方向	  1.Y轴方向  2.自动判断 3.不标注  4.无标注内容(X轴方向)  4.仅标尺寸线，无标注内容(X轴方向)主要用于江津(北)塔厂质检是盲检填数
	int  nMaxBoltNumAlongX;		//沿X轴方向标注支持的最大螺栓个数
	int  iJgGDimStyle;			//0.始端标注  1.中间标注 2.自动判断
	int iJgZoomSchema;			//角钢绘制方案，0.1:1绘制 1.使用构件图比例 2.长宽同比缩放 3.长宽分别缩放
	BOOL bDimRibPlateSetUpPos;	//标注角钢肋板安装位置
	double	fDimTextSize;		//长度尺寸标注文本高
	int iDimPrecision;			//尺寸精确度
	double fKaiHeJiaoThreshold; //开合角标注阈值(°) 
	BOOL bMaxExtendAngleLength;	//最大限度延伸角钢绘制长度
	double fRealToDraw;			//基础绘图比例尺＝实际尺寸/绘图尺寸，如1:20时，fRealToDraw=20
	double	fDimArrowSize;		//尺寸标注箭头长
	int  nMaxBoltNumStartDimG;	//集中在始端标注准距支持的最大螺栓数
	CDrawNewSysPara()
	{
		memset(this,0,sizeof(CDrawNewSysPara));
		fRealToDraw=5;
		iJgZoomSchema=3;
		fDimTextSize=3;
		nMaxBoltNumAlongX=100;
	}
};




