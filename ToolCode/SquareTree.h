#pragma once

#include "stdafx.h"
#include "f_ent_list.h"

//二叉树
struct IDivCond{
	virtual void Clear()=0;
	virtual bool Copy(IDivCond* pCondition)=0;
	virtual bool LeftOn(IDivCond* pCondition)=0;
	virtual bool RightOn(IDivCond* pCondition)=0;
	virtual int Compare(IDivCond* pCondition)=0;	//<0:Left on; 0.相同; >0：Right on
};
template<class TYPE> class CSimpleCond : public IDivCond
{
	TYPE _condition;
public:
	void Clear(){_condition=0;}
	bool Copy(IDivCond* pCondition){_condition=*pCondition;}
	bool LeftOn(IDivCond* pCondition){return _condition<pCondition->cond;}
	bool RightOn(IDivCond* pCondition){return _condition>pCondition->cond;}
	int Compare(IDivCond* pCondition)	//<0:Left on; 0.相同; >0：Right on
	{
		if(_condition<pCondition->cond)
			return -1;
		else if(_condition>pCondition->cond)
			return 1;
		else
			return 0;
	}
	TYPE get_TYPE_COND(){
		return _condition;
	}
	void set_TYPE_COND(TYPE value){
		_condition=value;
	}
	__declspec( property(put=set_TYPE_COND,get=get_TYPE_COND)) TYPE cond;
};
template<class TYPE,class TYPE_COND> struct IBinTreeNode{
	virtual bool IsBelongLeft(TYPE_COND condition)=0;
	virtual bool IsBelongRight(TYPE_COND condition)=0;
	virtual bool AddSonNode(TYPE_COND division)=0;
	virtual IBinTreeNode* TranverseToSonNode(TYPE_COND cond)=0;
	virtual IBinTreeNode* GetLeftNode()=0;
	virtual IBinTreeNode* GetRightNode()=0;
};
template<class TYPE,class TYPE_COND> class BINTREENODE : public IBinTreeNode<TYPE,TYPE_COND>
{
protected:
	bool m_bLeafNode;		//最底层叶子节点
	TYPE_COND cond_pivot;	//判断条件的中枢分界点
	BINTREENODE *left,*right;
public:
	TYPE data;
	BINTREENODE(){left=right=NULL;cond_pivot.Clear();m_bLeafNode=true;}
	bool IsBelongLeft(TYPE_COND condition);
	bool IsBelongRight(TYPE_COND condition);
	bool AddSonNode(TYPE_COND division)
	{
		cond_pivot.Copy(&division);
		m_bLeafNode=false;
		if(left==NULL)
			left = new BINTREENODE<TYPE,TYPE_COND>();
		if(right==NULL)
			right = new BINTREENODE<TYPE,TYPE_COND>();
	}
	IBinTreeNode* GetLeftNode(){return left;}
	IBinTreeNode* GetRightNode(){return right;}
	IBinTreeNode* TranverseToSonNode(TYPE_COND cond)
	{
		int state=m_bLeafNode?0:cond.Compare(&_condition);
		if(state<0&&left)
			return left->TranverseToSonNode(cond);
		else if(state>0&&right)
			return right->TranverseToSonNode(cond);
		else if(state==0)
			return this;
		else
			return NULL;
	}
};
//四方格分叉树
//四叉树
struct ISquareDivCond : public IDivCond{
	virtual void Clear()=0;
	virtual bool Copy(IDivCond* pCondition)=0;
	virtual bool LeftOn(IDivCond* pCondition)=0;
	virtual bool RightOn(IDivCond* pCondition)=0;
	//分叉点四方格编号，如下：　2 3
	//							0 1	,-1表示超出四方格界限区域
	virtual int Compare(IDivCond* pCondition)=0;	//<0:Left on; 0.相同; >0：Right on
};
struct SQUARERGN : public ISquareDivCond{
	DWORD state;	//状态，0.未初始化，具体含义由用户自行定义（默认不在任一多边形域面内);
	double fHalfWidth;		//正方形区域的半宽度
	double X,Y;	//x,y分量中较小的角点
	double weight;			//加权特征值（可以是加权后的面积）
	double get_right(){return X+fHalfWidth+fHalfWidth;};
	double get_upper(){return Y+fHalfWidth+fHalfWidth;};
	__declspec( property(get=get_right)) double right;
	__declspec( property(get=get_upper)) double upper;
	SQUARERGN(double x=0,double y=0,double width=0){ 
		state=0;
		X=x;
		Y=y;
		fHalfWidth=width*0.5;
		weight=0;
	}
	SQUARERGN& operator = (SQUARERGN& square){
		fHalfWidth=square.fHalfWidth;
		X=square.X;
		Y=square.Y;
		weight=square.weight;
		return *this;
	}
public:
	void Clear(){fHalfWidth=X=Y=weight=0;}
	bool Copy(IDivCond* pCondition){
		SQUARERGN* pSquare=(SQUARERGN*)pCondition;
		fHalfWidth=pSquare->fHalfWidth;
		X=pSquare->X;
		Y=pSquare->Y;
		weight=pSquare->weight;
		return true;
	}
	bool LeftOn(IDivCond* pCondition){
		SQUARERGN* pSquare=(SQUARERGN*)pCondition;
		return pSquare->X<X;
	}
	bool RightOn(IDivCond* pCondition){
		SQUARERGN* pSquare=(SQUARERGN*)pCondition;
		return pSquare->X>X;
	}
	//分叉点四方格编号，如下：　2 3
	//							0 1	,-1表示超出四方格界限区域
	int Compare(IDivCond* pCondition)
	{
		SQUARERGN* pSquare=(SQUARERGN*)pCondition;
		if( pSquare->X<X||pSquare->X>right||
			pSquare->Y<Y||pSquare->Y>upper)
			return -1;
		if(pSquare->X>X+fHalfWidth&&pSquare->Y<Y+fHalfWidth)
			return 1;
		else if(pSquare->X<X+fHalfWidth&&pSquare->Y>Y+fHalfWidth)
			return 2;
		else if(pSquare->X>X+fHalfWidth&&pSquare->Y>Y+fHalfWidth)
			return 3;
		else
			return 0;
	}
};
template<class TYPE_COND> struct ISquareTreeNode{
	virtual int GetSquareIndex(TYPE_COND condition)=0;
	virtual bool AddSonNode(TYPE_COND division)=0;
	virtual ISquareTreeNode<TYPE_COND>* TranverseToSonNode(TYPE_COND cond)=0;
	virtual ISquareTreeNode<TYPE_COND>* GetSquareNode(int index)=0;
};
class SQUARENODE : public ISquareTreeNode<SQUARERGN>
{
protected:
	bool m_bLeafNode;	//最底层叶子节点
	SQUARENODE* nodes[4];
public:
	SQUARERGN square;	//判断条件的中枢分界点
	//以下三个常量是SQUARERGN::state的３个常用值
	static const DWORD COVER_NONE	= 1;	//方格与多边形域面无重叠
	static const DWORD COVER_WITH	= 2;	//方格与多边形域面有重叠
	static const DWORD COVER_IN		= 3;	//方格包含在多边形域面内
	SQUARENODE(){
		nodes[0]=nodes[1]=nodes[2]=nodes[3]=NULL;
		square.Clear();
		m_bLeafNode=true;
	}
	~SQUARENODE()
	{
		for(int i=0;i<4;i++)
		{
			if(nodes[i]!=NULL)
				delete nodes[i];
		}
	}
	bool IsLeftNode(){return m_bLeafNode;}
	virtual int GetSquareIndex(SQUARERGN region){
		return square.Compare(&region);
	}
	double SumStateWeight(int state)
	{
		if(m_bLeafNode)
		{
			if(square.state==state)
				return (square.fHalfWidth*2)*(square.fHalfWidth*2);
			return 0;
		}
		double weight=0;
		for(int i=0;i<4;i++)
		{
			if(nodes[i]==NULL)
				break;
			double square_weight=nodes[i]->SumStateWeight(state);
			weight+=square_weight;
		}
		return weight;
	}
	virtual bool AddSonNode(SQUARERGN division)
	{
		square.Copy(&division);
		m_bLeafNode=false;
		if(nodes[0]!=NULL)
			return true;
		nodes[0] = new SQUARENODE();
		nodes[1] = new SQUARENODE();
		nodes[2] = new SQUARENODE();
		nodes[3] = new SQUARENODE();
		return true;
	}
	virtual ISquareTreeNode<SQUARERGN>* TranverseToSonNode(SQUARERGN cond)
	{
		if(m_bLeafNode)
			return this;
		int index=square.Compare(&cond);
		if(index>=0&&index<4)
			return nodes[index];
		else
			return NULL;
	}
	virtual ISquareTreeNode<SQUARERGN>* GetSquareNode(int index){
		if(index>=0&&index<4)
			return nodes[index];
		else
			return NULL;
	}
};
class POLYGONEX : public POLYGON
{
	GECS lcs;
	void CalLocalCS();
	//-1:方格不在多边形面域上;1.方格与多边形面域无重叠;2.方格与多边形域面有重叠;3.方格在多边形域面内
	static int IncludeState(GEPOINT *vertexes, int count,const double* x_y,double sqare_width);
public:
	POLYGONEX();
	~POLYGONEX();
	double CalPerimeter();
	double OverlappedWith(GEPOINT *vertexes2,int count2,SQUARENODE* pRootNode=NULL,double DECISION=2);
	int DetectSquareCoverState(GEPOINT *vertexes1,GEPOINT *vertexes2, int count2,SQUARENODE* pSquareNode,double DECISION=2);
};
