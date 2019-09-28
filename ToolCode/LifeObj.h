#ifndef __LIFE_OBJECT_H_
#define __LIFE_OBJECT_H_
#include <stdio.h>
struct ILife{
	virtual void RegisterBirthInfo(ILife* pParent)=0;
	virtual void RegisterChild(ILife* pChild)=0;
	virtual void NotifyToParent(DWORD dwMessage)=0;
	virtual void NotifyToChild(DWORD dwMessage)=0;
};
template<class TYPE> class CSelfDestoryObject{
	TYPE *m_pDestroyObj,**m_ppDestoryObjPtr;
	typedef bool (TYPE::*DESTORY_FUNC_PTR)();
	DESTORY_FUNC_PTR _destroy_func_of_type;
	bool _destroy_has_self_delete;
public:
	//self_destroy_func为指向TYPE类型的自销毁函数
	//destroy_has_self_delete为self_destroy_func中是否包含了自身对象的delete操作
	CSelfDestoryObject(TYPE*& pLifeObj,DESTORY_FUNC_PTR self_destroy_func=NULL,bool destroy_has_self_delete=true){
		_destroy_func_of_type=self_destroy_func;
		_destroy_has_self_delete=_destroy_func_of_type!=NULL?destroy_has_self_delete:false;
		m_pDestroyObj=pLifeObj;
		m_ppDestoryObjPtr=&pLifeObj;
	}
	~CSelfDestoryObject(){
		if(_destroy_func_of_type)
			(m_pDestroyObj->*_destroy_func_of_type)();
		if(!_destroy_has_self_delete)
			delete m_pDestroyObj;
		if(m_ppDestoryObjPtr)
			*m_ppDestoryObjPtr=NULL;
	}
};
struct ITAG{
	virtual int TagType()=0;
	virtual void DetachObject()=0;
};
struct TAG{
	ITAG* pTagObj;
	union{	//简单附加数，避免大量定义ITAG类型
		DWORD dwParam;
		long  lParam;
		void* pParam;
	};
	TAG(){lParam=0;pTagObj=NULL;}
	~TAG(){
		if(pTagObj)
			pTagObj->DetachObject();
	};
};
template<class TYPE> class TAG_INFO : public ITAG {
protected:
	int m_iTagType;
	TYPE* m_pAttachObj;
public:
	TAG_INFO() { m_pAttachObj = NULL; m_iTagType = 0; }
	virtual ~TAG_INFO() { DetachObject(); }
	virtual int TagType() { return m_iTagType; }
	virtual void AttachObject(TYPE* pObj) {
		m_pAttachObj = pObj;
		pObj->SetTagInfo(this);
	}
	virtual void DetachObject() {
		if (m_pAttachObj)
			m_pAttachObj->SetTagInfo(NULL);
		m_pAttachObj = NULL;
	}
};
class CSelfCloseFile {
protected:
	FILE* m_fp;
public:
	CSelfCloseFile(FILE* fp=NULL) { m_fp = fp;}
	virtual ~CSelfCloseFile() {
		if(m_fp!=NULL)
			fclose(m_fp); 
	}
	virtual void EarlyCloseFile() {
		if (m_fp != NULL)
			fclose(m_fp);
		m_fp = NULL;
	}
	virtual void DetachFile() {
		m_fp = NULL;
	}
};
class CDepthCounter{
	int* piVisit;
public:
	int AttachCounter(void* piCounter){
		piVisit=(int*)piCounter;
		if(piVisit)
			(*piVisit)++;
		return piVisit!=NULL? *piVisit : 0;
	}
	CDepthCounter(void* piCounter)
	{
		AttachCounter(piCounter);
	}
	~CDepthCounter(){
		if(piVisit)
			(*piVisit)--;
	}
	int VisitCounts() const{return *piVisit;}
	bool ManualUnlock(){
		piVisit=NULL;
		return true;
	};
};
template <class TYPE> class CStackVariant{
	bool m_bEnabled;
	TYPE* m_pxVariant,m_xOldValue;
public:
	CStackVariant(TYPE* pVariant,TYPE newValue) {
		m_xOldValue = *pVariant;
		m_pxVariant = pVariant;
		*pVariant = newValue;
		m_bEnabled=true;
	}
	void Enable(bool enabled=true){m_bEnabled=enabled;}
	bool PopStack() {
		if (m_pxVariant&&m_bEnabled)
			*m_pxVariant = m_xOldValue;
		else
			return false;
		m_pxVariant = NULL;
		return true;
	}
	~CStackVariant() { PopStack(); }
};
#endif