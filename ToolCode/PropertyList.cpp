// PropertyList.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyList.h"
#include "ColorSelectComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
static DWORD BINARYWORD[32]={ 0X00000001,0X00000002,0X00000004,0X00000008,0X00000010,0X00000020,0X00000040,0X00000080,
						  0X00000100,0X00000200,0X00000400,0X00000800,0X00001000,0X00002000,0X00004000,0X00008000,
						  0X00010000,0X00020000,0X00040000,0X00080000,0X00100000,0X00200000,0X00400000,0X00800000,
						  0X01000000,0X02000000,0X04000000,0X08000000,0X10000000,0X20000000,0X40000000,0X80000000};
DWORD GetBinaryWord(int base0Index)
{
	if(base0Index<0||base0Index>=32)
		return 0;
	else
		return BINARYWORD[base0Index];
}
BOOL CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
CPropTreeItem::CPropTreeItem()
{
	m_idProp=0;
	m_pParent = NULL;
	m_bHideSelf = m_bHideChildren = FALSE;
	m_iIndex = -1;
	m_nIndent = -1;
	m_lpNodeInfo = NULL;
	m_bReadOnly = m_bDefaultReadOnlyState;
	m_dwPropGroup=1;
}
CPropTreeItem::~CPropTreeItem()
{
	if(m_lpNodeInfo!=NULL)
	{
		delete m_lpNodeInfo;
		m_lpNodeInfo=NULL;
	}
	// delete child nodes
	POSITION pos = m_listChild.GetHeadPosition();
	while (pos != NULL)
	{
		CPropTreeItem* pItem =(CPropTreeItem*)m_listChild.GetNext(pos);
		if(pItem!=NULL)
			delete pItem;
	}
	m_listChild.RemoveAll();
}
int CPropTreeItem::GetTailIndex()
{
	int iTailIndex=m_iIndex;
	POSITION pos=m_listChild.GetHeadPosition();
	while(pos)
	{
		CPropTreeItem* pItem =(CPropTreeItem*)m_listChild.GetNext(pos);
		int index=pItem->GetTailIndex();
		if(index>iTailIndex)
			iTailIndex=index;
	}
	return iTailIndex;
}
// CPropertyList

CPropertyList::CPropertyList()
{
	root.m_nIndent=0;
	m_pCurEditItem = NULL;
	m_hPromptWnd = NULL;
	m_ToolTips=NULL;
	m_bEscKeyDown = FALSE;
	m_bPropValueModified = FALSE;
	m_nObjClassTypeId=0;
	m_pObj = NULL;
	m_iPropGroup=0;	//Ĭ����ʾ��һ��������
	m_bLocked=FALSE;
	m_wInteralLockRef=0;
	m_nDivider=0;
	m_fDividerScale=0.6;//�ָ����,Ĭ��ֵΪ0.6 wht 10-12-02
	FireCheckBoxClick = NULL;
	FireButtonClick = NULL;
	FireValueModify = NULL;
	FireStatusModify = NULL;
	FireHelpPrompt = NULL;
	FirePopMenuClick = NULL;
	FirePickColor = NULL;
	m_bEnterKeyDown = FALSE;
	m_bEnableTextChangeEnvent = TRUE;
}

CPropertyList::~CPropertyList()
{
	if(m_ToolTips!=NULL)
		delete m_ToolTips;
	CleanTree();
}

int CPropertyList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_bDivIsSet = FALSE;
	m_nDivider = 0;
	m_bTracking = FALSE;

	m_hCursorSize = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
	m_hCursorArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

	return 0;
}

BEGIN_MESSAGE_MAP(CPropertyList, CListBox)
	//{{AFX_MSG_MAP(CPropertyList)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	ON_WM_LBUTTONUP()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblclk)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	ON_CBN_KILLFOCUS(IDC_PROPCMBBOX, OnKillfocusCmbBox)
	ON_EN_KILLFOCUS(IDC_PROPEDITBOX, OnKillfocusEditBox)
	ON_BN_KILLFOCUS(IDC_PROPBTNCTRL, OnKillfocusBtn)
	ON_BN_CLICKED(IDC_PROPBTNCTRL, OnButton)
	ON_BN_CLICKED(IDC_PROPCHKBOX, OnCheckBox)
	ON_CBN_SELCHANGE(IDC_PROPCMBBOX, OnSelchangeCmbBox)
	ON_EN_CHANGE(IDC_PROPEDITBOX, OnChangeEditBox)
	ON_COMMAND_RANGE(ID_FIRST_POPMENU,ID_FIRST_POPMENU+100,OnClickPopMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyList message handlers

BOOL CPropertyList::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CListBox::PreCreateWindow(cs))
		return FALSE;

	cs.style &= ~(LBS_OWNERDRAWVARIABLE | LBS_SORT);
	cs.style |= LBS_OWNERDRAWFIXED;

	m_bTracking = FALSE;
	m_nDivider = 0;
	m_bDivIsSet = FALSE;

	return TRUE;
}

void CPropertyList::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = 20; //pixels
	//CListBox::MeasureItem(lpMeasureItemStruct);
	//lpMeasureItemStruct->itemHeight = nItemHeight + 4;//or should I go for max(nItemheight+4, m_cxImage+2);
}

void CPropertyList::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	if (lpDIS->CtlType != ODT_LISTBOX)
        return;
	CDC dc;
	dc.Attach(lpDIS->hDC);
	CRect rectFull = lpDIS->rcItem;
	CRect rectActual = rectFull;
	CRect rectRight = rectFull;
	CRect rectLeft = rectFull;
	CRect rectSide = rectFull;
	if (m_nDivider==0)	//ͨ���ָ��������m_nDivider wht 10-12-02
		m_nDivider = (int)(rectFull.Width()*m_fDividerScale);
	rectRight.left = m_nDivider+1;
	rectLeft.right = m_nDivider;
	UINT nIndex = lpDIS->itemID;
	//get the CPropTreeItem for the current row
	if(lpDIS->itemData==0||lpDIS->itemData==-1)
		return;
	CPropTreeItem* pItem = (CPropTreeItem*) lpDIS->itemData;
	rectActual.left += pItem->m_nIndent*12;
	rectSide.right = rectSide.left + 12;

	if (nIndex != (UINT) -1)
	{
		dc.SetBkMode(TRANSPARENT);
		COLORREF crBackground, crText;
		// Set the text background and foreground colors
		if(pItem->m_lpNodeInfo->m_controlType==PIT_STATIC||pItem->m_lpNodeInfo->m_controlType==PIT_GRAYBUTTON)
		{
			crBackground = RGB(212,212,212);
			crText = RGB(0,0,0);	
		}
		else
		{
			if (lpDIS->itemState & ODS_SELECTED)
			{
				crBackground = GetSysColor (COLOR_HIGHLIGHT);
				crText = GetSysColor (COLOR_HIGHLIGHTTEXT);
			}
			else
			{
				crBackground = GetSysColor (COLOR_WINDOW);
				crText = GetSysColor (COLOR_WINDOWTEXT);
			}
		}
		dc.SetBkColor (crBackground);
		dc.SetTextColor (crText);
		//fill background color
		if(pItem->m_lpNodeInfo->m_controlType==PIT_STATIC||pItem->m_lpNodeInfo->m_controlType==PIT_GRAYBUTTON)
			ExtTextOut(dc.GetSafeHdc(), 0, 0, ETO_OPAQUE, rectFull, NULL, 0, NULL);
		else
			ExtTextOut(dc.GetSafeHdc(), 0, 0, ETO_OPAQUE, rectLeft, NULL, 0, NULL);
		
		//draw rectSide
		dc.FillSolidRect(rectSide,RGB(212,212,212));
		//draw [+]/[-]
		DrawTreeItemSymbol(&dc,pItem,0,rectFull);
		//draw two rectangles, one for each row column
		if(pItem->m_lpNodeInfo->m_controlType==PIT_STATIC||pItem->m_lpNodeInfo->m_controlType==PIT_GRAYBUTTON)
		{
			//dc.FillSolidRect(rectActual,RGB(212,212,212));
			if(nIndex!=0)
			{
				CPen psPen(PS_SOLID, 1, RGB(160,160,160));
				CPen* pOldPen = dc.SelectObject(&psPen);
				dc.MoveTo(rectFull.left, rectFull.top);
				dc.LineTo(rectFull.right, rectFull.top);
				dc.SelectObject(pOldPen);
				psPen.DeleteObject();
			}
			//write the property name in the first rectangle
			dc.DrawText(pItem->m_lpNodeInfo->m_strPropName,CRect(rectActual.left+3,rectActual.top+3,
				rectActual.right-3,rectActual.bottom-3),DT_LEFT | DT_SINGLELINE);
		}
		else
		{
			rectLeft.left += 12;
			dc.DrawEdge(rectLeft,EDGE_SUNKEN,BF_BOTTOMRIGHT);
			//write the property name in the first rectangle
			//��rectLeft.right������ָ����仯
			dc.DrawText(pItem->m_lpNodeInfo->m_strPropName,CRect(rectActual.left+3,rectActual.top+3,
				rectLeft.right-3,rectActual.bottom-2),DT_LEFT | DT_SINGLELINE);
		}

		if(pItem->m_lpNodeInfo->m_controlType!=PIT_STATIC&&pItem->m_lpNodeInfo->m_controlType!=PIT_GRAYBUTTON)
		{
			if(pItem->m_lpNodeInfo->m_bMutiObjsProp)//&&!pItem->m_bReadOnly)
				dc.SetBkColor(GetSysColor(COLOR_INACTIVEBORDER));
			else
				dc.SetBkColor(RGB(255,255,255));
			if(pItem->m_lpNodeInfo->m_bMutiObjsProp&&!pItem->m_bReadOnly)
				dc.SetTextColor(RGB(0,0,255));
			else
				dc.SetTextColor(RGB(0,0,0));
			//fill background color
			ExtTextOut(dc.GetSafeHdc(), 0, 0, ETO_OPAQUE, rectRight, NULL, 0, NULL);
			dc.DrawEdge(rectRight,EDGE_SUNKEN,BF_BOTTOM);
		}
		//write the initial property value in the second rectangle
		if(pItem->m_lpNodeInfo->m_controlType==PIT_ITEMSET)
#ifdef AFX_TARG_ENU_ENGLISH
			dc.DrawText("*Collection*",CRect(rectRight.left+3,rectRight.top+3,
#else 
			dc.DrawText("*����*",CRect(rectRight.left+3,rectRight.top+3,
#endif
				rectRight.right-3,rectRight.bottom-3),DT_LEFT | DT_SINGLELINE);
		else if(pItem->m_lpNodeInfo->m_strPropValue.GetLength()>0)
		{
			char temp_clr[101];
			if(pItem->m_bReadOnly)
				dc.SetTextColor(RGB(255,0,0));//GetSysColor (COLOR_GRAYTEXT));
			if (pItem->m_lpNodeInfo->m_strPropValue.Find("RGB") > -1)
			{
				_snprintf(temp_clr,100,"%s",pItem->m_lpNodeInfo->m_strPropValue);
				memmove(temp_clr,temp_clr+3,97);
				COLORREF itemColor;
				sscanf(temp_clr,"%X",&itemColor);
				//������ɫ����
				CRect	colorRect;
				colorRect.left=rectRight.left+2;
				colorRect.top=rectRight.top+2;
				colorRect.right=rectRight.right-27;
				colorRect.bottom=rectRight.bottom - 2;
				//0xFFFFFFFF���Զ�����ɫ
				//0xEFFFFFFF��ʰȡ��ɫ
				//0xDFFFFFFF��͸��ɫ
				//0xCFFFFFFF����	wht 20-02-11
				int rectHalfWidth = (int)(colorRect.Width()*0.5);
				if (rectHalfWidth > COLOR_RECT_WIDTH)
					rectHalfWidth = COLOR_RECT_WIDTH;
				if (itemColor == 0xCFFFFFFF || itemColor == 0xDFFFFFFF)
				{	//��ɫ��͸��ɫ
					if (itemColor == 0xDFFFFFFF)
						itemColor = 0x00FFFFFF;
					colorRect.right = colorRect.left + rectHalfWidth;
				}
				if (itemColor == 0xFFFFFFFF || itemColor == 0xEFFFFFFF || itemColor == 0xCFFFFFFF)
				{	
					CBrush* oldbrush = (CBrush*)dc.SelectStockObject(HOLLOW_BRUSH);
					dc.Rectangle(&colorRect);
					dc.MoveTo(colorRect.left, colorRect.top);
					dc.LineTo(colorRect.right, colorRect.bottom);
					dc.MoveTo(colorRect.left, colorRect.bottom);
					dc.LineTo(colorRect.right, colorRect.top);
					dc.SelectObject(oldbrush);
				}
				else
				{	//������ɫ
					CBrush	brush(itemColor);
					CBrush	*oldbrush=dc.SelectObject (&brush);
					dc.Rectangle (&colorRect);
					dc.SelectObject (oldbrush);
					brush.DeleteObject();
				}
				//�õ�����������
				if (itemColor == 0xCFFFFFFF || itemColor == 0xDFFFFFFF || itemColor == 0x00FFFFFF)
				{
					colorRect.OffsetRect(2 * COLOR_RECT_BORDER + rectHalfWidth + 5, 0);
					dc.SetBkMode(TRANSPARENT);//�����������ģʽΪ͸������
					dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
					if(itemColor== 0xCFFFFFFF)
						DrawText(dc.GetSafeHdc(), "��ɫ", -1, &colorRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
					else //if(itemColor == 0xDFFFFFFF || itemColor == 0x00FFFFFF)
						DrawText(dc.GetSafeHdc(), "͸��", -1, &colorRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				}
			}
			else
				dc.DrawText(pItem->m_lpNodeInfo->m_strPropValue,CRect(rectRight.left+3,rectRight.top+3,
					rectRight.right-3,rectRight.bottom-3),DT_LEFT | DT_SINGLELINE);
		}
		//ѡ�����Ա���ʱ��ʾ���߿�
		if (lpDIS->itemState & ODS_SELECTED)
		{
			if(pItem->m_lpNodeInfo->m_controlType==PIT_STATIC)
			{
				CSize size = dc.GetTextExtent(pItem->m_lpNodeInfo->m_strPropName);
				CRect rectFocus(rectLeft.left+14,rectLeft.top+2,rectLeft.left+14+10+size.cx,rectLeft.top+2+3+size.cy);
				dc.DrawFocusRect(rectFocus);
			}
		}
	}
	dc.Detach();
}

void CPropertyList::DrawTreeItemSymbol(CDC* pDC, CPropTreeItem* pSelItem, int nListItem, const CRect& rcBounds)
{
	int nColWidth = m_nDivider;
	int yDown = rcBounds.top;

    CPen psPen(PS_SOLID, 1, RGB(212,212,212));
    CPen* pOldPen = pDC->SelectObject(&psPen);
	//setup plus/minus rectangle
	int nBottomDown = (int)((rcBounds.top+rcBounds.bottom)*0.5);
	int symbol_center_x = rcBounds.left  + (pSelItem->m_nIndent-1)*12+5;
	POINT box_lefttop = {symbol_center_x-4, nBottomDown-4};
	SIZE box_size = {9, 9};
	//
	BOOL bChild = pSelItem->m_listChild.GetCount()>0;
	BOOL bCollapsed = pSelItem->m_bHideChildren;
	//draw plus/minus sign
	if(bChild)
	{
		//erase bkgrnd
		CRect rc(box_lefttop, box_size);
		pDC->FillRect(rc, &CBrush(RGB(212,212,212)));
		//draw rectangle	
		CPen psrect(PS_SOLID, 1, RGB(0,0,0));
		CPen* pOldPen1 = pDC->SelectObject(&psrect);
		pDC->Rectangle(rc);
		pDC->SelectObject(pOldPen1);
		psrect.DeleteObject();
		//draw plus/minus sign
		CPen psPen1(PS_SOLID, 1, RGB(0,0,0));
		CPen* pOldPen2 = pDC->SelectObject(&psPen1);
		int topdown = nBottomDown;
		if(bCollapsed)
		{
			//plus
			pDC->MoveTo(symbol_center_x, topdown-2);
			pDC->LineTo(symbol_center_x, topdown+3);
			//	
			pDC->MoveTo(symbol_center_x-2, topdown);
			pDC->LineTo(symbol_center_x+3, topdown);
		}
		else {
			//minus 
			pDC->MoveTo(symbol_center_x-2, topdown);
			pDC->LineTo(symbol_center_x+3, topdown);
		}
		pDC->SelectObject(pOldPen2);
		psPen1.DeleteObject();
	}
	pDC->SelectObject(pOldPen);
	psPen.DeleteObject();
}

void CPropertyList::OnSelchangeCmbBox()
{
	try{
		if(!m_bEnableTextChangeEnvent)
			return;
		if(m_pCurEditItem==NULL)
			return;
		m_bPropValueModified=TRUE;
		CColorSelectComboBox *pCmbBox=NULL;
		if(m_pCurEditItem->m_lpNodeInfo->m_cmbType==CDT_EDIT)
			pCmbBox=&m_cmbBox1;		//return;	//��OnKillfocusCmbBox�д���ֵ�޸��¼�
		else if(m_pCurEditItem->m_lpNodeInfo->m_cmbType==CDT_COLOR)
			pCmbBox=&m_cmbBox3;
		else
			pCmbBox=&m_cmbBox2;
		if (pCmbBox->GetSafeHwnd())
		{
			if(m_bEscKeyDown)
				m_bEscKeyDown = FALSE;
			else if(m_pCurEditItem->m_lpNodeInfo->m_controlType==PIT_ITEMSET)
			{
				int iCurSel = pCmbBox->GetCurSel();
				m_pCurEditItem->m_lpNodeInfo->m_strPropValue.Format("%d",iCurSel);
				if(FireButtonClick)
					FireButtonClick(this,m_pCurEditItem);
				pCmbBox->SetCurSel(0);
			}
			else
			{	//���޴˶δ��뵱�������Ʒ����仯ʱ���ܼ�ʱ���¹�����ɫ 
				CString selStr;
				BOOL bUseColorDlg=FALSE;	//�Ƿ�������ɫ�Ի���
				if(m_pCurEditItem->m_lpNodeInfo->m_cmbType==CDT_COLOR)
				{
					DWORD color = (DWORD)pCmbBox->GetItemData(pCmbBox->GetCurSel());
					if(color==0XFFFFFFFF)
					{	//������ɫ�Ի����������ʧȥ���㴥��OnKillfocusCmbBox
						CColorDialog dlg(color);
						if(dlg.DoModal()==IDOK)
						{
							bUseColorDlg=TRUE;
							color = dlg.GetColor();
							int iCur = m_cmbBox3.IsHaveThisColor(color);
							if(iCur==-1)
							{
								iCur = m_cmbBox3.AddColor(color,"�Զ���",FALSE);
								m_cmbBox3.SetCurSel(iCur);
							}
							else
								m_cmbBox3.SetCurSel(iCur);
						}
					}
					else if(color==0XEFFFFFFF)
					{	//ͨ���ص�����ʰȡ��ɫ wht 19-02-11
						if(FirePickColor!=NULL)
							FirePickColor(this,m_pCurEditItem,color);
					}
					selStr.Format("RGB%X",color);
				}
				else
					pCmbBox->GetLBText(pCmbBox->GetCurSel(),selStr);
				CPropTreeItem *pItem=m_pCurEditItem;	//�ص������е���Redraw()��m_pCurEditItem=NULL
				if( m_bPropValueModified&&pItem&&(FireValueModify==NULL||
					FireValueModify(this,pItem,selStr)))
				{
					if(pItem->m_lpNodeInfo->m_bMutiObjsProp||	//��ֵ��������б仯 wht 10-12-23
						pItem->m_lpNodeInfo->m_strPropValue.CompareNoCase(selStr)!=0)
					{
						pItem->m_lpNodeInfo->m_strPropValue = selStr;
						pItem->m_lpNodeInfo->m_bMutiObjsProp=FALSE;
						m_bPropValueModified=FALSE;
					}
				}
				//ͨ����ɫ�Ի����Զ�����ɫ�����ڴ˴��ֶ�����OnKillfocusCmbBox(); wht 10-05-12
				if(bUseColorDlg)	
					OnKillfocusCmbBox();	
			}
		}
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
}

void CPropertyList::OnChangeEditBox()
{
	if(!m_bEnableTextChangeEnvent)
		return;
	CString selStr;
	m_editBox.GetWindowText(selStr);
	if( m_pCurEditItem&&
		(m_pCurEditItem->m_lpNodeInfo->m_bMutiObjsProp||	//��ֵ��������б仯 wht 10-12-23
		m_pCurEditItem->m_lpNodeInfo->m_strPropValue.CompareNoCase(selStr)!=0))
		m_bPropValueModified=TRUE;
}

void CPropertyList::OnSelchange() 
{
	CRect rect;
	COLORREF itemColor;
	char temp_clr[100];
	CString lBoxSelText;
	GetItemRect(m_curSel,rect);
	rect.left = m_nDivider+1;

	CPropTreeItem* pItem = (CPropTreeItem*) GetItemDataPtr(m_curSel);
	if(pItem==(void*)-1)
		return;
	if(pItem&&FireHelpPrompt)//pItem->FirePropHelpPrompt)
		FireHelpPrompt(this,pItem);
	if(m_hPromptWnd!=NULL)
	{
		CString sHelpText=pItem->m_lpNodeInfo->m_strPropHelp;
		if(sHelpText.GetLength()<=0)	//δ���ð����ַ���ʱĬ����������Ϊ�����ַ��� wht 11-09-26
			sHelpText=pItem->m_lpNodeInfo->m_strPropName;
		::SetWindowText(m_hPromptWnd,sHelpText);
		::UpdateWindow(m_hPromptWnd);
	}
	HideInputCtrl();	//OnSelchange()ʱӦ�������е�ǰ��ʾ������ؼ� wht 11-04-01
	/*if (m_btnCtrl)
	{
		HideEditBox();
		m_btnCtrl.ShowWindow(SW_HIDE);
	}*/
	if( pItem->m_bReadOnly&&!(	//�ɱ༭�ĺ��˵�������,ֻ��ʱҲӦ������ʾ���˵���ť wjh-2017.2.9
		pItem->m_lpNodeInfo->m_controlType==PIT_BUTTON && (pItem->m_lpNodeInfo->m_buttonType==BDT_COMMONEDIT||pItem->m_lpNodeInfo->m_buttonType==BDT_POPMENUEDIT)))
		return;
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	if (pItem->m_lpNodeInfo->m_controlType==PIT_STATIC)
	{
	}
	else if (pItem->m_lpNodeInfo->m_controlType==PIT_COMBO||pItem->m_lpNodeInfo->m_controlType==PIT_ITEMSET)
	{
		//display the combo box.  If the combo box has already been
		//created then simply move it to the new location, else create it
		m_nLastBox = 0;
		//CComboBox *pCmbBox=NULL;
		CColorSelectComboBox *pCmbBox=NULL;
		if(pItem->m_lpNodeInfo->m_cmbType==CDT_COLOR)
			pCmbBox=&m_cmbBox3;
		else if(pItem->m_lpNodeInfo->m_cmbType==CDT_EDIT)
			pCmbBox=&m_cmbBox1;
		else
			pCmbBox=&m_cmbBox2;
		if (pCmbBox->GetSafeHwnd())
			pCmbBox->MoveWindow(rect);
		else
		{	
			rect.bottom += 300;
			if(pItem->m_lpNodeInfo->m_cmbType==CDT_COLOR)
				pCmbBox->Create(CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_DROPDOWNLIST | CBS_NOINTEGRALHEIGHT | WS_VISIBLE | WS_CHILD | WS_VSCROLL,//| CBS_OWNERDRAWVARIABLE | CBS_HASSTRINGS | WS_BORDER,
								rect,this,IDC_PROPCMBBOX);
			else if(pItem->m_lpNodeInfo->m_cmbType==CDT_EDIT)
				pCmbBox->Create(CBS_DROPDOWN | CBS_NOINTEGRALHEIGHT | WS_VISIBLE | WS_CHILD | WS_VSCROLL,//| CBS_OWNERDRAWVARIABLE | CBS_HASSTRINGS | WS_BORDER,
								rect,this,IDC_PROPCMBBOX);
			else
				pCmbBox->Create(CBS_DROPDOWNLIST | CBS_NOINTEGRALHEIGHT | WS_VISIBLE | WS_CHILD | WS_VSCROLL,//| CBS_OWNERDRAWVARIABLE | CBS_HASSTRINGS | WS_BORDER,
								rect,this,IDC_PROPCMBBOX);
			pCmbBox->SetFont(&m_SSerif8Font); //�ڴ�����ɫ������ʱ�˾�������
		}

		//add the choices for this particular property
		if(pItem->m_lpNodeInfo->m_cmbType==CDT_COLOR)
		{
			pCmbBox->InitBox(RGB(0,0,0));
			sprintf(temp_clr,"%s",pItem->m_lpNodeInfo->m_strPropValue);
			memmove(temp_clr,temp_clr+3,97);
			sscanf(temp_clr,"%X",&itemColor);
		}
		else
		{
			CString cmbItems = pItem->m_lpNodeInfo->m_cmbItems;
			lBoxSelText = pItem->m_lpNodeInfo->m_strPropValue;
			
			pCmbBox->ResetContent();	
			int i,i2;
			i=0;
			while ((i2=cmbItems.Find('|',i)) != -1)
			{
				pCmbBox->AddString(cmbItems.Mid(i,i2-i));
				i=i2+1;
			}
			if(i<cmbItems.GetLength())
				pCmbBox->AddString(cmbItems.Right(cmbItems.GetLength()-i));
		}
		pCmbBox->ShowWindow(SW_SHOW);
		m_pCurEditItem = pItem;

		m_bEnableTextChangeEnvent=FALSE;	//�������ݸ����¼�����
		if(pItem->m_lpNodeInfo->m_cmbType==CDT_COLOR)
		{	//jump to the property's current value in the combo box
			
			int iCur = pCmbBox->IsHaveThisColor(itemColor);
			if(iCur==-1)
			{
				int iItem=pCmbBox->AddColor(itemColor,"�Զ���");
				pCmbBox->SetCurSel(iItem);
			}
			else
				pCmbBox->SetCurSel(iCur);
		}		
		else
		{
			//jump to the property's current value in the combo box
			int j = pCmbBox->FindStringExact(0,lBoxSelText);
			if (j != CB_ERR)
				pCmbBox->SetCurSel(j);
			else
				pCmbBox->SetCurSel(0);
			if(pItem->m_lpNodeInfo->m_cmbType==CDT_EDIT)
				pCmbBox->SetWindowText(lBoxSelText);
		}
		if(point.x<m_nDivider||point.x>rect.right)
			SetFocus();
		else
			pCmbBox->SetFocus();
		m_bEnableTextChangeEnvent=TRUE;	//�������ݸ����¼�����
	}
	else if (pItem->m_lpNodeInfo->m_controlType==PIT_EDIT)
	{
		//display edit box
		m_nLastBox = 1;
		m_prevSel = m_curSel;
		rect.left+=3;
		//rect.right-=3;
		rect.top+=3;
		rect.bottom -= 3;
		if (m_editBox)
			m_editBox.MoveWindow(rect);
		else
		{	
			m_editBox.Create(ES_LEFT | ES_AUTOHSCROLL | WS_VISIBLE | WS_CHILD,
							rect,this,IDC_PROPEDITBOX);
			m_editBox.SetFont(&m_SSerif8Font);
		}

		lBoxSelText = pItem->m_lpNodeInfo->m_strPropValue;
		/* ��ʱ���������ĿҲ��Ҫ�༭����ֵ wjh-2011.11.21
		if(pItem->m_listChild.GetCount()>0)
			HideEditBox();
		else*/
		{
			//Ӧ��m_editBox.SetWindowText()֮ǰ���ı䵱ǰѡ���������OnChangeEditBox()
			//�е�m_pCurEditItemΪǰһ��ѡ������ǵ�ǰѡ���� wht 11-04-14
			m_pCurEditItem = pItem;
			//set the text in the edit box to the property's current value
			m_bEnableTextChangeEnvent=FALSE;
			m_editBox.SetWindowText(lBoxSelText);
			m_bEnableTextChangeEnvent=TRUE;	//�������ݸ����¼�����
			if(point.x<m_nDivider||point.x>rect.right)
				SetFocus();
			else
			{
				m_editBox.ShowWindow(SW_SHOW);
				m_editBox.SetFocus();
				int len = m_editBox.GetWindowTextLength();
				m_editBox.SetSel(0, len);
			}
		}
	}
	else 
	{
		if(pItem->m_lpNodeInfo->m_controlType==PIT_CHECKBOX)
			DisplayCheckBox(rect,pItem);	//CheckBox
		else 
			DisplayButton(rect, pItem);		//��ͨ��ť
		if(point.x<m_nDivider||point.x>rect.right)
			SetFocus();
		else if(!m_bEnterKeyDown)
		{	//���»س�������OnSelchangeʱ��Ӧ����OnButton() wht 11-07-16
			if((pItem->m_lpNodeInfo->m_controlType==PIT_BUTTON||pItem->m_lpNodeInfo->m_controlType==PIT_GRAYBUTTON)&&pItem->m_lpNodeInfo->m_buttonType==BDT_COMMON)
				OnButton();
		}
	}
}

//��ʾCheckBox wht 11-04-27
void CPropertyList::DisplayCheckBox(CRect region, CPropTreeItem *pItem)	
{
	//����CheckBox���Ƴ���,Ĭ�ϰ�ť����ռ��������ʾ����ֵ�����1/3���ȣ���Ӧ��֤��С���ȴ������Ƴ���
	TEXTMETRIC tm;
	CClientDC dc(this);	
	CFont* pFont = GetFont();
	CFont* pOldFont = dc.SelectObject(pFont);	
	dc.GetTextMetrics(&tm);
	dc.SelectObject(pOldFont);
	CString sCheckBoxName=pItem->m_lpNodeInfo->m_strPropValue;
	if(sCheckBoxName.GetLength()<=0)	//δ��������ֵĬ����ʾ�ǻ��
	{	//����δ֧�ֵ�����״̬ 
		if(pItem->m_lpNodeInfo->GetCheck()==0)
			sCheckBoxName="��";
		else 
			sCheckBoxName="��";
	}
	int nNameLen=(sCheckBoxName.GetLength()+2)*tm.tmAveCharWidth;
	int nCheckBoxLen=max((int)(region.Width()*0.5),nNameLen);
	
	CRect rect;
	rect.left = region.left + 3;
	rect.right = rect.left + nCheckBoxLen;
	rect.bottom = region.bottom - 3;
	rect.top = region.top + 2;
	//
	m_prevSel = m_curSel;
	
	if (m_chkBtnCtrl)
		m_chkBtnCtrl.MoveWindow(rect);	
	else
	{	
		m_chkBtnCtrl.Create(sCheckBoxName,BS_AUTOCHECKBOX | WS_VISIBLE | WS_CHILD,
			rect,this,IDC_PROPCHKBOX);
		m_chkBtnCtrl.SetFont(&m_SSerif8Font);
		m_chkBtnCtrl.ModifyStyleEx(NULL,WS_EX_TRANSPARENT);
	}
	m_chkBtnCtrl.SetWindowText(sCheckBoxName);	//���ð�ť����
	m_chkBtnCtrl.ShowWindow(SW_SHOW);
}

void CPropertyList::DisplayButton(CRect region, CPropTreeItem* pItem)
{
	CString lBoxSelText = _T("");
	CRect rect;
	rect.left = region.left + 3;
	rect.right = region.right - 26;
	rect.bottom = region.bottom - 3;
	rect.top = region.top + 2;
	//displays a button if the property is a file/color/font chooser
	m_nLastBox = 2;
	m_prevSel = m_curSel;
	
	//
	int nMaxWidth=region.Width();	//��¼��������� wht 11-04-15
	if (region.Width() > 25) 
		region.left = region.right - 25;
	region.bottom -= 3;
	CString sButtonName=pItem->m_lpNodeInfo->m_sButtonName;
	if( (pItem->m_lpNodeInfo->m_controlType==PIT_BUTTON||pItem->m_lpNodeInfo->m_controlType==PIT_GRAYBUTTON)&&
		(pItem->m_lpNodeInfo->m_buttonType==BDT_POPMENU||pItem->m_lpNodeInfo->m_buttonType==BDT_POPMENUEDIT))
	{
		if(sButtonName.CompareNoCase("...")==0)
			sButtonName="��";
		else
			sButtonName+="��";
	}
	if(sButtonName.CompareNoCase("...")!=0)
	{	//���ݰ�ť���Ƶ�����ť��С�ұ�֤��СֵΪ25
		TEXTMETRIC tm;
		CClientDC dc(this);	
		CFont* pFont = GetFont();
		CFont* pOldFont = dc.SelectObject(pFont);	
		dc.GetTextMetrics(&tm);
		CSize size = dc.GetTextExtent(sButtonName,sButtonName.GetLength());
		size.cx+=10;	//10��Ϊ��ť��ʾ�߾�ԣ������ֵ wjh-2017.5.11
		dc.SelectObject(pOldFont);
		//
		int nNameLen=(pItem->m_lpNodeInfo->m_sButtonName.GetLength()+1)*tm.tmAveCharWidth;
		int nIncr=size.cx-region.Width();
		if(nIncr>=0&&nMaxWidth>nIncr)
		{	
			rect.right -= nIncr;
			region.left -= nIncr;
			//��ť����ʾ����ʱ�ʵ�������ť��� wht 11-04-14	
			region.bottom += 3;
		}
	}
	if (m_btnCtrl)
		m_btnCtrl.MoveWindow(region);
	//m_btnCtrl.Detach();	
	else
	{
		m_btnCtrl.Create("...",BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD,
			region,this,IDC_PROPBTNCTRL);
		m_btnCtrl.SetFont(&m_SSerif8Font);
	}
	m_btnCtrl.SetWindowText(sButtonName);	//���ð�ť����
	m_btnCtrl.ShowWindow(SW_SHOW);
	//m_btnCtrl.SetFocus();

	if(pItem->m_lpNodeInfo->m_controlType==PIT_BUTTON||pItem->m_lpNodeInfo->m_controlType==PIT_GRAYBUTTON)
	{
		//Ӧ��m_editBox.SetWindowText()֮ǰ���ı䵱ǰѡ���������OnChangeEditBox()
		//�е�m_pCurEditItemΪǰһ��ѡ������ǵ�ǰѡ���� wht 11-04-14
		m_pCurEditItem = pItem;
		if(!pItem->IsReadOnly()&&(
			pItem->m_lpNodeInfo->m_buttonType==BDT_FILEPATH ||	//�����ť����Ϊ�ļ�·������ʾ�༭��
			pItem->m_lpNodeInfo->m_buttonType==BDT_COMMONEDIT||pItem->m_lpNodeInfo->m_buttonType==BDT_POPMENUEDIT))	//��ť����Ϊ��ֱ�ӱ༭����
		{
			if (m_editBox)
				m_editBox.MoveWindow(rect);
			else
			{	
				m_editBox.Create(ES_LEFT | ES_AUTOHSCROLL | WS_VISIBLE | WS_CHILD,
					rect,this,IDC_PROPEDITBOX);
				m_editBox.SetFont(&m_SSerif8Font);
			}
			lBoxSelText = pItem->m_lpNodeInfo->m_strPropValue;	
			//set the text in the edit box to the property's current value
			m_bEnableTextChangeEnvent=FALSE;
			m_editBox.SetWindowText(lBoxSelText);
			m_editBox.ShowWindow(SW_SHOW);
			m_editBox.SetFocus();
			m_bEnableTextChangeEnvent=TRUE;
			int len = m_editBox.GetWindowTextLength();
			m_editBox.SetSel(0, len);
		}
	}
}

void CPropertyList::OnKillFocus(CWnd* pNewWnd) 
{
	CListBox::OnKillFocus(pNewWnd);
}

void CPropertyList::OnKillfocusCmbBox() 
{
	//�ڴ˴����øñ������ܵ���һֱΪTRUE,��ť���ʧЧ 
	//���������Ȼ���ٵ������ť�Ŀɱ༭�������ʹ��ť���ܵ�� wht 11-04-26
	//m_bPropValueModified=TRUE; 
	if(m_pCurEditItem==NULL||m_pCurEditItem->m_lpNodeInfo==NULL)
		return;
	if(m_bEscKeyDown)
		m_bEscKeyDown = FALSE;
	else if(m_pCurEditItem->m_lpNodeInfo->m_controlType==PIT_COMBO &&
		(m_pCurEditItem->m_lpNodeInfo->m_cmbType==CDT_EDIT || 
		m_pCurEditItem->m_lpNodeInfo->m_cmbType==CDT_COLOR))
	{	//�ɱ༭����������ڴ˴�����,��Ϊ�ڱ༭��Ŀʱ���ἤ����Ŀת���¼������޷�����ֵ�޸Ĵ��̺���
		CString selStr;
		if(m_pCurEditItem->m_lpNodeInfo->m_cmbType==CDT_EDIT && m_cmbBox1.GetSafeHwnd()!=NULL)
			m_cmbBox1.GetWindowText(selStr);
		else if(m_pCurEditItem->m_lpNodeInfo->m_cmbType==CDT_COLOR && m_cmbBox3.GetSafeHwnd()!=NULL)
		{
			COLORREF curClr = (DWORD)m_cmbBox3.GetItemData(m_cmbBox3.GetCurSel());
			if(curClr!=0XFFFFFFFF)
			{	//δ������ɫ�Ի���ʱִ�����´���
				m_pCurEditItem = NULL;
				HideCmbBox();
			}
			return;
		}
		if(selStr.CompareNoCase(m_pCurEditItem->m_lpNodeInfo->m_strPropValue)!=0)
			m_bPropValueModified=true;
		//ȥ��m_bPropValueModifiedΪTrue���ж� wht 11-04-26
		if( m_pCurEditItem&&(FireValueModify!=NULL&&m_bPropValueModified&&
			FireValueModify(this,m_pCurEditItem,selStr)))
		{
			m_pCurEditItem->m_lpNodeInfo->m_strPropValue = selStr;
			m_pCurEditItem->m_lpNodeInfo->m_bMutiObjsProp=FALSE;
			m_bPropValueModified=FALSE;
		}
	}
	m_pCurEditItem = NULL;
	HideCmbBox();
}

void CPropertyList::OnKillfocusBtn()
{
	if(m_btnCtrl)//&&(GetFocus()!=this||GetFocus()!=&m_editBox))
		m_btnCtrl.ShowWindow(SW_HIDE);
}
void CPropertyList::OnKillfocusEditBox()
{
	CString newStr;
	if(m_editBox)
	{
		if(m_bEscKeyDown)
			m_bEscKeyDown = FALSE;
		else
		{
			CString newStr;
			m_editBox.GetWindowText(newStr);
			//�����������ʱm_pCurEditItem->m_lpNodeInfo���Ϊ��,�����쳣����
			if( m_bPropValueModified&&m_pCurEditItem&&
				(FireValueModify==NULL||FireValueModify(this,m_pCurEditItem,newStr)))
			{
				if(m_pCurEditItem->m_lpNodeInfo->m_bMutiObjsProp||	//��ֵ��������б仯 wht 10-12-23
					m_pCurEditItem->m_lpNodeInfo->m_strPropValue.CompareNoCase(newStr)!=0)
				{
					m_pCurEditItem->m_lpNodeInfo->m_strPropValue = newStr;
					m_pCurEditItem->m_lpNodeInfo->m_bMutiObjsProp=FALSE;
					m_bPropValueModified=FALSE;
				}
			}
		}
		m_pCurEditItem = NULL;
		HideEditBox();
	}
}
void CPropertyList::OnCheckBox()
{
	CPropTreeItem* pItem = (CPropTreeItem*) GetItemDataPtr(m_curSel);
	if(pItem==(void*)-1)
		return;
	if (pItem->m_lpNodeInfo->m_controlType == PIT_CHECKBOX)
	{
		BOOL bCheck=pItem->m_lpNodeInfo->GetCheck();
		if(pItem&&(FireCheckBoxClick==NULL||FireCheckBoxClick(this,pItem)))
			pItem->m_lpNodeInfo->SetCheck(!bCheck);
		if(m_chkBtnCtrl)
		{
			if(pItem->m_lpNodeInfo->GetCheck())
				pItem->m_lpNodeInfo->m_strPropValue="��";	
			else 
				pItem->m_lpNodeInfo->m_strPropValue="��";
			m_chkBtnCtrl.SetWindowText(pItem->m_lpNodeInfo->m_strPropValue);
		}
	}
}
void CPropertyList::DisplayPopMenu(CPropTreeItem* pItem)
{
	if(pItem==(void*)-1)
		return;
	CRect rect;
	GetItemRect(pItem->m_iIndex,rect);
	rect.left = m_nDivider+1;
	if(!((pItem->m_lpNodeInfo->m_controlType==PIT_BUTTON||pItem->m_lpNodeInfo->m_controlType==PIT_GRAYBUTTON)&&
		(pItem->m_lpNodeInfo->m_buttonType==BDT_POPMENU||pItem->m_lpNodeInfo->m_buttonType==BDT_POPMENUEDIT)))
		return;
	if(m_popMenu.GetSafeHmenu()==NULL)
		m_popMenu.CreatePopupMenu();
	while(m_popMenu.GetMenuItemCount())
		m_popMenu.DeleteMenu(0,MF_BYPOSITION);
	int i=0,i2=0;
	UINT nMenuID=ID_FIRST_POPMENU;
	CString cmbItems = pItem->m_lpNodeInfo->m_cmbItems;
	int index=0,checked=pItem->m_lpNodeInfo->GetCheck();
	while ((i2=cmbItems.Find('|',i)) != -1)
	{
		CString itemname=cmbItems.Mid(i,i2-i);
		if(itemname.Compare("-")==0)	//�˵��ָ���
			m_popMenu.AppendMenu(MF_SEPARATOR,ID_SEPARATOR,"");
		else
		{
			index++;
			UINT nFlags=MF_STRING;
			if(checked<0&&itemname.Compare(pItem->m_lpNodeInfo->m_strPropValue)==0)
				nFlags|=MF_CHECKED;
			else if(checked>0&&checked==index)
				nFlags|=MF_CHECKED;
			m_popMenu.AppendMenu(nFlags,nMenuID++,itemname);
		}
		i=i2+1;
	}
	if(i<cmbItems.GetLength())
	{
		CString itemname=cmbItems.Right(cmbItems.GetLength()-i);
		index++;
		UINT nFlags=MF_STRING;
		if(checked<0&&itemname.Compare(pItem->m_lpNodeInfo->m_strPropValue)==0)
			nFlags|=MF_CHECKED;
		else if(checked>0&&checked==index)
			nFlags|=MF_CHECKED;
		m_popMenu.AppendMenu(nFlags,nMenuID++,itemname);
	}
	CPoint pt(rect.right,rect.bottom);
	ClientToScreen(&pt);
	m_popMenu.TrackPopupMenu(TPM_RIGHTALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,this);
}
void CPropertyList::OnButton()
{
	CPropTreeItem* pItem = (CPropTreeItem*) GetItemDataPtr(m_curSel);
	if(pItem==(void*)-1)
		return;
	if (pItem->m_lpNodeInfo->m_controlType == PIT_BUTTON||pItem->m_lpNodeInfo->m_controlType==PIT_GRAYBUTTON)
	{
		m_wInteralLockRef++;;//�粻���������ܻ��ں����İ�ť��������������������º��������쳣 wjh-2014.9.22
		if(pItem->m_lpNodeInfo->m_buttonType==BDT_POPMENU||pItem->m_lpNodeInfo->m_buttonType==BDT_POPMENUEDIT)
			DisplayPopMenu(pItem);
		else if(FireButtonClick==NULL||FireButtonClick(this,pItem))
		{
			if(	pItem->m_lpNodeInfo->m_buttonType==BDT_FILEPATH||pItem->m_lpNodeInfo->m_buttonType==BDT_COMMONEDIT)
			{
				if(m_editBox&&m_editBox.IsWindowVisible())
				{
					m_editBox.SetWindowText(pItem->m_lpNodeInfo->m_strPropValue);
					m_editBox.SetFocus();
					//int len = m_editBox.GetWindowTextLength();
					//m_editBox.SetSel(len, len);
				}
			}
			else
			{
				if(m_bEscKeyDown)
					m_bEscKeyDown = FALSE;
				else if(m_pCurEditItem&&FireValueModify)
				{
					FireValueModify(this,pItem,pItem->m_lpNodeInfo->m_strPropValue);
					m_pCurEditItem->m_lpNodeInfo->m_bMutiObjsProp = FALSE;				
				}
				/*
				POSITION pos = m_pCurEditItem->m_listChild.GetHeadPosition();
				while(pos!=NULL)
				{
					CPropTreeItem *pPropItem = (CPropTreeItem*)m_pCurEditItem->m_listChild.GetNext(pos);
					if(pPropItem&&pPropItem->FireValueModify && 
						pPropItem->FireValueModify(pPropItem,pPropItem->m_lpNodeInfo->m_strPropValue))
						pPropItem->m_lpNodeInfo->m_bMutiObjsProp = FALSE;
						
				}*/
				Invalidate();
			}
		}
		m_wInteralLockRef--;
	}
}

void CPropertyList::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetClientRect(rect);
	if (m_bTracking)
	{
		//if columns were being resized then this indicates
		//that mouse is up so resizing is done.  Need to redraw
		//columns to reflect their new widths.
		
		m_bTracking = FALSE;
		//if mouse was captured then release it
		if (GetCapture()==this)
			::ReleaseCapture();

		::ClipCursor(NULL);

		CClientDC dc(this);
		if(point.x<50)	//���Ʒָ��߲���̫�����
			point.x=50;
		if(point.x>(rect.right-40))	//���Ʒָ��߲���̫���Ҳ�
			point.x = rect.right - 40;
		InvertLine(&dc,CPoint(point.x,m_nDivTop),CPoint(point.x,m_nDivBtm));
		//set the divider position to the new value
		m_nDivider = point.x;

		//redraw
		Invalidate();
	}
	else
	{
		BOOL loc;
		int i = ItemFromPoint(point,loc);
		m_curSel = i;
		FireToolTip(point);
		CListBox::OnLButtonUp(nFlags, point);
	}
}

void CPropertyList::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ((point.x>=m_nDivider-5) && (point.x<=m_nDivider+5))
	{
		//if mouse clicked on divider line, then start resizing

		::SetCursor(m_hCursorSize);

		CRect windowRect;
		GetWindowRect(windowRect);
		windowRect.left += 10; windowRect.right -= 10;
		//do not let mouse leave the list box boundary
		::ClipCursor(windowRect);
		
		HideCmbBox();
		if (m_editBox)
			HideEditBox();
		m_pCurEditItem = NULL;

		CRect clientRect;
		GetClientRect(clientRect);

		m_bTracking = TRUE;
		m_nDivTop = clientRect.top;
		m_nDivBtm = clientRect.bottom;
		m_nOldDivX = point.x;

		CClientDC dc(this);
		InvertLine(&dc,CPoint(m_nOldDivX,m_nDivTop),CPoint(m_nOldDivX,m_nDivBtm));

		//capture the mouse
		SetCapture();
	}
	else
	{
		CListBox::OnLButtonDown(nFlags, point);
		m_bTracking = FALSE;
		BOOL bSelect=1;
		LVHITTESTINFO ht;
		bSelect = HitTestOnSign(point, ht);
		if(bSelect && ht.iItem!=-1)
		{
			//OnControlLButtonDown(nFlags, point, ht);
			//update row anyway for selection bar
			//CRect rc;
			//GetItemRect(ht.iItem, rc);
			//InvalidateRect(rc);
			//UpdateWindow();
		}
		FireToolTip(point);
	}
}

void CPropertyList::OnMouseMove(UINT nFlags, CPoint point) 
{	
	FireToolTip(point);
	
	CRect rect;
	GetClientRect(rect);
	
	if (m_bTracking && point.x>50 && point.x<(rect.right-40))
	{
		//move divider line to the mouse pos. if columns are
		//currently being resized
		CClientDC dc(this);
		//remove old divider line
		InvertLine(&dc,CPoint(m_nOldDivX,m_nDivTop),CPoint(m_nOldDivX,m_nDivBtm));
		//draw new divider line
		InvertLine(&dc,CPoint(point.x,m_nDivTop),CPoint(point.x,m_nDivBtm));
		m_nOldDivX = point.x;
	}
	else if ((point.x >= m_nDivider-5) && (point.x <= m_nDivider+5))
		//set the cursor to a sizing cursor if the cursor is over the row divider
		::SetCursor(m_hCursorSize);
	else
		CListBox::OnMouseMove(nFlags, point);
}

void CPropertyList::FireToolTip(CPoint point)
{
	BOOL IsOutside = false;   
	int iIndex = ItemFromPoint(point,IsOutside);   
	if(!IsOutside && iIndex!=-1)
	{
		RECT rc;
		GetItemRect(iIndex, &rc);
		CPropTreeItem* pItem = (CPropTreeItem*)GetItemDataPtr(iIndex);
		if(pItem==(void*)-1||pItem==NULL)
			return;
		CClientDC dc(this);
		CFont* pOldFont = dc.SelectObject(&m_SSerif8Font);
		CSize size = dc.GetTextExtent(pItem->m_lpNodeInfo->m_strPropName);
		dc.SelectObject(pOldFont);
		if(m_ToolTips==NULL)
			m_ToolTips = new CToolTipCtrl();
		if(m_ToolTips->GetSafeHwnd()==NULL)
			m_ToolTips->Create(this);
		if(size.cx+pItem->m_nIndent*12>=m_nDivider-1)
		{
			m_ToolTips->AddTool(this,pItem->m_lpNodeInfo->m_strPropName);
			ClientToScreen(&rc);   
			m_ToolTips->Activate(TRUE);   
		}
		else
			m_ToolTips->Activate(FALSE);   
	}
}

void CPropertyList::InvertLine(CDC* pDC,CPoint ptFrom,CPoint ptTo)
{
	int nOldMode = pDC->SetROP2(R2_NOT);
	
	pDC->MoveTo(ptFrom);
	pDC->LineTo(ptTo);

	pDC->SetROP2(nOldMode);
}

void CPropertyList::PreSubclassWindow() 
{
	m_bDivIsSet = FALSE;
	m_nDivider = 0;
	m_bTracking = FALSE;
	m_curSel = 1;

	m_hCursorSize = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
	m_hCursorArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

	if(m_SSerif8Font.GetSafeHandle()==NULL)
	{
		CFont *pFont=GetFont();
		LOGFONT logFont;
		if(pFont)
			pFont->GetLogFont(&logFont);
		m_SSerif8Font.CreateFontIndirect(&logFont);
	}
}

void CPropertyList::CleanPropTreeItem(CPropTreeItem *pItem)
{
	// delete child nodes
	POSITION pos = pItem->m_listChild.GetHeadPosition();
	while (pos != NULL)
	{
		CPropTreeItem* pItem=NULL;
		pItem=(CPropTreeItem*)pItem->m_listChild.GetNext(pos);
		if(pItem!=NULL)
			delete pItem;
	}
	pItem->m_listChild.RemoveAll();
}

void CPropertyList::InternalUpdateTreeItemIndex()
{
	int nItems = GetCount();
	for(int iItem=0; iItem < nItems; iItem++)
	{
		CPropTreeItem* pItem = (CPropTreeItem*)GetItemDataPtr(iItem);
		if(pItem!=(void*)-1)
			pItem->m_iIndex = iItem;
	}
}

//insert item and return new parent pointer.
CPropTreeItem* CPropertyList::InsertItem(CPropTreeItem *pParent,CItemInfo* lpInfo, int iInsertIndex, BOOL bUpdate)
{
	CPropTreeItem *pItem = new CPropTreeItem();
	if(lpInfo==NULL)
		lpInfo = new CItemInfo;

	if(pParent==NULL)
		return NULL;
	pItem->m_lpNodeInfo = lpInfo;
	pItem->m_nIndent = pParent->m_nIndent+1;
	pItem->m_pParent = pParent;
	//add as the last child 
	CPropTreeItem *pTailItem=NULL,*pPrevItem=NULL;
	int insert_index=pParent->m_iIndex+1;
	if(!pParent->m_listChild.IsEmpty())
	{
		pTailItem = (CPropTreeItem*)pParent->m_listChild.GetTail();
		insert_index=pTailItem->GetTailIndex()+1;
	}
	if(iInsertIndex<0||pParent->m_listChild.GetCount()==0)
		pParent->m_listChild.AddTail(pItem);
	else
	{
		int i=0;
		insert_index=pParent->m_iIndex+1;
		POSITION pos=NULL;
		for(pos = pParent->m_listChild.GetHeadPosition();pos!=NULL;i++)
		{
			if(i==iInsertIndex)
			{
				pParent->m_listChild.InsertBefore(pos,pItem);
				if(pPrevItem)
					insert_index=pPrevItem->GetTailIndex()+1;
				break;
			}
			pPrevItem=(CPropTreeItem*)pParent->m_listChild.GetNext(pos);
		}
		if(pos==NULL)
		{
			pParent->m_listChild.AddTail(pItem);
			insert_index=pPrevItem->GetTailIndex()+1;
		}
	}
	pItem->m_bHideSelf = pParent->m_bHideChildren||pParent->m_bHideSelf;
	if(bUpdate)
	{
		//if(!pItem->m_bHideSelf&&(pItem->m_dwPropGroup&GetBinaryWord(m_iPropGroup))!=0)
		if(!pItem->m_bHideSelf)
		{	//��ʱ�ر��ػ湦��(�������������һ����Ŀʱ��InsertString���ܴ���DrawItem,
			//��ʱ��δ����SetItemDataPtr,�ᵼ�»��ƴ���) wht 16-02-02
			SetRedraw(FALSE);	
			pItem->m_iIndex =InsertString(insert_index,_T(""));
			SetItemDataPtr(pItem->m_iIndex,pItem);
			SetRedraw(TRUE);
		}
		InternalUpdateTreeItemIndex();//better do this
	}
	return pItem;
}
//expand one folder and return the last index of the expanded folder
int CPropertyList::Expand(CPropTreeItem* pSelItem, int nIndex)
{
	if(pSelItem->m_listChild.GetCount()>0)
	{
		pSelItem->m_bHideChildren = FALSE;
		if(FireStatusModify)
			FireStatusModify(this,pSelItem);
		//expand children
		POSITION pos = pSelItem->m_listChild.GetHeadPosition();
		while(pos != NULL)
		{
			CPropTreeItem* pNextItem = (CPropTreeItem*)pSelItem->m_listChild.GetNext(pos);
			//չ������ʱӦ�޸�����������(������ǰ����ʾ������)��m_bHideSelfֵ��
			//����ĳ����(������)ͬʱ���ڲ�ͬ����A��Bʱ����A������չ����Ȼ���л���B
			//����ʱ����������л���A����ʱ����������ʾ������.	wht 09-10-09 
			if(pNextItem==NULL)
				continue;
			if((pNextItem->m_dwPropGroup&GetBinaryWord(m_iPropGroup))!=0)
			{	
				nIndex = InsertString(nIndex+1,_T(""));
				SetItemDataPtr(nIndex,pNextItem);
			}
			pNextItem->m_bHideSelf=FALSE;
			if(!pNextItem->m_bHideChildren&&pNextItem->m_listChild.GetCount()>0)
				nIndex = Expand(pNextItem,nIndex);
		}
	}
	InternalUpdateTreeItemIndex();
	HideInputCtrl();	//���ر༭�ؼ�
	return nIndex;
}
//collapse all children from pItem
void CPropertyList::Collapse(CPropTreeItem *pItem)
{
	if(pItem != NULL && pItem->m_listChild.GetCount()>0)
	{
		SetRedraw(0);
		int nIndex = pItem->m_iIndex;			
		HideChildren(pItem, nIndex+1);
		pItem->m_bHideChildren=TRUE;
		if(FireStatusModify)
			FireStatusModify(this,pItem);
		InternalUpdateTreeItemIndex();
		HideInputCtrl();	//���ر༭�ؼ�
		SetRedraw(1);
	}
}

//walk all over the place setting the hide/show flag of the nodes.
//it also deletes items from the listviewctrl.
void CPropertyList::HideChildren(CPropTreeItem *pItem,int nItem)
{
	if(!pItem->m_bHideChildren&&pItem->m_listChild.GetCount()>0)
	{
		POSITION pos = pItem->m_listChild.GetHeadPosition();
		while (pos != NULL)
		{
			CPropTreeItem *pSonItem = (CPropTreeItem *)pItem->m_listChild.GetNext(pos);
			if(pSonItem==NULL)
				continue;
			pSonItem->m_bHideSelf = TRUE;
			if((pSonItem->m_dwPropGroup&GetBinaryWord(m_iPropGroup))==0)
				continue;
			HideChildren(pSonItem,nItem+1);
			DeleteString(nItem);
		}
	}
}
//see if user clicked the [+] [-] sign.
BOOL CPropertyList::HitTestOnSign(CPoint point, LVHITTESTINFO& ht)
{
	ht.pt = point;
	ht.iItem = GetCurSel();
	if(ht.iItem!=-1)
	{
		CPropTreeItem* pItem = (CPropTreeItem*)GetItemDataPtr(ht.iItem);
		if(pItem==(void*)-1)
			return FALSE;
		if(pItem!=NULL)
		{
			//if haschildren and clicked on + or - then expand/collapse
			if(pItem->m_listChild.GetCount()>0)
			{
				//hittest on the plus/sign "button" 
				//see the DrawTreeItem for setting up the plus/sign "button" 
				CRect rcBounds;
				GetItemRect(ht.iItem, rcBounds);
				//setup plus/minus rectangle
				int nBottomDown = (rcBounds.top+rcBounds.bottom)/2;
				int symbol_center_x = rcBounds.left  + (pItem->m_nIndent-1)*12+5;
				POINT box_lefttop = {symbol_center_x-4, nBottomDown-4};
				SIZE right_bottom = {9,9};
				CRect rc(box_lefttop, right_bottom);

				if(rc.PtInRect(point))
				{
					SetRedraw(0);
					int nScrollIndex=0;
					HideInputCtrl();
					if(pItem->m_bHideChildren)
						nScrollIndex = Expand(pItem, ht.iItem);
					else
					   Collapse(pItem);
					SetRedraw(1);
					CRect rc;
					GetItemRect(ht.iItem, rc);
					InvalidateRect(rc);
					UpdateWindow();
					//EnsureVisible(nScrollIndex,1);
					return 0;
				}
			}//has kids
		}//pItem!=NULL
	}
	return 1;
}

void CPropertyList::OnDblclk() 
{
	int iCurItem = GetCurSel();
	if(iCurItem!=-1)
	{
		CPropTreeItem* pItem = (CPropTreeItem*)GetItemDataPtr(iCurItem);
		if(pItem==(void*)-1)
			return;
		if(pItem!=NULL&&pItem!=(void*)-1)
		{
			//if haschildren and clicked on + or - then expand/collapse
			if(pItem->m_listChild.GetCount()>0)
			{
				//hittest on the plus/sign "button" 
				//see the DrawTreeItem for setting up the plus/sign "button" 
				CRect rcBounds;
				GetItemRect(iCurItem, rcBounds);
				
				SetRedraw(0);
				HideInputCtrl();
				int nScrollIndex=0;
				if(pItem->m_bHideChildren)
					nScrollIndex = Expand(pItem, iCurItem);
				else
					Collapse(pItem);
				SetRedraw(1);
				CRect rc;
				GetItemRect(iCurItem, rc);
				InvalidateRect(rc);
				UpdateWindow();
			}//has kids
		}//pItem!=NULL
	}
}

BOOL CPropertyList::PreTranslateMessage(MSG* pMsg) 
{
	InternalUpdateTreeItemIndex();
	switch(pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		if(m_ToolTips==NULL)
			m_ToolTips = new CToolTipCtrl();
		if(m_ToolTips->GetSafeHwnd()==NULL)
			m_ToolTips->Create(this);
		m_ToolTips->RelayEvent(pMsg);
	}
	CPropTreeItem* pItem=NULL;
	if(pMsg->message == WM_KEYDOWN)
	{
		switch( pMsg->wParam )
		{
		case VK_LEFT:
		case VK_UP:
			if(this==GetFocus())
			{
				// Decrement the order number.
				m_curSel--;
				if(m_curSel < 0) 
					m_curSel = 0;
				SetCurSel(m_curSel);
				OnSelchange();
				return TRUE;
			}
			break;
		case VK_RIGHT:
		case VK_DOWN:
			if(this==GetFocus())
			{
				// Increment the order number.
				m_curSel++;
				int nColumnCount = GetCount();;
				// Don't go beyond the last column.
				if( m_curSel > nColumnCount -1 ) 
					m_curSel = nColumnCount-1;
				SetCurSel(m_curSel);
				OnSelchange();
				return TRUE;
			}
			break;
		case VK_RETURN://[+]/[-]
			m_curSel = GetCurSel();
			SetFocus();
			pItem = (CPropTreeItem*) GetItemDataPtr(m_curSel);
			if(pItem==(void*)-1)
				break;
			if(pItem&&pItem->m_listChild.GetCount()>0)
			{
				SetRedraw(0);
				HideInputCtrl();
				int nScrollIndex=0;
				if(pItem->m_bHideChildren)
					nScrollIndex = Expand(pItem, m_curSel);
				else
					Collapse(pItem);
				SetRedraw(1);
			}//has kids
			else
			{
				m_curSel++;
				if(m_curSel==GetCount())
					m_curSel--;
				SetCurSel(m_curSel);
				m_bEnterKeyDown=TRUE;	//���»س���
				OnSelchange();	
				m_bEnterKeyDown=FALSE;	//������OnSelChange()����ΪFALSE
			}
			break;
		case VK_ESCAPE:
			m_curSel = GetCurSel();
			m_bEscKeyDown = TRUE;
			SetFocus();
			break;
		default:
			break;
		}
	}	
	return CListBox::PreTranslateMessage(pMsg);
}

void CPropertyList::CleanTree()
{	
	while(GetSafeHwnd()!=NULL&&GetCount()>0)
		DeleteString(0);
	CleanPropTreeItem(&root);
	m_pCurEditItem=NULL;	//û�д��д���,��ĳЩ����������õı༭��Ƚ���ʧЧ�����лᵼ���쳣wjh-2010-3-24
	CPropTreeItem::m_bDefaultReadOnlyState=FALSE;	//��ʼ��������ֻ������Ĭ��ֵ wht 12-10-16
}
void CPropertyList::CleanCallBackFunc()
{
	m_pCurEditItem = NULL;
	//m_hPromptWnd = NULL;
	FireCheckBoxClick = NULL;
	FireButtonClick = NULL;
	FireValueModify = NULL;
	FireStatusModify = NULL;
	FireHelpPrompt = NULL;
	m_bEnterKeyDown = FALSE;
}
void CPropertyList::Redraw()
{
	UINT vPos=GetScrollPos(SB_VERT);
	HideInputCtrl();
	SetRedraw(FALSE);
	while(GetCount()>0)
		DeleteString(0);
	CPropTreeItem *pRoot = GetRootItem();
	POSITION pos = pRoot->m_listChild.GetHeadPosition();
	while(pos!=NULL)
	{
		CPropTreeItem *pItem = (CPropTreeItem*)pRoot->m_listChild.GetNext(pos);
		if(pItem&&(pItem->m_dwPropGroup&GetBinaryWord(m_iPropGroup))==0)
			continue;
		if(pItem&&!pItem->m_bHideSelf)
			InternalAppendItem(pRoot,pItem);
	}
	SetRedraw(TRUE);
	/*SetScrollPos(SB_VERT,vPos);
	CRect scrollRect;
	GetClientRect(&scrollRect);
	ScrollWindow(0,vPos*100,&scrollRect,&scrollRect);*/
	InternalUpdateTreeItemIndex();//better do this
}

void CPropertyList::InternalAppendItem(CPropTreeItem *pParent, CPropTreeItem *pItem)
{
	if(!pItem->m_bHideSelf)
	{
		int nIndex = AddString(_T(""));
		pItem->m_iIndex = nIndex;
		SetItemDataPtr(nIndex,pItem);
		POSITION pos = pItem->m_listChild.GetHeadPosition();
		while(pos!=NULL)
		{
			CPropTreeItem *pSonItem = (CPropTreeItem*)pItem->m_listChild.GetNext(pos);
			if(pSonItem&&(pSonItem->m_dwPropGroup&GetBinaryWord(m_iPropGroup))==0)
				continue;
			if(pSonItem&&!pSonItem->m_bHideSelf&&!pItem->m_bHideChildren)
				InternalAppendItem(pItem,pSonItem);
		}
	}
}

void CPropertyList::OnSize(UINT nType, int cx, int cy) 
{
	CListBox::OnSize(nType, cx, cy);
	RECT rc;
	if(m_btnCtrl)
	{
		m_btnCtrl.GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.left+=cx-rc.right;
		rc.right=cx;
		m_btnCtrl.MoveWindow(&rc);
	}
	if(m_editBox)
	{
		m_editBox.GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.right=cx;
		m_editBox.MoveWindow(&rc);
	}
	if(m_cmbBox1)
	{
		m_cmbBox1.GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.right=cx;
		m_cmbBox1.MoveWindow(&rc);
	}
	if(m_cmbBox2)
	{
		m_cmbBox2.GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.right=cx;
		m_cmbBox2.MoveWindow(&rc);
	}
	if(m_cmbBox3)
	{
		m_cmbBox3.GetWindowRect(&rc);
		ScreenToClient(&rc);
		rc.right=cx;
		m_cmbBox3.MoveWindow(&rc);
	}
}

void CPropertyList::HideEditBox()
{
	if(m_editBox)
	{
		RECT rc;
		m_editBox.GetWindowRect(&rc);
		m_editBox.ShowWindow(SW_HIDE);
		ScreenToClient(&rc);
		rc.left-=3;	//�༭��ʵ�ʳߴ����ռ�óߴ�ҪС������Ҫ��ԭ�ػ������С
		rc.top-=3;
		InvalidateRect(&rc);
	}
}

void CPropertyList::HideCmbBox()
{
	if(m_cmbBox1)
	{
		RECT rc;
		m_cmbBox1.GetWindowRect(&rc);
		m_cmbBox1.ShowWindow(SW_HIDE);
		ScreenToClient(&rc);
		rc.left-=3;	//�༭��ʵ�ʳߴ����ռ�óߴ�ҪС������Ҫ��ԭ�ػ������С
		rc.top-=3;
		InvalidateRect(&rc);
	}
	if(m_cmbBox2)
	{
		RECT rc;
		m_cmbBox2.GetWindowRect(&rc);
		m_cmbBox2.ShowWindow(SW_HIDE);
		ScreenToClient(&rc);
		rc.left-=3;	//�༭��ʵ�ʳߴ����ռ�óߴ�ҪС������Ҫ��ԭ�ػ������С
		rc.top-=3;
		InvalidateRect(&rc);
	}
	if(m_cmbBox3)
	{
		RECT rc;
		m_cmbBox3.GetWindowRect(&rc);
		m_cmbBox3.ShowWindow(SW_HIDE);
		ScreenToClient(&rc);
		rc.left-=3;	//�༭��ʵ�ʳߴ����ռ�óߴ�ҪС������Ҫ��ԭ�ػ������С
		rc.top-=3;
		InvalidateRect(&rc);
	}
}


CPropTreeItem* CPropertyList::InternalFindItemByPropId(long prop_id,CPropTreeItem *pItem, CPropTreeItem **ppParentItem)
{
	POSITION pos = pItem->m_listChild.GetHeadPosition();
	while(pos)
	{
		CPropTreeItem *pFindItem=(CPropTreeItem*)pItem->m_listChild.GetNext(pos);
		if(pFindItem->m_idProp==prop_id)
		{
			if(ppParentItem)
				*ppParentItem=pItem;
			return pFindItem;
		}
		else
			pFindItem=InternalFindItemByPropId(prop_id,pFindItem,ppParentItem);
		if(pFindItem)
			return pFindItem;
	}
	return NULL;
}

CPropTreeItem* CPropertyList::FindItemByPropId(long prop_id,CPropTreeItem **ppParentItem)
{
	CPropTreeItem *pRoot=GetRootItem();
	POSITION pos = pRoot->m_listChild.GetHeadPosition();
	while(pos)
	{
		CPropTreeItem *pItem=(CPropTreeItem*)pRoot->m_listChild.GetNext(pos);
		if(pItem->m_idProp==prop_id)
		{
			if(ppParentItem)
				*ppParentItem=pRoot;
			return pItem;
		}
		else
			pItem=InternalFindItemByPropId(prop_id,pItem,ppParentItem);
		if(pItem)
			return pItem;
	}
	return NULL;
}

BOOL CPropertyList::SetItemPropHelpStr(long prop_id, const char *sFormatStr,...)
{
	va_list ap;
	va_start(ap, sFormatStr);
	CString sHelpText;
	sHelpText.FormatV(sFormatStr,ap);
	va_end(ap);
	//
	CPropTreeItem *pParentItem;
	CPropTreeItem *pItem = FindItemByPropId(prop_id,&pParentItem);
	if(pItem==NULL)
		return FALSE;
	pItem->m_lpNodeInfo->m_strPropHelp = sHelpText;
	if(pItem&&!pItem->m_bHideSelf)	//��ǰ������ʾ״̬
	{
		RECT rc;
		GetItemRect(pItem->m_iIndex,&rc);
		InvalidateRect(&rc);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CPropertyList::SetItemPropValue(long prop_id, const char *sFormatStr,...)
{
	va_list ap;
	va_start(ap, sFormatStr);
	CString sValueText;
	sValueText.FormatV(sFormatStr,ap);
	va_end(ap);
	return SetItemPropValue(prop_id,sValueText);
}

BOOL CPropertyList::SetItemPropValue(long prop_id, CString &sValueText)
{
	CPropTreeItem *pParentItem;
	CPropTreeItem *pItem = FindItemByPropId(prop_id,&pParentItem);
	if(pItem==NULL)
		return FALSE;
	//if(pItem->m_bReadOnly)
	//	return FALSE;	//ֻ����ܸ���ֵ
	pItem->m_lpNodeInfo->m_strPropValue = sValueText;
	if(pItem&&!pItem->m_bHideSelf)	//��ǰ������ʾ״̬
	{
		RECT rc;
		GetItemRect(pItem->m_iIndex,&rc);
		InvalidateRect(&rc);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CPropertyList::SetItemPropName(long prop_id, CString &sPropName)
{
	CPropTreeItem *pItem = FindItemByPropId(prop_id,NULL);
	if(pItem==NULL)
		return FALSE;
	pItem->m_lpNodeInfo->m_strPropName = sPropName;
	if(pItem&&!pItem->m_bHideSelf)	//��ǰ������ʾ״̬
	{
		RECT rc;
		GetItemRect(pItem->m_iIndex,&rc);
		InvalidateRect(&rc);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CPropertyList::SetItemReadOnly(long prop_id,BOOL bReadOnly)
{
	CPropTreeItem *pParentItem;
	CPropTreeItem *pItem = FindItemByPropId(prop_id,&pParentItem);
	if(pItem==NULL)
		return FALSE;
	pItem->m_bReadOnly=bReadOnly;
	RECT rc;
	GetItemRect(pItem->m_iIndex,&rc);
	InvalidateRect(&rc);
	return TRUE;
}
BOOL CPropertyList::SetItemRedrawState(long prop_id)
{
	CPropTreeItem *pParentItem;
	CPropTreeItem *pItem = FindItemByPropId(prop_id,&pParentItem);
	if(pItem==NULL)
		return FALSE;
	RECT rc;
	GetItemRect(pItem->m_iIndex,&rc);
	InvalidateRect(&rc);
	return TRUE;
}

void CPropertyList::SetAllSonItemsReadOnly(CPropTreeItem *pParentItem,BOOL bReadOnly/*=TRUE*/)
{
	POSITION pos = pParentItem->m_listChild.GetHeadPosition();
	while(pos)
	{
		CPropTreeItem *pSonItem=(CPropTreeItem*)pParentItem->m_listChild.GetNext(pos);
		pSonItem->m_bReadOnly=bReadOnly;
		SetAllSonItemsReadOnly(pSonItem,bReadOnly);
	}
}

BOOL CPropertyList::GetItemPropValue(long prop_id, CString &sValueText)
{
	CPropTreeItem *pParentItem;
	CPropTreeItem *pItem = FindItemByPropId(prop_id,&pParentItem);
	if(pItem)
	{
		sValueText = pItem->m_lpNodeInfo->m_strPropValue;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CPropertyList::DeleteItemByPropId(long prop_id)
{
	CPropTreeItem *pItem,*pParentItem;
	pItem=FindItemByPropId(prop_id,&pParentItem);
	if(pItem)
		return DeletePropTreeItem(pItem,pParentItem);
	else
		return FALSE;
}

void CPropertyList::DeleteAllSonItems(CPropTreeItem *pParentItem)
{
	POSITION pos = pParentItem->m_listChild.GetHeadPosition();
	while(pos)
	{
		CPropTreeItem *pSonItem=(CPropTreeItem*)pParentItem->m_listChild.GetNext(pos);
		DeletePropTreeItem(pSonItem,pParentItem);
	}
}

BOOL CPropertyList::DeletePropTreeItem(CPropTreeItem *pItem, CPropTreeItem *pParentItem)
{
	POSITION pos2,pos = pItem->m_listChild.GetHeadPosition();
	while(pos)
	{
		CPropTreeItem *pSonItem=(CPropTreeItem*)pItem->m_listChild.GetNext(pos);
		DeletePropTreeItem(pSonItem,pItem);
	}
	for(pos=pParentItem->m_listChild.GetHeadPosition();(pos2=pos)!=NULL;)
	{
		CPropTreeItem *pTempItem=(CPropTreeItem*)pParentItem->m_listChild.GetNext(pos);
		if(pTempItem==pItem)
		{
			pParentItem->m_listChild.RemoveAt(pos2);
			break;
		}
	}
	if(!pItem->IsHideSelf()&&(pItem->m_dwPropGroup&GetBinaryWord(m_iPropGroup)))
	{
		DeleteString(pItem->m_iIndex);
		InternalUpdateTreeItemIndex();
	}
	//delete pItem->m_lpNodeInfo;
	delete pItem;
	return NULL;
}

void CPropertyList::HideInputCtrl()
{
	HideCmbBox();
	HideEditBox();
	if(m_btnCtrl)
		m_btnCtrl.ShowWindow(SW_HIDE);
	if(m_chkBtnCtrl)
		m_chkBtnCtrl.ShowWindow(SW_HIDE);
}

void CPropertyList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CRect rect;
	if(GetItemRect(m_curSel,rect)!=LB_ERR)
		InvalidateRect(&rect,FALSE);
	CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CPropertyList::SetPromptString(CString sPrompt)
{
	if(m_hPromptWnd!=NULL)
	{
		::SetWindowText(m_hPromptWnd,sPrompt);
		::UpdateWindow(m_hPromptWnd);
	}
}

BOOL CPropertyList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	BOOL bRet = CListBox::OnMouseWheel(nFlags, zDelta, pt);
	if(bRet)
	{
		//�޹����������������ײ��򶥲�ʱ�ڴ˴�ˢ�»������������˸ wht 11-04-01
		//Invalidate(FALSE); 
		OnSelchange();
	}
	return bRet;
}

void CPropertyList::SelectItem(int index,BOOL bDisplayCtrl/*=FALSE*/)
{
	m_curSel = SetCurSel(index);
	if(bDisplayCtrl)
	{
		CRect rect;
		GetItemRect(index,rect);
		int nDivider = m_nDivider;
		if (nDivider==0)	//ͨ���ָ��������m_nDivider wht 10-12-02
			nDivider = (int)(rect.Width()*m_fDividerScale);
		rect.left += nDivider;
		ClientToScreen(rect);
		SetCursorPos((int)(rect.left+rect.Width()*0.25),(int)((rect.bottom+rect.top)*0.5));
		SetFocus();
		OnSelchange();
	}
}
CPropTreeItem* CPropertyList::GetSelectedItem()
{
	void* pItem = GetItemDataPtr(GetCurSel());
	if(pItem==(void*)-1)
		return NULL;
	else
		return (CPropTreeItem*)pItem;
}
void CPropertyList::OnClickPopMenu(UINT nID)
{
	if(nID<ID_FIRST_POPMENU||nID>ID_FIRST_POPMENU+100)
		return;
	CPropTreeItem* pItem = (CPropTreeItem*)GetItemDataPtr(GetCurSel());
	if(pItem==(void*)-1)
		return;
	CString sMenuName;
	m_popMenu.GetMenuString(nID,sMenuName,MF_BYCOMMAND);
	int nStartId=ID_FIRST_POPMENU;
	if(FirePopMenuClick)
		FirePopMenuClick(this,pItem,sMenuName,nID-nStartId);
}