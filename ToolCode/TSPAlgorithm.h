#pragma once
#include "ArrayList.h"
#include "f_ent.h"

#define MAXN     300          //最多点数
#define INIT_T   2000         //初始温度
#define RATE     0.95         //温度衰减率
#define FINAL_T  1E-10        //终止温度
#define IN_LOOP  13000        //内层循环次数
#define OUT_LOOP  2000        //外层循环次数
#define P_LIMIT  10000        //概率选择次数
//
struct PATH{
	int indexArr[MAXN];		//依次遍历的序号
	double fSumDist;		//路径总长度
};
class CTSPAlgorithm
{
private:
	int nPtNum;                     //节点数量
	double m_fDist;					//用户记录临时多点间距离
	double distArr[MAXN][MAXN];		//任意两点之间的距离
	PATH xBestPath;                 //最优的遍历路径
	int bestIndexArr[MAXN];			//最优的索引序号
	BOOL m_bLoop;					//路径是否构成环
	BYTE m_ciAlgType;				//算法类型
protected:
	double TotalDist(PATH path);
	PATH GetNext(PATH cur_path);
	void RearRange(int iStart);
	void Annealing();			//退火算法
	void Backtrack(int i);		//回溯算法
	void Greedy(int iStart);	//贪心算法 
	void Swap(int &a ,int &b);
public:
	CTSPAlgorithm();
	~CTSPAlgorithm();
	void InitData(ARRAY_LIST<f3dPoint>& ptArr,BYTE ciAlgType=0);
	void CalBestPath(int iStart,int* arr);
};