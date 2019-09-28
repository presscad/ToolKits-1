#include "StdAfx.h"
#include "TSPAlgorithm.h"
#include <cmath>
#include <time.h>

 
CTSPAlgorithm::CTSPAlgorithm()
{
	m_ciAlgType = 0;
	nPtNum=0;
	m_fDist=0;
	m_bLoop=TRUE;
}
CTSPAlgorithm::~CTSPAlgorithm()
{

}
//计算遍历路径的总长度
double CTSPAlgorithm::TotalDist(PATH path)
{
	double fSumDist=0;
	for(int i=1;i<nPtNum;i++)
		fSumDist+=distArr[path.indexArr[i]][path.indexArr[i+1]];
	if(m_bLoop)
		fSumDist+=distArr[path.indexArr[nPtNum]][path.indexArr[1]];
	return fSumDist;
}
//读取数据进行初始化
void CTSPAlgorithm::InitData(ARRAY_LIST<f3dPoint>& ptArr,BYTE ciAlgType /*= 0*/)
{
	nPtNum=ptArr.GetSize()-1;
	if (ciAlgType == 1 || (ciAlgType == 0 && nPtNum <= 5))
		m_ciAlgType = 1;	//贪心算法进行路径优化
	if (ciAlgType == 2 || (ciAlgType == 0 && nPtNum > 5 && nPtNum < 15))
		m_ciAlgType = 2;	//回溯算法进行路径优化
	if (ciAlgType == 3 || (ciAlgType == 0 && nPtNum >= 15))
		m_ciAlgType = 3;	//模拟退火算法进行路径优化(考虑环形)
	if (m_ciAlgType == 2 && nPtNum >= 20)
		m_ciAlgType = 3;	//根据测试，回溯算法时点数超过20速度会变慢
	m_bLoop = (m_ciAlgType == 3) ? TRUE : FALSE;
	//记录任意两点之间的距离(跨过起始点)
	for(int i=1;i<=nPtNum;i++)
	{
		for(int j=i;j<=nPtNum;j++)
		{
			if(i==j)
				distArr[i][j]= 0;		
			else
			{
				distArr[i][j]=DISTANCE(ptArr[i],ptArr[j]);
				distArr[j][i]=distArr[i][j];
			}
		}
	}
	//计算当前路径的总长
	for(int i=1;i<=nPtNum;i++)
		xBestPath.indexArr[i]=i;
	xBestPath.fSumDist=TotalDist(xBestPath);
}
void CTSPAlgorithm::Swap(int &a ,int &b)
{
	int temp ;
	temp = a ;
	a = b ;
	b = temp ;
}
//新解产生函数
PATH CTSPAlgorithm::GetNext(PATH cur_path)
{
    UINT x, y;
    PATH ret;
    ret=cur_path;
    do {
        x = rand()%nPtNum+1;
        y = rand()%nPtNum+1;
    }while(x == y);
	Swap(ret.indexArr[x],ret.indexArr[y]);
	ret.fSumDist=TotalDist(ret);
    return ret;
}
//退火和降温过程  
void CTSPAlgorithm::Annealing()
{
    double T=INIT_T;		//温度
	PATH cur_path=xBestPath;
    UINT P_t=0, A_t=0; 
    while(true)
    {
        for (int i=1;i<IN_LOOP;i++)
        {
            PATH new_path=GetNext(cur_path);                    //获取新路径
            double delta= new_path.fSumDist-cur_path.fSumDist;
            if (delta<0.0)
            {
                cur_path = new_path;
                P_t = 0;
                A_t = 0;
            }
            else
            {
                double rnd = rand()%10000 /10000.0;
                double p = exp(-delta/T);
                if (p > rnd)
                    cur_path = new_path;
                P_t++;
            }
            if (P_t >=P_LIMIT)
            {
                A_t++;
                break;
            }
        }
        if(cur_path.fSumDist<xBestPath.fSumDist)
			 xBestPath=cur_path;		 
        if(A_t>=OUT_LOOP || T<FINAL_T) 
			break;
        T=T*RATE;   //降温                                       
    }	
}
//回溯算法
void CTSPAlgorithm::Backtrack(int i)
{
	double fDist1=0,fDist2=0;
	if(i==nPtNum)
	{	//搜索到底部，如果满足最优解则记录
		fDist1=distArr[xBestPath.indexArr[nPtNum-1]][xBestPath.indexArr[nPtNum]];
		if(m_bLoop)
			fDist2=distArr[xBestPath.indexArr[nPtNum]][xBestPath.indexArr[1]];
		if(fDist1!=0 && m_fDist+fDist1+fDist2<=xBestPath.fSumDist)
		{
			for(int k=1;k<=nPtNum;k++)
				bestIndexArr[k]= xBestPath.indexArr[k];
			xBestPath.fSumDist=m_fDist+fDist1+fDist2;
		}
	}
	else
	{
		for(int j=i;j<nPtNum;j++)
		{
			fDist1=distArr[xBestPath.indexArr[i-1]][xBestPath.indexArr[j]];
			if(fDist1!=0 && m_fDist+fDist1<xBestPath.fSumDist)
			{
				Swap(xBestPath.indexArr[i],xBestPath.indexArr[j]);
				m_fDist+=distArr[xBestPath.indexArr[i-1]][xBestPath.indexArr[i]];
				Backtrack(i+1);
				m_fDist-=distArr[xBestPath.indexArr[i-1]][xBestPath.indexArr[i]];
				Swap(xBestPath.indexArr[i],xBestPath.indexArr[j]);
			}
		}
	}
}
//贪心算法(保证每次最优)
void CTSPAlgorithm::Greedy(int iStart)
{
	Swap(xBestPath.indexArr[1],xBestPath.indexArr[iStart]);
	for(int i=2;i<nPtNum;i++)
	{
		double fMinDist=distArr[xBestPath.indexArr[i-1]][xBestPath.indexArr[i]];
		for(int j=i+1;j<=nPtNum;j++)
		{
			double fDist=distArr[xBestPath.indexArr[i-1]][xBestPath.indexArr[j]];
			if(fMinDist>fDist)
			{
				fMinDist=fDist;
				Swap(xBestPath.indexArr[i],xBestPath.indexArr[j]);
			}
		}
	}
	for(int i=1;i<=nPtNum;i++)
		bestIndexArr[i]=xBestPath.indexArr[i];
}
//重新排列顺序
void CTSPAlgorithm::RearRange(int iStart)
{ 
    int index=0,k=1;
	for(index=1;index<=nPtNum;index++)
	{
		if(xBestPath.indexArr[index]==iStart)
			break;
	}
	for(int j=index;j<=nPtNum;j++)
		bestIndexArr[k++]=xBestPath.indexArr[j];
	for(int j=1;j<=index-1;j++)
		bestIndexArr[k++]=xBestPath.indexArr[j];
}
//计算最优路径
void CTSPAlgorithm::CalBestPath(int iStart,int* arr)
{ 
	if(m_ciAlgType==1)
	{	//贪心算法进行路径优化
		Greedy(iStart);
	}
	else if (m_ciAlgType == 2)
	{	//回溯法进行路径优化
		Backtrack(2);
	}
	else //if(m_ciAlgType == 3)
	{	//模拟退火算法进行路径优化()
		Annealing();
		RearRange(iStart); 
	}
	for(int i=1;i<=nPtNum;i++)
		arr[i]=bestIndexArr[i];
}