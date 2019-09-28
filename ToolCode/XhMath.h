#pragma once
class CMaxDouble{
	bool inited;
public:
	double number;
	void* m_pRelaObj;
	CMaxDouble(){inited=false;m_pRelaObj=NULL;}
	CMaxDouble(double init_val,void* pRelaObj=NULL){inited=true;number=init_val;m_pRelaObj=pRelaObj;}
	operator double(){return number;}
	bool IsInited() const{return inited;}
	double Update(double val,void* pRelaObj=NULL,double tolerance=0);
};
class CMinDouble{
	bool inited;
public:
	double number;
	void* m_pRelaObj;
	CMinDouble(){inited=false;m_pRelaObj=NULL;}
	CMinDouble(double init_val,void* pRelaObj=NULL){inited=true;number=init_val;m_pRelaObj=pRelaObj;}
	operator double(){return number;}
	bool IsInited(){return inited;}
	double Update(double val,void* pRelaObj=NULL,double tolerance=0);
};
