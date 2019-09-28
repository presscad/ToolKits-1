#pragma once
#include "XhCharString.h"
#include "HashTable.h"
#include "objptr_list.h"

#define STATUS_EXPAND			1
#define STATUS_COLLAPSE			2
#define STATUS_NONE				0xFFFFFFFF

struct PROPLIST_ITEM	{
	long id;
	CXhChar50 name;
	CXhChar500 propHelpStr;
	CXhChar500 propCmbItemsStr;
	CXhChar100 propKeyStr;
	CXhChar100 Url;	//关联联机帮助文档的URL超链接位置
	void put_dwFlag(DWORD value)
	{
		memcpy((char*)Url,&value,4);
	}
	DWORD get_dwFlag()
	{
		DWORD flag=0;
		memcpy(&flag,(char*)Url,4);
		return flag;
	}
	__declspec(property(put=put_dwFlag,get=get_dwFlag)) DWORD dwFlag;
	PROPLIST_ITEM(long idProp=0,const char* propName=NULL,
		const char* propHelp=NULL,const char* cmbItems=NULL)
	{
		id=idProp;
		if(propName)
			name.Copy(propName);
		if(propHelp)
			propHelpStr.Copy(propHelp);
		if(cmbItems)
			propCmbItemsStr.Copy(cmbItems);
	}
};
//////////////////////////////////////////////////////////////////////////////
// 与属性栏相关的全局定义
#define ADD_PROPLISTITEM_FUNC(cls_name,propHashtable) \
	static PROPLIST_ITEM* AddPropItem(char* propKey,PROPLIST_ITEM item){\
		PROPLIST_ITEM* pItem=cls_name::propHashtable.SetValue(propKey,item);\
		pItem->propKeyStr.Copy(propKey);\
		return hashPropItemById.SetValue(item.id,pItem);\
	}
#define UPDATE_PROPLISTITEM_FUNC(cls_name,propHashtable) \
	ADD_PROPLISTITEM_FUNC(cls_name,propHashtable)	\
	static PROPLIST_ITEM* UpdatePropItem(char* propKey,char* propName=NULL){\
		PROPLIST_ITEM *pItem=propHashtable.GetValue(propKey);\
		if(pItem!=NULL&&propName!=NULL)\
			pItem->name.Copy(propName);\
		return pItem;\
	}

#define DECLARE_PROPITEMSTATUS_FUNC(cls_name)	\
	static CHashTable<DWORD>propStatusHashtable;		\
	static int GetPropStatus(long id);	\
	static BOOL SetPropStatus(long id,BOOL bHide);
#define IMPLEMENT_PROPITEMSTATUS_FUNC(cls_name)	\
	CHashTable<DWORD> cls_name::propStatusHashtable;\
	int cls_name::GetPropStatus(long id){	\
		DWORD dwStatus;	\
		if(cls_name::propStatusHashtable.GetValueAt(id,dwStatus)){	\
			if(dwStatus==STATUS_EXPAND)	\
				return 0;\
			else if(dwStatus==STATUS_COLLAPSE)	\
				return 1;\
		}\
		return -1;	\
	}\
	BOOL cls_name::SetPropStatus(long id, BOOL bHide){	\
		DWORD dwStatus;	\
		if(bHide)\
			dwStatus = STATUS_COLLAPSE;\
		else	\
			dwStatus = STATUS_EXPAND;	\
		return cls_name::propStatusHashtable.SetValueAt(id,dwStatus);	\
	}

#define DECLARE_PROPLISTITEM_FUNC(cls_name) \
	static int m_iCurDisplayPropGroup;	\
	static CXhChar100 HELPLINK_URL;		\
	static CHashStrList<PROPLIST_ITEM>propHashtable;	\
	static CHashSet<PROPLIST_ITEM*> hashPropItemById;	\
	static int GetPropStr(long id, char *propStr);\
	static long GetPropID(const char* propStr);	\
	static CXhChar50 GetPropName(long id);	\
	static PROPLIST_ITEM* GetPropItem(long id);\
	static PROPLIST_ITEM* AddPropItem(char* propKey,PROPLIST_ITEM item);\
	static PROPLIST_ITEM* UpdatePropItem(char* propKey,char* propName=NULL);\
	static BOOL IsPropEqual(CXhPtrSet<cls_name> &objSet,long idProp);

#define IMPLEMENT_PROPLISTITEM_FUNC(cls_name) \
	int cls_name::m_iCurDisplayPropGroup;	\
	CXhChar100 cls_name::HELPLINK_URL;		\
	CHashStrList<PROPLIST_ITEM> cls_name::propHashtable;	\
	CHashSet<PROPLIST_ITEM*>cls_name::hashPropItemById;		\
	long cls_name::GetPropID(const char* propStr){				\
		PROPLIST_ITEM *pItem=cls_name::propHashtable.GetValue(propStr);	\
		if(pItem!=NULL)	\
			return pItem->id;	\
		else	\
			return 0;	\
	}	\
	int cls_name::GetPropStr(long id,char* propStr){	\
		CXhChar100 sText;	\
		PROPLIST_ITEM* pItem=hashPropItemById.GetValue(id);	\
		if(pItem!=NULL)	\
			sText.Copy(pItem->propKeyStr);	\
		if(propStr)	\
			strcpy(propStr,sText);	\
		return sText.GetLength();	\
	}	\
	CXhChar50 cls_name::GetPropName(long id){	\
		PROPLIST_ITEM* pItem=cls_name::hashPropItemById.GetValue(id);	\
		if(pItem)\
			return pItem->name;\
		return CXhChar50("");\
	}\
	PROPLIST_ITEM* cls_name::GetPropItem(long id){\
		return cls_name::hashPropItemById.GetValue(id);\
	}\
	PROPLIST_ITEM* cls_name::AddPropItem(char* propKey,PROPLIST_ITEM item){\
		PROPLIST_ITEM* pItem=cls_name::propHashtable.SetValue(propKey,item);\
		pItem->propKeyStr.Copy(propKey);\
		return hashPropItemById.SetValue(item.id,pItem);}\
	PROPLIST_ITEM* cls_name::UpdatePropItem(char* propKey,char* propName/*=NULL*/){\
		PROPLIST_ITEM *pItem=propHashtable.GetValue(propKey);\
		if(pItem!=NULL&&propName!=NULL)\
			pItem->name.Copy(propName);\
		return pItem;\
	}\
	BOOL cls_name::IsPropEqual(CXhPtrSet<cls_name> &objSet,long idProp){\
		if(objSet.GetNodeNum()<=1)\
			return TRUE;\
		char valueStr[500]="";\
		int nPush=objSet.push_stack();\
		cls_name *pObj=NULL, *pFirstObj = (cls_name*)objSet.GetFirst();\
		pFirstObj->GetPropValueStr(idProp,valueStr);\
		for(pObj=(cls_name*)objSet.GetNext();pObj;pObj=(cls_name*)objSet.GetNext())\
		{\
			char valueStr2[500]="";\
			pObj->GetPropValueStr(idProp,valueStr2);\
			if(stricmp(valueStr,valueStr2)!=0){\
				objSet.pop_stack(nPush);\
				return FALSE;\
			}\
		}\
		objSet.pop_stack(nPush);\
		return TRUE;\
	}

#define DECLARE_PROP_FUNC(cls_name)	\
	static void InitPropHashtable();	\
	static void InitPropHashtable(void* pContext);	\
	DECLARE_PROPITEMSTATUS_FUNC(cls_name)	\
	DECLARE_PROPLISTITEM_FUNC(cls_name)	

#define IMPLEMENT_PROP_FUNC(cls_name)	\
	IMPLEMENT_PROPITEMSTATUS_FUNC(cls_name)	\
	IMPLEMENT_PROPLISTITEM_FUNC(cls_name)

class PROPLIST_TYPE{
	CHashStrList<PROPLIST_ITEM> propHashtable;
	CHashSet<PROPLIST_ITEM*> hashPropItemById;
public:
	void InitHashtableSize(UINT uGrowSize){propHashtable.SetHashTableGrowSize(uGrowSize);}
	long GetPropID(const char* propKeyStr){
		PROPLIST_ITEM *pItem=propHashtable.GetValue(propKeyStr);
		if(pItem!=NULL)
			return pItem->id;
		else
			return 0;
	}
	PROPLIST_ITEM* GetPropItem(long id){return hashPropItemById.GetValue(id);}
	PROPLIST_ITEM* GetPropItem(const char* propKeyStr){return propHashtable.GetValue(propKeyStr);}
	PROPLIST_ITEM* AddPropItem(char* propKey,PROPLIST_ITEM item){
		PROPLIST_ITEM* pItem=propHashtable.SetValue(propKey,item);
		pItem->propKeyStr.Copy(propKey);
		return hashPropItemById.SetValue(item.id,pItem);
	}
	PROPLIST_ITEM* EnumFirst(){return propHashtable.GetFirst();}
	PROPLIST_ITEM* EnumNext(){return propHashtable.GetNext();}
};