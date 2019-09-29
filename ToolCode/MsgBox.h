#pragma once
// 定义一个钩子句柄
HHOOK g_hMsgBoxHook;
char BTN_ID_YES_TEXT[100]="是";
char BTN_ID_NO_TEXT[100]="否";
char BTN_ID_CANCEL_TEXT[100]="关闭";
char BTN_ID_OK_TEXT[100]="打开文件夹";
char BTN_ID_IGNORE_TEXT[100]="忽略";
char BTN_ID_RETRY_TEXT[100]="重试";

// 定义CBT钩子过程
LRESULT CALLBACK MsgBoxCBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	switch (nCode) {
	case HCBT_ACTIVATE:
		HWND hWnd = (HWND)wParam;
		SetDlgItemText(hWnd,IDYES,BTN_ID_YES_TEXT);
		SetDlgItemText(hWnd,IDNO,BTN_ID_NO_TEXT);
		SetDlgItemText(hWnd,IDCANCEL,BTN_ID_CANCEL_TEXT);
		SetDlgItemText(hWnd,IDOK,BTN_ID_OK_TEXT);
		SetDlgItemText(hWnd,IDRETRY,BTN_ID_RETRY_TEXT);
		SetDlgItemText(hWnd,IDIGNORE,BTN_ID_IGNORE_TEXT);
		return 0;
	}
	return CallNextHookEx(g_hMsgBoxHook, nCode, wParam, lParam);
}

int MsgBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	g_hMsgBoxHook = SetWindowsHookEx(WH_CBT, MsgBoxCBTProc, NULL, GetCurrentThreadId());
	int ret = MessageBox(hWnd, lpText, lpCaption, uType);
	UnhookWindowsHookEx(g_hMsgBoxHook);
	return ret;
}