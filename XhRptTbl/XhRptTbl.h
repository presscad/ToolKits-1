// 下列 ifdef 块是创建使从 DLL 导出更简单的
//宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 XEROCAD_EXPORTS
// 符号编译的。在使用此 DLL 的
//任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将 
// XEROCAD_API 函数视为是从此 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#pragma once
#ifdef _XERORPTTBL_EMBEDDED_
	#define XERORPTTBL_API	//内嵌时XERORPTTBL_API什么都不代表
#else
	#ifdef XERORPTTBL_EXPORTS
		#define XERORPTTBL_API __declspec(dllexport)
	#else
		#define XERORPTTBL_API __declspec(dllimport)
	#endif
#endif

#include "Buffer.h"
#include "f_ent.h"
#include "XhCharString.h"

struct XHTBLDEF {
	//格式表的定位模式
	static const BYTE TOP_LEFT = 0;		//左上角
	static const BYTE TOP_RIGHT = 1;	//右上角
	static const BYTE BOTTOM_LEFT = 2;	//左下角
	static const BYTE BOTTOM_RIGHT = 3;	//右下角
	//文本对齐模式
	static const BYTE TEXTALIGN_RIGHT_CENTER  = 0;	//右中央
	static const BYTE TEXTALIGN_MIDDLE_CENTER = 1;	//正中央
	static const BYTE TEXTALIGN_BOTTOM_RIGHT  = 2;	//右下角
	static const BYTE TEXTALIGN_TOP_LEFT	  = 3;	//左下角
	static const BYTE TEXTALIGN_LEFT_CENTER	  = 4;	//左中央
	//表格单元格(显示)模式
	static const BYTE STRING_GRID		= 0;	//显示字符串
	static const BYTE GRAPH_BLOCK_GRID	= 1;	//显示图形
	static const BYTE PIC_BLOCK_GRID	= 2;	//显示OLE图片
	//单元格数据类型
	static const BYTE GRID_VT_EMPTY	= 0;	// 未指定
	static const BYTE GRID_VT_NULL	= 1;	// 空（暂保留）
	static const BYTE GRID_VT_I2	= 2;	// 2字节带符号整数
	static const BYTE GRID_VT_I4	= 3;	// 4字节带符号整数
	static const BYTE GRID_VT_R4	= 4;	// 4字节浮点数
	static const BYTE GRID_VT_R8	= 5;	// 8字节浮点数
	static const BYTE GRID_VT_DATE	= 6;	// 日期
	static const BYTE GRID_VT_VARSTR= 7;	// 字符串
	static const BYTE GRID_VT_POINTER = 8;	// 指针
	static const BYTE GRID_VT_BOOL	= 9;	// 布尔数; TRUE=1, FALSE=0
	static const BYTE GRID_VT_NSTR	=10;	// 固定长度字符串
public:
	struct XERORPTTBL_API COORD2D {
		double x,y;
		COORD2D(double _x=0,double _y=0) {
			this->x=_x;
			this->y=_y; 
		};
		COORD2D(const double* coord2d) {
			this->x=coord2d[0];
			this->y=coord2d[1]; 
		};
		void Set(double X, double Y){x=X;y=Y;}
		operator double*(){return &x;}
	};
};
class XERORPTTBL_API XHVAR{
public:
	BYTE ciType;
	union{
		BOOL		bVal;		// GRID_VT_BOOL
		__int16		iVal;		// GRID_VT_I2
		__int32		lVal;		// GRID_VT_I4
		float		fltVal;		// GRID_VT_R4
		double		dblVal;		// GRID_VT_R8
		//DATE		date;		// GRID_VT_DATE
		char		szStrBuf8[8];//GRID_VT_NSTR
		char*		sVal;		// GRID_VT_STRING
		void*		pVal;		// GRID_VT_POINTER
	};
public:
	XHVAR();
	XHVAR(const XHVAR& srcObj);
	~XHVAR();
	//
	void ClearVariant();
	bool SetGridString(const char *string,BOOL bForceToString=true);
	void Import(FILE* fp);
	void Export(FILE* fp);
};
class XERORPTTBL_API XHGRID
{
	static float PREFER_TEXT_SIZE;
	bool m_bSelectStatus;	//如果仅用于描述数据的话，此属性很可能无用 wjh-2019.10.10
public:
	//表格单元格类声明
	struct XERORPTTBL_API FONTSTYLE{
	public:
		//字体格式
		long lfHeight; 
		long lfWidth; 
		long lfEscapement; 
		long lfOrientation; 
		long lfWeight; 
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
		COLORREF crBackground;
		//文字对齐方式
		UINT m_nTextAlignFlags;
		//文字内边距
		float m_fLeftDist,m_fRightDist,m_fTopDist,m_fBottomDist;
	public:
		FONTSTYLE();
		FONTSTYLE(const FONTSTYLE& fontstyle);
		FONTSTYLE(const LOGFONT& fontstyle);
		~FONTSTYLE();
		void Import(FILE* fp);
		void Export(FILE* fp);
		//BOOL operator ==(const CFontStyle& fontstyle)const;
		//BOOL operator !=(const CFontStyle& fontstyle)const;
		operator LOGFONT()const;
		int SetFaceName(const TCHAR *face_name);
		void CopyFromLOGFONT(const LOGFONT &logfont);
		void CopyToLOGFONT(LOGFONT &logfont)const;
	};
	struct SIMPLE{
		float sfTextSize;
		float sfTextAngle;			//文字旋转方向
		BOOL bBold;					//是否加粗
	}simfont;
	struct{
		bool bTitle;		//用于标识是否为表格单元组中的标题项
		long type_id;		//用于标识单元格的内容类型
		long ext_feature;	//用于扩展,以后可能有用
	}mark;
	bool m_bVGridVirtual;	//竖直合并虚单元
	bool m_bHGridVirtual;	//水平合并虚单元
	int iRow,iColumn;	//行列位置号
	BYTE ciFontFormat;	//0.简单格式字体;1.Windows复杂格式字体
	BYTE grid_mode;		//GRID_MODE表格单元格(显示)模式
	XHVAR data;			//表格中的数据内容
	FONTSTYLE xFontStyle;
	DWORD uiTagFeature;	//定义表格的一些特征(由用户定)
	UINT uidCellValSource;	//用于可以柔性设定表格数据内容的单元格内容填充类型Id
public:
	XHGRID();
	XHGRID(const XHGRID& grid);
	~XHGRID();
	//
	bool get_blStateSelected();
	bool set_blStateSelected(bool blSelected);
	__declspec(property(put = set_blStateSelected, get = get_blStateSelected))bool blStateSelected;
public:
	bool ReadGridFromStream(BUFFER_IO* pIO);
	bool WriteGridToStream(BUFFER_IO* pIO);
	void Import(FILE* fp);
	void Export(FILE* fp);
	void Copy(const XHGRID& grid);
};

class XERORPTTBL_API CXhGridMap
{
	XHGRID *grid_map;		//表格存储区
	int m_nRow,m_nColumn;	//行总数与列总数
public:
	CXhGridMap();
	~CXhGridMap();
public:
	void Destroy();
	BOOL Create(int nRow,int nColumn);
	int GetRowsCount() const{return m_nRow;}
	int GetColumnsCount() const{return m_nColumn;}
	BOOL InsertRow(int iRow);
	BOOL DelRow(int iRow);
	BOOL InsertColumn(int iColumn);
	BOOL DelColumn(int iColumn);
	XHGRID* GetGridAt(int iRow,int iColumn);					//根据行列号获得指定单元格指针
	BOOL SetGrid(int iRow, int iColumn,const XHGRID &grid);	//填充单元格内容
};
//CXhTable
class XERORPTTBL_API CXhTable
{
	long m_iSerial;
	BYTE ciLocaStyle;			//TBL_LOCATE_STYLE类型, 左上角;左下角;右下角;右上角
	XHTBLDEF::COORD2D xLocation;	//基(定位)点二维坐标
	double *rows_arr_high;		//行高列表
	double *columns_arr_wide;	//列宽列表
	CXhGridMap grid_map;		//表格存储区
public:
	CXhChar50 m_sTblName;		//表格名称(sheet名)
	//
	int get_ID() const { return m_iSerial; }
	__declspec(property(get = get_ID)) int ID;					//序号
	int get_nRowsCount() const {return grid_map.GetRowsCount();}
	__declspec(property(get = get_nRowsCount)) int nRowsCount;	//行数
	int get_nColsCount() const {return grid_map.GetColumnsCount();}
	__declspec(property(get = get_nColsCount)) int nColsCount;	//列数
private:
	double getRowLinePos(BOOL bUpToDown, int iRow);		//获得指定行线距离格式表相应外缘的位置距离
	double getColumnLinePos(BOOL bLToR, int iColumn);	//获得指定列线距离格式表相应外缘的位置距离
public:
	CXhTable(long iSerial = 0);
	~CXhTable();
	//表格操作
	void Destroy();							//销毁表格,释放内存
	BOOL Create(int nRow, int nCol, double high = 6, double width = 16);
	void CopyFrom(CXhTable *pXhRptTbl);	//拷贝表格
	void CopyFrom(CXhTable *pXhRptTbl,int iRemoveStart,int iRemoveEnd);
	void SetBasePos(const double* pos2d);	//设定定位点二维坐标
	void GetBasePos(double* pos2d);			//获得格式表的定位点二维坐标
	void SetLocateStyle(BYTE ciStyle);		//设定表格定位形式
	BYTE GetLocateStyle();					//获得格式表的定位方式
	void SetFontSize(float fontSize=2);		//设定格式表的字体高度
	double GetTableWide();					//获得整张格式表的总宽
	double GetTableHeight();				//获得整张格式表的总高
	BOOL GetTblRect(f2dRect *tbl_rect);		//获得整张格式表的区域
	//行列操作
	BOOL DelRow(int iRow);
	BOOL DelCol(int iCol);
	BOOL InsertRow(int iRow, double high = 0);
	BOOL InsertCol(int iCol, double wide = 0);
	BOOL SetRowFontSize(int iRow, float fontSize,bool bBold=false);
	BOOL SetRowColor(int iRow, COLORREF crFontColor, COLORREF crBackColor=RGB(255,255,255));
	BOOL SetRowHigh(int iRow, double high);					//设定指定行高
	BOOL SetColWide(int iCol, double wide);					//设定指定列宽
	BOOL GetRowHigh(int iRow, double *high);				//获得指定行高
	BOOL GetColWide(int iCol, double *wide);				//获得指定列宽
	BOOL GetRowLineStart(int iRow, double* pStartPos2d);	//获得指定行线的起始位置
	BOOL GetRowLineEnd(int iRow, double* pEndPos2d);		//获得指定行线的终止位置
	BOOL GetColLineStart(int iCol, double* pStartPos2d);	//获得指定列线的起始位置
	BOOL GetColLineEnd(int iCol, double* pEndPos2d);		//获得指定列线的终止位置
	//单元格操作
	XHGRID* GetGridAt(int iRow, int iCol);					//根据行列号获得指定单元格指针
	BOOL CleanGrid(int iRowS, int iRowE);
	BOOL IsValidGrid(int iRow = 1, int iCol = 1);		//行列索引号标识有效性判断
	BOOL SetGrid(int iRow, int iCol, char *string, BOOL bForceToString = TRUE);	//填充单元格内容
	BOOL MergeGrid(int iCellRowS, int iCellColS, int iCellRowE, int iCellColE, BYTE cbMergeFlag = 0);
	BOOL GetGridRect(int iRow, int iCol, f2dRect &rect);	//获得指定单元格位置矩形
	BOOL GetGridSize(int iRow, int iCol, double *height = NULL, double *wide = NULL);
	BOOL GetGridRange(int &start_i, int &start_j, int *end_i, int *end_j);		//获得指定单元格大小(高宽)
	BOOL GetGridString(int iRow, int iCol, char *string);	//获得指定单元格的数据字符串
	//交互操作
	void SelectAll();
	int ReleaseSnapStatus();
	int GetSelectedCounts(); 
	BOOL SnapGridPos(double x, double y, double *pSnapPos2d, double scope = 1.0);
	XHGRID * SnapGrid(double x,double y,int *iRow=NULL,int *iCol=NULL);
	XHGRID *GetFirstSelectedGrid(CXhTable **pXhRptTbl = NULL, int *pos_i = NULL, int *pos_j = NULL); 
	//单位转换
	static double TransRowHightToCad(double fRowHight);
	static double TransRowHightToXls(double fRowHight);
	static double TransColWidthToCad(double fColWidth);
	static double TransColWidthToXls(double fColWidth);
public:	//表格数据导入导出
	BOOL WriteBasicTbl(BUFFER_IO *pIO);
	BOOL ReadBasicTbl(BUFFER_IO *pIO);
	BOOL ImportTxt(char *filename);				//从文本文件导入格式表
	BOOL ExportTxt(char *filename);				//向文本文件导出格式表
	BOOL ImportTxt(FILE *fp);					//从文本文件导入格式表
	BOOL ExportTxt(FILE *fp);					//向文本文件导出格式表
};
//CBasicTblLibaray
class XERORPTTBL_API CBasicTblLibaray
{
public:
	static CXhTable* AddBasicTable();
	static CXhTable* BasicTblFromSerial(long serial);
	static bool Destroy(long serial);
};


