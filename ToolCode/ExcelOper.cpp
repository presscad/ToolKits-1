#include "stdafx.h"
#include "ExcelOper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// ����AttachDispatch()��ReleaseDispatch()��DetachDispatch()�÷�ͳһ�淶˵��
// �������ַ�ʽ���ɱ�֤�����ͷ���Դ����������������õ�һ�ַ�ʽ��
// ��ʽһ��AttachDispatchʱ֧���Զ�ʹ�ã��ڶ�������ΪTure������ʾ�����£�
//  Range excel_range;
//  LPDISPATCH pRange1,pRange2; 
//  pRange1 = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
//  excel_range.AttachDispatch(pRange1);
//  ...
//  excel_range.AttachDispatch(pRange2);	//������һ������ʱ���Զ��ͷ�ǰһ������
//  ...
//  excel_range.ReleaseDispatch();			//�����Ҫ����һ���ͷź���
//
// ��ʽ����AttachDispatchʱ��֧���Զ�ʹ�ã��ڶ�������ΪFalse�����ֶ��ͷš�����ʾ�����£�
//  Range excel_range;
//  LPDISPATCH pRange1,pRange2; 
//  pRange1 = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
//  excel_range.AttachDispatch(pRange1,FALSE);
//  ...
//  excel_range.DetachDispatch();			//ʹ�����֮��Ͽ�����
//  pRange1->Release();						//ʹ�����֮�����ֶ��ͷ�
//  excel_range.AttachDispatch(pRange2,FALSE);	
//  ...
//  excel_range.DetachDispatch();			//ʹ�����֮��Ͽ�����
//  pRange2->Release();						//ʹ�����֮�����ֶ��ͷ�
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//CExcelOperObject
CExcelOperObject::CExcelOperObject()
{
	m_bFileOpened = false;
}
CExcelOperObject::~CExcelOperObject()
{
	if (m_bFileOpened)
	{
		COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
		excel_workbook.Close(covOptional,covOptional,covOptional);
		excel_workbooks.Close();
		excel_workbook.ReleaseDispatch();
		excel_workbooks.ReleaseDispatch();
		excel.Quit();	//������д���˳��Ҫ���ˣ������ܳ��׹ر�Excel���� wht 18-07-30
		excel.ReleaseDispatch();
	}
}
BOOL CExcelOperObject::OpenExcelFile(const char* sExcelFile)
{	//��ָ����Excel�ļ�
	int nRetCode = 0;
	LPDISPATCH   pDisp;
	LPUNKNOWN    pUnk;
	CLSID        clsid;
	if (!ExcelCLSIDFromProgID(clsid))
	{
		//AfxMessageBox("Excel program not found");
		logerr.Log("δ�ҵ�Excel����");
		return false;
	}
	if (::GetActiveObject(clsid, NULL, &pUnk) == S_OK)
	{
		VERIFY(pUnk->QueryInterface(IID_IDispatch, (void**)&pDisp) == S_OK);
		excel.AttachDispatch(pDisp);
		pUnk->Release();
	}
	else if (!excel.CreateDispatch(clsid))
	{
		//AfxMessageBox("Excel program not found");
		logerr.Log("δ�ҵ�Excel����");
		return false;
	}
	LPDISPATCH pWorkbooks;
	VERIFY(pWorkbooks = excel.GetWorkbooks());
	excel_workbooks.AttachDispatch(pWorkbooks);
	LPDISPATCH pWorkbook = NULL;
	pWorkbook = excel_workbooks.Open(sExcelFile);
	excel_workbook.AttachDispatch(pWorkbook);
	return m_bFileOpened = true;
}
LPDISPATCH CExcelOperObject::GetWorksheets()
{
	if (m_bFileOpened)
		return excel_workbook.GetWorksheets();
	else
		return NULL;
}
int CExcelOperObject::GetWorkSheetCount()
{
	int nSheetNum = 0;
	if (m_bFileOpened)
	{
		LPDISPATCH pWorksheets = GetWorksheets();
		if (pWorksheets)
		{
			ASSERT(pWorksheets != NULL);
			Sheets       excel_sheets;	//��������
			excel_sheets.AttachDispatch(pWorksheets);
			nSheetNum = excel_sheets.GetCount();
			excel_sheets.ReleaseDispatch();
		}
	}
	return nSheetNum;
}
void CExcelOperObject::ExitApplication()
{
	if (!m_bFileOpened)
		return;
}
void CExcelOperObject::Save()
{
	excel_workbook.Save();
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	excel_workbook.Close(covOptional, covOptional, covOptional);
	excel_workbooks.Close();
	excel_workbook.ReleaseDispatch();
	excel_workbooks.ReleaseDispatch();
	excel.Quit();
	excel.ReleaseDispatch();
}
LPDISPATCH CExcelOperObject::ClearContent(int index)
{
	LPDISPATCH pWorksheets = GetWorksheets();
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet3 = excel_sheets.GetItem(COleVariant((short)index));
	if (pWorksheet3)//�Ѵ���sheet����ɾ��
	{
		_Worksheet excel_sheet;
		excel_sheet.AttachDispatch(pWorksheet3);
		excel_sheet.GetRows()->Release();
		Range excel_range = excel_sheet.GetUsedRange();
		excel_range.SetRowHeight(COleVariant(15.0));
		excel_range.UnMerge();
		excel_sheet.ReleaseDispatch();
		excel_range.ReleaseDispatch();
		excel_sheets.ReleaseDispatch();
		return pWorksheet3;
	}
	else
	{
		excel_sheets.ReleaseDispatch();
		return Insert(index);
	}
}
LPDISPATCH CExcelOperObject::GetValidWorkSheet(int index)
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
		if (excel_sheets.GetCount() < index)
			index = excel_sheets.GetCount();
		if (index >= 1)
		{
			LPDISPATCH pWorksheet2 = excel_sheets.GetItem(COleVariant((short)(index - 1)));
			_Worksheet excel_sheet;
			excel_sheet.AttachDispatch(pWorksheet2);
			excel_sheet.Select();
			excel_sheet.ReleaseDispatch();
			pWorksheet3 = excel_sheets.Add(vtMissing, _variant_t(pWorksheet2), COleVariant((long)1), vtMissing);
		}
		else
			pWorksheet3 = excel_sheets.Add(vtMissing, vtMissing, COleVariant((long)1), vtMissing);
	}
	excel_sheets.ReleaseDispatch();
	return pWorksheet3;
}
LPDISPATCH CExcelOperObject::InsertSheetByAfterIndex(int afterIndex)
{
	if (afterIndex == -1)
		afterIndex = 0;
	LPDISPATCH pWorksheets = GetWorksheets();//CExcelOper::CreateExcelWorksheets(2);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet3 = NULL;
	LPDISPATCH pWorksheet2 = excel_sheets.GetItem(COleVariant((short)afterIndex));
	if (pWorksheet2)//after��sheet�������ڣ�����뵽���
		pWorksheet3 = excel_sheets.Add(vtMissing, _variant_t(pWorksheet2), COleVariant((long)1), vtMissing);
	else
		pWorksheet3 = InsertLastValidWorkSheet();
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet2);
	excel_sheet.Select();
	excel_sheet.ReleaseDispatch();
	excel_sheets.ReleaseDispatch();
	return pWorksheet3;
}
LPDISPATCH CExcelOperObject::InsertLastValidWorkSheet()
{
	LPDISPATCH pWorksheets = GetWorksheets();
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorkSheet = Insert(excel_sheets.GetCount() + 1);
	excel_sheets.ReleaseDispatch();
	return pWorkSheet;
}
bool CExcelOperObject::ExcelCLSIDFromProgID(CLSID &clsid)
{
	if (::CLSIDFromProgID(L"Excel.Application", &clsid) == S_OK ||		//Excel 2000
		::CLSIDFromProgID(L"Excel.Application.9", &clsid) == S_OK ||	//Excel 2000
		::CLSIDFromProgID(L"Excel.Application.10", &clsid) == S_OK ||	//Excel xp
		::CLSIDFromProgID(L"Excel.Application.11", &clsid) == S_OK ||	//Excel 2003
		::CLSIDFromProgID(L"Excel.Application.12", &clsid) == S_OK ||	//Excel 2007
		::CLSIDFromProgID(L"Excel.Application.13", &clsid) == S_OK ||	//Excel 2007
		::CLSIDFromProgID(L"Excel.Application.14", &clsid) == S_OK ||	//Excel 2010
		::CLSIDFromProgID(L"Excel.Application.15", &clsid) == S_OK ||	//Excel 2013
		::CLSIDFromProgID(L"Excel.Application.16", &clsid) == S_OK)	//Excel 2016
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////
//CExcelOper
CXhChar50 CExcelOper::GetCellPos(int iCol, int iRow)
{
	int nLen = iCol / 26;
	int nSubLen = iCol % 26;
	if (nLen > 26)
		return "";	//��ʱ��֧��A~AZ
	CXhChar50 cell;
	if (nLen > 0)
		cell.Printf("%c%c%d", 'A' + nLen - 1, 'A' + nSubLen, iRow);
	else
		cell.Printf("%c%d", 'A' + nSubLen, iRow);
	return cell;
}
CXhChar50 CExcelOper::GetCellName(int iCol)
{
	int nLen = iCol / 26;
	int nSubLen = iCol % 26;
	if (nLen > 26)
		return "";	//��ʱ��֧��A~AZ
	CXhChar50 cell;
	if (nLen > 0)
		cell.Printf("%c%c", 'A' + nLen - 1, 'A' + nSubLen);
	else
		cell.Printf("%c", 'A' + nSubLen);
	return cell;
}
//CreateExcelWorksheets��ͨ���ر�Excel�ֶ��ͷ���Դ wht 19-12-31
LPDISPATCH CExcelOper::CreateExcelWorksheets(int nSheet)
{	//�����EXCEL�ļ�
	LPDISPATCH pDisp;
	LPUNKNOWN pUnk;
	CLSID clsid;
	_Application excel_app;
	CExcelOperObject::ExcelCLSIDFromProgID(clsid);
	if (::GetActiveObject(clsid, NULL, &pUnk) == S_OK)
	{
		VERIFY(pUnk->QueryInterface(IID_IDispatch, (void**)&pDisp) == S_OK);
		excel_app.AttachDispatch(pDisp);
		pUnk->Release();
	}
	else if (!excel_app.CreateDispatch(clsid))
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
//���һ�����ݵ�Excel
void CExcelOper::AddRowToExcelSheet(_Worksheet &excel_sheet, int iRow, CStringArray &str_arr, 
	double *col_arr /*= NULL*/, BOOL bWrapText /*= FALSE*/)
{
	Range excel_range;
	LPDISPATCH pRange;
	for (int i = 0; i < str_arr.GetSize(); i++)
	{
		pRange = excel_sheet.GetRange(COleVariant(GetCellPos(i, iRow)));
		//AttachDispatchʱѡ���Զ��ͷţ�Attach�¶���ʱ���Զ��ͷ�ǰһ������
		//��������һ��ReleaseDispatch�Ϳ����ͷ���Դ wht 19-12-31
		excel_range.AttachDispatch(pRange);
		excel_range.SetValue(COleVariant(str_arr[i]));	//,VT_BSTR
		if (col_arr)
			excel_range.SetColumnWidth(COleVariant(col_arr[i]));
		if (bWrapText)
			excel_range.SetWrapText(COleVariant((BYTE)1));
	}
	excel_range.ReleaseDispatch();
}
//����������
void CExcelOper::SetMainTitle(_Worksheet excel_sheet, char *cell_start, char *cell_end,
	char *title, double row_height /*= 20.0*/, double font_size /*= 14.0*/,
	short horizAlignment /*= 3*/, short verticalAlignment /*= 2*/, bool bSetBorder /*= true*/)
{
	FontLegacy font;
	LPDISPATCH pRange;
	Range excel_range;
	pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.SetValue2(COleVariant(title));
	excel_range.SetHorizontalAlignment(COleVariant(horizAlignment));
	excel_range.SetVerticalAlignment(COleVariant(verticalAlignment));
	excel_range.SetRowHeight(COleVariant(row_height));
	if (bSetBorder)
	{
		Borders borders;
		Border border;
		borders.AttachDispatch(excel_range.GetBorders());
		//borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
		border.AttachDispatch(borders.GetItem(9));//xlBottom = 9
		border.SetLineStyle(COleVariant((short)-4119));	//xlDouble =-4119;
		border.SetWeight(COleVariant((short)4));		//xlThin=2;xlThick=4;
		border.ReleaseDispatch();
		borders.ReleaseDispatch();
	}
	font.AttachDispatch(excel_range.GetFont());
	COleVariant covTrue((short)TRUE);
	font.SetBold(covTrue);
	font.SetSize(COleVariant(font_size));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
}
void CExcelOper::SetBoldFont(_Worksheet excel_sheet, char *cell_start, char *cell_end)
{
	LPDISPATCH pRange;
	Range excel_range;
	FontLegacy font;
	if (cell_end != NULL)
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	else
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	font.AttachDispatch(excel_range.GetFont());
	COleVariant covTrue((short)TRUE);
	font.SetBold(covTrue);
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
}

void CExcelOper::SetFontBold(_Worksheet excel_sheet, int iStartCol, int iRow, int nCount)
{
	for (int i = 0; i < nCount; i++)
	{
		FontLegacy font;
		Range excel_range;
		LPDISPATCH pRange;
		CXhChar16 cell_start = GetCellPos(iStartCol, iRow);
		iStartCol++;
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_start));
		excel_range.AttachDispatch(pRange);
		font.AttachDispatch(excel_range.GetFont());
		COleVariant covTrue((short)TRUE);
		font.SetBold(covTrue);
		font.ReleaseDispatch();
		excel_range.ReleaseDispatch();
	}
}
void CExcelOper::SetFontSize(_Worksheet excel_sheet, const char *cell_start, const char *cell_end,
	double font_size /*= 14.0*/, bool bBold /*= FALSE*/)
{
	FontLegacy font;
	Range excel_range;
	LPDISPATCH pRange;
	if (cell_end != NULL)
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	else
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	font.AttachDispatch(excel_range.GetFont());
	font.SetBold(COleVariant((short)bBold));
	font.SetSize(COleVariant(font_size));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
}
// ����������ɫ
void CExcelOper::SetFontColor(_Worksheet excel_sheet, char *cell_start, char *cell_end, long colorId)
{
	FontLegacy font;
	Range excel_range;
	LPDISPATCH pRange;
	if (cell_end != NULL)
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	else
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	font.AttachDispatch(excel_range.GetFont());
	font.SetColorIndex(COleVariant(colorId));
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
}

//����ָ������Ԫ������
void CExcelOper::SetRangeValue(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var)
{
	LPDISPATCH pRange;
	Range excel_range;
	if (cell_end != NULL)
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	else
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue2(var);
	excel_range.ReleaseDispatch();
}
//����ָ����Ԫ������
void CExcelOper::SetRangeValue(_Worksheet &excel_sheet, int iCol, int iRow, char* sValue)
{
	LPDISPATCH pRange;
	Range excel_range;
	pRange = excel_sheet.GetRange(COleVariant(GetCellPos(iCol, iRow)));
	excel_range.AttachDispatch(pRange);
	excel_range.SetValue2(COleVariant(sValue));
	excel_range.ReleaseDispatch();
}
//����ָ������Ԫ�����ָ�ʽ
void CExcelOper::SetRangeNumberFormat(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var)
{
	LPDISPATCH pRange;
	Range excel_range;
	if (cell_end != NULL)
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	else
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetNumberFormat(var);
	excel_range.ReleaseDispatch();
}
//����ָ������Ԫ�����ָ�ʽ
void CExcelOper::SetRangeNumberFormatLocal(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var)
{
	LPDISPATCH pRange;
	Range excel_range;
	if (cell_end != NULL)
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	else
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetNumberFormatLocal(var);
	excel_range.SetHorizontalAlignment(COleVariant((long)4));
	excel_range.ReleaseDispatch();
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
void CExcelOper::SetRangeHorizontalAlignment(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var)
{
	LPDISPATCH pRange;
	Range excel_range;
	if (cell_end != NULL)
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	else
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetHorizontalAlignment(var);
	excel_range.ReleaseDispatch();
}
//�趨ָ������Ԫ��ֱ���뷽ʽ
//��  �� COleVariant((long)-4108)
// -4108:���У�-4160:���ϣ�-4107:����
void CExcelOper::SetRangeVerticalAlignment(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var)
{
	LPDISPATCH pRange;
	Range excel_range;
	if (cell_end != NULL)
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	else
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetVerticalAlignment(var);
	excel_range.ReleaseDispatch();
}
//�������ֶ��뷽ʽ(ˮƽ����ֱ)
void CExcelOper::SetRangeOrientation(_Worksheet excel_sheet, const char *cell_start, const char *cell_end, long degree /*= 0*/)
{
	LPDISPATCH pRange;
	Range excel_range;
	if (cell_end != NULL)
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	else
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetOrientation(COleVariant(degree));
	excel_range.ReleaseDispatch();
}
//�����Զ�����
void CExcelOper::SetRangeWrapText(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT var)
{
	LPDISPATCH pRange;
	Range excel_range;
	if (cell_end != NULL)
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	else
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetWrapText(var);	//�Զ�����
	excel_range.ReleaseDispatch();
}
//����ָ������ı߿�
void CExcelOper::SetRangeBorders(_Worksheet excel_sheet, char *cell_start, char *cell_end, VARIANT fontSize)
{
	Borders borders;
	FontLegacy font;
	LPDISPATCH pRange;
	Range excel_range;
	if (cell_end != NULL)
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	else
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	borders.AttachDispatch(excel_range.GetBorders());
	borders.SetWeight(COleVariant((short)2));	//xlThin=2;xlThick=4;
	borders.SetLineStyle(COleVariant((long)1));	//xlSingle
	borders.ReleaseDispatch();
	font.AttachDispatch(excel_range.GetFont());
	font.SetSize(fontSize);
	font.ReleaseDispatch();
	excel_range.ReleaseDispatch();
}
//�ϲ���
void CExcelOper::MergeRowRange(_Worksheet excel_sheet, char *cell_start, char *cell_end)
{
	LPDISPATCH pRange;
	Range excel_range;
	pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)0));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
}
//�ϲ���
void CExcelOper::MergeColRange(_Worksheet excel_sheet, char *cell_start, char *cell_end)
{
	LPDISPATCH pRange;
	Range excel_range;
	pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	excel_range.AttachDispatch(pRange);
	excel_range.Merge(COleVariant((BYTE)1));
	excel_range.SetHorizontalAlignment(COleVariant((short)3));
	excel_range.SetVerticalAlignment(COleVariant((short)2));
	excel_range.ReleaseDispatch();
}
//�����п�
void CExcelOper::SetRangeColWidth(_Worksheet excel_sheet, double width, char *cell_start,
	char *cell_end /*= NULL*/, BOOL bCenter /*= FALSE*/)
{
	LPDISPATCH pRange;
	Range excel_range;
	if (cell_end != NULL)
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	else
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetColumnWidth(COleVariant(width));
	if (bCenter)
	{
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
	}
	excel_range.ReleaseDispatch();
}
//
void CExcelOper::SetRangeRowHeight(_Worksheet excel_sheet, double height, char *cell_start,
	char *cell_end /*= NULL*/, BOOL bCenter /*= FALSE*/)
{
	LPDISPATCH pRange;
	Range excel_range;
	if (cell_end != NULL)
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	else
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetRowHeight(COleVariant(height));
	if (bCenter)
	{
		excel_range.SetHorizontalAlignment(COleVariant((short)3));
		excel_range.SetVerticalAlignment(COleVariant((short)2));
	}
	excel_range.ReleaseDispatch();
}
void CExcelOper::SetRangeBackColor(_Worksheet excel_sheet, long colorId, int iRow, int iCol, int nCount)
{
	for (int i = 0; i < nCount; i++)
	{
		LPDISPATCH pRange;
		Range excel_range;
		CXhChar16 sStart = GetCellPos(iCol, iRow);
		pRange = excel_sheet.GetRange(COleVariant((char*)sStart));
		excel_range.AttachDispatch(pRange);
		FontLegacy interior = excel_range.GetInterior();
		interior.SetColorIndex(COleVariant(colorId));
		excel_range.ReleaseDispatch();
		iCol++;
	}
}
//���õ�Ԫ��%����ʽ
void CExcelOper::SetRangeNumberFormatLocal(_Worksheet excel_sheet, const char* sLocal, char *cell_start, char *cell_end /*= NULL*/)
{
	LPDISPATCH pRange;
	Range excel_range;;
	if (cell_end != NULL)
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	else
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	excel_range.SetNumberFormatLocal(COleVariant(sLocal));
	excel_range.ReleaseDispatch();
}
//���õ�Ԫ�񱳾���ɫ
void CExcelOper::SetRangeBackColor(_Worksheet excel_sheet, long colorId, char *cell_start, char *cell_end /*= NULL*/)
{
	LPDISPATCH pRange;
	Range excel_range;;
	if (cell_end != NULL)
		pRange = excel_sheet.GetRange(COleVariant(cell_start), COleVariant(cell_end));
	else
		pRange = excel_sheet.GetRange(COleVariant(cell_start));
	excel_range.AttachDispatch(pRange);
	FontLegacy interior = excel_range.GetInterior();
	interior.SetColorIndex(COleVariant(colorId));
	excel_range.ReleaseDispatch();
}
BOOL CExcelOper::GetExcelContentOfSpecifySheet(const char* sExcelFile, CVariant2dArray &sheetContentMap,
											   int iSheetIndex, int maxColCount /*= -1*/)
{
	CExcelOperObject excelobj;
	if (excelobj.OpenExcelFile(sExcelFile))
		return GetExcelContentOfSpecifySheet(&excelobj, sheetContentMap, iSheetIndex,maxColCount);
	else
		return FALSE;
}
BOOL CExcelOper::GetExcelContentOfSpecifySheet(CExcelOperObject* pExcelOperObj, CVariant2dArray &sheetContentMap,
											   const char* sSheetName, int maxColCount /*= -1*/)
{
	int iSheet = GetExcelIndexOfSpecifySheet(pExcelOperObj, sSheetName);
	if (iSheet > 0)
		return GetExcelContentOfSpecifySheet(pExcelOperObj, sheetContentMap, iSheet,maxColCount);
	else
		return FALSE;
}
BOOL CExcelOper::GetExcelContentOfSpecifySheet(const char* sExcelFile, CVariant2dArray &sheetContentMap,
											   const char* sSheetName, int maxColCount /*= -1*/)
{
	CExcelOperObject excelobj;
	if (!excelobj.OpenExcelFile(sExcelFile))
		return FALSE;
	else
		return GetExcelContentOfSpecifySheet(&excelobj, sheetContentMap, sSheetName, maxColCount);
}
BOOL CExcelOper::GetExcelContentOfSpecifySheet(CExcelOperObject* pExcelOperObj, CVariant2dArray &sheetContentMap,
											   int iSheetIndex,int maxColCount /*= -1*/)
{	//��ָ����Excel�ļ�
	try
	{
		int nRetCode = 0;
		_Worksheet   excel_sheet;    // ������
		Sheets       excel_sheets;
		LPDISPATCH   pWorksheets = pExcelOperObj->GetWorksheets();
		ASSERT(pWorksheets != NULL);
		excel_sheets.AttachDispatch(pWorksheets);
		LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short)iSheetIndex));
		excel_sheet.AttachDispatch(pWorksheet);
		//1.����Excel������������
		Range excel_usedRange, col_range, row_range;
		excel_usedRange.AttachDispatch(excel_sheet.GetUsedRange());
		row_range.AttachDispatch(excel_usedRange.GetRows());
		long nRowNum = row_range.GetCount();
		row_range.ReleaseDispatch();
		col_range.AttachDispatch(excel_usedRange.GetColumns());
		long nColNum = col_range.GetCount();
		col_range.ReleaseDispatch();
		excel_usedRange.ReleaseDispatch();

		//AfxMessageBox(CXhChar50("Excel program Row Num=%d, Col Num = %d", nRowNum, nColNum));
		//�ⲿ֧�������������ֹ����˨���ർ�µĶ�ȡʧ�� wht 19-12-04
		if (maxColCount > 0 && nColNum > maxColCount)
			nColNum = maxColCount;
		//2.��ȡExcelָ��Sheet���ݴ洢��sheetContentMap��
		CXhChar50 cell = GetCellPos(nColNum, nRowNum);
		Range excel_content;
		LPDISPATCH pRange = excel_sheet.GetRange(COleVariant("A1"), COleVariant(cell));
		excel_content.AttachDispatch(pRange);
		sheetContentMap.var = excel_content.GetValue();
		//AfxMessageBox("Excel program 3");
		excel_content.ReleaseDispatch();
		excel_sheet.ReleaseDispatch();
		excel_sheets.ReleaseDispatch();
		return true;
	}
	catch (CString sError)
	{
		logerr.Log(sError);
		return false;
	}
}
BOOL CExcelOper::GetExcelContentOfSpecifySheet(const char* sExcelFile, CStringArray &sheetNameArr)
{
	CExcelOperObject excelobj;
	if (excelobj.OpenExcelFile(sExcelFile))
		return GetExcelContentOfSpecifySheet(&excelobj, sheetNameArr);
	else
		return FALSE;
}
BOOL CExcelOper::GetExcelContentOfSpecifySheet(CExcelOperObject* pExcelOperObj, CStringArray &sheetNameArr)
{	//��ָ����Excel�ļ�
	int nRetCode = 0;
	_Worksheet   excel_sheet;    // ������
	Sheets       excel_sheets;

	LPDISPATCH pWorksheets = pExcelOperObj->GetWorksheets();
	ASSERT(pWorksheets != NULL);
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = NULL;
	int nSheetCount = excel_sheets.GetCount();
	sheetNameArr.RemoveAll();
	if (nSheetCount > 0)
	{
		sheetNameArr.SetSize(nSheetCount);
		for (int i = 1; i < excel_sheets.GetCount() + 1; i++)
		{
			pWorksheet = excel_sheets.GetItem(COleVariant((short)i));
			excel_sheet.AttachDispatch(pWorksheet);
			sheetNameArr[i - 1] = excel_sheet.GetName();
			excel_sheet.ReleaseDispatch();
		}
	}
	excel_sheets.ReleaseDispatch();
	return true;
}
int CExcelOper::GetExcelIndexOfSpecifySheet(CExcelOperObject* pExcelOperObj, const char* sSheetName)
{	//��ָ����Excel�ļ�
	int nRetCode = 0;
	_Worksheet   excel_sheet;    // ������
	Sheets       excel_sheets;
	CStringArray sheetNameArr;
	LPDISPATCH pWorksheets = pExcelOperObj->GetWorksheets();
	ASSERT(pWorksheets != NULL);
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = NULL;
	int nSheetCount = excel_sheets.GetCount();
	if (nSheetCount > 0)
	{
		sheetNameArr.SetSize(nSheetCount);
		for (int i = 1; i < excel_sheets.GetCount() + 1; i++)
		{
			pWorksheet = excel_sheets.GetItem(COleVariant((short)i));
			excel_sheet.AttachDispatch(pWorksheet);
			sheetNameArr[i - 1] = excel_sheet.GetName();
			excel_sheet.ReleaseDispatch();
		}
	}
	excel_sheets.ReleaseDispatch();
	int iSheet = -1;
	for (int i = 0; i < sheetNameArr.GetSize(); i++)
	{
		if (sheetNameArr[i].CompareNoCase(sSheetName) == 0)
		{
			iSheet = i + 1;
			break;
		}
	}
	return iSheet;
}

#ifdef __SUPPORT_XH_REPORT_TBL_
BOOL CExcelOper::ExportExcle(CXhTable* pTbl)
{
	if (pTbl == NULL)
		return FALSE;
	LPDISPATCH pWorksheets = CExcelOper::CreateExcelWorksheets(1);
	ASSERT(pWorksheets);
	Sheets excel_sheets;
	excel_sheets.AttachDispatch(pWorksheets);
	LPDISPATCH pWorksheet = excel_sheets.GetItem(COleVariant((short)1));
	_Worksheet excel_sheet;
	excel_sheet.AttachDispatch(pWorksheet);
	excel_sheet.Select();
	//����sheet��
	if (strlen(pTbl->m_sTblName) > 0)
		excel_sheet.SetName(pTbl->m_sTblName);
	//�����и��п�
	CXhChar50 cellS, cellE;
	for (int iRow = 0; iRow < pTbl->nRowsCount; iRow++)
	{
		double height = 0;
		pTbl->GetRowHigh(iRow, &height);
		height = CXhTable::TransRowHightToXls(height);
		cellS = CExcelOper::GetCellPos(0, iRow + 1);
		cellE = CExcelOper::GetCellPos(pTbl->nColsCount - 1, iRow + 1);
		CExcelOper::SetRangeRowHeight(excel_sheet, height, cellS, cellS);
	}
	for (int iCol = 0; iCol < pTbl->nColsCount; iCol++)
	{
		double width = 0;
		pTbl->GetColWide(iCol, &width);
		width = CXhTable::TransColWidthToXls(width);
		cellS = CExcelOper::GetCellPos(iCol, 1);
		cellE = CExcelOper::GetCellPos(iCol, pTbl->nRowsCount);
		CExcelOper::SetRangeColWidth(excel_sheet, width, cellS, cellE);
	}
	//��д�����⼰����
	for (int iRow = 0; iRow < pTbl->nRowsCount; iRow++)
	{
		for (int iCol = 0; iCol < pTbl->nColsCount; iCol++)
		{
			XHGRID *pGrid = pTbl->GetGridAt(iRow, iCol);
			//if (pGrid->m_bHGridVirtual || pGrid->m_bVGridVirtual)
			//	continue;	//����Ԫ
			VARIANT varVA, varHA;
			if (pGrid->xFontStyle.m_nTextAlignFlags == XHTBLDEF::TEXTALIGN_RIGHT_CENTER)
			{	//���������
				varVA = COleVariant((long)-4108);
				varHA = COleVariant((long)4);
			}
			else if (pGrid->xFontStyle.m_nTextAlignFlags == XHTBLDEF::TEXTALIGN_MIDDLE_CENTER)
			{	//���������
				varVA = COleVariant((long)-4108);
				varHA = COleVariant((long)3);
			}
			else if (pGrid->xFontStyle.m_nTextAlignFlags == XHTBLDEF::TEXTALIGN_LEFT_CENTER)
			{	//���������
				varVA = COleVariant((long)-4108);
				varHA = COleVariant((long)2);
			}
			else if (pGrid->xFontStyle.m_nTextAlignFlags == XHTBLDEF::TEXTALIGN_BOTTOM_RIGHT)
			{	//���½Ƕ���
				varVA = COleVariant((long)-4107);
				varHA = COleVariant((long)4);
			}
			else if (pGrid->xFontStyle.m_nTextAlignFlags == XHTBLDEF::TEXTALIGN_TOP_LEFT)
			{	//���½Ƕ���
				varVA = COleVariant((long)-4107);
				varHA = COleVariant((long)2);
			}
			else
				continue;
			//
			CXhChar50 cell = CExcelOper::GetCellPos(iCol, iRow + 1);
			LPDISPATCH pRange = excel_sheet.GetRange(COleVariant(cell));
			Range excel_range;
			excel_range.AttachDispatch(pRange);
			excel_range.SetValue2(COleVariant(pGrid->data.sVal));
			excel_range.SetVerticalAlignment(varVA);
			excel_range.SetHorizontalAlignment(varHA);
			//��������
			double fSize = CXhTable::TransRowHightToXls(pGrid->simfont.sfTextSize);
			FontLegacy font;
			font.AttachDispatch(excel_range.GetFont());
			font.SetBold(COleVariant((short)pGrid->simfont.bBold));
			font.SetSize(COleVariant((float)fSize));
			font.SetColor(COleVariant((long)pGrid->xFontStyle.crTextColor));
			font.ReleaseDispatch();
			//��������
			FontLegacy interior;
			interior.AttachDispatch(excel_range.GetInterior());
			interior.SetColor(COleVariant((long)pGrid->xFontStyle.crBackground));
			interior.ReleaseDispatch();
			excel_range.ReleaseDispatch();
		}
	}
	//����ˮƽ�ϲ������кϲ���
	for (int iRow = 0; iRow < pTbl->nRowsCount; iRow++)
	{
		int iColS = 0, iColE = 0;
		for (int iCol = 0; iCol < pTbl->nColsCount; iCol++)
		{
			if (pTbl->GetGridAt(iRow, iCol)->m_bHGridVirtual)
			{	//��ǰ��Ϊ�鵥Ԫ
				iColE = iCol;
				if (iCol == pTbl->nColsCount - 1)
				{
					cellS = CExcelOper::GetCellPos(iColS, iRow + 1);
					cellE = CExcelOper::GetCellPos(iColE, iRow + 1);
					CExcelOper::MergeColRange(excel_sheet, cellS, cellE);
				}
			}
			else
			{
				if (iCol > 0 && pTbl->GetGridAt(iRow, iCol - 1)->m_bHGridVirtual)
				{
					cellS = CExcelOper::GetCellPos(iColS, iRow + 1);
					cellE = CExcelOper::GetCellPos(iColE, iRow + 1);
					CExcelOper::MergeColRange(excel_sheet, cellS, cellE);
				}
				iColS = iCol+1;
			}
		}
	}
	//������ֱ�ϲ������кϲ���
	for (int iCol = 0; iCol < pTbl->nColsCount; iCol++)
	{
		int iRowS = 0, iRowE = 0;
		for (int iRow = 0; iRow < pTbl->nRowsCount; iRow++)
		{
			if (pTbl->GetGridAt(iRow, iCol)->m_bVGridVirtual == TRUE)
			{	//��ǰ��Ϊ�鵥Ԫ
				iRowE = iRow;
				if (iRow == pTbl->nRowsCount - 1)
				{
					cellS = CExcelOper::GetCellPos(iCol, iRowS + 1);
					cellE = CExcelOper::GetCellPos(iCol, iRowE + 1);
					CExcelOper::MergeRowRange(excel_sheet, cellS, cellE);
				}
			}
			else
			{
				if (iRow > 0 && pTbl->GetGridAt(iRow - 1, iCol)->m_bVGridVirtual)
				{
					cellS = CExcelOper::GetCellPos(iCol, iRowS + 1);
					cellE = CExcelOper::GetCellPos(iCol, iRowE + 1);
					CExcelOper::MergeRowRange(excel_sheet, cellS, cellE);
				}
				iRowS = iRow;
			}
		}
	}
	//���ñ߿�
	cellS = CExcelOper::GetCellPos(0, 1);
	cellE = CExcelOper::GetCellPos(pTbl->nColsCount - 1, pTbl->nRowsCount);
	LPDISPATCH pRange = excel_sheet.GetRange(COleVariant(cellS), COleVariant(cellE));
	Range excel_range;
	excel_range.AttachDispatch(pRange);
	excel_range.SetWrapText(COleVariant((short)TRUE));	//�Զ�����
	Borders borders;
	borders.AttachDispatch(excel_range.GetBorders());
	borders.SetWeight(COleVariant((short)2));
	borders.SetLineStyle(COleVariant((long)1));
	borders.ReleaseDispatch();
	excel_range.ReleaseDispatch();
	
	//
	excel_sheet.ReleaseDispatch();
	excel_sheets.ReleaseDispatch();

	return TRUE;
}
BOOL CExcelOper::GetExcelContentOfSpecifySheet(const char* sExcelFile, const char* sSheetName,
										CXhTable* pTempTable, int maxColCount /*= -1*/)
{
	if (pTempTable == NULL)
		return FALSE;
	CExcelOperObject excelobj;
	if (excelobj.OpenExcelFile(sExcelFile))
		return GetExcelContentOfSpecifySheet(&excelobj, sSheetName, pTempTable, maxColCount);
	else
		return FALSE;
}
BOOL CExcelOper::GetExcelContentOfSpecifySheet(CExcelOperObject* pExcelOperObj, const char* sSheetName,
										CXhTable* pTempTable, int maxColCount /*= -1*/)
{
	if (pTempTable == NULL)
		return FALSE;
	int iSheet = GetExcelIndexOfSpecifySheet(pExcelOperObj, sSheetName);
	if (iSheet > 0)
		return GetExcelContentOfSpecifySheet(pExcelOperObj, iSheet, pTempTable, maxColCount);
	else
		return FALSE;
}
BOOL CExcelOper::GetExcelContentOfSpecifySheet(const char* sExcelFile, int iSheetIndex,
										CXhTable* pTempTable, int maxColCount /*= -1*/)
{
	if (pTempTable == NULL)
		return FALSE;
	CExcelOperObject excelobj;
	if (excelobj.OpenExcelFile(sExcelFile))
		return GetExcelContentOfSpecifySheet(&excelobj, iSheetIndex, pTempTable,maxColCount);
	else
		return FALSE;
}
BOOL CExcelOper::GetExcelContentOfSpecifySheet(CExcelOperObject* pExcelOperObj, int iSheetIndex,
										CXhTable* pTempTable, int maxColCount /*= -1*/)
{
	if (pTempTable == NULL)
		return FALSE;
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
		//1.����Excel������������
		Range excel_usedRange, excel_range;
		excel_usedRange.AttachDispatch(excel_sheet.GetUsedRange());
		excel_range.AttachDispatch(excel_usedRange.GetRows());
		long nRowNum = excel_range.GetCount();
		excel_range.AttachDispatch(excel_usedRange.GetColumns());
		long nColNum = excel_range.GetCount();
		//�ⲿ֧�������������ֹ����˨���ർ�µĶ�ȡʧ�� wht 19-12-04
		if (maxColCount > 0 && nColNum > maxColCount)
			nColNum = maxColCount;
		//2.��ȡExcelָ��Sheet���ݴ洢��pTempTable��
		CXhChar50 cell = GetCellPos(nColNum, nRowNum);
		pRange = excel_sheet.GetRange(COleVariant("A1"), COleVariant(cell));
		excel_range.AttachDispatch(pRange);
		Range  oCurCell;
		for (int i = 0; i < nRowNum; )
		{
			for (int j = 0; j <= nColNum - 1; )
			{
				oCurCell.AttachDispatch(excel_range.GetItem(COleVariant((long)(i + 1)), COleVariant((long)(j + 1))).pdispVal, TRUE);
				VARIANT text = oCurCell.GetText();
				VARIANT varMerge = oCurCell.GetMergeCells();
				LPDISPATCH lPDISPATCH = oCurCell.GetMergeArea();
				VARIANT height = oCurCell.GetHeight();
				VARIANT width = oCurCell.GetColumnWidth();
				VARIANT varVA = oCurCell.GetVerticalAlignment();
				VARIANT varHA = oCurCell.GetHorizontalAlignment();
				FontLegacy font;
				font.AttachDispatch(oCurCell.GetFont());
				VARIANT size = font.GetSize();
				font.ReleaseDispatch();
				if (pTempTable != NULL)
				{
					if (varVA.iVal == (long)-4108 && varHA.iVal == (long)-4152)
					{	//���������
						pTempTable->GetGridAt(i, j)->xFontStyle.m_nTextAlignFlags = XHTBLDEF::TEXTALIGN_RIGHT_CENTER;
					}
					else if (varVA.iVal == (long)-4108 && varHA.iVal == (long)-4108)
					{	//���������
						pTempTable->GetGridAt(i, j)->xFontStyle.m_nTextAlignFlags = XHTBLDEF::TEXTALIGN_MIDDLE_CENTER;
					}
					else if (varVA.iVal == (long)-4108 && varHA.iVal == (long)-4131)
					{	//���������
						pTempTable->GetGridAt(i, j)->xFontStyle.m_nTextAlignFlags = XHTBLDEF::TEXTALIGN_LEFT_CENTER;
					}
					else if (varVA.iVal == (long)-4107 && varHA.iVal == (long)-4152)
					{	//���½Ƕ���
						pTempTable->GetGridAt(i, j)->xFontStyle.m_nTextAlignFlags = XHTBLDEF::TEXTALIGN_BOTTOM_RIGHT;
					}
					else if (varVA.iVal == (long)-4107 && varHA.iVal == (long)-4131)
					{	//���½Ƕ���
						pTempTable->GetGridAt(i, j)->xFontStyle.m_nTextAlignFlags = XHTBLDEF::TEXTALIGN_TOP_LEFT;
					}
					pTempTable->GetGridAt(i, j)->m_bHGridVirtual = (varMerge.boolVal) ? true : false;
					if (j == 0)
						pTempTable->SetRowHigh(i, height.dblVal);
					if (i == 0)
						pTempTable->SetColWide(j, width.dblVal);
					pTempTable->GetGridAt(i, j)->simfont.sfTextSize = (float)size.dblVal;
				}
				j++;
			}
			i++;
		}
		excel_range.ReleaseDispatch();
		excel_usedRange.ReleaseDispatch();
		excel_sheet.ReleaseDispatch();
		excel_sheets.ReleaseDispatch();
		return true;
	}
	catch (CString sError)
	{
		logerr.Log(sError);
		return false;
	}
}
#endif