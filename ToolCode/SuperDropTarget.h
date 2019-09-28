#if !defined(AFX_SUPERDROPTARGET_H__4BEACB07_6C08_4996_B474_D7E59AEE9E5D__INCLUDED_)
#define AFX_SUPERDROPTARGET_H__4BEACB07_6C08_4996_B474_D7E59AEE9E5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SuperDropTarget.h : header file
//


#include <afxole.h>
/////////////////////////////////////////////////////////////////////////////
// CSuperDropTarget command target

template <class TYPE> class CSuperDropTarget : public COleDropTarget
{
	//DECLARE_DYNCREATE(CSuperDropTarget)
public:
	CSuperDropTarget(){;}
	DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point){
		ASSERT_VALID(this);
		ASSERT(IsWindow(pWnd->m_hWnd));
		if(pWnd->IsKindOf(RUNTIME_CLASS(TYPE)))
		{
			TYPE* pTreeCtrl = (TYPE*)pWnd;
			return pTreeCtrl->OnDragEnter(pWnd, pDataObject, dwKeyState, point);
		}
		else
			return COleDropTarget::OnDragEnter(pWnd, pDataObject, dwKeyState, point);      
	}
	DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point){
		ASSERT_VALID(this);
		ASSERT(IsWindow(pWnd->m_hWnd));
		if(pWnd->IsKindOf(RUNTIME_CLASS(TYPE)))
		{
			TYPE* pTreeCtrl = (TYPE*)pWnd;
			return pTreeCtrl->OnDragOver(pWnd, pDataObject, dwKeyState, point);
		}
		else
			return COleDropTarget::OnDragOver(pWnd, pDataObject, dwKeyState, point);
	}
	BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point){
		ASSERT_VALID(this);
		ASSERT(IsWindow(pWnd->m_hWnd));
		if(pWnd->IsKindOf(RUNTIME_CLASS(TYPE)))
		{
			TYPE* pTreeCtrl = (TYPE*)pWnd;
			return pTreeCtrl->OnDrop(pWnd, pDataObject, dropEffect, point);
		}
		else
			return COleDropTarget::OnDrop(pWnd, pDataObject, dropEffect, point); 
	}
	DROPEFFECT OnDropEx(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point){
		ASSERT_VALID(this);
		ASSERT(IsWindow(pWnd->m_hWnd));
		if(pWnd->IsKindOf(RUNTIME_CLASS(TYPE)))
		{
			TYPE* pTreeCtrl = (TYPE*)pWnd;
			return pTreeCtrl->OnDropEx(pWnd, pDataObject, dropDefault, dropList, point);
		}
		else
			return COleDropTarget::OnDropEx(pWnd, pDataObject, dropDefault, dropList, point);
	}
	//void OnDragLeave(CWnd* pWnd);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSuperDropTarget)
	//}}AFX_VIRTUAL

// Implementation
	virtual ~CSuperDropTarget(){;}

	// Generated message map functions
	//{{AFX_MSG(CSuperDropTarget)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUPERDROPTARGET_H__4BEACB07_6C08_4996_B474_D7E59AEE9E5D__INCLUDED_)
