// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- PartListCtrlBar.cpp : Implementation of CPartListCtrlBar
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//#include "resource.h"
#include "AcUiDialogPanel.h"

#ifdef __SUPPORT_DOCK_UI_

#if defined (_ARX_2007) && defined (__SUPPORT_DOCK_UI_) && !defined _UNICODE
#define ON_WM_CREATE_ARX2007() \
	{ WM_CREATE, 0, 0, 0, AfxSig_is, \
		(AFX_PMSG) (AFX_PMSGW) \
		(static_cast< int (AFX_MSG_CALL CWnd::*)(LPCREATESTRUCTW) > ( &ThisClass :: OnCreate)) },
#else
#define ON_WM_CREATE_ARX2007() ON_WM_CREATE()
#endif

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC (CAcUiDialogPanel, CAcUiDockControlBar)


BEGIN_MESSAGE_MAP(CAcUiDialogPanel, CAcUiDockControlBar)
	ON_WM_CREATE_ARX2007()
	ON_WM_NCCALCSIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
//----- CPartListCtrlBar *pInstance = new CPartListCtrlBar;
//----- pInstance->Create (acedGetAcadFrame (), "My title bar") ;
//----- pInstance->EnableDocking (CBRS_ALIGN_ANY) ;
//----- pInstance->RestoreControlBar () ;

//-----------------------------------------------------------------------------
static CLSID clsCPartListCtrlBar = {0xeab78c04, 0x2194, 0x47ad, {0xa4, 0xf2, 0xad, 0xca, 0x3e, 0x3b, 0xb6, 0x3c}} ;


//-----------------------------------------------------------------------------
CAcUiDialogPanel::CAcUiDialogPanel () : CAcUiDockControlBar() {
	m_pDialog = NULL;
	::CoCreateGuid(&m_clsId);
}

//-----------------------------------------------------------------------------
CAcUiDialogPanel::~CAcUiDialogPanel () {
}

//-----------------------------------------------------------------------------
#ifdef _DEBUG
//- Please uncomment the 2 following lines to avoid linker error when compiling
//- in release mode. But make sure to uncomment these lines only once per project
//- if you derive multiple times from CAdUiDockControlBar/CAcUiDockControlBar
//- classes.

//void CAdUiDockControlBar::AssertValid () const {
//}
#endif

void CAcUiDialogPanel::Init(CRuntimeClass *pRunClass, UINT idDlg)
{
	m_pDlgRunClass=pRunClass;
	m_idDialog=idDlg;
}

//-----------------------------------------------------------------------------
#if defined (_ARX_2007) && defined (__SUPPORT_DOCK_UI_) && !defined _UNICODE
BOOL CAcUiDialogPanel::Create (CWnd *pParent, LPCWSTR lpszTitle, int width/*=250*/)
#else
BOOL CAcUiDialogPanel::Create(CWnd *pParent, LPCSTR lpszTitle, int width/*=250*/)
#endif
{
	CString strWndClass ;
	strWndClass =AfxRegisterWndClass (CS_DBLCLKS, LoadCursor (NULL, IDC_ARROW)) ;
#if defined (_ARX_2007) && defined (__SUPPORT_DOCK_UI_) && !defined _UNICODE
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = width;
	rect.bottom = 200;
	if (!CAcUiDockControlBar::Create(
		_bstr_t(strWndClass), lpszTitle, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
		rect, pParent, 0
		)
	)
#else
	CRect rect(0, 0, width, 200);
	if (!CAcUiDockControlBar::Create(
		_bstr_t(strWndClass), lpszTitle, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
		rect, pParent, 0
	)
		)
#endif
		return (FALSE) ;
	
	SetToolID (&m_clsId) ;

	// TODO: Add your code here
	
	return (TRUE) ;
}

//-----------------------------------------------------------------------------
//----- This member function is called when an application requests the window be 
//----- created by calling the Create or CreateEx member function
#if defined (_ARX_2007) && defined (__SUPPORT_DOCK_UI_) && !defined _UNICODE
int CAcUiDialogPanel::OnCreate(LPCREATESTRUCTW lpCreateStruct) {
#else
int CAcUiDialogPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) {
#endif
	if ( CAcUiDockControlBar::OnCreate (lpCreateStruct) == -1 )
		return (-1) ;
	//显示子窗口
	ASSERT(m_idDialog>0&&m_pDlgRunClass);
	m_pDialog = (CDialog*)m_pDlgRunClass->CreateObject();
	if(m_pDialog!=NULL)
	{
		m_pDialog->Create(m_idDialog,this);
		m_pDialog->ShowWindow(SW_SHOW);
	}
	return (0) ;
}
int CAcUiDialogPanel::ShowDialog()
{	//显示子窗口
	if (m_pDialog == NULL && m_idDialog > 0 && m_pDlgRunClass)
	{
		m_pDialog = (CDialog*)m_pDlgRunClass->CreateObject();
		if (m_pDialog != NULL)
		{
			m_pDialog->Create(m_idDialog, this);
			m_pDialog->ShowWindow(SW_SHOW);
		}
	}
	else if (m_pDialog)
		m_pDialog->ShowWindow(SW_SHOW);
	return (0);
}
int CAcUiDialogPanel::CloseDialog()
{
	OnClosing();
	return (0);
}
//-----------------------------------------------------------------------------
void CAcUiDialogPanel::SizeChanged (CRect *lpRect, BOOL bFloating, int flags) 
{
	CAcModuleResourceOverride resourceOverride;
	if (m_pDialog != NULL)	
	{		
		//m_pDialog->SetWindowPos(this, lpRect->left, lpRect->top,
		//	lpRect->Width(),lpRect->Height(), SWP_NOZORDER);//SWP_NOZORDER:Retains the current Z order
		CRect rect;
		GetClientRect(rect);
		//标题高
		int height=::GetSystemMetrics(SM_CYCAPTION);
		//rect.top+=height;	//50;
		m_pDialog->MoveWindow(rect);
	}
}

bool CAcUiDialogPanel::OnClosing()
{
	if(m_pDialog)
	{
		m_pDialog->DestroyWindow();
		delete m_pDialog;
		m_pDialog=NULL;
	}
	return CAcUiDockControlBar::OnClosing();
}

void CAcUiDialogPanel::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CAcUiDockControlBar::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void CAcUiDialogPanel::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if(pWnd&&pWnd->GetSafeHwnd()!=NULL)
		::SendMessage(pWnd->GetSafeHwnd(),WM_CONTEXTMENU,NULL,NULL);
}
#endif
