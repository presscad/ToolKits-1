// XhCharString.h: interface for the CXhCharString class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stdio.h"
#include "XhCharString.h"
#include "list.h"
#include "objptr_list.h"

struct CATLOG_ITEM{
protected:
	unsigned int id;
	unsigned int idRelaObject;
public:
	BYTE ciType;	//0.目录项的类别分组，用于分类管理不同树控件的目录项
	unsigned int idParentCatg;	//上一级目录项
	CXhChar50 name;
	CATLOG_ITEM(){id=idParentCatg=idRelaObject=ciType=0;}
	void SetKey(unsigned int key){id=key;}
	unsigned int get_Id(){return id;}
	__declspec(property(get=get_Id)) unsigned int Id;
	unsigned int set_idRelaObj(unsigned int _idRelaObj){return idRelaObject=_idRelaObj;}
	unsigned int get_idRelaObj(){return idRelaObject;}
	__declspec(property(put=set_idRelaObj,get=get_idRelaObj)) unsigned int idRelaObj;
};

class CATLOG_TREEITEM{
protected:
	CATLOG_ITEM *pRelaCatlog;
	CXhPtrList<CATLOG_TREEITEM> hashSonItems;
public:
	static IXhList<CATLOG_ITEM>* pBelongCatlogLibary;
	CATLOG_ITEM* get_pCatlog(){return pRelaCatlog;}
	CATLOG_ITEM* set_pCatlog(CATLOG_ITEM* _pRelaCatlog){return pRelaCatlog=_pRelaCatlog;}
	__declspec(property(put=set_pCatlog,get=get_pCatlog)) CATLOG_ITEM* pCatlog;
public:
	CATLOG_TREEITEM(CATLOG_ITEM *pCatlog=NULL);
	CATLOG_TREEITEM* AppendSonItem(const char* itemname,int idCatgType=0,DWORD id=0);
	CATLOG_TREEITEM* MoveSonItemFirst();
	CATLOG_TREEITEM* MoveSonItemNext();
	void Empty() { hashSonItems.Empty(); }
	bool DeleteCursor(bool blClean=false);
	int GetSonItemCount();
};
