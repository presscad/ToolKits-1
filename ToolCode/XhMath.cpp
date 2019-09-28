#include "stdafx.h"
#include "XhMath.h"

//////////////////////////////////////////////////////////////////////////
double CMaxDouble::Update(double val,void* pRelaObj,double tolerance/*=0*/)
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
double CMinDouble::Update(double val,void* pRelaObj,double tolerance/*=0*/)
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
