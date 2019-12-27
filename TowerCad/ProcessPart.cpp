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

const DWORD BOLT_INFO::TWO_CAP   =0X00000001;	//脚钉螺栓标识位
const DWORD BOLT_INFO::ANTI_THEFT=0X00000002;	//防盗螺栓标识位
const DWORD BOLT_INFO::ANTI_LOOSE=0X00000004;	//含防松垫圈标识位
const DWORD BOLT_INFO::FOOT_NAIL =0X00000008;	//脚钉螺栓标识位
const DWORD BOLT_INFO::REVERSE_NORM =0X00000010;//螺栓朝向标识位
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
	if(vertex.IsEqual(endPoint))//防止因两点重合生成圆弧抛出异常
		arcline=f3dArcLine(f3dLine(vertex,endPoint));
	else if(type==1)	//普通直边
		arcline.CreateMethod2(vertex,endPoint,f3dPoint(0,0,1),0);
	else if(type==2)	//指定圆弧夹角
	{
		int sign=1;
		if(sector_angle<0)
			sign*=-1;
		if(work_norm.z<0)
			sign*=-1;
		arcline.CreateMethod2(vertex,endPoint,f3dPoint(0,0,sign),fabs(sector_angle));
	}
	else if(type==3)	//椭圆弧
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
	else if(type==4)	//指定圆弧半径
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
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"轮廓边基本信息"));
	AddPropItem("id",PROPLIST_ITEM(id++,"ID"));
	AddPropItem("cVertexType",PROPLIST_ITEM(id++,"顶点类型","顶点所在面号","0.基准面顶点|1.第2火曲面点|2.1-2面火曲点|3.第3火曲面点|4.1-3面火曲点"));
	AddPropItem("type",PROPLIST_ITEM(id++,"线型","轮廓边类型","1.普通直边|2.圆弧|3.椭圆弧|"));
	AddPropItem("sector_angle",PROPLIST_ITEM(id++,"圆弧扇形角"));
	AddPropItem("radius",PROPLIST_ITEM(id++,"短半轴长度","椭圆弧短半轴长度（即原始定义圆柱面半径）"));
	AddPropItem("center",PROPLIST_ITEM(id++,"圆心位置","椭圆弧圆心位置（固接坐标系下相对坐标）"));
	AddPropItem("center.x",PROPLIST_ITEM(id++,"X","椭圆圆心位置X方向分量"));
	AddPropItem("center.y",PROPLIST_ITEM(id++,"X","椭圆圆心位置X方向分量"));
	AddPropItem("column_norm",PROPLIST_ITEM(id++,"柱面轴向","椭圆弧原始定义柱面轴向（固接坐标系下相对方向）"));
	AddPropItem("column_norm.x",PROPLIST_ITEM(id++,"X","椭圆弧原始定义柱面轴向X方向分量"));
	AddPropItem("column_norm.y",PROPLIST_ITEM(id++,"Y","椭圆弧原始定义柱面轴向Y方向分量"));
	AddPropItem("column_norm.z",PROPLIST_ITEM(id++,"Z","椭圆弧原始定义柱面轴向Z方向分量"));
	AddPropItem("work_norm",PROPLIST_ITEM(id++,"圆弧法线方向"));
	AddPropItem("work_norm.x",PROPLIST_ITEM(id++,"X","圆弧法线方向X方向分量"));
	AddPropItem("work_norm.y",PROPLIST_ITEM(id++,"Y","圆弧法线方向Y方向分量"));
	AddPropItem("work_norm.z",PROPLIST_ITEM(id++,"Z","圆弧法线方向Z方向分量"));
	AddPropItem("edgeType",PROPLIST_ITEM(id++,"边工艺类型","","0.普通边|1.焊接边|2.卷边"));
	AddPropItem("roll_edge_offset_dist",PROPLIST_ITEM(id++,"卷边偏移距离"));
	AddPropItem("manu_space",PROPLIST_ITEM(id++,"卷边高度"));
	AddPropItem("vertex",PROPLIST_ITEM(id++,"始端点"));
	AddPropItem("vertex.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("vertex.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("endVertex",PROPLIST_ITEM(id++,"终端点"));
	AddPropItem("endVertex.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("endVertex.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("local_point_y",PROPLIST_ITEM(id++,"对活点位置"));
	AddPropItem("local_point_vec",PROPLIST_ITEM(id++,"对活点类型",
		"0.表示无对活点，1.表示对活点两侧标注50-->100的方向与焊接父杆件的start-->end方向相同，2.表示相反",
		"0.无|1.与杆件方向相同|2.与杆件方向相反"));
	AddPropItem("vertexInMcs", PROPLIST_ITEM(id++, "顶点坐标","在当前加工坐标系中的坐标值"));
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
			sprintf(sText,"1.第2火曲面点");
		else if(vertex.feature==12)
			sprintf(sText,"2.1-2面火曲点");
		else if(vertex.feature==3)
			sprintf(sText,"3.第3火曲面点");
		else if(vertex.feature ==13)
			sprintf(sText,"4.1-3面火曲点");
		else //if(vertex.feature==1)
			sprintf(sText,"0.基准面顶点");
	}
	else if(GetPropID("type")==itemId)
	{
		if(type==1)
			sText.Copy("1.普通直边");
		else if(type==2)
			sText.Copy("2.圆弧");
		else if(type==3)
			sText.Copy("3.椭圆弧");
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
		if(manu_space>0)	//卷边高度或焊缝加工间隙
		{
			if(m_bRollEdge)
				sText.Copy("2.卷边");
			else if(m_bWeldEdge)
				sText.Copy("1.焊接边");
		}
		else 
			sText.Copy("0.普通边");
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
			sText.Copy("1.与杆件方向相同");
		else if(local_point_vec==2)
			sText.Copy("2.与杆件方向相反");
		else //if(local_point_vec==0)
			sText.Copy("0.无");
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
	cFaceNo=1;		//工艺构件为板时，默认为单面板
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
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"螺栓孔基本信息"));
	AddPropItem("id",PROPLIST_ITEM(id++,"ID"));
	AddPropItem("bolt_d",PROPLIST_ITEM(id++,"直径","","12|16|20|24|30|"));
	AddPropItem("hole_d_increment",PROPLIST_ITEM(id++,"孔径增量"));
	AddPropItem("cFuncType",PROPLIST_ITEM(id++,"功用类型","","0.连接孔|1.脚钉孔|2.挂线孔|3.接地孔|4.装配孔|5.挂牌孔|6.引流孔"));
	AddPropItem("waistLen",PROPLIST_ITEM(id++,"腰圆长度"));	
	AddPropItem("waistVec",PROPLIST_ITEM(id++,"腰圆方向"));
	AddPropItem("waistVec.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("waistVec.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("pos",PROPLIST_ITEM(id++,"孔位"));
	AddPropItem("posX",PROPLIST_ITEM(id++,"X"));
	AddPropItem("posY",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("dwRayNo",PROPLIST_ITEM(id++,"射线号"));
	AddPropItem("m_dwFlag",PROPLIST_ITEM(id++,"类型"));
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
			sText.Copy("0.连接孔");
		else if(cFuncType==BOLT_INFO::FUNC_FOOTNAIL)
			sText.Copy("1.脚钉孔");
		else if(cFuncType==BOLT_INFO::FUNC_WIREHOLE)
			sText.Copy("2.挂线孔");
		else if(cFuncType==BOLT_INFO::FUNC_EARTHHOLE)
			sText.Copy("3.接地孔");
		else if(cFuncType==BOLT_INFO::FUNC_SETUPHOLE)
			sText.Copy("4.装配孔");
		else if(cFuncType==BOLT_INFO::FUNC_BRANDHOLE)
			sText.Copy("5.挂牌孔");
		else if(cFuncType==BOLT_INFO::FUNC_WATERHOLE)
			sText.Copy("6.引流孔");
		else if(cFuncType==BOLT_INFO::FUNC_FOOTNAIL2)
			sText.Copy("7.代孔脚钉孔");
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
	buffer.ReadFloat(&posX);				//螺栓X方向位置
	buffer.ReadFloat(&posY);				//螺栓Y方向位置
	buffer.ReadWord(&_bolt_d);				//螺栓直径
	buffer.ReadWord(&waistLen);				//腰圆孔腰长
	buffer.ReadDword(&dwRayNo);				//螺栓所在射线号
	buffer.ReadPoint(waistVec);				//腰圆孔方向
	buffer.ReadByte(&cFuncType);			//螺栓孔功能类型
	if( (buf_type==CProcessPart::PPI_BUFFER && version>=1000001)||
		(buf_type==CProcessPart::TID_BUFFER && version>=1000001)||
		(buf_type==CProcessPart::TAP_BUFFER && (version==0||version>=1000500)))
		buffer.ReadFloat(&hole_d_increment);
}
void BOLT_INFO::ToBuffer(CBuffer &buffer,long version,int buf_type)
{
	buffer.WriteFloat(posX);				//螺栓X方向位置
	buffer.WriteFloat(posY);				//螺栓Y方向位置
	buffer.WriteWord(_bolt_d);				//螺栓直径
	buffer.WriteWord(waistLen);				//腰圆孔腰长
	buffer.WriteDword(dwRayNo);				//螺栓所在射线号
	buffer.WritePoint(waistVec);			//腰圆孔方向
	buffer.WriteByte(cFuncType);			//螺栓孔功能类型
	if( (buf_type==CProcessPart::PPI_BUFFER && version>=1000001)||
		(buf_type==CProcessPart::TID_BUFFER && version>=1000001)||
		(buf_type==CProcessPart::TAP_BUFFER && (version==0||version>=1000400)))
		buffer.WriteFloat(hole_d_increment);	//孔径增量
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
	{	//TAP模板
		buffer.ReadBoolean(&m_bIsRod);
		if(m_bIsRod)
		{
			buffer.ReadPoint(startPos);
			buffer.ReadPoint(endPos);
			//if(doc_type==0&&(version==NULL||version>=1000500))
			{	//以下信息确认是多余属性，现为了保持版本兼容由成员变量改为局部变量
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
	pNextPart=NULL;		//处理非正常情况下同一编号构件并不相同且同时输出情况 wjh-2016.1.26
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
		_ciModifiedFlag&=0xfe;	//去掉0x01 数据更改标识位
	return bTrue;
}
void CGeneralPart::ClearUndoModifiedFlag()
{
	_ciModifiedFlag&=0xfd;		//去掉0x02 Undo操作标识位
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
	{	//避免重复添加同一个构件
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
	{	//如果角钢上布了脚钉，角钢备注中需要添加带脚钉字样
		CXhChar100 notes;
		notes.Copy(pPart->sNotes);
		if(pPart->IsHasFootNail())
			notes.Append(" 带脚钉");
		m_sNote.Copy(notes);
	}
	else if(pPart&&pPart->GetClassTypeId()==CLS_POLYPART)
	{
		CXhChar100 notes;
		notes.Copy("备注：");
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
//判断中性文件类型
/*
BYTE CProcessPart::ReadPartTypeFromBuffer(CBuffer &buffer)	
{
	BYTE partType=-1;
	//解析Buffer获取文件类型
	WORD blocki=0;
	buffer.ReadWord(&blocki);	//构件从属的块索引号
	buffer.ReadByte(&partType);	//读取构件类型
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
{	//克隆PPI构件信息
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
	acsPosList.SetMinmalId(9);	//前1~8为特殊占位
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
	else if ('h' == cMat)	//用小写h表示Q355,输出简化材质字符时需要转大写 wht 19-11-05
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
		cMat = 'h';	//用小写h表示Q355,输出简化材质字符时需要转大写 wht 19-11-05
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
//解析规格
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
	//else if(spec,"φ")
	//sscanf(spec,"%c%d%c%d",sMat,)
	if (matStr)
		strcpy(matStr, sMat);
}

#ifdef __PROPERTYLIST_UI_
PROPLIST_TYPE CProcessPart::propSync;
void CProcessPart::InitSyncProp()
{
	propSync.InitHashtableSize(32);
	propSync.AddPropItem("material",PROPLIST_ITEM(0x0001,"材质"));
	propSync.AddPropItem("spec",PROPLIST_ITEM(0x0002,"规格"));
	propSync.AddPropItem("length",PROPLIST_ITEM(0x0004,"长度"));
	propSync.AddPropItem("count",PROPLIST_ITEM(0x0008,"单基数"));
	propSync.AddPropItem("conn_bolt",PROPLIST_ITEM(0x0010,"连接螺栓"));
	propSync.AddPropItem("only_hole",PROPLIST_ITEM(0x0020,"虚拟孔"));
	propSync.AddPropItem("mkpos",PROPLIST_ITEM(0x0040,"钢印号"));
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
	{	//包括质量等级 wht 19-09-15
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
{	//acs定义时以X肢为基准，X肢法线方向为Y轴负方向
	return f3dPoint(0,-1,0);
}
f3dPoint CProcessAngle::GetWingNormY()
{
	f3dPoint norm_y_wing(-1,0,0);
	double fDecWingAngle=GetDecWingAngle();
	if(fabs(fDecWingAngle-90)>0)
	{	//acs定义时以X肢为基准，如果存在开合角钢此处需调整Y肢法线方向
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
			sNotes.Append("切角",' ');
			break;
		}
	}
	if(cut_wing[0]>0||cut_wing[1]>0)
		sNotes.Append("切肢",' ');
	KAI_HE_JIAO *pKaiHeJiao=kaiHeJiaoList.GetFirst();
	float wing_angle=0;
	if (pKaiHeJiao)
		wing_angle=pKaiHeJiao->decWingAngle;
	//根据97制图规定进行的修改 WJH-2004/09/28
	int kaihe_wingx0_y1=kaihe_base_wing_x0_y1;
	if((kaihe_wingx0_y1==0||kaihe_wingx0_y1==1))
	{
		if(wing_angle>92)
			sNotes.Append("开角",' ');
		else if(wing_angle>0&&wing_angle<88)
			sNotes.Append("合角",' ');
		if(bNumericKaiHeInfo)
			sNotes.Append(CXhChar16("%g°",ftoi(wing_angle*10)*0.1),0);
	}
	if(m_bCutRoot)	//刨根
		sNotes.Append("刨根",' ');
	else if(m_bCutBer)	//铲背处理
		sNotes.Append("铲背",' ');
	if(wing_push_X1_Y2!=0)
		sNotes.Append("压扁",' ');
	if(m_bWeld)
		sNotes.Append("焊接",' ');
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
	if(orgBuffer.GetLength()==0)	//初始化原始工艺信息Buffer
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
{	//比较基本信息
	if(pPart==NULL||pPart->m_cPartType!=m_cPartType)
	{
		if(error_info)
			strcpy(error_info,"类型不同");
		return false;
	}
	CProcessPart *pProcessPart=(CProcessPart*)pPart;
	if(cMaterial!=pProcessPart->cMaterial)
	{
		if(error_info)
			strcpy(error_info,"材质不同");
		return false;
	}
	if (cQuality != pProcessPart->cQuality)
	{
		if (error_info)
			strcpy(error_info, "质量等级不同");
		return false;
	}
	if( fabs(m_fWidth-pProcessPart->m_fWidth)>EPS||
		fabs(m_fThick-pProcessPart->m_fThick)>EPS||
		fabs(m_fHigh-pProcessPart->m_fHigh)>EPS)
	{
		if(error_info)
			strcpy(error_info,"规格不同");
		return false;
	}
	if(m_wLength!=pProcessPart->m_wLength)
	{
		if(error_info)
			strcpy(error_info,"长度不同");
		return false;
	}
	//比较螺栓信息
	if(m_xBoltInfoList.GetNodeNum()!=pProcessPart->m_xBoltInfoList.GetNodeNum())
	{
		if(error_info)
			strcpy(error_info,"螺栓数量不同");
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
			strcpy(error_info,"螺栓孔位不同");
		return false;
	}
#ifdef __TAP_CONTEXT_
	if(dwChkStyleFlag==NULL||(dwChkStyleFlag&CHECK_STYLE_ACS_POS))
	{
	//比较装配信息
	//CSolidBody solid;				//实体数据
	if(acsPosList.GetNodeNum()!=pProcessPart->acsPosList.GetNodeNum())
	{
		if(error_info)
			strcpy(error_info,"装配记录数量不同");
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
			strcpy(error_info,"装配记录位置不同");
		return false;
	}
	}
#endif
	return true;
}
BYTE CProcessPart::RetrievedPartTypeAndLabelFromBuffer(CBuffer &buffer,char *sPartLabel/*=NULL*/,UINT maxStrBuffLen/*=17*/)
{
	DWORD begin_pos=buffer.GetCursorPosition();
	//读取四个字节，判断是否为版本号
	BUFFER_VERSION v;
	buffer.ReadInteger(&v.version);
	//version==1000001已使用,此处需考虑兼容
	//以后版本第三个字节设置为0xFF,表示有版本号
	if(v.version!=1000001&&v.bytes[3]!=0xFF)
		buffer.SeekPosition(begin_pos);	//没有版本号跳转至开始位置
	else if(v.bytes[3]==0xFF)
		v.bytes[3]=0x0;	
	if(v.version>=1000005)
		buffer.Offset(4);	//m_verSerial.version
	BYTE cPartType=*((BYTE*)buffer.GetCursorBuffer());
	buffer.SeekPosition(buffer.GetCursorPosition()+24);	//跳转至件号位置(CProcessPart::ToPPIBuffer)
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
			if(idBuff==1)		//基础附加缓存
			{
				buffer.Offset(8);
				//buffer.ReadDword(&m_dwInheritPropFlag);
				//buffer.ReadInteger(&m_idSrcObject);
				buffer.ReadString(sPartLabel,maxStrBuffLen);
				break;
			}
			else	//跳过不识别的缓存区
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
	//读取四个字节，判断是否为版本号
	buffer.ReadInteger(&v.version);		//版本号
	if(v.version<1000001&&v.bytes[3]!=0xFF)
	{	//兼容无版本号的情况
		v.version=0;
		buffer.PopPositionStack();
	}
	else if(v.bytes[3]==0xFF)
		v.bytes[3]=0x0;	
	if(pVersion)
		*pVersion=v.version;
	if(v.version>=1000005)
		buffer.ReadInteger(&m_verSerial.version);
	//清空版本号标识字节,还原为真实的版本
	//1、基本信息
	buffer.ReadByte(&_cPartType);		//构件类型
	buffer.ReadByte(&_cMaterial);		//材质
	buffer.ReadInteger(&m_Seg.iSeg);	//归属分段号
	buffer.ReadFloat(&_fWidth);			//宽度
	buffer.ReadFloat(&_fThick);			//厚度
	buffer.ReadFloat(&_fHigh);			//高度
	buffer.ReadWord(&_wLength);			//长度(mm)
	buffer.ReadFloat(&_fWeight);		//单重(kg)
}
void CProcessPart::ReadBasicPPIAttachBuffer(CBuffer &buffer,long version/*=0*/)
{	//2.构件附加信息
	if(version>=1000005)
	{
		buffer.ReadDword(&m_dwInheritPropFlag);
		buffer.ReadInteger(&m_idSrcObject);
	}
	buffer.ReadString(m_sPartNo);		//构件编号
	buffer.ReadString(m_sSpec);			//构件规格
	buffer.ReadString(m_sNote);			//备注
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
			if(idBuff==1)		//基础附加缓存
				ReadBasicPPIAttachBuffer(CBuffer(buffer.GetCursorBuffer(),nBuffSize),version);
			else if(idBuff==2)	//加工工艺信息附加缓存
				ReadProcessBuffer(CBuffer(buffer.GetCursorBuffer(),nBuffSize),version,true,PPI_BUFFER);
			else if(idBuff==3&&m_cPartType==CProcessPart::TYPE_PLATE)
				((CProcessPlate*)this)->CutPointFromBuffer(CBuffer(buffer.GetCursorBuffer(),nBuffSize),version);
			//跳过缓存区
			buffer.Offset(nBuffSize);
		}
	}
	else
	{
		//2.构件附加信息
		buffer.ReadString(m_sPartNo);		//构件编号
		buffer.ReadString(m_sSpec);			//构件规格
		buffer.ReadString(m_sNote);			//备注
		if(version>=1000004)
			buffer.ReadString(m_sRelatePartNo);
		//3.工艺信息
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
	BUFFER_VERSION v(1000006);	//版本号最大值为:16777215(0x00FFFFFF)
	v.bytes[3]=0xFF;			//前三个字节用来表示版本号，第四个字节用来判断是否为版本号
	buffer.WriteInteger(v.version);		//版本号
	v.bytes[3]=0x0;				//清空版本号标识字节,还原为真实的版本
	buffer.WriteInteger(m_verSerial.version);	//版本流水号(控制不同时间的存储版本,与v.version不同无关乎格式)
	//写入基本信息
	buffer.WriteByte(m_cPartType);
	buffer.WriteByte(cMaterial);		//写入材质简化字符
	buffer.WriteInteger(m_Seg.iSeg);	//写入归属分段号
	buffer.WriteFloat(m_fWidth);		//写入宽度
	buffer.WriteFloat(m_fThick);		//写入厚度
	buffer.WriteFloat(m_fHigh);			//写入高度
	buffer.WriteWord(m_wLength);		//写入长度(mm)
	buffer.WriteFloat(m_fWeight);		//写入单重(kg)
	if(v.version>=1000005)
	{
		WORD wAttachBuff=2;
		if(m_cPartType==CProcessPart::TYPE_PLATE&&v.version>=1000005)
			wAttachBuff=3;
		buffer.WriteWord(wAttachBuff);
		//1.写入构件附加信息
		buffer.WriteInteger(1);	//缓存区类型标识, 1:基础附加缓存区
		DWORD stackPos=buffer.CursorPosition;
		buffer.WriteInteger(0);	//预留基础附加缓存区长度占位字节
		//构件工艺信息的版本流水号>=V1000005
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
		//2.写入工艺信息
		buffer.WriteInteger(2);	//缓存区类型标识, :工艺附加缓存区
		CBuffer processBuf;
		WriteProcessBuffer(processBuf,v.version,true,PPI_BUFFER);
		buffer.WriteInteger(processBuf.Length);
		buffer.Write(processBuf.GetBufferPtr(),processBuf.GetLength());
		if(m_cPartType==CProcessPart::TYPE_PLATE&&v.version>=1000005)
		{	//3.钢板切割点信息
			buffer.WriteInteger(3);	//缓存区类型标识, 3:钢板切割点信息附加缓存区
			stackPos=buffer.CursorPosition;
			buffer.WriteInteger(0);	//预留基础附加缓存区长度占位字节
			((CProcessPlate*)this)->CutPointToBuffer(buffer,v.version);
			currPos=buffer.CursorPosition;
			buffer.SeekPosition(stackPos);
			buffer.WriteInteger(currPos-stackPos-4);
			buffer.SeekPosition(currPos);
		}
	}
	else
	{	//写入构件附加信息
		buffer.WriteString(m_sPartNo);
		buffer.WriteString(m_sSpec);
		buffer.WriteString(m_sNote);
		if(v.version>=1000004)
			buffer.WriteString(m_sRelatePartNo);
		//写入工艺信息
		CBuffer processBuf;
		WriteProcessBuffer(processBuf,v.version,true,PPI_BUFFER);
		buffer.WriteInteger(processBuf.GetLength());
		buffer.Write(processBuf.GetBufferPtr(),processBuf.GetLength());
		//钢板切割点信息
		if(m_cPartType==CProcessPart::TYPE_PLATE&&v.version>=1000005)
			((CProcessPlate*)this)->CutPointToBuffer(buffer,v.version);
	}
}
#ifdef __TAP_CONTEXT_
bool CProcessPart::IsReplacePart()	//是否为代料构件
{
	if(m_cOrgMaterial!=0&&m_cOrgMaterial!=_cMaterial)
		return true;
	return false;
}
void CProcessPart::FromTIDBuffer(CBuffer &buffer,long version/* =0 */)
{
	//1、基本信息
	WORD blockID=0;
	buffer.ReadWord(&blockID);			//构件归属的模型空间
	buffer.ReadByte(&_cPartType);
	WORD state;							//构件状态属性
	buffer.ReadWord(&state);
	buffer.ReadByte(&_cMaterial);		//材质
	buffer.ReadInteger(&m_Seg.iSeg);	//归属分段号
	buffer.ReadFloat(&_fWidth);			//宽度
	buffer.ReadFloat(&_fThick);			//厚度
	buffer.ReadFloat(&_fHigh);			//高度
	buffer.ReadWord(&_wLength);			//长度(mm)
	buffer.ReadFloat(&_fWeight);		//单重(kg)
	buffer.ReadByte(&m_cFuncType);
	//2、实体信息
	DWORD dwSolidSize=0;
	buffer.ReadDword(&dwSolidSize);
	if(dwSolidSize)
	{
		solid.CopyBuffer(buffer.GetCursorBuffer(),dwSolidSize);	//实体
		buffer.Offset(dwSolidSize);
	}
	//3、附件信息
	buffer.ReadString(m_sPartNo);		//构件编号
	buffer.ReadString(m_sSpec);			//构件规格
	buffer.ReadString(m_sNote);			//备注
	//4、工艺信息
	DWORD processInfoSize=0;
	buffer.ReadDword(&processInfoSize);
	if(processInfoSize>0)
		ReadProcessBuffer(buffer,version,false,TID_BUFFER);
}
void CProcessPart::FromTAPBuffer(CBuffer &buffer,long version/* =0 */)
{
	//1.构件基本信息
	buffer.ReadByte(&_cPartType);
	buffer.ReadByte(&m_cFuncType);
	buffer.ReadByte(&_cMaterial);		//材质
	buffer.ReadInteger(&m_Seg.iSeg);	//归属分段号
	buffer.ReadFloat(&_fWidth);			//宽度
	buffer.ReadFloat(&_fThick);			//厚度
	buffer.ReadFloat(&_fHigh);			//高度
	buffer.ReadWord(&_wLength);			//长度(mm)
	buffer.ReadFloat(&_fWeight);		//单重(kg)
	buffer.ReadString(m_sPartNo);		//构件编号
	buffer.ReadString(m_sSpec);			//构件规格
	buffer.ReadString(m_sNote);			//备注
	//2.实体信息
	DWORD dwSolidSize=0;
	buffer.ReadDword(&dwSolidSize);
	if(dwSolidSize)
	{
		solid.CopyBuffer(buffer.GetCursorBuffer(),dwSolidSize);	//实体
		buffer.Offset(dwSolidSize);
	}
	//3.装配信息
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
	//4.工艺信息
	ReadProcessBuffer(buffer,version,true,TAP_BUFFER);
	//5、螺栓孔类型
	n=buffer.ReadInteger();
	if(n!=m_xBoltInfoList.GetNodeNum())
		buffer.SeekPosition(buffer.GetCursorPosition()+n);
	else
	{
		for(BOLT_INFO *pBolt=m_xBoltInfoList.GetFirst();pBolt;pBolt=m_xBoltInfoList.GetNext())
			buffer.ReadByte(&pBolt->cFuncType);
	}
	//6.读取构件原始工艺信息缓存
	n=buffer.ReadInteger();
	CBuffer orgProcessBuf;
	if(n>0)
		buffer.Read(orgProcessBuf.GetBufferPtr(),n);
	InitOrgProcessBuffer(orgProcessBuf);
}
void CProcessPart::ToTAPBuffer(CBuffer &buffer,bool bIncSolidData/*=true*/)
{
	long version=0;
	//1、基本信息
	buffer.WriteByte(m_cPartType);
	buffer.WriteByte(m_cFuncType);
	buffer.WriteByte(cMaterial);			//写入材质简化字符
	buffer.WriteInteger(m_Seg.iSeg);		//写入归属分段号
	buffer.WriteFloat(m_fWidth);			//写入宽度
	buffer.WriteFloat(m_fThick);			//写入厚度
	buffer.WriteFloat(m_fHigh);				//写入高度
	buffer.WriteWord(m_wLength);			//写入长度(mm)
	buffer.WriteFloat(m_fWeight);			//写入单重(kg)
	buffer.WriteString(m_sPartNo);
	buffer.WriteString(m_sSpec);
	buffer.WriteString(m_sNote);
	//2、实体信息
	DWORD dwSolidSize=bIncSolidData?solid.BufferLength():0;
	buffer.WriteDword(dwSolidSize);
	if(dwSolidSize>0)
		buffer.Write(solid.BufferPtr(),solid.BufferLength());
	//3、装配信息
	buffer.WriteInteger(acsPosList.GetNodeNum());
	for(ASSEMBLEY_POS *pPos=acsPosList.GetFirst();pPos;pPos=acsPosList.GetNext())
	{
		buffer.WriteDword(pPos->GetKey());
		pPos->ToBuffer(buffer);
	}
	buffer.WriteByte(m_cPlaceMode);
	buffer.WriteByte(m_cPlaceStateFromFile);
	buffer.WriteByte(m_cPriorPlaceState);
	//4、工艺信息
	WriteProcessBuffer(buffer,version,true,TAP_BUFFER);
	//5、螺栓孔类型
	buffer.WriteInteger(m_xBoltInfoList.GetNodeNum());
	for(BOLT_INFO *pBolt=m_xBoltInfoList.GetFirst();pBolt;pBolt=m_xBoltInfoList.GetNext())
		buffer.WriteByte(pBolt->cFuncType);
	//6.保存原始工艺信息缓存
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
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"切割点属性"));
	AddPropItem("hEntId",PROPLIST_ITEM(id++,"关联对象Id"));
	AddPropItem("InLine",PROPLIST_ITEM(id++,"引入线"));
	AddPropItem("cInLineLen",PROPLIST_ITEM(id++,"长度"));
	AddPropItem("fInAngle",PROPLIST_ITEM(id++,"修订角"));
	AddPropItem("OutLine",PROPLIST_ITEM(id++,"引出线"));
	AddPropItem("cOutLineLen",PROPLIST_ITEM(id++,"长度"));
	AddPropItem("fOutAngle",PROPLIST_ITEM(id++,"修订角"));
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
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"钢板基本信息"));
	AddPropItem("m_sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	AddPropItem("m_Seg",PROPLIST_ITEM(id++,"分段号"));
	AddPropItem("m_cMaterial",PROPLIST_ITEM(id++,"材质"));
	AddPropItem("m_fThick",PROPLIST_ITEM(id++,"厚度"));
	AddPropItem("m_fOrgThick",PROPLIST_ITEM(id++,"原厚度"));
	AddPropItem("m_fWeight",PROPLIST_ITEM(id++,"重量"));
	AddPropItem("m_cFuncType",PROPLIST_ITEM(id++,"功能类型","钢板功能","普通钢板|加劲肋板|脚钉钢板|挂线板|爬梯踏板|安装辅助挂板|塔脚板"));
	AddPropItem("WeldingRelation",PROPLIST_ITEM(id++,"焊接关系","与当前构件焊接在一起的构件"));
	AddPropItem("m_sNote",PROPLIST_ITEM(id++,"备注"));
	AddPropItem("m_sRelPartNo",PROPLIST_ITEM(id++,"关联件号"));
	AddPropItem("m_bDeformed",PROPLIST_ITEM(id++,"火曲变形","","0.未考虑|1.已考虑"));
	AddPropItem("m_ciDeformType",PROPLIST_ITEM(id++,"火曲变形方式","","0.装配面|1.中性层|2.凸面|3.凹面"));
	AddPropItem("m_ciRollProcessType",PROPLIST_ITEM(id++,"卷边处理方式","","0.最佳受力|1.最少刀数1|2.最少刀数2"));
	AddPropItem("m_ciRollOffsetType",PROPLIST_ITEM(id++,"卷边偏移方式","","0.外侧|1.内侧"));
	AddPropItem("m_dwInheritPropFlag",PROPLIST_ITEM(id++,"同步模式","","...."));
	AddPropItem("profile",PROPLIST_ITEM(id++,"外形"));
	AddPropItem("m_cFaceN",PROPLIST_ITEM(id++,"钢板类型","","1.单面板|2.双面板|3.三面板|"));
	AddPropItem("HuoquFace[0]",PROPLIST_ITEM(id++,"第一火曲面"));
	AddPropItem("HuoQuFaceNorm[0]",PROPLIST_ITEM(id++,"火曲面法线"));
	AddPropItem("HuoQuFaceNorm[0].x",PROPLIST_ITEM(id++,"火曲面法线X方向分量"));
	AddPropItem("HuoQuFaceNorm[0].y",PROPLIST_ITEM(id++,"火曲面法线Y方向分量"));
	AddPropItem("HuoQuFaceNorm[0].z",PROPLIST_ITEM(id++,"火曲面法线Z方向分量"));
	AddPropItem("HuoQuLine[0].startPt",PROPLIST_ITEM(id++,"火曲线始端点"));
	AddPropItem("HuoQuLine[0].startPt.x",PROPLIST_ITEM(id++,"火曲线始端点X方向分量"));
	AddPropItem("HuoQuLine[0].startPt.y",PROPLIST_ITEM(id++,"火曲线始端点Y方向分量"));
	AddPropItem("HuoQuLine[0].startPt.z",PROPLIST_ITEM(id++,"火曲线始端点Z方向分量"));
	AddPropItem("HuoQuLine[0].endPt",PROPLIST_ITEM(id++,"火曲线终端点"));
	AddPropItem("HuoQuLine[0].endPt.x",PROPLIST_ITEM(id++,"火曲线终端点X方向分量"));
	AddPropItem("HuoQuLine[0].endPt.y",PROPLIST_ITEM(id++,"火曲线终端点Y方向分量"));
	AddPropItem("HuoQuLine[0].endPt.z",PROPLIST_ITEM(id++,"火曲线终端点Z方向分量"));
	AddPropItem("HuoquFace[1]",PROPLIST_ITEM(id++,"第二火曲面"));
	AddPropItem("HuoQuFaceNorm[1]",PROPLIST_ITEM(id++,"火曲面法线"));
	AddPropItem("HuoQuFaceNorm[1].x",PROPLIST_ITEM(id++,"火曲面法线X方向分量"));
	AddPropItem("HuoQuFaceNorm[1].y",PROPLIST_ITEM(id++,"火曲面法线Y方向分量"));
	AddPropItem("HuoQuFaceNorm[1].z",PROPLIST_ITEM(id++,"火曲面法线Z方向分量"));
	AddPropItem("HuoQuLine[1].startPt",PROPLIST_ITEM(id++,"火曲线始端点"));
	AddPropItem("HuoQuLine[1].startPt.x",PROPLIST_ITEM(id++,"火曲线始端点X方向分量"));
	AddPropItem("HuoQuLine[1].startPt.y",PROPLIST_ITEM(id++,"火曲线始端点Y方向分量"));
	AddPropItem("HuoQuLine[1].startPt.z",PROPLIST_ITEM(id++,"火曲线始端点Z方向分量"));
	AddPropItem("HuoQuLine[1].endPt",PROPLIST_ITEM(id++,"火曲线终端点"));
	AddPropItem("HuoQuLine[1].endPt.x",PROPLIST_ITEM(id++,"火曲线终端点X方向分量"));
	AddPropItem("HuoQuLine[1].endPt.y",PROPLIST_ITEM(id++,"火曲线终端点Y方向分量"));
	AddPropItem("HuoQuLine[1].endPt.z",PROPLIST_ITEM(id++,"火曲线终端点Z方向分量"));
	AddPropItem("bolt_info",PROPLIST_ITEM(id++,"螺栓信息"));
	AddPropItem("M24",PROPLIST_ITEM(id++,"M24"));
	AddPropItem("M22",PROPLIST_ITEM(id++,"M22"));
	AddPropItem("M20",PROPLIST_ITEM(id++,"M20"));
	AddPropItem("M18",PROPLIST_ITEM(id++,"M18"));
	AddPropItem("M16",PROPLIST_ITEM(id++,"M16"));
	AddPropItem("Place",PROPLIST_ITEM(id++,"占位"));
	AddPropItem("m_cPlaceMode",PROPLIST_ITEM(id++,"占位模式","占位模式","0.自由占位|1.4占位模式1|2.4占位模式2|3.8占位模式1|4.8占位模式2"));
	AddPropItem("POS_FRONT_PLUS",PROPLIST_ITEM(id++,"前","","添加|删除"));
	AddPropItem("POS_BACK_PLUS",PROPLIST_ITEM(id++,"后","","添加|删除"));
	AddPropItem("POS_LEFT_PLUS",PROPLIST_ITEM(id++,"左","","添加|删除"));
	AddPropItem("POS_RIGHT_PLUS",PROPLIST_ITEM(id++,"右","","添加|删除"));
	AddPropItem("POS_FRONT_MINUS",PROPLIST_ITEM(id++,"前X-","","添加|删除"));
	AddPropItem("POS_BACK_MINUS",PROPLIST_ITEM(id++,"后X-","","添加|删除"));
	AddPropItem("POS_LEFT_MINUS",PROPLIST_ITEM(id++,"左Y-","","添加|删除"));
	AddPropItem("POS_RIGHT_MINUS",PROPLIST_ITEM(id++,"右Y-","","添加|删除"));
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
			sText.Copy("加劲肋板");
		else if(CProcessPlate::FUNC_FOOTNAIL==m_cFuncType)
			sText.Copy("脚钉钢板");
		else if(CProcessPlate::FUNC_WIREPLATE==m_cFuncType)
			sText.Copy("挂线板");
		else if(CProcessPlate::FUNC_STEPPLATE==m_cFuncType)
			sText.Copy("爬梯踏板");
		else if(CProcessPlate::FUNC_SETUPPLATE==m_cFuncType)
			sText.Copy("安装辅助挂板");
		else if(CProcessPlate::FUNC_FOOTPLATE==m_cFuncType)
			sText.Copy("塔脚板");
		else //if(CProcessPlate::FUNC_COMMON==m_cFuncType)
			sText.Copy("普通钢板");
	}
	else if(GetPropID("m_sNote")==id)
		sText.Copy(GetNotes());
	else if(GetPropID("m_bDeformed")==id)
	{
		if(m_bIncDeformed)
			sText.Copy("已考虑");
		else
			sText.Copy("未考虑");
	}
	else if(GetPropID("m_sRelPartNo")==id)
		sText.Copy(m_sRelatePartNo);
	else if(GetPropID("m_ciDeformType")==id)
	{
		if(m_ciDeformType==0)
			sText.Copy("0.装配面");
		else if(m_ciDeformType==1)
			sText.Copy("1.中性层");
		else if(m_ciDeformType==2)
			sText.Copy("2.凸面");
		else if(m_ciDeformType==3)
			sText.Copy("3.凹面");
	}
	else if(GetPropID("m_ciRollProcessType")==id)
	{
		if(m_ciRollProcessType==0)
			sText.Copy("0.最佳受力");
		else if(m_ciRollProcessType==1)
			sText.Copy("1.最少刀数1");
		else if(m_ciRollProcessType==2)
			sText.Copy("2.最少刀数2");
	}
	else if(GetPropID("m_ciRollOffsetType")==id)
	{
		if(m_ciRollOffsetType==0)
			sText.Copy("0.外侧");
		else if(m_ciRollOffsetType==1)
			sText.Copy("1.内侧");
	}
	else if(GetPropID("m_cFaceN")==id)
	{
		if(m_cFaceN==1)
			sText.Copy("1.单面板");
		else if(m_cFaceN==2)
			sText.Copy("2.双面板");
		else if(m_cFaceN==3)
			sText.Copy("3.三面板");
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
		CXhChar100 cmbItemArr[5]={"0.自由占位","1.4占位模式1","2.4占位模式2","3.8占位模式1","4.8占位模式2"};
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
			sText.Copy("√");

	}
	else if(GetPropID("POS_BACK_PLUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_BACK_PLUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_BACK_PLUS));
		else 
			sText.Copy("√");
	}
	else if(GetPropID("POS_LEFT_PLUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_LEFT_PLUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_LEFT_PLUS));
		else 
			sText.Copy("√");
	}
	else if(GetPropID("POS_RIGHT_PLUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_RIGHT_PLUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_RIGHT_PLUS));
		else 
			sText.Copy("√");
	}
	else if(GetPropID("POS_FRONT_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_FRONT_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_FRONT_MINUS));
		else 
			sText.Copy("√");
	}
	else if(GetPropID("POS_BACK_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_BACK_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_BACK_MINUS));
		else 
			sText.Copy("√");
	}
	else if(GetPropID("POS_LEFT_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_LEFT_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_LEFT_MINUS));
		else 
			sText.Copy("√");
	}
	else if(GetPropID("POS_RIGHT_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_RIGHT_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_RIGHT_MINUS));
		else 
			sText.Copy("√");
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
	mcsFlg.ciBottomEdge=255;	//相当于0xff，及以前的-1
	mcsFlg.ciOverturn=0;
	m_bIncDeformed=false;
	m_ciDeformType=0;
	m_ciRollProcessType=0;
	m_ciRollOffsetType=1;
	//火曲线的火曲半径和中性层系数主要用于输出钢板NC数据时考虑火曲变形量   wxc-2016.10.27
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
	if (result < 0)	// 后点在线右侧，有凹角出现
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
	//初始化加工坐标系
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
		edge_vec/=edge_dist;	//单位化边矢量
		if(i>0&&prev_vec*edge_vec>EPS_COS)	//连续共线边轮廓
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
			//凸弧取弧中点计算轮廓区域
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
		//将圆弧进行分段简化处理,求取钢板的最小轮廓区域
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
		InitMCS();	//初始化加工坐标系
	//根据bottom_edge_i计算加工坐标系
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
	//调整坐标系原点
	SCOPE_STRU scope=GetVertexsScope(&mcs);
	mcs.origin+=scope.fMinX*mcs.axis_x;
	mcs.origin+=scope.fMinY*mcs.axis_y;
	//如果钢板进行反转，更新钢板坐标系
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
	//1.火曲信息（钢板固接坐标系下）
	for(int i=0;i<pPlate->m_cFaceN-1;i++)
	{
		vector_trans(pPlate->HuoQuFaceNorm[i],mcs,FALSE);
		coord_trans(pPlate->HuoQuLine[i].startPt,mcs,FALSE);
		coord_trans(pPlate->HuoQuLine[i].endPt,mcs,FALSE);
	}
	//2.螺栓信息
	for(BOLT_INFO *pBolt=pPlate->m_xBoltInfoList.GetFirst();pBolt;pBolt=pPlate->m_xBoltInfoList.GetNext())
	{
		f3dPoint ls_pos(pBolt->posX,pBolt->posY,0);
		coord_trans(ls_pos,mcs,FALSE);
		pBolt->posX=(float)ls_pos.x;
		pBolt->posY=(float)ls_pos.y;
	}
	//3.板外形
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

bool CProcessPlate::IsReplacePart()	//是否为代料构件
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
	//初始化螺栓孔引入点
#endif
}
bool CProcessPlate::ClonePart(CProcessPart *pNewPart)
{
	bool bRetCode=CProcessPart::ClonePart(pNewPart);
	if(bRetCode)
	{	//拷贝螺栓及顶点id
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
	//1.孔数、边数及面数
	buffer.WriteWord((short)m_xBoltInfoList.GetNodeNum());	//孔数
	buffer.WriteWord((short)vertex_list.GetNodeNum());		//边数
	buffer.WriteByte(m_cFaceN);								//面数
	//2.火曲信息（钢板固接坐标系下）
	for(BYTE i=0;i<m_cFaceN-1;i++)
	{
		buffer.WritePoint(HuoQuFaceNorm[i]);				//(板相对坐标系下的)火曲面法线
		buffer.WritePoint(HuoQuLine[i].startPt);			//(板相对坐标系下的)火曲线起点
		buffer.WritePoint(HuoQuLine[i].endPt);				//(板相对坐标系下的)火曲线终点
		if(buf_type==CProcessPart::PPI_BUFFER && version>=1000003)
		{
			buffer.WriteDouble(m_fHuoQuLineR[i]);
			buffer.WriteDouble(m_fHuoQuLineX[i]);
		}
	}
	if(buf_type==CProcessPart::PPI_BUFFER && version>=1000005)
	{
		buffer.WriteByte(m_ciRollProcessType);	//卷边处理方式
		buffer.WriteByte(m_ciRollOffsetType);	//卷边线计算方式
		buffer.WriteBoolean(m_bIncDeformed);	//是否考虑火曲变形
		buffer.WriteByte(m_ciDeformType);		//火曲变形处理方式
	}
	BOLT_INFO *pBolt=m_xBoltInfoList.GetFirst();
	if(pBolt)
		buffer.WriteFloat(pBolt->hole_d_increment);	//螺栓直径增量
	else 
		buffer.WriteFloat((float)0);
	//3.螺栓信息
	//4B：X方向位置（float）
	//4B：Y方向位置（float）。
	//2B：螺栓孔径
	//2B：腰圆孔腰长
	//8B：螺栓所在的射线号
	//24B：腰圆孔方向
	for(pBolt=m_xBoltInfoList.GetFirst();pBolt;pBolt=m_xBoltInfoList.GetNext())
	{
		if(bSaveSubSerial)
			buffer.WriteInteger(pBolt->keyId);
		pBolt->ToBuffer(buffer,version,buf_type);
	}
	//4.板外形
	if(buf_type==CProcessPart::PPI_BUFFER && version>=1000003&&version<1000005)
		buffer.WriteBoolean(m_bIncDeformed);
	if(m_cFaceN==3)
		buffer.WritePoint(top_point);
	PROFILE_VER *pVertext= NULL;
	for(pVertext=vertex_list.GetFirst();pVertext;pVertext=vertex_list.GetNext())
	{	
		if(bSaveSubSerial)
			buffer.WriteDword(pVertext->keyId);
		//4.1 1B：边类型(1表示直线段；2表示圆弧；3表示椭圆弧）
		buffer.WriteByte(pVertext->type);
		//4.2 1B属性标识字节
		//1~4位表示起点归属面（如0x03表示起点同时归属于第一面和第二面，即1-2面火曲线上）
		//第8位为0时起点位置之后的后续2字节表示卷边高度，为1时表示焊缝边的加工间隙值
		//其余位保留
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
		//4.3 焊缝、卷边
		if(pVertext->m_bRollEdge)
			propId &=0x7F;								//标识卷边
		else if(pVertext->m_bWeldEdge)
			propId |=0x80;								//标识焊缝
		buffer.WriteByte(propId);
		buffer.WriteDouble(pVertext->vertex.x);			//边起点X坐标
		buffer.WriteDouble(pVertext->vertex.y);			//边起点Y坐标
		buffer.WriteWord(pVertext->manu_space);			//焊缝加工预留间隙或卷边高度(mm)
		if(buf_type==CProcessPart::PPI_BUFFER &&version>=1000006)
			buffer.WriteWord(pVertext->roll_edge_offset_dist);	//卷边偏移距离
		if(pVertext->m_bWeldEdge)
		{	
			buffer.WriteByte((BYTE)0);		//1B：焊缝高度
			buffer.WriteWord((WORD)0);		//2B：焊缝起点距板边起点距离
			buffer.WriteWord((WORD)0);		//2B：焊缝长度，为0时表示一直延伸至板边终点
		}
		//4.4 边类型=2，3时，（椭）圆弧扇形角、圆弧面法线方向
		if(pVertext->type>1)
		{	//8B：（椭）圆弧扇形角，弧度制
			buffer.WriteDouble(pVertext->sector_angle);
			//24B：圆弧面法线方向（固接坐标系下相对方向）
			buffer.WritePoint(pVertext->work_norm);  //圆弧面法线
			//4.5  边类型=3时，椭圆弧短半轴长度、椭圆弧圆心位置、椭圆弧原始定义柱面轴向
			if(pVertext->type==3)
			{
				buffer.WriteDouble(pVertext->radius	);		//8B：椭圆弧短半轴长度（即原始定义圆柱面半径）
				buffer.WritePoint(pVertext->center);		//24B：椭圆弧圆心位置（固接坐标系下相对坐标）
				buffer.WritePoint(pVertext->column_norm);	//24B：椭圆弧原始定义柱面轴向（固接坐标系下相对方向）
			}
		}
	}
	//5.加工坐标系基准边索引及打号位置
	buffer.WriteWord((WORD)mcsFlg.wFlag);		//加工坐标系基准边索引
	buffer.WriteFloat((float)mkpos.x);			//打号位置X坐标
	buffer.WriteFloat((float)mkpos.y);			//打号位置Y坐标
	if(buf_type==CProcessPart::PPI_BUFFER && version>=1000002)
	{	//记录矩形钢印号的方向
		buffer.WriteFloat((float)mkVec.x);
		buffer.WriteFloat((float)mkVec.y);	
	}
}
//计算卷边火曲线及卷边线位置
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
	//TODO 此处需要在外部进行，在此处不易理解 wht 19-09-22
	if(mcsFlg.ciOverturn>0)	//钢板翻转后，卷边偏移方向也应翻转 wht 19-09-15
		offset_vec.Set(-cur_vec.y, cur_vec.x);
	else
		offset_vec.Set(offset_vec.y, -offset_vec.x);
	normalize(offset_vec);
	double cosa=fabs(cur_vec*prev_vec);//判断当前边与前一边是否共线
	double cosb=fabs(cur_vec*next_vec);//判断当前边与下一边是否共线
	f2dLine edge_line,extend_line;
	//根据不同的卷边位置处理方式计算火曲线和卷边线的偏移距离
	int nHuoquOffDist=0,nRollOffDist=0;
	if(m_ciRollOffsetType==0)
	{	//外侧方式
		nHuoquOffDist=pRollStart->roll_edge_offset_dist+ftoi(m_fThick);
		nRollOffDist=pRollStart->manu_space;
	}
	else
	{	//内侧方式
		nHuoquOffDist=pRollStart->roll_edge_offset_dist;
		nRollOffDist=pRollStart->manu_space+ftoi(m_fThick);
	}
	//计算偏移后火曲线的位置
	huoqu_line.startPt=pRollStart->vertex+offset_vec*nHuoquOffDist;
	huoqu_line.endPt=pRollEnd->vertex+offset_vec*nHuoquOffDist;
	edge_line.startPt.Set(huoqu_line.startPt.x,huoqu_line.startPt.y);
	edge_line.endPt.Set(huoqu_line.endPt.x,huoqu_line.endPt.y);
	if(cosa<EPS_COS2)
	{	//当前边与前一边不共线	
		extend_line.startPt.Set(pRollStart->vertex.x,pRollStart->vertex.y);
		extend_line.endPt.Set(extend_line.startPt.x+prev_vec.x,extend_line.startPt.y+prev_vec.y);
		Int2dll(extend_line,edge_line,huoqu_line.startPt.x,huoqu_line.startPt.y);
	}
	if(cosb<EPS_COS2)
	{	//当前边与下一边不共线
		extend_line.startPt.Set(pRollEnd->vertex.x,pRollEnd->vertex.y);
		extend_line.endPt.Set(extend_line.startPt.x+next_vec.x,extend_line.startPt.y+next_vec.y);
		Int2dll(extend_line,edge_line,huoqu_line.endPt.x,huoqu_line.endPt.y);
	}
	f3dPoint new_edge_vec=huoqu_line.endPt-huoqu_line.startPt;
	normalize(new_edge_vec);	//卷边后新边方向
	if(new_edge_vec*cur_vec<0)	//得到的新边始末端颠倒，边始端取垂直方向为延伸方向
	{
		huoqu_line.startPt=pRollStart->vertex+offset_vec*nHuoquOffDist;
		prev_vec=offset_vec;
		huoqu_line.endPt=pRollEnd->vertex+offset_vec*nHuoquOffDist;
		next_vec=offset_vec;
	}
	//计算卷边线的位置
	roll_line.startPt=huoqu_line.startPt+offset_vec*abs(nRollOffDist);
	roll_line.endPt=huoqu_line.endPt+offset_vec*abs(nRollOffDist);
	if(m_ciRollProcessType==1||m_ciRollProcessType==2)
	{	//最少刀数
		edge_line.startPt.Set(roll_line.startPt.x,roll_line.startPt.y);
		edge_line.endPt.Set(roll_line.endPt.x,roll_line.endPt.y);
		if(fabs(cur_vec*prev_vec)<EPS_COS2)
		{	//当前边与前一边不共线	
			extend_line.startPt.Set(pRollStart->vertex.x,pRollStart->vertex.y);
			extend_line.endPt.Set(extend_line.startPt.x+prev_vec.x,extend_line.startPt.y+prev_vec.y);
			Int2dll(extend_line,edge_line,roll_line.startPt.x,roll_line.startPt.y);
		}
		if(fabs(cur_vec*next_vec)<EPS_COS2)
		{	//当前边与下一个边不共线 
			extend_line.startPt.Set(pRollEnd->vertex.x,pRollEnd->vertex.y);
			extend_line.endPt.Set(extend_line.startPt.x+next_vec.x,extend_line.startPt.y+next_vec.y);
			Int2dll(extend_line,edge_line,roll_line.endPt.x,roll_line.endPt.y);
		}
		new_edge_vec=roll_line.endPt-roll_line.startPt;
		normalize(new_edge_vec);	//卷边后新边方向
		if(new_edge_vec*cur_vec<0)	//得到的新边始末端颠倒，边始端取垂直方向为延伸方向 wht 09-08-14
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
//处理卷边（相对板相对坐标系而言）得到处理后钢板边对应点
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
		{	//输出卷边
			PROFILE_VER* pRollStart=pVertex,*pRollEnd=NULL,*pNextPnt=NULL;
			int nPush=vertex_list.push_stack();
			pRollEnd=(pTailVertex==pVertex)?vertex_list.GetFirst():vertex_list.GetNext();
			pNextPnt=(pTailVertex==pRollEnd)?vertex_list.GetFirst():vertex_list.GetNext();
			vertex_list.pop_stack(nPush);
			if(pRollStart==NULL||pRollEnd==NULL||pPrevPnt==NULL||pNextPnt==NULL)
				continue;
			f3dLine huoqu_line,roll_edge_line;
			CalRollLineAndHuoquLine(pRollStart,pRollEnd,pPrevPnt,pNextPnt,huoqu_line,roll_edge_line);
			//考虑火曲线偏移量后，制弯点坐标应做相应调整，否则最佳受力时卷边线与火曲线不垂直
			PROFILE_VER *pNewStartVertex=hashDestVertexPtrBySrcVertexPtr.GetValue((DWORD)pRollStart);
			PROFILE_VER *pNewEndVertex=hashDestVertexPtrBySrcVertexPtr.GetValue((DWORD)pRollEnd);
			if(pNewStartVertex)
				pNewStartVertex->vertex=huoqu_line.startPt;
			if(pNewEndVertex)
				pNewEndVertex->vertex=huoqu_line.endPt;
			PROFILE_VER new_vertex;
			new_vertex.type=1;	//直线
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
	//重新写入钢板的轮廓点
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
		{	//直线段沿着角平分线进行偏移
			f3dPoint pre_vec=(pVertex->vertex-pVertexPre->vertex).normalized();
			f3dPoint cur_vec=(pVertex->vertex-pVertexNext->vertex).normalized();
			if(fabs(pre_vec*cur_vec)<0.9999)
			{
				double angle=cal_angle_of_2vec(pre_vec,cur_vec);
				double offset=minDistance/sin(angle/2);
				f3dPoint verify_vec=cur_vec^pre_vec;
				f3dPoint vec=(pre_vec+cur_vec).normalized();
				//需根据轮廓边垂线方向修正，对角线偏移方向，否则凹点会向反方向偏移 wht 18-12-05
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
		{	//圆弧
			f3dArcLine* pArcLine=NULL;
			if(pVertex->type>=2)	//圆弧
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
	//1.构件工艺信息
	short bolt_count;
	buffer.ReadWord(&bolt_count);			//孔数
	short vertex_list_count;
	buffer.ReadWord(&vertex_list_count);	//边数
	buffer.ReadByte(&m_cFaceN);				//面数
	//2. 火曲信息
	for (BYTE i=0;i<m_cFaceN-1;i++)
	{
		buffer.ReadPoint(HuoQuFaceNorm[i]);				//(板相对坐标系下的)火曲面法线
		buffer.ReadPoint(HuoQuLine[i].startPt);			//(板相对坐标系下的)火曲线起点
		buffer.ReadPoint(HuoQuLine[i].endPt);			//(板相对坐标系下的)火曲线终点
		if(buf_type==CProcessPart::PPI_BUFFER && version>=1000003)
		{
			buffer.ReadDouble(&m_fHuoQuLineR[i]);
			buffer.ReadDouble(&m_fHuoQuLineX[i]);
		}
	}
	if(buf_type==CProcessPart::PPI_BUFFER && version>=1000005)
	{
		buffer.ReadByte(&m_ciRollProcessType);	//卷边处理方式
		buffer.ReadByte(&m_ciRollOffsetType);	//卷边线计算方式
		buffer.ReadBoolean(&m_bIncDeformed);	//是否考虑火曲变形
		buffer.ReadByte(&m_ciDeformType);		//火曲变形处理方式
	}
	float bolt_d_increment;					//螺栓直径增量
	buffer.ReadFloat(&bolt_d_increment);
	//3. 螺栓信息
	m_xBoltInfoList.Empty();
	for(int i=0;i<bolt_count;i++)
	{
		BOLT_INFO boltInfo;
		if(bSaveSubSerial)
			buffer.ReadInteger(&boltInfo.keyId);
		boltInfo.hole_d_increment=bolt_d_increment;
		boltInfo.FromBuffer(buffer,version,buf_type);
		//填充哈希表，根据keyI的初始化HiberId
		m_xBoltInfoList.SetValue(boltInfo.keyId,boltInfo,true);
	}
	//4. 钢板外形信息
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
		buffer.ReadByte(&profile_vertex.type);		//边类型(1表示直线段；2表示圆弧；3表示椭圆弧）
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
			profile_vertex.m_bWeldEdge = true;					//标识焊缝
		else 
			profile_vertex.m_bRollEdge = true;					//标识卷边
		buffer.ReadDouble(&profile_vertex.vertex.x);			//边起点X坐标
		buffer.ReadDouble(&profile_vertex.vertex.y);			//边起点Y坐标
		buffer.ReadWord(&profile_vertex.manu_space);			//焊缝加工预留间隙或卷边高度(mm)
		if(buf_type==CProcessPart::PPI_BUFFER &&version>=1000006)
			buffer.ReadWord(&profile_vertex.roll_edge_offset_dist);	//卷边偏移距离
		if(profile_vertex.m_bRollEdge&&profile_vertex.manu_space==0)
			profile_vertex.m_bRollEdge=false;
		if(profile_vertex.m_bWeldEdge)
		{	
			BYTE weld_high;
			WORD weld_dis,weld_len;
			buffer.ReadByte(&weld_high);	//焊缝高度
			buffer.ReadWord(&weld_dis);		//焊缝起点距板边起点距离
			buffer.ReadWord(&weld_len);		//焊缝长度，为0时表示一直延伸至板边终点
		}
		if(profile_vertex.type>1)
		{
			buffer.ReadDouble(&profile_vertex.sector_angle);	//（椭）圆弧扇形角，弧度制
			buffer.ReadPoint(profile_vertex.work_norm);			//圆弧面法线方向
			if(profile_vertex.type==3)
			{
				buffer.ReadDouble(&profile_vertex.radius);		//椭圆弧短半轴长度（即原始定义圆柱面半径）
				buffer.ReadPoint(profile_vertex.center);		//椭圆弧圆心位置（固接坐标系下相对坐标）
				buffer.ReadPoint(profile_vertex.column_norm);	//椭圆弧原始定义柱面轴向（固接坐标系下相对方向）
			}
		}
		//填充哈希表，根据keyId初始hiberId
		vertex_list.SetValue(profile_vertex.keyId,profile_vertex,true);
	}
	//5.加工坐标系基准边索引及打号位置
	buffer.ReadWord(&mcsFlg.wFlag);		//加工坐标系基准边索引
	if(mcsFlg.wFlag==0xffff)
	{	//兼容之前的内容
		mcsFlg.ciBottomEdge=255;	//表示-1
		mcsFlg.ciOverturn=0;
	}
	float posX=0,posY=0;
	buffer.ReadFloat(&posX);			//打号位置X坐标
	buffer.ReadFloat(&posY);			//打号位置Y坐标
	mkpos.Set(posX,posY);
	if(buf_type==CProcessPart::PPI_BUFFER && version>=1000002)
	{	//记录矩形钢印号的方向
		buffer.ReadFloat(&posX);
		buffer.ReadFloat(&posY);
		mkVec.Set(posX,posY);
	}
	//获取螺栓所在面号
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
	{	//根据轮廓点面号顺序进行判断，2到3中间无过度(12或13)轮廓点时为折叠板
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
//重新获取螺栓所在面号
void CProcessPlate::RetrievedBoltFaceNo()
{
	if(m_cFaceN<=1||vertex_list.GetNodeNum()<3)
		return;
	//1.建立各面轮廓点组成的多边形区域
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
		{	//计算三面板范围各面区域是应考虑top_point点
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
	//2.根据螺栓所在区域确定螺栓面号
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
//输入一个法线向量，返回此向量所在的板平面号0,1,2
//face_norm为钢板坐标系下的相对坐标
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
//根据螺栓直径统计钢板上该螺栓个数
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
//获取矩形钢印号的轮廓点坐标
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
		//计算第一面法线到其余面法线的旋转轴线
		rotate_axis = cross_prod(f3dPoint(0,0,1),HuoQuFaceNorm[i]);
		if(i==0)	//第一面与第二面间的夹角
			angle[0] = cal_angle_of_2vec(f3dPoint(0,0,1),HuoQuFaceNorm[i]);
		else		//第一面与第三面间的夹角
			angle[1] = cal_angle_of_2vec(f3dPoint(0,0,1),HuoQuFaceNorm[i]);
		//计算当前火曲线方向
		Sub_Pnt(vec,HuoQuLine[i].endPt,HuoQuLine[i].startPt);

		if(dot_prod(vec,rotate_axis)<0)	//当前火曲线与旋转轴线反向
		{								//火曲线方向应与旋转轴线方向一致
			tempt = HuoQuLine[i].startPt;
			HuoQuLine[i].startPt = HuoQuLine[i].endPt;
			HuoQuLine[i].endPt = tempt;
		}
	}
	vector_trans(ls_norm,acs,FALSE);
	int iLsFace=GetNormFaceI(ls_norm);
	if(iLsFace==1)		//第二面上的螺栓
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
	else if(iLsFace==2)	//第三面上的螺栓
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
	else				//第一面上的螺栓
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
		//计算第一面法线到其余面法线的旋转轴线
		rotate_axis = cross_prod(f3dPoint(0,0,1),HuoQuFaceNorm[i]);
		if(i==0)	//第一面与第二面间的夹角
			angle[0] = cal_angle_of_2vec(f3dPoint(0,0,1),HuoQuFaceNorm[i]);
		else		//第一面与第三面间的夹角
			angle[1] = cal_angle_of_2vec(f3dPoint(0,0,1),HuoQuFaceNorm[i]);
		//计算当前火曲线方向
		Sub_Pnt(vec,HuoQuLine[i].endPt,HuoQuLine[i].startPt);

		if(dot_prod(vec,rotate_axis)<0)	//当前火曲线与旋转轴线反向
		{								//火曲线方向应与旋转轴线方向一致
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
	//1.比较基本信息和螺栓信息
	if(!CProcessPart::IsEqual(pPart,epsilon,error_info,dwChkStyleFlag))
		return false;
	CProcessPlate *pPlate=(CProcessPlate*)pPart;
	if(m_cFaceN!=pPlate->m_cFaceN)
	{
		if(error_info)
			strcpy(error_info,"钢板面数不同");
		return false;
	}
	if( !top_point.IsEqual(pPlate->top_point,epsilon))
	{
		if(error_info)
			strcpy(error_info,"三面板火曲线交点不同");
		return false;
	}
	if( !m_tInnerOrigin.IsEqual(pPlate->m_tInnerOrigin)||
		!m_tInnerColumnNorm.IsEqual(pPlate->m_tInnerColumnNorm)||
		fabs(m_fInnerRadius-pPlate->m_fInnerRadius)>epsilon)
	{
		if(error_info)
			strcpy(error_info,"钢板内圆不同");
		return false;
	}
	//f3dLine HuoQuLine[2];				//火曲线（板相对坐标）
	//f3dPoint HuoQuFaceNorm[2];		//节点板各面的法线方向
	//2.比较外形轮廓点及火曲线
	if(vertex_list.GetNodeNum()!=pPlate->vertex_list.GetNodeNum())
	{
		if(error_info)
			strcpy(error_info,"钢板轮廓点数量不同");
		return false;
	}
	//暂时先简单比较轮廓点坐标值
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
			strcpy(error_info,"钢板轮廓点不同");
		return false;
	}
	return true;
}
double CProcessPlate::GetHuoQuAngle(int iFace,f3dLine *pHuoQuLine/*=NULL*/,BOOL bDisplayLog/*=TRUE*/)
{
	if(m_cFaceN<=1||vertex_list.GetNodeNum()<3||iFace<1||iFace>m_cFaceN-1)	//单面板没有变形
		return 0;
	else
	{
		iFace++;	
		f3dLine huoquline;
		BOOL bFindHuoQuStart=FALSE,bFindHuoQuEnd=FALSE;
		if(m_cFaceN==2||IsFoldPlate())	//双面板或折叠板
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
					bReverse=TRUE;	//始终点插入了基本面顶点，说明始终端反了，否则始终顶点间只能是火曲面顶点
			}
			if(bFindHuoQuStart&&bFindHuoQuEnd)
			{
				if(bReverse)
				{	//始终端反向
					f3dPoint pt=huoquline.startPt;
					huoquline.startPt=huoquline.endPt;
					huoquline.endPt=pt;
				}
			}
		}
		else if(m_cFaceN==3)	//普通三面板
		{
			int prev_face_no=0,now_face_no=0,next_face_no=0;
			bool bNeedInverseHuoQuFace=false;	//三面板是否需要颠倒两火曲面（便于逆时针生成实体面）
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
			if(pVertex!=NULL)	//找到火曲线端点
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
				logerr.Log("钢板%s没找到合理的火曲线与板边缘相交顶点!",(char*)m_sPartNo,iFace-1);
			return 0;
		}
		f3dPoint huoqu_axis_vec=huoquline.endPt-huoquline.startPt;
		f3dPoint huoqu_norm=HuoQuFaceNorm[iFace-2];
		if(huoqu_norm.IsZero())
		{
			if (bDisplayLog)
				logerr.Log("钢板%s的第%d(火曲)面法线为空，火曲角计算有误!",(char*)m_sPartNo,iFace-1);
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
				logerr.Log("钢板%s的第%d(火曲)面法线方向与火曲线不垂直，火曲角计算有误!", (char*)m_sPartNo, iFace - 1);
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

//iFace取值0或1
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
			//保证基准面和火曲面的拾取点不在火曲线上，否则无法正确判断内外曲 wht 11-04-21
			/** 后面添加fabs(dist)>10判断是为了避免连板轮廓顶点与火曲点接近（常出现于合并生成火曲连接板），
			   且基面火曲面错乱时，导致错误 wjh-2016.4.29
			*/
			double dist1 = 0, dist2 = 0;
			if (!basepick_finded)
			{
				dist1 = DistOf2dPtLine(pt, HuoQuLine[0].startPt, HuoQuLine[0].endPt);
				if (m_cFaceN >= 3)
					dist2 = DistOf2dPtLine(pt, HuoQuLine[1].startPt, HuoQuLine[1].endPt);
			}
			if (m_cFaceN == 3)
			{   //保证基准面和火曲面的拾取点不在火曲线上，否则无法正确判断内外曲 wht 11-04-21
				if (fabs(dist1) > 10 && fabs(dist2) > 10 && HuoQuLine[0].PtInLine(pt) == 0 && HuoQuLine[1].PtInLine(pt) == 0)
				{
					if (!basepick_finded&&pt.feature == 1)      //基准面拾取点
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
					if (!basepick_finded&&pt.feature == 1)      //基准面拾取点
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
		if (dd < Pi / 2 || (fabs(dd - Pi / 2) < EPS&&norm.z > 0))   //火曲面向外曲(正曲)
			bNeiQu = FALSE;
		cBendType = bNeiQu ? BEND_IN : BEND_OUT;
	}
	return cBendType;
}
f3dPoint CProcessPlate::GetDeformedVertex(f3dPoint vertice)
{
	if(m_ciDeformType==0||m_cFaceN<=1||vertex_list.GetNodeNum()<3||vertice.feature<=1)	//单面板没有变形
		return vertice;
	else
	{
		f3dLine huoquline;
		int iFace=vertice.feature%10;
		//为避免制弯线与制弯角度获取错误，统一都由GetHuoQuAngle来实现 wjh-2011.8.31
		double huoquangle=GetHuoQuAngle(iFace-1,&huoquline);
		f3dPoint huoqu_axis_vec=huoquline.endPt-huoquline.startPt;
		UCS_STRU huoquucs;
		huoquucs.axis_z=huoqu_axis_vec;
		normalize(huoquucs.axis_z);
		huoquucs.axis_x.Set(0,0,1);
		huoquucs.axis_y=huoquucs.axis_z^huoquucs.axis_x;
		//根据用户选择的火曲变形方式，调整火曲面上的坐标
		double r=0,x=0,odd_len=0;
		if(m_ciDeformType==1)	//中性层进行火曲变形
			x=0.35;
		else if(m_ciDeformType==2)			//凸面进行火曲变形
			x=1;
		else if(m_ciDeformType==3)			//凹面进行火曲变形
			x=0;
		//计算火曲补偿量
		if(huoquangle>0)	//内曲,底面是基准面
			odd_len=huoquangle*(r+x*m_fThick)-2*r*tan(huoquangle/2);
		else	//外曲,顶面是基准面
			odd_len=-huoquangle*(r+x*m_fThick)-2*(r+m_fThick)*tan(-huoquangle/2);
		if(vertice.feature>10)	//火曲点
			vertice+=huoquucs.axis_y*(odd_len/2);
		else	//火曲面上的其它点
			vertice+=huoquucs.axis_y*odd_len;
		return vertice;
	}
}
////////////////////////////////////////////////////////////////////
// CProcessAngle
#ifdef __PROPERTYLIST_UI_
IMPLEMENT_PROP_FUNC(CProcessAngle)
//初始化属性id
void CProcessAngle::InitPropHashtable()
{
	int id=1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"角钢基本信息"));
	AddPropItem("m_sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	AddPropItem("m_Seg",PROPLIST_ITEM(id++,"分段号"));
	AddPropItem("m_cMaterial",PROPLIST_ITEM(id++,"材质"));
	AddPropItem("m_sSpec",PROPLIST_ITEM(id++,"规格"));
#ifdef __TAP_CONTEXT_
	AddPropItem("m_cOrgMaterial",PROPLIST_ITEM(id++,"原材质"));
	AddPropItem("m_sOrgSpec",PROPLIST_ITEM(id++,"原规格"));
#endif
	AddPropItem("m_wLength",PROPLIST_ITEM(id++,"长度"));
	AddPropItem("m_fWeight",PROPLIST_ITEM(id++,"重量"));
	AddPropItem("m_cFuncType",PROPLIST_ITEM(id++,"功能类型","角钢功能","普通角钢|加劲角钢|脚钉角钢|塔腿角钢|塔身角钢|塔头角钢"));
	AddPropItem("m_fDecWingAngle",PROPLIST_ITEM(id++,"两肢夹角"));
	AddPropItem("m_sNote",PROPLIST_ITEM(id++,"备注"));
	AddPropItem("m_dwInheritPropFlag",PROPLIST_ITEM(id++,"同步模式","","...."));
	//工艺信息
	AddPropItem("processInfo",PROPLIST_ITEM(id++,"工艺信息"));
	AddPropItem("startCutWing",PROPLIST_ITEM(id++,"始端切角切肢"));
	AddPropItem("startCutWingX",PROPLIST_ITEM(id++,"X肢","始端X肢切角切肢信息"));
	AddPropItem("startCutWingY",PROPLIST_ITEM(id++,"Y肢","始端Y肢切角切肢信息"));
	AddPropItem("endCutWing",PROPLIST_ITEM(id++,"终端切角切肢"));
	AddPropItem("endCutWingX",PROPLIST_ITEM(id++,"X肢","终端X肢切角切肢信息"));
	AddPropItem("endCutWingY",PROPLIST_ITEM(id++,"Y肢","终端Y肢切角切肢信息"));
	AddPropItem("wing_push_X1_Y2",PROPLIST_ITEM(id++,"压扁","","0.无|1.X肢|2.Y肢|"));
	AddPropItem("start_push_pos",PROPLIST_ITEM(id++,"压扁起始位置"));
	AddPropItem("end_push_pos",PROPLIST_ITEM(id++,"压扁终止位置"));
	AddPropItem("m_bCutRoot",PROPLIST_ITEM(id++,"清根","","是|否"));
	AddPropItem("m_bCutBer",PROPLIST_ITEM(id++,"铲背","","是|否"));
	AddPropItem("m_bWeld",PROPLIST_ITEM(id++,"焊接","","是|否"));
	AddPropItem("m_bNeedFillet",PROPLIST_ITEM(id++,"坡口","","是|否"));
	AddPropItem("kaihe_base_wing_x0_y1",PROPLIST_ITEM(id++,"开合角","","0.X肢|1.Y肢|2.无|"));
	AddPropItem("kaihe_angle",PROPLIST_ITEM(id++,"两肢夹角"));
	//螺栓孔信息
	AddPropItem("boltInfo",PROPLIST_ITEM(id++,"螺栓孔"));
	AddPropItem("x_wing_ls",PROPLIST_ITEM(id++,"X肢","X肢螺栓孔数"));
	AddPropItem("y_wing_ls",PROPLIST_ITEM(id++,"Y肢","Y肢螺栓孔数"));
	/*for(int i=0;i<100;i++)
	{
		CXhChar50 sXWingHole("X肢孔-%d",i+1),sYWingHole("Y肢孔-%d",i+1),
			sXWingKey("x_wing_ls[%d]",i),sYWingKey("y_wing_ls[%d]",i);
		AddPropItem(sXWingKey,PROPLIST_ITEM(id++,sXWingHole));
		AddPropItem(sYWingKey,PROPLIST_ITEM(id++,sYWingHole));
	}*/
	AddPropItem("Place",PROPLIST_ITEM(id++,"占位"));
	AddPropItem("m_cPlaceMode",PROPLIST_ITEM(id++,"占位模式","占位模式","0.自由占位|1.4占位模式1|2.4占位模式2|3.8占位模式1|4.8占位模式2"));
	AddPropItem("POS_FRONT_PLUS",PROPLIST_ITEM(id++,"前","","添加|删除"));
	AddPropItem("POS_BACK_PLUS",PROPLIST_ITEM(id++,"后","","添加|删除"));
	AddPropItem("POS_LEFT_PLUS",PROPLIST_ITEM(id++,"左","","添加|删除"));
	AddPropItem("POS_RIGHT_PLUS",PROPLIST_ITEM(id++,"右","","添加|删除"));
	AddPropItem("POS_FRONT_MINUS",PROPLIST_ITEM(id++,"前X-","","添加|删除"));
	AddPropItem("POS_BACK_MINUS",PROPLIST_ITEM(id++,"后X-","","添加|删除"));
	AddPropItem("POS_LEFT_MINUS",PROPLIST_ITEM(id++,"左Y-","","添加|删除"));
	AddPropItem("POS_RIGHT_MINUS",PROPLIST_ITEM(id++,"右Y-","","添加|删除"));
	//
	InitSyncProp();
}

//通过属性Id获取属性值
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
		CXhChar100 cmbItemArr[5]={"0.自由占位","1.4占位模式1","2.4占位模式2","3.8占位模式1","4.8占位模式2"};
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
			sText.Copy("√");

	}
	else if(GetPropID("POS_BACK_PLUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_BACK_PLUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_BACK_PLUS));
		else 
			sText.Copy("√");
	}
	else if(GetPropID("POS_LEFT_PLUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_LEFT_PLUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_LEFT_PLUS));
		else 
			sText.Copy("√");
	}
	else if(GetPropID("POS_RIGHT_PLUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_RIGHT_PLUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_RIGHT_PLUS));
		else 
			sText.Copy("√");
	}
	else if(GetPropID("POS_FRONT_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_FRONT_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_FRONT_MINUS));
		else 
			sText.Copy("√");
	}
	else if(GetPropID("POS_BACK_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_BACK_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_BACK_MINUS));
		else 
			sText.Copy("√");
	}
	else if(GetPropID("POS_LEFT_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_LEFT_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_LEFT_MINUS));
		else 
			sText.Copy("√");
	}
	else if(GetPropID("POS_RIGHT_MINUS")==id)
	{
		if(IsIdleClassicPlace(ASSEMBLEY_POS::POS_RIGHT_MINUS))
			sText.Empty();
		else if(GetPlaceContentFunc)
			sText.Copy(GetPlaceContentFunc(this,ASSEMBLEY_POS::POS_RIGHT_MINUS));
		else 
			sText.Copy("√");
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
			sText.Copy("加劲角钢");
		else if(CProcessAngle::FUNC_FOOTNAIL==m_cFuncType)
			sText.Copy("脚钉角钢");
		else if(CProcessAngle::FUNC_LEGANGLE==m_cFuncType)
			sText.Copy("塔腿角钢");
		else if(CProcessAngle::FUNC_BODYANGLE==m_cFuncType)
			sText.Copy("塔身角钢");
		else if(CProcessAngle::FUNC_HEADANGLE==m_cFuncType)
			sText.Copy("塔头角钢");
		else //if(CProcessAngle::FUNC_COMMON==m_cFuncType)
			sText.Copy("普通角钢");
	}
	else if(GetPropID("m_sNote")==id)
		sText.Copy(GetNotes()); 
	else if(GetPropID("startCutWingX")==id)
	{	//始端切肢切角
		if(cut_wing[0]==1)	
		{	//始端X肢切肢
			sText.Printf("%.fx%.fx%.f",cut_wing_para[0][1],
						cut_wing_para[0][0],cut_wing_para[0][2]);
		}
		if(cut_angle[0][0]>0)
		{	//始端X肢切角
			if(sText.GetLength()>0)
				sText.Append(",");
			sText.Printf("%s%.fx%.f",(char*)sText,cut_angle[0][1],cut_angle[0][0]);
		}
	}
	else if(GetPropID("startCutWingY")==id)
	{	//始端切肢切角
		if(cut_wing[0]==2)	
		{	//始端Y肢切肢
			sText.Printf("%.fx%.fx%.f",cut_wing_para[0][1],
							cut_wing_para[0][0],cut_wing_para[0][2]);
		}
		if(cut_angle[1][0]>0)
		{	//始端Y肢切角
			if(sText.GetLength()>0)
				sText.Append(",");
			sText.Printf("%s%.fx%.f",(char*)sText,cut_angle[1][1],cut_angle[1][0]);
		}
	}
	else if(GetPropID("endCutWingX")==id)
	{	//终端切肢切角
		if(cut_wing[1]==1)	
		{	//终端X肢切肢
			sText.Printf("%.fx%.fx%.f",cut_wing_para[1][1],
						cut_wing_para[1][0],cut_wing_para[1][2]);
		}
		if(cut_angle[2][0]>0)
		{	//终端X肢切角
			if(sText.GetLength()>0)
				sText.Append(",");
			sText.Printf("%s%.fx%.f",(char*)sText,cut_angle[2][1],cut_angle[2][0]);
		}
	}
	else if(GetPropID("endCutWingY")==id)
	{	//终端切肢切角
		if(cut_wing[1]==2)	
		{	//终端Y肢切肢
			sText.Printf("%.fx%.fx%.f",cut_wing_para[1][1],
						cut_wing_para[1][0],cut_wing_para[1][2]);
		}
		if(cut_angle[3][0]>0)
		{	//终端Y肢切角
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
				sText.Copy("1.X肢");
			else if(wing_push_X1_Y2==2)
				sText.Copy("2.Y肢");
		}
		else 
			sText.Copy("0.无");
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
			sText.Copy("是");
		else 
			sText.Copy("否");
	}
	else if(GetPropID("m_bCutBer")==id)
	{
		if(m_bCutBer)
			sText.Copy("是");
		else 
			sText.Copy("否");
	}
	else if(GetPropID("m_bWeld")==id)
	{
		if(m_bWeld)
			sText.Copy("是");
		else 
			sText.Copy("否");
	}
	else if(GetPropID("m_bNeedFillet")==id)
	{
		if(m_bNeedFillet)
			sText.Copy("是");
		else 
			sText.Copy("否");
	}
	else if(GetPropID("kaihe_base_wing_x0_y1")==id)
	{
		if(IsWingKaiHe())
		{
			if(kaihe_base_wing_x0_y1==0)
				sText.Copy("0.X肢");
			else if(kaihe_base_wing_x0_y1==1)
				sText.Copy("1.Y肢");
		}
		else 
			sText.Copy("2.无");
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
		WORD start_flate_pos=0;//始端压扁位置
		double startLength=0.0; //记录压扁的分段角钢前几段角钢长度 
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
		WORD end_flate_pos=0;//终端压扁位置
		double startLength=0.0; //记录压扁的分段角钢前几段角钢长度 
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
			if(cut_wing[i]==1)		//X肢有切肢
				sCutAngleX.Printf("%.0f=%.0f",cut_wing_para[i][0],cut_wing_para[i][2]);
			else if(cut_wing[0]==2)//Y肢有切肢
				sCutAngleX.Printf("%.0f<%.0f",cut_wing_para[i][1],cut_wing_para[i][0]);
			else					//X肢有切角
				sCutAngleX.Printf("%.0fX%.0f",cut_angle[iCutAngleStart][1],cut_angle[iCutAngleStart][0]);
		}
		if((cut_angle[iCutAngleStart+1][0]>0&&cut_angle[iCutAngleStart+1][1]>0)||cut_wing[i]>0)
		{
			if(cut_wing[0]==1)		//X肢有切肢
				sCutAngleY.Printf("%.0f<%.0f",cut_wing_para[i][1],cut_wing_para[i][0]);
			else if(cut_wing[0]==2)//Y肢有切肢
				sCutAngleY.Printf("%.0f=%.0f",cut_wing_para[i][0],cut_wing_para[i][2]);
			else						//Y肢有切角
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
	{	//拷贝螺栓及顶点id
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
	float bolt_d_increment=0;	//螺栓直径增量
	BOLT_INFO *pFirstBolt=m_xBoltInfoList.GetFirst();
	if(pFirstBolt)
		bolt_d_increment=pFirstBolt->hole_d_increment;		//螺栓直径增量
	////////////////////////////////////////////////////////////////
	//1.孔数、螺栓孔径增量
	long nRibPlateCount=rib_plate_list.GetNodeNum();	
	if(nRibPlateCount>127)
		logerr.Log("%s角钢肋板个数为%d,角钢肋板个数最多支持127个！",(char*)GetPartNo(),nRibPlateCount);
	buffer.WriteByte((BYTE)nRibPlateCount);	//角钢肋板个数(角钢肋板个数，最多支持127块肋板)
	long bolt_count=m_xBoltInfoList.GetNodeNum();
	buffer.WriteWord((WORD)bolt_count);		//螺栓孔数
	buffer.WriteFloat(bolt_d_increment);	//螺栓直径增量
	//2.1B：工艺信息字节1(1.始端Ｘ肢切角(肢),2.始端Ｙ肢切角(肢),3.终端Ｘ肢切角(肢),4.终端Ｙ肢切角(肢),5.清根,6.铲背,7.焊接,8.坡口)
	BYTE process_info_first_byte=0;
	BOOL processStateArr[8]={((cut_wing_para[0][0]&&cut_wing[0]==1)||cut_angle[0][0]),	//1.始端X肢切肢(角)
							 ((cut_wing_para[0][0]&&cut_wing[0]==2)||cut_angle[1][0]),	//2.始端Y肢切肢(角)
							 ((cut_wing_para[1][0]&&cut_wing[1]==1)||cut_angle[2][0]),	//3.终端X肢切肢(角)
							 ((cut_wing_para[1][0]&&cut_wing[1]==2)||cut_angle[3][0]),	//4.终端Y肢切肢(角)
							 _bCutRoot,_bCutBer,_bWeld,FALSE};							//5.清根,6.铲背,7.焊接,8.坡口
	for(int i=0;i<8;i++)
	{
		if(processStateArr[i])
			process_info_first_byte |= byteConstArr[i];
	}
	buffer.WriteByte(process_info_first_byte);
	//3. 1B：工艺信息字节2(1~3位表示制弯分段数，第4位0表示X肢压扁，1表示Y肢压扁，5~8位保留)
	BYTE process_info_second_byte=0;
	//3.1 制弯分段数
	if(poly_part_num>7)
		logerr.Log("%s制弯分段数为%d,制弯件最多支持7个分段！",(char*)GetPartNo(),poly_part_num);
	process_info_second_byte =(BYTE)poly_part_num;		//制弯分段数
	//3.2 压扁肢
	if(wing_push_X1_Y2==1)
		process_info_second_byte &= 0xF7;	//第4位0表示x肢压扁
	else if(wing_push_X1_Y2==2)
		process_info_second_byte |= 0x08;	//第4位1表示y肢压扁
	buffer.WriteByte(process_info_second_byte);
	//4. 1B：工艺信息字节3（第1~7位：开合区域数，第8位：0表示X肢开合角，1表示Y肢开合角）
	BYTE process_info_third_byte =0;
	//4.1 开合区域数(第1~7位)
	if(kaiHeJiaoList.GetNodeNum()>127)
		logerr.Log("%s开合角区域数为%d,开合角区域数最多支持127个！",(char*)GetPartNo(),kaiHeJiaoList.GetNodeNum());
	process_info_third_byte=(BYTE)kaiHeJiaoList.GetNodeNum();
	//4.2 第8位：0表示X肢开合角，1表示Y肢开合角
	if(kaihe_base_wing_x0_y1==0)
		process_info_third_byte &= 0x7F;	//x肢开合角
	else if(kaihe_base_wing_x0_y1==1)
		process_info_third_byte |= 0x80;	//y肢开合角
	buffer.WriteByte(process_info_third_byte);
	//5.48B(4Bx4x3)切角(肢)数据
	for (int i=0;i<4;i++)
	{
		if(cut_angle[i][0]>0)
		{	//切角
			buffer.WriteWord((WORD)cut_angle[i][0]);
			buffer.WriteWord((WORD)cut_angle[i][1]);
			buffer.WriteWord((WORD)0xFFFF);
		}
		else 
		{	//切肢
			int index=i<2?0:1;
			buffer.WriteWord((WORD)cut_wing_para[index][0]);
			buffer.WriteWord((WORD)cut_wing_para[index][1]);
			buffer.WriteWord((WORD)cut_wing_para[index][2]);
		}
	}
	//6.螺栓信息
	BOLT_INFO *pBolt=NULL;
	for(pBolt=m_xBoltInfoList.GetFirst();pBolt;pBolt=m_xBoltInfoList.GetNext())
	{
		if(bSaveSubSerial)
			buffer.WriteInteger(pBolt->keyId);
		pBolt->ToBuffer(buffer,version,buf_type);
	}
	//7.分段角钢索引标识、Ｘ肢展开方向、Ｙ肢展开方向、延伸方向
	//均是在第一段直线角钢Ｘ肢展开方向为Ｘ轴，杆件延伸方向为Ｚ轴的制弯角钢定位坐标系下定义的
	for (POLY_ANGLE_INFO *pPolySeg =poly_part_info.GetFirst();pPolySeg;pPolySeg=poly_part_info.GetNext())
	{
		buffer.WriteInteger(pPolySeg->nIndex);			 //索引标识
		buffer.WritePoint(pPolySeg->norm_x_wing);		 //X肢展开方向
		buffer.WritePoint(pPolySeg->norm_y_wing);		 //Y肢展开方向
		buffer.WritePoint(pPolySeg->len_vec);			 //延伸方向
	}
	//8.压扁起始位置 、终止位置
	//	2B：压扁起始位置（长度方向，0表示始端压扁）
	//	2B：压扁终止位置（>构件长度或0xffff表示终端压遍），起始位置>=终止位置表示无压扁。
	buffer.WriteWord(start_push_pos);
	buffer.WriteWord(end_push_pos);
	//9.开合角LOOP
	for (KAI_HE_JIAO *kai_he_jiao=kaiHeJiaoList.GetFirst();kai_he_jiao;kai_he_jiao=kaiHeJiaoList.GetNext())
	{
		buffer.WriteFloat(kai_he_jiao->decWingAngle);		//两肢夹角
		buffer.WriteWord(kai_he_jiao->position);			//标定位置
		buffer.WriteWord(kai_he_jiao->startLength);			//始端开合长度
		buffer.WriteWord(kai_he_jiao->endLength);			//终端开合长度
	}
	//10.角钢肋板LOOP
	for (RIB_PLATE_INFO *pRibPlateInfoTemp=rib_plate_list.GetFirst();pRibPlateInfoTemp;pRibPlateInfoTemp=rib_plate_list.GetNext())
	{
		buffer.WriteFloat(pRibPlateInfoTemp->fPosZ);				//肋板位置
		buffer.WriteFloat(pRibPlateInfoTemp->fThick);				//肋板厚度
		buffer.Write(&pRibPlateInfoTemp->m_cMaterial,sizeof(char));	//材质
		buffer.Write(pRibPlateInfoTemp->sPlateNo,16*sizeof(char));	//肋板编号
	}
}
static bool RetrievedPolyAngleBoltFaceNo(CProcessAngle *pAngle)
{
	if(pAngle==NULL||pAngle->poly_part_info.GetNodeNum()<=0)
		return false;
	double fLen=0,fPrevLen=0;
	double huoqu_r=0;
	int nMinSpace=17;	//取M12螺栓最小轧制边距
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
	//1.构件工艺信息
	BYTE rib_plate_count=0;
	WORD bolt_count=0;
	float bolt_d_increment;
	buffer.ReadByte(&rib_plate_count);
	buffer.ReadWord(&bolt_count);
	buffer.ReadFloat(&bolt_d_increment);
	//2. 1B：工艺信息字节1(1.始端Ｘ肢切角(肢),2.始端Ｙ肢切角(肢),3.终端Ｘ肢切角(肢),4.终端Ｙ肢切角(肢)),5.清根,6.铲背,7.焊接,8.坡口)
	BYTE process_info=0,cut_info=0;
	buffer.ReadByte(&process_info);
	cut_info=process_info;		//记录角钢的切角标记位，用于初始化下面的切肢信息
	//角钢切肢切角信息(1.始端Ｘ肢切角(肢),2.始端Ｙ肢切角(肢),3.终端Ｘ肢切角(肢),4.终端Ｙ肢切角(肢)),5.清根,6.铲背,7.焊接,8.坡口
	if(byteConstArr[4]&process_info)
		m_bCutRoot = true;
	else if(byteConstArr[5]&process_info)
		m_bCutBer = true;
	else if(byteConstArr[6]&process_info)
		m_bWeld = true;
	//3 1B：工艺信息字节2(1~3位表示制弯分段数，第4位0表示X肢压扁，1表示Y肢压扁，5~8位保留)
	process_info=0;
	buffer.ReadByte(&process_info);
	//3.1 制弯分段数（1~3位表示制弯分段数）
	poly_part_num=process_info&0x07;
	//3.2 压扁肢(第4位0表示X肢压扁，1表示Y肢压扁，5~8位保留)
	if(byteConstArr[3]&process_info)
		wing_push_X1_Y2 = 2;			//表示y肢压扁
	else
		wing_push_X1_Y2 = 1;			//表示x肢压扁
	//4 1B：工艺信息字节3（第1~7位：开合区域数，第8位：0表示X肢开合角，1表示Y肢开合角）
	process_info=0;
	buffer.ReadByte(&process_info);
	//4.1 开合区域数(第1~7位)
	BYTE nKaiHeJiaoCount=process_info&0x7F;
	//4.2 第8位：0表示X肢开合角，1表示Y肢开合角
	if(process_info&0x80)
		kaihe_base_wing_x0_y1=1;	//0表示X肢开合角，1表示Y肢开合角
	else
		kaihe_base_wing_x0_y1=0;
	//5. 48B 切角切肢
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
		{	//切肢
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
	//根据角钢工艺标记位，初始化角钢切肢信息
	if((byteConstArr[0]&cut_info)&&!bCutAngleArr[0])
		cut_wing[0]=1;	//始端X肢切肢
	else if((byteConstArr[1]&cut_info)&&!bCutAngleArr[1])
		cut_wing[0]=2;	//始端Y肢切肢
	if((byteConstArr[2]&cut_info)&&!bCutAngleArr[2])
		cut_wing[1]=1;	//终端X肢切肢
	else if((byteConstArr[3]&cut_info)&&!bCutAngleArr[3])
		cut_wing[1]=2;	//终端Y肢切肢
	//6 螺栓基本信息
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
	//分段角钢所对应的构件索引标识、Ｘ肢展开方向、Ｙ肢展开方向、延伸方向
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
	//7 压扁
	buffer.ReadWord(&_start_push_pos);
	buffer.ReadWord(&_end_push_pos);
	if(_start_push_pos>=_end_push_pos)
		wing_push_X1_Y2=0;	//无压扁时压扁肢设置为无,否则绘制角钢工艺卡会错误添加压扁标识符 wht 16-04-27
	//8 开合角
	kaiHeJiaoList.Empty();
	for(int i=0;i<nKaiHeJiaoCount;i++)
	{
		KAI_HE_JIAO kai_he_jiao;
		buffer.ReadFloat(&kai_he_jiao.decWingAngle);		//两肢夹角
		buffer.ReadWord(&kai_he_jiao.position);				//标定位置
		buffer.ReadWord(&kai_he_jiao.startLength);			//始端开合长度
		buffer.ReadWord(&kai_he_jiao.endLength);			//终端开合长度
		kaiHeJiaoList.append(kai_he_jiao);
	}
	//9 角钢肋板
	rib_plate_list.Empty();
	for(int i=0;i<rib_plate_count;i++)
	{
		RIB_PLATE_INFO rib_plate_info;
		buffer.ReadFloat(&rib_plate_info.fPosZ);				//肋板位置
		buffer.ReadFloat(&rib_plate_info.fThick);				//肋板厚度
		buffer.Read(&rib_plate_info.m_cMaterial,sizeof(char));	//材质
		buffer.Read(rib_plate_info.sPlateNo,16*sizeof(char));	//肋板编号
		rib_plate_list.append(rib_plate_info);
	}
	//重设制弯角钢螺栓孔所在短角钢序号
	RetrievedPolyAngleBoltFaceNo(this);
}

double CProcessAngle::GetLsG(GECS &acs,f3dPoint ls_norm,f3dPoint ls_pos,int* wingx0_y1/*=NULL*/)
{
	f3dPoint perp,ls_vec,jg_vec,norm,base_pos;
	f3dPoint norm_x_wing=-acs.axis_y,norm_y_wing=-acs.axis_x;
	KAI_HE_JIAO *pKaiHe=kaiHeJiaoList.GetFirst();
	if(pKaiHe)
	{	//acs定义时以X肢为基准，如果存在开合角钢此处需调整Y肢法线方向
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
	//判断螺栓在楞线的哪一侧
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
	//--------VVVVVVVVVV----计算角钢相对坐标系的三个坐标轴方向  Wjh--00.05.15-------
	//由于三个坐标轴必须为右手螺旋正交，故用户给定的肢法线方向有可能
	//不合理，此时各坐标轴中以Z轴方向的级别为最高（不可变），其次为X轴、Y轴
	//Z轴代表角钢的延伸方向，X、Y轴分别代表两肢的伸展方向
	GECS ucs;
	ucs.origin = org;
	//计算角钢Z轴方向
	ucs.axis_z=pPolyAngle->len_vec;
	if(base_x0_y1_mid2==0)			//X肢为坐标系基准
	{
		ucs.axis_x = ucs.axis_z^pPolyAngle->norm_x_wing;
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
	}
	else if(base_x0_y1_mid2==1)		//Y肢为坐标系基准
	{
		ucs.axis_y = pPolyAngle->norm_y_wing^ucs.axis_z;
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
	}
	else// if(base_x0_y1_mid2==2)	//两肢夹角平分面为基准
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
		int nMinSpace=17;	//取M12螺栓最小轧制边距
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
			//螺栓Z方向坐标大于当前角钢长度(制弯角度较大时，螺栓不在当前角钢上但是Z方向长度可能仍在当前角钢范围内) 
			//TODO: 此种方法判断螺栓所在子角钢分段还不够严谨，待优化 wht 15-09-24
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
		int nMinSpace=17;	//取M12螺栓最小轧制边距
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
			strcpy(error_info,"角钢切肢不同");
		return false;
	}
	if( memcmp(cut_angle[0],pOtherAngle->cut_angle[0],sizeof(float)*2)!=0||
		memcmp(cut_angle[1],pOtherAngle->cut_angle[1],sizeof(float)*2)!=0||
		memcmp(cut_angle[2],pOtherAngle->cut_angle[2],sizeof(float)*2)!=0||
		memcmp(cut_angle[3],pOtherAngle->cut_angle[3],sizeof(float)*2)!=0)
	{
		if(error_info)
			strcpy(error_info,"角钢切角不同");
		return false;
	}
	if( start_push_pos!=pOtherAngle->start_push_pos||
		end_push_pos!=pOtherAngle->end_push_pos||
		wing_push_X1_Y2!=pOtherAngle->wing_push_X1_Y2)
	{
		if(error_info)
			strcpy(error_info,"角钢压扁不同");
		return false;
	}
	if(kaihe_base_wing_x0_y1!=pOtherAngle->kaihe_base_wing_x0_y1)
	{
		if(error_info)
			strcpy(error_info,"开合基准肢不同");
		return false;
	}
	if(poly_part_num!=pOtherAngle->poly_part_num)
	{
		if(error_info)
			strcpy(error_info,"制弯角钢分段数不同");
		return false;
	}
	if( m_bCutBer!=pOtherAngle->m_bCutBer||
		m_bCutRoot!=pOtherAngle->m_bCutRoot||
		m_bWeld!=pOtherAngle->m_bWeld||
		m_bNeedFillet!=pOtherAngle->m_bNeedFillet)
	{
		if(error_info)
			strcpy(error_info,"工艺信息不同");
		return false;
	}
	//ATOM_LIST<KAI_HE_JIAO> kaiHeJiaoList;		
	//ATOM_LIST<RIB_PLATE_INFO> rib_plate_list;		//角钢肋板链表
	//ATOM_LIST<POLY_ANGLE_INFO> poly_part_info;	//制弯角钢信息
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
	{	//初始化角钢原始材质、肢宽、肢厚
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
	{	//读取代料角钢肢宽、肢厚
		buffer.ReadFloat(&m_fOrgWidth);
		buffer.ReadFloat(&m_fOrgThick);
	}
	if(!IsReplacePart())
	{	//初始化角钢原始材质、肢宽、肢厚
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
		//存储代料角钢肢宽、肢厚
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
//初始化属性id
template<class TYPE> void InitPartPropHashtable()
{
	int id=1;
	TYPE::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	TYPE::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	TYPE::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"钢管基本信息"));
	TYPE::AddPropItem("m_sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	TYPE::AddPropItem("m_Seg",PROPLIST_ITEM(id++,"分段号"));
	TYPE::AddPropItem("m_cMaterial",PROPLIST_ITEM(id++,"材质"));
	TYPE::AddPropItem("m_sSpec",PROPLIST_ITEM(id++,"规格"));
#ifdef __TAP_CONTEXT_
	TYPE::AddPropItem("m_cOrgMaterial",PROPLIST_ITEM(id++,"原材质"));
	TYPE::AddPropItem("m_sOrgSpec",PROPLIST_ITEM(id++,"原规格"));
#endif
	TYPE::AddPropItem("m_wLength",PROPLIST_ITEM(id++,"长度"));
	TYPE::AddPropItem("m_fWeight",PROPLIST_ITEM(id++,"重量"));
	TYPE::AddPropItem("m_cFuncType",PROPLIST_ITEM(id++,"功能类型","角钢功能","普通角钢|加劲角钢|脚钉角钢|塔腿角钢|塔身角钢|塔头角钢"));
	TYPE::AddPropItem("m_sNote",PROPLIST_ITEM(id++,"备注"));
	TYPE::AddPropItem("m_dwInheritPropFlag",PROPLIST_ITEM(id++,"模式","","...."));
	//
	TYPE::AddPropItem("Place",PROPLIST_ITEM(id++,"占位"));
	TYPE::AddPropItem("m_cPlaceMode",PROPLIST_ITEM(id++,"占位模式","占位模式","0.自由占位|1.4占位模式1|2.4占位模式2|3.8占位模式1|4.8占位模式2"));
	TYPE::AddPropItem("POS_FRONT_PLUS",PROPLIST_ITEM(id++,"前","","添加|删除"));
	TYPE::AddPropItem("POS_BACK_PLUS",PROPLIST_ITEM(id++,"后","","添加|删除"));
	TYPE::AddPropItem("POS_LEFT_PLUS",PROPLIST_ITEM(id++,"左","","添加|删除"));
	TYPE::AddPropItem("POS_RIGHT_PLUS",PROPLIST_ITEM(id++,"右","","添加|删除"));
	TYPE::AddPropItem("POS_FRONT_MINUS",PROPLIST_ITEM(id++,"前X-","","添加|删除"));
	TYPE::AddPropItem("POS_BACK_MINUS",PROPLIST_ITEM(id++,"后X-","","添加|删除"));
	TYPE::AddPropItem("POS_LEFT_MINUS",PROPLIST_ITEM(id++,"左Y-","","添加|删除"));
	TYPE::AddPropItem("POS_RIGHT_MINUS",PROPLIST_ITEM(id++,"右Y-","","添加|删除"));
}
//通过属性Id获取属性值
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
		CXhChar100 cmbItemArr[5]={"0.自由占位","1.4占位模式1","2.4占位模式2","3.8占位模式1","4.8占位模式2"};
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
			sText.Copy("√");
	}
	else if(TYPE::GetPropID("POS_BACK_PLUS")==id)
	{
		if(pPart->IsIdleClassicPlace(ASSEMBLEY_POS::POS_BACK_PLUS))
			sText.Empty();
		else if(TYPE::GetPlaceContentFunc)
			sText.Copy(TYPE::GetPlaceContentFunc(pPart,ASSEMBLEY_POS::POS_BACK_PLUS));
		else 
			sText.Copy("√");
	}
	else if(TYPE::GetPropID("POS_LEFT_PLUS")==id)
	{
		if(pPart->IsIdleClassicPlace(ASSEMBLEY_POS::POS_LEFT_PLUS))
			sText.Empty();
		else if(TYPE::GetPlaceContentFunc)
			sText.Copy(TYPE::GetPlaceContentFunc(pPart,ASSEMBLEY_POS::POS_LEFT_PLUS));
		else 
			sText.Copy("√");
	}
	else if(TYPE::GetPropID("POS_RIGHT_PLUS")==id)
	{
		if(pPart->IsIdleClassicPlace(ASSEMBLEY_POS::POS_RIGHT_PLUS))
			sText.Empty();
		else if(TYPE::GetPlaceContentFunc)
			sText.Copy(TYPE::GetPlaceContentFunc(pPart,ASSEMBLEY_POS::POS_RIGHT_PLUS));
		else 
			sText.Copy("√");
	}
	else if(TYPE::GetPropID("POS_FRONT_MINUS")==id)
	{
		if(pPart->IsIdleClassicPlace(ASSEMBLEY_POS::POS_FRONT_MINUS))
			sText.Empty();
		else if(TYPE::GetPlaceContentFunc)
			sText.Copy(TYPE::GetPlaceContentFunc(pPart,ASSEMBLEY_POS::POS_FRONT_MINUS));
		else 
			sText.Copy("√");
	}
	else if(TYPE::GetPropID("POS_BACK_MINUS")==id)
	{
		if(pPart->IsIdleClassicPlace(ASSEMBLEY_POS::POS_BACK_MINUS))
			sText.Empty();
		else if(TYPE::GetPlaceContentFunc)
			sText.Copy(TYPE::GetPlaceContentFunc(pPart,ASSEMBLEY_POS::POS_BACK_MINUS));
		else 
			sText.Copy("√");
	}
	else if(TYPE::GetPropID("POS_LEFT_MINUS")==id)
	{
		if(pPart->IsIdleClassicPlace(ASSEMBLEY_POS::POS_LEFT_MINUS))
			sText.Empty();
		else if(TYPE::GetPlaceContentFunc)
			sText.Copy(TYPE::GetPlaceContentFunc(pPart,ASSEMBLEY_POS::POS_LEFT_MINUS));
		else 
			sText.Copy("√");
	}
	else if(TYPE::GetPropID("POS_RIGHT_MINUS")==id)
	{
		if(pPart->IsIdleClassicPlace(ASSEMBLEY_POS::POS_RIGHT_MINUS))
			sText.Empty();
		else if(TYPE::GetPlaceContentFunc)
			sText.Copy(TYPE::GetPlaceContentFunc(pPart,ASSEMBLEY_POS::POS_RIGHT_MINUS));
		else 
			sText.Copy("√");
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
	AddPropItem("basicInfo",PROPLIST_ITEM(id++,"螺栓基本信息"));
	AddPropItem("m_cFamily",PROPLIST_ITEM(id++,"螺栓规格系列"));
	AddPropItem("m_sSpec",PROPLIST_ITEM(id++,"螺栓规格"));
	AddPropItem("hole_d_increment",PROPLIST_ITEM(id++,"螺孔间隙"));
	AddPropItem("L0",PROPLIST_ITEM(id++,"通过厚度"));
	AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	AddPropItem("statSegStr",PROPLIST_ITEM(id++,"段号统计范围"));
	AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	AddPropItem("dwRayNo",PROPLIST_ITEM(id++,"射线号"));
	AddPropItem("DianQuan",PROPLIST_ITEM(id++,"垫圈信息"));
	AddPropItem("cDianQuanThick",PROPLIST_ITEM(id++,"垫圈厚度"));
	AddPropItem("cDianQuanN",PROPLIST_ITEM(id++,"垫圈数量"));
	AddPropItem("wDianQuanOffset",PROPLIST_ITEM(id++,"垫圈位移"));
	AddPropItem("ANTI_LOOSE",PROPLIST_ITEM(id++,"带防松垫圈",NULL,"是|否"));
	AddPropItem("ANTI_THEFT",PROPLIST_ITEM(id++,"防盗螺栓",NULL,"是|否"));
	AddPropItem("TWO_CAP",PROPLIST_ITEM(id++,"双帽螺栓",NULL,"是|否"));
	AddPropItem("origin",PROPLIST_ITEM(id++,"位置"));
	AddPropItem("origin.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("origin.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("origin.z",PROPLIST_ITEM(id++,"Z"));
	AddPropItem("axis_x",PROPLIST_ITEM(id++,"X轴方向"));
	AddPropItem("axis_x.x",PROPLIST_ITEM(id++,"X"));
	AddPropItem("axis_x.y",PROPLIST_ITEM(id++,"Y"));
	AddPropItem("axis_x.z",PROPLIST_ITEM(id++,"Z"));
	AddPropItem("work_norm",PROPLIST_ITEM(id++,"工作法线"));
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
			sText.Printf("是");
		else 
			sText.Printf("否");
	}
	else if(GetPropID("ANTI_THEFT")==id)
	{
		BITFLAG bitFlag(cFlag);
		if(bitFlag.IsHasFlag(BOLT_INFO::ANTI_THEFT))
			sText.Printf("是");
		else 
			sText.Printf("否");
	}
	else if(GetPropID("TWO_CAP")==id)
	{
		BITFLAG bitFlag(cFlag);
		if(bitFlag.IsHasFlag(BOLT_INFO::TWO_CAP))
			sText.Printf("是");
		else 
			sText.Printf("否");
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
			{	//非常规螺栓规格
				d=(m_wIndex>>8)&0x7f;
				L=m_wIndex&0xff;
				m_fWeight=0;
				logerr.Log("螺栓系列%d未找到对应的螺栓M%dX%g",m_cFamily,d,L);
			}
			else
				logerr.Log("螺栓系列%d: %d,未找到对应的螺栓",m_cFamily,m_wIndex);
		}
	}
	else 
		logerr.Log("未找到螺栓系列%d",m_cFamily);
}
void CStdBolt::FromTAPBuffer(CBuffer &buffer,long version/*=0*/)
{
	buffer.ReadByte(&m_cFamily);
	buffer.ReadWord(&m_wIndex);
	buffer.ReadFloat(&grade);
	//根据m_cFamily及m_wIndex获取螺栓型号
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
		{	//非常规螺栓规格
			d=(m_wIndex>>8)&0x7f;
			L=m_wIndex&0xff;
			m_fWeight=0;
			logerr.Log("螺栓系列%d未找到对应的螺栓M%dX%g",m_cFamily,d,L);
		}
		else
			logerr.Log("螺栓系列%d: %d,未找到对应的螺栓",m_cFamily,m_wIndex);
	}
	else 
		logerr.Log("未找到螺栓系列%d",m_cFamily);
	//读取螺栓主实体数据
	DWORD dwSolidSize=0;
	buffer.ReadDword(&dwSolidSize);
	if(dwSolidSize>0)
	{
		solidOfBolt.CopyBuffer(buffer.GetCursorBuffer(),dwSolidSize);
		buffer.Offset(dwSolidSize);
	}
	//读取螺栓帽实体数据
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
	//写入螺栓主实体数据
	DWORD dwSolidSize=bIncSolidData?solidOfBolt.BufferLength():0;
	buffer.WriteDword(dwSolidSize);
	if(dwSolidSize>0)
		buffer.Write(solidOfBolt.BufferPtr(),solidOfBolt.BufferLength());
	//写入螺栓帽实体数据
	dwSolidSize=bIncSolidData?solidOfCap.BufferLength():0;
	buffer.WriteDword(dwSolidSize);
	if(dwSolidSize>0)
		buffer.Write(solidOfCap.BufferPtr(),solidOfCap.BufferLength());
}
#endif
