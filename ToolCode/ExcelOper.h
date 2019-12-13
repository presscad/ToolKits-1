#ifndef __EXCEL_OPER_
#define __EXCEL_OPER_

#include "excel9.h"
#include "Variant.h"
#include "XhCharString.h"
#include "LogFile.h"
#include "comutil.h"
#include "..\XhRptTbl\XhRptTbl.h"

class CExcelOperObject
{
	bool m_bFileOpened;
	_Application excel;          // Excel程序
	Workbooks    excel_workbooks;// 工作簿集合
	_Workbook    excel_workbook; // 工作簿
public:
	CExcelOperObject();
	~CExcelOperObject();
	//
	BOOL OpenExcelFile(const char* sExcelFile);
	void ExitApplication();
	void Save();
	LPDISPATCH GetWorksheets();
	LPDISPATCH ClearContent(int index);
	LPDISPATCH GetValidWorkSheet(int index);
	//插入sheet页
	LPDISPATCH Insert(int index){return InsertSheetByAfterIndex(index-1);}
	LPDISPATCH InsertSheetByAfterIndex(int afterIndex);
	LPDISPATCH InsertLastValidWorkSheet();
	//
	static bool ExcelCLSIDFromProgID(CLSID &clsid);
};

class CExcelOper
{
public:
	//颜色列表
	static const long COLOR_ID_BLACK			= 1;	//RGB(0,0,0)		黑色
	static const long COLOR_ID_WHITE			= 2;	//RGB(255,255,255)	白色
	static const long COLOR_ID_RED				= 3;	//RGB(255,0,0)		红色
	static const long COLOR_ID_BIRGHT_GREEN		= 4;	//RGB(0,255,0)		鲜绿色
	static const long COLOR_ID_BLUE				= 5;	//RGB(0,0,255)		蓝色
	static const long COLOR_ID_YELLOW			= 6;	//RGB(255,255,0)	黄色
	static const long COLOR_ID_PINK				= 7;	//RGB(255,0,255)	粉红色
	static const long COLOR_ID_TURQUOISE		= 8;	//RGB(0,255,255)	青绿色
	static const long COLOR_ID_DARK_RED			= 9;	//RGB(128,0,0)		深红色
	static const long COLOR_ID_GREEN			= 10;	//RGB(0,128,0)		绿色
	static const long COLOR_ID_DARK_BLUE		= 11;	//RGB(0,0,128)		深蓝色
	static const long COLOR_ID_DARK_YELLOW		= 12;	//RGB(128,128,0)	深黄色
	static const long COLOR_ID_VIOLET			= 13;	//RGB(128,0,128)	紫罗兰
	static const long COLOR_ID_TEAL				= 14;	//RGB(0,128,128)	青色
	static const long COLOR_ID_GRAY_25			= 15;	//RGB(192,192,192)	灰-25%
	static const long COLOR_ID_GRAY_50			= 16;	//RGB(128,128,128)	灰-50%
	static const long COLOR_ID_PERIWINKLE		= 17;	//RGB(153,153,153)	海螺色
	static const long COLOR_ID_PLUM_PLUS		= 18;	//RGB(153,51,102)	梅红色
	static const long COLOR_ID_IVORY			= 19;	//RGB(255,255,204)	象牙色
	static const long COLOR_ID_LITE_TURQUOISE	= 20;	//RGB(204,255,255)	浅青色
	static const long COLOR_ID_DARK_PURPLE		= 21;	//RGB(102,0,102)	深紫色
	static const long COLOR_ID_CORAL			= 22;	//RGB(255,128,128)	珊瑚红
	static const long COLOR_ID_OCEAN_BLUE		= 23;	//RGB(0,102,204)	海蓝色
	static const long COLOR_ID_ICE_BLUE			= 24;	//RGB(204,204,255)	冰蓝
	static const long COLOR_ID_DARK_BLUE_PLUS	= 25;	//RGB(0,0,128)		深蓝色
	static const long COLOR_ID_PINK_PLUS		= 26;	//RGB(255,0,255)	粉红色
	static const long COLOR_ID_YELLOW_PLUS		= 27;	//RGB(255,255,0)	黄色
	static const long COLOR_ID_TURQUOISE_PLUS	= 28;	//RGB(0,255,255)	青绿色
	static const long COLOR_ID_VIOLET_PLUS		= 29;	//RGB(128,0,128)	紫罗色
	static const long COLOR_ID_DARK_RED_PLUS	= 30;	//RGB(128,0,0)		深红色
	static const long COLOR_ID_TEAL_PLUS		= 31;	//RGB(0,128,128)	青色
	static const long COLOR_ID_BLUE_PLUS		= 32;	//RGB(0,0,255)		蓝色
	static const long COLOR_ID_SKY_BLUE			= 33;	//RGB(0,204,255)	天蓝色
	static const long COLOR_ID_LIGHT_TURQUOISE	= 34;	//RGB(204,255,255)	浅青绿
	static const long COLOR_ID_LIGHT_GREEN		= 35;	//RGB(204,255,204)	浅绿色
	static const long COLOR_ID_LIGHT_YELLOW		= 36;	//RGB(255,255,153)	浅黄色
	static const long COLOR_ID_PALE_BLUE		= 37;	//RGB(153,204,255)	淡蓝色
	static const long COLOR_ID_ROSE				= 38;	//RGB(255,153,204)	玫瑰红
	static const long COLOR_ID_LAVENDER			= 39;	//RGB(204,153,255)	淡紫色
	static const long COLOR_ID_TAN				= 40;	//RGB(255,204,153)	茶色
	static const long COLOR_ID_LIGHT_BLUE		= 41;	//RGB(51,102,255)	浅蓝色
	static const long COLOR_ID_AQUA				= 42;	//RGB(51,204,204)	水绿色
	static const long COLOR_ID_LIME				= 43;	//RGB(153,204,0)	酸橙色
	static const long COLOR_ID_GOLD				= 44;	//RGB(255,204,0)	金色
	static const long COLOR_ID_LIGHT_ORANGE		= 45;	//RGB(255,153,0)	浅橙色
	static const long COLOR_ID_ORANGE			= 46;	//RGB(255,102,0)	橙色
	static const long COLOR_ID_BLUE_GRAY		= 47;	//RGB(102,102,153)	蓝-灰
	static const long COLOR_ID_GRAY_40			= 48;	//RGB(150,150,150)	灰-40%
	static const long COLOR_ID_DARK_TEAL		= 49;	//RGB(0,51,102)		深青
	static const long COLOR_ID_SEA_GREEN		= 50;	//RGB(51,153,102)	海绿
	static const long COLOR_ID_DARK_GREEN		= 51;	//RGB(0,51,0)		深绿
	static const long COLOR_ID_OLIVE_GREEN		= 52;	//RGB(51,51,0)		橄榄色
	static const long COLOR_ID_BROWN			= 53;	//RGB(153,51,0)		褐色
	static const long COLOR_ID_PLUM				= 54;	//RGB(153,51,102)	梅红色
	static const long COLOR_ID_INDIGO			= 55;	//RGB(51,51,153)	靛蓝
	static const long COLOR_ID_GRAY_80			= 56;	//RGB(51,51,51)		灰-80%
public:
	//创建指定数量的sheet页
	static LPDISPATCH CreateExcelWorksheets(int nSheet);
	//根据行、列获取单元格字符串形式位置
	static CXhChar50 GetCellPos(int iCol, int iRow);
	static CXhChar50 GetCellName(int iCol);			
	//添加一行数据到Excel
	static void AddRowToExcelSheet(_Worksheet &excel_sheet, int iRow, CStringArray &str_arr, 
		double *col_arr = NULL, BOOL bWrapText = FALSE);
	//设置主标题
	static void SetMainTitle(_Worksheet excel_sheet, char *cell_start, char *cell_end,
		char *title, double row_height = 20.0, double font_size = 14.0,
		short horizAlignment = 3, short verticalAlignment = 2, bool bSetBorder = true);
	//设置字体
	static void SetBoldFont(_Worksheet excel_sheet, char *cell_start, char *cell_end);
	static void SetFontBold(_Worksheet excel_sheet, int iStartCol, int iRow, int nCount);
	static void SetFontSize(_Worksheet excel_sheet, const char *cell_start, const char *cell_end,double font_size = 14.0, bool bBold = FALSE);
	static void SetFontColor(_Worksheet excel_sheet, char *cell_start, char *cell_end, long colorId);
	//设置指定区域单元格内容
	static void SetRangeValue(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var);
	static void SetRangeValue(_Worksheet &excel_sheet, int iCol, int iRow, char* sValue);
	//设置指定区域单元格数字格式
	static void SetRangeNumberFormat(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var);
	static void SetRangeNumberFormatLocal(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var);
	//设定指定区域单元水平对齐方式
	static void SetRangeHorizontalAlignment(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var);
	//设定指定区域单元竖直对齐方式
	static void SetRangeVerticalAlignment(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var);
	//设置文字对齐方式(水平、竖直)
	static void SetRangeOrientation(_Worksheet excel_sheet, const char *cell_start, const char *cell_end, long degree = 0);
	//设置自动换行
	static void SetRangeWrapText(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var);
	//设置指定区域的边框
	static void SetRangeBorders(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT fontSize);
	//合并行
	static void MergeRowRange(_Worksheet excel_sheet, char *cell_start, char *cell_end);
	//合并列
	static void MergeColRange(_Worksheet excel_sheet, char *cell_start, char *cell_end);
	//设置列宽
	static void SetRangeColWidth(_Worksheet excel_sheet, double width, char *cell_start,char *cell_end = NULL, BOOL bCenter = FALSE);
	//设置行高
	static void SetRangeRowHeight(_Worksheet excel_sheet, double height, char *cell_start,char *cell_end = NULL, BOOL bCenter = FALSE);
	//设置单元格背景颜色
	static void SetRangeBackColor(_Worksheet excel_sheet, long colorId, int iRow, int iCol, int nCount);
	static void SetRangeBackColor(_Worksheet excel_sheet, long colorId, char *cell_start, char *cell_end = NULL);
	//设置单元格%的形式
	static void SetRangeNumberFormatLocal(_Worksheet excel_sheet, const char* sLocal, char *cell_start, char *cell_end = NULL);
	//获取指定sheet内容
	static BOOL GetExcelContentOfSpecifySheet(const char* sExcelFile, CVariant2dArray &sheetContentMap, int iSheetIndex, CXhTable* pTempTable=NULL,int maxColCount=-1);
	static BOOL GetExcelContentOfSpecifySheet(CExcelOperObject* pExcelOperObj, CVariant2dArray &sheetContentMap, const char* sSheetName, CXhTable* pTempTable = NULL, int maxColCount = -1);
	static BOOL GetExcelContentOfSpecifySheet(const char* sExcelFile, CVariant2dArray &sheetContentMap, const char* sSheetName, CXhTable* pTempTable = NULL, int maxColCount = -1);
	static BOOL GetExcelContentOfSpecifySheet(CExcelOperObject* pExcelOperObj, CVariant2dArray &sheetContentMap, int iSheetIndex, CXhTable* pTempTable = NULL, int maxColCount = -1);
	static BOOL GetExcelContentOfSpecifySheet(const char* sExcelFile, CStringArray &sheetNameArr);
	static BOOL GetExcelContentOfSpecifySheet(CExcelOperObject* pExcelOperObj, CStringArray &sheetNameArr);
	//根据sheet名获取sheet序号
	static int  GetExcelIndexOfSpecifySheet(CExcelOperObject* pExcelOperObj, const char* sSheetName);
	//根据XhTable导出Excel
	static BOOL  ExportExcle(CXhTable* pTbl);
};

#endif