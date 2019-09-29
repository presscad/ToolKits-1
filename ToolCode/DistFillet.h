#pragma once
#include "ArrayList.h"

enum FILLET_PRECISION{FILLET_INTEGER,FILLET_HALF_INTEGER,FILLET_ONE_FLOAT};
class CDistFillet
{
	//�趨ջ���黺�棬����˨����������£����Ա����ظ�new-delete�ڴ�
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
