#pragma once
#include "objptr_list.h"
#include "PropertyList.h"

//////////////////////////////////////////////////////////////////////////
// 属性栏操作封装类
template <class TYPE,class TYPE2> class CPropListOper
{
	CPropertyList *m_pPropList;
	TYPE *m_pObj;
	TYPE2* m_pSelectObjs;
	DWORD m_dwPreferPropGroup;
public:
	CPropertyList *GetPropList(){return m_pPropList;}
	CPropListOper(CPropertyList *pPropList,TYPE *pObj,TYPE2 *pSelectObjs=NULL)
	{
		m_pPropList=pPropList;
		m_pObj=pObj;
		m_pSelectObjs=pSelectObjs;
		m_dwPreferPropGroup=0;
	}
	~CPropListOper(void){;}
	void SetPreferPropGroup(DWORD dwPreferBelongGroup=0){m_dwPreferPropGroup=dwPreferBelongGroup;}

	CPropTreeItem *InsertPropItem(CPropTreeItem *pParentItem,char* propKey,
		CString sPropName="",CString sPropHelp="",int ctrlType=PIT_STATIC,
		int subType=0,CString sItemAttachValue="",int iInsertIndex=-1,BOOL bUpdate=FALSE)
	{
		DWORD dwCurrPropGroup=m_dwPreferPropGroup!=0?m_dwPreferPropGroup:pParentItem->m_dwPropGroup;
		long id=TYPE::GetPropID(propKey);
		if((dwCurrPropGroup&GetBinaryWord(m_pPropList->m_iPropGroup))==0)
			bUpdate=FALSE;
		PROPLIST_ITEM *pItem=TYPE::GetPropItem(id);
		if(pItem==NULL)
			return NULL;
		if(sPropName.GetLength()<=0)
			sPropName=pItem->name;
		if(sPropHelp.GetLength()<=0)
			sPropHelp=pItem->propHelpStr;
		if(sPropHelp.GetLength()<=0)
			sPropHelp=pItem->name;
		if((ctrlType==PIT_COMBO||ctrlType==PIT_BUTTON||ctrlType==PIT_GRAYBUTTON)&&sItemAttachValue.GetLength()<=0)
			sItemAttachValue=pItem->propCmbItemsStr;
		//
		CXhChar500 valueText;
		CPropTreeItem* pPropItem=m_pPropList->FindItemByPropId(pItem->id,NULL);
		if(pPropItem==NULL)
		{	//新插入项
			CItemInfo* lpInfo = new CItemInfo();
			lpInfo->m_controlType=ctrlType;
			lpInfo->m_strPropName=sPropName;
			lpInfo->m_strPropHelp=sPropHelp;
			if(lpInfo->m_controlType==PIT_BUTTON||lpInfo->m_controlType==PIT_GRAYBUTTON)
			{
				lpInfo->m_buttonType=subType;
				if(subType==BDT_POPMENU||subType==BDT_POPMENUEDIT)
					lpInfo->m_cmbItems=sItemAttachValue;
				else if(sItemAttachValue.GetLength()>0)
					lpInfo->m_sButtonName=sItemAttachValue;
			}
			else if(lpInfo->m_controlType==PIT_COMBO)
			{
				lpInfo->m_cmbType=subType;
				lpInfo->m_cmbItems = sItemAttachValue;
			}
			if(m_pObj->GetPropValueStr(id,valueText,valueText.GetLengthMax())>0)
				lpInfo->m_strPropValue = valueText;
			//插入属性项
			pPropItem = m_pPropList->InsertItem(pParentItem,lpInfo,iInsertIndex,bUpdate);
			pPropItem->m_idProp = id;
			pPropItem->m_dwPropGroup=dwCurrPropGroup;
		}
		else
		{	//更新已有项
			pPropItem->m_lpNodeInfo->m_strPropName=sPropName;
			pPropItem->m_lpNodeInfo->m_strPropHelp=sPropHelp;
			if (ctrlType == PIT_COMBO && sItemAttachValue.GetLength() > 0)
			{
				pPropItem->m_lpNodeInfo->m_cmbType=subType;
				pPropItem->m_lpNodeInfo->m_cmbItems = sItemAttachValue;
			}
			if(m_pObj->GetPropValueStr(id,valueText,valueText.GetLengthMax())>0)
				pPropItem->m_lpNodeInfo->m_strPropValue=valueText;
			if(bUpdate&&!pPropItem->IsHideSelf())
			{
				RECT rc;
				m_pPropList->GetItemRect(pPropItem->m_iIndex,&rc);
				m_pPropList->InvalidateRect(&rc);
			}
		}
		if(m_pSelectObjs)
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp=!TYPE::IsPropEqual(*m_pSelectObjs,id);
		return pPropItem;
	}
	//下拉框
	CPropTreeItem *InsertCmbListPropItem(CPropTreeItem *pParentItem,char* propKey,
		CString sItemAttachValue="",CString sPropName="",CString sPropHelp="",int iInsertIndex=-1,BOOL bUpdate=FALSE)
	{
		return InsertPropItem(pParentItem,propKey,sPropName,sPropHelp,PIT_COMBO,CDT_LIST,sItemAttachValue,iInsertIndex,bUpdate);
	}
	CPropTreeItem *InsertCmbEditPropItem(CPropTreeItem *pParentItem,char* propKey,
		CString sItemAttachValue="",CString sPropName="",CString sPropHelp="",int iInsertIndex=-1,BOOL bUpdate=FALSE)
	{
		return InsertPropItem(pParentItem,propKey,sPropName,sPropHelp,PIT_COMBO,CDT_EDIT,sItemAttachValue,iInsertIndex,bUpdate);
	}
	CPropTreeItem* InsertCmbColorPropItem(CPropTreeItem *pParentItem,char* propKey,
		CString sItemAttachValue="",CString sPropName="",CString sPropHelp="",int iInsertIndex=-1,BOOL bUpdate=FALSE)
	{
		return InsertPropItem(pParentItem,propKey,sPropName,sPropHelp,PIT_COMBO,CDT_COLOR,sItemAttachValue,iInsertIndex,bUpdate);
	}
	//编辑框
	CPropTreeItem *InsertEditPropItem(CPropTreeItem *pParentItem,char* propKey,CString sPropName="",
										CString sPropHelp="",int iInsertIndex=-1,BOOL bUpdate=FALSE)
	{
		return InsertPropItem(pParentItem,propKey,sPropName,sPropHelp,PIT_EDIT,0,"",iInsertIndex,bUpdate);
	}
	//按钮
	CPropTreeItem *InsertFilePathPropItem(CPropTreeItem *pParentItem,char* propKey,CString sPropName="",
										CString sPropHelp="",int iInsertIndex=-1,BOOL bUpdate=FALSE)
	{
		return InsertPropItem(pParentItem,propKey,sPropName,sPropHelp,PIT_BUTTON,BDT_FILEPATH,"",iInsertIndex,bUpdate);
	}
	CPropTreeItem *InsertButtonPropItem(CPropTreeItem *pParentItem,char* propKey,CString sPropName="",
										CString sPropHelp="",int iInsertIndex=-1,BOOL bUpdate=FALSE)
	{
		return InsertPropItem(pParentItem,propKey,sPropName,sPropHelp,PIT_BUTTON,BDT_COMMON,"",iInsertIndex,bUpdate);
	}
	CPropTreeItem *InsertBtnEditPropItem(CPropTreeItem *pParentItem,char* propKey,CString sPropName="",
										CString sPropHelp="",int iInsertIndex=-1,BOOL bUpdate=FALSE)
	{
		return InsertPropItem(pParentItem,propKey,sPropName,sPropHelp,PIT_BUTTON,BDT_COMMONEDIT,"",iInsertIndex,bUpdate);
	}
	CPropTreeItem *InsertBtnPopMenuEditPropItem(CPropTreeItem *pParentItem,char* propKey,CString sPropName="",
										CString sPropHelp="",CString sItemAttachValue="",int iInsertIndex=-1,BOOL bUpdate=FALSE)
	{
		return InsertPropItem(pParentItem,propKey,sPropName,sPropHelp,PIT_BUTTON,BDT_POPMENUEDIT,sItemAttachValue,iInsertIndex,bUpdate);
	}
	CPropTreeItem *InsertPopMenuItem(CPropTreeItem *pParentItem,char* propKey,CString sPropName="",
									 CString sPropHelp="",CString sItemAttachValue="",int iInsertIndex=-1,BOOL bUpdate=FALSE)
	{
		return InsertPropItem(pParentItem,propKey,sPropName,sPropHelp,PIT_BUTTON,BDT_POPMENU,sItemAttachValue,iInsertIndex,bUpdate);
	}
	CPropTreeItem *InsertGrayButtonPropItem(CPropTreeItem *pParentItem,char* propKey,CString sPropName="",
		CString sPropHelp="",CString sItemAttachValue="",int iInsertIndex=-1,BOOL bUpdate=FALSE)
	{
		return InsertPropItem(pParentItem,propKey,sPropName,sPropHelp,PIT_GRAYBUTTON,BDT_COMMON,sItemAttachValue,iInsertIndex,bUpdate);
	}
	CPropTreeItem *InsertGrayPopMenuItem(CPropTreeItem *pParentItem,char* propKey,CString sPropName="",
		CString sPropHelp="",CString sItemAttachValue="",int iInsertIndex=-1,BOOL bUpdate=FALSE)
	{
		return InsertPropItem(pParentItem,propKey,sPropName,sPropHelp,PIT_GRAYBUTTON,BDT_POPMENU,sItemAttachValue,iInsertIndex,bUpdate);
	}
	CPropTreeItem *InsertGrayPopMenuEditItem(CPropTreeItem *pParentItem,char* propKey,CString sPropName="",
		CString sPropHelp="",CString sItemAttachValue="",int iInsertIndex=-1,BOOL bUpdate=FALSE)
	{
		return InsertPropItem(pParentItem,propKey,sPropName,sPropHelp,PIT_GRAYBUTTON,BDT_POPMENUEDIT,sItemAttachValue,iInsertIndex,bUpdate);
	}
	//更新指定项属性值
	BOOL SetItemPropValue(char* propKey)
	{
		long item_id=TYPE::GetPropID(propKey);
		return SetItemPropValue(item_id);
	}
	BOOL SetItemPropValue(long prop_id)
	{
		CXhChar100 sText;
		if(m_pObj->GetPropValueStr(prop_id,sText,sText.GetLengthMax())>0)
		{
			m_pPropList->SetItemPropValue(prop_id,CString(sText));
			return TRUE;
		}
		else 
			return FALSE;
	}
	BOOL UpdatePropItemValue(const char* propKey)
	{
		long id=TYPE::GetPropID(propKey);
		CPropTreeItem* pPropItem=m_pPropList->FindItemByPropId(id,NULL);
		if(pPropItem==NULL||m_pObj==NULL)
			return FALSE;
		return SetItemPropValue(id);
	}
};
 //"CPropListOper<TYPE,CXhPtrSet<TYPE>>"改为"CPropListOper<TYPE,CXhPtrSet<TYPE> >",兼容VS2003编译 wht 16-06-13
template <class TYPE> class CPropertyListOper : public CPropListOper<TYPE,CXhPtrSet<TYPE> >
{
public:
	CPropertyListOper(CPropertyList *pPropList,TYPE *pObj,CXhPtrSet<TYPE> *pSelectObjs=NULL)
		: CPropListOper<TYPE,CXhPtrSet<TYPE> >(pPropList,pObj,pSelectObjs)
	{
	}
};