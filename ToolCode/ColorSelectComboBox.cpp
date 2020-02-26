// ColorSelectComboBox.cpp : implementation file
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "ColorSelectComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorSelectComboBox

CColorSelectComboBox::CColorSelectComboBox()
{
}

CColorSelectComboBox::~CColorSelectComboBox()
{
}


BEGIN_MESSAGE_MAP(CColorSelectComboBox, CComboBox)
	//{{AFX_MSG_MAP(CColorSelectComboBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorSelectComboBox message handlers

void CColorSelectComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
}

void CColorSelectComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	if(lpDrawItemStruct->CtlType!=ODT_COMBOBOX)
		return;
	if(lpDrawItemStruct->itemID <0)
		return;
	COLORREF	itemColor=GetItemData(lpDrawItemStruct->itemID);//��Ŀ����ɫ
	COLORREF	textColor=::GetSysColor( COLOR_WINDOWTEXT );//�ı�����ɫ
	COLORREF	backColor=::GetSysColor(COLOR_WINDOW);//��Ŀ����ɫ
	CString		itemString;//�洢��Ŀ�ַ���
	//�õ���Ŀ�İ�Χ����
	CRect	itemRect=lpDrawItemStruct->rcItem;
	CDC	*pDC=CDC::FromHandle (lpDrawItemStruct->hDC );
	if( lpDrawItemStruct->itemState & ODS_FOCUS )
	{
		//�����Ŀ�������뽹��,�����ñ���ɫΪ��ϵͳ����ɫ(ͨ��Ϊ��ɫ)
		backColor=::GetSysColor(COLOR_HIGHLIGHT);
		textColor= 0x00FFFFFF & ~( textColor );
	}
	if(  lpDrawItemStruct->itemState & ODS_DISABLED )
	{
		//����ؼ�����ֹ,�������ı�ɫ����Ŀ��ɫΪ��ֹɫ
		textColor = ::GetSysColor( COLOR_INACTIVECAPTIONTEXT );
		itemColor=textColor;
	}
	//�Ȼ��Ʊ���
	pDC->FillRect (&itemRect,&CBrush(backColor));
	if( lpDrawItemStruct->itemState & ODS_FOCUS )
		pDC->DrawFocusRect( &itemRect);	//�����Ŀ�������뽹��,��Ҫ����һ�������
	//������ɫ����
	CRect	colorRect;
	colorRect.left=itemRect.left+COLOR_RECT_BORDER;
	colorRect.top=itemRect.top+COLOR_RECT_BORDER;
	colorRect.right=colorRect.left+COLOR_RECT_WIDTH;
	colorRect.bottom=itemRect.bottom - COLOR_RECT_BORDER;
	//������ɫ����
	if(itemColor!=0xFFFFFFFF&&itemColor!=0xEFFFFFFF&&itemColor!=0xCFFFFFFF)
	{
		if (itemColor == 0xDFFFFFFF)
			itemColor = 0x00FFFFFF;
		CBrush	brush(itemColor);
		CBrush	*oldbrush=pDC->SelectObject (&brush);
		pDC->Rectangle (&colorRect);
		pDC->SelectObject (oldbrush);
		brush.DeleteObject();
	}
	else
	{
		CBrush *oldbrush = (CBrush*)pDC->SelectStockObject(HOLLOW_BRUSH);
		pDC->Rectangle(&colorRect);
		pDC->MoveTo(colorRect.left,colorRect.top);
		pDC->LineTo(colorRect.right,colorRect.bottom);
		pDC->MoveTo(colorRect.left,colorRect.bottom);
		pDC->LineTo(colorRect.right,colorRect.top);
		pDC->SelectObject(oldbrush);
	}

	//�õ�����������
	if((int)lpDrawItemStruct->itemID>=0)
	{
		GetLBText(lpDrawItemStruct->itemID, itemString );
		itemRect.OffsetRect (2*COLOR_RECT_BORDER+COLOR_RECT_WIDTH+5,0);
		pDC->SetBkMode(TRANSPARENT);//�����������ģʽΪ͸������
		pDC->SetTextColor (textColor);
		DrawText(lpDrawItemStruct->hDC, itemString, -1, &itemRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
}

int CColorSelectComboBox::AddColor(COLORREF rgbColor, LPCTSTR sComment,BOOL bAppend)
{
	if(IsHaveThisColor(rgbColor)!=-1)
		return -1;
	int n = GetCount();
	int nItem;
	if(bAppend)
		nItem = AddString(sComment);
	else
		nItem=InsertString(n-1,sComment);
	if(nItem >=0)
		SetItemData (nItem,rgbColor);
	return	nItem;
}

int CColorSelectComboBox::AddString(LPCTSTR lpszItem)
{
	return ((CComboBox*)this)->AddString(lpszItem);
}

int CColorSelectComboBox::IsHaveThisColor(COLORREF rgbColor)
{
	int ntotal=GetCount( );
	for(int i=0;i<ntotal;i++)
	{
		if(GetItemData(i)==rgbColor)
			return	i;
	}
	return -1;
}

void CColorSelectComboBox::InitBox(COLORREF crColor)
{
#ifdef AFX_TARG_ENU_ENGLISH
	AddColor (RGB(255,0,0),"Red");
	AddColor (RGB(0,255,0),"Green");
	AddColor (RGB(0,0,255),"Blue");
	AddColor (RGB(255,255,0),"Yellow");
	AddColor (RGB(255,0,255)," Purplish Red");
	AddColor (RGB(0,255,255),"Cyan");
	AddColor (RGB(128,0,0),"Dark Red");
	AddColor (RGB(0,128,0),"Dark Green");
	AddColor (RGB(0,0,128),"Dark Blue");
	AddColor (RGB(128,128,0),"Dark Yellow");
	AddColor (RGB(128,0,128),"Dark Purple");
	AddColor (RGB(0,128,128),"Dark Cyan");
	AddColor (RGB(255,255,255),"White");
	AddColor (RGB(0,0,0),"Black");
#else 
	AddColor (RGB(255,0,0),"��");
	AddColor (RGB(0,255,0),"��");
	AddColor (RGB(0,0,255),"��");
	AddColor (RGB(255,255,0),"��");
	AddColor (RGB(255,0,255),"�Ϻ�");
	AddColor (RGB(0,255,255),"��");
	AddColor (RGB(128,0,0),"����");
	AddColor (RGB(0,128,0),"����");
	AddColor (RGB(0,0,128),"����");
	AddColor (RGB(128,128,0),"����");
	AddColor (RGB(128,0,128),"����");
	AddColor (RGB(0,128,128),"����");
	AddColor (RGB(255,255,255),"��");
	AddColor (RGB(0,0,0),"��");
#endif
	AddColor (0xDFFFFFFF, "͸��");
	AddColor (0xCFFFFFFF, "��");	//֧�ֲ�������ɫ wht 20-02-11
	AddColor (0XFFFFFFFF,"....");
	AddColor (0XEFFFFFFF,"ʰȡ��ɫ");
	int iCur = IsHaveThisColor(crColor);
	if(iCur>=0)
		SetCurSel (iCur);
	else
	{
		iCur = AddColor(crColor,"�Զ���",FALSE);
		SetCurSel(iCur);
	}
}

COLORREF CColorSelectComboBox::GetSelColor()
{
	int nSelectedItem=this->GetCurSel ();
	return GetItemData(nSelectedItem);
}


BOOL CColorSelectComboBox::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )	
	{		
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{			
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			GetParent()->SetFocus();
			return 1;
		}	
	}
	return CComboBox::PreTranslateMessage(pMsg);
}
