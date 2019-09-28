#include "stdafx.h"
#include "XhMenu.h"
//*************************************************************************   
// TCMenu.cpp : implementation file   
//           
#include "XhMenu.h"            
   
#ifdef _DEBUG   
#define new DEBUG_NEW   
#undef THIS_FILE   
static char THIS_FILE[] = __FILE__;   
#endif   
   
CXhMenu::CXhMenu()   
{   
    m_clrText =  GetSysColor (COLOR_MENUTEXT);   
    m_clrBack = GetSysColor (COLOR_MENU);   
    m_brBackground.CreateSolidBrush (m_clrBack);   
    m_penBack.CreatePen (PS_SOLID,0,m_clrBack);   
    m_crText = m_clrText;   
    m_bLBtnDown = FALSE;   
   
    m_iconX = 16;          
    m_iconY = 16;          
   
    m_clrHilight = GetSysColor (COLOR_HIGHLIGHT);      
    m_brSelect.CreateSolidBrush (m_clrHilight);        
    m_clrHilightText = GetSysColor (COLOR_HIGHLIGHTTEXT);   
   
    ZeroMemory ((PVOID) &m_lf,sizeof (LOGFONT));   
    NONCLIENTMETRICS nm;   
    nm.cbSize = sizeof (NONCLIENTMETRICS);   
   
    VERIFY (SystemParametersInfo (SPI_GETNONCLIENTMETRICS,0,&nm,0));    
   
    m_lf =  nm.lfMenuFont;   
    m_iMenuHeight = nm.iMenuHeight;   
    m_fontMenu.CreateFontIndirect (&m_lf);   
}   
   
   
CXhMenu::~CXhMenu()   
{   
    DestroyMenu();   
}   
   
BOOL CXhMenu::DestroyMenu()   
{   
    int m;   
    int numSubMenus = m_SubMenus.GetUpperBound();   
    for(m = numSubMenus; m >= 0; m--)   
        delete(m_SubMenus[m]);   
    //分别删除子菜单   
    m_SubMenus.RemoveAll();   
    //删除全部   
    if ((HBRUSH) m_brBackground != NULL)       
        m_brBackground.DeleteObject ();   
    if ((HFONT)m_fontMenu !=NULL)              
        m_fontMenu.DeleteObject ();   
    if ((HBRUSH)m_brSelect != NULL)   
        m_brSelect.DeleteObject ();   
    //删除其他的GDI对象   
    int numItems = m_MenuList.GetUpperBound();   
    for(m = 0; m <= numItems; m++)   
            delete(m_MenuList[m]);   
    //删除菜单项   
    m_MenuList.RemoveAll();   
    //删除全部的菜单项   
    return(CMenu::DestroyMenu());   
};   

void CXhMenu::DrawItem (LPDRAWITEMSTRUCT lpDIS)   
{   
    ASSERT(lpDIS != NULL);   
   
    CDC* pDC = CDC::FromHandle(lpDIS->hDC);   
    CRect rect;   
    HICON hIcon;   
    COLORREF crText = m_crText;   
    //文本颜色     
       
    rect.CopyRect(&lpDIS->rcItem);   
    //获取菜单项矩形   
       
    UINT action = lpDIS->itemAction;   
    UINT state = lpDIS->itemState;   
    CString strText;   
    LOGFONT lf;   
    lf = m_lf;   
       
    CFont dispFont;   
    CFont *pFont;   
           
    if (lpDIS->itemData != NULL)   
    {   
        UINT nIconNormal = (((CXhMenuData*)(lpDIS->itemData))->menuIconNormal);   
        UINT nIconSelected = (((CXhMenuData*)(lpDIS->itemData))->menuIconSelected);   
        UINT nIconDisabled = (((CXhMenuData*)(lpDIS->itemData))->menuIconDisabled);   
        strText = (((CXhMenuData*) (lpDIS->itemData))->menuText);   
   
        if(nIconNormal == -1)   
            hIcon = NULL;   
        else    
        {   
            hIcon = NULL;   
            //以下是根据不同的菜单状态，载入不同的图片资源   
            if (state & ODS_SELECTED && !(state & ODS_GRAYED))         
            {   
                if(nIconSelected != -1)   
                    hIcon = AfxGetApp ()->LoadIcon(nIconSelected);   
            }   
            else   
            {   
                if(state & ODS_GRAYED)                                 
                    if(nIconDisabled != -1)   
                        hIcon = AfxGetApp()->LoadIcon(nIconDisabled);   
            };   
   
   
            if(hIcon == NULL)   
                hIcon = AfxGetApp()->LoadIcon(nIconNormal);   
        }   
    }   
    else   
    {   
        strText.Empty();   
        hIcon = NULL;   
    }   
   
    if ( (state & ODS_SELECTED) )   
    {   
   
        CPen *pOldPen = pDC->SelectObject (&m_penBack);   
        //根据不同菜单状态，填充不同的色彩   
        if (m_hilightStyle != Normal)   
            pDC->FillRect (rect,&m_brBackground);   
        else   
            pDC->FillRect (rect,&m_brSelect);   
   
        pDC->SelectObject (pOldPen);   
   
        pDC->Draw3dRect (rect,GetSysColor (COLOR_3DHILIGHT),RGB(0,0,0));           
   
        if ((HFONT)dispFont != NULL)   
                dispFont.DeleteObject ();   
        dispFont.CreateFontIndirect (&lf);   
        crText = m_clrHilightText;   
    }   
    else   
    {   
        CPen *pOldPen = pDC->SelectObject (&m_penBack);   
        pDC->FillRect (rect,&m_brBackground);   
        pDC->SelectObject (pOldPen);   
   
        pDC->Draw3dRect (rect,m_clrBack,m_clrBack);   
        if ((HFONT)dispFont != NULL)   
                dispFont.DeleteObject ();   
        dispFont.CreateFontIndirect (&lf); //Normal   
   
    }   
   
   if (hIcon != NULL)   
    {   
       //绘制图片   
        DrawIconEx (pDC->GetSafeHdc(),rect.left,rect.top,hIcon,   
                    m_iconX,m_iconY,0,NULL,DI_NORMAL);   
    };   
   
    rect.left = rect.left +((m_iconX)?m_iconX:32) + 2;    
   
    if ( !strText.IsEmpty())   
    {   
        pFont->GetLogFont (&lf);   
   
        int iOldMode = pDC->GetBkMode();   
   
        pDC->SetBkMode( TRANSPARENT);   
        UINT nFormat = DT_LEFT|DT_SINGLELINE|DT_EXPANDTABS|DT_VCENTER;   
        if(!(lpDIS->itemState & ODS_GRAYED))   
        {   
            pDC->SetTextColor(crText);   
            pDC->DrawText (strText,rect,nFormat);   
        }   
        else   
        {   
   
            RECT offset = *rect;   
            offset.left+=1;   
            offset.right+=1;   
            offset.top+=1;   
            offset.bottom+=1;   
            pDC->SetTextColor(GetSysColor(COLOR_BTNHILIGHT));   
            pDC->DrawText(strText,&offset, nFormat);   
   
   
            pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));   
            pDC->DrawText(strText,rect, nFormat);   
        };   
        pFont = pDC->SelectObject (&dispFont);   
        pDC->SetBkMode( iOldMode );   
        pDC->SelectObject (pFont); //set it to the old font   
    }   
    dispFont.DeleteObject ();   
}   
   
   
void CXhMenu::MeasureItem( LPMEASUREITEMSTRUCT lpMIS )   
{   
    CWnd *pWnd = AfxGetMainWnd();                      
    CDC *pDC = pWnd->GetDC();                           
    CFont* pFont = pDC->SelectObject (&m_fontMenu);     
    LPCTSTR lpstrText = (((CXhMenuData*)(lpMIS->itemData))->menuText);   
    SIZE t;   
    GetTextExtentPoint32(pDC->GetSafeHdc(), lpstrText, strlen(lpstrText), &t);   
    pDC->SelectObject (pFont);                          
    AfxGetApp()->m_pMainWnd->ReleaseDC (pDC);        
   
    lpMIS->itemWidth = m_iconX + t.cx + 10;   
    lpMIS->itemHeight = m_iconY;   
}   
   
void CXhMenu::SetIconSize (int width, int height)   
{   
    m_iconX = width;   
    m_iconY = height;   
}   
   
void CXhMenu::SetTextColor (COLORREF clrText)   
{   
    m_crText = clrText;   
}   
   
void CXhMenu::SetBackColor (COLORREF clrBack)   
{   
    m_clrBack = clrBack;   
    if ((HBRUSH)m_brBackground != NULL)   
        m_brBackground.DeleteObject ();   
   
    m_brBackground.CreateSolidBrush (clrBack);   
}   
   
void CXhMenu::SetHighlightColor (COLORREF clrHilight)   
{   
    m_clrHilight = clrHilight;   
    if ((HBRUSH)m_brSelect != NULL)   
                    m_brSelect.DeleteObject ();   
   
    m_brSelect.CreateSolidBrush (clrHilight);   
}   
   
void CXhMenu::SetHighlightTextColor (COLORREF clrHilightText)   
{   
    m_clrHilightText = clrHilightText;   
}   
   
   
void CXhMenu::SetHighlightStyle (HIGHLIGHTSTYLE hilightStyle)   
{   
    m_hilightStyle = hilightStyle;   
}   
   
   
BOOL CXhMenu::AppendODMenu(LPCTSTR lpstrText,   
                          UINT  nFlags,   
                          UINT  nID,   
                          UINT  nIconNormal,   
                          UINT  nIconSelected,   
                          UINT  nIconDisabled)   
{   
    if(!(nFlags & MF_OWNERDRAW))   
        nFlags |= MF_OWNERDRAW;   
   
    CXhMenuData *mdata = new CXhMenuData;   
    m_MenuList.Add(mdata);   
    //添加数据项   
    lstrcpy(mdata->menuText, lpstrText);   
    //复制菜单字符串   
    //以下是设置菜单的图片类型   
    mdata->menuIconNormal = nIconNormal;   
    mdata->menuIconSelected = nIconSelected;   
    mdata->menuIconDisabled = nIconDisabled;   
    return(CMenu::AppendMenu(nFlags, nID, (LPCTSTR)mdata));   
};   
   
   
BOOL CXhMenu::ModifyODMenu(LPCTSTR lpstrText,   
                          UINT    nID,   
                          UINT    nIconNormal,   
                          UINT    nIconSelected,   
                          UINT    nIconDisabled)   
{   
    int numMenuItems = m_MenuList.GetUpperBound();   
    CXhMenuData *mdata;   
    for(int m = 0; m <= numMenuItems; m++)   
    {   
        if((mdata = m_MenuList[m]) != NULL)   
        {   
            if(mdata->nID == nID)   
            {   
                delete(mdata);   
                m_MenuList.RemoveAt(m);   
                break;   
            };   
        };   
    };   
    //根据菜单项ID查找该数据项，然后进行删除   
    mdata = new CXhMenuData;   
    m_MenuList.Add(mdata);   
    lstrcpy(mdata->menuText, lpstrText);   
    mdata->menuIconNormal = nIconNormal;   
    mdata->menuIconSelected = nIconSelected;   
    mdata->menuIconDisabled = nIconDisabled;   
    //添加一个新的菜单项   
    return(CMenu::ModifyMenu(nID, MF_BYCOMMAND | MF_OWNERDRAW, nID, (LPCTSTR)mdata));   
};   
   
   
BOOL CXhMenu::LoadMenu(LPCTSTR lpszResourceName)   
{   
    return(CXhMenu::LoadMenu(MAKEINTRESOURCE(lpszResourceName)));   
};   
   
BOOL CXhMenu::LoadMenu(int nResource)   
{   
   
    DWORD dwSize;   
    WORD wrd = MAKEWORD(nResource, 0);   
    HRSRC hRsrc = FindResource(NULL, (LPCTSTR)wrd, RT_MENU);   
    if(hRsrc == NULL)   
    {   
        TRACE0("TCMenu::LoadMenu() - Failed to find Menu Resource\n");   
        ASSERT(FALSE);   
    };   
   
    dwSize = SizeofResource(NULL, hRsrc);   
   
   
    HGLOBAL hGlobal = LoadResource(NULL, hRsrc);   
    if(hGlobal == NULL)   
    {   
        TRACE0("TCMenu::LoadMenu() - Failed to load Menu Resource\n");   
        ASSERT(FALSE);   
    };   
   
    if(!CMenu::CreateMenu())   
    {   
        TRACE0("TCMenu::LoadMenu() - Failed to create Menu\n");   
        ASSERT(FALSE);   
    };   
   
    MENUITEMTEMPLATEHEADER* pTpHdr = (MENUITEMTEMPLATEHEADER*)LockResource(hGlobal);   
    BYTE*                   pTp   = (BYTE*)pTpHdr +    
                                    (sizeof(MENUITEMTEMPLATEHEADER) + pTpHdr->offset);   
   
   
    CXhMenuData* pData = NULL;              
    WORD        dwFlags = 0;               
    WORD        dwID    = 0;               
    BOOL        bLastPopup = 0;            
    UINT        uFlags;                    
    char        caption[80];               
    int         nLen   = 0;                
    CTypedPtrArray<CPtrArray, CXhMenu*>    m_Stack;       
    m_Stack.Add(this);                                 
   
    do   
    {   
   
        memcpy(&dwFlags, pTp, sizeof(WORD));pTp+=sizeof(WORD);   
        if(!(dwFlags & MF_POPUP))   
        {   
            memcpy(&dwID, pTp, sizeof(WORD));                  
            pTp+=sizeof(WORD);   
        }   
        else   
            dwID = 0;   
   
        uFlags = (UINT)dwFlags;        
        if(uFlags & MF_END)            
            uFlags -= MF_END;   
   
        nLen = 0;   
        char *ch = (char*)pTp;   
        while(*ch != 0)   
        {   
            caption[nLen] = ch[0];   
            nLen++;            
            ch+=2;             
        };   
        caption[nLen] = 0;     
        pTp+=((nLen+1)*2);     
   
   
        if(dwFlags & MF_POPUP)   
        {   
            if(dwFlags & MF_END)   
                bLastPopup = TRUE;   
   
            CXhMenu* pSubMenu = new CXhMenu;   
            pSubMenu->CreatePopupMenu();   
   
            m_Stack[m_Stack.GetUpperBound()]->AppendMenu(uFlags, (UINT)pSubMenu->m_hMenu, caption);   
            m_Stack.Add(pSubMenu);                 
            m_SubMenus.Add(pSubMenu);              
        }   
        else   
        {   
            m_Stack[m_Stack.GetUpperBound()]->AppendODMenu(caption, uFlags, dwID, -1, -1, -1);   
            if(dwFlags & MF_END)   
                m_Stack.RemoveAt(m_Stack.GetUpperBound());   
   
            if(bLastPopup)   
            {   
                bLastPopup = FALSE;   
                m_Stack.RemoveAt(m_Stack.GetUpperBound());   
            };   
        };   
   
   
    }while(m_Stack.GetUpperBound() != -1);   
    return(TRUE);   
};   
