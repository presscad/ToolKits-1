#pragma once

#ifndef __DEF_MAX_DOUBLE_
#define __DEF_MAX_DOUBLE_

template <typename T>
class CMaxNumberT{
	bool inited;
public:
	T number;
	void* m_pRelaObj;
	CMaxNumberT(){inited=false;m_pRelaObj=NULL;number=0;}
	CMaxNumberT(T init_val,void* pRelaObj=NULL){inited=true;number=init_val;m_pRelaObj=pRelaObj;}
	operator T(){return number;}
	bool IsInited(){return inited;}
	T Update(T val,void* pRelaObj=NULL,T tolerance=0)
	{
		if(!inited)
		{
			number=val;
			inited=true;
			m_pRelaObj=pRelaObj;
		}
		else if(number<val-tolerance)
		{
			number=val;
			m_pRelaObj=pRelaObj;
		}
		return number;
	}
};
template <typename T>
class CMinNumberT{
	bool inited;
public:
	T number;
	void* m_pRelaObj;
	CMinNumberT(){inited=false;m_pRelaObj=NULL;number=0;}
	CMinNumberT(T init_val,void* pRelaObj=NULL){inited=true;number=init_val;m_pRelaObj=pRelaObj;}
	operator T(){return number;}
	bool IsInited(){return inited;}
	T Update(T val,void* pRelaObj=NULL,T tolerance=0)
	{
		if(!inited)
		{
			number=val;
			inited=true;
			m_pRelaObj=pRelaObj;
		}
		else if(number>val+tolerance)
		{
			number=val;
			m_pRelaObj=pRelaObj;
		}
		return number;
	}
};

typedef CMaxNumberT<BYTE> CMaxByte;
typedef CMaxNumberT<int> CMaxInt;
typedef CMaxNumberT<float> CMaxFloat;
typedef CMaxNumberT<double> CMaxD;

typedef CMinNumberT<BYTE> CMinByte;
typedef CMinNumberT<int> CMinInt;
typedef CMinNumberT<float> CMinFloat;
typedef CMinNumberT<double> CMinD;
#endif