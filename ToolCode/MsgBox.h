#pragma once
#include "XhCharString.h"
// 定义一个钩子句柄
extern HHOOK g_hMsgBoxHook;
extern CXhChar100 BTN_ID_YES_TEXT;		//[100]="是";
extern CXhChar100 BTN_ID_NO_TEXT;		//[100]="否";
extern CXhChar100 BTN_ID_CANCEL_TEXT;	//[100]="关闭";
extern CXhChar100 BTN_ID_OK_TEXT;		//[100]="打开文件夹";
extern CXhChar100 BTN_ID_IGNORE_TEXT;	//[100]="忽略";
extern CXhChar100 BTN_ID_RETRY_TEXT;	//[100]="重试";

// 定义CBT钩子过程
LRESULT CALLBACK MsgBoxCBTProc(int nCode, WPARAM wParam, LPARAM lParam);
int MsgBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
