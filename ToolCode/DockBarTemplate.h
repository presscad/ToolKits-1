#pragma once
#include "XhCharString.h"
#include "AcUiDialogPanel.h"

/*用法示例
struct UBOM_DLG_ID { static const int DLG_ID = CRevisionDlg::IDD; };
struct UBOM_NAME_ID { static const int NAME_ID = 0; }; //在资源中添加一个字符串
typedef CDlgDockBar<CRevisionDlg, UBOM_DLG_ID, UBOM_NAME_ID> CUBomDockBar;

extern CUBomDockBar g_ubomDocBar;
*/

template<class TYPE,class TYPE_ID,class TYPE_NAME_ID> class CDlgDockBar
{
	int m_idDialog;
	CXhChar50 m_sPageName;
#ifdef __SUPPORT_DOCK_UI_
	CAcUiDialogPanel *m_pDocBar;
#endif
	TYPE *m_pDlg;
public:
	CDlgDockBar(int DLG_IDD=0, const char* pageName=NULL)
	{
		Init(DLG_IDD, pageName);
	}
	~CDlgDockBar() { DestoryDockBar(); }
	
	void Init(int DLG_IDD, const char* pageName)
	{
		m_idDialog = DLG_IDD;
		m_sPageName.Copy(pageName);
	}
	
	TYPE* GetDlgPtr() 
	{ 
#ifdef __SUPPORT_DOCK_UI_
		if (m_pDocBar)
			return (TYPE*)m_pDocBar->GetDlgPtr();
		else
			return NULL;
#else
		return m_pDlg;
#endif
	}
	
	BOOL IsShowDockBar()
	{
#ifdef __SUPPORT_DOCK_UI_
		if (m_pDocBar == NULL)
			return FALSE;
		else
			return m_pDocBar->IsWindowVisible();
#else
		if (m_pDlg)
			return m_pDlg->IsWindowVisible();
		else
			return FALSE;
#endif
	}
	
	void DisplayDockBar(int width=200)
	{	//显示可停靠的窗体
		if (IsShowDockBar())
			return;
#ifdef __SUPPORT_DOCK_UI_
		CAcModuleResourceOverride resOverride;
		if (m_pDocBar == NULL)
		{
			m_pDocBar = new CAcUiDialogPanel();
			CString sPageName;
			sPageName.LoadString(TYPE_NAME_ID.NAME_ID);
			m_sPageName.Copy(sPageName);
			m_pDocBar->Init(RUNTIME_CLASS(TYPE), TYPE_ID.DLG_ID);
#if defined (_ARX_2007) && defined (__SUPPORT_DOCK_UI_) && !defined _UNICODE
			m_pDocBar->Create(acedGetAcadFrame(), (LPCWSTR)_bstr_t(m_sPageName), width);
#else
			m_pDocBar->Create(acedGetAcadFrame(), m_sPageName, width);
#endif
			m_pDocBar->SetWindowText(m_sPageName);// changes the text of the specified window's title bar (if it has one).
			m_pDocBar->EnableDocking(CBRS_ALIGN_ANY);//CBRS_ALIGN_ANY   Allows docking on any side of the client area.  		
		}
		else
			m_pDocBar->ShowDialog();

		acedGetAcadFrame()->FloatControlBar(m_pDocBar, CPoint(100, 100), CBRS_ALIGN_LEFT); //初始位置//CBRS_ALIGN_TOP   Orients the control bar vertically.
		acedGetAcadFrame()->DockControlBar(m_pDocBar);
		acedGetAcadFrame()->ShowControlBar(m_pDocBar, TRUE, TRUE);//void ShowControlBar( CControlBar* pBar, BOOL bShow, BOOL bDelay );
		acedGetAcadFrame()->RecalcLayout();
#else
		if (m_pDlg == NULL)
			m_pDlg = new TYPE();
		if (m_pDlg->GetSafeHwnd() == NULL)
			m_pDlg->CreateDlg();
		if (m_pDlg->GetSafeHwnd())
		{
			//m_pDlg->UpdatePartList();
			m_pDlg->ShowWindow(SW_SHOW);
		}
		else
		{
			//m_pDlg.DoModal();
		}
#endif
	}
	void HideDockBar() 
	{
#ifdef __SUPPORT_DOCK_UI_
		if (m_pDocBar)
			m_pDocBar->CloseDialog();
#else
		if(m_pDlg)
			m_pDlg->ShowWindow(SW_HIDE);
#endif
	}

	void DestoryDockBar()
	{
#ifdef __SUPPORT_DOCK_UI_
		if (m_pDocBar)
		{
			m_pDocBar->DestroyWindow();
			delete m_pDocBar;
			m_pDocBar = NULL;
		}
#else
		if (m_pDlg)
		{
			m_pDlg->DestroyWindow();
			delete m_pDlg;
			m_pDlg = NULL;
		}
#endif
	}
};
