// XhListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "XhListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CLinkProperties
UINT CLinkProperties::DEFAULT_HOT_CURSOR = 0;
CLinkProperties::CLinkProperties()
{
	//Initialize date with most commonly used values
	m_bUnderlinedLink	= DEFAULT_UNDERLINED;
	m_clrLink			= DEFAULT_UNVISITED_COLOR;
	m_clrVisited		= DEFAULT_VISITED_COLOR;
	if(DEFAULT_HOT_CURSOR>0)
		m_hCursorHotSpot = AfxGetApp()->LoadCursor(DEFAULT_HOT_CURSOR);
	else
		m_hCursorHotSpot = NULL;
#if(WINVER >= 0x0500)
	if(m_hCursorHotSpot==NULL)
		m_hCursorHotSpot = AfxGetApp()->LoadStandardCursor(IDC_HAND); //Icon hand
#endif
}

CLinkProperties::~CLinkProperties()
{

}

void CLinkProperties::SetLinkProperties(COLORREF clrLink /*= DEFAULT_UNDERLINED*/, 
										COLORREF clrVisited /*= DEFAULT_UNVISITED_COLOR*/, 
										HCURSOR hCursorHotSpot /*= DEFAULT_HOTSPOT_CURSOR*/, 
										BOOL bUnderlinedLink /*= DEFAULT_UNDERLINED*/) 
{
		m_bUnderlinedLink = bUnderlinedLink;
		m_clrLink = clrLink;
		m_clrVisited = clrVisited;
		if (hCursorHotSpot == NULL) 
		{
			if(DEFAULT_HOT_CURSOR>0)
				m_hCursorHotSpot = AfxGetApp()->LoadCursor(DEFAULT_HOT_CURSOR);
			else
				m_hCursorHotSpot = NULL;
#if(WINVER >= 0x0500)
			if(m_hCursorHotSpot==NULL)
				m_hCursorHotSpot = AfxGetApp()->LoadStandardCursor(IDC_HAND); //Icon hand
#endif
		}
		else 
			m_hCursorHotSpot = hCursorHotSpot;
}
//////////////////////////////////////////////////////////////////////////
//CLinkItem
COLORREF CLinkItem::GetLinkColor() 
{ 
	if (m_pLinkProp == NULL) return RGB(0,0,0);
	if (GetVisited()) 
		return m_pLinkProp->GetVisitedLinkColor();
	return m_pLinkProp->GetLinkColor();
}

BOOL CLinkItem::GetUnderlinedLink() 
{
	if (m_pLinkProp == NULL) 
		return DEFAULT_UNDERLINED;
	return m_pLinkProp->GetUnderlinedLink();
}

HCURSOR CLinkItem::GetCursor() 
{
	if (m_pLinkProp == NULL) 
		return DEFAULT_HOTSPOT_CURSOR;
	return m_pLinkProp->GetCursor();
}
/////////////////////////////////////////////////////////////////////////////
// CXhListCtrl

CXhListCtrl::CXhListCtrl()
{
	m_bEnableSortItems=FALSE;
	listViewComparer.m_pListCtrl = this;
	FireDblclk=NULL;
	m_oddItemBkColor=0xFEF1E3;	//奇数行背景颜色
	m_evenItemBkColor=0xFFFFFF;	//偶数行背景颜色
	m_hoverItemBkColor=0xFF;	//热点行背景颜色
	m_selectItemNoFocusBkColor=GetSysColor(COLOR_HIGHLIGHT);	//ListCtrl无焦点时选中行背景颜色
	m_selectItemBkColor=GetSysColor(COLOR_HIGHLIGHT);	//选中行背景颜色
	m_oddItemTextColor=GetSysColor(COLOR_BTNTEXT);		//奇数行文本颜色
	m_evenItemTextColor=GetSysColor(COLOR_BTNTEXT);		//偶数行文本颜色
	m_hoverItemTextColor=GetSysColor(COLOR_HIGHLIGHTTEXT);	//热点行文本颜色 
	m_selectItemTextColor=GetSysColor(COLOR_HIGHLIGHTTEXT);	//选中行文本颜色
}

CXhListCtrl::~CXhListCtrl()
{
	for (CLinkItem **ppItem=hashObjPtrList.GetFirst();ppItem;ppItem=hashObjPtrList.GetNext())
	{
		if(ppItem)
			delete *ppItem;
	}
}


BEGIN_MESSAGE_MAP(CXhListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CXhListCtrl)
	ON_NOTIFY_REFLECT_EX(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXhListCtrl message handlers

void CXhListCtrl::InitListCtrl(long* col_wide_arr)
{
	SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	m_hCursorArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_link.SetLinkProperties();
	InitFont(m_link, &m_fontLink);
	if (!(HFONT)m_fontNoLink) 
	{
		LOGFONT lf;
		GetFont()->GetObject(sizeof(lf), &lf);
		lf.lfUnderline = FALSE;
		m_fontNoLink.CreateFontIndirect(&lf);
	}
	RefreshListViewHeader(col_wide_arr);
	listViewComparer.SetMaxColumnCount((int)m_arrHeader.GetSize());
	
}

void CXhListCtrl::RefreshListViewHeader(long* col_wide_arr)
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
		NUM_COLUMNS = (int)m_arrHeader.GetSize();
	else
		NUM_COLUMNS = 0;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for(i = 0; i<NUM_COLUMNS; i++)
	{
		lvc.iSubItem = i;
		lvc.pszText = m_arrHeader[i].headerName;
		if(col_wide_arr!=NULL)
			lvc.cx = m_arrHeader[i].width = col_wide_arr[i];
		else
			lvc.cx = m_arrHeader[i].width;
		lvc.fmt = LVCFMT_LEFT;//_gnColumnFmt[i];
		InsertColumn(i,&lvc);
	}
}
int CXhListCtrl::InsertItemRecord(int iItem, CStringArray &str_arr)
{
	// insert items
	LV_ITEM lvi;
	if(iItem==-1)
		iItem=GetItemCount();

	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.pszText = str_arr[0].GetBuffer(1);//_gszItem[i][0];
	iItem = InsertItem(&lvi);
	// set item text for additional columns
	int nCount = (int)m_arrHeader.GetSize();
	nCount = __min(nCount,(int)str_arr.GetSize());
	for(int j = 1; j<nCount; j++)
	{
		str_arr[j].TrimLeft();
		str_arr[j].TrimRight();
		SetItemText(iItem,j,str_arr[j]);
	}
	RedrawItems(iItem,iItem);
	return iItem;
}

void CXhListCtrl::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();
	VERIFY( m_ctrlHeader.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd()) );
}
void CXhListCtrl::InitSortHeadCtrl()
{
	HWND hWnd=GetHeaderCtrl()->GetSafeHwnd();
	VERIFY( m_ctrlHeader.SubclassWindow(hWnd) );
}

void CXhListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	SortItemsBySpecSubItem(pNMListView->iSubItem);
	*pResult = 0;
}
void CXhListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	LVHITTESTINFO ht;
	ht.pt = point;
	SubItemHitTest(&ht);
	if(ht.iItem<0)
		return;
	CString strUrl;
	CLinkItem* pLink = GetLink(ht.iItem, ht.iSubItem);
	if (pLink != NULL) 
		SetCursor(pLink->GetCursor());
	else
		SetCursor(m_hCursorArrow);
}

void CXhListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( GetFocus() != this) 
		SetFocus();

	LVHITTESTINFO ht;
	ht.pt = point;
	SubItemHitTest(&ht);
	if(ht.iItem<0)
		return;
	CString strUrl;
	CLinkItem* pLink = GetLink(ht.iItem, ht.iSubItem);
	if (pLink != NULL) 
	{
		strUrl = pLink->GetUrl();
		if (!strUrl.IsEmpty()) 
		{
			ShellExecute(m_hWnd, NULL, strUrl, NULL, NULL, SW_SHOWNORMAL);
			pLink->SetVisited(TRUE);
		}
	}
	CListCtrl::OnLButtonDown(nFlags,point);
}

void CXhListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT *pResult) 
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	CLinkItem *pLink = NULL;
	*pResult = 0;

	const int OFFSET_FIRST=2;
	const int OFFSET_OTHER=6;
	int iItem=(int)pLVCD->nmcd.dwItemSpec;
	CDC* pDC=CDC::FromHandle(pLVCD->nmcd.hdc);
	switch (pLVCD->nmcd.dwDrawStage) 
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		pLink = GetLink((int)pLVCD->nmcd.dwItemSpec, pLVCD->iSubItem);
		if (pLink != NULL) 
		{
			pLVCD->clrText = pLink->GetLinkColor();
			if (pLink->GetUnderlinedLink()) 
				::SelectObject(pLVCD->nmcd.hdc, m_fontLink.m_hObject);
			else 
				::SelectObject(pLVCD->nmcd.hdc, m_fontNoLink.m_hObject);
		}
		else 
		{
			::SelectObject(pLVCD->nmcd.hdc, m_fontNoLink.m_hObject);
			/*if(iItem==m_nHoverIndex)
			{	//热点行  
				pLVCD->clrTextBk=m_hoverItemBkColor;  
				pLVCD->clrText=m_hoverItemTextColor;  
			}
			else */
			if(GetItemState((int)iItem,LVIS_SELECTED) == LVIS_SELECTED)
			{	//选中行  
				pLVCD->clrTextBk=m_selectItemBkColor;  
				pLVCD->clrText=m_selectItemTextColor;
				//
				CRect rc,rcIcon;
				GetItemRect((int)iItem,&rc,LVIR_BOUNDS);
				GetItemRect((int)iItem,&rcIcon,LVIR_ICON);
				rc.left=rcIcon.right;
				if(GetFocus()==this)
					pDC->FillSolidRect(&rc,m_selectItemBkColor);
				else
					pDC->FillSolidRect(&rc,m_selectItemNoFocusBkColor);
				pDC->SetTextColor(m_selectItemTextColor);
				pDC->SetBkColor(m_selectItemBkColor);
				CFont* pfnt=pDC->GetCurrentFont();
				
				CRect rcClipBox,rcSubItem,rcCurrent;
				pDC->GetClipBox(rcClipBox);
				GetItemRect((int)iItem, rcSubItem, LVIR_LABEL);
				LV_COLUMN lvc;
				lvc.mask = LVCF_FMT | LVCF_WIDTH;
				for(int iCol=0; GetColumn(iCol, &lvc); iCol++)
				{
					if(iCol>0)
					{
						rcSubItem.left=rcSubItem.right;
						rcSubItem.right+=lvc.cx;
					}
					if (rcSubItem.left < rcClipBox.right && rcSubItem.right > rcClipBox.left && rcSubItem.right > rcSubItem.left)
					{
						CString sText=GetItemText((int)iItem,iCol);
						rcCurrent=rcSubItem;
						rcCurrent.left+=(iCol==0?OFFSET_FIRST:OFFSET_OTHER);
						rcCurrent.right-=(iCol==0?OFFSET_FIRST:OFFSET_OTHER);
						pDC->DrawText(sText,-1,rcCurrent, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_EXTERNALLEADING);
					}
				}
				pDC->SelectObject(pfnt);
			}
			else if(iItem % 2==0)
			{	//偶数行
				pLVCD->clrTextBk=m_evenItemBkColor;  
				pLVCD->clrText=m_evenItemTextColor;  
			}
			else
			{	//奇数行 
				pLVCD->clrTextBk=m_oddItemBkColor;  
				pLVCD->clrText=m_oddItemTextColor;  
			}
			COLORREF *pClrText=m_mapTextClf.GetValue((int)pLVCD->nmcd.dwItemSpec+1,pLVCD->iSubItem+1);
			if(pClrText)
				pLVCD->clrText=*pClrText;
			COLORREF *pClrTextBk=m_mapBackClf.GetValue((int)pLVCD->nmcd.dwItemSpec+1,pLVCD->iSubItem+1);
			if(pClrTextBk)
				pLVCD->clrTextBk=*pClrTextBk;
			
		}
		//if(GetItemState(iItem,LVIS_SELECTED) == LVIS_SELECTED)
		//	*pResult = CDRF_SKIPDEFAULT;
		//else
			*pResult = CDRF_DODEFAULT;
		break;
	default:
		break;
	}
}
//按指定列升序或降序排列
void CXhListCtrl::SortItemsBySpecSubItem(int iSubItem,BOOL *pAscending/*=NULL*/)
{
	if(iSubItem<0||iSubItem>=m_arrHeader.GetSize())
		return;
	if(m_bEnableSortItems)
	{
		listViewComparer.m_iColumn = iSubItem;
		if(pAscending==NULL)
			listViewComparer.sortOrderAscending[iSubItem]=!listViewComparer.sortOrderAscending[iSubItem];
		else 
			listViewComparer.sortOrderAscending[iSubItem]=*pAscending;
		// show the appropriate arrow in the header control.
		if(m_ctrlHeader.GetSafeHwnd()!=NULL)
			m_ctrlHeader.SetSortArrow(iSubItem, listViewComparer.sortOrderAscending[iSubItem]);
		SortItems(listViewComparer.FireCompareItemFunc,(DWORD)&listViewComparer);
	}
}

int CXhListCtrl::GetSortColunmnIndex()
{
	return listViewComparer.m_iColumn;
}
void CXhListCtrl::SetColumnSortType(int iCol,COL_SORT_TYPE sortType)
{
	listViewComparer.SetColumnSortType(iCol,sortType);
}

int CXhListCtrl::GetSelectedItem(void) const
{
	return GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); 
}

void CXhListCtrl::InitFont(CLinkProperties link, CFont *pFont)
{
	if (pFont == NULL) 
		return;
	if (!(HFONT)(*pFont)) 
	{
		LOGFONT lf;
		GetFont()->GetObject(sizeof(lf), &lf);
		lf.lfUnderline = (BYTE)link.GetUnderlinedLink();
		pFont->CreateFontIndirect(&lf);
	}
}

CLinkItem* CXhListCtrl::GetLink(int i, int j) 
{
	char sLocation[100]="";
	sprintf(sLocation,"%d-%d",i,j);
	CLinkItem **ppItem=hashObjPtrList.GetValue(sLocation);
	if(ppItem)
		return *ppItem;
	else 
		return NULL;
}

void CXhListCtrl::SetLink(int i, int j, CLinkItem* pItemLink) 
{
	char sLocation[100]="";
	sprintf(sLocation,"%d-%d",i,j);
	hashObjPtrList.SetValue(sLocation,pItemLink);
}

BOOL CXhListCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpctszText,
	LPCTSTR lpctszUrl /*= NULL*/) 
{
	BOOL bRtn = CListCtrl::SetItemText(nItem, nSubItem, lpctszText);
	if(lpctszUrl!=NULL)
	{
		CLinkItem *pLinkItem = NULL;
		pLinkItem = new CLinkItem();
		if (lpctszUrl && _tcslen(lpctszUrl) != 0) 
		{
			pLinkItem->SetUrl(lpctszUrl);
			pLinkItem->SetLinkProperties(&m_link);
			SetLink(nItem, nSubItem, pLinkItem);
		} 
	}
	return bRtn;
}

void CXhListCtrl::SetItemTextColor(int iItem,int iSubItem,COLORREF clf)
{
	if(iItem<0||iItem>=GetItemCount()||iSubItem>=m_arrHeader.GetSize())
		return;
	if(iSubItem<0)
	{
		for(int i=0;i<m_arrHeader.GetSize();i++)
			m_mapTextClf.SetValue(iItem+1,i+1,clf);
	}
	else
		m_mapTextClf.SetValue(iItem+1,iSubItem+1,clf);
}

void CXhListCtrl::SetItemBackColor(int iItem,int iSubItem,COLORREF clf)
{
	if(iItem<0||iItem>=GetItemCount()||iSubItem>=m_arrHeader.GetSize())
		return;
	if(iSubItem<0)
	{
		for(int i=0;i<m_arrHeader.GetSize();i++)
			m_mapBackClf.SetValue(iItem+1,i+1,clf);
	}
	else
		m_mapBackClf.SetValue(iItem+1,iSubItem+1,clf);
}

void CXhListCtrl::EmptyItemColor(int iItem,int iSubItem,BOOL bBackColor)
{
	if(iItem<0||iItem>=GetItemCount()||iSubItem>=m_arrHeader.GetSize())
		return;
	CMapList<COLORREF> *pMapColor=bBackColor?&m_mapBackClf:&m_mapTextClf;
	if(iSubItem<0)
	{
		for(int i=0;i<m_arrHeader.GetSize();i++)
			pMapColor->DeleteNode(iItem+1,i+1);
	}
	else
		pMapColor->DeleteNode(iItem+1,iSubItem+1);
}

BOOL CXhListCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	LVHITTESTINFO ht;
	ht.pt = pNMListView->ptAction;
	SubItemHitTest(&ht);
	if(ht.iItem !=-1&&FireDblclk)
	{
		FireDblclk(this,ht.iItem,ht.iSubItem);
		return TRUE;
	}
	else
		return FALSE;	//返回FALSE时父窗口的NM_DBLCLK消息可用 wht 15-03-01
}

void CXhListCtrl::SetOddItemBkColor(COLORREF color,BOOL bDraw)
{	//设置奇数行背景颜色  
	m_oddItemBkColor=color;  
	if(bDraw)
		InvalidateRect(NULL);  
}  
void CXhListCtrl::SetEvenItemBkColor(COLORREF color,BOOL bDraw)
{	//设置偶数行背景颜色
	m_evenItemBkColor=color;  
	if(bDraw)
		InvalidateRect(NULL);  
}  
void CXhListCtrl::SetHoverItemBkColor(COLORREF color,BOOL bDraw)
{	//设置热点行背景颜色
	m_hoverItemBkColor=color;  
	if(bDraw)
		InvalidateRect(NULL);  
}  
void CXhListCtrl::SetSelectItemBkColor(COLORREF color,BOOL bFocused,BOOL bDraw)
{	//设置选中行背景颜色
	if(bFocused)
		m_selectItemNoFocusBkColor=color;
	else
		m_selectItemBkColor=color;
	if(bDraw)
		InvalidateRect(NULL);  
}  
void CXhListCtrl::SetOddItemTextColor(COLORREF color,BOOL bDraw)
{	//设置奇数行文本颜色
	m_oddItemTextColor=color;  
	if(bDraw)
		InvalidateRect(NULL);  
}  
void CXhListCtrl::SetEvenItemTextColor(COLORREF color,BOOL bDraw)
{	//设置偶数行文本颜色
	m_evenItemTextColor=color;  
	if(bDraw)
		InvalidateRect(NULL);  
}  
void CXhListCtrl::SetHoverItemTextColor(COLORREF color,BOOL bDraw)  
{	//设置热点行文本颜色
	m_hoverItemTextColor=color;  
	if(bDraw)
		InvalidateRect(NULL);  
}  
void CXhListCtrl::SetSelectItemTextColor(COLORREF color,BOOL bDraw)
{	//设置选中行文本颜色
	m_selectItemTextColor=color;  
	if(bDraw)
		InvalidateRect(NULL);  
}
void CXhListCtrl::AutoAdjustColumnWidth(int* ignoreColIndexArr/*=NULL*/,int arrSize/*=0*/)
{
	SetRedraw(FALSE);  
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
		if(i==nColumnCount-1)
			nColumnWidth-=15;	//最后一列保留一定宽度，方便框选
		SetColumnWidth(i,nColumnWidth); 
	}
	SetRedraw(TRUE);  
}