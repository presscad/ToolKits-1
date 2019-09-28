#pragma once
#include <WinDef.h>

struct WIREPLACE_CODE{	//挂位编码
	static const char EARTH_WIRE	 = 'E';
	static const char CONDUCTOR_WIRE = 'C';
	static const char JUMPER_WIRE	 = 'J';
	union DATA{
		struct BYTE4{
			BYTE cbBasicFlag;
			BYTE cbPostCode;	//后缀，包括线缆方向X/Y，单/双/V串挂接及附加序号码
			WORD wGroupMask;	//挂点分组信息，0x01表示归属第1组;0x02表示归属第2组;0x03表示同时归属于第1、2组
		}bytes;
		WORD wDataCode;
		DWORD dwDataCode;
	}data;
public:
	WIREPLACE_CODE(DWORD dwWirePlace=0){data.dwDataCode=dwWirePlace;}
	operator DWORD(){return data.dwDataCode;}
	WORD get_wGroupMask(){return data.bytes.wGroupMask;}
	__declspec(property(get=get_wGroupMask)) WORD wGroupMask;
	//'E'or 1地线;'C'or 2导线;'J'or 3跳线;0.无挂线
	char get_ciWireType() const;
	char set_ciWireType(char ciType);
	__declspec(property(put=set_ciWireType,get=get_ciWireType)) char ciWireType;//'E'or 1地线;'C'or 2导线;'J'or 3跳线;0.无挂线
	//相序号，取值:0（地线）,1,2,3
	BYTE get_ciPhaseSerial() const;
	BYTE set_ciPhaseSerial(char ciPhaseSerial);
	__declspec(property(put=set_ciPhaseSerial,get=get_ciPhaseSerial)) BYTE ciPhaseSerial;//相序号，取值:0（地线）,1,2,3
	//回路序号,0（地线）,1,2,3,4,5,6,7
	BYTE get_ciCircuitSerial() const;
	BYTE set_ciCircuitSerial(char ciCircuitSerial);
	__declspec(property(put=set_ciCircuitSerial,get=get_ciCircuitSerial)) BYTE ciCircuitSerial;//回路序号,0（地线）,1,2,3,4,5,6,7
	//回路类型,false（交流）,true直流
	bool get_blCircuitDC() const;
	bool set_blCircuitDC(bool blCircuitDC);
	__declspec(property(put=set_blCircuitDC,get=get_blCircuitDC)) bool blCircuitDC;//是否为直流电
	//张力类型:1.悬垂;2.耐张;3.终端
	char get_ciTensionType() const;
	char set_ciTensionType(char _ciTensionType);
	__declspec(property(put=set_ciTensionType,get=get_ciTensionType)) char ciTensionType;//张力类型:1.悬垂;2.耐张;3.终端
	//线缆方向'X' or 'Y'
	char get_ciWireDirection() const;
	char set_ciWireDirection(char ciWireAxisDirection);
	__declspec(property(put=set_ciWireDirection,get=get_ciWireDirection)) char ciWireDirection;//线缆方向'X' or 'Y'
	//线缆在塔身上的挂接方式'S' or 'V',0表示普通挂接
	char get_ciHangingStyle() const;
	char set_ciHangingStyle(char _ciHangingStyle);
	__declspec(property(put=set_ciHangingStyle,get=get_ciHangingStyle)) char ciHangingStyle;//线缆在塔身上的挂接方式'S' or 'V',0表示普通挂接
	//挂点附加码，如"导1-V3"中V后面的'3'表示1号相序导线的V挂点中的第三个
	char get_ciPostCode() const;
	char set_ciPostCode(char _ciPostSerialCode);
	__declspec(property(put=set_ciPostCode,get=get_ciPostCode)) char ciPostCode;
	//挂点序号
	int get_serial() const;
	__declspec( property(get=get_serial)) int iSerial;
};
