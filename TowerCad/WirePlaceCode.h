#pragma once
#include <WinDef.h>

struct WIREPLACE_CODE{	//��λ����
	static const char EARTH_WIRE	 = 'E';
	static const char CONDUCTOR_WIRE = 'C';
	static const char JUMPER_WIRE	 = 'J';
	union DATA{
		struct BYTE4{
			BYTE cbBasicFlag;
			BYTE cbPostCode;	//��׺���������·���X/Y����/˫/V���ҽӼ����������
			WORD wGroupMask;	//�ҵ������Ϣ��0x01��ʾ������1��;0x02��ʾ������2��;0x03��ʾͬʱ�����ڵ�1��2��
		}bytes;
		WORD wDataCode;
		DWORD dwDataCode;
	}data;
public:
	WIREPLACE_CODE(DWORD dwWirePlace=0){data.dwDataCode=dwWirePlace;}
	operator DWORD(){return data.dwDataCode;}
	WORD get_wGroupMask(){return data.bytes.wGroupMask;}
	__declspec(property(get=get_wGroupMask)) WORD wGroupMask;
	//'E'or 1����;'C'or 2����;'J'or 3����;0.�޹���
	char get_ciWireType() const;
	char set_ciWireType(char ciType);
	__declspec(property(put=set_ciWireType,get=get_ciWireType)) char ciWireType;//'E'or 1����;'C'or 2����;'J'or 3����;0.�޹���
	//����ţ�ȡֵ:0�����ߣ�,1,2,3
	BYTE get_ciPhaseSerial() const;
	BYTE set_ciPhaseSerial(char ciPhaseSerial);
	__declspec(property(put=set_ciPhaseSerial,get=get_ciPhaseSerial)) BYTE ciPhaseSerial;//����ţ�ȡֵ:0�����ߣ�,1,2,3
	//��·���,0�����ߣ�,1,2,3,4,5,6,7
	BYTE get_ciCircuitSerial() const;
	BYTE set_ciCircuitSerial(char ciCircuitSerial);
	__declspec(property(put=set_ciCircuitSerial,get=get_ciCircuitSerial)) BYTE ciCircuitSerial;//��·���,0�����ߣ�,1,2,3,4,5,6,7
	//��·����,false��������,trueֱ��
	bool get_blCircuitDC() const;
	bool set_blCircuitDC(bool blCircuitDC);
	__declspec(property(put=set_blCircuitDC,get=get_blCircuitDC)) bool blCircuitDC;//�Ƿ�Ϊֱ����
	//��������:1.����;2.����;3.�ն�
	char get_ciTensionType() const;
	char set_ciTensionType(char _ciTensionType);
	__declspec(property(put=set_ciTensionType,get=get_ciTensionType)) char ciTensionType;//��������:1.����;2.����;3.�ն�
	//���·���'X' or 'Y'
	char get_ciWireDirection() const;
	char set_ciWireDirection(char ciWireAxisDirection);
	__declspec(property(put=set_ciWireDirection,get=get_ciWireDirection)) char ciWireDirection;//���·���'X' or 'Y'
	//�����������ϵĹҽӷ�ʽ'S' or 'V',0��ʾ��ͨ�ҽ�
	char get_ciHangingStyle() const;
	char set_ciHangingStyle(char _ciHangingStyle);
	__declspec(property(put=set_ciHangingStyle,get=get_ciHangingStyle)) char ciHangingStyle;//�����������ϵĹҽӷ�ʽ'S' or 'V',0��ʾ��ͨ�ҽ�
	//�ҵ㸽���룬��"��1-V3"��V�����'3'��ʾ1�������ߵ�V�ҵ��еĵ�����
	char get_ciPostCode() const;
	char set_ciPostCode(char _ciPostSerialCode);
	__declspec(property(put=set_ciPostCode,get=get_ciPostCode)) char ciPostCode;
	//�ҵ����
	int get_serial() const;
	int set_serial(int _iSerial);
	__declspec( property(put=set_serial,get=get_serial)) int iSerial;
};
