#include "StdAfx.h"
#include ".\tidmodel.h"
#include "ParseAdaptNo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static DWORD CFG_NO[32]={ 0X00000001,0X00000002,0X00000004,0X00000008,0X00000010,0X00000020,0X00000040,0X00000080,
	0X00000100,0X00000200,0X00000400,0X00000800,0X00001000,0X00002000,0X00004000,0X00008000,
	0X00010000,0X00020000,0X00040000,0X00080000,0X00100000,0X00200000,0X00400000,0X00800000,
	0X01000000,0X02000000,0X04000000,0X08000000,0X10000000,0X20000000,0X40000000,0X80000000};

//呼高
TOWER_MODULE CModuleSection::GetModuleAt(int i)
{
	TOWER_MODULE module;
	BYTE* pBodyNo=(BYTE*)BufferPtr()+1+21*i;
	module.m_cBodyNo=*pBodyNo;
	memcpy(&module.m_dwLegCfgWord,pBodyNo+1,20);
	for(BYTE j=1;j<=160;j++)
	{
		if(module.m_dwLegCfgWord.IsHasNo(j))
		{
			module.m_arrActiveQuadLegNo[0]=module.m_arrActiveQuadLegNo[1]=
				module.m_arrActiveQuadLegNo[2]=module.m_arrActiveQuadLegNo[3]=j;
			break;
		}
	}
	return module;
}
//部件
CBlockSection::CBlockSection(DWORD zeroAddrOffset,char* buf/*=NULL*/,DWORD size/*=0*/,const char* version/*=NULL*/){
	m_dwZeroAddrOffset=zeroAddrOffset;
	m_data=buf;
	m_dwBufSize=size;
	m_sVersion.Copy(version);
}
TOWER_BLOCK CBlockSection::GetBlockAt(int i)
{
	TOWER_BLOCK block;
	CBuffer buf(BufferPtr(),BufferLength());
	buf.SeekPosition(2+100*i);
	buf.ReadPoint(block.lcs.origin);
	buf.ReadPoint(block.lcs.axis_x);
	buf.ReadPoint(block.lcs.axis_y);
	buf.ReadPoint(block.lcs.axis_z);
	DWORD name_addr;
	buf.ReadDword(&name_addr);
	buf.SeekPosition(AddrGtoL(name_addr));
	buf.ReadString(block.name,50);
	return block;
}
void CBlockSection::GetBlockBufferAt(int index,CBuffer& blockBuf)
{
	blockBuf.ClearContents();
	CBuffer buf(BufferPtr(),BufferLength());
	buf.SeekPosition(2+100*index);
	blockBuf.Write(buf,96);
	DWORD dwAddr;
	buf.ReadDword(&dwAddr);
	buf.SeekPosition(AddrGtoL(dwAddr));
	char name[50]="";
	buf.ReadString(name,50);
	blockBuf.WriteString(name,50);
}
//螺栓系列
bool CBoltSeries::GetBoltSizeAt(int i,BOLTINFO& bolt)
{
	CBuffer buf(BufferPtr(),BufferLength());
	BYTE d_count;
	WORD size_count;
	buf.ReadByte(&d_count);
	buf.Offset(2);	//偏移1B垫圈数量，1B螺帽数量
	buf.ReadWord(&size_count);
	buf.Offset(23);	//偏移23B螺栓库名称
	buf.Offset(i*36);
	buf.ReadWord(&bolt.d);			//螺栓直径
	buf.ReadWord(&bolt.L);			//有效长(mm)
	buf.ReadWord(&bolt.Lnt);		//无扣长
	buf.ReadWord(&bolt.up_lim);		//通厚上限
	buf.ReadWord(&bolt.down_lim);	//通厚下限
	buf.ReadDouble(&bolt.weight);	//kg
	buf.Read(bolt.spec,18);	//固定长度的螺栓规格描述字符（含末尾0截止字符）
	//附加螺栓柱实体数据内存
	DWORD offset,len;
	buf.SeekPosition(5+23+size_count*36+d_count*6+4*i);
	buf.ReadDword(&offset);
	buf.SeekPosition(AddrGtoL(offset));
	buf.ReadDword(&len);
	bolt.solidOfBolt.AttachBuffer(buf.GetCursorBuffer(),len);
	//附加螺帽实体数据内存
	buf.SeekPosition(5+23+size_count*36);
	for(BYTE di=0;di<d_count;di++)
	{
		WORD rd;
		buf.ReadWord(&rd);
		if(rd==bolt.d)
		{
			buf.ReadDword(&offset);	//螺栓帽实体数据存储地址
			buf.SeekPosition(AddrGtoL(offset));
			buf.ReadDword(&len);
			bolt.solidOfCap.AttachBuffer(buf.GetCursorBuffer(),len);
			break;
		}
		else
			buf.Offset(4);
	}
	return true;
}
bool CBoltSeries::GetBoltCapSolidBody(int indexId, CSolidBody& body)
{
	CBuffer buf(BufferPtr(),BufferLength());
	buf.SeekPosition(5+BoltSizeCount()*32+indexId*6-4);
	DWORD offset,len;
	buf.Offset(2);	//螺栓直径
	buf.ReadDword(&offset);
	buf.SeekPosition(AddrGtoL(offset));
	buf.ReadDword(&len);
	body.ReadFrom(&buf,len);
	return true;
}
bool CBoltSeries::GetBoltSolidBody(int indexId, CSolidBody& body)
{
	CBuffer buf(BufferPtr(),BufferLength());
		buf.SeekPosition(5+BoltSizeCount()*32+BoltDCount()*6+indexId*4-4);
	DWORD offset,len;
	buf.ReadDword(&offset);
	buf.SeekPosition(AddrGtoL(offset));
	buf.ReadDword(&len);
	body.ReadFrom(&buf,len);
	return true;
}
CBoltSeries CBoltSection::GetBoltSeriesAt(int i)
{
	CBuffer buf(BufferPtr(),BufferLength());
	buf.SeekPosition(1+i*4);
	DWORD offset,len;
	buf.ReadDword(&offset);
	buf.SeekPosition(AddrGtoL(offset));
	buf.ReadDword(&len);
	return CBoltSeries(ZeroAddrOffset()+buf.GetCursorPosition(),buf.GetCursorBuffer(),len,m_sVersion);
}
void CBoltSection::InitLsFamily(int i,CLsFamily* pFamily)
{
	CBoltSeries boltSeries=GetBoltSeriesAt(i);
	pFamily->m_nDisplayWasher=boltSeries.BoltWasherCount();
	pFamily->m_nLsCap=boltSeries.BoltCapCount();
	int nRec=boltSeries.BoltSizeCount();
	pFamily->CreateFamily(nRec);
	char sGrade[50]="";
	for(int j=0;j<pFamily->GetCount();j++)
	{
		LS_XING_HAO* pLsXingHao=pFamily->RecordAt(j);
		BOLTINFO boltInfo;
		boltSeries.GetBoltSizeAt(j,boltInfo);
		pLsXingHao->d=boltInfo.d;
		pLsXingHao->valid_len=boltInfo.L;
		pLsXingHao->no_thread_len=boltInfo.Lnt;
		pLsXingHao->up_lim=boltInfo.up_lim;
		pLsXingHao->down_lim=boltInfo.down_lim;
		strcpy(pLsXingHao->guige,boltInfo.spec);
		sscanf(pLsXingHao->guige,"%[^M]",sGrade);
		pLsXingHao->grade=atof(sGrade);
		pLsXingHao->weight=boltInfo.weight;
	}
	//读取螺栓库名称
	CBuffer buf(boltSeries.BufferPtr(),boltSeries.BufferLength());
	buf.SeekPosition(5);
	char series_name[23]="";
	buf.Read(series_name,23);	//固定长度的螺栓库名称（含末尾0截止字符）
	strncpy(pFamily->name,series_name,16);
}
//材质库
CMaterialLibrary CPartSection::GetMatLibrary()
{
	CBuffer buf(BufferPtr(),BufferLength());
	BYTE mat_n;
	buf.ReadByte(&mat_n);
	DWORD part_n;
	buf.ReadDword(&part_n);
	return CMaterialLibrary(mat_n,buf.GetCursorBuffer(),mat_n*8);
}
//构件
PART_INFO CPartSection::GetPartInfoAt(int indexId)
{
	BYTE mat_n;
	DWORD part_n;
	CBuffer buf(BufferPtr(),BufferLength());
	buf.ReadByte(&mat_n);
	buf.ReadDword(&part_n);
	buf.Offset(mat_n*8);
	buf.Offset((indexId-1)*33);
	PART_INFO part;
	buf.ReadWord(&part.wModelSpace);
	buf.ReadByte(&part.cPartType);
	buf.ReadWord(&part.wStateFlag);
	buf.ReadByte(&part.cMaterial);
	buf.ReadDword(&part.dwSeg);
	buf.ReadFloat(&part.fWidth);
	buf.ReadFloat(&part.fThick);
	buf.ReadFloat(&part.fHeight);
	buf.ReadWord(&part.wLength);
	buf.ReadFloat(&part.fWeight);
	buf.ReadByte(&part.cFuncType);
	buf.ReadDword(&part.addrAttachInfo);
	//读取附加信息
	buf.SeekPosition(AddrGtoL(part.addrAttachInfo));
	DWORD size;
	buf.ReadDword(&size);
	part.solid.AttachBuffer(buf.GetCursorBuffer(),size);
	buf.Offset(size);
	buf.ReadString(part.sPartNo,50);
	buf.ReadString(part.spec,50);
	buf.ReadString(part.sNotes,50);
	buf.ReadDword(&size);	//构件生产工艺信息存储长度
	buf.Offset(size);		//不需要输出工艺信息，所以略掉此部分信息
	return part;
}
void CPartSection::GetPartBufferAt(int indexId,CBuffer& partBuffer)
{
	partBuffer.ClearContents();
	CBuffer sectionBuf(BufferPtr(),BufferLength());
	BYTE mat_n;
	sectionBuf.ReadByte(&mat_n);
	DWORD iValue,iOffset,size;
	sectionBuf.ReadDword(&iValue);
	sectionBuf.Offset(mat_n*8);
	sectionBuf.Offset((indexId-1)*33);	//跳到第indexId处的信息处
	partBuffer.Write(sectionBuf,29);	//读取构件基本信息
	sectionBuf.ReadDword(&iOffset);
	sectionBuf.SeekPosition(AddrGtoL(iOffset));
	sectionBuf.ReadDword(&size);
	partBuffer.WriteDword(size);		//读取构件实体信息内存大小
	partBuffer.Write(sectionBuf,size);	//读取构件实体信息
	CXhChar50 sText;
	sectionBuf.ReadString(sText,50);
	partBuffer.WriteString(sText);		//读取构件编号
	sectionBuf.ReadString(sText,50);
	partBuffer.WriteString(sText);		//读取构件规格
	sectionBuf.ReadString(sText,50);
	partBuffer.WriteString(sText);		//读取备注
	sectionBuf.ReadDword(&size);		
	partBuffer.WriteDword(size);		//读取构件工艺信息内存大小
	partBuffer.Write(sectionBuf,size);	//读取构件工艺信息
}
DWORD CAssembleSection::BlockAssemblyCount()
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_count;
	buf.ReadDword(&block_count);
	return block_count;
}
BLOCK_ASSEMBLY CAssembleSection::GetAssemblyAt(int indexId)
{
	CBuffer buf(BufferPtr(),BufferLength());
	buf.Offset(4+(indexId-1)*128);
	BLOCK_ASSEMBLY assembly;
	buf.ReadWord(&assembly.wIndexId);
	buf.ReadPoint(assembly.acs.origin);
	buf.ReadPoint(assembly.acs.axis_x);
	buf.ReadPoint(assembly.acs.axis_y);
	buf.ReadPoint(assembly.acs.axis_z);
	buf.ReadDword(&assembly.dwSeg);
	buf.ReadByte(&assembly.cLegQuad);
	buf.Read(&assembly.cfgword,24);
	buf.ReadByte(&assembly.reserved);
	return assembly;
}
CBoltAssemblySection CAssembleSection::BoltSection()
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_assembly_n,size;
	buf.ReadDword(&block_assembly_n);
	buf.Offset(block_assembly_n*128);
	buf.ReadDword(&size);
	return CBoltAssemblySection(m_dwZeroAddrOffset+buf.GetCursorPosition(),buf.GetCursorBuffer(),size);
}
DWORD CBoltAssemblySection::AssemblyCount(bool bTowerSpace/*=true*/)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD assemble_n;
	if(bTowerSpace)
		buf.Offset(4);
	buf.ReadDword(&assemble_n);
	return assemble_n;
}
BOLT_ASSEMBLY CBoltAssemblySection::GetAssemblyAt(int indexId,bool bTowerSpace/*=true*/)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_bolt_n,tower_bolt_n;
	buf.ReadDword(&block_bolt_n);
	buf.ReadDword(&tower_bolt_n);

	BOLT_ASSEMBLY assembly;
	if(bTowerSpace)
	{
		buf.Offset(block_bolt_n*64);//跳过部件模型空间中螺栓装配信息区域
		buf.Offset((indexId-1)*92);	//跳到当前索引标识指向的记录
		assembly.bInBlockSpace=false;
		assembly.wBlockIndexId=0;
	}
	else
	{
		buf.Offset((indexId-1)*64);
		assembly.bInBlockSpace=true;
	}
	//共用属性
	buf.ReadByte(&assembly.cSeriesId);
	buf.ReadWord(&assembly.wIndexId);
	buf.ReadFloat(&assembly.grade);
	//共用属性
	buf.ReadPoint(assembly.origin);
	buf.ReadPoint(assembly.work_norm);
	buf.ReadWord(&assembly.wL0);
	buf.ReadByte(&assembly.cPropFlag);
	buf.ReadByte(&assembly.cDianQuanN);
	buf.ReadByte(&assembly.cDianQuanThick);
	buf.ReadWord(&assembly.wDianQuanOffset);
	if(!bTowerSpace)	//部件空间中螺栓装配对象对应的部件索引标识
		buf.ReadWord(&assembly.wBlockIndexId);
	else //if(bTowerSpace)
	{					//杆塔模型空间中螺栓装配对象对应的部件索引标识
		buf.ReadByte(&assembly.cStatFlag);
		buf.ReadDword(&assembly.dwSeg);
		buf.ReadByte(&assembly.cLegQuad);
		buf.Read(&assembly.cfgword,24);
		if(assembly.cStatFlag&0x80)	//螺栓同时归属多个段号，段号统计范围以字符串形式存储在后续４个字节所指地址中
		{
			buf.SeekPosition(AddrGtoL(assembly.dwSeg));
			buf.ReadString(assembly.statSegStr,50);
		}
	}
	return assembly;
}
void CBoltAssemblySection::GetBoltInfoBufferAt(int indexId,CBuffer& boltBuffer,bool bTowerSpace/*=true*/)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_bolt_n,tower_bolt_n;
	buf.ReadDword(&block_bolt_n);
	buf.ReadDword(&tower_bolt_n);
	if(bTowerSpace)
	{
		buf.Offset(block_bolt_n*64);//跳过部件模型空间中螺栓装配信息区域
		buf.Offset((indexId-1)*92);	//跳到当前索引标识指向的记录
	}
	else
		buf.Offset((indexId-1)*64);
	boltBuffer.ClearContents();
	boltBuffer.Write(buf,7);
}
void CBoltAssemblySection::GetBoltAssemblyBufferAt(int indexId,CBuffer& assemblyBuf,bool bTowerSpace/*=true*/)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_bolt_n,tower_bolt_n;
	buf.ReadDword(&block_bolt_n);
	buf.ReadDword(&tower_bolt_n);
	if(bTowerSpace)
	{
		buf.Offset(block_bolt_n*64);//跳过部件模型空间中螺栓装配信息区域
		buf.Offset((indexId-1)*92);	//跳到当前索引标识指向的记录
	}
	else
		buf.Offset((indexId-1)*64);
	buf.Offset(7);
	assemblyBuf.ClearContents();
	assemblyBuf.Write(buf,55);
	WORD wBlockIndexId=0;
	if(bTowerSpace)
	{					
		assemblyBuf.WriteWord(wBlockIndexId);
		BYTE cFlag=0,cQuad=0;
		DWORD dwSeg=0;
		CFGWORD cfgword;
		CXhChar50 statSetStr;
		// 最高位为０表示螺栓仅归属后续４个字节所代表段号；
		// 最高位为１表示螺栓同时归属多个段号，段号统计范围以字符串形式存储在后续４个字节所指地址中；
		// 次高位为１表示是否为接腿上螺栓；
		// 其余低６位表示归属接腿的象限号，如为０表示归属杆塔本体。
		buf.ReadByte(&cFlag);
		buf.ReadDword(&dwSeg);
		buf.ReadByte(&cQuad);
		buf.Read(&cfgword,24);
		if(cFlag&0x80)		//螺栓同时归属多个段号
		{	//
			buf.SeekPosition(AddrGtoL(dwSeg));
			buf.ReadString(statSetStr,50);
			ATOM_LIST<SEGI> segNoList;
			if(GetSortedSegNoList(CString(statSetStr),segNoList)>0)
				dwSeg=segNoList.GetFirst()->iSeg;
			else 
				dwSeg=0;
		}
		assemblyBuf.WriteDword(dwSeg);
		assemblyBuf.WriteString(statSetStr);
		assemblyBuf.WriteByte(cQuad);	//腿部构件从属象限
		assemblyBuf.Write(&cfgword,24);	//配材号
	}
	else
	{
		buf.ReadWord(&wBlockIndexId);
		assemblyBuf.WriteWord(wBlockIndexId);
	}
}
CPartAssemblySection CAssembleSection::PartSection()
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_assembly_n,size;
	buf.ReadDword(&block_assembly_n);
	buf.Offset(block_assembly_n*128);
	buf.ReadDword(&size);
	buf.Offset(size);	//跳过螺栓装配区
	buf.ReadDword(&size);
	return CPartAssemblySection(m_dwZeroAddrOffset+buf.GetCursorPosition(),buf.GetCursorBuffer(),size);
}
DWORD CPartAssemblySection::AssemblyCount(bool bTowerSpace/*=true*/)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD assemble_n;
	if(bTowerSpace)
		buf.Offset(4);
	buf.ReadDword(&assemble_n);
	return assemble_n;
}
void CPartAssemblySection::GetAssemblyAt(int indexId,bool bTowerSpace,PART_ASSEMBLY &assembly)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_part_n,tower_part_n,rod_n;
	buf.ReadDword(&block_part_n);
	buf.ReadDword(&tower_part_n);
	buf.ReadDword(&rod_n);
	DWORD addrSingleLinePos=0;
	if(bTowerSpace)
	{
		buf.Offset(block_part_n*(106+4));	//跳过部件模型空间中构件装配信息区域
		buf.Offset((indexId-1)*(129+4));	//跳到当前索引标识指向的记录
		assembly.bInBlockSpace=false;
		assembly.wBlockIndexId=0;
		buf.ReadDword(&assembly.dwIndexId);
		buf.ReadDword(&addrSingleLinePos);
		buf.ReadByte(&assembly.cLegQuad);
		buf.Read(&assembly.cfgword,24);
	}
	else
	{
		buf.Offset((indexId-1)*(106+4));	//跳到当前索引标识指向的记录
		assembly.bInBlockSpace=true;
		assembly.cLegQuad=0;
		buf.ReadDword(&assembly.dwIndexId);
		buf.ReadDword(&addrSingleLinePos);
		buf.ReadWord(&assembly.wBlockIndexId);
	}
	buf.ReadPoint(assembly.acs.origin);
	buf.ReadPoint(assembly.acs.axis_x);
	buf.ReadPoint(assembly.acs.axis_y);
	buf.ReadPoint(assembly.acs.axis_z);
	//读取装配记录螺栓孔信息地址
	DWORD addrHoleInfo=0;
	buf.ReadDword(&addrHoleInfo);
	if(addrHoleInfo>0)
	{	//读取附加信息
		buf.SeekPosition(AddrGtoL(addrHoleInfo));
		DWORD size;
		buf.ReadDword(&size);
		assembly.hashBoltIndexByHoleId.Empty();
		for(DWORD i=0;i<size;i++)
		{
			DWORD dwBoltIndex=0,dwHoleId=0;
			buf.ReadDword(&dwHoleId);
			buf.ReadDword(&dwBoltIndex);
			assembly.hashBoltIndexByHoleId.SetValue(dwHoleId,dwBoltIndex);
		}
	}
	//读取杆件简化线数据
	if(addrSingleLinePos>0)
	{
		buf.SeekPosition(AddrGtoL(addrSingleLinePos));
		buf.ReadPoint(assembly.startPt);
		buf.ReadPoint(assembly.endPt);
		assembly.bIsRod=true;
	}
}
//////////////////////////////////////////////////////////////////////////
// 
CPartOrgProcessInfoSection CAttachDataSection::PartOrgProcessInfoSection()
{
	CBuffer buf(BufferPtr(),BufferLength());
	buf.SeekPosition(0);
	DWORD pos=buf.ReadDword();
	DWORD sect_len=0;
	if(pos>0)
	{
		buf.SeekPosition(AddrGtoL(pos));
		sect_len=buf.ReadDword();
	}
	return CPartOrgProcessInfoSection(m_dwZeroAddrOffset+buf.GetCursorPosition(),buf.GetCursorBuffer(),sect_len);
}
//////////////////////////////////////////////////////////////////////////
//CPartOrgProcessInfoSection
BOOL CPartOrgProcessInfoSection::GetOrgProcessInfoBufAt(int indexId,CBuffer& orgProcessInfoBuf)
{
	if(BufferLength()<=0)	//没有附加信息
		return FALSE;
	CBuffer buf(BufferPtr(),BufferLength());
	buf.Offset((indexId-1)*4);
	DWORD offset,len;
	buf.ReadDword(&offset);
	if(offset==0)
		return FALSE;
	buf.SeekPosition(AddrGtoL(offset));
	buf.ReadDword(&len);
	if(len>0)
		orgProcessInfoBuf.Write(buf.GetCursorBuffer(),len);
	else
		return FALSE;
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////
//CIIDBuffer
//////////////////////////////////////////////////////////////////////////////////////////////
CTIDBuffer::CTIDBuffer(void)
{
	m_dwZeroAddrOffset=0;
}

CTIDBuffer::~CTIDBuffer(void)
{
}
CModuleSection CTIDBuffer::ModuleSection()
{
	DWORD pos,sect_len;
	SeekPosition(AddrLtoG(0));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	return CModuleSection(GetCursorBuffer(),sect_len,m_sVersion);
}
CBlockSection CTIDBuffer::BlockSection()
{
	DWORD pos,sect_len;
	SeekPosition(AddrLtoG(4));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	return CBlockSection(GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
}
CBoltSection CTIDBuffer::BoltSection()
{
	DWORD pos,sect_len;
	SeekPosition(AddrLtoG(8));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	return CBoltSection(GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
}
CPartSection CTIDBuffer::PartSection()
{
	DWORD pos,sect_len;
	SeekPosition(AddrLtoG(12));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	return CPartSection(GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
}
CAssembleSection CTIDBuffer::AssembleSection()
{
	DWORD pos,sect_len;
	SeekPosition(AddrLtoG(16));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	return CAssembleSection(GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
}

CAttachDataSection CTIDBuffer::AttachDataSection()
{
	DWORD pos,sect_len;
	SeekPosition(AddrLtoG(16));	//附加数据分区在最后一个分区之后，此处根据最后一个分区进行跳转
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	SeekPosition(pos+sect_len+4);	//+4为sect_len所在的4个字节
	DWORD attach_sect_len=GetLength()-GetCursorPosition();
	return CAttachDataSection(GetCursorPosition(),GetCursorBuffer(),attach_sect_len,m_sVersion);
}
void CTIDBuffer::InitBuffer(const char* srcBuf, DWORD buf_size)
{
	ClearBuffer();
	Write(srcBuf,buf_size);
	SeekToBegin();
	CXhChar50 sDocType;
	ReadString(sDocType,50);
	if(strstr(sDocType,"Xerofox tower solid data media file")==NULL)
		throw "非预期的文件格式，文件打开失败!";
	char* key=strtok(sDocType,"-");
	char* ver=strtok(NULL,"-");
	double version=1000;
	if(ver!=NULL)
		version=atof(ver);
	if(version>1.2)
		throw "此文件版本号太高,不能直接读取此文件";
	m_sVersion.Copy(ver);
	ReadString(m_sTowerType,50);
	ReadPoint(mcs.axis_x);
	ReadPoint(mcs.axis_y);
	ReadPoint(mcs.axis_z);
	SetZeroAddrOffset(GetCursorPosition());
}