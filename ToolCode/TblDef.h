#ifndef __TABLE_DEF_H
#define __TABLE_DEF_H

	//格式表的定位模式
enum TBL_LOCATE_STYLE{
	TOP_LEFT = 0,		//左上角
	TOP_RIGHT = 1,		//右上角
	BOTTOM_LEFT = 2,	//左下角
	BOTTOM_RIGHT = 3,	//右下角
};
enum TBL_TEXT_ALIGN{
	TEXTALIGN_RIGHT_CENTER  = 0,	//右中央
	TEXTALIGN_MIDDLE_CENTER = 1,	//正中央
	TEXTALIGN_BOTTOM_RIGHT  = 2,	//右下角
	TEXTALIGN_TOP_LEFT	  = 3,	//左下角
	TEXTALIGN_LEFT_CENTER	  = 4,	//左中央
};
	//表格单元格(显示)模式
enum GRID_MODE{
	STRING_GRID		= 0,	//显示字符串
	GRAPH_BLOCK_GRID= 1,	//显示图形
	PIC_BLOCK_GRID	= 2,	//显示OLE图片
};
	//表格中所填充内容的数据类型
#ifndef _DEF_GRID_DATA_V_TYPE
#define _DEF_GRID_DATA_V_TYPE
enum GRID_DATA_TYPE{
	GRID_VT_EMPTY	= 0,	// 未指定
	GRID_VT_NULL	= 1,	// 空（暂保留）
	GRID_VT_I2		= 2,	// 2字节带符号整数
	GRID_VT_I4		= 3,	// 4字节带符号整数
	GRID_VT_R4		= 4,	// 4字节浮点数
	GRID_VT_R8		= 5,	// 8字节浮点数
	GRID_VT_DATE	= 6,	// 日期
	GRID_VT_STRING	= 7,	// 字符串
	GRID_VT_POINTER = 8,	// 指针
	GRID_VT_BOOL	= 9,	// 布尔数; TRUE=1, FALSE=0
	// 其余枚举常数在此无效
};
#endif
GRID_DATA_TYPE IntToGridDataType(int nType);
class CGridVariant{
public:
	GRID_DATA_TYPE vType;
	union{
		BOOL					bVal;     // GRID_VT_BOOL
		__int16					iVal;     // GRID_VT_I2
		__int32					lVal;     // GRID_VT_I4
		float					fltVal;   // GRID_VT_R4
		double					dblVal;   // GRID_VT_R8
		DATE					date;     // GRID_VT_DATE
		char*					sVal;     // GRID_VT_STRING
		void*					pVal;// GRID_VT_POINTER
	};
public:
	void Import(FILE* fp);
	void Export(FILE* fp);
	CGridVariant();
	CGridVariant(CGridVariant& SourceObj);
	~CGridVariant();
	CGridVariant& operator = (const CGridVariant &var);
	void ChangeToType(GRID_DATA_TYPE data_type);
	void ClearVariant();
	BOOL SetGridString(char *string,BOOL bForceToString=TRUE);
};
#ifndef __DECLARE_GRID_DATA_STRU__
#define __DECLARE_GRID_DATA_STRU__
typedef struct tagGRID_DATA_STRU
{
	int data_type;	//0.标题文本1.实时文本2.实时数据3.草图区域
	long type_id;
	char sContent[MAX_PATH];
	char sContentPrefix[16];
	char sContentSuffix[16];
	double min_x,min_y,max_x,max_y;
	double fTextHigh,fHighToWide;
	//1.TopLeft    2.TopCenter    3.TopRight
	//4.MiddleLeft 5.MiddleCenter 6.MiddleRight
	//7.BottomLeft 8.BottomCenter 9.BottomRight
	int align_type;	
	tagGRID_DATA_STRU(){
		data_type=0;
		type_id=0;
		strcpy(sContent,"");
		strcpy(sContentPrefix,"");
		strcpy(sContentSuffix,"");
		min_x=min_y=max_x=max_y=0;
		fTextHigh=fHighToWide=0;
		align_type=0;
	}
}GRID_DATA_STRU;
#endif
class CGrid
{
public:
	//表格单元格类声明
	class CFontStyle{
	public:
		//字体格式
		LONG lfHeight; 
		LONG lfWidth; 
		LONG lfEscapement; 
		LONG lfOrientation; 
		LONG lfWeight; 
		BYTE lfItalic; 
		BYTE lfUnderline; 
		BYTE lfStrikeOut; 
		BYTE lfCharSet; 
		BYTE lfOutPrecision; 
		BYTE lfClipPrecision; 
		BYTE lfQuality; 
		BYTE lfPitchAndFamily; 
		TCHAR* lfFaceName; 
		COLORREF crTextColor;
		//文字对齐方式
		UINT m_nTextAlignFlags;
		//文字内边距
		double m_fLeftDist,m_fRightDist,m_fTopDist,m_fBottomDist;
	public:
		CFontStyle();
		~CFontStyle();
		void Import(FILE* fp);
		void Export(FILE* fp);
		CFontStyle& operator =(const CFontStyle& fontstyle);
		CFontStyle& operator =(const LOGFONT& fontstyle);
		BOOL operator ==(const CFontStyle& fontstyle)const;
		BOOL operator !=(const CFontStyle& fontstyle)const;
		operator LOGFONT()const;
		int SetFaceName(const TCHAR *face_name);
		void CopyFromLOGFONT(const LOGFONT &logfont);
		void CopyToLOGFONT(LOGFONT &logfont)const;
	};
	//竖直合并虚单元；水平合并虚单元（与两种虚单元对应的是[真实单元]）
	struct{
		BOOL bTitle;		//用于标识是否为表格单元组中的标题项
		long type_id;		//用于标识单元格的内容类型
		long ext_feature;	//用于扩展,以后可能有用
	}mark;
	BOOL m_bVGridVirtual,m_bHGridVirtual;
	BOOL m_bSelectStatus;
	int iRow,iColumn;			//行列位置号
#ifdef __DRAWING_CONTEXT_
	double textsize;
	double text_angle;			//文字旋转方向
	static double PREFER_TEXT_SIZE;
#else
	CFontStyle font_style;			//字高
#endif
	GRID_MODE grid_mode;		//表格单元格(显示)模式

	CGridVariant data;			//表格中的数据内容
	DWORD feature;				//定义表格的一些特征(由用户定)
public:
	CGrid();
	~CGrid();
public:
#ifndef __DRAWING_CONTEXT_
	HRESULT ReadGridFromStream(IStream *pStream);
	HRESULT WriteGridToStream(IStream *pStream);
#endif
	void Import(FILE* fp);
	void Export(FILE* fp);
	void Copy(const CGrid& grid);
	CGrid& operator = (CGrid& grid);
};

#endif
