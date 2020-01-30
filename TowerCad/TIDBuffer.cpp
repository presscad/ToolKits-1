#include "StdAfx.h"
#include ".\TIDBuffer.h"
#include "LogFile.h"

#ifndef TIDCORE_EXPORTS
#include "ParseAdaptNo.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

CSubLegFoundationSection::CSubLegFoundationSection(BYTE _cnHeightCount,DWORD zeroAddrOffset,char* buf/*=NULL*/,DWORD size/*=0*/,const char* version/*=NULL*/)
{
	m_dwZeroAddrOffset=zeroAddrOffset;
	m_data=buf;
	m_dwBufSize=size;
	m_sVersion.Copy(version);
	cnHeightCount=_cnHeightCount;
	if(m_data!=NULL)
	{
		ciBasePlateThick =(BYTE)*m_data;
		cnAnchorBoltCount=(BYTE)*(m_data+1);
		cnSubLegCount=*((BYTE*)(buf+2+cnAnchorBoltCount*4+cnHeightCount));
	}
}
ANCHOR_LOCATION CSubLegFoundationSection::GetAnchorLocationAt(short index)	//0 based index;
{
	//SeekPosition(AddrLtoG(2+index*4));
	CBuffer sectbuf(m_data,m_dwBufSize);
	sectbuf.SeekPosition(2+index*4);
	ANCHOR_LOCATION location;
	sectbuf.ReadWord(&location.siPosX);
	sectbuf.ReadWord(&location.siPosY);
	return location;
}
GEPOINT CSubLegFoundationSection::GetSubLegFoundationOrgBySerial(BYTE ciBodySerial,BYTE ciLegSerial)	//1 based serial
{
	CBuffer sectbuf(m_data,m_dwBufSize);
	sectbuf.SeekPosition(2+cnAnchorBoltCount*4+ciBodySerial-1);//(wiLegSerial-1)*24);
	BYTE ciStartIndexofHeightSubLeg=0;
	sectbuf.ReadByte(&ciStartIndexofHeightSubLeg);
	sectbuf.SeekPosition(2+cnAnchorBoltCount*4+cnHeightCount+1+(ciStartIndexofHeightSubLeg+ciLegSerial-1)*24);
	GEPOINT xBaseOrg;
	sectbuf.ReadPoint(xBaseOrg);
	return xBaseOrg;
}
//����
CModuleSection::CModuleSection(DWORD zeroAddrOffset,char* buf/*=NULL*/,DWORD size/*=0*/,const char* version/*=NULL*/)
{
	m_dwZeroAddrOffset=zeroAddrOffset;
	m_data=buf;
	m_dwBufSize=size;
	m_sVersion.Copy(version);
	if(m_data!=NULL&&compareVersion(version,"1.4")>=0)
		memcpy(&m_fNamedHeightZeroZ,m_data,8);
	else
		m_fNamedHeightZeroZ=0;
}
BYTE CModuleSection::GetModuleCount()
{
	if(m_data!=NULL&&m_dwBufSize>0)
	{
		if(compareVersion(m_sVersion,"1.4")>=0)
			return *(m_data+8);	//����ֵ8B
		else
			return *m_data;
	}
	else
		return 0;
}
TOWER_MODULE CModuleSection::GetModuleAt(int i)
{
	TOWER_MODULE module;
	bool bAfterV11=compareVersion(m_sVersion,"1.1")>=0;
	bool bAfterV14=compareVersion(m_sVersion,"1.4")>=0;
	int j,record_n=25,init_offset=0;
	if(bAfterV14)
	{
		init_offset=8;	
		record_n=30;	//V1.4�����˺�����Ϣ4��
	}
	else if(bAfterV11)
		record_n=26;	//V1.1�����˼�����Ϣ����
	BYTE* pBodyNo=(BYTE*)BufferPtr()+init_offset+1+record_n*i;
	module.m_cBodyNo=*pBodyNo;
	module.m_uiNamedHeight=0;
	if(bAfterV11)//compareVersion(m_sVersion,"1.1")>=0)
	{
		module.m_cbLegInfo=*(pBodyNo+1);
		if(bAfterV14)
			module.m_uiNamedHeight=*((UINT*)(pBodyNo+2));
		BYTE offset=bAfterV14?6:2;
		memcpy(&module.m_dwLegCfgWord,pBodyNo+offset,24);
	}
	else
	{
		module.m_cbLegInfo=0;
		memcpy(&module.m_dwLegCfgWord,pBodyNo+1,24);
	}
	for(j=1;j<=192;j++)
	{
		if(module.m_dwLegCfgWord.IsHasNo(j))
		{
			module.m_arrActiveQuadLegNo[0]=module.m_arrActiveQuadLegNo[1]=
				module.m_arrActiveQuadLegNo[2]=module.m_arrActiveQuadLegNo[3]=j;
			break;
		}
	}
	//��ȡ���߷��������
	char* pNameSection=BufferPtr()+init_offset+1+record_n*GetModuleCount();
	CBuffer pool(pNameSection,GetModuleCount()*100);	//��֪�����������峤�ȣ�ֻ�ܸ���һ���϶��Ƚϴ�Ļ���ռ�
	for(j=0;j<i;j++)
		pool.Offset(pool.ReadStringLength());
	pool.ReadString(module.name,51);
	return module;
}
CWireNodeSection::CWireNodeSection(DWORD zeroAddrOffset,char* buf/*=NULL*/,DWORD size/*=0*/,const char* version/*=NULL*/)
{
	m_dwZeroAddrOffset=zeroAddrOffset;
	m_data=buf;
	m_dwBufSize=size;
	m_sVersion.Copy(version);
	m_wnWireNodeCount=buf?*((WORD*)buf):0;
}
WIRE_NODE CWireNodeSection::GetWireNodeAt(int i)
{
	CBuffer sectbuf(m_data,m_dwBufSize);
	sectbuf.SeekPosition(2+i*64);	//��ͷΪ2���ֽڣ��洢�ҵ���
	WIRE_NODE wirenode;
	sectbuf.ReadWord(&wirenode.wiCode);
	sectbuf.ReadPoint(wirenode.position);
	sectbuf.Read(wirenode.name,38);
	return wirenode;
}
CProjectInfoSection::CProjectInfoSection(DWORD zeroAddrOffset,char* buf/*=NULL*/,DWORD size/*=0*/,const char* version/*=NULL*/)
{
	m_dwZeroAddrOffset=zeroAddrOffset;
	m_data=buf;
	m_dwBufSize=size;
	m_sVersion.Copy(version);
	m_ciSubProjSectionCount=buf?(BYTE)*buf:0;
}
CProjInfoSubSection::CProjInfoSubSection(DWORD zeroAddrOffset,char* buf/*=NULL*/,DWORD size/*=0*/,const char* version/*=NULL*/)
{
	m_dwZeroAddrOffset=zeroAddrOffset;
	m_data=buf;
	m_dwBufSize=size;
	m_sVersion.Copy(version);
}
CProjInfoSubSection CProjectInfoSection::GetSubSectionAt(int i)
{
	CBuffer sectbuf(m_data,m_dwBufSize);
	sectbuf.SeekPosition(1+i*12);
	DWORD uidSubSection,dwSectAddr=0,dwSectSize=0;
	sectbuf.ReadDword(&uidSubSection);
	sectbuf.ReadDword(&dwSectAddr);
	sectbuf.ReadDword(&dwSectSize);
	//�ӷ����洢��
	char* pBuff=m_data+AddrGtoL(dwSectAddr);
	CProjInfoSubSection subsect(dwSectAddr,pBuff,dwSectSize,m_sVersion);
	subsect.m_uidSubSection=uidSubSection;
	return subsect;
}
//����
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
//��˨ϵ��
CXhChar24 CBoltSeries::GetSeriesName()
{
	if(compareVersion(m_sVersion,"1.1")>=0)
		return CXhChar24(m_data+5);
	return CXhChar24("");
}
bool CBoltSeries::GetBoltSizeAt(int i,CBoltSizeSection& bolt)
{
	CBuffer buf(BufferPtr(),BufferLength());
	bolt.InitBuffer();
	BYTE d_count;
	WORD size_count;
	buf.ReadByte(&d_count);
	buf.Offset(2);	//ƫ��1B��Ȧ������1B��ñ����
	buf.ReadWord(&size_count);
	if(compareVersion(m_sVersion,"1.1")>=0)
	{	//����ͨ������2B������2B
		buf.Offset(23);	//ƫ��23B��˨������
		buf.Offset(i*36);
		bolt.InitBuffer(0,buf.GetCursorBuffer(),36,m_sVersion);
		buf.SeekPosition(5+23+size_count*36+d_count*6+4*i);
	}
	else
	{
		buf.Offset(i*32);
		bolt.InitBuffer(0,buf.GetCursorBuffer(),32,m_sVersion);
		buf.SeekPosition(5+size_count*32+d_count*6+4*i);
	}
	//������˨��ʵ�������ڴ�
	DWORD offset,len;
	buf.ReadDword(&offset);
	buf.SeekPosition(AddrGtoL(offset));
	buf.ReadDword(&len);
	bolt.solid.bolt.AttachBuffer(buf.GetCursorBuffer(),len);
	//������ñʵ�������ڴ�
	if(compareVersion(m_sVersion,"1.1")>=0)
		buf.SeekPosition(5+23+size_count*36);
	else
		buf.SeekPosition(5+size_count*32);
	for(BYTE di=0;di<d_count;di++)
	{
		WORD rd;
		buf.ReadWord(&rd);
		if(rd==bolt.GetDiameter())
		{
			buf.ReadDword(&offset);	//��˨ñʵ�����ݴ洢��ַ
			buf.SeekPosition(AddrGtoL(offset));
			buf.ReadDword(&len);
			bolt.solid.nut.AttachBuffer(buf.GetCursorBuffer(),len);
			break;
		}
		else
			buf.Offset(4);
	}
	return true;
}
bool CBoltSeries::GetBoltSizeAt(int i,BOLTINFO& bolt)
{
	CBuffer buf(BufferPtr(),BufferLength());
	BYTE d_count;
	WORD size_count;
	buf.ReadByte(&d_count);
	buf.Offset(2);	//ƫ��1B��Ȧ������1B��ñ����
	buf.ReadWord(&size_count);
	if(compareVersion(m_sVersion,"1.1")>=0)
	{	//����ͨ������2B������2B
		buf.Offset(23);	//ƫ��23B��˨������
		buf.Offset(i*36);
	}
	else//��V1.1��:ͨ������2B��ͨ������2B
		buf.Offset(i*32);
	buf.ReadWord(&bolt.d);			//��˨ֱ��
	buf.ReadWord(&bolt.L);			//��Ч��(mm)
	buf.ReadWord(&bolt.Lnt);		//�޿۳�
	if(compareVersion(m_sVersion,"1.1")>=0)
	{	//����ͨ������2B������2B
		buf.ReadWord(&bolt.up_lim);		//ͨ������
		buf.ReadWord(&bolt.down_lim);	//ͨ������
	}
	else
		bolt.up_lim=bolt.down_lim=0;
	buf.ReadDouble(&bolt.weight);	//kg
	buf.Read(bolt.spec,18);	//�̶����ȵ���˨��������ַ�����ĩβ0��ֹ�ַ���
	//������˨��ʵ�������ڴ�
	DWORD offset,len;
	if(compareVersion(m_sVersion,"1.1")>=0)
		buf.SeekPosition(5+23+size_count*36+d_count*6+4*i);
	else
		buf.SeekPosition(5+size_count*32+d_count*6+4*i);
	buf.ReadDword(&offset);
	buf.SeekPosition(AddrGtoL(offset));
	buf.ReadDword(&len);
	bolt.solidOfBolt.AttachBuffer(buf.GetCursorBuffer(),len);
	//������ñʵ�������ڴ�
	if(compareVersion(m_sVersion,"1.1")>=0)
		buf.SeekPosition(5+23+size_count*36);
	else
		buf.SeekPosition(5+size_count*32);
	for(BYTE di=0;di<d_count;di++)
	{
		WORD rd;
		buf.ReadWord(&rd);
		if(rd==bolt.d)
		{
			buf.ReadDword(&offset);	//��˨ñʵ�����ݴ洢��ַ
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
#ifndef TIDCORE_EXPORTS
bool CBoltSeries::GetBoltCapSolidBody(int indexId, CSolidBody& body)
{
	CBuffer buf(BufferPtr(),BufferLength());
	if(compareVersion(m_sVersion,"1.1")>=0)
		buf.SeekPosition(5+23+BoltSizeCount()*36+indexId*6-4);
	else
		buf.SeekPosition(5+BoltSizeCount()*32+indexId*6-4);
	DWORD offset,len;
	buf.Offset(2);	//��˨ֱ��
	buf.ReadDword(&offset);
	buf.SeekPosition(AddrGtoL(offset));
	buf.ReadDword(&len);
	body.ReadFrom(&buf,len);
	return true;
}
bool CBoltSeries::GetBoltSolidBody(int indexId, CSolidBody& body)
{
	CBuffer buf(BufferPtr(),BufferLength());
	if(compareVersion(m_sVersion,"1.1")>=0)
		buf.SeekPosition(5+23+BoltSizeCount()*36+BoltDCount()*6+indexId*4-4);
	else
		buf.SeekPosition(5+BoltSizeCount()*32+BoltDCount()*6+indexId*4-4);
	DWORD offset,len;
	buf.ReadDword(&offset);
	buf.SeekPosition(AddrGtoL(offset));
	buf.ReadDword(&len);
	body.ReadFrom(&buf,len);
	return true;
}
#endif
CBoltSizeSection::CBoltSizeSection(DWORD zeroAddrOffset/*=0*/,char* buf/*=NULL*/,DWORD size/*=0*/,const char* version/*=NULL*/)
{
	InitBuffer(zeroAddrOffset,buf,size,version);
}
void CBoltSizeSection::InitBuffer(DWORD zeroAddrOffset/*=0*/,char* buf/*=NULL*/,DWORD size/*=0*/,const char* version/*=NULL*/)
{
	m_dwZeroAddrOffset=zeroAddrOffset;
	m_data=buf;
	m_dwBufSize=size;
	m_sVersion.Copy(version);
}
short CBoltSizeSection::GetDiameter()
{	//��˨ֱ��
	CBuffer buf(BufferPtr(),BufferLength());
	WORD wDiameter;
	buf.ReadWord(&wDiameter);
	return wDiameter;
}
short CBoltSizeSection::GetLenValid()
{	//��˨��Ч��
	CBuffer buf(BufferPtr(),BufferLength());
	buf.SeekPosition(2);
	short lenValid;
	buf.ReadWord(&lenValid);
	return lenValid;
}
short CBoltSizeSection::GetLenNoneThread()
{	//��˨�޿۳�
	CBuffer buf(BufferPtr(),BufferLength());
	buf.SeekPosition(4);
	short lenNoneThread;
	buf.ReadWord(&lenNoneThread);
	return lenNoneThread;
}
short CBoltSizeSection::GetMaxL0Limit()	//��˨ͨ������
{
	if(compareVersion(m_sVersion,"1.1")<0)
		return 0;
	CBuffer buf(BufferPtr(),BufferLength());
	buf.SeekPosition(6);
	short wMaxL0Limit;
	buf.ReadWord(&wMaxL0Limit);
	return wMaxL0Limit;
}
short CBoltSizeSection::GetMinL0Limit()	//��˨ͨ������
{
	if(compareVersion(m_sVersion,"1.1")<0)
		return 0;
	CBuffer buf(BufferPtr(),BufferLength());
	buf.SeekPosition(8);
	short wMinL0Limit;
	buf.ReadWord(&wMinL0Limit);
	return wMinL0Limit;
}
double CBoltSizeSection::GetTheoryWeight()
{	//��������(kg)
	CBuffer buf(BufferPtr(),BufferLength());
	if(compareVersion(m_sVersion,"1.1")>=0)
		buf.SeekPosition(10);
	else
		buf.SeekPosition(6);
	double theoryWeight;
	buf.ReadDouble(&theoryWeight);
	return theoryWeight;
}
short CBoltSizeSection::GetSpec(char* spec)	//��˨��������ַ�����ĩβ0��ֹ�ַ���
{
	CBuffer buf(BufferPtr(),BufferLength());
	if(compareVersion(m_sVersion,"1.1")>=0)
		buf.SeekPosition(18);
	else
		buf.SeekPosition(14);
	char sizestr[18]={0};
	buf.Read(sizestr,18);
	strcpy(spec,sizestr);
	return strlen(sizestr);
}
CBoltSection::CBoltSection(DWORD zeroAddrOffset/*=0*/,char* buf/*=NULL*/,DWORD size/*=0*/,const char* version/*=NULL*/)
{
	m_dwZeroAddrOffset=zeroAddrOffset;
	m_data=buf;
	m_dwBufSize=size;
	m_sVersion.Copy(version);
	if(m_data!=NULL&&compareVersion(version,"1.4")>=0)
	{	//��ȡ�ؽ���˨��
		WORD wBoltSeriesCount=BoltSeriesCount();
		CBuffer sectbuf(BufferPtr(),BufferLength());
		sectbuf.SeekPosition(1+wBoltSeriesCount*4);
		DWORD dwAnchorAddr=0,dwSolidSize=0;
		sectbuf.ReadDword(&dwAnchorAddr);
		sectbuf.SeekPosition(this->AddrGtoL(dwAnchorAddr));
		sectbuf.ReadWord(&anchorInfo.d);
		sectbuf.ReadWord(&anchorInfo.wiLe);	//�ؽ���˨��¶��(mm)
		sectbuf.ReadWord(&anchorInfo.wiLa);	//�ؽ���˨�޿۳�(mm) <��������
		sectbuf.ReadWord(&anchorInfo.wiWidth);	//�ؽ���˨���������
		sectbuf.ReadWord(&anchorInfo.wiThick);
		sectbuf.ReadWord(&anchorInfo.wiHoleD);	//�ؽ���˨��׾�
		sectbuf.ReadWord(&anchorInfo.wiBasePlateHoleD);	//�ؽ���˨���ӵ����彨��׾�ֵ
		sectbuf.Read(anchorInfo.szSizeSymbol,64);		//�������ֽڣ��̶����ȵĵؽ���˨������ַ�������ĩβ0��ֹ�ַ�����
		//��ȡ�ؽ���˨ʵ����Ϣ
		dwSolidSize=sectbuf.ReadDword();
		xAnchorSolid.nut.AttachBuffer(sectbuf.GetCursorBuffer(),dwSolidSize);
		sectbuf.Offset(dwSolidSize);
		dwSolidSize=sectbuf.ReadDword();
		xAnchorSolid.bolt.AttachBuffer(sectbuf.GetCursorBuffer(),dwSolidSize);
	}
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
#ifndef TIDCORE_EXPORTS
void CBoltSection::InitLsFamily(int i,CLsFamily* pFamily)
{
	CBoltSeries boltSeries=GetBoltSeriesAt(i);
	//֮ǰ�ĵ�Ȧ������ת��Ϊ�Ƿ񺬷��ɵ�Ȧ��������  wjh-2018.4.21
	pFamily->m_bAntiLoose=boltSeries.BoltWasherCount()>0;
	pFamily->m_bDualCap=boltSeries.BoltCapCount()>=2;
	int nRec=boltSeries.BoltSizeCount();
	StrCopy(pFamily->name,boltSeries.GetSeriesName(),17);
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
}
#endif
//���ʿ�
char CMaterialLibrarySection::BriefSymbolAt(int i)
{
	char* cursor=BufferPtr()+i*8;
	return *cursor;
}
int  CMaterialLibrarySection::NameCodeAt(int i,char* nameCode)	//�����ַ�������,��������NULL
{
	char* cursor=BufferPtr()+i*8+1;
	char name[8]={0};
	memcpy(name,cursor,7);
	if(nameCode)
		strcpy(nameCode,name);
	return strlen(name);
}
CMaterialLibrarySection CPartSection::GetMatLibrarySection()
{
	CBuffer buf(BufferPtr(),BufferLength());
	BYTE mat_n;
	buf.ReadByte(&mat_n);
	DWORD part_n;
	buf.ReadDword(&part_n);
	CMaterialLibrarySection material(mat_n,buf.GetCursorBuffer(),mat_n*8);
	material.SetBelongTidBuffer(BelongTidBuffer());
	return material;
}

UINT CPartSection::GetPartCount()
{
	UINT part_n;
	CBuffer buf(BufferPtr(),BufferLength());
	buf.SeekPosition(1);
	buf.ReadInteger(&part_n);
	return part_n;
}
//����
PART_INFO CPartSection::GetPartInfoByIndexId(int indexId)
{
	BYTE mat_n;
	DWORD part_n;
	CBuffer buf(BufferPtr(),BufferLength());
	buf.ReadByte(&mat_n);
	buf.ReadDword(&part_n);
	buf.Offset(mat_n*8);
	if(compareVersion(m_sVersion,"1.1")>=0)
		buf.Offset((indexId-1)*33);
	else
		buf.Offset((indexId-1)*32);
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
	if(compareVersion(m_sVersion,"1.1")>=0)
		buf.ReadByte(&part.cFuncType);
	else
		part.cFuncType=0;
	buf.ReadDword(&part.addrAttachInfo);
	//��ȡ������Ϣ
	buf.SeekPosition(AddrGtoL(part.addrAttachInfo));
	DWORD size;
	buf.ReadDword(&size);
	part.solid.AttachBuffer(buf.GetCursorBuffer(),size);
	buf.Offset(size);
	buf.ReadString(part.sPartNo,50);
	buf.ReadString(part.spec,50);
	buf.ReadString(part.sNotes,50);
	buf.ReadDword(&part.dwProcessBuffSize);	//��������������Ϣ�洢����
	part.processBuffBytes=buf.GetCursorBuffer();
	buf.Offset(size);		//����Ҫ���������Ϣ�������Ե��˲�����Ϣ
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
	if(compareVersion(m_sVersion,"1.1")>=0)
	{
		sectionBuf.Offset((indexId-1)*33);	//������indexId������Ϣ��
		partBuffer.Write(sectionBuf,29);	//��ȡ����������Ϣ
	}
	else
	{
		sectionBuf.Offset((indexId-1)*32);	//������indexId������Ϣ��
		partBuffer.Write(sectionBuf,28);	//��ȡ����������Ϣ
		partBuffer.WriteByte(0);	//���빹�����������ֽ�	wjh-2016.1.18
	}
	sectionBuf.ReadDword(&iOffset);
	sectionBuf.SeekPosition(AddrGtoL(iOffset));
	sectionBuf.ReadDword(&size);
	partBuffer.WriteDword(size);		//��ȡ����ʵ����Ϣ�ڴ��С
	partBuffer.Write(sectionBuf,size);	//��ȡ����ʵ����Ϣ
	CXhChar50 sText;
	sectionBuf.ReadString(sText,50);
	partBuffer.WriteString(sText);		//��ȡ�������
	sectionBuf.ReadString(sText,50);
	partBuffer.WriteString(sText);		//��ȡ�������
	sectionBuf.ReadString(sText,50);
	partBuffer.WriteString(sText);		//��ȡ��ע
	sectionBuf.ReadDword(&size);		
	partBuffer.WriteDword(size);		//��ȡ����������Ϣ�ڴ��С
	partBuffer.Write(sectionBuf,size);	//��ȡ����������Ϣ
}
DWORD CAssembleSection::BlockAssemblyCount()
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_count;
	buf.ReadDword(&block_count);
	return block_count;
}
WORD CAssembleSection::GetBlockIndexId(int indexId)	//��ȡָ������װ�����Ĺ�������������ʶ���
{
	CBuffer buf(BufferPtr(),BufferLength());
	buf.Offset(4+(indexId-1)*128);
	WORD wBlockIndexId=0;
	buf.ReadWord(&wBlockIndexId);
	return wBlockIndexId;
}
BLOCK_ASSEMBLY CAssembleSection::GetAssemblyByIndexId(int indexId)
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
CNodeSection CAssembleSection::NodeSection()
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_assembly_n,size;
	buf.ReadDword(&block_assembly_n);
	buf.Offset(block_assembly_n*128);
	if(compareVersion(this->m_sVersion,"1.4")>=0)
		buf.ReadDword(&size);	//�ڵ�װ����
	else
		size=0;
	return CNodeSection(m_dwZeroAddrOffset+buf.GetCursorPosition(),buf.GetCursorBuffer(),size,m_sVersion);
}
CBoltAssemblySection CAssembleSection::BoltSection()
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_assembly_n,size;
	buf.ReadDword(&block_assembly_n);
	buf.Offset(block_assembly_n*128);
	if(compareVersion(this->m_sVersion,"1.4")>=0)
	{	//�����ڵ�װ����
		buf.ReadDword(&size);
		buf.Offset(size);
	}
	buf.ReadDword(&size);
	return CBoltAssemblySection(m_dwZeroAddrOffset+buf.GetCursorPosition(),buf.GetCursorBuffer(),size,m_sVersion);
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
BOLT_ASSEMBLY CBoltAssemblySection::GetAssemblyByIndexId(int indexId,bool bTowerSpace/*=true*/)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_bolt_n,tower_bolt_n;
	buf.ReadDword(&block_bolt_n);
	buf.ReadDword(&tower_bolt_n);

	BOLT_ASSEMBLY assembly;
	if(bTowerSpace)
	{
		buf.Offset(block_bolt_n*64);//��������ģ�Ϳռ�����˨װ����Ϣ����
		buf.Offset((indexId-1)*92);	//������ǰ������ʶָ��ļ�¼
		assembly.bInBlockSpace=false;
		assembly.wBlockIndexId=0;
	}
	else
	{
		buf.Offset((indexId-1)*64);
		assembly.bInBlockSpace=true;
	}
	//��������
	buf.ReadByte(&assembly.cSeriesId);
	buf.ReadWord(&assembly.wIndexId);
	buf.ReadFloat(&assembly.grade);
	//��������
	buf.ReadPoint(assembly.origin);
	buf.ReadPoint(assembly.work_norm);
	buf.ReadWord(&assembly.wL0);
	buf.ReadByte(&assembly.cPropFlag);
	buf.ReadByte(&assembly.cDianQuanN);
	buf.ReadByte(&assembly.cDianQuanThick);
	buf.ReadWord(&assembly.wDianQuanOffset);
	if(!bTowerSpace)	//�����ռ�����˨װ������Ӧ�Ĳ���������ʶ
		buf.ReadWord(&assembly.wBlockIndexId);
	else //if(bTowerSpace)
	{					//����ģ�Ϳռ�����˨װ������Ӧ�Ĳ���������ʶ
		buf.ReadByte(&assembly.cStatFlag);
		buf.ReadDword(&assembly.dwSeg);
		buf.ReadByte(&assembly.cLegQuad);
		buf.Read(&assembly.cfgword,24);
		if(assembly.cStatFlag&0x80)	//��˨ͬʱ��������κţ��κ�ͳ�Ʒ�Χ���ַ�����ʽ�洢�ں��������ֽ���ָ��ַ��
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
		buf.Offset(block_bolt_n*64);//��������ģ�Ϳռ�����˨װ����Ϣ����
		buf.Offset((indexId-1)*92);	//������ǰ������ʶָ��ļ�¼
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
		buf.Offset(block_bolt_n*64);//��������ģ�Ϳռ�����˨װ����Ϣ����
		buf.Offset((indexId-1)*92);	//������ǰ������ʶָ��ļ�¼
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
		// ���λΪ����ʾ��˨���������������ֽ�������κţ�
		// ���λΪ����ʾ��˨ͬʱ��������κţ��κ�ͳ�Ʒ�Χ���ַ�����ʽ�洢�ں��������ֽ���ָ��ַ�У�
		// �θ�λΪ����ʾ�Ƿ�Ϊ��������˨��
		// ����ͣ�λ��ʾ�������ȵ����޺ţ���Ϊ����ʾ�����������塣
		buf.ReadByte(&cFlag);
		buf.ReadDword(&dwSeg);
		buf.ReadByte(&cQuad);
		buf.Read(&cfgword,24);
		if(cFlag&0x80)		//��˨ͬʱ��������κ�
		{	//
			buf.SeekPosition(AddrGtoL(dwSeg));
			buf.ReadString(statSetStr,50);
#ifndef TIDCORE_EXPORTS
			ATOM_LIST<SEGI> segNoList;
			if(GetSortedSegNoList(statSetStr,segNoList)>0)
				dwSeg=segNoList.GetFirst()->iSeg;
#else
			CXhSimpleList<SEGI> listSegI;
			if(GetSortedSegNoList(statSetStr,listSegI)>0)
				dwSeg=listSegI.EnumObjectFirst()->iSeg;
#endif
			else 
				dwSeg=0;
		}
		assemblyBuf.WriteDword(dwSeg);
		assemblyBuf.WriteString(statSetStr);
		assemblyBuf.WriteByte(cQuad);	//�Ȳ�������������
		assemblyBuf.Write(&cfgword,24);	//��ĺ�
	}
	else
	{
		buf.ReadWord(&wBlockIndexId);
		assemblyBuf.WriteWord(wBlockIndexId);
	}
}
void CNodeSection::GetNodeByIndexId(int indexId,bool bTowerSpace,NODE_ASSEMBLY &node)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_node_n,tower_node_n;
	buf.ReadDword(&block_node_n);
	buf.ReadDword(&tower_node_n);

	UINT uiBlkRecLength=33,uiTowerRecLength=56;
	if(bTowerSpace)
	{
		node.bInBlockSpace=false;
		node.wBlockIndexId=0;
		buf.Offset(block_node_n*uiBlkRecLength);	//��������ģ�Ϳռ��й���װ����Ϣ����
		buf.Offset((indexId-1)*uiTowerRecLength);	//������ǰ������ʶָ��ļ�¼
		buf.ReadInteger(&node.uiPointI);
		buf.ReadByte(&node.cLegQuad);
		buf.Read(node.szLayerSymbol,3);
		buf.Read(&node.cfgword,24);
	}
	else
	{
		node.bInBlockSpace=true;
		node.cLegQuad=0;
		buf.Offset((indexId-1)*uiBlkRecLength);	//������ǰ������ʶָ��ļ�¼
		buf.ReadInteger(&node.uiPointI);
		buf.Read(node.szLayerSymbol,3);
	}
	buf.ReadPoint(node.xPosition);
}
DWORD CNodeSection::NodeCount(bool bTowerSpace/*=true*/)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD assemble_n;
	if(bTowerSpace)
		buf.Offset(4);
	buf.ReadDword(&assemble_n);
	return assemble_n;
}
PART_ASSEMBLY::PART_ASSEMBLY()
{
	bInBlockSpace=false;
	wBlockIndexId=0;
	cLegQuad=0;
	szLayerSymbol[0]=szLayerSymbol[1]=szLayerSymbol[2]=szLayerSymbol[3]=0;
	uiStartPointI=uiEndPointI=xGroupAngle.uidGroupAngle=0;
	dwIndexId=dwAddrBriefLine=0;
	bIsRod=false;
	cStartCoordType=cEndCoordType=0;
}
PART_ASSEMBLY::PART_ASSEMBLY(PART_ASSEMBLY &srcPart)
{
	CloneFrom(srcPart);
}
PART_ASSEMBLY& PART_ASSEMBLY::operator=(PART_ASSEMBLY &srcPart)
{
	CloneFrom(srcPart);
	return *this;
}
void PART_ASSEMBLY::CloneFrom(PART_ASSEMBLY &srcPart)
{
	bInBlockSpace=srcPart.bInBlockSpace;	//�Ƿ�����װ�䲿���ռ�۹������ԣ�
	wBlockIndexId=srcPart.wBlockIndexId;//���������ı�ʶ�����ţ۲���װ�乹�����ԣ�
	cLegQuad=srcPart.cLegQuad;		//�������ȵ����޺ţ���Ϊ����ʾ���������۸���װ�乹�����ԣ�
	cfgword=srcPart.cfgword;	//����ģ�Ϳռ����������ֹ������ߵ���ĺţ۸���װ�乹�����ԣ�
	dwIndexId=srcPart.dwIndexId;	//�ڹ�����Ϣ�����еı�ʶ�����ţ۹������ԣ�
	uiStartPointI=srcPart.uiStartPointI;
	uiEndPointI=srcPart.uiEndPointI;
	memcpy(szLayerSymbol,srcPart.szLayerSymbol,4);
	xGroupAngle=srcPart.xGroupAngle;
	dwAddrBriefLine=srcPart.dwAddrBriefLine;
	bIsRod=srcPart.bIsRod;
	cStartCoordType=srcPart.cStartCoordType;
	cEndCoordType=srcPart.cEndCoordType;
	startPt=srcPart.startPt;
	endPt=srcPart.endPt;
	acs=srcPart.acs;			//�ڹ���ģ�Ϳռ��ڵ�װ������ϵ����۹������ԣ�
	hashBoltIndexByHoleId.Empty();
	for(DWORD *pBoltIndexId=srcPart.hashBoltIndexByHoleId.GetFirst();pBoltIndexId;pBoltIndexId=srcPart.hashBoltIndexByHoleId.GetNext())
		hashBoltIndexByHoleId.SetValue(srcPart.hashBoltIndexByHoleId.GetCursorKey(),*pBoltIndexId);
}
CPartAssemblySection CAssembleSection::PartSection()
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_assembly_n,size;
	buf.ReadDword(&block_assembly_n);
	buf.Offset(block_assembly_n*128);
	if(compareVersion(this->m_sVersion,"1.4")>=0)
	{	//�����ڵ�װ����
		buf.ReadDword(&size);
		buf.Offset(size);
	}
	buf.ReadDword(&size);
	buf.Offset(size);	//������˨װ����
	buf.ReadDword(&size);
	CPartAssemblySection section(m_dwZeroAddrOffset+buf.GetCursorPosition(),buf.GetCursorBuffer(),size,m_sVersion);
	section.SetBelongTidBuffer(BelongTidBuffer());
	return section;
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
void CPartAssemblySection::GetAssemblyByIndexId(int indexId,bool bTowerSpace,PART_ASSEMBLY &assembly)
{
	CBuffer buf(BufferPtr(),BufferLength());
	DWORD block_part_n,tower_part_n,rod_n;
	buf.ReadDword(&block_part_n);
	buf.ReadDword(&tower_part_n);
	if(compareVersion(m_pTidBuff->Version(),"1.1")>=0)
		buf.ReadDword(&rod_n);	//�����˼���������

	bool bAfterVer14=compareVersion(m_pTidBuff->Version(),"1.4")>=0;
	UINT uiBlkRecLength=!bAfterVer14?110:117;
	UINT uiRecLength=!bAfterVer14?133:140;
	if(bTowerSpace)
	{
		assembly.bInBlockSpace=false;
		assembly.wBlockIndexId=0;
		if(compareVersion(m_pTidBuff->Version(),"1.1")>=0)
		{	//���Ӽ��ߴ洢λ��4B����λӰ���洢λ��4B wjh-2016.1.18
			buf.Offset(block_part_n*uiBlkRecLength);	//��������ģ�Ϳռ��й���װ����Ϣ����
			buf.Offset((indexId-1)*uiRecLength);	//������ǰ������ʶָ��ļ�¼
			buf.ReadDword(&assembly.dwIndexId);
			if(bAfterVer14)
			{
				buf.Read(assembly.szLayerSymbol,3);
				buf.ReadInteger(&assembly.xGroupAngle.uidGroupAngle);
			}
			buf.ReadDword(&assembly.dwAddrBriefLine);
		}
		else
		{
			buf.Offset(block_part_n*102);	//��������ģ�Ϳռ��й���װ����Ϣ����
			buf.Offset((indexId-1)*125);	//������ǰ������ʶָ��ļ�¼
			buf.ReadDword(&assembly.dwIndexId);
		}
		buf.ReadByte(&assembly.cLegQuad);
		buf.Read(&assembly.cfgword,24);
	}
	else
	{
		assembly.bInBlockSpace=true;
		assembly.cLegQuad=0;
		if(compareVersion(m_pTidBuff->Version(),"1.1")>=0)
		{
			buf.Offset((indexId-1)*uiBlkRecLength);	//������ǰ������ʶָ��ļ�¼
			buf.ReadDword(&assembly.dwIndexId);
			if(bAfterVer14)
			{
				buf.Read(assembly.szLayerSymbol,3);
				buf.ReadInteger(&assembly.xGroupAngle.uidGroupAngle);
			}
			buf.ReadDword(&assembly.dwAddrBriefLine);
		}
		else
		{
			buf.Offset((indexId-1)*106);	//������ǰ������ʶָ��ļ�¼
			buf.ReadDword(&assembly.dwIndexId);
		}
		buf.ReadWord(&assembly.wBlockIndexId);
	}
	buf.ReadPoint(assembly.acs.origin);
	buf.ReadPoint(assembly.acs.axis_x);
	buf.ReadPoint(assembly.acs.axis_y);
	buf.ReadPoint(assembly.acs.axis_z);
	//��ȡװ���¼��˨����Ϣ��ַ
	if(compareVersion(m_pTidBuff->Version(),"1.1")>=0)
	{
		DWORD addrHoleInfo=0;
		buf.ReadDword(&addrHoleInfo);
		if(addrHoleInfo>0)
		{	//��ȡ������Ϣ
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
		//��ȡ�˼���������
		if(assembly.dwAddrBriefLine>0)
		{
			buf.SeekPosition(AddrGtoL(assembly.dwAddrBriefLine));
			buf.ReadPoint(assembly.startPt);
			buf.ReadPoint(assembly.endPt);
			if(bAfterVer14)//compareVersion(m_pTidBuff->Version(),"1.4")>=0)
			{
				buf.ReadInteger(&assembly.uiStartPointI);
				buf.ReadInteger(&assembly.uiEndPointI);
			}
			if(compareVersion(m_pTidBuff->Version(),"1.2")>=0)
			{	//��ȡ�˼�ʼ�ն˽ڵ����ͣ��Ա����ģ���п����Զ���ȡģ�ͳߴ��ע wht 16-12-01
				buf.ReadByte(&assembly.cStartCoordType);
				buf.ReadByte(&assembly.cEndCoordType);
			}
			assembly.bIsRod=true;
		}
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
	if(BufferLength()<=0)	//û�и�����Ϣ
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
	CModuleSection section(GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
	section.SetBelongTidBuffer(this);
	return section;
}
CSubLegFoundationSection CTIDBuffer::SubLegFoundationSection()
{
	DWORD pos,sect_len;
	BYTE cnHeightCount=ModuleSection().GetModuleCount();
	SeekPosition(AddrLtoG(4));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	CSubLegFoundationSection section(cnHeightCount,GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
	section.SetBelongTidBuffer(this);
	return section;
}
CWireNodeSection CTIDBuffer::WireNodeSection()
{
	DWORD pos,sect_len;
	SeekPosition(AddrLtoG(8));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	CWireNodeSection section(GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
	section.SetBelongTidBuffer(this);
	return section;
}
CProjectInfoSection CTIDBuffer::ProjectInfoSection()
{
	DWORD pos,sect_len;
	SeekPosition(AddrLtoG(12));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	CProjectInfoSection section(GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
	section.SetBelongTidBuffer(this);
	return section;
}
CBlockSection CTIDBuffer::BlockSection()
{
	DWORD pos,sect_len;
	if(compareVersion(m_sVersion,"1.4")>=0)
		SeekPosition(AddrLtoG(4+12));
	else
		SeekPosition(AddrLtoG(4));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	CBlockSection section(GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
	section.SetBelongTidBuffer(this);
	return section;
}
CBoltSection CTIDBuffer::BoltSection()
{
	DWORD pos,sect_len;
	if(compareVersion(m_sVersion,"1.4")>=0)
		SeekPosition(AddrLtoG(8+12));
	else
		SeekPosition(AddrLtoG(8));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	CBoltSection section(GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
	section.SetBelongTidBuffer(this);
	return section;
}
CPartSection CTIDBuffer::PartSection()
{
	DWORD pos,sect_len;
	if(compareVersion(m_sVersion,"1.4")>=0)
		SeekPosition(AddrLtoG(12+12));
	else
		SeekPosition(AddrLtoG(12));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	CPartSection section(GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
	section.SetBelongTidBuffer(this);
	return section;
}
CAssembleSection CTIDBuffer::AssembleSection()
{
	DWORD pos,sect_len;
	if(compareVersion(m_sVersion,"1.4")>=0)
		SeekPosition(AddrLtoG(16+12));
	else
		SeekPosition(AddrLtoG(16));
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	CAssembleSection section(GetCursorPosition(),GetCursorBuffer(),sect_len,m_sVersion);
	section.SetBelongTidBuffer(this);
	return section;
}

CAttachDataSection CTIDBuffer::AttachDataSection()
{
	DWORD pos,sect_len;
	SeekPosition(AddrLtoG(16));	//�������ݷ��������һ������֮�󣬴˴��������һ������������ת
	ReadDword(&pos);
	SeekPosition(pos);
	ReadDword(&sect_len);
	SeekPosition(pos+sect_len+4);	//+4Ϊsect_len���ڵ�4���ֽ�
	DWORD attach_sect_len=GetLength()-GetCursorPosition();
	CAttachDataSection section(GetCursorPosition(),GetCursorBuffer(),attach_sect_len,m_sVersion);
	section.SetBelongTidBuffer(this);
	return section;
}
bool CTIDBuffer::InitBuffer(const char* srcBuf, DWORD buf_size)
{
	ClearBuffer();
	Write(srcBuf,buf_size);
	SeekToBegin();
	CXhChar50 sDocType;
	ReadString(sDocType,50);
	if(strstr(sDocType,"Xerofox tower solid data media file")==NULL)
	{
		//throw "��Ԥ�ڵ��ļ���ʽ���ļ���ʧ��!";
		logerr.Log("��Ԥ�ڵ��ļ���ʽ���ļ���ʧ��!\n");
		return false;
	}
	char* key=strtok(sDocType,"-");
	char* ver=strtok(NULL,"-");
	double version=1000;
	if(ver!=NULL)
		version=atof(ver);
	if(version>1.4)
	{
		logerr.Log("���ļ��汾��̫��,����ֱ�Ӷ�ȡ���ļ�\n");
		return false;
	}
	m_sVersion.Copy(ver);
	ReadString(m_sTowerType,50);
	ReadPoint(mcs.axis_x);
	ReadPoint(mcs.axis_y);
	ReadPoint(mcs.axis_z);
	SetZeroAddrOffset(GetCursorPosition());
	return true;
}
