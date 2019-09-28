#ifndef SORTHEADERCTRL_H
#define SORTHEADERCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSortHeaderCtrl : public CHeaderCtrl
{
// Construction
public:
	int m_R;    
	int m_G;    
	int m_B;    
	int m_Gradient; // 画立体背景，渐变系数    
	double m_heightCoef;  //表头高度，这是倍数,    
	int m_fontHeight; //字体高度    
	int m_fontWidth;   //字体宽度    
	COLORREF m_colorText;
	CStringArray m_titleArr;    
	CString m_alignmentFormat; //表示对齐类型的整型数组,0表示左对齐，1表示中间对齐，2表示右对齐
	BOOL m_bEnableSort;
	BOOL m_bCustomDrawTitle;
public:
	CSortHeaderCtrl();

// Attributes
public:

// Operations
public:
	int GetCurrSortColumn(){return m_iSortColumn;}
	BOOL IsSortCurrColAscending(){return m_bSortAscending;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortHeaderCtrl)
	public:
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSortHeaderCtrl();

	void SetSortArrow( const int iColumn, const BOOL bAscending );

	// Generated message map functions
protected:
	afx_msg void OnPaint();    
	void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	LRESULT OnLayout( WPARAM wParam, LPARAM lParam );
	int m_iSortColumn;
	BOOL m_bSortAscending;

	//{{AFX_MSG(CSortHeaderCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // SORTHEADERCTRL_H
