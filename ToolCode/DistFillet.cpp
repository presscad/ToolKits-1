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
		if(m_blScatterTolerance)	//���ںᵣ�������ļ��ˮƽ�����Ǹ��ϷǱ����γ��ۻ����϶�ʱ������÷�ɢ���ķ�ʽ�����ܱ�֤��ࣩ wjh-2019.8.14
			dfAccumRoundingTolerance-=(distArr[i]-dist);
	}
	int group_start=-1,group_end=-1,group_mid=0;
	for(i=0;i<distArr.GetSize();i++)
	{
		if(i==0)
			coordArr[i]=distArr[0];
		else if(i==distArr.GetSize()-1)
		{	//�����һ���������꣬���ֻ��ǰ��
			double foreward_offset=FilletDist(coordArr[i-1]+distArr[i],precision)-FilletDist(coordArr[i],precision);
			if(fabs(foreward_offset)>0)
			{
				bool bForewardOffset=false;
				for(j=i-1;j>0;j--)
				{	//Ԥ���Ƿ������ǰ�� wjh-2017.3.10
					if(distArr[j]>groupDistThreshold)
						bForewardOffset=true;
				}
				for(j=i-1;bForewardOffset&&j>0;j--)
				{	//��ǰ�������ҵ�һ�����ڵ�����ֵ����Գߴ���е���,ͬʱ������Ӧ�������� wht 10-02-26
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
					return distArr[i]>groupDistThreshold;	//������������ֵ������ִ��� wjh-2015.11.30
				}
				else
					return true;
			}
		}
		else if(distArr[i]<=groupDistThreshold)	//С��ͬ������ֵ��ֻ�ܵ�����������
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
				{	//Ԥ���Ƿ������ǰ�� wjh-2017.3.10
					if(distArr[j]>groupDistThreshold)
						bForewardOffset=true;
				}
				for(j=i-1;bForewardOffset&&j>0;j--)
				{	//��ǰ�������ҵ�һ�����ڵ�����ֵ����Գߴ���е���,ͬʱ������Ӧ�������� wht 10-02-26
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
