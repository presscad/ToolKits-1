#ifndef __HASH_TABLE_H_
#define __HASH_TABLE_H_
#include "list.h"
//Hash表模板类
template <class TYPE> class CHashTable
{
//1.私有成员变量定义
	typedef struct tagDATA_TYPE
	{
		DWORD key;
		TYPE atom;
		tagDATA_TYPE *next;
	}DATA_TYPE;
public:
    CHashTable(DWORD HashTblSize=0)
	{// 构造函数
		HashFunc=NULL;
		m_nHashSize=HashTblSize;
		if(m_nHashSize>0)
		{
			data = new DATA_TYPE[m_nHashSize];
			memset(data,0,sizeof(DATA_TYPE)*m_nHashSize);
		}
		else
			data = NULL;
	}
    ~CHashTable(){Empty();}
	bool CreateHashTable(DWORD HashTblSize)
	{
		if(m_nHashSize>0)
			Empty();	//必须先清空再创建
		m_nHashSize=HashTblSize;
		if(data!=NULL)
			Empty();
		if(m_nHashSize>0)
		{
			data = new DATA_TYPE[m_nHashSize];
			memset(data,0,sizeof(DATA_TYPE)*m_nHashSize);
		}
		else
			data = NULL;
		return true;
	}
    DWORD m_nHashSize;	// Hash表中最多存储TYPE元素的个数（即Hash表的大小）
    DATA_TYPE* data;// Hash表中指向表数据的指针

//2.公有成员变量定义
public:
	DWORD (*HashFunc)(DWORD key,DWORD nHashSize);
//3.私有成员函数定义
private:
	DWORD Hash(DWORD key)const	//根据输入的关键字key计算相应的Hash映像值
	{
		if(HashFunc)
			return HashFunc(key,m_nHashSize);
		else
		{
			if(m_nHashSize>0)
			{
				if(key<m_nHashSize)
					return key;
				else
					return key%m_nHashSize;
			}
			else
				return 0;
		}
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
	bool ContainsKey(DWORD key)
	{
		if(data==NULL||key==0)	//未初始化或键值为0(不能为0)
			return false;
		DWORD i = Hash(key);
		if(i<0)//=0)	//计算出的哈希值可能是0
			return false;
		DATA_TYPE *cursor=&data[i];
		while(cursor)
		{
			if(cursor->key==key)
				return true;
			else
				cursor=cursor->next;
		}
		return false;
	}
	bool GetValueAt(DWORD key,TYPE &val)
	{
		if(data==NULL||key==0)	//未初始化或键值为0(不能为0)
			return false;
		DWORD i = Hash(key);
		if(i<0)//=0)	//计算出的哈希值可能是0
			return false;
		DATA_TYPE *cursor=&data[i];
		while(cursor)
		{
			if(cursor->key==key)
			{
				val=cursor->atom;
				return true;
			}
			else
				cursor=cursor->next;
		}
		return false;
	}
    bool SetValueAt(DWORD key,TYPE val)//根据索引号获取线段实体
	{
		bool bOk=false;
		if(data==NULL||key==0)	//未初始化或键值为0(不能为0)
			return false;
		DWORD i = Hash(key);
		if(i<0)		//计算出的哈希值当哈希表未初始化时可能是0
			return false;
		if(data[i].key!=0)
		{
			if(data[i].key==key)
			{
				data[i].atom = val;
				return true;
			}
			else
			{
				DATA_TYPE *tail_item=&data[i];
				DATA_TYPE *cursor=data[i].next;
				while(cursor)
				{
					tail_item=cursor;
					if(cursor->key==key)
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
					tail_item->next->key=key;
					tail_item->next->atom=val;
				}
				return true;
			}
		}
		else
		{
			data[i].key=key;
			data[i].atom=val;
			return true;
		}
	}
	bool RemoveKey(DWORD key)
	{
		if(data==NULL||key==0)	//未初始化或键值为0(不能为0)
			return false;
		DWORD i = Hash(key);
		if(i<0)//=0)	//计算出的哈希值可能是0
			return false;
		DATA_TYPE *prev=NULL,*cursor=&data[i];
		while(cursor)
		{
			if(cursor->key==key)
			{
				if(prev==NULL)
				{
					if(data[i].next)
					{	//当前节点之后存在其他数据时，直接清空当前节点将导致数据丢失 wht 12-08-30
						data[i].key=data[i].next->key;
						data[i].atom=data[i].next->atom;
						DATA_TYPE *del_item=data[i].next;
						data[i].next=data[i].next->next;
						delete del_item;
					}
					else
					{
						data[i].key=0;
						memset(cursor,0,sizeof(DATA_TYPE));
					}
					return true;
				}
				else
				{
					prev->next=cursor->next;
					delete cursor;
					return true;
				}
			}
			else
			{
				prev=cursor;
				cursor=cursor->next;
			}
		}
		return false;
	}
    bool Delete(DWORD key)	// 删除一个节点
	{
		bool bOk=false;
		if(data==NULL||key==0)	//未初始化或键值为0(不能为0)
			return false;
		DWORD i = Hash(key);
		if(i<0)
			return false;
		if(data[i].key!=0)
		{
			if(data[i].key==key)
			{
				if(data[i].next)
				{
					data[i].key=data[i].next->key;
					data[i].atom=data[i].next->atom;
					DATA_TYPE *del_item=data[i].next;
					data[i].next=data[i].next->next;
					delete del_item;
				}
				else
					data[i].key=0;
				return true;
			}
			else
			{
				DATA_TYPE *cursor=data[i].next;
				while(cursor)
				{
					if(cursor->key==key)
					{
						if(cursor->next)
						{
							cursor->key=cursor->next->key;
							cursor->atom=cursor->next->atom;
							DATA_TYPE *del_item=cursor->next;
							cursor->next=cursor->next->next;
							delete del_item;
						}
						else
							cursor->key=0;
						bOk=true;
						break;
					}
					else
						cursor=cursor->next;
				}
				return bOk;
			}
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
			data[i].key=0;
		}
	}
};
struct HASH2KEY{
	DWORD key1,key2;
	bool operator==(HASH2KEY hash2key)
	{
		if(key1==hash2key.key1&&key2==hash2key.key2)
			return true;
		else
			return false;
	}
	bool operator==(DWORD key)
	{
		if(key1==key && key2==key)
			return true;
		else
			return false;
	}
	bool operator!=(DWORD key)
	{
		if(key1!=key || key2!=key)
			return true;
		else
			return false;
	}
	DWORD HashCode(){
		DWORD key=(key1<<16)+(key1>>16)+key2;
		return key;
	}
	HASH2KEY(DWORD k1=0,DWORD k2=0){key1=k1;key2=k2;}
};
template <class TYPE> class CHashMap
{
//1.私有成员变量定义
	typedef struct tagDATA_TYPE
	{
		HASH2KEY key;
		TYPE atom;
		tagDATA_TYPE *next;
	}DATA_TYPE;
public:
    CHashMap(DWORD HashTblSize=0)
	{// 构造函数
		HashFunc=NULL;
		m_nHashSize=HashTblSize;
		if(m_nHashSize>0)
		{
			data = new DATA_TYPE[m_nHashSize];
			memset(data,0,sizeof(DATA_TYPE)*m_nHashSize);
		}
		else
			data = NULL;
	}
    ~CHashMap(){Empty();}
	bool CreateHashTable(DWORD HashTblSize)
	{
		if(m_nHashSize>0)
			Empty();	//必须先清空再创建
		m_nHashSize=HashTblSize;
		if(data!=NULL)
			Empty();
		if(m_nHashSize>0)
		{
			data = new DATA_TYPE[m_nHashSize];
			memset(data,0,sizeof(DATA_TYPE)*m_nHashSize);
		}
		else
			data = NULL;
		return true;
	}
    DWORD m_nHashSize;	// Hash表中最多存储TYPE元素的个数（即Hash表的大小）
    DATA_TYPE* data;// Hash表中指向表数据的指针

//2.公有成员变量定义
public:
	DWORD (*HashFunc)(DWORD key1,DWORD key2,DWORD nHashSize);
//3.私有成员函数定义
private:
	DWORD Hash(DWORD key1,DWORD key2)const	//根据输入的关键字key计算相应的Hash映像值
	{
		if(HashFunc)
			return HashFunc(key1,key2,m_nHashSize);
		else
		{
			HASH2KEY hashkey(key1,key2);
			DWORD key=hashkey.HashCode();
			if(m_nHashSize>0)
				return key%m_nHashSize;
			else
				return 0;
		}
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
	bool ContainsKey(DWORD key1,DWORD key2)
	{
		if(data==NULL||(key1==0&&key2==0))	//未初始化或键值为0(不能为0)
			return false;
		DWORD i = Hash(key1,key2);
		if(i<0)//=0)	//计算出的哈希值可能是0
			return false;
		DATA_TYPE *cursor=&data[i];
		while(cursor)
		{
			if(cursor->key==HASH2KEY(key1,key2))
				return true;
			else
				cursor=cursor->next;
		}
		return false;
	}
	bool GetValueAt(DWORD key1,DWORD key2,TYPE &val)
	{
		if(data==NULL||(key1==0&&key2==0))	//未初始化或键值为0(不能为0)
			return false;
		DWORD i = Hash(key1,key2);
		if(i<0)//=0)	//计算出的哈希值可能是0
			return false;
		DATA_TYPE *cursor=&data[i];
		while(cursor)
		{
			if(cursor->key==HASH2KEY(key1,key2))
			{
				val=cursor->atom;
				return true;
			}
			else
				cursor=cursor->next;
		}
		return false;
	}
    bool SetValueAt(DWORD key1,DWORD key2,TYPE val)//根据索引号获取线段实体
	{
		bool bOk=false;
		if(data==NULL||(key1==0&&key2==0))	//未初始化或键值为0(不能为0)
			return false;
		DWORD i = Hash(key1,key2);
		if(i<0)//=0)	//计算出的哈希值可能是0
			return false;
		if(data[i].key!=0)
		{
			if(data[i].key==HASH2KEY(key1,key2))
			{
				data[i].atom = val;
				return true;
			}
			else
			{
				DATA_TYPE *tail_item=&data[i];
				DATA_TYPE *cursor=data[i].next;
				while(cursor)
				{
					tail_item=cursor;
					if(cursor->key==HASH2KEY(key1,key2))
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
					tail_item->next->key=HASH2KEY(key1,key2);
					tail_item->next->atom=val;
				}
				return true;
			}
		}
		else
		{
			data[i].key=HASH2KEY(key1,key2);
			data[i].atom=val;
			return true;
		}
	}
	bool RemoveKey(DWORD key1,DWORD key2)
	{
		if(data==NULL||(key1==0&&key2==0))	//未初始化或键值为0(不能为0)
			return false;
		DWORD i = Hash(key1,key2);
		if(i<0)//=0)	//计算出的哈希值可能是0
			return false;
		DATA_TYPE *prev=NULL,*cursor=&data[i];
		while(cursor)
		{
			if(cursor->key==HASH2KEY(key1,key2))
			{
				if(prev==NULL)
				{
					memset(cursor,0,sizeof(DATA_TYPE));
					return true;
				}
				else
				{
					prev->next=cursor->next;
					delete cursor;
					return true;
				}
			}
			else
			{
				prev=cursor;
				cursor=cursor->next;
			}
		}
		return false;
	}
    bool Delete(DWORD key1,DWORD key2)	// 删除一个节点
	{
		bool bOk=false;
		if(data==NULL||(key1==0&&key2==0))	//未初始化或键值为0(不能为0)
			return false;
		DWORD i = Hash(key1,key2);
		if(i<0)
			return false;
		if(data[i].key!=0)
		{
			if(data[i].key==HASH2KEY(key1,key2))
			{
				if(data[i].next)
				{
					data[i].key=data[i].next->key;
					data[i].atom=data[i].next->atom;
					DATA_TYPE *del_item=data[i].next;
					data[i].next=data[i].next->next;
					delete del_item;
				}
				else
					data[i].key.key1=data[i].key.key2=0;
				return true;
			}
			else
			{
				DATA_TYPE *cursor=data[i].next;
				while(cursor)
				{
					if(cursor->key1==key1&&cursor->key2==key2)
					{
						if(cursor->next)
						{
							cursor->key=cursor->next->key;
							cursor->atom=cursor->next->atom;
							DATA_TYPE *del_item=cursor->next;
							cursor->next=cursor->next->next;
							delete del_item;
						}
						else
							cursor->key.key1=cursor->key.key2=0;
						bOk=true;
						break;
					}
					else
						cursor=cursor->next;
				}
				return bOk;
			}
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
			data[i].key.key1=data[i].key.key2=0;
		}
	}
};
//CStringKeyHashTable名称过长，更名为CHashStrTable wjh-2014.6.24
#define CStringKeyHashTable CHashStrTable
template <class TYPE> class CHashStrTable
{
public:
    CHashStrTable(DWORD HashTblSize=0)
	{// 构造函数
		HashFunc=NULL;
		m_nHashSize=HashTblSize;
		if(m_nHashSize>0)
		{
			data = new DATA_TYPE[m_nHashSize];
			memset(data,0,sizeof(DATA_TYPE)*m_nHashSize);
		}
		else
			data = NULL;
	}
    ~CHashStrTable(){Empty();}
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
//1.私有成员变量定义
private:			// 注:“索引”即“下标”
	typedef struct tagDATA_TYPE
	{
		char key[100];
		TYPE atom;
		tagDATA_TYPE *next;
	}DATA_TYPE;
    DWORD m_nHashSize;	// Hash表中最多存储TYPE元素的个数（即Hash表的大小）
    DATA_TYPE* data;// Hash表中指向表数据的指针

//2.公有成员变量定义
public:
	DWORD (*HashFunc)(const char *key,DWORD nHashSize,BYTE cnMaxKeyBufSize);	//根据输入的关键字key计算相应的Hash映像值
//3.私有成员函数定义
private:
	DWORD Hash(const char *key)const	//根据输入的关键字key计算相应的Hash映像值
	{
		if(HashFunc)
			return HashFunc(key,m_nHashSize,100);
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

	bool ContainsKey(const char *key)
	{
		if(data==NULL||key==NULL)
			return false;
		DWORD i = Hash(key);
		if(i<0)//=0)	//计算出的哈希值可能是0
			return false;
		DATA_TYPE *cursor=&data[i];
		while(cursor)
		{
			if(strcmp(cursor->key,key)==0)
				return true;
			else
				cursor=cursor->next;
		}
		return false;
	}
	bool GetValueAt(const char *key,TYPE &val)
	{
		if(data==NULL||key==0)	//未初始化或键值为0(不能为0)
			return false;
		DWORD i = Hash(key);
		if(i<0)		//i=0?
			return false;
		DATA_TYPE *cursor=&data[i];
		while(cursor)
		{
			if(strcmp(cursor->key,key)==0)
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
					if(strcmp(cursor->key,key)==0)
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
			/*if(strcmp(data[i].key,key)==0)
			{
				if(data[i].next)
				{
					strncpy(data[i].key,data[i].next->key,99);
					data[i].atom=data[i].next->atom;
					DATA_TYPE *del_item=data[i].next;
					data[i].next=data[i].next->next;
					delete del_item;
				}
				else
					strcpy(data[i].key,"");
				return true;
			}
			else*/
			{
				DATA_TYPE *cursor=&data[i];
				while(cursor)
				{
					if(strcmp(cursor->key,key)==0)
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
	bool RemoveKey(const char* key)
	{
		if(data==NULL||key==NULL)	//未初始化或键值为0(不能为0)
			return false;
		DWORD i = Hash(key);
		if(i<0)//=0)	//计算出的哈希值可能是0
			return false;
		DATA_TYPE *prev=NULL,*cursor=&data[i];
		while(cursor)
		{
			if(_stricmp(cursor->key,key)==0)
			{
				if(prev==NULL)
				{
					if(data[i].next)
					{
						strcpy(data[i].key,data[i].next->key);
						data[i].atom=data[i].next->atom;
						DATA_TYPE *del_item=data[i].next;
						data[i].next=data[i].next->next;
						delete del_item;
					}
					else
						memset(cursor,0,sizeof(DATA_TYPE));
					return true;
				}
				else
				{
					prev->next=cursor->next;
					delete cursor;
					return true;
				}
			}
			else
			{
				prev=cursor;
				cursor=cursor->next;
			}
		}
		return false;
	}
};
//普通堆栈类
template <class TYPE> class CHashStack
{
	char *top;
	char *bottom;
	char *cursor;
	char *work;
	size_t bytes;
	size_t blockSize;
public:
	CHashStack();
	CHashStack(size_t nCount);
	~CHashStack(){ delete []top; }
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
		return (cursor-top)/bytes;
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
			int nPos=(int)(bottom-cursor);
			int nLen=(int)(bottom-top);
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
CHashStack<TYPE>::CHashStack()
{
	blockSize=2;
	bytes = sizeof(TYPE);
	top = new char[bytes*2];
	bottom = cursor = top + bytes*2;
	work = NULL;
}

template <class TYPE> 
CHashStack<TYPE>::CHashStack(size_t nCount)
{
	blockSize=2;
	bytes = sizeof(TYPE);
	top = new char[nCount*bytes];
	bottom = cursor = top + nCount*bytes;
	work = NULL;
}
//链表模板类
template <class TYPE> class CHashList : public IXhList<TYPE>
{
protected:
	DWORD unique_id;
	DWORD m_dwMinmalId;
#pragma pack(push,1)
	struct DATA_TYPE
	{
		TYPE atom;
		DATA_TYPE *prev;
		DATA_TYPE *next;
		DWORD key;
		bool bDeleted;
		DATA_TYPE(){key=0;bDeleted=FALSE;}
	};
#pragma pack(pop)
	ULONG m_uHashTableGrowSize;
	CHashTable<TYPE*> hashTable;
	CHashStack<DATA_TYPE*> POS_STACK;
    DWORD NodeNum;			// 总个数
    DATA_TYPE* cursor;		// 当前线段指针
    DATA_TYPE* tail;		// 线段链表末尾指针
    DATA_TYPE* head;		// 线段链表起始指针
public:
    CHashList()
	{// 构造函数
		unique_id=0;
		m_dwMinmalId=1;
		NodeNum=0;
		cursor = head = tail = NULL;
		m_uHashTableGrowSize=500;
		LoadDefaultObjectInfo=NULL;
		AllocateUniqueIdFunc=NULL;
	}
    ~CHashList(){Empty();}

//2.公有成员变量定义
public:
	void SetHashFunc(DWORD (*hash_func)(DWORD key,DWORD nHashSize)){hashTable.HashFunc=hash_func;}
	DWORD (*AllocateUniqueIdFunc)(void* pContext);						//可选择设定回调函数
	void (*LoadDefaultObjectInfo)(TYPE *pObj,DWORD key);
public:	//链表基类接口指针
	virtual TYPE* MoveFirst(){return GetFirst();}
	virtual TYPE* MoveNext(){return GetNext();}
	virtual TYPE* Append(DWORD id=0){return Add(id);}
	virtual TYPE* Append(TYPE obj,DWORD key=0)//在节点链表的末尾添加节点
	{
		if(key>0)
			return SetValue(key,obj);
		else
			return SetValue(key,obj,true);
	}
	virtual TYPE* getCurrent(){return GetCursor();}
	virtual UINT  getCount(){return GetNodeNum();}
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
			unique_id=max(unique_id,cursor->key);
			if(!cursor->bDeleted)
				hashTable.SetValueAt(cursor->key,&cursor->atom);
		}
		pop_stack();
	}
	//获取一个新的局部唯一标识（在当前哈希链表内）
	DWORD GetLocalUniqueId()
	{
		DWORD testid=m_dwMinmalId;
		DATA_TYPE* indicator=tail;
		while(testid<10000000){	//设10000000是为了防止万一出错时陷入死循环
			if(hashTable.GetHashTableSize()>0&&hashTable.ContainsKey(testid))
			{
				if(indicator&&indicator->key>testid)
					testid=indicator->key+1;
				else
					testid++;
				if(indicator)
					indicator=indicator->prev;
			}
			else	//当前testid已是唯一标识
				break;
		}
		return testid;
	}
//4.公有成员函数定义
public:
	void SetMinmalId(DWORD minId){m_dwMinmalId=minId;}
	DWORD AllocateUniqueId()
	{
		DWORD id=0;
		if(AllocateUniqueIdFunc==NULL)
			id=GetLocalUniqueId();
		if(id>0)
			return id;
		return AllocateUniqueIdFunc(this);
	}
	//暂时由GetNewId改名而来，改名目的是为了避免代码编写时智能提示与GetNext过近干扰，将来该函数视情况可能被AllocateUniqueId取代
	long NewUniqueId(){
		unique_id++;
		return unique_id;
	}
	TYPE* ModifyKeyId(DWORD oldkey, DWORD newkey)
	{
		if(newkey==oldkey)	//键值未发生变化
			return GetValue(newkey);
		else if(GetValue(newkey)!=NULL)
			return FALSE;	//新键值已被占用
		else if(GetValue(oldkey)==NULL)
			return FALSE;	//旧键值未存储数据
		TYPE* ret_obj_ptr=NULL;
		push_stack();
		for(cursor=head;cursor!=NULL;cursor=cursor->next)
		{
			if(cursor->key==oldkey)
			{
				hashTable.RemoveKey(cursor->key);
				cursor->key=newkey;
				hashTable.SetValueAt(cursor->key,&cursor->atom);
				ret_obj_ptr=&cursor->atom;
				break;
			}
		}
		pop_stack();
		return ret_obj_ptr;
	}
	int push_stack()
	{
		if(cursor)
			return POS_STACK.push(cursor);
		else
			return FALSE;  //0
	}
	bool pop_stack(int pos=-1)
	{
		if(pos==0)
			return false;
		else if(pos>0&&pos!=POS_STACK.GetPushSize())
			return false;
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
 	//ii==-1时，在当前节点的前面添加节点；否则在index所指向节点前添加，该函数主用于板轮廓顶点添加 wjh-2014.8.25
    TYPE* insert(const TYPE &obj,long ii=-1,DWORD key=0)
	{
		TYPE *pObj=NULL;
		if(key<=0)	//key<0时自动生成key值
			key=AllocateUniqueId();
		else 
			pObj=GetValue(key);
		if(pObj!=NULL)
		{
			*pObj=obj;
			return pObj;
		}

		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->bDeleted = FALSE;
		ptr->atom = obj;

		if(NodeNum<=0)//空链表
		{
    		cursor = head = tail = ptr;
			cursor->prev = NULL;
			cursor->next = NULL;
		}
		else if(ii < 0)//以当前节点位置为新节点插入位置
		{
			ptr->next = cursor;
  			ptr->prev = cursor->prev;
				//原当前节点处理
			if(cursor->prev!=NULL)
    			cursor->prev->next = ptr;
			else	//cursor原为首节点
				head = ptr;
			cursor->prev = ptr;
		}
		else if(ii>=0&&ii<(long)NodeNum)//指定插入点位置
		{
			int i=0;
			for(cursor=head;cursor;cursor=cursor->next,i++)
			{
				if(cursor->bDeleted)
				{
					i--;
					continue;
				}
				else if(i==ii)
					break;
			}
        		//建立新添节点与链表内原点间的连接关系
			ptr->next = cursor;
  			ptr->prev = cursor->prev;
				//插入位置处原节点处理
			if(cursor->prev!=NULL)
    			cursor->prev->next = ptr;
			else if(cursor->prev==NULL)//指定插入点原为链表首节点
				head = ptr;
			cursor->prev = ptr;
		}
		else //(ii>=NodeNum)
		{
  			//GetTail();//cursor is equal to tail now.   --00.05.25
			ptr->prev = tail;
			ptr->next = NULL;
			tail->next = ptr;
			cursor = tail = ptr;
		}
		cursor=ptr;
		NodeNum++;
		if(LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(&ptr->atom,key);
		unique_id=max(unique_id,ptr->key);
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,&ptr->atom);
		return &ptr->atom;
	}
    TYPE* Add(DWORD key)//在节点链表的末尾添加节点
	{
		TYPE *pObj=NULL;
		if(key<=0)	//key<0时自动生成key值
			key=AllocateUniqueId();
		else 
			pObj=GetValue(key);
		if(pObj!=NULL)
			return pObj;
		else if(NodeNum<=0)//空链表
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->key = key;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
		}
		else
		{
			DATA_TYPE* ptr = new DATA_TYPE;
			ptr->key = key;
			ptr->prev = tail;
			ptr->next = NULL;
			tail->next = ptr;
			cursor = tail = ptr;
			NodeNum++;
		}
		if(LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(&cursor->atom,key);
		unique_id=max(unique_id,cursor->key);
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,&cursor->atom);
		return &cursor->atom;
	}
	//allocZeroKey=true时，如果key=0系统会自动分配一个唯一标识key；allocZeroKey=false时，则直接返回空
    TYPE* SetValue(DWORD key, TYPE obj,bool allocZeroKey=false)//在节点链表的末尾添加节点
	{
		if(key==0&&!allocZeroKey)	//因为Hashtable不支持key值为0,所以key不能为0 wjh-2013.2.23
			return NULL;
		TYPE *pObj=NULL;
		if(key==0)	//key<0时自动生成key值
			key=AllocateUniqueId();
		else 
			pObj=GetValue(key);
		if(pObj!=NULL)
		{
			*pObj=obj;
			hashTable.SetValueAt(key,pObj);
			//防止后续SetValue时键值重复，但obj.key未初始化或不一致情况导致异常错误 wjh-2016.9.12
			if(LoadDefaultObjectInfo)
				LoadDefaultObjectInfo(pObj,key);
			return pObj;
		}
		else if(NodeNum<=0)//空链表
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->atom=obj;
			cursor->key = key;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
		}
		else
		{
			DATA_TYPE* ptr = new DATA_TYPE;
			ptr->atom=obj;
			ptr->key = key;
			ptr->prev = tail;
			ptr->next = NULL;
			tail->next = ptr;
			cursor = tail = ptr;
			NodeNum++;
		}
		if(LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(&cursor->atom,key);
		unique_id=max(unique_id,cursor->key);
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,&cursor->atom);
		return &cursor->atom;
	}

    TYPE* GetValue(DWORD key)
	{
		TYPE *pObj=NULL;
		if(hashTable.GetHashTableSize()>0)
			hashTable.GetValueAt(key,pObj);
		return pObj;
	}
    TYPE& operator[](DWORD key)
	{	//使用[]时无法根据返回值判断是否找到对应的存储内容，应慎用
		//如：TYPE为指针类型，输入一个不存在的key时会返回一个错误的指针,而不是NULL wht 12-09-19
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
		/*if(cursor&&cursor->next)
		{
			cursor = cursor->next;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return &cursor->atom;
				else
					cursor=cursor->next;
			}
			return &cursor->atom;	//当下一个节点为末节点且已被设为删除标志时会出现异常 wjh-2013.10.12
		}*/
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
    DWORD GetCursorKey()
	{
		if(cursor)
			return cursor->key;
		else
			return 0;
	}
	//根据内容得到相应的key wht 10-09-07
	DWORD GetKey(const TYPE *pObj)
	{
		DWORD key=0;
		DATA_TYPE *temp=((DATA_TYPE*)((BYTE*)pObj - offsetof(DATA_TYPE, atom)));
		if(temp!=NULL&&GetValue(temp->key)==pObj)
			return temp->key;	//目前此种方法暂不能判断pObj非法情况　wjh-2014.7.18
		/*
		for(temp=head;temp;temp=temp->next)
		{
			if(&temp->atom==pObj)
			{
				key=temp->key;
				break;
			}
		}*/
		return key;
	}
	DWORD GetNodeNum()const{return NodeNum;}
	// 删除一个节点，自动定位到前一个合法节点，删除节点为首节点时定位到下一节点
    BOOL DeleteNode(DWORD key)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(!cursor->bDeleted&&cursor->key==key)	//跳过已删除节点
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
		unique_id=0;
	}
	void EmptyStack(){POS_STACK.Empty();}
#ifdef _DEBUG
	int GetMemorySize()
	{
		int size=sizeof(this);
		size+=hashTable.m_nHashSize*sizeof(TYPE*);
		size+=POS_STACK.GetRemnantSize();
		return size;
		return size;
	}
#endif
};

template <class TYPE> void SetHashObjKey(TYPE *pObj,DWORD key){pObj->SetKey(key);}
template <class TYPE> class CHashListEx : public CHashList<TYPE>{
public:
	CHashListEx(){
		LoadDefaultObjectInfo=SetHashObjKey;
	}
};
//链表模板类管理对象指针集合，不涉及对象生命周期，相对于CHashList，返回值直接即为TYPE_PTR
template <class TYPE_PTR> class CHashSet : public IXhSet<TYPE_PTR>
{
	DWORD unique_id;
	typedef struct tagDATA_TYPE
	{
		TYPE_PTR atom;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
		DWORD key;
		BOOL bDeleted;
		tagDATA_TYPE(){key=0;bDeleted=FALSE;}
	}DATA_TYPE;
	ULONG m_uHashTableGrowSize;
	CHashTable<TYPE_PTR> hashTable;
	CHashStack<DATA_TYPE*> POS_STACK;
    DWORD NodeNum;			// 总个数
    DATA_TYPE* cursor;		// 当前线段指针
    DATA_TYPE* tail;		// 线段链表末尾指针
    DATA_TYPE* head;		// 线段链表起始指针
public:
    CHashSet()
	{// 构造函数
		unique_id=0;
		NodeNum=0;
		cursor = head = tail = NULL;
		m_uHashTableGrowSize=500;
		LoadDefaultObjectInfo=NULL;
	}
    ~CHashSet(){Empty();}

//2.公有成员变量定义
public:
	void SetHashFunc(DWORD (*hash_func)(DWORD key,DWORD nHashSize)){hashTable.HashFunc=hash_func;}
	void (*LoadDefaultObjectInfo)(TYPE_PTR pObj,DWORD key);
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
			unique_id=max(unique_id,cursor->key);
			if(!cursor->bDeleted)
				hashTable.SetValueAt(cursor->key,cursor->atom);
		}
		pop_stack();
	}
//4.公有成员函数定义
public:
	//暂时由GetNewId改名而来，改名目的是为了避免代码编写时智能提示与GetNext过近干扰，将来该函数视情况可能被AllocateUniqueId取代
	long NewUniqueId(){
		unique_id++;
		return unique_id;
	}
	TYPE_PTR ModifyKeyId(DWORD oldkey, DWORD newkey)
	{
		if(newkey==oldkey)	//键值未发生变化
			return GetValue(newkey);
		else if(GetValue(newkey)!=NULL)
			return FALSE;	//新键值已被占用
		else if(GetValue(oldkey)==NULL)
			return FALSE;	//旧键值未存储数据
		TYPE_PTR ret_obj_ptr=NULL;
		push_stack();
		for(cursor=head;cursor!=NULL;cursor=cursor->next)
		{
			if(cursor->key==oldkey)
			{
				hashTable.RemoveKey(cursor->key);
				cursor->key=newkey;
				hashTable.SetValueAt(cursor->key,cursor->atom);
				ret_obj_ptr=cursor->atom;
				break;
			}
		}
		pop_stack();
		return ret_obj_ptr;
	}
	int push_stack()
	{
		if(cursor)
			return POS_STACK.push(cursor);
		else
			return FALSE;  //0
	}
	bool pop_stack(int pos=-1)
	{
		if(pos==0)
			return false;
		else if(pos>0&&pos!=POS_STACK.GetPushSize())
			return false;
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
		for(TYPE_PTR pTypeObj=GetFirst();pTypeObj;pTypeObj=GetNext())
			pInterEnum->AppendObjPtr(pTypeObj);
		return pInterEnum;
	}
    TYPE_PTR SetValue(DWORD key, TYPE_PTR obj)//在节点链表的末尾添加节点
	{
		if(key==0)	//因为Hashtable不支持key值为0,所以key不能为0 wjh-2013.2.23
			return NULL;
		TYPE_PTR pObj=GetValue(key);
		if(pObj!=NULL)
		{
			pObj=obj;
			hashTable.SetValueAt(key,pObj);
			return pObj;
		}
		else if(NodeNum<=0)//空链表
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->atom=obj;
			cursor->key = key;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
		}
		else
		{
			DATA_TYPE* ptr = new DATA_TYPE;
			ptr->atom=obj;
			ptr->key = key;
			ptr->prev = tail;
			ptr->next = NULL;
			tail->next = ptr;
			cursor = tail = ptr;
			NodeNum++;
		}
		if(LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(cursor->atom,key);
		unique_id=max(unique_id,cursor->key);
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,cursor->atom);
		return cursor->atom;
	}

    TYPE_PTR operator[](DWORD key)
	{	//使用[]时无法根据返回值判断是否找到对应的存储内容，应慎用
		//如：TYPE为指针类型，输入一个不存在的key时会返回一个错误的指针,而不是NULL wht 12-09-19
		return GetValue(key);
	}
    TYPE_PTR GetValue(DWORD key)
	{
		TYPE_PTR pObj=NULL;
		if(hashTable.GetHashTableSize()>0)
			hashTable.GetValueAt(key,pObj);
		return pObj;
	}
    TYPE_PTR GetNext(BOOL bIterDelete=FALSE)
	{
		do{
			if(cursor)
				cursor = cursor->next;
			else
				break;
		}while(cursor&&cursor->bDeleted&&!bIterDelete);
		if(cursor)
			return cursor->atom;
		/*if(cursor&&cursor->next)
		{
			cursor = cursor->next;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return cursor->atom;
				else
					cursor=cursor->next;
			}
			return cursor->atom;	//当下一个节点为末节点且已被设为删除标志时会出现异常 wjh-2013.10.12
		}*/
		else
			return NULL;
	}
    TYPE_PTR GetPrev(BOOL bIterDelete=FALSE)
	{
		if(cursor&&cursor->prev)
		{
	    	cursor = cursor->prev;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return cursor->atom;
				else
					cursor=cursor->prev;
			}
			return NULL;
		}
		else
			return NULL;
	}
    TYPE_PTR GetFirst(BOOL bIterDelete=FALSE)
	{
		if(head==NULL)
			return NULL;
		else
		{
			cursor = head;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return cursor->atom;
				else
					cursor=cursor->next;
			}
			return NULL;
		}
	}
    TYPE_PTR GetTail(BOOL bIterDelete=FALSE)
	{
		if(tail==NULL)
			return NULL;
		else
		{
			cursor = tail;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return cursor->atom;
				else
					cursor=cursor->prev;
			}
			return NULL;
		}
	}
    TYPE_PTR GetCursor()
	{
		if(cursor)
			return cursor->atom;
		else
			return NULL;
	}
    DWORD GetCursorKey()
	{
		if(cursor)
			return cursor->key;
		else
			return 0;
	}
	//根据内容得到相应的key wht 10-09-07
	DWORD GetKey(const TYPE_PTR pObj)
	{
		DWORD key=0;
		DATA_TYPE *temp=NULL;
		for(temp=head;temp;temp=temp->next)
		{
			if(temp->atom==pObj)
			{
				key=temp->key;
				break;
			}
		}
		return key;
	}
	DWORD GetNodeNum()const{return NodeNum;}
	// 删除一个节点，自动定位到前一个合法节点，删除节点为首节点时定位到下一节点
    BOOL DeleteNode(DWORD key)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(!cursor->bDeleted&&cursor->key==key)	//跳过已删除节点
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
		unique_id=0;
	}
	void EmptyStack(){POS_STACK.Empty();}
public:	//IXhSet重载函数
	virtual TYPE_PTR MoveFirst(){return GetFirst();}
	virtual TYPE_PTR MoveNext(){return GetNext();}
	virtual TYPE_PTR getCurrent(){return GetCursor();}
	virtual UINT  getCount(){return GetNodeNum();}
};
//链表模板类(以指针的形式管理对象的全生命周期，适用于一些复杂对象（内含链表等复杂结构引用）
template <class TYPE> class CHashPtrList
{
	DWORD unique_id;
	typedef struct tagDATA_TYPE
	{
		TYPE* pAtom;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
		DWORD key;
		BOOL bDeleted;
		tagDATA_TYPE(){key=0;bDeleted=FALSE;pAtom=NULL;}
	}DATA_TYPE;
	ULONG m_uHashTableGrowSize;
	CHashTable<TYPE*> hashTable;
	CHashStack<DATA_TYPE*> POS_STACK;
    DWORD NodeNum;			// 总个数
    DATA_TYPE* cursor;		// 当前线段指针
    DATA_TYPE* tail;		// 线段链表末尾指针
    DATA_TYPE* head;		// 线段链表起始指针
public:
    CHashPtrList()
	{// 构造函数
		unique_id=0;
		NodeNum=0;
		cursor = head = tail = NULL;
		m_uHashTableGrowSize=500;
		LoadDefaultObjectInfo=NULL;
		DeleteObjectFunc=NULL;
		AllocateUniqueIdFunc=NULL;
	}
	~CHashPtrList(){;Empty();}

//2.公有成员变量定义
public:
	void (*LoadDefaultObjectInfo)(TYPE *pObj,DWORD key);
	void (*DeleteObjectFunc)(TYPE* pObj);
	DWORD (*AllocateUniqueIdFunc)(void* pContext);						//可选择设定回调函数
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
				hashTable.SetValueAt(cursor->key,cursor->pAtom);
		}
		pop_stack();
	}
	//获取一个新的局部唯一标识（在当前哈希链表内）
	DWORD GetLocalUniqueId()
	{
		DWORD testid=1;
		DATA_TYPE* indicator=tail;
		while(testid<10000000){	//设10000000是为了防止万一出错时陷入死循环
			if(hashTable.GetHashTableSize()>0&&hashTable.ContainsKey(testid))
			{
				if(indicator&&indicator->key>testid)
					testid=indicator->key+1;
				else
					testid++;
				if(indicator)
					indicator=indicator->prev;
			}
			else	//当前testid已是唯一标识
				break;
		}
		return testid;
	}
//4.公有成员函数定义
public:
	DWORD AllocateUniqueId()
	{
		DWORD id=0;
		if(AllocateUniqueIdFunc==NULL)
			id=GetLocalUniqueId();
		if(id>0)
			return id;
		return AllocateUniqueIdFunc(this);
	}
//4.公有成员函数定义
public:
	int push_stack()
	{
		if(cursor)
			return POS_STACK.push(cursor);
		else
			return FALSE;  //0
	}
	bool pop_stack(int pos=-1)
	{
		if(pos==0)
			return false;
		else if(pos>0&&pos!=POS_STACK.GetPushSize())
			return false;
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
    TYPE* Add(DWORD key)//在节点链表的末尾添加节点
	{
		TYPE *pObj=NULL;
		if(key<=0)	//key<0时自动生成key值
			key=AllocateUniqueId();
		else 
			pObj=GetValue(key);
		if(pObj!=NULL)
			return pObj;
		else if(NodeNum<=0)//空链表
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->key = key;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
		}
		else
		{
			DATA_TYPE* ptr = new DATA_TYPE;
			ptr->key = key;
			ptr->prev = tail;
			ptr->next = NULL;
			tail->next = ptr;
			cursor = tail = ptr;
			NodeNum++;
		}
		cursor->pAtom=new TYPE(key);
		if(LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(cursor->pAtom,key);
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,cursor->pAtom);
		return cursor->pAtom;
	}
     TYPE* AddNoneKeyObj(DWORD key)//在节点链表的末尾添加节点(对象构造函数中无key参数赋值)wjh-2019.11.11
	{
		TYPE *pObj=NULL;
		if(key<=0)	//key<0时自动生成key值
			key=AllocateUniqueId();
		else 
			pObj=GetValue(key);
		if(pObj!=NULL)
			return pObj;
		else if(NodeNum<=0)//空链表
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->key = key;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
		}
		else
		{
			DATA_TYPE* ptr = new DATA_TYPE;
			ptr->key = key;
			ptr->prev = tail;
			ptr->next = NULL;
			tail->next = ptr;
			cursor = tail = ptr;
			NodeNum++;
		}
		cursor->pAtom=new TYPE();
		if(LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(cursor->pAtom,key);
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,cursor->pAtom);
		return cursor->pAtom;
	}
   /*	由于SetValue会导致对象生命期管理复杂化，暂不支持此函数 wjh-2014.3.22
	TYPE* SetValue(DWORD key, TYPE* pObj)//在节点链表的末尾添加节点
	{
		TYPE *pObj=GetValue(key);
		if(pObj!=NULL)
		{
			*pObj=obj;
			return pObj;
		}
		else if(NodeNum<=0)//空链表
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->pAtom=pObj;
			cursor->key = key;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
		}
		else
		{
			DATA_TYPE* ptr = new DATA_TYPE;
			ptr->pAtom=pObj;
			ptr->key = key;
			ptr->prev = tail;
			ptr->next = NULL;
			tail->next = ptr;
			cursor = tail = ptr;
			NodeNum++;
		}
		//if(LoadDefaultObjectInfo)
		//	LoadDefaultObjectInfo(&cursor->atom,key);
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,cursor->pAtom);
		return cursor->pAtom;
	}*/

    TYPE* GetValue(DWORD key)
	{
		TYPE *pObj=NULL;
		if(hashTable.GetHashTableSize()>0)
			hashTable.GetValueAt(key,pObj);
		return pObj;
	}
    TYPE& operator[](DWORD key)//根据索引号获取线段实体
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
			return cursor->pAtom;
		/*if(cursor&&cursor->next)
		{
			cursor = cursor->next;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return cursor->pAtom;
				else
					cursor=cursor->next;
			}
			return cursor->pAtom;	//当下一个节点为末节点且已被设为删除标志时会出现异常 wjh-2013.10.12
		}*/
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
					return cursor->pAtom;
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
					return cursor->pAtom;
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
					return cursor->pAtom;
				else
					cursor=cursor->prev;
			}
			return NULL;
		}
	}
    TYPE* GetCursor()
	{
		if(cursor)
			return cursor->pAtom;
		else
			return NULL;
	}
    DWORD GetCursorKey()
	{
		if(cursor)
			return cursor->key;
		else
			return 0;
	}
	//根据内容得到相应的key wht 10-09-07
	DWORD GetKey(const TYPE *pObj)
	{
		DWORD key=0;
		DATA_TYPE *temp=NULL;
		for(temp=head;temp;temp=temp->next)
		{
			if(temp->pAtom==pObj)
			{
				key=temp->key;
				break;
			}
		}
		return key;
	}
	DWORD GetNodeNum()const{return NodeNum;}
	// 删除一个节点，自动定位到前一个合法节点，删除节点为首节点时定位到下一节点
    BOOL DeleteNode(DWORD key)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(!cursor->bDeleted&&cursor->key==key)	//跳过已删除节点
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
				if(DeleteObjectFunc)
					DeleteObjectFunc(cursor->pAtom);
				else 
					delete cursor->pAtom;
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
				if(DeleteObjectFunc)
					DeleteObjectFunc(cursor->pAtom);
				else 
					delete cursor->pAtom;
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
			if(DeleteObjectFunc)
				DeleteObjectFunc(tail->pAtom);
			else 
				delete tail->pAtom;
			delete tail;
			tail = cursor;
		}
		hashTable.EmptyKeys();
		NodeNum=0;
		head = tail = NULL;
		cursor = NULL;
	}
	void EmptyStack(){POS_STACK.Empty();}
};

template <class TYPE> class CMapList
{
	typedef struct tagDATA_TYPE
	{
		TYPE atom;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
		HASH2KEY key;
		BOOL bDeleted;
		tagDATA_TYPE(){key.key1=key.key2=0;bDeleted=FALSE;}
	}DATA_TYPE;
	ULONG m_uHashTableGrowSize;
	CHashMap<TYPE*> hashTable;
	CHashStack<DATA_TYPE*> POS_STACK;
    DWORD NodeNum;			// 总个数
    DATA_TYPE* cursor;		// 当前线段指针
    DATA_TYPE* tail;		// 线段链表末尾指针
    DATA_TYPE* head;		// 线段链表起始指针
public:
    CMapList()
	{// 构造函数
		NodeNum=0;
		cursor = head = tail = NULL;
		m_uHashTableGrowSize=500;
		LoadDefaultObjectInfo=NULL;
	}
    ~CMapList(){Empty();}

//2.公有成员变量定义
public:
	void SetHashFunc(DWORD (*hash_func)(DWORD key1,DWORD key2,DWORD nHashSize)){hashTable.HashFunc=hash_func;}
	void (*LoadDefaultObjectInfo)(TYPE *pObj,DWORD key1,DWORD key2);
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
				hashTable.SetValueAt(cursor->key.key1,cursor->key.key2,&cursor->atom);
		}
		pop_stack();
	}
//4.公有成员函数定义
public:
	int push_stack()
	{
		if(cursor)
			return POS_STACK.push(cursor);
		else
			return FALSE;  //0
	}
	bool pop_stack(int pos=-1)
	{
		if(pos==0)
			return false;
		else if(pos>0&&pos!=POS_STACK.GetPushSize())
			return false;
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
    TYPE* Add(DWORD key1,DWORD key2)//在节点链表的末尾添加节点
	{
		TYPE *pObj=GetValue(key1,key2);
		if(pObj!=NULL)
			return pObj;
		else if(NodeNum<=0)//空链表
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->key = HASH2KEY(key1,key2);
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
		}
		else
		{
			DATA_TYPE* ptr = new DATA_TYPE;
			ptr->key = HASH2KEY(key1,key2);
			ptr->prev = tail;
			ptr->next = NULL;
			tail->next = ptr;
			cursor = tail = ptr;
			NodeNum++;
		}
		if(LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(&cursor->atom,key1,key2);
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key1,key2,&cursor->atom);
		return &cursor->atom;
	}
    TYPE* SetValue(DWORD key1,DWORD key2, TYPE obj)//在节点链表的末尾添加节点
	{
		TYPE *pObj=GetValue(key1,key2);
		if(pObj!=NULL)
		{
			*pObj=obj;
			hashTable.SetValueAt(key1,key2,pObj);
			//防止后续SetValue时键值重复，但obj.key未初始化或不一致情况导致异常错误 wjh-2016.9.12
			if(LoadDefaultObjectInfo)
				LoadDefaultObjectInfo(pObj,key1,key2);
			return pObj;
		}
		else if(NodeNum<=0)//空链表
		{
			cursor = tail = head = new DATA_TYPE;
			cursor->atom=obj;
			cursor->key = HASH2KEY(key1,key2);
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
			//return &cursor->atom;
		}
		else
		{
			DATA_TYPE* ptr = new DATA_TYPE;
			ptr->atom=obj;
			ptr->key = HASH2KEY(key1,key2);
			ptr->prev = tail;
			ptr->next = NULL;
			tail->next = ptr;
			cursor = tail = ptr;
			NodeNum++;
		}
		if(LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(&cursor->atom,key1,key2);
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key1,key2,&cursor->atom);
		return &cursor->atom;
	}

    TYPE* GetValue(DWORD key1,DWORD key2)
	{
		TYPE *pObj=NULL;
		if(hashTable.GetHashTableSize()>0)
			hashTable.GetValueAt(key1,key2,pObj);
		return pObj;
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
		/*if(cursor&&cursor->next)
		{
			cursor = cursor->next;
			while(cursor!=NULL){
				if(!cursor->bDeleted||bIterDelete)
					return &cursor->atom;
				else
					cursor=cursor->next;
			}
			return &cursor->atom;	//当下一个节点为末节点且已被设为删除标志时会出现异常 wjh-2013.10.12
		}*/
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
    HASH2KEY GetCursorKey()
	{
		if(cursor)
			return cursor->key;
		else
			return 0;
	}
	//根据内容得到相应的key wht 10-09-07
	HASH2KEY GetKey(const TYPE *pObj)
	{
		HASH2KEY key;
		DATA_TYPE *temp=NULL;
		for(temp=head;temp;temp=temp->next)
		{
			if(&temp->atom==pObj)
			{
				key=temp->key;
				break;
			}
		}
		return key;
	}
	DWORD GetNodeNum()const{return NodeNum;}
	// 删除一个节点，自动定位到前一个合法节点，删除节点为首节点时定位到下一节点
    BOOL DeleteNode(DWORD key1,DWORD key2)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(!cursor->bDeleted&&cursor->key==HASH2KEY(key1,key2))
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
			hashTable.RemoveKey(cursor->key.key1,cursor->key.key2);
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
				hashTable.RemoveKey(cursor->key.key1,cursor->key.key2);
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
	void EmptyStack(){POS_STACK.Empty();}
};

//字符串哈希链表模板类
//CStringKeyHashList名称过长，更名为CHashStrList wjh-2014.6.24
#define CStringKeyHashList CHashStrList
template <class TYPE> class CHashStrList
{
//1.私有成员变量定义
	typedef struct tagDATA_TYPE
	{
		TYPE atom;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
		char key[100];
		BOOL bDeleted;
		tagDATA_TYPE(){prev=NULL;next=NULL;strcpy(key,"");bDeleted=FALSE;}
	}DATA_TYPE;
	CHashStrTable<TYPE*> hashTable;
	ULONG m_uHashTableGrowSize;
	CHashStack<DATA_TYPE*> POS_STACK;
    DWORD NodeNum;		// 总个数
    DATA_TYPE* cursor;    // 当前线段指针
    DATA_TYPE* tail;		// 线段链表末尾指针
    DATA_TYPE* head;		// 线段链表起始指针
public:
    CHashStrList()
	{// 构造函数
		NodeNum=0;
		m_uHashTableGrowSize=500;
		cursor = head = tail = NULL;
	}
    ~CHashStrList(){Empty();}

//2.公有成员变量定义
public:
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
		TYPE *pObj=GetValue(key);
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
		TYPE *pObj=GetValue(key);
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

    TYPE* GetValue(const char *key)
	{
		TYPE *pObj=NULL;
		if(hashTable.GetHashTableSize()>0)
			hashTable.GetValueAt(key,pObj);
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

//////////////////////////////////////////////////////////////////////////
//使用该链表时需要设置回调函数:CreateNewData,DeleteData
template <class TYPE> class CSuperHashList
{
#pragma pack(push,1)
	struct DATA_TYPE
	{
		TYPE *pAtom;
		DATA_TYPE *prev;
		DATA_TYPE *next;
		DWORD key;
		bool bDeleted;
		DATA_TYPE(){bDeleted=FALSE; pAtom=NULL; prev=NULL; next=NULL; key=0;}
	};
#pragma pack(pop)
	ULONG m_uHashTableGrowSize;
	CHashTable<TYPE*> hashTable;
	CHashStack<DATA_TYPE*> POS_STACK;
    DWORD NodeNum;		// 总个数
    DATA_TYPE* cursor;    // 当前线段指针
    DATA_TYPE* tail;		// 线段链表末尾指针
    DATA_TYPE* head;		// 线段链表起始指针
public:
	void* m_pContext;	//上下文环境
    CSuperHashList()
	{// 构造函数
		NodeNum=0;
		cursor = head = tail = NULL;
		m_uHashTableGrowSize=500;
		CreateNewAtom=NULL;
		DeleteAtom=NULL;
		LoadDefaultObjectInfo=NULL;
		AllocateUniqueIdFunc=NULL;
		m_pContext=NULL;
	}
    ~CSuperHashList(){Empty();}

//2.公有成员变量定义
public:
	//回调函数
	TYPE* (*CreateNewAtom)(int idClsType,DWORD key,void* pContext);	//必设回调函数
	BOOL (*DeleteAtom)(TYPE *pAtom);				//必设回调函数
	long (*AllocateUniqueIdFunc)();						//可选择设定回调函数
	void (*LoadDefaultObjectInfo)(TYPE *pObj,DWORD key);//一般不需要设定
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
				hashTable.SetValueAt(cursor->key,cursor->pAtom);
		}
		pop_stack();
	}
	//获取一个新的局部唯一标识（在当前哈希链表内）
	DWORD GetLocalUniqueId()
	{
		if(hashTable.GetHashTableSize()==0)
			return 1;
		DWORD testid=1;
		DATA_TYPE* indicator=tail;
		do{	//设10000000是为了防止万一出错时陷入死循环
			if(hashTable.ContainsKey(testid))
			{
				if(indicator&&indicator->key>testid)
					testid=indicator->key+1;
				else
					testid++;
				if(indicator)
					indicator=indicator->prev;
			}
			else	//当前testid已是唯一标识
				break;
		}while(testid<10000000);
		return testid;
	}
//4.公有成员函数定义
public:
	//获取一个新的局部唯一标识（在当前哈希链表内）
	DWORD AllocateUniqueId()
	{
		if(AllocateUniqueIdFunc==NULL)
			return GetLocalUniqueId();
		else
			return AllocateUniqueIdFunc();
	}
	BOOL push_stack()
	{
		if(cursor)
		{
			return POS_STACK.push(cursor);
			//return true;
		}
		else
			return 0;//false;
	}
	bool pop_stack(int pos=-1)
	{
		if(pos==0)
			return false;
		else if(pos>0&&pos!=POS_STACK.GetPushSize())
			return false;
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
	//在节点链表的末尾添加节点,key==0(key不能为负数)时自动生成key值
    TYPE* Add(DWORD key,int CLSTYPE_ID=0,bool loadDefaultObjInfo=true)
	{
		TYPE *pAtom=NULL;
		if(key<=0)	//key<0时自动生成key值
			key=AllocateUniqueId();
		else 
		{
			pAtom=GetValue(key);
			if(pAtom!=NULL)
				return pAtom;//throw "出现重复key值,可能导致程序异常！";
		}
		if(CreateNewAtom!=NULL)
			pAtom = CreateNewAtom(CLSTYPE_ID,key,m_pContext);
		if(pAtom==NULL)
			return NULL;
		if(loadDefaultObjInfo&&LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(pAtom,key);
		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->pAtom=pAtom;
		if(NodeNum<=0)//空链表
		{
			cursor = tail = head = ptr;
			cursor->key = key;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
			if(hashTable.GetHashTableSize()<NodeNum)
				RebuildHashtable();
			else
				hashTable.SetValueAt(key,cursor->pAtom);
			return cursor->pAtom;
		}
		//
		ptr->key = key;
		ptr->prev = tail;
		ptr->next = NULL;
		tail->next = ptr;
		cursor = tail = ptr;
		NodeNum++;
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,cursor->pAtom);
		return ptr->pAtom;
	}
	//在当前节点的前面添加节点,key==-1时自动生成key值
    TYPE* insert(DWORD key,int CLSTYPE_ID=0,bool loadDefaultObjInfo=true)
	{
		TYPE *pAtom=NULL;
		if(key<=0)	//key<0时自动生成key值
			key=AllocateUniqueId();
		else 
		{
			TYPE *pAtom=GetValue(key);
			if(pAtom!=NULL)
				return pAtom;//throw "出现重复key值,可能导致程序异常！";
		}
		if(CreateNewAtom!=NULL)
			pAtom = CreateNewAtom(CLSTYPE_ID,key,m_pContext);
		if(pAtom==NULL)
			pAtom = new TYPE;
		if(loadDefaultObjInfo&&LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(pAtom,key);
		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->pAtom=pAtom;
		ptr->bDeleted = FALSE;
		ptr->key = key;
		if(NodeNum<=0)//空链表
		{
			cursor = tail = head = ptr;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
			if(hashTable.GetHashTableSize()<NodeNum)
				RebuildHashtable();
			else
				hashTable.SetValueAt(key,cursor->pAtom);
			return cursor->pAtom;
		}
		
		//在当前节点的前面添加节点
		ptr->next = cursor;
		ptr->prev = cursor->prev;
		//原当前节点处理
		if(cursor->prev!=NULL)
			cursor->prev->next = ptr;
		else	//cursor原为首节点
			head = ptr;
		cursor->prev = ptr;
		cursor=ptr;	//不把新增节点设为当前节点,后面更新hashTable代码会更新错误 wjh-2016.11.04
		NodeNum++;
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,cursor->pAtom);
		return ptr->pAtom;
	}

    TYPE* GetValue(DWORD key)
	{
		TYPE *pObj=NULL;
		if(hashTable.GetHashTableSize()>0)
			hashTable.GetValueAt(key,pObj);
		return pObj;
	}
    TYPE& operator[](DWORD key)//根据索引号获取线段实体
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
			return cursor->pAtom;
		else
			return NULL;
		/*
		if(cursor&&cursor->next)
		{
			cursor = cursor->next;
			return cursor->pAtom;
		}
		else
			return NULL;
		*/
	}
    TYPE* GetPrev(BOOL bIterDelete=FALSE)
	{
		do{
			if(cursor)
				cursor = cursor->prev;
			else
				break;
		}while(cursor&&cursor->bDeleted&&!bIterDelete);
		if(cursor)
			return cursor->pAtom;
		else
			return NULL;
		/*
		if(cursor&&cursor->prev)
		{
	    	cursor = cursor->prev;
			return cursor->pAtom;
		}
		else
			return NULL;
		*/
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
					return cursor->pAtom;
				else
					cursor=cursor->next;
			}
			return NULL;
			//return cursor->pAtom;
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
					return cursor->pAtom;
				else
					cursor=cursor->prev;
			}
			return NULL;
			//return cursor->pAtom;
		}
	}
    TYPE* GetCursor()
	{
		if(cursor)
			return cursor->pAtom;
		else
			return NULL;
	}
    DWORD GetCursorKey()
	{
		if(cursor)
			return cursor->key;
		else
			return 0;
	}
	//根据内容得到相应的key wht 10-09-07
	long GetKey(const TYPE *pObj)
	{
		long key=0;
		DATA_TYPE *temp=NULL;
		for(temp=head;temp;temp=temp->next)
		{
			if(temp->pAtom==pObj)
			{
				key=temp->key;
				break;
			}
		}
		return key;
	}
	//移动当前指定项位置
	bool Move(TYPE *pObj,int head0_prev1_next2_tail3)
	{
		DATA_TYPE *temp=NULL;
		for(temp=head;temp;temp=temp->next)
		{
			if(temp->pAtom==pObj)
				break;
		}
		if(temp==NULL)
			return false;
		if(head0_prev1_next2_tail3==0)
		{	//移动当前节点至首节点
			if(temp==head)
				return false;
			temp->prev->next=temp->next;
			if(temp->next)
				temp->next->prev=temp->prev;
			else //当前节点为尾节点，上移后temp->prev为尾节点
				tail=temp->prev;
			temp->prev=NULL;
			temp->next=head;
			head->prev=temp;
			head=temp;
		}
		else if(head0_prev1_next2_tail3==1)
		{	//将当前节点上移一位
			if(temp==head)
				return false;
			DATA_TYPE *pPrev=temp->prev;
			if(pPrev==NULL)
				return false;
			DATA_TYPE *pPrevPrev=pPrev->prev;
			if(pPrevPrev)
				pPrevPrev->next=temp;
			else //移动后当前节点为头结点
				head=temp;
			if(temp->next)
				temp->next->prev=pPrev;
			else //当前节点为尾节点，上移后pPrev为尾节点
				tail=pPrev;
			pPrev->next=temp->next;
			pPrev->prev=temp;
			temp->next=pPrev;
			temp->prev=pPrevPrev;
		}
		else if(head0_prev1_next2_tail3==2)
		{	//将当前节点下移一位
			if(temp==tail)
				return false;
			DATA_TYPE *pNext=temp->next;
			if(pNext==NULL)
				return false;
			DATA_TYPE *pNextNext=pNext->next;
			if(pNextNext)
				pNextNext->prev=temp;
			else //移动后当前节点为尾结点
				tail=temp;
			if(temp->prev)
				temp->prev->next=pNext;
			else //当前节点为头结点,上移后pNext为头结点
				head=pNext;
			pNext->next=temp;
			pNext->prev=temp->prev;
			temp->prev=pNext;
			temp->next=pNextNext;
		}
		else if(head0_prev1_next2_tail3==3)
		{	//将当前节点移至尾部
			if(temp==tail)
				return false;
			if(temp->prev)
				temp->prev->next=temp->next;
			else //当前节点为头结点,置尾后temp->next为头结点
				head=temp->next;
			temp->next->prev=temp->prev;
			//
			temp->prev=tail;
			temp->next=NULL;
			tail->next=temp;
			tail=temp;
		}
		return true;
	}

	DWORD GetNodeNum()const{return NodeNum;}
	// 删除一个节点，自动定位到前一个合法节点，删除节点为首节点时定位到下一节点
    BOOL DeleteNode(DWORD key)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(cursor->key==key)
				return DeleteCursor();
		}
		return FALSE;
	}
	// 删除当前节点
    BOOL DeleteCursor(BOOL bClean=TRUE)
	{
		if(cursor==NULL)
			return FALSE;
		else
		{
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
				hashTable.RemoveKey(cursor->key);
				if(DeleteAtom)
					DeleteAtom(cursor->pAtom);
				else 
					delete cursor->pAtom;
				delete cursor;
				cursor=cursor2;
			}
			else 
				cursor->bDeleted=TRUE;
			NodeNum--;
			if(NodeNum<0)
				NodeNum=0;
			return TRUE;
		}
	}
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
				if(DeleteAtom)
					DeleteAtom(cursor->pAtom);
				else 
					delete cursor->pAtom;
				delete cursor;
			}
			cursor=cursor2;
		}
	}
    void Destroy()		// 销毁整个当前类实例
	{
		~CSuperHashList();
	}
    void Empty()		// 清空当前类实例的所有元素
	{
		POS_STACK.Empty();
		cursor=tail;
		while(cursor!=NULL)
		{
			cursor = cursor->prev;
			if(tail)
			{
				if(DeleteAtom)
					DeleteAtom(tail->pAtom);
				else 
					delete tail->pAtom;
			}
			delete tail;
			tail = cursor;
		}
		hashTable.EmptyKeys();
		NodeNum=0;
		head = tail = NULL;
		cursor = NULL;
	}
	void EmptyStack(){POS_STACK.Empty();}
	TYPE* ModifyKey(DWORD dwOldKey,DWORD dwNewKey)
	{
		if(dwOldKey==dwNewKey)	//键值未发生变化
			return GetValue(dwNewKey);
		else if(GetValue(dwNewKey)!=NULL)
			return NULL;	//新键值已被占用
		else if(GetValue(dwOldKey)==NULL)
			return NULL;	//旧键值未存储数据
		TYPE* ret_obj_ptr=NULL;
		hashTable.GetValueAt(dwOldKey,ret_obj_ptr);
		if(ret_obj_ptr)
		{
			hashTable.RemoveKey(dwOldKey);
			hashTable.SetValueAt(dwNewKey,ret_obj_ptr);
		}
		return ret_obj_ptr;	
	}
	//批量更新对象Key值,保证对象指针不变的情况下更新对应key值（TAP螺栓表更新之后需更新螺栓Key值） wht 16-12-08
	void UpdateHashTblKeys(CHashTable<DWORD> &hashKeyByTypePtr)
	{
		push_stack();
		for(cursor=head;cursor!=NULL;cursor=cursor->next)
		{
			DWORD dwKey=0;
			if(hashKeyByTypePtr.GetValueAt((DWORD)cursor->pAtom,dwKey))
				cursor->key=dwKey;
		}
		pop_stack();
		RebuildHashtable();
	}
};

template <class TYPE> class CSuperHashListEx : public CSuperHashList<TYPE>{
public:
	CSuperHashListEx(){
		LoadDefaultObjectInfo=SetHashObjKey;
	}
};
//////////////////////////////////////////////////////////////////////////
//使用该链表时需要设置回调函数:CreateNewData,DeleteData
//key不能为NULL或""
template <class TYPE> class CSuperHashStrList
{
	typedef struct tagDATA_TYPE
	{
		TYPE *pAtom;
		tagDATA_TYPE *prev;
		tagDATA_TYPE *next;
		char key[100];
		BOOL bDeleted;
		tagDATA_TYPE(){bDeleted=FALSE; pAtom=NULL; prev=NULL; next=NULL; strcpy(key,"");}
	}DATA_TYPE;
	ULONG m_uHashTableGrowSize;
	CHashStrTable<TYPE*> hashTable;
	CHashStack<DATA_TYPE*> POS_STACK;
	DWORD NodeNum;		// 总个数
	DATA_TYPE* cursor;    // 当前线段指针
	DATA_TYPE* tail;		// 线段链表末尾指针
	DATA_TYPE* head;		// 线段链表起始指针
public:
	void* m_pContext;	//上下文环境
	CSuperHashStrList()
	{// 构造函数
		NodeNum=0;
		cursor = head = tail = NULL;
		m_uHashTableGrowSize=500;
		CreateNewAtom=NULL;
		DeleteAtom=NULL;
		LoadDefaultObjectInfo=NULL;
		AllocateUniqueIdFunc=NULL;
		m_pContext=NULL;
	}
	~CSuperHashStrList(){Empty();}

	//2.公有成员变量定义
public:
	//回调函数
	TYPE* (*CreateNewAtom)(int idClsType,const char* key,void* pContext);	//必设回调函数
	BOOL (*DeleteAtom)(TYPE *pAtom);				//必设回调函数
	long (*AllocateUniqueIdFunc)();						//可选择设定回调函数
	void (*LoadDefaultObjectInfo)(TYPE *pObj,const char* key);//一般不需要设定
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
				hashTable.SetValueAt(cursor->key,cursor->pAtom);
		}
		pop_stack();
	}
	//4.公有成员函数定义
public:
	BOOL push_stack()
	{
		if(cursor)
		{
			return POS_STACK.push(cursor);
			//return true;
		}
		else
			return 0;//false;
	}
	bool pop_stack(int pos=-1)
	{
		if(pos==0)
			return false;
		else if(pos>0&&pos!=POS_STACK.GetPushSize())
			return false;
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
	//在节点链表的末尾添加节点,key不能等于NULL
	TYPE* Add(const char* key,int CLSTYPE_ID=0,bool loadDefaultObjInfo=true)
	{
		TYPE *pAtom=GetValue(key);
		if(pAtom!=NULL)
			return pAtom;//throw "出现重复key值,可能导致程序异常！";
		if(CreateNewAtom!=NULL)
			pAtom = CreateNewAtom(CLSTYPE_ID,key,m_pContext);
		if(pAtom==NULL)
			return NULL;
		if(loadDefaultObjInfo&&LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(pAtom,key);
		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->pAtom=pAtom;
		strncpy(ptr->key,key,99);
		if(NodeNum<=0)//空链表
		{
			cursor = tail = head = ptr;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
			if(hashTable.GetHashTableSize()<NodeNum)
				RebuildHashtable();
			else
				hashTable.SetValueAt(key,cursor->pAtom);
			return cursor->pAtom;
		}
		//
		ptr->prev = tail;
		ptr->next = NULL;
		tail->next = ptr;
		cursor = tail = ptr;
		NodeNum++;
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,cursor->pAtom);
		return ptr->pAtom;
	}
	//在当前节点的前面添加节点
	TYPE* insert(const char* key,int CLSTYPE_ID=0,bool loadDefaultObjInfo=true)
	{
		TYPE *pAtom=GetValue(key);
		if(pAtom!=NULL)
			return pAtom;//throw "出现重复key值,可能导致程序异常！";
		if(CreateNewAtom!=NULL)
			pAtom = CreateNewAtom(CLSTYPE_ID,key,m_pContext);
		if(pAtom==NULL)
			pAtom = new TYPE;
		if(loadDefaultObjInfo&&LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(pAtom,key);
		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->pAtom=pAtom;
		ptr->bDeleted = FALSE;
		strncpy(ptr->key,key,99);
		if(NodeNum<=0)//空链表
		{
			cursor = tail = head = ptr;
			cursor->prev = NULL;
			cursor->next = NULL;
			NodeNum=1;
			if(hashTable.GetHashTableSize()<NodeNum)
				RebuildHashtable();
			else
				hashTable.SetValueAt(key,cursor->pAtom);
			return cursor->pAtom;
		}

		//在当前节点的前面添加节点
		ptr->next = cursor;
		ptr->prev = cursor->prev;
		//原当前节点处理
		if(cursor->prev!=NULL)
			cursor->prev->next = ptr;
		else	//cursor原为首节点
			head = ptr;
		cursor->prev = ptr;
		cursor=ptr;
		NodeNum++;
		if(hashTable.GetHashTableSize()<NodeNum)
			RebuildHashtable();
		else
			hashTable.SetValueAt(key,cursor->pAtom);
		return ptr->pAtom;
	}

	TYPE* GetValue(const char* key)
	{
		TYPE *pObj=NULL;
		if(hashTable.GetHashTableSize()>0)
			hashTable.GetValueAt(key,pObj);
		return pObj;
	}
	TYPE& operator[](const char* key)//根据索引号获取线段实体
	{
		return *GetValue(key);
	}
	TYPE* GetNext()
	{
		if(cursor&&cursor->next)
		{
			cursor = cursor->next;
			return cursor->pAtom;
		}
		else
			return NULL;
	}
	TYPE* GetPrev()
	{
		if(cursor&&cursor->prev)
		{
			cursor = cursor->prev;
			return cursor->pAtom;
		}
		else
			return NULL;
	}
	TYPE* GetFirst()
	{
		if(head==NULL)
			return NULL;
		else
		{
			cursor = head;
			return cursor->pAtom;
		}
	}
	TYPE* GetTail()
	{
		if(tail==NULL)
			return NULL;
		else
		{
			cursor = tail;
			return cursor->pAtom;
		}
	}
	TYPE* GetCursor()
	{
		if(cursor)
			return cursor->pAtom;
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
			if(temp->pAtom==pObj)
			{
				if(key)
					strcpy(key,temp->key);
				return true;
			}
		}
		return false;
	}
	//移动当前指定项位置
	bool Move(TYPE *pObj,int head0_prev1_next2_tail3)
	{
		DATA_TYPE *temp=NULL;
		for(temp=head;temp;temp=temp->next)
		{
			if(temp->pAtom==pObj)
				break;
		}
		if(temp==NULL)
			return false;
		if(head0_prev1_next2_tail3==0)
		{	//移动当前节点至首节点
			if(temp==head)
				return false;
			temp->prev->next=temp->next;
			if(temp->next)
				temp->next->prev=temp->prev;
			else //当前节点为尾节点，上移后temp->prev为尾节点
				tail=temp->prev;
			temp->prev=NULL;
			temp->next=head;
			head->prev=temp;
			head=temp;
		}
		else if(head0_prev1_next2_tail3==1)
		{	//将当前节点上移一位
			if(temp==head)
				return false;
			DATA_TYPE *pPrev=temp->prev;
			if(pPrev==NULL)
				return false;
			DATA_TYPE *pPrevPrev=pPrev->prev;
			if(pPrevPrev)
				pPrevPrev->next=temp;
			else //移动后当前节点为头结点
				head=temp;
			if(temp->next)
				temp->next->prev=pPrev;
			else //当前节点为尾节点，上移后pPrev为尾节点
				tail=pPrev;
			pPrev->next=temp->next;
			pPrev->prev=temp;
			temp->next=pPrev;
			temp->prev=pPrevPrev;
		}
		else if(head0_prev1_next2_tail3==2)
		{	//将当前节点下移一位
			if(temp==tail)
				return false;
			DATA_TYPE *pNext=temp->next;
			if(pNext==NULL)
				return false;
			DATA_TYPE *pNextNext=pNext->next;
			if(pNextNext)
				pNextNext->prev=temp;
			else //移动后当前节点为尾结点
				tail=temp;
			if(temp->prev)
				temp->prev->next=pNext;
			else //当前节点为头结点,上移后pNext为头结点
				head=pNext;
			pNext->next=temp;
			pNext->prev=temp->prev;
			temp->prev=pNext;
			temp->next=pNextNext;
		}
		else if(head0_prev1_next2_tail3==3)
		{	//将当前节点移至尾部
			if(temp==tail)
				return false;
			if(temp->prev)
				temp->prev->next=temp->next;
			else //当前节点为头结点,置尾后temp->next为头结点
				head=temp->next;
			temp->next->prev=temp->prev;
			//
			temp->prev=tail;
			temp->next=NULL;
			tail->next=temp;
			tail=temp;
		}
		return true;
	}

	DWORD GetNodeNum()const{return NodeNum;}
	// 删除一个节点，自动定位到前一个合法节点，删除节点为首节点时定位到下一节点
	BOOL DeleteNode(const char* key)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(strcmp(cursor->key,key)==0)
				return DeleteCursor();
		}
		return FALSE;
	}
	// 删除当前节点
	BOOL DeleteCursor(BOOL bClean=TRUE)
	{
		if(cursor==NULL)
			return FALSE;
		else
		{
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
				hashTable.RemoveKey(cursor->key);
				if(DeleteAtom)
					DeleteAtom(cursor->pAtom);
				else 
					delete cursor->pAtom;
				delete cursor;
				cursor=cursor2;
			}
			else 
				cursor->bDeleted=TRUE;
			NodeNum--;
			if(NodeNum<0)
				NodeNum=0;
			return TRUE;
		}
	}
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
				if(DeleteAtom)
					DeleteAtom(cursor->pAtom);
				else 
					delete cursor->pAtom;
				delete cursor;
			}
			cursor=cursor2;
		}
	}
	void Destroy()		// 销毁整个当前类实例
	{
		~CSuperHashStrList();
	}
	void Empty()		// 清空当前类实例的所有元素
	{
		POS_STACK.Empty();
		cursor=tail;
		while(cursor!=NULL)
		{
			cursor = cursor->prev;
			if(tail)
			{
				if(DeleteAtom)
					DeleteAtom(tail->pAtom);
				else 
					delete tail->pAtom;
			}
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
		hashTable.GetValueAt(oldkey,ret_obj_ptr);
		if(ret_obj_ptr)
		{
			hashTable.RemoveKey(oldkey);
			hashTable.SetValueAt(newkey,ret_obj_ptr);
		}	
		return ret_obj_ptr;	
	}
	void EmptyStack(){POS_STACK.Empty();}
};
#endif