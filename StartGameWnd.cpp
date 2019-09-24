#include "StartGameWnd.h"

#include <stdio.h>
#include "resource.h"
#include "JubmooWnd.h"
#include "CoolButton.h"
#include "PlayerWnd.h"

#define EMPTY_LOT			-1
#define SETAS_SPECTATOR		0

#define NO_SELECTED_PLAYER	-1

#define IDC_SCORE500		1100
#define IDC_SCORE1000		1101
#define IDC_SCORE2000		1102
#define IDC_ISSWAPJQ		1103

BEGIN_HANDLE_MSG(C_StartGameWnd,C_GenericWnd)
	HANDLE_MSG(WM_PAINT,OnPaint)
	HANDLE_MSG(WM_ERASEBKGND,OnEraseBkgnd)
	HANDLE_MSG(WM_COMMAND,OnCommand)
	HANDLE_MSG(WM_RBUTTONDOWN,OnRButtonDown)

	HANDLE_MSG(WM_GENERICCLICKWND_BEGIN_DRAG,OnPlayerWndBeginDrag)
	HANDLE_MSG(WM_GENERICCLICKWND_END_DRAG,OnPlayerWndEndDrag)
	HANDLE_MSG(WM_GENERICCLICKWND_RBUTTONCLICK,OnPlayerWndRButtonClick)
END_HANDLE_MSG()

C_StartGameWnd::C_StartGameWnd(C_JubmooWnd * pJubmooWnd,LPSTART_GAME_SETTING pSetting)
			: C_GenericWnd(pJubmooWnd)
{
	m_hWnd = CreateWindow(
                    m_szClassName,
                    "",
                    WS_CHILD|WS_CLIPSIBLINGS,
                    (JUBMOOWND_WIDTH-STARTGAMEWND_WIDTH)/2,115,
                    STARTGAMEWND_WIDTH,STARTGAMEWND_HEIGHT,
                    pJubmooWnd->GetHandle(),
                    NULL,
                    C_App::m_hInstance,
                    (LPSTR) this );

    if ( ! m_hWnd )
        exit( FALSE );

	m_pJubmoo = pJubmooWnd->GetJubmoo();
	m_pJubmoo->SetPlayMode(SETTING_PLAYMODE);
    m_bInitBkg = FALSE;
	m_nDragSlot = NO_SELECTED_PLAYER;

	// Create Command Button
	RECT rcButton = {215,280,315,315};
	RECT rcGroup = { 10,220,130,310 };
	RECT rcScore = { 20,240,120,260 };
	RECT rcSwapJQ = { 140,250,315,270 };

	// only server can press start game
	m_pContinueButton = new C_CoolButton;
	m_pGroup = new C_CoolButton;
	m_pScore500 = new C_CoolButton;
	m_pScore1000 = new C_CoolButton;
	m_pScore2000 = new C_CoolButton;
	m_pSwapJQ = new C_CoolButton;

	m_Tooltip.Create(this);

	DWORD dwEnable;
	if ( m_pJubmoo->GetGameMode() == SERVER_GAMEMODE )
		dwEnable = 0;
	else
		dwEnable = WS_DISABLED;

	m_pContinueButton->Create("Start Game!",NULL,
			            BS_PUSHBUTTON|WS_VISIBLE|WS_CHILD|dwEnable,
				        rcButton,this,IDOK);
	m_pContinueButton->SetFont(pJubmooWnd->GetJmFont(),FALSE);

	m_pScore500->Create("-500",NULL,
			            BS_AUTORADIOBUTTON|WS_VISIBLE|WS_CHILD|WS_GROUP|dwEnable,
				        rcScore,this,IDC_SCORE500);
	m_pScore500->SetFont(pJubmooWnd->GetJmFont(),FALSE);
	
	OffsetRect(&rcScore,0,20);
	m_pScore1000->Create("-1000",NULL,
			            BS_AUTORADIOBUTTON|WS_VISIBLE|WS_CHILD|dwEnable,
				        rcScore,this,IDC_SCORE1000);
	m_pScore1000->SetFont(pJubmooWnd->GetJmFont(),FALSE);
	m_pScore1000->SetCheck(BST_CHECKED);

	OffsetRect(&rcScore,0,20);
	m_pScore2000->Create("-2000",NULL,
			            BS_AUTORADIOBUTTON|WS_VISIBLE|WS_CHILD|dwEnable,
				        rcScore,this,IDC_SCORE2000);
	m_pScore2000->SetFont(pJubmooWnd->GetJmFont(),FALSE);

	m_pSwapJQ->Create("If 'Chuan', Swap value J by Q",NULL,
			            BS_AUTOCHECKBOX|WS_VISIBLE|WS_CHILD|dwEnable,
				        rcSwapJQ,this,IDC_ISSWAPJQ);
	m_pSwapJQ->SetFont(pJubmooWnd->GetJmFont(),FALSE);
	m_Tooltip.AddTool(m_pSwapJQ->GetHandle(),"ถ้ามีการช้วนเกิดขึ้น J จะเป็น -100 และ Q จะเป็น +100");

	m_pGroup->Create("Playing Score",NULL,
			            BS_GROUPBOX|WS_VISIBLE|WS_CHILD|dwEnable,
				        rcGroup,this,-1);
	m_pGroup->SetFont(pJubmooWnd->GetJmFont(),FALSE);

	int i;
   	for ( i=0 ; i<8 ; i++ )
		m_pPlayerWndLotStatus[i] = NULL;
	for ( i=0 ; i<4 ; i++ )
		m_nSelectedSlotID[i] = NO_SELECTED_PLAYER;
	for ( i=0 ; i<8 ; i++ )
		m_nLotStatus[i] = EMPTY_LOT;
	
	m_hrgnPos[0] = CreateRectRgn(20,180,52,212);
	m_hrgnPos[1] = CreateRectRgn(20,30,52,62);
	m_hrgnPos[2] = CreateRectRgn(STARTGAMEWND_WIDTH-80,30,STARTGAMEWND_WIDTH-48,62);
	m_hrgnPos[3] = CreateRectRgn(STARTGAMEWND_WIDTH-80,180,STARTGAMEWND_WIDTH-48,212);
    
	if ( pSetting == NULL )
	{
		// make default position
		for ( i=0 ; i < m_pJubmoo->GetPlayerList()->Count() ; i++ )
    		AddPlayer(m_pJubmoo->GetPlayerList()->Items(i));
	}
	else
		SetGame(pSetting);

    BringWindowToTop(m_hWnd);
    ShowWindow(m_hWnd,SW_SHOW);
}

C_StartGameWnd::~C_StartGameWnd( void )
{
	DeleteObject(SelectObject(m_hdcBkMem,m_hbmpBkTmp));
    DeleteDC(m_hdcBkMem);

	if ( IsWindow(m_hWnd) ) DestroyWindow(m_hWnd);

    delete m_pContinueButton;
	delete m_pGroup;
	delete m_pScore500;
	delete m_pScore1000;
	delete m_pScore2000;
	delete m_pSwapJQ;

	int i;
	for ( i=0 ; i<4 ; i++ )
		DeleteObject(m_hrgnPos[i]);
    for ( i=0 ; i<8 ; i++ )
        delete m_pPlayerWndLotStatus[i];
}

int C_StartGameWnd::AddPlayer(C_Player * pPlayer)
{
	POINT nPlayerPos[8] =
    { {4,100},{44,100},{84,100},{124,100},{164,100},{204,100},{244,100},{284,100} };

    if ( pPlayer == NULL ) return FALSE;

    // finding for empty slot
    int nEmptyLot;
    for ( nEmptyLot=0 ; nEmptyLot<8 ; nEmptyLot++ )
        if ( m_pPlayerWndLotStatus[nEmptyLot] == NULL )
            break;
    // if no slot left
    if ( nEmptyLot == 8 )
        return FALSE;

    // create UI object at empty slot
    C_PlayerWnd * pPlayerWnd;
   	pPlayerWnd = new C_PlayerWnd(this,SINGLE_SIZE_PLAYERWND,
									nPlayerPos[nEmptyLot].x,nPlayerPos[nEmptyLot].y,
									pPlayer );
	// only server can change position of player
	if ( m_pJubmoo->GetGameMode() == SERVER_GAMEMODE )
	{
		pPlayerWnd->EnableDragWindow(TRUE);
		pPlayerWnd->EnableMoveBkgWindow(TRUE);
		pPlayerWnd->EnableClickToTop(TRUE);
		pPlayerWnd->EnableClickNotify(TRUE);
		pPlayerWnd->Refresh();
	}
	m_pPlayerWndLotStatus[nEmptyLot] = pPlayerWnd;
	m_nLotStatus[nEmptyLot] = SETAS_SPECTATOR;
    return TRUE;
}

int C_StartGameWnd::RemovePlayer(C_Player * pPlayer)
{
    if ( pPlayer == NULL ) return FALSE;

    for ( int i = 0 ; i < 8 ; i++ )
    {
		if ( m_pPlayerWndLotStatus[i] != NULL )
    		if ( m_pPlayerWndLotStatus[i]->GetPlayer() == pPlayer )
    		{
	            delete m_pPlayerWndLotStatus[i];
		        m_pPlayerWndLotStatus[i] = NULL;
				// if to be set as player
				if ( m_nLotStatus[i] > SETAS_SPECTATOR )
				{
					// remove from selected player slot
					m_nSelectedSlotID[m_nLotStatus[i]-1] = NO_SELECTED_PLAYER;
				}
				m_nLotStatus[i] = EMPTY_LOT;
			    return TRUE;
			}
    }
    return FALSE;
}

// nSlotID  =>  0-7
// nOrder   =>  0-3
int C_StartGameWnd::SetPlayer(int nSlotID,int nOrder)
{
	POINT nPos[4] = { {60,180},{60,30},{280,30},{280,180} };

	if ( m_pPlayerWndLotStatus[nSlotID] != NULL &&
				m_nSelectedSlotID[nOrder] == NO_SELECTED_PLAYER )
	{
		// if player present and this order is empty
		// set pPlayer to be player # nOrder
		MoveWindow(m_pPlayerWndLotStatus[nSlotID]->GetHandle(),
					nPos[nOrder].x,nPos[nOrder].y,
					PLAYERWND_WIDTH,PLAYERWND_HEIGHT,TRUE);
		m_pPlayerWndLotStatus[nSlotID]->Refresh();
		m_nLotStatus[nSlotID] = nOrder+1;
		m_nSelectedSlotID[nOrder] = nSlotID;
		return TRUE;
	}
	return FALSE;
}

int C_StartGameWnd::ClearPlayer(int nOrder)
{
	POINT nPlayerPos[8] =
    { {4,100},{44,100},{84,100},{124,100},{164,100},{204,100},{244,100},{284,100} };

	int nSelSlot = m_nSelectedSlotID[nOrder];

	if ( nSelSlot == NO_SELECTED_PLAYER ) return FALSE;

	// check for selected player
	if ( m_nLotStatus[nSelSlot] > SETAS_SPECTATOR )
	{
		MoveWindow(m_pPlayerWndLotStatus[nSelSlot]->GetHandle(),
					nPlayerPos[nSelSlot].x,nPlayerPos[nSelSlot].y,
					PLAYERWND_WIDTH,PLAYERWND_HEIGHT,TRUE);
		m_pPlayerWndLotStatus[nSelSlot]->Refresh();
		m_nLotStatus[nSelSlot] = SETAS_SPECTATOR;
		m_nSelectedSlotID[nOrder] = NO_SELECTED_PLAYER;
		return TRUE;
	}
	return FALSE;
}

int C_StartGameWnd::MovePlayer(int nSlotID,int nOrder,int nOldOrder)
{
	int nSelSlot = m_nSelectedSlotID[nOldOrder];
	if ( nSelSlot == NO_SELECTED_PLAYER ) return FALSE;

	// must be player
	if ( m_nLotStatus[nSelSlot] > SETAS_SPECTATOR )
	{
		m_nLotStatus[nSelSlot] = SETAS_SPECTATOR;
		m_nSelectedSlotID[nOldOrder] = NO_SELECTED_PLAYER;
		// set at new order
		return SetPlayer(nSlotID,nOrder);
	}
	return FALSE;
}

int C_StartGameWnd::SetGame(LPSTART_GAME_SETTING pSetting)
{
	// set game by input setting
	int i,j;
	LockWindowUpdate(m_hWnd);

	// add player , to make it match with playerlist in jubmoo
	for ( i=0 ; i<m_pJubmoo->GetPlayerList()->Count() ; i++ )
	{
		for ( j=0 ; j<8 ; j++ )
		{
			if ( m_pPlayerWndLotStatus[j] != NULL )
				if ( m_pPlayerWndLotStatus[j]->GetPlayer()->GetPlayerID() ==
						m_pJubmoo->GetPlayerList()->Items(i)->GetPlayerID() )
				{
					// player already in start window
					break;
				}
		}
		if ( j==8 ) // player not found in start window
		{
			AddPlayer(m_pJubmoo->GetPlayerList()->Items(i));
		}
	}

	// remove set player
	ClearPlayer(0);ClearPlayer(1);ClearPlayer(2);ClearPlayer(3);

	// reset player
	for ( i=0 ; i<8 ; i++ )
		if ( m_pPlayerWndLotStatus[i] != NULL )
		{
			for ( int j=0 ; j<4 ; j++ )
				if ( m_pPlayerWndLotStatus[i]->GetPlayer()->GetPlayerID() ==
						pSetting->nPlayerID[j] )
				{
					SetPlayer(i,j);
					continue;
				}
		}

	m_pScore500->SetCheck(FALSE);
	m_pScore1000->SetCheck(FALSE);
	m_pScore2000->SetCheck(FALSE);
	switch ( pSetting->nGameScore )
	{
	case -500: m_pScore500->SetCheck(TRUE); break;
	case -1000: m_pScore1000->SetCheck(TRUE); break;
	case -2000: m_pScore2000->SetCheck(TRUE); break;
	}
	m_pSwapJQ->SetCheck( (WORD)pSetting->bSwapJQ );

	LockWindowUpdate(NULL);
	return 0;
}

int C_StartGameWnd::SendUpdateSetting()
{
	// get current setting and change to setting structure
	START_GAME_SETTING GameSetting;

	// empty slot must be more than 0
	// to distinct from normal player
	// that why player ID must start from MYID ( default is 1 )
	for ( int i=0 ; i<4 ; i++ )
		if ( m_nSelectedSlotID[i] == NO_SELECTED_PLAYER )
			GameSetting.nPlayerID[i] = 0;
		else
			GameSetting.nPlayerID[i] = m_pPlayerWndLotStatus[m_nSelectedSlotID[i]]->GetPlayer()->GetPlayerID();

	if ( m_pScore500->GetCheck() == BST_CHECKED )
		GameSetting.nGameScore =-500;
	else if ( m_pScore1000->GetCheck() == BST_CHECKED )
		GameSetting.nGameScore =-1000;
	else if ( m_pScore2000->GetCheck() == BST_CHECKED )
		GameSetting.nGameScore =-2000;

	if ( m_pSwapJQ->GetCheck() == BST_CHECKED )
		GameSetting.bSwapJQ = 1;
	else
		GameSetting.bSwapJQ = 0;

	GetJubmoo()->UpdateSetting(&GameSetting);
	return 0;
}

void C_StartGameWnd::Refresh()
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
    m_hbmpBkMem = CreateCompatibleBitmap(m_hDC,STARTGAMEWND_WIDTH,STARTGAMEWND_HEIGHT);
    m_hbmpBkTmp = (HBITMAP)SelectObject(m_hdcBkMem,m_hbmpBkMem);
    BitBlt(m_hdcBkMem,0,0,STARTGAMEWND_WIDTH,STARTGAMEWND_HEIGHT,m_hDC,0,0,SRCCOPY);

    RECT rc;
    GetClientRect(m_hWnd,&rc);

	DrawEdge( m_hdcBkMem,&rc,EDGE_ETCHED,BF_RECT);
	DrawIcon(m_hdcBkMem,20,180,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS1)));
	DrawIcon(m_hdcBkMem,20,30,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS2)));
	DrawIcon(m_hdcBkMem,rc.right-80,30,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS3)));
	DrawIcon(m_hdcBkMem,rc.right-80,180,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS4)));

    ReleaseDC(m_hWnd,m_hDC);
}


// =========================================================
// Event handler
// =========================================================
LRESULT C_StartGameWnd::OnPaint(WPARAM ,LPARAM )
{
	POINT nPlayerPos[8] =
    { {4,100},{44,100},{84,100},{124,100},{164,100},{204,100},{244,100},{284,100} };
	POINT nPos[4] = { {60,180},{60,30},{280,30},{280,180} };

    PAINTSTRUCT ps;
    m_hDC = BeginPaint( m_hWnd, &ps );

	BitBlt(m_hDC,0,0,STARTGAMEWND_WIDTH,STARTGAMEWND_HEIGHT,m_hdcBkMem,0,0,SRCCOPY);

	int i;
	for ( i=0 ; i<8 ; i++ )
		if ( m_nLotStatus[i] == SETAS_SPECTATOR && i != m_nDragSlot )
		{
			int nResID = m_pPlayerWndLotStatus[i]->GetCharacterResID(m_pPlayerWndLotStatus[i]->GetPlayer()->GetCharacterID());
			DrawIcon(m_hDC,nPlayerPos[i].x,nPlayerPos[i].y,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResID)));
		}
	for ( i=0 ; i<4 ; i++ )
		if ( m_nSelectedSlotID[i] > NO_SELECTED_PLAYER && m_nSelectedSlotID[i] != m_nDragSlot )
		{
			int nResID = m_pPlayerWndLotStatus[m_nSelectedSlotID[i]]->GetCharacterResID(m_pPlayerWndLotStatus[m_nSelectedSlotID[i]]->GetPlayer()->GetCharacterID());
			DrawIcon(m_hDC,nPos[i].x,nPos[i].y,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResID)));
		}
	EndPaint( m_hWnd, &ps );
	return 0;
}

LRESULT C_StartGameWnd::OnEraseBkgnd(WPARAM wParam,LPARAM )
{
    if ( !m_bInitBkg )
    {
        m_hDC = (HDC)wParam;
        m_hdcBkMem = CreateCompatibleDC(m_hDC);
        m_hbmpBkMem = CreateCompatibleBitmap(m_hDC,STARTGAMEWND_WIDTH,STARTGAMEWND_HEIGHT);
        m_hbmpBkTmp = (HBITMAP)SelectObject(m_hdcBkMem,m_hbmpBkMem);
        BitBlt(m_hdcBkMem,0,0,STARTGAMEWND_WIDTH,STARTGAMEWND_HEIGHT,m_hDC,0,0,SRCCOPY);
		RECT rc;
		GetClientRect(m_hWnd,&rc);

		DrawEdge( m_hdcBkMem,&rc,EDGE_ETCHED,BF_RECT);
		DrawIcon(m_hdcBkMem,20,180,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS1)));
		DrawIcon(m_hdcBkMem,20,30,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS2)));
		DrawIcon(m_hdcBkMem,rc.right-80,30,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS3)));
		DrawIcon(m_hdcBkMem,rc.right-80,180,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_POS4)));
        m_bInitBkg = TRUE;
    }
	return 1;
}

LRESULT C_StartGameWnd::OnCommand(WPARAM wParam,LPARAM )
{
	switch ( LOWORD(wParam) )
    {
	case IDC_SCORE500:
	case IDC_SCORE1000:
	case IDC_SCORE2000:
	case IDC_ISSWAPJQ:
//		TRACE("option setting changed\n");
		// send changing to all other client
		SendUpdateSetting();
		break;
    case IDOK:
    	// validate player
//		TRACE("TRACE Lotstatu %d,%d,%d,%d,%d,%d,%d,%d\n",
//			m_nLotStatus[0],m_nLotStatus[1],m_nLotStatus[2],m_nLotStatus[3],
//			m_nLotStatus[4],m_nLotStatus[5],m_nLotStatus[6],m_nLotStatus[7]);
//		TRACE("TRACE selectslot %d,%d,%d,%d\n",
//			m_nSelectedSlotID[0],m_nSelectedSlotID[1],m_nSelectedSlotID[2],
//			m_nSelectedSlotID[3]);
		// notify to parent to start game
		m_pJubmoo->StartGame();
    	break;
    }
    return 0;
}

LRESULT C_StartGameWnd::OnRButtonDown(WPARAM wParam,LPARAM lParam)
{
	return ((C_JubmooWnd*)m_pParentWnd)->OnRButtonDown(wParam,lParam);
}

LRESULT C_StartGameWnd::OnPlayerWndBeginDrag(WPARAM wParam,LPARAM lParam)
{
    for ( int i = 0 ; i < 8 ; i++ )
    {
		if ( m_pPlayerWndLotStatus[i] != NULL )
    		if ( m_pPlayerWndLotStatus[i] == (C_PlayerWnd*)lParam )
				m_nDragSlot = i;
    }
	m_pntLastPos.x = (int)LOWORD(wParam);
	m_pntLastPos.y = (int)HIWORD(wParam);
	return 0;
}

LRESULT C_StartGameWnd::OnPlayerWndEndDrag(WPARAM wParam,LPARAM lParam)
{
	// check end point
	int nX = (int)LOWORD(wParam)+16;
	int nY = (int)HIWORD(wParam)+16;

	int nOrder = -1;

	if ( PtInRegion(m_hrgnPos[0],nX,nY) )
		nOrder = 0;
	else if ( PtInRegion(m_hrgnPos[1],nX,nY) )
		nOrder = 1;
	else if ( PtInRegion(m_hrgnPos[2],nX,nY) )
		nOrder = 2;
	else if ( PtInRegion(m_hrgnPos[3],nX,nY) )
		nOrder = 3;

	if ( nOrder < 0 || m_nSelectedSlotID[nOrder] > NO_SELECTED_PLAYER )
	{
//		TRACE("error moving %d\n",nOrder);
		MoveWindow( ((C_Wnd*)lParam)->GetHandle(),
				m_pntLastPos.x,m_pntLastPos.y,
				PLAYERWND_WIDTH,PLAYERWND_HEIGHT,
				TRUE);
		((C_GenericClickWnd*)lParam)->Refresh();
	}
	else
	{
		// then set new player
		if ( m_nLotStatus[m_nDragSlot] > SETAS_SPECTATOR )
		{
//			TRACE("Clear from %d\n",m_nLotStatus[m_nDragSlot]-1);
			MovePlayer(m_nDragSlot,nOrder,m_nLotStatus[m_nDragSlot]-1);
			// clear last player
//			ClearPlayer(m_nLotStatus[m_nDragSlot]-1);
		}
		else
		{
//			TRACE("Set player %d\n",nOrder);
			SetPlayer(m_nDragSlot,nOrder);
		}

		// send changing to all other client
		SendUpdateSetting();
	}
	m_nDragSlot = NO_SELECTED_PLAYER;
 	return 0;
}

LRESULT C_StartGameWnd::OnPlayerWndRButtonClick(WPARAM ,LPARAM lParam)
{
	// only server do that,
    if ( m_pJubmoo->GetGameMode() != SERVER_GAMEMODE )
    	return 0;

	// if player , clear him
	int nSlot;
	for ( nSlot = 0 ; nSlot < 8 ; nSlot++ )
	{
		if ( m_pPlayerWndLotStatus[nSlot] == (C_PlayerWnd*)lParam )
			break;
	}
	if ( m_nLotStatus[nSlot] > SETAS_SPECTATOR )
	{
		ClearPlayer(m_nLotStatus[nSlot]-1);

		// send changing to all other client
		SendUpdateSetting();
	}

	return 0;
}

