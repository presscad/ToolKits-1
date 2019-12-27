#include "stdafx.h"
#include "ProcessPart.h"
#include "ArrayList.h"
#include "SortFunc.h"
#include "LogFile.h"
#ifndef _LEGACY_LICENSE
#include "XhLdsLm.h"
#else
#include "Lic.h"
#endif

const DWORD BOLT_INFO::TWO_CAP   =0X00000001;	//�Ŷ���˨��ʶλ
const DWORD BOLT_INFO::ANTI_THEFT=0X00000002;	//������˨��ʶλ
const DWORD BOLT_INFO::ANTI_LOOSE=0X00000004;	//�����ɵ�Ȧ��ʶλ
const DWORD BOLT_INFO::FOOT_NAIL =0X00000008;	//�Ŷ���˨��ʶλ
const DWORD BOLT_INFO::REVERSE_NORM =0X00000010;//��˨�����ʶλ
const BYTE byteConstArr[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

const DWORD HASHTABLESIZE = 500;
const DWORD STATUSHASHTABLESIZE = 50;
////////////////////////////////////////////////////////////////////
//
//#ifdef _LEGACY_LICENSE
//BOOL Standarized(f3dPoint& v)
//{
//	return normalize(v);
//}
//#endif

PROFILE_VER::PROFILE_VER(f3dPoint point)
{
	keyId=0;
	type=0;
	manu_space=0;
	sector_angle=radius=0;
	m_bWeldEdge=m_bRollEdge=false;
	roll_edge_offset_dist=0;
	local_point_y=0;
	local_point_vec=0;
	vertex=point;
#ifdef __TAP_CONTEXT_
	m_bUpVertex=false;
	rayVecX=rayVecY=0;
#endif
}
PROFILE_VER::PROFILE_VER(double x/*=0*/,double y/*=0*/,double z/*=0*/)
{
	keyId=0;
	type=0;
	manu_space=0;
	sector_angle=radius=0;
	m_bWeldEdge=m_bRollEdge=false;
	roll_edge_offset_dist=0;
	local_point_y=0;
	local_point_vec=0;
	vertex.Set(x,y,z);
#ifdef __TAP_CONTEXT_
	m_bUpVertex=false;
	rayVecX=rayVecY=0;
#endif
}

BOOL PROFILE_VER::CloneVertex(PROFILE_VER *pNewVertex)
{
	if(pNewVertex==NULL)
		return FALSE;
	type=pNewVertex->type;	
	m_bWeldEdge=pNewVertex->m_bWeldEdge;
	m_bRollEdge=pNewVertex->m_bRollEdge;
	roll_edge_offset_dist=pNewVertex->roll_edge_offset_dist;
	radius=pNewVertex->radius;
	sector_angle=pNewVertex->sector_angle;
	manu_space=pNewVertex->manu_space;
	vertex=pNewVertex->vertex;
	center=pNewVertex->center;
	column_norm=pNewVertex->column_norm;
	work_norm=pNewVertex->work_norm;
	local_point_y=pNewVertex->local_point_y;
	local_point_vec=pNewVertex->local_point_vec;
	return TRUE;
}
void PROFILE_VER::RetrieveArcLine(f3dArcLine &arcline,f3dPoint endPoint,UCS_STRU *ucs)
{
	if(vertex.IsEqual(endPoint))//��ֹ�������غ�����Բ���׳��쳣
		arcline=f3dArcLine(f3dLine(vertex,endPoint));
	else if(type==1)	//��ֱͨ��
		arcline.CreateMethod2(vertex,endPoint,f3dPoint(0,0,1),0);
	else if(type==2)	//ָ��Բ���н�
	{
		int sign=1;
		if(sector_angle<0)
			sign*=-1;
		if(work_norm.z<0)
			sign*=-1;
		arcline.CreateMethod2(vertex,endPoint,f3dPoint(0,0,sign),fabs(sector_angle));
	}
	else if(type==3)	//��Բ��
	{
		int sign=1;
		if(radius<0)
			sign=-1;
		if(work_norm.z<0)
			sign*=-1;
		f3dPoint columnNorm=column_norm;
		f3dPoint centerTemp=center;
		if(ucs!=NULL)
		{
			coord_trans(centerTemp,*ucs,FALSE);
			vector_trans(columnNorm,*ucs,FALSE);
		}
		arcline.CreateEllipse(centerTemp,vertex,endPoint,columnNorm,f3dPoint(0,0,sign),fabs(radius));
	}
	else if(type==4)	//ָ��Բ���뾶
	{
		int sign=radius<0?-1:1;
		arcline.CreateMethod3(vertex,endPoint,f3dPoint(0,0,sign),sign*radius,center);
	}
}
#ifdef __PROPERTYLIST_UI_
IMPLEMENT_PROP_FUNC(PROFILE_VER)
void PROFILE_VER::InitPropHashtable()
{
	int id=1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"�����߻�����Ϣ"));
	AddPropItem("id",PROPLIST_ITEM(id++,"ID"));
	AddPropItem("cVertexType",PROPLIST_ITEM(id++,"��������","�����������","0.��׼�涥��|1.��2�������|2.1-2�������|3.��3�������|4.1-3�������"));
	AddPropItem("type",PROPLIST_ITEM(id++,"����","����������","1.��ֱͨ��|2.Բ��|3.��Բ��|"));
	AddPropItem("sector_angle",PROPLIST_ITEM(id++,"Բ�����ν�"));
	AddPropItem("radius",PROPLIST_ITEM(id++,"�̰��᳤��","��Բ���̰��᳤�ȣ���ԭʼ����Բ����뾶��"));
	AddPropItem("center",PROPLIST_ITEM(id++,"Բ��λ��","��Բ��Բ��λ�ã��̽�����ϵ��������꣩"));
	AddPropItem("center.x",PROPLIST_ITEM(id++,"X","��ԲԲ��λ��X�������"));
	AddPropItem("center.y",PROPLIST_ITEM(id++,"X","��ԲԲ��λ��X�������"));
	AddPropItem("column_norm",PROPLIST_ITEM(id++,"��������","��Բ��ԭʼ�����������򣨹̽�����ϵ����Է���"));
	AddPropItem("column_norm.x",PROPLIST_ITEM(id++,"X","��Բ��ԭʼ������������X�������"));
	AddPropItem("column_norm.y",PROPLIST_ITEM(id++,"Y","��Բ��ԭʼ������������Y�������"));
	AddPropItem("column_norm.z",PROPLIST_ITEM(id++,"Z","��Բ��ԭʼ������������Z�������"));
	AddPropItem("work_norm",PROPLIST_ITEM(id++,"Բ�����߷���"));
	AddPropItem("work_norm.x",PROPLIST_ITEM(id++,"X","Բ�����߷���X�������"));
	AddPropItem("work_norm.y",PROPLIST_ITEM(id++,"Y","Բ�����߷���Y�������"));
	AddPropItem("work_norm.z",PROPLIST_ITEM(id++,"Z","Բ�����߷���Z�������"));
	AddPropItem("edgeType",PROPLIST_ITEM(id++,"�߹�������","","0.��ͨ��|1.���ӱ�|2.���"));
	AddPropItem("roll_edge_offset_dist",PROPLIST_ITEM(id++,"���ƫ�ƾ���"));
	AddPropItem("manu_space",PROPLIST_ITEM(id++,"��߸߶�"));
	AddPropItem("vertex",PROPLIST_ITEM(id++,"ʼ�˵�"));
	AddPropItem("vertex.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("vertex.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("endVertex",PROPLIST_ITEM(id++,"�ն˵�"));
	AddPropItem("endVertex.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("endVertex.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("local_point_y",PROPLIST_ITEM(id++,"�Ի��λ��"));
	AddPropItem("local_point_vec",PROPLIST_ITEM(id++,"�Ի������",
		"0.��ʾ�޶Ի�㣬1.��ʾ�Ի�������ע50-->100�ķ����뺸�Ӹ��˼���start-->end������ͬ��2.��ʾ�෴",
		"0.��|1.��˼�������ͬ|2.��˼������෴"));
	AddPropItem("vertexInMcs", PROPLIST_ITEM(id++, "��������","�ڵ�ǰ�ӹ�����ϵ�е�����ֵ"));
	AddPropItem("vertexInMcs.x", PROPLIST_ITEM(id++, "X"));
	AddPropItem("vertexInMcs.y", PROPLIST_ITEM(id++, "Y"));
}
int PROFILE_VER::GetPropValueStr(long itemId, char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(GetPropID("id")==itemId)
		sText.Printf("%d",hiberId.HiberDownId(2));
	else if(GetPropID("cVertexType")==itemId)
	{
		if(vertex.feature==2)
			sprintf(sText,"1.��2�������");
		else if(vertex.feature==12)
			sprintf(sText,"2.1-2�������");
		else if(vertex.feature==3)
			sprintf(sText,"3.��3�������");
		else if(vertex.feature ==13)
			sprintf(sText,"4.1-3�������");
		else //if(vertex.feature==1)
			sprintf(sText,"0.��׼�涥��");
	}
	else if(GetPropID("type")==itemId)
	{
		if(type==1)
			sText.Copy("1.��ֱͨ��");
		else if(type==2)
			sText.Copy("2.Բ��");
		else if(type==3)
			sText.Copy("3.��Բ��");
	}
	else if(GetPropID("sector_angle")==itemId)
	{
		double angle=sector_angle*DEGTORAD_COEF;
		sText.Printf("%f",angle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("radius")==itemId)
	{
		sText.Printf("%f",radius);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("center")==itemId)
		sText.ConvertFromPoint(center,2);
	else if(GetPropID("center.x")==itemId)
	{
		sText.Printf("%f",center.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("center.y")==itemId)
	{
		sText.Printf("%f",center.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("column_norm")==itemId)
		sText.ConvertFromPoint(column_norm,3);
	else if(GetPropID("column_norm.x")==itemId)
	{
		sText.Printf("%f",column_norm.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("column_norm.y")==itemId)
	{
		sText.Printf("%f",column_norm.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("column_norm.z")==itemId)
	{
		sText.Printf("%f",column_norm.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("work_norm")==itemId)
		sText.ConvertFromPoint(work_norm,3);
	else if(GetPropID("work_norm.x")==itemId)
	{
		sText.Printf("%f",work_norm.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("work_norm.y")==itemId)
	{
		sText.Printf("%f",work_norm.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("work_norm.z")==itemId)
	{
		sText.Printf("%f",work_norm.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("edgeType")==itemId)
	{
		if(manu_space>0)	//��߸߶Ȼ򺸷�ӹ���϶
		{
			if(m_bRollEdge)
				sText.Copy("2.���");
			else if(m_bWeldEdge)
				sText.Copy("1.���ӱ�");
		}
		else 
			sText.Copy("0.��ͨ��");
	}
	else if(GetPropID("roll_edge_offset_dist")==itemId)
		sText.Printf("%d",roll_edge_offset_dist);
	else if(GetPropID("manu_space")==itemId)
		sText.Printf("%d",manu_space);
	else if(GetPropID("vertex")==itemId)
		sText.ConvertFromPoint(vertex,2);
	else if(GetPropID("vertex.x")==itemId)
	{
		sText.Printf("%f",vertex.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("vertex.y")==itemId)
	{
		sText.Printf("%f",vertex.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("local_point_y")==itemId)
	{
		sText.Printf("%f",local_point_y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("local_point_vec")==itemId)
	{
		if(local_point_vec==1)
			sText.Copy("1.��˼�������ͬ");
		else if(local_point_vec==2)
			sText.Copy("2.��˼������෴");
		else //if(local_point_vec==0)
			sText.Copy("0.��");
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return sText.GetLength();
}
#endif
/////////////////////////////////////////////////////////////////////
// 
BOLT_INFO::BOLT_INFO()
{
	keyId=0;				
	bolt_d=0;
	hole_d_increment=0;		
	waistLen=0;
	posX=posY=0;
	dwRayNo=0;
	cFaceNo=1;		//���չ���Ϊ��ʱ��Ĭ��Ϊ�����
	feature=0;
	cFuncType=0;
}

#ifdef __PROPERTYLIST_UI_
IMPLEMENT_PROP_FUNC(BOLT_INFO)
void BOLT_INFO::InitPropHashtable()
{
	int id=1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"��˨�׻�����Ϣ"));
	AddPropItem("id",PROPLIST_ITEM(id++,"ID"));
	AddPropItem("bolt_d",PROPLIST_ITEM(id++,"ֱ��","","12|16|20|24|30|"));
	AddPropItem("hole_d_increment",PROPLIST_ITEM(id++,"�׾�����"));
	AddPropItem("cFuncType",PROPLIST_ITEM(id++,"��������","","0.���ӿ�|1.�Ŷ���|2.���߿�|3.�ӵؿ�|4.װ���|5.���ƿ�|6.������"));
	AddPropItem("waistLen",PROPLIST_ITEM(id++,"��Բ����"));	
	AddPropItem("waistVec",PROPLIST_ITEM(id++,"��Բ����"));
	AddPropItem("waistVec.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("waistVec.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("pos",PROPLIST_ITEM(id++,"��λ"));
	AddPropItem("posX",PROPLIST_ITEM(id++,"X"));
	AddPropItem("posY",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("dwRayNo",PROPLIST_ITEM(id++,"���ߺ�"));
	AddPropItem("m_dwFlag",PROPLIST_ITEM(id++,"����"));
}
CXhChar100 GetRayNoStr(DWORD dwRayNo)
{
	CXhChar100 sRayNo;
	for(int i=0;i<32;i++)
	{
		if(dwRayNo&GetSingleWord(i+1))
		{
			CXhChar50 sText("%d",i+1);
			if(sRayNo.GetLength()>0)
				sRayNo.Append(",");
			sRayNo.Append(sText);
		}
	}
	return sRayNo;
}
int BOLT_INFO::GetPropValueStr(long itemId, char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(GetPropID("id")==itemId)
		sText.Printf("%d",hiberId.HiberDownId(2));
	else if (GetPropID("bolt_d") == itemId)
	{
		sText.Printf("%f", bolt_d);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("hole_d_increment")==itemId)
	{
		sText.Printf("%f",hole_d_increment);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cFuncType")==itemId)
	{
		if(cFuncType==BOLT_INFO::FUNC_COMMON)
			sText.Copy("0.���ӿ�");
		else if(cFuncType==BOLT_INFO::FUNC_FOOTNAIL)
			sText.Copy("1.�Ŷ���");
		else if(cFuncType==BOLT_INFO::FUNC_WIREHOLE)
			sText.Copy("2.���߿�");
		else if(cFuncType==BOLT_INFO::FUNC_EARTHHOLE)
			sText.Copy("3.�ӵؿ�");
		else if(cFuncType==BOLT_INFO::FUNC_SETUPHOLE)
			sText.Copy("4.װ���");
		else if(cFuncType==BOLT_INFO::FUNC_BRANDHOLE)
			sText.Copy("5.���ƿ�");
		else if(cFuncType==BOLT_INFO::FUNC_WATERHOLE)
			sText.Copy("6.������");
		else if(cFuncType==BOLT_INFO::FUNC_FOOTNAIL2)
			sText.Copy("7.���׽Ŷ���");
	}
	else if(GetPropID("waistLen")==itemId)
		sText.Printf("%d",waistLen);
	else if(GetPropID("waistVec")==itemId)
		sText.ConvertFromPoint(waistVec,2);
	else if(GetPropID("waistVec.x")==itemId)
	{
		sText.Printf("%f",waistVec.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("waistVec.y")==itemId)
	{
		sText.Printf("%f",waistVec.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("pos")==itemId)
		sText.ConvertFromPoint(f3dPoint(posX,posY),3);
	else if(GetPropID("posX")==itemId)
	{
		sText.Printf("%f",posX);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("posY")==itemId)
	{
		sText.Printf("%f",posY);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("dwRayNo")==itemId)
		sText=GetRayNoStr(dwRayNo);
	else if(GetPropID("m_dwFlag")==itemId)
	{
		sText.Printf("%d",m_dwFlag);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return sText.GetLength();
}
#endif
BOOL BOLT_INFO::CloneBolt(BOLT_INFO *pNewBoltInfo)
{
	if(pNewBoltInfo==NULL)
		return false;
	bolt_d=pNewBoltInfo->bolt_d;
	hole_d_increment=pNewBoltInfo->hole_d_increment;
	waistLen=pNewBoltInfo->waistLen;
	waistVec=pNewBoltInfo->waistVec;
	posX=pNewBoltInfo->posX;
	posY=pNewBoltInfo->posY;
	dwRayNo=pNewBoltInfo->dwRayNo;
	cFuncType=pNewBoltInfo->cFuncType;
	m_dwFlag=pNewBoltInfo->m_dwFlag;
	return true;
}
void BOLT_INFO::FromBuffer(CBuffer &buffer,long version,int buf_type)
{
	buffer.ReadFloat(&posX);				//��˨X����λ��
	buffer.ReadFloat(&posY);				//��˨Y����λ��
	buffer.ReadWord(&_bolt_d);				//��˨ֱ��
	buffer.ReadWord(&waistLen);				//��Բ������
	buffer.ReadDword(&dwRayNo);				//��˨�������ߺ�
	buffer.ReadPoint(waistVec);				//��Բ�׷���
	buffer.ReadByte(&cFuncType);			//��˨�׹�������
	if( (buf_type==CProcessPart::PPI_BUFFER && version>=1000001)||
		(buf_type==CProcessPart::TID_BUFFER && version>=1000001)||
		(buf_type==CProcessPart::TAP_BUFFER && (version==0||version>=1000500)))
		buffer.ReadFloat(&hole_d_increment);
}
void BOLT_INFO::ToBuffer(CBuffer &buffer,long version,int buf_type)
{
	buffer.WriteFloat(posX);				//��˨X����λ��
	buffer.WriteFloat(posY);				//��˨Y����λ��
	buffer.WriteWord(_bolt_d);				//��˨ֱ��
	buffer.WriteWord(waistLen);				//��Բ������
	buffer.WriteDword(dwRayNo);				//��˨�������ߺ�
	buffer.WritePoint(waistVec);			//��Բ�׷���
	buffer.WriteByte(cFuncType);			//��˨�׹�������
	if( (buf_type==CProcessPart::PPI_BUFFER && version>=1000001)||
		(buf_type==CProcessPart::TID_BUFFER && version>=1000001)||
		(buf_type==CProcessPart::TAP_BUFFER && (version==0||version>=1000400)))
		buffer.WriteFloat(hole_d_increment);	//�׾�����
}
//////////////////////////////////////////////////////////////////////////
// ASSEMBLEY_POS
//////////////////////////////////////////////////////////////////////////
#ifdef __TAP_CONTEXT_
ASSEMBLEY_POS::ASSEMBLEY_POS()
{
	m_dwAssemblePlace=1;
	m_dwBlockIndex=0;
	cLegQuad=0;
	acs.origin.Set();
	acs.axis_x.Set();
	acs.axis_y.Set();
	acs.axis_z.Set();
	m_dwInitPlace=0;
}
ASSEMBLEY_POS::ASSEMBLEY_POS(const ASSEMBLEY_POS &srcParam)
{
	CloneFrom(srcParam);
}
ASSEMBLEY_POS& ASSEMBLEY_POS::operator=(const ASSEMBLEY_POS &srcParam)
{
	CloneFrom(srcParam);
	return *this;
}
void ASSEMBLEY_POS::ToBuffer(CBuffer &buffer,long version/*=0*/,int doc_type/*=0*/)
{
	buffer.WriteDword(m_dwAssemblePlace);
	buffer.WriteDword(m_dwBlockIndex);
	buffer.WritePoint(acs.origin);
	buffer.WritePoint(acs.axis_x);
	buffer.WritePoint(acs.axis_y);
	buffer.WritePoint(acs.axis_z);
	if(version==0||doc_type==1||(doc_type==0&&version>=1000100))
	{
		buffer.WriteBoolean(m_bIsRod);
		if(m_bIsRod)
		{
			buffer.WritePoint(startPos);
			buffer.WritePoint(endPos);
			if(doc_type==0&&(version==0||version>=1000500))
			{
				buffer.WriteByte(0);//cStartCoordType);
				buffer.WriteByte(0);//cEndCoordType);
			}
		}
	}
	if((version==0||doc_type==1||(doc_type==0&&version>=1000200))&&m_dwBlockIndex==0)
	{
		buffer.WriteByte(cLegQuad);
		buffer.Write(&cfgword,24);
	}
}
void ASSEMBLEY_POS::FromBuffer(CBuffer &buffer,long version/*=0*/,int doc_type/*=0*/)
{
	buffer.ReadDword(&m_dwAssemblePlace);
	buffer.ReadDword(&m_dwBlockIndex);
	buffer.ReadPoint(acs.origin);
	buffer.ReadPoint(acs.axis_x);
	buffer.ReadPoint(acs.axis_y);
	buffer.ReadPoint(acs.axis_z);
	if(version==0||doc_type==1||(doc_type==0&&version>=1000100))
	{	//TAPģ��
		buffer.ReadBoolean(&m_bIsRod);
		if(m_bIsRod)
		{
			buffer.ReadPoint(startPos);
			buffer.ReadPoint(endPos);
			//if(doc_type==0&&(version==NULL||version>=1000500))
			{	//������Ϣȷ���Ƕ������ԣ���Ϊ�˱��ְ汾�����ɳ�Ա������Ϊ�ֲ�����
				buffer.Offset(2);
				//buffer.ReadByte(&cStartCoordType);
				//buffer.ReadByte(&cEndCoordType);
			}
		}
	}
	if((version==0||doc_type==1||(doc_type==0&&version>=1000200))&&m_dwBlockIndex==0)
	{
		buffer.ReadByte(&cLegQuad);
		buffer.Read(&cfgword,24);
	}
}
void ASSEMBLEY_POS::CloneFrom(const ASSEMBLEY_POS &srcAssembleyPos)
{
	m_dwInitPlace=srcAssembleyPos.m_dwInitPlace;
	m_dwBlockIndex=srcAssembleyPos.m_dwBlockIndex;
	acs=srcAssembleyPos.acs;
	cfgword=srcAssembleyPos.cfgword;
	cLegQuad=srcAssembleyPos.cLegQuad;
	startPos=srcAssembleyPos.startPos;
	endPos=srcAssembleyPos.endPos;
}
bool ASSEMBLEY_POS::IsInited()
{
	if(acs.origin.IsZero()&&acs.axis_x.IsZero()&&acs.axis_y.IsZero()&&acs.axis_z.IsZero())
		return false;
	else
		return true;
}
#endif
//////////////////////////////////////////////////////////////////////////
//CGeneralPart
//////////////////////////////////////////////////////////////////////////
CGeneralPart::CGeneralPart()
{
#ifdef __DRAWING_CONTEXT_
	pPart=NULL;
#elif defined(__TAP_CONTEXT_)
	pNextPart=NULL;		//��������������ͬһ��Ź���������ͬ��ͬʱ������ wjh-2016.1.26
#endif
	_ciModifiedFlag=_cPartType=(BYTE)0;
	_fWeight=0;
	_fSumWeight = 0;
	strcpy(m_sSpec,"");
	strcpy(m_sNote,"");
	m_cFuncType=0;
	feature=0;
	m_dwPartKey.dwItem=0;
	m_dwOrgPartKey.dwItem=0;
}
CGeneralPart::~CGeneralPart()
{
	
}
BOOL CGeneralPart::SetModified(BOOL bTrue/*=TRUE*/)
{
	if(bTrue)
		_ciModifiedFlag|=0x03;
	else
		_ciModifiedFlag&=0xfe;	//ȥ��0x01 ���ݸ��ı�ʶλ
	return bTrue;
}
void CGeneralPart::ClearUndoModifiedFlag()
{
	_ciModifiedFlag&=0xfd;		//ȥ��0x02 Undo������ʶλ
}
IMPLEMENT_GET(CGeneralPart,bDataModified){return (_ciModifiedFlag&0x01)>0;}
IMPLEMENT_GET(CGeneralPart,bUndoModified){return (_ciModifiedFlag&0x02)>0;}

#ifdef __TAP_CONTEXT_
bool CGeneralPart::AttachSameLabelPart(CGeneralPart* pSameLabelPart)
{
	CGeneralPart* pCurrPart=this;
	if(IsBolt())
		return false;
	if(this==pSameLabelPart)
		return false;
	bool bExistPart=false;
	while(pCurrPart->pNextPart!=NULL)
	{
		pCurrPart=pCurrPart->pNextPart;
		if(pCurrPart==pSameLabelPart)
		{
			bExistPart=true;
			break;
		}
	}
	if(!bExistPart)
	{	//�����ظ����ͬһ������
		pCurrPart->pNextPart=pSameLabelPart;
		pSameLabelPart->pNextPart=NULL;
	}
	return true;
}
#endif
char* CGeneralPart::GetSpec(BOOL bIncMat/*=TRUE*/){
#ifdef __DRAWING_CONTEXT_
	if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
		((CLDSLineAngle*)pPart)->GetSpecification(m_sSpec);
	else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
		((CLDSPolyPart*)pPart)->segset.GetFirst()->GetSpecification(m_sSpec);
#endif
	return m_sSpec;
}
char* CGeneralPart::SetSpec(char* spec)
{
	m_sSpec.Copy(spec);
	return m_sSpec;
}
SEGI CGeneralPart::GetSegI(){
#ifdef __DRAWING_CONTEXT_
	if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
		return ((CLDSLineAngle*)pPart)->iSeg;
	else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
		return ((CLDSPolyPart*)pPart)->segset.GetFirst()->iSeg;
#endif
	return m_Seg;
}
SEGI CGeneralPart::SetSegI(SEGI segi)
{
	m_Seg=segi;
	return m_Seg;
}
char* CGeneralPart::GetNotes(){
#ifdef __DRAWING_CONTEXT_
	if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
	{	//����Ǹ��ϲ��˽Ŷ����Ǹֱ�ע����Ҫ��Ӵ��Ŷ�����
		CXhChar100 notes;
		notes.Copy(pPart->sNotes);
		if(pPart->IsHasFootNail())
			notes.Append(" ���Ŷ�");
		m_sNote.Copy(notes);
	}
	else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
	{
		CXhChar100 notes;
		notes.Copy("��ע��");
		CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
		CLsRefList lsRefList;
		CLDSLinePart *pLinePart=NULL;
		for(pLinePart=pPolyPart->segset.GetFirst();pLinePart;pLinePart=pPolyPart->segset.GetNext())
		{
			if(strlen(pLinePart->sNotes)>0)
			{
				notes.Append(",");
				notes.Append(pLinePart->sNotes);
			}
		}
	}
#endif
	return m_sNote;
}
char* CGeneralPart::SetNotes(const char* sNotes)
{
	m_sNote.Copy(sNotes);
	return m_sNote;
}
#if defined(_DEBUG)&&defined(__TAP_CONTEXT_)
int CGeneralPart::GetMemorySize()
{
	int size=0;
	if(IsBolt())
	{
		CStdBolt *pBolt=(CStdBolt*)this;
		size+=sizeof(*pBolt);
		size+=pBolt->solidOfBolt.BufferLength();
		size+=pBolt->solidOfCap.BufferLength();
	}
	else if(IsProcessPart())
	{
		CProcessPart *pPart=(CProcessPart*)this;
		size+=pPart->solid.BufferLength();
		size+=sizeof(BOLT_INFO)*(pPart->m_xBoltInfoList.GetNodeNum());
		size+=sizeof(ASSEMBLEY_POS)*pPart->acsPosList.GetNodeNum();
		size+=pPart->m_xOrgProcessBuffer.GetLength();
		if(IsAngle())
		{
			CProcessAngle *pAngle=(CProcessAngle*)this;
			size+=sizeof(*pAngle);
			size+=sizeof(KAI_HE_JIAO)*pAngle->kaiHeJiaoList.GetNodeNum();
			size+=sizeof(RIB_PLATE_INFO)*pAngle->rib_plate_list.GetNodeNum();
			size+=sizeof(POLY_ANGLE_INFO)*pAngle->poly_part_info.GetNodeNum();
		}
		else if(IsPlate())
		{
			CProcessPlate *pPlate=(CProcessPlate*)this;
			size+=sizeof(*pPlate);
			size+=sizeof(CUT_POINT)*(1+pPlate->m_hashHoleCutPtByKey.GetNodeNum());
			size+=sizeof(PROFILE_VER)*pPlate->vertex_list.GetNodeNum();
		}
	}
	return size;
}
#endif
////////////////////////////////////////////////////////////////////
// CProcessPart
////////////////////////////////////////////////////////////////////
//�ж������ļ�����
/*
BYTE CProcessPart::ReadPartTypeFromBuffer(CBuffer &buffer)	
{
	BYTE partType=-1;
	//����Buffer��ȡ�ļ�����
	WORD blocki=0;
	buffer.ReadWord(&blocki);	//���������Ŀ�������
	buffer.ReadByte(&partType);	//��ȡ��������
	return partType;
}
*/
#ifdef __TAP_CONTEXT_
PLACE_CONTENT_FUNC CProcessPart::GetPlaceContentFunc=NULL;
BOOL CProcessPart::IsIdleClassicPlace(BYTE cPlaceId)
{
	if(cPlaceId<1||cPlaceId>8)
		return FALSE;
	else 
		return !(m_cPlaceStateFromFile&byteConstArr[cPlaceId-1])&&!(m_cPriorPlaceState&byteConstArr[cPlaceId-1]);
}
BOOL CProcessPart::IsExistPriorClassicPlace(BYTE cPlaceId)
{
	if(cPlaceId<1||cPlaceId>8)
		return FALSE;
	else
		return (m_cPriorPlaceState&byteConstArr[cPlaceId-1]);
}
#endif
bool CProcessPart::MergeToSelfWithModel(CBuffer& legacyPpiBufFromPart)
{
	CProcessPart* pLegacyPart=NULL;
	if(m_dwInheritPropFlag==CProcessPart::PATTERN_OVERWRITE)
		return false;
	BYTE ciPartType=RetrievedPartTypeAndLabelFromBuffer(legacyPpiBufFromPart);
	if(m_cPartType!=ciPartType)
		return false;
	CProcessAngle xAnglePart;
	CProcessPlate xPlatePart;
	bool modified=false;
	if(ciPartType==CProcessPart::TYPE_LINEANGLE)
		pLegacyPart=&xAnglePart;
	else if(ciPartType==CProcessPart::TYPE_PLATE)
		pLegacyPart=&xPlatePart;
	else
		return false;
	legacyPpiBufFromPart.SeekToBegin();
	pLegacyPart->FromPPIBuffer(legacyPpiBufFromPart);
	if(m_dwInheritPropFlag&0x0001)
	{	//0x0001==CProcessPart::PATTERN_MATERIAL
		if(pLegacyPart->cMaterial!=cMaterial)
			modified=true;
		pLegacyPart->cMaterial=cMaterial;
	}
	else if(m_dwInheritPropFlag&0x0002)
	{	//0x0002==CProcessPart::PATTERN_SIZE
		if(pLegacyPart->m_fThick!=m_fThick)
			modified=true;
		pLegacyPart->m_fThick=m_fThick;
		if(ciPartType==CProcessPart::TYPE_LINEANGLE)
		{
			if(pLegacyPart->m_fWidth!=m_fWidth)
				modified=true;
			pLegacyPart->m_fWidth=m_fWidth;
		}
	}
	else if(m_dwInheritPropFlag&CProcessPart::PATTERN_LENGTH)
	{	//0x0004==CProcessPart::PATTERN_LENGTH
		if(pLegacyPart->m_wLength!=m_wLength)
			modified=true;
		pLegacyPart->m_wLength=m_wLength;
	}
	if(ciPartType==CProcessPart::TYPE_PLATE&&m_dwInheritPropFlag&0x0040)
	{	//0x0040==CProcessPart::PATTERN_MKPOS
		if( xPlatePart.mcsFlg.wFlag!=((CProcessPlate*)this)->mcsFlg.wFlag||
			xPlatePart.mkpos!=((CProcessPlate*)this)->mkpos)
			modified=true;
		xPlatePart.mcsFlg.wFlag=((CProcessPlate*)this)->mcsFlg.wFlag;
		xPlatePart.mkpos=((CProcessPlate*)this)->mkpos;
	}
	//else if(pProcessPart->m_dwInheritPropFlag&CProcessPart::PATTERN_COUNT)
	//{	//0x0008==CProcessPart::PATTERN_COUNT
	//	pNewCopyPart->m_nDanJiNum=pProcessPart->m_nDanJiNum;
	//}
	if(!modified)
	{
		legacyPpiBufFromPart.SeekToBegin();
		FromPPIBuffer(legacyPpiBufFromPart);
	}
	else
	{
		CBuffer buff(legacyPpiBufFromPart.Length);
		pLegacyPart->ToPPIBuffer(buff);
		buff.SeekToBegin();
		FromPPIBuffer(buff);
	}
	return true;
}
bool CProcessPart::MergeToSelfWithPPE(CBuffer& bufFromPpePart)
{
	CProcessPart* pPpePart=NULL;
	if(m_dwInheritPropFlag==CProcessPart::PATTERN_OVERWRITE)
		return false;
	BYTE ciPartType=RetrievedPartTypeAndLabelFromBuffer(bufFromPpePart);
	if(m_cPartType!=ciPartType)
		return false;
	CProcessAngle xAnglePart;
	CProcessPlate xPlatePart;
	if(ciPartType==CProcessPart::TYPE_LINEANGLE)
		pPpePart=&xAnglePart;
	else if(ciPartType==CProcessPart::TYPE_PLATE)
		pPpePart=&xPlatePart;
	else
		return false;
	bufFromPpePart.SeekToBegin();
	pPpePart->FromPPIBuffer(bufFromPpePart);
	return MergeToSelfWithPPE(pPpePart);
}
bool CProcessPart::MergeToSelfWithPPE(CProcessPart* pPpePart)
{
	if(m_dwInheritPropFlag==CProcessPart::PATTERN_OVERWRITE)
		return false;
	BYTE ciPartType=pPpePart->m_cPartType;
	bool modified=false;
	if(m_dwInheritPropFlag&0x0001)
	{	//0x0001==CProcessPart::PATTERN_MATERIAL
		if(pPpePart->cMaterial!=cMaterial)
			modified=true;
		cMaterial=pPpePart->cMaterial;
	}
	else if(m_dwInheritPropFlag&0x0002)
	{	//0x0002==CProcessPart::PATTERN_SIZE
		if(pPpePart->m_fThick!=m_fThick)
			modified=true;
		m_fThick=pPpePart->m_fThick;
		if(ciPartType==CProcessPart::TYPE_LINEANGLE)
		{
			if(pPpePart->m_fWidth!=m_fWidth)
				modified=true;
			m_fWidth=pPpePart->m_fWidth;
		}
	}
	else if(m_dwInheritPropFlag&CProcessPart::PATTERN_LENGTH)
	{	//0x0004==CProcessPart::PATTERN_LENGTH
		if(pPpePart->m_wLength!=m_wLength)
			modified=true;
		m_wLength=pPpePart->m_wLength;
	}
	if(ciPartType==CProcessPart::TYPE_PLATE&&m_dwInheritPropFlag&0x0040)
	{	//0x0040==CProcessPart::PATTERN_MKPOS
		CProcessPlate* pProcessPlate=(CProcessPlate*)pPpePart;
		if( pProcessPlate->mcsFlg.wFlag!=((CProcessPlate*)this)->mcsFlg.wFlag||
			pProcessPlate->mkpos!=((CProcessPlate*)this)->mkpos)
			modified=true;
		((CProcessPlate*)this)->mcsFlg.wFlag=pProcessPlate->mcsFlg.wFlag;
		((CProcessPlate*)this)->mkpos=pProcessPlate->mkpos;
	}
	//else if(pProcessPart->m_dwInheritPropFlag&CProcessPart::PATTERN_COUNT)
	//{	//0x0008==CProcessPart::PATTERN_COUNT
	//	m_nDanJiNum=pProcessPart->m_nDanJiNum;
	//}
	return true;
}
bool CProcessPart::ClonePart(CProcessPart *pNewPart)
{	//��¡PPI������Ϣ
	if(pNewPart==NULL||pNewPart->m_cPartType!=m_cPartType)
		return false;
	CBuffer partBuffer;
	ToPPIBuffer(partBuffer);
	partBuffer.SeekToBegin();
	pNewPart->FromPPIBuffer(partBuffer);
	return true;
}

CProcessPart* CProcessPartList::CreateProcessPart(int idPartClsType,const char* key,void* pContext)
{
	CProcessPart* pPart=NULL;
	if(idPartClsType==CProcessPart::TYPE_LINEANGLE)
		pPart=new CProcessAngle();
	else if(idPartClsType==CProcessPart::TYPE_PLATE)
		pPart=new CProcessPlate();
	else
		pPart=new CProcessPart();	
	return pPart;
}
BOOL CProcessPartList::DestroyProcessPart(CProcessPart* pPart)
{
	if(pPart->m_cPartType==CProcessPart::TYPE_LINEANGLE)
		delete (CProcessAngle*)pPart;
	else if(pPart->m_cPartType==CProcessPart::TYPE_PLATE)
		delete (CProcessPlate*)pPart;
	else
		delete (CProcessPart*)pPart;
	return TRUE;
}
CProcessPartList::CProcessPartList()
{
	CreateNewAtom=CreateProcessPart;
	DeleteAtom = DestroyProcessPart;
}

int CProcessPart::BUFFER_VERSION::Compare(const BUFFER_VERSION& ver)
{
	if(major>ver.major)
		return 1;
	else if(major<ver.major)
		return -1;
	else if(revision>ver.revision)
		return 1;
	else if(revision<ver.revision)
		return -1;
	else
		return 0;
}
CProcessPart::CProcessPart()
{
#ifdef __TAP_CONTEXT_
	m_fOrgWidth=m_fOrgThick=m_wOrgLength=0;
	m_cPlaceMode=0;
	m_cPlaceStateFromFile=0;
	m_cPriorPlaceState=0;
	m_cOrgMaterial = 0;
	m_cOrgQuality = 0;
	acsPosList.SetMinmalId(9);	//ǰ1~8Ϊ����ռλ
#endif
	_fWidth=_fThick=_fHigh=_fWeight=_wLength=0;
	_cMaterial=NULL;
	_cQuality = 0;
	strcpy(m_sPartNo,"");
	mkpos.Set(0,0,0);
	m_nDanJiNum=0;
	m_idSrcObject=0;
	m_dwInheritPropFlag=PATTERN_INHERIT;
}
CProcessPart::~CProcessPart()
{
#ifdef __TAP_CONTEXT_
	solid.Empty();
	acsPosList.Empty();
#endif
}

CXhChar16 CProcessPart::QuerySteelMatMark(char cMat, char* matStr/*=NULL*/)
{
	CXhChar16 sMatMark;
	if ('H' == cMat)
		sMatMark.Copy("Q345");
	else if ('h' == cMat)	//��Сдh��ʾQ355,����򻯲����ַ�ʱ��Ҫת��д wht 19-11-05
		sMatMark.Copy("Q355");
	else if ('G' == cMat)
		sMatMark.Copy("Q390");
	else if ('P' == cMat)
		sMatMark.Copy("Q420");
	else if ('T' == cMat)
		sMatMark.Copy("Q460");
	else if ('S' == cMat || 0 == cMat)
		sMatMark.Copy("Q235");
	else if (matStr)
		sMatMark.Copy(matStr);
	return sMatMark;
}
char CProcessPart::QueryBriefMatMark(const char* sMatMark)
{
	char cMat = 'A';
	if (strstr(sMatMark, "Q235") || strlen(sMatMark) == 0)
		cMat = 'S';
	else if (strstr(sMatMark, "Q345"))
		cMat = 'H';
	else if (strstr(sMatMark, "Q355"))
		cMat = 'h';	//��Сдh��ʾQ355,����򻯲����ַ�ʱ��Ҫת��д wht 19-11-05
	else if (strstr(sMatMark, "Q390"))
		cMat = 'G';
	else if (strstr(sMatMark, "Q420"))
		cMat = 'P';
	else if (strstr(sMatMark, "Q460"))
		cMat = 'T';
	return cMat;
}
char CProcessPart::QueryBriefQuality(const char* sMatMark)
{
	char cQuality = 0;
	CXhChar16 sMaterial(sMatMark);
	if(sMaterial.GetLength() ==5)
		cQuality = toupper(sMaterial.At(4));
	return cQuality;
}
//�������
void CProcessPart::RestoreSpec(const char* spec, int *width, int *thick, char *matStr/*=NULL*/)
{
	char sMat[16] = "", cMark1 = ' ', cMark2 = ' ';
	if (strstr(spec, "Q") == (char*)spec)
	{
		if (strstr(spec, "L"))
			sscanf(spec, "%[^L]%c%d%c%d", sMat, &cMark1, width, &cMark2, thick);
		else if (strstr(spec, "-"))
			sscanf(spec, "%[^-]%c%d", sMat, &cMark1, thick);
	}
	else if (strstr(spec, "L"))
	{
		CXhChar16 sSpec(spec);
		sSpec.Replace("L", "");
		sSpec.Replace("*", " ");
		sSpec.Replace("X", " ");
		sscanf(sSpec, "%d%d", width, thick);
	}
	else if (strstr(spec, "-"))
		sscanf(spec, "%c%d", sMat, thick);
	//else if(spec,"��")
	//sscanf(spec,"%c%d%c%d",sMat,)
	if (matStr)
		strcpy(matStr, sMat);
}

#ifdef __PROPERTYLIST_UI_
PROPLIST_TYPE CProcessPart::propSync;
void CProcessPart::InitSyncProp()
{
	propSync.InitHashtableSize(32);
	propSync.AddPropItem("material",PROPLIST_ITEM(0x0001,"����"));
	propSync.AddPropItem("spec",PROPLIST_ITEM(0x0002,"���"));
	propSync.AddPropItem("length",PROPLIST_ITEM(0x0004,"����"));
	propSync.AddPropItem("count",PROPLIST_ITEM(0x0008,"������"));
	propSync.AddPropItem("conn_bolt",PROPLIST_ITEM(0x0010,"������˨"));
	propSync.AddPropItem("only_hole",PROPLIST_ITEM(0x0020,"�����"));
	propSync.AddPropItem("mkpos",PROPLIST_ITEM(0x0040,"��ӡ��"));
}
#endif
float CProcessPart::GetWeight(BOOL bSolidLength/*=TRUE*/)
{
	return m_fWeight;
}

float CProcessPart::GetWidth()
{
	return m_fWidth;
}

float CProcessPart::GetThick()
{
	return m_fThick;
}

float CProcessPart::GetHeight()
{
	return m_fHigh;
}

WORD CProcessPart::GetLength()
{
	return m_wLength;
}
char* CProcessPart::GetSpec(BOOL bIncMat/*=TRUE*/)
{
#ifdef __DRAWING_CONTEXT_
	if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
		((CLDSLineAngle*)pPart)->GetSpecification(m_sSpec);
	else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
		((CLDSPolyPart*)pPart)->segset.GetFirst()->GetSpecification(m_sSpec);
#else
	char steelmark[20]="";
	if(bIncMat&&cMaterial!='S')
		CProcessPart::QuerySteelMatMark(cMaterial,steelmark);
	if (strlen(steelmark) > 0 && cQuality != 0)
	{	//���������ȼ� wht 19-09-15
		char sQuality[10]="";
		sprintf(sQuality, "%C", cQuality);
		strcat(steelmark, sQuality);
	}
	if(IsAngle())
		m_sSpec.Printf("%s L%gx%g",steelmark,m_fWidth,m_fThick);
	else if(IsPlate())
		m_sSpec.Printf("%s-%g",steelmark,m_fThick);
#endif
	return m_sSpec;
}
struct LS_HOLE{double hole_d;short n;};
int CProcessPart::GetLsString(char *ls_str)
{
	ATOM_LIST<LS_HOLE>hole_list;
	ATOM_LIST<BOLT_INFO>bolt_list_temp;
	for(BOLT_INFO *pBoltInfo=m_xBoltInfoList.GetFirst();pBoltInfo;pBoltInfo=m_xBoltInfoList.GetNext())
		bolt_list_temp.append(*pBoltInfo);
	for(BOLT_INFO *pBoltInfo=bolt_list_temp.GetFirst();pBoltInfo;pBoltInfo=bolt_list_temp.GetNext())
	{
		LS_HOLE *pHole;
		for(pHole=hole_list.GetFirst();pHole;pHole=hole_list.GetNext())
		{
			if(pHole->hole_d<(pBoltInfo->bolt_d+pBoltInfo->hole_d_increment))
				continue;
			else if(pHole->hole_d==(pBoltInfo->bolt_d+pBoltInfo->hole_d_increment))
			{
				pHole->n++;
				break;
			}
			else 
			{
				pHole=hole_list.insert(*pHole);
				pHole->hole_d=pBoltInfo->bolt_d+pBoltInfo->hole_d_increment;
				pHole->n=1;
				break;
			}
		}
		if(pHole==NULL)
		{
			pHole=hole_list.append();
			pHole->hole_d=pBoltInfo->bolt_d+pBoltInfo->hole_d_increment;
			pHole->n=1;
		}
	}
	char str[100]="",ss[500]="";
	for(LS_HOLE *pHole=hole_list.GetFirst();pHole;pHole=hole_list.GetNext())
	{
		sprintf(str,"%gX%d",pHole->hole_d,pHole->n);
		if(strlen(ss)>0)
		{
			strcat(ss," ");
			strcat(ss,str);
		}
		else
			strcpy(ss,str);
	}
	if(ls_str)
		strcpy(ls_str,ss);
	return (int)strlen(ss);
}
int CProcessPart::GetLsCount(short d/*=0*/)
{
	int nBoltNum=0;
#ifdef __DRAWING_CONTEXT_
	if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
	{
		CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
		for(CLDSLineAngle *pJg=(CLDSLineAngle*)pPolyPart->segset.GetFirst();pJg;pJg=(CLDSLineAngle*)pPolyPart->segset.GetNext())
			nBoltNum+=pJg->GetLsCount(d);
		return nBoltNum;
	}
	else if(pPart)
		return pPart->GetLsCount(d);
#endif
	if(d==0)
		return m_xBoltInfoList.GetNodeNum();
	for(BOLT_INFO* pBoltInfo=m_xBoltInfoList.GetFirst();pBoltInfo;pBoltInfo=m_xBoltInfoList.GetNext())
	{
		if (pBoltInfo->bolt_d==d)
			nBoltNum++;
	}
	return nBoltNum;
}
int CProcessPart::GetDrillHoleCount()
{
	int nHoleNum=0;
#ifdef __DRAWING_CONTEXT_
	if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
	{
		CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
		for(CLDSLineAngle *pJg=(CLDSLineAngle*)pPolyPart->segset.GetFirst();pJg;pJg=(CLDSLineAngle*)pPolyPart->segset.GetNext())
		{
			for(CLsRef *pLsRef=pJg->GetFirstLsRef();pLsRef;pLsRef=pJg->GetNextLsRef())
			{
				if (pLsRef->cFlag&CLsRef::DRILL_HOLE)	//CLsRef::DRILL_HOLE
					nHoleNum++;
			}
		}
	}
	else if(pPart)
	{
		CLsRefList *pLsRefList=pPart->GetLsRefList();
		if(pLsRefList)
		{
			for(CLsRef *pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
			{
				if (pLsRef->cFlag&CLsRef::DRILL_HOLE)	//CLsRef::DRILL_HOLE
					nHoleNum++;
			}
		}
	}
#else
	for(BOLT_INFO* pBoltInfo=m_xBoltInfoList.GetFirst();pBoltInfo;pBoltInfo=m_xBoltInfoList.GetNext())
	{
		if (pBoltInfo->cFlag==1)	//CLsRef::DRILL_HOLE
			nHoleNum++;
	}
#endif
	return nHoleNum;
}
BOLT_INFO* CProcessPart::FromHoleId(DWORD holeId)
{
	return m_xBoltInfoList.GetValue(holeId);
}
bool CProcessPart::DeleteBoltHoleById(DWORD id)
{
	return m_xBoltInfoList.DeleteNode(id)==TRUE;
}

BOLT_INFO* CProcessPart::FromHoleHiberId(HIBERARCHY id)
{
	BOLT_INFO *pBoltInfo=NULL;
	int nPush=m_xBoltInfoList.push_stack();
	for(pBoltInfo=m_xBoltInfoList.GetFirst();pBoltInfo;pBoltInfo=m_xBoltInfoList.GetNext())
	{
		if(id.IsEqual(pBoltInfo->hiberId))
			break;
	}
	m_xBoltInfoList.pop_stack(nPush);
	return pBoltInfo;
}
bool CProcessPart::DeleteBoltHoleByHiberId(HIBERARCHY id)
{
	bool bRetCode=false;
	BOLT_INFO *pBoltInfo=NULL;
	int nPush=m_xBoltInfoList.push_stack();
	for(pBoltInfo=m_xBoltInfoList.GetFirst();pBoltInfo;pBoltInfo=m_xBoltInfoList.GetNext())
	{
		if(id.IsEqual(pBoltInfo->hiberId))
		{
			m_xBoltInfoList.DeleteCursor(TRUE);
			bRetCode=true;
			break;
		}
	}
	m_xBoltInfoList.pop_stack(nPush);
	return bRetCode;
}
f3dPoint CProcessAngle::GetWingNormX()
{	//acs����ʱ��X֫Ϊ��׼��X֫���߷���ΪY�Ḻ����
	return f3dPoint(0,-1,0);
}
f3dPoint CProcessAngle::GetWingNormY()
{
	f3dPoint norm_y_wing(-1,0,0);
	double fDecWingAngle=GetDecWingAngle();
	if(fabs(fDecWingAngle-90)>0)
	{	//acs����ʱ��X֫Ϊ��׼��������ڿ��ϽǸִ˴������Y֫���߷���
		double angle=(fDecWingAngle-90)*RADTODEG_COEF;
		RotateVectorAroundVector(norm_y_wing,angle,f3dPoint(0,0,1));
	}
	return norm_y_wing;
}
f3dPoint CProcessAngle::GetWingVecX()
{
	return f3dPoint(1,0,0);
}
f3dPoint CProcessAngle::GetWingVecY()
{
	f3dPoint wing_vec = GetWingNormY()^f3dPoint(0,0,1);
	return wing_vec.normalized();
}
void CProcessAngle::GetProcessStr(char *notes, size_t max_len,bool bByEnglish/*=false*/,bool bNumericKaiHeInfo/*=false*/)
{
	if(notes==NULL)
		return;
	CXhString sNotes(notes,(int)max_len);
	for(int i=0;i<4;i++)
	{
		if(cut_angle[i][0]>EPS)
		{
			sNotes.Append("�н�",' ');
			break;
		}
	}
	if(cut_wing[0]>0||cut_wing[1]>0)
		sNotes.Append("��֫",' ');
	KAI_HE_JIAO *pKaiHeJiao=kaiHeJiaoList.GetFirst();
	float wing_angle=0;
	if (pKaiHeJiao)
		wing_angle=pKaiHeJiao->decWingAngle;
	//����97��ͼ�涨���е��޸� WJH-2004/09/28
	int kaihe_wingx0_y1=kaihe_base_wing_x0_y1;
	if((kaihe_wingx0_y1==0||kaihe_wingx0_y1==1))
	{
		if(wing_angle>92)
			sNotes.Append("����",' ');
		else if(wing_angle>0&&wing_angle<88)
			sNotes.Append("�Ͻ�",' ');
		if(bNumericKaiHeInfo)
			sNotes.Append(CXhChar16("%g��",ftoi(wing_angle*10)*0.1),0);
	}
	if(m_bCutRoot)	//�ٸ�
		sNotes.Append("�ٸ�",' ');
	else if(m_bCutBer)	//��������
		sNotes.Append("����",' ');
	if(wing_push_X1_Y2!=0)
		sNotes.Append("ѹ��",' ');
	if(m_bWeld)
		sNotes.Append("����",' ');
	return;
}

CXhChar16 CProcessPart::GetPartNo() const
{
#ifdef __DRAWING_CONTEXT_
	if(pPart)
		return pPart->GetPartNo();
#endif
		return m_sPartNo;
}
const char* CProcessPart::GetLabel()const
{
#ifdef __DRAWING_CONTEXT_
	if(pPart)
		return pPart->GetPartNo();
#endif
		return m_sPartNo;
}
void CProcessPart::SetPartNo(const char *part_no,bool bIncStdPart/*=false*/)
{
		m_sPartNo.Copy(part_no);
}
CBuffer *CProcessPart::GetOrgProcessBuffer()
{
	CBuffer buffer;
	WriteOrgProcessBuffer(buffer);
	if(m_xOrgProcessBuffer.GetLength()<=0)
		return NULL;
	else if(buffer.GetLength()!=m_xOrgProcessBuffer.GetLength()||
		memcmp(buffer.GetBufferPtr(),m_xOrgProcessBuffer.GetBufferPtr(),buffer.GetLength())!=0)
		return &m_xOrgProcessBuffer;
	else
		return NULL;
}
void CProcessPart::WriteOrgProcessBuffer(CBuffer &buffer)
{
	if(buffer.GetLength()<=0)
		return;
	buffer.ClearContents();
	buffer.WriteByte(cMaterial);
	buffer.WriteFloat(m_fWidth);
	buffer.WriteFloat(m_fThick);
	buffer.WriteWord(m_wLength);
	buffer.WriteDword(m_dwOrgPartKey);
	WriteProcessBuffer(buffer,0,true);
}
void CProcessPart::InitOrgProcessBuffer(CBuffer &orgBuffer)
{
	m_xOrgProcessBuffer.ClearContents();
	if(orgBuffer.GetLength()==0)	//��ʼ��ԭʼ������ϢBuffer
		WriteOrgProcessBuffer(m_xOrgProcessBuffer);
	else
	{
		m_xOrgProcessBuffer.Write(orgBuffer.GetBufferPtr(),orgBuffer.GetLength());
#ifdef __TAP_CONTEXT_
		m_xOrgProcessBuffer.SeekToBegin();
		m_xOrgProcessBuffer.ReadByte(&m_cOrgMaterial);
		m_xOrgProcessBuffer.ReadFloat(&m_fOrgWidth);
		m_xOrgProcessBuffer.ReadFloat(&m_fOrgThick);
		m_xOrgProcessBuffer.ReadWord(&m_wOrgLength);
		m_xOrgProcessBuffer.ReadDword(&m_dwOrgPartKey.dwItem);
#endif
	}
}
bool CProcessPart::IsEqual(CGeneralPart *pPart,double epsilon/*=1.0*/,char *error_info/*=NULL*/,DWORD dwChkStyleFlag/*=NULL*/)
{	//�Ƚϻ�����Ϣ
	if(pPart==NULL||pPart->m_cPartType!=m_cPartType)
	{
		if(error_info)
			strcpy(error_info,"���Ͳ�ͬ");
		return false;
	}
	CProcessPart *pProcessPart=(CProcessPart*)pPart;
	if(cMaterial!=pProcessPart->cMaterial)
	{
		if(error_info)
			strcpy(error_info,"���ʲ�ͬ");
		return false;
	}
	if (cQuality != pProcessPart->cQuality)
	{
		if (error_info)
			strcpy(error_info, "�����ȼ���ͬ");
		return false;
	}
	if( fabs(m_fWidth-pProcessPart->m_fWidth)>EPS||
		fabs(m_fThick-pProcessPart->m_fThick)>EPS||
		fabs(m_fHigh-pProcessPart->m_fHigh)>EPS)
	{
		if(error_info)
			strcpy(error_info,"���ͬ");
		return false;
	}
	if(m_wLength!=pProcessPart->m_wLength)
	{
		if(error_info)
			strcpy(error_info,"���Ȳ�ͬ");
		return false;
	}
	//�Ƚ���˨��Ϣ
	if(m_xBoltInfoList.GetNodeNum()!=pProcessPart->m_xBoltInfoList.GetNodeNum())
	{
		if(error_info)
			strcpy(error_info,"��˨������ͬ");
		return false;
	}
	BOLT_INFO *pSrcInfo=NULL;
	for(pSrcInfo=m_xBoltInfoList.GetFirst();pSrcInfo;pSrcInfo=m_xBoltInfoList.GetNext())
	{
		BOLT_INFO *pDestInfo=pProcessPart->m_xBoltInfoList.GetValue(m_xBoltInfoList.GetCursorKey());
		if(pDestInfo==NULL)
			break;
		if(pSrcInfo->cFaceNo!=pDestInfo->cFaceNo)
			break;
		if(pSrcInfo->bolt_d!=pDestInfo->bolt_d)
			break;
		if(pSrcInfo->hole_d_increment!=pDestInfo->hole_d_increment)
			break;
		if(pSrcInfo->waistLen!=pDestInfo->waistLen)
			break;
		if(!pSrcInfo->waistVec.IsEqual(pDestInfo->waistVec.x,pDestInfo->waistVec.y,pDestInfo->waistVec.z,epsilon))
			break;
		if(pSrcInfo->cFuncType!=pDestInfo->cFuncType)
			break;
		f3dPoint srcPos(pSrcInfo->posX,pSrcInfo->posY),destPos(pDestInfo->posX,pDestInfo->posY);
		if(!srcPos.IsEqual(destPos,epsilon))
			break;
		if(pSrcInfo->m_dwFlag.FlagWord()!=pDestInfo->m_dwFlag.FlagWord())
			break;
	}
	if(pSrcInfo)
	{
		if(error_info)
			strcpy(error_info,"��˨��λ��ͬ");
		return false;
	}
#ifdef __TAP_CONTEXT_
	if(dwChkStyleFlag==NULL||(dwChkStyleFlag&CHECK_STYLE_ACS_POS))
	{
	//�Ƚ�װ����Ϣ
	//CSolidBody solid;				//ʵ������
	if(acsPosList.GetNodeNum()!=pProcessPart->acsPosList.GetNodeNum())
	{
		if(error_info)
			strcpy(error_info,"װ���¼������ͬ");
		return false;
	}
	ASSEMBLEY_POS *pAcs=NULL;
	for(pAcs=acsPosList.GetFirst();pAcs;pAcs=acsPosList.GetNext())
	{
		ASSEMBLEY_POS *pOtherAcs=pProcessPart->acsPosList.GetValue(pAcs->GetKey());
		if(pOtherAcs==NULL)
			break;
		GECS acs=pAcs->acs,otherAcs=pOtherAcs->acs;
		if(!otherAcs.origin.IsEqual(acs.origin.x,acs.origin.y,acs.origin.z,epsilon))
			break;
		if(!otherAcs.axis_x.IsEqual(acs.axis_x.x,acs.axis_x.y,acs.axis_x.z,epsilon))
			break;
		if(!otherAcs.axis_y.IsEqual(acs.axis_y.x,acs.axis_y.y,acs.axis_y.z,epsilon))
			break;
		if(!otherAcs.axis_z.IsEqual(acs.axis_z.x,acs.axis_z.y,acs.axis_z.z,epsilon))
			break;
	}
	if(pAcs)
	{
		if(error_info)
			strcpy(error_info,"װ���¼λ�ò�ͬ");
		return false;
	}
	}
#endif
	return true;
}
BYTE CProcessPart::RetrievedPartTypeAndLabelFromBuffer(CBuffer &buffer,char *sPartLabel/*=NULL*/,UINT maxStrBuffLen/*=17*/)
{
	DWORD begin_pos=buffer.GetCursorPosition();
	//��ȡ�ĸ��ֽڣ��ж��Ƿ�Ϊ�汾��
	BUFFER_VERSION v;
	buffer.ReadInteger(&v.version);
	//version==1000001��ʹ��,�˴��迼�Ǽ���
	//�Ժ�汾�������ֽ�����Ϊ0xFF,��ʾ�а汾��
	if(v.version!=1000001&&v.bytes[3]!=0xFF)
		buffer.SeekPosition(begin_pos);	//û�а汾����ת����ʼλ��
	else if(v.bytes[3]==0xFF)
		v.bytes[3]=0x0;	
	if(v.version>=1000005)
		buffer.Offset(4);	//m_verSerial.version
	BYTE cPartType=*((BYTE*)buffer.GetCursorBuffer());
	buffer.SeekPosition(buffer.GetCursorPosition()+24);	//��ת������λ��(CProcessPart::ToPPIBuffer)
	if(sPartLabel&&v.version<1000005)
		buffer.ReadString(sPartLabel);
	else if(sPartLabel)
	{
		WORD wAttachBuff=2;
		buffer.ReadWord(&wAttachBuff);
		for(WORD i=0;i<wAttachBuff;i++)
		{
			DWORD idBuff=0,nBuffSize=0;
			idBuff=buffer.ReadInteger();
			nBuffSize=buffer.ReadInteger();
			if(idBuff==1)		//�������ӻ���
			{
				buffer.Offset(8);
				//buffer.ReadDword(&m_dwInheritPropFlag);
				//buffer.ReadInteger(&m_idSrcObject);
				buffer.ReadString(sPartLabel,maxStrBuffLen);
				break;
			}
			else	//������ʶ��Ļ�����
				buffer.Offset(nBuffSize);
		}
	}
	buffer.SeekPosition(begin_pos);
	return cPartType;
}
void CProcessPart::ReadPPIBufferBasicInfo(CBuffer &buffer,long* pVersion/*=NULL*/)
{
	BUFFER_VERSION v;
	buffer.PushPositionStack();
	//��ȡ�ĸ��ֽڣ��ж��Ƿ�Ϊ�汾��
	buffer.ReadInteger(&v.version);		//�汾��
	if(v.version<1000001&&v.bytes[3]!=0xFF)
	{	//�����ް汾�ŵ����
		v.version=0;
		buffer.PopPositionStack();
	}
	else if(v.bytes[3]==0xFF)
		v.bytes[3]=0x0;	
	if(pVersion)
		*pVersion=v.version;
	if(v.version>=1000005)
		buffer.ReadInteger(&m_verSerial.version);
	//��հ汾�ű�ʶ�ֽ�,��ԭΪ��ʵ�İ汾
	//1��������Ϣ
	buffer.ReadByte(&_cPartType);		//��������
	buffer.ReadByte(&_cMaterial);		//����
	buffer.ReadInteger(&m_Seg.iSeg);	//�����ֶκ�
	buffer.ReadFloat(&_fWidth);			//���
	buffer.ReadFloat(&_fThick);			//���
	buffer.ReadFloat(&_fHigh);			//�߶�
	buffer.ReadWord(&_wLength);			//����(mm)
	buffer.ReadFloat(&_fWeight);		//����(kg)
}
void CProcessPart::ReadBasicPPIAttachBuffer(CBuffer &buffer,long version/*=0*/)
{	//2.����������Ϣ
	if(version>=1000005)
	{
		buffer.ReadDword(&m_dwInheritPropFlag);
		buffer.ReadInteger(&m_idSrcObject);
	}
	buffer.ReadString(m_sPartNo);		//�������
	buffer.ReadString(m_sSpec);			//�������
	buffer.ReadString(m_sNote);			//��ע
	if(version>=1000004)
		buffer.ReadString(m_sRelatePartNo);
}
void CProcessPart::FromPPIBuffer(CBuffer &buffer)
{
	long version=0;
	ReadPPIBufferBasicInfo(buffer,&version);
	if(version>=1000005)
	{
		WORD wAttachBuff=2;
		buffer.ReadWord(&wAttachBuff);
		for(WORD i=0;i<wAttachBuff;i++)
		{
			DWORD idBuff=0,nBuffSize=0;
			idBuff=buffer.ReadInteger();
			nBuffSize=buffer.ReadInteger();
			if(idBuff==1)		//�������ӻ���
				ReadBasicPPIAttachBuffer(CBuffer(buffer.GetCursorBuffer(),nBuffSize),version);
			else if(idBuff==2)	//�ӹ�������Ϣ���ӻ���
				ReadProcessBuffer(CBuffer(buffer.GetCursorBuffer(),nBuffSize),version,true,PPI_BUFFER);
			else if(idBuff==3&&m_cPartType==CProcessPart::TYPE_PLATE)
				((CProcessPlate*)this)->CutPointFromBuffer(CBuffer(buffer.GetCursorBuffer(),nBuffSize),version);
			//����������
			buffer.Offset(nBuffSize);
		}
	}
	else
	{
		//2.����������Ϣ
		buffer.ReadString(m_sPartNo);		//�������
		buffer.ReadString(m_sSpec);			//�������
		buffer.ReadString(m_sNote);			//��ע
		if(version>=1000004)
			buffer.ReadString(m_sRelatePartNo);
		//3.������Ϣ
		DWORD dwProcessBufLen=0;
		buffer.ReadInteger(&dwProcessBufLen);
		ReadProcessBuffer(buffer,version,true,PPI_BUFFER);
		//
		if(m_cPartType==CProcessPart::TYPE_PLATE&&version>=1000005)
			((CProcessPlate*)this)->CutPointFromBuffer(buffer,version);
	}
}
void CProcessPart::ToPPIBuffer(CBuffer &buffer)
{
	BUFFER_VERSION v(1000006);	//�汾�����ֵΪ:16777215(0x00FFFFFF)
	v.bytes[3]=0xFF;			//ǰ�����ֽ�������ʾ�汾�ţ����ĸ��ֽ������ж��Ƿ�Ϊ�汾��
	buffer.WriteInteger(v.version);		//�汾��
	v.bytes[3]=0x0;				//��հ汾�ű�ʶ�ֽ�,��ԭΪ��ʵ�İ汾
	buffer.WriteInteger(m_verSerial.version);	//�汾��ˮ��(���Ʋ�ͬʱ��Ĵ洢�汾,��v.version��ͬ�޹غ���ʽ)
	//д�������Ϣ
	buffer.WriteByte(m_cPartType);
	buffer.WriteByte(cMaterial);		//д����ʼ��ַ�
	buffer.WriteInteger(m_Seg.iSeg);	//д������ֶκ�
	buffer.WriteFloat(m_fWidth);		//д����
	buffer.WriteFloat(m_fThick);		//д����
	buffer.WriteFloat(m_fHigh);			//д��߶�
	buffer.WriteWord(m_wLength);		//д�볤��(mm)
	buffer.WriteFloat(m_fWeight);		//д�뵥��(kg)
	if(v.version>=1000005)
	{
		WORD wAttachBuff=2;
		if(m_cPartType==CProcessPart::TYPE_PLATE&&v.version>=1000005)
			wAttachBuff=3;
		buffer.WriteWord(wAttachBuff);
		//1.д�빹��������Ϣ
		buffer.WriteInteger(1);	//���������ͱ�ʶ, 1:�������ӻ�����
		DWORD stackPos=buffer.CursorPosition;
		buffer.WriteInteger(0);	//Ԥ���������ӻ���������ռλ�ֽ�
		//����������Ϣ�İ汾��ˮ��>=V1000005
		buffer.WriteDword(m_dwInheritPropFlag);
		buffer.WriteInteger(m_idSrcObject);

		buffer.WriteString(m_sPartNo);
		buffer.WriteString(m_sSpec);
		buffer.WriteString(m_sNote);
		if(v.version>=1000004)
			buffer.WriteString(m_sRelatePartNo);
		DWORD currPos=buffer.CursorPosition;
		buffer.SeekPosition(stackPos);
		buffer.WriteInteger(currPos-stackPos-4);
		buffer.SeekPosition(currPos);
		//2.д�빤����Ϣ
		buffer.WriteInteger(2);	//���������ͱ�ʶ, :���ո��ӻ�����
		CBuffer processBuf;
		WriteProcessBuffer(processBuf,v.version,true,PPI_BUFFER);
		buffer.WriteInteger(processBuf.Length);
		buffer.Write(processBuf.GetBufferPtr(),processBuf.GetLength());
		if(m_cPartType==CProcessPart::TYPE_PLATE&&v.version>=1000005)
		{	//3.�ְ��и����Ϣ
			buffer.WriteInteger(3);	//���������ͱ�ʶ, 3:�ְ��и����Ϣ���ӻ�����
			stackPos=buffer.CursorPosition;
			buffer.WriteInteger(0);	//Ԥ���������ӻ���������ռλ�ֽ�
			((CProcessPlate*)this)->CutPointToBuffer(buffer,v.version);
			currPos=buffer.CursorPosition;
			buffer.SeekPosition(stackPos);
			buffer.WriteInteger(currPos-stackPos-4);
			buffer.SeekPosition(currPos);
		}
	}
	else
	{	//д�빹��������Ϣ
		buffer.WriteString(m_sPartNo);
		buffer.WriteString(m_sSpec);
		buffer.WriteString(m_sNote);
		if(v.version>=1000004)
			buffer.WriteString(m_sRelatePartNo);
		//д�빤����Ϣ
		CBuffer processBuf;
		WriteProcessBuffer(processBuf,v.version,true,PPI_BUFFER);
		buffer.WriteInteger(processBuf.GetLength());
		buffer.Write(processBuf.GetBufferPtr(),processBuf.GetLength());
		//�ְ��и����Ϣ
		if(m_cPartType==CProcessPart::TYPE_PLATE&&v.version>=1000005)
			((CProcessPlate*)this)->CutPointToBuffer(buffer,v.version);
	}
}
#ifdef __TAP_CONTEXT_
bool CProcessPart::IsReplacePart()	//�Ƿ�Ϊ���Ϲ���
{
	if(m_cOrgMaterial!=0&&m_cOrgMaterial!=_cMaterial)
		return true;
	return false;
}
void CProcessPart::FromTIDBuffer(CBuffer &buffer,long version/* =0 */)
{
	//1��������Ϣ
	WORD blockID=0;
	buffer.ReadWord(&blockID);			//����������ģ�Ϳռ�
	buffer.ReadByte(&_cPartType);
	WORD state;							//����״̬����
	buffer.ReadWord(&state);
	buffer.ReadByte(&_cMaterial);		//����
	buffer.ReadInteger(&m_Seg.iSeg);	//�����ֶκ�
	buffer.ReadFloat(&_fWidth);			//���
	buffer.ReadFloat(&_fThick);			//���
	buffer.ReadFloat(&_fHigh);			//�߶�
	buffer.ReadWord(&_wLength);			//����(mm)
	buffer.ReadFloat(&_fWeight);		//����(kg)
	buffer.ReadByte(&m_cFuncType);
	//2��ʵ����Ϣ
	DWORD dwSolidSize=0;
	buffer.ReadDword(&dwSolidSize);
	if(dwSolidSize)
	{
		solid.CopyBuffer(buffer.GetCursorBuffer(),dwSolidSize);	//ʵ��
		buffer.Offset(dwSolidSize);
	}
	//3��������Ϣ
	buffer.ReadString(m_sPartNo);		//�������
	buffer.ReadString(m_sSpec);			//�������
	buffer.ReadString(m_sNote);			//��ע
	//4��������Ϣ
	DWORD processInfoSize=0;
	buffer.ReadDword(&processInfoSize);
	if(processInfoSize>0)
		ReadProcessBuffer(buffer,version,false,TID_BUFFER);
}
void CProcessPart::FromTAPBuffer(CBuffer &buffer,long version/* =0 */)
{
	//1.����������Ϣ
	buffer.ReadByte(&_cPartType);
	buffer.ReadByte(&m_cFuncType);
	buffer.ReadByte(&_cMaterial);		//����
	buffer.ReadInteger(&m_Seg.iSeg);	//�����ֶκ�
	buffer.ReadFloat(&_fWidth);			//���
	buffer.ReadFloat(&_fThick);			//���
	buffer.ReadFloat(&_fHigh);			//�߶�
	buffer.ReadWord(&_wLength);			//����(mm)
	buffer.ReadFloat(&_fWeight);		//����(kg)
	buffer.ReadString(m_sPartNo);		//�������
	buffer.ReadString(m_sSpec);			//�������
	buffer.ReadString(m_sNote);			//��ע
	//2.ʵ����Ϣ
	DWORD dwSolidSize=0;
	buffer.ReadDword(&dwSolidSize);
	if(dwSolidSize)
	{
		solid.CopyBuffer(buffer.GetCursorBuffer(),dwSolidSize);	//ʵ��
		buffer.Offset(dwSolidSize);
	}
	//3.װ����Ϣ
	int n=0;
	buffer.ReadInteger(&n);
	acsPosList.Empty();
	for(int i=0;i<n;i++)
	{
		DWORD dwKey=0;
		buffer.ReadDword(&dwKey);
		ASSEMBLEY_POS *pPos=acsPosList.Add(dwKey);
		pPos->FromBuffer(buffer,version);
	}
	if(version==NULL||version>=1000200)
	{
		buffer.ReadByte(&m_cPlaceMode);
		buffer.ReadByte(&m_cPlaceStateFromFile);
		buffer.ReadByte(&m_cPriorPlaceState);
	}
	//4.������Ϣ
	ReadProcessBuffer(buffer,version,true,TAP_BUFFER);
	//5����˨������
	n=buffer.ReadInteger();
	if(n!=m_xBoltInfoList.GetNodeNum())
		buffer.SeekPosition(buffer.GetCursorPosition()+n);
	else
	{
		for(BOLT_INFO *pBolt=m_xBoltInfoList.GetFirst();pBolt;pBolt=m_xBoltInfoList.GetNext())
			buffer.ReadByte(&pBolt->cFuncType);
	}
	//6.��ȡ����ԭʼ������Ϣ����
	n=buffer.ReadInteger();
	CBuffer orgProcessBuf;
	if(n>0)
		buffer.Read(orgProcessBuf.GetBufferPtr(),n);
	InitOrgProcessBuffer(orgProcessBuf);
}
void CProcessPart::ToTAPBuffer(CBuffer &buffer,bool bIncSolidData/*=true*/)
{
	long version=0;
	//1��������Ϣ
	buffer.WriteByte(m_cPartType);
	buffer.WriteByte(m_cFuncType);
	buffer.WriteByte(cMaterial);			//д����ʼ��ַ�
	buffer.WriteInteger(m_Seg.iSeg);		//д������ֶκ�
	buffer.WriteFloat(m_fWidth);			//д����
	buffer.WriteFloat(m_fThick);			//д����
	buffer.WriteFloat(m_fHigh);				//д��߶�
	buffer.WriteWord(m_wLength);			//д�볤��(mm)
	buffer.WriteFloat(m_fWeight);			//д�뵥��(kg)
	buffer.WriteString(m_sPartNo);
	buffer.WriteString(m_sSpec);
	buffer.WriteString(m_sNote);
	//2��ʵ����Ϣ
	DWORD dwSolidSize=bIncSolidData?solid.BufferLength():0;
	buffer.WriteDword(dwSolidSize);
	if(dwSolidSize>0)
		buffer.Write(solid.BufferPtr(),solid.BufferLength());
	//3��װ����Ϣ
	buffer.WriteInteger(acsPosList.GetNodeNum());
	for(ASSEMBLEY_POS *pPos=acsPosList.GetFirst();pPos;pPos=acsPosList.GetNext())
	{
		buffer.WriteDword(pPos->GetKey());
		pPos->ToBuffer(buffer);
	}
	buffer.WriteByte(m_cPlaceMode);
	buffer.WriteByte(m_cPlaceStateFromFile);
	buffer.WriteByte(m_cPriorPlaceState);
	//4��������Ϣ
	WriteProcessBuffer(buffer,version,true,TAP_BUFFER);
	//5����˨������
	buffer.WriteInteger(m_xBoltInfoList.GetNodeNum());
	for(BOLT_INFO *pBolt=m_xBoltInfoList.GetFirst();pBolt;pBolt=m_xBoltInfoList.GetNext())
		buffer.WriteByte(pBolt->cFuncType);
	//6.����ԭʼ������Ϣ����
	CBuffer *pBuf=GetOrgProcessBuffer();
	if(pBuf)
	{
		buffer.WriteInteger(pBuf->GetLength());
		buffer.Write(pBuf->GetBufferPtr(),pBuf->GetLength());
	}
	else 
		buffer.WriteInteger(0);
}
#endif
////////////////////////////////////////////////////////////////////
// CUT_POINT
void CUT_POINT::ToBuffer(CBuffer &buffer,long version)
{
	buffer.WriteFloat(fInAngle);
	buffer.WriteFloat(fOutAngle);
	buffer.WriteByte(cInLineLen);
	buffer.WriteByte(cOutLineLen);
}
void CUT_POINT::FromBuffer(CBuffer &buffer,long version)
{
	buffer.ReadFloat(&fInAngle);
	buffer.ReadFloat(&fOutAngle);
	buffer.ReadByte(&cInLineLen);
	buffer.ReadByte(&cOutLineLen);
}
#ifdef __PROPERTYLIST_UI_
IMPLEMENT_PROP_FUNC(CUT_POINT)
void CUT_POINT::InitPropHashtable()
{
	int id=1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"�и������"));
	AddPropItem("hEntId",PROPLIST_ITEM(id++,"��������Id"));
	AddPropItem("InLine",PROPLIST_ITEM(id++,"������"));
	AddPropItem("cInLineLen",PROPLIST_ITEM(id++,"����"));
	AddPropItem("fInAngle",PROPLIST_ITEM(id++,"�޶���"));
	AddPropItem("OutLine",PROPLIST_ITEM(id++,"������"));
	AddPropItem("cOutLineLen",PROPLIST_ITEM(id++,"����"));
	AddPropItem("fOutAngle",PROPLIST_ITEM(id++,"�޶���"));
}
int CUT_POINT::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(GetPropID("hEntId")==id)
		sText.Printf("%d",hEntId);
	else if(GetPropID("cInLineLen")==id)
		sText.Printf("%d",cInLineLen);
	else if(GetPropID("fInAngle")==id)
		sText.Copy(CXhChar50(fInAngle));
	if(GetPropID("cOutLineLen")==id)
		sText.Printf("%d",cOutLineLen);
	else if(GetPropID("fOutAngle")==id)
		sText.Copy(CXhChar50(fOutAngle));
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
#endif
////////////////////////////////////////////////////////////////////
// CProcessPlate
#ifdef __PROPERTYLIST_UI_
IMPLEMENT_PROP_FUNC(CProcessPlate)
void CProcessPlate::InitPropHashtable()
{
	int id=1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"�ְ������Ϣ"));
	AddPropItem("m_sPartNo",PROPLIST_ITEM(id++,"�������"));
	AddPropItem("m_Seg",PROPLIST_ITEM(id++,"�ֶκ�"));
	AddPropItem("m_cMaterial",PROPLIST_ITEM(id++,"����"));
	AddPropItem("m_fThick",PROPLIST_ITEM(id++,"���"));
	AddPropItem("m_fOrgThick",PROPLIST_ITEM(id++,"ԭ���"));
	AddPropItem("m_fWeight",PROPLIST_ITEM(id++,"����"));
	AddPropItem("m_cFuncType",PROPLIST_ITEM(id++,"��������","�ְ幦��","��ͨ�ְ�|�Ӿ��߰�|�Ŷ��ְ�|���߰�|����̤��|��װ�����Ұ�|���Ű�"));
	AddPropItem("WeldingRelation",PROPLIST_ITEM(id++,"���ӹ�ϵ","�뵱ǰ����������һ��Ĺ���"));
	AddPropItem("m_sNote",PROPLIST_ITEM(id++,"��ע"));
	AddPropItem("m_sRelPartNo",PROPLIST_ITEM(id++,"��������"));
	AddPropItem("m_bDeformed",PROPLIST_ITEM(id++,"��������","","0.δ����|1.�ѿ���"));
	AddPropItem("m_ciDeformType",PROPLIST_ITEM(id++,"�������η�ʽ","","0.װ����|1.���Բ�|2.͹��|3.����"));
	AddPropItem("m_ciRollProcessType",PROPLIST_ITEM(id++,"��ߴ���ʽ","","0.�������|1.���ٵ���1|2.���ٵ���2"));
	AddPropItem("m_ciRollOffsetType",PROPLIST_ITEM(id++,"���ƫ�Ʒ�ʽ","","0.���|1.�ڲ�"));
	AddPropItem("m_dwInheritPropFlag",PROPLIST_ITEM(id++,"ͬ��ģʽ","","...."));
	AddPropItem("profile",PROPLIST_ITEM(id++,"����"));
	AddPropItem("m_cFaceN",PROPLIST_ITEM(id++,"�ְ�����","","1.�����|2.˫���|3.�����|"));
	AddPropItem("HuoquFace[0]",PROPLIST_ITEM(id++,"��һ������"));
	AddPropItem("HuoQuFaceNorm[0]",PROPLIST_ITEM(id++,"�����淨��"));
	AddPropItem("HuoQuFaceNorm[0].x",PROPLIST_ITEM(id++,"�����淨��X�������"));
	AddPropItem("HuoQuFaceNorm[0].y",PROPLIST_ITEM(id++,"�����淨��Y�������"));
	AddPropItem("HuoQuFaceNorm[0].z",PROPLIST_ITEM(id++,"�����淨��Z�������"));
	AddPropItem("HuoQuLine[0].startPt",PROPLIST_ITEM(id++,"������ʼ�˵�"));
	AddPropItem("HuoQuLine[0].startPt.x",PROPLIST_ITEM(id++,"������ʼ�˵�X�������"));
	AddPropItem("HuoQuLine[0].startPt.y",PROPLIST_ITEM(id++,"������ʼ�˵�Y�������"));
	AddPropItem("HuoQuLine[0].startPt.z",PROPLIST_ITEM(id++,"������ʼ�˵�Z�������"));
	AddPropItem("HuoQuLine[0].endPt",PROPLIST_ITEM(id++,"�������ն˵�"));
	AddPropItem("HuoQuLine[0].endPt.x",PROPLIST_ITEM(id++,"�������ն˵�X�������"));
	AddPropItem("HuoQuLine[0].endPt.y",PROPLIST_ITEM(id++,"�������ն˵�Y�������"));
	AddPropItem("HuoQuLine[0].endPt.z",PROPLIST_ITEM(id++,"�������ն˵�Z�������"));
	AddPropItem("HuoquFace[1]",PROPLIST_ITEM(id++,"�ڶ�������"));
	AddPropItem("HuoQuFaceNorm[1]",PROPLIST_ITEM(id++,"�����淨��"));
	AddPropItem("HuoQuFaceNorm[1].x",PROPLIST_ITEM(id++,"�����淨��X�������"));
	AddPropItem("HuoQuFaceNorm[1].y",PROPLIST_ITEM(id++,"�����淨��Y�������"));
	AddPropItem("HuoQuFaceNorm[1].z",PROPLIST_ITEM(id++,"�����淨��Z�������"));
	AddPropItem("HuoQuLine[1].startPt",PROPLIST_ITEM(id++,"������ʼ�˵�"));
	AddPropItem("HuoQuLine[1].startPt.x",PROPLIST_ITEM(id++,"������ʼ�˵�X�������"));
	AddPropItem("HuoQuLine[1].startPt.y",PROPLIST_ITEM(id++,"������ʼ�˵�Y�������"));
	AddPropItem("HuoQuLine[1].startPt.z",PROPLIST_ITEM(id++,"������ʼ�˵�Z�������"));
	AddPropItem("HuoQuLine[1].endPt",PROPLIST_ITEM(id++,"�������ն˵�"));
	AddPropItem("HuoQuLine[1].endPt.x",PROPLIST_ITEM(id++,"�������ն˵�X�������"));
	AddPropItem("HuoQuLine[1].endPt.y",PROPLIST_ITEM(id++,"�������ն˵�Y�������"));
	AddPropItem("HuoQuLine[1].endPt.z",PROPLIST_ITEM(id++,"�������ն˵�Z�������"));
	AddPropItem("bolt_info",PROPLIST_ITEM(id++,"��˨��Ϣ"));
	AddPropItem("M24",PROPLIST_ITEM(id++,"M24"));
	AddPropItem("M22",PROPLIST_ITEM(id++,"M22"));
	AddPropItem("M20",PROPLIST_ITEM(id++,"M20"));
	AddPropItem("M18",PROPLIST_ITEM(id++,"M18"));
	AddPropItem("M16",PROPLIST_ITEM(id++,"M16"));
	AddPropItem("Place",PROPLIST_ITEM(id++,"ռλ"));
	AddPropItem("m_cPlaceMode",PROPLIST_ITEM(id++,"ռλģʽ","ռλģʽ","0.����ռλ|1.4ռλģʽ1|2.4ռλģʽ2|3.8ռλģʽ1|4.8ռλģʽ2"));
	AddPropItem("POS_FRONT_PLUS",PROPLIST_ITEM(id++,"ǰ","","���|ɾ��"));
	AddPropItem("POS_BACK_PLUS",PROPLIST_ITEM(id++,"��","","���|ɾ��"));
	AddPropItem("POS_LEFT_PLUS",PROPLIST_ITEM(id++,"��","","���|ɾ��"));
	AddPropItem("POS_RIGHT_PLUS",PROPLIST_ITEM(id++,"��","","���|ɾ��"));
	AddPropItem("POS_FRONT_MINUS",PROPLIST_ITEM(id++,"ǰX-","","���|ɾ��"));
	AddPropItem("POS_BACK_MINUS",PROPLIST_ITEM(id++,"��X-","","���|ɾ��"));
	AddPropItem("POS_LEFT_MINUS",PROPLIST_ITEM(id++,"��Y-","","���|ɾ��"));
	AddPropItem("POS_RIGHT_MINUS",PROPLIST_ITEM(id++,"��Y-","","���|ɾ��"));
	//
	InitSyncProp();
}

int CProcessPlate::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(GetPropID("m_sPartNo")==id)
		sText.Copy(GetPartNo());
	else if(GetPropID("m_Seg")==id)
		sText.Copy((char*)GetSegI().ToString());
	else if(GetPropID("m_cMaterial")==id)	
	{
		char matStr[20];
		CProcessPart::QuerySteelMatMark(cMaterial,matStr);
		sText.Printf("%s",matStr);
	}
	else if (GetPropID("cQuality") == id)
	{
		if(cQuality!=0)
			sText.Printf("%C", cQuality);
	}
	else if(GetPropID("m_fThick")==id)
		sText.Printf("%d",(int)m_fThick);
#ifdef __TAP_CONTEXT_
	else if (GetPropID("m_fOrgThick")==id)
		sText.Printf("%d",(int)m_fOrgThick);
#endif
	else if(GetPropID("m_fWeight")==id)
	{
		sText.Printf("%f",m_fWeight);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_cFuncType")==id)
	{
		if(CProcessPlate::FUNC_REINFORCE==m_cFuncType)
			sText.Copy("�Ӿ��߰�");
		else if(CProcessPlate::FUNC_FOOTNAIL==m_cFuncType)
			sText.Copy("�Ŷ��ְ�");
		else if(CProcessPlate::FUNC_WIREPLATE==m_cFuncType)
			sText.Copy("���߰�");
		else if(CProcessPlate::FUNC_STEPPLATE==m_cFuncType)
			sText.Copy("����̤��");
		else if(CProcessPlate::FUNC_SETUPPLATE==m_cFuncType)
			sText.Copy("��װ�����Ұ�");
		else if(CProcessPlate::FUNC_FOOTPLATE==m_cFuncType)
			sText.Copy("���Ű�");
		else //if(CProcessPlate::FUNC_COMMON==m_cFuncType)
			sText.Copy("��ͨ�ְ�");
	}
	else if(GetPropID("m_sNote")==id)
		sText.Copy(GetNotes());
	else if(GetPropID("m_bDeformed")==id)
	{
		if(m_bIncDeformed)
			sText.Copy("�ѿ���");
		else
			sText.Copy("δ����");
	}
	else if(GetPropID("m_sRelPartNo")==id)
		sText.Copy(m_sRelatePartNo);
	else if(GetPropID("m_ciDeformType")==id)
	{
		if(m_ciDeformType==0)
			sText.Copy("0.װ����");
		else if(m_ciDeformType==1)
			sText.Copy("1.���Բ�");
		else if(m_ciDeformType==2)
			sText.Copy("2.͹��");
		else if(m_ciDeformType==3)
			sText.Copy("3.����");
	}
	else if(GetPropID("m_ciRollProcessType")==id)
	{
		if(m_ciRollProcessType==0)
			sText.Copy("0.�������");
		else if(m_ciRollProcessType==1)
			sText.Copy("1.���ٵ���1");
		else if(m_ciRollProcessType==2)
			sText.Copy("2.���ٵ���2");
	}
	else if(GetPropID("m_ciRollOffsetType")==id)
	{
		if(m_ciRollOffsetType==0)
			sText.Copy("0.���");
		else if(m_ciRollOffsetType==1)
			sText.Copy("1.�ڲ�");
	}
	else if(GetPropID("m_cFaceN")==id)
	{
		if(m_cFaceN==1)
			sText.Copy("1.�����");
		else if(m_cFaceN==2)
			sText.Copy("2.˫���");
		else if(m_cFaceN==3)
			sText.Copy("3.�����");
	}
	else if(GetPropID("HuoQuFaceNorm[0]")==id)
		sText.ConvertFromPoint(HuoQuFaceNorm[0],3);
	else if(GetPropID("HuoQuFaceNorm[0].x")==id)
	{
		sText.Printf("%f",HuoQuFaceNorm[0].x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuFaceNorm[0].y")==id)
	{
		sText.Printf("%f",HuoQuFaceNorm[0].y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuFaceNorm[0].z")==id)
	{
		sText.Printf("%f",HuoQuFaceNorm[0].z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuLine[0].startPt")==id)
		sText.ConvertFromPoint(HuoQuLine[0].startPt,3);
	else if(GetPropID("HuoQuLine[0].startPt.x")==id)
	{
		sText.Printf("%f",HuoQuLine[0].startPt.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuLine[0].startPt.y")==id)
	{
		sText.Printf("%f",HuoQuLine[0].startPt.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuLine[0].startPt.z")==id)
	{
		sText.Printf("%f",HuoQuLine[0].startPt.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuLine[0].endPt")==id)
		sText.ConvertFromPoint(HuoQuLine[0].endPt,3);
	else if(GetPropID("HuoQuLine[0].endPt.x")==id)
	{
		sText.Printf("%f",HuoQuLine[0].endPt.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuLine[0].endPt.y")==id)
	{
		sText.Printf("%f",HuoQuLine[0].endPt.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuLine[0].endPt.z")==id)
	{
		sText.Printf("%f",HuoQuLine[0].endPt.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuFaceNorm[1]")==id)
		sText.ConvertFromPoint(HuoQuFaceNorm[1],3);
	else if(GetPropID("HuoQuFaceNorm[1].x")==id)
	{
		sText.Printf("%f",HuoQuFaceNorm[1].x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuFaceNorm[1].y")==id)
	{
		sText.Printf("%f",HuoQuFaceNorm[1].y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuFaceNorm[1].z")==id)
	{
		sText.Printf("%f",HuoQuFaceNorm[1].z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuLine[1].startPt")==id)
		sText.ConvertFromPoint(HuoQuLine[1].startPt,3);
	else if(GetPropID("HuoQuLine[1].startPt.x")==id)
	{
		sText.Printf("%f",HuoQuLine[1].startPt.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuLine[1].startPt.y")==id)
	{
		sText.Printf("%f",HuoQuLine[1].startPt.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuLine[1].startPt.z")==id)
	{
		sText.Printf("%f",HuoQuLine[1].startPt.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuLine[1].endPt")==id)
		sText.ConvertFromPoint(HuoQuLine[1].endPt,3);
	else if(GetPropID("HuoQuLine[1].endPt.x")==id)
	{
		sText.Printf("%f",HuoQuLine[1].endPt.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuLine[1].endPt.y")==id)
	{
		sText.Printf("%f",HuoQuLine[1].endPt.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("HuoQuLine[1].endPt.z")==id)
	{
		sText.Printf("%f",HuoQuLine[1].endPt.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("M24")==id)
		sText.Printf("%d",GetBoltNumByD(24));
	else if(GetPropID("M22")==id)
		sText.Printf("%d",GetBoltNumByD(22));
	else if(GetPropID("M20")==id)
		sText.Printf("%d",GetBoltNumByD(20));
	else if(GetPropID("M18")==id)
		sText.Printf("%d",GetBoltNumByD(18));
	else if(GetPropID("M16")==id)
		sText.Printf("%d",GetBoltNumByD(16));
#ifdef __TAP_CONTEXT_
	else if(GetPropID("m_cPlaceMode")==id)
	{
		CXhChar100 cmbItemArr[5]={"0.����ռλ","1.4ռλģʽ1","2.4ռλģʽ2","3.8ռλģʽ1","4.8ռλģʽ2"};
		if(m_cPlaceMode>=0||m_cPlaceMode<=4)
			sText.Copy(cmbItemArr[m_cPlaceMode]);
		else
			sText.Empty();
	}
	else if(GetPropID("POS_FRONT_PLUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_FRONT_PLUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_FRONT_PLUS));
		else 
			sText.Copy("��");

	}
	else if(GetPropID("POS_BACK_PLUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_BACK_PLUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_BACK_PLUS));
		else 
			sText.Copy("��");
	}
	else if(GetPropID("POS_LEFT_PLUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_LEFT_PLUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_LEFT_PLUS));
		else 
			sText.Copy("��");
	}
	else if(GetPropID("POS_RIGHT_PLUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_RIGHT_PLUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_RIGHT_PLUS));
		else 
			sText.Copy("��");
	}
	else if(GetPropID("POS_FRONT_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_FRONT_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_FRONT_MINUS));
		else 
			sText.Copy("��");
	}
	else if(GetPropID("POS_BACK_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_BACK_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_BACK_MINUS));
		else 
			sText.Copy("��");
	}
	else if(GetPropID("POS_LEFT_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_LEFT_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_LEFT_MINUS));
		else 
			sText.Copy("��");
	}
	else if(GetPropID("POS_RIGHT_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_RIGHT_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_RIGHT_MINUS));
		else 
			sText.Copy("��");
	}
#endif
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
#endif
CProcessPlate::CProcessPlate()
{
	m_cPartType=TYPE_PLATE;
	m_cFaceN=0;
	m_fInnerRadius=0;
	mcsFlg.ciBottomEdge=255;	//�൱��0xff������ǰ��-1
	mcsFlg.ciOverturn=0;
	m_bIncDeformed=false;
	m_ciDeformType=0;
	m_ciRollProcessType=0;
	m_ciRollOffsetType=1;
	//�����ߵĻ����뾶�����Բ�ϵ����Ҫ��������ְ�NC����ʱ���ǻ���������   wxc-2016.10.27
	m_fHuoQuLineR[0]=m_fHuoQuLineR[1]=0;		//
	m_fHuoQuLineX[0]=m_fHuoQuLineX[1]=0.35;		//
}
void CProcessPlate::CutPointToBuffer(CBuffer &buffer,long version/*=0*/)
{
	if(version<1000005)
		return;
	if(vertex_list.GetValue(m_xCutPt.hEntId))
	{
		buffer.WriteInteger(m_xCutPt.hEntId);
		m_xCutPt.ToBuffer(buffer,version);
	}
	else
		buffer.WriteInteger(0);
	buffer.WriteInteger(m_hashHoleCutPtByKey.GetNodeNum());
	for(CUT_POINT *pPt=m_hashHoleCutPtByKey.GetFirst();pPt;pPt=m_hashHoleCutPtByKey.GetNext())
	{
		if(m_xBoltInfoList.GetValue(pPt->hEntId))
		{
			buffer.WriteInteger(pPt->hEntId);
			pPt->ToBuffer(buffer,version);
		}
		else 
			buffer.WriteInteger(0);
	}
}
void CProcessPlate::CutPointFromBuffer(CBuffer &buffer,long version/*=0*/)
{
	if(version<1000005)
		return;
	buffer.ReadInteger(&m_xCutPt.hEntId);
	if(m_xCutPt.hEntId>0)
		m_xCutPt.FromBuffer(buffer,version);
	//
	m_hashHoleCutPtByKey.Empty();
	long hKey=0,n=buffer.ReadInteger();
	for(long i=0;i<n;i++)
	{
		hKey=buffer.ReadInteger();
		if(hKey>0)
		{
			CUT_POINT *pPt=m_hashHoleCutPtByKey.Add(hKey);
			pPt->FromBuffer(buffer,version);
		}
	}
}
CUT_POINT *CProcessPlate::FromCutPtHiberId(HIBERID hiberId,BYTE cType/*=CUT_PT_CIR*/)
{
	if(m_xCutPt.GetHiberId(GetKey()).IsEqual(hiberId))
		return &m_xCutPt;
	for(CUT_POINT *pCutPt=m_hashHoleCutPtByKey.GetFirst();pCutPt;pCutPt=m_hashHoleCutPtByKey.GetNext())
	{
		if(cType==CProcessPlate::CUT_PT_CIR)
		{
			if(pCutPt->GetHiberId(GetKey()).IsEqual(hiberId))
				return pCutPt;
		}
		else if(cType==CProcessPlate::CUT_IN_LINE)
		{
			if(pCutPt->GetInLineHiberId(GetKey()).IsEqual(hiberId))
				return pCutPt;
		}
		else if(cType==CProcessPlate::CUT_OUT_LINE)
		{
			if(pCutPt->GetOutLineHiberId(GetKey()).IsEqual(hiberId))
				return pCutPt;
		}
	}
	return NULL;
}
bool CProcessPlate::IsConcaveVertex(int iVertexIndex)
{
	int i=0,n = vertex_list.GetNodeNum();
	DYN_ARRAY<GEPOINT> vertex_arr(n);
	PROFILE_VER *pCurVertex=NULL,*pPreVertex = vertex_list.GetTail();
	for (PROFILE_VER *pVertex = vertex_list.GetFirst(); pVertex; pVertex = vertex_list.GetNext(), i++)
	{
		vertex_arr[i] = pVertex->vertex;
		if (i == iVertexIndex)
			pCurVertex = pVertex;
		if (i < iVertexIndex )
			pPreVertex = pVertex;
	}
	if (pPreVertex->type != 1 || pCurVertex->type != 1)
		return false;
	f3dPoint ptPre = vertex_arr[(iVertexIndex-1+n)%n];
	f3dPoint ptCur = vertex_arr[iVertexIndex];
	f3dPoint ptNex = vertex_arr[(iVertexIndex+1)%n];
	double result = DistOf2dPtLine(ptNex, ptPre, ptCur);
	if (result < 0)	// ��������Ҳ࣬�а��ǳ���
		return true;
	return false;
}
bool CProcessPlate::IsInitedMCS()
{
	if(mcsFlg.ciBottomEdge==-1||mcsFlg.ciBottomEdge>=vertex_list.GetNodeNum())
		return false;
	else
		return true;
}
void CProcessPlate::InitMCS()
{
	if(IsInitedMCS())
		return;
	//��ʼ���ӹ�����ϵ
	int i=0,n=vertex_list.GetNodeNum();
	DYN_ARRAY<GEPOINT> vertex_arr(n);
	for(PROFILE_VER *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext(),i++)
		vertex_arr[i]=pVertex->vertex;
	GEPOINT vertice,prev_vec,edge_vec;
	double prev_edge_dist=0, edge_dist = 0, max_edge = 0;
	mcsFlg.ciBottomEdge=0;
	for(i=0;i<n;i++)
	{
		edge_vec=vertex_arr[(i+1)%n]-vertex_arr[i];
		edge_dist = edge_vec.mod();
		edge_vec/=edge_dist;	//��λ����ʸ��
		if(i>0&&prev_vec*edge_vec>EPS_COS)	//�������߱�����
			edge_dist+=edge_dist+prev_edge_dist;
		if(edge_dist>max_edge)
		{
			max_edge = edge_dist;
			mcsFlg.ciBottomEdge=i;
		}
		prev_edge_dist=edge_dist;
		prev_vec=edge_vec;
	}
}

SCOPE_STRU CProcessPlate::GetVertexsScope(GECS *pCS/*=NULL*/)
{
	SCOPE_STRU scope;
	scope.ClearScope();
	int index=0;
	PROFILE_VER* pPreVertex=vertex_list.GetTail();
	for(PROFILE_VER *pVertex=vertex_list.GetFirst();pVertex;index++,pVertex=vertex_list.GetNext())
	{
		f3dPoint pt=pPreVertex->vertex;
		if(pCS)
			coord_trans(pt,*pCS,FALSE);
		scope.VerifyVertex(pt);
		if (pPreVertex->type <= 1)
		{
			pPreVertex = pVertex;
			continue;
		}
		f3dArcLine arcLine;
		if(pPreVertex->type==2)
		{
			arcLine.CreateMethod2(pPreVertex->vertex,pVertex->vertex,pPreVertex->work_norm,pPreVertex->sector_angle);
			if(arcLine.WorkNorm().z<=0)
			{
				pPreVertex=pVertex;
				continue;
			}
			//͹��ȡ���е������������
			double half_angle = arcLine.SectorAngle()*0.5;
			GEPOINT tem_pt = arcLine.PositionInAngle(half_angle);
			if (pCS)
				coord_trans(tem_pt, *pCS, FALSE);
			scope.VerifyVertex(tem_pt);
		}
		else if(pPreVertex->type==3)
		{
			arcLine.CreateEllipse(pPreVertex->center,pPreVertex->vertex,pVertex->vertex,pPreVertex->column_norm,
				pPreVertex->work_norm,pPreVertex->radius);
			if(arcLine.WorkNorm().z<=0)
			{
				pPreVertex=pVertex;
				continue;
			}
		}
		//��Բ�����зֶμ򻯴���,��ȡ�ְ����С��������
		int nSlices = CalArcResolution(arcLine.Radius(), arcLine.SectorAngle(), 1.0, 5.0, 18);
		double angle = arcLine.SectorAngle() / nSlices;
		for (int i = 1; i < nSlices; i++)
		{
			GEPOINT tem_pt = arcLine.PositionInAngle(angle*i);
			if (pCS)
				coord_trans(tem_pt, *pCS, FALSE);
			scope.VerifyVertex(tem_pt);
		}
		pPreVertex=pVertex;
	}
	return scope;
}

BOOL CProcessPlate::GetMCS(GECS &mcs)
{
	int i=0,n=vertex_list.GetNodeNum();
	if(n<3)
		return FALSE;
	DYN_ARRAY<GEPOINT> vertex_arr(n);
	for(PROFILE_VER *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext(),i++)
		vertex_arr[i]=pVertex->vertex;
	if(!IsInitedMCS())	
		InitMCS();	//��ʼ���ӹ�����ϵ
	//����bottom_edge_i����ӹ�����ϵ
	int bottom_edge=mcsFlg.ciBottomEdge;
	f3dPoint cur_pt=vertex_arr[bottom_edge];
	f3dPoint next_pt=vertex_arr[(bottom_edge+1)%n];
	if(IsConcaveVertex((bottom_edge+1)%n))
		next_pt=vertex_arr[(bottom_edge+2)%n];
	mcs.origin=cur_pt;
	mcs.axis_x=next_pt-cur_pt;
	normalize(mcs.axis_x);
	mcs.axis_y.Set(-mcs.axis_x.y,mcs.axis_x.x);
	mcs.axis_z=mcs.axis_x^mcs.axis_y;
	//��������ϵԭ��
	SCOPE_STRU scope=GetVertexsScope(&mcs);
	mcs.origin+=scope.fMinX*mcs.axis_x;
	mcs.origin+=scope.fMinY*mcs.axis_y;
	//����ְ���з�ת�����¸ְ�����ϵ
	if(mcsFlg.ciOverturn==TRUE)
	{
		mcs.origin+=((scope.fMaxX-scope.fMinX)*mcs.axis_x);
		mcs.axis_x*=-1.0;
		mcs.axis_z*=-1.0;
	}
	return TRUE;
}

void CProcessPlate::TransPlateToMCS(CProcessPlate *pPlate,GECS &mcs)
{
	if(pPlate==NULL)
		return;
	//1.������Ϣ���ְ�̽�����ϵ�£�
	for(int i=0;i<pPlate->m_cFaceN-1;i++)
	{
		vector_trans(pPlate->HuoQuFaceNorm[i],mcs,FALSE);
		coord_trans(pPlate->HuoQuLine[i].startPt,mcs,FALSE);
		coord_trans(pPlate->HuoQuLine[i].endPt,mcs,FALSE);
	}
	//2.��˨��Ϣ
	for(BOLT_INFO *pBolt=pPlate->m_xBoltInfoList.GetFirst();pBolt;pBolt=pPlate->m_xBoltInfoList.GetNext())
	{
		f3dPoint ls_pos(pBolt->posX,pBolt->posY,0);
		coord_trans(ls_pos,mcs,FALSE);
		pBolt->posX=(float)ls_pos.x;
		pBolt->posY=(float)ls_pos.y;
	}
	//3.������
	if(pPlate->m_cFaceN==3 && !pPlate->top_point.IsZero())
		coord_trans(pPlate->top_point,mcs,FALSE);
	PROFILE_VER *pVertex=NULL;
	for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
	{	
		coord_trans(pVertex->vertex,mcs,FALSE);
		if(pVertex->type>0)
		{
			vector_trans(pVertex->work_norm,mcs,FALSE);
			if(pVertex->type==3)
			{
				coord_trans(pVertex->center,mcs,FALSE);
				vector_trans(pVertex->column_norm,mcs,FALSE);	
			}
		}
	}
}
#ifdef __TAP_CONTEXT_
BOOL CProcessPlate::IsModifyedThick()
{
	if(m_fOrgThick==0||m_fOrgThick==m_fThick)
		return FALSE;
	else
		return TRUE;
}

bool CProcessPlate::IsReplacePart()	//�Ƿ�Ϊ���Ϲ���
{
	if(m_cOrgMaterial!=0&&m_cOrgMaterial!=_cMaterial)
		return true;
	if(m_fOrgThick!=0&&m_fOrgThick!=m_fThick)
		return true;
	return false;
}
void CProcessPlate::FromTIDBuffer(CBuffer &buffer,long version/*=0*/)
{
	CProcessPart::FromTIDBuffer(buffer,version);
	if(!IsModifyedThick())
		m_fOrgThick=m_fThick;
}
void CProcessPlate::FromTAPBuffer(CBuffer &buffer,long version/*=0*/)
{
	CProcessPart::FromTAPBuffer(buffer,version);
	if(version>=1000600||version==NULL)
	{
		buffer.ReadFloat(&m_fOrgThick);
		if(!IsModifyedThick())
			m_fOrgThick=m_fThick;
	}
}
void CProcessPlate::ToTAPBuffer(CBuffer &buffer,bool bIncSolidData/*=true*/)
{
	CProcessPart::ToTAPBuffer(buffer,bIncSolidData);
	if(IsModifyedThick())
		buffer.WriteFloat(m_fOrgThick);
	else 
		buffer.WriteFloat((float)0);
}
#endif
void CProcessPlate::InitCutPt(bool bFarOrg)
{
#ifdef __PNC_
	if(!IsInitedMCS())
		InitMCS();
	//
	if(vertex_list.GetValue(m_xCutPt.hEntId))
		return;
	if(mcsFlg.ciOverturn)
	{
		m_xCutPt.hEntId=mcsFlg.ciBottomEdge+1;
		if(!bFarOrg)
			m_xCutPt.hEntId+=1;
		if(m_xCutPt.hEntId>vertex_list.GetNodeNum())
			m_xCutPt.hEntId=1;
	}
	else
	{
		m_xCutPt.hEntId=mcsFlg.ciBottomEdge+1;
		if(bFarOrg)
			m_xCutPt.hEntId+=1;
		if(m_xCutPt.hEntId>vertex_list.GetNodeNum())
			m_xCutPt.hEntId=1;
	}
	//��ʼ����˨�������
#endif
}
bool CProcessPlate::ClonePart(CProcessPart *pNewPart)
{
	bool bRetCode=CProcessPart::ClonePart(pNewPart);
	if(bRetCode)
	{	//������˨������id
		CProcessPlate *pPlate=(CProcessPlate*)pNewPart;
		pPlate->m_xBoltInfoList.Empty();
		pPlate->vertex_list.Empty();
		for(BOLT_INFO* pBolt=m_xBoltInfoList.GetFirst();pBolt!=NULL;pBolt=m_xBoltInfoList.GetNext())
			pPlate->m_xBoltInfoList.Append(*pBolt,m_xBoltInfoList.GetCursorKey());
		for(PROFILE_VER* pVertex=vertex_list.GetFirst();pVertex!=NULL;pVertex=vertex_list.GetNext())
			pPlate->vertex_list.Append(*pVertex,vertex_list.GetCursorKey());
	}
	return bRetCode;
}
void CProcessPlate::WriteProcessBuffer(CBuffer& buffer,long version/*=0*/,bool bSaveSubSerial/*=false*/,int buf_type/*=0*/)
{
	//1.����������������
	buffer.WriteWord((short)m_xBoltInfoList.GetNodeNum());	//����
	buffer.WriteWord((short)vertex_list.GetNodeNum());		//����
	buffer.WriteByte(m_cFaceN);								//����
	//2.������Ϣ���ְ�̽�����ϵ�£�
	for(BYTE i=0;i<m_cFaceN-1;i++)
	{
		buffer.WritePoint(HuoQuFaceNorm[i]);				//(���������ϵ�µ�)�����淨��
		buffer.WritePoint(HuoQuLine[i].startPt);			//(���������ϵ�µ�)���������
		buffer.WritePoint(HuoQuLine[i].endPt);				//(���������ϵ�µ�)�������յ�
		if(buf_type==CProcessPart::PPI_BUFFER && version>=1000003)
		{
			buffer.WriteDouble(m_fHuoQuLineR[i]);
			buffer.WriteDouble(m_fHuoQuLineX[i]);
		}
	}
	if(buf_type==CProcessPart::PPI_BUFFER && version>=1000005)
	{
		buffer.WriteByte(m_ciRollProcessType);	//��ߴ���ʽ
		buffer.WriteByte(m_ciRollOffsetType);	//����߼��㷽ʽ
		buffer.WriteBoolean(m_bIncDeformed);	//�Ƿ��ǻ�������
		buffer.WriteByte(m_ciDeformType);		//�������δ���ʽ
	}
	BOLT_INFO *pBolt=m_xBoltInfoList.GetFirst();
	if(pBolt)
		buffer.WriteFloat(pBolt->hole_d_increment);	//��˨ֱ������
	else 
		buffer.WriteFloat((float)0);
	//3.��˨��Ϣ
	//4B��X����λ�ã�float��
	//4B��Y����λ�ã�float����
	//2B����˨�׾�
	//2B����Բ������
	//8B����˨���ڵ����ߺ�
	//24B����Բ�׷���
	for(pBolt=m_xBoltInfoList.GetFirst();pBolt;pBolt=m_xBoltInfoList.GetNext())
	{
		if(bSaveSubSerial)
			buffer.WriteInteger(pBolt->keyId);
		pBolt->ToBuffer(buffer,version,buf_type);
	}
	//4.������
	if(buf_type==CProcessPart::PPI_BUFFER && version>=1000003&&version<1000005)
		buffer.WriteBoolean(m_bIncDeformed);
	if(m_cFaceN==3)
		buffer.WritePoint(top_point);
	PROFILE_VER *pVertext= NULL;
	for(pVertext=vertex_list.GetFirst();pVertext;pVertext=vertex_list.GetNext())
	{	
		if(bSaveSubSerial)
			buffer.WriteDword(pVertext->keyId);
		//4.1 1B��������(1��ʾֱ�߶Σ�2��ʾԲ����3��ʾ��Բ����
		buffer.WriteByte(pVertext->type);
		//4.2 1B���Ա�ʶ�ֽ�
		//1~4λ��ʾ�������棨��0x03��ʾ���ͬʱ�����ڵ�һ��͵ڶ��棬��1-2��������ϣ�
		//��8λΪ0ʱ���λ��֮��ĺ���2�ֽڱ�ʾ��߸߶ȣ�Ϊ1ʱ��ʾ����ߵļӹ���϶ֵ
		//����λ����
		BYTE  propId=0;
		if(pVertext->vertex.feature==2)
			propId = 0x02;
		else if(pVertext->vertex.feature==12)
			propId = 0x03;
		else if(pVertext->vertex.feature==3)
			propId =0x04;
		else if(pVertext->vertex.feature ==13)
			propId =0x05;
		else //if(vertex.feature==1)
			propId =0x01;
		//4.3 ���졢���
		if(pVertext->m_bRollEdge)
			propId &=0x7F;								//��ʶ���
		else if(pVertext->m_bWeldEdge)
			propId |=0x80;								//��ʶ����
		buffer.WriteByte(propId);
		buffer.WriteDouble(pVertext->vertex.x);			//�����X����
		buffer.WriteDouble(pVertext->vertex.y);			//�����Y����
		buffer.WriteWord(pVertext->manu_space);			//����ӹ�Ԥ����϶���߸߶�(mm)
		if(buf_type==CProcessPart::PPI_BUFFER &&version>=1000006)
			buffer.WriteWord(pVertext->roll_edge_offset_dist);	//���ƫ�ƾ���
		if(pVertext->m_bWeldEdge)
		{	
			buffer.WriteByte((BYTE)0);		//1B������߶�
			buffer.WriteWord((WORD)0);		//2B������������������
			buffer.WriteWord((WORD)0);		//2B�����쳤�ȣ�Ϊ0ʱ��ʾһֱ����������յ�
		}
		//4.4 ������=2��3ʱ�����֣�Բ�����νǡ�Բ���淨�߷���
		if(pVertext->type>1)
		{	//8B�����֣�Բ�����νǣ�������
			buffer.WriteDouble(pVertext->sector_angle);
			//24B��Բ���淨�߷��򣨹̽�����ϵ����Է���
			buffer.WritePoint(pVertext->work_norm);  //Բ���淨��
			//4.5  ������=3ʱ����Բ���̰��᳤�ȡ���Բ��Բ��λ�á���Բ��ԭʼ������������
			if(pVertext->type==3)
			{
				buffer.WriteDouble(pVertext->radius	);		//8B����Բ���̰��᳤�ȣ���ԭʼ����Բ����뾶��
				buffer.WritePoint(pVertext->center);		//24B����Բ��Բ��λ�ã��̽�����ϵ��������꣩
				buffer.WritePoint(pVertext->column_norm);	//24B����Բ��ԭʼ�����������򣨹̽�����ϵ����Է���
			}
		}
	}
	//5.�ӹ�����ϵ��׼�����������λ��
	buffer.WriteWord((WORD)mcsFlg.wFlag);		//�ӹ�����ϵ��׼������
	buffer.WriteFloat((float)mkpos.x);			//���λ��X����
	buffer.WriteFloat((float)mkpos.y);			//���λ��Y����
	if(buf_type==CProcessPart::PPI_BUFFER && version>=1000002)
	{	//��¼���θ�ӡ�ŵķ���
		buffer.WriteFloat((float)mkVec.x);
		buffer.WriteFloat((float)mkVec.y);	
	}
}
//�����߻����߼������λ��
void CProcessPlate::CalRollLineAndHuoquLine(PROFILE_VER *pRollStart,PROFILE_VER *pRollEnd,PROFILE_VER *pPreVertex,PROFILE_VER *pNextVertex,
	f3dLine &huoqu_line,f3dLine &roll_line)
{
	if(pRollStart==NULL||pRollEnd==NULL||pPreVertex==NULL||pNextVertex==NULL)
		return;
	f3dPoint prev_vec=pRollStart->vertex-pPreVertex->vertex;
	normalize(prev_vec);
	f3dPoint next_vec=pRollEnd->vertex-pNextVertex->vertex;
	normalize(next_vec);
	f3dPoint cur_vec=pRollEnd->vertex-pRollStart->vertex;
	normalize(cur_vec);
	GEPOINT offset_vec=cur_vec;	
	//TODO �˴���Ҫ���ⲿ���У��ڴ˴�������� wht 19-09-22
	if(mcsFlg.ciOverturn>0)	//�ְ巭ת�󣬾��ƫ�Ʒ���ҲӦ��ת wht 19-09-15
		offset_vec.Set(-cur_vec.y, cur_vec.x);
	else
		offset_vec.Set(offset_vec.y, -offset_vec.x);
	normalize(offset_vec);
	double cosa=fabs(cur_vec*prev_vec);//�жϵ�ǰ����ǰһ���Ƿ���
	double cosb=fabs(cur_vec*next_vec);//�жϵ�ǰ������һ���Ƿ���
	f2dLine edge_line,extend_line;
	//���ݲ�ͬ�ľ��λ�ô���ʽ��������ߺ;���ߵ�ƫ�ƾ���
	int nHuoquOffDist=0,nRollOffDist=0;
	if(m_ciRollOffsetType==0)
	{	//��෽ʽ
		nHuoquOffDist=pRollStart->roll_edge_offset_dist+ftoi(m_fThick);
		nRollOffDist=pRollStart->manu_space;
	}
	else
	{	//�ڲ෽ʽ
		nHuoquOffDist=pRollStart->roll_edge_offset_dist;
		nRollOffDist=pRollStart->manu_space+ftoi(m_fThick);
	}
	//����ƫ�ƺ�����ߵ�λ��
	huoqu_line.startPt=pRollStart->vertex+offset_vec*nHuoquOffDist;
	huoqu_line.endPt=pRollEnd->vertex+offset_vec*nHuoquOffDist;
	edge_line.startPt.Set(huoqu_line.startPt.x,huoqu_line.startPt.y);
	edge_line.endPt.Set(huoqu_line.endPt.x,huoqu_line.endPt.y);
	if(cosa<EPS_COS2)
	{	//��ǰ����ǰһ�߲�����	
		extend_line.startPt.Set(pRollStart->vertex.x,pRollStart->vertex.y);
		extend_line.endPt.Set(extend_line.startPt.x+prev_vec.x,extend_line.startPt.y+prev_vec.y);
		Int2dll(extend_line,edge_line,huoqu_line.startPt.x,huoqu_line.startPt.y);
	}
	if(cosb<EPS_COS2)
	{	//��ǰ������һ�߲�����
		extend_line.startPt.Set(pRollEnd->vertex.x,pRollEnd->vertex.y);
		extend_line.endPt.Set(extend_line.startPt.x+next_vec.x,extend_line.startPt.y+next_vec.y);
		Int2dll(extend_line,edge_line,huoqu_line.endPt.x,huoqu_line.endPt.y);
	}
	f3dPoint new_edge_vec=huoqu_line.endPt-huoqu_line.startPt;
	normalize(new_edge_vec);	//��ߺ��±߷���
	if(new_edge_vec*cur_vec<0)	//�õ����±�ʼĩ�˵ߵ�����ʼ��ȡ��ֱ����Ϊ���췽��
	{
		huoqu_line.startPt=pRollStart->vertex+offset_vec*nHuoquOffDist;
		prev_vec=offset_vec;
		huoqu_line.endPt=pRollEnd->vertex+offset_vec*nHuoquOffDist;
		next_vec=offset_vec;
	}
	//�������ߵ�λ��
	roll_line.startPt=huoqu_line.startPt+offset_vec*abs(nRollOffDist);
	roll_line.endPt=huoqu_line.endPt+offset_vec*abs(nRollOffDist);
	if(m_ciRollProcessType==1||m_ciRollProcessType==2)
	{	//���ٵ���
		edge_line.startPt.Set(roll_line.startPt.x,roll_line.startPt.y);
		edge_line.endPt.Set(roll_line.endPt.x,roll_line.endPt.y);
		if(fabs(cur_vec*prev_vec)<EPS_COS2)
		{	//��ǰ����ǰһ�߲�����	
			extend_line.startPt.Set(pRollStart->vertex.x,pRollStart->vertex.y);
			extend_line.endPt.Set(extend_line.startPt.x+prev_vec.x,extend_line.startPt.y+prev_vec.y);
			Int2dll(extend_line,edge_line,roll_line.startPt.x,roll_line.startPt.y);
		}
		if(fabs(cur_vec*next_vec)<EPS_COS2)
		{	//��ǰ������һ���߲����� 
			extend_line.startPt.Set(pRollEnd->vertex.x,pRollEnd->vertex.y);
			extend_line.endPt.Set(extend_line.startPt.x+next_vec.x,extend_line.startPt.y+next_vec.y);
			Int2dll(extend_line,edge_line,roll_line.endPt.x,roll_line.endPt.y);
		}
		new_edge_vec=roll_line.endPt-roll_line.startPt;
		normalize(new_edge_vec);	//��ߺ��±߷���
		if(new_edge_vec*cur_vec<0)	//�õ����±�ʼĩ�˵ߵ�����ʼ��ȡ��ֱ����Ϊ���췽�� wht 09-08-14
		{
			roll_line.startPt=huoqu_line.startPt+offset_vec*abs(nRollOffDist);
			roll_line.endPt=huoqu_line.endPt+offset_vec*abs(nRollOffDist);
		}
	}
	huoqu_line.startPt.z=huoqu_line.endPt.z=0;
	roll_line.startPt.z=roll_line.endPt.z=0;
}
bool CProcessPlate::IsRollEdge()
{
	for(PROFILE_VER *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pVertex->type==1&&pVertex->m_bRollEdge&&pVertex->manu_space!=0)
			return true;
	}
	return false;
}
//�����ߣ���԰��������ϵ���ԣ��õ������ְ�߶�Ӧ��
void CProcessPlate::ProcessRollEdgeVertex()
{
	if(vertex_list.GetNodeNum()<3)
		return;
	ATOM_LIST<PROFILE_VER> xVertexList;
	CHashSet<PROFILE_VER*> hashDestVertexPtrBySrcVertexPtr;
	for(PROFILE_VER* pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		PROFILE_VER *pNewVertex=xVertexList.append(*pVertex);
		hashDestVertexPtrBySrcVertexPtr.SetValue((DWORD)pVertex,pNewVertex);
	}
	int i=0,roll_edge_num=0,n=vertex_list.GetNodeNum();
	PROFILE_VER *pPrevPnt=vertex_list.GetTail(),*pTailVertex=vertex_list.GetTail();
	for(PROFILE_VER *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext(),i++)
	{
		if(pVertex->type==1&&pVertex->m_bRollEdge&&pVertex->manu_space!=0)
		{	//������
			PROFILE_VER* pRollStart=pVertex,*pRollEnd=NULL,*pNextPnt=NULL;
			int nPush=vertex_list.push_stack();
			pRollEnd=(pTailVertex==pVertex)?vertex_list.GetFirst():vertex_list.GetNext();
			pNextPnt=(pTailVertex==pRollEnd)?vertex_list.GetFirst():vertex_list.GetNext();
			vertex_list.pop_stack(nPush);
			if(pRollStart==NULL||pRollEnd==NULL||pPrevPnt==NULL||pNextPnt==NULL)
				continue;
			f3dLine huoqu_line,roll_edge_line;
			CalRollLineAndHuoquLine(pRollStart,pRollEnd,pPrevPnt,pNextPnt,huoqu_line,roll_edge_line);
			//���ǻ�����ƫ���������������Ӧ����Ӧ�����������������ʱ�����������߲���ֱ
			PROFILE_VER *pNewStartVertex=hashDestVertexPtrBySrcVertexPtr.GetValue((DWORD)pRollStart);
			PROFILE_VER *pNewEndVertex=hashDestVertexPtrBySrcVertexPtr.GetValue((DWORD)pRollEnd);
			if(pNewStartVertex)
				pNewStartVertex->vertex=huoqu_line.startPt;
			if(pNewEndVertex)
				pNewEndVertex->vertex=huoqu_line.endPt;
			PROFILE_VER new_vertex;
			new_vertex.type=1;	//ֱ��
			new_vertex.vertex=roll_edge_line.startPt;
			if((i+roll_edge_num*2+1)%(n+roll_edge_num*2)==0)
				xVertexList.append(new_vertex);
			else
				xVertexList.insert(new_vertex,(i+roll_edge_num*2+1)%(n+roll_edge_num*2));
			new_vertex.vertex=roll_edge_line.endPt;
			if((i+roll_edge_num*2+2)%(n+roll_edge_num*2+1)==0)
				xVertexList.append(new_vertex);
			else
				xVertexList.insert(new_vertex,(i+roll_edge_num*2+2)%(n+roll_edge_num*2+1));
			roll_edge_num++;
		}
		pPrevPnt=pVertex;
	}
	//����д��ְ��������
	vertex_list.Empty();
	for(PROFILE_VER* pVertex=xVertexList.GetFirst();pVertex;pVertex=xVertexList.GetNext())
		vertex_list.Append(*pVertex);
}
void CProcessPlate::CalEquidistantShape(double minDistance,ATOM_LIST<PROFILE_VER> *pDestList)
{
	CHashList<f3dArcLine> hashArcLine;
	f3dArcLine arcLine;
	PROFILE_VER* pPrevVertex=vertex_list.GetTail();
	for(PROFILE_VER *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(pPrevVertex->type==1)
		{
			pPrevVertex=pVertex;
			continue;
		}
		if(pPrevVertex->type==2)
			arcLine.CreateMethod2(pPrevVertex->vertex,pVertex->vertex,pPrevVertex->work_norm,pPrevVertex->sector_angle);
		else
			arcLine.CreateEllipse(pPrevVertex->center,pPrevVertex->vertex,pVertex->vertex,pPrevVertex->column_norm,pPrevVertex->work_norm,pPrevVertex->radius);
		hashArcLine.SetValue(pPrevVertex->keyId,arcLine);
		pPrevVertex=pVertex;
	}
	for(PROFILE_VER *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(minDistance>-eps && minDistance<eps)
		{	//
			pDestList->append(*pVertex);
			continue;
		}
		PROFILE_VER *pVertexPre=vertex_list.GetPrev();
		if(pVertexPre==NULL)
		{
			pVertexPre=vertex_list.GetTail();
			vertex_list.GetFirst();
		}
		else
			vertex_list.GetNext();
		PROFILE_VER *pVertexNext=vertex_list.GetNext();
		if(pVertexNext == NULL)
		{
			pVertexNext=vertex_list.GetFirst();
			vertex_list.GetTail();
		}
		else
			vertex_list.GetPrev();
		//
		PROFILE_VER exterVertex=*pVertex;
		if(pVertexPre->type==1 && pVertex->type==1)
		{	//ֱ�߶����Ž�ƽ���߽���ƫ��
			f3dPoint pre_vec=(pVertex->vertex-pVertexPre->vertex).normalized();
			f3dPoint cur_vec=(pVertex->vertex-pVertexNext->vertex).normalized();
			if(fabs(pre_vec*cur_vec)<0.9999)
			{
				double angle=cal_angle_of_2vec(pre_vec,cur_vec);
				double offset=minDistance/sin(angle/2);
				f3dPoint verify_vec=cur_vec^pre_vec;
				f3dPoint vec=(pre_vec+cur_vec).normalized();
				//����������ߴ��߷����������Խ���ƫ�Ʒ��򣬷��򰼵���򷴷���ƫ�� wht 18-12-05
				if( (mcsFlg.ciOverturn==0&&verify_vec.z<0)||
					(mcsFlg.ciOverturn==1&&verify_vec.z>0))
					vec*=-1.0;
				exterVertex.vertex=pVertex->vertex+vec*offset;
			}
			else
			{
				f3dPoint vec(-pre_vec.y,pre_vec.x,pre_vec.z);
				exterVertex.vertex=pVertex->vertex+vec*minDistance;
			}
		}
		else
		{	//Բ��
			f3dArcLine* pArcLine=NULL;
			if(pVertex->type>=2)	//Բ��
				pArcLine=hashArcLine.GetValue(pVertex->keyId);
			else //if(pVertexPre->type>=2)
				pArcLine=hashArcLine.GetValue(pVertexPre->keyId);
			f3dPoint center=pArcLine->Center();
			f3dPoint off_vec=(pVertex->vertex-center).normalized();
			if(pArcLine->WorkNorm().z<0)
				off_vec*=-1;
			exterVertex.vertex=pVertex->vertex+off_vec*minDistance;
			if(pVertex->type>=2)
			{
				int sign=(pVertex->radius>0)?1:-1;
				pVertex->radius+=sign*minDistance;
			}
		}
		pDestList->append(exterVertex);
	}
}
void CProcessPlate::ReadProcessBuffer(CBuffer& buffer,long version/*=0*/,bool bSaveSubSerial/*=false*/,int buf_type/*=0*/)
{
	//1.����������Ϣ
	short bolt_count;
	buffer.ReadWord(&bolt_count);			//����
	short vertex_list_count;
	buffer.ReadWord(&vertex_list_count);	//����
	buffer.ReadByte(&m_cFaceN);				//����
	//2. ������Ϣ
	for (BYTE i=0;i<m_cFaceN-1;i++)
	{
		buffer.ReadPoint(HuoQuFaceNorm[i]);				//(���������ϵ�µ�)�����淨��
		buffer.ReadPoint(HuoQuLine[i].startPt);			//(���������ϵ�µ�)���������
		buffer.ReadPoint(HuoQuLine[i].endPt);			//(���������ϵ�µ�)�������յ�
		if(buf_type==CProcessPart::PPI_BUFFER && version>=1000003)
		{
			buffer.ReadDouble(&m_fHuoQuLineR[i]);
			buffer.ReadDouble(&m_fHuoQuLineX[i]);
		}
	}
	if(buf_type==CProcessPart::PPI_BUFFER && version>=1000005)
	{
		buffer.ReadByte(&m_ciRollProcessType);	//��ߴ���ʽ
		buffer.ReadByte(&m_ciRollOffsetType);	//����߼��㷽ʽ
		buffer.ReadBoolean(&m_bIncDeformed);	//�Ƿ��ǻ�������
		buffer.ReadByte(&m_ciDeformType);		//�������δ���ʽ
	}
	float bolt_d_increment;					//��˨ֱ������
	buffer.ReadFloat(&bolt_d_increment);
	//3. ��˨��Ϣ
	m_xBoltInfoList.Empty();
	for(int i=0;i<bolt_count;i++)
	{
		BOLT_INFO boltInfo;
		if(bSaveSubSerial)
			buffer.ReadInteger(&boltInfo.keyId);
		boltInfo.hole_d_increment=bolt_d_increment;
		boltInfo.FromBuffer(buffer,version,buf_type);
		//����ϣ������keyI�ĳ�ʼ��HiberId
		m_xBoltInfoList.SetValue(boltInfo.keyId,boltInfo,true);
	}
	//4. �ְ�������Ϣ
	if(buf_type==CProcessPart::PPI_BUFFER && version>=1000003&&version<1000005)
		buffer.ReadBoolean(&m_bIncDeformed);
	if(m_cFaceN==3)
		buffer.ReadPoint(top_point);
	vertex_list.Empty();
	for(int i=0;i<vertex_list_count;i++)
	{
		PROFILE_VER profile_vertex;
		if(bSaveSubSerial)
			buffer.ReadDword(&profile_vertex.keyId);
		buffer.ReadByte(&profile_vertex.type);		//������(1��ʾֱ�߶Σ�2��ʾԲ����3��ʾ��Բ����
		BYTE cProcessFlag=0;
		buffer.ReadByte(&cProcessFlag);
		if(0x02==(cProcessFlag&0x0f))
			profile_vertex.vertex.feature=2;
		else if(0x03==(cProcessFlag&0x0f))
			profile_vertex.vertex.feature=12;
		else if(0x04==(cProcessFlag&0x0f))
			profile_vertex.vertex.feature=3;
		else if(0x05==(cProcessFlag&0x0f))
			profile_vertex.vertex.feature=13;
		else
			profile_vertex.vertex.feature=1;

		if(byteConstArr[7]&cProcessFlag)
			profile_vertex.m_bWeldEdge = true;					//��ʶ����
		else 
			profile_vertex.m_bRollEdge = true;					//��ʶ���
		buffer.ReadDouble(&profile_vertex.vertex.x);			//�����X����
		buffer.ReadDouble(&profile_vertex.vertex.y);			//�����Y����
		buffer.ReadWord(&profile_vertex.manu_space);			//����ӹ�Ԥ����϶���߸߶�(mm)
		if(buf_type==CProcessPart::PPI_BUFFER &&version>=1000006)
			buffer.ReadWord(&profile_vertex.roll_edge_offset_dist);	//���ƫ�ƾ���
		if(profile_vertex.m_bRollEdge&&profile_vertex.manu_space==0)
			profile_vertex.m_bRollEdge=false;
		if(profile_vertex.m_bWeldEdge)
		{	
			BYTE weld_high;
			WORD weld_dis,weld_len;
			buffer.ReadByte(&weld_high);	//����߶�
			buffer.ReadWord(&weld_dis);		//����������������
			buffer.ReadWord(&weld_len);		//���쳤�ȣ�Ϊ0ʱ��ʾһֱ����������յ�
		}
		if(profile_vertex.type>1)
		{
			buffer.ReadDouble(&profile_vertex.sector_angle);	//���֣�Բ�����νǣ�������
			buffer.ReadPoint(profile_vertex.work_norm);			//Բ���淨�߷���
			if(profile_vertex.type==3)
			{
				buffer.ReadDouble(&profile_vertex.radius);		//��Բ���̰��᳤�ȣ���ԭʼ����Բ����뾶��
				buffer.ReadPoint(profile_vertex.center);		//��Բ��Բ��λ�ã��̽�����ϵ��������꣩
				buffer.ReadPoint(profile_vertex.column_norm);	//��Բ��ԭʼ�����������򣨹̽�����ϵ����Է���
			}
		}
		//����ϣ������keyId��ʼhiberId
		vertex_list.SetValue(profile_vertex.keyId,profile_vertex,true);
	}
	//5.�ӹ�����ϵ��׼�����������λ��
	buffer.ReadWord(&mcsFlg.wFlag);		//�ӹ�����ϵ��׼������
	if(mcsFlg.wFlag==0xffff)
	{	//����֮ǰ������
		mcsFlg.ciBottomEdge=255;	//��ʾ-1
		mcsFlg.ciOverturn=0;
	}
	float posX=0,posY=0;
	buffer.ReadFloat(&posX);			//���λ��X����
	buffer.ReadFloat(&posY);			//���λ��Y����
	mkpos.Set(posX,posY);
	if(buf_type==CProcessPart::PPI_BUFFER && version>=1000002)
	{	//��¼���θ�ӡ�ŵķ���
		buffer.ReadFloat(&posX);
		buffer.ReadFloat(&posY);
		mkVec.Set(posX,posY);
	}
	//��ȡ��˨�������
	RetrievedBoltFaceNo();
}
PROFILE_VER* CProcessPlate::FromVertexHiberId(HIBERARCHY id)
{
	PROFILE_VER *pVertex=NULL;
	for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(id.IsEqual(pVertex->hiberId))
			break;
	}
	return pVertex;
}
bool CProcessPlate::DeleteVertexByHiberId(HIBERARCHY id)
{
	bool bRetCode=false;
	int nPush=vertex_list.push_stack();
	for(PROFILE_VER *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		if(id.IsEqual(pVertex->hiberId))
		{	
			vertex_list.DeleteCursor(TRUE);
			bRetCode=true;
			break;
		}
	}
	vertex_list.pop_stack(nPush);
	return bRetCode;
}
bool CProcessPlate::IsFoldPlate()
{
	if(m_cFaceN==3)
	{	//�������������˳������жϣ�2��3�м��޹���(12��13)������ʱΪ�۵���
		PROFILE_VER *pPreVertex=NULL;
		int iPush = vertex_list.push_stack();
		for(PROFILE_VER *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
		{
			if(pPreVertex!=NULL)
			{
				if (pPreVertex->vertex.feature < 10 && pVertex->vertex.feature < 10 &&
					pPreVertex->vertex.feature != pVertex->vertex.feature)
				{
					vertex_list.pop_stack(iPush);
					return false;
				}
			}
			pPreVertex=pVertex;
		}
		vertex_list.pop_stack(iPush);
		return true;
	}
	else 
		return false;
}
bool CProcessPlate::IsPeakPointPlate()
{
	if(m_cFaceN==3 && !IsFoldPlate())
		return TRUE;
	else
		return FALSE;
}
//���»�ȡ��˨�������
void CProcessPlate::RetrievedBoltFaceNo()
{
	if(m_cFaceN<=1||vertex_list.GetNodeNum()<3)
		return;
	//1.����������������ɵĶ��������
	int prevFaceNo=0;
	ATOM_LIST<f3dPoint> faceVertexList[3];
	for(PROFILE_VER *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
	{
		int faceNo1=pVertex->vertex.feature/10,faceNo2=pVertex->vertex.feature%10;
		if(faceNo1>=1&&faceNo1<=3)
			faceVertexList[faceNo1-1].append(f3dPoint(pVertex->vertex.x,pVertex->vertex.y));
		if(faceNo2>=1&&faceNo2<=3)
			faceVertexList[faceNo2-1].append(f3dPoint(pVertex->vertex.x,pVertex->vertex.y));
		if((prevFaceNo==2&&pVertex->vertex.feature==3)||(prevFaceNo==3&&pVertex->vertex.feature==2))
		{	//��������巶Χ����������Ӧ����top_point��
			faceVertexList[0].append(top_point);
			faceVertexList[1].append(top_point);
			faceVertexList[2].append(top_point);
		}
		prevFaceNo=pVertex->vertex.feature;
	}
	fPolygonRgn facePloyArr[3];
	DYN_ARRAY<f3dPoint> faceVertexArr[3];
	for(int i=0;i<m_cFaceN;i++)
	{
		if(faceVertexList[i].GetNodeNum()<=0)
			continue;
		int j=0;
		faceVertexArr[i].Resize(faceVertexList[i].GetNodeNum());
		for(f3dPoint *pPt=faceVertexList[i].GetFirst();pPt;pPt=faceVertexList[i].GetNext(),j++)
			faceVertexArr[i][j]=*pPt;
		facePloyArr[i].CreatePolygonRgn(faceVertexArr[i],faceVertexArr[i].Size());
	}
	//2.������˨��������ȷ����˨���
	for(BOLT_INFO *pBoltInfo=m_xBoltInfoList.GetFirst();pBoltInfo;pBoltInfo=m_xBoltInfoList.GetNext())
	{
		for(int i=0;i<m_cFaceN;i++)
		{
			if(facePloyArr[i].PtInRgn(f3dPoint(pBoltInfo->posX,pBoltInfo->posY))==1)
			{
				pBoltInfo->cFaceNo=i+1;
				break;
			}
		}
	}
}
//����һ���������������ش��������ڵİ�ƽ���0,1,2
//face_normΪ�ְ�����ϵ�µ��������
int CProcessPlate::GetNormFaceI(f3dPoint face_norm)
{
	if(m_cFaceN==1)
		return 0;
	else if(m_cFaceN==2)
	{
		Standarized(face_norm);
		if(fabs(face_norm*f3dPoint(0,0,1))>fabs(face_norm*HuoQuFaceNorm[0]))
			return 0;
		else
			return 1;
	}
	else if(m_cFaceN==3)
	{
		Standarized(face_norm);
		if( fabs(face_norm*f3dPoint(0,0,1))>fabs(face_norm*HuoQuFaceNorm[0])
			&&fabs(face_norm*f3dPoint(0,0,1))>fabs(face_norm*HuoQuFaceNorm[1]))
			return 0;
		else if(fabs(face_norm*HuoQuFaceNorm[0])>fabs(face_norm*HuoQuFaceNorm[1]))
			return 1;
		else
			return 2;
	}
	else
		return 0;
}
//������˨ֱ��ͳ�Ƹְ��ϸ���˨����
int CProcessPlate::GetBoltNumByD(int bolt_d)
{
	int nBolt=0;
	for(BOLT_INFO* pBolt=m_xBoltInfoList.GetFirst();pBolt;pBolt=m_xBoltInfoList.GetNext())
	{
		if(pBolt->bolt_d!=bolt_d)
			continue;
		nBolt++;
	}
	return nBolt;
}
//��ȡ���θ�ӡ�ŵ�����������
void CProcessPlate::GetMkRect(double fLen,double fWidth,ATOM_LIST<f3dPoint>& ptArr)
{
	double fHalfL=fLen*0.5,fHalfW=fWidth*0.5;
	f3dPoint offsetVec,pt;
	offsetVec.Set(-mkVec.y,mkVec.x,0);
	pt=mkpos-mkVec*fHalfL;
	pt+=offsetVec*fHalfW;
	ptArr.append(pt);
	pt=mkpos+mkVec*fHalfL;
	pt+=offsetVec*fHalfW;
	ptArr.append(pt);
	pt=mkpos+mkVec*fHalfL;
	pt-=offsetVec*fHalfW;
	ptArr.append(pt);
	pt=mkpos-mkVec*fHalfL;
	pt-=offsetVec*fHalfW;
	ptArr.append(pt);
}
BOOL CProcessPlate::SpreadLsPos(GECS &acs,f3dPoint ls_norm,double* ls_pos_coord)
{
	double angle[2];
	f3dPoint rotate_axis,tempt,vec;
	f3dPoint ls_pos(ls_pos_coord);
	for(int i=0;i<m_cFaceN-1;i++)
	{
		//�����һ�淨�ߵ������淨�ߵ���ת����
		rotate_axis = cross_prod(f3dPoint(0,0,1),HuoQuFaceNorm[i]);
		if(i==0)	//��һ����ڶ����ļн�
			angle[0] = cal_angle_of_2vec(f3dPoint(0,0,1),HuoQuFaceNorm[i]);
		else		//��һ����������ļн�
			angle[1] = cal_angle_of_2vec(f3dPoint(0,0,1),HuoQuFaceNorm[i]);
		//���㵱ǰ�����߷���
		Sub_Pnt(vec,HuoQuLine[i].endPt,HuoQuLine[i].startPt);

		if(dot_prod(vec,rotate_axis)<0)	//��ǰ����������ת���߷���
		{								//�����߷���Ӧ����ת���߷���һ��
			tempt = HuoQuLine[i].startPt;
			HuoQuLine[i].startPt = HuoQuLine[i].endPt;
			HuoQuLine[i].endPt = tempt;
		}
	}
	vector_trans(ls_norm,acs,FALSE);
	int iLsFace=GetNormFaceI(ls_norm);
	if(iLsFace==1)		//�ڶ����ϵ���˨
	{
		coord_trans(ls_pos,acs,FALSE);
		rotate_point_around_axis(ls_pos,-angle[0],HuoQuLine[0]);
		coord_trans(ls_pos,acs,TRUE);
		project_point(ls_pos,acs.origin,acs.axis_z);
		coord_trans(ls_pos,acs,FALSE);
		if(ls_pos_coord)
		{
			ls_pos_coord[0]=ls_pos.x;
			ls_pos_coord[1]=ls_pos.y;
			ls_pos_coord[2]=ls_pos.z;
		}
		return TRUE;
	}
	else if(iLsFace==2)	//�������ϵ���˨
	{
		coord_trans(ls_pos,acs,FALSE);
		rotate_point_around_axis(ls_pos,-angle[1],HuoQuLine[1]);
		coord_trans(ls_pos,acs,TRUE);
		project_point(ls_pos,acs.origin,acs.axis_z);
		coord_trans(ls_pos,acs,FALSE);
		if(ls_pos_coord)
		{
			ls_pos_coord[0]=ls_pos.x;
			ls_pos_coord[1]=ls_pos.y;
			ls_pos_coord[2]=ls_pos.z;
		}
		return TRUE;
	}
	else				//��һ���ϵ���˨
	{
		project_point(ls_pos,acs.origin,acs.axis_z);
		coord_trans(ls_pos,acs,FALSE);
		if(ls_pos_coord)
		{
			ls_pos_coord[0]=ls_pos.x;
			ls_pos_coord[1]=ls_pos.y;
			ls_pos_coord[2]=ls_pos.z;
		}
		return TRUE;
	}
}
f3dPoint CProcessPlate::RestoreLsPos(const double* ls_pos,int iFace012/*=0*/,GECS *pAcs/*=NULL*/)
{
	double angle[2];
	f3dPoint rotate_axis,tempt,vec;
	GEPOINT boltpos(ls_pos);

	for(int i=0;i<m_cFaceN-1;i++)
	{
		//�����һ�淨�ߵ������淨�ߵ���ת����
		rotate_axis = cross_prod(f3dPoint(0,0,1),HuoQuFaceNorm[i]);
		if(i==0)	//��һ����ڶ����ļн�
			angle[0] = cal_angle_of_2vec(f3dPoint(0,0,1),HuoQuFaceNorm[i]);
		else		//��һ����������ļн�
			angle[1] = cal_angle_of_2vec(f3dPoint(0,0,1),HuoQuFaceNorm[i]);
		//���㵱ǰ�����߷���
		Sub_Pnt(vec,HuoQuLine[i].endPt,HuoQuLine[i].startPt);

		if(dot_prod(vec,rotate_axis)<0)	//��ǰ����������ת���߷���
		{								//�����߷���Ӧ����ת���߷���һ��
			tempt = HuoQuLine[i].startPt;
			HuoQuLine[i].startPt = HuoQuLine[i].endPt;
			HuoQuLine[i].endPt = tempt;
		}
	}
	if(iFace012==1&&m_cFaceN>=2)
		rotate_point_around_axis(boltpos,angle[0],HuoQuLine[0]);
	else if(iFace012==2&&m_cFaceN>2)
		rotate_point_around_axis(boltpos,angle[1],HuoQuLine[1]);
	if(pAcs)
		coord_trans(boltpos,*pAcs,TRUE);
	return boltpos;
}
bool CProcessPlate::IsEqual(CGeneralPart *pPart,double epsilon/*=1.0*/,char *error_info/*=NULL*/,DWORD dwChkStyleFlag/*=NULL*/)
{
	//1.�Ƚϻ�����Ϣ����˨��Ϣ
	if(!CProcessPart::IsEqual(pPart,epsilon,error_info,dwChkStyleFlag))
		return false;
	CProcessPlate *pPlate=(CProcessPlate*)pPart;
	if(m_cFaceN!=pPlate->m_cFaceN)
	{
		if(error_info)
			strcpy(error_info,"�ְ�������ͬ");
		return false;
	}
	if( !top_point.IsEqual(pPlate->top_point,epsilon))
	{
		if(error_info)
			strcpy(error_info,"���������߽��㲻ͬ");
		return false;
	}
	if( !m_tInnerOrigin.IsEqual(pPlate->m_tInnerOrigin)||
		!m_tInnerColumnNorm.IsEqual(pPlate->m_tInnerColumnNorm)||
		fabs(m_fInnerRadius-pPlate->m_fInnerRadius)>epsilon)
	{
		if(error_info)
			strcpy(error_info,"�ְ���Բ��ͬ");
		return false;
	}
	//f3dLine HuoQuLine[2];				//�����ߣ���������꣩
	//f3dPoint HuoQuFaceNorm[2];		//�ڵ�����ķ��߷���
	//2.�Ƚ����������㼰������
	if(vertex_list.GetNodeNum()!=pPlate->vertex_list.GetNodeNum())
	{
		if(error_info)
			strcpy(error_info,"�ְ�������������ͬ");
		return false;
	}
	//��ʱ�ȼ򵥱Ƚ�����������ֵ
	PROFILE_VER *pSrcVertex=NULL;
	for(pSrcVertex=vertex_list.GetFirst();pSrcVertex;pSrcVertex=vertex_list.GetNext())
	{
		PROFILE_VER *pDestVertex=pPlate->vertex_list.GetValue(pSrcVertex->keyId);
		if(pDestVertex==NULL)
			break;
		if(!pDestVertex->vertex.IsEqual(pSrcVertex->vertex,epsilon))
			break;
		if( pDestVertex->type!=pSrcVertex->type||
			(pDestVertex->type>0&&pDestVertex->radius!=pSrcVertex->radius))
			break;
		if( pDestVertex->m_bRollEdge!=pSrcVertex->m_bRollEdge||pDestVertex->m_bWeldEdge!=pSrcVertex->m_bWeldEdge||
			pDestVertex->manu_space!=pSrcVertex->manu_space)
			break;
	}
	if(pSrcVertex||vertex_list.GetNodeNum()!=pPlate->vertex_list.GetNodeNum())
	{
		if(error_info)
			strcpy(error_info,"�ְ������㲻ͬ");
		return false;
	}
	return true;
}
double CProcessPlate::GetHuoQuAngle(int iFace,f3dLine *pHuoQuLine/*=NULL*/,BOOL bDisplayLog/*=TRUE*/)
{
	if(m_cFaceN<=1||vertex_list.GetNodeNum()<3||iFace<1||iFace>m_cFaceN-1)	//�����û�б���
		return 0;
	else
	{
		iFace++;	
		f3dLine huoquline;
		BOOL bFindHuoQuStart=FALSE,bFindHuoQuEnd=FALSE;
		if(m_cFaceN==2||IsFoldPlate())	//˫�����۵���
		{
			BOOL bReverse=FALSE;
			for(PROFILE_VER *pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				if(pVertex->vertex.feature>10&&pVertex->vertex.feature%10==iFace)
				{
					if(!bFindHuoQuStart)
					{
						huoquline.startPt=pVertex->vertex;
						bFindHuoQuStart=TRUE;
						continue;
					}
					else
					{
						huoquline.endPt=pVertex->vertex;
						bFindHuoQuEnd=TRUE;
						break;
					}
				}
				if(bFindHuoQuStart&&!bReverse&&(pVertex->vertex.feature==1||pVertex->vertex.feature/10==1))
					bReverse=TRUE;	//ʼ�յ�����˻����涥�㣬˵��ʼ�ն˷��ˣ�����ʼ�ն����ֻ���ǻ����涥��
			}
			if(bFindHuoQuStart&&bFindHuoQuEnd)
			{
				if(bReverse)
				{	//ʼ�ն˷���
					f3dPoint pt=huoquline.startPt;
					huoquline.startPt=huoquline.endPt;
					huoquline.endPt=pt;
				}
			}
		}
		else if(m_cFaceN==3)	//��ͨ�����
		{
			int prev_face_no=0,now_face_no=0,next_face_no=0;
			bool bNeedInverseHuoQuFace=false;	//������Ƿ���Ҫ�ߵ��������棨������ʱ������ʵ���棩
			PROFILE_VER *pVertex;
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				if(!bNeedInverseHuoQuFace&&IsPeakPointPlate())
				{
					if(prev_face_no==0)
						prev_face_no=pVertex->vertex.feature;
					else if(now_face_no==0)
						now_face_no=pVertex->vertex.feature;
					else if(next_face_no==0)
						next_face_no=pVertex->vertex.feature;
					else
					{
						prev_face_no=now_face_no;
						now_face_no=next_face_no;
						next_face_no=pVertex->vertex.feature;
					}
					if((prev_face_no==3&&next_face_no==1)||(prev_face_no==1&&next_face_no==2))
					{
						bNeedInverseHuoQuFace=true;
						break;
					}
				}
			}
			for(pVertex=vertex_list.GetFirst();pVertex;pVertex=vertex_list.GetNext())
			{
				if(pVertex->vertex.feature>10&&pVertex->vertex.feature%10==iFace)
				{
					huoquline.startPt=pVertex->vertex;
					break;
				}
			}
			if(pVertex!=NULL)	//�ҵ������߶˵�
			{
				bFindHuoQuStart=bFindHuoQuEnd=TRUE;
				if(iFace==2)
				{
					if(bNeedInverseHuoQuFace)
						huoquline.endPt=top_point;
					else
					{
						huoquline.endPt=huoquline.startPt;
						huoquline.startPt=top_point;	
					}
				}
				else if(iFace==3)
				{
					if(bNeedInverseHuoQuFace)
					{
						huoquline.endPt=huoquline.startPt;
						huoquline.startPt=top_point;	
					}
					else
						huoquline.endPt=top_point;
				}
				else
					bFindHuoQuStart=bFindHuoQuEnd=FALSE;
			}
		}
		if(!bFindHuoQuStart||!bFindHuoQuEnd)
		{
			if (bDisplayLog)
				logerr.Log("�ְ�%sû�ҵ�����Ļ���������Ե�ཻ����!",(char*)m_sPartNo,iFace-1);
			return 0;
		}
		f3dPoint huoqu_axis_vec=huoquline.endPt-huoquline.startPt;
		f3dPoint huoqu_norm=HuoQuFaceNorm[iFace-2];
		if(huoqu_norm.IsZero())
		{
			if (bDisplayLog)
				logerr.Log("�ְ�%s�ĵ�%d(����)�淨��Ϊ�գ������Ǽ�������!",(char*)m_sPartNo,iFace-1);
			return 0;
		}
		UCS_STRU huoquucs;
		huoquucs.axis_z=huoqu_axis_vec;
		normalize(huoquucs.axis_z);
		huoquucs.axis_x.Set(0,0,1);
		huoquucs.axis_y=huoquucs.axis_z^huoquucs.axis_x;
		if (fabs(huoqu_norm*huoquucs.axis_z) > 0.001)
		{
			if(bDisplayLog)
				logerr.Log("�ְ�%s�ĵ�%d(����)�淨�߷���������߲���ֱ�������Ǽ�������!", (char*)m_sPartNo, iFace - 1);
		}
		vector_trans(huoqu_norm,huoquucs,FALSE);
		double huoquangle=Cal2dLineAng(0,0,huoqu_norm.x,huoqu_norm.y);
		if(huoquangle>Pi)
			huoquangle-=2*Pi;
		if(pHuoQuLine)
			*pHuoQuLine=huoquline;
		return huoquangle;
	}
}

//iFaceȡֵ0��1
BOOL CProcessPlate::GetBendLineAt(int iBendLine, f3dLine *pBendLine)
{
	f3dLine line;
	BOOL bRetCode = FALSE;
	if (GetHuoQuAngle(iBendLine+1, &line, FALSE) != 0)
		bRetCode = TRUE;
	if (!bRetCode && m_cFaceN>1 && iBendLine>=0 && iBendLine<m_cFaceN-1)
	{
		BOOL bFindStart = FALSE, bFindEnd = FALSE;
		f3dPoint startPt = HuoQuLine[iBendLine].startPt;
		f3dPoint endPt = HuoQuLine[iBendLine].endPt;
		f3dPoint vec = endPt - startPt;
		normalize(vec);
		f2dLine huoquLine, tem_line;
		huoquLine.startPt.Set(f3dPoint(startPt - vec * 1000).x, f3dPoint(startPt - vec * 1000).y);
		huoquLine.endPt.Set(f3dPoint(endPt + vec * 1000).x, f3dPoint(endPt + vec * 1000).y);
		PROFILE_VER *pVertex=NULL, *pPrevPnt = vertex_list.GetTail();
		for (pVertex = vertex_list.GetFirst(); pVertex; pVertex = vertex_list.GetNext())
		{
			tem_line.startPt.Set(pPrevPnt->vertex.x, pPrevPnt->vertex.y);
			tem_line.endPt.Set(pVertex->vertex.x, pVertex->vertex.y);
			f3dPoint pt;
			if (Int2dll(tem_line, huoquLine, pt.x, pt.y) > 0)
			{
				if (!bFindStart&&DISTANCE(startPt, pt) < DISTANCE(endPt, pt))
				{
					line.startPt = pt;
					bFindStart = TRUE;
				}
				if (!bFindEnd&&DISTANCE(startPt, pt) > DISTANCE(endPt, pt))
				{
					line.endPt = pt;
					bFindEnd = TRUE;
				}
			}
			if (bFindStart&&bFindEnd)
				break;
			pPrevPnt = pVertex;
		}
		if (bFindStart&&bFindEnd)
			bRetCode = TRUE;
	}
	if (pBendLine)
		*pBendLine = line;
	return bRetCode;
}

BYTE CProcessPlate::GetHuoQuFaceBendType(int iFace)
{
	BYTE cBendType = BEND_NONE;
	if (m_cFaceN > 1 && iFace > 1 && iFace <= m_cFaceN)
	{
		f3dLine huoquline = HuoQuLine[iFace - 2];
		f3dPoint basePt, base_face_pick;
		bool basepick_finded = false;
		int n = vertex_list.GetNodeNum();
		for (PROFILE_VER *pVertex = vertex_list.GetFirst(); pVertex; pVertex = vertex_list.GetNext())
		{
			f3dPoint pt = pVertex->vertex;
			//��֤��׼��ͻ������ʰȡ�㲻�ڻ������ϣ������޷���ȷ�ж������� wht 11-04-21
			/** �������fabs(dist)>10�ж���Ϊ�˱����������������������ӽ����������ںϲ����ɻ������Ӱ壩��
			   �һ�����������ʱ�����´��� wjh-2016.4.29
			*/
			double dist1 = 0, dist2 = 0;
			if (!basepick_finded)
			{
				dist1 = DistOf2dPtLine(pt, HuoQuLine[0].startPt, HuoQuLine[0].endPt);
				if (m_cFaceN >= 3)
					dist2 = DistOf2dPtLine(pt, HuoQuLine[1].startPt, HuoQuLine[1].endPt);
			}
			if (m_cFaceN == 3)
			{   //��֤��׼��ͻ������ʰȡ�㲻�ڻ������ϣ������޷���ȷ�ж������� wht 11-04-21
				if (fabs(dist1) > 10 && fabs(dist2) > 10 && HuoQuLine[0].PtInLine(pt) == 0 && HuoQuLine[1].PtInLine(pt) == 0)
				{
					if (!basepick_finded&&pt.feature == 1)      //��׼��ʰȡ��
					{
						basepick_finded = true;
						base_face_pick = pt;
					}
				}
			}
			else if (m_cFaceN == 2)
			{
				if (fabs(dist1) > 10 && HuoQuLine[0].PtInLine(pt) == 0)
				{
					if (!basepick_finded&&pt.feature == 1)      //��׼��ʰȡ��
					{
						basepick_finded = true;
						base_face_pick = pt;
					}
				}
			}
		}
		f3dPoint prj_base_pick, base_vec;
		SnapPerp(&prj_base_pick, huoquline, base_face_pick);
		base_vec = base_face_pick - prj_base_pick;
		normalize(base_vec);
		f3dPoint norm = HuoQuFaceNorm[iFace - 2];
		//vector_trans(norm, ucs, FALSE);
		BOOL bNeiQu = TRUE;
		double dd = cal_angle_of_2vec(base_vec, norm);
		if (dd < Pi / 2 || (fabs(dd - Pi / 2) < EPS&&norm.z > 0))   //������������(����)
			bNeiQu = FALSE;
		cBendType = bNeiQu ? BEND_IN : BEND_OUT;
	}
	return cBendType;
}
f3dPoint CProcessPlate::GetDeformedVertex(f3dPoint vertice)
{
	if(m_ciDeformType==0||m_cFaceN<=1||vertex_list.GetNodeNum()<3||vertice.feature<=1)	//�����û�б���
		return vertice;
	else
	{
		f3dLine huoquline;
		int iFace=vertice.feature%10;
		//Ϊ����������������ǶȻ�ȡ����ͳһ����GetHuoQuAngle��ʵ�� wjh-2011.8.31
		double huoquangle=GetHuoQuAngle(iFace-1,&huoquline);
		f3dPoint huoqu_axis_vec=huoquline.endPt-huoquline.startPt;
		UCS_STRU huoquucs;
		huoquucs.axis_z=huoqu_axis_vec;
		normalize(huoquucs.axis_z);
		huoquucs.axis_x.Set(0,0,1);
		huoquucs.axis_y=huoquucs.axis_z^huoquucs.axis_x;
		//�����û�ѡ��Ļ������η�ʽ�������������ϵ�����
		double r=0,x=0,odd_len=0;
		if(m_ciDeformType==1)	//���Բ���л�������
			x=0.35;
		else if(m_ciDeformType==2)			//͹����л�������
			x=1;
		else if(m_ciDeformType==3)			//������л�������
			x=0;
		//�������������
		if(huoquangle>0)	//����,�����ǻ�׼��
			odd_len=huoquangle*(r+x*m_fThick)-2*r*tan(huoquangle/2);
		else	//����,�����ǻ�׼��
			odd_len=-huoquangle*(r+x*m_fThick)-2*(r+m_fThick)*tan(-huoquangle/2);
		if(vertice.feature>10)	//������
			vertice+=huoquucs.axis_y*(odd_len/2);
		else	//�������ϵ�������
			vertice+=huoquucs.axis_y*odd_len;
		return vertice;
	}
}
////////////////////////////////////////////////////////////////////
// CProcessAngle
#ifdef __PROPERTYLIST_UI_
IMPLEMENT_PROP_FUNC(CProcessAngle)
//��ʼ������id
void CProcessAngle::InitPropHashtable()
{
	int id=1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"�Ǹֻ�����Ϣ"));
	AddPropItem("m_sPartNo",PROPLIST_ITEM(id++,"�������"));
	AddPropItem("m_Seg",PROPLIST_ITEM(id++,"�ֶκ�"));
	AddPropItem("m_cMaterial",PROPLIST_ITEM(id++,"����"));
	AddPropItem("m_sSpec",PROPLIST_ITEM(id++,"���"));
#ifdef __TAP_CONTEXT_
	AddPropItem("m_cOrgMaterial",PROPLIST_ITEM(id++,"ԭ����"));
	AddPropItem("m_sOrgSpec",PROPLIST_ITEM(id++,"ԭ���"));
#endif
	AddPropItem("m_wLength",PROPLIST_ITEM(id++,"����"));
	AddPropItem("m_fWeight",PROPLIST_ITEM(id++,"����"));
	AddPropItem("m_cFuncType",PROPLIST_ITEM(id++,"��������","�Ǹֹ���","��ͨ�Ǹ�|�Ӿ��Ǹ�|�Ŷ��Ǹ�|���ȽǸ�|����Ǹ�|��ͷ�Ǹ�"));
	AddPropItem("m_fDecWingAngle",PROPLIST_ITEM(id++,"��֫�н�"));
	AddPropItem("m_sNote",PROPLIST_ITEM(id++,"��ע"));
	AddPropItem("m_dwInheritPropFlag",PROPLIST_ITEM(id++,"ͬ��ģʽ","","...."));
	//������Ϣ
	AddPropItem("processInfo",PROPLIST_ITEM(id++,"������Ϣ"));
	AddPropItem("startCutWing",PROPLIST_ITEM(id++,"ʼ���н���֫"));
	AddPropItem("startCutWingX",PROPLIST_ITEM(id++,"X֫","ʼ��X֫�н���֫��Ϣ"));
	AddPropItem("startCutWingY",PROPLIST_ITEM(id++,"Y֫","ʼ��Y֫�н���֫��Ϣ"));
	AddPropItem("endCutWing",PROPLIST_ITEM(id++,"�ն��н���֫"));
	AddPropItem("endCutWingX",PROPLIST_ITEM(id++,"X֫","�ն�X֫�н���֫��Ϣ"));
	AddPropItem("endCutWingY",PROPLIST_ITEM(id++,"Y֫","�ն�Y֫�н���֫��Ϣ"));
	AddPropItem("wing_push_X1_Y2",PROPLIST_ITEM(id++,"ѹ��","","0.��|1.X֫|2.Y֫|"));
	AddPropItem("start_push_pos",PROPLIST_ITEM(id++,"ѹ����ʼλ��"));
	AddPropItem("end_push_pos",PROPLIST_ITEM(id++,"ѹ����ֹλ��"));
	AddPropItem("m_bCutRoot",PROPLIST_ITEM(id++,"���","","��|��"));
	AddPropItem("m_bCutBer",PROPLIST_ITEM(id++,"����","","��|��"));
	AddPropItem("m_bWeld",PROPLIST_ITEM(id++,"����","","��|��"));
	AddPropItem("m_bNeedFillet",PROPLIST_ITEM(id++,"�¿�","","��|��"));
	AddPropItem("kaihe_base_wing_x0_y1",PROPLIST_ITEM(id++,"���Ͻ�","","0.X֫|1.Y֫|2.��|"));
	AddPropItem("kaihe_angle",PROPLIST_ITEM(id++,"��֫�н�"));
	//��˨����Ϣ
	AddPropItem("boltInfo",PROPLIST_ITEM(id++,"��˨��"));
	AddPropItem("x_wing_ls",PROPLIST_ITEM(id++,"X֫","X֫��˨����"));
	AddPropItem("y_wing_ls",PROPLIST_ITEM(id++,"Y֫","Y֫��˨����"));
	/*for(int i=0;i<100;i++)
	{
		CXhChar50 sXWingHole("X֫��-%d",i+1),sYWingHole("Y֫��-%d",i+1),
			sXWingKey("x_wing_ls[%d]",i),sYWingKey("y_wing_ls[%d]",i);
		AddPropItem(sXWingKey,PROPLIST_ITEM(id++,sXWingHole));
		AddPropItem(sYWingKey,PROPLIST_ITEM(id++,sYWingHole));
	}*/
	AddPropItem("Place",PROPLIST_ITEM(id++,"ռλ"));
	AddPropItem("m_cPlaceMode",PROPLIST_ITEM(id++,"ռλģʽ","ռλģʽ","0.����ռλ|1.4ռλģʽ1|2.4ռλģʽ2|3.8ռλģʽ1|4.8ռλģʽ2"));
	AddPropItem("POS_FRONT_PLUS",PROPLIST_ITEM(id++,"ǰ","","���|ɾ��"));
	AddPropItem("POS_BACK_PLUS",PROPLIST_ITEM(id++,"��","","���|ɾ��"));
	AddPropItem("POS_LEFT_PLUS",PROPLIST_ITEM(id++,"��","","���|ɾ��"));
	AddPropItem("POS_RIGHT_PLUS",PROPLIST_ITEM(id++,"��","","���|ɾ��"));
	AddPropItem("POS_FRONT_MINUS",PROPLIST_ITEM(id++,"ǰX-","","���|ɾ��"));
	AddPropItem("POS_BACK_MINUS",PROPLIST_ITEM(id++,"��X-","","���|ɾ��"));
	AddPropItem("POS_LEFT_MINUS",PROPLIST_ITEM(id++,"��Y-","","���|ɾ��"));
	AddPropItem("POS_RIGHT_MINUS",PROPLIST_ITEM(id++,"��Y-","","���|ɾ��"));
	//
	InitSyncProp();
}

//ͨ������Id��ȡ����ֵ
int CProcessAngle::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(GetPropID("m_sPartNo")==id)
		sText.Copy(GetPartNo());
	else if(GetPropID("m_Seg")==id)
		sText.Copy((char*)GetSegI().ToString());
	else if(GetPropID("m_cMaterial")==id)	
	{
		char matStr[20];
		CProcessPart::QuerySteelMatMark(cMaterial,matStr);
		sText.Printf("%s",matStr);
	}
	else if(GetPropID("m_sSpec")==id)
		sText.Printf("%dX%d",(int)m_fWidth,(int)m_fThick);
#ifdef __TAP_CONTEXT_
	else if(GetPropID("m_cOrgMaterial")==id)	
	{
		char matStr[20];
		CProcessPart::QuerySteelMatMark(m_cOrgMaterial,matStr);
		sText.Printf("%s",matStr);
	}
	else if(GetPropID("m_sOrgSpec")==id)
		sText.Printf("%dX%d",(int)m_fOrgWidth,(int)m_fOrgThick);
	else if(GetPropID("m_cPlaceMode")==id)
	{
		CXhChar100 cmbItemArr[5]={"0.����ռλ","1.4ռλģʽ1","2.4ռλģʽ2","3.8ռλģʽ1","4.8ռλģʽ2"};
		if(m_cPlaceMode>=0||m_cPlaceMode<=4)
			sText.Copy(cmbItemArr[m_cPlaceMode]);
		else
			sText.Empty();
	}
	else if(GetPropID("POS_FRONT_PLUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_FRONT_PLUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_FRONT_PLUS));
		else 
			sText.Copy("��");

	}
	else if(GetPropID("POS_BACK_PLUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_BACK_PLUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_BACK_PLUS));
		else 
			sText.Copy("��");
	}
	else if(GetPropID("POS_LEFT_PLUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_LEFT_PLUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_LEFT_PLUS));
		else 
			sText.Copy("��");
	}
	else if(GetPropID("POS_RIGHT_PLUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_RIGHT_PLUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_RIGHT_PLUS));
		else 
			sText.Copy("��");
	}
	else if(GetPropID("POS_FRONT_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_FRONT_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_FRONT_MINUS));
		else 
			sText.Copy("��");
	}
	else if(GetPropID("POS_BACK_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_BACK_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_BACK_MINUS));
		else 
			sText.Copy("��");
	}
	else if(GetPropID("POS_LEFT_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_LEFT_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_LEFT_MINUS));
		else 
			sText.Copy("��");
	}
	else if(GetPropID("POS_RIGHT_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_RIGHT_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_RIGHT_MINUS));
		else 
			sText.Copy("��");
	}
#endif
	else if(GetPropID("m_wLength")==id)
		sText.Printf("%d",m_wLength);
	else if(GetPropID("m_fWeight")==id)
	{
		sText.Printf("%f",m_fWeight);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fDecWingAngle")==id)
	{
		sText.Printf("%f",GetDecWingAngle());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_cFuncType")==id)
	{
		if(CProcessAngle::FUNC_REINFORCE==m_cFuncType)
			sText.Copy("�Ӿ��Ǹ�");
		else if(CProcessAngle::FUNC_FOOTNAIL==m_cFuncType)
			sText.Copy("�Ŷ��Ǹ�");
		else if(CProcessAngle::FUNC_LEGANGLE==m_cFuncType)
			sText.Copy("���ȽǸ�");
		else if(CProcessAngle::FUNC_BODYANGLE==m_cFuncType)
			sText.Copy("����Ǹ�");
		else if(CProcessAngle::FUNC_HEADANGLE==m_cFuncType)
			sText.Copy("��ͷ�Ǹ�");
		else //if(CProcessAngle::FUNC_COMMON==m_cFuncType)
			sText.Copy("��ͨ�Ǹ�");
	}
	else if(GetPropID("m_sNote")==id)
		sText.Copy(GetNotes()); 
	else if(GetPropID("startCutWingX")==id)
	{	//ʼ����֫�н�
		if(cut_wing[0]==1)	
		{	//ʼ��X֫��֫
			sText.Printf("%.fx%.fx%.f",cut_wing_para[0][1],
						cut_wing_para[0][0],cut_wing_para[0][2]);
		}
		if(cut_angle[0][0]>0)
		{	//ʼ��X֫�н�
			if(sText.GetLength()>0)
				sText.Append(",");
			sText.Printf("%s%.fx%.f",(char*)sText,cut_angle[0][1],cut_angle[0][0]);
		}
	}
	else if(GetPropID("startCutWingY")==id)
	{	//ʼ����֫�н�
		if(cut_wing[0]==2)	
		{	//ʼ��Y֫��֫
			sText.Printf("%.fx%.fx%.f",cut_wing_para[0][1],
							cut_wing_para[0][0],cut_wing_para[0][2]);
		}
		if(cut_angle[1][0]>0)
		{	//ʼ��Y֫�н�
			if(sText.GetLength()>0)
				sText.Append(",");
			sText.Printf("%s%.fx%.f",(char*)sText,cut_angle[1][1],cut_angle[1][0]);
		}
	}
	else if(GetPropID("endCutWingX")==id)
	{	//�ն���֫�н�
		if(cut_wing[1]==1)	
		{	//�ն�X֫��֫
			sText.Printf("%.fx%.fx%.f",cut_wing_para[1][1],
						cut_wing_para[1][0],cut_wing_para[1][2]);
		}
		if(cut_angle[2][0]>0)
		{	//�ն�X֫�н�
			if(sText.GetLength()>0)
				sText.Append(",");
			sText.Printf("%s%.fx%.f",(char*)sText,cut_angle[2][1],cut_angle[2][0]);
		}
	}
	else if(GetPropID("endCutWingY")==id)
	{	//�ն���֫�н�
		if(cut_wing[1]==2)	
		{	//�ն�Y֫��֫
			sText.Printf("%.fx%.fx%.f",cut_wing_para[1][1],
						cut_wing_para[1][0],cut_wing_para[1][2]);
		}
		if(cut_angle[3][0]>0)
		{	//�ն�Y֫�н�
			if(sText.GetLength()>0)
				sText.Append(",");
			sText.Printf("%s%.fx%.f",(char*)sText,cut_angle[3][1],cut_angle[3][0]);
		}
	}
	else if(GetPropID("wing_push_X1_Y2")==id)
	{
		if(IsWingPush())
		{
			if(wing_push_X1_Y2==1)
				sText.Copy("1.X֫");
			else if(wing_push_X1_Y2==2)
				sText.Copy("2.Y֫");
		}
		else 
			sText.Copy("0.��");
	}
	else if(GetPropID("start_push_pos")==id)
	{
		sText.Printf("%d",start_push_pos);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("end_push_pos")==id)
	{
		sText.Printf("%d",end_push_pos);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bCutRoot")==id)
	{
		if(m_bCutRoot)
			sText.Copy("��");
		else 
			sText.Copy("��");
	}
	else if(GetPropID("m_bCutBer")==id)
	{
		if(m_bCutBer)
			sText.Copy("��");
		else 
			sText.Copy("��");
	}
	else if(GetPropID("m_bWeld")==id)
	{
		if(m_bWeld)
			sText.Copy("��");
		else 
			sText.Copy("��");
	}
	else if(GetPropID("m_bNeedFillet")==id)
	{
		if(m_bNeedFillet)
			sText.Copy("��");
		else 
			sText.Copy("��");
	}
	else if(GetPropID("kaihe_base_wing_x0_y1")==id)
	{
		if(IsWingKaiHe())
		{
			if(kaihe_base_wing_x0_y1==0)
				sText.Copy("0.X֫");
			else if(kaihe_base_wing_x0_y1==1)
				sText.Copy("1.Y֫");
		}
		else 
			sText.Copy("2.��");
	}
	else if(GetPropID("x_wing_ls")==id)
		sText.Printf("%d",GetXOrYWingLsNum(FALSE));
	else if(GetPropID("y_wing_ls")==id)
		sText.Printf("%d",GetXOrYWingLsNum(TRUE));
	/*else 
	{
	ATOM_LIST XBoltList
	for(int i=0;i<100;i++)
	{
	CXhChar50 sXWingKey("x_wing_ls[%d]",i),sYWingKey("y_wing_ls[%d]",i);
	if(GetPropID(sXWingKey)==id)
	{
	sText.ConvertFromPoint(f3dPoint(x_wing_ls[i].posX,x_wing_ls[i].posY),2);
	break;
	}
	else if(GetPropID(sYWingKey)==id)
	{
	sText.ConvertFromPoint(f3dPoint(y_wing_ls[i].posX,y_wing_ls[i].posY),2);
	break;
	}
	}
	}*/
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
#endif
#ifdef __DRAWING_CONTEXT_
CProcessAngle::CProcessAngle(CLDSPart* pAnglePart/*=NULL*/)
{
	if(pAnglePart!=NULL&&pAnglePart->IsAngle())
		this->pPart=pAnglePart;
	else if(pAnglePart!=NULL&&pAnglePart->GetClassTypeId()==CLS_POLYPART)
		this->pPart=pAnglePart;
	else
		this->pPart=NULL;
#else
CProcessAngle::CProcessAngle()
{
#endif
	m_cPartType=TYPE_LINEANGLE;
	memset(_cut_wing,0,sizeof(_cut_wing));
	memset(_cut_wing_para,0,sizeof(_cut_wing_para));
	memset(_cut_angle,0,sizeof(_cut_angle));
	m_fThick=0;
	m_fWeight=0;
	m_wLength=0;
	m_bCutBer=false;
	m_bCutRoot=false;
	m_bWeld=false;
	for (int i=0;i<2;i++)
		cut_wing_para_ptr[i]=&_cut_wing_para[i][0];
	for (int i=0;i<4;i++)
		cut_angle_ptr[i]=&_cut_angle[i][0];
	_wing_push_X1_Y2=0;
	_start_push_pos=0;
	_end_push_pos=0;
	_poly_part_num=0;
	_kaihe_base_wing_x0_y1=-1;
	_bCutRoot=FALSE;
	_bCutBer=FALSE;
	_bWeld=FALSE;
	_bNeedFillet=FALSE;
	m_bUserSpecStartCutAngle=FALSE;
	m_bUserSpecEndCutAngle=FALSE;
}

BYTE CProcessAngle::GetPushFlatType()
{
	if(wing_push_X1_Y2!=1&&wing_push_X1_Y2!=2)
		return PUSH_FLAT_NONE;
	if(start_push_pos<=0)
		return PUSH_FLAT_START;
	else if(end_push_pos>=(m_wLength-1))
		return PUSH_FLAT_END;
	else if(start_push_pos>0&&end_push_pos<m_wLength&&start_push_pos<end_push_pos)
		return PUSH_FLAT_MID;
	else
		return PUSH_FLAT_NONE;
}

WORD CProcessAngle::GetStartPushPos()
{
#ifdef __DRAWING_CONTEXT_
	if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		f3dPoint mid_push_flat_pos;
		CLDSLineAngle* pAngle = (CLDSLineAngle*)pPart;
#ifndef __LDS_VERSION_1_1_
		CLDSNode *pMidPushFlatNode=pAngle->BelongModel()->FromNodeHandle(pAngle->mid_push_flat_node_handle);
		if(pMidPushFlatNode)
			mid_push_flat_pos=pAngle->GetDatumPos(pMidPushFlatNode);
			//SnapPerp(&mid_push_flat_pos,pAngle->Start(),pAngle->End(),pMidPushFlatNode->Position(false));
#endif
		double len_starttomid =pAngle->startOdd()+DISTANCE(pAngle->Start(),mid_push_flat_pos);
		if(((CLDSLineAngle*)pPart)->start_push_flat_x1_y2>0)
			return 0;
		else if(((CLDSLineAngle*)pPart)->mid_push_flat_x1_y2>0)
			return (WORD)(len_starttomid-((CLDSLineAngle*)pPart)->mid_push_flat_start_offset);
		else if(((CLDSLineAngle*)pPart)->end_push_flat_x1_y2>0)
			return (WORD)(GetLength()-((CLDSLineAngle*)pPart)->end_push_flat_length);
	}
	else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
	{
		CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
		WORD start_flate_pos=0;//ʼ��ѹ��λ��
		double startLength=0.0; //��¼ѹ��ķֶνǸ�ǰ���νǸֳ��� 
		int nBiaoShi=0;	
		for (CLDSLineAngle *pJg=(CLDSLineAngle*)pPolyPart->segset.GetFirst();pJg;pJg=(CLDSLineAngle*)pPolyPart->segset.GetNext())
		{
			double length=0.0;
			if(nBiaoShi>0)
			{
				length+=startLength;
				startLength+=pJg->GetLength();
			}
			nBiaoShi++;
			if(pJg->start_push_flat_x1_y2>0)
			{
				start_flate_pos=(WORD)length;
				break;
			}
			else if(pJg->end_push_flat_x1_y2>0)
			{
				double lengthTemp=pJg->GetLength();
				start_flate_pos=(WORD)(lengthTemp-pJg->end_push_flat_length+length);
				break;
			}
			else if(pJg->mid_push_flat_x1_y2>0)
			{
				f3dPoint mid_push_flat_pos;
#ifndef __LDS_VERSION_1_1_
				CLDSNode *pMidPushFlatNode=pJg->BelongModel()->FromNodeHandle(pJg->mid_push_flat_node_handle);
				if(pMidPushFlatNode)
					mid_push_flat_pos=pJg->GetDatumPos(pMidPushFlatNode);
					//SnapPerp(&mid_push_flat_pos,pJg->Start(),pJg->End(),pMidPushFlatNode->Position(false));
#endif
				double len_starttomid = pJg->startOdd()+DISTANCE(pJg->Start(),mid_push_flat_pos);
				double start_pos = len_starttomid-pJg->mid_push_flat_start_offset+length;
				start_flate_pos = (WORD)start_pos;
				break;
			}
		}
		return start_flate_pos;
	}
#endif
	return _start_push_pos;
}

WORD CProcessAngle::SetStartPushPos(WORD start_push_pos)
{
	_start_push_pos=start_push_pos;
	return _start_push_pos;
}

WORD CProcessAngle::GetEndPushPos()
{
#ifdef __DRAWING_CONTEXT_
	if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
	{
		f3dPoint mid_push_flat_pos;
		CLDSLineAngle* pAngle = (CLDSLineAngle*)pPart;
#ifndef __LDS_VERSION_1_1_
		CLDSNode *pMidPushFlatNode=pAngle->BelongModel()->FromNodeHandle(pAngle->mid_push_flat_node_handle);
		if(pMidPushFlatNode)
			mid_push_flat_pos=pAngle->GetDatumPos(pMidPushFlatNode);
			//SnapPerp(&mid_push_flat_pos,pAngle->Start(),pAngle->End(),pMidPushFlatNode->Position(false));
#endif
		double len_starttomid =pAngle->startOdd()+DISTANCE(pAngle->Start(),mid_push_flat_pos);
		if(((CLDSLineAngle*)pPart)->start_push_flat_x1_y2>0)
			return (WORD)((CLDSLineAngle*)pPart)->start_push_flat_length;
		else if(((CLDSLineAngle*)pPart)->mid_push_flat_x1_y2>0)
			return (WORD)(len_starttomid+((CLDSLineAngle*)pPart)->mid_push_flat_end_offset);
		else if(((CLDSLineAngle*)pPart)->end_push_flat_x1_y2>0)
			return (WORD)pPart->GetLength();
	}
	else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
	{
		CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
		WORD end_flate_pos=0;//�ն�ѹ��λ��
		double startLength=0.0; //��¼ѹ��ķֶνǸ�ǰ���νǸֳ��� 
		int nBiaoShi=0;	
		for (CLDSLineAngle *pJg=(CLDSLineAngle*)pPolyPart->segset.GetFirst();pJg;pJg=(CLDSLineAngle*)pPolyPart->segset.GetNext())
		{
			double length=0.0;
			if(nBiaoShi>0)
			{
				length+=startLength;
				startLength+=pJg->GetLength();
			}
			nBiaoShi++;
			if(pJg->start_push_flat_x1_y2>0)
			{
				end_flate_pos = (WORD)(pJg->start_push_flat_length+length);
				break;
			}
			else if(pJg->end_push_flat_x1_y2>0)
			{
				double lengthTemp=pJg->GetLength();
				end_flate_pos = (WORD)(lengthTemp+length);
				break;
			}
			else if(pJg->mid_push_flat_x1_y2>0)
			{
				f3dPoint mid_push_flat_pos;
#ifndef __LDS_VERSION_1_1_
				CLDSNode *pMidPushFlatNode=pJg->BelongModel()->FromNodeHandle(pJg->mid_push_flat_node_handle);
				if(pMidPushFlatNode)
					mid_push_flat_pos=pJg->GetDatumPos(pMidPushFlatNode);
					//SnapPerp(&mid_push_flat_pos,pJg->Start(),pJg->End(),pMidPushFlatNode->Position(false));
#endif
				double len_starttomid = pJg->startOdd()+DISTANCE(pJg->Start(),mid_push_flat_pos);
				double start_pos = len_starttomid-pJg->mid_push_flat_start_offset+length;
				double end_pos = len_starttomid+pJg->mid_push_flat_end_offset+length;
				end_flate_pos=(WORD)end_pos;
				break;
			}
		}
		return end_flate_pos;
	}
#endif
	return _end_push_pos;
}

WORD CProcessAngle::SetEndPushPos(WORD end_push_pos)
{
	_end_push_pos=end_push_pos;
	return _end_push_pos;
}

double CProcessAngle::GetDecWingAngle()
{
#ifdef __DRAWING_CONTEXT_
	if(pPart&&pPart->GetClassTypeId()==CLS_LINEANGLE)
		return ((CLDSLineAngle*)pPart)->GetDecWingAngle();
	else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
	{
		CLDSPolyPart *pPolyPart=(CLDSPolyPart*)pPart;
		CLDSLineAngle *pAngle=(CLDSLineAngle*)pPolyPart->segset.GetFirst();
		return pAngle->GetDecWingAngle();
	}
#endif
	KAI_HE_JIAO *pKaiHeJiao=kaiHeJiaoList.GetFirst();
	if (pKaiHeJiao)
		return pKaiHeJiao->decWingAngle;
	return 90;
}

bool CProcessAngle::IsStartCutAngleOrWing()
{
	double cut_angle_value= cut_angle[0][0]+cut_angle[0][1]+
		cut_angle[1][0]+cut_angle[1][1];
	if(cut_wing[0]>0||cut_angle_value>0)
		return true;
	else
		return false;
}

bool CProcessAngle::IsEndCutAngleOrWing()
{
	double cut_angle_value= cut_angle[2][0]+cut_angle[2][1]+
		cut_angle[3][0]+cut_angle[3][1];
	if(cut_wing[1]>0||cut_angle_value>0)
		return true;
	else
		return false;
}
void CProcessAngle::GetCutAngleAngWingStr(bool bStart,char* cut_angle_x,char* cut_angle_y,int max_len)
{
	CXhChar50 sCutAngleX,sCutAngleY;
	for(int i=0;i<2;i++)
	{
		if((i==0&&!bStart)||(i==1&&bStart))
			continue;
		if((i==0&&!IsStartCutAngleOrWing())||(i==1&&!IsEndCutAngleOrWing()))
			continue;
		int iCutAngleStart=0;
		if(i==1)
			iCutAngleStart=2;
		if((cut_angle[iCutAngleStart][0]>0&&cut_angle[iCutAngleStart][1]>0)||cut_wing[i]>0)
		{
			if(cut_wing[i]==1)		//X֫����֫
				sCutAngleX.Printf("%.0f=%.0f",cut_wing_para[i][0],cut_wing_para[i][2]);
			else if(cut_wing[0]==2)//Y֫����֫
				sCutAngleX.Printf("%.0f<%.0f",cut_wing_para[i][1],cut_wing_para[i][0]);
			else					//X֫���н�
				sCutAngleX.Printf("%.0fX%.0f",cut_angle[iCutAngleStart][1],cut_angle[iCutAngleStart][0]);
		}
		if((cut_angle[iCutAngleStart+1][0]>0&&cut_angle[iCutAngleStart+1][1]>0)||cut_wing[i]>0)
		{
			if(cut_wing[0]==1)		//X֫����֫
				sCutAngleY.Printf("%.0f<%.0f",cut_wing_para[i][1],cut_wing_para[i][0]);
			else if(cut_wing[0]==2)//Y֫����֫
				sCutAngleY.Printf("%.0f=%.0f",cut_wing_para[i][0],cut_wing_para[i][2]);
			else						//Y֫���н�
				sCutAngleY.Printf("%.0fX%.0f",cut_angle[iCutAngleStart+1][1],cut_angle[iCutAngleStart+1][0]);
		}
	}
	if(cut_angle_x)
		StrCopy(cut_angle_x,sCutAngleX,max_len);
	if(cut_angle_y)
		StrCopy(cut_angle_y,sCutAngleY,max_len);
}
bool CProcessAngle::ClonePart(CProcessPart *pNewPart)
{
	bool bRetCode=CProcessPart::ClonePart(pNewPart);
	if(bRetCode)
	{	//������˨������id
		CProcessAngle *pAngle=(CProcessAngle*)pNewPart;
		pAngle->m_xBoltInfoList.Empty();
		for(BOLT_INFO* pBolt=m_xBoltInfoList.GetFirst();pBolt;pBolt=m_xBoltInfoList.GetNext())
			pAngle->m_xBoltInfoList.Append(*pBolt,m_xBoltInfoList.GetCursorKey());
	}
	return bRetCode;
}
int CProcessAngle::GetXOrYWingLsNum(int X0_Y1/*=0*/)
{
	int nLsNum=0;
	for(BOLT_INFO* pBoltInfo=m_xBoltInfoList.GetFirst();pBoltInfo;pBoltInfo=m_xBoltInfoList.GetNext())
	{
		if(X0_Y1==0 && pBoltInfo->posY<0)
			nLsNum++;
		else if(X0_Y1==1 && pBoltInfo->posY>0)
			nLsNum++;
	}
	return nLsNum;
}
void CProcessAngle::WriteProcessBuffer(CBuffer& buffer,long version/*=0*/,bool bSaveSubSerial/*=false*/,int buf_type/*=0*/)
{
	float bolt_d_increment=0;	//��˨ֱ������
	BOLT_INFO *pFirstBolt=m_xBoltInfoList.GetFirst();
	if(pFirstBolt)
		bolt_d_increment=pFirstBolt->hole_d_increment;		//��˨ֱ������
	////////////////////////////////////////////////////////////////
	//1.��������˨�׾�����
	long nRibPlateCount=rib_plate_list.GetNodeNum();	
	if(nRibPlateCount>127)
		logerr.Log("%s�Ǹ��߰����Ϊ%d,�Ǹ��߰�������֧��127����",(char*)GetPartNo(),nRibPlateCount);
	buffer.WriteByte((BYTE)nRibPlateCount);	//�Ǹ��߰����(�Ǹ��߰���������֧��127���߰�)
	long bolt_count=m_xBoltInfoList.GetNodeNum();
	buffer.WriteWord((WORD)bolt_count);		//��˨����
	buffer.WriteFloat(bolt_d_increment);	//��˨ֱ������
	//2.1B��������Ϣ�ֽ�1(1.ʼ�ˣ�֫�н�(֫),2.ʼ�ˣ�֫�н�(֫),3.�նˣ�֫�н�(֫),4.�նˣ�֫�н�(֫),5.���,6.����,7.����,8.�¿�)
	BYTE process_info_first_byte=0;
	BOOL processStateArr[8]={((cut_wing_para[0][0]&&cut_wing[0]==1)||cut_angle[0][0]),	//1.ʼ��X֫��֫(��)
							 ((cut_wing_para[0][0]&&cut_wing[0]==2)||cut_angle[1][0]),	//2.ʼ��Y֫��֫(��)
							 ((cut_wing_para[1][0]&&cut_wing[1]==1)||cut_angle[2][0]),	//3.�ն�X֫��֫(��)
							 ((cut_wing_para[1][0]&&cut_wing[1]==2)||cut_angle[3][0]),	//4.�ն�Y֫��֫(��)
							 _bCutRoot,_bCutBer,_bWeld,FALSE};							//5.���,6.����,7.����,8.�¿�
	for(int i=0;i<8;i++)
	{
		if(processStateArr[i])
			process_info_first_byte |= byteConstArr[i];
	}
	buffer.WriteByte(process_info_first_byte);
	//3. 1B��������Ϣ�ֽ�2(1~3λ��ʾ����ֶ�������4λ0��ʾX֫ѹ�⣬1��ʾY֫ѹ�⣬5~8λ����)
	BYTE process_info_second_byte=0;
	//3.1 ����ֶ���
	if(poly_part_num>7)
		logerr.Log("%s����ֶ���Ϊ%d,��������֧��7���ֶΣ�",(char*)GetPartNo(),poly_part_num);
	process_info_second_byte =(BYTE)poly_part_num;		//����ֶ���
	//3.2 ѹ��֫
	if(wing_push_X1_Y2==1)
		process_info_second_byte &= 0xF7;	//��4λ0��ʾx֫ѹ��
	else if(wing_push_X1_Y2==2)
		process_info_second_byte |= 0x08;	//��4λ1��ʾy֫ѹ��
	buffer.WriteByte(process_info_second_byte);
	//4. 1B��������Ϣ�ֽ�3����1~7λ����������������8λ��0��ʾX֫���Ͻǣ�1��ʾY֫���Ͻǣ�
	BYTE process_info_third_byte =0;
	//4.1 ����������(��1~7λ)
	if(kaiHeJiaoList.GetNodeNum()>127)
		logerr.Log("%s���Ͻ�������Ϊ%d,���Ͻ����������֧��127����",(char*)GetPartNo(),kaiHeJiaoList.GetNodeNum());
	process_info_third_byte=(BYTE)kaiHeJiaoList.GetNodeNum();
	//4.2 ��8λ��0��ʾX֫���Ͻǣ�1��ʾY֫���Ͻ�
	if(kaihe_base_wing_x0_y1==0)
		process_info_third_byte &= 0x7F;	//x֫���Ͻ�
	else if(kaihe_base_wing_x0_y1==1)
		process_info_third_byte |= 0x80;	//y֫���Ͻ�
	buffer.WriteByte(process_info_third_byte);
	//5.48B(4Bx4x3)�н�(֫)����
	for (int i=0;i<4;i++)
	{
		if(cut_angle[i][0]>0)
		{	//�н�
			buffer.WriteWord((WORD)cut_angle[i][0]);
			buffer.WriteWord((WORD)cut_angle[i][1]);
			buffer.WriteWord((WORD)0xFFFF);
		}
		else 
		{	//��֫
			int index=i<2?0:1;
			buffer.WriteWord((WORD)cut_wing_para[index][0]);
			buffer.WriteWord((WORD)cut_wing_para[index][1]);
			buffer.WriteWord((WORD)cut_wing_para[index][2]);
		}
	}
	//6.��˨��Ϣ
	BOLT_INFO *pBolt=NULL;
	for(pBolt=m_xBoltInfoList.GetFirst();pBolt;pBolt=m_xBoltInfoList.GetNext())
	{
		if(bSaveSubSerial)
			buffer.WriteInteger(pBolt->keyId);
		pBolt->ToBuffer(buffer,version,buf_type);
	}
	//7.�ֶνǸ�������ʶ����֫չ�����򡢣�֫չ���������췽��
	//�����ڵ�һ��ֱ�߽Ǹ֣�֫չ������Ϊ���ᣬ�˼����췽��Ϊ���������Ǹֶ�λ����ϵ�¶����
	for (POLY_ANGLE_INFO *pPolySeg =poly_part_info.GetFirst();pPolySeg;pPolySeg=poly_part_info.GetNext())
	{
		buffer.WriteInteger(pPolySeg->nIndex);			 //������ʶ
		buffer.WritePoint(pPolySeg->norm_x_wing);		 //X֫չ������
		buffer.WritePoint(pPolySeg->norm_y_wing);		 //Y֫չ������
		buffer.WritePoint(pPolySeg->len_vec);			 //���췽��
	}
	//8.ѹ����ʼλ�� ����ֹλ��
	//	2B��ѹ����ʼλ�ã����ȷ���0��ʾʼ��ѹ�⣩
	//	2B��ѹ����ֹλ�ã�>�������Ȼ�0xffff��ʾ�ն�ѹ�飩����ʼλ��>=��ֹλ�ñ�ʾ��ѹ�⡣
	buffer.WriteWord(start_push_pos);
	buffer.WriteWord(end_push_pos);
	//9.���Ͻ�LOOP
	for (KAI_HE_JIAO *kai_he_jiao=kaiHeJiaoList.GetFirst();kai_he_jiao;kai_he_jiao=kaiHeJiaoList.GetNext())
	{
		buffer.WriteFloat(kai_he_jiao->decWingAngle);		//��֫�н�
		buffer.WriteWord(kai_he_jiao->position);			//�궨λ��
		buffer.WriteWord(kai_he_jiao->startLength);			//ʼ�˿��ϳ���
		buffer.WriteWord(kai_he_jiao->endLength);			//�ն˿��ϳ���
	}
	//10.�Ǹ��߰�LOOP
	for (RIB_PLATE_INFO *pRibPlateInfoTemp=rib_plate_list.GetFirst();pRibPlateInfoTemp;pRibPlateInfoTemp=rib_plate_list.GetNext())
	{
		buffer.WriteFloat(pRibPlateInfoTemp->fPosZ);				//�߰�λ��
		buffer.WriteFloat(pRibPlateInfoTemp->fThick);				//�߰���
		buffer.Write(&pRibPlateInfoTemp->m_cMaterial,sizeof(char));	//����
		buffer.Write(pRibPlateInfoTemp->sPlateNo,16*sizeof(char));	//�߰���
	}
}
static bool RetrievedPolyAngleBoltFaceNo(CProcessAngle *pAngle)
{
	if(pAngle==NULL||pAngle->poly_part_info.GetNodeNum()<=0)
		return false;
	double fLen=0,fPrevLen=0;
	double huoqu_r=0;
	int nMinSpace=17;	//ȡM12��˨��С���Ʊ߾�
	f3dPoint org;
	POLY_ANGLE_INFO *pCurPolyAngle=NULL,*pPrevPolyAngle=NULL;
	for(int i=0;i<pAngle->poly_part_num;i++)
	{
		fPrevLen=fLen;
		POLY_ANGLE_INFO *pPolyAngle=pAngle->poly_part_info.GetByIndex(i);
		fLen+=pPolyAngle->len_vec.mod();
		if(pPrevPolyAngle)
		{
			f3dPoint vec_start,vec_end;
			vec_start = pPrevPolyAngle->len_vec;
			vec_end   = pPolyAngle->len_vec;
			Standarized(vec_start);
			Standarized(vec_end);
			double angle = cal_angle_of_2vec(vec_start,vec_end)/2.0;
			double sub_len = ((Pi/2-angle)*huoqu_r-tan(Pi/2.0-angle)*huoqu_r)*2;
			fLen+=sub_len;
			org+=vec_start*pPrevPolyAngle->len_vec.mod();
		}
		pPrevPolyAngle=pPolyAngle;
		for(BOLT_INFO *pBoltInfo=pAngle->m_xBoltInfoList.GetFirst();pBoltInfo;pBoltInfo=pAngle->m_xBoltInfoList.GetNext())
		{
			if(pBoltInfo->posX>fPrevLen&&pBoltInfo->posX<fLen)
			{
				pBoltInfo->cFaceNo=i;;
				break;
			}
		}
	}
	return true;
}
void CProcessAngle::ReadProcessBuffer(CBuffer& buffer,long version/*=0*/,bool bSaveSubSerial/*=false*/,int buf_type/*=0*/)
{
	//1.����������Ϣ
	BYTE rib_plate_count=0;
	WORD bolt_count=0;
	float bolt_d_increment;
	buffer.ReadByte(&rib_plate_count);
	buffer.ReadWord(&bolt_count);
	buffer.ReadFloat(&bolt_d_increment);
	//2. 1B��������Ϣ�ֽ�1(1.ʼ�ˣ�֫�н�(֫),2.ʼ�ˣ�֫�н�(֫),3.�նˣ�֫�н�(֫),4.�նˣ�֫�н�(֫)),5.���,6.����,7.����,8.�¿�)
	BYTE process_info=0,cut_info=0;
	buffer.ReadByte(&process_info);
	cut_info=process_info;		//��¼�Ǹֵ��нǱ��λ�����ڳ�ʼ���������֫��Ϣ
	//�Ǹ���֫�н���Ϣ(1.ʼ�ˣ�֫�н�(֫),2.ʼ�ˣ�֫�н�(֫),3.�նˣ�֫�н�(֫),4.�նˣ�֫�н�(֫)),5.���,6.����,7.����,8.�¿�
	if(byteConstArr[4]&process_info)
		m_bCutRoot = true;
	else if(byteConstArr[5]&process_info)
		m_bCutBer = true;
	else if(byteConstArr[6]&process_info)
		m_bWeld = true;
	//3 1B��������Ϣ�ֽ�2(1~3λ��ʾ����ֶ�������4λ0��ʾX֫ѹ�⣬1��ʾY֫ѹ�⣬5~8λ����)
	process_info=0;
	buffer.ReadByte(&process_info);
	//3.1 ����ֶ�����1~3λ��ʾ����ֶ�����
	poly_part_num=process_info&0x07;
	//3.2 ѹ��֫(��4λ0��ʾX֫ѹ�⣬1��ʾY֫ѹ�⣬5~8λ����)
	if(byteConstArr[3]&process_info)
		wing_push_X1_Y2 = 2;			//��ʾy֫ѹ��
	else
		wing_push_X1_Y2 = 1;			//��ʾx֫ѹ��
	//4 1B��������Ϣ�ֽ�3����1~7λ����������������8λ��0��ʾX֫���Ͻǣ�1��ʾY֫���Ͻǣ�
	process_info=0;
	buffer.ReadByte(&process_info);
	//4.1 ����������(��1~7λ)
	BYTE nKaiHeJiaoCount=process_info&0x7F;
	//4.2 ��8λ��0��ʾX֫���Ͻǣ�1��ʾY֫���Ͻ�
	if(process_info&0x80)
		kaihe_base_wing_x0_y1=1;	//0��ʾX֫���Ͻǣ�1��ʾY֫���Ͻ�
	else
		kaihe_base_wing_x0_y1=0;
	//5. 48B �н���֫
	memset(_cut_wing,0,sizeof(long)*2);
	memset(_cut_wing_para,0,sizeof(float)*6);
	memset(_cut_angle,0,sizeof(float)*8);
	BOOL bCutAngleArr[4]={FALSE,FALSE,FALSE,FALSE};
	for(int i=0;i<4;i++)
	{
		WORD wPara[3]={0};
		buffer.ReadWord(&wPara[0]);
		buffer.ReadWord(&wPara[1]);
		buffer.ReadWord(&wPara[2]);
		if(wPara[2]!=0xFFFF)
		{	//��֫
			int index=i<2?0:1;
			cut_wing_para[index][0]=wPara[0];
			cut_wing_para[index][1]=wPara[1];
			cut_wing_para[index][2]=wPara[2];
		}
		else 
		{
			cut_angle[i][0]=wPara[0];
			cut_angle[i][1]=wPara[1];
			bCutAngleArr[i]=TRUE;
		}
	}
	//���ݽǸֹ��ձ��λ����ʼ���Ǹ���֫��Ϣ
	if((byteConstArr[0]&cut_info)&&!bCutAngleArr[0])
		cut_wing[0]=1;	//ʼ��X֫��֫
	else if((byteConstArr[1]&cut_info)&&!bCutAngleArr[1])
		cut_wing[0]=2;	//ʼ��Y֫��֫
	if((byteConstArr[2]&cut_info)&&!bCutAngleArr[2])
		cut_wing[1]=1;	//�ն�X֫��֫
	else if((byteConstArr[3]&cut_info)&&!bCutAngleArr[3])
		cut_wing[1]=2;	//�ն�Y֫��֫
	//6 ��˨������Ϣ
	m_xBoltInfoList.Empty();
	for (int i=0;i<bolt_count;i++)
	{
		BOLT_INFO bolt_info;
		if(bSaveSubSerial)
			buffer.ReadInteger(&bolt_info.keyId);
		bolt_info.hole_d_increment = bolt_d_increment;
		bolt_info.FromBuffer(buffer,version,buf_type);
		m_xBoltInfoList.SetValue(bolt_info.keyId,bolt_info,true);
	}
	//�ֶνǸ�����Ӧ�Ĺ���������ʶ����֫չ�����򡢣�֫չ���������췽��
	poly_part_info.Empty();
	for (int i=0;i<poly_part_num;i++)
	{
		POLY_ANGLE_INFO poly_part;
		buffer.ReadInteger(&poly_part.nIndex);
		buffer.ReadPoint(poly_part.norm_x_wing);
		buffer.ReadPoint(poly_part.norm_y_wing);
		buffer.ReadPoint(poly_part.len_vec);
		poly_part_info.append(poly_part);
	}
	//7 ѹ��
	buffer.ReadWord(&_start_push_pos);
	buffer.ReadWord(&_end_push_pos);
	if(_start_push_pos>=_end_push_pos)
		wing_push_X1_Y2=0;	//��ѹ��ʱѹ��֫����Ϊ��,������ƽǸֹ��տ���������ѹ���ʶ�� wht 16-04-27
	//8 ���Ͻ�
	kaiHeJiaoList.Empty();
	for(int i=0;i<nKaiHeJiaoCount;i++)
	{
		KAI_HE_JIAO kai_he_jiao;
		buffer.ReadFloat(&kai_he_jiao.decWingAngle);		//��֫�н�
		buffer.ReadWord(&kai_he_jiao.position);				//�궨λ��
		buffer.ReadWord(&kai_he_jiao.startLength);			//ʼ�˿��ϳ���
		buffer.ReadWord(&kai_he_jiao.endLength);			//�ն˿��ϳ���
		kaiHeJiaoList.append(kai_he_jiao);
	}
	//9 �Ǹ��߰�
	rib_plate_list.Empty();
	for(int i=0;i<rib_plate_count;i++)
	{
		RIB_PLATE_INFO rib_plate_info;
		buffer.ReadFloat(&rib_plate_info.fPosZ);				//�߰�λ��
		buffer.ReadFloat(&rib_plate_info.fThick);				//�߰���
		buffer.Read(&rib_plate_info.m_cMaterial,sizeof(char));	//����
		buffer.Read(rib_plate_info.sPlateNo,16*sizeof(char));	//�߰���
		rib_plate_list.append(rib_plate_info);
	}
	//��������Ǹ���˨�����ڶ̽Ǹ����
	RetrievedPolyAngleBoltFaceNo(this);
}

double CProcessAngle::GetLsG(GECS &acs,f3dPoint ls_norm,f3dPoint ls_pos,int* wingx0_y1/*=NULL*/)
{
	f3dPoint perp,ls_vec,jg_vec,norm,base_pos;
	f3dPoint norm_x_wing=-acs.axis_y,norm_y_wing=-acs.axis_x;
	KAI_HE_JIAO *pKaiHe=kaiHeJiaoList.GetFirst();
	if(pKaiHe)
	{	//acs����ʱ��X֫Ϊ��׼��������ڿ��ϽǸִ˴������Y֫���߷���
		double angle=(pKaiHe->decWingAngle-90)*RADTODEG_COEF;
		RotateVectorAroundVector(norm_y_wing,angle,acs.axis_z);
	}
	f3dPoint cur_wing_norm=norm_y_wing;
	int x0_y1_wing=1;
	if( fabs(norm_x_wing*ls_norm)>fabs(norm_y_wing*ls_norm))
	{
		cur_wing_norm=norm_x_wing;
		x0_y1_wing=0;
	}
	f3dPoint startPt=acs.origin,endPt=acs.origin+acs.axis_z*m_wLength;
	SnapPerp(&perp,startPt,endPt,ls_pos);
	base_pos = ls_pos;
	project_point(base_pos,perp,cur_wing_norm);
	double dd = DistOf3dPtLine(base_pos,startPt,endPt);
	/*
	//�ж���˨�����ߵ���һ��
	ls_vec=base_pos-startPt;
	jg_vec=endPt-startPt;
	normalize(ls_vec);
	normalize(jg_vec);
	norm=jg_vec^ls_vec;
	double dd1=norm*cur_wing_norm;
	if((dd1>0&&x0_y1_wing==0)||(dd1<0&&x0_y1_wing==1))
		dd=-dd;*/
	if(wingx0_y1)
		*wingx0_y1=x0_y1_wing;
	return dd;
}
GECS GetPolyAngleUCS(GECS *pAcs,POLY_ANGLE_INFO *pPolyAngle,GEPOINT org,int base_x0_y1_mid2)
{
	//--------VVVVVVVVVV----����Ǹ��������ϵ�����������᷽��  Wjh--00.05.15-------
	//�����������������Ϊ�����������������û�������֫���߷����п���
	//��������ʱ������������Z�᷽��ļ���Ϊ��ߣ����ɱ䣩�����ΪX�ᡢY��
	//Z�����Ǹֵ����췽��X��Y��ֱ������֫����չ����
	GECS ucs;
	ucs.origin = org;
	//����Ǹ�Z�᷽��
	ucs.axis_z=pPolyAngle->len_vec;
	if(base_x0_y1_mid2==0)			//X֫Ϊ����ϵ��׼
	{
		ucs.axis_x = ucs.axis_z^pPolyAngle->norm_x_wing;
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
	}
	else if(base_x0_y1_mid2==1)		//Y֫Ϊ����ϵ��׼
	{
		ucs.axis_y = pPolyAngle->norm_y_wing^ucs.axis_z;
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
	}
	else// if(base_x0_y1_mid2==2)	//��֫�н�ƽ����Ϊ��׼
	{
		f3dPoint bisec,vec;
		bisec=-pPolyAngle->norm_x_wing-pPolyAngle->norm_y_wing;
		vec = pPolyAngle->norm_x_wing-pPolyAngle->norm_y_wing;
		Standarized(vec);
		ucs.axis_x = bisec+bisec.mod()*vec;
		ucs.axis_y = bisec-bisec.mod()*vec;
	}
	if(pAcs)
	{
		vector_trans(ucs.axis_x,*pAcs,TRUE);
		vector_trans(ucs.axis_y,*pAcs,TRUE);
		vector_trans(ucs.axis_z,*pAcs,TRUE);
	}
#ifdef _LEGACY_LICENSE
	Standarized(f3dPoint(ucs.axis_x));
	Standarized(f3dPoint(ucs.axis_y));
	Standarized(f3dPoint(ucs.axis_z));
#else
	Standarized(ucs.axis_x);
	Standarized(ucs.axis_y);
	Standarized(ucs.axis_z);
#endif
	return ucs;
}

BOOL CProcessAngle::SpreadLsPos(GECS &acs,f3dPoint ls_norm,double* ls_pos_coord)
{	
	f3dPoint ls_pos(ls_pos_coord);
	int wing_x0_y1=0;
	if(poly_part_info.GetNodeNum()<=0)
	{
		double g=GetLsG(acs,ls_norm,ls_pos,&wing_x0_y1);
		coord_trans(ls_pos,acs,FALSE);
		ls_pos.Set(ls_pos.z,g,0);
		if(wing_x0_y1==0)
			ls_pos.y*=-1;
		ls_pos_coord[0]=ls_pos.x;
		ls_pos_coord[1]=ls_pos.y;
		ls_pos_coord[2]=ls_pos.z;
		return TRUE;
	}
	else
	{
		f3dPoint base_pos=ls_pos;
		coord_trans(base_pos,acs,FALSE);
		double fLen=0,fPrevLen=0;
		double huoqu_r=0;
		int nMinSpace=17;	//ȡM12��˨��С���Ʊ߾�
		f3dPoint org=acs.origin,local_ls_norm=ls_norm;
		vector_trans(local_ls_norm,acs,FALSE);
		POLY_ANGLE_INFO *pCurPolyAngle=NULL,*pPrevPolyAngle=NULL;
		for(int i=0;i<poly_part_num;i++)
		{
			fPrevLen=fLen;
			POLY_ANGLE_INFO *pPolyAngle=poly_part_info.GetByIndex(i);
			fLen+=pPolyAngle->len_vec.mod();
			if(pPrevPolyAngle)
			{
				f3dPoint vec_start,vec_end;
				vec_start = pPrevPolyAngle->len_vec;
				vec_end   = pPolyAngle->len_vec;
				Standarized(vec_start);
				Standarized(vec_end);
				double angle = cal_angle_of_2vec(vec_start,vec_end)/2.0;
				double sub_len = ((Pi/2-angle)*huoqu_r-tan(Pi/2.0-angle)*huoqu_r)*2;
				fLen+=sub_len;
				//
				vector_trans(vec_start,acs,TRUE);
				normalize(vec_start);
				org+=vec_start*pPrevPolyAngle->len_vec.mod();
			}
			pPrevPolyAngle=pPolyAngle;
			//��˨Z����������ڵ�ǰ�Ǹֳ���(����ǶȽϴ�ʱ����˨���ڵ�ǰ�Ǹ��ϵ���Z���򳤶ȿ������ڵ�ǰ�Ǹַ�Χ��) 
			//TODO: ���ַ����ж���˨�����ӽǸֶַλ������Ͻ������Ż� wht 15-09-24
			if(base_pos.z+nMinSpace>fLen)
				continue;		
			double dd1=fabs(local_ls_norm*pPolyAngle->norm_x_wing);
			double dd2=fabs(local_ls_norm*pPolyAngle->norm_y_wing);
			if((dd1>EPS_COS2||dd2>EPS_COS2))
			{
				pCurPolyAngle=pPolyAngle;
				break;
			}
		}
		if(pCurPolyAngle)
		{
			GECS subAcs=GetPolyAngleUCS(&acs,pCurPolyAngle,org,0);
			double g=GetLsG(subAcs,ls_norm,ls_pos,&wing_x0_y1);
			coord_trans(ls_pos,subAcs,FALSE);
			ls_pos.Set(ls_pos.z+fPrevLen,g,0);
			if(wing_x0_y1==0)
				ls_pos.y*=-1;
			ls_pos_coord[0]=ls_pos.x;
			ls_pos_coord[1]=ls_pos.y;
			ls_pos_coord[2]=ls_pos.z;
			return TRUE;
		}
		else 
			return FALSE;
	}
	return FALSE;
}
f3dPoint CProcessAngle::RestoreLsPos(const double* ls_pos,int iFace012/*=0*/,GECS *pAcs/*=NULL*/)
{
	f3dPoint pos,start;
	GEPOINT boltpos(ls_pos);
	if(poly_part_info.GetNodeNum()<=0)
	{
		GEPOINT wing_vec=boltpos.y>0?GetWingVecY():GetWingVecX();
		GEPOINT axis_z(0,0,1);
		if(pAcs)
		{
			start=pAcs->origin;
			axis_z=pAcs->axis_z;
			vector_trans(wing_vec,*pAcs,true);
		}
		pos=start+wing_vec*fabs(boltpos.y)+axis_z*boltpos.x;
	}
	else
	{
		double fLen=0,fPrevLen=0;
		double huoqu_r=0;
		int nMinSpace=17;	//ȡM12��˨��С���Ʊ߾�
		f3dPoint org;
		if(pAcs)
			org=pAcs->origin;
		POLY_ANGLE_INFO *pCurPolyAngle=NULL,*pPrevPolyAngle=NULL;
		for(int i=0;i<poly_part_num;i++)
		{
			fPrevLen=fLen;
			POLY_ANGLE_INFO *pPolyAngle=poly_part_info.GetByIndex(i);
			fLen+=pPolyAngle->len_vec.mod();
			if(pPrevPolyAngle)
			{
				f3dPoint vec_start,vec_end;
				vec_start = pPrevPolyAngle->len_vec;
				vec_end   = pPolyAngle->len_vec;
				Standarized(vec_start);
				Standarized(vec_end);
				double angle = cal_angle_of_2vec(vec_start,vec_end)/2.0;
				double sub_len = ((Pi/2-angle)*huoqu_r-tan(Pi/2.0-angle)*huoqu_r)*2;
				fLen+=sub_len;
				//
				if(pAcs)
				{
					vector_trans(vec_start,*pAcs,TRUE);
					normalize(vec_start);
				}
				org+=vec_start*pPrevPolyAngle->len_vec.mod();
			}
			pPrevPolyAngle=pPolyAngle;
			if(boltpos.x>fPrevLen&&boltpos.x<fLen)
			{
				pCurPolyAngle=pPolyAngle;
				break;
			}
		}
		if(pCurPolyAngle)
		{
		
			GEPOINT wing_vec=boltpos.y>0?GetWingVecY():GetWingVecX();
			GEPOINT axis_z=pCurPolyAngle->len_vec;
			normalize(axis_z);
			if(pAcs)
			{
				start=org;
				vector_trans(axis_z,*pAcs,true);
				vector_trans(wing_vec,*pAcs,true);
			}
			pos=start+wing_vec*fabs(boltpos.y)+axis_z*(boltpos.x-fPrevLen);
		}
	}
	return pos;
}
bool CProcessAngle::IsEqual(CGeneralPart *pPart,double epsilon/* =1.0 */,char *error_info/*=NULL*/,DWORD dwChkStyleFlag/*=NULL*/)
{
	if(!CProcessPart::IsEqual(pPart,epsilon,error_info,dwChkStyleFlag))
		return false;
	CProcessAngle *pOtherAngle=(CProcessAngle*)pPart;
	if( cut_wing[0]!=pOtherAngle->cut_wing[0]||
		cut_wing[1]!=pOtherAngle->cut_wing[1]||
		memcmp(cut_wing_para[0],pOtherAngle->cut_wing_para[0],sizeof(float)*3)!=0||
		memcmp(cut_wing_para[1],pOtherAngle->cut_wing_para[1],sizeof(float)*3)!=0)
	{
		if(error_info)
			strcpy(error_info,"�Ǹ���֫��ͬ");
		return false;
	}
	if( memcmp(cut_angle[0],pOtherAngle->cut_angle[0],sizeof(float)*2)!=0||
		memcmp(cut_angle[1],pOtherAngle->cut_angle[1],sizeof(float)*2)!=0||
		memcmp(cut_angle[2],pOtherAngle->cut_angle[2],sizeof(float)*2)!=0||
		memcmp(cut_angle[3],pOtherAngle->cut_angle[3],sizeof(float)*2)!=0)
	{
		if(error_info)
			strcpy(error_info,"�Ǹ��нǲ�ͬ");
		return false;
	}
	if( start_push_pos!=pOtherAngle->start_push_pos||
		end_push_pos!=pOtherAngle->end_push_pos||
		wing_push_X1_Y2!=pOtherAngle->wing_push_X1_Y2)
	{
		if(error_info)
			strcpy(error_info,"�Ǹ�ѹ�ⲻͬ");
		return false;
	}
	if(kaihe_base_wing_x0_y1!=pOtherAngle->kaihe_base_wing_x0_y1)
	{
		if(error_info)
			strcpy(error_info,"���ϻ�׼֫��ͬ");
		return false;
	}
	if(poly_part_num!=pOtherAngle->poly_part_num)
	{
		if(error_info)
			strcpy(error_info,"����Ǹֶַ�����ͬ");
		return false;
	}
	if( m_bCutBer!=pOtherAngle->m_bCutBer||
		m_bCutRoot!=pOtherAngle->m_bCutRoot||
		m_bWeld!=pOtherAngle->m_bWeld||
		m_bNeedFillet!=pOtherAngle->m_bNeedFillet)
	{
		if(error_info)
			strcpy(error_info,"������Ϣ��ͬ");
		return false;
	}
	//ATOM_LIST<KAI_HE_JIAO> kaiHeJiaoList;		
	//ATOM_LIST<RIB_PLATE_INFO> rib_plate_list;		//�Ǹ��߰�����
	//ATOM_LIST<POLY_ANGLE_INFO> poly_part_info;	//����Ǹ���Ϣ
	return true;
}
#ifdef __TAP_CONTEXT_
bool CProcessAngle::IsReplacePart()
{
	if(QuerySteelMatByBriefMark(m_cOrgMaterial)==NULL||m_fOrgWidth==0||m_fOrgThick==0)
		return false;
	if( m_cOrgMaterial==cMaterial&&m_fOrgWidth==m_fWidth&&m_fOrgThick==m_fThick&&
		m_cOrgQuality==cQuality)
		return false;
	return true;
}
void CProcessAngle::FromTIDBuffer(CBuffer &buffer,long version/*=0*/)
{
	CProcessPart::FromTIDBuffer(buffer,version);
	if(!IsReplacePart())
	{	//��ʼ���Ǹ�ԭʼ���ʡ�֫��֫��
		m_cOrgMaterial=cMaterial;
		m_fOrgWidth=m_fWidth;
		m_fOrgThick=m_fThick;
	}
}
void CProcessAngle::FromTAPBuffer(CBuffer &buffer,long version/*=0*/)
{
	CProcessPart::FromTAPBuffer(buffer,version);
	buffer.ReadByte(&m_cOrgMaterial);
	if(m_cOrgMaterial!=0)
	{	//��ȡ���ϽǸ�֫��֫��
		buffer.ReadFloat(&m_fOrgWidth);
		buffer.ReadFloat(&m_fOrgThick);
	}
	if(!IsReplacePart())
	{	//��ʼ���Ǹ�ԭʼ���ʡ�֫��֫��
		m_cOrgMaterial=cMaterial;
		m_fOrgWidth=m_fWidth;
		m_fOrgThick=m_fThick;
	}
}
void CProcessAngle::ToTAPBuffer(CBuffer &buffer,bool bIncSolidData/*=true*/)
{
	CProcessPart::ToTAPBuffer(buffer,bIncSolidData);
	if(IsReplacePart())
	{
		buffer.WriteByte(m_cOrgMaterial);
		//�洢���ϽǸ�֫��֫��
		buffer.WriteFloat(m_fOrgWidth);
		buffer.WriteFloat(m_fOrgThick);
	}
	else 
		buffer.WriteByte(0);
}
#endif
//////////////////////////////////////////////////////////////////////////
// CProcessTube
CProcessTube::CProcessTube()
{
	m_cPartType=CProcessPart::TYPE_LINETUBE;
}
#ifdef __PROPERTYLIST_UI_
IMPLEMENT_PROP_FUNC(CProcessTube)
//��ʼ������id
template<class TYPE> void InitPartPropHashtable()
{
	int id=1;
	TYPE::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	TYPE::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	TYPE::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"�ֹܻ�����Ϣ"));
	TYPE::AddPropItem("m_sPartNo",PROPLIST_ITEM(id++,"�������"));
	TYPE::AddPropItem("m_Seg",PROPLIST_ITEM(id++,"�ֶκ�"));
	TYPE::AddPropItem("m_cMaterial",PROPLIST_ITEM(id++,"����"));
	TYPE::AddPropItem("m_sSpec",PROPLIST_ITEM(id++,"���"));
#ifdef __TAP_CONTEXT_
	TYPE::AddPropItem("m_cOrgMaterial",PROPLIST_ITEM(id++,"ԭ����"));
	TYPE::AddPropItem("m_sOrgSpec",PROPLIST_ITEM(id++,"ԭ���"));
#endif
	TYPE::AddPropItem("m_wLength",PROPLIST_ITEM(id++,"����"));
	TYPE::AddPropItem("m_fWeight",PROPLIST_ITEM(id++,"����"));
	TYPE::AddPropItem("m_cFuncType",PROPLIST_ITEM(id++,"��������","�Ǹֹ���","��ͨ�Ǹ�|�Ӿ��Ǹ�|�Ŷ��Ǹ�|���ȽǸ�|����Ǹ�|��ͷ�Ǹ�"));
	TYPE::AddPropItem("m_sNote",PROPLIST_ITEM(id++,"��ע"));
	TYPE::AddPropItem("m_dwInheritPropFlag",PROPLIST_ITEM(id++,"ģʽ","","...."));
	//
	TYPE::AddPropItem("Place",PROPLIST_ITEM(id++,"ռλ"));
	TYPE::AddPropItem("m_cPlaceMode",PROPLIST_ITEM(id++,"ռλģʽ","ռλģʽ","0.����ռλ|1.4ռλģʽ1|2.4ռλģʽ2|3.8ռλģʽ1|4.8ռλģʽ2"));
	TYPE::AddPropItem("POS_FRONT_PLUS",PROPLIST_ITEM(id++,"ǰ","","���|ɾ��"));
	TYPE::AddPropItem("POS_BACK_PLUS",PROPLIST_ITEM(id++,"��","","���|ɾ��"));
	TYPE::AddPropItem("POS_LEFT_PLUS",PROPLIST_ITEM(id++,"��","","���|ɾ��"));
	TYPE::AddPropItem("POS_RIGHT_PLUS",PROPLIST_ITEM(id++,"��","","���|ɾ��"));
	TYPE::AddPropItem("POS_FRONT_MINUS",PROPLIST_ITEM(id++,"ǰX-","","���|ɾ��"));
	TYPE::AddPropItem("POS_BACK_MINUS",PROPLIST_ITEM(id++,"��X-","","���|ɾ��"));
	TYPE::AddPropItem("POS_LEFT_MINUS",PROPLIST_ITEM(id++,"��Y-","","���|ɾ��"));
	TYPE::AddPropItem("POS_RIGHT_MINUS",PROPLIST_ITEM(id++,"��Y-","","���|ɾ��"));
}
//ͨ������Id��ȡ����ֵ
template<class TYPE> 
int GetPartPropValueStr(CProcessPart *pPart,long id, char *valueStr)
{
	CXhChar100 sText;
	if(TYPE::GetPropID("m_sPartNo")==id)
		sText.Copy(pPart->GetPartNo());
	else if(TYPE::GetPropID("m_Seg")==id)
		sText.Copy((char*)pPart->GetSegI().ToString());
	else if(TYPE::GetPropID("m_cMaterial")==id)	
	{
		char matStr[20];
		CProcessPart::QuerySteelMatMark(pPart->cMaterial,matStr);
		sText.Printf("%s",matStr);
	}
	else if(TYPE::GetPropID("m_sSpec")==id)
		sText.Printf("%dX%d",(int)pPart->m_fWidth,(int)pPart->m_fThick);
#ifdef __TAP_CONTEXT_
	else if(TYPE::GetPropID("m_cOrgMaterial")==id)	
	{
		char matStr[20];
		CProcessPart::QuerySteelMatMark(pPart->m_cOrgMaterial,matStr);
		sText.Printf("%s",matStr);
	}
	else if(TYPE::GetPropID("m_sOrgSpec")==id)
		sText.Printf("%dX%d",(int)pPart->m_fOrgWidth,(int)pPart->m_fOrgThick);
	else if(TYPE::GetPropID("m_cPlaceMode")==id)
	{
		CXhChar100 cmbItemArr[5]={"0.����ռλ","1.4ռλģʽ1","2.4ռλģʽ2","3.8ռλģʽ1","4.8ռλģʽ2"};
		if(pPart->m_cPlaceMode>=0||pPart->m_cPlaceMode<=4)
			sText.Copy(cmbItemArr[pPart->m_cPlaceMode]);
		else
			sText.Empty();
	}
	else if(TYPE::GetPropID("POS_FRONT_PLUS")==id)
	{
		if(pPart->IsIdleClassicPlace(ASSEMBLEY_POS::POS_FRONT_PLUS))
			sText.Empty();
		else if(TYPE::GetPlaceContentFunc)
			sText.Copy(TYPE::GetPlaceContentFunc(pPart,ASSEMBLEY_POS::POS_FRONT_PLUS));
		else 
			sText.Copy("��");
	}
	else if(TYPE::GetPropID("POS_BACK_PLUS")==id)
	{
		if(pPart->IsIdleClassicPlace(ASSEMBLEY_POS::POS_BACK_PLUS))
			sText.Empty();
		else if(TYPE::GetPlaceContentFunc)
			sText.Copy(TYPE::GetPlaceContentFunc(pPart,ASSEMBLEY_POS::POS_BACK_PLUS));
		else 
			sText.Copy("��");
	}
	else if(TYPE::GetPropID("POS_LEFT_PLUS")==id)
	{
		if(pPart->IsIdleClassicPlace(ASSEMBLEY_POS::POS_LEFT_PLUS))
			sText.Empty();
		else if(TYPE::GetPlaceContentFunc)
			sText.Copy(TYPE::GetPlaceContentFunc(pPart,ASSEMBLEY_POS::POS_LEFT_PLUS));
		else 
			sText.Copy("��");
	}
	else if(TYPE::GetPropID("POS_RIGHT_PLUS")==id)
	{
		if(pPart->IsIdleClassicPlace(ASSEMBLEY_POS::POS_RIGHT_PLUS))
			sText.Empty();
		else if(TYPE::GetPlaceContentFunc)
			sText.Copy(TYPE::GetPlaceContentFunc(pPart,ASSEMBLEY_POS::POS_RIGHT_PLUS));
		else 
			sText.Copy("��");
	}
	else if(TYPE::GetPropID("POS_FRONT_MINUS")==id)
	{
		if(pPart->IsIdleClassicPlace(ASSEMBLEY_POS::POS_FRONT_MINUS))
			sText.Empty();
		else if(TYPE::GetPlaceContentFunc)
			sText.Copy(TYPE::GetPlaceContentFunc(pPart,ASSEMBLEY_POS::POS_FRONT_MINUS));
		else 
			sText.Copy("��");
	}
	else if(TYPE::GetPropID("POS_BACK_MINUS")==id)
	{
		if(pPart->IsIdleClassicPlace(ASSEMBLEY_POS::POS_BACK_MINUS))
			sText.Empty();
		else if(TYPE::GetPlaceContentFunc)
			sText.Copy(TYPE::GetPlaceContentFunc(pPart,ASSEMBLEY_POS::POS_BACK_MINUS));
		else 
			sText.Copy("��");
	}
	else if(TYPE::GetPropID("POS_LEFT_MINUS")==id)
	{
		if(pPart->IsIdleClassicPlace(ASSEMBLEY_POS::POS_LEFT_MINUS))
			sText.Empty();
		else if(TYPE::GetPlaceContentFunc)
			sText.Copy(TYPE::GetPlaceContentFunc(pPart,ASSEMBLEY_POS::POS_LEFT_MINUS));
		else 
			sText.Copy("��");
	}
	else if(TYPE::GetPropID("POS_RIGHT_MINUS")==id)
	{
		if(pPart->IsIdleClassicPlace(ASSEMBLEY_POS::POS_RIGHT_MINUS))
			sText.Empty();
		else if(TYPE::GetPlaceContentFunc)
			sText.Copy(TYPE::GetPlaceContentFunc(pPart,ASSEMBLEY_POS::POS_RIGHT_MINUS));
		else 
			sText.Copy("��");
	}
#endif
	else if(TYPE::GetPropID("m_wLength")==id)
		sText.Printf("%d",pPart->m_wLength);
	else if(TYPE::GetPropID("m_fWeight")==id)
	{
		sText.Printf("%f",pPart->m_fWeight);
		SimplifiedNumString(sText);
	}
	else if(TYPE::GetPropID("m_sNote")==id)
		sText.Copy(pPart->GetNotes()); 
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
void CProcessTube::InitPropHashtable()
{
	InitPartPropHashtable<CProcessTube>();
}
int CProcessTube::GetPropValueStr(long id, char *valueStr)
{
	return GetPartPropValueStr<CProcessTube>(this,id,valueStr);
}
#endif
//////////////////////////////////////////////////////////////////////////
// CProcessSlot
CProcessSlot::CProcessSlot()
{
	m_cPartType=CProcessPart::TYPE_LINESLOT;
}
#ifdef __PROPERTYLIST_UI_
IMPLEMENT_PROP_FUNC(CProcessSlot)
void CProcessSlot::InitPropHashtable()
{
	InitPartPropHashtable<CProcessSlot>();
}
int CProcessSlot::GetPropValueStr(long id, char *valueStr)
{
	return GetPartPropValueStr<CProcessSlot>(this,id,valueStr);
}
#endif
//////////////////////////////////////////////////////////////////////////
// CProcessFlat
CProcessFlat::CProcessFlat()
{
	m_cPartType=CProcessPart::TYPE_LINEFLAT;
}
#ifdef __PROPERTYLIST_UI_
IMPLEMENT_PROP_FUNC(CProcessFlat)
void CProcessFlat::InitPropHashtable()
{
	InitPartPropHashtable<CProcessFlat>();
}
int CProcessFlat::GetPropValueStr(long id, char *valueStr)
{
	return GetPartPropValueStr<CProcessFlat>(this,id,valueStr);
}
#endif
//////////////////////////////////////////////////////////////////////////
//CProcessBolt
//////////////////////////////////////////////////////////////////////////
#ifdef __TAP_CONTEXT_
#ifdef __PROPERTYLIST_UI_
IMPLEMENT_PROP_FUNC(CBoltAssembly)
void CBoltAssembly::InitPropHashtable()
{
	int id=1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"��˨������Ϣ"));
	AddPropItem("m_cFamily",PROPLIST_ITEM(id++,"��˨���ϵ��"));
	AddPropItem("m_sSpec",PROPLIST_ITEM(id++,"��˨���"));
	AddPropItem("hole_d_increment",PROPLIST_ITEM(id++,"�ݿ׼�϶"));
	AddPropItem("L0",PROPLIST_ITEM(id++,"ͨ�����"));
	AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	AddPropItem("statSegStr",PROPLIST_ITEM(id++,"�κ�ͳ�Ʒ�Χ"));
	AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	AddPropItem("dwRayNo",PROPLIST_ITEM(id++,"���ߺ�"));
	AddPropItem("DianQuan",PROPLIST_ITEM(id++,"��Ȧ��Ϣ"));
	AddPropItem("cDianQuanThick",PROPLIST_ITEM(id++,"��Ȧ���"));
	AddPropItem("cDianQuanN",PROPLIST_ITEM(id++,"��Ȧ����"));
	AddPropItem("wDianQuanOffset",PROPLIST_ITEM(id++,"��Ȧλ��"));
	AddPropItem("ANTI_LOOSE",PROPLIST_ITEM(id++,"�����ɵ�Ȧ",NULL,"��|��"));
	AddPropItem("ANTI_THEFT",PROPLIST_ITEM(id++,"������˨",NULL,"��|��"));
	AddPropItem("TWO_CAP",PROPLIST_ITEM(id++,"˫ñ��˨",NULL,"��|��"));
	AddPropItem("origin",PROPLIST_ITEM(id++,"λ��"));
	AddPropItem("origin.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("origin.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("origin.z",PROPLIST_ITEM(id++,"Z"));
	AddPropItem("axis_x",PROPLIST_ITEM(id++,"X�᷽��"));
	AddPropItem("axis_x.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("axis_x.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("axis_x.z",PROPLIST_ITEM(id++,"Z"));
	AddPropItem("work_norm",PROPLIST_ITEM(id++,"��������"));
	AddPropItem("work_norm.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("work_norm.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("work_norm.z",PROPLIST_ITEM(id++,"Y"));
}

int CBoltAssembly::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(GetPropID("m_cFamily")==id)
	{
		if(m_pBolt)
		{
			CLsFamily* pFamily=NULL;
			if(m_pBolt->m_pBelongBoltLib)
				pFamily=m_pBolt->m_pBelongBoltLib->FromHandle(m_pBolt->m_cFamily);
			if(pFamily==NULL)
				pFamily=CLsLibrary::FromHandle(m_pBolt->m_cFamily);
			if(pFamily==NULL)
				pFamily=CLsLibrary::DefaultFamily();
			if(pFamily)
				strcpy(sText,pFamily->name);
		}
	}
	else if(GetPropID("m_sSpec")==id)
	{
		if(m_pBolt)
			sText.Copy(m_pBolt->GetPartNo());
	}
	//else if(GetPropID("hole_d_increment")==id)
	else if(GetPropID("L0")==id)
		sText.Printf("%d",L0);
	else if(GetPropID("iSeg")==id)
		sText.Copy(iSeg.ToString());
	else if(GetPropID("statSegStr")==id)
		sText.Copy(statSegStr);
	else if(GetPropID("cfgword")==id)
	{
		if(cLegQuad!=0)
			cfgword.GetLegScopeStr(sText,17,true);
		else 
			cfgword.GetBodyScopeStr(sText);
	}
	//else if(GetPropID("dwRayNo")==id)
	//	MakeWordToString(dwRayNo,valueStr)
	else if(GetPropID("DianQuan")==id)
	{
		if(cDianQuanN>0&&cDianQuanThick>0)
			sText.Printf("-%dx%d",(BYTE)cDianQuanThick,(BYTE)cDianQuanN);
	}
	else if(GetPropID("cDianQuanThick")==id)
		sText.Printf("%d",(BYTE)cDianQuanThick);
	else if(GetPropID("cDianQuanN")==id)
		sText.Printf("%d",(BYTE)cDianQuanN);
	else if(GetPropID("wDianQuanOffset")==id)
		sText.Printf("%d",wDianQuanOffset);
	else if(GetPropID("ANTI_LOOSE")==id)
	{
		BITFLAG bitFlag(cFlag);
		if(bitFlag.IsHasFlag(BOLT_INFO::ANTI_LOOSE))
			sText.Printf("��");
		else 
			sText.Printf("��");
	}
	else if(GetPropID("ANTI_THEFT")==id)
	{
		BITFLAG bitFlag(cFlag);
		if(bitFlag.IsHasFlag(BOLT_INFO::ANTI_THEFT))
			sText.Printf("��");
		else 
			sText.Printf("��");
	}
	else if(GetPropID("TWO_CAP")==id)
	{
		BITFLAG bitFlag(cFlag);
		if(bitFlag.IsHasFlag(BOLT_INFO::TWO_CAP))
			sText.Printf("��");
		else 
			sText.Printf("��");
	}
	else if(GetPropID("origin")==id)
		sText.ConvertFromPoint(origin,3);
	else if(GetPropID("origin.x")==id)
	{
		sText.Printf("%f",origin.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("origin.y")==id)
	{
		sText.Printf("%f",origin.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("origin.z")==id)
	{
		sText.Printf("%f",origin.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("work_norm")==id)
		sText.ConvertFromPoint(work_norm,3);
	else if(GetPropID("work_norm.x")==id)
	{
		sText.Printf("%f",work_norm.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("work_norm.y")==id)
	{
		sText.Printf("%f",work_norm.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("work_norm.z")==id)
	{
		sText.Printf("%f",work_norm.z);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
#endif
CBoltAssembly::CBoltAssembly()
{
	memset(this,0,sizeof(CBoltAssembly));
}
CBoltAssembly::~CBoltAssembly()
{

}
CBoltAssembly::CBoltAssembly(const CBoltAssembly &srcParam)
{
	CloneFrom(srcParam);
}
CBoltAssembly& CBoltAssembly::operator =(const CBoltAssembly &srcParam)
{
	CloneFrom(srcParam);
	return *this;
}
void CBoltAssembly::SetBelongBolt(CStdBolt *pBolt)
{
	m_pBolt=pBolt;
	if(m_pBolt)
		idBolt=m_pBolt->GetKey();
}
void CBoltAssembly::FromBuffer(CBuffer &buffer,long version/*=0*/)
{
	buffer.ReadPoint(origin);
	buffer.ReadPoint(work_norm);
	buffer.ReadWord(&L0);
	buffer.ReadByte(&cFlag);
	buffer.ReadByte(&cDianQuanN);
	buffer.ReadByte(&cDianQuanThick);
	buffer.ReadWord(&wDianQuanOffset);
	WORD wIndex;
	buffer.ReadWord(&wIndex);
	m_dwBlockIndex=wIndex;
	if(m_dwBlockIndex==0)
	{
		buffer.ReadDword((DWORD*)&iSeg.iSeg);
		buffer.ReadString(statSegStr);
		buffer.ReadByte(&cLegQuad);
		buffer.Read(&cfgword,24);
	}
}
void CBoltAssembly::ToBuffer(CBuffer &buffer) const
{
	double org_pos[3]={origin.x,origin.y,origin.z};
	double norm[3]={work_norm.x,work_norm.y,work_norm.z};
	buffer.WritePoint(org_pos);
	buffer.WritePoint(norm);
	buffer.WriteWord(L0);
	buffer.WriteByte(cFlag);
	buffer.WriteByte(cDianQuanN);
	buffer.WriteByte(cDianQuanThick);
	buffer.WriteWord(wDianQuanOffset);
	buffer.WriteWord((WORD)m_dwBlockIndex);
	if(m_dwBlockIndex==0)
	{
		buffer.WriteDword((DWORD)iSeg.iSeg);
		buffer.WriteString(statSegStr);
		buffer.WriteByte(cLegQuad);
		buffer.Write(&cfgword,24);
	}
}
GECS CBoltAssembly::GetACS()
{
	if(!acs.origin.IsEqual(origin.x,origin.y,origin.z)||fabs(acs.axis_z*work_norm)<EPS_COS)
	{
		acs.origin=origin;
		acs.axis_z=work_norm;
		acs.axis_x=inters_vec(f3dPoint(work_norm));
		acs.axis_y=acs.axis_z^acs.axis_x;
		Standarized(acs.axis_y);
		acs.axis_x=acs.axis_y^acs.axis_z;
		Standarized(acs.axis_x);
	}
	return acs;
}
void CBoltAssembly::CloneFrom(const CBoltAssembly &srcBoltAssembly)
{
	CBuffer buffer;
	srcBoltAssembly.ToBuffer(buffer);
	buffer.SeekToBegin();
	FromBuffer(buffer);
	SetBelongBolt(srcBoltAssembly.BelongBolt());
}
bool CBoltAssembly::IsInited()
{
	if(origin.IsZero()&&work_norm.IsZero()&&L0==0)
		return false;
	else
		return true;
}
#ifdef _DEBUG
int CBoltAssembly::GetMemorySize()
{
	return sizeof(CBoltAssembly);
}
#endif
//
CStdBolt::CStdBolt()
{
	_cPartType=TYPE_BOLT;
	m_cFamily=0;
	m_wIndex=0;
	grade=6.8f;
	d=20;
	L=65;
	down_lim=26;
	up_lim=35;
	m_pBelongBoltLib=NULL;
}
CStdBolt::~CStdBolt()
{
	solidOfCap.Empty();
	solidOfBolt.Empty();
}
void CStdBolt::FromTIDBuffer(CBuffer &buffer,long version/* =0 */)
{
	buffer.ReadByte(&m_cFamily);
	buffer.ReadWord(&m_wIndex);
	buffer.ReadFloat(&grade);
	CLsFamily* pLsFamily=NULL;
	if(m_pBelongBoltLib)
		pLsFamily=m_pBelongBoltLib->FromHandle(m_cFamily);
	if(pLsFamily==NULL)
		pLsFamily=CLsLibrary::FromHandle(m_cFamily);
	if(pLsFamily==NULL)
		pLsFamily=CLsLibrary::DefaultFamily();
	if(pLsFamily)
	{
		LS_XING_HAO *pLsXingHao=NULL;
		if((m_wIndex&0x8000)==0&&m_wIndex>0)
			pLsXingHao=pLsFamily->RecordAt(m_wIndex-1);
		if(pLsXingHao)
		{
			d=pLsXingHao->d;
			L=pLsXingHao->valid_len;
			down_lim=pLsXingHao->down_lim;
			up_lim=pLsXingHao->up_lim;
			strcpy(m_sSpec,pLsXingHao->guige);
			m_fWeight=(float)pLsXingHao->weight;
		}
		else 
		{
			if((m_wIndex&0x8000)>0)
			{	//�ǳ�����˨���
				d=(m_wIndex>>8)&0x7f;
				L=m_wIndex&0xff;
				m_fWeight=0;
				logerr.Log("��˨ϵ��%dδ�ҵ���Ӧ����˨M%dX%g",m_cFamily,d,L);
			}
			else
				logerr.Log("��˨ϵ��%d: %d,δ�ҵ���Ӧ����˨",m_cFamily,m_wIndex);
		}
	}
	else 
		logerr.Log("δ�ҵ���˨ϵ��%d",m_cFamily);
}
void CStdBolt::FromTAPBuffer(CBuffer &buffer,long version/*=0*/)
{
	buffer.ReadByte(&m_cFamily);
	buffer.ReadWord(&m_wIndex);
	buffer.ReadFloat(&grade);
	//����m_cFamily��m_wIndex��ȡ��˨�ͺ�
	CLsFamily* pLsFamily=NULL;
	if(m_pBelongBoltLib)
		pLsFamily=m_pBelongBoltLib->FromHandle(m_cFamily);
	if(pLsFamily==NULL)
		pLsFamily=CLsLibrary::FromHandle(m_cFamily);
	if(pLsFamily==NULL)
		pLsFamily=CLsLibrary::DefaultFamily();
	if(pLsFamily)
	{
		LS_XING_HAO *pLsXingHao=NULL;
		if((m_wIndex&0x8000)==0&&m_wIndex>0)
			pLsXingHao=pLsFamily->RecordAt(m_wIndex-1);
		if(pLsXingHao)
		{
			d=pLsXingHao->d;
			L=pLsXingHao->valid_len;
			down_lim=pLsXingHao->down_lim;
			up_lim=pLsXingHao->up_lim;
			strcpy(m_sSpec,pLsXingHao->guige);
			m_fWeight=(float)pLsXingHao->weight;
		}
		else if((m_wIndex&0x8000)>0)
		{	//�ǳ�����˨���
			d=(m_wIndex>>8)&0x7f;
			L=m_wIndex&0xff;
			m_fWeight=0;
			logerr.Log("��˨ϵ��%dδ�ҵ���Ӧ����˨M%dX%g",m_cFamily,d,L);
		}
		else
			logerr.Log("��˨ϵ��%d: %d,δ�ҵ���Ӧ����˨",m_cFamily,m_wIndex);
	}
	else 
		logerr.Log("δ�ҵ���˨ϵ��%d",m_cFamily);
	//��ȡ��˨��ʵ������
	DWORD dwSolidSize=0;
	buffer.ReadDword(&dwSolidSize);
	if(dwSolidSize>0)
	{
		solidOfBolt.CopyBuffer(buffer.GetCursorBuffer(),dwSolidSize);
		buffer.Offset(dwSolidSize);
	}
	//��ȡ��˨ñʵ������
	buffer.ReadDword(&dwSolidSize);
	if(dwSolidSize>0)
	{
		solidOfCap.CopyBuffer(buffer.GetCursorBuffer(),dwSolidSize);
		buffer.Offset(dwSolidSize);
	}
}
void CStdBolt::ToTAPBuffer(CBuffer &buffer,bool bIncSolidData/*=true*/)
{
	buffer.WriteByte(m_cFamily);
	buffer.WriteWord(m_wIndex);
	buffer.WriteFloat(grade);
	//д����˨��ʵ������
	DWORD dwSolidSize=bIncSolidData?solidOfBolt.BufferLength():0;
	buffer.WriteDword(dwSolidSize);
	if(dwSolidSize>0)
		buffer.Write(solidOfBolt.BufferPtr(),solidOfBolt.BufferLength());
	//д����˨ñʵ������
	dwSolidSize=bIncSolidData?solidOfCap.BufferLength():0;
	buffer.WriteDword(dwSolidSize);
	if(dwSolidSize>0)
		buffer.Write(solidOfCap.BufferPtr(),solidOfCap.BufferLength());
}
#endif
