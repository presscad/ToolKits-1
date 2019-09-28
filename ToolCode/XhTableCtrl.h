#pragma once

#include "XhTable.h"

class CXhTableCtrl : public CStatic//, public CFormatTbl
{
	DECLARE_DYNAMIC(CXhTableCtrl)
	
public:
	CXhTableCtrl();
	virtual ~CXhTableCtrl();
	CFormatTbl m_xTable;
protected:
	// Generated message map functions
protected:
	//{{AFX_MSG(CUserDefStaticText)
	afx_msg void OnPaint();
	//afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


