#include <stdafx.h>
#include "MsgBox.h"

// ����һ�����Ӿ��
HHOOK g_hMsgBoxHook;
CXhChar100 BTN_ID_YES_TEXT="��";
CXhChar100 BTN_ID_NO_TEXT="��";
CXhChar100 BTN_ID_CANCEL_TEXT="�ر�";
CXhChar100 BTN_ID_OK_TEXT="���ļ���";
CXhChar100 BTN_ID_IGNORE_TEXT="����";
CXhChar100 BTN_ID_RETRY_TEXT="����";

// ����CBT���ӹ���
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