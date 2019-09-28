#include "stdafx.h"
#include "TaperPolygon.h"
#include "f_alg_fun.h"
#include "ArrayList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

TAPER_POLYGON::TAPER_POLYGON()
{
	dfMinZ=dfMaxZ=0;//多面体相对原点和基面的Z向最小与最大范围值
	_nCount =0;		//顶点数
	pxArrPrjVertices=NULL;

}
TAPER_POLYGON::~TAPER_POLYGON()
{
	if (pxArrPrjVertices)
		delete[]pxArrPrjVertices;
}
void TAPER_POLYGON::ClearScope()
{
	_nCount = 0;
	if (pxArrPrjVertices)
		delete[]pxArrPrjVertices;
	pxArrPrjVertices = NULL;
}
bool TAPER_POLYGON::InitPolyVertexes(const double* pxOrg, const double* pxPlaneNormal, GEPOINT* xarrVertices, int count)
{
	cs.origin = GEPOINT(pxOrg);
	cs.axis_z = GEPOINT(pxPlaneNormal);
	if (count<3||!normalize(cs.axis_z))
		return false;
	if (pxArrPrjVertices)
		delete[] pxArrPrjVertices;
	if (cs.axis_z.y > EPS_COS)	//基面为X-Z平面，前后面对称
	{
		cs.axis_x.Set(1, 0, 0);
		cs.axis_y.Set(0, 0, -1);
	}
	else if (cs.axis_z.y < -EPS_COS)	//基面为X-Z平面，前后面对称
	{
		cs.axis_x.Set(1, 0, 0);
		cs.axis_y.Set(0, 0, 1);
	}
	else if (cs.axis_z.x > EPS_COS)
	{
		cs.axis_x.Set(0, -1, 0);
		cs.axis_y.Set(0, 0, -1);
	}
	else if (cs.axis_z.x < -EPS_COS)
	{
		cs.axis_x.Set(0, -1, 0);
		cs.axis_y.Set(0, 0, 1);
	}
	_nCount = count;
	pxArrPrjVertices = new GEPOINT[count];
	int i;
	for (i = 0; i < count; i++)
	{
		GEPOINT v2o = xarrVertices[i] - cs.origin;
		double dz = v2o * cs.axis_z;
		GEPOINT dv(v2o.x - dz * cs.axis_z.x,v2o.y-dz*cs.axis_z.y,v2o.z-dz*cs.axis_z.z);	//基面上的投影方向
		if (!dv.IsZero())
		{
			double dx = dv.mod();
			if (cs.axis_x.IsZero())
			{
				pxArrPrjVertices[i].Set(dx, 0);
				//初始化坐标系X与Y轴
				cs.axis_x = dv / dx;
				cs.axis_y = cs.axis_z^cs.axis_x;
				normalize(cs.axis_y);
			}
			else
				pxArrPrjVertices[i] = cs.TransPToCS(xarrVertices[i]);
		}
		else
			pxArrPrjVertices[i].Set(0, 0, 0);
		if (pxArrPrjVertices[i].z < dfMinZ)
			dfMinZ = pxArrPrjVertices[i].z;
		if (pxArrPrjVertices[i].z > dfMaxZ)
			dfMaxZ = pxArrPrjVertices[i].z;
		pxArrPrjVertices[i].z = 0;
	}
	dfMaxZ = max(-dfMinZ, dfMaxZ);
	dfMinZ = -dfMaxZ;
	GEPOINT vprev, vcurr;
	bool blFlipReverse = false;
	for (i = 1; i <= count; i++)
	{
		vprev = pxArrPrjVertices[i%count] - pxArrPrjVertices[i-1];
		vcurr = pxArrPrjVertices[(i+1)%count] - pxArrPrjVertices[i%count];
		GEPOINT normal = vprev ^ vcurr;
		if (normal.z < -EPS)
		{	//为保证边界逆时针，需要旋转平面的Z轴与Y轴
			cs.axis_z *= -1.0;
			cs.axis_y *= -1.0;
			blFlipReverse = true;
			break;
		}
	}
	if(blFlipReverse)
	{
		for (i = 0; i < count; i++)
			pxArrPrjVertices[i].y*=-1.0;
	}
	return true;
}
bool TAPER_POLYGON::IsIncludePoint(double px, double py, double pz, double tolerance/* = EPS*/)
{
	double coord3d[3] = { px,py,pz };
	return IsIncludePoint(coord3d,tolerance);
}
bool TAPER_POLYGON::IsIncludePoint(const double* ptcoords, double TOLERANCE/* = EPS*/)
{
	GEPOINT pick = cs.TransPToCS(ptcoords);
	for (int i = 0; i < _nCount; i++)
	{
		f3dPoint v1 = pxArrPrjVertices[(i + 1) % _nCount] - pxArrPrjVertices[i];
		f3dPoint v2 = pick - pxArrPrjVertices[i];
		double L1 = v1.mod();
		double L2 = v2.mod();
		if (L2 > EPS&&v1.x*v2.y - v1.y*v2.x < -TOLERANCE * L1)
			break;	//点在某条边的右侧故不是凸多边形的内点
	}
	if (i == nCount)
		return true;
	return false;
}
bool TAPER_POLYGON::IsIncludeLine(const double* pLineStart, const double* pLineEnd, double TOLERANCE/* = EPS*/)
{
	GEPOINT xPickStart = cs.TransPToCS(pLineStart);
	GEPOINT xPickEnd = cs.TransPToCS(pLineEnd);
	if (xPickStart.z<this->dfMinZ - TOLERANCE || xPickStart.z>this->dfMaxZ + TOLERANCE)
		return false;
	if (xPickEnd.z<this->dfMinZ - TOLERANCE || xPickEnd.z>this->dfMaxZ + TOLERANCE)
		return false;
	xPickStart.z = xPickEnd.z = 0;
	for (int i = 0; i < _nCount; i++)
	{
		GEPOINT v1 = pxArrPrjVertices[(i + 1) % _nCount] - pxArrPrjVertices[i];
		GEPOINT v2 = xPickStart - pxArrPrjVertices[i];
		GEPOINT v3 = xPickEnd - pxArrPrjVertices[i];
		double L1 = v1.mod();
		double L2 = v2.mod();
		double L3 = v3.mod();
		if (L2 > EPS&&v1.x*v2.y - v1.y*v2.x < -TOLERANCE * L1)
			break;	//起点在某条边的右侧故不是凸多边形的内点
		if (L3 > EPS&&v1.x*v3.y - v1.y*v3.x < -TOLERANCE * L1)
			break;	//终点在某条边的右侧故不是凸多边形的内点
	}
	if (i == _nCount)
		return true;
	return false;
}
