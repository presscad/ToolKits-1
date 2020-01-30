// JgNcDataManager.h: interface for the CJgNcDataManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JGNCDATAMANAGER_H__5A5F4482_9864_42A3_AE86_7BA1525A3662__INCLUDED_)
#define AFX_JGNCDATAMANAGER_H__5A5F4482_9864_42A3_AE86_7BA1525A3662__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "f_ent_list.h"
#include "Variant.h"
typedef struct tagHOLE_HEAD
{
	int d;
	char alias[10];
}HOLE_HEAD;
class CNCDirectoryFile
{
public:
	char DIR_FILE_NAME[MAX_PATH];
	BOOL m_bDirectoryPrintByASCII;
	ATOM_LIST<CVariant> contents;
	CNCDirectoryFile(){memset(DIR_FILE_NAME,0,MAX_PATH);m_bDirectoryPrintByASCII=FALSE;}
};
class CJgNcDataManager  
{
public:
	char extension[4];
	char sAbsolutePosMark[10],sRelativePosMark[10];
	char sQ235[10],sQ345[10],sQ355[10],sQ390[10],sQ420[10],sQ460[10];
	char sQ235BriefMark[10],sQ345BriefMark[10],sQ355BriefMark[10],sQ390BriefMark[10],sQ420BriefMark[10],sQ460BriefMark[10];
	BOOL m_bPrintByASCII;
	int STAMP_MAX_LEN;		//���ӡ(��ģ)����,mm
	int STAMP_START_LENPOS;	//��ӡ(��ģ)��ʼ����λ��,mm
	int STAMP_CLEARANCE;	//��ӡ(��ģ)����˨�׼����С��϶,mm
	char STAMP_IN_WING;		//��ӡ������֫��δ����ʱ������֫�����ʵ�λ��(��X����ʾ��X֫,��Y����ʾ��Y֫,����Ϊ˫֫����)
	BOOL m_bSpecialHoleOutputHoleD;	//��������׳�ͷ��Ϣʱֱ�������ֱ�� wht 17-05-18
	BOOL m_bSpecialHoleAutoDrillD;	//������Զ�������ӽ��ĳ�ͷ wht 19-09-13
	BOOL m_bUsePartLabelPrefix;		//�Ƿ����ü���ǰ׺ wht 19-10-18
	CJgNcDataManager();
	virtual ~CJgNcDataManager();
	BOOL InitJgNcDriver(const char *nc_driver);
	ATOM_LIST<CNCDirectoryFile>directory;
	ATOM_LIST<HOLE_HEAD>hole_head;
	ATOM_LIST<CVariant> print_list;
};

#endif // !defined(AFX_JGNCDATAMANAGER_H__5A5F4482_9864_42A3_AE86_7BA1525A3662__INCLUDED_)
