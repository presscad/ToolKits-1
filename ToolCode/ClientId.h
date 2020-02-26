#pragma once
#ifndef __CLIENT_ID_H_
#define __CLIENT_ID_H_
#include "XhCharString.h"
#include "HashTable.h"

#define CLIENT_NanTong_HuaCan		2	//id=2.��ʾ������ͨ����
#define CLIENT_QingDao_WuXiao		3	//id=3.�ൺ����
#define CLIENT_ChongQing_ShunTai	4	//id=4.����˳̩
#define CLIENT_QingDao_DongFang		5	//id=5.�ൺ����
#define CLIENT_GuangZhou_ZengLi		6	//id=6.��������
#define CLIENT_AnHui_HongYuan		7	//id=7.���պ�Դ
#define CLIENT_YunNan_JingYunXiang	8	//id=8.���Ͼ�����
#define CLIENT_ChongQing_JiangDian	9	//id=9.���콭��
#define CLIENT_QingDao_HuiJinTong	10	//id=10.�ൺ���ͨ
#define CLIENT_ChangChun_JuDeLong	11	//id=11.�����۵�¡
#define CLIENT_ShanXi_BaoJi			12	//id=12.������
#define CLIENT_YunNan_JianYuan		13	//id=13.���Ͻ�Դ
#define CLIENT_YunNan_DongDian		14	//id=14.���϶���
#define CLIENT_YunNan_ZhanYe		15	//id=15.����չҵ
#define CLIENT_XiNanYuanPower		16	//id=16.���ϵ������Ժ
#define CLIENT_AnHuiYuanPower		17	//id=17.����ʡ�������Ժ
struct XHCID{
	static const BYTE CID_Xerofox				=1;	//id=1.�ź���Ϲ�˾
	static const BYTE CID_NanTong_HuaCan		=2;	//id=2.��ʾ������ͨ����
	static const BYTE CID_QingDao_WuXiao		=3;	//id=3.�ൺ����
	static const BYTE CID_ChongQing_ShunTai		=4;	//id=4.����˳̩
	static const BYTE CID_QingDao_DongFang		=5;	//id=5.�ൺ����
	static const BYTE CID_GuangZhou_ZengLi		=6;	//id=6.��������
	static const BYTE CID_AnHui_HongYuan		=7;	//id=7.���պ�Դ
	static const BYTE CID_YunNan_JingYunXiang	=8;	//id=8.���Ͼ�����
	static const BYTE CID_ChongQing_JiangDian	=9;	//id=9.���콭��
	static const BYTE CID_QingDao_HuiJinTong	=10;//id=10.�ൺ���ͨ
	static const BYTE CID_ChangChun_JuDeLong	=11;//id=11.�����۵�¡
	static const BYTE CID_ShanXi_BaoJi			=12;//id=12.������
	static const BYTE CID_YunNan_JianYuan		=13;//id=13.���Ͻ�Դ
	static const BYTE CID_YunNan_DongDian		=14;//id=14.���϶���
	static const BYTE CID_YunNan_ZhanYe			=15;//id=15.����չҵ
	static const BYTE CID_XiNanYuanPower		=16;//id=16.���ϵ������Ժ
	static const BYTE CID_AnHuiYuanPower		=17;//id=17.����ʡ�������Ժ
	static const BYTE CID_ZheJiangShengDa		=18;//id=18.�㽭ʢ��������
	CXhChar50 GetClientName(int clientId,char separator=0);
};
CXhChar50 GetClientNameById(int clientId,char separator=0);
#endif