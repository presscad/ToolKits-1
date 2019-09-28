#include "stdafx.h"
#include <math.h>
#include "Deg.h"

DEG::DEG(double dfDegAngle/*=0*/)
{
	m_dfDegAngle=dfDegAngle;
	Unify();
}
DEG::DEG(const double* vec2d){
	FromVector(vec2d);
}
double DEG::Unify()	//统一到0<=m_dfDegAngle<360度
{
	while(m_dfDegAngle<0)
		m_dfDegAngle+=360;
	while(m_dfDegAngle>=360)
		m_dfDegAngle-=360;
	return m_dfDegAngle;
}
double DEG::FromVector(const double* vec2d)
{
	double Vx=vec2d[0],Vy=vec2d[1];
	double d=fabs(Vx)+fabs(Vy);
	if(d<=1e-9)
		return m_dfDegAngle=0;//矢量定义有误
	d=Vx/sqrt(Vx*Vx+Vy*Vy);
	//DEGTORAD_COEF    57.2957795130823208768
	m_dfDegAngle=acos(d)*57.2957795130823208768;
	if(Vy<-1e-9)
		m_dfDegAngle=360-m_dfDegAngle;
	return m_dfDegAngle;
}
bool DEG::IsColinearDegAngle(double degAngle,double tolerance/*=1*/)
{
	degAngle=DEG(degAngle);
	double diff=fabs(degAngle-m_dfDegAngle);
	if(fabs(diff)<=tolerance||fabs(diff-180)<=tolerance)
		return true;
	else
		return false;
}
//dfMinDegAngle允许为负数，dfMaxDegAngle也允许大于360°，但dfMaxDegAngle-dfMinDegAngle<360
bool DEG::IsInSector(double dfMinDegAngle,double dfMaxDegAngle,bool bIncPeerSector/*=true*/)
{
	if(dfMinDegAngle<0&&dfMaxDegAngle<0)
	{
		dfMinDegAngle=DEG(dfMinDegAngle);
		dfMaxDegAngle=DEG(dfMaxDegAngle);
	}
	double dfSectorAngle=dfMaxDegAngle-dfMinDegAngle;
	if(bIncPeerSector&&DEG(m_dfDegAngle+180).IsInSector(dfMinDegAngle,dfMaxDegAngle,false))
		return true;
	if(dfSectorAngle<0||dfSectorAngle>=360)
		return false;
	if(dfMinDegAngle<0&&m_dfDegAngle<dfMinDegAngle+360&&m_dfDegAngle>dfMaxDegAngle)
		return false;
	else if(dfMinDegAngle>=0&&(m_dfDegAngle<dfMinDegAngle||m_dfDegAngle>dfMaxDegAngle))
		return false;
	else if(dfMaxDegAngle>=360&&m_dfDegAngle>dfMaxDegAngle-360&&m_dfDegAngle<dfMinDegAngle)
		return false;
	return true;
}
