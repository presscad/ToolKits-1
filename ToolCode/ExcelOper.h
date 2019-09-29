#ifndef __EXCEL_OPER_
#define __EXCEL_OPER_

#include "excel9.h"
#include "Variant.h"
#include "XhCharString.h"
#include "LogFile.h"

class CExcelOperObject
{
	bool m_bFileOpened;
	_Application excel;          // Excel����
	Workbooks    excel_workbooks;// ����������
	_Workbook    excel_workbook; // ������
	
public:
	BOOL OpenExcelFile(const char* sExcelFile)
	{	//��ָ����Excel�ļ�
		int nRetCode = 0;
		Sheets       excel_sheets;
		LPDISPATCH   pDisp;
		LPUNKNOWN    pUnk ;
		CLSID        clsid;
		if(!ExcelCLSIDFromProgID(clsid))
		{
			//AfxMessageBox("Excel program not found");
			logerr.Log("δ�ҵ�Excel����");
			return false;
		}
		if(::GetActiveObject(clsid,NULL,&pUnk) == S_OK)
		{
			VERIFY(pUnk->QueryInterface(IID_IDispatch,(void**) &pDisp) == S_OK);
			excel.AttachDispatch(pDisp);
			pUnk->Release();
		}
		else if(!excel.CreateDispatch(clsid))
		{
			//AfxMessageBox("Excel program not found");
			logerr.Log("δ�ҵ�Excel����");
			return false;
		}
		LPDISPATCH pWorkbooks;
		VERIFY(pWorkbooks = excel.GetWorkbooks());
		excel_workbooks.AttachDispatch(pWorkbooks);
		LPDISPATCH pWorkbook = NULL;
		pWorkbook=excel_workbooks.Open(sExcelFile);
		excel_workbook.AttachDispatch(pWorkbook);
		return m_bFileOpened=true;
	}
	LPDISPATCH GetWorksheets()
	{
		if(m_bFileOpened)
			return excel_workbook.GetWorksheets();
		else
			return NULL;
	}
	void ExitApplication()
	{
		if(!m_bFileOpened)
			return;
	}
	void Save()
	{
		excel_workbook.Save();
		excel_workbook.ReleaseDispatch();
		excel_workbooks.ReleaseDispatch();
		excel.Quit();
		excel.ReleaseDispatch();
	}
	LPDISPATCH ClearContent(int index)
	{
		LPDISPATCH pWorksheets = GetWorksheets();
		ASSERT(pWorksheets);
		Sheets excel_sheets;
		excel_sheets.AttachDispatch(pWorksheets);
		LPDISPATCH pWorksheet3 = excel_sheets.GetItem(COleVariant((short)index));
		if(pWorksheet3)//�Ѵ���sheet����ɾ��
		{
			_Worksheet excel_sheet;
			excel_sheet.AttachDispatch(pWorksheet3, FALSE);
			excel_sheet.GetRows()->Release();
			Range excel_range=excel_sheet.GetUsedRange();
			excel_range.SetRowHeight(COleVariant(15.0));
			excel_range.UnMerge();
			return pWorksheet3;
		}
		else
			return Insert(index);
	}
	LPDISPATCH GetValidWorkSheet(int index)
	{
		LPDISPATCH pWorksheets = GetWorksheets();//CExcelOper::CreateExcelWorksheets(2);
		ASSERT(pWorksheets);
		Sheets excel_sheets;
		excel_sheets.AttachDispatch(pWorksheets);
		LPDISPATCH pWorksheet3 = NULL;
		try
		{
			pWorksheet3 = excel_sheets.GetItem(COleVariant((short)index));
		}
		catch (...)
		{	//excel���в�������Ӧ������sheet���򽫱����ɵ����һ��
			if(excel_sheets.GetCount()<index)
				index=excel_sheets.GetCount();
			if(index>=1)
			{
				LPDISPATCH pWorksheet2 = excel_sheets.GetItem(COleVariant((short)(index-1)));
				_Worksheet excel_sheet;
				excel_sheet.AttachDispatch(pWorksheet2, FALSE);
				excel_sheet.Select();
				pWorksheet3 = excel_sheets.Add(vtMissing, _variant_t(pWorksheet2), COleVariant((long)1), vtMissing);
			}
			else
				pWorksheet3 = excel_sheets.Add(vtMissing, vtMissing, COleVariant((long)1), vtMissing);
			
		}
		return pWorksheet3;
	}
	LPDISPATCH Insert(int index)
	{
		return InsertSheetByAfterIndex(index-1);
	}
	LPDISPATCH InsertSheetByAfterIndex(int afterIndex)
	{
		if(afterIndex==-1)
			afterIndex=0;
		LPDISPATCH pWorksheets = GetWorksheets();//CExcelOper::CreateExcelWorksheets(2);
		ASSERT(pWorksheets);
		Sheets excel_sheets;
		excel_sheets.AttachDispatch(pWorksheets);
		LPDISPATCH pWorksheet3 = NULL;
		LPDISPATCH pWorksheet2 = excel_sheets.GetItem(COleVariant((short)afterIndex));
		if(pWorksheet2)//after��sheet�������ڣ�����뵽���
			pWorksheet3 = excel_sheets.Add(vtMissing, _variant_t(pWorksheet2), COleVariant((long)1), vtMissing);
		else
			pWorksheet3 = InsertLastValidWorkSheet();
		_Worksheet excel_sheet;
		excel_sheet.AttachDispatch(pWorksheet2, FALSE);
		excel_sheet.Select();
		return pWorksheet3;
	}
	LPDISPATCH InsertLastValidWorkSheet()
	{
		LPDISPATCH pWorksheets = GetWorksheets();
		ASSERT(pWorksheets);
		Sheets excel_sheets;
		excel_sheets.AttachDispatch(pWorksheets);
		
		return Insert(excel_sheets.GetCount() + 1);
	}
	CExcelOperObject(){m_bFileOpened=false;}
	~CExcelOperObject()
	{
		if(m_bFileOpened)
		{
			excel_workbook.ReleaseDispatch();
			excel_workbooks.ReleaseDispatch();
			excel.Quit();	//������д���˳��Ҫ���ˣ������ܳ��׹ر�Excel���� wht 18-07-30
			excel.ReleaseDispatch();
		}
	}
	static bool ExcelCLSIDFromProgID(CLSID &clsid)
	{
		if( ::CLSIDFromProgID(L"Excel.Application", &clsid)==S_OK||		//Excel 2000
			::CLSIDFromProgID(L"Excel.Application.9", &clsid)==S_OK||	//Excel 2000
			::CLSIDFromProgID(L"Excel.Application.10", &clsid)==S_OK||	//Excel xp
			::CLSIDFromProgID(L"Excel.Application.11", &clsid)==S_OK||	//Excel 2003
			::CLSIDFromProgID(L"Excel.Application.12", &clsid)==S_OK||	//Excel 2007
			::CLSIDFromProgID(L"Excel.Application.13", &clsid)==S_OK||	//Excel 2007
			::CLSIDFromProgID(L"Excel.Application.14", &clsid)==S_OK||	//Excel 2010
			::CLSIDFromProgID(L"Excel.Application.15", &clsid)==S_OK||	//Excel 2013
			::CLSIDFromProgID(L"Excel.Application.16", &clsid)==S_OK)	//Excel 2016
			return true;
		else
			return false;
	}
};

class CExcelOper
{
public:
	//�����С��л�ȡ��Ԫ���ַ�����ʽλ��
	static CXhChar50 GetCellPos(int iCol,int iRow)
	{
		int nLen=iCol/26;
		int nSubLen=iCol%26;
		if(nLen>26)
			return "";	//��ʱ��֧��A~AZ
		CXhChar50 cell;
		if(nLen>0)
			cell.Printf("%c%c%d",'A'+nLen-1,'A'+nSubLen,iRow);
		else
			cell.Printf("%c%d",'A'+nSubLen,iRow);
		return cell;
	}
	static CXhChar50 GetCellName(int iCol)
	{
		int nLen=iCol/26;
		int nSubLen=iCol%26;
		if(nLen>26)
			return "";	//��ʱ��֧��A~AZ
		CXhChar50 cell;
		if(nLen>0)
			cell.Printf("%c%c",'A'+nLen-1,'A'+nSubLen);
		else
			cell.Printf("%c",'A'+nSubLen);
		return cell;
	}
	static LPDISPATCH CreateExcelWorksheets(int nSheet)
	{	//�����EXCEL�ļ�
		LPDISPATCH pDisp;
		LPUNKNOWN pUnk;
		CLSID clsid;
		_Application excel_app;
		CExcelOperObject::ExcelCLSIDFromProgID(clsid);
		if(::GetActiveObject(clsid, NULL, &pUnk) == S_OK) 
		{
			VERIFY(pUnk->QueryInterface(IID_IDispatch,(void**) &pDisp) == S_OK);
			excel_app.AttachDispatch(pDisp);
			pUnk->Release();
		} 
		else if(!excel_app.CreateDispatch(clsid)) 
		{
			AfxMessageBox("Excel program not found");
			return NULL;
		}
		LPDISPATCH pWorkbooks;

		CWnd* pWnd = CWnd::FindWindow("XLMAIN", NULL);
		if (pWnd != NULL) 
		{
			pWnd->ShowWindow(SW_SHOWNORMAL);
			pWnd->UpdateWindow();
			pWnd->BringWindowToTop();
		}

		excel_app.SetSheetsInNewWorkbook(nSheet);	//�����ɹ�������Ĭ�Ϲ������������

		Workbooks excel_workbooks;
		_Workbook excel_workbook;
		VERIFY(pWorkbooks = excel_app.GetWorkbooks());
		excel_workbooks.AttachDispatch(pWorkbooks);

		LPDISPATCH pWorkbook = excel_workbooks.Add(); // Save the pointer for
		excel_workbook.AttachDispatch(pWorkbook);
		LPDISPATCH pWorksheets = excel_workbook.GetWorksheets();
		return pWorksheets;
	}
	//�ú��������ã�ʹ�����º�����Excel����һֱռ�ã��޷��ͷ�
	//ʹ��CExcelOperObject::OpenExcelFile()ȡ��	wht 19-01-10
	/*static LPDISPATCH OpenExcelWorksheets(const char* file_name,bool show_window=false)
	{
		_Application excel;          // Excel����
		LPDISPATCH   pDisp;
		LPUNKNOWN    pUnk ;
		CLSID        clsid;
		CExcelOperObject::ExcelCLSIDFromProgID(clsid);
		if(::GetActiveObject(clsid,NULL,&pUnk) == S_OK)
		{
			VERIFY(pUnk->QueryInterface(IID_IDispatch,(void**) &pDisp) == S_OK);
			excel.AttachDispatch(pDisp);
			pUnk->Release();
		}
		else if(!excel.CreateDispatch(clsid))
		{
			printf("Excel program not found");
			return false;
		}
		if(show_window)
		{
			CWnd* pWnd = CWnd::FindWindow("XLMAIN", NULL);
			if (pWnd != NULL) 
			{
				pWnd->ShowWindow(SW_SHOWNORMAL);
				pWnd->UpdateWindow();
				pWnd->BringWindowToTop();
			}
		}
		try
		{
			int nSheetNum=0;
			LPDISPATCH pWorkbooks;
			VERIFY(pWorkbooks = excel.GetWorkbooks());
			Workbooks    excel_workbooks;// ����������
			excel_workbooks.AttachDispatch(pWorkbooks);
			LPDISPATCH pWorkbook = NULL;
			pWorkbook=excel_workbooks.Open(file_name);
			_Workbook    excel_workbook; // ������
			excel_workbook.AttachDispatch(pWorkbook);
			LPDISPATCH pWorksheets = excel_workbook.GetWorksheets();
			excel_workbook.ReleaseDispatch();
			excel_workbooks.ReleaseDispatch();
			excel.Quit();
			excel.ReleaseDispatch();
			return pWorksheets;
		}
		catch (char *)
		{
			return NULL;
		}
	}*/
	//���һ�����ݵ�Excel
	static void AddRowToExcelSheet(_Worksheet &excel_sheet,int iRow,CStringArray &str_arr,double *col_arr=NULL,BOOL bWrapText=FALSE)
	{
		Range excel_range;
		LPDISPATCH pRange;
		for(int i=0;i<str_arr.GetSize();i++)
		{
			pRange=excel_sheet.GetRange(COleVariant(GetCellPos(i,iRow)));
			excel_range.AttachDispatch(pRange,FALSE);
			excel_range.SetValue(COleVariant(str_arr[i]));	//,VT_BSTR
			if(col_arr)
				excel_range.SetColumnWidth(COleVariant(col_arr[i]));
			if(bWrapText)
				excel_range.SetWrapText(COleVariant((BYTE)1));
			excel_range.DetachDispatch();
			pRange->Release();

		}
	}
	//����������
	static void SetMainTitle(_Worksheet excel_sheet,char *cell_start, char *cell_end,
							char *title,double row_height=20.0,double font_size=14.0,
							short horizAlignment=3,short verticalAlignment=2,bool bSetBorder=true)
	{
		FontLegacy font;
		LPDISPATCH pRange;
		Range excel_range;
		pRange = excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.Merge(COleVariant((BYTE)1));
		excel_range.SetValue2(COleVariant(title));
		excel_range.SetHorizontalAlignment(COleVariant(horizAlignment));
		excel_range.SetVerticalAlignment(COleVariant(verticalAlignment));
		excel_range.SetRowHeight(COleVariant(row_height));
		if(bSetBorder)
		{
			Borders borders;
			Border border;
			borders.AttachDispatch(excel_range.GetBorders(),FALSE);
			//borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
			border.AttachDispatch(borders.GetItem(9),FALSE);//xlBottom = 9
			border.SetLineStyle(COleVariant((short)-4119));	//xlDouble =-4119;
			border.SetWeight(COleVariant((short)4));		//xlThin=2;xlThick=4;
			border.ReleaseDispatch();
			borders.ReleaseDispatch();
		}
		font.AttachDispatch(excel_range.GetFont(),FALSE);
		COleVariant covTrue((short)TRUE);
		font.SetBold(covTrue);
		font.SetSize(COleVariant(font_size));
		font.ReleaseDispatch();
		excel_range.ReleaseDispatch();
	}
	static void SetBoldFont(_Worksheet excel_sheet,char *cell_start, char *cell_end)
	{
		LPDISPATCH pRange;
		Range excel_range;
		FontLegacy font;
		if(cell_end!=NULL)
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		else 
			pRange=excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		font.AttachDispatch(excel_range.GetFont(),FALSE);
		COleVariant covTrue((short)TRUE);
		font.SetBold(covTrue);
		font.ReleaseDispatch();
		excel_range.ReleaseDispatch();
	}

	static void SetFontBold(_Worksheet excel_sheet,int iStartCol, int iRow, int nCount)
	{
		for (int i=0;i<nCount;i++)
		{
			FontLegacy font;
			Range excel_range;
			LPDISPATCH pRange;
			CXhChar16 cell_start=GetCellPos(iStartCol,iRow);
			iStartCol++;	
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_start));
			excel_range.AttachDispatch(pRange,FALSE);
			font.AttachDispatch(excel_range.GetFont(),FALSE);
			COleVariant covTrue((short)TRUE);
			font.SetBold(covTrue);
			font.ReleaseDispatch();
			excel_range.ReleaseDispatch();
		}
	}
	static void SetFontSize(_Worksheet excel_sheet,const char *cell_start,const char *cell_end,
							double font_size=14.0,bool bBold=FALSE)
	{
		FontLegacy font;
		Range excel_range;
		LPDISPATCH pRange;
		if(cell_end!=NULL)
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		else 
			pRange=excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		font.AttachDispatch(excel_range.GetFont(),FALSE);
		font.SetBold(COleVariant((short)bBold));
		font.SetSize(COleVariant(font_size));
		font.ReleaseDispatch();
		excel_range.ReleaseDispatch();
	}
	// ����������ɫ
	static void SetFontColor(_Worksheet excel_sheet,char *cell_start, char *cell_end,long colorId)
	{
		FontLegacy font;
		Range excel_range;
		LPDISPATCH pRange;
		if(cell_end!=NULL)
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		else 
			pRange=excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		font.AttachDispatch(excel_range.GetFont(),FALSE);
		font.SetColorIndex(COleVariant(colorId));
		font.ReleaseDispatch();
		excel_range.ReleaseDispatch();
	}

	//����ָ������Ԫ������
	static void SetRangeValue(_Worksheet excel_sheet,char *cell_start, char *cell_end,VARIANT var)
	{
		LPDISPATCH pRange;
		Range excel_range;
		if(cell_end!=NULL)
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		else 
			pRange=excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.SetValue2(var);
		excel_range.DetachDispatch();
		pRange->Release();
	}
	//����ָ����Ԫ������
	static void SetRangeValue(_Worksheet &excel_sheet,int iCol,int iRow,char* sValue)
	{
		LPDISPATCH pRange;
		Range excel_range;
		pRange=excel_sheet.GetRange(COleVariant(GetCellPos(iCol,iRow)));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.SetValue2(COleVariant(sValue));
		excel_range.DetachDispatch();
		pRange->Release();
	}
	//����ָ������Ԫ�����ָ�ʽ
	static void SetRangeNumberFormat(_Worksheet excel_sheet,char *cell_start, char *cell_end, VARIANT var)
	{
		LPDISPATCH pRange;
		Range excel_range;
		if(cell_end!=NULL)
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		else 
			pRange=excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.SetNumberFormat(var);
		excel_range.DetachDispatch();
		pRange->Release();
	}
	//����ָ������Ԫ�����ָ�ʽ
	static void SetRangeNumberFormatLocal(_Worksheet excel_sheet,char *cell_start, char *cell_end, VARIANT var)
	{
		LPDISPATCH pRange;
		Range excel_range;
		if(cell_end!=NULL)
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		else 
			pRange=excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.SetNumberFormatLocal(var);
		excel_range.SetHorizontalAlignment(COleVariant((long)4));
		excel_range.DetachDispatch();
		pRange->Release();
	}
	//�趨ָ������Ԫˮƽ���뷽ʽ
	//�Ҷ��� COleVariant((long)2)
	//��  �� COleVariant((long)3)
	//����� COleVariant((long)4)
	//����루xlLeft����-4131
	//���У�xlCenter����-4108 
	//�¶��루xlBottom����-4107
	//�϶��루xlTop����-4160
	//�Ҷ��루xlRight����-4152 
	static void SetRangeHorizontalAlignment(_Worksheet excel_sheet,char *cell_start, char *cell_end, VARIANT var)
	{
		LPDISPATCH pRange;
		Range excel_range;
		if(cell_end!=NULL)
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		else 
			pRange=excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.SetHorizontalAlignment(var);
		excel_range.DetachDispatch();
		pRange->Release();
	}
	//�趨ָ������Ԫ��ֱ���뷽ʽ
	//��  �� COleVariant((long)-4108)
	// -4108:���У�-4160:���ϣ�-4107:����
	static void SetRangeVerticalAlignment(_Worksheet excel_sheet,char *cell_start, char *cell_end, VARIANT var)
	{
		LPDISPATCH pRange;
		Range excel_range;
		if(cell_end!=NULL)
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		else 
			pRange=excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.SetVerticalAlignment(var);
		excel_range.DetachDispatch();
		pRange->Release();
	}
	//�������ֶ��뷽ʽ(ˮƽ����ֱ)
	static void SetRangeOrientation(_Worksheet excel_sheet,const char *cell_start, const char *cell_end, long degree=0)
	{
		LPDISPATCH pRange;
		Range excel_range;
		if(cell_end!=NULL)
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		else 
			pRange=excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.SetOrientation(COleVariant(degree));
		excel_range.DetachDispatch();
		pRange->Release();
	}
	//�����Զ�����
	static void SetRangeWrapText(_Worksheet excel_sheet,char *cell_start, char *cell_end, VARIANT var)
	{
		LPDISPATCH pRange;
		Range excel_range;
		if(cell_end!=NULL)
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		else 
			pRange=excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.SetWrapText(var);	//�Զ�����
		excel_range.DetachDispatch();
		pRange->Release();
	}
	//����ָ������ı߿�
	static void SetRangeBorders(_Worksheet excel_sheet,char *cell_start, char *cell_end, VARIANT fontSize)
	{
		Borders borders;
		FontLegacy font;
		LPDISPATCH pRange;
		Range excel_range;
		if(cell_end!=NULL)
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		else 
			pRange=excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		borders.AttachDispatch(excel_range.GetBorders(),FALSE);
		borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
		borders.SetLineStyle(COleVariant((long)1));	//xlSingle
		borders.ReleaseDispatch();
		font.AttachDispatch(excel_range.GetFont(),FALSE);
		font.SetSize(fontSize);
		font.ReleaseDispatch();
		excel_range.ReleaseDispatch();
	}
	//�ϲ���
	static void MergeRowRange(_Worksheet excel_sheet,char *cell_start, char *cell_end)
	{
		LPDISPATCH pRange;
		Range excel_range;
		pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange);
		excel_range.Merge(COleVariant((BYTE)0));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.ReleaseDispatch();
	}
	//�ϲ���
	static void MergeColRange(_Worksheet excel_sheet,char *cell_start, char *cell_end)
	{
		LPDISPATCH pRange;
		Range excel_range;
		pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		excel_range.AttachDispatch(pRange);
		excel_range.Merge(COleVariant((BYTE)1));
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
		excel_range.ReleaseDispatch();
	}
	//�����п�
	static void SetRangeColWidth(_Worksheet excel_sheet,double width,char *cell_start, 
								char *cell_end=NULL,BOOL bCenter=FALSE)
	{
		LPDISPATCH pRange;
		Range excel_range;
		if(cell_end!=NULL)
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		else 
			pRange=excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.SetColumnWidth(COleVariant(width));
		if(bCenter)
		{
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
		}
		excel_range.DetachDispatch();
		pRange->Release();
	}
	//
	static void SetRangeRowHeight(_Worksheet excel_sheet,double height,char *cell_start, 
								char *cell_end=NULL,BOOL bCenter=FALSE)
	{
		LPDISPATCH pRange;
		Range excel_range;
		if(cell_end!=NULL)
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		else 
			pRange=excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.SetRowHeight(COleVariant(height));
		if(bCenter)
		{
			excel_range.SetHorizontalAlignment(COleVariant((short)3));
			excel_range.SetVerticalAlignment(COleVariant((short)2));
		}
		excel_range.DetachDispatch();
		pRange->Release();
	}

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

	static void SetRangeBackColor(_Worksheet excel_sheet,long colorId,int iRow,int iCol,int nCount)
	{
		for (int i=0;i<nCount;i++)
		{
			LPDISPATCH pRange;
			Range excel_range;
			CXhChar16 sStart=GetCellPos(iCol,iRow);
			pRange=excel_sheet.GetRange(COleVariant((char*)sStart));
			excel_range.AttachDispatch(pRange,FALSE);
			FontLegacy interior = excel_range.GetInterior();
			interior.SetColorIndex(COleVariant(colorId));
			excel_range.DetachDispatch();
			pRange->Release();
			iCol++;
		}
	}
	//���õ�Ԫ��%����ʽ
	static void SetRangeNumberFormatLocal(_Worksheet excel_sheet,const char* sLocal ,char *cell_start,char *cell_end=NULL)
	{
		LPDISPATCH pRange;
		Range excel_range;;
		if(cell_end!=NULL)
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		else 
			pRange=excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		excel_range.SetNumberFormatLocal(COleVariant(sLocal));
		excel_range.DetachDispatch();
		pRange->Release();
	}
	//���õ�Ԫ�񱳾���ɫ
	static void SetRangeBackColor(_Worksheet excel_sheet,long colorId,char *cell_start,char *cell_end=NULL)
	{
		LPDISPATCH pRange;
		Range excel_range;;
		if(cell_end!=NULL)
			pRange=excel_sheet.GetRange(COleVariant(cell_start),COleVariant(cell_end));
		else 
			pRange=excel_sheet.GetRange(COleVariant(cell_start));
		excel_range.AttachDispatch(pRange,FALSE);
		FontLegacy interior = excel_range.GetInterior();
		interior.SetColorIndex(COleVariant(colorId));
		excel_range.DetachDispatch();
		pRange->Release();
	}
	static BOOL GetExcelContentOfSpecifySheet(const char* sExcelFile,CVariant2dArray &sheetContentMap,int iSheetIndex)
	{
		CExcelOperObject excelobj;
		if(excelobj.OpenExcelFile(sExcelFile))
			return GetExcelContentOfSpecifySheet(&excelobj,sheetContentMap,iSheetIndex);
		else
			return FALSE;
	}
	static BOOL GetExcelContentOfSpecifySheet(CExcelOperObject* pExcelOperObj,CVariant2dArray &sheetContentMap,const char* sSheetName)
	{
		int iSheet = GetExcelIndexOfSpecifySheet(pExcelOperObj,sSheetName);
		if(iSheet>0)
			return GetExcelContentOfSpecifySheet(pExcelOperObj,sheetContentMap,iSheet);
		else
			return FALSE;
	}
	static BOOL GetExcelContentOfSpecifySheet(const char* sExcelFile,CVariant2dArray &sheetContentMap,const char* sSheetName)
	{
		CStringArray sheetNameArr;
		CExcelOperObject excelobj;
		if(!excelobj.OpenExcelFile(sExcelFile))
			return FALSE;
		if(GetExcelContentOfSpecifySheet(&excelobj,sheetNameArr))
		{
			int iSheet=-1;
			for(int i=0;i<sheetNameArr.GetSize();i++)
			{
				if(sheetNameArr[i].CompareNoCase(sSheetName)==0)
				{
					iSheet=i+1;
					break;
				}
			}
			if(iSheet>0)
				return GetExcelContentOfSpecifySheet(&excelobj,sheetContentMap,iSheet);
			else
				return FALSE;
		}
		else
			return FALSE;
	}
	static BOOL GetExcelContentOfSpecifySheet(CExcelOperObject* pExcelOperObj,CVariant2dArray &sheetContentMap,int iSheetIndex)
	{	//��ָ����Excel�ļ�
		try
		{
			int nRetCode = 0;
			_Worksheet   excel_sheet;    // ������
			Sheets       excel_sheets;
			LPDISPATCH   pRange;
			LPDISPATCH   pWorksheets = pExcelOperObj->GetWorksheets();
			ASSERT(pWorksheets != NULL);
			excel_sheets.AttachDispatch(pWorksheets);
			LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheetIndex));
			excel_sheet.AttachDispatch(pWorksheet);
			//excel_sheet.Select();
			//1.����Excel������������
			Range excel_usedRange, excel_range;
			excel_usedRange.AttachDispatch(excel_sheet.GetUsedRange());
			excel_range.AttachDispatch(excel_usedRange.GetRows());
			long nRowNum = excel_range.GetCount();
			excel_range.AttachDispatch(excel_usedRange.GetColumns());
			long nColNum = excel_range.GetCount();
			//2.��ȡExcelָ��Sheet���ݴ洢��sheetContentMap��
			CXhChar50 cell = GetCellPos(nColNum, nRowNum);
			pRange = excel_sheet.GetRange(COleVariant("A1"), COleVariant(cell));
			excel_range.AttachDispatch(pRange, FALSE);
			sheetContentMap.var = excel_range.GetValue();
			excel_range.ReleaseDispatch();
			excel_sheet.ReleaseDispatch();
			excel_sheets.ReleaseDispatch();
			excel_usedRange.ReleaseDispatch();
			return true;
		}
		catch (CString sError)
		{
			logerr.Log(sError);
			return false;
		}
	}
	static BOOL GetExcelContentOfSpecifySheet(const char* sExcelFile,CStringArray &sheetNameArr)
	{
		CExcelOperObject excelobj;
		if(excelobj.OpenExcelFile(sExcelFile))
			return GetExcelContentOfSpecifySheet(&excelobj,sheetNameArr);
		else
			return FALSE;
	}
	static BOOL GetExcelContentOfSpecifySheet(CExcelOperObject* pExcelOperObj,CStringArray &sheetNameArr)
	{	//��ָ����Excel�ļ�
		int nRetCode = 0;
		_Worksheet   excel_sheet;    // ������
		Sheets       excel_sheets;

		LPDISPATCH pWorksheets = pExcelOperObj->GetWorksheets();
		ASSERT(pWorksheets != NULL);
		excel_sheets.AttachDispatch(pWorksheets);
		LPDISPATCH pWorksheet=NULL;
		int nSheetCount=excel_sheets.GetCount();
		sheetNameArr.RemoveAll();
		if(nSheetCount>0)
		{
			sheetNameArr.SetSize(nSheetCount);
			for(int i=1;i<excel_sheets.GetCount()+1;i++)
			{
				pWorksheet = excel_sheets.GetItem(COleVariant((short)i));
				excel_sheet.AttachDispatch(pWorksheet);
				sheetNameArr[i-1]=excel_sheet.GetName();
				excel_sheet.ReleaseDispatch();
			}
		}
		excel_sheets.ReleaseDispatch();
		return true;
	}
	static int  GetExcelIndexOfSpecifySheet(CExcelOperObject* pExcelOperObj,const char* sSheetName)
	{	//��ָ����Excel�ļ�
		int nRetCode = 0;
		_Worksheet   excel_sheet;    // ������
		Sheets       excel_sheets;
		CStringArray sheetNameArr;
		LPDISPATCH pWorksheets = pExcelOperObj->GetWorksheets();
		ASSERT(pWorksheets != NULL);
		excel_sheets.AttachDispatch(pWorksheets);
		LPDISPATCH pWorksheet=NULL;
		int nSheetCount=excel_sheets.GetCount();
		if(nSheetCount>0)
		{
			sheetNameArr.SetSize(nSheetCount);
			for(int i=1;i<excel_sheets.GetCount()+1;i++)
			{
				pWorksheet = excel_sheets.GetItem(COleVariant((short)i));
				excel_sheet.AttachDispatch(pWorksheet);
				sheetNameArr[i-1]=excel_sheet.GetName();
				excel_sheet.ReleaseDispatch();
			}
		}
		excel_sheets.ReleaseDispatch();
		int iSheet=-1;
		for(int i=0;i<sheetNameArr.GetSize();i++)
		{
			if(sheetNameArr[i].CompareNoCase(sSheetName)==0)
			{
				iSheet=i+1;
				break;
			}
		}
		return iSheet;
	}
};

#endif