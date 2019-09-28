// Matrix.h: interface for the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include <stdio.h>

template<class TYPE> class MAP_ARRAY 
{
	inline TYPE& unit_val(const int row,const int col){
		if((row >= m_nRowN || row < 0) ||(col >= m_nColumnN || col < 0))
			throw "超界";//CMatrixException(M_INVALIDACCESS);
		if(lpRowHeader)
			return lpRowHeader[row][col];
		else if(m_nColumnN!=1)
			return datamap[row*m_nColumnN + col];
		else
			return datamap[row];
	}
	bool m_bInternalBuff;
	long m_nCurrBuffSize;
	long m_nBuffGrowSize;
	bool RebuildIndexHeader(int nNeedDataBufSize=0)
	{
		if(m_nRowN<=0||m_nColumnN<=0||datamap==NULL)
			return false;
		if(nNeedDataBufSize<=0)
			nNeedDataBufSize=m_nRowN*m_nColumnN*8;
		lpRowHeader=(TYPE*)(data+nNeedDataBufSize);
		TYPE* prevIndexHeader=lpRowHeader[0]=datamap;
		for(int i=1;i<m_nRowN;i++)
			prevIndexHeader=lpRowHeader[i]=prevIndexHeader+m_nColumnN;
		return true;
	}
public:
	int m_nRowN;
	int m_nColumnN;
	union{
		char* data;
		TYPE* datamap;
	};
	TYPE** lpRowHeader;
public:
	MAP_ARRAY()
	{
		m_nRowN=0;
		m_nColumnN=0;
		lpRowHeader=NULL;
		datamap = NULL;
		m_bInternalBuff=true;
		m_nCurrBuffSize=0;
		m_nBuffGrowSize=8;	//默认8 bytes缓存增量
	}
	MAP_ARRAY(int row, int column, TYPE init_val=0,char* buff=NULL,UINT nBufSize=0)
	{
		if(row <= 0 || column <= 0)
			throw "未初始化";//CMatrixException(M_CANTINIT);

		m_nRowN = row;
		m_nColumnN = column;
		m_nCurrBuffSize=nBufSize;
		m_nBuffGrowSize=8;	//默认8 bytes缓存增量
		int nNeedDataBufSize=row*column*sizeof(TYPE);
		int nNeedIndexBufSize=row*sizeof(TYPE*);
		if(buff==NULL || m_nCurrBuffSize<nNeedDataBufSize+nNeedIndexBufSize)
		{
			//mat = new double[m_nRowN*m_nColumnN];
			//row_header=new DOUBLE_PTR[row];
			m_bInternalBuff=true;
			m_nCurrBuffSize=nNeedDataBufSize+nNeedIndexBufSize;
			data=new char[nNeedDataBufSize+nNeedIndexBufSize];
			lpRowHeader=(TYPE**)(data+nNeedDataBufSize);
		}
		else
		{
			datamap=buff;
			lpRowHeader=(TYPE**)(data+nNeedDataBufSize);
			m_bInternalBuff=false;
		}
		if(datamap == NULL)
			throw "内存分配失败";//CMatrixException(M_CANTGETMEMORY);
		for(int i=0;i<m_nRowN;i++)
		{
			lpRowHeader[i]=&datamap[i*m_nColumnN];
			if(fabs(init_val)>ZERO_EPS)
			{
				for(int j=0;j<m_nColumnN;j++)
					lpRowHeader[i][j]=init_val;
				//mat[i*m_nColumnN+j] = init_val;
			}
		}
		if(init_val==0)
			memset(datamap,0,nNeedDataBufSize);
	}
	//MAP_ARRAY(MAP_ARRAY& other);
	virtual ~MAP_ARRAY(){
		if(m_bInternalBuff&&data)
			delete []data;
	}
	long BufferSize() const{return m_nCurrBuffSize;}
	_declspec(property(get=BufferSize))	 long liBuffSize;
	//long SetBuffGrowSize(long nGrowSize=0x4000){return m_nBuffGrowSize=nGrowSize;}
	//long GetBuffGrowSize() const{return m_nBuffGrowSize;}
	//_declspec(property(get=GetBuffGrowSize)) long BuffGrowSize;
	void InitZero(){
		if(m_nRowN>0&&m_nColumnN>0)
			memset(datamap,0,sizeof(TYPE)*m_nRowN*m_nColumnN);
	}
	long GetRowsNum(){return m_nRowN;}		//获得矩阵行数
	long GetColumnsNum(){return m_nColumnN;}//获得矩阵列数
	_declspec(property(get=GetRowsNum))		long liRowCount;
	_declspec(property(get=GetColumnsNum))	long liColCount;
	void Resize(int row, int column)
	{
		if(row <= 0 || column <= 0)
			throw "未初始化";//CMatrixException(M_CANTINIT);

		m_nRowN = row;
		m_nColumnN = column;
		m_nCurrBuffSize=this->liBuffSize;
		m_nBuffGrowSize=8;	//默认8 bytes缓存增量
		int nNeedDataBufSize=row*column*sizeof(TYPE);
		int nNeedIndexBufSize=row*sizeof(TYPE*);
		//mat = new double[m_nRowN*m_nColumnN];
		//row_header=new DOUBLE_PTR[row];
		m_bInternalBuff=true;
		m_nCurrBuffSize=nNeedDataBufSize+nNeedIndexBufSize;
		datamap=(TYPE*)(new BYTE[nNeedDataBufSize+nNeedIndexBufSize]);
		lpRowHeader=(TYPE**)(data+nNeedDataBufSize);

		if(datamap == NULL)
			throw "内存分配失败";//CMatrixException(M_CANTGETMEMORY);
		for(int i=0;i<m_nRowN;i++)
			lpRowHeader[i]=&datamap[i*m_nColumnN];
	}
	//operator TYPE*(){return datamap;}
	inline TYPE* operator [](const int row)const{
		if((row >= m_nRowN || row < 0))
			throw "越界";//CMatrixException(M_INVALIDACCESS);
		if(lpRowHeader)
			return lpRowHeader[row];
		else if(m_nColumnN!=1)
			return &datamap[row*m_nColumnN];
		else
			return &datamap[row];
	}
};
typedef MAP_ARRAY<BYTE> MAP_BYTES;
