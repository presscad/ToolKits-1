// Expression.h: interface for the CExpression class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct EXPRESSION_VALUE
{
	//表达式计算结果值类型
	const static BYTE VALUETYPE_DOUBLE	= 0;
	const static BYTE VALUETYPE_LONG	= 1;
	const static BYTE VALUETYPE_BOOL	= 2;
	const static BYTE VALUETYPE_ARRAY	= 3;
	const static BYTE VALUETYPE_OBJECT	= 4;
	const static BYTE VALUETYPE_OBJPROP = 5;
	const static BYTE VALUETYPE_STRING  = 6;
	BYTE ciValType;	//0.double;1.integer;2.bool逻辑真假;3.数组;4.数据对象;5.数据对象的属性值
	union{
		double fValue;
		long liValue;
		bool blValue;
		void* pObj;
		double coord3d[3];
		struct{
			double origin3d[3];	//坐标系原点
			double axisx3d[3],axisy3d[3],axisz3d[3];	//坐标系的坐标轴方向
		};
		char szStrBuf[96];
		struct{	//数组变量
			UINT uiUpperBound;	//数组变量的索引上限(最大值, 0 based index)
			double* arrValues;	//数组变量时，指向数组的索引（可能指向临时内存）
		};
	};
	const static BYTE OBJTYPE_COORD3D = 1;
	const static BYTE OBJTYPE_ACS3D	  = 2;
	struct OBJTYPE{
		BYTE ciObjType;	//
		WORD wiPropValue;	//数据对象的子属性标识
	}objtype;
	double get_lfSafeVal(){
		if(ciValType==0)
			return fValue;
		else if(ciValType==1)
			return liValue;
		else if(ciValType==2)
			return blValue?1.0:0;
		else
			return 0;
	}
	long get_liSafeVal(){
		if(ciValType==0)
		{
			if(fValue>=0)
				return (long)(fValue+0.5);
			else
				return (long)(fValue-0.5);
		}
		else if(ciValType==1)
			return liValue;
		else if(ciValType==2)
			return blValue?1:0;
		else
			return 0;
	}
	bool get_blSafeVal(){
		if(ciValType==0)
			return fValue!=0;
		else if(ciValType==1)
			return liValue!=0;
		else if(ciValType==2)
			return blValue;
		else
			return false;
	}
	__declspec(property(get=get_lfSafeVal)) double lfSafeVal;	//获取不受ciValType类型影响的安全结果值
	__declspec(property(get=get_liSafeVal)) long   liSafeVal;	//获取不受ciValType类型影响的安全结果值
	__declspec(property(get=get_blSafeVal)) bool   blSafeVal;	//获取不受ciValType类型影响的安全结果值
	EXPRESSION_VALUE(){
		ciValType=0;
		coord3d[0]=coord3d[1]=coord3d[2]=0;
		memset(szStrBuf,0,96);
		objtype.wiPropValue=objtype.ciObjType=0;
	}
};

