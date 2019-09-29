#pragma once
#include <WinDef.h>

//角钢肢摆放方式
struct ANGLE_LAYOUT{
protected:
	BYTE _ciLayoutStyle;	//肢朝向1、4为外铁，2、3为里铁
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
	//MirLayStyle的类型
	static const BYTE MIRXorY_UNCHAHNGE		= 0;//X或Y轴对称时,保留原有里外铁
	static const BYTE MIRXorY_SAMEDIRECT	= 1;//X或Y轴对称时,工作肢里外同向
	static const BYTE MIRXorY_INVERT		= 2;//X或Y轴对称时,工作肢里外反向
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
		char ciAngleWorkWing;	//'X','x','Y','y',大写为外铁，小写为里铁对应(角钢的布置方式1,3,4,2);<=0表示不指定肢朝向，相对于贴合肢法线看
	BYTE get_ciWorkWingX0Y1(){
		if(ciLayoutStyle==1||ciLayoutStyle==3)
			return 0;
		else //if(ciLayoutStyle==2||ciLayoutStyle==4)
			return 1;
	}
	__declspec(property(get=get_ciWorkWingX0Y1)) BYTE ciWorkWingX0Y1;	//工作肢为X肢返回0,Y肢返回1
};
//角钢肢摆放方式
struct ANGLE_WORKWING{
protected:
	//'X','x','Y','y',大写为外铁，小写为里铁对应(角钢的布置方式1,3,4,2);<=0表示不指定肢朝向，相对于贴合肢法线看
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
public:	//属性
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
