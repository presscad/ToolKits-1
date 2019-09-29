#pragma once
#include "ArrayList.h"

enum FILLET_PRECISION{FILLET_INTEGER,FILLET_HALF_INTEGER,FILLET_ONE_FLOAT};
class CDistFillet
{
	//设定栈数组缓存，在螺栓数不多情况下，可以避免重复new-delete内存
	double _coordValPool[200],_distValPool[200];
public:
	bool m_blScatterTolerance;
	static double FilletDist(double dist, FILLET_PRECISION precision);
	ARRAY_LIST<double>coordArr;
	ARRAY_LIST<double>distArr;
	CDistFillet(bool blScatterTolerance=false);
	~CDistFillet(void);
	bool Fillet(double groupDistThreshold=120,FILLET_PRECISION precision=FILLET_INTEGER);
};
