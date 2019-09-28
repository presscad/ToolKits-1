#if !defined(AFX_SUPERGRIDCTRL_H__C6DF1701_806D_11D2_9A94_002018026B76__INCLUDED_)
#define AFX_SUPERGRIDCTRL_H__C6DF1701_806D_11D2_9A94_002018026B76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SuperGridCtrl.h : header file

#define GCT_STATIC				0	//GCT = grid cell type
#define GCT_EDIT				1
#define GCT_GRAYBUTTON			2	//孤立的灰色条目按钮项
#define GCT_ITEMSET				3	//一组通过下拉框选择进行的操作，而非选择其中一个选项赋值
#define GCT_BTN_FILEPATH		4	//按钮显示方式
#define GCT_BTN_COMMON			5
#define GCT_BTN_EDIT			6
#define GCT_CMB_EDIT			7	//下拉框显示方式
#define GCT_CMB_LIST			8
#define GCT_CMB_COLOR			9

#define IDC_CELLCMBBOX   4712
#define IDC_CELLEDITBOX  4713
#define IDC_CELLBTNCTRL  4714

////////////////////////////////////////////
#include ".\SuperEdit.h"
#include ".\ColorSelectComboBox.h"
#include ".\XhCharString.h"
#include ".\HashTable.h"
#include ".\SortHeaderCtrl.h"

class CListCtrlItemInfo //should have called this LV_INSERTITEM ..what ever
{
public:
	CListCtrlItemInfo(){m_bCheck=FALSE;m_iImage=-1;m_lParam=NULL;}
	~CListCtrlItemInfo();
	//
	int GetItemCount(void) const { return (int)m_subItems.GetSize(); }
	int AddSubItemText(const char* strSubItem, BOOL bReadOnly=FALSE);
	void SetSubItemText(int iSubItem, const char* strSubItem, const char* strHelp=_T(""));
	void SetSubItemText(int iSubItem, const char* strSubItem, BOOL bReadOnly, const char* strHelp=_T(""));
	CString GetSubItemText(int iSubItem);
	int  InsertSubItemText(int nCol, LPCTSTR strSubItem="", BOOL bReadOnly=FALSE);
	BOOL DeleteSubItemText(int nCol);
	//
	void SetSubItemHelpText(int iSubItem, const char* strHelp);
	CString GetSubItemHelpText(int iSubItem);
	// set item read only state
	//void InitItemReadOnly(int nSize,BOOL *read_only_arr=NULL);
	void SetSubItemReadOnly(int iSubItem,BOOL bReadOnly);
	BOOL GetSubItemReadOnly(int iSubItem) const;

	//all cols in this row is default edit
	void SetControlType(int iSubItem, int nCtrlType);
	BOOL GetControlType(int iSubItem, int& nCtrlType);
	//
	void SetListItemsStr(int iSubItem, CString itemsStr);
	BOOL GetListItemsStr(int iSubItem, CString& itemsStr);

	//used if LVS_EX_CHECKBOXES style
	void SetCheck(BOOL bCheck){ m_bCheck = bCheck;}
	BOOL GetCheck(void) const {return m_bCheck;}

	void SetImage(int iImage){m_iImage = iImage;}
	int GetImage(void) const {return m_iImage;}//return icon
	void SetSubItemColor(int iSubItem, COLORREF clf);
	void DeleteSubItemColor(int iSubItem){
		m_mapClf.DeleteNode(iSubItem+1);
	}
	COLORREF GetSubItemColor(int iSubItem)
	{
		if(m_mapClf.GetValue(iSubItem+1)==NULL)
			return (COLORREF)-1;
		else
			return m_mapClf[iSubItem+1];
	}

	void SetSubItemTextColor(int iSubItem, COLORREF clf);
	COLORREF GetSubItemTextColor(int iSubItem)
	{
		if(m_mapTextClf.GetValue(iSubItem+1)==NULL)
			return (COLORREF)-1;
		else
			return m_mapTextClf[iSubItem+1];
	}
	//YOU SHOULD MODIFY THIS WHEN EVER YOU ADD NEW DATA TO THIS CLASS
	void CopyObjects(CListCtrlItemInfo* pItemInfo);
	void CopyControls(CListCtrlItemInfo* pItemInfo);
	void CopyColors(CListCtrlItemInfo* pItemInfo);	

	LPARAM m_lParam;//why not use this like you use it in listbox, listctrl...
private:
	CHashList<int> m_controlType;//hmm
	CHashList<COLORREF> m_mapClf;//colors
	CHashList<COLORREF> m_mapTextClf;//colors
	CHashList<CString> m_listdata;//listbox
	CStringArray m_subItems;//col 0... N
	CStringArray m_subHelpItems;//col 0... N 
	CArray<BOOL,BOOL> m_arrItemReadOnly;
	BOOL m_bCheck;
	int m_iImage;
};


/////////////////////////////////////////////////////////////////////////////
//
// CSuperGridCtrl 
//
/////////////////////////////////////////////////////////////////////////////
struct SUPERGRIDCTRL_COLUMN{
	int width;
	CXhChar100 helpStr;
	CXhChar50 headerName;
	DWORD dwFlag;
	SUPERGRIDCTRL_COLUMN(){
		width=80;
		dwFlag=1;
	}
	SUPERGRIDCTRL_COLUMN(const char* name,int colWidth=80, DWORD flag=1, const char* promptStr=NULL){
		headerName.Copy(name);
		helpStr.Copy(promptStr);
		width=colWidth;
		dwFlag=flag;
	}
};
class CSuperGridCtrl : public CListCtrl
{
public:
	//the nested class 
	class CTreeItem : public CObject
	{
		CTreeItem(): m_pParent(NULL),m_bHideChildren(0),m_idProp(0),m_lpNodeInfo(NULL),
			m_nIndex(-1),m_nIndent(-1),m_bSetChildFlag(-1),m_dwPropGroup(1),m_bStrikeout(FALSE){};
		~CTreeItem();
		CObList m_listChild;
		CTreeItem* m_pParent;
		int m_nIndex; //CListCtrl index
		int m_nIndent; 
		int m_bSetChildFlag;
		friend class CSuperGridCtrl;
	public:
		CListCtrlItemInfo* m_lpNodeInfo;
		DWORD m_dwPropGroup;	//TreeItem所属分类 wht 10-11-29
		long m_idProp;
		BOOL m_bHideChildren;
		BOOL m_bStrikeout;		//添加删除线 wht 15-06-06
		int GetIndex() const	{return m_nIndex;}
		int GetChildCount() const {return (int)m_listChild.GetCount();}
		int GetTailIndex();
		void SetBkColor(COLORREF clr);
		int  InsertSubItemText(CSuperGridCtrl* pListCtrl,int nCol, LPCTSTR strSubItem="", BOOL bReadOnly=FALSE);
		BOOL DeleteSubItemText(CSuperGridCtrl* pListCtrl,int nCol);
	};
	typedef CSuperGridCtrl::CTreeItem* CSuperGridCtrlItemPtr;
// Construction
public:
	CSuperGridCtrl();
//protected:
	//读取剪切板中的数据
	typedef struct tagTABLE_DATA
	{
		int nMaxRow;
		int nMaxCol;
		CStringArray strArr;
		tagTABLE_DATA(){nMaxRow=nMaxCol=0;}
		CString GetData(int nRow,int nCol)
		{
			if(nRow<0||nCol<0||nRow>nMaxRow-1||nCol>nMaxCol-1)
				return _T("");
			int nCurIndex=nRow*nMaxCol+nCol;
			if(nCurIndex<strArr.GetSize())
				return strArr[nRow*nMaxCol+nCol];
			else
				return _T("");
		}
	}TABLE_DATA;
public:
	// Overrides
	//MUST override this to make a copy of CListCtrlItemInfo..see the CMySuperGrid.cpp for implementation
	//used in drag/drop operations
	virtual CListCtrlItemInfo* CopyData(CListCtrlItemInfo* lpSrc);
	//sets the icon state...called from within DrawItem returns valid image index
	//You MUST override this function to set your current icon, must of course be a valid CImageList index
	virtual int GetIcon(const CTreeItem* pItem);
	//override this to set the color for current cell
	virtual COLORREF GetCellRGB(void);
	//override this to update listview items, called from within OnEndlabeledit.
	virtual void OnUpdateListViewItem(CTreeItem* lpItem, LV_ITEM *plvItem);
	//override this to activate any control when lButtonDown in a cell, called from within OnLButtonDown
	virtual void OnControlLButtonDown(UINT nFlags, CPoint point, LVHITTESTINFO& ht);
	//override this to provide your dragimage, default: creates an image + text
	virtual CImageList *CreateDragImageEx(int nItem);
    //called before item is about to explode, return TRUE to continue, FALSE to prevent expanding
	virtual BOOL OnItemExpanding(CTreeItem *pItem, int iItem);
	//called after item has expanded
	virtual BOOL OnItemExpanded(CTreeItem* pItem, int iItem);
	//called before item are collapsed,return TRUE to continue, FALSE to prevent collapse
	virtual BOOL OnCollapsing(CTreeItem *pItem);
	//called after item has collapsed
	virtual BOOL OnItemCollapsed(CTreeItem *pItem);
	//called before item is about to be deleted,return TRUE to continue, FALSE to prevent delete item
	virtual BOOL OnDeleteItem(CTreeItem* pItem, int nIndex);
	//called before VK_MULTIPLY keydown, return TRUE to continue, FALSE to prevent expandall
	virtual BOOL OnVKMultiply(CTreeItem *pItem, int nIndex);
	//called before VK_SUBTRACT keydown, return TRUE to continue, FALSE to prevent collapse item
	virtual BOOL OnVkSubTract(CTreeItem *pItem, int nIndex);
	//called before VK_ADD keydown, return TRUE to continue, FALSE to prevent expanding item
	virtual BOOL OnVKAdd(CTreeItem *pItem, int nIndex);
	//called from PreTranslateMessage, override this to handle other controls than editctrl's
	virtual BOOL OnVkReturn(void);
	//called before from within OnlButtonDown and OnDblclk, but before anything happens, return TRUE to continue, FALSE to say not selecting the item
	virtual BOOL OnItemLButtonDown(LVHITTESTINFO& ht);
public:
	bool EnableSortItems(bool bCanSorted=true,bool bCanCustomDraw=false);
	// Inserts a column into a report-mode control.
	int _InsertColumn(int nCol, LPCTSTR lpszColumnHeading="",int nWidth=40);
	// Deletes a column from a report-mode control.
	BOOL _DeleteColumn(int nCol);
	void InternalUpdateItemText(CSuperGridCtrl::CTreeItem *pItem);
	void SetColumnHeaderTitle(int nCol, LPCTSTR lpszColumnHeading="");
	//creates a root
	CTreeItem*  InsertRootItem(CListCtrlItemInfo * lpRoot,BOOL bUpdate=TRUE);
	void DeleteRootItem(CTreeItem * lpRoot);
	BOOL IsRoot(CTreeItem * lpItem);
	CTreeItem* GetRootItem(int nIndex);
	int GetRootCount() { return (int)m_RootItems.GetCount();}
	BOOL DeleteAllItems();
	BOOL DeleteAllSonItems(CTreeItem *pSelItem);
	void MoveTreeItem(CTreeItem* pSelItem,int iInsertIndex);
	void MoveTreeItems(CTreeItem** ppSelItem,int nItemCount,int iInsertIndex);
	void RefreshItem(CTreeItem *pSelItem);
	//
	CTreeItem* GetPrev(CTreeItem* pRoot, CTreeItem *pItem, BOOL bInit = TRUE, BOOL bDontIncludeHidden=TRUE);
	CTreeItem* GetNext(CTreeItem* pRoot, CTreeItem* pNode, BOOL bInit = TRUE, BOOL bDontIncludeHidden=TRUE);
	int GetSelectedItem(void) const;
	//returns the itemdata associated with the supergrid
	CTreeItem* GetTreeItem(int nIndex);
	CTreeItem* FindTreeItemById(int idProp,CTreeItem *pParent=NULL);
	CTreeItem* FindItemByPropId(long prop_id,CTreeItem **ppParentItem);
	// Retrieves the number of items associated with the SuperGrid control.
	UINT GetCount(void);
	//returns number of children given the pItem node ptr.
	int NumChildren(const CTreeItem *pItem) const;
	//Determines if this tree item is a child of the specified parent
	BOOL IsChildOf(const CTreeItem* pParent, const CTreeItem* pChild) const;
	//hmm
	BOOL ItemHasChildren(const CTreeItem* pItem) const;
	// Use this to indicate that pItem has children, but has not been inserted yet.
	void SetChildrenFlag(CTreeItem *pItem,int nFlag) const;
	//return the Indent Level of pItem
	int GetIndent(const CTreeItem* pItem) const;
	//Sets the Indentlevel of pItem
	void SetIndent(CTreeItem *pItem, int iIndent);
	//get the pItems current listview index, 
	int GetCurIndex(const CTreeItem *pItem) const;
	//set pItems current listview index
	void SetCurIndex(CTreeItem* pItem, int nIndex);
	//sets the pItem' parent
	void SetParentItem(CTreeItem*pItem, CTreeItem* pParent);
	//gets pItems parent
	CTreeItem* GetParentItem(const CTreeItem* pItem); 
	//Insert item and return new parent node.
	//the bUpdate is here for performance reasons, when you insert say 100 node each having 10 children(1000 listview items)
	//the bUpdate should be set to FALSE(default) but when you want to insert an item, and you want to user to see it right away
	//set bUpdate to TRUE.(see the use of bUpdate in the HowToInsertItemsAfterTheGridHasBeenInitialized function in the CMySuperGridCtrl)
	CTreeItem* InsertItem(CTreeItem *pParent, CListCtrlItemInfo* lpInfo, int iInsertIndex=-1, bool bUpdate=false);
	//collapse all children from pItem
	void Collapse(CTreeItem *pItem);
	//expand one folder and return the last index of the expanded folder
	int Expand(CTreeItem* pSelItem, int nIndex);
	//expand all items from pItem and return last index of the expanded folder
	void ExpandAll(CTreeItem *pItem, int& nScroll);
	//expand all node in pItem and stop at pStopAt, used in SelectNode function
	void ExpandUntil(CTreeItem *pItem, CTreeItem* pStopAt);
	//use this if you want to select a node 
	//if the node is collapsed all items with in the node are expanded and the node is selected
	//it returns the listview index for the selected node
	//设置当前当前选中项，可设置当前活动单元格，并且支持显示当前活动单元格控件 wht 10-09-07
	int SelectItem(CTreeItem *pLocateNode,int iSubItem=0,BOOL bDisplayCellCtrl=FALSE,BOOL bExclusiveOtherItemSelectState=FALSE);
	//Delete an item in the listview 
	//takes the node to be delete and its listview item index as arg.
	//note the item you delete must be visible, hence the nItem as arg. 
	void DeleteItemEx(CTreeItem *pSelItem, int nItem);
	//returns the number of columns in the listview
	int GetNumCol(void);
	//does a Quicksort of the pParents children and if bSortChildren set, 
	//all items from pParent are sorted. 
	void Sort(CTreeItem* pParent, BOOL bSortChildren);
	// simpel wrapper for the CObList in CTreeItem, same usage as in the COblist
	POSITION GetHeadPosition(CTreeItem* pItem) const;
	POSITION GetTailPosition(CTreeItem *pItem) const;
	CTreeItem* GetNextChild(CTreeItem *pItem, POSITION& pos) const;
	CTreeItem* GetPrevChild(CTreeItem *pItem, POSITION& pos) const;
	void AddTail(CTreeItem *pParent, CTreeItem *pChild);
	//simpel wrapper for the CPtrList collection of rootitems
	//feel free to add more of these simple wrappers.
	POSITION GetRootHeadPosition(void) const;
	POSITION GetRootTailPosition(void) const;
	CTreeItem* GetNextRoot(POSITION& pos) const;
	CTreeItem* GetPrevRoot(POSITION& pos) const;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSuperGridCtrl)
	protected:
	//handle VK_xxxx
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
// Implementation
public:
	bool m_bFireOnNoneItemSelect;	//当没有任何项目选中时也调用FireItemChanged通知 wjh-2019.4.23;
	virtual ~CSuperGridCtrl();
protected:
	//given the rootptr it returns the rootindex.
	int GetRootIndex(CTreeItem * lpRoot);
	//delete pItem and all of its children
	BOOL Delete(CTreeItem *pItem, BOOL bClean=TRUE/*delete itemdata*/);
	//used in drag/drop operation..
	void CopyChildren(CTreeItem* pDest, CTreeItem* pSrc);
	//drag/drop thing....clipboard not supported!
	BOOL DoDragDrop(CTreeItem* pTarget, CTreeItem* pSelItem);
	//updates internal nodes, called when ever insert,delete on listview
	void InternaleUpdateTree(void);
	//Get ListView Index from pNode
	int NodeToIndex(CTreeItem *pNode);
	//see if user clicked the [+] [-] sign, also handles LVS_EX_CHECKBOXES.
	BOOL HitTestOnSign(CPoint point, LVHITTESTINFO& ht);
	int m_cxImage, m_cyImage;//icon size
	//translates column index value to a column order value.
	int IndexToOrder(int iIndex);
	//hmm
	int GetDragItem(void) const {return m_nDragItem; }
	int GetDropTargetItem(void) const {return m_nDragTarget; }
private:
	bool m_bEnableSort;	//指示该超级列表控件是否支持列排序功能
	BOOL m_bCustomDrawTitle;
	BOOL m_bImmerseMode;	//沉浸模式，除当前编辑行外其它行，灰色显示 wht 18-05-10
	COLORREF m_clrImmerse;	//沉浸色
	DWORD m_verticalAligment;	//文字竖直方向对齐方式，默认居中
	COLORREF m_clrGridLine;		//设置网格线颜色
	COLORREF m_clrEvenRow;		//偶数行背景颜色
	CSortHeaderCtrl m_ctrlHeader;
	//list of rootItems
	CPtrList m_RootItems;
	//internal helpers
	BOOL _Delete(CTreeItem* pStartAt, CTreeItem *pItem, BOOL bClean=TRUE/*delete itemdata*/);
	UINT _GetCount(CTreeItem* pItem, UINT& nCount);
	//hmm
	void DrawTreeItem(CDC* pDC, CTreeItem* pSelItem, int nListItem, const CRect& rcBounds);
	//makes the dot ... thing
	LPCTSTR MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset);
	//set the hideflag from pItem and all its children
	void HideChildren(CTreeItem *pItem, BOOL bHide, int iIndent);
	//checks whether a column is visible, if not scrolls to it
	void MakeColumnVisible(int nCol);
	//hmm
	void DrawFocusCell(CDC *pDC, int nItem, int iSubItem);
	int m_CurSubItem;//keyboard handling..it is what it says
	//hmm these represents the state of my expresso machine
    int m_nDragItem, m_nDragTarget;
    BOOL m_bIsDragging;
	//helper compare fn used with Quicksort
	static int CompareChildren(const void* p1, const void* p2);
	void CleanMe(CTreeItem *pItem);
	int _NodeToIndex(CTreeItem *pStartpos, CTreeItem* pNode, int& nIndex, BOOL binit = TRUE);
	CPen m_psTreeLine;
	CPen m_psRectangle;
	CPen m_psPlusMinus;
	CBrush m_brushErase;
	friend class CRectangle;
	HIMAGELIST m_himl;
// wht 10-05-20
private:
	BOOL m_bHasInputCtrlFocus;
	BOOL m_bDblclkDisplayCellCtrl;	//设置为双击显示编辑控件,默认为单击显示控件 wht 11-04-14
	CImageList m_imageList;
	CArray<SUPERGRIDCTRL_COLUMN,SUPERGRIDCTRL_COLUMN&> m_arrHeader;
	int  (*FireCompareItem)(const CSuperGridCtrlItemPtr& pItem1,const CSuperGridCtrlItemPtr& pItem2,DWORD lPara);
	BOOL (*FireColumnClick)(CSuperGridCtrl* pListCtrl,int iSubItem,bool bAscending);
	BOOL (*FireKeyDownItem)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,LV_KEYDOWN* pLVKeyDow);
	BOOL (*FireKeyUpItem)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,UINT nChar,UINT nRepCnt,UINT nFlags);
	BOOL (*FireEditBoxKeyUpItem)(CSuperEdit* pEdit,UINT nChar,UINT nRepCnt,UINT nFlags);
	BOOL (*FireDeleteItem)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem);
	BOOL (*FireContextMenu)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,CPoint point);
	BOOL (*FireButtonClick)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,int iSubItem);
	BOOL (*FireValueModify)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,int iSubItem,CString&);
	BOOL (*FireStatusModify)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,BOOL bHideChild);
	BOOL (*FireItemChanged)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,NM_LISTVIEW* pNMListView);
	BOOL (*FireItemUnselect)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,NM_LISTVIEW* pNMListView);
	BOOL (*FireLButtonDblclk)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,int iSubItem);
	BOOL (*FireLButtonDown)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,int iSubItem);
	BOOL (*FireMouseWheel)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,UINT nFlags, short zDelta, CPoint pt);
	BOOL (*FireScroll)(CSuperGridCtrl* pListCtrl,UINT nSBCode, UINT nPos, CScrollBar* pScrollBar,BOOL bHScroll);
	BOOL (*FireEndtrack)(CSuperGridCtrl* pListCtrl);
	void InternalInsertItem(CTreeItem *pParent, CTreeItem *pItem,BOOL bUpdate,int index=-1);
	void InternalAppendItem(CTreeItem *pParent, CTreeItem *pItem);
	CTreeItem* InternalFindItemByPropId(long prop_id,CTreeItem *pItem,CTreeItem **ppParentItem);
	void HideEditBox();	
	void HideCmbBox();
	BOOL WriteStrToClipboard(CString sValueText);
	BOOL ReadStrFromClipboard(TABLE_DATA &table_data);
	void SortItemsBySpecSubItem(int iSubItem,BOOL *pAscending=NULL);//按指定列升序或降序排列
public:
	void HideInputCtrl();
public:
	char m_cSplit;		//默认m_listdata中下拉框列表项的分隔符，默认为'|'
	int m_iPropGroup;	//当前显示的属性归属分类 wht 10-11-29
	HWND m_hPromptWnd;	//用于显示帮助字符串 wht 10-09-13
	BOOL m_bEscKeyDown;
	BOOL m_bPropValueModified;
	LV_ITEM m_curEditItem;
	CColorSelectComboBox m_cmbBox1,m_cmbBox2,m_cmbBox3;
	CSuperEdit m_editBox;
	CButton m_btnCtrl;
	CFont m_SSerif8Font;
	BOOL m_bDrag;	//drag col
	BOOL m_bCloseEditBoxImm;	//关闭文本框输入法 wht 18-06-30
	int AddColumnHeader(const char* name,int colWidth=80, DWORD flag=1, const char* helpStr=NULL){
		m_arrHeader.Add(SUPERGRIDCTRL_COLUMN(name,colWidth,flag,helpStr));
		m_ctrlHeader.m_titleArr.Add(name);
		return (int)m_arrHeader.GetSize()-1;
	}
	int GetColumnCount(){return (int)m_arrHeader.GetSize();}
	SUPERGRIDCTRL_COLUMN* GetColumnInfo(int i){return &m_arrHeader[i];}
	void EmptyColumnHeader(){m_arrHeader.RemoveAll();m_ctrlHeader.m_titleArr.RemoveAll();}
	void SelPage(int iPage);
	void SetImageWidth(int nWidth=0){m_cxImage=m_cyImage=nWidth;}	//设置缩进量 wht 11-04-22
	BOOL InitImageList(long nImageListID);
	void InitListCtrl(long* col_wide_arr=NULL,BOOL bSingleSel=TRUE,long* col_fmt_arr=NULL);
	void RefreshListViewHeader(long* col_wide_arr=NULL,long* col_fmt_arr=NULL);
	BOOL GetCellReadOnly(int iRow,int iCol);
	BOOL SetCellReadOnly(int iRow,int iCol,BOOL bReadOnly);
	int GetControlType(int iRow,int iCol);
	void SetSubItemText(CTreeItem *pItem,int iSubItem,LPCTSTR lpszText);
	void SetCompareItemFunc(int (*func)(const CSuperGridCtrlItemPtr& pItem1,const CSuperGridCtrlItemPtr& pItem2,DWORD lPara)){FireCompareItem = func;}
	void SetColumnClickFunc(int (*func)(CSuperGridCtrl* pListCtrl,int iSubItem,bool bAscending)){FireColumnClick = func;}
	void SetContextMenuFunc(BOOL (*func)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,CPoint point)){FireContextMenu = func;}
	void SetKeyDownItemFunc(BOOL (*func)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,LV_KEYDOWN* pLVKeyDow)){FireKeyDownItem = func;}
	void SetKeyUpItemFunc(BOOL (*func)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,UINT nChar,UINT nRepCnt,UINT nFlags)){FireKeyUpItem = func;}
	void SetEditBoxKeyUpItemFunc(BOOL (*func)(CSuperEdit* pEdit,UINT nChar,UINT nRepCnt,UINT nFlags)){FireEditBoxKeyUpItem = func;}
	void SetDeleteItemFunc(BOOL (*func)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem)){FireDeleteItem = func;}
	void SetButtonClickFunc(BOOL (*func)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,int iSubItem)){FireButtonClick = func;}
	void SetModifyValueFunc(BOOL (*func)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,int iSubItem,CString &valueStr)){FireValueModify = func;}
	void SetModifyStatusFunc(BOOL (*func)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,BOOL bHideChild)){FireStatusModify = func;}
	void SetItemChangedFunc(BOOL (*func)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,NM_LISTVIEW* pNMListView)){FireItemChanged = func;}
	void SetItemUnselectFunc(BOOL (*func)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,NM_LISTVIEW* pNMListView)){FireItemUnselect = func;}
	void SetLButtonDblclkFunc(BOOL (*func)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,int iSubItem)){FireLButtonDblclk = func;}
	void SetLButtonDownFunc(BOOL (*func)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,int iSubItem)){FireLButtonDown = func;}
	void SetMouseWheelFunc(BOOL (*func)(CSuperGridCtrl* pListCtrl,CTreeItem* pItem,UINT nFlags, short zDelta, CPoint pt)){FireMouseWheel = func;}
	void SetScrollFunc(BOOL (*func)(CSuperGridCtrl* pListCtrl,UINT nSBCode, UINT nPos, CScrollBar* pScrollBar,BOOL bHScroll)){FireScroll = func;}
	void SetEndtrackFunc(BOOL (*func)(CSuperGridCtrl* pListCtrl)){FireEndtrack = func;}
	BOOL CopyDataToClipboard();
	BOOL PasteDataFromClipboard(int nMaxColNum=0,BOOL *read_statu_arr=NULL,BOOL bExpandAll=FALSE,BOOL bStartFromSelCol=TRUE,BOOL bIgnore=FALSE);
	void Redraw();
	int GetCurSubItem(void) const {return m_CurSubItem;}
	void SetDblclkDisplayCellCtrl(BOOL bDblclkDispCtrl){m_bDblclkDisplayCellCtrl=bDblclkDispCtrl;}
	void SetListItemSplitChar(char split_char='|'){m_cSplit=split_char;}
	void DisplayCellCtrl(int iItem,int iCol,const char* cell_text=NULL);
	BOOL IsDisplayCellCtrl(int iItem,int iCol);
	BOOL SetImmerseMode(BOOL bImmerseMode,COLORREF immerseClr=-1);
	void AutoAdjustColumnWidth(int* ignoreColIndexArr=NULL,int arrSize=0);
	void KillfocusEditBox(){return OnKillfocusEditBox();}
	void SetTextVerticalAligment(DWORD vAligment);
	void SetGridLineColor(COLORREF clr);
	void SetEvenRowBackColor(COLORREF clr);
	// Gradient - 渐变系数，立体背景用,不用渐变设为0  
	void CSuperGridCtrl::SetHeaderBKColor(int R, int G, int B, int Gradient);
	void SetHeaderHeightCoef(double heightCoef);	//设置表头高度  
	void SetHeaderFontHW(int nHeight,int nWith);	//设置表头字体高度宽度
	void SetHeaderTextColor(COLORREF color);		//设置表头字体颜色
	void SetFontHW(int nHeight,int nWith);			//设置字体高度宽度
	void SetRowHeight(int nHeight);					//设置行高  
	// Generated message map functions
protected:
	//{{AFX_MSG(CSuperGridCtrl)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
#if defined(_WIN64)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
#else
	afx_msg void OnTimer(UINT nIDEvent);
#endif
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);	
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSysColorChange();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
protected:
	afx_msg void OnKillfocusCmbBox();
	afx_msg void OnKillfocusEditBox();
	afx_msg void OnKillfocusBtn();
	afx_msg void OnSelchangeCmbBox();
	afx_msg void OnChangeEditBox();
	afx_msg void OnButton();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult);
};

class CRectangle
{
public:
	CRectangle(CSuperGridCtrl* pCtrl, CDC* pDC, int iIndent, const CRect& rcBounds);
	~CRectangle();
	void DrawRectangle(CSuperGridCtrl *pCtrl);
	BOOL HitTest(CPoint pt);
	void DrawPlus(void);
	void DrawMinus(void);
	int GetLeft(void){return m_left;}
	int GetTop(void){return m_top;}
	CRect GetHitTestRect(void) { return CRect(m_left_top, m_right_bottom);}
private:
	CDC *m_pDC;
	SIZE m_right_bottom;
	int m_left;
	int m_top;
	POINT m_left_top;
	int m_topdown;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUPERGRIDCTRL_H__C6DF1701_806D_11D2_9A94_002018026B76__INCLUDED_)
