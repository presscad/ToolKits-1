#ifndef __LIC_FUNCTION_DEFINED_
#define __LIC_FUNCTION_DEFINED_

#if defined(__TMA_FILE_) || defined(__ALL_LICFUNC_)
struct TMA_LICFUNC{
/*Serial:31*/static const char* FUNC_IDENTITY_BASIC;//="7F601F34C0F884947770541562990587";
/*Serial:32*/static const char* FUNC_IDENTITY_PPE_EDITOR;//="965BCAC3EC47E7423FA3A5721962D3D1";
/*Serial:21*/static const char* FUNC_IDENTITY_UBOM;//="70165D193E679090E1526D09E95CB19E";
/*Serial:27*/static const char* FUNC_IDENTITY_GRX_MODULE;//="6E4CF1392ED1E08635CF8D1E96699079";
/*Serial:29*/static const char* FUNC_IDENTITY_ZRX_MODULE;//="8D39F9F3C0381DB7DFE2898EE20347FF";
/*Serial:23*/static const char* FUNC_IDENTITY_STD_PANEL;//="16C074564221E85604A1372C4B9B0B15";
/*Serial: 5*/static const char* FUNC_IDENTITY_AUTO_ARCLIFT;//="EEF495D8D3EB82CDFAD302B2002F6CA3";
/*Serial:11*/static const char* FUNC_IDENTITY_SHADOW_OBJECT;//="D6C272A79F6D44A6053E30DC436B578C";
/*Serial:17*/static const char* FUNC_IDENTITY_BLOCK_ASSEMBLY;//="05F06A9CA18B117DFF796B79A359C9D5";
/*Serial: 4*/static const char* FUNC_IDENTITY_COPY_LEG;//="ECAECB3FA2D235756ECFE8D2E49EB8E0";
/*Serial:19*/static const char* FUNC_IDENTITY_PLATE_DRAWING_NC;//="656D37AEBC229927D612ED7458E1E219";
/*Serial: 2*/static const char* FUNC_IDENTITY_SIZE_CHECK;//="3EC500A97724AF04A42D14A3DDA9A1E7";
/*Serial:30*/static const char* FUNC_IDENTITY_INTERNAL_TEST;//="6F6BE6AE044F76A26607813A52CD5CD2";
/*Serial:22*/static const char* FUNC_IDENTITY_TIDMODEL_EXPORT;//="2E3A7EDCE4324C67EBBDFD62798CA011";
/*Serial: 3*/static const char* FUNC_IDENTITY_DWGSOLID_EXPORT;//="7F5218FC52556A252F11A15373CE8D1D";
};
#ifdef __TMA_FILE_
#define LICFUNC TMA_LICFUNC
#endif
#endif
#if defined(__LMA_FILE_) || defined(__ALL_LICFUNC_)
struct LMA_LICFUNC{
/*Serial:30*/static const char* FUNC_IDENTITY_BASIC;//="C277CD3B0549FDA54DC6352749245CD7";
/*Serial: 7*/static const char* FUNC_IDENTITY_PPE_EDITOR;//="D2FE7B2AC2CF7D7D418438CBDACBCA70";
/*Serial:16*/static const char* FUNC_IDENTITY_GRX_MODULE;//="17FD11C76FFED880F3ED33941853BA0A";
/*Serial:14*/static const char* FUNC_IDENTITY_ZRX_MODULE;//="8567C6501F0F6CDB3C903E9F767F9AE4";
/*Serial:20*/static const char* FUNC_IDENTITY_STD_PANEL;//="CF6994C7AD266788D6DAAB2FA61C25FE";
/*Serial:15*/static const char* FUNC_IDENTITY_AUTO_ARCLIFT;//="B7DCBC50C1F83F6AECB0160D80C85677";
/*Serial:12*/static const char* FUNC_IDENTITY_SHADOW_OBJECT;//="ED39F7EFB8061E0813A80485093AF958";
/*Serial:23*/static const char* FUNC_IDENTITY_BLOCK_ASSEMBLY;//="5E36D43223EF9E267BF0A10B0B452DFC";
/*Serial: 2*/static const char* FUNC_IDENTITY_COPY_LEG;	//="05E571E8861264D641D744BD8DAF5823";
/*Serial: 2*/static const char* FUNC_IDENTITY_UBOM;		//="05E571E8861264D641D744BD8DAF5823";
/*Serial:22*/static const char* FUNC_IDENTITY_COLUMN_RAYROD;//="77FD1DC07ACDAD025558D055EA87CD7D";
/*Serial:28*/static const char* FUNC_IDENTITY_SPHERE_DESIGN;//="E8CD7E4456A489A3726B6B76107C020A";
/*Serial:29*/static const char* FUNC_IDENTITY_INTERNAL_TEST;//="E8BD0445B4AC30AE58E53787D2DAA87D";
/*Serial:24*/static const char* FUNC_IDENTITY_TIDMODEL_EXPORT;//="4A5B8165BC9DECEAD15EEE1A76965579";
/*Serial:11*/static const char* FUNC_IDENTITY_DWGSOLID_EXPORT;//="EF886A8E206AC3F8690626C6EA7D2A65";
/*Serial:27*/static const char* FUNC_IDENTITY_SUBSTATION;//="05E571E8861264D641D744BD8DAF5823";
};
#ifdef __LMA_FILE_
#define LICFUNC LMA_LICFUNC
#endif
#endif
#if defined(__TSA_FILE_) || defined(__ALL_LICFUNC_)
struct TSA_LICFUNC{
/*Serial:29*/static const char* FUNC_IDENTITY_BASIC;//="C164E9BC6B42E6352BCFA73B74FA9D20";
/*Serial:24*/static const char* FUNC_IDENTITY_GRX_MODULE;//="A3F2FD30D7D2EE74E72DEBB2D80924FF";
/*Serial: 1*/static const char* FUNC_IDENTITY_ZRX_MODULE;//="59C188912A3BE06250750F22DCDC48E1";
/*Serial:21*/static const char* FUNC_IDENTITY_NONLINEAR_ANLYS;//="4BBC125B1B06526B5670724096B73FB2";
/*Serial:28*/static const char* FUNC_IDENTITY_DYNAMIC_ANLYS;//="ACB3B81D1502C12CAECD3FE5C47E5471";
/*Serial:19*/static const char* FUNC_IDENTITY_CABLE_ANLYS;//="32BE56060275C33A833A58609860A306";
};
#ifdef __TSA_FILE_
#define LICFUNC TSA_LICFUNC
#endif
#endif
#if defined(__LDS_FILE_) || defined(__LDS_) || defined(__ALL_LICFUNC_)
struct LDS_LICFUNC{
	static const char* FUNC_IDENTITY_BASIC;
	static const char* FUNC_IDENTITY_PPE_EDITOR;
	//static const char* FUNC_IDENTITY_ROOF_DESIGN="";
	static const char* FUNC_IDENTITY_GRX_MODULE;
	static const char* FUNC_IDENTITY_ZRX_MODULE;
	static const char* FUNC_IDENTITY_INTERNAL_TEST;
	static const char* FUNC_IDENTITY_AUTO_ARCLIFT;	//原智能绘图中自动预拱功能项;
	static const char* FUNC_IDENTITY_SMART_DRAWING;	//智能绘图取代原预拱功能项现等同于FUNC_IDENTITY_AUTO_ARCLIFT;
	static const char* FUNC_IDENTITY_SHADOW_OBJECT;
	static const char* FUNC_IDENTITY_BLOCK_ASSEMBLY;
	static const char* FUNC_IDENTITY_COLUMN_RAYROD;
	static const char* FUNC_IDENTITY_SPHERE_DESIGN;
	static const char* FUNC_IDENTITY_NONLINEAR_ANLYS;
	static const char* FUNC_IDENTITY_DYNAMIC_ANLYS;
	static const char* FUNC_IDENTITY_CABLE_ANLYS;
	static const char* FUNC_IDENTITY_TIDMODEL_EXPORT;		//导出生成TID模型文件
	static const char* FUNC_IDENTITY_DWGSOLID_EXPORT;
	static const char* FUNC_IDENTITY_PROCESS_EXPORT;
	static const char* FUNC_IDENTITY_3D_HANDOVER;			//三维移交
	static const char* FUNC_IDENTITY_3DMODEL_BATCH_EXPORT;	//批量导出三维模型
	static const char* FUNC_IDENTITY_TOWERTYPE_PLANNING;	//杆塔规划
	static const char* FUNC_IDENTITY_EXPERTS_KNOWLEGE;		//专家知识库(含典型节点库、挂点库及其它标准组件库)
};
#if defined(__LDS_FILE_)&&!defined(__LDS_FEATURE_ID_DISABLED_)
#define LICFUNC LDS_LICFUNC
#endif
#endif
#if defined(__TDA_FILE_) || defined(__ALL_LICFUNC_)
struct TDA_LICFUNC{
/*Serial:27*/static const char* FUNC_IDENTITY_BASIC;//="4178568EB88BA82649D78F40FF5BEAE3";
/*Serial:22*/static const char* FUNC_IDENTITY_PPE_EDITOR;//="801C62AEBE928317D97B11CF529717AE";
/*Serial:13*/static const char* FUNC_IDENTITY_ANGLE_DRAWING;//="17AC549A8327F40743E54B61290E9689";
/*Serial:17*/static const char* FUNC_IDENTITY_COMPLEX_DRAWING;//="FBCCDE285F75C3886D1AFDFD77EEAB29";
/*Serial:30*/static const char* FUNC_IDENTITY_GRX_MODULE;//="8807B865A3C308C91CAABACF4B9A7D48";
/*Serial:14*/static const char* FUNC_IDENTITY_ZRX_MODULE;//="669D0D08B9AF29D1C714E9C4425843C1";
/*Serial: 5*/static const char* FUNC_IDENTITY_AUTO_ARCLIFT;//="B5A1DBF8697D2CF71EBEEDAF5CB71912";
/*Serial: 2*/static const char* FUNC_IDENTITY_SHADOW_OBJECT;//="C882CADF44A2377951EDE1421E38A91D";
/*Serial:12*/static const char* FUNC_IDENTITY_BLOCK_ASSEMBLY;//="9509BAB837F0F796400612A74420D3B8";
/*Serial:23*/static const char* FUNC_IDENTITY_COLUMN_RAYROD;//="9858200836A82ED8AE1B941AE9FB8084";
/*Serial:16*/static const char* FUNC_IDENTITY_SPHERE_DESIGN;//="9271D2FDB0DBE3A5A9886A4317F87B18";
/*Serial:15*/static const char* FUNC_IDENTITY_NONLINEAR_ANLYS;//="F194A7F6A2CC7E0F7BA48D8A2A759645";
/*Serial:25*/static const char* FUNC_IDENTITY_DYNAMIC_ANLYS;//="1C86167382EBB467534944826DEF5594";
/*Serial: 3*/static const char* FUNC_IDENTITY_PROCESS_EXPORT;//="9ADD650EBF4CA0EDF14A5B5461375200";
/*Serial:31*/static const char* FUNC_IDENTITY_TIDMODEL_EXPORT;//="1EEC1994D967815AEDBA9220CF4A70F8";
/*Serial: 4*/static const char* FUNC_IDENTITY_DWGSOLID_EXPORT;//="412DE3DF48912664B6B5FF70BF35E71B";
/*Serial:11*/static const char* FUNC_IDENTITY_INTERNAL_TEST;//="59C368D2E6A5012C1BE5BF34ABCE8A94";
};
#ifdef __TDA_FILE_
#define LICFUNC TDA_LICFUNC
#endif
#endif
//ProductType:6
// 0:32
// 1: 9
// 2:21
// 3: 7
// 4:25
// 5:20
// 6:18
// 7:27
// 8:29
// 9:24
//10:30
//11:13
//12:17
//13: 5
//14: 2
//15:31
//16:14
//17:12
//18:28
//19:16
//20: 3
//21:15
//22: 4
//23:22
//24:26
//25: 1
//26:11
//27: 6
//28:23
//29:19
//30:10
//31: 8
//ProductType:7
#if defined(__PAILIAO_) || defined(__ALL_LICFUNC_)
struct PAILIAO_LICFUNC{
/*Serial: 1*/static const char* FUNC_IDENTITY_BASIC;//="6BD54D6F6A2B5D1ABC5AE537EB610EE2";
/*Serial:17*/static const char* FUNC_IDENTITY_LABELSAVING;//="75E98779D323097DDB55EC15F6DC80E6";
/*Serial:25*/static const char* FUNC_IDENTITY_INTERNAL_TEST;//="23B744D0D6766FBEBC8ACA2B4AD5EBD0";
// 3: 6
// 4:15
// 5:16
// 6:11
// 7:29
// 8: 8
// 9:10
//10:24
//11: 9
//12: 7
//13:28
//14:22
//15:27
//16:19
//17:31
//18:23
//19:21
//20:32
//21: 5
//22: 2
//23:18
//24:20
//25:13
//26:30
//27:26
//28: 3
//29:14
//30: 4
//31:12
};
#ifdef __PAILIAO_
#define LICFUNC PAILIAO_LICFUNC
#endif
#endif
#if defined(__PMS_) || defined(__ALL_LICFUNC_)
struct PMS_LICFUNC{
	static const char* FUNC_IDENTITY_BASIC;			//包装方案制作
	static const char* FUNC_IDENTITY_WHITEPART;		//白件分捡管理
	static const char* FUNC_IDENTITY_LIVEPARCEL;	//现场打包管理
	static const char* FUNC_IDENTITY_CONSIGNMENT;	//成品发货管理
};
#ifdef __PMS_
#define LICFUNC PMS_LICFUNC
#endif
#endif
//ProductType:9
#if defined(__MRP_) || defined(__ALL_LICFUNC_)
struct MRP_LICFUNC{	//试组装平台
/*0->Serial:23*/static const char* FUNC_IDENTITY_BASIC;	//="495A98ACB8E18E55F343988C11CB034B";
};
#endif
// 0:23
// 1:28
// 2:31
// 3:30
// 4: 2
// 5:29
// 6:27
// 7: 5
// 8:19
// 9:32
//10:22
//11: 3
//12:25
//13:13
//14: 1
//15: 4
//16:18
//17:24
//18:12
//19:21
//20:14
//21: 6
//22:20
//23:17
//24:11
//25:26
//26: 8
//27:10
//28:15
//29: 9
//30: 7
//31:16
#if defined(__TAP_) || defined(__ALL_LICFUNC_)
//ProductType:10
struct TAP_LICFUNC{	//试组装平台
/*0->Serial:32*/static const char* FUNC_IDENTITY_BASIC;	//="521F11746DD1BE876E9811D6649C4CAD";
/*1->Serial:14*/static const char* FUNC_IDENTITY_RETRIEVE_BOM;	//="521F11746DD1BE876E9811D6649C4CAD";
/*2->Serial: 8*/static const char* FUNC_IDENTITY_SIZE_CHECK;	//="521F11746DD1BE876E9811D6649C4CAD";
/*3->Serial:29*/static const char* FUNC_IDENTITY_PPE_EDITOR;	//="521F11746DD1BE876E9811D6649C4CAD";
/*4->Serial:11*/static const char* FUNC_IDENTITY_DATA_CHECK;	//="521F11746DD1BE876E9811D6649C4CAD";
/*5->Serial: 7*/static const char* FUNC_IDENTITY_ANGLE_CUT;		//="521F11746DD1BE876E9811D6649C4CAD";
/*6->Serial:10*/static const char* FUNC_IDENTITY_REPOSITION;	//="521F11746DD1BE876E9811D6649C4CAD";
};
// 7: 9
// 8:18
// 9:28
//10:22
//11: 2
//12:15
//13:17
//14: 5
//15:16
//16:31
//17:20
//18: 3
//19:21
//20:30
//21: 1
//22: 4
//23:24
//24:25
//25:23
//26:27
//27:13
//28: 6
//29:26
//30:19
//31:12
#ifdef __TAP_
#define LICFUNC TAP_LICFUNC
#endif
#endif
#if defined(__PNC_) || defined(__ALL_LICFUNC_)
//ProductType:12
struct PNC_LICFUNC{	//PNC
/*Serial:32*/static const char* FUNC_IDENTITY_BASIC;		//="7FA1A73DFAF0F6728DD0954F5E1E6355";
/*Serial: 3*/static const char* FUNC_IDENTITY_PPE_EDITOR;	//="E4083E129359BCE0246F1C428FB3A7EA"
/*Serial:15*/static const char* FUNC_IDENTITY_HOLE_ROUTER;	//="70E1449AB5C114230F843B4426EF444B"
/*Serial: 1*/static const char* FUNC_IDENTITY_CUT_FILE;		//="4CA1F35DE3AAC07EBCFBB0C53872DA54"
/*Serial:18*/static const char* FUNC_IDENTITY_PBJ_FILE;		//="DEEF0D0668CFDF74E2752FE4196796A1"
/*Serial:23*/static const char* FUNC_IDENTITY_INTERNAL_TEST;//="80A3D300E8BA74FF52A555BB9A1EB414";
/*Serial: 4*/static const char* FUNC_IDENTITY_UBOM;			//="3E4EBDD5DF4865CDE3310A6D57350B18";	//UBOM对料功能 wht 19-08-04
};
// 7:16
// 8:17
// 9:24
//10:27
//11:28
//12:25
//13: 6
//14:20
//15:13
//16:22
//17:12
//18:21
//19:30
//20: 8
//21:31
//22:11
//23: 7
//24:10
//25: 9
//26: 2
//27:14
//28:29
//29:26
//30:19
//31: 5
#ifdef __PNC_
#define LICFUNC PNC_LICFUNC
#endif
#endif
#if defined(__IBOM_) || defined(__ALL_LICFUNC_)
//ProductType:13
struct IBOM_LICFUNC{	//试组装平台
/*0->Serial: 7*/static const char* FUNC_IDENTITY_BASIC;	//="6AB2F26D8FCFFCE22EB1284EAB922DAD";
/*1->Serial: 2*/static const char* FUNC_IDENTITY_READ_IMG;//="4BBE674848F1EB246C31953060741DA6"
/*2->Serial:20*/static const char* FUNC_IDENTITY_READ_DWG;//="11BDB4418DD785F151EE54AD3E641DD5"
/*3->Serial:31*/static const char* FUNC_IDENTITY_CMP_BOM;//="D04FCFE52C2E00476965D49FF3A660CD"
/*4->Serial: 9*/static const char* FUNC_IDENTITY_INTERNAL_TEST;//="88631CB4C994AC451AE0B3D726A7A050"
};
// 5:10
// 6:11
// 7: 5
// 8:32
// 9: 3
//10: 1
//11:14
//12:21
//13:22
//14: 4
//15:25
//16:19
//17:27
//18:15
//19:18
//20:29
//21:16
//22: 6
//23:17
//24:28
//25:30
//26:26
//27:23
//28:13
//29:24
//30: 8
//31:12
#ifdef __IBOM_
#define LICFUNC IBOM_LICFUNC
#endif
#endif
///////////////////////////////////////////////////////////////
#if defined(__TMDH_) || defined(__ALL_LICFUNC_)
//ProductType:14
struct TMDH_LICFUNC{	//国网GIM批量打包
/*0->Serial:18*/static const char* FUNC_IDENTITY_BASIC;	//="D326BC52C79D940D5A1C4FE8F3F26160";
};
//ProductType PRODUCT_TMDH 14#
//15<->Serial  1's id
//17<->Serial  2's id
//16<->Serial  3's id
//27<->Serial  4's id
//24<->Serial  5's id
// 7<->Serial  6's id
//12<->Serial  7's id
// 6<->Serial  8's id
// 1<->Serial  9's id
//11<->Serial 10's id
//29<->Serial 11's id
//28<->Serial 12's id
// 8<->Serial 13's id
// 9<->Serial 14's id
// 4<->Serial 15's id
//10<->Serial 16's id
// 2<->Serial 17's id
// 0<->Serial 18's id
//26<->Serial 19's id
//22<->Serial 20's id
//31<->Serial 21's id
// 3<->Serial 22's id
//13<->Serial 23's id
//19<->Serial 24's id
//23<->Serial 25's id
//30<->Serial 26's id
//20<->Serial 27's id
//21<->Serial 28's id
//14<->Serial 29's id
// 5<->Serial 30's id
//25<->Serial 31's id
//18<->Serial 32's id
#ifdef __TMDH_
#define LICFUNC TMDH_LICFUNC
#endif
#endif
#if defined(__CNC_) || defined(__ALL_LICFUNC_)
//ProductType:15
struct CNC_LICFUNC{	//角钢工艺卡反提
//All 1 based index
/*1->Serial:12*/static const char* FUNC_IDENTITY_BASIC;	//="E6FB217465B12B903F92CB21BFF99000";
};
// 2->Serial    7's 
// 3->Serial   18's 
// 4->Serial   26's 
// 5->Serial   20's 
// 6->Serial   16's 
// 7->Serial    8's 
// 8->Serial   25's 
// 9->Serial   13's 
//10->Serial    1's 
//11->Serial   28's 
//12->Serial   30's 
//13->Serial    9's 
//14->Serial    2's 
//15->Serial   29's 
//16->Serial   23's 
//17->Serial   19's 
//18->Serial    3's 
//19->Serial   14's 
//20->Serial   10's 
//21->Serial   17's 
//22->Serial    4's 
//23->Serial   22's 
//24->Serial   27's 
//25->Serial   24's 
//26->Serial    5's 
//27->Serial   11's 
//28->Serial   21's 
//29->Serial   15's 
//30->Serial    6's 
//31->Serial   32's 
//32->Serial   31's 
#ifdef __CNC_
#define LICFUNC CNC_LICFUNC
#endif
#endif

#endif