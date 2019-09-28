//--------------------------------------------------
// MenuButton.cpp : implementation file
//

#include "stdafx.h"
#include "afxwin.h"
#include "MenuButton.h"
#include "LogFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
void FillSolidRect(CDC * pDC, int x, int y, int cx, int cy, COLORREF clr)
{
	ASSERT(pDC != NULL);
	ASSERT(pDC->m_hDC != NULL);

	COLORREF oldColor = pDC->SetBkColor(clr);
	CRect rect(x, y, x + cx, y + cy);
	//CString szTitle;
	//szTitle="创建分段";
	pDC->ExtTextOut(0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	pDC->SetBkColor(oldColor);
}

void Draw3dRect(CDC * pDC, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	FillSolidRect(pDC, x, y, cx - 1, 1, clrTopLeft);
	FillSolidRect(pDC, x, y, 1, cy - 1, clrTopLeft);
	FillSolidRect(pDC, x + cx, y, -1, cy, clrBottomRight);
	FillSolidRect(pDC, x, y + cy, cx, -1, clrBottomRight);
}

void Draw3dRect(CDC * pDC, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	Draw3dRect(pDC, lpRect->left, lpRect->top, lpRect->right - lpRect->left, 
		lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
}
/////////////////////////////////////////////////////////////////////////////
// CMenuButton

CMenuButton::CMenuButton()
{
	m_bLeftBtnDown 	= m_bRightBtnDown	= FALSE;
	m_bLeftBtnOut 	= FALSE;
	m_liCurrWorkItem = 0;
	FireClickComboMenu=NULL;
}

CMenuButton::~CMenuButton()
{
}


BEGIN_MESSAGE_MAP(CMenuButton, CButton)
	//{{AFX_MSG_MAP(CMenuButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMenuButton::PreSubclassWindow()
{
	SetButtonStyle(this->GetButtonStyle()|BS_OWNERDRAW);
}
/////////////////////////////////////////////////////////////////////////////
// CMenuButton message handlers
DWORD CMenuButton::DoSingleClick()
{
    CRect rect;
    GetWindowRect(rect);
    POINT point;
    point.x = rect.left;
    point.y = rect.bottom;
	if(FireClickComboMenu!=NULL)
		return FireClickComboMenu(GetParent(),point);
	else
		return 0;
    // -[Feral]-----------------------------------------------------------
    // Vars and Assignment
    /*CMenu menu;
    int iMenuID = 0;
    DWORD dwSelectionMade;
    VERIFY(menu.LoadMenu(uiIDMenuResource) );

    CMenu *pmenuPopup = menu.GetSubMenu(iMenuID);
    ASSERT(pmenuPopup != NULL);

    // -[Feral]-----------------------------------------------------------
    // show and track the menu
    dwSelectionMade = pmenuPopup->TrackPopupMenu( (TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD),
                                                  point.x, point.y, this
                                                );
    // -[Feral]-----------------------------------------------------------
    // Clean Up and Return
    pmenuPopup->DestroyMenu();
    // The value of dwSelectionMade is the id of the command selected or 0 if no selection was made
    return dwSelectionMade;*/
}
void CMenuButton::OnLButtonDown(UINT nFlags, CPoint point) 
{         
	CRect rect;
	GetClientRect(&rect);
	CRect rcLeft(rect.left, rect.top, rect.right - 15, rect.bottom);
	CRect rcRight(rect.right - 15, rect.top, rect.right, rect.bottom);
	
	if (rcLeft.PtInRect(point))
	{             
		m_bLeftBtnDown	= TRUE;
		m_bRightBtnDown = FALSE;
	}
	else if (rcRight.PtInRect(point))
	{
		m_bRightBtnDown ^= 1;
		DoSingleClick();
	}
	else              
		m_bLeftBtnDown 	= m_bRightBtnDown = FALSE;
  
  	GetWindowRect(rect);
	//m_wndColor.SetWindowPos(NULL, rect.left, rect.bottom, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	//m_wndColor.SetSelectedItem(m_iRealItem);
	//m_wndColor.ShowWindow(m_bRightBtnDown ? SW_SHOW : SW_HIDE);

	Invalidate(FALSE);
	UpdateWindow();

	SetCapture();
 	
	CWnd::OnLButtonDown(nFlags, point);
}

void CMenuButton::OnLButtonUp(UINT nFlags, CPoint point) 
{       
    ReleaseCapture();

	if (m_bLeftBtnDown)
		RevokeComboMenuItemSelected(true);//, m_iRealItem);
	m_bLeftBtnDown = FALSE;
	Invalidate(FALSE);
	UpdateWindow();
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CMenuButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	BOOL bOut = m_bLeftBtnOut;
	CRect rect;
	GetClientRect(&rect);
	CRect rcLeft(rect.left, rect.top, rect.right - 15, rect.bottom);
	
	if (rcLeft.PtInRect(point))
		m_bLeftBtnOut	= FALSE;
	else
		m_bLeftBtnOut 	= TRUE;

	if (bOut != m_bLeftBtnOut)
	{
		Invalidate(FALSE);
		UpdateWindow();
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

void CMenuButton::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	DrawButtonAspect(&dc);
}
void CMenuButton::DrawButtonAspect(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	CRect rcLeft(rect.left, rect.top, rect.right - 15, rect.bottom-1);
	CRect rcRight(rect.right - 15, rect.top, rect.right - 1, rect.bottom-1);

	CDC memDC;
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	memDC.CreateCompatibleDC(pDC);
	CBitmap * pOldBitmap = memDC.SelectObject(&bitmap);

	CBrush brBack(GetSysColor(COLOR_BTNFACE));
	memDC.FillRect(rect, &brBack);
	memDC.SetBkColor(GetSysColor(COLOR_BTNFACE));

	Draw3dRect(&memDC, &rect, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW)); 

	CRect rcImage(rcLeft.left+1, rcLeft.top+1, rcLeft.right-1, rcLeft.bottom-1);
	if (!m_bLeftBtnDown || m_bLeftBtnOut) // Draw Button Up
		Draw3dRect(&memDC, &rcLeft, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW)); 
	else
	{
		rcLeft.SetRect(rcLeft.left, rcLeft.top, rcLeft.right+1, rcLeft.bottom+1);
		Draw3dRect(&memDC, &rcLeft, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNFACE)); 
		rcImage.OffsetRect(1, 1);
	}
	CString szButtonName;
	GetWindowText(szButtonName);
	//设定字体
	LOGFONT logfont;
	memset(&logfont, 0, sizeof(logfont));
	// 8 point height Sans Serif font
	logfont.lfHeight = -MulDiv(8, memDC.GetDeviceCaps(LOGPIXELSY), 72);
	logfont.lfWeight = FW_NORMAL;
	logfont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	static char BASED_CODE szFaceName[] = "MS Sans Serif";
	//TODO:lstrcpy(logfont.lfFaceName, szFaceName);  wjh
	CFont font;
	font.CreateFontIndirect(&logfont);
	CFont* pOldFont=memDC.SelectObject(&font);
	memDC.TextOut(5,5,szButtonName);
	memDC.SelectObject(pOldFont);
	font.DeleteObject();
	//COLORREF oldColor = memDC.SetBkColor(clr);
	//CRect rect(x, y, x + cx, y + cy);
	//CString szTitle;
	//szTitle="创建分段";
	//pDC->ExtTextOut(0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	//pDC->SetBkColor(oldColor);

	//m_wndColor.DrawBtnFace(&memDC, rcImage, m_liCurrWorkItem);

	if (!m_bRightBtnDown) // Draw Button Up
		Draw3dRect(&memDC, &rcRight, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW)); 
	else
	{
		rcRight.SetRect(rcRight.left, rcRight.top, rcRight.right+1, rcRight.bottom+1);
		Draw3dRect(&memDC, &rcRight, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNFACE)); 
		rcRight.OffsetRect(1, 1);
	}
	
	DrawArrow(&memDC, &rcRight);	
    
    if (!IsWindowEnabled())
		DrawDisabled(&memDC, rect);

	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, rect.left, rect.top, SRCCOPY);
	memDC.SelectObject(pOldBitmap);

	brBack.DeleteObject();
	bitmap.DeleteObject();
}

void CMenuButton::NotifyComboMenuClicked(UINT idCmdMenuItem,const char* szCmdMenuName)
{
	m_liCurrWorkItem=idCmdMenuItem;
	if(strlen(szCmdMenuName)>0)
		this->SetWindowText(CString(szCmdMenuName));
	m_bLeftBtnDown 	= FALSE;
	m_bRightBtnDown = FALSE;
	Invalidate(FALSE);
	UpdateWindow();
}
void CMenuButton::RevokeComboMenuItemSelected(bool revokeCmdExecute/*=false*/)
{                  
	CWnd* pParent=GetParent();
	if(revokeCmdExecute && m_liCurrWorkItem > 0)
	{                
		if (pParent->IsKindOf(RUNTIME_CLASS(CDialog)))
			pParent->SendMessage(WM_COMMAND,m_liCurrWorkItem,0);//WM_USER+21, (WPARAM)m_nID, (LPARAM)m_liCurrWorkItem);
		else	
			pParent->SendMessage(WM_COMMAND,m_liCurrWorkItem,0);//WM_USER+21, (WPARAM)m_nID, (LPARAM)m_liCurrWorkItem);
	}
}

void CMenuButton::DrawDisabled( CDC * pDC, const CRect & rc )
{
	// create a monochrome memory DC
	CDC memDC;
	memDC.CreateCompatibleDC(0);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&memDC, rc.Width(), rc.Height());
	CBitmap * pOldBmp = memDC.SelectObject(&bmp);
	
	// build a mask
	memDC.PatBlt(0, 0, rc.Width(), rc.Height(), WHITENESS);
	pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
	memDC.BitBlt(0, 0, rc.Width(), rc.Height(), pDC, rc.left, rc.top, SRCCOPY);
	pDC->SetBkColor(GetSysColor(COLOR_BTNHIGHLIGHT));
	memDC.BitBlt(0, 0, rc.Width(), rc.Height(), pDC, rc.left, rc.top, SRCPAINT);

	// Copy the image from the toolbar into the memory DC
	// and draw it (grayed) back into the toolbar.
	CBrush 	brShadow(GetSysColor(COLOR_BTNSHADOW)), 
			brHilight(GetSysColor(COLOR_BTNHIGHLIGHT)), 
			brFace(GetSysColor(COLOR_BTNFACE));
	
//	pDC->FillRect(rc, &brFace);
	pDC->SetBkColor(RGB(0, 0, 0));
	pDC->SetTextColor(RGB(255, 255, 255));


	CBrush * pOldBrush = pDC->SelectObject(&brHilight);
	pDC->BitBlt(rc.left+1, rc.top+1, rc.Width(), rc.Height(), &memDC, 0, 0, 0x00E20746L);
	pDC->SelectObject(&brShadow);
	pDC->BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &memDC, 0, 0, 0x00E20746L);
	
	// reset DCs
	pDC->SelectObject(pOldBrush);
	memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();

	brShadow.DeleteObject();
	brHilight.DeleteObject();
	brFace.DeleteObject();
	bmp.DeleteObject();
}

void CMenuButton::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);
	
	Invalidate(FALSE);
	UpdateWindow();
}

void CMenuButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    // -[Feral]-----------------------------------------------------------
    // VALIDATION:
    ASSERT(lpDrawItemStruct != NULL);

    // -[Feral]-----------------------------------------------------------
    // Vars and Assignment
    CRect rect      = lpDrawItemStruct->rcItem;
    CDC *pDC        = CDC::FromHandle(lpDrawItemStruct->hDC);
    UINT uiState    = lpDrawItemStruct->itemState;
	DrawButtonAspect(pDC);
	return;	//保证系统默认的后台按钮绘制函数与OnPaint一致 wjh-2019.4.15
    CPen pen;
    CPen *ppenOld   = NULL;

    // -[Feral]-----------------------------------------------------------
    // set the pen color based on if we are disabled or not
    if( (uiState&ODS_DISABLED) )
    {
        pen.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_GRAYTEXT) );
    }
    else
    {
        pen.CreatePen(PS_SOLID, 0, ::GetSysColor(COLOR_BTNTEXT) );
    }


    // -[Feral]-----------------------------------------------------------
    // select the pen into the DC.
    ppenOld = pDC->SelectObject(&pen);


    // -[Feral]-----------------------------------------------------------
    // draw the border
    if( (uiState&ODS_SELECTED) )
    {
        pDC->DrawFrameControl(rect, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_PUSHED);
    }
    else
    {
        pDC->DrawFrameControl(rect, DFC_BUTTON, DFCS_BUTTONPUSH);
    }


    // -[Feral]-----------------------------------------------------------
    // Draw the Arrow...
    // Something like:
    // X
    // XX
    // XXX
    // XXXX
    // XXX
    // XX
    // X
    // In the Center of the button.
    CSize sizeArrow(4,7);
    CSize sizeOffsetTop(1,+1);          // size of the top stairsteps
    CSize sizeOffsetBottom(1,-1);       // size of the bottom stairsteps
    CPoint pointCenter( (rect.Width()/2), (rect.Height()/2) );
    CPoint pointStart( (pointCenter.x-(sizeArrow.cx/2) ), (pointCenter.y-(sizeArrow.cy/2) ) );
    CPoint pointStop ( (pointStart.x), (pointStart.y+sizeArrow.cy) );
    // -[Feral]-----------------------------------------------------------
    // start at the left and work to the right...
    for(int iInd=0; iInd<sizeArrow.cx; iInd++)
    {
        // -[Feral]-------------------------------------------------------
        // draw the lines
        pDC->MoveTo(pointStart);
        pDC->LineTo(pointStop);

        // -[Feral]-------------------------------------------------------
        // offset our points. (going right!)
        pointStart.Offset(sizeOffsetTop);
        pointStop .Offset(sizeOffsetBottom);
    }

    // -[Feral]-----------------------------------------------------------
    // Draw the focus rectangle if necessary.
    if( (uiState&ODS_FOCUS) )
    {
        CRect rectFocus(rect);
        rectFocus.DeflateRect(3,3);     // This looked prety close
        pDC->DrawFocusRect(rectFocus);
    }

    // -[Feral]-----------------------------------------------------------
    // Clean Up and Return
    pDC->SelectObject(ppenOld);         // Restore the pen
}

void CMenuButton::DrawArrow(CDC * pDC, CRect * pRect)
{
	ASSERT(pRect);

	CPoint ptCenter(pRect->left + (pRect->Width() / 2), pRect->top + (pRect->Height() / 2));

	CPoint ptArrow[7];
	ptArrow[0].x = ptCenter.x-1;
	ptArrow[0].y = ptCenter.y-4;
	
	ptArrow[1].x = ptCenter.x-1;
	ptArrow[1].y = ptCenter.y-1;

	ptArrow[2].x = ptCenter.x-3;
	ptArrow[2].y = ptCenter.y-1;

	ptArrow[3].x = ptCenter.x;
	ptArrow[3].y = ptCenter.y+2;

	ptArrow[4].x = ptCenter.x+3;
	ptArrow[4].y = ptCenter.y-1;

	ptArrow[5].x = ptCenter.x+1;
	ptArrow[5].y = ptCenter.y-1;

	ptArrow[6].x = ptCenter.x+1;
	ptArrow[6].y = ptCenter.y-4;

	CBrush brBlack(RGB(0, 0, 0));
  	CBrush * pOldBrush = pDC->SelectObject(&brBlack);
	pDC->Polygon(ptArrow, 7);  
	pDC->MoveTo(ptCenter.x - 3, ptCenter.y + 4);
	pDC->LineTo(ptCenter.x + 4, ptCenter.y + 4);
  	pDC->SelectObject(pOldBrush);
	brBlack.DeleteObject();
}

//
///EOF
