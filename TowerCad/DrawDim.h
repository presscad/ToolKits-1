#pragma once
//#include "stdafx.h"
#include "f_ent.h"
#include "f_ent_list.h"
#include "f_alg_fun.h"
#include "XhCharString.h"
#include "HashTable.h"
#include "ArrayList.h"
#ifdef __DRAWING_CONTEXT_
#include "../LDS/MapLds/RxTools.h"
#endif

enum DIMALIGN_TYPE{AlignDefault=1,TopLeft,TopCenter,TopRight,MiddleLeft,MiddleCenter,MiddleRight,BottomLeft,BottomCenter,BottomRight};

class CDimStyle{
public:
#ifdef __DRAWING_CONTEXT_
	TMADRAWING_DIM_STYLE style;
#else
	struct DIM_STYLE
	{
	protected:
		double gap;				//�������׼�߼�϶
		double m_fInnerSetTextSize;
	public:
		CXhChar16 dimStyleName;	//������ʽ����
		double arrowSize;		//��ͷ��С
		//AcDbObjectId dimStyleId;	//������ʽ
	public:
		DIM_STYLE(){m_fInnerSetTextSize=arrowSize=gap=0;}
		void InitDimStyle(double textHeight=0,double arrowSize=0,double gap=0);
		double get_Gap(){return gap;}
		double set_Gap(double dimgap){return gap=dimgap;}
		__declspec(property(get=get_Gap,put=set_Gap)) double Gap;
		//static double CalGap(double text_height);
	} style;
#endif
	double textHeight;
	CDimStyle(){textHeight=2.0;}
};
struct SAFE_TEXTSIZE{
	double textsize;	//=0��ʾ����ϵͳ����Ĭ�ϴ�С
	double fontscale;	//���ָ߶�����ϵ��
	static double minFontSize;	//������С�߶�
	SAFE_TEXTSIZE(){
		textsize=0;
		fontscale=1.0;
	}
	double EnsureSize(double textheight=0){
		double th=textheight>0?textheight:textsize;
		th*=fontscale;
		return th<minFontSize?minFontSize:th;
	}
};
//�Ƕȱ�ע
class CSizeAngleDim
{
public:
	static CDimStyle dimStyle;
	f3dPoint vertex;		//�Ƕȱ�ע����
	f3dPoint dimStartPos;	//�Ƕȱ�ע��ʼ��
	f3dPoint dimEndPos;		//�Ƕȱ�ע��ֹ��
	f3dPoint dimPos;		//�Ƕȱ�ע����λ��
	CXhChar16 dimText;		//ĳЩ�����Ҫָ����ע����
	BYTE cFlag;				//�Ƕȱ�ע����״̬��ע
public:
	CSizeAngleDim();
	void Reset();
	double GetWidth();
	double GetHeight();
	double GetDimAngle();
	//���ر�ע������(������ע��ͷ)����rgnVertArr[0];����rgnVertArr[1];����rgnVertArr[2];�����ĸ��ǵ�rgnVertArr[3]
	void GetDimTextRect(f3dPoint rgnVertArr[4],double draw_scale=1.0,double text_size=0) const; //�ɸ��ݱ����õ����ο��С��Ĭ��Ϊ1 wht 10-09-16
	void GetDimRect(f3dPoint rgnVertArr[4],double draw_scale=1.0) const;
	void GetDimTextRect(f3dPoint rgnVertArr[4],SAFE_TEXTSIZE &safeTextSize) const;
	f2dArc GetDimArcLine();
};
//����ע�ߵ��ı��ߴ��ע
class CSizeTextDim
{
	CXhChar50 dimText;
	DIMALIGN_TYPE alignType;
public:
	static double PRECISION;		//�������־����עʱ�ľ���,ֻȡ1.0,0.5,0.1����ֵ
	static CDimStyle dimStyle;
	f3dPoint origin;		//���붨λ����λ��
	f3dPoint dimStartPos;	//��ע����ʼ��(����ڶ��붨λ����)
	f3dPoint dimEndPos;		//��ע����ֹ��(����ڶ��붨λ����)
	f3dPoint dimPos;		//��ע����λ��(����ڶ��붨λ����)
	f3dPoint initDimPos;	//��¼��ʼ��עλ��,ÿ�ε�����עλ��ʱ��initDimPosΪ��׼ wht 18-09-17
	double angle;			//����
	double dist;			//��ע����
	int nHits;				//�����ĵȾ�ΪS�ı�ע�����Ժϲ�Ϊһ��S X nHits�ı�ע
	//0.װ����ߴ� 1.�ֲ�ƫ�Ƴߴ� 2.�Ŷ���� 
	//3(-3).������λ�ߴ� (+-������) 4.����浽���������ע
	int iOrder;				//�ֲ��עʱ����ȷ���ߴ綨λ˳��
	int iTextDimSylte;		//���ֱ�ע��ʽ 0.��ע���ϲ� 1.��ע���²�
	BYTE cFlag;				//���ֱ�ע����״̬���
public:
	BOOL bCanTextOutter;	//֧�������ڳߴ�������ע wht 15-04-27
	BOOL bAllowRelayout;	//�Ƿ������Զ�������עλ��,ĳЩ������������Զ�����λ�� wht 10-08-15
	BOOL bShortenDimStartLine,bShortenDimEndLine;
	CSizeTextDim();
	CSizeTextDim(const double* dimStartCoords,const double* dimEndCoords,const double* dimPosCoords,
		double fDist,double angle,BOOL bAllowRelayout=FALSE,BOOL bCanTextOutter=FALSE);
	//precisionֻ��Ϊ"0","0.5","0.0"
	double Fillet();	//����ע����dist����precision����Բ����
	const char* DimText();
	double GetWidth();
	double GetHeight(BOOL bIncShortDimLineOnly=FALSE);
	//�ɸ��ݱ����õ����ο��С��Ĭ��Ϊ1 wht 10-09-16;
	//���ر�ע������(������ע��ͷ)����rgnVertArr[0];����rgnVertArr[1];����rgnVertArr[2];�����ĸ��ǵ�rgnVertArr[3]
	void GetDimRect(f3dPoint rgnVertArr[4],BOOL bIncShortDimLineOnly=FALSE,double draw_scale=1.0,double text_height=0) const;
	void GetDimRect(f3dPoint rgnVertArr[4],BOOL bIncShortDimLineOnly,SAFE_TEXTSIZE& safetextsize) const;
	DIMALIGN_TYPE AlignType(){return alignType;}
	void ResetPosition(DIMALIGN_TYPE align=AlignDefault,bool bAccumulation=false);
	void ResetPosition2(DIMALIGN_TYPE align=AlignDefault);
};

class CTextOnlyDrawing{
public:
	static double PRECISION;
	DIMALIGN_TYPE alignType;
	CXhChar50 dimText;		//��ע���ı�����
	f3dPoint origin;		//���붨λ����λ��
	double textHeight;
	f3dPoint dimPos;		//��ע����λ��(����ڶ��붨λ����)
	bool bNeedConvertDistToText;	//��Ҫ��dist��GetPrecisionDimText()�б�Ϊ��ע�ַ���
	double angle;			//����
	double dist;			//һ������dimText���Ӧ��˫�����������Ǳ��룩
	BOOL bInXWing;
	//0.���ӷ������� 1.���Գߴ� +-2(������).��ͼ����  3(-3).����ͼ�� +��ʾ��X�������� -��ʾ��X�Ḻ����  4.����������
	int iOrder;				//�ֲ��עʱ����ȷ���ߴ綨λ˳��
	BYTE cFlag;				//�ı���ע����״̬���
	BYTE m_nResetHits;
public:
	CTextOnlyDrawing(){iOrder=0;angle=dist=0;textHeight=2.0;alignType=MiddleLeft;bNeedConvertDistToText=true;cFlag=0;m_nResetHits=0;}
	double Fillet();
	//precisionֻ��Ϊ"0","0.5","0.0"
	const char* GetPrecisionDimText();
	//���ر�ע������(������ע��ͷ)����rgnVertArr[0];����rgnVertArr[1];����rgnVertArr[2];�����ĸ��ǵ�rgnVertArr[3]
	void GetDimRect(f3dPoint rgnVertArr[4],double draw_scale=1.0,double textsize=0); //�ɸ��ݱ����õ����ο��С��Ĭ��Ϊ1 wht 10-09-16
	void GetDimRect(f3dPoint rgnVertArr[4],SAFE_TEXTSIZE& safetextsize);
	double GetHeight();
	double GetWidth();
	DIMALIGN_TYPE AlignType(){return alignType;}
	void ResetPosition(DIMALIGN_TYPE align=AlignDefault,bool bAccumulation=false);
};

class CZhunXianTextDim : public CSizeTextDim
{
public:
	f3dPoint lineStart;
	f3dPoint lineEnd;
	long hRelaBolt;		//������˨���,���ܻ�ͼʱʹ�� wht 11-08-10
	bool bDimZhunJu;	//��ע����׼��
	bool bDimOnLeftSide;
	CZhunXianTextDim() {hRelaBolt=0;bDimZhunJu=bDimOnLeftSide=true;}
};

class ACDB
{
public:
	enum TextHorzMode      
	{	kTextLeft    = 0,   // TH_LEFT,
		kTextCenter  = 1,   // TH_CENT,
		kTextRight   = 2,   // TH_RIGHT,
		kTextAlign   = 3,   // THV_ALIGN,
		kTextMid     = 4,   // THV_MID,
		kTextFit     = 5 }; // THV_FIT

	enum TextVertMode      
	{	kTextBase    = 0,   // TV_BASE,
		kTextBottom  = 1,   // TV_BOT,
		kTextVertMid = 2,   // TV_MID,
		kTextTop     = 3 }; // TV_TOP
};

class CKaiHeAngleDim
{
	DIMALIGN_TYPE alignType;
public:
	CDimStyle dimStyle;
	CXhChar100 dimText;		//���ϽǱ�ע����(�����Ǹ��ϵ����λ�ü����ϽǶ�)
	f3dPoint origin;		//���붨λ����λ��
	//real_XXX��ʾ�����Ǳ����ߵĳߴ�
	f3dPoint dimPos,real_dim;			//���ϽǱ�עλ��(����ڶ��붨λ����)
	f3dPoint dimStartPos,start_ls_pos;	//���ϽǱ�ע��ʼ��(����ڶ��붨λ����)
	f3dPoint real_start,real_start_ls_pos;
	f3dPoint dimEndPos,end_ls_pos;		//���ϽǱ�ע��ֹ��
	f3dPoint real_end,real_end_ls_pos;
	double dist;		//���Ϸ�Χ
	BOOL bInWingX;
	int start0_end1_else2;	//��ʾ
public:
	CKaiHeAngleDim(){dist=0;bInWingX=0;start0_end1_else2=2;}
	//���ر�ע������(������ע��ͷ)����rgnVertArr[0];����rgnVertArr[1];����rgnVertArr[2];�����ĸ��ǵ�rgnVertArr[3]
	void GetDimRect(f3dPoint rgnVertArr[4]);
	double GetHeight();
	double GetWidth();
	DIMALIGN_TYPE AlignType(){return alignType;}
	void ResetPosition();
};
struct DATAPOINT_TAG
{
	DWORD key_tag;
	ARRAY_LIST<f3dPoint> vertexArr;
	DATAPOINT_TAG(){
		key_tag=0;
		vertexArr.Empty();
	}
};
#ifndef __TEXTFRAMDE_SIZE_DEF_
#define __TEXTFRAMDE_SIZE_DEF_
struct FRAMESIZE{
	short siFrameStyle;	//-1.�Զ��жϱ�ű�עȦ��ʽ;0.ȦȦ;1.��Բ����;2.��ͨ���ο�
	double fMargin;		//�ı���߿�ļ�϶
	double fFrameWidth;	//�߿�Ŀ��
	double fTextHeight;	//����߶�
	//double fFrameHeight;	//�߿�ĸ߶�һ���ɱ߿���ʽ���߾༰�߿��Ⱦ���
};
#endif

//���߿���ı�
class CTextFrame{
	double margin;		//�ı���߿�ļ�϶
	double frameWidth;	//�߿�ĳ���
	double frameHeight;	//�߿�ĸ߶�
	double textHeight;	//����߶�
	double WtoH;		//�����߱�
	CXhChar16 dimText,dimText2;	//�߿��б�ע���ı�
public:
	ACDB::TextHorzMode textAlignHori;
	ACDB::TextVertMode textAlignVert;
	static const double BULGERECT_EXTEND_COEF;	// = 0.1;��Բ���������ͨ���ο�ȵ����ӳ�ϵ��
	int frameStyle;		//-1.�Զ��ж�;0.ԲȦ;1.��Բ���ο�;2.��ͨ���ο�
	f3dPoint dataPos;	//����λ��
	f3dPoint dimPos;	//��עλ��
	long hPart;			//�����������
	int	 cls_id;		//������������
	int	 iCurLayer;		//�ı�����ͼ��
	DWORD dwKeyTag;		//���ݵ�ı�ǩ��ֵ
	BYTE cFlag;			//���ݵ�����״̬���
public:
	f3dPoint center,widthVec;	//�����ڻ�ͼʱ��ʱ��ȡ�����ã�ֻӰ��߿�λ�ò�Ӱ��߿��С
	CTextFrame();
	//��ʼ��
	void InitTextFrame(double text_height=2.5,double frame_width=0,
		int frame_style=-1,double space=0.2,double w_to_h=0.7);
	//���ܽ����߿� frameStyle:-1.�Զ��ж�;0.ԲȦ;1.��Բ���ο�;2.��ͨ���ο�
	bool BuildFrame(char* dimText,char* dimText2=NULL,double frameWidth=0,int frameStyle=-1,double text_height=0,double _margin=0);
	bool BuildFrame(char* dimText,FRAMESIZE& frame);
	bool BuildFrame(char* dimText,char* dimText2,FRAMESIZE& frame);
	void ZoomFrameSize(double zoomscale_coef,double fMinFontSize=1.0);
	//���������Ի�ȡ����
	char*  Text(){return dimText;}
	char*  Text2(){return dimText2;}
	__declspec( property (get=Text)) char* szText;
	__declspec( property (get=Text2)) char* szText2;
	//�߿�����:0.ԲȦ;1.��Բ���ο�;2.��ͨ���ο�
	int    Style(){return frameStyle;}
	//�߿���(ֱ��)
	double Width(){return frameWidth;}
	//�߿�߶�
	double Height(){return frameHeight;}
	//����߶�
	double TextHeight(){return textHeight;}
	//�߿���ı��ļ�϶
	double BoundarySpace(){return margin;}
	//�õ���ž��ο� //�ɸ��ݱ����õ����ο��С��Ĭ��Ϊ1 wht 10-09-16
	void GetDimRect(f3dPoint rgnVertArr[4],double draw_scale=1.0) const;
	//������������ı������Ӷ˵Ķ˵�λ�� 
	//���ݱ����õ������߶˽ڵ�λ�� ����1:1�麸ͼʱ��Ҫ wht 10-09-17
	f3dPoint GetRefLineEndPos(double draw_scale=1.0);
	//��������λ��
	void UpdateDataPos(CHashList<DATAPOINT_TAG>& xDataPointTagHash);
};
#ifdef __DRAWING_CONTEXT_
CXhChar16 GetPartNoIncMat(const char* sPartNo,char cMat);
#else
CXhChar16 GetPartNoIncMat(const char* sPartNo,char cMat,int iMatCharPosType=0,char cMatSeparator=0);
#endif
