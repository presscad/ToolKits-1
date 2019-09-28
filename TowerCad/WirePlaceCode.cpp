#include "stdafx.h"
#include "WirePlaceCode.h"

//挂位编码WIREPLACE_CODE类定义 based index is 0.
//bit scope( 0~ 1):cWireType表示导、地及跳线类型
//bit scope( 2~ 3):ciPhaseSerial表示（1~3相序号，交流电最多3相）
//bit scope( 4~ 6):ciCircuitSerial表示回路号（0~7回路，据调研，目前最多6回路）
//bit scope( 7~ 8):ciTensionType表示张力类型:（1.悬垂;2.耐张;3.终端）
//bit scope( 9~10):ciHangingStyle表示线缆在塔身上的挂接方式(最多可表达0~3四种)'S' or 'V',0表示普通挂接
//bit scope( 11~11):ciWireDirection表示线缆方向'X' or 'Y'
//bit scope( 12~14):ciPostCode表示线缆挂点附加码，如"导1-V3"中V后面的'3'表示1号相序导线的V挂点中的第三个
//bit scope( 15~15):blCircuitDC表示当前回路是否为直流(0表示False,1表示True)

//'E'or 1地线;'C'or 2导线;'J'or 3跳线;0.无挂线
char WIREPLACE_CODE::get_ciWireType() const
{
	BYTE ciTypeFlag=data.bytes.cbBasicFlag&0x03;
	if(ciTypeFlag==1)
		return EARTH_WIRE;//'E';
	else if(ciTypeFlag==2)
		return CONDUCTOR_WIRE;//'C';
	else if(ciTypeFlag==3)
		return JUMPER_WIRE;//'J';
	else
		return 0;
}
char WIREPLACE_CODE::set_ciWireType(char ciType)
{
	BYTE ciTypeFlag=0;
	if(ciType==EARTH_WIRE)//'E')
		ciTypeFlag=1;
	else if(ciType==CONDUCTOR_WIRE)//'C')
		ciTypeFlag=2;
	else if(ciType==JUMPER_WIRE)//'J')
		ciTypeFlag=3;
	else if(ciType<=3)
		ciTypeFlag=ciType;
	else 
		ciTypeFlag=0;
	data.bytes.cbBasicFlag&=0xfc;
	data.bytes.cbBasicFlag|=ciTypeFlag;
	return ciWireType;
}
//相序号，取值:0（地线）,1,2,3
BYTE WIREPLACE_CODE::get_ciPhaseSerial() const
{
	BYTE ciPhaseFlag=data.bytes.cbBasicFlag&0x0c;
	return ciPhaseFlag>>2;
}
BYTE WIREPLACE_CODE::set_ciPhaseSerial(char _ciPhaseSerial)
{
	BYTE ciPhaseFlag=_ciPhaseSerial&0x03;
	ciPhaseFlag<<=2;
	data.bytes.cbBasicFlag&=0xf3;
	data.bytes.cbBasicFlag|=ciPhaseFlag;
	return _ciPhaseSerial&0x03;
}
//回路号，取值:0（地线）,1,2,3
BYTE WIREPLACE_CODE::get_ciCircuitSerial() const
{
	BYTE ciCircuitFlag=data.bytes.cbBasicFlag&0x70;
	return ciCircuitFlag>>4;
}
BYTE WIREPLACE_CODE::set_ciCircuitSerial(char ciCircuitSerial)
{
	BYTE ciCircuitFlag=ciCircuitSerial&0x07;
	ciCircuitFlag<<=4;
	data.bytes.cbBasicFlag&=0x8f;
	data.bytes.cbBasicFlag|=ciCircuitFlag;
	return ciCircuitSerial&0x07;
}
//张力类型:1.悬垂;2.耐张;3.终端
char WIREPLACE_CODE::get_ciTensionType() const
{
	WORD wTensionType=data.wDataCode&0x0180;
	return wTensionType>>7;
}
char WIREPLACE_CODE::set_ciTensionType(char _ciTensionType)
{
	WORD wTensionType=_ciTensionType&0x03;
	wTensionType<<=7;
	data.wDataCode&=0xfe7f;
	data.wDataCode|=wTensionType;
	return _ciTensionType&0x03;
}
//线缆在塔身上的挂接方式'S' or 'V',0表示普通挂接
char WIREPLACE_CODE::get_ciHangingStyle() const
{
	if((data.bytes.cbPostCode&0x02)==0x02)
		return 'S';	//双挂
	else if((data.bytes.cbPostCode&0x04)==0x04)
		return 'V';	//V串挂
	else
		return 0;	//普通挂点
}
char WIREPLACE_CODE::set_ciHangingStyle(char _ciHangingStyle)
{
	if(_ciHangingStyle=='S'||_ciHangingStyle=='s')
	{
		data.bytes.cbPostCode|=0x02;	//添加S(双)挂信息位
		data.bytes.cbPostCode&=0xfb;	//去掉V串挂接信息位
	}
	else if(_ciHangingStyle=='V'||_ciHangingStyle=='v')
	{
		data.bytes.cbPostCode&=0xfd;	//去掉S(双)挂信息位
		data.bytes.cbPostCode|=0x04;	//添加V串挂接信息位
	}
	else
		data.bytes.cbPostCode&=0xf9;
	return ciHangingStyle;
}
//线缆方向'Q' or 'H'
char WIREPLACE_CODE::get_ciWireDirection() const
{
	if((data.bytes.cbPostCode&0x08)>0)
		return 'Q';
	else 
		return 'H';
}
char WIREPLACE_CODE::set_ciWireDirection(char ciWireAxisDirection)
{
	data.bytes.cbPostCode &= 0xf7;
	if(ciWireAxisDirection=='Q'||ciWireAxisDirection=='q')
		data.bytes.cbPostCode|=0x08;
	return ciWireDirection;
}
//挂点附加码，如"导1-V3"中V后面的'3'表示1号相序导线的V挂点中的第三个
char WIREPLACE_CODE::get_ciPostCode() const
{
	char ciPostCode=data.bytes.cbPostCode>>4;
	return ciPostCode&0x07;
}
char WIREPLACE_CODE::set_ciPostCode(char _ciPostSerialCode)
{
	_ciPostSerialCode&=0x07;
	_ciPostSerialCode<<=4;
	data.bytes.cbPostCode&=0x0f;
	data.bytes.cbPostCode|=_ciPostSerialCode;
	return _ciPostSerialCode;
}
bool WIREPLACE_CODE::get_blCircuitDC() const
{
	return (data.bytes.cbPostCode&0x80)>0;
}
bool WIREPLACE_CODE::set_blCircuitDC(bool blCircuitDC)
{
	if(blCircuitDC)
		data.bytes.cbPostCode|=0x80;
	else
		data.bytes.cbPostCode&=0x7f;
	return (data.bytes.cbPostCode&0x80)>0;
}
//挂点的序号，根据挂点所在回路及回路中的相序号
int WIREPLACE_CODE::get_serial() const
{
	if(ciCircuitSerial>0)
		return (ciCircuitSerial-1)*3+ciPhaseSerial;
	else
		return ciPhaseSerial;
}

