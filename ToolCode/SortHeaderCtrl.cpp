#include "stdafx.h"
#include "SortHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSortHeaderCtrl::CSortHeaderCtrl()
	: m_iSortColumn( -1 )
	, m_bSortAscending( TRUE )
	, m_R(153)    
	, m_G(204)    
	, m_B(255)    
	, m_Gradient(3)
{    
	m_heightCoef = 1.2;    
	m_fontHeight = 15;    
	m_fontWidth = 0;    
	m_colorText = RGB(255,255,255);
	m_bEnableSort=FALSE;
	m_bCustomDrawTitle=FALSE;
}

CSortHeaderCtrl::~CSortHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CSortHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CSortHeaderCtrl)
	ON_WM_PAINT()    
	ON_MESSAGE(HDM_LAYOUT, OnLayout)  
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSortHeaderCtrl message handlers

void CSortHeaderCtrl::SetSortArrow( const int iSortColumn, const BOOL bSortAscending )
{
	m_iSortColumn = iSortColumn;
	m_bSortAscending = bSortAscending;

	// change the item to owner drawn.
	HD_ITEM hditem;

	if(iSortColumn>=0)
	{
		hditem.mask = HDI_FORMAT;
		VERIFY( GetItem( iSortColumn, &hditem ) );
		hditem.fmt |= HDF_OWNERDRAW;
		VERIFY( SetItem( iSortColumn, &hditem ) );
	}
	// invalidate the header control so it gets redrawn
	Invalidate();
}


void CSortHeaderCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	if(!m_bEnableSort)
		return;
	// attath to the device context.
	CDC dc;
	VERIFY( dc.Attach( lpDrawItemStruct->hDC ) );
	// save the device context.
	const int iSavedDC = dc.SaveDC();
	// get the column rect.
	CRect rc( lpDrawItemStruct->rcItem );

	// set the clipping region to limit drawing within the column.
	CRgn rgn;
	VERIFY( rgn.CreateRectRgnIndirect( &rc ) );
	(void)dc.SelectObject( &rgn );
	VERIFY( rgn.DeleteObject() );

	// draw the background,
	CBrush brush( GetSysColor( COLOR_3DFACE ) );
	dc.FillRect( rc, &brush );

	// get the column text and format.
	TCHAR szText[ 256 ];
	HD_ITEM hditem;

	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = szText;
	hditem.cchTextMax = 255;

	VERIFY( GetItem( lpDrawItemStruct->itemID, &hditem ) );

	// determine the format for drawing the column label.
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS ;

	if( hditem.fmt & HDF_CENTER)
		uFormat |= DT_CENTER;
	else if( hditem.fmt & HDF_RIGHT)
		uFormat |= DT_RIGHT;
	else
		uFormat |= DT_LEFT;

	// adjust the rect if the mouse button is pressed on it.
	if( lpDrawItemStruct->itemState == ODS_SELECTED )
	{
		rc.left++;
		rc.top += 2;
		rc.right++;
	}

	CRect rcIcon( lpDrawItemStruct->rcItem );
	const int iOffset = ( rcIcon.bottom - rcIcon.top ) / 4;

	// adjust the rect further if the sort arrow is to be displayed.
	if( lpDrawItemStruct->itemID == (UINT)m_iSortColumn )
		rc.right -= 3 * iOffset;

	rc.left += iOffset;
	rc.right -= iOffset;

	// draw the column label.
	if( rc.left < rc.right )
		(void)dc.DrawText( szText, -1, rc, uFormat );

	// draw the sort arrow.
	if( lpDrawItemStruct->itemID == (UINT)m_iSortColumn )
	{
		// set up the pens to use for drawing the arrow.
		CPen penLight( PS_SOLID, 1, GetSysColor( COLOR_3DHILIGHT ) );
		CPen penShadow( PS_SOLID, 1, GetSysColor( COLOR_3DSHADOW ) );
		CPen* pOldPen = dc.SelectObject( &penLight );

		if( m_bSortAscending )
		{
			// draw the arrow pointing upwards.
			dc.MoveTo( rcIcon.right - 2 * iOffset, iOffset);
			dc.LineTo( rcIcon.right - iOffset, rcIcon.bottom - iOffset - 1 );
			dc.LineTo( rcIcon.right - 3 * iOffset - 2, rcIcon.bottom - iOffset - 1 );
			(void)dc.SelectObject( &penShadow );
			dc.MoveTo( rcIcon.right - 3 * iOffset - 1, rcIcon.bottom - iOffset - 1 );
			dc.LineTo( rcIcon.right - 2 * iOffset, iOffset - 1);		
		}
		else
		{
			// draw the arrow pointing downwards.
			dc.MoveTo( rcIcon.right - iOffset - 1, iOffset );
			dc.LineTo( rcIcon.right - 2 * iOffset - 1, rcIcon.bottom - iOffset );
			(void)dc.SelectObject( &penShadow );
			dc.MoveTo( rcIcon.right - 2 * iOffset - 2, rcIcon.bottom - iOffset );
			dc.LineTo( rcIcon.right - 3 * iOffset - 1, iOffset );
			dc.LineTo( rcIcon.right - iOffset - 1, iOffset );		
		}
		// restore the pen.
		(void)dc.SelectObject( pOldPen );
	}
	// restore the previous device context.
	VERIFY( dc.RestoreDC( iSavedDC ) );
	// detach the device context before returning.
	(void)dc.Detach();
}


void CSortHeaderCtrl::Serialize( CArchive& ar )
{
	if( ar.IsStoring() )
	{
		const int iItemCount = GetItemCount();
		if( iItemCount != -1 )
		{
			ar << iItemCount;

			HD_ITEM hdItem = { 0 };
			hdItem.mask = HDI_WIDTH;

			for( int i = 0; i < iItemCount; i++ )
			{
				VERIFY( GetItem( i, &hdItem ) );
				ar << hdItem.cxy;
			}
		}
	}
	else
	{
		int iItemCount;
		ar >> iItemCount;
		
		if( GetItemCount() != iItemCount )
			TRACE0( _T("Different number of columns in registry.") );
		else
		{
			HD_ITEM hdItem = { 0 };
			hdItem.mask = HDI_WIDTH;

			for( int i = 0; i < iItemCount; i++ )
			{
				ar >> hdItem.cxy;
				VERIFY( SetItem( i, &hdItem ) );
			}
		}
	}
}

void CSortHeaderCtrl::OnPaint()    
{
	if(!m_bCustomDrawTitle)
		CHeaderCtrl::OnPaint();
	else
	{
		CPaintDC dc(this); // device context for painting    
		// TODO: 在此处添加消息处理程序代码    
		// 不为绘图消息调用 CHeaderCtrl::OnPaint()    
		int nItem;     
		nItem = GetItemCount();//得到有几个单元     
		for(int i = 0; i<nItem;i ++)     
		{     
			CRect tRect;    
			GetItemRect(i,&tRect);//得到Item的尺寸    
			int R = m_R,G = m_G,B = m_B;    
			CRect nRect(tRect);//拷贝尺寸到新的容器中     
			nRect.left++;//留出分割线的地方     
			//绘制立体背景     
			for(int j = tRect.top;j<=tRect.bottom;j++)     
			{     
				nRect.bottom = nRect.top+1;     
				CBrush _brush;     
				_brush.CreateSolidBrush(RGB(R, G, B));//创建画刷     
				dc.FillRect(&nRect,&_brush); //填充背景     
				_brush.DeleteObject(); //释放画刷     
				R-=m_Gradient;G-=m_Gradient;B-=m_Gradient;    
				if (R<0)R = 0;    
				if (G<0)G = 0;    
				if (B<0)B= 0;    
				nRect.top = nRect.bottom;     
			}     
			dc.SetBkMode(TRANSPARENT);    
			CFont nFont ,* nOldFont;     
			//dc.SetTextColor(RGB(250,50,50));     
			dc.SetTextColor(m_colorText);    
			nFont.CreateFont(m_fontHeight,m_fontWidth,0,0,700,FALSE,FALSE,0,0,0,0,0,0,_TEXT("宋体"));//创建字体     
			
			nOldFont = dc.SelectObject(&nFont);    
			UINT nFormat = 1;    
			if(m_alignmentFormat.GetLength()>i)
			{
				if(m_alignmentFormat[i]=='0')    
				{    
					nFormat = DT_LEFT;    
					tRect.left+=3;    
				}    
				else if(m_alignmentFormat[i]=='1')
				{    
					nFormat = DT_CENTER;    
				}    
				else if(m_alignmentFormat[i]=='2')    
				{    
					nFormat = DT_RIGHT;    
					tRect.right-=3;    
				}
			}
			//将文字显示在一个适合的高度位置    
			TEXTMETRIC metric;    
			dc.GetTextMetrics(&metric);    
			int ofst = 0;    
			ofst = tRect.Height() - metric.tmHeight;    
			tRect.OffsetRect(0,ofst/2);    
			dc.DrawText(m_titleArr[i],&tRect,nFormat);    
			dc.SelectObject(nOldFont);     
			nFont.DeleteObject(); //释放字体     
		}     
		//画头部剩余部分(没有标题的那部分)    
		CRect rtRect;    
		CRect clientRect;    
		GetItemRect(nItem - 1,rtRect);    
		GetClientRect(clientRect); //当前(CHeaderCtrl)控件的大小    
		rtRect.left = rtRect.right+1;    
		rtRect.right = clientRect.right;    
		int R = m_R,G = m_G,B = m_B;    
		CRect nRect(rtRect);    
		//绘制立体背景     
		for(int j = rtRect.top;j<=rtRect.bottom;j++)     
		{     
			nRect.bottom = nRect.top+1;     
			CBrush _brush;     
			_brush.CreateSolidBrush(RGB(R,G,B));//创建画刷     
			dc.FillRect(&nRect,&_brush); //填充背景     
			_brush.DeleteObject(); //释放画刷     
			R-=m_Gradient;G-=m_Gradient;B-=m_Gradient;    
			if (R<0)R = 0;    
			if (G<0)G = 0;    
			if (B<0)B= 0;    
			nRect.top = nRect.bottom;     
		}
	}
}

LRESULT CSortHeaderCtrl::OnLayout( WPARAM wParam, LPARAM lParam )    
{    
	LRESULT lResult = CHeaderCtrl::DefWindowProc(HDM_LAYOUT, 0, lParam);     
	HD_LAYOUT &hdl = *( HD_LAYOUT * ) lParam;     
	RECT *prc = hdl.prc;     
	WINDOWPOS *pwpos = hdl.pwpos;     
	//表头高度为原来1.5倍，如果要动态修改表头高度的话，将1.5设成一个全局变量     
	int nHeight = (int)(pwpos->cy * m_heightCoef);
	pwpos->cy = nHeight;     
	prc->top = nHeight;     
	return lResult;     
}  
