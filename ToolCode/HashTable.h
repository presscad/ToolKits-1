#ifndef __HASH_TABLE_H_
#define __HASH_TABLE_H_
#include "list.h"
//Hash��ģ����
template <class TYPE> class CHashTable
{
//1.˽�г�Ա��������
	typedef struct tagDATA_TYPE
	{
		DWORD key;
		TYPE atom;
		tagDATA_TYPE *next;
	}DATA_TYPE;
public:
    CHashTable(DWORD HashTblSize=0)
	{// ���캯��
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
			Empty();	//����������ٴ���
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
    DWORD m_nHashSize;	// Hash�������洢TYPEԪ�صĸ�������Hash��Ĵ�С��
    DATA_TYPE* data;// Hash����ָ������ݵ�ָ��

//2.���г�Ա��������
public:
	DWORD (*HashFunc)(DWORD key,DWORD nHashSize);
//3.˽�г�Ա��������
private:
	DWORD Hash(DWORD key)const	//��������Ĺؼ���key������Ӧ��Hashӳ��ֵ
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

//4.���г�Ա��������
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
		if(data==NULL||key==0)	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key);
		if(i<0)//=0)	//������Ĺ�ϣֵ������0
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
		if(data==NULL||key==0)	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key);
		if(i<0)//=0)	//������Ĺ�ϣֵ������0
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
    bool SetValueAt(DWORD key,TYPE val)//���������Ż�ȡ�߶�ʵ��
	{
		bool bOk=false;
		if(data==NULL||key==0)	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key);
		if(i<0)		//������Ĺ�ϣֵ����ϣ��δ��ʼ��ʱ������0
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
		if(data==NULL||key==0)	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key);
		if(i<0)//=0)	//������Ĺ�ϣֵ������0
			return false;
		DATA_TYPE *prev=NULL,*cursor=&data[i];
		while(cursor)
		{
			if(cursor->key==key)
			{
				if(prev==NULL)
				{
					if(data[i].next)
					{	//��ǰ�ڵ�֮�������������ʱ��ֱ����յ�ǰ�ڵ㽫�������ݶ�ʧ wht 12-08-30
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
    bool Delete(DWORD key)	// ɾ��һ���ڵ�
	{
		bool bOk=false;
		if(data==NULL||key==0)	//δ��ʼ�����ֵΪ0(����Ϊ0)
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
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
	{
		for(DWORD i=0;i<m_nHashSize;i++)
		{
			if(data[i].next)
			{	//����Ԫ�ص�
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
    void EmptyKeys()		// ��յ�ǰ��ʵ��������Ԫ��
	{
		for(DWORD i=0;i<m_nHashSize;i++)
		{
			if(data[i].next)
			{	//����Ԫ�ص�
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
//1.˽�г�Ա��������
	typedef struct tagDATA_TYPE
	{
		HASH2KEY key;
		TYPE atom;
		tagDATA_TYPE *next;
	}DATA_TYPE;
public:
    CHashMap(DWORD HashTblSize=0)
	{// ���캯��
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
			Empty();	//����������ٴ���
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
    DWORD m_nHashSize;	// Hash�������洢TYPEԪ�صĸ�������Hash��Ĵ�С��
    DATA_TYPE* data;// Hash����ָ������ݵ�ָ��

//2.���г�Ա��������
public:
	DWORD (*HashFunc)(DWORD key1,DWORD key2,DWORD nHashSize);
//3.˽�г�Ա��������
private:
	DWORD Hash(DWORD key1,DWORD key2)const	//��������Ĺؼ���key������Ӧ��Hashӳ��ֵ
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

//4.���г�Ա��������
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
		if(data==NULL||(key1==0&&key2==0))	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key1,key2);
		if(i<0)//=0)	//������Ĺ�ϣֵ������0
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
		if(data==NULL||(key1==0&&key2==0))	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key1,key2);
		if(i<0)//=0)	//������Ĺ�ϣֵ������0
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
    bool SetValueAt(DWORD key1,DWORD key2,TYPE val)//���������Ż�ȡ�߶�ʵ��
	{
		bool bOk=false;
		if(data==NULL||(key1==0&&key2==0))	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key1,key2);
		if(i<0)//=0)	//������Ĺ�ϣֵ������0
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
		if(data==NULL||(key1==0&&key2==0))	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key1,key2);
		if(i<0)//=0)	//������Ĺ�ϣֵ������0
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
    bool Delete(DWORD key1,DWORD key2)	// ɾ��һ���ڵ�
	{
		bool bOk=false;
		if(data==NULL||(key1==0&&key2==0))	//δ��ʼ�����ֵΪ0(����Ϊ0)
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
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
	{
		for(DWORD i=0;i<m_nHashSize;i++)
		{
			if(data[i].next)
			{	//����Ԫ�ص�
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
    void EmptyKeys()		// ��յ�ǰ��ʵ��������Ԫ��
	{
		for(DWORD i=0;i<m_nHashSize;i++)
		{
			if(data[i].next)
			{	//����Ԫ�ص�
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
//CStringKeyHashTable���ƹ���������ΪCHashStrTable wjh-2014.6.24
#define CStringKeyHashTable CHashStrTable
template <class TYPE> class CHashStrTable
{
public:
    CHashStrTable(DWORD HashTblSize=0)
	{// ���캯��
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
			return false;	//����������ٴ���
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
//1.˽�г�Ա��������
private:			// ע:�������������±ꡱ
	typedef struct tagDATA_TYPE
	{
		char key[100];
		TYPE atom;
		tagDATA_TYPE *next;
	}DATA_TYPE;
    DWORD m_nHashSize;	// Hash�������洢TYPEԪ�صĸ�������Hash��Ĵ�С��
    DATA_TYPE* data;// Hash����ָ������ݵ�ָ��

//2.���г�Ա��������
public:
	DWORD (*HashFunc)(const char *key,DWORD nHashSize,BYTE cnMaxKeyBufSize);	//��������Ĺؼ���key������Ӧ��Hashӳ��ֵ
//3.˽�г�Ա��������
private:
	DWORD Hash(const char *key)const	//��������Ĺؼ���key������Ӧ��Hashӳ��ֵ
	{
		if(HashFunc)
			return HashFunc(key,m_nHashSize,100);
		ULONG hashkey=0,keylen=(int)strlen(key);
		/*for(int i=0;i<keylen;i++)
			hashkey+=key[i];*/
		//keyΪ��ʱĬ����"CHashStrTable:NULL-KEY"��Ϊ��ֵ,
		//������������ٴ��ÿ��ַ�������ֵ��ѯʱ������Чֵ wht 16-12-05
		char sNullKey[]="CHashStrTable:NULL-KEY";
		const char *sRealKey=keylen>0?key:sNullKey;
		if(keylen<=0)
			keylen=(int)strlen(sRealKey);
		union KEY_VALUE
		{
			BYTE key[4]; //0.A1 1.A2 2.A3 3.A4
			DWORD dwKey; //A4A3A2A1 �Ӹߵ���
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
		/* //hashkeyΪULONG �˴�������ָ��� 
		if(hashkey<0)	//keyֵ��������ʱhashkey����Ϊ��ֵ wht 10-09-08
			hashkey*=-1;
		*/
		return hashkey%m_nHashSize;
	}

//4.���г�Ա��������
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
		if(i<0)//=0)	//������Ĺ�ϣֵ������0
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
		if(data==NULL||key==0)	//δ��ʼ�����ֵΪ0(����Ϊ0)
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
    bool SetValueAt(const char *key,TYPE val)//���������Ż�ȡ�߶�ʵ��
	{
		bool bOk=false;
		if(data==NULL||key==NULL)	//δ��ʼ�����ֵΪ0(����Ϊ0)
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
    bool Delete(const char *key)	// ɾ��һ���ڵ�
	{
		bool bOk=false;
		if(data==NULL||key==null)	//δ��ʼ�����ֵΪ0(����Ϊ0)
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
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
	{
		for(DWORD i=0;i<m_nHashSize;i++)
		{
			if(data[i].next)
			{	//����Ԫ�ص�
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
    void EmptyKeys()		// ��յ�ǰ��ʵ��������Ԫ��
	{
		for(DWORD i=0;i<m_nHashSize;i++)
		{
			if(data[i].next)
			{	//����Ԫ�ص�
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
		if(data==NULL||key==NULL)	//δ��ʼ�����ֵΪ0(����Ϊ0)
			return false;
		DWORD i = Hash(key);
		if(i<0)//=0)	//������Ĺ�ϣֵ������0
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
//��ͨ��ջ��
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
	TYPE* TopAtom()		//����ջ��Ԫ��
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
//����ģ����
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
    DWORD NodeNum;			// �ܸ���
    DATA_TYPE* cursor;		// ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��
public:
    CHashList()
	{// ���캯��
		unique_id=0;
		m_dwMinmalId=1;
		NodeNum=0;
		cursor = head = tail = NULL;
		m_uHashTableGrowSize=500;
		LoadDefaultObjectInfo=NULL;
		AllocateUniqueIdFunc=NULL;
	}
    ~CHashList(){Empty();}

//2.���г�Ա��������
public:
	void SetHashFunc(DWORD (*hash_func)(DWORD key,DWORD nHashSize)){hashTable.HashFunc=hash_func;}
	DWORD (*AllocateUniqueIdFunc)(void* pContext);						//��ѡ���趨�ص�����
	void (*LoadDefaultObjectInfo)(TYPE *pObj,DWORD key);
public:	//�������ӿ�ָ��
	virtual TYPE* MoveFirst(){return GetFirst();}
	virtual TYPE* MoveNext(){return GetNext();}
	virtual TYPE* Append(DWORD id=0){return Add(id);}
	virtual TYPE* Append(TYPE obj,DWORD key=0)//�ڽڵ������ĩβ��ӽڵ�
	{
		if(key>0)
			return SetValue(key,obj);
		else
			return SetValue(key,obj,true);
	}
	virtual TYPE* getCurrent(){return GetCursor();}
	virtual UINT  getCount(){return GetNodeNum();}
//3.˽�г�Ա��������
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
			hashSize=50;	//��С��ϣ���СΪ50;
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
	//��ȡһ���µľֲ�Ψһ��ʶ���ڵ�ǰ��ϣ�����ڣ�
	DWORD GetLocalUniqueId()
	{
		DWORD testid=m_dwMinmalId;
		DATA_TYPE* indicator=tail;
		while(testid<10000000){	//��10000000��Ϊ�˷�ֹ��һ����ʱ������ѭ��
			if(hashTable.GetHashTableSize()>0&&hashTable.ContainsKey(testid))
			{
				if(indicator&&indicator->key>testid)
					testid=indicator->key+1;
				else
					testid++;
				if(indicator)
					indicator=indicator->prev;
			}
			else	//��ǰtestid����Ψһ��ʶ
				break;
		}
		return testid;
	}
//4.���г�Ա��������
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
	//��ʱ��GetNewId��������������Ŀ����Ϊ�˱�������дʱ������ʾ��GetNext�������ţ������ú�����������ܱ�AllocateUniqueIdȡ��
	long NewUniqueId(){
		unique_id++;
		return unique_id;
	}
	TYPE* ModifyKeyId(DWORD oldkey, DWORD newkey)
	{
		if(newkey==oldkey)	//��ֵδ�����仯
			return GetValue(newkey);
		else if(GetValue(newkey)!=NULL)
			return FALSE;	//�¼�ֵ�ѱ�ռ��
		else if(GetValue(oldkey)==NULL)
			return FALSE;	//�ɼ�ֵδ�洢����
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
 	//ii==-1ʱ���ڵ�ǰ�ڵ��ǰ����ӽڵ㣻������index��ָ��ڵ�ǰ��ӣ��ú��������ڰ������������ wjh-2014.8.25
    TYPE* insert(const TYPE &obj,long ii=-1,DWORD key=0)
	{
		TYPE *pObj=NULL;
		if(key<=0)	//key<0ʱ�Զ�����keyֵ
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

		if(NodeNum<=0)//������
		{
    		cursor = head = tail = ptr;
			cursor->prev = NULL;
			cursor->next = NULL;
		}
		else if(ii < 0)//�Ե�ǰ�ڵ�λ��Ϊ�½ڵ����λ��
		{
			ptr->next = cursor;
  			ptr->prev = cursor->prev;
				//ԭ��ǰ�ڵ㴦��
			if(cursor->prev!=NULL)
    			cursor->prev->next = ptr;
			else	//cursorԭΪ�׽ڵ�
				head = ptr;
			cursor->prev = ptr;
		}
		else if(ii>=0&&ii<(long)NodeNum)//ָ�������λ��
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
        		//��������ڵ���������ԭ�������ӹ�ϵ
			ptr->next = cursor;
  			ptr->prev = cursor->prev;
				//����λ�ô�ԭ�ڵ㴦��
			if(cursor->prev!=NULL)
    			cursor->prev->next = ptr;
			else if(cursor->prev==NULL)//ָ�������ԭΪ�����׽ڵ�
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
    TYPE* Add(DWORD key)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pObj=NULL;
		if(key<=0)	//key<0ʱ�Զ�����keyֵ
			key=AllocateUniqueId();
		else 
			pObj=GetValue(key);
		if(pObj!=NULL)
			return pObj;
		else if(NodeNum<=0)//������
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
	//allocZeroKey=trueʱ�����key=0ϵͳ���Զ�����һ��Ψһ��ʶkey��allocZeroKey=falseʱ����ֱ�ӷ��ؿ�
    TYPE* SetValue(DWORD key, TYPE obj,bool allocZeroKey=false)//�ڽڵ������ĩβ��ӽڵ�
	{
		if(key==0&&!allocZeroKey)	//��ΪHashtable��֧��keyֵΪ0,����key����Ϊ0 wjh-2013.2.23
			return NULL;
		TYPE *pObj=NULL;
		if(key==0)	//key<0ʱ�Զ�����keyֵ
			key=AllocateUniqueId();
		else 
			pObj=GetValue(key);
		if(pObj!=NULL)
		{
			*pObj=obj;
			hashTable.SetValueAt(key,pObj);
			//��ֹ����SetValueʱ��ֵ�ظ�����obj.keyδ��ʼ����һ����������쳣���� wjh-2016.9.12
			if(LoadDefaultObjectInfo)
				LoadDefaultObjectInfo(pObj,key);
			return pObj;
		}
		else if(NodeNum<=0)//������
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
	{	//ʹ��[]ʱ�޷����ݷ���ֵ�ж��Ƿ��ҵ���Ӧ�Ĵ洢���ݣ�Ӧ����
		//�磺TYPEΪָ�����ͣ�����һ�������ڵ�keyʱ�᷵��һ�������ָ��,������NULL wht 12-09-19
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
			return &cursor->atom;	//����һ���ڵ�Ϊĩ�ڵ����ѱ���Ϊɾ����־ʱ������쳣 wjh-2013.10.12
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
	//�������ݵõ���Ӧ��key wht 10-09-07
	DWORD GetKey(const TYPE *pObj)
	{
		DWORD key=0;
		DATA_TYPE *temp=((DATA_TYPE*)((BYTE*)pObj - offsetof(DATA_TYPE, atom)));
		if(temp!=NULL&&GetValue(temp->key)==pObj)
			return temp->key;	//Ŀǰ���ַ����ݲ����ж�pObj�Ƿ������wjh-2014.7.18
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
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
    BOOL DeleteNode(DWORD key)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(!cursor->bDeleted&&cursor->key==key)	//������ɾ���ڵ�
				return DeleteCursor();
		}
		return FALSE;
	}
	// ɾ����ǰ�ڵ�
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
			NodeNum--;	//ֻ���ڶ�δɾ���ڵ����ɾ��ʱ�ڵ�������һ�������ظ�ɾ���ڵ���������
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
			{	//�ѱ�ɾ��Ԫ��,��λ�����
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
    void Destroy()		// ����������ǰ��ʵ��
	{
		~CHashList();
	}
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
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
//����ģ����������ָ�뼯�ϣ����漰�����������ڣ������CHashList������ֱֵ�Ӽ�ΪTYPE_PTR
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
    DWORD NodeNum;			// �ܸ���
    DATA_TYPE* cursor;		// ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��
public:
    CHashSet()
	{// ���캯��
		unique_id=0;
		NodeNum=0;
		cursor = head = tail = NULL;
		m_uHashTableGrowSize=500;
		LoadDefaultObjectInfo=NULL;
	}
    ~CHashSet(){Empty();}

//2.���г�Ա��������
public:
	void SetHashFunc(DWORD (*hash_func)(DWORD key,DWORD nHashSize)){hashTable.HashFunc=hash_func;}
	void (*LoadDefaultObjectInfo)(TYPE_PTR pObj,DWORD key);
//3.˽�г�Ա��������
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
			hashSize=50;	//��С��ϣ���СΪ50;
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
//4.���г�Ա��������
public:
	//��ʱ��GetNewId��������������Ŀ����Ϊ�˱�������дʱ������ʾ��GetNext�������ţ������ú�����������ܱ�AllocateUniqueIdȡ��
	long NewUniqueId(){
		unique_id++;
		return unique_id;
	}
	TYPE_PTR ModifyKeyId(DWORD oldkey, DWORD newkey)
	{
		if(newkey==oldkey)	//��ֵδ�����仯
			return GetValue(newkey);
		else if(GetValue(newkey)!=NULL)
			return FALSE;	//�¼�ֵ�ѱ�ռ��
		else if(GetValue(oldkey)==NULL)
			return FALSE;	//�ɼ�ֵδ�洢����
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
    TYPE_PTR SetValue(DWORD key, TYPE_PTR obj)//�ڽڵ������ĩβ��ӽڵ�
	{
		if(key==0)	//��ΪHashtable��֧��keyֵΪ0,����key����Ϊ0 wjh-2013.2.23
			return NULL;
		TYPE_PTR pObj=GetValue(key);
		if(pObj!=NULL)
		{
			pObj=obj;
			hashTable.SetValueAt(key,pObj);
			return pObj;
		}
		else if(NodeNum<=0)//������
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
	{	//ʹ��[]ʱ�޷����ݷ���ֵ�ж��Ƿ��ҵ���Ӧ�Ĵ洢���ݣ�Ӧ����
		//�磺TYPEΪָ�����ͣ�����һ�������ڵ�keyʱ�᷵��һ�������ָ��,������NULL wht 12-09-19
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
			return cursor->atom;	//����һ���ڵ�Ϊĩ�ڵ����ѱ���Ϊɾ����־ʱ������쳣 wjh-2013.10.12
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
	//�������ݵõ���Ӧ��key wht 10-09-07
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
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
    BOOL DeleteNode(DWORD key)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(!cursor->bDeleted&&cursor->key==key)	//������ɾ���ڵ�
				return DeleteCursor();
		}
		return FALSE;
	}
	// ɾ����ǰ�ڵ�
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
			NodeNum--;	//ֻ���ڶ�δɾ���ڵ����ɾ��ʱ�ڵ�������һ�������ظ�ɾ���ڵ���������
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
			{	//�ѱ�ɾ��Ԫ��,��λ�����
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
    void Destroy()		// ����������ǰ��ʵ��
	{
		~CHashList();
	}
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
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
public:	//IXhSet���غ���
	virtual TYPE_PTR MoveFirst(){return GetFirst();}
	virtual TYPE_PTR MoveNext(){return GetNext();}
	virtual TYPE_PTR getCurrent(){return GetCursor();}
	virtual UINT  getCount(){return GetNodeNum();}
};
//����ģ����(��ָ�����ʽ��������ȫ�������ڣ�������һЩ���Ӷ����ں�����ȸ��ӽṹ���ã�
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
    DWORD NodeNum;			// �ܸ���
    DATA_TYPE* cursor;		// ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��
public:
    CHashPtrList()
	{// ���캯��
		unique_id=0;
		NodeNum=0;
		cursor = head = tail = NULL;
		m_uHashTableGrowSize=500;
		LoadDefaultObjectInfo=NULL;
		DeleteObjectFunc=NULL;
		AllocateUniqueIdFunc=NULL;
	}
	~CHashPtrList(){;Empty();}

//2.���г�Ա��������
public:
	void (*LoadDefaultObjectInfo)(TYPE *pObj,DWORD key);
	void (*DeleteObjectFunc)(TYPE* pObj);
	DWORD (*AllocateUniqueIdFunc)(void* pContext);						//��ѡ���趨�ص�����
//3.˽�г�Ա��������
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
			hashSize=50;	//��С��ϣ���СΪ50;
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
	//��ȡһ���µľֲ�Ψһ��ʶ���ڵ�ǰ��ϣ�����ڣ�
	DWORD GetLocalUniqueId()
	{
		DWORD testid=1;
		DATA_TYPE* indicator=tail;
		while(testid<10000000){	//��10000000��Ϊ�˷�ֹ��һ����ʱ������ѭ��
			if(hashTable.GetHashTableSize()>0&&hashTable.ContainsKey(testid))
			{
				if(indicator&&indicator->key>testid)
					testid=indicator->key+1;
				else
					testid++;
				if(indicator)
					indicator=indicator->prev;
			}
			else	//��ǰtestid����Ψһ��ʶ
				break;
		}
		return testid;
	}
//4.���г�Ա��������
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
//4.���г�Ա��������
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
    TYPE* Add(DWORD key)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pObj=NULL;
		if(key<=0)	//key<0ʱ�Զ�����keyֵ
			key=AllocateUniqueId();
		else 
			pObj=GetValue(key);
		if(pObj!=NULL)
			return pObj;
		else if(NodeNum<=0)//������
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
     TYPE* AddNoneKeyObj(DWORD key)//�ڽڵ������ĩβ��ӽڵ�(�����캯������key������ֵ)wjh-2019.11.11
	{
		TYPE *pObj=NULL;
		if(key<=0)	//key<0ʱ�Զ�����keyֵ
			key=AllocateUniqueId();
		else 
			pObj=GetValue(key);
		if(pObj!=NULL)
			return pObj;
		else if(NodeNum<=0)//������
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
   /*	����SetValue�ᵼ�¶��������ڹ����ӻ����ݲ�֧�ִ˺��� wjh-2014.3.22
	TYPE* SetValue(DWORD key, TYPE* pObj)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pObj=GetValue(key);
		if(pObj!=NULL)
		{
			*pObj=obj;
			return pObj;
		}
		else if(NodeNum<=0)//������
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
    TYPE& operator[](DWORD key)//���������Ż�ȡ�߶�ʵ��
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
			return cursor->pAtom;	//����һ���ڵ�Ϊĩ�ڵ����ѱ���Ϊɾ����־ʱ������쳣 wjh-2013.10.12
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
	//�������ݵõ���Ӧ��key wht 10-09-07
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
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
    BOOL DeleteNode(DWORD key)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(!cursor->bDeleted&&cursor->key==key)	//������ɾ���ڵ�
				return DeleteCursor();
		}
		return FALSE;
	}
	// ɾ����ǰ�ڵ�
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
			NodeNum--;	//ֻ���ڶ�δɾ���ڵ����ɾ��ʱ�ڵ�������һ�������ظ�ɾ���ڵ���������
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
			{	//�ѱ�ɾ��Ԫ��,��λ�����
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
    void Destroy()		// ����������ǰ��ʵ��
	{
		~CHashList();
	}
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
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
    DWORD NodeNum;			// �ܸ���
    DATA_TYPE* cursor;		// ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��
public:
    CMapList()
	{// ���캯��
		NodeNum=0;
		cursor = head = tail = NULL;
		m_uHashTableGrowSize=500;
		LoadDefaultObjectInfo=NULL;
	}
    ~CMapList(){Empty();}

//2.���г�Ա��������
public:
	void SetHashFunc(DWORD (*hash_func)(DWORD key1,DWORD key2,DWORD nHashSize)){hashTable.HashFunc=hash_func;}
	void (*LoadDefaultObjectInfo)(TYPE *pObj,DWORD key1,DWORD key2);
//3.˽�г�Ա��������
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
			hashSize=50;	//��С��ϣ���СΪ50;
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
//4.���г�Ա��������
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
    TYPE* Add(DWORD key1,DWORD key2)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pObj=GetValue(key1,key2);
		if(pObj!=NULL)
			return pObj;
		else if(NodeNum<=0)//������
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
    TYPE* SetValue(DWORD key1,DWORD key2, TYPE obj)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pObj=GetValue(key1,key2);
		if(pObj!=NULL)
		{
			*pObj=obj;
			hashTable.SetValueAt(key1,key2,pObj);
			//��ֹ����SetValueʱ��ֵ�ظ�����obj.keyδ��ʼ����һ����������쳣���� wjh-2016.9.12
			if(LoadDefaultObjectInfo)
				LoadDefaultObjectInfo(pObj,key1,key2);
			return pObj;
		}
		else if(NodeNum<=0)//������
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
			return &cursor->atom;	//����һ���ڵ�Ϊĩ�ڵ����ѱ���Ϊɾ����־ʱ������쳣 wjh-2013.10.12
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
	//�������ݵõ���Ӧ��key wht 10-09-07
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
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
    BOOL DeleteNode(DWORD key1,DWORD key2)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(!cursor->bDeleted&&cursor->key==HASH2KEY(key1,key2))
				return DeleteCursor();
		}
		return FALSE;
	}
	// ɾ����ǰ�ڵ�
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
			NodeNum--;	//ֻ���ڶ�δɾ���ڵ����ɾ��ʱ�ڵ�������һ�������ظ�ɾ���ڵ���������
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
			{	//�ѱ�ɾ��Ԫ��,��λ�����
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
    void Destroy()		// ����������ǰ��ʵ��
	{
		~CHashList();
	}
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
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

//�ַ�����ϣ����ģ����
//CStringKeyHashList���ƹ���������ΪCHashStrList wjh-2014.6.24
#define CStringKeyHashList CHashStrList
template <class TYPE> class CHashStrList
{
//1.˽�г�Ա��������
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
    DWORD NodeNum;		// �ܸ���
    DATA_TYPE* cursor;    // ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��
public:
    CHashStrList()
	{// ���캯��
		NodeNum=0;
		m_uHashTableGrowSize=500;
		cursor = head = tail = NULL;
	}
    ~CHashStrList(){Empty();}

//2.���г�Ա��������
public:
//3.˽�г�Ա��������
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
			hashSize=50;	//��С��ϣ���СΪ50;
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
//4.���г�Ա��������
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
    TYPE* Add(const char *key)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pObj=GetValue(key);
		if(pObj!=NULL)
			return pObj;
		else if(NodeNum<=0)//������
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
    TYPE* SetValue(const char *key, TYPE obj)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pObj=GetValue(key);
		if(pObj!=NULL)
		{
			*pObj=obj;
			hashTable.SetValueAt(key,pObj);
			return pObj;
		}
		else if(NodeNum<=0)//������
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
    TYPE& operator[](const char *key)//���������Ż�ȡ�߶�ʵ��
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
	//�������ݵõ���Ӧ��key wht 10-09-07
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
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
    BOOL DeleteNode(const char *key)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(!cursor->bDeleted&&strcmp(cursor->key,key)==0)	//������ɾ���ڵ�
				return DeleteCursor();
		}
		return FALSE;
	}
	// ɾ����ǰ�ڵ�
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
			NodeNum--;	//ֻ���ڶ�δɾ���ڵ����ɾ��ʱ�ڵ�������һ�������ظ�ɾ���ڵ���������
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
			{	//�ѱ�ɾ��Ԫ��,��λ�����
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
    void Destroy()		// ����������ǰ��ʵ��
	{
		~CHashList();
	}
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
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
		if(strcmp(oldkey,newkey)==0)	//��ֵδ�����仯
			return GetValue(newkey);
		else if(GetValue(newkey)!=NULL)
			return FALSE;	//�¼�ֵ�ѱ�ռ��
		else if(GetValue(oldkey)==NULL)
			return FALSE;	//�ɼ�ֵδ�洢����
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
//ʹ�ø�����ʱ��Ҫ���ûص�����:CreateNewData,DeleteData
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
    DWORD NodeNum;		// �ܸ���
    DATA_TYPE* cursor;    // ��ǰ�߶�ָ��
    DATA_TYPE* tail;		// �߶�����ĩβָ��
    DATA_TYPE* head;		// �߶�������ʼָ��
public:
	void* m_pContext;	//�����Ļ���
    CSuperHashList()
	{// ���캯��
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

//2.���г�Ա��������
public:
	//�ص�����
	TYPE* (*CreateNewAtom)(int idClsType,DWORD key,void* pContext);	//����ص�����
	BOOL (*DeleteAtom)(TYPE *pAtom);				//����ص�����
	long (*AllocateUniqueIdFunc)();						//��ѡ���趨�ص�����
	void (*LoadDefaultObjectInfo)(TYPE *pObj,DWORD key);//һ�㲻��Ҫ�趨
//3.˽�г�Ա��������
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
			hashSize=50;	//��С��ϣ���СΪ50;
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
	//��ȡһ���µľֲ�Ψһ��ʶ���ڵ�ǰ��ϣ�����ڣ�
	DWORD GetLocalUniqueId()
	{
		if(hashTable.GetHashTableSize()==0)
			return 1;
		DWORD testid=1;
		DATA_TYPE* indicator=tail;
		do{	//��10000000��Ϊ�˷�ֹ��һ����ʱ������ѭ��
			if(hashTable.ContainsKey(testid))
			{
				if(indicator&&indicator->key>testid)
					testid=indicator->key+1;
				else
					testid++;
				if(indicator)
					indicator=indicator->prev;
			}
			else	//��ǰtestid����Ψһ��ʶ
				break;
		}while(testid<10000000);
		return testid;
	}
//4.���г�Ա��������
public:
	//��ȡһ���µľֲ�Ψһ��ʶ���ڵ�ǰ��ϣ�����ڣ�
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
	//�ڽڵ������ĩβ��ӽڵ�,key==0(key����Ϊ����)ʱ�Զ�����keyֵ
    TYPE* Add(DWORD key,int CLSTYPE_ID=0,bool loadDefaultObjInfo=true)
	{
		TYPE *pAtom=NULL;
		if(key<=0)	//key<0ʱ�Զ�����keyֵ
			key=AllocateUniqueId();
		else 
		{
			pAtom=GetValue(key);
			if(pAtom!=NULL)
				return pAtom;//throw "�����ظ�keyֵ,���ܵ��³����쳣��";
		}
		if(CreateNewAtom!=NULL)
			pAtom = CreateNewAtom(CLSTYPE_ID,key,m_pContext);
		if(pAtom==NULL)
			return NULL;
		if(loadDefaultObjInfo&&LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(pAtom,key);
		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->pAtom=pAtom;
		if(NodeNum<=0)//������
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
	//�ڵ�ǰ�ڵ��ǰ����ӽڵ�,key==-1ʱ�Զ�����keyֵ
    TYPE* insert(DWORD key,int CLSTYPE_ID=0,bool loadDefaultObjInfo=true)
	{
		TYPE *pAtom=NULL;
		if(key<=0)	//key<0ʱ�Զ�����keyֵ
			key=AllocateUniqueId();
		else 
		{
			TYPE *pAtom=GetValue(key);
			if(pAtom!=NULL)
				return pAtom;//throw "�����ظ�keyֵ,���ܵ��³����쳣��";
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
		if(NodeNum<=0)//������
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
		
		//�ڵ�ǰ�ڵ��ǰ����ӽڵ�
		ptr->next = cursor;
		ptr->prev = cursor->prev;
		//ԭ��ǰ�ڵ㴦��
		if(cursor->prev!=NULL)
			cursor->prev->next = ptr;
		else	//cursorԭΪ�׽ڵ�
			head = ptr;
		cursor->prev = ptr;
		cursor=ptr;	//���������ڵ���Ϊ��ǰ�ڵ�,�������hashTable�������´��� wjh-2016.11.04
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
    TYPE& operator[](DWORD key)//���������Ż�ȡ�߶�ʵ��
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
	//�������ݵõ���Ӧ��key wht 10-09-07
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
	//�ƶ���ǰָ����λ��
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
		{	//�ƶ���ǰ�ڵ����׽ڵ�
			if(temp==head)
				return false;
			temp->prev->next=temp->next;
			if(temp->next)
				temp->next->prev=temp->prev;
			else //��ǰ�ڵ�Ϊβ�ڵ㣬���ƺ�temp->prevΪβ�ڵ�
				tail=temp->prev;
			temp->prev=NULL;
			temp->next=head;
			head->prev=temp;
			head=temp;
		}
		else if(head0_prev1_next2_tail3==1)
		{	//����ǰ�ڵ�����һλ
			if(temp==head)
				return false;
			DATA_TYPE *pPrev=temp->prev;
			if(pPrev==NULL)
				return false;
			DATA_TYPE *pPrevPrev=pPrev->prev;
			if(pPrevPrev)
				pPrevPrev->next=temp;
			else //�ƶ���ǰ�ڵ�Ϊͷ���
				head=temp;
			if(temp->next)
				temp->next->prev=pPrev;
			else //��ǰ�ڵ�Ϊβ�ڵ㣬���ƺ�pPrevΪβ�ڵ�
				tail=pPrev;
			pPrev->next=temp->next;
			pPrev->prev=temp;
			temp->next=pPrev;
			temp->prev=pPrevPrev;
		}
		else if(head0_prev1_next2_tail3==2)
		{	//����ǰ�ڵ�����һλ
			if(temp==tail)
				return false;
			DATA_TYPE *pNext=temp->next;
			if(pNext==NULL)
				return false;
			DATA_TYPE *pNextNext=pNext->next;
			if(pNextNext)
				pNextNext->prev=temp;
			else //�ƶ���ǰ�ڵ�Ϊβ���
				tail=temp;
			if(temp->prev)
				temp->prev->next=pNext;
			else //��ǰ�ڵ�Ϊͷ���,���ƺ�pNextΪͷ���
				head=pNext;
			pNext->next=temp;
			pNext->prev=temp->prev;
			temp->prev=pNext;
			temp->next=pNextNext;
		}
		else if(head0_prev1_next2_tail3==3)
		{	//����ǰ�ڵ�����β��
			if(temp==tail)
				return false;
			if(temp->prev)
				temp->prev->next=temp->next;
			else //��ǰ�ڵ�Ϊͷ���,��β��temp->nextΪͷ���
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
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
    BOOL DeleteNode(DWORD key)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(cursor->key==key)
				return DeleteCursor();
		}
		return FALSE;
	}
	// ɾ����ǰ�ڵ�
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
			{	//�ѱ�ɾ��Ԫ��,��λ�����
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
    void Destroy()		// ����������ǰ��ʵ��
	{
		~CSuperHashList();
	}
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
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
		if(dwOldKey==dwNewKey)	//��ֵδ�����仯
			return GetValue(dwNewKey);
		else if(GetValue(dwNewKey)!=NULL)
			return NULL;	//�¼�ֵ�ѱ�ռ��
		else if(GetValue(dwOldKey)==NULL)
			return NULL;	//�ɼ�ֵδ�洢����
		TYPE* ret_obj_ptr=NULL;
		hashTable.GetValueAt(dwOldKey,ret_obj_ptr);
		if(ret_obj_ptr)
		{
			hashTable.RemoveKey(dwOldKey);
			hashTable.SetValueAt(dwNewKey,ret_obj_ptr);
		}
		return ret_obj_ptr;	
	}
	//�������¶���Keyֵ,��֤����ָ�벻�������¸��¶�Ӧkeyֵ��TAP��˨�����֮���������˨Keyֵ�� wht 16-12-08
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
//ʹ�ø�����ʱ��Ҫ���ûص�����:CreateNewData,DeleteData
//key����ΪNULL��""
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
	DWORD NodeNum;		// �ܸ���
	DATA_TYPE* cursor;    // ��ǰ�߶�ָ��
	DATA_TYPE* tail;		// �߶�����ĩβָ��
	DATA_TYPE* head;		// �߶�������ʼָ��
public:
	void* m_pContext;	//�����Ļ���
	CSuperHashStrList()
	{// ���캯��
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

	//2.���г�Ա��������
public:
	//�ص�����
	TYPE* (*CreateNewAtom)(int idClsType,const char* key,void* pContext);	//����ص�����
	BOOL (*DeleteAtom)(TYPE *pAtom);				//����ص�����
	long (*AllocateUniqueIdFunc)();						//��ѡ���趨�ص�����
	void (*LoadDefaultObjectInfo)(TYPE *pObj,const char* key);//һ�㲻��Ҫ�趨
	//3.˽�г�Ա��������
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
			hashSize=50;	//��С��ϣ���СΪ50;
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
	//4.���г�Ա��������
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
	//�ڽڵ������ĩβ��ӽڵ�,key���ܵ���NULL
	TYPE* Add(const char* key,int CLSTYPE_ID=0,bool loadDefaultObjInfo=true)
	{
		TYPE *pAtom=GetValue(key);
		if(pAtom!=NULL)
			return pAtom;//throw "�����ظ�keyֵ,���ܵ��³����쳣��";
		if(CreateNewAtom!=NULL)
			pAtom = CreateNewAtom(CLSTYPE_ID,key,m_pContext);
		if(pAtom==NULL)
			return NULL;
		if(loadDefaultObjInfo&&LoadDefaultObjectInfo)
			LoadDefaultObjectInfo(pAtom,key);
		DATA_TYPE* ptr = new DATA_TYPE;
		ptr->pAtom=pAtom;
		strncpy(ptr->key,key,99);
		if(NodeNum<=0)//������
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
	//�ڵ�ǰ�ڵ��ǰ����ӽڵ�
	TYPE* insert(const char* key,int CLSTYPE_ID=0,bool loadDefaultObjInfo=true)
	{
		TYPE *pAtom=GetValue(key);
		if(pAtom!=NULL)
			return pAtom;//throw "�����ظ�keyֵ,���ܵ��³����쳣��";
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
		if(NodeNum<=0)//������
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

		//�ڵ�ǰ�ڵ��ǰ����ӽڵ�
		ptr->next = cursor;
		ptr->prev = cursor->prev;
		//ԭ��ǰ�ڵ㴦��
		if(cursor->prev!=NULL)
			cursor->prev->next = ptr;
		else	//cursorԭΪ�׽ڵ�
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
	TYPE& operator[](const char* key)//���������Ż�ȡ�߶�ʵ��
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
	//�������ݵõ���Ӧ��key wht 10-09-07
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
	//�ƶ���ǰָ����λ��
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
		{	//�ƶ���ǰ�ڵ����׽ڵ�
			if(temp==head)
				return false;
			temp->prev->next=temp->next;
			if(temp->next)
				temp->next->prev=temp->prev;
			else //��ǰ�ڵ�Ϊβ�ڵ㣬���ƺ�temp->prevΪβ�ڵ�
				tail=temp->prev;
			temp->prev=NULL;
			temp->next=head;
			head->prev=temp;
			head=temp;
		}
		else if(head0_prev1_next2_tail3==1)
		{	//����ǰ�ڵ�����һλ
			if(temp==head)
				return false;
			DATA_TYPE *pPrev=temp->prev;
			if(pPrev==NULL)
				return false;
			DATA_TYPE *pPrevPrev=pPrev->prev;
			if(pPrevPrev)
				pPrevPrev->next=temp;
			else //�ƶ���ǰ�ڵ�Ϊͷ���
				head=temp;
			if(temp->next)
				temp->next->prev=pPrev;
			else //��ǰ�ڵ�Ϊβ�ڵ㣬���ƺ�pPrevΪβ�ڵ�
				tail=pPrev;
			pPrev->next=temp->next;
			pPrev->prev=temp;
			temp->next=pPrev;
			temp->prev=pPrevPrev;
		}
		else if(head0_prev1_next2_tail3==2)
		{	//����ǰ�ڵ�����һλ
			if(temp==tail)
				return false;
			DATA_TYPE *pNext=temp->next;
			if(pNext==NULL)
				return false;
			DATA_TYPE *pNextNext=pNext->next;
			if(pNextNext)
				pNextNext->prev=temp;
			else //�ƶ���ǰ�ڵ�Ϊβ���
				tail=temp;
			if(temp->prev)
				temp->prev->next=pNext;
			else //��ǰ�ڵ�Ϊͷ���,���ƺ�pNextΪͷ���
				head=pNext;
			pNext->next=temp;
			pNext->prev=temp->prev;
			temp->prev=pNext;
			temp->next=pNextNext;
		}
		else if(head0_prev1_next2_tail3==3)
		{	//����ǰ�ڵ�����β��
			if(temp==tail)
				return false;
			if(temp->prev)
				temp->prev->next=temp->next;
			else //��ǰ�ڵ�Ϊͷ���,��β��temp->nextΪͷ���
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
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
	BOOL DeleteNode(const char* key)
	{
		for(cursor=head;cursor;cursor=cursor->next)
		{
			if(strcmp(cursor->key,key)==0)
				return DeleteCursor();
		}
		return FALSE;
	}
	// ɾ����ǰ�ڵ�
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
			{	//�ѱ�ɾ��Ԫ��,��λ�����
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
	void Destroy()		// ����������ǰ��ʵ��
	{
		~CSuperHashStrList();
	}
	void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
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
		if(strcmp(oldkey,newkey)==0)	//��ֵδ�����仯
			return GetValue(newkey);
		else if(GetValue(newkey)!=NULL)
			return FALSE;	//�¼�ֵ�ѱ�ռ��
		else if(GetValue(oldkey)==NULL)
			return FALSE;	//�ɼ�ֵδ�洢����
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