/************************************
  REVISION LOG ENTRY
  Revision By: Mihai Filimon
  Revised on 6/1/98 4:50:35 PM
  Comments: MyFD.cpp : implementation file
 ************************************/

#include "stdafx.h"
#include "folder_dialog.h"
#include "afxdlgs.h"
#include <DLGS.H>
#include <WINUSER.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFolderDialog

IMPLEMENT_DYNAMIC(CFolderDialog, CFileDialog)

WNDPROC CFolderDialog::m_wndProc = NULL;


// Function name	: CFolderDialog::CFolderDialog
// Description	    : Constructor
// Return type		: 
// Argument         : CString* pPath ; represent string where selected folder wil be saved
CFolderDialog::CFolderDialog(CString* pPath) : CFileDialog(TRUE, NULL, _T("*..*"))
{
	m_pPath = pPath;
}


BEGIN_MESSAGE_MAP(CFolderDialog, CFileDialog)
	//{{AFX_MSG_MAP(CFolderDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Function name	: WindowProcNew
// Description	    : Call this function when user navigate into CFileDialog.
// Return type		: LRESULT
// Argument         : HWND hwnd
// Argument         : UINT message
// Argument         : WPARAM wParam
// Argument         : LPARAM lParam
LRESULT CALLBACK WindowProcNew(HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam)
{
	
	if (message ==  WM_COMMAND)
		if (HIWORD(wParam) == BN_CLICKED)
			if (LOWORD(wParam) == IDOK)
			{
				if (CFileDialog* pDlg = (CFileDialog*)CWnd::FromHandle(hwnd))
				{
					TCHAR path[MAX_PATH];
					GetCurrentDirectory(MAX_PATH, path);
					*((CFolderDialog*)pDlg->GetDlgItem(0))->m_pPath = CString(path);
					pDlg->EndDialog(IDOK);
					return NULL;
				}
			}
	return CallWindowProc(CFolderDialog::m_wndProc, hwnd, message, wParam, lParam);
}

// Function name	: CFolderDialog::OnInitDone
// Description	    : For update the wiew of CFileDialog
// Return type		: void 
void CFolderDialog::OnInitDone()
{
	//旧版VC2003中文件名称是edt1控件，而最新VC2010中可能变为了cmb13，扩展名均为cmb1
	HideControl(cmb13);
	HideControl(edt1);
	HideControl(stc3);
	HideControl(cmb1);
	HideControl(stc2);
	CWnd* pFD = GetParent();
	CRect rectCancel; pFD->GetDlgItem(IDCANCEL)->GetWindowRect(rectCancel);
	pFD->ScreenToClient(rectCancel);
	CRect rectOK; pFD->GetDlgItem(IDOK)->GetWindowRect(rectOK);
	pFD->ScreenToClient(rectOK);
	pFD->GetDlgItem(IDOK)->SetWindowPos(0,rectCancel.left - rectOK.Width() - 4, rectCancel.top, 0,0, SWP_NOZORDER | SWP_NOSIZE);
	CRect rectList2; pFD->GetDlgItem(lst1)->GetWindowRect(rectList2);
	pFD->ScreenToClient(rectList2);
	pFD->GetDlgItem(lst1)->SetWindowPos(0,0,0,rectList2.Width(), abs(rectList2.top - (rectCancel.top - 4)), SWP_NOMOVE | SWP_NOZORDER);
	SetControlText(IDOK, _T("选择"));
	pFD->SetWindowText(_T("选择文件夹"));
	m_wndProc = (WNDPROC)SetWindowLong(pFD->m_hWnd, GWL_WNDPROC, (long)WindowProcNew);
}

BOOL IsWindows7OrGreater()
{
	//2000	0X0500   
	//XP	0X0501
	//2003	0X0502
	//VISTA 0X0600
	//Win7	0X0601
#ifdef _WIN64
    OSVERSIONINFO osVer;
    osVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
    if (!::GetVersionEx (&osVer)) 
    {
        return FALSE;
    }
	if(osVer.dwMajorVersion>6||((osVer.dwMajorVersion==6)&&osVer.dwMinorVersion>=1))
		return TRUE;
	else
		return FALSE;
#else
	int i=0,j=0;
	_asm
	{
		pushad
			mov ebx,fs:[0x18] ; get self pointer from TEB
			mov eax,fs:[0x30] ; get pointer to PEB / database
			mov ebx,[eax+0A8h] ; get OSMinorVersion
			mov eax,[eax+0A4h] ; get OSMajorVersion
			mov j,ebx
			mov i,eax
			popad
	}
	if(i>6||((i==6)&&j>=1))
		return TRUE;
	else
		return FALSE;
#endif
}
BOOL InvokeFolderPickerDlg(CString &sWorkFolder)
{
#ifdef __VS2010_
	if(!IsWindows7OrGreater())
#endif
	{	//Win7之前版本
		CFolderDialog dlg(&sWorkFolder);
		if(dlg.DoModal()!=IDOK)
			return FALSE;
	}
#ifdef __VS2010_
	else
	{	//Win7及以上版本
		CFolderPickerDialog dlg(sWorkFolder);
		if(dlg.DoModal()!=IDOK)
			return FALSE;
		sWorkFolder=dlg.GetPathName();
	}
#endif
	return TRUE;
}