#include "PlayerDetailWnd.h"

#include <stdio.h>
#include "resource.h"
#include "JubmooWnd.h"
#include "PlayerWnd.h"
#include "Player.h"
#include "kswafx/DCEx.h"
#include "BmpDigit.h"
#include "Jubmoo.h"
#include "kswafx/Tooltip.h"

#define PD_MOVE_TIMERID 6101
#define PD_MOVE_TIMER 50

#define PD_ACTIVE_PLAYER_TIMERID	6102
#define PD_ACTIVE_PLAYER_TIMER		250

#define SM_ICON_POSX	24
#define SM_ICON_POSY	12
#define LG_ICON_POSX	10
#define LG_ICON_POSY	12

char C_PlayerDetailWnd::szDisplayModeText[3][24] = {"Current score",
													"Player Statistic",
													"Inhand/Expectant score"};

BEGIN_HANDLE_MSG(C_PlayerDetailWnd,C_GenericClickWnd)
	HANDLE_MSG(WM_PAINT,OnPaint)
	HANDLE_MSG(WM_LBUTTONDOWN,OnLButtonDown)
	HANDLE_MSG(WM_LBUTTONUP,OnLButtonUp)
	HANDLE_MSG(WM_RBUTTONDOWN,OnRButtonDown)
	HANDLE_MSG(WM_RBUTTONUP,OnRButtonUp)
	HANDLE_MSG(WM_ERASEBKGND,OnEraseBkgnd)
	HANDLE_MSG(WM_TIMER,OnTimer)

END_HANDLE_MSG()

C_PlayerDetailWnd::C_PlayerDetailWnd(C_Wnd *pParentWnd,int nDisplayMode,int PosX,int PosY)
		: C_GenericClickWnd(pParentWnd)
{
	m_hWnd = CreateWindow(
                            m_szClassName,
                            "",
                            WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS,
                            PosX,PosY,
                            PLAYERDETAIL_WIDTH,PLAYERDETAIL_HEIGHT,
                            pParentWnd->GetHandle(),
                            NULL,
                            C_App::m_hInstance,
                            (LPSTR) this );

    if ( ! m_hWnd )
        exit( FALSE );

    //ShowWindow(m_hWnd,SW_SHOWNA);
	//UpdateWindow(m_hWnd);

    m_nDisplayMode = nDisplayMode;
	m_nPosX = PosX;
	m_nPosY = PosY;
    m_pPlayerWnd = NULL;
	m_pPlayer = NULL;
	m_hdcBkMem = NULL;
	m_pBmpDigit = new C_BmpDigit;
    m_pBmpDigit->SetDigitImage(C_App::m_hInstance,MAKEINTRESOURCE(IDB_NUMBER),USE_DELIMETER);
	m_nMoveTick = 0;
	m_bActivate = FALSE;

    // set default background
	SetImage();

	HRGN hrgn;
	hrgn = CreateRoundRectRgn(0,0,PLAYERDETAIL_WIDTH,PLAYERDETAIL_HEIGHT,
								24,24);
	SetWindowRgn(m_hWnd,hrgn,TRUE);

	SetActiveCursor( LoadCursor(C_App::m_hInstance,MAKEINTRESOURCE(IDC_POINT)) );
	SetClickCursor( LoadCursor(C_App::m_hInstance,MAKEINTRESOURCE(IDC_CLICK)) );
//	SetDragCursor( LoadCursor(C_App::m_hInstance,MAKEINTRESOURCE(IDC_GRASP)) );

    EnableKBMsgParentNotify(TRUE);

	m_pTooltip = new C_Tooltip;
	m_pTooltip->Create(this);
	m_pTooltip->AddTool(m_hWnd,szDisplayModeText[m_nDisplayMode]);
}

C_PlayerDetailWnd::~C_PlayerDetailWnd()
{
	if ( m_nMoveTick != 5 && m_nMoveTick != 0 )
		KillTimer(m_hWnd,PD_MOVE_TIMERID);
	ClearImage();
	m_pBmpDigit->ClearDigitImage();
	delete m_pPlayerWnd;
	delete m_pBmpDigit;
	delete m_pTooltip;
}

void C_PlayerDetailWnd::SetDisplayMode(int nMode)
{
	m_nDisplayMode = nMode;
    switch( m_nDisplayMode )
    {
    case SHOW_JMPLAYER_NORMAL:
    	if ( m_pPlayerWnd != NULL )
        {
        	m_pPlayerWnd->ShowSize(DOUBLE_SIZE_PLAYERWND);
        	SetWindowPos(m_pPlayerWnd->GetHandle(),NULL,LG_ICON_POSX,LG_ICON_POSY,0,0,SWP_NOSIZE);
        }
        break;
    case SHOW_JMPLAYER_INFO:
    case SHOW_JMPLAYER_SCOREONHAND:
    	if ( m_pPlayerWnd != NULL )
        {
        	m_pPlayerWnd->ShowSize(SINGLE_SIZE_PLAYERWND);
        	SetWindowPos(m_pPlayerWnd->GetHandle(),NULL,SM_ICON_POSX,SM_ICON_POSY,0,0,SWP_NOSIZE);
        }
        break;
    }
	m_pTooltip->UpdateTipText(m_hWnd,szDisplayModeText[m_nDisplayMode]);
    InvalidateRect(m_hWnd,NULL,FALSE);
}

// ================================================================
// Input image should be 180x100
// ================================================================
void C_PlayerDetailWnd::SetImage(LPCSTR lpszImage)
{
	// ==============Set Background====================
    ClearImage();

	RECT rc;
	GetClientRect(m_hWnd,&rc);
    int nWidth = rc.right-rc.left;
    int nHeight = rc.bottom-rc.top;

	m_hDC = GetDC(m_hWnd);
    m_hdcBkMem = CreateCompatibleDC(m_hDC);
	m_hbmpBkMem = CreateCompatibleBitmap(m_hDC,rc.right-rc.left,rc.bottom-rc.top);
    m_hbmpBkTmp = (HBITMAP)SelectObject(m_hdcBkMem,m_hbmpBkMem);

	if ( lpszImage != NULL )
	{
		HDC hdcCustomBmp;
		HBITMAP hbmpTmpCustomBmp;
		HBITMAP hbmpCustomBmp = (HBITMAP)LoadImage(NULL,lpszImage,
							IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if ( hbmpCustomBmp != NULL )
		{
		    hdcCustomBmp = CreateCompatibleDC(m_hDC);
			hbmpTmpCustomBmp = (HBITMAP)SelectObject(hdcCustomBmp,hbmpCustomBmp);

			BITMAP bm;
			GetObject(hbmpCustomBmp,sizeof(bm),&bm);
			for ( int i = 0 ; i < nWidth ; i+=bm.bmWidth )
				for ( int j = 0 ; j < nHeight ; j+=bm.bmHeight )
					BitBlt(m_hdcBkMem,i,j,bm.bmWidth,bm.bmHeight,
							hdcCustomBmp,0,0,SRCCOPY);

			DeleteObject(SelectObject(hdcCustomBmp,hbmpTmpCustomBmp));
			DeleteDC(hdcCustomBmp);
			ReleaseDC(m_hWnd,m_hDC);
			return;
		}
	}
    // if not enter filename or load file fail , set to default image
    HDC hdcPaint = CreateCompatibleDC(m_hDC);
    HBITMAP hbmpPaint = LoadBitmap(C_App::m_hInstance,MAKEINTRESOURCE(IDB_PLAYERDETAIL));
    HBITMAP hbmpTmp = (HBITMAP)SelectObject(hdcPaint,hbmpPaint);
    BITMAP bm;
    GetObject(hbmpPaint,sizeof(bm),&bm);
    for ( int i = 0 ; i < nWidth ; i+=bm.bmWidth )
        for ( int j = 0 ; j < nHeight ; j+=bm.bmHeight )
            BitBlt(m_hdcBkMem,i,j,bm.bmWidth,bm.bmHeight,
                    hdcPaint,0,0,SRCCOPY);
    DeleteObject(SelectObject(hdcPaint,hbmpTmp));
    DeleteDC(hdcPaint);

	ReleaseDC(m_hWnd,m_hDC);
}

int C_PlayerDetailWnd::ClearImage()
{
	if ( m_hdcBkMem != NULL )
	{
		DeleteObject(SelectObject(m_hdcBkMem,m_hbmpBkTmp));
		DeleteDC(m_hdcBkMem);
		m_hdcBkMem = NULL;
	}
	return TRUE;
}

C_Player * C_PlayerDetailWnd::SetPlayer(C_Player *pPlayer)
{
	// create PlayerWnd here and delete the old one
    if ( m_nDisplayMode == SHOW_JMPLAYER_NORMAL )
    {
	   	m_pPlayerWnd = new C_PlayerWnd(this,DOUBLE_SIZE_PLAYERWND,LG_ICON_POSX,LG_ICON_POSY,pPlayer );
    }
    else
    {
	   	m_pPlayerWnd = new C_PlayerWnd(this,SINGLE_SIZE_PLAYERWND,SM_ICON_POSX,SM_ICON_POSY,pPlayer );
    }
    pPlayer->SetUI(m_pPlayerWnd);
    Refresh();
	m_pPlayer = pPlayer;
//	if ( pPlayer->GetJubmoo()->GetActivePlayerID() != pPlayer->GetPlayerID() )
	// change to always float
		Float();
    return pPlayer;
}

C_Player * C_PlayerDetailWnd::ClearPlayer()
{
	// delete PlayerWnd here and create the new one at JubmooWnd
	m_bActivate = FALSE;
    if ( m_pPlayerWnd != NULL )
    {
        C_Player * pPlayer = m_pPlayerWnd->GetPlayer();
        delete m_pPlayerWnd;
        m_pPlayerWnd = NULL;
		m_pPlayer = NULL;
        Refresh();
		Sink();

		// clear timer
		ShowInactive();
	    return pPlayer;
    }
	Sink();
    return NULL;
}

void C_PlayerDetailWnd::Refresh()
{
    InvalidateRect(m_hWnd,NULL,FALSE);
}

int C_PlayerDetailWnd::PaintNormal(HDC hDC)
{
    if ( m_pPlayerWnd != NULL )
    {
        RECT rc;
        char * lpszName;
        char lpszScore[16];
        HFONT htmpFont;

	    lpszName = m_pPlayerWnd->GetPlayer()->GetName();
		sprintf(lpszScore,"%d",m_pPlayerWnd->GetPlayer()->GetScore());
        htmpFont = (HFONT)SelectObject(hDC,((C_JubmooWnd*)m_pParentWnd)->GetBigJmFont());
        rc.left = 75;	rc.right = 175;
        rc.top = 10;	rc.bottom = 45;
        DrawText(hDC,lpszName,strlen(lpszName),&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        rc.left = 75;	rc.right = 170;
        rc.top = 50;	rc.bottom = 80;
        m_pBmpDigit->PaintNumber(hDC,rc,m_pPlayerWnd->GetPlayer()->GetScore(),0);
        SelectObject(hDC,htmpFont);
    }
    else
	    DrawIconEx(hDC,LG_ICON_POSX,LG_ICON_POSY,
                LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_NOBODY)),
                64,64,0,NULL,DI_NORMAL);
	return 0;
}

int C_PlayerDetailWnd::PaintInfo(HDC hDC)
{
    if ( m_pPlayerWnd != NULL )
    {
        RECT rc;
        char * lpszName;
        char lpszScore[16];
        HFONT htmpFont;

	    lpszName = m_pPlayerWnd->GetPlayer()->GetName();
		sprintf(lpszScore,"%d",m_pPlayerWnd->GetPlayer()->GetScore());
        htmpFont = (HFONT)SelectObject(hDC,((C_JubmooWnd*)m_pParentWnd)->GetJmFont());
        rc.left = 5;	rc.right = 75;
        rc.top = 45;	rc.bottom = 63;
        DrawText(hDC,lpszName,strlen(lpszName),&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOCLIP);
        rc.left = 5;	rc.right = 75;
        rc.top = 60;	rc.bottom = 78;
        DrawText(hDC,lpszScore,strlen(lpszScore),&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        char tmpbuf[8];
        sprintf(tmpbuf,"%d",m_pPlayerWnd->GetPlayer()->GetNoOfBigChuan());
        TextOut(hDC,109,20,tmpbuf,strlen(tmpbuf));
        sprintf(tmpbuf,"%d",m_pPlayerWnd->GetPlayer()->GetNoOfChuan());
        TextOut(hDC,158,20,tmpbuf,strlen(tmpbuf));
        sprintf(tmpbuf,"%d",m_pPlayerWnd->GetPlayer()->GetNoOfWinner());
        TextOut(hDC,109,55,tmpbuf,strlen(tmpbuf));
        sprintf(tmpbuf,"%d",m_pPlayerWnd->GetPlayer()->GetNoOfPig());
        TextOut(hDC,158,55,tmpbuf,strlen(tmpbuf));

		MoveToEx(hDC,108,68,NULL);
		LineTo(hDC,121,68);
		MoveToEx(hDC,157,68,NULL);
		LineTo(hDC,170,68);
        sprintf(tmpbuf,"%d",m_pPlayerWnd->GetPlayer()->GetNoOfGamePlay());
        TextOut(hDC,109,68,tmpbuf,strlen(tmpbuf));
        sprintf(tmpbuf,"%d",m_pPlayerWnd->GetPlayer()->GetNoOfGamePlay());
        TextOut(hDC,158,68,tmpbuf,strlen(tmpbuf));
        SelectObject(m_hDC,htmpFont);
    }
    else
        DrawIcon(hDC,SM_ICON_POSX,SM_ICON_POSY,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_NOBODY)));
	DrawIcon(hDC,75,10,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_NOOFBIGCHUAN)));
    DrawIcon(hDC,124,10,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_NOOFCHUAN)));
    DrawIcon(hDC,75,50,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_NOOFWINNER)));
    DrawIcon(hDC,124,50,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_NOOFPIG)));
	return 0;
}

int C_PlayerDetailWnd::PaintScoreOnHand(HDC hDC)
{
    if ( m_pPlayerWnd != NULL )
    {
        RECT rc;
        char * lpszName;
        char lpszScore[16];
        HFONT htmpFont;

	    lpszName = m_pPlayerWnd->GetPlayer()->GetName();
		sprintf(lpszScore,"%d",m_pPlayerWnd->GetPlayer()->GetScore());
        htmpFont = (HFONT)SelectObject(hDC,((C_JubmooWnd*)m_pParentWnd)->GetJmFont());
        rc.left = 5;	rc.right = 75;
        rc.top = 45;	rc.bottom = 63;
        DrawText(hDC,lpszName,strlen(lpszName),&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOCLIP);
        rc.left = 5;	rc.right = 75;
        rc.top = 60;	rc.bottom = 78;
        DrawText(hDC,lpszScore,strlen(lpszScore),&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

        // calculate current score on hand
       	C_Jubmoo * pJm = ((C_JubmooWnd *)m_pParentWnd)->GetJubmoo();
		int nScore = pJm->CalculateScore(m_pPlayerWnd->GetPlayer()->GetTakenCardList());

        rc.left = 75;	rc.right = 170;
        rc.top = 10;	rc.bottom = 50;
        m_pBmpDigit->PaintNumber(hDC,rc,nScore,0);
        // show expected score
        rc.left = 75;	rc.right = 170;
        rc.top = 50;	rc.bottom = 90;
        m_pBmpDigit->PaintNumber(hDC,rc,m_pPlayerWnd->GetPlayer()->GetScore()+nScore,0);
        SelectObject(hDC,htmpFont);
    }
    else
        DrawIcon(hDC,SM_ICON_POSX,SM_ICON_POSY,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_NOBODY)));
	return 0;
}

void C_PlayerDetailWnd::Float()
{
	if ( m_nMoveTick > 0 ) return;
	if ( m_nMoveTick < 0 )
		m_nMoveTick = -m_nMoveTick;
	else
		m_nMoveTick = 0;
	m_nMoveTick++;
	SetTimer(m_hWnd,PD_MOVE_TIMERID,PD_MOVE_TIMER,NULL);
	int i = ((m_nMoveTick>0)?m_nMoveTick:-m_nMoveTick);
	MoveWindow(m_hWnd,m_nPosX-i,m_nPosY-i,PLAYERDETAIL_WIDTH,PLAYERDETAIL_HEIGHT,TRUE);
}

void C_PlayerDetailWnd::Sink()
{
	if ( m_nMoveTick <= 0 ) return ;
	m_nMoveTick = -m_nMoveTick;
	m_nMoveTick++;
	SetTimer(m_hWnd,PD_MOVE_TIMERID,PD_MOVE_TIMER,NULL);
	int i = ((m_nMoveTick>0)?m_nMoveTick:-m_nMoveTick);
	MoveWindow(m_hWnd,m_nPosX-i,m_nPosY-i,PLAYERDETAIL_WIDTH,PLAYERDETAIL_HEIGHT,TRUE);
}

void C_PlayerDetailWnd::ShowActive()
{
	if ( !m_bActivate )
	{
		m_bActivate = TRUE;
		SetTimer(m_hWnd,PD_ACTIVE_PLAYER_TIMERID,PD_ACTIVE_PLAYER_TIMER,NULL);
		FlashCaption(TRUE);
	}
	RECT rc;
	rc.left = 10;rc.right = 74;
	rc.top = 78;rc.bottom = 94;
	InvalidateRect(m_hWnd,&rc,FALSE);
}

void C_PlayerDetailWnd::ShowInactive()
{
	FlashCaption(FALSE);

	KillTimer(m_hWnd,PD_ACTIVE_PLAYER_TIMERID);
	m_bActivate = FALSE;
	RECT rc;
	rc.left = 10;rc.right = 74;
	rc.top = 78;rc.bottom = 94;
	InvalidateRect(m_hWnd,&rc,FALSE);
}

void C_PlayerDetailWnd::FlashCaption(int bFlag)
{
    C_JubmooWnd * pJubmooWnd = ((C_JubmooWnd*)m_pParentWnd);
    if ( m_pPlayer != NULL )
        if ( m_pPlayer->GetPlayerID() == pJubmooWnd->GetJubmoo()->GetMyPlayer()->GetPlayerID() )
		{
			if ( pJubmooWnd->GetHandle() != GetActiveWindow() || !bFlag )
			{
				int nRet = FlashWindow(m_pParentWnd->GetHandle(),TRUE);
				if ( !bFlag && nRet == 0 )
					FlashWindow(m_pParentWnd->GetHandle(),TRUE);
			}
		}
}

///////////////////// Event Handler /////////////////////////
LRESULT C_PlayerDetailWnd::OnPaint(WPARAM ,LPARAM )
{
    PAINTSTRUCT ps;
    m_hDC = BeginPaint( m_hWnd, &ps );

    // Paint Background
	RECT rc;
	GetClientRect(m_hWnd,&rc);
	BitBlt(m_hDC,0,0,rc.right-rc.left,rc.bottom-rc.top,m_hdcBkMem,0,0,SRCCOPY);

	SetBkMode(m_hDC,TRANSPARENT);
    switch( m_nDisplayMode )
    {
    case SHOW_JMPLAYER_NORMAL: PaintNormal(m_hDC); break;
    case SHOW_JMPLAYER_INFO: PaintInfo(m_hDC); break;
    case SHOW_JMPLAYER_SCOREONHAND: PaintScoreOnHand(m_hDC); break;
    }
	
	if ( m_pPlayerWnd != NULL && m_bActivate )
	{
		int nIconID = 0;
		int nPosX = 0;
		switch (m_pPlayer->GetJubmoo()->GetTurnNumber())
		{
		case 0: nIconID = IDI_POS1; nPosX = 10;break;
		case 1: nIconID = IDI_POS2; nPosX = 26; break;
		case 2: nIconID = IDI_POS3; nPosX = 42; break;
		case 3: nIconID = IDI_POS4; nPosX = 58; break;
		}
		DrawIconEx(m_hDC,nPosX,78,
		    	LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nIconID)),
				16,16,0,NULL,DI_NORMAL);
	}
 
	EndPaint(m_hWnd,&ps);
	return 0;
}

LRESULT C_PlayerDetailWnd::OnLButtonDown(WPARAM wParam,LPARAM lParam)
{
	Sink();
	return C_GenericClickWnd::OnLButtonDown(wParam,lParam);
}

LRESULT C_PlayerDetailWnd::OnLButtonUp(WPARAM wParam,LPARAM lParam)
{
	if ( m_pPlayer != NULL )
		Float();

	if ( m_bMouseAbove && m_bLPressed )
	{
        switch ( m_nDisplayMode )
        {
        case SHOW_JMPLAYER_NORMAL:
            SetDisplayMode(SHOW_JMPLAYER_INFO);
            break;
        case SHOW_JMPLAYER_INFO:
            SetDisplayMode(SHOW_JMPLAYER_SCOREONHAND);
            break;
        case SHOW_JMPLAYER_SCOREONHAND:
            SetDisplayMode(SHOW_JMPLAYER_NORMAL);
            break;
        }
    }
	return C_GenericClickWnd::OnLButtonUp(wParam,lParam);
}

LRESULT C_PlayerDetailWnd::OnRButtonDown(WPARAM wParam,LPARAM lParam)
{
	Sink();
	return C_GenericClickWnd::OnRButtonDown(wParam,lParam);
}

LRESULT C_PlayerDetailWnd::OnRButtonUp(WPARAM wParam,LPARAM lParam)
{
	if ( m_pPlayer != NULL )
		Float();

	if ( m_bMouseAbove && m_bRPressed )
	{
		int nDisplayMode; 
        switch ( m_nDisplayMode )
        {
        case SHOW_JMPLAYER_NORMAL:
            nDisplayMode = SHOW_JMPLAYER_INFO;
            break;
        case SHOW_JMPLAYER_INFO:
            nDisplayMode = SHOW_JMPLAYER_SCOREONHAND;
            break;
        case SHOW_JMPLAYER_SCOREONHAND:
            nDisplayMode = SHOW_JMPLAYER_NORMAL;
            break;
        }
		for ( int i = 0 ; i<4 ; i++ )
		{
			((C_JubmooWnd*)m_pParentWnd)->GetPlayerDetailWnd(i)->SetDisplayMode(nDisplayMode);
		}
    }
	return C_GenericClickWnd::OnRButtonUp(wParam,lParam);
}

LRESULT C_PlayerDetailWnd::OnEraseBkgnd(WPARAM ,LPARAM )
{
	return 1;
}

LRESULT C_PlayerDetailWnd::OnTimer(WPARAM wParam,LPARAM )
{
	if ( wParam == PD_ACTIVE_PLAYER_TIMERID )
	{
		FlashCaption(TRUE);
		return 0;
	}

//	TRACE("#%d",m_nMoveTick);
	m_nMoveTick++;
	if ( m_nMoveTick >= 5 || m_nMoveTick == 0 )
	{
		if ( m_nMoveTick > 5 ) m_nMoveTick = 5;
		KillTimer(m_hWnd,PD_MOVE_TIMERID);
	}
	int i = ((m_nMoveTick>0)?m_nMoveTick:-m_nMoveTick);
	MoveWindow(m_hWnd,m_nPosX-i,m_nPosY-i,PLAYERDETAIL_WIDTH,PLAYERDETAIL_HEIGHT,TRUE);

    if ( m_nMoveTick == 0 )
    	if ( m_pPlayer != NULL )
        	//if ( m_pPlayer->GetJubmoo()->GetActivePlayerID() != m_pPlayer->GetPlayerID() )
            	Float();

	return 0;
}
