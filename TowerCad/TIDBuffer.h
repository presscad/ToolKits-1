#pragma once
#include "f_ent.h"
#include "XhCharString.h"
#include "SolidBody.h"
#ifdef TIDCORE_EXPORTS
#include "TidToolFunc.h"
#else
#include "PartLib.h"
#include "XhLdsLm.h"
#endif
#include "HashTable.h"

class CTIDBuffer;
class CDataSection
{
protected:
	CTIDBuffer* m_pTidBuff;
	DWORD m_dwZeroAddrOffset;	//����������ʼ����������ڴ��еĵ�ַƫ��λ��
	DWORD m_dwBufSize;
	char* m_data;
	CXhChar16 m_sVersion;
public:
	CDataSection(){m_dwBufSize=0;m_data=NULL;m_dwZeroAddrOffset=0;m_pTidBuff=NULL;}
	void SetBelongTidBuffer(CTIDBuffer* pTidBuffer){m_pTidBuff=pTidBuffer;}
	CTIDBuffer* BelongTidBuffer(){return m_pTidBuff;}
	char* BufferPtr(){return m_data;}
	DWORD BufferLength(){return m_dwBufSize;}
	DWORD ZeroAddrOffset(){return m_dwZeroAddrOffset;}
	//�������ӷ����ڵ���Ե�ַת�����ļ��洢�ռ��ȫ�ֵ�ַ
	DWORD AddrLtoG(DWORD addr){return addr+m_dwZeroAddrOffset;}
	//�����ļ��洢�ռ��ȫ�ֵ�ַת�������ӷ����ڵ���Ե�ַ
	DWORD AddrGtoL(DWORD addr){return addr-m_dwZeroAddrOffset;}
};
//����
struct TOWER_MODULE
{
	BYTE m_cBodyNo;		//������(��&ͷ��)����Ӧ����ģ����ȣ���
	BYTE m_cbLegInfo;	//������Ϣ
	UINT m_uiNamedHeight;	//������Ƹߣ�����Ⱥ��ߣ���mm
	BYTE m_arrActiveQuadLegNo[4];	//��ģ�͵�ǰ��һ�������޵ĵ�ǰ���Ⱥ�
	CFGWORD m_dwLegCfgWord;	//������ĺ�
	CXhChar50 name;
	TOWER_MODULE(){m_cBodyNo=m_cbLegInfo=0;m_uiNamedHeight=0;}
};
struct ANCHOR_LOCATION{short siPosX,siPosY;};
class  CSubLegFoundationSection : public CDataSection{
protected:
	BYTE cnHeightCount;
public:
	BYTE ciBasePlateThick;
	BYTE cnAnchorBoltCount;
	BYTE cnSubLegCount;
public:
	CSubLegFoundationSection(BYTE _cnHeightCount=0,DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL);
	ANCHOR_LOCATION GetAnchorLocationAt(short index);	//0 based index;
	GEPOINT GetSubLegFoundationOrgBySerial(BYTE ciBodySerial,BYTE ciLegSerial);	//1 based serial
};
class CModuleSection : public CDataSection
{
public:
	double m_fNamedHeightZeroZ;			//ָ���������߼���ʱ����0��׼ƽ���Z����ֵ(V1.4��������)
	CModuleSection(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL);
	BYTE GetModuleCount();
	TOWER_MODULE GetModuleAt(int i);
};

struct WIRE_NODE{
	WORD wiCode;	//WIREPLACE_CODE
	GEPOINT position;
	GEPOINT relaHolePt[2];
	char name[38];	//�ҵ�����
	//
	WIRE_NODE() { wiCode = 0; strcpy(name, ""); }
};
class CWireNodeSection : public CDataSection
{
public:
	WORD m_wnWireNodeCount;	//�ҵ�����
	CWireNodeSection(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL);
	WIRE_NODE GetWireNodeAt(int i);
};
class CProjInfoSubSection : public CDataSection
{
public:
	UINT m_uidSubSection;	//�ӷ������ͱ�ʶ�֣�������ƽ�GIM��׼Ϊ23545686
	CProjInfoSubSection(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL);
};
class CProjectInfoSection : public CDataSection
{
public:
	BYTE m_ciSubProjSectionCount;	//���������Ϣ�ӷ�������
	CProjectInfoSection(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL);
	CProjInfoSubSection GetSubSectionAt(int i);	//0 based index
};
//����
struct TOWER_BLOCK
{
	GECS lcs;			//������ģ�Ϳռ��ڶ����װ���ö�λ����ϵ
	CXhChar50 name;			//����������
	CSolidBody solid;		//ʵ�����ݣ�ע���ֹ���������������������Զ��������⣬�纯����������ֵʱ��
};
//����
class CBlockSection : public CDataSection
{
public:
	CBlockSection(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL);
	WORD GetBlockCount(){return *((WORD*)m_data);}
	TOWER_BLOCK GetBlockAt(int i);
	void GetBlockBufferAt(int index,CBuffer& blockBuf);
};
struct BOLTINFO
{
	WORD d;			//��˨ֱ��
	WORD L;			//��Ч��(mm)
	WORD Lnt;		//�޿۳�
	WORD down_lim;	//��˨ͨ������
	WORD up_lim;	//��˨ͨ������
	double weight;	//kg
	char spec[18];	//�̶����ȵ���˨��������ַ�����ĩβ0��ֹ�ַ���
public:	//����ʵ����ʾ���ݣ�ע���ֹ���������������������Զ��������⣬�纯����������ֵʱ��
	CSolidBody solidOfCap;	//��ñ
	CSolidBody solidOfBolt;	//��˨��
};
/// <summary>
/// �������漰����˨���ϵ��
/// </summary>
class CBoltSizeSection : public CDataSection
{
public:
	CBoltSizeSection(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL);
	CXhChar16 sizeDescript;//��˨��������ַ�����ĩβ0��ֹ�ַ�����
	struct BOLTSOLID{CSolidBody bolt,nut;}solid;	//��˨����ñʵ��	
public:
	void InitBuffer(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL);
	virtual int GetId(){return 0;}
	virtual short GetDiameter();		//��˨ֱ��
	virtual short GetLenValid();		//��˨��Ч��
	virtual short GetLenNoneThread();	//��˨�޿۳�
	virtual short GetMaxL0Limit();		//��˨ͨ������
	virtual short GetMinL0Limit();		//��˨ͨ������
	virtual double GetTheoryWeight();	//��������(kg)
	virtual short GetSpec(char* spec);	//��˨��������ַ�����ĩβ0��ֹ�ַ���
};
class CBoltSeries : public CDataSection
{
public:
	CBoltSeries(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	BYTE BoltDCount(){return *((BYTE*)m_data);}
	BYTE BoltWasherCount(){return *((BYTE*)(m_data+1));}
	BYTE BoltCapCount(){return *((BYTE*)(m_data+2));}
	WORD BoltSizeCount(){return *((WORD*)(m_data+3));}
	CXhChar24 GetSeriesName();
	bool GetBoltSizeAt(int i,CBoltSizeSection& bolt);
	bool GetBoltSizeAt(int i,BOLTINFO& bolt);
#ifndef TIDCORE_EXPORTS
	bool GetBoltCapSolidBody(int indexId, CSolidBody& body);
	bool GetBoltSolidBody(int indexId, CSolidBody& body);
#endif
};
//�ؽ���˨��Ϣ
struct ANCHORBOLT_INFO{
	WORD d;					//�ؽ���˨����ֱ��
	WORD wiLe;				//�ؽ���˨��¶��(mm)
	WORD wiLa;				//�ؽ���˨�޿۳�(mm) <��������
	WORD wiWidth,wiThick;	//�ؽ���˨���������
	WORD wiHoleD;			//�ؽ���˨��׾�
	WORD wiBasePlateHoleD;	//�ؽ���˨���ӵ����彨��׾�ֵ
	char szSizeSymbol[64];	//�������ֽڣ��̶����ȵĵؽ���˨������ַ�������ĩβ0��ֹ�ַ�����
};
class CBoltSection : public CDataSection
{
public:
	CBoltSection(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL);
	WORD BoltSeriesCount(){return *((BYTE*)m_data);}
	CBoltSeries GetBoltSeriesAt(int i);
#ifndef TIDCORE_EXPORTS
	void InitLsFamily(int i,CLsFamily* pFamily);
#endif
	ANCHORBOLT_INFO anchorInfo;
	struct ANCHORSOLID{CSolidBody bolt,nut;}xAnchorSolid;	//��˨����ñʵ��
};
class CMaterialLibrarySection : public CDataSection
{
	BYTE m_byteMatTypeCount;
public:
	CMaterialLibrarySection(BYTE mat_n=0,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_byteMatTypeCount=mat_n,m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	BYTE TypeCount(){return m_byteMatTypeCount;}
	char BriefSymbolAt(int i);
	int  NameCodeAt(int i,char* nameCode);	//�����ַ�������,��������NULL
};
//����������Ϣ
struct PART_INFO{
	WORD wModelSpace;
	BYTE cPartType;	//�������࣬1:�Ǹ�2:��˨3:�ְ�4:�ֹ�5:����6:�۸�
	WORD wStateFlag;//����״̬���ԣ���λΪ1��ʾ����ͬʱ����Ƕ��ʽ�����͸���ģ�Ϳռ䣬��0x8000
	BYTE cMaterial;	//���ʼ��ַ�
	DWORD dwSeg;	//�κţ���16λΪ�ַ�������λΪ���ֲ���
	float fWidth;	//���(mm)
	float fThick;	//���(mm)
	float fHeight;	//�߶�(mm)
	WORD  wLength;	//����(mm)
	float fWeight;	//����(kg)
	BYTE cFuncType;	//��������
	DWORD addrAttachInfo;	//������Ϣ�ļ��ڴ洢��ַ
public:
	DWORD dwProcessBuffSize;
	char* processBuffBytes;
public:	//������������
	CXhChar50 sPartNo;	//�������
	CXhChar50 spec;		//���
	CXhChar50 sNotes;	//��ע
	CSolidBody solid;	//ʵ����ʾ���ݣ�ע���ֹ���������������������Զ��������⣬�纯����������ֵʱ��
};
class CPartSection : public CDataSection
{
public:
	CPartSection(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	CMaterialLibrarySection GetMatLibrarySection();
	UINT GetPartCount();
	PART_INFO GetPartInfoByIndexId(int indexId);
	void GetPartBufferAt(int indexId,CBuffer& buffer);
};
struct BLOCK_ASSEMBLY
{
	WORD wIndexId;		//����������ʶ��
	GECS acs;		//�ڸ���ģ�Ϳռ��ڵ�װ������ϵ����
	DWORD dwSeg;		//�κ�
	CFGWORD cfgword;	//��ĺ�
	BYTE cLegQuad;		//�������ȵ����޺ţ���Ϊ����ʾ��������
	BYTE reserved;		//1���ֽڶ��뱣��λ
};
struct BOLT_ASSEMBLY
{
	bool bInBlockSpace;	//�Ƿ�����װ�䲿���ռ�۹������ԣ�
	BYTE cSeriesId;	//��˨���ϵ�������ţ۹������ԣ�
	WORD wIndexId;	//��˨�����ϵ���ڵ�������ʶ�ţ۹������ԣ�
	WORD  wBlockIndexId;//���������ı�ʶ�����ţ۲���װ����˨���ԣ�
	float grade;		//��˨�ȼ�
	GEPOINT origin;		//��˨װ��ԭ��۹������ԣ�
	GEPOINT work_norm;	//��˨�������ߣ۹������ԣ�
	WORD wL0;			//��˨ͨ��(mm)�۹������ԣ�
	BYTE cPropFlag;		//����Ҫ���ʶ�ֽڣ�ĿǰĩλΪ����ʾ������˨������λ�������۹������ԣ�
	BYTE cDianQuanN,cDianQuanThick;	//��Ȧ����,��Ȧ��ȣ۹������ԣ�
	WORD wDianQuanOffset;	//��Ȧλ��ƫ����������˨���������ع������ߵ�ƫ�������۹������ԣ�
	//ͳ�����ͣ����λΪ����ʾ��˨���������������ֽ�������κţ��۸����ռ�װ����˨���ԣ�
	//���λΪ����ʾ��˨ͬʱ��������κţ��κ�ͳ�Ʒ�Χ���ַ�����ʽ�洢�ں��������ֽ���ָ��ַ�У�
	//�θ�λΪ1��ʾ�Ƿ�Ϊ��������˨������ͣ�λ��ʾ�������ȵ����޺ţ���Ϊ����ʾ������������
	BYTE cStatFlag;
	//�κ�ǰ׺�ַ����ɺ������ַ������κ����ֲ���;�۸����ռ�װ����˨���ԣ�
	//���ͳ����������λΪ1ʱ���ˣ����ֽڱ�ʾ����˨�Ķκ�ͳ�Ʒ�Χ�ַ����Ĵ洢��ַ
	DWORD dwSeg;			//�۸����ռ�װ����˨���ԣ�
	CXhChar50 statSegStr;	//���ַ�����ʽ���δ洢��Ҫ���ͳ�Ƶ���˨�κ�ͳ�Ʒ�Χ�۸����ռ�װ����˨���ԣ�
	BYTE cLegQuad;		//�������ȵ����޺ţ���Ϊ����ʾ���������۸���װ�乹�����ԣ�
	CFGWORD cfgword;		//�۸����ռ�װ����˨���ԣ�
};
class CBoltAssemblySection : public CDataSection
{
public:
	CBoltAssemblySection(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	DWORD AssemblyCount(bool bTowerSpace=true);
	BOLT_ASSEMBLY GetAssemblyByIndexId(int indexId,bool bTowerSpace=true);
	void GetBoltInfoBufferAt(int indexId,CBuffer& boltBuffer,bool bTowerSpace=true);
	void GetBoltAssemblyBufferAt(int indexId,CBuffer& assemblyBuf,bool bTowerSpace=true);
};
struct PART_ASSEMBLY
{
	bool bInBlockSpace;	//�Ƿ�����װ�䲿���ռ�۹������ԣ�
	WORD  wBlockIndexId;//���������ı�ʶ�����ţ۲���װ�乹�����ԣ�
	BYTE cLegQuad;		//�������ȵ����޺ţ���Ϊ����ʾ���������۸���װ�乹�����ԣ�
	CFGWORD cfgword;	//����ģ�Ϳռ����������ֹ������ߵ���ĺţ۸���װ�乹�����ԣ�
	DWORD dwIndexId;	//�ڹ�����Ϣ�����еı�ʶ�����ţ۹������ԣ�
	char szLayerSymbol[4];	//�ۺ����Ա�ʶ�ַ�����LDS�е�ͼ������
	union GROUPANGLE{
		struct{	//��ϽǸֱ�ʶ
			char ciSectType;	//'L','D','T','X'
			BYTE ciSonIndex;	//��ǰ�Ǹ�����ϽǸ��е�λ���������0~3
			WORD widGroup;		//
		};
		UINT uidGroupAngle;
	}xGroupAngle;
	DWORD dwAddrBriefLine;	//�˼����ߵĴ洢λ��,0��ʾ�޼���
	GECS acs;			//�ڹ���ģ�Ϳռ��ڵ�װ������ϵ����۹������ԣ�
	bool bIsRod;
	GEPOINT startPt,endPt; 
	UINT uiStartPointI,uiEndPointI;
	BYTE cStartCoordType,cEndCoordType;
	CHashList<DWORD> hashBoltIndexByHoleId;
	PART_ASSEMBLY();
	PART_ASSEMBLY(PART_ASSEMBLY &srcPart);
	PART_ASSEMBLY& operator=(PART_ASSEMBLY &srcPart);
	void CloneFrom(PART_ASSEMBLY &srcPart);
};
class CPartAssemblySection : public CDataSection
{
public:
	CPartAssemblySection(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	void GetAssemblyByIndexId(int indexId,bool bTowerSpace,PART_ASSEMBLY &assmbly);
	DWORD AssemblyCount(bool bTowerSpace=true);
};
struct NODE_ASSEMBLY
{
	UINT uiPointI;		//�ڵ���
	char szLayerSymbol[4];	//�ۺ����Ա�ʶ�ַ�����LDS�е�ͼ������
	bool bInBlockSpace;	//�Ƿ�����װ�䲿���ռ�۹������ԣ�
	WORD  wBlockIndexId;//���������ı�ʶ�����ţ۲���װ�乹�����ԣ�
	BYTE cLegQuad;		//�������ȵ����޺ţ���Ϊ����ʾ���������۸���װ�乹�����ԣ�
	CFGWORD cfgword;	//����ģ�Ϳռ����������ֹ������ߵ���ĺţ۸���װ�乹�����ԣ�
	GEPOINT xPosition;	//װ��λ��
};
class CNodeSection : public CDataSection
{
public:
	CNodeSection(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	void GetNodeByIndexId(int indexId,bool bTowerSpace,NODE_ASSEMBLY &node);
	DWORD NodeCount(bool bTowerSpace=true);
};
class CAssembleSection : public CDataSection
{
public:
	CAssembleSection(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	DWORD BlockAssemblyCount();
	BLOCK_ASSEMBLY GetAssemblyByIndexId(int indexId);
	WORD GetBlockIndexId(int indexId);	//��ȡָ������װ�����Ĺ�������������ʶ���
	CNodeSection NodeSection();
	CBoltAssemblySection BoltSection();
	CPartAssemblySection PartSection();
};
class CTowerInstanceSection : public CDataSection
{
public:
	CTowerInstanceSection(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
};
//////////////////////////////////////////////////////////////////////////
// CPartOrgProcessInfoSection
class CPartOrgProcessInfoSection : public CDataSection
{
public:
	CPartOrgProcessInfoSection(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
public:
	BOOL GetOrgProcessInfoBufAt(int indexId,CBuffer& orgProcessInfoBuf);
};
//////////////////////////////////////////////////////////////////////////
//������Ϣ
class CAttachDataSection : public CDataSection
{
public:
	CAttachDataSection(DWORD zeroAddrOffset=0,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
public:
	CPartOrgProcessInfoSection PartOrgProcessInfoSection();
};
//////////////////////////////////////////////////////////////////////////
//TID�ļ��洢(�������͸�����Ϣ)
class CTIDBuffer : public CBuffer
{
	GECS mcs;					//������ģ�Ϳռ�����ϵ���������Ļ��
	DWORD m_dwZeroAddrOffset;	//�ļ�ͷ��ָ����������洢��ַ��λ��ƫ����
	CXhChar16 m_sVersion;
	CXhChar50 m_sTowerType;
public:
	CTIDBuffer(void);
	~CTIDBuffer(void);
	void SetZeroAddrOffset(DWORD zeroAddrOffset){m_dwZeroAddrOffset=zeroAddrOffset;}
	DWORD ZeroAddrOffset(){return m_dwZeroAddrOffset;}
	//�������ӷ����ڵ���Ե�ַת�����ļ��洢�ռ��ȫ�ֵ�ַ
	DWORD AddrLtoG(DWORD addr){return addr+m_dwZeroAddrOffset;}
	//�����ļ��洢�ռ��ȫ�ֵ�ַת�������ӷ����ڵ���Ե�ַ
	DWORD AddrGtoL(DWORD addr){return addr-m_dwZeroAddrOffset;}
	//
	CXhChar16 Version(){return m_sVersion;}
	CXhChar50 GetTowerType(){return m_sTowerType;}
	GECS ModelCoordSystem(){return mcs;}
	bool InitBuffer(const char* srcBuf, DWORD buf_size);
public:
	CXhChar16 MemBufVersion(){return m_sVersion;}
	CModuleSection ModuleSection();
	CSubLegFoundationSection SubLegFoundationSection();
	CWireNodeSection WireNodeSection();
	CProjectInfoSection ProjectInfoSection();
	CBlockSection BlockSection();
	CBoltSection BoltSection();
	CPartSection PartSection();
	CAssembleSection AssembleSection();
	//CTowerInstanceSection TowerInstanceSection(){};
	//������
	CAttachDataSection AttachDataSection();
};
