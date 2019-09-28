#if !defined(AFX_XHLISTCTRL_H__26598C4B_999C_47F6_B65E_ABFD5D7E4E06__INCLUDED_)
#define AFX_XHLISTCTRL_H__26598C4B_999C_47F6_B65E_ABFD5D7E4E06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XhListCtrl.h : header file
//

#include "SortHeaderCtrl.h"
#include "ListViewComparer.h"
#include "XhCharString.h"
#include "HashTable.h"
#include "MemberProperty.h"

//////////////////////////////////////////////////////////////////////////
// CLinkProperties
#define DEFAULT_UNVISITED_COLOR RGB(0, 0, 255)
#define DEFAULT_VISITED_COLOR RGB(128,0,128)
#define DEFAULT_UNDERLINED TRUE
#define DEFAULT_HOTSPOT_CURSOR NULL

class CLinkProperties  
{
public:
	static UINT DEFAULT_HOT_CURSOR;
public:
	CLinkProperties();
	virtual ~CLinkProperties();

	void SetLinkProperties(	COLORREF clrLink = DEFAULT_UNVISITED_COLOR, 
							COLORREF clrVisited = DEFAULT_VISITED_COLOR, 
							HCURSOR hCursorHotSpot = DEFAULT_HOTSPOT_CURSOR, 
							BOOL bUnderlinedLink = DEFAULT_UNDERLINED);
	BOOL		GetUnderlinedLink() {return m_bUnderlinedLink;}
	COLORREF	GetLinkColor() { return m_clrLink;}
	COLORREF	GetVisitedLinkColor() { return m_clrVisited;}
	HCURSOR		GetCursor() { return m_hCursorHotSpot;}
private:
	BOOL		m_bUnderlinedLink;
	COLORREF	m_clrLink, m_clrVisited;
	HCURSOR		m_hCursorHotSpot;
};

//////////////////////////////////////////////////////////////////////////
// CLinkItem
class CLinkItem : public CObject
{
public:
	CLinkItem(){m_bVisited = FALSE; m_pLinkProp = NULL;}
	virtual ~CLinkItem(){;}

	//Attributes
public:
	CString GetUrl() { return m_strUrl; }
	void SetUrl(CString strUrl) { m_strUrl = strUrl; }
	BOOL GetVisited() { return m_bVisited; }
	void SetVisited(BOOL bVisited) { m_bVisited = bVisited; }
	BOOL GetUnderlinedLink();
	COLORREF GetLinkColor(); 
	HCURSOR GetCursor();
	CLinkProperties* GetLinkProperties() { return m_pLinkProp;}
	void SetLinkProperties(CLinkProperties* pLinkProp) { m_pLinkProp = pLinkProp;}

protected:
	BOOL m_bVisited;
	CString m_strUrl;
	CLinkProperties* m_pLinkProp;
};

/////////////////////////////////////////////////////////////////////////////
// CXhListCtrl window
struct XHLISTCTRL_COLUMN{
	int width;
	CXhChar50 headerName;
	XHLISTCTRL_COLUMN(){
		width=80;
	}
	XHLISTCTRL_COLUMN(char* name,int colWidth=80){
		headerName.Copy(name);
		width=colWidth;
	}
};
#include "afxtempl.h"
class CXhListCtrl : public CListCtrl
{
	COLORREF m_oddItemBkColor;		//奇数行背景颜色
	COLORREF m_evenItemBkColor;		//偶数行背景颜色
	COLORREF m_oddItemTextColor;	//奇数行文本颜色
	COLORREF m_evenItemTextColor;	//偶数行文本颜色
	COLORREF m_selectItemNoFocusBkColor;//无焦点时选中行背景颜色
	COLORREF m_selectItemBkColor;	//选中行背景颜色
	COLORREF m_selectItemTextColor;	//选中行文本颜色
	COLORREF m_hoverItemBkColor;	//热点行背景颜色
	COLORREF m_hoverItemTextColor;	//热点行文本颜色

	CListViewComparer listViewComparer;
	BOOL (*FireDblclk)(CXhListCtrl* pListCtrl,int iItem,int iSubItem);
// Construction
public:
	CXhListCtrl();
	virtual ~CXhListCtrl();

// Attributes
public:
	CArray<XHLISTCTRL_COLUMN,XHLISTCTRL_COLUMN&> m_arrHeader;
	int AddColumnHeader(char* name,int colWidth=80){
		m_arrHeader.Add(XHLISTCTRL_COLUMN(name,colWidth));
		return m_arrHeader.GetSize()-1;
	}
// Operations
public:
	int InsertItemRecord(int iItem, CStringArray &str_arr);
	BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText, LPCTSTR lpctszUrl = NULL);
	void SetItemTextColor(int iItem,int iSubItem,COLORREF clf);
	void SetItemBackColor(int iItem,int iSubItem,COLORREF clf);
	void EmptyItemColor(int iItem,int iSubItem,BOOL bBackColor);
	void SetOddItemBkColor(COLORREF color,BOOL bDraw);		//设置奇数行背景颜色
	void SetEvenItemBkColor(COLORREF color,BOOL bDraw);		//设置偶数行背景颜色
	void SetOddItemTextColor(COLORREF color,BOOL bDraw);	//设置奇数行文本颜色
	void SetEvenItemTextColor(COLORREF color,BOOL bDraw);   //设置偶数行文本颜色
	void SetSelectItemBkColor(COLORREF color,BOOL bFocused,BOOL bDraw);	//设置选中行背景颜色  
	void SetSelectItemTextColor(COLORREF color,BOOL bDraw); //设置选中行文本颜色
	void SetHoverItemBkColor(COLORREF color,BOOL bDraw);	//设置热点行背景颜色
	void SetHoverItemTextColor(COLORREF color,BOOL bDraw);  //设置热点行文本颜色
	void AutoAdjustColumnWidth(int* ignoreColIndexArr=NULL,int arrSize=0);
private:
	void RefreshListViewHeader(long* col_wide_arr);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXhListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
//
protected:	//超链接功能属性方法
	BOOL _bEnableSortItems;
	CLinkProperties m_link;
	CFont m_fontLink;
	CFont m_fontNoLink;
	HCURSOR m_hCursorArrow;
	CHashStrList<CLinkItem*> hashObjPtrList;
	CMapList<COLORREF> m_mapTextClf,m_mapBackClf;	//colors
	void InitFont(CLinkProperties link, CFont *pFont);
	CLinkItem* GetLink(int i, int j);
	void SetLink(int i, int j, CLinkItem* pItemLink);
// Implementation
public:	
	PROPERTY(BOOL,m_bEnableSortItems);
	GET(m_bEnableSortItems){return _bEnableSortItems;}
	SET(m_bEnableSortItems){_bEnableSortItems=m_bEnableSortItems;m_ctrlHeader.m_bEnableSort=m_bEnableSortItems;}
	void InitListCtrl(long* col_wide_arr=NULL);
	void InitSortHeadCtrl();
	void SetColumnSortType(int iCol,COL_SORT_TYPE sortType);
	int GetSelectedItem(void) const;
	int GetSortColunmnIndex();
	//按指定列升序或降序排列
	void SortItemsBySpecSubItem(int iSubItem,BOOL *pAscending=NULL);
	void SetDblclkFunc(BOOL (*func)(CXhListCtrl* pListCtrl,int iItem,int iSubItem)){FireDblclk = func;}
	// Generated message map functions
protected:
	CSortHeaderCtrl m_ctrlHeader;
	//{{AFX_MSG(CXhListCtrl)
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XHLISTCTRL_H__26598C4B_999C_47F6_B65E_ABFD5D7E4E06__INCLUDED_)
