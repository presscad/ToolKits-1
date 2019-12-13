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
	_Application excel;          // Excel����
	Workbooks    excel_workbooks;// ����������
	_Workbook    excel_workbook; // ������
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
	//����sheetҳ
	LPDISPATCH Insert(int index){return InsertSheetByAfterIndex(index-1);}
	LPDISPATCH InsertSheetByAfterIndex(int afterIndex);
	LPDISPATCH InsertLastValidWorkSheet();
	//
	static bool ExcelCLSIDFromProgID(CLSID &clsid);
};

class CExcelOper
{
public:
	//��ɫ�б�
	static const long COLOR_ID_BLACK			= 1;	//RGB(0,0,0)		��ɫ
	static const long COLOR_ID_WHITE			= 2;	//RGB(255,255,255)	��ɫ
	static const long COLOR_ID_RED				= 3;	//RGB(255,0,0)		��ɫ
	static const long COLOR_ID_BIRGHT_GREEN		= 4;	//RGB(0,255,0)		����ɫ
	static const long COLOR_ID_BLUE				= 5;	//RGB(0,0,255)		��ɫ
	static const long COLOR_ID_YELLOW			= 6;	//RGB(255,255,0)	��ɫ
	static const long COLOR_ID_PINK				= 7;	//RGB(255,0,255)	�ۺ�ɫ
	static const long COLOR_ID_TURQUOISE		= 8;	//RGB(0,255,255)	����ɫ
	static const long COLOR_ID_DARK_RED			= 9;	//RGB(128,0,0)		���ɫ
	static const long COLOR_ID_GREEN			= 10;	//RGB(0,128,0)		��ɫ
	static const long COLOR_ID_DARK_BLUE		= 11;	//RGB(0,0,128)		����ɫ
	static const long COLOR_ID_DARK_YELLOW		= 12;	//RGB(128,128,0)	���ɫ
	static const long COLOR_ID_VIOLET			= 13;	//RGB(128,0,128)	������
	static const long COLOR_ID_TEAL				= 14;	//RGB(0,128,128)	��ɫ
	static const long COLOR_ID_GRAY_25			= 15;	//RGB(192,192,192)	��-25%
	static const long COLOR_ID_GRAY_50			= 16;	//RGB(128,128,128)	��-50%
	static const long COLOR_ID_PERIWINKLE		= 17;	//RGB(153,153,153)	����ɫ
	static const long COLOR_ID_PLUM_PLUS		= 18;	//RGB(153,51,102)	÷��ɫ
	static const long COLOR_ID_IVORY			= 19;	//RGB(255,255,204)	����ɫ
	static const long COLOR_ID_LITE_TURQUOISE	= 20;	//RGB(204,255,255)	ǳ��ɫ
	static const long COLOR_ID_DARK_PURPLE		= 21;	//RGB(102,0,102)	����ɫ
	static const long COLOR_ID_CORAL			= 22;	//RGB(255,128,128)	ɺ����
	static const long COLOR_ID_OCEAN_BLUE		= 23;	//RGB(0,102,204)	����ɫ
	static const long COLOR_ID_ICE_BLUE			= 24;	//RGB(204,204,255)	����
	static const long COLOR_ID_DARK_BLUE_PLUS	= 25;	//RGB(0,0,128)		����ɫ
	static const long COLOR_ID_PINK_PLUS		= 26;	//RGB(255,0,255)	�ۺ�ɫ
	static const long COLOR_ID_YELLOW_PLUS		= 27;	//RGB(255,255,0)	��ɫ
	static const long COLOR_ID_TURQUOISE_PLUS	= 28;	//RGB(0,255,255)	����ɫ
	static const long COLOR_ID_VIOLET_PLUS		= 29;	//RGB(128,0,128)	����ɫ
	static const long COLOR_ID_DARK_RED_PLUS	= 30;	//RGB(128,0,0)		���ɫ
	static const long COLOR_ID_TEAL_PLUS		= 31;	//RGB(0,128,128)	��ɫ
	static const long COLOR_ID_BLUE_PLUS		= 32;	//RGB(0,0,255)		��ɫ
	static const long COLOR_ID_SKY_BLUE			= 33;	//RGB(0,204,255)	����ɫ
	static const long COLOR_ID_LIGHT_TURQUOISE	= 34;	//RGB(204,255,255)	ǳ����
	static const long COLOR_ID_LIGHT_GREEN		= 35;	//RGB(204,255,204)	ǳ��ɫ
	static const long COLOR_ID_LIGHT_YELLOW		= 36;	//RGB(255,255,153)	ǳ��ɫ
	static const long COLOR_ID_PALE_BLUE		= 37;	//RGB(153,204,255)	����ɫ
	static const long COLOR_ID_ROSE				= 38;	//RGB(255,153,204)	õ���
	static const long COLOR_ID_LAVENDER			= 39;	//RGB(204,153,255)	����ɫ
	static const long COLOR_ID_TAN				= 40;	//RGB(255,204,153)	��ɫ
	static const long COLOR_ID_LIGHT_BLUE		= 41;	//RGB(51,102,255)	ǳ��ɫ
	static const long COLOR_ID_AQUA				= 42;	//RGB(51,204,204)	ˮ��ɫ
	static const long COLOR_ID_LIME				= 43;	//RGB(153,204,0)	���ɫ
	static const long COLOR_ID_GOLD				= 44;	//RGB(255,204,0)	��ɫ
	static const long COLOR_ID_LIGHT_ORANGE		= 45;	//RGB(255,153,0)	ǳ��ɫ
	static const long COLOR_ID_ORANGE			= 46;	//RGB(255,102,0)	��ɫ
	static const long COLOR_ID_BLUE_GRAY		= 47;	//RGB(102,102,153)	��-��
	static const long COLOR_ID_GRAY_40			= 48;	//RGB(150,150,150)	��-40%
	static const long COLOR_ID_DARK_TEAL		= 49;	//RGB(0,51,102)		����
	static const long COLOR_ID_SEA_GREEN		= 50;	//RGB(51,153,102)	����
	static const long COLOR_ID_DARK_GREEN		= 51;	//RGB(0,51,0)		����
	static const long COLOR_ID_OLIVE_GREEN		= 52;	//RGB(51,51,0)		���ɫ
	static const long COLOR_ID_BROWN			= 53;	//RGB(153,51,0)		��ɫ
	static const long COLOR_ID_PLUM				= 54;	//RGB(153,51,102)	÷��ɫ
	static const long COLOR_ID_INDIGO			= 55;	//RGB(51,51,153)	����
	static const long COLOR_ID_GRAY_80			= 56;	//RGB(51,51,51)		��-80%
public:
	//����ָ��������sheetҳ
	static LPDISPATCH CreateExcelWorksheets(int nSheet);
	//�����С��л�ȡ��Ԫ���ַ�����ʽλ��
	static CXhChar50 GetCellPos(int iCol, int iRow);
	static CXhChar50 GetCellName(int iCol);			
	//���һ�����ݵ�Excel
	static void AddRowToExcelSheet(_Worksheet &excel_sheet, int iRow, CStringArray &str_arr, 
		double *col_arr = NULL, BOOL bWrapText = FALSE);
	//����������
	static void SetMainTitle(_Worksheet excel_sheet, char *cell_start, char *cell_end,
		char *title, double row_height = 20.0, double font_size = 14.0,
		short horizAlignment = 3, short verticalAlignment = 2, bool bSetBorder = true);
	//��������
	static void SetBoldFont(_Worksheet excel_sheet, char *cell_start, char *cell_end);
	static void SetFontBold(_Worksheet excel_sheet, int iStartCol, int iRow, int nCount);
	static void SetFontSize(_Worksheet excel_sheet, const char *cell_start, const char *cell_end,double font_size = 14.0, bool bBold = FALSE);
	static void SetFontColor(_Worksheet excel_sheet, char *cell_start, char *cell_end, long colorId);
	//����ָ������Ԫ������
	static void SetRangeValue(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var);
	static void SetRangeValue(_Worksheet &excel_sheet, int iCol, int iRow, char* sValue);
	//����ָ������Ԫ�����ָ�ʽ
	static void SetRangeNumberFormat(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var);
	static void SetRangeNumberFormatLocal(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var);
	//�趨ָ������Ԫˮƽ���뷽ʽ
	static void SetRangeHorizontalAlignment(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var);
	//�趨ָ������Ԫ��ֱ���뷽ʽ
	static void SetRangeVerticalAlignment(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var);
	//�������ֶ��뷽ʽ(ˮƽ����ֱ)
	static void SetRangeOrientation(_Worksheet excel_sheet, const char *cell_start, const char *cell_end, long degree = 0);
	//�����Զ�����
	static void SetRangeWrapText(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var);
	//����ָ������ı߿�
	static void SetRangeBorders(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT fontSize);
	//�ϲ���
	static void MergeRowRange(_Worksheet excel_sheet, char *cell_start, char *cell_end);
	//�ϲ���
	static void MergeColRange(_Worksheet excel_sheet, char *cell_start, char *cell_end);
	//�����п�
	static void SetRangeColWidth(_Worksheet excel_sheet, double width, char *cell_start,char *cell_end = NULL, BOOL bCenter = FALSE);
	//�����и�
	static void SetRangeRowHeight(_Worksheet excel_sheet, double height, char *cell_start,char *cell_end = NULL, BOOL bCenter = FALSE);
	//���õ�Ԫ�񱳾���ɫ
	static void SetRangeBackColor(_Worksheet excel_sheet, long colorId, int iRow, int iCol, int nCount);
	static void SetRangeBackColor(_Worksheet excel_sheet, long colorId, char *cell_start, char *cell_end = NULL);
	//���õ�Ԫ��%����ʽ
	static void SetRangeNumberFormatLocal(_Worksheet excel_sheet, const char* sLocal, char *cell_start, char *cell_end = NULL);
	//��ȡָ��sheet����
	static BOOL GetExcelContentOfSpecifySheet(const char* sExcelFile, CVariant2dArray &sheetContentMap, int iSheetIndex, CXhTable* pTempTable=NULL,int maxColCount=-1);
	static BOOL GetExcelContentOfSpecifySheet(CExcelOperObject* pExcelOperObj, CVariant2dArray &sheetContentMap, const char* sSheetName, CXhTable* pTempTable = NULL, int maxColCount = -1);
	static BOOL GetExcelContentOfSpecifySheet(const char* sExcelFile, CVariant2dArray &sheetContentMap, const char* sSheetName, CXhTable* pTempTable = NULL, int maxColCount = -1);
	static BOOL GetExcelContentOfSpecifySheet(CExcelOperObject* pExcelOperObj, CVariant2dArray &sheetContentMap, int iSheetIndex, CXhTable* pTempTable = NULL, int maxColCount = -1);
	static BOOL GetExcelContentOfSpecifySheet(const char* sExcelFile, CStringArray &sheetNameArr);
	static BOOL GetExcelContentOfSpecifySheet(CExcelOperObject* pExcelOperObj, CStringArray &sheetNameArr);
	//����sheet����ȡsheet���
	static int  GetExcelIndexOfSpecifySheet(CExcelOperObject* pExcelOperObj, const char* sSheetName);
	//����XhTable����Excel
	static BOOL  ExportExcle(CXhTable* pTbl);
};

#endif