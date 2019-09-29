#include "stdafx.h"
#include "WirePlaceCode.h"

//��λ����WIREPLACE_CODE�ඨ�� based index is 0.
//bit scope( 0~ 1):cWireType��ʾ�����ؼ���������
//bit scope( 2~ 3):ciPhaseSerial��ʾ��1~3����ţ����������3�ࣩ
//bit scope( 4~ 6):ciCircuitSerial��ʾ��·�ţ�0~7��·���ݵ��У�Ŀǰ���6��·��
//bit scope( 7~ 8):ciTensionType��ʾ��������:��1.����;2.����;3.�նˣ�
//bit scope( 9~10):ciHangingStyle��ʾ�����������ϵĹҽӷ�ʽ(���ɱ��0~3����)'S' or 'V',0��ʾ��ͨ�ҽ�
//bit scope( 11~11):ciWireDirection��ʾ���·���'X' or 'Y'
//bit scope( 12~14):ciPostCode��ʾ���¹ҵ㸽���룬��"��1-V3"��V�����'3'��ʾ1�������ߵ�V�ҵ��еĵ�����
//bit scope( 15~15):blCircuitDC��ʾ��ǰ��·�Ƿ�Ϊֱ��(0��ʾFalse,1��ʾTrue)

//'E'or 1����;'C'or 2����;'J'or 3����;0.�޹���
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
//����ţ�ȡֵ:0�����ߣ�,1,2,3
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
//��·�ţ�ȡֵ:0�����ߣ�,1,2,3
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
//��������:1.����;2.����;3.�ն�
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
//�����������ϵĹҽӷ�ʽ'S' or 'V',0��ʾ��ͨ�ҽ�
char WIREPLACE_CODE::get_ciHangingStyle() const
{
	if((data.bytes.cbPostCode&0x02)==0x02)
		return 'S';	//˫��
	else if((data.bytes.cbPostCode&0x04)==0x04)
		return 'V';	//V����
	else
		return 0;	//��ͨ�ҵ�
}
char WIREPLACE_CODE::set_ciHangingStyle(char _ciHangingStyle)
{
	if(_ciHangingStyle=='S'||_ciHangingStyle=='s')
	{
		data.bytes.cbPostCode|=0x02;	//���S(˫)����Ϣλ
		data.bytes.cbPostCode&=0xfb;	//ȥ��V���ҽ���Ϣλ
	}
	else if(_ciHangingStyle=='V'||_ciHangingStyle=='v')
	{
		data.bytes.cbPostCode&=0xfd;	//ȥ��S(˫)����Ϣλ
		data.bytes.cbPostCode|=0x04;	//���V���ҽ���Ϣλ
	}
	else
		data.bytes.cbPostCode&=0xf9;
	return ciHangingStyle;
}
//���·���'X' or 'Y'
char WIREPLACE_CODE::get_ciWireDirection() const
{
	if((data.bytes.cbPostCode&0x08)>0)
		return 'X';
	else //if((data.bytes.cbPostCode&0x08)==0)
		return 'Y';
}
char WIREPLACE_CODE::set_ciWireDirection(char ciWireAxisDirection)
{
	if(ciWireAxisDirection=='X'||ciWireAxisDirection=='x')
		data.bytes.cbPostCode|=0x08;
	else //if(ciWireAxisDirection=='Y'||ciWireAxisDirection=='y')
		data.bytes.cbPostCode&=0xf7;
	return ciWireDirection;
}
//�ҵ㸽���룬��"��1-V3"��V�����'3'��ʾ1�������ߵ�V�ҵ��еĵ�����
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
//�ҵ����ţ����ݹҵ����ڻ�·����·�е������
int WIREPLACE_CODE::get_serial() const
{
	int nPhaseNum = blCircuitDC ? 2 : 3;
	if(ciCircuitSerial>0)
		return (ciCircuitSerial-1)*nPhaseNum+ciPhaseSerial;
	else
		return ciPhaseSerial;
}
int WIREPLACE_CODE::set_serial(int _iSerial)
{
	int nPhaseNum = blCircuitDC ? 2 : 3;
	if (ciWireType != 'E' && _iSerial > 0)
	{
		_iSerial -= 1;
		ciCircuitSerial = 1 + (_iSerial / nPhaseNum);
		ciPhaseSerial = 1 + _iSerial % nPhaseNum;
	}
	else
		ciPhaseSerial = _iSerial;
	return ciPhaseSerial;
}

