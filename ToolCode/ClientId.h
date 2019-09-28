#pragma once
#ifndef __CLIENT_ID_H_
#define __CLIENT_ID_H_
#include "XhCharString.h"
#include "HashTable.h"

#define CLIENT_NanTong_HuaCan		2	//id=2.表示江苏南通华灿
#define CLIENT_QingDao_WuXiao		3	//id=3.青岛武晓
#define CLIENT_ChongQing_ShunTai	4	//id=4.重庆顺泰
#define CLIENT_QingDao_DongFang		5	//id=5.青岛东方
#define CLIENT_GuangZhou_ZengLi		6	//id=6.广州增立
#define CLIENT_AnHui_HongYuan		7	//id=7.安徽宏源
#define CLIENT_YunNan_JingYunXiang	8	//id=8.云南景云祥
#define CLIENT_ChongQing_JiangDian	9	//id=9.重庆江电
#define CLIENT_QingDao_HuiJinTong	10	//id=10.青岛汇金通
#define CLIENT_ChangChun_JuDeLong	11	//id=11.长春聚德隆
#define CLIENT_ShanXi_BaoJi			12	//id=12.宝鸡塔
#define CLIENT_YunNan_JianYuan		13	//id=13.云南建源
#define CLIENT_YunNan_DongDian		14	//id=14.云南东电
#define CLIENT_YunNan_ZhanYe		15	//id=15.云南展业
#define CLIENT_XiNanYuanPower		16	//id=16.西南电力设计院
#define CLIENT_AnHuiYuanPower		17	//id=17.安徽省电力设计院
struct XHCID{
	static const BYTE CID_Xerofox				=1;	//id=1.信狐天诚公司
	static const BYTE CID_NanTong_HuaCan		=2;	//id=2.表示江苏南通华灿
	static const BYTE CID_QingDao_WuXiao		=3;	//id=3.青岛武晓
	static const BYTE CID_ChongQing_ShunTai		=4;	//id=4.重庆顺泰
	static const BYTE CID_QingDao_DongFang		=5;	//id=5.青岛东方
	static const BYTE CID_GuangZhou_ZengLi		=6;	//id=6.广州增立
	static const BYTE CID_AnHui_HongYuan		=7;	//id=7.安徽宏源
	static const BYTE CID_YunNan_JingYunXiang	=8;	//id=8.云南景云祥
	static const BYTE CID_ChongQing_JiangDian	=9;	//id=9.重庆江电
	static const BYTE CID_QingDao_HuiJinTong	=10;//id=10.青岛汇金通
	static const BYTE CID_ChangChun_JuDeLong	=11;//id=11.长春聚德隆
	static const BYTE CID_ShanXi_BaoJi			=12;//id=12.宝鸡塔
	static const BYTE CID_YunNan_JianYuan		=13;//id=13.云南建源
	static const BYTE CID_YunNan_DongDian		=14;//id=14.云南东电
	static const BYTE CID_YunNan_ZhanYe			=15;//id=15.云南展业
	static const BYTE CID_XiNanYuanPower		=16;//id=16.西南电力设计院
	static const BYTE CID_AnHuiYuanPower		=17;//id=17.安徽省电力设计院
	CXhChar50 GetClientName(int clientId,char separator=0);
};
CXhChar50 GetClientNameById(int clientId,char separator=0);
#endif