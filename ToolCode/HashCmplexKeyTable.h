#pragma once

#include "list.h"
#include "Hashtable.h"
//Hash表模板类
//CStringKeyHashTable名称过长，更名为CHashStrTable wjh-2014.6.24
template <class TYPE> class CHashStrTableEx
{
//1.私有成员变量定义
private:			// 注:“索引”即“下标”
	struct DATA_TYPE
	{
		char key[100];
		TYPE atom;
		DATA_TYPE *next;
	};
	DATA_TYPE* data;// Hash表中指向表数据的指针
	DWORD m_nHashSize;	// Hash表中最多存储TYPE元素的个数（即Hash表的大小）
public:
	CHashStrTableEx(DWORD HashTblSize=0)
	{// 构造函数
		HashFunc=NULL;
		KeyMatchFunc=NULL;
		m_nHashSize=HashTblSize;
		if(m_nHashSize>0)
		{
			data = new DATA_TYPE[m_nHashSize];
			memset(data,0,sizeof(DATA_TYPE)*m_nHashSize);
		}
		else
			data = NULL;
	}
    ~CHashStrTableEx(){Empty();}
	bool CreateHashTable(DWORD HashTblSize)
	{
		if(m_nHashSize>0)
			return false;	//必须先清空再创建
		m_nHashSize=HashTblSize;
		if(m_nHashSize>0)
		{
			data = new DATA_TYPE[m_nHashSize];
			memset(data,0,sizeof(DATA_TYPE)*m_nHashSize);
		}
		else
			data = NULL;
		return true;
	}

//2.公有成员变量定义
public:
	DWORD (*HashFunc)(const char *key, DWORD nHashSize);	//根据输入的关键字key计算相应的Hash映像值
	bool  (*KeyMatchFunc)(const char* key, const char* szPartialMatchWithKey);	//判断key是否与withKey匹配，个别情况withKey更精准，允许key与withKey部分匹配
//3.私有成员函数定义
private:
	DWORD Hash(const char *key)const	//根据输入的关键字key计算相应的Hash映像值
	{
		if(HashFunc)
			return HashFunc(key, m_nHashSize);
		ULONG hashkey=0,keylen=(int)strlen(key);
		/*for(int i=0;i<keylen;i++)
			hashkey+=key[i];*/
		//key为空时默认以"CHashStrTable:NULL-KEY"作为键值,
		//否则清空链表，再次用空字符串做键值查询时返回无效值 wht 16-12-05
		char sNullKey[]="CHashStrTable:NULL-KEY";
		const char *sRealKey=keylen>0?key:sNullKey;
		if(keylen<=0)
			keylen=(int)strlen(sRealKey);
		union KEY_VALUE
		{
			BYTE key[4]; //0.A1 1.A2 2.A3 3.A4
			DWORD dwKey; //A4A3A2A1 从高到低
		}key_value,value;
		memset(key_value.key,0,sizeof(BYTE)*4);
		for(ULONG i=0;i<keylen;i++)
		{
			ULONG nRemainder=i%3;
			if(nRemainder==0)
				memset(value.key,0,sizeof(BYTE)*4);
			value.key[nRemainder]=sRealKey[i];
			if(nRemainder==2||i==keylen-1)
				key_value.dwKey+=value.dwKey;
		}
		key_value.key[3]=(BYTE)keylen;
		hashkey=key_value.dwKey;
		/* //hashkey为ULONG 此处不会出现负数 
		if(hashkey<0)	//key值包括汉字时hashkey可能为负值 wht 10-09-08
			hashkey*=-1;
		*/
		return hashkey%m_nHashSize;
	}

//4.公有成员函数定义
public:
	DWORD GetHashTableSize()const{return m_nHashSize;}
	DWORD GetMaxDepth()
	{
		int nMaxDepth=0;
		for(DWORD i=0;i<m_nHashSize;i++)
		{
			DATA_TYPE *cursor=&data[i];
			int nTmpDepth=0;
			while(cursor)
			{
				cursor=cursor->next;
				if(cursor)
					nTmpDepth++;
			}
			if(nTmpDepth>nMaxDepth)
				nMaxDepth=nTmpDepth;
		}
		return nMaxDepth;
	}
	bool ContainsKey(const char *key,bool blCompletelyMatch=false)
	{
		if(data==NULL||key==NULL)
			return false;
		DWORD i = Hash(key);
		if(i<0)//=0)	//计算出的哈希值可能是0
			return false;
		DATA_TYPE *cursor=&data[i];
		while(cursor)
		{
			if ((blCompletelyMatch || KeyMatchFunc == NULL) && strcmp(cursor->key, key) == 0)
				return true;
			else if (!blCompletelyMatch&&KeyMatchFunc != NULL && KeyMatchFunc(key, cursor->key))
				return true;
			else
				cursor=cursor->next;
		}
		return false;
	}
	bool GetValueAt(const char *key,TYPE &val,bool blCompletelyMatch=false)
	{
		if(data==NULL||key==0)	//未初始化或键值为0(不能为0)
			return false;
		DWORD i = Hash(key);
		if(i<0)		//i=0?
			return false;
		DATA_TYPE *cursor=&data[i];
		while(cursor)
		{
			if (((blCompletelyMatch || KeyMatchFunc == NULL) && strcmp(cursor->key, key) == 0)||
				(!blCompletelyMatch&&KeyMatchFunc != NULL && KeyMatchFunc(key, cursor->key)))
			{
				val=cursor->atom;
				return true;
			}
			else
				cursor=cursor->next;
		}
		return false;
	}
    bool SetValueAt(const char *key,TYPE val)//根据索引号获取线段实体
	{
		bool bOk=false;
		if(data==NULL||key==NULL)	//未初始化或键值为0(不能为0)
			return false;
		bool blCompletelyMatch=true;	//写入值时应严格匹配键值 wjh-2019.6.17
		DWORD i = Hash(key);
		if(i<0)	//i=0?
			return false;
		if(data[i].key!=NULL&&*data[i].key!=0)//strlen(data[i].key)>0)
		{
			/*if(strcmp(data[i].key,key)==0)
			{
				data[i].atom = val;
				return true;
			}
			else*/
			{
				DATA_TYPE *tail_item=NULL;
				//DATA_TYPE *cursor=data[i].next;
				DATA_TYPE *cursor=&data[i];
				while(cursor)
				{
					tail_item = cursor;
					if (((blCompletelyMatch || KeyMatchFunc == NULL) && strcmp(cursor->key, key) == 0) ||
						(!blCompletelyMatch&&KeyMatchFunc != NULL && KeyMatchFunc(key, cursor->key)))
					{
						cursor->atom=val;
						bOk=true;
						break;
					}
					else
						cursor=cursor->next;
				}
				if(!bOk)
				{
					tail_item->next=new DATA_TYPE;
					tail_item->next->next=NULL;
					strncpy(tail_item->next->key,key,99);
					tail_item->next->atom=val;
				}
				return true;
			}
		}
		else
		{
			strncpy(data[i].key,key,99);
			data[i].atom=val;
			return true;
		}
	}
    bool Delete(const char *key)	// 删除一个节点
	{
		bool bOk=false;
		if(data==NULL||key==null)	//未初始化或键值为0(不能为0)
			return false;
		DWORD i = Hash(key);
		if(i<0)
			return false;
		if(strlen(data[i].key)>0)
		{
			DATA_TYPE *cursor=&data[i];
			while(cursor)
			{
				if (((blCompletelyMatch || KeyMatchFunc == NULL) && strcmp(cursor->key, key) == 0) ||
					(!blCompletelyMatch&&KeyMatchFunc != NULL && KeyMatchFunc(key, cursor->key)))
				{
					if(cursor->next)
					{
						strncpy(cursor->key,cursor->next->key,99);
						cursor->atom=cursor->next->atom;
						DATA_TYPE *del_item=cursor->next;
						cursor->next=cursor->next->next;
						delete del_item;
					}
					else
						strcpy(cursor->key,"");
					bOk=true;
					break;
				}
				else
					cursor=cursor->next;
			}
			return bOk;
		}
		else
			return false;
	}
    void Empty()		// 清空当前类实例的所有元素
	{
		for(DWORD i=0;i<m_nHashSize;i++)
		{
			if(data[i].next)
			{	//有子元素的
				while(data[i].next)
				{
					DATA_TYPE *cursor=data[i].next;
					data[i].next=data[i].next->next;
					delete cursor;
				}
			}
		}
		delete[]data;
		m_nHashSize=0;
		data=NULL;
	}
    void EmptyKeys()		// 清空当前类实例的所有元素
	{
		for(DWORD i=0;i<m_nHashSize;i++)
		{
			if(data[i].next)
			{	//有子元素的
				while(data[i].next)
				{
					DATA_TYPE *cursor=data[i].next;
					data[i].next=data[i].next->next;
					delete cursor;
				}
			}
			strcpy(data[i].key,"");
		}
	}
};
//字符串哈希链表模板类
template <class TYPE> class CHashStrListEx
{
//1.私有成员变量定义
	struct DATA_TYPE
	{
		TYPE atom;
		DATA_TYPE *prev;
		DATA_TYPE *next;
		char key[100];
		bool bDeleted;
		DATA_TYPE(){
			prev=next=NULL;
			memset(key,0,100);
			bDeleted=false;
		}
	};
	CHashStrTableEx<TYPE*> hashTable;
	ULONG m_uHashTableGrowSize;
	CHashStack<DATA_TYPE*> POS_STACK;
    DWORD NodeNum;		// 总个数
    DATA_TYPE* cursor;    // 当前线段指针
    DATA_TYPE* tail;		// 线段链表末尾指针
    DATA_TYPE* head;		// 线段链表起始指针
public:
    CHashStrListEx()
	{// 构造函数
		NodeNum=0;
		m_uHashTableGrowSize=500;
		cursor = head = tail = NULL;
	}
    ~CHashStrListEx(){Empty();}

//2.公有成员变量定义
public:
	//根据输入的关键字key计算相应的Hash映像值
	void SetHashFunc(DWORD (*HashFunc)(const char *key, DWORD nHashSize))
	{hashTable.HashFunc=HashFunc;}
	//判断key是否与withKey匹配，个别情况withKey更精准，允许key与withKey部分匹配
	void SetKeyMatchFunc(bool (*KeyMatchFunc)(const char* key, const char* szPartialMatchWithKey))
	{hashTable.KeyMatchFunc=KeyMatchFunc;}
//3.私有成员函数定义
private:
	void RebuildHashtable()
	{
		int hashSize;
		if(m_uHashTableGrowSize>0)
			hashSize=NodeNum-NodeNum%m_uHashTableGrowSize+m_uHashTableGrowSize;
		else if(NodeNum>0)
			hashSize=NodeNum+NodeNum;
		else
			hashSize=100;
		if(hashSize<50)
			hashSize=50;	//最小哈希表大小为50;
		hashTable.Empty();
		hashTable.CreateHashTable(hashSize);
		push_stack();
		for(cursor=head;cursor!=NULL;cursor=cursor->next)
		{
			if(!cursor->bDeleted)
				hashTable.SetValueAt(cursor->key,&cursor->atom);
		}
		pop_stack();
	}
//4.公有成员函数定义
public:
	bool push_stack()
	{
		if(cursor)
		{
			POS_STACK.push(cursor);
			return true;
		}
		else
			return false;
	}
	bool pop_stack()
	{
		if(POS_STACK.pop(cursor))
			return true;
		else
		{
			cursor = NULL;
			return false;
		}
	}
	void SetHashTableGrowSize(ULONG growSize,bool bRebuildHashTbl=true){
		m_uHashTableGrowSize=growSize;
		if(bRebuildHashTbl)
			RebuildHashtable();
	}
	int GetStackRemnantSize(){return POS_STACK.GetRemnantSize();}
	IXhEnumerator* NewEnumerator()
	{
		CXhInternalTemplEnumerator<void>* pInterEnum=(CXhInternalTemplEnumerator<void>*)IXhEnumerator::NewEnumerator();
		for(TYPE* pTypeObj=GetFirst();pTypeObj;pTypeObj=GetNext())
			pInterEnum->AppendObjPtr(pTypeObj);
		return pInterEnum;
	}
    TYPE* Add(const char *key)//在节点链表的末尾添加节点
	{
		TYPE *pObj=GetValue(key,true);
		if(pObj!=NULL)
			return pObj;
		else if(NodeNum<=0)//空链表
		{
			cursor = tail = head = new DATA_TYPE;
			strncpy(cursor->key,key,99);
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
			if(hashTable.GetHashTableSize()<NodeNum)
				RebuildHashtable();
			else
				hashTable.SetValueAt(key,&cursor->atom);
			return &cursor->atom;
		}
		
		DATA_TYPE* ptr = new DATA_TYPE;
		strncpy(ptr->key,key,99);
		ptr->prev = tail;
		ptr->next = NULL;
		tail->next = ptr;
		cursor = tail = ptr;
		NodeNum++;
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,&cursor->atom);
		return &ptr->atom;
	}
    TYPE* SetValue(const char *key, TYPE obj)//在节点链表的末尾添加节点
	{
		TYPE *pObj=GetValue(key,true);
		if(pObj!=NULL)
		{
			*pObj=obj;
			hashTable.SetValueAt(key,pObj);
			return pObj;
		}
		else if(NodeNum<=0)//空链表
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->atom=obj;
			strncpy(cursor->key,key,99);
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
			if(hashTable.GetHashTableSize()<NodeNum)
				RebuildHashtable();
			else
				hashTable.SetValueAt(key,&cursor->atom);
			return &cursor->atom;
		}
		
		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->atom=obj;
		strncpy(ptr->key,key,99);
		ptr->prev = tail;
		ptr->next = NULL;
		tail->next = ptr;
		cursor = tail = ptr;
		NodeNum++;
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,&cursor->atom);
		return &ptr->atom;
	}

    TYPE* GetValue(const char *key,bool blCompletelyMatch=false)
	{
		TYPE *pObj=NULL;
		if(hashTable.GetHashTableSize()>0)
			hashTable.GetValueAt(key,pObj,blCompletelyMatch);
		return pObj;
	}
    TYPE& operator[](const char *key)//根据索引号获取线段实体
	{
		return *GetValue(key);
	}
    TYPE* GetNext(BOOL bIterDelete=FALSE)
	{
		do{
			if(cursor)
				cursor = cursor->next;
			else
				break;
		}while(cursor&&cursor->bDeleted&&!bIterDelete);
		if(cursor)
			return &cursor->atom;
		else
			return NULL;
	}
    TYPE* GetPrev(BOOL bIterDelete=FALSE)
	{
		if(cursor&&cursor->prev)
		{
	    	cursor = cursor->prev;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return &cursor->atom;
				else
					cursor=cursor->prev;
			}
			return NULL;
		}
		else
			return NULL;
	}
    TYPE* GetFirst(BOOL bIterDelete=FALSE)
	{
		if(head==NULL)
			return NULL;
		else
		{
			cursor = head;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return &cursor->atom;
				else
					cursor=cursor->next;
			}
			return NULL;
		}
	}
    TYPE* GetTail(BOOL bIterDelete=FALSE)
	{
		if(tail==NULL)
			return NULL;
		else
		{
			cursor = tail;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return &cursor->atom;
				else
					cursor=cursor->prev;
			}
			return NULL;
		}
	}
    TYPE* GetCursor()
	{
		if(cursor)
			return &cursor->atom;
		else
			return NULL;
	}
    char* GetCursorKey() const
	{
		if(cursor)
			return cursor->key;
		else
			return NULL;
	}
	//根据内容得到相应的key wht 10-09-07
	bool GetKey(const TYPE *pObj,char *key)
	{
		DATA_TYPE *temp=NULL;
		for(temp=head;temp;temp=temp->next)
		{
			if(&temp->atom==pObj)
			{
				if(key)
					strcpy(key,temp->key);
				return true;
			}
		}
		return false;
	}
	DWORD GetNodeNum()const{return NodeNum;}
	// 删除一个节点，自动定位到前一个合法节点，删除节点为首节点时定位到下一节点
    BOOL DeleteNode(const char *key)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(!cursor->bDeleted&&strcmp(cursor->key,key)==0)	//跳过已删除节点
				return DeleteCursor();
		}
		return FALSE;
	}
	// 删除当前节点
    BOOL DeleteCursor(BOOL bClean=FALSE)
	{
		if(cursor==NULL||cursor->bDeleted)
			return FALSE;
		else
		{
			hashTable.RemoveKey(cursor->key);
			if(bClean)
			{
				DATA_TYPE *cursor2=NULL;
				if(cursor!=head&&cursor!=tail)
				{
					cursor->prev->next=cursor->next;
					cursor->next->prev=cursor->prev;
					cursor2=cursor->prev;
				}
				else
				{
					if(cursor==head)
					{
						head=cursor->next;
						if(head)
							head->prev=NULL;
						cursor2=head;
					}
					if(cursor==tail)
					{
						tail=cursor->prev;
						if(tail)
							tail->next=NULL;
						cursor2=tail;
					}
				}
				delete cursor;
				cursor=cursor2;
			}
			else 
				cursor->bDeleted=TRUE;
			NodeNum--;	//只是在对未删除节点进行删除时节点总数减一，对已重复删除节点总数不变
			if(NodeNum<0)
				NodeNum=0;
			return TRUE;
		}
	}
	BOOL IsCursorDeleted(){ return cursor->bDeleted;}
	void Clean()
	{
		for(cursor=head;cursor;)
		{
			DATA_TYPE *cursor2=cursor->next;
			if(cursor->bDeleted)
			{	//已被删除元素,移位后清除
				if(cursor!=head&&cursor!=tail)
				{
					cursor->prev->next=cursor->next;
					cursor->next->prev=cursor->prev;
				}
				else
				{
					if(cursor==head)
					{
						head=cursor->next;
						if(head)
							head->prev=NULL;
					}
					if(cursor==tail)
					{
						tail=cursor->prev;
						if(tail)
							tail->next=NULL;
					}
				}
				hashTable.RemoveKey(cursor->key);
				delete cursor;
			}
			cursor=cursor2;
		}
	}
    void Destroy()		// 销毁整个当前类实例
	{
		~CHashList();
	}
    void Empty()		// 清空当前类实例的所有元素
	{
		POS_STACK.Empty();
		cursor=tail;
		while(cursor!=NULL)
		{
			cursor = cursor->prev;
			delete tail;
			tail = cursor;
		}
		hashTable.EmptyKeys();
		NodeNum=0;
		head = tail = NULL;
		cursor = NULL;
	}
	TYPE* ModifyKeyStr(const char *oldkey,const char *newkey)
	{
		if(strcmp(oldkey,newkey)==0)	//键值未发生变化
			return GetValue(newkey);
		else if(GetValue(newkey)!=NULL)
			return FALSE;	//新键值已被占用
		else if(GetValue(oldkey)==NULL)
			return FALSE;	//旧键值未存储数据
		TYPE* ret_obj_ptr=NULL;
		push_stack();
		for(cursor=head;cursor!=NULL;cursor=cursor->next)
		{
			if(strcmp(cursor->key,oldkey)==0)
			{
				hashTable.RemoveKey(cursor->key);
				strncpy(cursor->key,newkey,99);
				hashTable.SetValueAt(cursor->key,&cursor->atom);
				ret_obj_ptr=&cursor->atom;
				break;
			}
		}
		pop_stack();
		return ret_obj_ptr;
	}
	void EmptyStack(){POS_STACK.Empty();}
};
