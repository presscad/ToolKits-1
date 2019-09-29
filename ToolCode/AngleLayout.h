#pragma once
#include <WinDef.h>

//�Ǹ�֫�ڷŷ�ʽ
struct ANGLE_LAYOUT{
protected:
	BYTE _ciLayoutStyle;	//֫����1��4Ϊ������2��3Ϊ����
public:
	BYTE get_ciLayoutStyle(){return _ciLayoutStyle;}
	BYTE set_ciLayoutStyle(BYTE ciValue){
		if(ciValue=='X')
			_ciLayoutStyle=1;
		else if(ciValue=='y')
			_ciLayoutStyle=2;
		else if(ciValue=='x')
			_ciLayoutStyle=3;
		else if(ciValue=='Y')
			_ciLayoutStyle=4;
		else if(ciValue>=0&&ciValue<=4)
			_ciLayoutStyle=ciValue;
		else
			_ciLayoutStyle=0;
		return _ciLayoutStyle;
	}
	__declspec(property(put=set_ciLayoutStyle,get=get_ciLayoutStyle)) BYTE ciLayoutStyle;
	//MirLayStyle������
	static const BYTE MIRXorY_UNCHAHNGE		= 0;//X��Y��Գ�ʱ,����ԭ��������
	static const BYTE MIRXorY_SAMEDIRECT	= 1;//X��Y��Գ�ʱ,����֫����ͬ��
	static const BYTE MIRXorY_INVERT		= 2;//X��Y��Գ�ʱ,����֫���ⷴ��
	BYTE ciMirLayStyle;
	ANGLE_LAYOUT(BYTE _ciAngleLayout,BYTE _ciMirLayoutStyle){
		ciLayoutStyle=_ciAngleLayout;
		ciMirLayStyle=_ciMirLayoutStyle;
	}
	ANGLE_LAYOUT(WORD wLayoutFlag=0){memcpy(this,&wLayoutFlag,2);}
	operator WORD(){return *((WORD*)this);}
	WORD operator = (WORD wLayout){memcpy(this,&wLayout,2);return *this;}
public:
	char get_ciAngleWorkWing(){
		if(ciLayoutStyle==1)
			return 'X';
		else if(ciLayoutStyle==2)
			return 'y';
		else if(ciLayoutStyle==3)
			return 'x';
		else if(ciLayoutStyle==4)
			return 'Y';
		else
			return 0;
	}
	char set_ciAngleWorkWing(char ciWorkWing){
		if(ciWorkWing=='X')
			ciLayoutStyle=1;
		else if(ciWorkWing=='y')
			ciLayoutStyle=2;
		else if(ciWorkWing=='x')
			ciLayoutStyle=3;
		else if(ciWorkWing=='Y')
			ciLayoutStyle=4;
		else
			ciLayoutStyle=0;
		return ciWorkWing;
	}
	__declspec(property(put=set_ciAngleWorkWing,get=get_ciAngleWorkWing)) \
		char ciAngleWorkWing;	//'X','x','Y','y',��дΪ������СдΪ������Ӧ(�ǸֵĲ��÷�ʽ1,3,4,2);<=0��ʾ��ָ��֫�������������֫���߿�
	BYTE get_ciWorkWingX0Y1(){
		if(ciLayoutStyle==1||ciLayoutStyle==3)
			return 0;
		else //if(ciLayoutStyle==2||ciLayoutStyle==4)
			return 1;
	}
	__declspec(property(get=get_ciWorkWingX0Y1)) BYTE ciWorkWingX0Y1;	//����֫ΪX֫����0,Y֫����1
};
//�Ǹ�֫�ڷŷ�ʽ
struct ANGLE_WORKWING{
protected:
	//'X','x','Y','y',��дΪ������СдΪ������Ӧ(�ǸֵĲ��÷�ʽ1,3,4,2);<=0��ʾ��ָ��֫�������������֫���߿�
	char _ciAngleWorkWing;	
public:
	ANGLE_WORKWING(char _ciWorkWing=0){
		if(_ciWorkWing==1)
			_ciAngleWorkWing='X';
		else if(_ciWorkWing==2)
			_ciAngleWorkWing='y';
		else if(_ciWorkWing==3)
			_ciAngleWorkWing='x';
		else if(_ciWorkWing==4)
			_ciAngleWorkWing='Y';
		else
			_ciAngleWorkWing=_ciWorkWing;
	}
	operator char(){return _ciAngleWorkWing;}
public:	//����
	BYTE get_ciLayoutStyle(){
		if(_ciAngleWorkWing=='X')
			return 1;
		else if(_ciAngleWorkWing=='y')
			return 2;
		else if(_ciAngleWorkWing=='x')
			return 3;
		else if(_ciAngleWorkWing=='Y')
			return 4;
		else
			return 0;
	}
	BYTE set_ciLayoutStyle(BYTE ciValue){
		if(ciValue==1)
			_ciAngleWorkWing='X';
		else if(ciValue==2)
			_ciAngleWorkWing='y';
		else if(ciValue==3)
			_ciAngleWorkWing='x';
		else if(ciValue==4)
			_ciAngleWorkWing='Y';
		else
			_ciAngleWorkWing=0;
		return ciValue;
	}
	__declspec(property(put=set_ciLayoutStyle,get=get_ciLayoutStyle)) BYTE ciLayoutStyle;
};
