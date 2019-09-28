#include "StdAfx.h"
#include "ClientId.h"

CXhChar50 GetClientNameById(int clientId,char separator/*=0*/)
{
	CXhChar50 szName;
	if(clientId==CLIENT_NanTong_HuaCan)
		szName.Copy("南通华灿");
	else if(clientId==CLIENT_QingDao_WuXiao)
		szName.Copy("青岛武晓");
	else if(clientId==CLIENT_ChongQing_ShunTai)
		szName.Copy("重庆顺泰");
	else if(clientId==CLIENT_QingDao_DongFang)
		szName.Copy("青岛东方");
	else if(clientId==CLIENT_GuangZhou_ZengLi)
		szName.Copy("广州增立");
	else if(clientId==CLIENT_AnHui_HongYuan)
		szName.Copy("安徽宏源");
	else if(clientId==CLIENT_YunNan_JingYunXiang)
		szName.Copy("云南景云祥");
	else if(clientId==CLIENT_ChongQing_JiangDian)
		szName.Copy("重庆江电");
	else if(clientId==CLIENT_QingDao_HuiJinTong)
		szName.Copy("青岛汇金通");
	else if(clientId==CLIENT_ChangChun_JuDeLong)
		szName.Copy("长春聚德隆");
	else if(clientId==CLIENT_ShanXi_BaoJi)
		szName.Copy("宝鸡塔厂");
	else if(clientId==CLIENT_YunNan_JianYuan)
		szName.Copy("云南建源");
	else if(clientId==CLIENT_YunNan_DongDian)
		szName.Copy("云南东电");
	else if(clientId==CLIENT_YunNan_ZhanYe)
		szName.Copy("云南展业");
	else if (clientId == XHCID::CID_XiNanYuanPower)
		szName.Copy("西南电力设计院");
	else if (clientId == XHCID::CID_AnHuiYuanPower)
		szName.Copy("安徽省电力设计院");
	if(szName.GetLength()>0&&separator!=0)
		szName.Append(separator);
	return szName;
}
CXhChar50 XHCID::GetClientName(int clientId,char separator/*=0*/)
{
	return ::GetClientNameById(clientId,separator);
}
