#pragma once

#include "f_ent.h"
//由两个相对基准平面对称的多边形形成的一个多面体（一般两侧多边形面不平行，沿某方向逐步收窄）
struct TAPER_POLYGON {
	GECS cs;			//基准坐标系，X-Y平面为基准平面，原点为定位拾取原点（允许不在多面体范围内）
	double dfMinZ,dfMaxZ;		//多面体相对原点和基面的Z向最小与最大范围值
	long _nCount;	//顶点数
	GEPOINT* pxArrPrjVertices;
public:
	long get_nCount() { return _nCount; }
	__declspec(property(get = get_nCount)) long nCount;
	double MaxZ()const { return dfMaxZ; }
	double MinZ()const { return dfMinZ; }
	TAPER_POLYGON();
	~TAPER_POLYGON();
	bool InitPolyVertexes(const double* pxOrg,const double* pxPlaneNormal,GEPOINT* xarrVertices, int count);
	void ClearScope();
	bool IsIncludePoint(double px, double py, double pz, double tolerance = EPS);
	bool IsIncludePoint(const double* ptcoords, double tolerance = EPS);
	bool IsIncludeLine(const double* pLineStart, const double* pLineEnd, double tolerance = EPS);
};
