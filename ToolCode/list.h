#pragma once

//普通堆栈类
template <class TYPE> class SIMPLE_STACK
{
	char *top;
	char *bottom;
	char *cursor;
	char *work;
	size_t bytes;
	size_t blockSize;
public:
	SIMPLE_STACK();
	SIMPLE_STACK(size_t nCount);
	~SIMPLE_STACK(){ delete []top; }
	BOOL IsEmpty()
	{
		if(bottom<=cursor)
			return TRUE;
		else
			return FALSE;
	}
	void Empty()
	{
		cursor = bottom;
	}
	void SetBlockSize(size_t nCount)
	{
		blockSize=nCount;
	}
	int GetRemnantSize()
	{
		return (int)(cursor-top)/(int)bytes;
	}
	int GetPushSize()
	{
		return (int)(bottom-cursor)/(int)bytes;
	}
	TYPE* TopAtom()		//访问栈顶元素
	{
		return (TYPE*)cursor;
	}
	int push(const TYPE& val)
	{
		if(top > cursor - bytes)
		{
			INT_PTR nPos=bottom-cursor;
			INT_PTR nLen=bottom-top;
			char *tm=new char[nLen+blockSize*bytes];
			memcpy(tm+blockSize*bytes,top,nLen);
			delete []top;
			top=tm;
			bottom=top+nLen+blockSize*bytes;
			cursor=bottom-nPos-bytes;
			memcpy(cursor,&val, bytes);
		}
		else
		{
			cursor -= bytes;
			memcpy(cursor, &val, bytes);
		}
		return (int)(bottom-cursor)/(int)bytes;
	}
	bool pop(TYPE& res)
	{
		if(bottom < (cursor + bytes))
			return false;
		else
		{
			memcpy(&res, cursor, bytes);
			cursor += bytes;
			return true;
		}
	}
};

template <class TYPE> 
SIMPLE_STACK<TYPE>::SIMPLE_STACK()
{
	blockSize=2;
	bytes = sizeof(TYPE);
	top = new char[bytes*2];
	bottom = cursor = top + bytes*2;
	work = NULL;
}

template <class TYPE> 
SIMPLE_STACK<TYPE>::SIMPLE_STACK(size_t nCount)
{
	blockSize=2;
	bytes = sizeof(TYPE);
	top = new char[nCount*bytes];
	bottom = cursor = top + nCount*bytes;
	work = NULL;
}


template<class TYPE> class LIST_NODE{
protected:
	int _serial;	//由于最初将此值定义为首个有效节点的序号为1，现已大量被采用，故保留此特例
public:	//常用链表属性
	bool get_Deleted(){return (_serial&0x80000000)!=0;}
	void set_Deleted(bool deleted){_serial=deleted?(_serial|0x80000000):(_serial&0x7fffffff);}
	__declspec( property(put=set_Deleted,get=get_Deleted)) bool Deleted;
	int get_serial(){return _serial&0x7fffffff;};
	void set_serial(int serialIndex){_serial=(_serial&0x80000000)|serialIndex;}
	__declspec( property(put=set_serial,get=get_serial)) int serial;
public:
	TYPE data;
	LIST_NODE<TYPE>* next;
	LIST_NODE(){_serial=0;next=NULL;}
	virtual void Destroy(){delete this;}
};

//////////////////枚举器类定义
struct IXhEnumerator{
	virtual void* MoveFirst()=0;
	virtual void* MoveNext()=0;
	virtual UINT Serial()=0;
	virtual bool Destroy();
	virtual void* getCurrent()=0;
	virtual UINT  getCount()=0;
	__declspec( property(get=getCurrent)) void* pCurrObj;
	__declspec( property(get=getCount)) UINT Count;
public:
	static bool DestroyEnumerBySerial(UINT uiEnumerSerial);
	static IXhEnumerator* NewEnumerator();
};
template<class TYPE> struct IXhTemplEnumerator : public IXhEnumerator{
protected:
	LIST_NODE<TYPE*> head,*current;
	UINT m_uiSerial,m_nObjCount;
public:
	IXhTemplEnumerator(){current=&head;m_nObjCount=0;}
	virtual UINT Serial(){return m_uiSerial;}
	void SetKey(DWORD key){m_uiSerial=key;}
	virtual void* MoveFirst(){
		current=head.next;
		if(current)
			return current->data;
		else
			return NULL;
	}
	virtual void* MoveNext(){
		if(current&&current->next)
		{
			current=current->next;
			return current->data;
		}
		else
			return NULL;
	}
	virtual void* getCurrent(){
		if(current)
			return current->data;
		else
			return NULL;
	}
	virtual UINT  getCount(){return m_nObjCount;};
public:	//template TYPE function
	virtual TYPE* EnumFirst(){return (TYPE*)MoveFirst();}
	virtual TYPE* EnumNext(){return (TYPE*)MoveNext();}
	virtual TYPE* getCurrentTypeObj(){return (TYPE*)getCurrent();};
	__declspec( property(get=getCurrentTypeObj)) TYPE* pCurrTypeObj;
};
template<class TYPE> class CXhInternalTemplEnumerator : public IXhTemplEnumerator<TYPE>{
public:
	virtual void Empty(){
		LIST_NODE<TYPE*>* iter=head.next;
		while(iter!=NULL)
		{
			head.next=iter->next;
			iter->Destroy();
			iter=head.next;
		}
		current=&head;
		m_nObjCount=0;
	}
	virtual void* AppendObjPtr(TYPE* pObj){
		LIST_NODE<TYPE*>* sonnode=new LIST_NODE<TYPE*>();
		sonnode->data=pObj;
		if(current==NULL)
			current=&head;
		while(current->next!=NULL)
			current=current->next;
		current->next=sonnode;
		sonnode->serial=current->serial+1;
		current=sonnode;
		m_nObjCount++;
		return current->data;
	}
};
//////////////////链表基类定义
template<class TYPE> struct IXhList{
	virtual TYPE* MoveFirst()=0;
	virtual TYPE* MoveNext()=0;
	virtual TYPE* Append(DWORD id=0)=0;
	virtual TYPE* Append(TYPE typeobj,DWORD id=0)=0;
private:
	virtual TYPE* getCurrent()=0;
public:
	TYPE* get_Current(){return getCurrent();}
	__declspec( property(get=get_Current)) TYPE* pCurrObj;
private:
	virtual UINT  getCount()=0;
public:
	UINT get_Count(){return getCount();}
	__declspec( property(get=get_Count)) UINT Count;
};
template<class TYPE_PTR> struct IXhSet{
	virtual TYPE_PTR MoveFirst()=0;
	virtual TYPE_PTR MoveNext()=0;
	//virtual TYPE_PTR Append()=0;
	//virtual TYPE_PTR Append(TYPE_PTR typeobj)=0;
	virtual TYPE_PTR getCurrent(){return NULL;}	//因后续定义的Current属性，此处不能为纯虚函数
	virtual UINT	 getCount(){return 0;}		//因后续定义的Count属性，此处不能为纯虚函数
	__declspec( property(get=getCurrent)) TYPE_PTR pCurrObj;
	__declspec( property(get=getCount)) UINT Count;
};
//////////////////简单链表类定义
template<class TYPE> class CXhSimpleList : public IXhList<TYPE>{
protected:
	LIST_NODE<TYPE> head,*current;
	LIST_NODE<TYPE>* current_prev;	//current_prev主要用于DeleteCursor
	SIMPLE_STACK<PVOID>* m_pStack;
	UINT _count;
public:	//链表基类接口指针
	virtual TYPE* MoveFirst(){return EnumObjectFirst();}
	virtual TYPE* MoveNext(){return EnumObjectNext();}
	virtual TYPE* Append(DWORD id=0){return AttachObject();}
	virtual TYPE* Append(TYPE typeobj,DWORD id=0){return AttachObject(typeobj);}
	virtual TYPE* getCurrent(){return GetCurrentObject();}
	virtual UINT  getCount(){return GetCount();}
public:
	UINT GetCount(){return _count;}
	__declspec( property(get=GetCount)) UINT Count;
	LIST_NODE<TYPE>* GetCurrent(){return current;}
	__declspec( property(get=GetCurrent)) LIST_NODE<TYPE>* Current;
	TYPE* GetCurrentObject(){
		if(current)
			return &current->data;
		else
			return NULL;
	}
	__declspec( property(get=GetCurrentObject)) TYPE* CurrentObject;
public:
	CXhSimpleList(){current=&head;current_prev=NULL;m_pStack=NULL;_count=0;}
	~CXhSimpleList(){DeleteList();}
	void AttachStack(SIMPLE_STACK<PVOID>* pStack){m_pStack=pStack;}
	void DetachStack(){m_pStack=NULL;}
	void UpdateListNodeSerial()
	{
		LIST_NODE<TYPE>* iter=&head;
		head.serial=0;
		while(iter->next!=NULL)
		{
			iter->next->serial=iter->serial+1;
			iter=iter->next;
		}
	}
	int push_stack()
	{
		if(m_pStack!=NULL&&current)
			return m_pStack->push(current);
		else
			return 0;//false
	}
	bool pop_stack(int pos=-1)
	{
		if(m_pStack==NULL)
			return false;
		else if(pos==0)
			return false;
		else if(pos>0&&pos!=m_pStack->GetPushSize())
			return false;
		PVOID tmCurr=current;
		if(m_pStack->pop(tmCurr))
		{	//直接m_pStack->pop(current)会导致编译错误，之前未发现是因为一直未启用 wjh-2017.7.4
			current=(LIST_NODE<TYPE>*)tmCurr;
			return true;
		}
		else
		{
			current = NULL;
			return false;
		}
	}
	int GetStackRemnantSize(){return m_pStack!=NULL?m_pStack->GetRemnantSize():0;}
	LIST_NODE<TYPE>* EnumFirst(BOOL bIterDelete=FALSE){
		current_prev=NULL;
		current=head.next;
		while(current!=NULL){
			if(!current->Deleted||bIterDelete)
				return current;
			else
				current=current->next;
		}
		return current;
	}
	LIST_NODE<TYPE>* EnumNext(BOOL bIterDelete=FALSE){
		do{
			if(current)
			{
				current_prev=current;
				current = current->next;
			}
			else
				return NULL;
		}while(current&&current->Deleted&&!bIterDelete);
		return current;
	}
	LIST_NODE<TYPE>* EnumTail(BOOL bIterDelete=FALSE){
		LIST_NODE<TYPE>* pValidTailNode=NULL,*pPrevValidTailNode=NULL;
		if(current==NULL)
		{
			current_prev=&head;
			current=head.next;
			if(!current->Deleted||bIterDelete)
			{
				pValidTailNode=current;
				pPrevValidTailNode=current_prev;
			}
		}
		pValidTailNode=current;
		while(current&&current->next!=NULL)
		{
			current_prev=current;
			current=current->next;
			if(!current->Deleted||bIterDelete)
			{
				pValidTailNode=current;
				pPrevValidTailNode=current_prev;
			}
		}
		current=pValidTailNode;
		current_prev=pPrevValidTailNode;
		return current;
	};
	TYPE* get_pHeadObj() {
		LIST_NODE<TYPE> *pIter = head.next;
		while (pIter != NULL) {
			if (!pIter->Deleted)
				return &pIter->data;
			else
				pIter = pIter->next;
		}
		return NULL;
	}
	__declspec(property(get = get_pHeadObj)) TYPE* pHeadObj;
	TYPE* get_pSecondObj() {
		LIST_NODE<TYPE> *pIter = head.next;
		//查找第二个有效节点
		BYTE hits = 1;
		while (pIter != NULL && hits < 2) {
			if (!pIter->Deleted)
			{
				hits++;
				pIter = pIter->next;
			}
			else
				pIter = pIter->next;
		}
		return pIter != NULL ? &pIter->data : NULL;
	}
	__declspec(property(get = get_pSecondObj)) TYPE* pSecondObj;
	TYPE* EnumObjectFirst(){
		LIST_NODE<TYPE> *pFirstNode=EnumFirst();
		if(pFirstNode)
			return &pFirstNode->data;
		else
			return NULL;
	}
	TYPE* EnumObjectNext(){
		LIST_NODE<TYPE> *pNextNode=EnumNext();
		if(pNextNode)
			return &pNextNode->data;
		else
			return NULL;
	}
	TYPE* EnumObjectTail(){
		LIST_NODE<TYPE>* pTailNode=EnumTail();
		if(pTailNode)
			return &pTailNode->data;
		else
			return NULL;
	};
	bool DeleteCursor(bool clean=false)
	{
		if(current==NULL)
			return false;
		if(clean)
		{
			if(current_prev==NULL)
				head.next=current->next;
			else
				current_prev->next=current->next;
			LIST_NODE<TYPE>* iter=current->next;
			current->Destroy();
			current=iter;
			UpdateListNodeSerial();
		}
		else
			current->Deleted=true;
		_count--;
		return true;
	}
	void Clean(bool updateSerial=false)
	{
		LIST_NODE<TYPE> *iter=&head;
		head.serial=0;
		while(iter->next!=NULL)
		{
			while(iter->next!=NULL&&iter->next->Deleted)
			{
				LIST_NODE<TYPE> *pRemoved=iter->next;
				pRemoved->Destroy();
				iter->next=iter->next->next;
			}
			if(updateSerial)
				iter->next->serial=iter->serial+1;
			iter=iter->next;
		}
		current_prev=NULL;
		current=head.next;
	}
	IXhEnumerator* NewEnumerator()
	{
		CXhInternalTemplEnumerator<void>* pInterEnum=(CXhInternalTemplEnumerator<void>*)IXhEnumerator::NewEnumerator();
		for(TYPE* pTypeObj=EnumObjectFirst();pTypeObj;pTypeObj=EnumObjectNext())
			pInterEnum->AppendObjPtr(pTypeObj);
		return pInterEnum;
	}
	void DeleteList()
	{
		LIST_NODE<TYPE>* iter=head.next;
		while(iter!=NULL)
		{
			head.next=iter->next;
			iter->Destroy();
			iter=head.next;
		}
		current=&head;
		_count=0;
	}
	void Empty(){DeleteList();}
	LIST_NODE<TYPE>* AttachNode()
	{
		LIST_NODE<TYPE>* sonnode=new LIST_NODE<TYPE>();
		LIST_NODE<TYPE>* parent=NULL;
		if(current==NULL)
			current=&head;
		while(current->next!=NULL)
			current=current->next;
		current_prev=current;
		current->next=sonnode;
		sonnode->serial=current->serial+1;
		_count++;
		return current=sonnode;
	}
	LIST_NODE<TYPE>* AttachNode(TYPE son)
	{
		LIST_NODE<TYPE>* sonnode=AttachNode();
		sonnode->data=son;
		return sonnode;
	}
	LIST_NODE<TYPE>* InsertNode(int iInsertIndex=-1)
	{
		if(iInsertIndex==-1)
			return AttachNode();
		LIST_NODE<TYPE>* sonnode=new LIST_NODE<TYPE>();
		LIST_NODE<TYPE>* parent=NULL;
		current_prev=&head;
		current=head.next;
		int index=1;	//head.index=0,故首个有效链表元素索引值为1
		bool bInserted=false;
		while(current)
		{
			if(index-1==iInsertIndex)
			{
				current_prev->next=sonnode;
				sonnode->serial=index;
				sonnode->next=current;
				current_prev=sonnode;
				bInserted=true;
				index++;
				continue;
			}
			current->serial=index;
			current_prev=current;
			current=current->next;
			index++;
		}
		if(!bInserted)
		{
			sonnode->serial=index;
			current_prev->next=current=sonnode;
		}
		_count++;
		return sonnode;
	}
	LIST_NODE<TYPE>* InsertNode(TYPE son,int iInsertIndex=-1)
	{
		LIST_NODE<TYPE>* pInsertNode=InsertNode(iInsertIndex);
		pInsertNode->data=son;
		return pInsertNode;
	}
	TYPE* AttachObject(){ return &(AttachNode()->data);}
	TYPE* AttachObject(TYPE son){ return &(AttachNode(son)->data); }
	TYPE* InsertObject(int iInsertIndex=-1){ return &(InsertNode(iInsertIndex)->data); }
	TYPE* InsertObject(TYPE son,int iInsertIndex=-1){ return &(InsertNode(son,iInsertIndex)->data);}
};

template<class TYPE> class SUPERLIST_NODE{
public:
	int index;
	TYPE* pDataObj;
	SUPERLIST_NODE<TYPE>* next;
	SUPERLIST_NODE(){index=0;next=NULL;}
	virtual void Destroy(){delete this;}
};
template<class TYPE> class CXhSuperList{
	SUPERLIST_NODE<TYPE> head,*current;
public:
	TYPE* (*CreateNewObject)(int idClsType);	//必设回调函数
	BOOL (*DeleteObject)(TYPE *pDataObject);				//必设回调函数
public:
	CXhSuperList(){current=&head;CreateNewObject=NULL;DeleteObject=NULL;}
	~CXhSuperList(){DeleteList();}
	SUPERLIST_NODE<TYPE>* EnumFirst(){return current=head.next;}
	SUPERLIST_NODE<TYPE>* EnumNext(){
		if(current&&current->next)
			return current=current->next;
		else
			return NULL;
	}

	void DeleteList()
	{
		SUPERLIST_NODE<TYPE>* iter=head.next;
		while(iter!=NULL)
		{
			head.next=iter->next;
			if(DeleteObject!=NULL)
				DeleteObject(iter->pDataObj);
			iter->Destroy();
			iter=head.next;
		}
		current=&head;
	}
	SUPERLIST_NODE<TYPE>* AttachNode(int idClsType)
	{
		SUPERLIST_NODE<TYPE>* sonnode=new SUPERLIST_NODE<TYPE>();
		if(CreateNewObject)
			return AttachNode(CreateNewObject(idClsType));
		else
			return NULL;
	}
	SUPERLIST_NODE<TYPE>* AttachNode(TYPE* pDataObject)
	{
		SUPERLIST_NODE<TYPE>* sonnode=new SUPERLIST_NODE<TYPE>();
		sonnode->pDataObj=pDataObject;
		LIST_NODE<TYPE>* parent=NULL;
		if(current==NULL)
			current=&head;
		while(current->next!=NULL)
			current=current->next;
		current->next=sonnode;
		sonnode->index=current->index+1;
		return sonnode;
	}
};

template<class TYPE> class CXhSimpleListStack{
	int nPush;
	CXhSimpleList<TYPE> *m_pList;
	SIMPLE_STACK<PVOID> stack;
public:
	CXhSimpleListStack(CXhSimpleList<TYPE> *pList){
		m_pList=pList;
		pList->AttachStack(&stack);
		nPush=m_pList->push_stack();
	}
	~CXhSimpleListStack(){
		if(nPush)
			m_pList->pop_stack(nPush);
		m_pList->DetachStack();
	}
};
