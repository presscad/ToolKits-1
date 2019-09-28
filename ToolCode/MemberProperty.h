#ifndef _CLASS_MEMBER_PROPERTIES_H
#define _CLASS_MEMBER_PROPERTIES_H

#define PROPERTY(t,n)  __declspec( property ( put = property__set_##n, get = property__get_##n ) ) t n;\
	typedef t property__tmp_type_##n
#define READONLY_PROPERTY(t,n) __declspec( property (get = property__get_##n) ) t n;\
	typedef t property__tmp_type_##n
#define WRITEONLY_PROPERTY(t,n) __declspec( property (put = property__set_##n) ) t n;\
	typedef t property__tmp_type_##n
#define ADD_READONLY_PROPERTY(t,n) __declspec( property (get = property__get_##n) ) t n;\
	typedef t property__tmp_type_##n;\
	property__tmp_type_##n property__get_##n()
#define ADD_WRITEONLY_PROPERTY(t,n) __declspec( property (put = property__set_##n) ) t n;\
	typedef t property__tmp_type_##n;\
	void property__set_##n(const property__tmp_type_##n& value)

#define DECLARE_PROPERTY(t,n) __declspec( property (put = property__set_##n, get = property__get_##n) ) t n;\
	typedef t property__tmp_type_##n;\
	property__tmp_type_##n property__get_##n();\
	void property__set_##n(const property__tmp_type_##n& value);
#define DECLARE_READONLY_PROPERTY(t,n) __declspec( property (get = property__get_##n) ) t n;\
	typedef t property__tmp_type_##n;\
	property__tmp_type_##n property__get_##n()

#define DECLARE_VIRTUAL_PROPERTY(t,n) __declspec( property (put = property__set_##n, get = property__get_##n) ) t n;\
	typedef t property__tmp_type_##n;\
	virtual property__tmp_type_##n property__get_##n();\
	virtual void property__set_##n(const property__tmp_type_##n& value);
#define DECLARE_READONLY_VIRTUAL_PROPERTY(t,n) __declspec( property (get = property__get_##n) ) t n;\
	typedef t property__tmp_type_##n;\
	virtual property__tmp_type_##n property__get_##n()

#define GET(n) property__tmp_type_##n property__get_##n()
#define SET(n) void property__set_##n(const property__tmp_type_##n& value)
#define SETEX(n) property__tmp_type_##n property__set_##n(const property__tmp_type_##n& value)
#define VIRTUAL_GET(n) virtual property__tmp_type_##n property__get_##n()
#define VIRTUAL_SET(n) virtual void property__set_##n(const property__tmp_type_##n& value)
#define IMPLEMENT_GET(classname,n) classname::property__tmp_type_##n classname::property__get_##n()
#define IMPLEMENT_SET(classname,n) void classname::property__set_##n(const property__tmp_type_##n& value)

//示例
class POINT_REF
{
	double* coord;
	bool m_bReadOnly;
public:
	POINT_REF(){
		coord=NULL;
	}
	POINT_REF(double* data,bool bReadOnly=false){
		coord=data;
		m_bReadOnly=bReadOnly;
	}
	operator double*(){
		return coord;
	};			//返回一个双精度指针
	////////////////////////////////////////////////////////////
	//对外属性
	PROPERTY(double, x);
	GET(x){
		if(coord!=NULL)
			return coord[0];
		else
			return 0;
	}
	SET(x){
		if(coord!=NULL&&!m_bReadOnly)
			coord[0]=value;
	}
	PROPERTY(double, y);
	GET(y){
		if(coord!=NULL)
			return coord[1];
		else
			return 0;
	}
	SET(y){
		if(coord!=NULL&&!m_bReadOnly)
			coord[1]=value;
	}
	PROPERTY(double, z);
	GET(z){
		if(coord!=NULL)
			return coord[2];
		else
			return 0;
	}
	SET(z){
		if(coord!=NULL&&!m_bReadOnly)
			coord[2]=value;
	}
};
class POINT_COPY
{
	double coord[3];
public:
	POINT_COPY(double* data){
		coord[0]=coord[1]=coord[2]=0;
		for(BYTE i=0;i<3;i++)
			coord[i]=data[i];
	}
	operator POINT_REF(){
		return POINT_REF(coord);
	}			//返回一个双精度指针
	PROPERTY(double, x);
	GET(x){
		return coord[0];}
	SET(x){
		coord[0]=value;}
	PROPERTY(double, y);
	GET(y){
		return coord[1];}
	SET(y){
		coord[1]=value;
	}
	PROPERTY(double, z);
	GET(z){
		return coord[2];
	}
	SET(z){
		coord[2]=value;
	}
};
/*int TestConstPtRef(POINT_REF ref,POINT_REF ref2)
{
	ref.x=ref2.x;
	return 0;
}
void TestFunc()
{
	GEPOINT pt(1,2,3),pt2(4,5,6),pt3;
	TestConstPtRef((double*)pt,POINT_REF(pt2));
	TestConstPtRef(POINT_REF(pt3,true),POINT_REF(pt2));
}*/

#endif /* _CLASS_MEMBER_PROPERTIES_H */ 

