#pragma once

#include "SegI.h"
#include "HashTable.h"
#include "ArrayList.h"
#if defined(__LDS_)||defined(__TMA_)||defined(__TSA_)||defined(__LMA_)||defined(__TDA_)||defined(__TAP_)
#include "f_ent_list.h"
#else
#include "atom_list.h"
#endif

bool IsAdaptNoEnd();
bool IsAdaptNoEndForSegI();
int GetNoArr(const char *limit_str,int no_arr[],int maxNoN);
long FindAdaptNo(const char *limit_str,char *delimiter1=NULL,char *delimiter2=NULL);
SEGI FindAdaptNoForSegI(const char *limit_str,const char *delimiter1=NULL,const char *delimiter2=NULL);
//ͨ���κ��ַ����õ��κŹ�ϣ��
DWORD GetSegNoHashTblBySegStr(const char* sSegStr,CHashList<SEGI> &segNoHashTbl);
//���ݶκŹ�ϣ���ȡ����Ķκ�����
int GetSortedSegNoList(CHashList<SEGI> &segNoHashList,ATOM_LIST<SEGI> &segNoList);
//���ݶκŹ�ϣ���ȡ�κ��ַ���
CXhChar200 SegNoHashListToString(CHashList<SEGI> &segNoHashList);
//��������Ķκ������ȡ�κ��ַ���
CXhChar200 SortedSegNoListToString(ATOM_LIST<SEGI> &segNoList);
//���ݶκ��ַ�����ȡ����Ķκ�����
int GetSortedSegNoList(const char* sSegStr,ATOM_LIST<SEGI> &segNoList);

struct COEF_NO{
	int no;
	double coef;
	COEF_NO(){coef=no=0;}
	COEF_NO(int no_,double coef_){no=no_;coef=coef_;}
	CXhChar100 ToString(bool bSegI,bool bIncCoef=true){
		CXhChar100 sNo("%d",no);
		if(bSegI)
			sNo.Printf("%s",(char*)SEGI((long)no).ToString());
		if(coef>1&&bIncCoef)
			sNo.Printf("%s*%.f",(char*)sNo,coef);
		return sNo;
	}
};
// <summary>
// ��һ������ϵ���ı�����е��ַ�������Ϊ�����ϵ�����
// limit_str ����������ַ��� </param>
// delimiter1����ż�ָ�����һ��Ϊ','</param>
// delimiter2����ŷ�Χ��������һ��Ϊ'-'</param>
// delimiter3����ŵĹ���ϵ���ָ���һ��Ϊ'*'</param>
COEF_NO* FindAdaptCoefNo(const char *limit_str,const char* delimiter1=NULL,const char *delimiter2=NULL,const char *delimiter3=NULL);

//���ַ�������ȡCOEF_NO����
void RetrievedCoefHashListFromStr(const char* coefNoStr,CHashList<COEF_NO> &hashCoefNoBySegI);
//����COEF_NO��ϣ���ȡ�����COEF_NO����
int GetSortedCoefNoList(CHashList<COEF_NO> &coefNoHashList,ATOM_LIST<COEF_NO> &coefNoList);
//���������COEF_NO�����ȡCOEF_NO�ַ���
CXhChar200 SortedCoefNoListToString(ATOM_LIST<COEF_NO> &coefNoList,bool bSegI);