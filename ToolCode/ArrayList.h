#ifndef __ARRAY_LIST_H_
#define __ARRAY_LIST_H_

template<class TYPE> struct IXhArray{
	virtual DWORD Size() const=0;
	virtual TYPE* Data() = 0;
	virtual TYPE* GetAt(UINT i) = 0;
	//operator TYPE*(){return Data();}
	virtual TYPE& At(UINT i)=0;
};

//���¶�̬������������������Զ��ͷ��ڴ棬Ҳ������;ͨ��Resize�ı䶯̬����Ĵ�С
template<class TYPE> class DYN_ARRAY : public IXhArray<TYPE>{
	DWORD m_size;
	DWORD m_bufSize;
	DWORD m_nBuffGrowSize;
	TYPE* m_data;
	bool m_bExternalMemory;
public:
	DYN_ARRAY(DWORD size=0,bool initNewDataZero=false,TYPE* exterdata=NULL,UINT exterdatacount=0){
		m_size=m_bufSize=0;
		m_nBuffGrowSize=1;
		m_data=NULL;
		m_bExternalMemory=false;
		if(size>0)
			Resize(size,initNewDataZero,exterdata,exterdatacount);
	}
	~DYN_ARRAY(){
		if(m_bufSize>0&&!m_bExternalMemory)
			delete []m_data;
	}
	virtual DWORD Size() const{return m_size;}
	__declspec(property(get = Size)) DWORD Count;
	virtual TYPE* Data(){return m_data;}
	long SetBuffGrowSize(long nGrowSize=1){return m_nBuffGrowSize=nGrowSize;}
	long GetBuffGrowSize(){return m_nBuffGrowSize;}
	_declspec(property(get=GetBuffGrowSize)) long BuffGrowSize;
	virtual TYPE& At(UINT i)
	{//���������Ż�ȡ�߶�ʵ��
		if (i < 0 || i >= m_size)
			throw "�����ַԽ�磡";
		return m_data[i];
	}
	virtual TYPE* GetAt(UINT i)
	{//���������Ż�ȡ�߶�ʵ��
		if (i < 0 || i >= m_size)
			return NULL;
		return &m_data[i];
	}
	virtual TYPE& Get(UINT i){return At(i);}
	operator TYPE*(){return m_data;}
	DYN_ARRAY<TYPE>& operator=(const DYN_ARRAY<TYPE> &atom)
	{
		if(m_bufSize>0&&!m_bExternalMemory)
		{
			delete []m_data;
			m_data=NULL;
		}
		DYN_ARRAY<TYPE> *pAtom=const_cast<DYN_ARRAY<TYPE>*>(&atom);
		m_bufSize=m_size=atom.Size();
		if(m_size>0)
		{
			m_data = new TYPE[m_size];
			m_bExternalMemory=false;
			memcpy(m_data,(TYPE*)pAtom->Data(),sizeof(TYPE)*m_size);
		}
		return *this;
	}
	bool Resize(DWORD size,bool initNewDataZero=false,TYPE* exterdata=NULL,UINT exterdatacount=0)
	{
		TYPE* data=NULL;
		if(size>m_bufSize)
		{
			UINT uBuffSize=size;
			if(m_nBuffGrowSize>1)
			{
				UINT extrasize=size%m_nBuffGrowSize;
				extrasize=extrasize>0?m_nBuffGrowSize-extrasize:0;
				uBuffSize+=extrasize;
			}
			bool bReallocMemory=false;
			if(exterdata!=NULL&&uBuffSize<=exterdatacount)
				data=exterdata;
			else
			{
				data=new TYPE[uBuffSize];
				bReallocMemory=true;
			}
			if(data==NULL)
				return false;
			if(initNewDataZero)
				memset(data,0,sizeof(TYPE)*uBuffSize);
			if(m_size>0)
				memcpy(data,m_data,sizeof(TYPE)*min(size,m_size));
			if(m_bufSize>0&&!m_bExternalMemory)
				delete []m_data;
			m_bExternalMemory=!bReallocMemory;
			m_bufSize=uBuffSize;
			m_size=size;
			m_data=data;
		}
		else
			m_size=size;
		if(m_data)
			return true;
		else
			return false;
	}
	void Clear()
	{
		m_size=0;
		memset(m_data,0,sizeof(TYPE)*m_bufSize);
	}
	void Empty()
	{
		if(m_bufSize>0&&m_data!=NULL&&!m_bExternalMemory)
			delete []m_data;
		m_bufSize=m_size=0;
		m_data=NULL;
	}
};
typedef DYN_ARRAY<unsigned char> BYTE_ARRAY;
typedef DYN_ARRAY<char> CHAR_ARRAY;
typedef DYN_ARRAY<long> INT_ARRAY;
typedef DYN_ARRAY<double> DOUBLE_ARRAY;

//����ģ����
template <class TYPE> class ARRAY_LIST : public IXhArray<TYPE>
{
	bool m_bExternalMemory;
public:
	//ARRAY_LIST(long nInitSize = 0, long nGrowBy = 1)
    ARRAY_LIST(long nGrowBy=1)
	{// ���캯��
		index = 0;
		m_nSize= m_nMaxSize = 0;
		m_nGrowBy=nGrowBy;
		m_pData=NULL;
		m_bExternalMemory=false;
	}
    ARRAY_LIST(TYPE* pExterData, UINT uiExterDataCount, UINT nGrowBy=1)
	{// ���캯��
		index = 0;
		if (pExterData == NULL)
		{	//���ݾɰ�ARRAY_LIST(long nInitSize=0,long nGrowBy=1)
			m_nSize = m_nMaxSize = 0;
			m_nGrowBy = (long)(nGrowBy > uiExterDataCount ? nGrowBy : max(uiExterDataCount, 1));
			m_pData = NULL;
			m_bExternalMemory = false;
		}
		else if (pExterData!=NULL)
		{
			if (uiExterDataCount == 0)
			{
				m_pData = NULL;
				m_bExternalMemory = false;
			}
			else
			{
				m_pData = pExterData;
				m_bExternalMemory = true;
			}
			m_nSize = 0;
			m_nMaxSize = uiExterDataCount;
			m_nGrowBy = max(nGrowBy,1);
		}
	}
   ~ARRAY_LIST(){Empty();}

//1.˽�г�Ա��������
private:			// ע:�������������±ꡱ
	long index;			// �׽ڵ�������(�±�)Ϊ1
	long m_nSize;
	long m_nMaxSize;
	long m_nGrowBy;
//2.���г�Ա��������
public:
	TYPE* m_pData;   // the actual array of data
//3.˽�г�Ա��������
private:

//4.���г�Ա��������
public:
    TYPE* append()//�ڽڵ������ĩβ��ӽڵ�
	{
		if(m_nSize>=m_nMaxSize)
		{
			TYPE* pData=new TYPE[m_nMaxSize+m_nGrowBy];
			if(m_pData)
			{
				memcpy(pData,m_pData,sizeof(TYPE)*m_nMaxSize);
				if (!m_bExternalMemory)
					delete[]m_pData;
			}
			m_bExternalMemory = false;
			m_pData=pData;
			m_nMaxSize+=m_nGrowBy;
		}
		m_nSize++;
		index=m_nSize-1;
		return &m_pData[m_nSize-1];
	}
    TYPE* append(const TYPE &atom)//�ڽڵ������ĩβ��ӽڵ�
	{
		TYPE *pAtom = append();
		*pAtom = atom;
		return pAtom;
	}
	//ii==-1ʱ���ڵ�ǰ�ڵ��ǰ����ӽڵ㣻������index��ָ��ڵ�ǰ���
    TYPE* insert(const TYPE &atom, long ii=-1)
	{
		if(m_nSize>=m_nMaxSize)
		{
			TYPE* pData=new TYPE[m_nMaxSize+m_nGrowBy];
			if(m_pData)
			{
				memcpy(pData,m_pData,sizeof(TYPE)*m_nMaxSize);
				if(!m_bExternalMemory)
					delete[]m_pData;
			}
			m_bExternalMemory = false;
			m_pData=pData;
			m_nMaxSize+=m_nGrowBy;
		}
		if(ii>=0&&ii<m_nSize)
		{	//��ָ��λ�ò���ڵ�
			index=ii;
			memmove(m_pData+ii+1,m_pData+ii,sizeof(TYPE)*(m_nSize-ii));
		}
		else 
			index=m_nSize;
		m_nSize++;
		TYPE *pAtom=&m_pData[index];
		*pAtom=atom;
		return pAtom;
	}
	long GetCurrentIndex()const//��ȡ��ǰ�ڵ��������
	{
		return index;
	}

    TYPE* GetByIndex(long ii)
	{
		if(ii<0||ii>=m_nSize)
    		return NULL;
		return &m_pData[ii];
	}
	TYPE& SetAt(long ii,TYPE& obj)
	{
		if(ii<0||ii>=m_nMaxSize)
    		throw "�����ַԽ�磡";
		m_pData[ii]=obj;
		if(ii>=m_nSize)
			m_nSize=ii+1;
		return m_pData[ii];
	}
	virtual TYPE& Get(UINT i){return (*this)[i];}
	virtual TYPE& At(UINT i){return (*this)[i];}
	virtual TYPE* GetAt(UINT i)
	{//���������Ż�ȡ�߶�ʵ��
		if (i < 0 || i >= (UINT)m_nSize)
			return NULL;
		return &m_pData[i];
	}
	TYPE& operator[](long ii)//���������Ż�ȡ�߶�ʵ��
	{
		if(ii<0||ii>=m_nSize)
    		throw "�����ַԽ�磡";
		return m_pData[ii];
	}
    TYPE* GetNext()
	{
		while(index<m_nSize-1)
		{
			index++;
			return &m_pData[index];
		}
		return NULL;
	}
    TYPE* GetPrev()
	{
		while(index>0)
		{
			index--;
			return &m_pData[index];
		}
		return NULL;
	}
    TYPE* GetFirst()
	{
		index=0;
		if(m_nSize>0)
			return &m_pData[index];
		else
			return NULL;
	}
    TYPE* GetTail()
	{
		index=m_nSize-1;
		if(index>=0)
			return &m_pData[index];
		else
		{
			index=0;
			return NULL;
		}
	}
    TYPE* GetCursor()
	{
		if(index<m_nSize)
			return &m_pData[index];
		else
			return NULL;
	}
	void SetSize(long size,long growBy=1)
	{
		if(size>0)
			m_nSize=size;
		else
			m_nSize=0;
		if(growBy>0)
			m_nGrowBy=growBy;
		if(size>m_nMaxSize)
		{
			TYPE* pData=new TYPE[size];
			if(m_pData)
			{
				if(m_nMaxSize>0)
					memcpy(pData,m_pData,sizeof(TYPE)*m_nMaxSize);
				memset(pData+m_nMaxSize,0,sizeof(TYPE)*(size-m_nMaxSize));
				if(!m_bExternalMemory)
					delete[]m_pData;
			}
			else
				memset(pData,0,sizeof(TYPE)*size);
			m_bExternalMemory = false;
			m_pData=pData;
			m_nMaxSize=size;
		}
		else if(m_nMaxSize==0)
			m_pData=NULL;
	}
	long GetSize()const{return m_nSize;}
	__declspec(property(get = GetSize)) long Count;
	virtual DWORD Size() const{return m_nSize;}
	virtual TYPE* Data(){return m_pData;}
	// ɾ��һ���ڵ㣬�Զ���λ��ǰһ���Ϸ��ڵ㣬ɾ���ڵ�Ϊ�׽ڵ�ʱ��λ����һ�ڵ�
    bool DeleteCursor()
	{
		if(index<m_nSize)
		{
			memmove(m_pData+index,m_pData+index+1,sizeof(TYPE)*(m_nSize-index-1));
			m_nSize--;
			return true;
		}
		else
			return false;
	}
    bool RemoveAt(long ii)	// ɾ��һ���ڵ�
	{
		index=ii;
		return DeleteCursor();
	}
    void Destroy()		// ����������ǰ��ʵ��
	{
		~ARRAY_LIST();
	}
    void Empty()		// ��յ�ǰ��ʵ��������Ԫ��
	{
		if (m_pData != NULL&& !m_bExternalMemory)
		{
			delete[]m_pData;
			m_pData = NULL;
			m_nMaxSize = 0;
			m_bExternalMemory = false;
		}
		m_nSize=index=0;
	}
	TYPE* Detach(long* pArraySize=NULL,long* pArrayMaxSize=NULL){
		TYPE* pArrayData=m_pData;
		m_pData=NULL;
		if(pArraySize)
			*pArraySize=this->m_nSize;
		if(pArrayMaxSize)
			*pArrayMaxSize=this->m_nMaxSize;
		m_nSize=m_nMaxSize=0;
		m_bExternalMemory = false;
		return pArrayData;
	}
	TYPE* Attach(TYPE* arraydata,long arraySize,long arrayMaxSize)
	{
		m_pData=arraydata;
		m_nSize=arraySize;
		m_nMaxSize=arrayMaxSize;
		m_bExternalMemory = true;
		return m_pData;
	}
};
///////////////////////////////////////////////////////////////////
//Ԥ��һ��Ԫ�����İ붯̬���飬һ��Ԥ�����������������������ʱ������䶯̬�ڴ� wjh-2017.12.8
struct PRESET_OBJS4  {static const int COUNT=   4;};
struct PRESET_OBJS8  {static const int COUNT=   8;};
struct PRESET_OBJS16 {static const int COUNT=  16;};
struct PRESET_OBJS32 {static const int COUNT=  32;};
struct PRESET_OBJS64 {static const int COUNT=  64;};
struct PRESET_OBJS128{static const int COUNT=128;};
template <class TYPE> struct IXhPresetArray : public IXhArray<TYPE>
{
protected:
	virtual TYPE* GetPresetValArr()=0;
	WORD m_wnValidValCount; //��ǰ���趨������Ч����Ԫ����
	struct LIST_NODE{
		TYPE dataobj;
		LIST_NODE* next;
		LIST_NODE(const TYPE& valobj){
			dataobj=valobj;
			next=NULL;
		}
	}*pAttachValues;
public:
	void ZeroPresetMemory(){
		memset(GetPresetValArr(),0,sizeof(TYPE)*GetCountOfPreset());
	}
	void Clear(){
		m_wnValidValCount=0;
		while(pAttachValues)
		{
			LIST_NODE* pNextNode=pAttachValues->next;
			if(pNextNode==NULL)
			{
				delete pAttachValues;
				pAttachValues=NULL;
				break;
			}
			else
			{
				pAttachValues->next=pNextNode->next;
				delete pNextNode;
			}
		}
	}
	TYPE* Insert(UINT index,const TYPE& valobj)
	{
		if(index<Count)
			return Append(valobj);
		TYPE currobj,prevobj=At(index);
		Set(index,valobj);
		for(UINT i=index+1;i<Count;i++)
		{
			currobj=At(i);
			Set(i,prevobj);
			prevobj=currobj;
		}
		Set(Count,prevobj);
		return &At(Count);
	}
	TYPE* Append(const TYPE& valobj)
	{
		TYPE* pNewDataObj=NULL;
		TYPE* pPresetValArr=GetPresetValArr();
		if(m_wnValidValCount<this->GetCountOfPreset())
		{
			pPresetValArr[m_wnValidValCount]=valobj;
			pNewDataObj=&pPresetValArr[m_wnValidValCount];
		}
		else if(pAttachValues==NULL)
		{
			pAttachValues=new LIST_NODE(valobj);
			pNewDataObj=&pAttachValues->dataobj;
		}
		else
		{
			LIST_NODE* pNextNode=pAttachValues;
			while(pNextNode)
			{
				if(pNextNode->next!=NULL)
					pNextNode=pNextNode->next;
				else
				{
					pNextNode->next=new LIST_NODE(valobj);
					pNewDataObj=&pNextNode->next->dataobj;
					break;
				}
			}
		}
		m_wnValidValCount++;
		return pNewDataObj;
	}
	virtual TYPE& At(UINT index)
	{
		TYPE* pPresetValArr=GetPresetValArr();
		if(index<this->GetCountOfPreset())
			return pPresetValArr[index];
		else
		{
			LIST_NODE* pCurrNode=this->pAttachValues;
			for(UINT i=GetCountOfPreset();i<m_wnValidValCount;i++,pCurrNode=pCurrNode->next)
			{
				if(pCurrNode==NULL)
					throw "Ԥ�������ַԽ��";
				if(i==index)
					return pCurrNode->dataobj;
			}
			throw "Ԥ�������ַԽ��";
		}
	}
	TYPE& operator[](long ii){	return At(ii);	}//���������Ż�ȡ�߶�ʵ��
	virtual TYPE* GetAt(UINT index)
	{
		TYPE* pPresetValArr=GetPresetValArr();
		if(index<this->GetCountOfPreset())
			return &pPresetValArr[index];
		else
		{
			LIST_NODE* pCurrNode=this->pAttachValues;
			for(UINT i=GetCountOfPreset();pCurrNode!=NULL&&i<m_wnValidValCount;i++,pCurrNode=pCurrNode->next)
			{
				if(i==index)
					return &pCurrNode->dataobj;
			}
			return NULL;
		}
	}
	int Set(UINT index,const TYPE& valobj,bool bAutoAppendOnOverlow=true)//����ʵ���趨������<0��ʾ�趨ʧ��
	{
		TYPE* pPresetValArr=GetPresetValArr();
		if(index<this->GetCountOfPreset())
		{
			pPresetValArr[index]=valobj;
			if(index>=m_wnValidValCount)
				m_wnValidValCount=index+1;
			return index;
		}
		else if(pAttachValues==NULL)
		{
			this->pAttachValues=new LIST_NODE(valobj);
			m_wnValidValCount=GetCountOfPreset()+1;
			return GetCountOfPreset();
		}
		else
		{
			LIST_NODE *pPrevNode=this->pAttachValues;
			for(int i=GetCountOfPreset()+1;true;i++)
			{
				if(i==index&&pPrevNode->next!=NULL)
				{
					pPrevNode->next->dataobj=valobj;
					return i;
				}
				else if(pPrevNode->next==NULL)
				{
					pPrevNode->next=new LIST_NODE(valobj);
					m_wnValidValCount++;
					return i;
				}
				else
					pPrevNode=pPrevNode->next;
			}
		}
	}
	virtual TYPE* Data(){return GetPresetValArr();}
	__declspec(property(get = Data)) TYPE* pPresetObjs;
	virtual DWORD ReSize(UINT count){
		if(count<=GetCountOfPreset())
			m_wnValidValCount=count;
		else
		{
			UINT hits=GetCountOfPreset();
			if(pAttachValues)
			{
				LIST_NODE* pCurrNode=pAttachValues;
				while(pCurrNode!=NULL)
				{
					hits++;
					m_wnValidValCount=hits;
					if(count>hits)
					{
						pCurrNode=pCurrNode->next;
						continue;
					}
					else if(hits>=count)
					{
						LIST_NODE* pNextNode=pCurrNode->next;
						pCurrNode->next=NULL;
						while(pNextNode!=NULL)
						{
							LIST_NODE* pDelNode=pNextNode;
							pNextNode=pNextNode->next;
							delete pDelNode;
						}
					}
				}
			}
		}
		return m_wnValidValCount;
	}
	virtual DWORD SetSize(UINT count){return ReSize(count);}
	virtual DWORD Size() const{return get_Count();}
	virtual UINT GetCountOfPreset()=0;
public:
	UINT get_CountOfPreset(){return GetCountOfPreset();};
	__declspec(property(get=get_CountOfPreset)) UINT CountOfPreset;
	UINT get_Count() const{return m_wnValidValCount;};
	__declspec(property(get=get_Count)) UINT Count;
};
template <class TYPE> struct CPresetArray : public IXhPresetArray<TYPE>
{
protected:
	TYPE* m_arrPresetElems;
	UINT m_nPresetCount;
	virtual TYPE* GetPresetValArr(){return m_arrPresetElems;}
	virtual UINT GetCountOfPreset(){return m_nPresetCount;}
public:
	CPresetArray(TYPE* presetelems=NULL,UINT uiPresetCount=0){
		m_nPresetCount=uiPresetCount;
		m_arrPresetElems=NULL;
	};
	~CPresetArray() { Clear(); }
};
template <class TYPE, class TYPE_PRESET_COUNT> class PRESET_ARRAY : public IXhPresetArray<TYPE>
{
protected:
	TYPE presetValArr[TYPE_PRESET_COUNT::COUNT];
	virtual TYPE* GetPresetValArr() { return presetValArr; }
public:
	PRESET_ARRAY() { pAttachValues = NULL; m_wnValidValCount = 0; }
	~PRESET_ARRAY() { Clear(); }
	void CloneFrom(PRESET_ARRAY<TYPE, TYPE_PRESET_COUNT> &objarr)
	{
		if (Count > objarr.Count)
			Clear();
		for (int i = 0; i < Count; i++)
			Set(i, objarr.At(i), true);
	}
public:
	virtual UINT GetCountOfPreset() { return TYPE_PRESET_COUNT::COUNT; };
};
template <class TYPE> class PRESET_ARRAY4 : public PRESET_ARRAY<TYPE,PRESET_OBJS4>
{
};
template <class TYPE> class PRESET_ARRAY8 : public PRESET_ARRAY<TYPE,PRESET_OBJS8>
{
};
template <class TYPE> class PRESET_ARRAY16 : public PRESET_ARRAY<TYPE,PRESET_OBJS16>
{
};
template <class TYPE> class PRESET_ARRAY32 : public PRESET_ARRAY<TYPE,PRESET_OBJS32>
{
};
template <class TYPE> class PRESET_ARRAY64 : public PRESET_ARRAY<TYPE,PRESET_OBJS64>
{
};
template <class TYPE> class PRESET_ARRAY128 : public PRESET_ARRAY<TYPE,PRESET_OBJS128>
{
};
typedef PRESET_ARRAY<long,PRESET_OBJS4>  PRESET_LONG4ARR;
typedef PRESET_ARRAY<long,PRESET_OBJS8>  PRESET_LONG8ARR;
typedef PRESET_ARRAY<long,PRESET_OBJS16> PRESET_LONG16ARR;
typedef PRESET_ARRAY<UINT,PRESET_OBJS4>  PRESET_UINT4ARR;
typedef PRESET_ARRAY<UINT,PRESET_OBJS8>  PRESET_UINT8ARR;
typedef PRESET_ARRAY<UINT,PRESET_OBJS16> PRESET_UINT16ARR;
////////////////////////////////////////////////////////////////////////////////////////////////////
//SIMPLE_ARRAY���PRESET_ARRAYû��������������ͨ��memset(this,0,sizeof())�����ʼ�� wjh-2019.10.2
template <class TYPE, class TYPE_PRESET_COUNT> class SIMPLE_ARRAY
{
protected:
	TYPE presetValArr[TYPE_PRESET_COUNT::COUNT];
	TYPE* GetPresetValArr() { return presetValArr; }
	WORD m_wnValidValCount; //��ǰ���趨������Ч����Ԫ����
	struct LIST_NODE {
		TYPE dataobj;
		LIST_NODE* next;
		LIST_NODE(const TYPE& valobj) {
			dataobj = valobj;
			next = NULL;
		}
	}*pAttachValues;
public:
	void ZeroPresetMemory() {
		memset(GetPresetValArr(), 0, sizeof(TYPE)*GetCountOfPreset());
	}
	void Clear() {
		m_wnValidValCount = 0;
		while (pAttachValues)
		{
			LIST_NODE* pNextNode = pAttachValues->next;
			if (pNextNode == NULL)
			{
				delete pAttachValues;
				pAttachValues = NULL;
				break;
			}
			else
			{
				pAttachValues->next = pNextNode->next;
				delete pNextNode;
			}
		}
	}
	TYPE* Insert(UINT index, const TYPE& valobj)
	{
		if (index < Count)
			return Append(valobj);
		TYPE currobj, prevobj = At(index);
		Set(index, valobj);
		for (UINT i = index + 1; i < Count; i++)
		{
			currobj = At(i);
			Set(i, prevobj);
			prevobj = currobj;
		}
		Set(Count, prevobj);
		return &At(Count);
	}
	TYPE* Append(const TYPE& valobj)
	{
		TYPE* pNewDataObj = NULL;
		TYPE* pPresetValArr = GetPresetValArr();
		if (m_wnValidValCount < this->GetCountOfPreset())
		{
			pPresetValArr[m_wnValidValCount] = valobj;
			pNewDataObj = &pPresetValArr[m_wnValidValCount];
		}
		else if (pAttachValues == NULL)
		{
			pAttachValues = new LIST_NODE(valobj);
			pNewDataObj = &pAttachValues->dataobj;
		}
		else
		{
			LIST_NODE* pNextNode = pAttachValues;
			while (pNextNode)
			{
				if (pNextNode->next != NULL)
					pNextNode = pNextNode->next;
				else
				{
					pNextNode->next = new LIST_NODE(valobj);
					pNewDataObj = &pNextNode->next->dataobj;
					break;
				}
			}
		}
		m_wnValidValCount++;
		return pNewDataObj;
	}
	TYPE& At(UINT index)
	{
		TYPE* pPresetValArr = GetPresetValArr();
		if (index < this->GetCountOfPreset())
		{
			m_wnValidValCount = max(m_wnValidValCount, index + 1);
			return pPresetValArr[index];
		}
		else
		{
			LIST_NODE* pCurrNode = this->pAttachValues;
			for (UINT i = GetCountOfPreset(); i < m_wnValidValCount; i++, pCurrNode = pCurrNode->next)
			{
				if (pCurrNode == NULL)
					throw "Ԥ�������ַԽ��";
				if (i == index)
					return pCurrNode->dataobj;
			}
			throw "Ԥ�������ַԽ��";
		}
	}
	TYPE* GetAt(UINT index)
	{
		TYPE* pPresetValArr = GetPresetValArr();
		if (index < this->GetCountOfPreset())
		{
			m_wnValidValCount = max(m_wnValidValCount, index + 1);
			return &pPresetValArr[index];
		}
		else
		{
			LIST_NODE* pCurrNode = this->pAttachValues;
			for (UINT i = GetCountOfPreset(); pCurrNode != NULL && i < m_wnValidValCount; i++, pCurrNode = pCurrNode->next)
			{
				if (i == index)
					return &pCurrNode->dataobj;
			}
			return NULL;
		}
	}
	int Set(UINT index, const TYPE& valobj, bool bAutoAppendOnOverlow = true)//����ʵ���趨������<0��ʾ�趨ʧ��
	{
		TYPE* pPresetValArr = GetPresetValArr();
		if (index < this->GetCountOfPreset())
		{
			pPresetValArr[index] = valobj;
			if (index >= m_wnValidValCount)
				m_wnValidValCount = index + 1;
			return index;
		}
		else if (pAttachValues == NULL)
		{
			this->pAttachValues = new LIST_NODE(valobj);
			m_wnValidValCount = GetCountOfPreset() + 1;
			return GetCountOfPreset();
		}
		else
		{
			LIST_NODE *pPrevNode = this->pAttachValues;
			for (int i = GetCountOfPreset() + 1; true; i++)
			{
				if (i == index && pPrevNode->next != NULL)
				{
					pPrevNode->next->dataobj = valobj;
					return i;
				}
				else if (pPrevNode->next == NULL)
				{
					pPrevNode->next = new LIST_NODE(valobj);
					m_wnValidValCount++;
					return i;
				}
				else
					pPrevNode = pPrevNode->next;
			}
		}
	}
	TYPE* Data() { return GetPresetValArr(); }
	operator TYPE*() { return GetPresetValArr(); }
public:
	DWORD ReSize(UINT count) {
		if (count <= GetCountOfPreset())
			m_wnValidValCount = count;
		else
		{
			UINT hits = GetCountOfPreset();
			if (pAttachValues)
			{
				LIST_NODE* pCurrNode = pAttachValues;
				while (pCurrNode != NULL)
				{
					hits++;
					m_wnValidValCount = hits;
					if (count > hits)
					{
						pCurrNode = pCurrNode->next;
						continue;
					}
					else if (hits >= count)
					{
						LIST_NODE* pNextNode = pCurrNode->next;
						pCurrNode->next = NULL;
						while (pNextNode != NULL)
						{
							LIST_NODE* pDelNode = pNextNode;
							pNextNode = pNextNode->next;
							delete pDelNode;
						}
					}
				}
			}
		}
		return m_wnValidValCount;
	}
	DWORD SetSize(UINT count) { return ReSize(count); }
	DWORD Size() const { return get_Count(); }
	UINT GetCountOfPreset() { return TYPE_PRESET_COUNT::COUNT; };
	UINT get_CountOfPreset() { return GetCountOfPreset(); };
	__declspec(property(get = get_CountOfPreset)) UINT CountOfPreset;
	UINT get_Count() const { return m_wnValidValCount; };
	__declspec(property(get = get_Count)) UINT Count;
public:
	SIMPLE_ARRAY() { pAttachValues = NULL; m_wnValidValCount = 0; }
	~SIMPLE_ARRAY() { Clear(); }
	void CloneFrom(SIMPLE_ARRAY<TYPE, TYPE_PRESET_COUNT> &objarr)
	{
		if (Count > objarr.Count)
			Clear();
		for (UINT i = 0; i < Count; i++)
			Set(i, objarr.At(i), true);
	}
};
#endif