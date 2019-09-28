#ifndef __TOWER_PTR_LIST_H_
#define __TOWER_PTR_LIST_H_
#include "f_ent_list.h"

template <class TYPE> class CXhPtrList : public ATOM_LIST<TYPE*>
{
public:
	void (*DeleteObjectFunc)(TYPE* pObj);
	CXhPtrList(){DeleteObjectFunc=NULL;}
	~CXhPtrList(){Empty();}
public:
	TYPE* append(TYPE* pType)//在节点链表的末尾添加节点
	{
		TYPE **ppTypeObj = ATOM_LIST<TYPE*>::append(pType);
		return *ppTypeObj;
	}
	TYPE* append()//在节点链表的末尾添加节点
	{
		TYPE* pTypeObj=new TYPE();
		TYPE **ppTypeObj = ATOM_LIST<TYPE*>::append(pTypeObj);
		return *ppTypeObj;
	}
    TYPE* GetNext(BOOL bIterDelete=FALSE)
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetNext(bIterDelete);
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
    TYPE* GetPrev(BOOL bIterDelete=FALSE)
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetPrev(bIterDelete);
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
    TYPE* GetFirst(BOOL bIterDelete=FALSE)
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetFirst(bIterDelete);
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
    TYPE* GetTail(BOOL bIterDelete=FALSE)
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetTail(bIterDelete);
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
	BOOL DeleteCursor()
	{
		TYPE* pObj=GetCursor();
		if(pObj)
		{
			if(DeleteObjectFunc)
				DeleteObjectFunc(pObj);
			else 
				delete pObj;
			return ATOM_LIST<TYPE*>::DeleteCursor(TRUE);
		}
		return FALSE;
	}
	BOOL DeleteNode(TYPE *pType)
	{
		BOOL bRetCode=FALSE;
		int nPush=push_stack();
		for(TYPE* pObj=GetFirst(TRUE);pObj;pObj=GetNext(TRUE))
		{
			if(pObj==pType)
			{
				bRetCode=DeleteCursor();
				break;
			}
		}
		pop_stack(nPush);
		return bRetCode;
	}
    TYPE* GetCursor()
	{
		TYPE **ppType=ATOM_LIST<TYPE*>::GetCursor();
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
	void Empty(){
		if(DeleteObjectFunc!=NULL)
		{
			for(TYPE* pObj=GetFirst(TRUE);pObj;pObj=GetNext(TRUE))
				DeleteObjectFunc(pObj);
		}
		else
		{
			for(TYPE* pObj=GetFirst(TRUE);pObj;pObj=GetNext(TRUE))
				delete pObj;
		}
		ATOM_LIST<TYPE*>::Empty(); 
	}
};

template <class TYPE,class TYPE_PTR> class CTmaPtrList : public ATOM_LIST<TYPE_PTR>
{
	BOOL *m_pModified;
public:
	CTmaPtrList(){;}
	~CTmaPtrList(){;}
public:
	TYPE* append(TYPE* pType)//在节点链表的末尾添加节点
	{
		TYPE **ppTypeObj = ATOM_LIST<TYPE_PTR>::append(pType);
		return *ppTypeObj;
	}
    TYPE* GetNext()
	{
		TYPE **ppType=ATOM_LIST<TYPE_PTR>::GetNext();
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
    TYPE* GetPrev()
	{
		TYPE **ppType=ATOM_LIST<TYPE_PTR>::GetPrev();
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
    TYPE* GetFirst()
	{
		TYPE **ppType=ATOM_LIST<TYPE_PTR>::GetFirst();
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
    TYPE* GetTail()
	{
		TYPE **ppType=ATOM_LIST<TYPE_PTR>::GetTail();
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
    TYPE* GetCursor()
	{
		TYPE **ppType=ATOM_LIST<TYPE_PTR>::GetCursor();
		if(ppType)
			return *ppType;
		else
			return NULL;
	}
	TYPE* FromHandle(long handle)	//根据句柄获得节点
	{
		TYPE* pType;
		BOOL bPush=push_stack();
		for(pType=GetFirst();pType!=NULL;pType=GetNext())
		{
			if(pType->handle == handle)
			{
				if(bPush)
					pop_stack();
				return pType;
			}
		}
		if(bPush)
			pop_stack();
		return NULL;
	}
	BOOL DeleteAt(long ii)
	{
		return ATOM_LIST<TYPE_PTR>::DeleteAt(ii);
	}
	BOOL DeleteNode(long handle)
	{
		int hits=0;
		for(TYPE* pType=GetFirst();pType;pType=GetNext())
		{
			if(!IsCursorDeleted()&&pType->handle== handle)
			{
				ATOM_LIST<TYPE_PTR>::DeleteCursor();
				hits++;
			}
		}
		if(hits>0)
			return TRUE;
		else
			return FALSE;
	}
	void Empty(){
		ATOM_LIST<TYPE_PTR>::Empty(); 
	}
};

#endif