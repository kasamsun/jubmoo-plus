#include "EndGameWnd.h"

#include "kswafx/DCEx.h"
#include "resource.h"
#include "JubmooWnd.h"
#include "kswafx/CoolButton.h"
#include "Jubmoo.h"

#define PIGDANCE_TIMERID	6001
#define PIGDANCE_TIMER		500

BEGIN_HANDLE_MSG(C_EndGameWnd,C_GenericWnd)
	HANDLE_MSG(WM_PAINT,OnPaint)
	HANDLE_MSG(WM_ERASEBKGND,OnEraseBkgnd)
	HANDLE_MSG(WM_RBUTTONDOWN,OnRButtonDown)

	HANDLE_MSG(WM_COMMAND,OnCommand)
	HANDLE_MSG(WM_TIMER,OnTimer)
END_HANDLE_MSG()

C_EndGameWnd::C_EndGameWnd(C_JubmooWnd * pJubmooWnd) : C_GenericWnd(pJubmooWnd)
{
	m_hWnd = CreateWindow(
                    m_szClassName,
                    "",
                    WS_CHILD|WS_CLIPSIBLINGS,
                    (JUBMOOWND_WIDTH-ENDGAMEWND_WIDTH)/2,115,
                    ENDGAMEWND_WIDTH,ENDGAMEWND_HEIGHT,
                    pJubmooWnd->GetHandle(),
                    NULL,
                    C_App::m_hInstance,
                    (LPSTR) this );

    if ( ! m_hWnd )
        exit( FALSE );

	m_pJubmoo = pJubmooWnd->GetJubmoo();
    m_bInitBkg = FALSE;

	// Create Command Button
	RECT rcButton = {500,190,620,230};

	// only server can press start game
	m_pContinueButton = new C_CoolButton;

	DWORD dwEnable;
	if ( m_pJubmoo->GetGameMode() == SERVER_GAMEMODE )
		dwEnable = 0;
	else
		dwEnable = WS_DISABLED;

	m_pContinueButton->Create("Continue",NULL,
			            BS_PUSHBUTTON|WS_VISIBLE|WS_CHILD|dwEnable,
				        rcButton,this,IDOK);
	m_pContinueButton->SetFont(pJubmooWnd->GetJmFont(),FALSE);

	CreatePlayerWnd();
    SetPigImg();

    BringWindowToTop(m_hWnd);
    ShowWindow(m_hWnd,SW_SHOW);

    m_nPigFrame = 0;
	SetTimer(m_hWnd,PIGDANCE_TIMERID,PIGDANCE_TIMER,NULL);
}

C_EndGameWnd::~C_EndGameWnd()
{
	ClearPigImg();
	KillTimer(m_hWnd,PIGDANCE_TIMERID);
	DeleteObject(SelectObject(m_hdcBkMem,m_hbmpBkTmp));
    DeleteDC(m_hdcBkMem);

	if ( IsWindow(m_hWnd) ) DestroyWindow(m_hWnd);

    delete m_pContinueButton;

	m_pPigPlayerWnd.DeleteObjects();
	m_pPlayerWnd.DeleteObjects();
}

void C_EndGameWnd::CreatePlayerWnd()
{
	int i,j;
	C_Player * pPlayer;
	C_Player * pNonPigPlayer[4];

	// find pig player
	int nPigPos = 352;
	int nNonPigCount = 0;
	for ( i=0 ; i<4 ; i++ )
	{
		pPlayer = m_pJubmoo->GetPlayer(m_pJubmoo->GetGameSetting()->nPlayerID[i]);
		if ( pPlayer == NULL ) continue;
		if ( pPlayer->GetScore() <= m_pJubmoo->GetGameSetting()->nGameScore )
		{
			m_pPigPlayerWnd.Add( new C_PlayerWnd(this,DOUBLE_SIZE_PLAYERWND,
									nPigPos,70,pPlayer ) );
			nPigPos+=68;
		}
		else
		{
			pNonPigPlayer[nNonPigCount++] = pPlayer;
		}
	}

	// sort score
	for ( j=0 ; j<nNonPigCount-1 ; j++ )
		for ( i=0 ; i<nNonPigCount-j-1 ; i++ )
		{
			if ( pNonPigPlayer[i]->GetScore() < pNonPigPlayer[i+1]->GetScore() )
			{
				C_Player * pTmpPlayer = pNonPigPlayer[i];
				pNonPigPlayer[i] = pNonPigPlayer[i+1];
				pNonPigPlayer[i+1] = pTmpPlayer;
			}
		}

	POINT nNonPigPos[3] = { {128,30},{64,50},{192,50} };
	for ( i=0 ; i<nNonPigCount ; i++ )
	{
		m_pPlayerWnd.Add( new C_PlayerWnd(this,DOUBLE_SIZE_PLAYERWND,
						nNonPigPos[i].x,nNonPigPos[i].y,pNonPigPlayer[i] ) );
	}

}

void C_EndGameWnd::Refresh()
{
	m_bInitBkg = FALSE;
    RECT rMyRect;
    GetClientRect(m_hWnd,&rMyRect);
    ClientToScreen(m_hWnd,(LPPOINT)&rMyRect.left);
    ClientToScreen(m_hWnd,(LPPOINT)&rMyRect.right);
    ScreenToClient(m_pParentWnd->GetHandle(),(LPPOINT)&rMyRect.left);
    ScreenToClient(m_pParentWnd->GetHandle(),(LPPOINT)&rMyRect.right);
    InvalidateRect(m_pParentWnd->GetHandle(),&rMyRect,FALSE);

	DeleteObject(SelectObject(m_hdcBkMem,m_hbmpBkTmp));

    m_hDC = GetDC(m_hWnd);
    m_hbmpBkMem = CreateCompatibleBitmap(m_hDC,ENDGAMEWND_WIDTH,ENDGAMEWND_HEIGHT);
    m_hbmpBkTmp = (HBITMAP)SelectObject(m_hdcBkMem,m_hbmpBkMem);
    BitBlt(m_hdcBkMem,0,0,ENDGAMEWND_WIDTH,ENDGAMEWND_HEIGHT,m_hDC,0,0,SRCCOPY);

    RECT rc;
    GetClientRect(m_hWnd,&rc);

	DrawEdge( m_hdcBkMem,&rc,EDGE_ETCHED,BF_RECT);
	DrawIconEx(m_hdcBkMem,64,120,
        	LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS2)),
			64,64,0,NULL,DI_NORMAL);
	DrawIconEx(m_hdcBkMem,128,100,
        	LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS1)),
			64,84,0,NULL,DI_NORMAL);
	DrawIconEx(m_hdcBkMem,192,120,
        	LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS3)),
			64,64,0,NULL,DI_NORMAL);

    ReleaseDC(m_hWnd,m_hDC);
}

void C_EndGameWnd::SetPigImg()
{
	m_hDC = GetDC(m_hWnd);

    m_hdcPig = CreateCompatibleDC(m_hDC);
    m_hbmpPig = LoadBitmap(C_App::m_hInstance,MAKEINTRESOURCE(IDB_JMDANCE));
    m_hbmpPigTmp = (HBITMAP) SelectObject(m_hdcPig,m_hbmpPig);

    BITMAP bm;
    GetObject(m_hbmpPig,sizeof(BITMAP),&bm);
    m_hdcPigMsk = CreateCompatibleDC(m_hDC);
    C_DCEx * pDCEx = new C_DCEx(m_hdcPig);
    m_hbmpPigMsk = pDCEx->CreateMaskBitmap(m_hdcPig,bm.bmWidth,bm.bmHeight,TRANSPARENT_COLOR);
    m_hbmpPigMskTmp = (HBITMAP)SelectObject(m_hdcPigMsk,m_hbmpPigMsk);

    ReleaseDC(m_hWnd,m_hDC);
}

void C_EndGameWnd::ClearPigImg()
{
	DeleteObject(SelectObject(m_hdcPig,m_hbmpPigTmp));
    DeleteDC(m_hdcPig);
	DeleteObject(SelectObject(m_hdcPigMsk,m_hbmpPigMskTmp));
    DeleteDC(m_hdcPigMsk);
}

// =========================================================
// Event handler
// =========================================================
LRESULT C_EndGameWnd::OnPaint(WPARAM ,LPARAM )
{
    PAINTSTRUCT ps;
    m_hDC = BeginPaint( m_hWnd, &ps );

	BitBlt(m_hDC,0,0,ENDGAMEWND_WIDTH,ENDGAMEWND_HEIGHT,m_hdcBkMem,0,0,SRCCOPY);

	EndPaint( m_hWnd, &ps );
	return 0;
}

LRESULT C_EndGameWnd::OnEraseBkgnd(WPARAM wParam,LPARAM )
{
    if ( !m_bInitBkg )
    {
        m_hDC = (HDC)wParam;
        m_hdcBkMem = CreateCompatibleDC(m_hDC);
        m_hbmpBkMem = CreateCompatibleBitmap(m_hDC,ENDGAMEWND_WIDTH,ENDGAMEWND_HEIGHT);
        m_hbmpBkTmp = (HBITMAP)SelectObject(m_hdcBkMem,m_hbmpBkMem);
        BitBlt(m_hdcBkMem,0,0,ENDGAMEWND_WIDTH,ENDGAMEWND_HEIGHT,m_hDC,0,0,SRCCOPY);
		RECT rc;
		GetClientRect(m_hWnd,&rc);

		DrawEdge( m_hdcBkMem,&rc,EDGE_ETCHED,BF_RECT);
		DrawIconEx(m_hdcBkMem,64,120,
        		LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS2)),
				64,64,0,NULL,DI_NORMAL);
		DrawIconEx(m_hdcBkMem,128,100,
        		LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS1)),
				64,84,0,NULL,DI_NORMAL);
		DrawIconEx(m_hdcBkMem,192,120,
        		LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS3)),
				64,64,0,NULL,DI_NORMAL);

        m_bInitBkg = TRUE;
    }
	return 1;
}

LRESULT C_EndGameWnd::OnCommand(WPARAM wParam,LPARAM )
{
	switch ( LOWORD(wParam) )
    {
    case IDOK:
		m_pJubmoo->ResetGame();
    	break;
    }
    return 0;
}

LRESULT C_EndGameWnd::OnTimer(WPARAM ,LPARAM )
{
	m_nPigFrame++;
    if ( m_nPigFrame == 4 ) m_nPigFrame = 0;

	m_hDC = GetDC(m_hWnd);

    C_DCEx * pDCEx = new C_DCEx(m_hDC);

    int yPos[2] = {20,144};
    for ( int i=0 ; i<2 ; i++ )
    	for ( int j=0 ; j<6 ; j++ )
        {
            BitBlt(m_hDC,302+j*50,yPos[i],40,40,m_hdcBkMem,302+j*50,yPos[i],SRCCOPY);
            pDCEx->TransparentBlt(302+j*50,yPos[i],40,40,m_hdcPig,m_hdcPigMsk,40*((m_nPigFrame+j)%4),0);
	    }

    delete pDCEx;
    ReleaseDC(m_hWnd,m_hDC);
    return 0;
}

LRESULT C_EndGameWnd::OnRButtonDown(WPARAM wParam,LPARAM lParam)
{
	return ((C_JubmooWnd*)m_pParentWnd)->OnRButtonDown(wParam,lParam);
}

