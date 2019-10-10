#include "stdafx.h"
#include "Catlog.h"

////////////////////////////////////////////////////////////////////////////////
//CATLOG_TREEITEM
IXhList<CATLOG_ITEM>* CATLOG_TREEITEM::pBelongCatlogLibary=NULL;
CATLOG_TREEITEM::CATLOG_TREEITEM(CATLOG_ITEM *pCatlog/*=NULL*/)
{
	pRelaCatlog=NULL;
}
CATLOG_TREEITEM* CATLOG_TREEITEM::AppendSonItem(const char* itemname,
	int idCatgType/*=0*/,DWORD id/*=0*/)
{
	CATLOG_ITEM catgitem;
	CATLOG_ITEM *pCatgItem=pBelongCatlogLibary->Append(catgitem,id);
	if (idCatgType==0&&pCatlog)
		idCatgType=pCatlog->ciType;
	else if (idCatgType<=0)
		idCatgType=1;
	pCatgItem->ciType=idCatgType;
	pCatgItem->name=itemname;
	if (pRelaCatlog)
		pCatgItem->idParentCatg=this->pRelaCatlog->Id;
	CATLOG_TREEITEM* pCatgTreeItem=hashSonItems.append();
	pCatgTreeItem->pCatlog=pCatgItem;
	return pCatgTreeItem;
}
CATLOG_TREEITEM* CATLOG_TREEITEM::MoveSonItemFirst()
{
	return hashSonItems.GetFirst();
}
CATLOG_TREEITEM* CATLOG_TREEITEM::MoveSonItemNext()
{
	return hashSonItems.GetNext();
}
bool CATLOG_TREEITEM::DeleteCursor(bool blClean/*= false*/)
{
	return hashSonItems.DeleteCursor()!=FALSE;
}
int CATLOG_TREEITEM::GetSonItemCount()
{
	return hashSonItems.GetNodeNum();
}
