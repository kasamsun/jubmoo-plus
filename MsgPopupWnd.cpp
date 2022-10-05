#include "MsgPopupWnd.h"
#include "kswafx/DCEx.h"
#include "resource.h"
#include <stdio.h>

BEGIN_HANDLE_MSG(C_MsgPopupWnd,C_GenericWnd)
	HANDLE_MSG(WM_PAINT,OnPaint)
	HANDLE_MSG(WM_TIMER,OnTimer)
	HANDLE_MSG(WM_LBUTTONDOWN,OnLButtonDown)
	HANDLE_MSG(WM_DESTROY,OnDestroy)
END_HANDLE_MSG()

C_MsgPopupWnd::C_MsgPopupWnd( C_Wnd* pParentWnd,
							int nMaxLine,
                            int nMsgTimeout,
                            int nIdleTimeout )
							: C_GenericWnd(pParentWnd)
{
	m_hWnd = CreateWindow(m_szClassName,
                        "",
                        WS_POPUP|WS_CLIPSIBLINGS,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        pParentWnd->GetHandle(),
                        NULL,
                        C_App::m_hInstance,
                        (LPSTR) this );
    if ( ! m_hWnd )
        exit( FALSE );

    // Default parameter
	m_nLeftMargin = 3;
	m_nTopMargin = 2;
    m_nFloatHeight = 2;//2
    m_nDirHeight = 8;//8
    m_nDirWidth = 10;//10
    m_nDirMargin = 10;

    m_nMsgTimeout = nMsgTimeout;	// showing msg time out, def=8s
    m_nIdleTimeout = nIdleTimeout;	// delete old message timeout, def=5s
    m_nMaxLine = nMaxLine;

	CreateMsgPopupWndFont();
}

C_MsgPopupWnd::~C_MsgPopupWnd()
{
	if ( m_pszMsgList.Count() > 0 )
		m_pszMsgList.DeleteObjects();
}

void C_MsgPopupWnd::CreateMsgPopupWndFont()
{
	LOGFONT lf;
	m_hDC = GetDC(m_hWnd);

	// set font size 8 -->
	lf.lfHeight = -MulDiv(8,GetDeviceCaps(m_hDC,LOGPIXELSY),72);
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = 0;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = 222;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = 0;
	lf.lfPitchAndFamily = 0;
	strcpy(lf.lfFaceName,"MS Sans Serif");
	m_hFont = CreateFontIndirect(&lf);

	HFONT tmpFont = (HFONT)SelectObject(m_hDC,m_hFont);
	TEXTMETRIC tm;
	GetTextMetrics(m_hDC,&tm);

	m_nFontHeight = tm.tmHeight+tm.tmExternalLeading;
	SelectObject(m_hDC,tmpFont);
	ReleaseDC(m_hWnd,m_hDC);

}

void C_MsgPopupWnd::PopupMessage(LPCTSTR pszMsg)
{
	if ( pszMsg != NULL )
    {
        if ( m_pszMsgList.Count() >= m_nMaxLine )
        {
            delete []m_pszMsgList.Items( 0 );
            m_pszMsgList.Remove( 0 );
        }

        char * pszNewMsg = new char[strlen(pszMsg)+1];
        strcpy(pszNewMsg,pszMsg);
        m_pszMsgList.Add(pszNewMsg);
	}
    else
    {
    	if ( m_pszMsgList.Count() <= 0 )
        	return ;
    }

	// calculate new window size
    m_hDC = GetDC(m_hWnd);
	HFONT hTmpFont = (HFONT)SelectObject(m_hDC,m_hFont);
	SIZE size;
	int nMaxLen = 0;
	for (int i=0; i < m_pszMsgList.Count() ; i++)
	{
		GetTextExtentPoint32(m_hDC,m_pszMsgList.Items(i),strlen(m_pszMsgList.Items(i)),&size);
		if ( size.cx > nMaxLen )
			nMaxLen = size.cx;
	}
    SelectObject(m_hDC,hTmpFont);
    ReleaseDC(m_hWnd,m_hDC);

	// set new window size, position ( relative to parent )
	RECT rc;
	GetClientRect(m_hWnd,&rc);
	ClientToScreen(GetParentWnd()->GetHandle(),(LPPOINT)&rc.left);
	ClientToScreen(GetParentWnd()->GetHandle(),(LPPOINT)&rc.right);
    int nWidth = nMaxLen+m_nLeftMargin*2+m_nFloatHeight;
    int nHeight = m_nFontHeight*m_pszMsgList.Count()+m_nTopMargin*2+m_nFloatHeight
    					+ m_nDirHeight;
	// if top-left is less than 0, set to 0.                        
	SetWindowPos(m_hWnd,HWND_TOP,
    				rc.left,(rc.top-nHeight < 0)?0:rc.top-nHeight,
    				nWidth,nHeight,
                    SWP_NOACTIVATE|SWP_SHOWWINDOW);
	SetWndRegion();

	///////////////////////////////////////
	if ( !IsWindowVisible(m_hWnd) )
	{
    	ShowWindow(m_hWnd,SW_SHOWNA);
    }
	else
    {
		KillTimer(m_hWnd,(UINT)m_hWnd);
    }
	InvalidateRect(m_hWnd,NULL,FALSE);
	SetTimer(m_hWnd,(UINT)m_hWnd,m_nMsgTimeout,NULL);
}

void C_MsgPopupWnd::SetWndRegion()
{
	RECT rc;
	HRGN hrgn1,hrgn2,hrgn3,hrgn4;

	GetClientRect(m_hWnd,&rc);
	// create content region
	hrgn1 = CreateRectRgn(rc.left,
    					rc.top,
    					rc.right-m_nFloatHeight,
                        rc.bottom-m_nFloatHeight-m_nDirHeight);
	hrgn2 = CreateRectRgn(rc.left+m_nFloatHeight,
    					rc.top+m_nFloatHeight,
    					rc.right,
                        rc.bottom-m_nDirHeight);
	CombineRgn(hrgn1,hrgn1,hrgn2,RGN_OR);

    // create parent pointer region
    POINT *pPoint = &m_pDirPoint[0];

    if ( rc.right - rc.left < m_nDirMargin + m_nDirWidth + m_nFloatHeight )
    {
    	// show shrink
        if ( rc.right - rc.left < m_nDirWidth + m_nFloatHeight )
        {
        	pPoint->x = rc.left + m_nFloatHeight;
        	(pPoint+2)->x = rc.right-1;
        	(pPoint+1)->x = (rc.right - rc.left)/2  + m_nFloatHeight;
        }
        else
        {
        	pPoint->x = rc.right - m_nDirWidth;
        	(pPoint+2)->x = rc.right-1;
        	(pPoint+1)->x = ( (pPoint+2)->x - pPoint->x )/2  + m_nFloatHeight;
        }
        pPoint->y = rc.bottom - m_nDirHeight -1;
        (pPoint+1)->y = rc.bottom;
        (pPoint+2)->y = pPoint->y;
    }
    else
    {
    	// show normal
    	pPoint->x = rc.left + m_nDirMargin + m_nFloatHeight;
        pPoint->y = rc.bottom - m_nDirHeight -1;
    	(pPoint+1)->x = rc.left + (int)m_nDirMargin/2 + m_nFloatHeight;
        (pPoint+1)->y = rc.bottom;
    	(pPoint+2)->x = rc.left + m_nDirMargin + m_nDirWidth + m_nFloatHeight-1;
        (pPoint+2)->y = pPoint->y;
    }
	hrgn3 = CreatePolygonRgn(pPoint,3,WINDING);
// WINDING / ALTERNATE
	CombineRgn(hrgn1,hrgn1,hrgn3,RGN_OR);

    for ( int i = 0 ; i < 3 ; i ++ )
    {
	    (pPoint+i)->x -= m_nFloatHeight;
    	(pPoint+i)->y -= m_nFloatHeight;
    }
	hrgn4 = CreatePolygonRgn(pPoint,3,WINDING);
	CombineRgn(hrgn1,hrgn1,hrgn4,RGN_OR);

	DeleteObject(hrgn2);
    DeleteObject(hrgn3);
    DeleteObject(hrgn4);
	SetWindowRgn(m_hWnd,hrgn1,TRUE);
}

void C_MsgPopupWnd::PutTextMessage(HDC hDC,int xPos,int yPos,
								LPCSTR pszMsg,int nMsgLen)
{
	if ( pszMsg[0] == '/' )
	{
		COLORREF crTmp;
		COLORREF crText = RGB(0,0,0);
		COLORREF crBkTmp;
		COLORREF crBk;
		// ===========================================
		// fix limit to 64 character.
		// ===========================================
		char szCmd[64];
		char szMsg[64];
		char cCmd;
		int nColor;
		memset(szMsg,0x00,64);
		memset(szCmd,0x00,64);
		if ( sscanf(pszMsg+1,"%c%d%64c",&cCmd,&nColor,szMsg) >= 2 ) // == 3 )
		{
			if ( cCmd == 'c' && nColor < 256 )
			{
				int nBkColor = nColor/16;
				int nFgColor = nColor%16;

				if ( nBkColor == 0 )
					SetBkMode(hDC,TRANSPARENT);
				else
				{
					SetBkMode(hDC,OPAQUE);
					switch ( nBkColor )
					{
					case 1:crBk = RGB(0,0,128);break;
					case 2:crBk = RGB(0,128,0);break;
					case 3:crBk = RGB(0,128,128);break;
					case 4:crBk = RGB(128,0,0);break;
					case 5:crBk = RGB(128,0,128);break;
					case 6:crBk = RGB(128,128,0);break;
					case 7:crBk = RGB(128,128,128);break;
					case 8:crBk = RGB(192,192,192);break;
					case 9:crBk = RGB(0,0,255);break;
					case 10:crBk = RGB(0,255,0);break;
					case 11:crBk = RGB(0,255,255);break;
					case 12:crBk = RGB(255,0,0);break;
					case 13:crBk = RGB(255,0,255);break;
					case 14:crBk = RGB(255,255,0);break;
					case 15:crBk = RGB(255,255,255);break;
					}
					crBkTmp = SetBkColor(hDC,crBk);
				}
				switch ( nFgColor )
				{
				case 0:crText = RGB(0,0,0);break;
				case 1:crText = RGB(0,0,128);break;
				case 2:crText = RGB(0,128,0);break;
				case 3:crText = RGB(0,128,128);break;
				case 4:crText = RGB(128,0,0);break;
				case 5:crText = RGB(128,0,128);break;
				case 6:crText = RGB(128,128,0);break;
				case 7:crText = RGB(128,128,128);break;
				case 8:crText = RGB(192,192,192);break;
				case 9:crText = RGB(0,0,255);break;
				case 10:crText = RGB(0,255,0);break;
				case 11:crText = RGB(0,255,255);break;
				case 12:crText = RGB(255,0,0);break;
				case 13:crText = RGB(255,0,255);break;
				case 14:crText = RGB(255,255,0);break;
				case 15:crText = RGB(255,255,255);break;
				default:crText = RGB(0,0,0);break;
				}
				crTmp = SetTextColor(hDC,crText);
				TextOut(hDC,xPos,yPos,szMsg,strlen(szMsg));
				SetTextColor(hDC,crTmp);
				if ( nBkColor > 0 )
				{
					SetBkMode(hDC,TRANSPARENT);
					SetBkColor(hDC,crBkTmp);
				}
                return ;
			}
		}
		else if ( sscanf(pszMsg+1,"%s %64c",szCmd,szMsg) > 0 )//== 2 )
		{
			if ( strcmp(szCmd,"me") == 0 )
			{
				DrawIconEx(hDC,xPos,yPos,LoadIcon(C_App::m_hInstance,
								MAKEINTRESOURCE(IDI_MSGACTION)),16,16,
								0,NULL,DI_NORMAL);
				crText = RGB(0,128,128);
			}
			else if ( strcmp(szCmd,"sys") == 0 )
			{
				DrawIconEx(hDC,xPos,yPos,LoadIcon(C_App::m_hInstance,
								MAKEINTRESOURCE(IDI_MSGSYSTEM)),16,16,
								0,NULL,DI_NORMAL);
				crText = RGB(128,0,0);
			}
            else
            {
            	TextOut(hDC,xPos,yPos,pszMsg,nMsgLen);
				return;
            }
			crTmp = SetTextColor(hDC,crText);
			TextOut(hDC,xPos+16,yPos,szMsg,strlen(szMsg));
			SetTextColor(hDC,crTmp);
            return;
		}
	}

	TextOut(hDC,xPos,yPos,pszMsg,nMsgLen);
}


// ===================== event handler ====================================
LRESULT C_MsgPopupWnd::OnPaint(WPARAM,LPARAM)
{
    PAINTSTRUCT ps;
    m_hDC = BeginPaint( m_hWnd, &ps );

	RECT rc;
	GetClientRect(m_hWnd,&rc);

	HDC hdcMem;
	HBITMAP hbmpMem,hbmpTmp;

	hdcMem = CreateCompatibleDC(m_hDC);
	hbmpMem = CreateCompatibleBitmap(m_hDC,rc.right-rc.left,rc.bottom-rc.top);
	hbmpTmp = (HBITMAP)SelectObject(hdcMem,hbmpMem);

    // fill background shadow
	FillRect(hdcMem,&rc,(HBRUSH)GetStockObject(BLACK_BRUSH));

	HRGN hrgnDir;
	hrgnDir = CreatePolygonRgn(&m_pDirPoint[0],3,WINDING);
	HBRUSH hb;
	hb = CreateSolidBrush(GetSysColor(COLOR_3DHILIGHT));
	FillRgn(hdcMem,hrgnDir,hb);
    MoveToEx(hdcMem,m_pDirPoint[0].x,m_pDirPoint[0].y,NULL);
    LineTo(hdcMem,m_pDirPoint[1].x,m_pDirPoint[1].y-1);
    LineTo(hdcMem,m_pDirPoint[2].x,m_pDirPoint[2].y);
    DeleteObject(hb);
    DeleteObject(hrgnDir);

    // fill text background
	RECT rcg;
	SetRect(&rcg,1,1,
    			rc.right-m_nFloatHeight-1,
                rc.bottom-m_nFloatHeight-1-m_nDirHeight);

    C_DCEx * pDCEx = new C_DCEx(hdcMem);
    pDCEx->FillGradientRect(&rcg,RGB(255,255,255),
                                GetSysColor(COLOR_3DHILIGHT));
    delete pDCEx;
	// set font
	m_hOldFont = (HFONT)SelectObject(hdcMem,m_hFont);
	SetBkMode(hdcMem,TRANSPARENT);
	TEXTMETRIC tm;
	GetTextMetrics(hdcMem,&tm);
	int nPos = 0;
	for ( int i=0; i < m_pszMsgList.Count() ; i++ )
	{
		PutTextMessage(hdcMem,m_nLeftMargin,nPos+m_nTopMargin,
        		m_pszMsgList.Items(i),
                strlen(m_pszMsgList.Items(i)) );
		nPos += tm.tmHeight+tm.tmExternalLeading;
	}
	SelectObject(hdcMem,m_hOldFont);

	BitBlt(m_hDC,0,0,rc.right-rc.left,rc.bottom-rc.top,hdcMem,0,0,SRCCOPY);

	DeleteObject(SelectObject(hdcMem,hbmpTmp));
	DeleteDC(hdcMem);

	EndPaint( m_hWnd, &ps );
	return 0;
}

LRESULT C_MsgPopupWnd::OnTimer(WPARAM,LPARAM)
{
	if ( IsWindowVisible(m_hWnd) )
	{
		KillTimer(m_hWnd,(UINT)m_hWnd);
		ShowWindow(m_hWnd,SW_HIDE);
        SetTimer(m_hWnd,(UINT)m_hWnd,m_nIdleTimeout,NULL);
    }
    else
    {
    	// window is hide
    	if ( m_pszMsgList.Count() > 0 )
        {
    		delete []m_pszMsgList.Items( 0 );
			m_pszMsgList.Remove( 0 );
        }
        else
        	KillTimer(m_hWnd,(UINT)m_hWnd);
    }
	return 0;
}

LRESULT C_MsgPopupWnd::OnDestroy(WPARAM,LPARAM)
{
	KillTimer(m_hWnd,(UINT)m_hWnd);
	DeleteObject(m_hFont);
	return 0;
}

LRESULT C_MsgPopupWnd::OnLButtonDown(WPARAM,LPARAM)
{
	KillTimer(m_hWnd,(UINT)m_hWnd);
	ShowWindow(m_hWnd,SW_HIDE);
    SetTimer(m_hWnd,(UINT)m_hWnd,m_nIdleTimeout,NULL);
	return 0;
}
