#include "stdafx.h"
#include "distfillet.h"
#include "f_alg_fun.h"

CDistFillet::CDistFillet(bool blScatterTolerance/*=false*/)
{
	m_blScatterTolerance=blScatterTolerance;
	coordArr.Attach(_coordValPool,0,200);
	distArr.Attach(_distValPool,0,200);
}

CDistFillet::~CDistFillet(void)
{
}

double CDistFillet::FilletDist(double dist, FILLET_PRECISION precision)
{
	if(precision==FILLET_INTEGER)
		return ftoi(dist);
	else if(precision==FILLET_HALF_INTEGER)
		return fto_halfi(dist);
	else //if(precision==FILLET_ONE_FLOAT)
		return ftoi(dist*10)/10.0;
}

bool CDistFillet::Fillet(double groupDistThreshold/*=120*/,FILLET_PRECISION precision/*=FILLET_INTEGER*/)
{
	int i,j;
	distArr.SetSize(coordArr.GetSize());
	double dfAccumRoundingTolerance=0;
	for(i=0;i<coordArr.GetSize();i++)
	{
		double dist=coordArr[i];
		if(i>0)
			dist=coordArr[i]-coordArr[i-1];
		distArr[i]=FilletDist(dist+dfAccumRoundingTolerance,precision);
		if(m_blScatterTolerance)	//对于横担与塔身横材间的水平外贴角钢上非标间距形成累积误差较多时，须采用分散误差的方式（不能保证间距） wjh-2019.8.14
			dfAccumRoundingTolerance-=(distArr[i]-dist);
	}
	int group_start=-1,group_end=-1,group_mid=0;
	for(i=0;i<distArr.GetSize();i++)
	{
		if(i==0)
			coordArr[i]=distArr[0];
		else if(i==distArr.GetSize()-1)
		{	//到最后一个绝对坐标，误差只能前移
			double foreward_offset=FilletDist(coordArr[i-1]+distArr[i],precision)-FilletDist(coordArr[i],precision);
			if(fabs(foreward_offset)>0)
			{
				bool bForewardOffset=false;
				for(j=i-1;j>0;j--)
				{	//预判是否可以向前移 wjh-2017.3.10
					if(distArr[j]>groupDistThreshold)
						bForewardOffset=true;
				}
				for(j=i-1;bForewardOffset&&j>0;j--)
				{	//向前搜索查找第一个大于调整阈值的相对尺寸进行调整,同时调整相应绝对坐标 wht 10-02-26
					coordArr[j]-=foreward_offset;
					if(distArr[j]>groupDistThreshold)
					{
						distArr[j]-=foreward_offset;
						break;
					}
				}
				if(!bForewardOffset)
				{
					distArr[i]=coordArr[i]-coordArr[i-1];
					return distArr[i]>groupDistThreshold;	//可能因对齐数字导致数字错误 wjh-2015.11.30
				}
				else
					return true;
			}
		}
		else if(distArr[i]<=groupDistThreshold)	//小于同组间距阈值的只能调整绝对坐标
			coordArr[i]=coordArr[i-1]+distArr[i];
		else
		{
			distArr[i]=FilletDist(coordArr[i]-coordArr[i-1],precision);
			double foreward_offset=0,afterward_offset=coordArr[i-1]+distArr[i]-coordArr[i];
			if(fabs(afterward_offset)>1)
				foreward_offset=FilletDist(afterward_offset/2,precision);
			afterward_offset-=foreward_offset;
			bool bForewardOffset=false;
			if(foreward_offset!=0)
			{
				for(j=i-1;j>0;j--)
				{	//预判是否可以向前移 wjh-2017.3.10
					if(distArr[j]>groupDistThreshold)
						bForewardOffset=true;
				}
				for(j=i-1;bForewardOffset&&j>0;j--)
				{	//向前搜索查找第一个大于调整阈值的相对尺寸进行调整,同时调整相应绝对坐标 wht 10-02-26
					coordArr[j]-=foreward_offset;
					if(distArr[j]>groupDistThreshold)
					{
						distArr[j]-=foreward_offset;
						break;
					}
				}
			}
			if(bForewardOffset)
				coordArr[i]+=afterward_offset;
			else
				coordArr[i]=coordArr[i-1]+distArr[i];
		}
	}
	return true;
}
