#pragma once

#include "f_ent.h"
#include "f_ent_list.h"
#include "f_alg_fun.h"
#ifndef _LEGACY_LICENSE
#include "XhLdsLm.h"
#else
#include "Lic.h"
#endif
#include "Buffer.h"
#include "XhCharString.h"
#include "SegI.h"
#include "HiberarchyId.h"
#ifdef __TAP_CONTEXT_
#include "SolidBody.h"
#include "PartLib.h"
#endif
#ifdef __PROPERTYLIST_UI_
#include "PropListItem.h"
#endif
#include "MemberProperty.h"
#include "HashTable.h"
#include "ComplexId.h"
#ifdef  __DRAWING_CONTEXT_
#include "Tower.h"
#endif

#ifndef __DEF_KAIHEJIAO_STRUC__
#define __DEF_KAIHEJIAO_STRUC__
struct KAI_HE_JIAO
{
	float decWingAngle;		//��֫�н�
	short position;			//�궨λ��
	short startLength;		//ʼ�˿��ϳ���
	short endLength;		//�ն˿��ϳ���
	KAI_HE_JIAO() {memset(this,0,sizeof(KAI_HE_JIAO));}
};
#endif
struct PROFILE_VER{
	HIBERID hiberId;				//���洢,ʹ��+2��ռλ�洢�ְ�������
	DWORD keyId;					//�洢������Ψһ��ʶhiberId
	BYTE type;						//1:��ֱͨ�� 2:Բ�� 3:��Բ��
	bool m_bWeldEdge;				//�Ƿ񺸷��
	bool m_bRollEdge;				//�˱�����
	short roll_edge_offset_dist;	//���ƫ�ƾ���
	double radius;					//��Բ������Ҫ�Ĳ���
	double sector_angle;			//��׼Բ������Ҫ�Ĳ���
	short manu_space;				//����ӹ�Ԥ����϶���߸߶�(mm)
	f3dPoint vertex;				//�����λ������
	f3dPoint center,column_norm;	//��Բ������Ҫ�Ĳ�������Զ�����������),center�����ʾԲ��Բ��(ʰȡ��)ʱ���������������
	f3dPoint work_norm;				//Բ�����߷���
	double local_point_y;			//�Ի��λ�ã��ڸõ������ע
	int local_point_vec;			//0.��ʾ�޶Ի�㣬1.��ʾ�Ի�������ע50-->100�ķ����뺸�Ӹ��˼���start-->end������ͬ��2.��ʾ�෴
#ifdef __TAP_CONTEXT_
	bool m_bUpVertex;
	double rayVecX,rayVecY;
#endif
	PROFILE_VER(f3dPoint pt);
	PROFILE_VER(double x=0,double y=0,double z=0);
	void SetKey(DWORD key){keyId=key;hiberId.SetHiberarchy(0,0,0,keyId);}
	BOOL CloneVertex(PROFILE_VER *pNewVertex);
	void RetrieveArcLine(f3dArcLine &arcline,f3dPoint endPoint,UCS_STRU *ucs);
#ifdef __PROPERTYLIST_UI_
	DECLARE_PROP_FUNC(PROFILE_VER)
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);
#endif
};
struct BOLT_INFO
{
private :
	short _bolt_d;
public:
	//����λ���
	static const DWORD TWO_CAP;		//˫ñ��˨��ʶλ
	static const DWORD ANTI_THEFT;	//������˨��ʶλ
	static const DWORD ANTI_LOOSE;	//�����ɵ�Ȧ��ʶλ
	static const DWORD FOOT_NAIL;	//�Ŷ���˨��ʶλ
	static const DWORD REVERSE_NORM;//��ת��˨�����ʶλ
	//��˨��������
	static const BYTE FUNC_COMMON	= 0;//0.������˨
	static const BYTE FUNC_FOOTNAIL	= 1;//1.�Ŷ����۸֡��Ǹֱ�ʾ�Ŷ��������ְ��ʾ�Ŷ���;�ĸְ�
	static const BYTE FUNC_WIREHOLE	= 2;//2.���߿�;
	static const BYTE FUNC_EARTHHOLE= 3;//3.�ӵؿ�;
	static const BYTE FUNC_SETUPHOLE= 4;//4.װ���;
	static const BYTE FUNC_BRANDHOLE= 5;//5.���ƿ�;
	static const BYTE FUNC_WATERHOLE= 6;//6.������
	static const BYTE FUNC_FOOTNAIL2= 7;//7.���׽Ŷ�

	HIBERID hiberId;			//���洢,ʹ��+1��ռλ�洢��˨id
	DWORD keyId;				//�洢������Ψһ��ʶhiberId
	BYTE cFaceNo;				//��˨�����������(�ְ���˨��Ҫ) ��ʱʹ�ò��洢
	
	PROPERTY(double, bolt_d);
	GET(bolt_d)
	{
		double fBoltD = _bolt_d;
		if (_bolt_d > 1000)	//С��*100,��������ʽ�洢 wht 19-09-12
			fBoltD = _bolt_d * 0.01;
		return fBoltD;
	}
	SET(bolt_d) {
		double fBoltD = value;
		int nBoltD = (int)fBoltD;
		if (fabs(fBoltD - nBoltD) > EPS2)
			_bolt_d = (int)(fBoltD * 100);	//С��*100,��������ʽ�洢 wht 19-09-12
		else
			_bolt_d = (int)value;
	}
	float hole_d_increment;		//��˨��ֱ������
	short waistLen;				// ��Բ������
	GEPOINT waistVec;			// ��Բ�׷���
	float posX;					//
	float posY;					//
	DWORD dwRayNo;				//��˨���������ߺ�
	BITFLAG m_dwFlag;			// ������Ҫ��ı�ʶλ����ANTITHEFT_BOLT(1),ANTILOOSE_BOLT(2),FOOTNAIL_BOLT(4)
	long feature;
	BYTE cFuncType;				//��˨������
	BYTE cFlag;					//��˨�׼ӹ���������  0.���  1.���
	BOLT_INFO();
	void SetKey(DWORD key){keyId=key;hiberId.SetHiberarchy(0,0,2,keyId);}
	BOOL IsFootNail(){ return m_dwFlag.IsHasFlag(FOOT_NAIL);}
	BOOL IsAntiTheft(){ return m_dwFlag.IsHasFlag(ANTI_THEFT);}
	BOOL IsTwoCap(){ return m_dwFlag.IsHasFlag(TWO_CAP);}
	BOOL IsAntiLoose(){ return m_dwFlag.IsHasFlag(ANTI_LOOSE);}
	BOOL CloneBolt(BOLT_INFO *pNewBoltInfo);
	void ToBuffer(CBuffer &buffer,long version,int buf_type);
	void FromBuffer(CBuffer &buffer,long version,int buf_type);
#ifdef __PROPERTYLIST_UI_
	DECLARE_PROP_FUNC(BOLT_INFO);
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//ͨ������Id��ȡ����ֵ
#endif
};
struct RIB_PLATE_INFO
{
	HIBERID hiberId;		 //���洢��ʹ��-1��ռλ�洢�Ǹ��߰�
	float fThick;            //�߰���
	float fPosZ;			 //�߰�λ��
	CXhChar16 sPlateNo;		 //�߰���
	char m_cMaterial;		 //����
	RIB_PLATE_INFO()
	{
		fThick=fPosZ=0;
		m_cMaterial='S';
	}
};
struct POLY_ANGLE_INFO
{
	int nIndex;				//��������
	GEPOINT norm_x_wing;	//X֫չ������
	GEPOINT norm_y_wing;	//Y֫չ������
	GEPOINT len_vec;		//���췽��
};
//////////////////////////////////////////////////////////////////////////
//CGeneralPart
//////////////////////////////////////////////////////////////////////////
class CGeneralPart
{
public:
	static const BYTE TYPE_LINEANGLE	= 1;	//�Ǹ�
	static const BYTE TYPE_BOLT			= 2;	//��˨
	static const BYTE TYPE_PLATE		= 3;	//�ְ�
	static const BYTE TYPE_LINETUBE		= 4;	//�ֹ�
	static const BYTE TYPE_LINEFLAT		= 5;	//����
	static const BYTE TYPE_LINESLOT		= 6;	//�۸�
	static const BYTE TYPE_SPHERE		= 7;	//���
#ifdef __DRAWING_CONTEXT_
	CLDSPart *pPart;
#elif defined(__TAP_CONTEXT_)
	CGeneralPart* pNextPart;		//��������������ͬһ��Ź���������ͬ��ͬʱ������ wjh-2016.1.26
	bool AttachSameLabelPart(CGeneralPart* pSameLabelPart);
#endif
protected:
	KEY4C m_dwPartKey;
	BYTE _ciModifiedFlag;			//�����޸ı�ʶ��Undo��ʶ, ������Undo����ʵ�ֹ����м�¼�����Ƿ������޸Ĳ��洢���ļ� wjh-2017.7.12
	BYTE _cPartType;				//��������
	SEGI m_Seg;						//�κ�
	CXhChar100 m_sSpec;				//���
	float _fWeight;					//����
	float _fSumWeight;				//���أ����洢 wht 19-08-22
	CXhChar50 m_sNote;				//��ע
	KEY4C m_dwOrgPartKey;			//��ǰ������Ӧ��ԭʼ����	wht 15-05-23
public:
	CXhChar100 m_sRelatePartNo;		//�����ļ��ţ�����һ���ŵ������	wxc-2016.12.13
	BYTE m_cFuncType;				//
	long feature;					//���洢
	virtual BOOL SetModified(BOOL bTrue=TRUE);
	virtual void ClearUndoModifiedFlag();
	DECLARE_READONLY_PROPERTY(bool,bDataModified);
	DECLARE_READONLY_PROPERTY(bool,bUndoModified);
	PROPERTY(BYTE,m_cPartType);
	GET(m_cPartType)
	{
#ifdef __DRAWING_CONTEXT_
		if(pPart)
			if(pPart->GetClassTypeId()==CLS_LINEANGLE||pPart->GetClassTypeId()==CLS_POLYPART)
				_cPartType=TYPE_LINEANGLE;
#endif
		return _cPartType;
	}
	SET(m_cPartType){
		_cPartType=value;
	}
	PROPERTY(float,m_fWeight); 
	GET(m_fWeight){
#ifdef __DRAWING_CONTEXT_
		if(pPart)
			return (float)pPart->GetWeight();
#endif
		return _fWeight;
	}
	SET(m_fWeight){
		_fWeight=value;
	}
	PROPERTY(float, m_fSumWeight);
	GET(m_fSumWeight) { return _fSumWeight; }
	SET(m_fSumWeight) { _fSumWeight = value; }
public:
	CGeneralPart();
	~CGeneralPart();
	SEGI GetSegI();
	SEGI SetSegI(SEGI seg);
	__declspec( property(put=SetSegI,get=GetSegI)) SEGI iSeg;
	char * GetSpec(BOOL bIncMat=TRUE);
	char* SetSpec(char* spec);
	char * GetNotes();
	char* SetNotes(const char* sNotes);
	void SetKey(KEY4C key){m_dwPartKey=key;}
	KEY4C GetKey(){return m_dwPartKey;}
	__declspec( property(get=GetKey)) KEY4C Key;
	UINT GetId(){return m_dwPartKey.dwItem;}
	__declspec( property(get=GetId)) UINT Id;
	void SetOrgPartKey(KEY4C key){m_dwOrgPartKey=key;}
	KEY4C GetOrgPartKey(){return m_dwOrgPartKey;}
	virtual CXhChar16 GetPartNo()const=0;
	virtual bool IsProcessPart(){return false;}
	virtual bool IsBolt(){return false;}
	virtual bool IsRod(){return false;}
	virtual bool IsPlate(){return false;}
	virtual bool IsAngle(){return false;}
	virtual bool IsTube(){return false;}
	virtual bool IsSlot(){return false;}
	virtual bool IsFlat(){return false;}
	virtual bool IsEqual(CGeneralPart *pPart,double epsilon=1.0,char *error_info=NULL,DWORD dwChkStyleFlag=NULL){return false;}
#ifdef __TAP_CONTEXT_
	virtual void FromTIDBuffer(CBuffer &buffer,long version=0)=0;
	virtual void FromTAPBuffer(CBuffer &buffer,long version=0)=0;
	virtual void ToTAPBuffer(CBuffer &buffer,bool bIncSolidData=true)=0;
#endif
#if defined(_DEBUG)&&defined(__TAP_CONTEXT_)
	virtual int GetMemorySize();
#endif
};
//////////////////////////////////////////////////////////////////////////
//CProcessPart
//////////////////////////////////////////////////////////////////////////
#ifdef __TAP_CONTEXT_
struct ASSEMBLEY_POS{
	static const DWORD POS_FRONT_PLUS		= 1;	//4ռλ:ǰ 8ռλ:ǰX+(Ʋ)
	static const DWORD POS_BACK_PLUS		= 2;	//4ռλ:�� 8ռλ:��X+(Ʋ)
	static const DWORD POS_LEFT_PLUS		= 3;	//4ռλ:�� 8ռλ:��Y+(Ʋ)
	static const DWORD POS_RIGHT_PLUS		= 4;	//4ռλ:�� 8ռλ:��Y+(Ʋ)
	static const DWORD POS_FRONT_MINUS		= 5;	//4ռλ:�� 8ռλ:ǰX-(��)
	static const DWORD POS_BACK_MINUS		= 6;	//4ռλ:�� 8ռλ:��X-(��)
	static const DWORD POS_LEFT_MINUS		= 7;	//4ռλ:�� 8ռλ:��Y-(��)
	static const DWORD POS_RIGHT_MINUS		= 8;	//4ռλ:�� 8ռλ:��Y-(��)
	
public:
	DWORD m_dwInitPlace;			//���洢����TID�ļ���װ���¼��˳���һ�� wht 16-12-05
	DWORD m_dwAssemblePlace;		//1~8Ϊ�ض�װ��λ�ã����಻����װ��λ������Ӧ��9��ʼ
	DWORD m_dwBlockIndex;			//0:��ʾ������ģ��   >1��ʾ���ڲ���ģ�͵�������
	GECS acs;
	bool m_bIsRod;					//�Ժ����Ӧȥ����wjh-2017.7.13
	GEPOINT startPos,endPos;		//�Ժ����Ӧȥ����wjh-2017.7.13
	CFGWORD cfgword;				//��ĺ�
	BYTE cLegQuad;					//�������޽��Ⱥ�
public:
	ASSEMBLEY_POS();
	ASSEMBLEY_POS(const ASSEMBLEY_POS &srcParam);
	ASSEMBLEY_POS& operator=(const ASSEMBLEY_POS &srcParam);
	void SetKey(DWORD key){m_dwAssemblePlace=key;}
	DWORD GetKey(){return m_dwAssemblePlace;}
	void ToBuffer(CBuffer &buffer,long version=0,int doc_type=0);
	void FromBuffer(CBuffer &buffer,long version=0,int doc_type=0);
	void CloneFrom(const ASSEMBLEY_POS &srcAssembleyPos);
	bool IsInited();
#ifdef _DEBUG
	int GetMemorySize();
#endif
};
#endif
#ifdef __TAP_CONTEXT_
class CProcessPart;
typedef CXhChar100 (*PLACE_CONTENT_FUNC)(CProcessPart *pPart,BYTE cPlaceId);
#endif
class CProcessPart : public CGeneralPart
{
public:
	union BUFFER_VERSION{
		struct{WORD major,revision;};
		UINT uiVersion;
		long version;
		BYTE bytes[4];
		int Compare(const BUFFER_VERSION& ver);
		BUFFER_VERSION(long v=0){version=v;}
	};
	static const int PPI_BUFFER		=1;
	static const int TAP_BUFFER		=2;
	static const int TID_BUFFER		=3;

	static const UINT PATTERN_NONE		= 0;			//��ȫ������ά����ģ��
	static const UINT PATTERN_INHERIT	= 0x0040;		//����̳�,������ά����ģ�ͼ̳и���
	static const UINT PATTERN_OVERWRITE	= 0xffffffff;	//����,��ȫ��PPE���߱༭�洢
	//�����������ʶλ��m_dwFlag��Ӧ��������
	static const UINT PATTERN_MATERIAL	= 0x0001;	//����
	static const UINT PATTERN_SIZE		= 0x0002;	//���
	static const UINT PATTERN_LENGTH	= 0x0004;	//����
	static const UINT PATTERN_COUNT		= 0x0008;	//������
	static const UINT PATTERN_PROP_CONNBOLTS= 0x0010;	//������˨
	static const UINT PATTERN_PROP_ONLYHOLES= 0x0020;	//�����
	static const UINT PATTERN_MKPOS		= 0x0040;		//��ģ(��ӡ)λ��
	BUFFER_VERSION m_verSerial;	//�����İ汾��ˮ��
	DWORD m_dwInheritPropFlag;	//��ԭʼ��ά����ģ�ͼ�ļ̳й�ϵ
	long  m_idSrcObject;		//ԭʼ��ά����ģ���ж�Ӧ��Դ����Id(��T/LMA�������)
#ifdef __TAP_CONTEXT_
public:
	CSolidBody solid;				//ʵ������
	CHashListEx<ASSEMBLEY_POS> acsPosList;	//������װ������ϵ����
	static const BYTE PLACE_MODE_NONE	= 0;//�Զ���
	static const BYTE PLACE_MODE_4_1	= 1;//4ռλģʽ1
	static const BYTE PLACE_MODE_4_2	= 2;//4ռλģʽ2
	static const BYTE PLACE_MODE_8_1	= 3;//8ռλģʽ1
	static const BYTE PLACE_MODE_8_2	= 4;//8ռλģʽ2
	BYTE m_cPlaceMode;				//����ռλģʽ
	BYTE m_cPlaceStateFromFile;		//����ռλ״̬(���ļ���ʼ����)
	BYTE m_cPriorPlaceState;		//��������ռλ״̬
	char m_cOrgMaterial;					//ԭ����
	char m_cOrgQuality;						//ԭ�����ȼ� wht 19-09-15
	float m_fOrgWidth,m_fOrgThick;			//ԭ֫��ȡ����
	WORD m_wOrgLength;						//ԭʼ����
	BOOL IsIdleClassicPlace(BYTE cPlaceId);
	BOOL IsExistPriorClassicPlace(BYTE cPlaceId);
public:
	static PLACE_CONTENT_FUNC GetPlaceContentFunc;
	virtual bool IsReplacePart();	//�Ƿ�Ϊ���Ϲ���
	virtual void FromTIDBuffer(CBuffer &buffer,long version=0);
	virtual void FromTAPBuffer(CBuffer &buffer,long version=0);
	virtual void ToTAPBuffer(CBuffer &buffer,bool bIncSolidData=true);
#endif
protected:
	//����������Ϣ
	char _cMaterial;				//����
	char _cQuality;					//�����ȼ� wht 19-09-15
	float _fWidth;					//���
	WORD _wLength;					//����
	float _fThick;					//���
	float _fHigh;					//�߶�
	CXhChar16 m_sPartNo;			//������
public:
	CBuffer m_xOrgProcessBuffer;	//ԭʼ������Ϣbuffer,�����ȶԹ�����Ϣ�Ƿ����仯 wht 15-05-23
	CHashListEx<BOLT_INFO> m_xBoltInfoList;	//��˨��Ϣ
public://��������
	//�ֺ�λ��,���ڽǸ֣�X��ʾ������룬Y��ʾ�ľࣨ��������ֵ������֫��
	GEPOINT mkpos,mkVec;
	int m_nDanJiNum;				//���������ݲ��洢��
	PROPERTY(char,cMaterial);
	GET(cMaterial)
	{
#ifdef __DRAWING_CONTEXT_
		if(pPart)
			return pPart->cMaterial;
#endif
		return _cMaterial;
	}
	SET(cMaterial){
		_cMaterial=value;
	}
	PROPERTY(char, cQuality);
	GET(cQuality)
	{
#ifdef __DRAWING_CONTEXT_
		if (pPart)
			return pPart->cQualityLevel;
#endif
		return _cQuality;
}
	SET(cQuality) {
		_cQuality = value;
	}
	PROPERTY(float,m_fThick); 
	GET(m_fThick){
#ifdef __DRAWING_CONTEXT_
		if(pPart)
			return (float)pPart->GetThick();
#endif
		return _fThick;
	}
	SET(m_fThick){
			_fThick=value;
	}
	PROPERTY(WORD,m_wLength); 
	GET(m_wLength){
#ifdef __DRAWING_CONTEXT_
		if(pPart)
			return (WORD)ftoi(pPart->GetLength());
#endif
		return _wLength;
	}
	SET(m_wLength){
			_wLength=value;
	}
	PROPERTY(float,m_fWidth); 
	GET(m_fWidth){
#ifdef __DRAWING_CONTEXT_
		if(pPart)
			return (float)pPart->GetWidth();
#endif		
		return _fWidth;
	}
	SET(m_fWidth){
			_fWidth=value;
	}
	PROPERTY(float,m_fHigh); 
	GET(m_fHigh){
#ifdef __DRAWING_CONTEXT_
		if(pPart)
			return (float)pPart->GetHeight();
#endif
		return _fHigh;
	}
	SET(m_fHigh){
			_fHigh=value;
	}
	GEPOINT GetMarkPos(){return mkpos;};
	GEPOINT SetMarkPos(GEPOINT pos){return mkpos=pos;};
	CXhChar16 GetPartNo()const;
	const char* GetLabel()const;
	void SetPartNo(const char *part_no,bool bIncStdPart=false);
	__declspec( property(get=GetLabel)) const char* Label;
	char * GetSpec(BOOL bIncMat=TRUE);
	float GetWidth();	//��λΪmm
	float GetThick();	//��λΪmm
	float GetHeight();	//��λΪmm
	WORD GetLength();	//��λΪmm
	float GetWeight(BOOL bSolidLength=TRUE);	//��λΪkg
	virtual bool IsProcessPart(){return true;}
	virtual bool IsBolt(){return false;}
public:
	CProcessPart();
	~CProcessPart();
	static void QuerySteelMatMark(char cMat,char* sMatMark);
	//����m_dwInheritPropFlag����ppibufFromPart*����CLDSPart::ToPPIBuffer) wjh-2017.1.18
	virtual bool MergeToSelfWithModel(CBuffer& bufFromModelPart);
	virtual bool MergeToSelfWithPPE(CBuffer& bufFromPpePart);
	virtual bool MergeToSelfWithPPE(CProcessPart* pPpePart);
	virtual bool ClonePart(CProcessPart *pNewPart);	//��¡PPI������Ϣ
	virtual int GetVertexCount(){return 0;}
	virtual void ReadProcessBuffer(CBuffer& buffer,long version=0,bool bSaveSubSerial=false,int buf_type=0){;}
	virtual void WriteProcessBuffer(CBuffer& buffer,long version=0,bool bSaveSubSerial=false,int buf_type=0){;}
	static BYTE RetrievedPartTypeAndLabelFromBuffer(CBuffer &buffer,char *sPartLabel=NULL,UINT maxStrBuffLen=17);
	virtual void ToPPIBuffer(CBuffer &buffer);
	virtual void FromPPIBuffer(CBuffer &buffer);
	virtual void ReadPPIBufferBasicInfo(CBuffer &buffer,long* pVersion=NULL);
	virtual void ReadBasicPPIAttachBuffer(CBuffer &buffer,long version=0);
	virtual PROFILE_VER* FromVertexHiberId(HIBERARCHY id){return NULL;}
	virtual bool DeleteVertexByHiberId(HIBERARCHY id){return false;}
	//��˨����
	virtual BOLT_INFO* FromHoleId(DWORD holeId);
	virtual bool DeleteBoltHoleById(DWORD id);
	virtual BOLT_INFO* FromHoleHiberId(HIBERARCHY id);
	virtual bool DeleteBoltHoleByHiberId(HIBERARCHY id);
	virtual int GetLsString(char *ls_str);
	virtual int GetLsCount(short d=0);	//�������CLDSPart::GetLsCount����һ�£�����������ΪGetBoltCount
	int GetDrillHoleCount();
	//acs			����װ������ϵ
	//ls_norm		��˨����(��������ϵ��)
	//ls_pos_coord	��˨����(�����������,����������ϵ)
	virtual BOOL SpreadLsPos(GECS &acs,f3dPoint ls_norm,double* ls_pos_coord){return FALSE;}
	//��չ���������-->ʵ���������(��ʵ�ʾ�������)
	virtual f3dPoint RestoreLsPos(const double* ls_pos,int iFace012=0,GECS *pAcs=NULL){return f3dPoint();}
	CBuffer *GetOrgProcessBuffer();
	void WriteOrgProcessBuffer(CBuffer &buffer);
	void InitOrgProcessBuffer(CBuffer &orgBuffer);
	static const DWORD CHECK_STYLE_BASIC	= 0x01;
	static const DWORD CHECK_STYLE_ACS_POS	= 0x02;
	virtual bool IsEqual(CGeneralPart *pPart,double epsilon=1.0,char *error_info=NULL,DWORD dwChkStyleFlag=NULL);
#ifdef __PROPERTYLIST_UI_
	static PROPLIST_TYPE propSync;
	static void InitSyncProp();
	PROPLIST_TYPE* GetSyncPropList(){return &propSync;}
	virtual int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100){return 0;}	//ͨ������Id��ȡ����ֵ
#endif
};
typedef CProcessPart* CProcessPartPtr;
class CProcessPartList : public CSuperHashStrList<CProcessPart>
{
public:
	static CProcessPart* CreateProcessPart(int idPartClsType,const char* key,void* pContext);
	static BOOL DestroyProcessPart(CProcessPart* pPart);
	CProcessPartList();
};
struct CUT_POINT{
public:
	DWORD hEntId;		//�����㡢��
	float fInAngle,fOutAngle;
	BYTE cInLineLen,cOutLineLen;
	void SetKey(long hKey){hEntId=hKey;}
	HIBERID GetHiberId(DWORD hPlate){
		HIBERID hiberId(hPlate);
		hiberId.SetHiberarchy(0,0,3,hEntId);
		return hiberId;
	}
	HIBERID GetInLineHiberId(DWORD hPlate){
		HIBERID hiberId(hPlate);
		hiberId.SetHiberarchy(0,0,4,hEntId);
		return hiberId;
	}
	HIBERID GetOutLineHiberId(DWORD hPlate){
		HIBERID hiberId(hPlate);
		hiberId.SetHiberarchy(0,0,5,hEntId);
		return hiberId;
	}
	CUT_POINT(){
		memset(this,0,sizeof(CUT_POINT));
		cInLineLen=10;
		cOutLineLen=5;
	}
	void ToBuffer(CBuffer &buffer,long version);
	void FromBuffer(CBuffer &buffer,long version);
#ifdef __PROPERTYLIST_UI_
	DECLARE_PROP_FUNC(CUT_POINT);
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//ͨ������Id��ȡ����ֵ
#endif
};

class CProcessPlate : public CProcessPart
{
public:
	static const BYTE FUNC_COMMON		= 0;	//��ͨ����
	static const BYTE FUNC_FOOTNAIL		= 1;	//�Ŷ����۸֡��Ǹֱ�ʾ�Ŷ��������ְ��ʾ�Ŷ���;�ĸְ�
	static const BYTE FUNC_REINFORCE	= 2;	//��(ǿ)�̣��Ǹֱ�ʾ�Ӿ��̽Ǹ֣��ְ��ʾ��ǿ�߰�
	static const BYTE FUNC_WIREPLATE	= 3;	//���߰�
	static const BYTE FUNC_STEPPLATE	= 4;	//�����ϵ�̤��
	static const BYTE FUNC_SETUPPLATE	= 5;	//������װ�Ұ�
	static const BYTE FUNC_FOOTPLATE	= 7;	//���Ű�
	static const BYTE FUNC_GASKET		= 8;	//���
public:
	//�ְ������Ϣ
	BYTE m_cFaceN;					//����(�������)
	f3dLine HuoQuLine[2];			//�����ߣ���������꣩
	f3dPoint HuoQuFaceNorm[2];		//�ڵ�����ķ��߷���
	double m_fInnerRadius;			//��Բ���뾶
	f3dPoint top_point;				//������������ߵĽ��㣨��������꣩
	f3dPoint m_tInnerOrigin;			//��ԲԲ��λ��---�������	
	f3dPoint m_tInnerColumnNorm;		//��Բ���߷���
	double m_fHuoQuLineR[2];			//�����ߵĻ����뾶
	double m_fHuoQuLineX[2];			//�����ߵ����Բ�ϵ��
	bool m_bIncDeformed;			//�Ƿ��ǻ��� 0.δ���� 1.�ѿ���
	BYTE m_ciDeformType;			//�������δ���ʽ 0.װ���� 1.���Բ� 2.͹�� 3.����
	BYTE m_ciRollProcessType;		//��ߴ���ʽ 
	BYTE m_ciRollOffsetType;		//���ƫ�Ʒ�ʽ 0.��� 1.�ڲ�
	//��������Ϣ
	CHashListEx<PROFILE_VER> vertex_list;
	/*
	mcs��Ӱ������
	cBottomEdge�ײ���������m_wBottomEdge==-1��ʾδ��ʼ��NC����ģʽ����
	cOvertrun �Ƿ�����˷�ת					
	*/
	union MCS_FLAG{
		WORD wFlag;
		struct{ BYTE ciBottomEdge,ciOverturn;};
	}mcsFlg;
	//
	CUT_POINT m_xCutPt;
	CHashListEx<CUT_POINT> m_hashHoleCutPtByKey;
public:
	CProcessPlate();
	void CutPointToBuffer(CBuffer &buffer,long version=0);
	void CutPointFromBuffer(CBuffer &buffer,long version=0);
	const static BYTE CUT_PT_CIR	= 0;
	const static BYTE CUT_IN_LINE	= 1;
	const static BYTE CUT_OUT_LINE	= 2;
	CUT_POINT *FromCutPtHiberId(HIBERID hiberId,BYTE cType=CUT_PT_CIR);
	bool IsInitedMCS();
	void InitMCS();
	BOOL GetMCS(GECS &mcs);
	SCOPE_STRU GetVertexsScope(GECS *pCS=NULL);
	void InitCutPt(bool bFarOrg=false);
	bool ClonePart(CProcessPart *pNewPart);
	int GetVertexCount(){return vertex_list.GetNodeNum();}
	bool IsRollEdge();
	void ProcessRollEdgeVertex();
	void CalRollLineAndHuoquLine(PROFILE_VER *pRollStart,PROFILE_VER *pRollEnd,PROFILE_VER *pPreVertex,PROFILE_VER *pNextVertex,
		f3dLine &huoqu_line,f3dLine &roll_line);
	void CalEquidistantShape(double minDistance,ATOM_LIST<PROFILE_VER> *pDestList);
	void ReadProcessBuffer(CBuffer& buffer,long version=0,bool bSaveSubSerial=false,int buf_type=0);
	void WriteProcessBuffer(CBuffer& buffer,long version=0,bool bSaveSubSerial=false,int buf_type=0);
	PROFILE_VER* FromVertexHiberId(HIBERARCHY id);
	bool DeleteVertexByHiberId(HIBERARCHY id);
	bool IsFoldPlate();
	bool IsPeakPointPlate();
	bool IsDisplayMK(){return mkpos.x!=1000000;}
	bool IsConcaveVertex(int iVertexIndex);		//�ж�ָ���������Ƿ�Ϊ����
	void RetrievedBoltFaceNo();	//���»�ȡ��˨�������
	int GetNormFaceI(f3dPoint face_norm);
	int GetBoltNumByD(int bolt_d);
	void GetMkRect(double fLen,double fWidth,ATOM_LIST<f3dPoint>& ptArr);
	f3dPoint GetDeformedVertex(f3dPoint vertice);
	double GetHuoQuAngle(int iFace,f3dLine *pHuoQuLine=NULL,BOOL bDisplayLog=TRUE);
	BOOL GetBendLineAt(int iHuoQuLine, f3dLine *pHuoQuLine);
	static const BYTE BEND_NONE = 0;	//������
	static const BYTE BEND_OUT = 1;		//����
	static const BYTE BEND_IN = 2;		//����
	BYTE GetHuoQuFaceBendType(int iFace);
	//��ʵ�ʾ�������-->չ���������
	//acs			����װ������ϵ
	//ls_norm		��˨����(��������ϵ��)
	//ls_pos_coord	��˨����(�����������,����������ϵ)
	virtual BOOL SpreadLsPos(GECS &acs,f3dPoint ls_norm,double* ls_pos_coord);
	//��չ���������-->ʵ���������(��ʵ�ʾ�������)
	virtual f3dPoint RestoreLsPos(const double* ls_pos,int iFace012=0,GECS *pAcs=NULL);
	virtual bool IsPlate(){return true;}
	virtual bool IsEqual(CGeneralPart *pPart,double epsilon=1.0,char *error_info=NULL,DWORD dwChkStyleFlag=NULL);
#ifdef __PROPERTYLIST_UI_
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//ͨ������Id��ȡ����ֵ
	DECLARE_PROP_FUNC(CProcessPlate)
#endif
#ifdef __TAP_CONTEXT_
	BOOL IsModifyedThick();					//�Ƿ�Ϊ�޸ĸְ�
	virtual bool IsReplacePart();					//�Ƿ�Ϊ���ϽǸ�
	virtual void FromTIDBuffer(CBuffer &buffer,long version=0);
	virtual void FromTAPBuffer(CBuffer &buffer,long version=0);
	virtual void ToTAPBuffer(CBuffer &buffer,bool bIncSolidData=true);
#endif
	static void TransPlateToMCS(CProcessPlate *pPlate,GECS &mcs);
};

class CProcessAngle :public CProcessPart 
{
public:
	static const BYTE FUNC_COMMON		= 0;	//��ͨ����
	static const BYTE FUNC_FOOTNAIL		= 1;	//�Ŷ����۸֡��Ǹֱ�ʾ�Ŷ��������ְ��ʾ�Ŷ���;�ĸְ�
	static const BYTE FUNC_REINFORCE	= 2;	//��(ǿ)�̣��Ǹֱ�ʾ�Ӿ��̽Ǹ֣��ְ��ʾ��ǿ�߰�
	static const BYTE FUNC_LEGANGLE		= 6;	//���ȽǸ�
	static const BYTE FUNC_BODYANGLE	= 9;	//�������ĽǸ�
	static const BYTE FUNC_HEADANGLE	= 10;	//ͷ�����ĽǸ�
#ifdef __TAP_CONTEXT_
public:
	virtual bool IsReplacePart();					//�Ƿ�Ϊ���ϽǸ�
	virtual void FromTIDBuffer(CBuffer &buffer,long version=0);
	virtual void FromTAPBuffer(CBuffer &buffer,long version=0);
	virtual void ToTAPBuffer(CBuffer &buffer,bool bIncSolidData=true);
#endif
private:
	//�Ǹֹ�����Ϣ
	// --------- 1.2. ��֫ -----------
	/*cut_wing_para[0]��ʾʼ�˱���֫����cut_wing_para[1]��ʾ�ն˱���֫����
	...[][0]��ʾ�Ǹ�������ȡ����
	...[][1]��ʾ��һ֫��ȡ���
	...[][2]��ʾ֫������ȡ����
	*/
	float _cut_wing_para[2][3];
	//��ʶ���нǸ�֫,0��ʾ����֫,1��ʾX֫,2��ʾY֫
	long _cut_wing[2];
    // --------- 1.3. �н� -----------
	/*
	cut_angle[0]��ʾ���X֫
	cut_angle[1]��ʾ���Y֫
	cut_angle[2]��ʾ�յ�X֫
	cut_angle[3]��ʾ�յ�Y֫
	...[][0]��ʾƽ�����߷�����ȡ��  ...[][1]��ʾ��ֱ���߷�����ȡ��
	*/
	float _cut_angle[4][2];
	typedef float* FloatPtr;
private:
	FloatPtr cut_angle_ptr[4];
	FloatPtr cut_wing_para_ptr[2];
	// ѹ��
	WORD _start_push_pos,_end_push_pos;//ѹ����ʼλ�� ѹ����ֹλ��
	int _wing_push_X1_Y2;			//1��ʾx֫ѹ�⣬2��ʾy֫ѹ��
	// ���Ͻ�
	int  _kaihe_base_wing_x0_y1;		//0��ʾX֫���Ͻǣ�1��ʾY֫���Ͻ�
	int _poly_part_num;
	//����������Ϣ
	bool _bCutRoot;		//���
	bool _bCutBer;		//����
	bool _bWeld;		//����
	bool _bNeedFillet;	//�¿�
public:
	ATOM_LIST<KAI_HE_JIAO> kaiHeJiaoList;
	ATOM_LIST<RIB_PLATE_INFO> rib_plate_list;//�Ǹ��߰�����
	//����Ǹ���Ϣ
	ATOM_LIST<POLY_ANGLE_INFO> poly_part_info;
public://��������
	BOOL m_bUserSpecStartCutAngle;		     //�ɲ�����Ա�ֶ�ά��ʼ���н���֫��Ϣ
	BOOL m_bUserSpecEndCutAngle;		     //�ɲ�����Ա�ֶ�ά���ն��н���֫��Ϣ
	PROPERTY(FloatPtr*,cut_wing_para); 
	GET(cut_wing_para){
#ifdef __DRAWING_CONTEXT_
		if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			for(int i=0;i<2;i++)
			{
				_cut_wing_para[i][0]=(float)((CLDSLineAngle*)pPart)->cut_wing_para[i][0];
				_cut_wing_para[i][1]=(float)((CLDSLineAngle*)pPart)->cut_wing_para[i][1];
				_cut_wing_para[i][2]=(float)((CLDSLineAngle*)pPart)->cut_wing_para[i][2];
				cut_wing_para_ptr[i]=&_cut_wing_para[i][0];
			}
		}
		else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
			CLDSLineAngle *pStartAngle=(CLDSLineAngle*)pPolyPart->segset.GetFirst();
			CLDSLineAngle *pEndAngle=(CLDSLineAngle*)pPolyPart->segset.GetTail();
			CLDSLineAngle *pJg;
			for (int i=0;i<2;i++)
			{
				pJg=(i<1)?pStartAngle:pEndAngle;
				_cut_wing_para[i][0]=(float)pJg->cut_wing_para[i][0];
				_cut_wing_para[i][1]=(float)pJg->cut_wing_para[i][1];
				_cut_wing_para[i][2]=(float)pJg->cut_wing_para[i][2];
				cut_wing_para_ptr[i]=&_cut_wing_para[i][0];
			}
		}
		else if(pPart==NULL)
		{
			for (int i=0;i<2;i++)
				cut_wing_para_ptr[i]=&_cut_wing_para[i][0];
		}	
#endif
		return cut_wing_para_ptr;
	}

	PROPERTY(long*, cut_wing); 
	GET(cut_wing){
#ifdef __DRAWING_CONTEXT_
		if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
			return (long*)((CLDSLineAngle*)pPart)->cut_wing;
		else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
			CLDSLineAngle *pStartAngle=(CLDSLineAngle*)pPolyPart->segset.GetFirst();
			CLDSLineAngle *pEndAngle=(CLDSLineAngle*)pPolyPart->segset.GetTail();
			CLDSLineAngle *pJg;
			for (int i=0;i<2;i++)
			{
				pJg=(i<1)?pStartAngle:pEndAngle;
				cut_wing[i]=pJg->cut_wing[i];
			}
		}
#endif
		return (long*)_cut_wing;
	}
	PROPERTY(FloatPtr*,cut_angle); 
	GET(cut_angle){
#ifdef __DRAWING_CONTEXT_
		if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			for(int i=0;i<4;i++)
			{
				_cut_angle[i][0]=(float)((CLDSLineAngle*)pPart)->cut_angle[i][0];
				_cut_angle[i][1]=(float)((CLDSLineAngle*)pPart)->cut_angle[i][1];
				cut_angle_ptr[i]=&_cut_angle[i][0];
			}
		}	
		else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
			CLDSLineAngle *pStartAngle=(CLDSLineAngle*)pPolyPart->segset.GetFirst();
			CLDSLineAngle *pEndAngle=(CLDSLineAngle*)pPolyPart->segset.GetTail();
			CLDSLineAngle *pJg;
			for (int i=0;i<4;i++)
			{
				pJg=(i<2)?pStartAngle:pEndAngle;
				_cut_angle[i][0]=(float)pJg->cut_angle[i][0];
				_cut_angle[i][1]=(float)((CLDSLineAngle*)pPart)->cut_angle[i][1];
				cut_angle_ptr[i]=&_cut_angle[i][0];
			}
		}
		else if(pPart==NULL)
		{
			for (int i=0;i<4;i++)
				cut_angle_ptr[i]=&_cut_angle[i][0];
		}	
#endif
		return cut_angle_ptr;
	}
	PROPERTY(WORD,start_push_pos); 
	GET(start_push_pos){
		_start_push_pos=GetStartPushPos();
		return _start_push_pos;
	}
	SET(start_push_pos){
		_start_push_pos=value;
	}
	PROPERTY(WORD,end_push_pos); 
	GET(end_push_pos){
		_end_push_pos=GetEndPushPos();
		return _end_push_pos;
	}
	SET(end_push_pos){
		_end_push_pos=value;
	}
	//wbt������ѹ�⣺1.Ĭ��ֻ��һ��ѹ�� 2.����Ǹֵ�һ��Ǹ���Ϊʼ��ѹ�⣬���һ��Ǹ���Ϊ�նˣ��н���֫���ƴ���
	PROPERTY(int,wing_push_X1_Y2);
	GET(wing_push_X1_Y2){
#ifdef __DRAWING_CONTEXT_
		if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
		{
			if(((CLDSLineAngle*)pPart)->start_push_flat_x1_y2>0)
				return ((CLDSLineAngle*)pPart)->start_push_flat_x1_y2;
			if(((CLDSLineAngle*)pPart)->mid_push_flat_x1_y2>0)
				return ((CLDSLineAngle*)pPart)->mid_push_flat_x1_y2;
			if(((CLDSLineAngle*)pPart)->end_push_flat_x1_y2>0)
				return ((CLDSLineAngle*)pPart)->end_push_flat_x1_y2;
			return 0;
		}
		else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
			CLDSLineAngle *pAngle[2]={(CLDSLineAngle*)pPolyPart->segset.GetFirst(),(CLDSLineAngle*)pPolyPart->segset.GetTail()};
			if(pAngle[0]->start_push_flat_x1_y2>0)
				return pAngle[0]->start_push_flat_x1_y2;
			if(pAngle[1]->end_push_flat_x1_y2>0)
				return pAngle[1]->end_push_flat_x1_y2;
			if(pAngle[1]->mid_push_flat_x1_y2>0)
				return pAngle[1]->mid_push_flat_x1_y2;
			return 0;
		}
#endif
		return _wing_push_X1_Y2;
	}
	SET(wing_push_X1_Y2){
		_wing_push_X1_Y2=value;
	}
	PROPERTY(int,kaihe_base_wing_x0_y1); 
	GET(kaihe_base_wing_x0_y1){
#ifdef __DRAWING_CONTEXT_
		if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
			return ((CLDSLineAngle*)pPart)->GetKaiHeWingX0Y1();
		else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
			CLDSLineAngle *pAngle=(CLDSLineAngle*)pPolyPart->segset.GetFirst();
			return pAngle->GetKaiHeWingX0Y1();
		}
#endif
		int nPush=kaiHeJiaoList.push_stack();
		KAI_HE_JIAO *pKaiHeJiao=kaiHeJiaoList.GetFirst();
		if(nPush)
			kaiHeJiaoList.pop_stack(nPush);
		if (pKaiHeJiao&&fabs(pKaiHeJiao->decWingAngle-90)>2)
			return _kaihe_base_wing_x0_y1;
		else
			return -1;
	}
	SET(kaihe_base_wing_x0_y1){
		_kaihe_base_wing_x0_y1=value;
	}
	PROPERTY(int,poly_part_num); 
	GET(poly_part_num){
#ifdef __DRAWING_CONTEXT_
		if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
			return pPolyPart->segset.GetNodeNum();
		}
#endif
		return _poly_part_num;
	}
	SET(poly_part_num){
		_poly_part_num=value;
	}
	PROPERTY(bool,m_bCutRoot); 
	GET(m_bCutRoot){
#ifdef __DRAWING_CONTEXT_
		if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
			return ((CLDSLineAngle*)pPart)->m_bCutRoot? true:false;
		else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
			CLDSLineAngle *pAngle=(CLDSLineAngle*)pPolyPart->segset.GetFirst();
			return pAngle->m_bCutRoot?true:false;
		}
#endif
		return _bCutRoot;
	}
	SET(m_bCutRoot){
		_bCutRoot=(bool)value;
	}
	PROPERTY(bool,m_bCutBer); 
	GET(m_bCutBer){
#ifdef __DRAWING_CONTEXT_
		if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
			return ((CLDSLineAngle*)pPart)->m_bCutBer?true:false;
		else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
			CLDSLineAngle *pAngle=(CLDSLineAngle*)pPolyPart->segset.GetFirst();
			return pAngle->m_bCutBer?true:false;
		}
#endif
		return _bCutBer;
	}
	SET(m_bCutBer){
		_bCutBer=value;
	}
	PROPERTY(bool,m_bWeld); 
	GET(m_bWeld){
#ifdef __DRAWING_CONTEXT_
		if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
			return ((CLDSLineAngle*)pPart)->m_bWeldPart ? true:false;
		else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
		{
			CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
			CLDSLineAngle *pAngle=(CLDSLineAngle*)pPolyPart->segset.GetFirst();
			return pAngle->m_bWeldPart? true:false;
		}
#endif
		return _bWeld;
	}
	SET(m_bWeld){
		_bWeld=value;
	}
	PROPERTY(bool,m_bNeedFillet); 
	GET(m_bNeedFillet){
		return _bNeedFillet;
	}
	SET(m_bNeedFillet){
		_bNeedFillet=value;
	}
	static const BYTE PUSH_FLAT_NONE	= 0;	//��ѹ��
	static const BYTE PUSH_FLAT_START	= 1;	//ʼ��ѹ��
	static const BYTE PUSH_FLAT_END		= 2;	//�ն�ѹ��
	static const BYTE PUSH_FLAT_MID		= 3;	//�м�ѹ��
	BYTE GetPushFlatType();
	WORD GetStartPushPos();
	WORD SetStartPushPos(WORD start_push_pos);
	WORD GetEndPushPos();
	WORD SetEndPushPos(WORD end_push_pos);
public:
#ifdef __DRAWING_CONTEXT_
	CProcessAngle(CLDSPart* pAnglePart=NULL);
#else
	CProcessAngle();
#endif
	f3dPoint GetWingNormX();
	f3dPoint GetWingNormY();
	f3dPoint GetWingVecX();
	f3dPoint GetWingVecY();
	void GetProcessStr(char *notes,size_t max_len,bool bByEnglish=false,bool bNumericKaiHeInfo=false);
	double GetDecWingAngle();
	bool IsStartCutAngleOrWing();
	bool IsEndCutAngleOrWing();
	void GetCutAngleAngWingStr(bool bStart,char* cut_angle_x,char* cut_angle_y,int max_len);	
	bool IsWingPush(){return start_push_pos<end_push_pos;}
	bool IsWingKaiHe(){return kaiHeJiaoList.GetNodeNum()>0;}
	bool IsUnequalAngle(){return (m_fHigh>=m_fThick&&fabs(m_fWidth-m_fHigh)>0);}
	double GetWidthWingY(){return IsUnequalAngle()?m_fHigh:m_fWidth;}
	bool ClonePart(CProcessPart *pNewPart);
	int GetXOrYWingLsNum(int X0_Y1=0);
	int GetVertexCount(){return 0;}
	void ReadProcessBuffer(CBuffer& buffer,long version=0,bool bSaveSubSerial=false,int buf_type=0);
	void WriteProcessBuffer(CBuffer& buffer,long version=0,bool bSaveSubSerial=false,int buf_type=0);
	virtual bool IsRod(){return true;}
	virtual bool IsAngle(){return true;}
	virtual bool IsEqual(CGeneralPart *pPart,double epsilon=1.0,char *error_info=NULL,DWORD dwChkStyleFlag=NULL);
	double GetLsG(GECS &acs,f3dPoint ls_norm,f3dPoint ls_pos,int* wingx0_y1=NULL);
	//acs			����װ������ϵ
	//ls_norm		��˨����(��������ϵ��)
	//ls_pos_coord	��˨����(�����������,����������ϵ)
	virtual BOOL SpreadLsPos(GECS &acs,f3dPoint ls_norm,double* ls_pos_coord);
	//��չ���������-->ʵ���������(��ʵ�ʾ�������)
	virtual f3dPoint RestoreLsPos(const double* ls_pos,int iFace012=0,GECS *pAcs=NULL);
#ifdef __PROPERTYLIST_UI_
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//ͨ������Id��ȡ����ֵ
	DECLARE_PROP_FUNC(CProcessAngle)
#endif
};

class CProcessTube : public CProcessPart
{
public:
	CProcessTube();
	virtual bool IsRod(){return true;}
	virtual bool IsTube(){return true;}
#ifdef __PROPERTYLIST_UI_
	int GetPropValueStr(long id, char *valueStr);	//ͨ������Id��ȡ����ֵ
	DECLARE_PROP_FUNC(CProcessTube)
#endif
};

class CProcessSlot : public CProcessPart
{
public:
	CProcessSlot();
	virtual bool IsRod(){return true;}
	virtual bool IsSlot(){return true;}
#ifdef __PROPERTYLIST_UI_
	int GetPropValueStr(long id, char *valueStr);	//ͨ������Id��ȡ����ֵ
	DECLARE_PROP_FUNC(CProcessSlot)
#endif
};

class CProcessFlat : public CProcessPart
{
public:
	CProcessFlat();
	virtual bool IsRod(){return true;}
	virtual bool IsFlat(){return true;}
#ifdef __PROPERTYLIST_UI_
	int GetPropValueStr(long id, char *valueStr);	//ͨ������Id��ȡ����ֵ
	DECLARE_PROP_FUNC(CProcessFlat)
#endif
};
///////////////////////////////////////////////////////////////////////////////////////////////
//CProcessBolt
///////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __TAP_CONTEXT_
class CStdBolt;
class CBoltAssembly
{
	DWORD dwAssemblePlace;
	KEY4C idBolt;
	CStdBolt *m_pBolt;
	GECS acs;
public:
	GEPOINT origin;
	GEPOINT work_norm;
	BYTE cFlag;				//������˨
	WORD L0;				//ͨ��
	BYTE cDianQuanN;		//��Ȧ��
	BYTE cDianQuanThick;	//��Ȧ���
	WORD wDianQuanOffset;	//��Ȧƫ��
	DWORD m_dwBlockIndex;	//0:��ʾ������ģ��   >1��ʾ���ڲ���ģ�͵�������
	SEGI iSeg;
	CXhChar50 statSegStr;
	CFGWORD cfgword;		//��ĺ�
	BYTE cLegQuad;			//�������޽��Ⱥ�
	CBoltAssembly();
	~CBoltAssembly();
	CBoltAssembly(const CBoltAssembly &srcParam);
	CBoltAssembly& operator=(const CBoltAssembly &srcParam);
	void SetKey(DWORD key){dwAssemblePlace=key;}
	DWORD GetKey(){return dwAssemblePlace;}
	void SetBelongBolt(CStdBolt *pBolt);
	CStdBolt *BelongBolt()const {return m_pBolt;}
	DWORD GetBoltId(){return idBolt;}
	void FromBuffer(CBuffer &buffer,long version=0);
	void ToBuffer(CBuffer &buffer) const;
	void CloneFrom(const CBoltAssembly &srcBoltAssembly);
	bool IsInited();
#ifdef _DEBUG
	int GetMemorySize();
#endif
public:
	GECS GetACS();
#ifdef __PROPERTYLIST_UI_
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//ͨ������Id��ȡ����ֵ
	DECLARE_PROP_FUNC(CBoltAssembly)
#endif
};
class CStdBolt : public CGeneralPart
{
public:
	BYTE m_cFamily;		//������˨���ϵ��
	WORD m_wIndex;		//������ϵ���е����
	float grade;		//��˨����
	int d;				//��˨ֱ��,���洢
	double L;			//��Ч��(mm),���洢
	int down_lim;		//ͨ������
	int up_lim;			//ͨ������
	CSolidBody solidOfCap;	//��ñʵ��
	CSolidBody solidOfBolt;	//��˨��ʵ��
	IBoltLibrary *m_pBelongBoltLib;
public:
	CStdBolt();
	~CStdBolt();
	void FromTIDBuffer(CBuffer &buffer,long version=0);
	void FromTAPBuffer(CBuffer &buffer,long version=0);
	void ToTAPBuffer(CBuffer &buffer,bool bIncSolidData=true);
	virtual CXhChar16 GetPartNo()const{return CXhChar16("%.1fM%dX%.f",grade,d,L);}
	virtual bool IsProcessPart(){return false;}
	virtual bool IsBolt(){return true;}
};
#endif
