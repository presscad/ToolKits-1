#pragma once
#ifndef __XHMENU_H_
#define __XHMENU_H_ 

#include "afxwin.h"

class CXhMenuData 
{ 
public: 
	CXhMenuData () { menuIconNormal = -1; menuIconSelected = -1; menuIconDisabled = -1;nID=0;}; 
	char menuText[32]; 
	UINT menuIconNormal; 
	UINT menuIconSelected; 
	UINT menuIconDisabled; 
	UINT nID; 
}; 

typedef enum {Normal,TextOnly} HIGHLIGHTSTYLE; 
class CXhMenu : public CMenu	 
{ 
	// Construction 
public: 
	CXhMenu();  
	// Attributes 
protected: 
	CTypedPtrArray<CPtrArray, CXhMenuData*> m_MenuList;	 
	//菜单项管理 
	CTypedPtrArray<CPtrArray, CXhMenu*>	m_SubMenus;	 
	//菜单管理 
public:  
	// ClassWizard generated virtual function overrides 
	//{{AFX_VIRTUAL() 
	//}}AFX_VIRTUAL  
	// Implementation 
public: 
	virtual ~CXhMenu();	// Virtual Destructor  
	// Drawing:  
	virtual void DrawItem( LPDRAWITEMSTRUCT);	 
	//自绘制函数 
	virtual void MeasureItem( LPMEASUREITEMSTRUCT ); 
	//菜单项大小调整 

	void SetTextColor (COLORREF ); 
	//设置文本颜色 
	void SetBackColor (COLORREF); 
	//设置菜单背景 
	void SetHighlightColor (COLORREF);	 
	//设置高亮色彩 
	void SetIconSize (int, int);	 
	//设置图标大小 
	void SetHighlightStyle (HIGHLIGHTSTYLE ); 
	//设置高亮风格 
	void SetHighlightTextColor (COLORREF);	 
	//设置高亮状态的文本色彩 

	virtual BOOL AppendODMenu(LPCTSTR lpstrText,  
		UINT nFlags = MF_OWNERDRAW, 
		UINT nID = 0, 
		UINT nIconNormal = -1,  
		UINT nIconSelected = -1, 
		UINT nIconDisabled = -1);	 
	//添加菜单项 
	virtual BOOL ModifyODMenu(LPCTSTR lpstrText, 
		UINT	nID = 0, 
		UINT	nIconNormal = -1, 
		UINT	nIconSelected = -1, 
		UINT	nIconDisabled = -1);	 
	//更改菜单项 
	virtual BOOL LoadMenu(LPCTSTR lpszResourceName); 
	//载入菜单 
	virtual BOOL LoadMenu(int nResource); 
	virtual BOOL DestroyMenu(); 

	// Generated message map functions 
protected: 
	COLORREF m_crText; 
	COLORREF m_clrBack; 
	COLORREF m_clrText; 
	COLORREF m_clrHilight; 
	COLORREF m_clrHilightText; 
	LOGFONT m_lf; 
	CFont m_fontMenu; 
	UINT m_iMenuHeight; 
	BOOL m_bLBtnDown; 
	CBrush m_brBackground,m_brSelect; 
	CPen m_penBack; 
	int m_iconX,m_iconY; 
	HIGHLIGHTSTYLE m_hilightStyle;  
	//{{AFX_MSG(CXhMenu) 
	// NOTE - the ClassWizard will add and remove member functions here. 
	//}}AFX_MSG 
};  
/////////////////////////////////////////////////////////////////////////// 
//  
//{{AFX_INSERT_LOCATION}} 
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.  
#endif // __XHMENU_H_ 