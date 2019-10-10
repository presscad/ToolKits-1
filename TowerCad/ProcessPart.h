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
	float decWingAngle;		//两肢夹角
	short position;			//标定位置
	short startLength;		//始端开合长度
	short endLength;		//终端开合长度
	KAI_HE_JIAO() {memset(this,0,sizeof(KAI_HE_JIAO));}
};
#endif
struct PROFILE_VER{
	HIBERID hiberId;				//不存储,使用+2级占位存储钢板轮廓边
	DWORD keyId;					//存储，用来唯一标识hiberId
	BYTE type;						//1:普通直边 2:圆弧 3:椭圆弧
	bool m_bWeldEdge;				//是否焊缝边
	bool m_bRollEdge;				//此边需卷边
	short roll_edge_offset_dist;	//卷边偏移距离
	double radius;					//椭圆弧所需要的参数
	double sector_angle;			//标准圆弧所需要的参数
	short manu_space;				//焊缝加工预留间隙或卷边高度(mm)
	f3dPoint vertex;				//顶点的位置坐标
	f3dPoint center,column_norm;	//椭圆弧所需要的参数（永远输入绝对坐标),center如果表示圆弧圆心(拾取点)时，则输入相对坐标
	f3dPoint work_norm;				//圆弧法线方向
	double local_point_y;			//对活点位置，在该点两侧标注
	int local_point_vec;			//0.表示无对活点，1.表示对活点两侧标注50-->100的方向与焊接父杆件的start-->end方向相同，2.表示相反
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
	//特殊位标记
	static const DWORD TWO_CAP;		//双帽螺栓标识位
	static const DWORD ANTI_THEFT;	//防盗螺栓标识位
	static const DWORD ANTI_LOOSE;	//含防松垫圈标识位
	static const DWORD FOOT_NAIL;	//脚钉螺栓标识位
	static const DWORD REVERSE_NORM;//调转螺栓朝向标识位
	//螺栓功用类型
	static const BYTE FUNC_COMMON	= 0;//0.连接螺栓
	static const BYTE FUNC_FOOTNAIL	= 1;//1.脚钉，槽钢、角钢表示脚钉底座，钢板表示脚钉用途的钢板
	static const BYTE FUNC_WIREHOLE	= 2;//2.挂线孔;
	static const BYTE FUNC_EARTHHOLE= 3;//3.接地孔;
	static const BYTE FUNC_SETUPHOLE= 4;//4.装配孔;
	static const BYTE FUNC_BRANDHOLE= 5;//5.挂牌孔;
	static const BYTE FUNC_WATERHOLE= 6;//6.引流孔
	static const BYTE FUNC_FOOTNAIL2= 7;//7.代孔脚钉

	HIBERID hiberId;			//不存储,使用+1级占位存储螺栓id
	DWORD keyId;				//存储，用来唯一标识hiberId
	BYTE cFaceNo;				//螺栓孔所在面序号(钢板螺栓需要) 临时使用不存储
	
	PROPERTY(double, bolt_d);
	GET(bolt_d)
	{
		double fBoltD = _bolt_d;
		if (_bolt_d > 1000)	//小数*100,以整数形式存储 wht 19-09-12
			fBoltD = _bolt_d * 0.01;
		return fBoltD;
	}
	SET(bolt_d) {
		double fBoltD = value;
		int nBoltD = (int)fBoltD;
		if (fabs(fBoltD - nBoltD) > EPS2)
			_bolt_d = (int)(fBoltD * 100);	//小数*100,以整数形式存储 wht 19-09-12
		else
			_bolt_d = (int)value;
	}
	float hole_d_increment;		//螺栓孔直径增量
	short waistLen;				// 腰圆孔腰长
	GEPOINT waistVec;			// 腰圆孔方向
	float posX;					//
	float posY;					//
	DWORD dwRayNo;				//螺栓孔所在射线号
	BITFLAG m_dwFlag;			// 有特殊要求的标识位，如ANTITHEFT_BOLT(1),ANTILOOSE_BOLT(2),FOOTNAIL_BOLT(4)
	long feature;
	BYTE cFuncType;				//螺栓孔类型
	BYTE cFlag;					//螺栓孔加工工艺类型  0.冲孔  1.钻孔
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
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//通过属性Id获取属性值
#endif
};
struct RIB_PLATE_INFO
{
	HIBERID hiberId;		 //不存储，使用-1级占位存储角钢肋板
	float fThick;            //肋板厚度
	float fPosZ;			 //肋板位置
	CXhChar16 sPlateNo;		 //肋板编号
	char m_cMaterial;		 //材质
	RIB_PLATE_INFO()
	{
		fThick=fPosZ=0;
		m_cMaterial='S';
	}
};
struct POLY_ANGLE_INFO
{
	int nIndex;				//构件索引
	GEPOINT norm_x_wing;	//X肢展开方向
	GEPOINT norm_y_wing;	//Y肢展开方向
	GEPOINT len_vec;		//延伸方向
};
//////////////////////////////////////////////////////////////////////////
//CGeneralPart
//////////////////////////////////////////////////////////////////////////
class CGeneralPart
{
public:
	static const BYTE TYPE_LINEANGLE	= 1;	//角钢
	static const BYTE TYPE_BOLT			= 2;	//螺栓
	static const BYTE TYPE_PLATE		= 3;	//钢板
	static const BYTE TYPE_LINETUBE		= 4;	//钢管
	static const BYTE TYPE_LINEFLAT		= 5;	//扁铁
	static const BYTE TYPE_LINESLOT		= 6;	//槽钢
	static const BYTE TYPE_SPHERE		= 7;	//球壳
#ifdef __DRAWING_CONTEXT_
	CLDSPart *pPart;
#elif defined(__TAP_CONTEXT_)
	CGeneralPart* pNextPart;		//处理非正常情况下同一编号构件并不相同且同时输出情况 wjh-2016.1.26
	bool AttachSameLabelPart(CGeneralPart* pSameLabelPart);
#endif
protected:
	KEY4C m_dwPartKey;
	BYTE _ciModifiedFlag;			//数据修改标识及Undo标识, 仅用于Undo操作实现过程中记录数据是否发生了修改不存储于文件 wjh-2017.7.12
	BYTE _cPartType;				//构件类型
	SEGI m_Seg;						//段号
	CXhChar100 m_sSpec;				//规格
	float _fWeight;					//单重
	float _fSumWeight;				//总重，不存储 wht 19-08-22
	CXhChar50 m_sNote;				//备注
	KEY4C m_dwOrgPartKey;			//当前构件对应的原始构件	wht 15-05-23
public:
	CXhChar100 m_sRelatePartNo;		//关联的件号（处理一板多号的情况）	wxc-2016.12.13
	BYTE m_cFuncType;				//
	long feature;					//不存储
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
	static const DWORD POS_FRONT_PLUS		= 1;	//4占位:前 8占位:前X+(撇)
	static const DWORD POS_BACK_PLUS		= 2;	//4占位:后 8占位:后X+(撇)
	static const DWORD POS_LEFT_PLUS		= 3;	//4占位:左 8占位:左Y+(撇)
	static const DWORD POS_RIGHT_PLUS		= 4;	//4占位:右 8占位:右Y+(撇)
	static const DWORD POS_FRONT_MINUS		= 5;	//4占位:无 8占位:前X-(捺)
	static const DWORD POS_BACK_MINUS		= 6;	//4占位:无 8占位:后X-(捺)
	static const DWORD POS_LEFT_MINUS		= 7;	//4占位:无 8占位:左Y-(捺)
	static const DWORD POS_RIGHT_MINUS		= 8;	//4占位:无 8占位:右Y-(捺)
	
public:
	DWORD m_dwInitPlace;			//不存储，与TID文件中装配记录的顺序号一致 wht 16-12-05
	DWORD m_dwAssemblePlace;		//1~8为特定装配位置，其余不规则装配位置索引应从9开始
	DWORD m_dwBlockIndex;			//0:表示属于塔模型   >1表示属于部件模型的索引号
	GECS acs;
	bool m_bIsRod;					//以后择机应去掉　wjh-2017.7.13
	GEPOINT startPos,endPos;		//以后择机应去掉　wjh-2017.7.13
	CFGWORD cfgword;				//配材号
	BYTE cLegQuad;					//归属象限接腿号
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

	static const UINT PATTERN_NONE		= 0;			//完全根据三维数据模型
	static const UINT PATTERN_INHERIT	= 0x0040;		//常规继承,根据三维数据模型继承更新
	static const UINT PATTERN_OVERWRITE	= 0xffffffff;	//覆盖,完全由PPE工具编辑存储
	//构件属性项标识位，m_dwFlag对应的属性项
	static const UINT PATTERN_MATERIAL	= 0x0001;	//材质
	static const UINT PATTERN_SIZE		= 0x0002;	//规格
	static const UINT PATTERN_LENGTH	= 0x0004;	//长度
	static const UINT PATTERN_COUNT		= 0x0008;	//单基数
	static const UINT PATTERN_PROP_CONNBOLTS= 0x0010;	//连接螺栓
	static const UINT PATTERN_PROP_ONLYHOLES= 0x0020;	//虚拟孔
	static const UINT PATTERN_MKPOS		= 0x0040;		//字模(钢印)位置
	BUFFER_VERSION m_verSerial;	//构件的版本流水号
	DWORD m_dwInheritPropFlag;	//与原始三维数据模型间的继承关系
	long  m_idSrcObject;		//原始三维数据模型中对应的源对象Id(如T/LMA构件句柄)
#ifdef __TAP_CONTEXT_
public:
	CSolidBody solid;				//实体数据
	CHashListEx<ASSEMBLEY_POS> acsPosList;	//构件的装配坐标系集合
	static const BYTE PLACE_MODE_NONE	= 0;//自定义
	static const BYTE PLACE_MODE_4_1	= 1;//4占位模式1
	static const BYTE PLACE_MODE_4_2	= 2;//4占位模式2
	static const BYTE PLACE_MODE_8_1	= 3;//8占位模式1
	static const BYTE PLACE_MODE_8_2	= 4;//8占位模式2
	BYTE m_cPlaceMode;				//构件占位模式
	BYTE m_cPlaceStateFromFile;		//构件占位状态(从文件初始化的)
	BYTE m_cPriorPlaceState;		//构件优先占位状态
	char m_cOrgMaterial;					//原材质
	char m_cOrgQuality;						//原质量等级 wht 19-09-15
	float m_fOrgWidth,m_fOrgThick;			//原肢宽度、厚度
	WORD m_wOrgLength;						//原始长度
	BOOL IsIdleClassicPlace(BYTE cPlaceId);
	BOOL IsExistPriorClassicPlace(BYTE cPlaceId);
public:
	static PLACE_CONTENT_FUNC GetPlaceContentFunc;
	virtual bool IsReplacePart();	//是否为代料构件
	virtual void FromTIDBuffer(CBuffer &buffer,long version=0);
	virtual void FromTAPBuffer(CBuffer &buffer,long version=0);
	virtual void ToTAPBuffer(CBuffer &buffer,bool bIncSolidData=true);
#endif
protected:
	//构件基本信息
	char _cMaterial;				//材质
	char _cQuality;					//质量等级 wht 19-09-15
	float _fWidth;					//宽度
	WORD _wLength;					//长度
	float _fThick;					//厚度
	float _fHigh;					//高度
	CXhChar16 m_sPartNo;			//构件号
public:
	CBuffer m_xOrgProcessBuffer;	//原始工艺信息buffer,用来比对工艺信息是否发生变化 wht 15-05-23
	CHashListEx<BOLT_INFO> m_xBoltInfoList;	//螺栓信息
public://公用属性
	//字号位置,对于角钢：X表示轴向距离，Y表示心距（可用正负值区别两肢）
	GEPOINT mkpos,mkVec;
	int m_nDanJiNum;				//单基数（暂不存储）
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
	float GetWidth();	//单位为mm
	float GetThick();	//单位为mm
	float GetHeight();	//单位为mm
	WORD GetLength();	//单位为mm
	float GetWeight(BOOL bSolidLength=TRUE);	//单位为kg
	virtual bool IsProcessPart(){return true;}
	virtual bool IsBolt(){return false;}
public:
	CProcessPart();
	~CProcessPart();
	static void QuerySteelMatMark(char cMat,char* sMatMark);
	//根据m_dwInheritPropFlag整合ppibufFromPart*来自CLDSPart::ToPPIBuffer) wjh-2017.1.18
	virtual bool MergeToSelfWithModel(CBuffer& bufFromModelPart);
	virtual bool MergeToSelfWithPPE(CBuffer& bufFromPpePart);
	virtual bool MergeToSelfWithPPE(CProcessPart* pPpePart);
	virtual bool ClonePart(CProcessPart *pNewPart);	//克隆PPI构件信息
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
	//螺栓操作
	virtual BOLT_INFO* FromHoleId(DWORD holeId);
	virtual bool DeleteBoltHoleById(DWORD id);
	virtual BOLT_INFO* FromHoleHiberId(HIBERARCHY id);
	virtual bool DeleteBoltHoleByHiberId(HIBERARCHY id);
	virtual int GetLsString(char *ls_str);
	virtual int GetLsCount(short d=0);	//因跟以往CLDSPart::GetLsCount命名一致，不宜再命名为GetBoltCount
	int GetDrillHoleCount();
	//acs			构件装配坐标系
	//ls_norm		螺栓法线(绝对坐标系下)
	//ls_pos_coord	螺栓坐标(输入绝对坐标,输出相对坐标系)
	virtual BOOL SpreadLsPos(GECS &acs,f3dPoint ls_norm,double* ls_pos_coord){return FALSE;}
	//由展开相对坐标-->实际相对坐标(或实际绝对坐标)
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
	virtual int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100){return 0;}	//通过属性Id获取属性值
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
	DWORD hEntId;		//轮廓点、孔
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
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//通过属性Id获取属性值
#endif
};

class CProcessPlate : public CProcessPart
{
public:
	static const BYTE FUNC_COMMON		= 0;	//普通构件
	static const BYTE FUNC_FOOTNAIL		= 1;	//脚钉，槽钢、角钢表示脚钉底座，钢板表示脚钉用途的钢板
	static const BYTE FUNC_REINFORCE	= 2;	//加(强)固，角钢表示加劲短角钢，钢板表示加强肋板
	static const BYTE FUNC_WIREPLATE	= 3;	//挂线板
	static const BYTE FUNC_STEPPLATE	= 4;	//爬梯上的踏板
	static const BYTE FUNC_SETUPPLATE	= 5;	//辅助安装挂板
	static const BYTE FUNC_FOOTPLATE	= 7;	//塔脚板
	static const BYTE FUNC_GASKET		= 8;	//垫板
public:
	//钢板基本信息
	BYTE m_cFaceN;					//面数(不含卷边)
	f3dLine HuoQuLine[2];			//火曲线（板相对坐标）
	f3dPoint HuoQuFaceNorm[2];		//节点板各面的法线方向
	double m_fInnerRadius;			//内圆弧半径
	f3dPoint top_point;				//三面板两火曲线的交点（板相对坐标）
	f3dPoint m_tInnerOrigin;			//内圆圆心位置---相对坐标	
	f3dPoint m_tInnerColumnNorm;		//内圆法线方向
	double m_fHuoQuLineR[2];			//火曲线的火曲半径
	double m_fHuoQuLineX[2];			//火曲线的中性层系数
	bool m_bIncDeformed;			//是否考虑火曲 0.未考虑 1.已考虑
	BYTE m_ciDeformType;			//火曲变形处理方式 0.装配面 1.中性层 2.凸面 3.凹面
	BYTE m_ciRollProcessType;		//卷边处理方式 
	BYTE m_ciRollOffsetType;		//卷边偏移方式 0.外侧 1.内侧
	//轮廓边信息
	CHashListEx<PROFILE_VER> vertex_list;
	/*
	mcs的影响因素
	cBottomEdge底部边索引，m_wBottomEdge==-1表示未初始化NC绘制模式参数
	cOvertrun 是否进行了反转					
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
	bool IsConcaveVertex(int iVertexIndex);		//判断指定轮廓点是否为凹点
	void RetrievedBoltFaceNo();	//重新获取螺栓所在面号
	int GetNormFaceI(f3dPoint face_norm);
	int GetBoltNumByD(int bolt_d);
	void GetMkRect(double fLen,double fWidth,ATOM_LIST<f3dPoint>& ptArr);
	f3dPoint GetDeformedVertex(f3dPoint vertice);
	double GetHuoQuAngle(int iFace,f3dLine *pHuoQuLine=NULL,BOOL bDisplayLog=TRUE);
	BOOL GetBendLineAt(int iHuoQuLine, f3dLine *pHuoQuLine);
	static const BYTE BEND_NONE = 0;	//无制弯
	static const BYTE BEND_OUT = 1;		//外曲
	static const BYTE BEND_IN = 2;		//内曲
	BYTE GetHuoQuFaceBendType(int iFace);
	//由实际绝对坐标-->展开相对坐标
	//acs			构件装配坐标系
	//ls_norm		螺栓法线(绝对坐标系下)
	//ls_pos_coord	螺栓坐标(输入绝对坐标,输出相对坐标系)
	virtual BOOL SpreadLsPos(GECS &acs,f3dPoint ls_norm,double* ls_pos_coord);
	//由展开相对坐标-->实际相对坐标(或实际绝对坐标)
	virtual f3dPoint RestoreLsPos(const double* ls_pos,int iFace012=0,GECS *pAcs=NULL);
	virtual bool IsPlate(){return true;}
	virtual bool IsEqual(CGeneralPart *pPart,double epsilon=1.0,char *error_info=NULL,DWORD dwChkStyleFlag=NULL);
#ifdef __PROPERTYLIST_UI_
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//通过属性Id获取属性值
	DECLARE_PROP_FUNC(CProcessPlate)
#endif
#ifdef __TAP_CONTEXT_
	BOOL IsModifyedThick();					//是否为修改钢板
	virtual bool IsReplacePart();					//是否为代料角钢
	virtual void FromTIDBuffer(CBuffer &buffer,long version=0);
	virtual void FromTAPBuffer(CBuffer &buffer,long version=0);
	virtual void ToTAPBuffer(CBuffer &buffer,bool bIncSolidData=true);
#endif
	static void TransPlateToMCS(CProcessPlate *pPlate,GECS &mcs);
};

class CProcessAngle :public CProcessPart 
{
public:
	static const BYTE FUNC_COMMON		= 0;	//普通构件
	static const BYTE FUNC_FOOTNAIL		= 1;	//脚钉，槽钢、角钢表示脚钉底座，钢板表示脚钉用途的钢板
	static const BYTE FUNC_REINFORCE	= 2;	//加(强)固，角钢表示加劲短角钢，钢板表示加强肋板
	static const BYTE FUNC_LEGANGLE		= 6;	//塔腿角钢
	static const BYTE FUNC_BODYANGLE	= 9;	//塔身主材角钢
	static const BYTE FUNC_HEADANGLE	= 10;	//头部主材角钢
#ifdef __TAP_CONTEXT_
public:
	virtual bool IsReplacePart();					//是否为代料角钢
	virtual void FromTIDBuffer(CBuffer &buffer,long version=0);
	virtual void FromTAPBuffer(CBuffer &buffer,long version=0);
	virtual void ToTAPBuffer(CBuffer &buffer,bool bIncSolidData=true);
#endif
private:
	//角钢工艺信息
	// --------- 1.2. 切肢 -----------
	/*cut_wing_para[0]表示始端背切肢参数cut_wing_para[1]表示终端背切肢参数
	...[][0]表示角钢楞线切取长度
	...[][1]表示另一肢切取宽度
	...[][2]表示肢棱线切取长度
	*/
	float _cut_wing_para[2][3];
	//标识所切角钢肢,0表示无切肢,1表示X肢,2表示Y肢
	long _cut_wing[2];
    // --------- 1.3. 切角 -----------
	/*
	cut_angle[0]表示起点X肢
	cut_angle[1]表示起点Y肢
	cut_angle[2]表示终点X肢
	cut_angle[3]表示终点Y肢
	...[][0]表示平行楞线方向切取量  ...[][1]表示垂直楞线方向切取量
	*/
	float _cut_angle[4][2];
	typedef float* FloatPtr;
private:
	FloatPtr cut_angle_ptr[4];
	FloatPtr cut_wing_para_ptr[2];
	// 压扁
	WORD _start_push_pos,_end_push_pos;//压扁起始位置 压扁终止位置
	int _wing_push_X1_Y2;			//1表示x肢压扁，2表示y肢压扁
	// 开合角
	int  _kaihe_base_wing_x0_y1;		//0表示X肢开合角，1表示Y肢开合角
	int _poly_part_num;
	//其他工艺信息
	bool _bCutRoot;		//清根
	bool _bCutBer;		//铲背
	bool _bWeld;		//焊接
	bool _bNeedFillet;	//坡口
public:
	ATOM_LIST<KAI_HE_JIAO> kaiHeJiaoList;
	ATOM_LIST<RIB_PLATE_INFO> rib_plate_list;//角钢肋板链表
	//制弯角钢信息
	ATOM_LIST<POLY_ANGLE_INFO> poly_part_info;
public://公用属性
	BOOL m_bUserSpecStartCutAngle;		     //由操作人员手动维护始端切角切肢信息
	BOOL m_bUserSpecEndCutAngle;		     //由操作人员手动维护终端切角切肢信息
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
	//wbt：对于压扁：1.默认只有一端压扁 2.制弯角钢第一块角钢作为始端压扁，最后一块角钢作为终端（切角切肢类似处理）
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
	static const BYTE PUSH_FLAT_NONE	= 0;	//无压扁
	static const BYTE PUSH_FLAT_START	= 1;	//始端压扁
	static const BYTE PUSH_FLAT_END		= 2;	//终端压扁
	static const BYTE PUSH_FLAT_MID		= 3;	//中间压扁
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
	//acs			构件装配坐标系
	//ls_norm		螺栓法线(绝对坐标系下)
	//ls_pos_coord	螺栓坐标(输入绝对坐标,输出相对坐标系)
	virtual BOOL SpreadLsPos(GECS &acs,f3dPoint ls_norm,double* ls_pos_coord);
	//由展开相对坐标-->实际相对坐标(或实际绝对坐标)
	virtual f3dPoint RestoreLsPos(const double* ls_pos,int iFace012=0,GECS *pAcs=NULL);
#ifdef __PROPERTYLIST_UI_
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//通过属性Id获取属性值
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
	int GetPropValueStr(long id, char *valueStr);	//通过属性Id获取属性值
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
	int GetPropValueStr(long id, char *valueStr);	//通过属性Id获取属性值
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
	int GetPropValueStr(long id, char *valueStr);	//通过属性Id获取属性值
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
	BYTE cFlag;				//防盗螺栓
	WORD L0;				//通厚
	BYTE cDianQuanN;		//垫圈数
	BYTE cDianQuanThick;	//垫圈厚度
	WORD wDianQuanOffset;	//垫圈偏移
	DWORD m_dwBlockIndex;	//0:表示属于塔模型   >1表示属于部件模型的索引号
	SEGI iSeg;
	CXhChar50 statSegStr;
	CFGWORD cfgword;		//配材号
	BYTE cLegQuad;			//归属象限接腿号
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
	int GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen=100);	//通过属性Id获取属性值
	DECLARE_PROP_FUNC(CBoltAssembly)
#endif
};
class CStdBolt : public CGeneralPart
{
public:
	BYTE m_cFamily;		//所属螺栓规格系列
	WORD m_wIndex;		//在所属系列中的序号
	float grade;		//螺栓级别
	int d;				//螺栓直径,不存储
	double L;			//有效长(mm),不存储
	int down_lim;		//通厚下限
	int up_lim;			//通厚上限
	CSolidBody solidOfCap;	//螺帽实体
	CSolidBody solidOfBolt;	//螺栓柱实体
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
