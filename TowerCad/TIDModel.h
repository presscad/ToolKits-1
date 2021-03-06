#pragma once
#include "f_ent.h"
#include "XhLdsLm.h"
#include "XhCharString.h"
#include "I_DrawSolid.h"
#include "PartLib.h"
#include "HashTable.h"

class CDataSection
{
protected:
	DWORD m_dwZeroAddrOffset;	//本数据区起始零点在整体内存中的地址偏移位置
	DWORD m_dwBufSize;
	char* m_data;
	CXhChar16 m_sVersion;
public:
	CDataSection(){m_dwBufSize=0;m_data=NULL;m_dwZeroAddrOffset=0;}
	char* BufferPtr(){return m_data;}
	DWORD BufferLength(){return m_dwBufSize;}
	DWORD ZeroAddrOffset(){return m_dwZeroAddrOffset;}
	//将缓存子分区内的相对地址转换到文件存储空间的全局地址
	DWORD AddrLtoG(DWORD addr){return addr+m_dwZeroAddrOffset;}
	//将到文件存储空间的全局地址转换缓存子分区内的相对地址
	DWORD AddrGtoL(DWORD addr){return addr-m_dwZeroAddrOffset;}
};
//呼高
struct TOWER_MODULE
{
	BYTE m_cBodyNo;			//塔主体(身&头部)所对应的配材（接腿）号
	BYTE m_arrActiveQuadLegNo[4];	//本模型当前第一～四象限的当前接腿号
	CFGWORD m_dwLegCfgWord;	//接腿配材号
};
//呼高
class CModuleSection : public CDataSection
{
public:
	CModuleSection(char* buf=NULL,DWORD size=0,const char* version=NULL){m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	BYTE GetModuleCount(){return *((BYTE*)m_data);}
	TOWER_MODULE GetModuleAt(int i);
};
//部件
struct TOWER_BLOCK
{
	UCS_STRU lcs;			//在自身模型空间内定义的装配用定位坐标系
	CXhChar50 name;			//部件的名称
	CSolidBody solid;		//实体数据（注意防止内生缓存在析构函数中自动销毁问题，如函数参数、赋值时）
};
//部件
class CBlockSection : public CDataSection
{
public:
	CBlockSection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL);
	WORD GetBlockCount(){return *((WORD*)m_data);}
	TOWER_BLOCK GetBlockAt(int i);
	void GetBlockBufferAt(int index,CBuffer& blockBuf);
};
struct BOLTINFO
{
	WORD d;			//螺栓直径
	WORD L;			//有效长(mm)
	WORD Lnt;		//无扣长
	WORD down_lim;	//螺栓通厚下限
	WORD up_lim;	//螺栓通厚上限
	double weight;	//kg
	char spec[18];	//固定长度的螺栓规格描述字符（含末尾0截止字符）
public:	//附加实体显示数据（注意防止内生缓存在析构函数中自动销毁问题，如函数参数、赋值时）
	CSolidBody solidOfCap;	//螺帽
	CSolidBody solidOfBolt;	//螺栓柱
};
class CBoltSeries : public CDataSection
{
public:
	CBoltSeries(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	BYTE BoltDCount(){return *((BYTE*)m_data);}
	BYTE BoltWasherCount(){return *((BYTE*)(m_data+1));}
	BYTE BoltCapCount(){return *((BYTE*)(m_data+2));}
	WORD BoltSizeCount(){return *((WORD*)(m_data+3));}
	bool GetBoltSizeAt(int i,BOLTINFO& bolt);
	bool GetBoltCapSolidBody(int indexId, CSolidBody& body);
	bool GetBoltSolidBody(int indexId, CSolidBody& body);
};
class CBoltSection : public CDataSection
{
public:
	CBoltSection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	WORD BoltSeriesCount(){return *((BYTE*)m_data);}
	CBoltSeries GetBoltSeriesAt(int i);
	void InitLsFamily(int i,CLsFamily* pFamily);
};
class CMaterialLibrary : public CDataSection
{
	BYTE m_byteMatTypeCount;
public:
	CMaterialLibrary(BYTE mat_n,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_byteMatTypeCount=mat_n,m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
};
//构件基本信息
struct PART_INFO{
	WORD wModelSpace;
	BYTE cPartType;	//构件分类，1:角钢2:螺栓3:钢板4:钢管5:扁铁6:槽钢
	WORD wStateFlag;//构件状态属性，首位为1表示构件同时属于嵌入式部件和杆塔模型空间，即0x8000
	BYTE cMaterial;	//材质简化字符
	DWORD dwSeg;	//段号，高16位为字符，低两位为数字部分
	float fWidth;	//宽度(mm)
	float fThick;	//厚度(mm)
	float fHeight;	//高度(mm)
	WORD wLength;	//长度(mm)
	float fWeight;	//单重(kg)
	BYTE cFuncType;	//功用类型
	DWORD addrAttachInfo;	//附加信息文件内存储地址
public:	//构件附加属性
	CXhChar50 sPartNo;	//构件编号
	CXhChar50 spec;		//规格
	CXhChar50 sNotes;	//备注
	CSolidBody solid;	//实体显示数据（注意防止内生缓存在析构函数中自动销毁问题，如函数参数、赋值时）
};
class CPartSection : public CDataSection
{
public:
	CPartSection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	CMaterialLibrary GetMatLibrary();
	PART_INFO GetPartInfoAt(int indexId);
	void GetPartBufferAt(int indexId,CBuffer& buffer);
};
struct BLOCK_ASSEMBLY
{
	WORD wIndexId;		//部件索引标识号
	GECS acs;		//在杆塔模型空间内的装配坐标系定义
	DWORD dwSeg;		//段号
	CFGWORD cfgword;	//配材号
	BYTE cLegQuad;		//归属接腿的象限号，如为０表示属塔身本件
	BYTE reserved;		//1个字节对齐保留位
};
struct BOLT_ASSEMBLY
{
	bool bInBlockSpace;	//是否属于装配部件空间［共享属性］
	BYTE cSeriesId;	//螺栓规格系列索引号［共享属性］
	WORD wIndexId;	//螺栓规格在系列内的索引标识号［共享属性］
	WORD  wBlockIndexId;//归属部件的标识索引号［部件装配螺栓属性］
	float grade;		//螺栓等级
	GEPOINT origin;		//螺栓装配原点［共享属性］
	GEPOINT work_norm;	//螺栓工作法线［共享属性］
	WORD wL0;			//螺栓通厚(mm)［共享属性］
	BYTE cPropFlag;		//特殊要求标识字节（目前末位为１表示防盗螺栓，其余位保留）［共享属性］
	BYTE cDianQuanN,cDianQuanThick;	//垫圈数量,垫圈厚度［共享属性］
	WORD wDianQuanOffset;	//垫圈位置偏移量（自螺栓工作基点沿工作法线的偏移量）［共享属性］
	//统材类型，最高位为０表示螺栓仅归属后续４个字节所代表段号；［杆塔空间装配螺栓属性］
	//最高位为１表示螺栓同时归属多个段号，段号统计范围以字符串形式存储在后续４个字节所指地址中；
	//次高位为1表示是否为接腿上螺栓；其余低６位表示归属接腿的象限号，如为０表示归属杆塔本体
	BYTE cStatFlag;
	//段号前缀字符（可含２个字符），段号数字部分;［杆塔空间装配螺栓属性］
	//如果统材类型中首位为1时，此４个字节表示该螺栓的段号统计范围字符串的存储地址
	DWORD dwSeg;			//［杆塔空间装配螺栓属性］
	CXhChar50 statSegStr;	//以字符串形式依次存储需要多段统计的螺栓段号统计范围［杆塔空间装配螺栓属性］
	BYTE cLegQuad;		//归属接腿的象限号，如为０表示属塔身本件［杆塔装配构件属性］
	CFGWORD cfgword;		//［杆塔空间装配螺栓属性］
};
class CBoltAssemblySection : public CDataSection
{
public:
	CBoltAssemblySection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	DWORD AssemblyCount(bool bTowerSpace=true);
	BOLT_ASSEMBLY GetAssemblyAt(int indexId,bool bTowerSpace=true);
	void GetBoltInfoBufferAt(int indexId,CBuffer& boltBuffer,bool bTowerSpace=true);
	void GetBoltAssemblyBufferAt(int index,CBuffer& assemblyBuf,bool bTowerSpace=true);
};
struct PART_ASSEMBLY
{
	bool bInBlockSpace;	//是否属于装配部件空间［共享属性］
	WORD  wBlockIndexId;//归属部件的标识索引号［部件装配构件属性］
	BYTE cLegQuad;		//归属接腿的象限号，如为０表示属塔身本件［杆塔装配构件属性］
	CFGWORD cfgword;	//杆塔模型空间中用于区分归属呼高的配材号［杆塔装配构件属性］
	DWORD dwIndexId;	//在构件信息分区中的标识索引号［共享属性］
	GECS acs;			//在归属模型空间内的装配坐标系定义［共享属性］
	bool bIsRod;
	GEPOINT startPt,endPt; 
	CHashList<DWORD> hashBoltIndexByHoleId;
	PART_ASSEMBLY(){bInBlockSpace=false;wBlockIndexId=0;cLegQuad=0;dwIndexId=0;bIsRod=false;}
};
class CPartAssemblySection : public CDataSection
{
public:
	CPartAssemblySection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	void GetAssemblyAt(int indexId,bool bTowerSpace,PART_ASSEMBLY &assmbly);
	DWORD AssemblyCount(bool bTowerSpace=true);
};
class CAssembleSection : public CDataSection
{
public:
	CAssembleSection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
	DWORD BlockAssemblyCount();
	BLOCK_ASSEMBLY GetAssemblyAt(int indexId);
	CBoltAssemblySection BoltSection();
	CPartAssemblySection PartSection();
};
class CTowerInstanceSection : public CDataSection
{
public:
	CTowerInstanceSection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
};
//////////////////////////////////////////////////////////////////////////
// CPartOrgProcessInfoSection
class CPartOrgProcessInfoSection : public CDataSection
{
public:
	CPartOrgProcessInfoSection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
public:
	BOOL GetOrgProcessInfoBufAt(int indexId,CBuffer& orgProcessInfoBuf);
};
//////////////////////////////////////////////////////////////////////////
//附加信息
class CAttachDataSection : public CDataSection
{
public:
	CAttachDataSection(DWORD zeroAddrOffset,char* buf=NULL,DWORD size=0,const char* version=NULL)
	{m_dwZeroAddrOffset=zeroAddrOffset;m_data=buf;m_dwBufSize=size;m_sVersion.Copy(version);}
public:
	CPartOrgProcessInfoSection PartOrgProcessInfoSection();
};
//////////////////////////////////////////////////////////////////////////
//TID文件存储(考虑塔型附加信息)
class CTIDBuffer : public CBuffer
{
	GECS mcs;					//杆塔的模型空间坐标系（相对于屏幕）
	DWORD m_dwZeroAddrOffset;	//文件头中指向五个分区存储地址的位置偏移量
	CXhChar16 m_sVersion;
	CXhChar50 m_sTowerType;
public:
	CTIDBuffer(void);
	~CTIDBuffer(void);
	void SetZeroAddrOffset(DWORD zeroAddrOffset){m_dwZeroAddrOffset=zeroAddrOffset;}
	DWORD ZeroAddrOffset(){return m_dwZeroAddrOffset;}
	//将缓存子分区内的相对地址转换到文件存储空间的全局地址
	DWORD AddrLtoG(DWORD addr){return addr+m_dwZeroAddrOffset;}
	//将到文件存储空间的全局地址转换缓存子分区内的相对地址
	DWORD AddrGtoL(DWORD addr){return addr-m_dwZeroAddrOffset;}
	//
	CXhChar16 Version(){return m_sVersion;}
	CXhChar50 GetTowerType(){return m_sTowerType;}
	GECS ModelCoordSystem(){return mcs;}
	void InitBuffer(const char* srcBuf, DWORD buf_size);
public:
	CModuleSection ModuleSection();
	CBlockSection BlockSection();
	CBoltSection BoltSection();
	CPartSection PartSection();
	CAssembleSection AssembleSection();
	//CTowerInstanceSection TowerInstanceSection(){};
	//附加区
	CAttachDataSection AttachDataSection();

};
