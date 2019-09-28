#pragma once
#include "Buffer.h"
#include "TempFile.h"
#include "HashTable.h"
#include "Hash64Table.h"
#include "XhCharString.h"
#ifdef _DEBUG
#include "ArrayList.h"
#endif

struct IHashSetBufferObject{
	virtual bool ReadObjectKey(BUFFER_IO* pBuff,DWORD version){return false;}	//从缓存读入键值信息
	virtual void WriteObjectKey(BUFFER_IO* pBuff,DWORD version){;}//将键值信息写入缓存
	virtual bool ReadObject(BUFFER_IO* pBuff,DWORD version)=0;
	virtual bool WriteObject(BUFFER_IO* pBuff,DWORD version)=0;
	virtual CXhChar50 GetKeyString() const=0;
	virtual XHUINT64 GetCompositeKey() const=0;
};
//TYPE类须支持以上IHashSetBufferObject所述接口函数(GetKeyWord()除外)
template <class TYPE> class CHashStrSetBuffer : public CTempFileBuffer
{
protected:
	DWORD m_dwVersion;
	long m_nSizeOfObj;	//该类对象的占用缓存大小
	long m_nObjGrowSize;//哈希表递增占用缓存大小
	CHash64Table<long> hashObjBuffPosition;	//对象在缓存中的存储位置
#ifdef _DEBUG
	DYN_ARRAY<char>OBJ_BUFFER;
#endif
public:
	CHashStrSetBuffer(long nObjGrowSize=0,long nSizeOfObj=0){
		m_dwVersion=0;
		m_nSizeOfObj=4;
		m_nObjGrowSize=4;
		m_nSizeOfObj=nSizeOfObj;
		m_nObjGrowSize=nObjGrowSize;
	}
	DWORD SetBuffVersion(DWORD dwVersion){
		m_nSizeOfObj=TYPE::BufferSize(dwVersion);
		return m_dwVersion=dwVersion;
	}
	DWORD BuffVersion(){return m_dwVersion;}
	void InitBuffer(long nObjGrowSize,long nSizeOfObj,DWORD dwVersion)
	{
		m_dwVersion=dwVersion;
		m_nSizeOfObj=4;
		m_nObjGrowSize=4;
		m_nSizeOfObj=nSizeOfObj;
		m_nObjGrowSize=nObjGrowSize;

		if(nObjGrowSize>0)
			hashObjBuffPosition.CreateHashTable(nObjGrowSize);
#ifdef _DEBUG
		OBJ_BUFFER.Resize(m_nSizeOfObj);
#endif
	}
	void InitBuffer(char* filename,long nObjGrowSize,long nSizeOfObj,DWORD dwVersion)
	{
		m_dwVersion=dwVersion;
		UINT bufSize=nObjGrowSize*nSizeOfObj;
		if(bufSize>0xA00000)
			bufSize=0xA00000-0xA00000%nSizeOfObj;
		CreateFile(filename,bufSize);
		m_nSizeOfObj=4;
		m_nObjGrowSize=4;
		m_nSizeOfObj=nSizeOfObj;
		m_nObjGrowSize=nObjGrowSize;

		if(nObjGrowSize>0)
			hashObjBuffPosition.CreateHashTable(nObjGrowSize);
	#ifdef _DEBUG
		OBJ_BUFFER.Resize(m_nSizeOfObj);
	#endif
	}
	virtual ~CHashStrSetBuffer(void){;}
	void ClearBuffer(){
		ClearContents();
		hashObjBuffPosition.EmptyKeys();
	}
	//TODO: 遇到大文件时缓存可能达上百M此时，很难获取上百M的连接续内存空间，必须借助物理文件进行处理
	long SetObjGrowSize(long nObjGrowSize){
		m_nObjGrowSize=min(nObjGrowSize,10000000);
		return m_nObjGrowSize;
	}
	long GetObjectNum(){	return GetLength()/m_nSizeOfObj;}
	virtual bool RebuildHashTable()
	{
		TYPE obj;
		SeekPosition(0);
		hashObjBuffPosition.Empty();
		hashObjBuffPosition.CreateHashTable(GetObjectNum());
		DWORD position=GetCursorPosition();
		DWORD blocksize=0xA00000-0xA00000%m_nSizeOfObj;
		DYN_ARRAY<char> block(blocksize);
		UINT CONTENT_SIZE=m_nSizeOfObj-TYPE::KeyHeadInfoSize(m_dwVersion);
		while(position<GetLength()){
			obj.ReadObjectKey(this,m_dwVersion);	//读取TYPE::KeyHeadInfoSize()
			SeekOffset(CONTENT_SIZE);
			hashObjBuffPosition.SetValueAt(obj.GetCompositeKey(),position);
			position=GetCursorPosition();
		}
		return true;
	}
	bool AddObject(TYPE& obj,bool updateHashTable=true)
	{
		if(updateHashTable)
		{
			long object_num=GetObjectNum();
			if(object_num*2>(long)hashObjBuffPosition.GetHashTableSize()*2)
				RebuildHashTable();
			long start_write_pos=-1;
			XHUINT64 key64=obj.GetCompositeKey();
			if(hashObjBuffPosition.GetValueAt(key64,start_write_pos))
				SeekPosition(start_write_pos);
			else	//新键值对象
			{
				if(GetCursorPosition()<GetLength())
					SeekPosition(GetLength());
				hashObjBuffPosition.SetValueAt(key64,(long)GetLength());
			}
		}
	#ifdef _DEBUG
		CBuffer objbuf(OBJ_BUFFER,m_nSizeOfObj);
		obj.WriteObject(&objbuf,m_dwVersion);
		if(objbuf.GetLength()!=m_nSizeOfObj)
			throw "预定模板类对象序列化大小有误";
		Write((char*)OBJ_BUFFER,m_nSizeOfObj);
	#else
		obj.WriteObject(this,m_dwVersion);
	#endif
		return true;
	}
	bool GetObject(XHUINT64 key,TYPE* pObj)
	{
		long start_read_pos=-1;
		if(!hashObjBuffPosition.GetValueAt(key,start_read_pos))
			return false;
		if(!SeekPosition(start_read_pos))
			return false;
		return pObj->ReadObject(this,m_dwVersion);
	}
	bool GetObjectAt(long i,TYPE* pObj)
	{
		if(!SeekPosition(i*m_nSizeOfObj))
			return false;
		return pObj->ReadObject(this,m_dwVersion);
	}
	bool GetNextObject(TYPE* pObj)
	{
		if(m_nSizeOfObj==0||(GetCursorPosition()%m_nSizeOfObj!=0))
			return false;
		return pObj->ReadObject(this,m_dwVersion);
	}
};
