#pragma once
#include "XhCharString.h"
// ����һ�����Ӿ��
extern HHOOK g_hMsgBoxHook;
extern CXhChar100 BTN_ID_YES_TEXT;		//[100]="��";
extern CXhChar100 BTN_ID_NO_TEXT;		//[100]="��";
extern CXhChar100 BTN_ID_CANCEL_TEXT;	//[100]="�ر�";
extern CXhChar100 BTN_ID_OK_TEXT;		//[100]="���ļ���";
extern CXhChar100 BTN_ID_IGNORE_TEXT;	//[100]="����";
extern CXhChar100 BTN_ID_RETRY_TEXT;	//[100]="����";

// ����CBT���ӹ���
LRESULT CALLBACK MsgBoxCBTProc(int nCode, WPARAM wParam, LPARAM lParam);
int MsgBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
