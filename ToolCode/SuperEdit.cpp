// SuperEdit.cpp : implementation file
//

#include "stdafx.h"
#include "SuperEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSuperEdit

CSuperEdit::CSuperEdit()
{
	FireKeyUpItem = NULL;
}

CSuperEdit::~CSuperEdit()
{
}


BEGIN_MESSAGE_MAP(CSuperEdit, CEdit)
	//{{AFX_MSG_MAP(CSuperEdit)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSuperEdit message handlers


BOOL CSuperEdit::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )	
	{		
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{			
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			GetParent()->SetFocus();
			return 1;
		}
		if ( (pMsg->wParam == _T('C') || pMsg->wParam == _T('X')   
			|| pMsg->wParam == _T('V')) &&   
			(::GetKeyState(VK_CONTROL) & 0x8000) )  
		{  
			::TranslateMessage(pMsg);  
			::DispatchMessage(pMsg);  
			return(TRUE);
		}
	}
	else if(pMsg->message==WM_KEYUP)
	{		
		if(FireKeyUpItem)
		{
			BOOL bRetCode=FireKeyUpItem(this,pMsg->wParam,0,0);
			if(bRetCode)
				return TRUE;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}
