#pragma once

enum FILLET_PRECISION{FILLET_INTEGER,FILLET_HALF_INTEGER,FILLET_ONE_FLOAT};
class CDistFillet
{
public:
	static double FilletDist(double dist, FILLET_PRECISION precision);
	CArray<double,double&>coordArr;
	CArray<double,double&>distArr;
	CDistFillet(void);
	~CDistFillet(void);
	bool Fillet(double groupDistThreshold=120,FILLET_PRECISION precision=FILLET_INTEGER);
};
