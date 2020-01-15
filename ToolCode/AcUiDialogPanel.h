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
//----- PartListCtrlBar.h : Declaration of the CPartListCtrlBar
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
#include "acui.h"

#ifdef __SUPPORT_DOCK_UI_
//-----------------------------------------------------------------------------
class CAcUiDialogPanel : public CAcUiDockControlBar {
	DECLARE_DYNAMIC (CAcUiDialogPanel)
private:
	CLSID m_clsId;
	UINT m_idDialog;
	CDialog *m_pDialog;
	CRuntimeClass *m_pDlgRunClass;
public:
	CAcUiDialogPanel ();
	virtual ~CAcUiDialogPanel ();

	void Init(CRuntimeClass *pClass, UINT idDlg);
	CDialog *GetDlgPtr(){return m_pDialog;}
#if defined (_ARX_2007) && defined (__SUPPORT_DOCK_UI_) && !defined _UNICODE
	virtual BOOL Create (CWnd *pParent, LPCWSTR lpszTitle, int width = 250);
#else
	virtual BOOL Create(CWnd *pParent, LPCSTR lpszTitle, int width = 250);
#endif
	int ShowDialog();
	int CloseDialog();
	DECLARE_MESSAGE_MAP()
protected:
	virtual void SizeChanged (CRect *lpRect, BOOL bFloating, int flags) ;
	virtual bool OnClosing ();
#if defined (_ARX_2007) && defined (__SUPPORT_DOCK_UI_) && !defined _UNICODE
	afx_msg int OnCreate (LPCREATESTRUCTW lpCreateStruct) ;
#else
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
#endif
public:
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
} ;
#endif