#include "StdAfx.h"
#include "ClientId.h"

CXhChar50 GetClientNameById(int clientId,char separator/*=0*/)
{
	CXhChar50 szName;
	if(clientId==CLIENT_NanTong_HuaCan)
		szName.Copy("��ͨ����");
	else if(clientId==CLIENT_QingDao_WuXiao)
		szName.Copy("�ൺ����");
	else if(clientId==CLIENT_ChongQing_ShunTai)
		szName.Copy("����˳̩");
	else if(clientId==CLIENT_QingDao_DongFang)
		szName.Copy("�ൺ����");
	else if(clientId==CLIENT_GuangZhou_ZengLi)
		szName.Copy("��������");
	else if(clientId==CLIENT_AnHui_HongYuan)
		szName.Copy("���պ�Դ");
	else if(clientId==CLIENT_YunNan_JingYunXiang)
		szName.Copy("���Ͼ�����");
	else if(clientId==CLIENT_ChongQing_JiangDian)
		szName.Copy("���콭��");
	else if(clientId==CLIENT_QingDao_HuiJinTong)
		szName.Copy("�ൺ���ͨ");
	else if(clientId==CLIENT_ChangChun_JuDeLong)
		szName.Copy("�����۵�¡");
	else if(clientId==CLIENT_ShanXi_BaoJi)
		szName.Copy("��������");
	else if(clientId==CLIENT_YunNan_JianYuan)
		szName.Copy("���Ͻ�Դ");
	else if(clientId==CLIENT_YunNan_DongDian)
		szName.Copy("���϶���");
	else if(clientId==CLIENT_YunNan_ZhanYe)
		szName.Copy("����չҵ");
	else if (clientId == XHCID::CID_XiNanYuanPower)
		szName.Copy("���ϵ������Ժ");
	else if (clientId == XHCID::CID_AnHuiYuanPower)
		szName.Copy("����ʡ�������Ժ");
	else if (clientId == XHCID::CID_ZheJiangShengDa)
		szName.Copy("�㽭ʢ��������");
	if(szName.GetLength()>0&&separator!=0)
		szName.Append(separator);
	return szName;
}
CXhChar50 XHCID::GetClientName(int clientId,char separator/*=0*/)
{
	return ::GetClientNameById(clientId,separator);
}
