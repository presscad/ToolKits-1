// SuperGridCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SuperGridCtrl.h"
#include "SortFunc.h"
#include "imm.h"

#pragma comment(lib, "imm32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//SuperGrid, YALC (yet another listview control) 
static DWORD BINARYWORD[32]={ 0X00000001,0X00000002,0X00000004,0X00000008,0X00000010,0X00000020,0X00000040,0X00000080,
0X00000100,0X00000200,0X00000400,0X00000800,0X00001000,0X00002000,0X00004000,0X00008000,
0X00010000,0X00020000,0X00040000,0X00080000,0X00100000,0X00200000,0X00400000,0X00800000,
0X01000000,0X02000000,0X04000000,0X08000000,0X10000000,0X20000000,0X40000000,0X80000000};
static DWORD GetBinaryWord(int index)
{
	if(index<0||index>=32)
		return 0;
	else
		return BINARYWORD[index];
}
// CListCtrlItemInfo
int CListCtrlItemInfo::AddSubItemText(const char* strSubItem, BOOL bReadOnly/*=FALSE*/)
{ 
	int index=m_subItems.Add(strSubItem); 
	m_arrItemReadOnly.SetAtGrow(index,bReadOnly);
	SetControlType(index,GCT_EDIT);
	return index;
}
int  CListCtrlItemInfo::InsertSubItemText(int nCol, LPCTSTR strSubItem, BOOL bReadOnly/*=FALSE*/)
{
	int index=nCol;
	if(nCol<m_subItems.GetCount())
	{
		m_subItems.InsertAt(nCol,strSubItem); 
		if(nCol<m_arrItemReadOnly.GetCount())
			m_arrItemReadOnly.InsertAt(index,bReadOnly);
		if(nCol<m_subHelpItems.GetCount())
			m_subHelpItems.InsertAt(nCol,"");
		//更新ControlType
		int itemcount=m_subItems.GetCount();
		for(int itemid=itemcount;itemid>nCol;itemid--)
		{
			int* piOldType=m_controlType.GetValue(itemid-1);
			if(piOldType)
				m_controlType.SetValue(itemid,*piOldType);
			else
				m_controlType.DeleteCursor();
			CString* pszOldItems=m_listdata.GetValue(itemid-1);
			if(pszOldItems)
				m_listdata.SetValue(itemid,*pszOldItems);
			else
				m_listdata.DeleteCursor();
			COLORREF* pClrOld=m_mapClf.GetValue(itemid-1);
			if(pClrOld)
				m_mapClf.SetValue(itemid,*pClrOld);
			else
				m_mapClf.DeleteCursor();
			pClrOld=m_mapTextClf.GetValue(itemid-1);
			if(pClrOld)
				m_mapTextClf.SetValue(itemid,*pClrOld);
			else
				m_mapTextClf.DeleteCursor();
		}
		m_controlType.SetValue(nCol,GCT_EDIT);
	}
	else
		return -1;
	return index;
}
BOOL CListCtrlItemInfo::DeleteSubItemText(int nCol)
{
	if(nCol>=m_subItems.GetCount())
		return FALSE;
	if(m_subItems.GetSize()>nCol)
		m_subItems.RemoveAt(nCol);
	if(m_arrItemReadOnly.GetSize()>nCol)
		m_arrItemReadOnly.RemoveAt(nCol);
	if(m_subHelpItems.GetSize()>nCol)
		m_subHelpItems.RemoveAt(nCol);
	//TODO:更新ControlType
	int itemcount=m_subItems.GetCount();
	for(int itemid=nCol;itemid<itemcount;itemid++)
	{
		int* piOldType=m_controlType.GetValue(itemid+1);
		if(piOldType)
			m_controlType.SetValue(itemid,*piOldType);
		else
			m_controlType.DeleteCursor();
		CString* pszOldItems=m_listdata.GetValue(itemid+1);
		if(pszOldItems)
			m_listdata.SetValue(itemid,*pszOldItems);
		else
			m_listdata.DeleteCursor();
		COLORREF* pClrOld=m_mapClf.GetValue(itemid+1);
		if(pClrOld)
			m_mapClf.SetValue(itemid,*pClrOld);
		else
			m_mapClf.DeleteCursor();
		pClrOld=m_mapTextClf.GetValue(itemid+1);
		if(pClrOld)
			m_mapTextClf.SetValue(itemid,*pClrOld);
		else
			m_mapTextClf.DeleteCursor();
	}
	if(m_controlType.GetValue(nCol)==NULL)
		m_controlType.SetValue(nCol,GCT_EDIT);
	return TRUE;
}
void CListCtrlItemInfo::SetSubItemColor(int iSubItem, COLORREF clf)
{ 
	m_mapClf.SetValue(iSubItem+1, clf);	//索引加1是为了使哈希表键值不出现0
}
void CListCtrlItemInfo::SetSubItemTextColor(int iSubItem, COLORREF clf)
{
	m_mapTextClf.SetValue(iSubItem+1, clf);	//索引加1是为了使哈希表键值不出现0
}
void CListCtrlItemInfo::SetSubItemText(int iSubItem, const char* strSubItem, const char* strHelp/*=_T("")*/)
{
	//如果系统先SetControlType后调用SetSubItemText，可能会因为是新增项目，而冲掉之前的下拉框类型 wjh-2016.10.9
	//if(m_subItems.GetSize()<=iSubItem)
	if(m_controlType.GetValue(iSubItem+1)==NULL)
		SetControlType(iSubItem,GCT_EDIT);
	m_subItems.SetAtGrow(iSubItem, strSubItem);
	if(m_arrItemReadOnly.GetSize()<m_subItems.GetSize())
	{
		int iInitSize=m_arrItemReadOnly.GetSize();
		m_arrItemReadOnly.SetSize(m_subItems.GetSize());
		for(int i=iInitSize;i<m_subItems.GetSize()-1;i++)
			m_arrItemReadOnly[i]=TRUE;
		m_arrItemReadOnly[m_subItems.GetSize()-1]=FALSE;
	}
	m_subHelpItems.SetAtGrow(iSubItem,strHelp); //设置帮助字符串内容 wht 10-09-13
}

void CListCtrlItemInfo::SetSubItemText(int iSubItem, const char* strSubItem, BOOL bReadOnly, const char* strHelp/*=_T("")*/)
{ 
	if(m_subItems.GetSize()<=iSubItem)
		SetControlType(iSubItem,GCT_EDIT);
	m_subItems.SetAtGrow(iSubItem, strSubItem);
	if(m_arrItemReadOnly.GetSize()<m_subItems.GetSize())
	{
		int iInitSize=m_arrItemReadOnly.GetSize();
		m_arrItemReadOnly.SetSize(m_subItems.GetSize());
		for(int i=iInitSize;i<m_subItems.GetSize();i++)
			m_arrItemReadOnly[i]=TRUE;
	}
	m_arrItemReadOnly[iSubItem]=bReadOnly;
	m_subHelpItems.SetAtGrow(iSubItem,strHelp); //设置帮助字符串内容  wht 10-09-13
}

void CListCtrlItemInfo::SetSubItemHelpText(int iSubItem, const char* strHelp)
{	//设置帮助字符串内容  wht 10-09-13
	m_subHelpItems.SetAtGrow(iSubItem, strHelp);
}
// item read only state
// void CListCtrlItemInfo::InitItemReadOnly(int nSize,BOOL *read_only_arr/*=NULL*/)
// {	
// 	m_arrItemReadOnly.SetSize(nSize);
// 	for(int i=0;i<nSize;i++)
// 	{
// 		if(read_only_arr)
// 			m_arrItemReadOnly[i]=read_only_arr[i];
// 		else
// 			m_arrItemReadOnly[i]=TRUE;
// 	}
// }
CString CListCtrlItemInfo::GetSubItemText(int iSubItem)
{
	if(m_subItems.GetSize()>iSubItem)
		return m_subItems.GetAt(iSubItem); 
	else
		return "";
}
CString CListCtrlItemInfo::GetSubItemHelpText(int iSubItem)
{
	if(m_subHelpItems.GetSize()>iSubItem&&iSubItem>=0)
		return m_subHelpItems.GetAt(iSubItem); 
	else
		return "";
}
void CListCtrlItemInfo::SetSubItemReadOnly(int iSubItem,BOOL bReadOnly)
{	//
	if(iSubItem<m_arrItemReadOnly.GetSize())
		m_arrItemReadOnly[iSubItem]=bReadOnly;
	else
	{
		int iInitSize=m_arrItemReadOnly.GetSize();
		m_arrItemReadOnly.SetAtGrow(iSubItem,bReadOnly);
		for(int i=iInitSize;i<m_arrItemReadOnly.GetSize();i++)
		{
			if(i!=iSubItem)
				m_arrItemReadOnly[i]=TRUE;
		}
	}
}

BOOL CListCtrlItemInfo::GetSubItemReadOnly(int iSubItem) const 
{
	if(iSubItem<m_arrItemReadOnly.GetSize()&&iSubItem>=0)
		return m_arrItemReadOnly[iSubItem];
	else //not find this subitem return read only;
		return TRUE;
}

//all cols in this row is default edit
void CListCtrlItemInfo::SetControlType(int iSubItem,int nCtrlType)
{ 
	m_controlType.SetValue(iSubItem+1, nCtrlType);
}

BOOL CListCtrlItemInfo::GetControlType(int iSubItem, int& nCtrlType)
{
	if(m_controlType.GetValue(iSubItem+1)==NULL)
		return FALSE;
	else
	{
		nCtrlType=m_controlType[iSubItem+1];
		return TRUE;
	}
}

void CListCtrlItemInfo::SetListItemsStr(int iSubItem, CString itemsStr)
{
	m_listdata.SetValue(iSubItem+1, itemsStr);	//索引加1是为了使哈希表键值不出现0
}	

BOOL CListCtrlItemInfo::GetListItemsStr(int iSubItem, CString& itemsStr)
{	
	CString *pStr = m_listdata.GetValue(iSubItem+1);
	if(pStr)
	{
		itemsStr=*pStr;
		return TRUE;
	}
	else
		return FALSE;
}

//YOU SHOULD MODIFY THIS WHEN EVER YOU ADD NEW DATA TO THIS CLASS
void CListCtrlItemInfo::CopyObjects(CListCtrlItemInfo* pItemInfo)
{
	m_subItems.Copy(pItemInfo->m_subItems);
	m_arrItemReadOnly.Copy(pItemInfo->m_arrItemReadOnly);
	CopyControls(pItemInfo);
	CopyColors(pItemInfo);
	SetCheck(pItemInfo->GetCheck());
	SetImage(pItemInfo->GetImage());
	m_lParam = pItemInfo->m_lParam;
}

void CListCtrlItemInfo::CopyControls(CListCtrlItemInfo* pItemInfo)
{
	for(int nCol=0; nCol < pItemInfo->GetItemCount(); nCol++)
	{
		int nCtrlType;
		CString itemsStr;
		if(pItemInfo->GetControlType(nCol, nCtrlType))//true if other than edit-control
		{
			SetControlType(nCtrlType, nCol);
			//should test if this is listdata material
			if(pItemInfo->GetListItemsStr(nCol, itemsStr))
				SetListItemsStr(nCol, itemsStr);	
		}
	}
}

void CListCtrlItemInfo::CopyColors(CListCtrlItemInfo* pItemInfo)
{
	for(int iCol=0; iCol < pItemInfo->GetItemCount(); iCol++)
	{
		if(pItemInfo->m_mapClf.GetValue(iCol+1))
			m_mapClf.SetValue(iCol+1, pItemInfo->m_mapClf[iCol+1]);
		if(pItemInfo->m_mapTextClf.GetValue(iCol+1))
			m_mapTextClf.SetValue(iCol+1, pItemInfo->m_mapTextClf[iCol+1]);
	}
}

CListCtrlItemInfo::~CListCtrlItemInfo()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSuperGridCtrl
CSuperGridCtrl::CSuperGridCtrl()
{
	m_cxImage = m_cyImage = 12;
	m_bIsDragging = m_CurSubItem = 0;
    m_nDragTarget=m_nDragItem = -1;
	m_psTreeLine.CreatePen(PS_SOLID, 1, RGB(192,192,192));
	m_psRectangle.CreatePen(PS_SOLID, 1, RGB(198,198,198));
	m_psPlusMinus.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	m_brushErase.CreateSolidBrush(RGB(255,255,255));
	m_himl = NULL;
	m_bDrag=FALSE;
	FireContextMenu=NULL;
	FireKeyUpItem=NULL;
	FireEditBoxKeyUpItem=NULL;
	FireKeyDownItem=NULL;
	FireDeleteItem=NULL;
	FireButtonClick=NULL;
	FireValueModify=NULL;
	FireStatusModify=NULL;
	FireItemChanged=NULL;
	FireItemUnselect=NULL;
	FireLButtonDblclk=NULL;
	FireLButtonDown=NULL;
	FireCompareItem=NULL;
	FireColumnClick=NULL;
	FireMouseWheel=NULL;
	FireScroll=NULL;
	FireEndtrack=NULL;
	m_bEscKeyDown=FALSE;
	m_bPropValueModified=FALSE;
	m_curEditItem.iItem=-1;
	m_iPropGroup=0;	//默认显示第一个属性组
	m_bDblclkDisplayCellCtrl=FALSE;	//设置为双击显示编辑控件,默认为单击显示控件 wht 11-04-14
	m_bHasInputCtrlFocus=FALSE;
	m_bEnableSort=false;
	m_cSplit='|';
	m_bImmerseMode=FALSE;
	m_clrImmerse=RGB(128,128,128);
	m_verticalAligment=DT_VCENTER;
	m_clrImmerse=0;
	m_clrEvenRow=0;
	m_clrGridLine=0;
	m_bCustomDrawTitle=FALSE;
	m_bCloseEditBoxImm=FALSE;
	m_bFireOnNoneItemSelect = false;
}


CSuperGridCtrl::~CSuperGridCtrl()
{
	m_psPlusMinus.DeleteObject();
	m_psRectangle.DeleteObject();
	m_psTreeLine.DeleteObject();
	m_brushErase.DeleteObject();

	while(m_RootItems.GetCount())
	{
		CTreeItem * root = (CTreeItem*)m_RootItems.RemoveHead();
		if(root!=NULL && root->m_lpNodeInfo != NULL)
			delete root->m_lpNodeInfo;
		delete root;
	}
	m_RootItems.RemoveAll();
}

BEGIN_MESSAGE_MAP(CSuperGridCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSuperGridCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	//}}AFX_MSG_MAP
	ON_CBN_KILLFOCUS(IDC_CELLCMBBOX, OnKillfocusCmbBox)
	ON_EN_KILLFOCUS(IDC_CELLEDITBOX, OnKillfocusEditBox)
	ON_BN_KILLFOCUS(IDC_CELLBTNCTRL, OnKillfocusBtn)
	ON_BN_CLICKED(IDC_CELLBTNCTRL, OnButton)
	ON_CBN_SELCHANGE(IDC_CELLCMBBOX, OnSelchangeCmbBox)
	ON_EN_CHANGE(IDC_CELLEDITBOX, OnChangeEditBox)
	ON_NOTIFY(HDN_ENDTRACKA, 0, &CSuperGridCtrl::OnHdnEndtrack)
	ON_NOTIFY(HDN_ENDTRACKW, 0, &CSuperGridCtrl::OnHdnEndtrack)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSuperGridCtrl message handlers

void CSuperGridCtrl::InitListCtrl(long* col_wide_arr,BOOL bSingleSel/*=TRUE*/,long* col_fmt_arr/*=NULL*/)
{
	DWORD dwFlag=LVS_REPORT | LVS_SHAREIMAGELISTS | LVS_OWNERDRAWFIXED | LVS_SHOWSELALWAYS;
	if(bSingleSel)
		dwFlag|=LVS_SINGLESEL;
	ModifyStyle(0,dwFlag);
	SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	RefreshListViewHeader(col_wide_arr,col_fmt_arr);
	if(m_SSerif8Font.GetSafeHandle()==NULL)
	{
		CFont *pFont=GetFont();
		LOGFONT logFont;
		pFont->GetLogFont(&logFont);
		m_SSerif8Font.CreateFontIndirect(&logFont);
	}
}

void CSuperGridCtrl::RefreshListViewHeader(long* col_wide_arr,long* col_fmt_arr/*=NULL*/)
{
	LV_COLUMN lvc;
	int i, NUM_COLUMNS, nColumnCount;
	CHeaderCtrl *pHeader = GetHeaderCtrl();
	nColumnCount = pHeader->GetItemCount();
	
	// Delete all of the columns.
	for (i=0;i < nColumnCount;i++)
	{
		DeleteColumn(0);
	}
	if(m_arrHeader.GetSize()>0)
		NUM_COLUMNS = m_arrHeader.GetSize();
	else
		NUM_COLUMNS = 0;
	
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(i = 0; i<NUM_COLUMNS; i++)
	{
		lvc.iSubItem = i;
		lvc.pszText = m_arrHeader[i].headerName;//_gszColumnLabel[i];
		if(col_wide_arr!=NULL)
			lvc.cx = m_arrHeader[i].width = col_wide_arr[i];
		else
			lvc.cx = m_arrHeader[i].width;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		if(col_fmt_arr!=NULL)
			lvc.fmt = col_fmt_arr[i];
		InsertColumn(i,&lvc);
	}
}

void CSuperGridCtrl::SelPage(int iPage)
{
	DWORD flag=GetBinaryWord(iPage);
	for(int i=0;i<m_arrHeader.GetSize();i++)
	{
		int colWidth=GetColumnWidth(i);
		if(m_arrHeader[i].dwFlag&flag)
		{
			if(colWidth==0)	//恢复
				SetColumnWidth(i,m_arrHeader[i].width);
			else	//保留不变
				continue;
		}
		else
		{
			SetColumnWidth(i,0);
			if(colWidth>0)
				m_arrHeader[i].width=colWidth;	//保留之前状态
		}
	}
}

int CSuperGridCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

// initialize image list, -1---not image list
BOOL CSuperGridCtrl::InitImageList(long nImageListID)
{
	if(nImageListID==-1)
	{
		m_cxImage=m_cyImage=12;
		return -1;
	}	
	if(!m_imageList.Create(nImageListID,12,1,RGB(0,255,0)))
		return -1;
	
	SetImageList(&m_imageList,  LVSIL_SMALL);
	CImageList *pImageList = GetImageList( LVSIL_SMALL);
	if(pImageList)
		ImageList_GetIconSize(pImageList->m_hImageList, &m_cxImage, &m_cyImage);
	else
	{
		m_cxImage=m_cyImage=12;
		return -1;
	}
	return TRUE;
}

#define OFFSET_FIRST	2 
#define OFFSET_OTHER	6
void CSuperGridCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (lpDrawItemStruct->CtlType != ODT_LISTVIEW)
        return;
	if(lpDrawItemStruct->itemAction == ODA_DRAWENTIRE)
	{
		if(m_himl==NULL)
		{	
			m_himl = (HIMAGELIST)::SendMessage(m_hWnd, LVM_GETIMAGELIST, (WPARAM)(int)( LVSIL_SMALL), 0L);
			//if(m_himl==NULL) //may not image list //wht 10-05-21
			//	return;
		}

		LV_ITEM lvi;
		static _TCHAR szBuff[MAX_PATH];
		LPCTSTR pszText;
		
		CRect orgRectItem(lpDrawItemStruct->rcItem);
		int nItem = lpDrawItemStruct->itemID;
		CRect rcItem(orgRectItem);
		lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_INDENT | LVIF_PARAM; 
		lvi.iItem = nItem;
		lvi.iSubItem =0;
		lvi.pszText = szBuff;
		lvi.cchTextMax = sizeof(szBuff);
		lvi.stateMask = 0xFFFF;		
		GetItem(&lvi);

		CTreeItem *pSelItem = (CTreeItem*)lpDrawItemStruct->itemData;

		CRect rcLabel;
		GetItemRect(nItem, rcLabel, LVIR_LABEL);
		
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		ASSERT(pDC);

		CRect rcClipBox;
		pDC->GetClipBox(rcClipBox);

		COLORREF crBackground, crText;
		BOOL bCanUseTextColor=FALSE;
		if (lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			// Set the text background and foreground colors
			if(m_bImmerseMode)
			{
				crBackground = GetSysColor (COLOR_WINDOW);
				if(m_clrEvenRow>0&&nItem%2==0)
					crBackground = m_clrEvenRow;
				crText = RGB(255,0,0);//GetSysColor (COLOR_WINDOWTEXT);
			}
			else
			{
				crBackground = GetSysColor (COLOR_HIGHLIGHT);
				crText = GetSysColor (COLOR_HIGHLIGHTTEXT);
			}
		}
		else
		{
			// Set the text background and foreground colors to the standard window
			// colors
			crBackground = GetSysColor (COLOR_WINDOW);
			if(m_clrEvenRow>0&&nItem%2==0)
				crBackground = m_clrEvenRow;
			if(m_bImmerseMode)
				crText = m_clrImmerse;
			else
			{
				crText = GetSysColor (COLOR_WINDOWTEXT);
				bCanUseTextColor=TRUE;
			}
		}

		//Get current Icon, you have overridden this I hope :-)
		int iImage = GetIcon(pSelItem);
		if(iImage!=-1)
		{
			if(lvi.iImage!=iImage)
			{
				LV_ITEM lvItem;				
				lvItem.mask =  LVIF_IMAGE;
				lvItem.iImage = iImage;
				lvItem.iItem = nItem;
				lvItem.iSubItem = 0;
				SetItem(&lvItem);
				lvi.iImage = iImage; 
			}
		}

		CRect rc;
		GetItemRect(nItem, rc, LVIR_BOUNDS);
		CRect rcIcon;
		GetItemRect(nItem, rcIcon, LVIR_ICON);
		//Draw Current image
		int nOffset = (rcItem.Height() - m_cyImage)/2;
		int nY = rcItem.bottom - m_cyImage - nOffset;
		int nWidth = m_cxImage;//def icon size
		//do not draw icon out side 1.st column.
		int iTryIndent = 0;
		if(m_himl)	//has icon //wht 10-05-21
			iTryIndent=(GetIndent(pSelItem)+1) * m_cxImage;
		else		//hasn't icon
			iTryIndent=GetIndent(pSelItem) * m_cxImage;
		if(rc.left + iTryIndent  > GetColumnWidth(0)-2/*looks better -2*/)
			nWidth = (rc.left + iTryIndent) - GetColumnWidth(0);


		//Draw selection bar (erase old selection too)
		pDC->SetBkColor(crBackground);

		CRect rcClip = orgRectItem;
		rcClip.left += (iTryIndent+2);//GetIndent(pSelItem) * m_cxImage + m_cxImage + 2;
		if(rcClip.left > GetColumnWidth(0))
			rcClip.left = GetColumnWidth(0);
		//fill background color
		ExtTextOut(pDC->GetSafeHdc(), 0, 0, ETO_OPAQUE, rcClip, NULL, 0, NULL);
		
		//draw color in first col if any
		rcClip.right = rcLabel.right;
		CListCtrlItemInfo *pItemInfo = pSelItem->m_lpNodeInfo;		
		COLORREF clf = pItemInfo->GetSubItemColor(0);
		if(clf!=-1)
		{	
			CBrush br(clf);
			//第一个单元格填充颜色时从头开始 wht 10-09-10
			CRect realRect=orgRectItem;
			realRect.right=rcClip.right;
			pDC->FillRect(realRect, &br);//rcClip
		}
		//draw selection rect in 1.st col if selected
		if ((lpDrawItemStruct->itemState & ODS_SELECTED) && (m_CurSubItem==0))
		{
			CBrush br(GetCellRGB());
			pDC->FillRect(rcClip,&br);
			pDC->DrawFocusRect(rcClip);
		}

		//if checkbox style
		UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
		if (nStateImageMask)
		{
			int nImage = (nStateImageMask>>12) - 1;
			CImageList *pImageList = GetImageList(LVSIL_STATE);
			if (pImageList)
			{
				int cxIcon,cyIcon=0;
				ImageList_GetIconSize(pImageList->m_hImageList, &cxIcon, &cyIcon);
				if(rc.left + (GetIndent(pSelItem) * m_cxImage) + m_cxImage + cxIcon < GetColumnWidth(0))
					pImageList->Draw(pDC, nImage,CPoint(rc.left + (GetIndent(pSelItem) * m_cxImage) + cxIcon, nY), ILD_TRANSPARENT);
			}
		}
		//将绘制加减号以及添加图像的操作放置在给第一个单元格填充颜色之后 wht 10-09-10
		UINT uiFlags = ILD_TRANSPARENT;
		if( GetItemState(nItem, LVIF_STATE) & LVIS_DROPHILITED)//if dragging show a SelectDropTarget alike effect :)
			uiFlags |= ILD_BLEND50;
		
		if((nWidth=m_cxImage-nWidth) >-1)//calc width of icon
		{
			ImageList_DrawEx(m_himl, lvi.iImage, pDC->m_hDC,  
				rc.left + (GetIndent(pSelItem) * m_cxImage), 
				nY, 
				nWidth,	
				m_cyImage,
				CLR_DEFAULT, 
				CLR_DEFAULT, 
				uiFlags);
			DrawTreeItem(pDC, pSelItem, nItem, rc);
		}

		//draw 1. item	
		GetItemRect(nItem, rcItem, LVIR_LABEL);
		if(m_himl==NULL) //wht 10-05-21
			rcItem.left+=iTryIndent;
		pszText = MakeShortString(pDC, szBuff, rcItem.right - rcItem.left, 2*OFFSET_FIRST);
		rcLabel = rcItem;
		rcLabel.left+=OFFSET_FIRST;
		rcLabel.right-=OFFSET_FIRST;
		
		pDC->SetBkColor (crBackground);
		COLORREF cellTextClr=-1,defaultTextClr=crText;
		if(bCanUseTextColor&&(cellTextClr=pItemInfo->GetSubItemTextColor(0))!=-1)
			crText=cellTextClr;
		else
			crText=defaultTextClr;
		pDC->SetTextColor (crText);
		pDC->DrawText(pszText,-1, rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | m_verticalAligment | DT_EXTERNALLEADING);

		//draw subitems..
		LV_COLUMN lvc;
		lvc.mask = LVCF_FMT | LVCF_WIDTH;
		for(int nColumn=1; GetColumn(nColumn, &lvc); nColumn++)
		{
			rcItem.left=rcItem.right;
			rcItem.right+=lvc.cx;
			if (rcItem.left < rcClipBox.right && rcItem.right > rcClipBox.left && rcItem.right > rcItem.left)
			{
				//support for colors in each cell
				COLORREF clf = pItemInfo->GetSubItemColor(nColumn);
				if(clf!=-1)
				{	
					CBrush br(clf);
					pDC->FillRect(rcItem, &br);
				}
				
				int nRetLen = GetItemText(nItem, nColumn, szBuff, sizeof(szBuff));
				if(nRetLen==0)
					pszText=NULL;
				else
					pszText=MakeShortString(pDC,szBuff,rcItem.right-rcItem.left,2*OFFSET_OTHER);

				UINT nJustify=DT_LEFT;

				if(pszText==szBuff)
				{
					switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
					{
					case LVCFMT_RIGHT:
						nJustify=DT_RIGHT;
						break;
					case LVCFMT_CENTER:
						nJustify=DT_CENTER;
						break;
					default:
						break;
					}
				}
				rcLabel=rcItem;
				rcLabel.left+=OFFSET_OTHER;
				rcLabel.right-=OFFSET_OTHER;
				if (lpDrawItemStruct->itemState & ODS_SELECTED && !m_bIsDragging)
					DrawFocusCell(pDC, lpDrawItemStruct->itemID, nColumn);
				if(pszText!=NULL)
				{
					if(strstr(pszText,"RGB")!=NULL)
					{
						char temp_clr[101]="";
						_snprintf(temp_clr,100,"%s",pszText);
						memmove(temp_clr,temp_clr+3,97);
						COLORREF itemColor;
						sscanf(temp_clr,"%X",&itemColor);
						//设置颜色区域
						CRect	colorRect;
						colorRect.left=rcItem.left+2;
						colorRect.top=rcItem.top+2;
						colorRect.right=rcItem.right-27;
						colorRect.bottom=rcItem.bottom - 2;
						//画出颜色
						CBrush	brush(itemColor);
						CBrush	*oldbrush=pDC->SelectObject (&brush);
						pDC->Rectangle (&colorRect);
						pDC->SelectObject (oldbrush);
						brush.DeleteObject();
					}
					else
					{
						if(bCanUseTextColor&&(cellTextClr=pItemInfo->GetSubItemTextColor(nColumn))!=-1)
							crText=cellTextClr;
						else
							crText=defaultTextClr;
						pDC->SetTextColor (crText);
						pDC->DrawText(pszText,-1,rcLabel, nJustify | DT_SINGLELINE | DT_NOPREFIX | m_verticalAligment | DT_EXTERNALLEADING);
					}
				}
			}//if
		}//for
		// 绘制删除线 wht 15-06-06
		if(pSelItem->m_bStrikeout)
		{
			POINT lineStart,lineEnd;
			lineStart.y=lineEnd.y=(orgRectItem.bottom+orgRectItem.top)/2;
			lineStart.x=orgRectItem.left;
			lineEnd.x=orgRectItem.right;
			pDC->MoveTo(lineStart);
			pDC->LineTo(lineEnd);
		}
		// 绘制网格线 wht 18-04-20
		// Draw the lines only for LVS_REPORT mode
		if(m_clrGridLine>0 && (GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
		{
			// Get the number of columns
			CClientDC dc(this );
			CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
			int nColumnCount = pHeader->GetItemCount();
			// The bottom of the header corresponds to the top of the line 
			RECT rect;
			pHeader->GetClientRect( &rect );
			int top = rect.bottom;
			// Now get the client rect so we know the line length and
			// when to stop
			GetClientRect( &rect );
			
			CPen newPen, *oldPen;
			newPen.CreatePen(PS_SOLID, 1, m_clrGridLine);
			oldPen = dc.SelectObject(&newPen);
			// The border of the column is offset by the horz scroll
			int borderx = 0 - GetScrollPos( SB_HORZ );
			for( int i = 0; i < nColumnCount; i++ )
			{
				// Get the next border
				borderx += GetColumnWidth( i );
				// if next border is outside client area, break out
				if( borderx >= rect.right ) 
					break;
				// Draw the line.
				dc.MoveTo( borderx-1, top);
				dc.LineTo( borderx-1, rect.bottom );
			}

			// Draw the horizontal grid lines
			// First get the height
			if(GetItemRect( 0, &rect, LVIR_BOUNDS ))
			{
				int height = rect.bottom - rect.top;
				GetClientRect( &rect );
				int width = rect.right;
				for(int i = 1; i <= GetCountPerPage(); i++ )
				{
					dc.MoveTo( 0, top + height*i);
					dc.LineTo( width, top + height*i );
				}
			}
			dc.SelectObject(oldPen);
			newPen.DeleteObject();
		}
	}//ODA_DRAWENTIRE
}

//this piece of code is borrowed from the wndproc.c file in the odlistvw.exe example from MSDN and was converted to mfc-style
void CSuperGridCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    if (lpMeasureItemStruct->CtlType != ODT_LISTVIEW)
        return;

	TEXTMETRIC tm;
	CClientDC dc(this);	
	CFont* pFont = GetFont();
	CFont* pOldFont = dc.SelectObject(pFont);	
	dc.GetTextMetrics(&tm);
	int nItemHeight = tm.tmHeight + tm.tmExternalLeading;
	//lpMeasureItemStruct->itemHeight = nItemHeight + 4; //or should I go for max(nItemheight+4, m_cxImage+2);
	lpMeasureItemStruct->itemHeight=max(nItemHeight, m_cxImage+2);
	dc.SelectObject(pOldFont);
}

//the basic rutine making the ... thing snatched it from some tedious code example some where in MSDN call odlistvw.exe
LPCTSTR CSuperGridCtrl::MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
{
	static const _TCHAR szThreeDots[]=_T("...");

	int nStringLen=lstrlen(lpszLong);

	if(nStringLen==0 || pDC->GetTextExtent(lpszLong,nStringLen).cx + nOffset < nColumnLen)
		return(lpszLong);

	static _TCHAR szShort[MAX_PATH];

	lstrcpy(szShort,lpszLong);
	int nAddLen = pDC->GetTextExtent(szThreeDots,sizeof(szThreeDots)).cx;

	for(int i=nStringLen-1; i > 0; i--)
	{
		szShort[i]=0;
		if(pDC->GetTextExtent(szShort,i).cx + nOffset + nAddLen < nColumnLen)
			break;
	}
	lstrcat(szShort,szThreeDots);
	return(szShort);
}

void CSuperGridCtrl::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	BOOL bRetCode=FALSE;
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	switch(pLVKeyDow->wVKey)
	{
		case VK_SPACE: 
			{
				BOOL processed=FALSE;
				if(FireKeyDownItem!=NULL)
				{
					int nIndex = GetSelectedItem();
					CTreeItem* pSelItem=nIndex!=-1?GetTreeItem(nIndex):NULL;
					processed=FireKeyDownItem(this,pSelItem,pLVKeyDow);
				}
				if(!processed && (GetExtendedStyle() & LVS_EX_CHECKBOXES))
				{	
					int nIndex = GetSelectedItem();
					if(nIndex !=-1)
					{
						CTreeItem* pItem = GetTreeItem(nIndex);
						if(pItem!=NULL)
						{
							CListCtrlItemInfo *pInfo = pItem->m_lpNodeInfo;
							pInfo->SetCheck(!pInfo->GetCheck());
						}
					}
				}
			}break;

		case VK_DELETE: 
			{	//支持
				CPtrList itemPtrSet;
				POSITION pos=GetFirstSelectedItemPosition();
				while (pos!=NULL)
				{
					int nItem=GetNextSelectedItem(pos);
					CTreeItem* pSelItem = GetTreeItem(nItem);
					itemPtrSet.AddTail(pSelItem);
				}
				pos=itemPtrSet.GetHeadPosition();
				while(pos!=NULL)
				{
					CTreeItem *pSelItem=(CTreeItem*)itemPtrSet.GetNext(pos);
					if(pSelItem != NULL)
					{
						// call delete item callback function
						BOOL bCanDelete=TRUE;
						if(FireDeleteItem)	//设置FireDeleteItem()时执行以下操作 wht 11-05-03
							bCanDelete=FireDeleteItem(this,pSelItem);
						if(bCanDelete)
						{
							if(OnDeleteItem(pSelItem, pSelItem->m_nIndex))
								DeleteItemEx(pSelItem, pSelItem->m_nIndex);
						}
					}
				}
			}	break;

		/*case VK_MULTIPLY:
			{  
				int nIndex = GetSelectedItem();
				if(nIndex != -1)
				{
					CWaitCursor wait;
					SetRedraw(0);
					CTreeItem *pParent = GetTreeItem(nIndex);
					int nScroll=0;
					if(OnVKMultiply(pParent, nIndex))
					{	
						ExpandAll(pParent, nScroll);
					}		
					SetRedraw(1);
					RedrawItems(nIndex, nScroll);
					EnsureVisible(nScroll, TRUE);
				 }
			 }break;
		*/
		case VK_ADD:
			{
				int nIndex = GetSelectedItem();
				if(nIndex!=-1)
				{
					CWaitCursor wait;
					CTreeItem *pSelItem = GetTreeItem(nIndex);
					int nScrollIndex = 0;
					if(OnVKAdd(pSelItem, nIndex))
					{
						nScrollIndex = Expand(pSelItem, nIndex);
					}
					CRect rc;
					GetItemRect(nIndex, rc, LVIR_BOUNDS);
					InvalidateRect(rc);
					UpdateWindow();
					EnsureVisible(nScrollIndex, 1);
				}
				
			}break;


		case VK_SUBTRACT:
			{
				int nIndex = GetSelectedItem();
				if(nIndex!=-1)
				{
					CWaitCursor wait;
					CTreeItem *pSelItem = GetTreeItem(nIndex);
					if(OnVkSubTract(pSelItem, nIndex))
					{
						Collapse(pSelItem);
					}
					CRect rc;
					GetItemRect(nIndex, rc, LVIR_BOUNDS);
					InvalidateRect(rc);
					UpdateWindow();
				}
			}break;
		default :
			if(FireKeyDownItem!=NULL)
			{
				int nIndex = GetSelectedItem();
				CTreeItem* pSelItem=nIndex!=-1?GetTreeItem(nIndex):NULL;
				if(pSelItem)
					bRetCode=FireKeyDownItem(this,pSelItem,pLVKeyDow);
			}
			break;
	}
	if(bRetCode)
		*pResult=1;
	else
		*pResult = 0;
}

void CSuperGridCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(FireKeyUpItem)
	{
		int nIndex = GetSelectedItem();
		CTreeItem* pSelItem=nIndex!=-1?GetTreeItem(nIndex):NULL;
		if(FireKeyUpItem(this,pSelItem,nChar,nRepCnt,nFlags))
			return;
	}
	CListCtrl::OnKeyUp(nChar,nRepCnt,nFlags);
}

BOOL CSuperGridCtrl::HitTestOnSign(CPoint point, LVHITTESTINFO& ht)
{
	ht.pt = point;
	// Test which subitem was clicked.
	SubItemHitTest(&ht);
	if(ht.iItem!=-1)
	{
		//first hittest on checkbox
		BOOL bHit = FALSE;
		if(GetExtendedStyle() & LVS_EX_CHECKBOXES)
		{
			if (ht.flags == LVHT_ONITEM && (GetStyle() & LVS_OWNERDRAWFIXED))//isn't this allways ownerdrawfixed :-)
			{
				CRect rcIcon,rcLabel;
				GetItemRect(ht.iItem, rcIcon, LVIR_ICON);//has to be between these two ....right :)
				GetItemRect(ht.iItem, rcLabel, LVIR_LABEL);
				// check if hit was on a state icon 
				if (point.x > rcIcon.left && point.x < rcLabel.left)
					bHit = TRUE;
			}
			else if (ht.flags & LVHT_ONITEMSTATEICON)
				bHit = TRUE;
		}

		CTreeItem* pItem = GetTreeItem(ht.iItem);
		if(pItem!=NULL)
		{
			if(bHit)//if checkbox
			{
				//yes I know..have to maintain to sets of checkstates here...
				//one for listview statemask and one for CTreeItem..but its located here so no harm done
				SetCheck(ht.iItem,!GetCheck(ht.iItem));
				CListCtrlItemInfo *pInfo = pItem->m_lpNodeInfo;
				pInfo->SetCheck(!pInfo->GetCheck());
			}
			//if haschildren and clicked on + or - then expand/collapse
			if(ItemHasChildren(pItem))
			{
				//hittest on the plus/sign "button" 
				CRect rcBounds;
				GetItemRect(ht.iItem, rcBounds, LVIR_BOUNDS);
				CRectangle rect(this, NULL, GetIndent(pItem), rcBounds);
				BOOL bRedraw=0;//if OnItemExpanding or OnCollapsing returns false, dont redraw
				if(rect.HitTest(point))
				{
					SetRedraw(0);
					int nScrollIndex=0;
					if(pItem->m_bHideChildren)
					{	
						if(OnItemExpanding(pItem, ht.iItem))
						{
							nScrollIndex = Expand(pItem, ht.iItem);
							OnItemExpanded(pItem, ht.iItem);
							bRedraw=1;
						}
					}	
					else {
					   if(OnCollapsing(pItem))
					   {
							Collapse(pItem);
							OnItemCollapsed(pItem);
							bRedraw=1;
					   }
					}
					SetRedraw(1);
					if(bRedraw)
					{
						CRect rc;
						GetItemRect(ht.iItem, rc, LVIR_BOUNDS);
						InvalidateRect(rc);
						UpdateWindow();
						EnsureVisible(nScrollIndex, 1);
						return 0;
					}	
				}
			}//has kids
		}//pItem!=NULL
	}
	return 1;
}

void CSuperGridCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if( GetFocus() != this) 
		SetFocus();

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	LVHITTESTINFO ht;
	ht.pt = pNMListView->ptAction;
	SubItemHitTest(&ht);
	if(OnItemLButtonDown(ht))
	{
		BOOL bSelect=1;
		bSelect = HitTestOnSign(pNMListView->ptAction, ht);
		//normal selection
		if(bSelect && ht.iItem !=-1)
		{		
			int nIndex = GetSelectedItem();
			if(nIndex!=-1)
			{
				CTreeItem *pSelItem = GetTreeItem(nIndex);
				if (pSelItem != NULL)
				{
					if(m_bDblclkDisplayCellCtrl)	//设置双击显示可编辑控件时在此处显示控件 wht 11-04-14
						DisplayCellCtrl(ht.iItem,ht.iSubItem);
					BOOL bRedraw=0;
					if(ItemHasChildren(pSelItem))
					{
						SetRedraw(0);
						int nScrollIndex=0;
						if(pSelItem->m_bHideChildren)
						{		
							if(OnItemExpanding(pSelItem, nIndex))
							{
								nScrollIndex = Expand(pSelItem, nIndex);
								OnItemExpanded(pSelItem, nIndex);
								bRedraw=1;
							}
						}	
					
						else 
						{
						   if(OnCollapsing(pSelItem))
						   {
								Collapse(pSelItem);
								OnItemCollapsed(pSelItem);
								bRedraw=1;
						   }
						}
						SetRedraw(1);

						if(bRedraw)
						{
							CRect rc;
							GetItemRect(nIndex,rc,LVIR_BOUNDS);
							InvalidateRect(rc);
							UpdateWindow();
							EnsureVisible(nScrollIndex,1);
						}
					}//ItemHasChildren	
					//双击SuperGridCtrl时调用该函数 wht 10-09-13
					if(FireLButtonDblclk)			
					{	//双击时的回调函数应放置在最后调用，因为在回调函数中可能重新刷新列表内容 wht 12-10-17
						FireLButtonDblclk(this,pSelItem,ht.iSubItem);
					}
				}//!=NULL
			}
		}
	}
	*pResult = 0;
}

void CSuperGridCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem != -1)
	{
		m_nDragItem = pNMListView->iItem;
		CImageList* pDragImage=NULL;
		pDragImage = CreateDragImageEx(m_nDragItem);//override this if you want another dragimage or none at all.
		if(pDragImage)
		{
			pDragImage->BeginDrag(0, CPoint(0,0));
			pDragImage->DragEnter(this, pNMListView->ptAction);
			SetCapture();
			m_bIsDragging = TRUE;
		}
		delete pDragImage;
	}
	*pResult = 0;
}

//Create dragimage : Icon + the itemtext
CImageList *CSuperGridCtrl::CreateDragImageEx(int nItem)
{
	if(!m_bDrag)
		return NULL;
    CImageList *pList = new CImageList;          
	//get image index
	LV_ITEM lvItem;
	lvItem.mask =  LVIF_IMAGE;
	lvItem.iItem = nItem;
	lvItem.iSubItem = 0;
	GetItem(&lvItem);

	CRect rc;
	GetItemRect(nItem, &rc, LVIR_BOUNDS);         

	CString str;
	str = GetItemText(nItem, 0);
	CFont *pFont = GetFont();

	rc.OffsetRect(-rc.left, -rc.top);            
	rc.right = GetColumnWidth(0);                
	pList->Create(rc.Width(), rc.Height(),ILC_COLOR24| ILC_MASK , 1, 1);
	CDC *pDC = GetDC();                          
	if(pDC) 
	{
		CDC dc;	      
		dc.CreateCompatibleDC(pDC);      
		CBitmap bmpMap;
		bmpMap.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());

		CBitmap *pOldBmp = dc.SelectObject(&bmpMap);
		CFont *pOldFont = dc.SelectObject(pFont);
		dc.FillSolidRect(rc, GetSysColor(COLOR_WINDOW));
		CImageList *pImgList = GetImageList( LVSIL_SMALL);
		if(pImgList)
			pImgList->Draw(&dc, lvItem.iImage, CPoint(0,0), ILD_TRANSPARENT);
		dc.TextOut(m_cxImage + 4, 0, str);
		dc.SelectObject(pOldFont);
		dc.SelectObject(pOldBmp);                 
		//causes an error if the 1st column is hidden so must check the imagelist
		if(pList->m_hImageList != NULL)
			pList->Add(&bmpMap, RGB(255,255,255));
		else { 
			delete pList;
			pList=NULL;
		}
		ReleaseDC(pDC);   
	}   
	return pList;
}
void CSuperGridCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
    if(m_bIsDragging)
    {
		KillTimer(1);
		if (CWnd::GetCapture() != this)
			m_bIsDragging=0;
		
		if(nFlags==MK_RBUTTON || nFlags==MK_MBUTTON)
			m_bIsDragging=0;

		
		if(GetKeyState(VK_ESCAPE) < 0)		
			m_bIsDragging=0;
		
		if(!m_bIsDragging)//why not put this in a funtion :)
		{
			SetItemState (m_nDragTarget, 0, LVIS_DROPHILITED);
			CImageList::DragLeave(this);
			CImageList::EndDrag();
			ReleaseCapture();
			InvalidateRect(NULL);
			UpdateWindow();
		}
		else
		{
			CPoint ptList(point);
			MapWindowPoints(this, &ptList, 1);
			CImageList::DragMove(ptList);
			UINT uHitTest = LVHT_ONITEM;
			m_nDragTarget = HitTest(ptList, &uHitTest);
			// try turn off hilight for previous DROPHILITED state
			int nPrev = GetNextItem(-1,LVNI_DROPHILITED);
			if(nPrev != m_nDragTarget)//prevents flicker 
				SetItemState(nPrev, 0, LVIS_DROPHILITED);

			CRect rect;
			GetClientRect (rect);
			int cy = rect.Height();
			if(m_nDragTarget!=-1)
			{
				SetItemState(m_nDragTarget, LVIS_DROPHILITED, LVIS_DROPHILITED);
				CTreeItem* pTarget = GetTreeItem(m_nDragTarget);
				if ((point.y >= 0 && point.y <= m_cyImage) || (point.y >= cy - m_cyImage && point.y <= cy) || 	
					( pTarget!=NULL && ItemHasChildren(pTarget) && pTarget->m_bHideChildren))
				{
					SetTimer(1,300,NULL);
				}
			}
		}
    }
	CListCtrl::OnMouseMove(nFlags, point);
}

#if defined(_WIN64)
void CSuperGridCtrl::OnTimer(UINT_PTR nIDEvent) 
#else
void CSuperGridCtrl::OnTimer(UINT nIDEvent) 
#endif
{
	CListCtrl::OnTimer(nIDEvent);
	if(nIDEvent==1)
	{
		if(CWnd::GetCapture()!=this)
		{
			SetItemState(m_nDragTarget, 0, LVIS_DROPHILITED);
			m_bIsDragging=0;
			CImageList::DragLeave(this);
			CImageList::EndDrag();
			ReleaseCapture();
			InvalidateRect(NULL);
			UpdateWindow();
			KillTimer(1);
			return;
		}

		SetTimer(1,300,NULL);//reset timer
		DWORD dwPos = ::GetMessagePos();
		CPoint ptList(LOWORD(dwPos),HIWORD(dwPos));
		ScreenToClient(&ptList);

		CRect rect;
		GetClientRect(rect);
		int cy = rect.Height();
		//
		// perform autoscroll if the cursor is near the top or bottom.
		//
		if (ptList.y >= 0 && ptList.y <= m_cyImage) 
		{
			int n = GetTopIndex(); 
			CImageList::DragShowNolock(0);
			SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), NULL);
			CImageList::DragShowNolock(1);
			if (GetTopIndex()== n)
				KillTimer (1);
			else {
				CImageList::DragShowNolock(0);
				CImageList::DragMove(ptList);
				CImageList::DragShowNolock(1);
				return;
			}
		}
		else if (ptList.y >= cy - m_cyImage && ptList.y <= cy) 
		{
			int n = GetTopIndex(); 
			CImageList::DragShowNolock(0);
			SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, 0), NULL);
			CImageList::DragShowNolock(1);
			if (GetTopIndex()== n)
				KillTimer (1);
			else {
				CImageList::DragShowNolock(0);
				CImageList::DragMove(ptList);
				CImageList::DragShowNolock(1);
				return;
			}
		}
		//Hover test 
		CImageList::DragMove(ptList);
		UINT uHitTest = LVHT_ONITEM;
		m_nDragTarget = HitTest(ptList, &uHitTest);
	
		if(m_nDragTarget!=-1)
		{
			//if the target has children
			//expand them
			CTreeItem* pTarget=GetTreeItem(m_nDragTarget);
			if(pTarget != NULL && ItemHasChildren(pTarget) && pTarget->m_bHideChildren && (m_nDragItem!=-1))
			{
				CImageList::DragShowNolock(0);
				CTreeItem* pSource = GetTreeItem(m_nDragItem);
	
				SetRedraw(0);
				int nScrollIndex=0;
				if(ItemHasChildren(pTarget) && pTarget->m_bHideChildren)
				{	
					if(OnItemExpanding(pTarget, m_nDragTarget))
					{	
						nScrollIndex = Expand(pTarget, m_nDragTarget);
						OnItemExpanded(pTarget, m_nDragTarget);
					}
				}		
				m_nDragItem = NodeToIndex(pSource);
				SetRedraw(1);
				EnsureVisible(nScrollIndex, 1);
				InvalidateRect(NULL);
				UpdateWindow();
				CImageList::DragShowNolock(1);
				KillTimer(1);
				return;
			}	
		}
		KillTimer(1);
	}
}

void CSuperGridCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bIsDragging == TRUE)
    {
		KillTimer(1);
        CImageList::DragLeave(this);
        CImageList::EndDrag();
        ReleaseCapture();
        m_bIsDragging = FALSE;
		SetItemState(m_nDragTarget, 0, LVIS_DROPHILITED);
        if((m_nDragTarget != -1) && (m_nDragTarget != m_nDragItem) && (m_nDragItem!=-1))//no drop on me self
        {
		
			CTreeItem* pSource = GetTreeItem(m_nDragItem);
			CTreeItem* pTarget = GetTreeItem(m_nDragTarget);
			if(IsRoot(pSource))
				return;
			CTreeItem* pParent = GetParentItem(pSource);
			if(pParent==pTarget) //can't drag child to parent
				return;

			if(!IsChildOf(pSource, pTarget))//can't drag parent to child
			{
				CWaitCursor wait;
				SetRedraw(0);
				if(DoDragDrop(pTarget, pSource))
				{
					UINT uflag = LVIS_SELECTED | LVIS_FOCUSED;
					SetItemState(m_nDragTarget, uflag, uflag);
					m_nDragItem=-1;
					//delete source
					int nIndex = NodeToIndex(pSource);			
					DeleteItem(nIndex);
					HideChildren(pSource, TRUE, nIndex);
					Delete(pSource);
					InternaleUpdateTree();
					SetRedraw(1);
					InvalidateRect(NULL);
					UpdateWindow();
				}else
					SetRedraw(1);
			}
	    }
    }
    else
		CListCtrl::OnLButtonUp(nFlags, point);
}

//used with the drag/drop operation
void CSuperGridCtrl::CopyChildren(CTreeItem* pDest, CTreeItem* pSrc)
{
	if (ItemHasChildren(pSrc))
	{
		POSITION pos = pSrc->m_listChild.GetHeadPosition();
		while (pos != NULL)
		{
			CTreeItem* pItem = (CTreeItem *)pSrc->m_listChild.GetNext(pos);
			CListCtrlItemInfo* lp = CopyData(pItem->m_lpNodeInfo);
			CTreeItem* pNewItem = InsertItem(pDest, lp);
			CopyChildren(pNewItem, pItem);
		}
	}
}

//hmmm 
BOOL CSuperGridCtrl::DoDragDrop(CTreeItem* pTarget, CTreeItem* pSource)
{
	if(pTarget==NULL)
		return 0;

	bool bUpdate=false;
	if(!pTarget->m_bHideChildren)
		bUpdate=true; //children are expanded, want to see update right away
	
	//make a copy of the source data
	CListCtrlItemInfo* lp = CopyData(pSource->m_lpNodeInfo);
	//create new node with the source data and make pTarget the parent
	
	CTreeItem* pNewParent = InsertItem(pTarget, lp, -1,bUpdate);
	//if the source has children copy all source data and make the newly create item the parent
	if(ItemHasChildren(pSource))
		CopyChildren(pNewParent, pSource);

	return 1;
}


void CSuperGridCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{	//its not meself
	if( GetFocus() != this) 
		SetFocus();
	if(FireScroll)
		FireScroll(this,nSBCode,nPos,pScrollBar,TRUE);
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSuperGridCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( GetFocus() != this) 
		SetFocus();
	if(FireScroll)
		FireScroll(this,nSBCode,nPos,pScrollBar,FALSE);
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CSuperGridCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN&&GetFocus()==this)
	{
		switch( pMsg->wParam )
		{
		case VK_LEFT:
			{
				BOOL bRetCode=FALSE;
				if(FireKeyDownItem)
				{
					int iItem = GetSelectedItem();
					CTreeItem* pSelItem=iItem!=-1?GetTreeItem(iItem):NULL;
					NMLVKEYDOWN vKeyDown;
					vKeyDown.wVKey=pMsg->wParam;
					bRetCode=FireKeyDownItem(this,pSelItem,&vKeyDown);
					if(bRetCode)
						return TRUE;
				}
				if(!bRetCode)
				{
					// Decrement the order number.
					m_CurSubItem--;
					if(m_CurSubItem < 0) 
						m_CurSubItem = 0;
					else{
					
						CHeaderCtrl* pHeader = GetHeaderCtrl();
						// Make the column visible.
						// We have to take into account that the header may be reordered.
						MakeColumnVisible( Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem));
						// Invalidate the item.
						int iItem = GetSelectedItem();
						if( iItem != -1 )
						{
							CRect rcBounds;
							GetItemRect(iItem, rcBounds, LVIR_BOUNDS);
							InvalidateRect(&rcBounds);
							UpdateWindow();
						}
					}
					return TRUE;
				}
			}
			break;
		case VK_RIGHT:
			{
				BOOL bRetCode=FALSE;
				if(FireKeyDownItem)
				{
					int iItem = GetSelectedItem();
					CTreeItem* pSelItem=iItem!=-1?GetTreeItem(iItem):NULL;
					NMLVKEYDOWN vKeyDown;
					vKeyDown.wVKey=pMsg->wParam;
					bRetCode=FireKeyDownItem(this,pSelItem,&vKeyDown);
					if(bRetCode)
						return TRUE;
				}
				if(!bRetCode)
				{
					// Increment the order number.
					m_CurSubItem++;
					CHeaderCtrl* pHeader = GetHeaderCtrl();
					int nColumnCount = pHeader->GetItemCount();
					// Don't go beyond the last column.
					if( m_CurSubItem > nColumnCount -1 ) 
						m_CurSubItem = nColumnCount-1;
					else
					{
						MakeColumnVisible(Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem));
					
						int iItem = GetSelectedItem();
						// Invalidate the item.
						if( iItem != -1 )
						{
							CRect rcBounds;
							GetItemRect(iItem, rcBounds, LVIR_BOUNDS);
							InvalidateRect(&rcBounds);
							UpdateWindow();
						}
					}
					return TRUE;
				}
			}
			break;
		case VK_UP:
		case VK_DOWN:
		case VK_HOME:
		case VK_END:
			if(FireKeyDownItem)
			{
				int iItem = GetSelectedItem();
				CTreeItem* pSelItem=iItem!=-1?GetTreeItem(iItem):NULL;
				NMLVKEYDOWN vKeyDown;
				vKeyDown.wVKey=pMsg->wParam;
				if(FireKeyDownItem(this,pSelItem,&vKeyDown))
					return TRUE;
			}
			break;
		case VK_RETURN://edit itemdata
			{
				if(FireKeyDownItem)
				{
					int iItem = GetSelectedItem();
					CTreeItem* pSelItem=iItem!=-1?GetTreeItem(iItem):NULL;
					NMLVKEYDOWN vKeyDown;
					vKeyDown.wVKey=pMsg->wParam;
					if(FireKeyDownItem(this,pSelItem,&vKeyDown))
						return TRUE;
				}
				BOOL bResult = OnVkReturn();
				if(!bResult)
				{
					int iItem = GetSelectedItem();
					if( m_CurSubItem != -1 && iItem != -1)
					{
						CHeaderCtrl* pHeader = GetHeaderCtrl();
						int iSubItem = Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem);
						DisplayCellCtrl(iItem, iSubItem);	
						return 1;
					}
				}
			}
			break;
		default:
			{
				if(FireKeyDownItem)
				{
					int iItem = GetSelectedItem();
					CTreeItem* pSelItem=iItem!=-1?GetTreeItem(iItem):NULL;
					NMLVKEYDOWN vKeyDown;
					if(GetAsyncKeyState(VK_CONTROL))
						vKeyDown.flags=MK_CONTROL;
					else
						vKeyDown.flags=0;
					vKeyDown.wVKey=pMsg->wParam;
					if(FireKeyDownItem(this,pSelItem,&vKeyDown))
						return TRUE;

				}
			}
			break;
		}
	}
	return CListCtrl::PreTranslateMessage(pMsg);
}

int CSuperGridCtrl::GetNumCol()
{
	CHeaderCtrl* pHeader = GetHeaderCtrl();
	return pHeader ? pHeader->GetItemCount() : 0;
}

//Think Rex Myer is spooking here
void CSuperGridCtrl::MakeColumnVisible(int nCol)
{
	if(nCol < 0)
		return;
	// Get the order array to total the column offset.
	CHeaderCtrl* pHeader = GetHeaderCtrl();

	int nColCount = pHeader->GetItemCount();
	ASSERT( nCol < nColCount);
	int *pOrderarray = new int[nColCount];
	Header_GetOrderArray(pHeader->m_hWnd, nColCount, pOrderarray);
	// Get the column offset
	int offset = 0;
	for(int i = 0; pOrderarray[i] != nCol; i++)
		offset += GetColumnWidth(pOrderarray[i]);

	int colwidth = GetColumnWidth(nCol);
	delete[] pOrderarray;

	CRect rect;
	GetItemRect(0, &rect, LVIR_BOUNDS);
	// Now scroll if we need to show the column
	CRect rcClient;
	GetClientRect(&rcClient);
	if(offset + rect.left < 0 || offset + colwidth + rect.left > rcClient.right)
	{
		CSize size(offset + rect.left,0);
		Scroll(size);
		InvalidateRect(NULL);
		UpdateWindow();
	}
}

//Think Rex Myer is spooking here
int CSuperGridCtrl::IndexToOrder( int iIndex )
{
	// This translates a column index value to a column order value.
	CHeaderCtrl* pHeader = GetHeaderCtrl();
	int nColCount = pHeader->GetItemCount();
	int *pOrderarray = new int[nColCount];
	Header_GetOrderArray(pHeader->m_hWnd, nColCount, pOrderarray);
	for(int i=0; i<nColCount; i++)
	{
		if(pOrderarray[i] == iIndex )
		{
			delete[] pOrderarray;
			return i;
		}
	}
	delete[] pOrderarray;
	return -1;
}

void CSuperGridCtrl::DrawFocusCell(CDC *pDC, int nItem, int iSubItem)
{
	if(iSubItem==m_CurSubItem)
	{
		CRect rect;
		GetSubItemRect(nItem, iSubItem, LVIR_BOUNDS, rect);
		CBrush br(GetCellRGB());
		if(iSubItem==0)
			GetItemRect(iSubItem, rect, LVIR_LABEL);
		pDC->FillRect(rect, &br);
		pDC->DrawFocusRect(rect);
	}
}

//insert item and return new parent pointer.
CSuperGridCtrl::CTreeItem* CSuperGridCtrl::
InsertItem(CTreeItem *pParent, CListCtrlItemInfo* lpInfo, int iInsertIndex/*=-1*/, bool bUpdate/*=false*/)
{
	if(pParent==NULL)
		return NULL;

	CTreeItem *pItem = NULL;
	//TODO: 会出现内存泄漏 wjh-2013-8-13
	pItem =  new CTreeItem();

	if(lpInfo==NULL)
		lpInfo = new CListCtrlItemInfo;

	pItem->m_lpNodeInfo=lpInfo;
	
	int insert_index=-1;
	if(iInsertIndex<0||pParent->m_listChild.GetCount()==0)
		pParent->m_listChild.AddTail(pItem);	//add as the last child 
	else
	{	//在指定索引后插入节点 wht 12-03-25
		CTreeItem *pTailItem=NULL,*pPrevItem=NULL;
		int i=pParent->GetIndex();	//初始化索引值
		POSITION pos=NULL;
		for(pos = pParent->m_listChild.GetHeadPosition();pos!=NULL;i++)
		{	//遍历子节点，查找插入位置
			if(i==iInsertIndex)
			{
				pParent->m_listChild.InsertBefore(pos,pItem);
				if(pPrevItem)
					insert_index=pPrevItem->GetTailIndex()+1;
				break;
			}
			//前一节点存在子节点,当前索引设置为前一节点尾节点索引
			if(pPrevItem&&!pPrevItem->m_listChild.IsEmpty())
				i=pPrevItem->GetTailIndex();
			//更新前一节点
			pPrevItem=(CTreeItem*)pParent->m_listChild.GetNext(pos);
		}
		if(pos==NULL)
			pParent->m_listChild.AddTail(pItem);
	}
	//
	InternalInsertItem(pParent,pItem,bUpdate,insert_index);
	if(bUpdate)
		InternaleUpdateTree();
	return pItem;
}	

void CSuperGridCtrl::InternaleUpdateTree()
{
	int nItems = GetItemCount();
	for(int nItem=0; nItem < nItems; nItem++)
	{
		CTreeItem* pItem = GetTreeItem(nItem);
		SetCurIndex(pItem, nItem);
	}
}

int CSuperGridCtrl::NodeToIndex(CTreeItem *pNode)
{
	int nStartIndex=0;
	POSITION pos = m_RootItems.GetHeadPosition();
	while(pos!=NULL)
	{
		CTreeItem * pRoot = (CTreeItem*)m_RootItems.GetNext(pos);
		if(pRoot&&(pRoot->m_dwPropGroup&GetBinaryWord(m_iPropGroup))==0)
			continue;	//不属于当前分组 wht 10-12-02
		int ret = _NodeToIndex(pRoot, pNode, nStartIndex);
		if(ret != -1)
			return ret;
	}
	return -1;
}

CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetRootItem(int nIndex)
{
	POSITION pos = m_RootItems.FindIndex(nIndex);
	if(pos==NULL)
		return NULL;
	return (CTreeItem*)m_RootItems.GetAt(pos);
}

int CSuperGridCtrl::GetRootIndex(CTreeItem * root)
{
	int nIndex = 0;
	POSITION pos = m_RootItems.GetHeadPosition();
	while(pos != NULL)
	{
		CTreeItem * pItem = (CTreeItem*)m_RootItems.GetNext(pos);
		if(pItem&&(pItem->m_dwPropGroup&GetBinaryWord(m_iPropGroup))==0)
			continue;	//不属于当前分组 wht 10-12-02
		if(pItem== root)
			return nIndex;
		nIndex++;
	}
	return -1;
}

BOOL CSuperGridCtrl::IsRoot(CTreeItem * lpItem)
{
	return m_RootItems.Find(lpItem) != NULL;
}

void CSuperGridCtrl::DeleteRootItem(CTreeItem * root)
{
	POSITION pos = m_RootItems.Find(root);
	if(pos!=NULL)
	{
		CTreeItem* pRoot=(CTreeItem*)m_RootItems.GetAt(pos);
		if(pRoot->m_lpNodeInfo!=NULL)
			delete pRoot->m_lpNodeInfo;
		delete pRoot;
		m_RootItems.RemoveAt(pos);
	}
}

void CSuperGridCtrl::InternalUpdateItemText(CSuperGridCtrl::CTreeItem *pItem)
{
	if(pItem==NULL)
		return;
	int nSize = pItem->m_lpNodeInfo->GetItemCount();
	for(int i=0; i< nSize;i++)
	{
		CString str=pItem->m_lpNodeInfo->GetSubItemText(i);
		SetItemText(pItem->GetIndex(),i,str);
	}
}
// Inserts a column into a report-mode control.
int CSuperGridCtrl::_InsertColumn(int nCol, LPCTSTR lpszColumnHeading,int nWidth)
{
	m_ctrlHeader.m_titleArr.Add(lpszColumnHeading);
	LVCOLUMN lvc;
	lvc.iSubItem = nCol;//nColCount;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
	lvc.cx=nWidth;
	char szColTitle[64]={0};
	if(lpszColumnHeading && strlen(lpszColumnHeading)<64)
		strcpy(szColTitle,lpszColumnHeading);	//防止lengthOfDestBuf输入错误,实际字符串也并不超过strDest缓存长度时出错 
	else if(lpszColumnHeading)
		strncpy(szColTitle,lpszColumnHeading,64-1);
	lvc.pszText=szColTitle;
	int index=-1;
	if((index=InsertColumn(nCol,&lvc))<0)
		return -1;
	POSITION pos = m_RootItems.GetHeadPosition();
	while(pos != NULL)
	{
		CTreeItem * pItem = (CTreeItem*)m_RootItems.GetNext(pos);
		if(pItem!=NULL)
			pItem->InsertSubItemText(this,nCol);
	}
	return index;
}
// Deletes a column from a report-mode control.
BOOL CSuperGridCtrl::_DeleteColumn(int nCol)
{
	if(!DeleteColumn(nCol))
		return FALSE;

	POSITION pos = m_RootItems.GetHeadPosition();
	while(pos != NULL)
	{
		CTreeItem * pItem = (CTreeItem*)m_RootItems.GetNext(pos);
		if(pItem!=NULL)
			pItem->DeleteSubItemText(this,nCol);
	}
	return TRUE;
}
void CSuperGridCtrl::SetColumnHeaderTitle(int nCol, LPCTSTR lpszColumnHeading/*=""*/)
{
	LVCOLUMN lvc;
	char szHeaderTitle[256]={0};
	lvc.pszText=szHeaderTitle;
	lvc.cchTextMax=256;
	lvc.iSubItem=nCol;
	lvc.mask=LVCF_TEXT;
	if(lpszColumnHeading && strlen(lpszColumnHeading)<256)
		strcpy(szHeaderTitle,lpszColumnHeading);	//防止lengthOfDestBuf输入错误,实际字符串也并不超过strDest缓存长度时出错 
	else if(lpszColumnHeading)
		strncpy(szHeaderTitle,lpszColumnHeading,256-1);
	strcpy(szHeaderTitle,lpszColumnHeading);
	SetColumn(nCol,&lvc);
}
CSuperGridCtrl::CTreeItem*  CSuperGridCtrl::InsertRootItem(CListCtrlItemInfo * lpInfo,BOOL bUpdate/*=TRUE*/)
{
	if(lpInfo==NULL)
		lpInfo = new CListCtrlItemInfo;

	CTreeItem* pRoot = NULL;
	
	pRoot =  new CTreeItem();

	CleanMe(pRoot);
	pRoot->m_lpNodeInfo=lpInfo;
	m_RootItems.AddTail(pRoot);
	//
	InternalInsertItem(NULL,pRoot,bUpdate);
	if(bUpdate)
		InternaleUpdateTree();
	return pRoot;
}

void CSuperGridCtrl::MoveTreeItem(CTreeItem* pSelItem,int iInsertIndex)
{
	MoveTreeItems(&pSelItem,1,iInsertIndex);
}
void CSuperGridCtrl::MoveTreeItems(CTreeItem** ppSelItem,int nItemCount,int iInsertIndex)
{
	if(ppSelItem==NULL)
		return;
	if(iInsertIndex<0)
		iInsertIndex=0;
	else if(iInsertIndex>GetItemCount())
		iInsertIndex=GetItemCount();
	CTreeItem* pSelItem=ppSelItem[0];
	CTreeItem* pPrevItem=iInsertIndex>0?GetTreeItem(iInsertIndex-1):NULL;
	int i,iFirstIndex=pSelItem->GetIndex();
	if(iInsertIndex==iFirstIndex || iInsertIndex<0)
		return;
	CPtrList temporyItems;
	POSITION pos = m_RootItems.GetHeadPosition();
	while(pos!=NULL)
	{
		CTreeItem* pItem=(CTreeItem*)m_RootItems.GetNext(pos);
		for(i=0;i<nItemCount;i++)
		{
			if(ppSelItem[i]==pItem)
				break;
		}
		if(i==nItemCount)
			temporyItems.AddTail(pItem);
	}
	m_RootItems.RemoveAll();
	pos = temporyItems.GetHeadPosition();
	CTreeItem* pLastItem=(CTreeItem*)temporyItems.GetNext(pos);
	bool inserted=false;
	while(pLastItem)
	{
		if(pPrevItem==NULL&&!inserted)
		{
			for(int j=0;j<nItemCount;j++)
				m_RootItems.AddTail(ppSelItem[j]);
			inserted=true;
		}
 		m_RootItems.AddTail(pLastItem);
		if(pPrevItem==pLastItem&&!inserted)
		{
			for(int j=0;j<nItemCount;j++)
				m_RootItems.AddTail(ppSelItem[j]);
			inserted=true;
		}
		if(pos==NULL)
			pLastItem=NULL;
		else	
			pLastItem=(CTreeItem*)temporyItems.GetNext(pos);
	}
	if (!inserted)
	{
		for (int j = 0; j < nItemCount; j++)
			m_RootItems.AddTail(ppSelItem[j]);
		inserted = true;
	}
	Redraw();
}

void CSuperGridCtrl::DrawTreeItem(CDC* pDC, CTreeItem* pSelItem, int nListItem, const CRect& rcBounds)
{
	if(m_cxImage==0)
		return;
	int nColWidth = GetColumnWidth(0);
	int yDown = rcBounds.top;
    CPen* pPenTreeLine = pDC->SelectObject(&m_psTreeLine);
	int iIndent = GetIndent(pSelItem);
	int nHalfImage = (m_cxImage >> 1);
	int nBottomDown = yDown + nHalfImage + ((rcBounds.Height() - m_cyImage) >> 1);
	//
	BOOL bChild = ItemHasChildren(pSelItem);
	BOOL bCollapsed = pSelItem->m_bHideChildren;
	//draw outline	
	while(1)
	{
		CTreeItem* pParent = GetParentItem(pSelItem);
		if(pParent==NULL)//no more parents, stop
			break;

		POSITION pos = pParent->m_listChild.GetTailPosition();
		while(pos!=NULL)
		{
			CTreeItem *pLastChild = (CTreeItem*)pParent->m_listChild.GetPrev(pos);
			int nIndex = GetCurIndex(pLastChild);
			int nCurIndent = GetIndent(pLastChild);
			if(nListItem > nIndex && iIndent > nCurIndent)//no need to go further in this loop
				break;

			//no drawing outside the 1st columns right
			int xLine =  rcBounds.left + nCurIndent * m_cxImage - nHalfImage;
			if(nIndex == nListItem && nCurIndent==iIndent)
			{
				//draw '-
				int x;
				pDC->MoveTo(xLine, yDown);
				pDC->LineTo(xLine, nBottomDown);
				// -
				xLine + nHalfImage > nColWidth ? x = nColWidth: x = xLine + nHalfImage;
				
				pDC->MoveTo(xLine, nBottomDown);
				pDC->LineTo(x, nBottomDown);
				break;
			}
			else
			if(nIndex > nListItem && nCurIndent==iIndent)
			{
				//draw |-
				int x;
				xLine + nHalfImage > nColWidth ? x = nColWidth : x = xLine + nHalfImage;
				pDC->MoveTo(xLine, nBottomDown);
				pDC->LineTo(x, nBottomDown);
				//-
				pDC->MoveTo(xLine, yDown);
				pDC->LineTo(xLine, rcBounds.bottom);
				break;
			}
			else
			if(nIndex > nListItem && nCurIndent < iIndent)
			{
				//draw |
				pDC->MoveTo(xLine, yDown);
				pDC->LineTo(xLine, rcBounds.bottom);
				break;
			}
		}			
		pSelItem = pParent;//next
	}

	//draw plus/minus sign
	if(bChild)
	{
		CRectangle rect(this, pDC, iIndent, rcBounds);

		rect.DrawRectangle(this);

		CPen* pPenPlusMinus = pDC->SelectObject(&m_psPlusMinus);
		if(bCollapsed)
			rect.DrawPlus();
		else {
			rect.DrawMinus();
			//draw line up to parent folder
			CPen* pLine = pDC->SelectObject(&m_psTreeLine);
			int nOffset = (rcBounds.Height() - m_cyImage)/2;
			pDC->MoveTo(rect.GetLeft()+ m_cxImage, rcBounds.top + m_cyImage+nOffset);
			pDC->LineTo(rect.GetLeft() + m_cxImage, rcBounds.bottom);
			pDC->SelectObject(pLine);		
		}
		pDC->SelectObject(pPenPlusMinus);		
	}
	pDC->SelectObject(pPenTreeLine);
}

//walk all over the place setting the hide/show flag of the nodes.
//it also deletes items from the listviewctrl.
void CSuperGridCtrl::HideChildren(CTreeItem *pItem, BOOL bHide,int nItem)
{
	if(pItem&&!pItem->m_bHideChildren&&ItemHasChildren(pItem))
	{
		pItem->m_bHideChildren=bHide;
		POSITION pos = pItem->m_listChild.GetHeadPosition();
		while (pos != NULL)
		{
			CTreeItem *pSonItem=(CTreeItem *)pItem->m_listChild.GetNext(pos);
			if(pSonItem&&(pSonItem->m_dwPropGroup&GetBinaryWord(m_iPropGroup))==0)
				continue;	//不属于当前分组不显示 wht 10-11-29
			HideChildren(pSonItem,bHide,nItem+1);
			DeleteItem(nItem);
		}
	}
}

void CSuperGridCtrl::Collapse(CTreeItem *pItem)
{
	if(pItem != NULL && ItemHasChildren(pItem))
	{
		if(FireStatusModify&&!FireStatusModify(this,pItem,TRUE))
			return;
		SetRedraw(0);
		int nIndex = NodeToIndex(pItem);			
		HideChildren(pItem, TRUE, nIndex+1);
		InternaleUpdateTree();
		SetRedraw(1);
	}
}

void CSuperGridCtrl::ExpandAll(CTreeItem *pItem, int& nScroll)
{
	const int nChildren = pItem->m_listChild.GetCount();
	if (nChildren > 0)
	{
		int nIndex = NodeToIndex(pItem);
		nScroll = Expand(pItem, nIndex);
	}

	POSITION pos = pItem->m_listChild.GetHeadPosition();
	while (pos)
	{
		CTreeItem *pChild = (CTreeItem*)pItem->m_listChild.GetNext(pos);
		ExpandAll(pChild, nScroll);
	}
}

int CSuperGridCtrl::Expand(CTreeItem* pSelItem, int nIndex)
{
	if(ItemHasChildren(pSelItem) && pSelItem->m_bHideChildren)
	{
		LV_ITEM lvItem;
		lvItem.mask = LVIF_INDENT;
		lvItem.iItem = nIndex;
		lvItem.iSubItem = 0;
		lvItem.lParam=(LPARAM)pSelItem;
		lvItem.iIndent = GetIndent(pSelItem);
		SetItem(&lvItem);

		pSelItem->m_bHideChildren=FALSE;
		if(FireStatusModify)
			FireStatusModify(this,pSelItem,FALSE);
		//expand children
		POSITION pos = pSelItem->m_listChild.GetHeadPosition();
		while(pos != NULL)
		{
			CTreeItem* pNextNode = (CTreeItem*)pSelItem->m_listChild.GetNext(pos);
			if(pNextNode==NULL)
				continue;
			if((pNextNode->m_dwPropGroup&GetBinaryWord(m_iPropGroup))==0)
				continue;	//不属于当前分组 wht 10-11-29
			CString str = pNextNode->m_lpNodeInfo->GetSubItemText(0);
			LV_ITEM lvItem;
			lvItem.mask = LVIF_TEXT | LVIF_INDENT | LVIF_PARAM;
			lvItem.pszText =str.GetBuffer(1); 
			lvItem.iItem = nIndex + 1;
			lvItem.iSubItem = 0;
			lvItem.lParam=(LPARAM)pNextNode;
			lvItem.iIndent = GetIndent(pSelItem)+1;
			nIndex=CListCtrl::InsertItem(&lvItem);
			if(pNextNode->m_lpNodeInfo->GetCheck())
				SetCheck(nIndex + 1);
			//get subitems
			int nSize = pNextNode->m_lpNodeInfo->GetItemCount();
			for(int i=1; i< nSize;i++)
			{
			   CString str=pNextNode->m_lpNodeInfo->GetSubItemText(i);
			   lvItem.mask = LVIF_TEXT;
			   lvItem.iSubItem = i;
			   lvItem.pszText=str.GetBuffer(1);
			   SetItem(&lvItem);
			}
			//pNextNode->m_bHideSelf=FALSE;
			if(!pNextNode->m_bHideChildren&&pNextNode->m_listChild.GetCount()>0)
				nIndex = Expand(pNextNode,nIndex);
		}
	}
	InternaleUpdateTree();
	return nIndex;
}

int CSuperGridCtrl::SelectItem(CTreeItem *pLocateNode,int iSubItem/*=0*/,BOOL bDisplayCellCtrl/*=FALSE*/,
	BOOL bExclusiveOtherItemSelectState/*=FALSE*/)
{
	if(pLocateNode==NULL)
		return -1;
	if(bExclusiveOtherItemSelectState)
	{
		POSITION pos=GetFirstSelectedItemPosition();
		while(pos!=NULL)
		{
			int index=GetNextSelectedItem(pos);
			SetItemState(index, 0, LVIS_SELECTED);
			SetRedraw(1);
		}
	}
	int iOldSubItem=m_CurSubItem;
	m_CurSubItem=iSubItem;	//设置第iSubItem为活动列
	int nSelectedItem=-1;
	if(IsRoot(pLocateNode))
	{
		UINT uflag = LVIS_SELECTED | LVIS_FOCUSED;
		//SetItemState(0, uflag, uflag);
		//return 0;
		SetRedraw(0);
		nSelectedItem = NodeToIndex(pLocateNode);
		SetItemState(nSelectedItem, uflag, uflag);
		SetRedraw(1);
		EnsureVisible(nSelectedItem, TRUE);
		if(iOldSubItem!=m_CurSubItem)
		{
			CHeaderCtrl* pHeader = GetHeaderCtrl();
			// Make the column visible.
			// We have to take into account that the header may be reordered.
			MakeColumnVisible( Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem));
			// Invalidate the item.
			if( nSelectedItem != -1 )
			{
				CRect rcBounds;
				GetItemRect(nSelectedItem, rcBounds, LVIR_BOUNDS);
				InvalidateRect(&rcBounds);
				UpdateWindow();
			}
		}
		return nSelectedItem;
	}
	CTreeItem* pNode = pLocateNode;
	CTreeItem* pTopLevelParent=NULL;
	//Get top parent
	while(1)
	{
		CTreeItem *pParent = GetParentItem(pLocateNode);
		if(IsRoot(pParent))
			break;
		pLocateNode = pParent;
	}
	pTopLevelParent = pLocateNode;//on top of all
	//Expand the folder
	if(pTopLevelParent != NULL)
	{
		SetRedraw(0);
		CWaitCursor wait;
		CTreeItem *pRoot = GetParentItem(pTopLevelParent);

		if(pRoot->m_bHideChildren)
			Expand(pRoot,0);

		ExpandUntil(pTopLevelParent, pNode);

		UINT uflag = LVIS_SELECTED | LVIS_FOCUSED;
		nSelectedItem = NodeToIndex(pNode);
		SetItemState(nSelectedItem, uflag, uflag);
		SetRedraw(1);
		EnsureVisible(nSelectedItem, TRUE);
	}
	//显示当前单元格控件 wht 10-09-07
	if(bDisplayCellCtrl&&nSelectedItem>=0&&iSubItem>=0)
		DisplayCellCtrl(nSelectedItem,iSubItem);
	return nSelectedItem;
}

void CSuperGridCtrl::ExpandUntil(CTreeItem *pItem, CTreeItem* pStopAt)
{
	const int nChildren = pItem->m_listChild.GetCount();
	if (nChildren > 0)
	{
		POSITION pos = pItem->m_listChild.GetHeadPosition();
		while (pos)
		{
			CTreeItem *pChild = (CTreeItem*)pItem->m_listChild.GetNext(pos);
			if(pChild == pStopAt)	
			{
				int nSize = GetIndent(pChild);
				CTreeItem** ppParentArray = new CTreeItem*[nSize];
				int i=0;
				while(1)
				{
					CTreeItem *pParent = GetParentItem(pChild);
					
					if(IsRoot(pParent))
						break;
					pChild = pParent;
					ppParentArray[i] = pChild;
					i++;
				}

				for(int x=i; x > 0; x--)
				{
					CTreeItem *pParent = ppParentArray[x-1];
					Expand(pParent, NodeToIndex(pParent));
				}
				delete [] ppParentArray;
				return;
			}
		}
	}
	POSITION pos = pItem->m_listChild.GetHeadPosition();
	while (pos)
	{
		CTreeItem *pChild = (CTreeItem*)pItem->m_listChild.GetNext(pos);
		ExpandUntil(pChild, pStopAt);
	}
}

void CSuperGridCtrl::DeleteItemEx(CTreeItem *pSelItem, int nItem)
{
	SetRedraw(0);
	DeleteItem(nItem);//delete cur item in listview
	//delete/hide all children in pSelItem
	HideChildren(pSelItem, TRUE, nItem);
	//delete all internal nodes
	// If root, must delete from m_rootData
	if(GetParentItem(pSelItem) == NULL )
		DeleteRootItem(pSelItem);
	else
		Delete(pSelItem);

	InternaleUpdateTree();
	if(nItem-1<0)//no more items in list
	{
		SetRedraw(1); 
		InvalidateRect(NULL);
		UpdateWindow();
		return;
	}

	UINT uflag = LVIS_SELECTED | LVIS_FOCUSED;
	CRect rcTestIfItemIsValidToSelectOtherWiseSubtrackOneFromItem;//just to get the documention right :)
	GetItemRect(nItem, rcTestIfItemIsValidToSelectOtherWiseSubtrackOneFromItem ,LVIR_LABEL) ? SetItemState(nItem, uflag, uflag) : SetItemState(nItem-1, uflag, uflag);
	
	SetRedraw(1);
	InvalidateRect(NULL);
	UpdateWindow();
}

void CSuperGridCtrl::CleanMe(CTreeItem *pItem)
{
	// delete child nodes
	POSITION pos = pItem->m_listChild.GetHeadPosition();
	while (pos != NULL)
	{
		CTreeItem* pItemData = (CTreeItem*)pItem->m_listChild.GetNext(pos);
		if(pItemData!=NULL)
		{
			if(pItemData->m_lpNodeInfo!=NULL)
				delete pItemData->m_lpNodeInfo;

			pItemData->m_listChild.RemoveAll();
			delete pItemData;
		}
	}
	pItem->m_listChild.RemoveAll();
}

CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetNext(CTreeItem* pStartAt, CTreeItem* pNode, BOOL bInit, BOOL bDontIncludeHidden)
{
	static BOOL bFound;
	if (bInit)
		bFound = FALSE;
		
	if (pNode == pStartAt)
		bFound = TRUE;

	if(bDontIncludeHidden)
	{
		if (!pStartAt->m_bHideChildren)
		{
			POSITION pos = pStartAt->m_listChild.GetHeadPosition();
			while (pos != NULL)
			{
				CTreeItem* pChild = (CTreeItem*)pStartAt->m_listChild.GetNext(pos);
				if (bFound)
					return pChild;
				pChild = GetNext(pChild, pNode, FALSE, TRUE);
				if (pChild != NULL)
					return pChild;
			}
		}
	}
	else {
			POSITION pos = pStartAt->m_listChild.GetHeadPosition();
			while (pos != NULL)
			{
				CTreeItem* pChild = (CTreeItem*)pStartAt->m_listChild.GetNext(pos);
				if (bFound)
					return pChild;
				pChild = GetNext(pChild, pNode, FALSE,FALSE);
				if (pChild != NULL)
					return pChild;
			}
	}
	// if reached top and last level return original
	if (bInit)
		return pNode;
	else
		return NULL;
}

CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetPrev(CTreeItem* pStartAt, CTreeItem* pNode, BOOL bInit, BOOL bDontIncludeHidden)
{
	static CTreeItem* pPrev;
	if (bInit)
		pPrev = pStartAt;

	if (pNode == pStartAt)
		return pPrev;

	pPrev = pStartAt;

	if(bDontIncludeHidden)
	{
		if (!pStartAt->m_bHideChildren)
		{
			POSITION pos = pStartAt->m_listChild.GetHeadPosition();
			while (pos != NULL)
			{
				CTreeItem* pCur = (CTreeItem*)pStartAt->m_listChild.GetNext(pos);
				CTreeItem* pChild = GetPrev(pCur,pNode, FALSE,TRUE);
				if (pChild != NULL)
					return pChild;
			}
		}
	}
	else {
		POSITION pos = pStartAt->m_listChild.GetHeadPosition();
		while (pos != NULL)
		{
			CTreeItem* pCur = (CTreeItem*)pStartAt->m_listChild.GetNext(pos);
			CTreeItem* pChild = GetPrev(pCur,pNode, FALSE,FALSE);
			if (pChild != NULL)
				return pChild;
		}
	}

	if (bInit)
		return pPrev;
	else
		return NULL;
}

int CSuperGridCtrl::_NodeToIndex(CTreeItem *pStartpos, CTreeItem* pNode, int& nIndex, BOOL binit)
{
	static BOOL bFound;	
	// Account for other root nodes
	if(GetParentItem(pStartpos) == NULL && GetRootIndex(pStartpos) !=0)
		nIndex++;

	if(binit)
		bFound=FALSE;

	if(pStartpos==pNode)
		bFound=TRUE;

	if(!pStartpos->m_bHideChildren)
	{
		POSITION pos = GetHeadPosition(pStartpos);
		while (pos)
		{
			CTreeItem *pChild = GetNextChild(pStartpos, pos);
			if(bFound)
				return nIndex;

//	Craig Schmidt: Cannot set nIndex as return value.  Worked find with single root but
//				   the calling function get confused since the return value may indicate
//				   that the next root needs to be searched.  Didn'd spend much time on
//				   this so there is probably a better way of doing this.
//			nIndex = _NodeToIndex(pChild, pNode, nIndex, binit);
			if(pChild&&(pChild->m_dwPropGroup&GetBinaryWord(m_iPropGroup))==0)
				continue;	//不属于当前分组 wht 10-12-02
			_NodeToIndex(pChild, pNode, nIndex, binit);
			nIndex++;
		}
	}
	if(binit && bFound)
		return nIndex;
	else
		return -1;
}

BOOL CSuperGridCtrl::Delete(CTreeItem* pNode, BOOL bClean)
{
	POSITION pos = m_RootItems.GetHeadPosition();
	while(pos!=NULL)
	{
		CTreeItem * pRoot = (CTreeItem*)m_RootItems.GetNext(pos);
		if(_Delete(pRoot, pNode, bClean))
			return TRUE;
	}
	return FALSE;
}

BOOL CSuperGridCtrl::_Delete(CTreeItem* pStartAt, CTreeItem* pNode, BOOL bClean)
{
	POSITION pos = pStartAt->m_listChild.GetHeadPosition();
	while (pos != NULL)
	{
		POSITION posPrev = pos;
		CTreeItem *pChild = (CTreeItem*)pStartAt->m_listChild.GetNext(pos);
		if (pChild == pNode)
		{
			pStartAt->m_listChild.RemoveAt(posPrev);
			if(bClean)
			{
				if(pNode->m_lpNodeInfo!=NULL)
					delete pNode->m_lpNodeInfo;
				delete pNode;
			}
			return TRUE;
		}
		if (_Delete(pChild, pNode) == TRUE)
			return TRUE;
	}
	return FALSE;
}

BOOL CSuperGridCtrl::IsChildOf(const CTreeItem* pParent, const CTreeItem* pChild) const
{
	if (pChild == pParent)
		return TRUE;
	POSITION pos = pParent->m_listChild.GetHeadPosition();
	while (pos != NULL)
	{
		CTreeItem* pNode = (CTreeItem*)pParent->m_listChild.GetNext(pos);
		if (IsChildOf(pNode, pChild))
			return TRUE;
	}
	return FALSE;
}

void CSuperGridCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( GetFocus() != this) 
		SetFocus();
	LVHITTESTINFO ht;
	ht.pt = point;
	SubItemHitTest(&ht);
	BOOL bExecuteLBtnDown=FALSE;
	if(OnItemLButtonDown(ht))
	{
		BOOL bSelect=1;
		bSelect = HitTestOnSign(point, ht);
		if(bSelect && ht.iItem!=-1)
		{
			m_CurSubItem = IndexToOrder(ht.iSubItem);
			CHeaderCtrl* pHeader = GetHeaderCtrl();
			// Make the column fully visible.
			MakeColumnVisible(Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem));
			CListCtrl::OnLButtonDown(nFlags, point);
			bExecuteLBtnDown=TRUE;
			OnControlLButtonDown(nFlags, point, ht);
			//update row anyway for selection bar
			CRect rc;
			GetItemRect(ht.iItem, rc, LVIR_BOUNDS);
			InvalidateRect(rc);
			UpdateWindow();
		}
	}
	if(!bExecuteLBtnDown)
		CListCtrl::OnLButtonDown(nFlags, point);
}

void CSuperGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if( GetFocus() != this) 
		SetFocus();
	
	LVHITTESTINFO ht;
	ht.pt = point;
	SubItemHitTest(&ht);
	BOOL bExecuteLBtnDblClk=FALSE;
	if(OnItemLButtonDown(ht))
	{
		BOOL bSelect=1;
		bSelect = HitTestOnSign(point, ht);
		if(bSelect && ht.iItem!=-1)
		{
			m_CurSubItem = IndexToOrder(ht.iSubItem);
			CHeaderCtrl* pHeader = GetHeaderCtrl();
			// Make the column fully visible.
			MakeColumnVisible(Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem));
			CListCtrl::OnLButtonDblClk(nFlags, point);
			bExecuteLBtnDblClk=TRUE;
			//OnControlLButtonDown(nFlags, point, ht);
			//update row anyway for selection bar
			CRect rc;
			GetItemRect(ht.iItem, rc, LVIR_BOUNDS);
			InvalidateRect(rc);
			UpdateWindow();
		}
	}
	if(!bExecuteLBtnDblClk)
		CListCtrl::OnNcLButtonDblClk(nFlags,point);
}

void CSuperGridCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( GetFocus() != this) 
		SetFocus();	
	LVHITTESTINFO ht;
	ht.pt = point;
	SubItemHitTest(&ht);
	CTreeItem *pCurItem=NULL;
	BOOL bSelect = HitTestOnSign(point, ht);
	if(bSelect && ht.iItem!=-1)
	{
		pCurItem=GetTreeItem(ht.iItem);
		m_CurSubItem = IndexToOrder(ht.iSubItem);
		CHeaderCtrl* pHeader = GetHeaderCtrl();
		// Make the column fully visible.
		MakeColumnVisible(Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem));
		CListCtrl::OnRButtonDown(nFlags, point);
		//update row anyway for selection bar
		CRect rc;
		GetItemRect(ht.iItem, rc, LVIR_BOUNDS);
		InvalidateRect(rc);
		UpdateWindow();
	}
	if(FireContextMenu)
		FireContextMenu(this,pCurItem,point);
}

void CSuperGridCtrl::OnUpdateListViewItem(CTreeItem* lpItem, LV_ITEM *plvItem)
{
	//default implementation you would go for this 9 out of 10 times
	if(lpItem->m_lpNodeInfo!=NULL)
		lpItem->m_lpNodeInfo->SetSubItemText(plvItem->iSubItem, plvItem->pszText);
	SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
}

BOOL CSuperGridCtrl::DeleteAllItems()
{
	HideInputCtrl();
	POSITION pos = m_RootItems.GetHeadPosition();
	while(pos!=NULL)
	{
		CTreeItem * pRoot = (CTreeItem*)m_RootItems.GetNext(pos);
		if(pRoot==NULL)
			continue;
		//DeleteItemEx(pRoot, 0);
		//删除所有项时使用 DeleteItemEx(pRoot, 0);效率较低，修改为如下方式 wht 10-09-01
		CleanMe(pRoot);	//清空子项
		if(pRoot->m_lpNodeInfo)
			delete pRoot->m_lpNodeInfo;
		delete pRoot;
	}
	m_RootItems.RemoveAll();
	if(m_bEnableSort&&m_ctrlHeader.GetSafeHwnd()!=NULL)
		m_ctrlHeader.SetSortArrow(-1,TRUE);
	return CListCtrl::DeleteAllItems();
}

BOOL CSuperGridCtrl::DeleteAllSonItems(CTreeItem *pSelItem)
{
	SetRedraw(0);
	int nIndex = NodeToIndex(pSelItem);			
	//delete/hide all children in pSelItem
	HideChildren(pSelItem, TRUE, nIndex+1);
	//delete all internal nodes
	POSITION pos = pSelItem->m_listChild.GetHeadPosition();
	while (pos != NULL)
	{
		CTreeItem* pItem = (CTreeItem*)pSelItem->m_listChild.GetNext(pos);
		if(pItem!=NULL)
			Delete(pItem);
	}
	InternaleUpdateTree();
	SetRedraw(1);
	InvalidateRect(NULL);
	UpdateWindow();
	return TRUE;
}
void CSuperGridCtrl::RefreshItem(CTreeItem *pSelItem)
{
	int nSize = pSelItem->m_lpNodeInfo->GetItemCount();
	for(int i=0;i< nSize;i++)
	{
		CString str=pSelItem->m_lpNodeInfo->GetSubItemText(i);
		SetSubItemText(pSelItem,i,str);
	}
	InternaleUpdateTree();
	SetRedraw(1);
	InvalidateRect(NULL);
	UpdateWindow();
}
POSITION CSuperGridCtrl::GetRootHeadPosition() const
{
	return m_RootItems.GetHeadPosition();
}

POSITION CSuperGridCtrl::GetRootTailPosition() const
{
	return m_RootItems.GetTailPosition();
}

CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetNextRoot(POSITION& pos) const
{
	return (CTreeItem*)m_RootItems.GetNext(pos);
}

CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetPrevRoot(POSITION& pos) const
{
	return (CTreeItem*)m_RootItems.GetPrev(pos);
}

POSITION CSuperGridCtrl::GetHeadPosition(CTreeItem* pItem) const
{
	return pItem->m_listChild.GetHeadPosition();
}

CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetNextChild(CTreeItem *pItem, POSITION& pos) const
{
	return (CTreeItem*)pItem->m_listChild.GetNext(pos);
}

CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetPrevChild(CTreeItem *pItem, POSITION& pos) const
{
	return (CTreeItem*)pItem->m_listChild.GetPrev(pos);
}

POSITION CSuperGridCtrl::GetTailPosition(CTreeItem *pItem) const
{
	return pItem->m_listChild.GetTailPosition();
}



void CSuperGridCtrl::AddTail(CTreeItem *pParent, CTreeItem *pChild)
{
	pParent->m_listChild.AddTail(pChild);
}


inline int StrComp(const CString* pElement1, const CString* pElement2)
{
	return pElement1->Compare(*pElement2);
}

int CSuperGridCtrl::CompareChildren(const void* p1, const void* p2)
{
	CTreeItem * pChild1 = *(CTreeItem**)p1;
	CTreeItem * pChild2 = *((CTreeItem**)p2);
	CListCtrlItemInfo *pItem1=pChild1->m_lpNodeInfo;
	CListCtrlItemInfo *pItem2=pChild2->m_lpNodeInfo;
	return StrComp(&(pItem1->GetSubItemText(0)), &(pItem2->GetSubItemText(0)));
}

void CSuperGridCtrl::Sort(CTreeItem* pParent, BOOL bSortChildren)
{
	const int nChildren = NumChildren(pParent);
	if (nChildren > 1)
	{
		CTreeItem** ppSortArray = new CTreeItem*[nChildren];
		// Fill in array with pointers to our children.
		POSITION pos = pParent->m_listChild.GetHeadPosition();
		int i;
		for (i=0; pos; i++)
		{
			ASSERT(i < nChildren);
			ppSortArray[i] = (CTreeItem*)pParent->m_listChild.GetAt(pos);
			pParent->m_listChild.GetNext(pos);
		}

		qsort(ppSortArray, nChildren, sizeof(CTreeItem*), CompareChildren);
		// reorg children with new sorted list
		pos = pParent->m_listChild.GetHeadPosition();
		for (i=0; pos; i++)
		{
			ASSERT(i < nChildren);
			pParent->m_listChild.SetAt(pos, ppSortArray[i]);
			pParent->m_listChild.GetNext(pos);
		}

		delete [] ppSortArray;
	}

	if(bSortChildren)
	{
		POSITION pos = pParent->m_listChild.GetHeadPosition();
		while (pos)
		{
			CTreeItem *pChild = (CTreeItem*)pParent->m_listChild.GetNext(pos);
			Sort(pChild, TRUE);
		}
	}
}

int CSuperGridCtrl::NumChildren(const CTreeItem *pItem) const
{
	return pItem->m_listChild.GetCount();
}

BOOL CSuperGridCtrl::ItemHasChildren(const CTreeItem* pItem) const
{
	BOOL bChildren = pItem->m_listChild.GetCount() != 0;
	//see if we have a flag
	int nFlag = pItem->m_bSetChildFlag;
	if(nFlag!=-1)
		return 1;
	else
		return bChildren;
}

void CSuperGridCtrl::SetChildrenFlag(CTreeItem *pItem, int nFlag) const
{
	pItem->m_bSetChildFlag = nFlag;
}

int CSuperGridCtrl::GetIndent(const CTreeItem* pItem) const
{
	return pItem->m_nIndent;
}

void CSuperGridCtrl::SetIndent(CTreeItem *pItem, int iIndent)
{
	pItem->m_nIndent = iIndent;
}

int CSuperGridCtrl::GetCurIndex(const CTreeItem *pItem) const
{
	return pItem->m_nIndex;
}

void CSuperGridCtrl::SetCurIndex(CTreeItem* pItem, int nIndex) 
{
	pItem->m_nIndex = nIndex;
}

void CSuperGridCtrl::SetParentItem(CTreeItem*pItem, CTreeItem* pParent)
{
	pItem->m_pParent=pParent;
}

CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetParentItem(const CTreeItem* pItem) 
{
	if(pItem==NULL)
		return NULL;
	return pItem->m_pParent;
};

//overrides
CListCtrlItemInfo* CSuperGridCtrl::CopyData(CListCtrlItemInfo* lpSrc)
{
	ASSERT(FALSE);//debug
	return NULL;  //release
}

//default implementation for setting icons
int CSuperGridCtrl::GetIcon(const CTreeItem* pItem)
{
	CListCtrlItemInfo *pItemInfo=pItem->m_lpNodeInfo;
	if(pItemInfo)
		return pItemInfo->GetImage();
	else
		return 0;//just take the first item in CImageList ..what ever that is
}

void CSuperGridCtrl::OnControlLButtonDown(UINT nFlags, CPoint point, LVHITTESTINFO& ht)
{
	CTreeItem *pItem=GetTreeItem(ht.iItem);
	if(pItem==NULL)
	{
		m_curEditItem.iItem=-1;
		return;
	}
	//记录当前编辑的单元格所在行、列
	m_curEditItem.iItem=ht.iItem;
	m_curEditItem.iSubItem=ht.iSubItem;
	//显示帮助字符串 wht 11-04-16
	if(m_hPromptWnd!=NULL&&pItem&&ht.iSubItem>=0&&ht.iSubItem<m_arrHeader.GetSize())
	{
		CString strHelp=pItem->m_lpNodeInfo->GetSubItemHelpText(ht.iSubItem);
		if(strHelp.GetLength()==0&&m_arrHeader[ht.iSubItem].helpStr.GetLength()>0)
			strHelp=(char*)m_arrHeader[ht.iSubItem].helpStr;
		::SetWindowText(m_hPromptWnd,strHelp);
		::UpdateWindow(m_hPromptWnd);
	}
	//if(!m_bDblclkDisplayCellCtrl&&m_bHasInputCtrlFocus)	//未设置双击显示可编辑控件时单击显示 wht 11-04-14
	if(!m_bDblclkDisplayCellCtrl)	//未设置双击显示可编辑控件时单击显示 wht 11-04-14
		DisplayCellCtrl(ht.iItem,ht.iSubItem);
	if(FireLButtonDown)
		FireLButtonDown(this,pItem,ht.iSubItem);
}

COLORREF CSuperGridCtrl::GetCellRGB()
{
	int iItem=GetSelectedItem();
	if(iItem>=0)
	{
		if(GetCellReadOnly(iItem,m_CurSubItem))
		{
			if(m_bImmerseMode)
				return RGB(210,210,210);
			else 
				return RGB(200,200,200);	//RGB(192,0,0);
		}
		else
		{
			if(m_bImmerseMode)
				return RGB(192,235,198);	//底色，护眼绿
			else
				return RGB(0,192,0);
		}
	}
	return RGB(0,192,0);
}
	
BOOL CSuperGridCtrl::OnVKMultiply(CTreeItem *pItem, int nIndex)
{
	return 1;
}

BOOL CSuperGridCtrl::OnVkSubTract(CTreeItem *pItem, int nIndex)
{
	return 1;
}

BOOL CSuperGridCtrl::OnVKAdd(CTreeItem *pItem, int nIndex)
{
	return 1;
}

BOOL CSuperGridCtrl::OnDeleteItem(CTreeItem* pItem, int nIndex)
{
	return 1;
}

BOOL CSuperGridCtrl::OnItemExpanding(CTreeItem *pItem, int iItem)
{
	return 1;
}

BOOL CSuperGridCtrl::OnItemExpanded(CTreeItem* pItem, int iItem)
{
	return 1;
}

BOOL CSuperGridCtrl::OnCollapsing(CTreeItem *pItem)
{
	return 1;
}

BOOL CSuperGridCtrl::OnItemCollapsed(CTreeItem *pItem)
{
	return 1;
}

BOOL CSuperGridCtrl::OnItemLButtonDown(LVHITTESTINFO& ht)
{
	return 1;
}

BOOL CSuperGridCtrl::OnVkReturn(void)
{
	return 0;
}

void CSuperGridCtrl::OnSysColorChange() 
{
	CListCtrl::OnSysColorChange();
	//nop nothing yet
}

UINT CSuperGridCtrl::_GetCount(CTreeItem* pItem, UINT& nCount)
{
	POSITION pos = pItem->m_listChild.GetHeadPosition();
	while (pos)
	{
		CTreeItem *pChild = (CTreeItem*)pItem->m_listChild.GetNext(pos);
		nCount = _GetCount(pChild, nCount);
		nCount++;				
	}
	return nCount;
}

UINT CSuperGridCtrl::GetCount(void) 
{
	UINT nCount=0;
	UINT _nCount=0;
	POSITION pos = m_RootItems.GetHeadPosition();
	while(pos!=NULL)
	{
		CTreeItem * pRoot = (CTreeItem*)m_RootItems.GetNext(pos);
		nCount += _GetCount(pRoot, _nCount) + 1;
	}
	return nCount;
}

CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetTreeItem(int nIndex /*nIndex must be valid of course*/ ) 
{
	if(nIndex<0||nIndex>=GetItemCount())
		return NULL;
	else
		return reinterpret_cast<CTreeItem*>(GetItemData(nIndex));
}

CSuperGridCtrl::CTreeItem* CSuperGridCtrl::FindTreeItemById(int idProp,CTreeItem *pParent/*=NULL*/)
{
	if(pParent!=NULL&&pParent->m_idProp==idProp)
		return pParent;
	POSITION pos;
	CTreeItem* pItem;
	if(pParent!=NULL)
		pos=pParent->m_listChild.GetHeadPosition();
	else
		pos=m_RootItems.GetHeadPosition();
	while(pos!=NULL)
	{
		if(pParent!=NULL)
			pItem=(CTreeItem*)pParent->m_listChild.GetNext(pos);
		else
			pItem=(CTreeItem*)m_RootItems.GetNext(pos);
		pItem=FindTreeItemById(idProp,pItem);
		if(pItem!=NULL)
			return pItem;
	}
	return NULL;
}

CSuperGridCtrl::CTreeItem* CSuperGridCtrl::InternalFindItemByPropId(long prop_id,CTreeItem *pItem, CTreeItem **ppParentItem)
{
	POSITION pos = pItem->m_listChild.GetHeadPosition();
	while(pos)
	{
		CTreeItem *pFindItem=(CTreeItem*)pItem->m_listChild.GetNext(pos);
		if(pFindItem->m_idProp==prop_id)
		{
			if(ppParentItem)
				*ppParentItem=pItem;
			return pFindItem;
		}
		else
			pFindItem=InternalFindItemByPropId(prop_id,pFindItem,ppParentItem);
		if(pFindItem)
			return pFindItem;
	}
	return NULL;
}

CSuperGridCtrl::CTreeItem* CSuperGridCtrl::FindItemByPropId(long prop_id,CTreeItem **ppParentItem)
{
	POSITION pos=m_RootItems.GetHeadPosition();
	while(pos)
	{
		CTreeItem *pItem=(CTreeItem*)m_RootItems.GetNext(pos);
		if(pItem->m_idProp==prop_id)
		{
			if(ppParentItem)
				*ppParentItem=NULL;
			return pItem;
		}
		else
			pItem=InternalFindItemByPropId(prop_id,pItem,ppParentItem);
		if(pItem)
			return pItem;
	}
	return NULL;
}

int CSuperGridCtrl::GetSelectedItem(void) const
{
	return GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); 
}
void CSuperGridCtrl::SetSubItemText(CTreeItem *pItem,int iSubItem,LPCTSTR lpszText)
{
	SetItemText(pItem->GetIndex(),iSubItem,lpszText);
	pItem->m_lpNodeInfo->SetSubItemText(iSubItem,lpszText);
}

BOOL CSuperGridCtrl::GetCellReadOnly(int iRow,int iSubItem)
{
	if(iRow<0)
		return FALSE;
	CSuperGridCtrl::CTreeItem *pTreeItem=GetTreeItem(iRow);
	if(pTreeItem==NULL)
		return FALSE;
	CListCtrlItemInfo *pItemInfo=pTreeItem->m_lpNodeInfo;
	if(pItemInfo==NULL)
		return FALSE;
	return pItemInfo->GetSubItemReadOnly(iSubItem);
}

BOOL CSuperGridCtrl::SetCellReadOnly(int iRow,int iSubItem,BOOL bReadOnly)
{
	if(iRow<0)
		return FALSE;
	CTreeItem *pTreeItem=GetTreeItem(iRow);
	if(pTreeItem==NULL)
		return FALSE;
	CListCtrlItemInfo *pItemInfo=pTreeItem->m_lpNodeInfo;
	if(pItemInfo==NULL)
		return FALSE;
	pItemInfo->SetSubItemReadOnly(iSubItem,bReadOnly);
	return TRUE;
}

//
void CSuperGridCtrl::InternalInsertItem(CTreeItem *pParent, CTreeItem *pItem,BOOL bUpdate,int index/*=-1*/)
{
	int nIndex = 0;
	if(pParent)
	{
		SetParentItem(pItem, pParent);	//应放在NodeToIndex()之前
		SetIndent(pItem, GetIndent(pParent)+1);
		if(index==-1)
			nIndex=NodeToIndex(pItem);
		else 
			nIndex=index;
	}
	else
	{
		SetParentItem(pItem,NULL);
		SetIndent(pItem, 1);
		if(index==-1)
			nIndex=GetItemCount();
		else 
			nIndex=index;
	}
	SetCurIndex(pItem,nIndex);
	//
	if(bUpdate)
	{
		CString str = pItem->m_lpNodeInfo->GetSubItemText(0);
		LV_ITEM lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_INDENT | LVIF_PARAM;
		lvItem.pszText =str.GetBuffer(1); 
		lvItem.iItem = nIndex;
		lvItem.iSubItem = 0;
		lvItem.lParam=(LPARAM)pItem;
		lvItem.iIndent = GetIndent(pItem);
		lvItem.iImage = pItem->m_lpNodeInfo->GetImage(); //更新图像
		CListCtrl::InsertItem(&lvItem);
		if(pItem->m_lpNodeInfo->GetCheck())
			SetCheck(nIndex);
		//get subitems
		int nSize = pItem->m_lpNodeInfo->GetItemCount();
		for(int i=1; i< nSize;i++)
		{
			CString str=pItem->m_lpNodeInfo->GetSubItemText(i);
			lvItem.mask = LVIF_TEXT;
			lvItem.iSubItem = i;
			lvItem.pszText=str.GetBuffer(1);
			SetItem(&lvItem);
		}
		//由于在RedrawItems过程中调用时导致效率低下，已移至各宿主函数调用中 wjh-2015.9.30
		//InternaleUpdateTree();//better do this
	}
}

void CSuperGridCtrl::InternalAppendItem(CTreeItem *pParent, CTreeItem *pItem)
{
	InternalInsertItem(pParent,pItem,TRUE);
	//
	if(ItemHasChildren(pItem) && !pItem->m_bHideChildren)
	{
		POSITION pos = GetHeadPosition(pItem);
		while(pos!=NULL)
		{
			CTreeItem *pSonItem = (CTreeItem*)GetNextChild(pItem,pos);
			if(pSonItem&&(pSonItem->m_dwPropGroup&GetBinaryWord(m_iPropGroup))==0)
				continue;	//不属于当前分组 wht 10-11-29
			if(pSonItem)
				InternalAppendItem(pItem,pSonItem);
		}
	}
}

void CSuperGridCtrl::Redraw()
{
	SetRedraw(FALSE);
	HideInputCtrl();
	while(GetItemCount()>0)
		DeleteItem(0);
	POSITION pos = GetRootHeadPosition();
	while(pos!=NULL)
	{
		CTreeItem *pRoot = (CTreeItem*)GetNextRoot(pos);
		if(pRoot==NULL)
			continue;
		if((pRoot->m_dwPropGroup&GetBinaryWord(m_iPropGroup))==0)
			continue;	//不属于当前分组不显示 wht 10-11-29
		InternalInsertItem(NULL,pRoot,TRUE);
		if(!pRoot->m_bHideChildren)
		{
			POSITION pos = GetHeadPosition(pRoot);
			while(pos!=NULL)
			{
				CTreeItem *pItem = (CTreeItem*)GetNextChild(pRoot,pos);
				if(pItem)
				{
					if((pItem->m_dwPropGroup&GetBinaryWord(m_iPropGroup))==0)
						continue;	//不属于当前分组不显示 wht 10-11-29
					InternalAppendItem(pRoot,pItem);
				}
			}
		}
	}
	InternaleUpdateTree();
	SetRedraw(TRUE);
}

//显示指定单元格控件	
void CSuperGridCtrl::DisplayCellCtrl(int iItem,int iCol,const char* cell_text/*=NULL*/)
{
	if (m_btnCtrl)
	{
		//HideEditBox();
		m_btnCtrl.ShowWindow(SW_HIDE);
	}
	CTreeItem *pItem=GetTreeItem(iItem);
	if(pItem==NULL)
	{
		m_curEditItem.iItem=-1;
		return;
	}
	//记录当前编辑的单元格所在行、列
	m_curEditItem.iItem=iItem;
	m_curEditItem.iSubItem=iCol;
	//显示帮助字符串 wht 10-09-13
	if(m_hPromptWnd!=NULL&&pItem&&iCol>=0&&iCol<m_arrHeader.GetSize())
	{
		CString strHelp=pItem->m_lpNodeInfo->GetSubItemHelpText(iCol);
		if(strHelp.GetLength()==0&&m_arrHeader[iCol].helpStr.GetLength()>0)
			strHelp=(char*)m_arrHeader[iCol].helpStr;
		::SetWindowText(m_hPromptWnd,strHelp);
		::UpdateWindow(m_hPromptWnd);
	}
	int nCtrlType=GetControlType(iItem,iCol);	//获取当前单元格控件类型
	if(GetCellReadOnly(iItem,iCol)&&nCtrlType!=GCT_GRAYBUTTON)
		return;

	CRect rect;
	if(iCol==0)
	{
		GetItemRect(iItem, rect, LVIR_LABEL);
		if(m_himl==NULL)
			rect.left+=GetIndent(pItem)*m_cxImage;
	}
	else
	{
		int offset = 0;
		if(!EnsureVisible(iItem, TRUE)) 
			return;
		GetSubItemRect(iItem, iCol, LVIR_BOUNDS, rect);
		// Now scroll if we need to expose the column
		CRect rcClient;
		GetClientRect(rcClient);
		if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
		{
			CSize size(offset + rect.left,0);		
			Scroll(size);
			rect.left -= size.cx;
		}
		rect.left += offset;	
		rect.right = rect.left + GetColumnWidth(iCol);
		if(rect.right > rcClient.right) 
			rect.right = rcClient.right;
	}
	//
	CString lBoxSelText;
	char sTempText[100]="";
	GetItemText(iItem,iCol,sTempText,100);
	lBoxSelText.Format("%s",sTempText);
	if (nCtrlType==GCT_STATIC)
	{
	}
	else if (nCtrlType==GCT_CMB_EDIT||nCtrlType==GCT_CMB_LIST
		||nCtrlType==GCT_CMB_COLOR||nCtrlType==GCT_ITEMSET)
	{
		//display the combo box.  If the combo box has already been
		//created then simply move it to the new location, else create it
		rect.top-=2;
		rect.bottom-=1;
		CColorSelectComboBox *pCmbBox=NULL;
		if(nCtrlType==GCT_CMB_COLOR)
			pCmbBox=&m_cmbBox3;
		else if(nCtrlType==GCT_CMB_EDIT)
			pCmbBox=&m_cmbBox1;
		else
			pCmbBox=&m_cmbBox2;
		if (pCmbBox->GetSafeHwnd())
			pCmbBox->MoveWindow(rect);
		else
		{	
			rect.bottom += 300;
			if(nCtrlType==GCT_CMB_COLOR)
				pCmbBox->Create(CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_DROPDOWNLIST | CBS_NOINTEGRALHEIGHT | WS_VISIBLE | WS_CHILD | WS_VSCROLL,//| CBS_OWNERDRAWVARIABLE | CBS_HASSTRINGS | WS_BORDER,
								rect,this,IDC_CELLCMBBOX);
			else if(nCtrlType==GCT_CMB_EDIT)
				pCmbBox->Create(CBS_DROPDOWN | CBS_NOINTEGRALHEIGHT | WS_VISIBLE | WS_CHILD | WS_VSCROLL,//| CBS_OWNERDRAWVARIABLE | CBS_HASSTRINGS | WS_BORDER,
								rect,this,IDC_CELLCMBBOX);
			else 
				pCmbBox->Create(CBS_DROPDOWNLIST | CBS_NOINTEGRALHEIGHT | WS_VISIBLE | WS_CHILD | WS_VSCROLL,//| CBS_OWNERDRAWVARIABLE | CBS_HASSTRINGS | WS_BORDER,
								rect,this,IDC_CELLCMBBOX);
			pCmbBox->SetFont(&m_SSerif8Font); 
		}

		//add the choices for this particular property
		COLORREF itemColor=RGB(255,0,0);
		char temp_clr[100]="";
		if(nCtrlType==GCT_CMB_COLOR)
		{
			pCmbBox->InitBox(RGB(0,0,0));
			sprintf(temp_clr,"%s",sTempText);
			memmove(temp_clr,temp_clr+3,97);
			sscanf(temp_clr,"%X",&itemColor);
		}
		else
		{
			CString itemsStr;
			pItem->m_lpNodeInfo->GetListItemsStr(iCol,itemsStr);
			while(pCmbBox->GetCount()>0)
				pCmbBox->DeleteString(0);
			//add the items from CStringlist
			if(itemsStr.GetLength()>0)
			{
				int i=0,i2=0;
				while ((i2=itemsStr.Find(m_cSplit,i)) != -1)
				{
					pCmbBox->AddString(itemsStr.Mid(i,i2-i));
					i=i2+1;
				}
				if(i<itemsStr.GetLength())
					pCmbBox->AddString(itemsStr.Right(itemsStr.GetLength()-i));
			}
		}
		pCmbBox->ShowWindow(SW_SHOW);

		if(nCtrlType==GCT_CMB_COLOR)
		{	//jump to the property's current value in the combo box
			int iCur = pCmbBox->IsHaveThisColor(itemColor);
			if(iCur==-1)
			{
				int iItem=pCmbBox->AddColor(itemColor,"自定义");
				pCmbBox->SetCurSel(iItem);
			}
			else
				pCmbBox->SetCurSel(iCur);
		}		
		else
		{
			//jump to the property's current value in the combo box
			int j = pCmbBox->FindStringExact(0,lBoxSelText);
			if (j != CB_ERR)
				pCmbBox->SetCurSel(j);
			else
				pCmbBox->SetCurSel(0);
			if(nCtrlType=GCT_CMB_EDIT)
				pCmbBox->SetWindowText(lBoxSelText);
		}
		pCmbBox->SetFocus();
	}
	else if (nCtrlType==GCT_EDIT)
	{
		//display edit box
		rect.top-=2;
		rect.bottom+=1;
		if (m_editBox)
			m_editBox.MoveWindow(rect);
		else
		{	
			m_editBox.Create(WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | WS_VISIBLE | WS_CHILD,rect,this,IDC_CELLEDITBOX);
			m_editBox.SetFont(&m_SSerif8Font);
		}
		//set the text in the edit box to the property's current value
		if(cell_text!=NULL)
			m_editBox.SetWindowText(cell_text);
		else
			m_editBox.SetWindowText(lBoxSelText);
		m_editBox.ShowWindow(SW_SHOW);
		m_editBox.SetFocus();
		int len = m_editBox.GetWindowTextLength();
		if(cell_text!=NULL)
			m_editBox.SetSel(len, len);
		else
			m_editBox.SetSel(0, len);
		InvalidateRect(rect);
		if(FireEditBoxKeyUpItem)
			m_editBox.SetKeyUpItemFunc(FireEditBoxKeyUpItem);
		//关闭输入法
		if(m_bCloseEditBoxImm&&m_editBox.GetSafeHwnd())
		{
			HIMC hImc = ImmGetContext(m_editBox.GetSafeHwnd());
			if(hImc)
			{
				ImmAssociateContext(m_editBox.GetSafeHwnd(),NULL);
				ImmReleaseContext(m_editBox.GetSafeHwnd(),hImc);
			}
		}
	}
	else if(nCtrlType==GCT_BTN_COMMON||nCtrlType==GCT_BTN_EDIT||nCtrlType==GCT_BTN_FILEPATH)
	{
		rect.top-=2;
		rect.bottom+=1;
		CRect region;
		region.left = rect.left;
		region.right = rect.right - 26;
		region.bottom = rect.bottom;
		region.top = rect.top;
		//displays a button if the property is a file/color/font chooser
		if (rect.Width() > 25) 
			rect.left = rect.right - 25;
		if (m_btnCtrl)
			m_btnCtrl.MoveWindow(rect);
		else
		{
			m_btnCtrl.Create("...",BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD,
				rect,this,IDC_CELLBTNCTRL);
			m_btnCtrl.SetFont(&m_SSerif8Font);
		}
		m_btnCtrl.ShowWindow(SW_SHOW);
		
		if( nCtrlType==GCT_BTN_FILEPATH ||	//如果按钮类型为文件路径则显示编辑框
			nCtrlType==GCT_BTN_EDIT)	//按钮类型为可直接编辑类型
		{
			if (m_editBox)
				m_editBox.MoveWindow(region);
			else
			{	
				m_editBox.Create(WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | WS_VISIBLE | WS_CHILD,
					region,this,IDC_CELLEDITBOX);
				m_editBox.SetFont(&m_SSerif8Font);
			}	
			//set the text in the edit box to the property's current value
			m_editBox.SetWindowText(lBoxSelText);
			m_editBox.ShowWindow(SW_SHOW);
			m_editBox.SetFocus();
			int len = m_editBox.GetWindowTextLength();
			m_editBox.SetSel(0, len);
		}
	}
	else if(nCtrlType==GCT_GRAYBUTTON)
	{
		rect.top-=2;
		rect.bottom+=1;
		CRect region;
		region.left = rect.left;
		region.right = rect.right;
		region.bottom = rect.bottom;
		region.top = rect.top;
		if (m_btnCtrl)
			m_btnCtrl.MoveWindow(rect);
		else
		{
			m_btnCtrl.Create("...",BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD,
				rect,this,IDC_CELLBTNCTRL);
			m_btnCtrl.SetFont(&m_SSerif8Font);
		}
		m_btnCtrl.ShowWindow(SW_SHOW);
		m_btnCtrl.SetWindowText(pItem->m_lpNodeInfo->GetSubItemText(iCol));
	}
	m_bHasInputCtrlFocus=TRUE;
}

BOOL CSuperGridCtrl::IsDisplayCellCtrl(int iItem,int iCol)
{
	if(!m_bHasInputCtrlFocus)
		return false;
	int nCtrlType=GetControlType(iItem,iCol);	//获取当前单元格控件类型
	if (nCtrlType==GCT_CMB_EDIT||nCtrlType==GCT_CMB_LIST
		||nCtrlType==GCT_CMB_COLOR||nCtrlType==GCT_ITEMSET)
	{
		CColorSelectComboBox *pCmbBox=NULL;
		if(nCtrlType==GCT_CMB_COLOR)
			pCmbBox=&m_cmbBox3;
		else if(nCtrlType==GCT_CMB_EDIT)
			pCmbBox=&m_cmbBox1;
		else
			pCmbBox=&m_cmbBox2;
		return pCmbBox->IsWindowVisible();
	}
	else if (nCtrlType==GCT_EDIT||nCtrlType==GCT_BTN_COMMON||
			 nCtrlType==GCT_BTN_EDIT||nCtrlType==GCT_BTN_FILEPATH)
	{
		return m_editBox.IsWindowVisible();
	}
	else if(nCtrlType==GCT_GRAYBUTTON)
	{
		return m_btnCtrl.IsWindowVisible();
	}
	else
		return false;
}

BOOL CSuperGridCtrl::SetImmerseMode(BOOL bImmerseMode,COLORREF immerseClr/*=-1*/)
{
	m_bImmerseMode=bImmerseMode;
	if(immerseClr!=-1)
		m_clrImmerse=immerseClr;
	return m_bImmerseMode;
}

void CSuperGridCtrl::OnKillfocusCmbBox() 
{
	m_bPropValueModified=TRUE;
	m_bHasInputCtrlFocus=FALSE;
	if(m_curEditItem.iItem<0||m_curEditItem.iSubItem<0)
		return;
	CTreeItem *pCurItem=GetTreeItem(m_curEditItem.iItem);
	if(pCurItem==NULL||pCurItem->m_lpNodeInfo==NULL)
		return;
	int nCtrlType=GetControlType(m_curEditItem.iItem,m_curEditItem.iSubItem);
	if(m_bEscKeyDown)
		m_bEscKeyDown = FALSE;
	else if(nCtrlType==GCT_CMB_COLOR||nCtrlType==GCT_CMB_EDIT)
	{	//可编辑下拉框必须在此处激发,因为在编辑条目时不会激发条目转换事件，故无法触发值修改存盘函数
		CString selStr;
		if(nCtrlType==GCT_CMB_EDIT && m_cmbBox1.GetSafeHwnd()!=NULL)
			m_cmbBox1.GetWindowText(selStr);
		else if(nCtrlType==GCT_CMB_COLOR && m_cmbBox3.GetSafeHwnd()!=NULL)
		{
			COLORREF curClr = m_cmbBox3.GetItemData(m_cmbBox3.GetCurSel());
			if(curClr!=0XFFFFFFFF)
			{	//未启用颜色对话框时执行以下代码
				m_curEditItem.iItem=-1;
				HideCmbBox();
			}
			return;
		}
		//仅有当前内容发生变化时才应该触发FireValueModify回调函数 wht 10-09-13
		char sTextValue[200]="";
		GetItemText(m_curEditItem.iItem,m_curEditItem.iSubItem,sTextValue,200);
		if( m_bPropValueModified&&selStr.CompareNoCase(sTextValue)!=0
			&&(FireValueModify==NULL||FireValueModify(this,pCurItem,m_curEditItem.iSubItem,selStr)))
		{
			pCurItem->m_lpNodeInfo->SetSubItemText(m_curEditItem.iSubItem,selStr);
			SetItemText(m_curEditItem.iItem,m_curEditItem.iSubItem,selStr.GetBuffer(1));
			m_bPropValueModified=FALSE;
		}
	}
	m_curEditItem.iItem = -1;
	HideCmbBox();
}

void CSuperGridCtrl::OnKillfocusBtn()
{
	m_bHasInputCtrlFocus=FALSE;
	if(m_btnCtrl.GetSafeHwnd())
		m_btnCtrl.ShowWindow(SW_HIDE);
	//带编辑框的按钮
	if(GetControlType(m_curEditItem.iItem,m_curEditItem.iSubItem)==GCT_BTN_EDIT
		||GetControlType(m_curEditItem.iItem,m_curEditItem.iSubItem)==GCT_BTN_FILEPATH)
	{	
		if(!m_editBox.IsWindowVisible()&&!m_btnCtrl.IsWindowVisible())
			m_curEditItem.iItem = -1;
	}
	else
		m_curEditItem.iItem = -1;
}
void CSuperGridCtrl::OnKillfocusEditBox()
{
	m_bHasInputCtrlFocus=FALSE;
	if(m_curEditItem.iItem<0||m_curEditItem.iSubItem<0)
		return;
	CTreeItem *pCurItem=GetTreeItem(m_curEditItem.iItem);
	if(pCurItem==NULL||pCurItem->m_lpNodeInfo==NULL)
		return;
	BOOL bValueChange=FALSE;
	int nCtrlType=GetControlType(m_curEditItem.iItem,m_curEditItem.iSubItem);
	if(m_editBox)
	{
		if(m_bEscKeyDown)
			m_bEscKeyDown = FALSE;
		else
		{
			CString newStr;
			m_editBox.GetWindowText(newStr);
			char sTextValue[200]="";
			GetItemText(m_curEditItem.iItem,m_curEditItem.iSubItem,sTextValue,200);
			//当前内容未发生变化时也应触发回调函数，故在判断内容是否相同之前调用回调函数 wht 10-07-03
			//仅有当前内容发生变化时才应该触发FireValueModify回调函数 wht 10-09-13
			if( m_bPropValueModified&&newStr.CompareNoCase(sTextValue)!=0
				&&(FireValueModify==NULL||FireValueModify(this,pCurItem,m_curEditItem.iSubItem,newStr)))	
			{
				pCurItem->m_lpNodeInfo->SetSubItemText(m_curEditItem.iSubItem,newStr);
				SetItemText(m_curEditItem.iItem,m_curEditItem.iSubItem,newStr.GetBuffer(1));
				m_bPropValueModified=FALSE;
				bValueChange=TRUE;
			}
		}
		//带编辑框的按钮
		if(GetControlType(m_curEditItem.iItem,m_curEditItem.iSubItem)==GCT_BTN_EDIT
			||GetControlType(m_curEditItem.iItem,m_curEditItem.iSubItem)==GCT_BTN_FILEPATH)
		{	
			if(!m_editBox.IsWindowVisible()&&!m_btnCtrl.IsWindowVisible())
				m_curEditItem.iItem = -1;
		}
		else
			m_curEditItem.iItem = -1;
		HideEditBox();
		if(FireKeyDownItem&&bValueChange)
		{	//编辑框失去焦点是调用，等价于按下VK_RETURN键 wht 18-03-30
			int iItem = GetSelectedItem();
			CTreeItem* pSelItem=iItem!=-1?GetTreeItem(iItem):NULL;
			NMLVKEYDOWN vKeyDown;
			vKeyDown.wVKey=VK_RETURN;
			FireKeyDownItem(this,pSelItem,&vKeyDown);
		}
	}
}

void CSuperGridCtrl::OnButton()
{
	if(m_curEditItem.iItem<0||m_curEditItem.iSubItem<0)
		return;
	CTreeItem *pItem=GetTreeItem(m_curEditItem.iItem);
	if(pItem==NULL)
		return;
	int nControlType=0;
	pItem->m_lpNodeInfo->GetControlType(m_curEditItem.iSubItem,nControlType);
	if (nControlType==GCT_GRAYBUTTON || nControlType == GCT_BTN_COMMON ||
		nControlType == GCT_BTN_FILEPATH ||nControlType == GCT_BTN_EDIT)
	{
		if(FireButtonClick==NULL||FireButtonClick(this,pItem,m_curEditItem.iSubItem))
		{
			if(	nControlType==GCT_BTN_FILEPATH || nControlType==GCT_BTN_EDIT)	
			{
				if(m_editBox.IsWindowVisible())
				{
					m_editBox.SetWindowText(pItem->m_lpNodeInfo->GetSubItemText(m_curEditItem.iSubItem));
					m_editBox.SetFocus();
					//int len = m_editBox.GetWindowTextLength();
					//m_editBox.SetSel(len, len);
				}
			}
			else 
			{
				if(m_bEscKeyDown)
					m_bEscKeyDown = FALSE;
				else if(FireValueModify&&nControlType!=GCT_GRAYBUTTON) //灰色孤立按钮不用调用FirValueModify wht 10-09-07
					FireValueModify(this,pItem,m_curEditItem.iSubItem,pItem->m_lpNodeInfo->GetSubItemText(m_curEditItem.iSubItem));
				/*
				POSITION pos = m_pCurEditItem->m_listChild.GetHeadPosition();
				while(pos!=NULL)
				{
					CPropTreeItem *pPropItem = (CPropTreeItem*)m_pCurEditItem->m_listChild.GetNext(pos);
					if(pPropItem&&pPropItem->FireValueModify && 
						pPropItem->FireValueModify(pPropItem,pPropItem->m_lpNodeInfo->m_strPropValue))
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = FALSE;
						
				}*/
				Invalidate();
			}
		}
	}
}

void CSuperGridCtrl::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	CTreeItem *pItem=GetTreeItem(pNMListView->iItem);
	if(pItem==NULL)
	{
		if (m_bFireOnNoneItemSelect&&FireItemChanged)
			FireItemChanged(this, NULL, pNMListView);
		return;
	}
	if(pNMListView->uNewState==0&&pNMListView->uOldState&TVIS_SELECTED)
	{
		if((pItem->m_dwPropGroup&GetBinaryWord(m_iPropGroup))==0)
			return;	//不属于当前分组 wht 10-12-02
		if(FireItemUnselect) //选中项发生改变时调用该函数 wht 10-09-13
			FireItemUnselect(this,pItem,pNMListView);
		if (m_bFireOnNoneItemSelect&&FireItemChanged)
			FireItemChanged(this,NULL,pNMListView);
	}
	else if(pNMListView->uNewState&TVIS_SELECTED)
	{
		if((pItem->m_dwPropGroup&GetBinaryWord(m_iPropGroup))==0)
			return;	//不属于当前分组 wht 10-12-02
		if(FireItemChanged) //选中项发生改变时调用该函数 wht 10-09-13
			FireItemChanged(this,pItem,pNMListView);
	}
}

void CSuperGridCtrl::OnSelchangeCmbBox()
{
	m_bPropValueModified=TRUE;
	m_bHasInputCtrlFocus=FALSE;
	if(m_curEditItem.iItem<0||m_curEditItem.iSubItem<0)
		return;
	CTreeItem *pItem=GetTreeItem(m_curEditItem.iItem);
	if(pItem==NULL)
		return;
	CColorSelectComboBox *pCmbBox=NULL;
	int iControlType=-1;
	pItem->m_lpNodeInfo->GetControlType(m_curEditItem.iSubItem,iControlType);
	if(iControlType==GCT_CMB_EDIT)
		pCmbBox=&m_cmbBox1;		//return;	//在OnKillfocusCmbBox中处理值修改事件
	else if(iControlType==GCT_CMB_COLOR)
		pCmbBox=&m_cmbBox3;
	else
		pCmbBox=&m_cmbBox2;
	if (pCmbBox->GetSafeHwnd())
	{
		if(m_bEscKeyDown)
			m_bEscKeyDown = FALSE;
		else if(iControlType==GCT_ITEMSET)
		{
			int iCurSel = pCmbBox->GetCurSel();
			//pItem->m_lpNodeInfo->SetSubItemText("%d",iCurSel);
			if(FireButtonClick)
				FireButtonClick(this,pItem,m_curEditItem.iSubItem);
			pCmbBox->SetCurSel(0);
		}
		else
		{	//若无此段代码当构件名称发生变化时不能即时更新构件颜色 
			CString selStr;
			BOOL bUseColorDlg=FALSE;	//是否启用颜色对话框
			if(iControlType==GCT_CMB_COLOR)
			{
				DWORD color = pCmbBox->GetItemData(pCmbBox->GetCurSel());
				if(color==0XFFFFFFFF)
				{	//弹出颜色对话框后，下拉框失去焦点触发OnKillfocusCmbBox
					CColorDialog dlg(color);
					if(dlg.DoModal()==IDOK)
					{
						bUseColorDlg=TRUE;
						color = dlg.GetColor();
						int iCur = m_cmbBox3.IsHaveThisColor(color);
						if(iCur==-1)
						{
							iCur = m_cmbBox3.AddColor(color,"自定义",FALSE);
							m_cmbBox3.SetCurSel(iCur);
						}
						else
							m_cmbBox3.SetCurSel(iCur);
					}
				}
				selStr.Format("RGB%X",color);
			}
			else
				pCmbBox->GetLBText(pCmbBox->GetCurSel(),selStr);
			CString sSubItemText=pItem->m_lpNodeInfo->GetSubItemText(m_curEditItem.iSubItem);
			//仅有当前内容发生变化时才应该触发FireValueModify回调函数 wht 10-09-13
			if( m_bPropValueModified&&pItem&&sSubItemText.CompareNoCase(selStr)!=0
				&&(FireValueModify==NULL||FireValueModify(this,pItem,m_curEditItem.iSubItem,selStr)))
			{	
				pItem->m_lpNodeInfo->SetSubItemText(m_curEditItem.iSubItem,selStr);
				SetItemText(m_curEditItem.iItem,m_curEditItem.iSubItem,selStr.GetBuffer(1));
				//pItem->m_lpNodeInfo->m_bMutiObjsProp=FALSE;
				m_bPropValueModified=FALSE;
			}
			//通过颜色对话框自定义颜色后，需在此处手动调用OnKillfocusCmbBox(); wht 10-05-12
			if(bUseColorDlg)	
				OnKillfocusCmbBox();	
		}
	}
}

void CSuperGridCtrl::OnChangeEditBox()
{
	m_bPropValueModified=TRUE;
	/*	//暂无必要这么早更新数据 wjh-2010.06.18
	if(m_editBox)
	{
		CTreeItem *pCurItem=GetTreeItem(m_curEditItem.iItem);
		if(pCurItem==NULL||pCurItem->m_lpNodeInfo==NULL)
			return;
		CString newStr;
		m_editBox.GetWindowText(newStr);
		if(FireValueModify)
			FireValueModify(this,pCurItem,m_curEditItem.iSubItem,newStr);
	}*/
}

int CSuperGridCtrl::GetControlType(int iRow,int iCol)
{
	if(iRow<0)
		return -1;
	CSuperGridCtrl::CTreeItem *pTreeItem=GetTreeItem(iRow);
	if(pTreeItem==NULL)
		return -1;
	CListCtrlItemInfo *pItemInfo=pTreeItem->m_lpNodeInfo;
	if(pItemInfo==NULL)
		return -1;
	int nCtrlType=-1;
	pItemInfo->GetControlType(iCol,nCtrlType);
	return nCtrlType;
}

void CSuperGridCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CListCtrl::OnSize(nType, cx, cy);
	RECT rc;
	if(m_btnCtrl)
	{
		m_btnCtrl.GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.left+=cx-rc.right;
		rc.right=cx;
		m_btnCtrl.MoveWindow(&rc);
	}
	if(m_editBox)
	{
		m_editBox.GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.right=cx;
		m_editBox.MoveWindow(&rc);
	}
	if(m_cmbBox1)
	{
		m_cmbBox1.GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.right=cx;
		m_cmbBox1.MoveWindow(&rc);
	}
	if(m_cmbBox2)
	{
		m_cmbBox2.GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.right=cx;
		m_cmbBox2.MoveWindow(&rc);
	}
	if(m_cmbBox3)
	{
		m_cmbBox3.GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.right=cx;
		m_cmbBox3.MoveWindow(&rc);
	}
}

void CSuperGridCtrl::HideEditBox()
{
	if(m_editBox)
	{
		RECT rc;
		m_editBox.GetWindowRect(&rc);
		m_editBox.ShowWindow(SW_HIDE);
		ScreenToClient(&rc);
		rc.top-=3;
		rc.bottom+=3;
		InvalidateRect(&rc);
	}
}

void CSuperGridCtrl::HideCmbBox()
{
	if(m_cmbBox1)
	{
		RECT rc;
		m_cmbBox1.GetWindowRect(&rc);
		m_cmbBox1.ShowWindow(SW_HIDE);
		ScreenToClient(&rc);
		rc.top-=3;
		rc.bottom+=3;
		InvalidateRect(&rc);
	}
	if(m_cmbBox2)
	{
		RECT rc;
		m_cmbBox2.GetWindowRect(&rc);
		m_cmbBox2.ShowWindow(SW_HIDE);
		ScreenToClient(&rc);
		rc.top-=3;
		rc.bottom+=3;
		InvalidateRect(&rc);
	}
	if(m_cmbBox3)
	{
		RECT rc;
		m_cmbBox3.GetWindowRect(&rc);
		m_cmbBox3.ShowWindow(SW_HIDE);
		ScreenToClient(&rc);
		rc.top-=3;
		rc.bottom+=3;
		InvalidateRect(&rc);
	}
}

void CSuperGridCtrl::HideInputCtrl()
{
	HideCmbBox();
	HideEditBox();
	if(m_btnCtrl)
		m_btnCtrl.ShowWindow(SW_HIDE);
}

//写文本数据到剪切板中
BOOL CSuperGridCtrl::WriteStrToClipboard(CString sValueText)
{
	if(sValueText.GetLength()<=0)
		return FALSE; 
	if(!::OpenClipboard(NULL)) 
		return FALSE; 
	::EmptyClipboard();
	HGLOBAL hglb=GlobalAlloc(GHND,sValueText.GetLength()+1);
	if(hglb != NULL) 
	{
		char *sTemp=(char*)::GlobalLock(hglb); 
		if(sTemp!=NULL) 
		{
			strcpy(sTemp,sValueText);
		}
		::SetClipboardData(CF_TEXT,hglb);//设置格式
		::GlobalUnlock(hglb); 
	}
	::CloseClipboard();
	return TRUE;
}
//从剪切板中读取文本数据 单元格内容不能为空
BOOL CSuperGridCtrl::ReadStrFromClipboard(TABLE_DATA &table_data)
{
	if(!IsClipboardFormatAvailable(CF_TEXT)) 
		return FALSE; 
	if(!::OpenClipboard(NULL)) 
		return FALSE; 
	HGLOBAL hglb=::GetClipboardData(CF_TEXT); 
	if(hglb != NULL) 
	{
		char *sTemp=(char*)::GlobalLock(hglb); 
		if(sTemp==NULL) 
			return FALSE;
		CString sBuffer;
		sBuffer.Format("%s",sTemp);
		sBuffer.Replace('\r',' ');
		//sBuffer.Replace('\t',' ');
		int nRowIndex=0;
		//按行拆分字符串
		while(nRowIndex<sBuffer.GetLength()&&nRowIndex!=-1)
		{
			int nColIndex=0;
			int nPrevRowIndex=nRowIndex;
			nRowIndex=sBuffer.Find("\n",nPrevRowIndex+1);
			if(nRowIndex>sBuffer.GetLength())
				break;
			CString sLineText=_T("");
			if(nPrevRowIndex==0)
				sLineText=sBuffer.Mid(0,nRowIndex);
			else
				sLineText=sBuffer.Mid(nPrevRowIndex+1,nRowIndex-nPrevRowIndex);
			if(sLineText.GetLength()<=0)
				continue;
			sLineText.Replace('\n',' ');
			int nCol=0;
			//按列拆分每行的字符串
			while(nColIndex<sLineText.GetLength()&&nColIndex!=-1)
			{
				int nPrevColIndex=nColIndex;
				nColIndex=sLineText.Find('\t',nPrevColIndex+1);
				if(nColIndex>sLineText.GetLength())
					break;
				CString sCellText=_T("");
				if(nPrevColIndex==0)
					sCellText=sLineText.Mid(0,nColIndex);
				else
					sCellText=sLineText.Mid(nPrevColIndex,nColIndex-nPrevColIndex);
				//将回车换行制表符等字符串替换为空格，移除字符串中多余的空格
				sCellText.Replace('\n',' ');
				sCellText.Replace('\t',' ');
				sCellText.Replace('\r',' ');
				sCellText.TrimLeft(' ');
				sCellText.TrimRight(' ');
				//if(sCellText.GetLength()<=0)
				//	continue;
				//将单元格元素添加到字符串数组中
				table_data.strArr.Add(sCellText);
				//统计列数
				nCol++;
			}
			//统计列数
			if(nCol>table_data.nMaxCol)
				table_data.nMaxCol=nCol;
			//统计行数
			if(nCol>0)
				table_data.nMaxRow++;
		}
		::GlobalUnlock(hglb); 
	}
	::CloseClipboard();
	return TRUE;
}
//拷贝选中项数据到剪贴板
BOOL CSuperGridCtrl::CopyDataToClipboard()
{	
	//1.将选中行转换为字符串,目前仅支持单行复制
	int iCurItem=GetSelectedItem();
	if(iCurItem==-1)
		return FALSE;
	CTreeItem *pItem=GetTreeItem(iCurItem);
	CHeaderCtrl *pHeader = GetHeaderCtrl();
	int nColumnCount = pHeader->GetItemCount();
	CString sLineText;
	for(int i=0;i<nColumnCount;i++)
	{
		if(sLineText.GetLength()>0)
			sLineText+="\t";
		pItem->m_lpNodeInfo->GetSubItemText(i);
		sLineText+=pItem->m_lpNodeInfo->GetSubItemText(i);
	}
	sLineText+="\r\n";
	//2.将字符串写入剪贴板
	return WriteStrToClipboard(sLineText);
}
//通过剪贴板中数据更新数据 
BOOL CSuperGridCtrl::PasteDataFromClipboard(int nMaxColNum/*=0*/,BOOL *read_statu_arr/*=NULL*/,
											BOOL bExpandAll/*=FALSE*/,BOOL bStartFromSelCol/*=TRUE*/,BOOL bIgnore/*=FALSE*/)
{	
	TABLE_DATA table_data;
	if(!ReadStrFromClipboard(table_data))
		return FALSE;
	CHeaderCtrl* pHeader = GetHeaderCtrl();
	if(nMaxColNum==0)
		nMaxColNum=pHeader->GetItemCount();
	int iStartRow=0,iStartCol=0,iCurItem=GetSelectedItem();
	if( m_CurSubItem !=-1&&iCurItem!=-1)
	{
		iStartRow=iCurItem;
		if(bStartFromSelCol)
			iStartCol = Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem);
	}
	else //默认从最后一行开始粘贴
		iStartRow=GetItemCount();
	CTreeItem *pStartItem=NULL,*pParentItem=NULL;
	if(GetItemCount()-1>iStartCol)
	{
		pStartItem=GetTreeItem(iStartRow);
		if(pStartItem)
			pParentItem=GetParentItem(pStartItem);
	}
	if(bExpandAll&&pStartItem)
	{
		int nScroll=0;
		ExpandAll(pStartItem,nScroll);
		SetRedraw(1);
		RedrawItems(iCurItem, nScroll);
		EnsureVisible(nScroll, TRUE);
	}
	for(int i=0;i<table_data.nMaxRow;i++)
	{
		CListCtrlItemInfo* lpInfo=NULL;
		CTreeItem *pCurItem=NULL;
		if((iStartRow+i)<GetItemCount())
			pCurItem=GetTreeItem(iStartRow+i);
		if(pCurItem)
			lpInfo=pCurItem->m_lpNodeInfo;
		else
		{	//不存在该条记录，需新插入一条记录
			lpInfo = new CListCtrlItemInfo();
			for(int k=0;k<nMaxColNum;k++)
			{
				if(read_statu_arr)
					lpInfo->AddSubItemText(_T(""),read_statu_arr[k]);
				else
				{	//未指定子项读写属性是新加项读写属性同开始项保持一致
					BOOL bReadOnly=FALSE;
					if(pStartItem&&pStartItem->m_lpNodeInfo)
						bReadOnly=pStartItem->m_lpNodeInfo->GetSubItemReadOnly(k);
					lpInfo->AddSubItemText(_T(""),bReadOnly);
				}
			}
		}
		for(int j=0;j<table_data.nMaxCol;j++)
		{
			CString sTextValue=table_data.GetData(i,j);
			BOOL bReadOnly=lpInfo->GetSubItemReadOnly(j+iStartCol);
			if(!bIgnore||!bReadOnly)//不修改只读单元格的内容
				lpInfo->SetSubItemText(j+iStartCol,sTextValue,bReadOnly);
		}
		if(pCurItem==NULL)
		{
			if(pParentItem==NULL)
				InsertRootItem(lpInfo);
			else
				InsertItem(pParentItem,lpInfo);
		}
	}
	Redraw();
	return TRUE;
}

BOOL CSuperGridCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	BOOL bRet = CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
	if(bRet&&IsDisplayCellCtrl(m_curEditItem.iItem,m_curEditItem.iSubItem))
		DisplayCellCtrl(m_curEditItem.iItem,m_curEditItem.iSubItem);
	if(FireMouseWheel)
	{
		//TODO:m_curEditItem.iItem==-1导致pSelItem=NULL情况 wjh-2018.3.30
		int iItem = GetSelectedItem();
		CTreeItem* pSelItem=iItem!=-1?GetTreeItem(iItem):NULL;
		FireMouseWheel(this,pSelItem,nFlags,zDelta,pt);
	}
	return bRet;
}

void CSuperGridCtrl::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();
	if((m_bEnableSort||m_bCustomDrawTitle)&&GetHeaderCtrl()!=NULL)	//CDialogEx::OnInitDialog中会先调用UpdateData进而触发PreSubclassWindow()，而此时GetHeaderCtrl可能还无值 wjh-2014.7.5
	{
		m_ctrlHeader.m_bEnableSort=m_bEnableSort;
		m_ctrlHeader.m_bCustomDrawTitle=m_bCustomDrawTitle;
		VERIFY( m_ctrlHeader.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd()) );
	}
}
bool CSuperGridCtrl::EnableSortItems(bool bCanSorted/*=true*/,bool bCanCustomDraw/*=false*/)
{
	m_bEnableSort=bCanSorted;
	m_bCustomDrawTitle=bCanCustomDraw;
	if(GetHeaderCtrl()->GetSafeHwnd())
	{
		if((bCanCustomDraw||bCanSorted)&&m_ctrlHeader.GetSafeHwnd()==NULL)
		{
			m_ctrlHeader.m_bEnableSort=m_bEnableSort;
			m_ctrlHeader.m_bCustomDrawTitle=m_bCustomDrawTitle;
			m_ctrlHeader.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());
		}
		else if(!bCanSorted&&!bCanCustomDraw)
			GetHeaderCtrl()->UnsubclassWindow();
		else
			return true;
		GetHeaderCtrl()->Invalidate();
		return true;
	}
	return false;
}

void CSuperGridCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(FireColumnClick!=NULL)
		FireColumnClick(this,pNMListView->iSubItem,m_ctrlHeader.IsSortCurrColAscending()!=FALSE);
	SortItemsBySpecSubItem(pNMListView->iSubItem);
	*pResult = 0;
}
//按指定列升序或降序排列
static int DefaultCompareSubItem(const CSuperGridCtrl::CSuperGridCtrlItemPtr& pItem1,
	const CSuperGridCtrl::CSuperGridCtrlItemPtr& pItem2,DWORD lPara)
{
	COMPARE_FUNC_EXPARA* pExPara=(COMPARE_FUNC_EXPARA*)lPara;
	int iSubItem=0;
	BOOL bAscending=true;
	if(pExPara)
	{
		iSubItem=pExPara->iSubItem;
		bAscending=pExPara->bAscending;
	}
	CString sText1=pItem1->m_lpNodeInfo->GetSubItemText(iSubItem);
	CString sText2=pItem2->m_lpNodeInfo->GetSubItemText(iSubItem);
	int result=sText1.Compare(sText2);
	if(!bAscending)
		result*=-1;
	return result;
}
void CSuperGridCtrl::SortItemsBySpecSubItem(int iSubItem,BOOL *pAscending/*=NULL*/)
{
	if(!m_bEnableSort||iSubItem<0||iSubItem>=m_arrHeader.GetSize())
		return;
	int i=0,item_count=m_RootItems.GetCount();
	CSuperGridCtrlItemPtr* arrItemPtrs = new CSuperGridCtrlItemPtr[item_count];
	POSITION pos=m_RootItems.GetHeadPosition();
	while(pos)
	{
		arrItemPtrs[i]=(CTreeItem*)m_RootItems.GetNext(pos);
		i++;
	}
	BOOL bAscending=TRUE;
	if(pAscending!=NULL)
		bAscending=*pAscending;
	else if(m_ctrlHeader.GetCurrSortColumn()==iSubItem)
		bAscending=!m_ctrlHeader.IsSortCurrColAscending();
	if(m_ctrlHeader.GetSafeHwnd()!=NULL)
		m_ctrlHeader.SetSortArrow(iSubItem,bAscending);
	COMPARE_FUNC_EXPARA sortpara(bAscending,iSubItem);
	if(FireCompareItem==NULL)
		CHeapSort<CTreeItem*>::HeapSortEx(arrItemPtrs,item_count,DefaultCompareSubItem,(DWORD)&sortpara);
	else
		CHeapSort<CTreeItem*>::HeapSortEx(arrItemPtrs,item_count,FireCompareItem,(DWORD)&sortpara);
	m_RootItems.RemoveAll();
	for(int i=0;i<item_count;i++)
		m_RootItems.AddTail(arrItemPtrs[i]);
	delete []arrItemPtrs;
	//重绘排序后的各超级列表项
	Redraw();
}

//////////////////////////////////////////////////////////////////////////
//
// not much but ... 
//
//////////////////////////////////////////////////////////////////////////

CSuperGridCtrl::CTreeItem::~CTreeItem()
{
	// delete child nodes
	POSITION pos = m_listChild.GetHeadPosition();
	while (pos != NULL)
	{
		CTreeItem* pItem = (CTreeItem*)m_listChild.GetNext(pos);
		if(pItem!=NULL)
		{
			if(pItem->m_lpNodeInfo!=NULL)
				delete pItem->m_lpNodeInfo;
			delete pItem;
		}
	}
	m_listChild.RemoveAll();
}

int CSuperGridCtrl::CTreeItem::GetTailIndex()
{
	int iTailIndex=m_nIndex;
	POSITION pos=m_listChild.GetHeadPosition();
	while(pos)
	{
		CTreeItem* pItem =(CTreeItem*)m_listChild.GetNext(pos);
		int index=pItem->GetTailIndex();
		if(index>iTailIndex)
			iTailIndex=index;
	}
	return iTailIndex;
}

void CSuperGridCtrl::CTreeItem::SetBkColor(COLORREF clr)
{
	for(int iCol=0; iCol < m_lpNodeInfo->GetItemCount(); iCol++)
		m_lpNodeInfo->SetSubItemColor(iCol,clr);
}
int  CSuperGridCtrl::CTreeItem::InsertSubItemText(CSuperGridCtrl* pListCtrl,int nCol, LPCTSTR strSubItem, BOOL bReadOnly/*=FALSE*/)
{
	if(m_lpNodeInfo==NULL)
		return -1;
	int index=m_lpNodeInfo->InsertSubItemText(nCol,strSubItem,bReadOnly);
	pListCtrl->InternalUpdateItemText(this);
	POSITION pos=m_listChild.GetHeadPosition();
	while(pos)
	{
		CTreeItem* pItem =(CTreeItem*)m_listChild.GetNext(pos);
		if(pItem)
			pItem->InsertSubItemText(pListCtrl,nCol,strSubItem,bReadOnly);
	}
	return index;
}
BOOL CSuperGridCtrl::CTreeItem::DeleteSubItemText(CSuperGridCtrl* pListCtrl,int nCol)
{
	if(m_lpNodeInfo==NULL)
		return -1;
	BOOL deleted=m_lpNodeInfo->DeleteSubItemText(nCol);
	pListCtrl->InternalUpdateItemText(this);
	POSITION pos=m_listChild.GetHeadPosition();
	while(pos)
	{
		CTreeItem* pItem =(CTreeItem*)m_listChild.GetNext(pos);
		if(pItem)
			pItem->DeleteSubItemText(pListCtrl,nCol);
	}
	return deleted;
}
void CSuperGridCtrl::AutoAdjustColumnWidth(int* ignoreColIndexArr/*=NULL*/,int arrSize/*=0*/)
{
	SetRedraw(FALSE);  
	CClientDC dc(this);
	CFont* pFont = GetFont();
	CFont* pOldFont = dc.SelectObject(pFont);
	CHeaderCtrl *pHeader = GetHeaderCtrl();  
	int nColumnCount = pHeader->GetItemCount();  
	for(int i = 0; i < nColumnCount; i++)  
	{  
		if(ignoreColIndexArr)
		{	//
			int j=0;
			for(j=0;j<arrSize;j++)
			{
				if(i==ignoreColIndexArr[j])
					break;
			}
			if(j<arrSize)
				continue;
		}
		SetColumnWidth(i, LVSCW_AUTOSIZE);  
		int nColumnWidth = GetColumnWidth(i);  
		SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);  
		int nHeaderWidth = GetColumnWidth(i);
		nColumnWidth=max(nColumnWidth, nHeaderWidth);
		for(int j=0;j<GetItemCount();j++)
		{
			CTreeItem *pItem=GetTreeItem(j);
			if(pItem)
			{
				CString ss=pItem->m_lpNodeInfo->GetSubItemText(i);
				int width=dc.GetTextExtent(ss).cx;
				nColumnWidth=max(nColumnWidth,width);
			}
		}
		if(i==nColumnCount-1)
			nColumnWidth-=15;	//最后一列保留一定宽度，方便框选
		SetColumnWidth(i,nColumnWidth); 
	}
	dc.SelectObject(pOldFont);
	SetRedraw(TRUE);  
}
//////////////////////////////////////////////////////////////////////////
//
// Simple class CRectangle for the + - sign, 
//
//////////////////////////////////////////////////////////////////////////

CRectangle::CRectangle(CSuperGridCtrl* pCtrl, CDC* pDC, int iIndent, const CRect& rcBounds)
{
	m_pDC=pDC;
	int nHalfImage = (pCtrl->m_cxImage >> 1);
	int nBottomDown = rcBounds.top + nHalfImage + ((rcBounds.Height() - pCtrl->m_cyImage) >> 1);
	m_right_bottom.cx = (pCtrl->m_cxImage>>1)+2+1;
	m_right_bottom.cy = (pCtrl->m_cyImage>>1)+2+1;
	m_left = rcBounds.left  + iIndent * pCtrl->m_cxImage - nHalfImage;
	m_top = nBottomDown - (m_right_bottom.cy >> 1);
	m_left_top.x = m_left -  (m_right_bottom.cx >> 1);
	m_left_top.y = m_top;
	m_topdown = nBottomDown;
}

void CRectangle::DrawRectangle(CSuperGridCtrl* pCtrl)
{
	//erase bkgrnd
	CRect rc(m_left_top, m_right_bottom);
	m_pDC->FillRect(rc, &pCtrl->m_brushErase);
	//draw rectangle	
	CPen* pPenRectangle = m_pDC->SelectObject(&pCtrl->m_psRectangle);
	m_pDC->Rectangle(rc);
	m_pDC->SelectObject(pPenRectangle);		
}

CRectangle::~CRectangle()
{
}

BOOL CRectangle::HitTest(CPoint pt)
{
	CRect rc = GetHitTestRect();
	return rc.PtInRect(pt);
}

void CRectangle::DrawPlus(void)
{
	m_pDC->MoveTo(m_left, m_topdown-2);
	m_pDC->LineTo(m_left, m_topdown+3);

	m_pDC->MoveTo(m_left-2, m_topdown);
	m_pDC->LineTo(m_left+3, m_topdown);
}

void CRectangle::DrawMinus(void)
{
	m_pDC->MoveTo(m_left-2, m_topdown);
	m_pDC->LineTo(m_left+3, m_topdown);
}

void CSuperGridCtrl::OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	if(FireEndtrack)
		FireEndtrack(this);
	Invalidate();
	*pResult = 0;
}

void CSuperGridCtrl::SetTextVerticalAligment(DWORD vAligment)
{
	m_verticalAligment=vAligment;
}
void CSuperGridCtrl::SetGridLineColor(COLORREF clr)
{
	m_clrGridLine=clr;
}
void CSuperGridCtrl::SetEvenRowBackColor(COLORREF clr)
{
	m_clrEvenRow=clr;
}
// Gradient - 渐变系数，立体背景用,不用渐变设为0  
void CSuperGridCtrl::SetHeaderBKColor(int R, int G, int B, int Gradient) //设置表头背景色  
{  
	m_ctrlHeader.m_R = R;  
	m_ctrlHeader.m_G = G;  
	m_ctrlHeader.m_B = B;  
	m_ctrlHeader.m_Gradient = Gradient;  
}  
// 设置表头高度  
void CSuperGridCtrl::SetHeaderHeightCoef(double heightCoef) //设置表头高度  
{  
	m_ctrlHeader.m_heightCoef = heightCoef;  
}  
void CSuperGridCtrl::SetHeaderFontHW(int nHeight,int nWidth) //设置头部字体宽和高  
{  
	m_ctrlHeader.m_fontHeight = nHeight;  
	m_ctrlHeader.m_fontWidth = nWidth;  
}  
void CSuperGridCtrl::SetHeaderTextColor(COLORREF color) //设置头部字体颜色  
{  
	m_ctrlHeader.m_colorText = color;  
}
void CSuperGridCtrl::SetFontHW(int nHeight,int nWith) //设置字体高和宽  
{  
	//m_fontHeight = nHeight;  
	//m_fontWidth = nWidth;  
}  
void CSuperGridCtrl::SetRowHeight(int nHeight) // 设置行高  
{  
	/*m_nRowHeight = nHeight;  
	CRect rcWin;  
	GetWindowRect(&rcWin);  
	WINDOWPOS wp;  
	wp.hwnd = m_hWnd;  
	wp.cx = rcWin.Width();  
	wp.cy = rcWin.Height();  
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;  
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);  */
}