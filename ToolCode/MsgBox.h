#pragma once
#ifndef __MSG_BOX_INC____
#define __MSG_BOX_INC____
// 定义一个钩子句柄
extern HHOOK g_hMsgBoxHook;
extern char BTN_ID_YES_TEXT[100];
extern char BTN_ID_NO_TEXT[100];
extern char BTN_ID_CANCEL_TEXT[100];
extern char BTN_ID_OK_TEXT[100];
extern char BTN_ID_IGNORE_TEXT[100];
extern char BTN_ID_RETRY_TEXT[100];

// 定义CBT钩子过程
LRESULT CALLBACK MsgBoxCBTProc(int nCode, WPARAM wParam, LPARAM lParam);
int MsgBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
#endif