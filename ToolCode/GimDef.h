// ModDataFile.h: interface for the CModDataFile class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Buffer.h"
#include "XhCharString.h"
//////////////////////////////////////////////////////////////////////////
struct GIM_HEAD_PROP_ITEM
{
	char m_sFileTag[16];		//文件标识
	char m_sFileName[256];		//文件名称
	char m_sDesigner[64];		//设计者
	char m_sUnit[256];			//组织单位
	char m_sSoftName[128];		//软件名称
	char m_sTime[16];			//创建时间
	char m_sSoftMajorVer[8];	//软件主版本号
	char m_sSoftMinorVer[8];	//软件次版本号
	char m_sMajorVersion[8];	//标准主版本号
	char m_sMinorVersion[8];	//标准次版本号
	char m_sBufSize[8];			//存储区域大小
	//
	GIM_HEAD_PROP_ITEM()
	{
		memset(this,0,sizeof(GIM_HEAD_PROP_ITEM));
		strcpy(m_sFileTag,"GIMPKGT");
		strcpy(m_sSoftName,"信狐天诚设计绘图一体化软件");
		strcpy(m_sSoftMajorVer,"1");	//软件主版本号
		strcpy(m_sSoftMinorVer,"0");	//软件次版本号
		strcpy(m_sMajorVersion,"1");	//标准主版本号
		strcpy(m_sMinorVersion,"0");	//标准次版本号
	}
	void Clone(GIM_HEAD_PROP_ITEM* pSrcItem)
	{
		strcpy(m_sDesigner, pSrcItem->m_sDesigner);
		strcpy(m_sUnit, pSrcItem->m_sUnit);
		strcpy(m_sSoftMajorVer,pSrcItem->m_sSoftMajorVer);
		strcpy(m_sSoftMinorVer,pSrcItem->m_sSoftMinorVer);
		strcpy(m_sMajorVersion,pSrcItem->m_sMajorVersion);
		strcpy(m_sMinorVersion,pSrcItem->m_sMinorVersion);
	}
	void ToBuffer(BUFFER_IO* pIO)
	{	//写入指定头部分
		pIO->Write(m_sFileTag,16);			//文件标识
		pIO->Write(m_sFileName,256);		//文件名称
		pIO->Write(m_sDesigner,64);		//设计者
		pIO->Write(m_sUnit,256);			//组织单位
		pIO->Write(m_sSoftName,128);		//软件名称
		pIO->Write(m_sTime,16);			//创建时间		
		pIO->Write(m_sSoftMajorVer,8);		//软件主版本号
		pIO->Write(m_sSoftMinorVer,8);		//软件次版本号
		pIO->Write(m_sMajorVersion,8);		//标准主版本号
		pIO->Write(m_sMinorVersion,8);		//标准次版本号
		pIO->Write(m_sBufSize,8);			//存储区域大小
	}
	void FromBuffer(BUFFER_IO* pIO)
	{	//读取头部分
		pIO->Read(m_sFileTag,16);		//文件标识
		pIO->Read(m_sFileName,256);	//文件名称
		pIO->Read(m_sDesigner,64);		//设计者
		pIO->Read(m_sUnit,256);		//组织单位
		pIO->Read(m_sSoftName,128);	//软件名称
		pIO->Read(m_sTime,16);			//创建时间		
		pIO->Read(m_sSoftMajorVer,8);	//软件主版本号
		pIO->Read(m_sSoftMinorVer,8);	//软件次版本号
		pIO->Read(m_sMajorVersion,8);	//标准主版本号
		pIO->Read(m_sMinorVersion,8);	//标准次版本号
		pIO->Read(m_sBufSize,8);		//存储区域大小
	}
};
struct TOWER_PRPERTY_ITEM
{
	CXhChar100 m_sVoltGrade;		//电压等级(单位KV)
	CXhChar100 m_sType;				//型号
	CXhChar100 m_sTexture;			//杆塔材质：角钢塔|钢管塔
	CXhChar100 m_sFixedType;		//固定方式：自立
	CXhChar100 m_sTaType;			//杆塔类型
	int m_nCircuit;					//回路数
	CXhChar100 m_sCWireSpec;		//导线型号
	CXhChar100 m_sEWireSpec;		//地线型号
	double m_fWindSpeed;			//基本风速
	double m_fNiceThick;			//覆冰厚度
	CXhChar100 m_sWindSpan;			//水平档距
	CXhChar100 m_sWeightSpan;		//垂直档距
	double m_fFrontRulingSpan;		//前侧代表档距
	double m_fBackRulingSpan;		//后侧代表档距
	double m_fMaxSpan;				//最大档距
	CXhChar100 m_sAngleRange;		//转角范围
	double m_fDesignKV;				//设计Kv值
	CXhChar100 m_sRatedHeight;		//计算呼高
	CXhChar500 m_sHeightRange;		//呼高范围
	CXhChar500 m_sTowerWeight;		//塔重
	double m_fFrequencyRockAngle;	//工频摇摆角度
	double m_fLightningRockAngle;	//雷电摇摆角度
	double m_fSwitchingRockAngle;	//操作摇摆角度
	double m_fWorkingRockAngle;		//带电作业摇摆角度
	CXhChar100 m_sManuFacturer;		//生产厂家
	CXhChar100 m_sMaterialCode;		//物资编码
	CXhChar100 m_sProModelCode;		//装配模型编号
	//
	TOWER_PRPERTY_ITEM()
	{
		m_sVoltGrade.Copy("220KV");
		m_sTexture.Copy("角钢塔");
		m_sFixedType.Copy("拉线");
		m_sTaType.Copy("耐张塔");
		m_nCircuit=1;
		m_fWindSpeed=0;
		m_fNiceThick=0;
		m_fFrontRulingSpan=0;
		m_fBackRulingSpan=0;
		m_fMaxSpan=0;
		m_fDesignKV=0;
		m_fFrequencyRockAngle=0;
		m_fLightningRockAngle=0;
		m_fSwitchingRockAngle=0;
		m_fWorkingRockAngle=0;
	}
	void Clone(TOWER_PRPERTY_ITEM* pSrcItem)
	{
		m_sVoltGrade=pSrcItem->m_sVoltGrade;
		m_sType=pSrcItem->m_sType;
		m_sTexture=pSrcItem->m_sTexture;
		m_sFixedType=pSrcItem->m_sFixedType;
		m_sTaType=pSrcItem->m_sTaType;
		m_nCircuit=pSrcItem->m_nCircuit;
		m_sCWireSpec=pSrcItem->m_sCWireSpec;
		m_sEWireSpec=pSrcItem->m_sEWireSpec;
		m_fWindSpeed=pSrcItem->m_fWindSpeed;
		m_fNiceThick=pSrcItem->m_fNiceThick;
		m_sWindSpan=pSrcItem->m_sWindSpan;
		m_sWeightSpan=pSrcItem->m_sWeightSpan;
		m_fFrontRulingSpan=pSrcItem->m_fFrontRulingSpan;
		m_fBackRulingSpan=pSrcItem->m_fBackRulingSpan;
		m_fMaxSpan=pSrcItem->m_fMaxSpan;
		m_sAngleRange=pSrcItem->m_sAngleRange;
		m_fDesignKV=pSrcItem->m_fDesignKV;
		m_sRatedHeight=pSrcItem->m_sRatedHeight;
		m_sHeightRange=pSrcItem->m_sHeightRange;
		m_sTowerWeight=pSrcItem->m_sTowerWeight;
		m_fFrequencyRockAngle=pSrcItem->m_fFrequencyRockAngle;
		m_fLightningRockAngle=pSrcItem->m_fLightningRockAngle;
		m_fSwitchingRockAngle=pSrcItem->m_fSwitchingRockAngle;
		m_fWorkingRockAngle=pSrcItem->m_fWorkingRockAngle;
		m_sManuFacturer=pSrcItem->m_sManuFacturer;
		m_sMaterialCode=pSrcItem->m_sMaterialCode;
		m_sProModelCode=pSrcItem->m_sProModelCode;
	}
};
