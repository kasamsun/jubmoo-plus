#include "JubmooWnd.h"

#include <stdio.h>
#include "kswafx/DCEx.h"
#include "kswafx/CoolButton.h"
#include "kswafx/CoolDlg.h"
#include "kswafx/GenericClickWnd.h"

#include "resource.h"
#include "CardNum.h"
#include "Jubmoo.h"
#include "AboutDlg.h"
#include "HelpDlg.h"
#include "InfoDlg.h"
#include "SettingDlg.h"
#include "ChatEdit.h"
#include "PlayerDetailWnd.h"
#include "PlayerWnd.h"
#include "CardWnd.h"
#include "BmpDigit.h"
#include "EndGameWnd.h"
#include "JMComm.h"
#include "JoinDlg.h"
#include "RoundDlg.h"

// Note :: 1.  now it'll have leak , if u exit game when card is picking up.

#define START_BUTTON	0
#define JOIN_BUTTON		1
#define QUIT_BUTTON		2
#define SETTING_BUTTON	3
#define INFO_BUTTON		4
#define HELP_BUTTON		5
#define ABOUT_BUTTON	6
#define EXIT_BUTTON		7

#define PICKUPCARD_READY	1
#define PICKUPCARD_TIMER	1000
#define AI_TIMER			2000

extern HHOOK hCoolDlgHook;
extern C_JubmooApp JubmooApp;

BEGIN_HANDLE_MSG(C_JubmooWnd,C_GenericWnd)
	HANDLE_MSG(WM_PAINT,OnPaint)
	HANDLE_MSG(WM_NCLBUTTONDBLCLK,OnNCLButtonDblClk)
	HANDLE_MSG(WM_COMMAND,OnCommand)
	HANDLE_MSG(WM_CLOSE,OnClose)
	HANDLE_MSG(WM_DESTROY,OnDestroy)
    HANDLE_MSG(WM_CHAR,OnChar)

	HANDLE_MSG(WM_GENERICCLICKWND_LBUTTONCLICK,OnCardWndLButtonClick)
	HANDLE_MSG(WM_PICKCARD_MOVED,OnPickCardReady)
	HANDLE_MSG(WM_RBUTTONDOWN,OnRButtonDown)
	HANDLE_MSG(WM_TIMER,OnTimer)
END_HANDLE_MSG()

C_JubmooWnd::C_JubmooWnd(C_Jubmoo * pJubmoo) : C_GenericWnd()
{
	RECT rc;

	SetRect(&rc,0,0,JUBMOOWND_WIDTH,JUBMOOWND_HEIGHT);
	AdjustWindowRect(&rc,WS_OVERLAPPED|WS_SYSMENU|WS_MINIMIZEBOX|WS_DLGFRAME|WS_CAPTION,NULL);

	m_hWnd = CreateWindow(
                    m_szClassName,
                    "JubMoo+",
                    WS_OVERLAPPED|WS_SYSMENU|WS_MINIMIZEBOX|WS_DLGFRAME|
							WS_CAPTION|WS_CLIPSIBLINGS,
                    rc.left,
                    rc.top,
                    rc.right-rc.left,
                    rc.bottom-rc.top,
                    NULL,
                    NULL,
                    C_App::m_hInstance,
                    (LPSTR) this );

    if ( ! m_hWnd )
        exit( FALSE );

	MoveToCenter();

	m_hdcBkMem = NULL;

	m_pJubmoo = pJubmoo;
	for ( int i=0 ;i<MAX_NO_OF_PLAYER; i++ )
        m_pPlayerLotStatus[i] = NULL;

	m_pStartGameWnd = NULL;
    m_pEndGameWnd = NULL;
	m_pDialog = NULL;
	m_bPickingUp = FALSE;
	
	for ( int j=0 ; j<4 ; j++ )
		m_pPickCardWnd[j] = NULL;

	// Create Command Button
	InitScreen();

	// should show window after initializing in Jumboo class
// for test endgamewnd
//    ShowEndGame();
}

C_JubmooWnd::~C_JubmooWnd( void )
{
	int i;
    for ( i = 0 ; i<MAX_JUBMOO_BUTTON ; i++ )
        delete m_pButton[i];
	delete m_pContinueButton;
    delete m_pChatEdit;
    for ( i = 0 ; i<4 ; i++ )
	{
		delete m_pPlayerDetailWnd[i];
		delete m_pPickCardWnd[i];
	}

    m_MyCardList.DeleteObjects();
	m_DropCardList.DeleteObjects();

    delete m_pStartGameWnd;
    delete m_pEndGameWnd;

   	C_CoolDlg::CoolDlgClearImage();
    C_CardWnd::ClearBackCardImage();
    C_CardWnd::ClearCardImage();
	//  this clear image is no need , because jubmoownd is destroyed
	//  before jubmoo destructor
	//ClearImage();
	CoolDlgTerminate();
}

void C_JubmooWnd::InitScreen()
{
	int i;

	CreateJmFont();

	RECT rcButton[MAX_JUBMOO_BUTTON] = {
                        {10,500,42,532},
                        {50,500,82,532},
                        {90,500,122,532},
                        {130,500,162,532},
                        {170,500,202,532},
                        {600,500,632,532},
                        {640,500,672,532},
                        {680,500,712,532} };
    char szButtonCaption[8][8] = {
                        "Start","Join","Quit",
                        "Setting","Info","Help",
                        "About","Exit" };
    UINT nButtonID[MAX_JUBMOO_BUTTON] = {
                        ID_JM_START,ID_JM_JOIN,ID_JM_QUIT,
                        ID_JM_SETTING,ID_JM_INFO,ID_JM_HELP,
                        ID_JM_ABOUT,ID_JM_EXIT};
	int nButtonResID[MAX_JUBMOO_BUTTON] = {
						IDI_START,IDI_JOIN,IDI_QUIT,IDI_SETTING,IDI_INFO,
						IDI_HELP,IDI_ABOUT,IDI_EXIT };

 
	m_Tooltip.Create(this);
	// Create Command Button
    for ( i = 0 ; i<MAX_JUBMOO_BUTTON ; i++ )
    {
        m_pButton[i] = new C_CoolButton;
        m_pButton[i]->Create(szButtonCaption[i],NULL,
                        BS_PUSHBUTTON|BS_CENTER|BS_VCENTER|WS_VISIBLE|WS_CHILD|BS_ICON,
                        rcButton[i],this,nButtonID[i]);
        m_pButton[i]->SetFont(m_hFont,TRUE);
		m_pButton[i]->SetImage(IMAGE_ICON,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nButtonResID[i])));
		m_pButton[i]->EnableKBMsgParentNotify(TRUE);
		m_pButton[i]->EnableFocusOnMouseMove(TRUE);

        m_Tooltip.AddTool(m_pButton[i]->GetHandle(),szButtonCaption[i]);
    }

	// Create Continue Button
	RECT rcContinue = {420,300,505,350};
	m_pContinueButton = new C_CoolButton;
    m_pContinueButton->Create("Continue",NULL,
                    BS_PUSHBUTTON|BS_CENTER|BS_VCENTER|WS_CHILD,
                    rcContinue,this,IDI_CONTINUE);
    m_pContinueButton->SetFont(m_hBigFont,TRUE);
	m_pContinueButton->EnableKBMsgParentNotify(TRUE);
	m_pContinueButton->EnableFocusOnMouseMove(TRUE);
	m_Tooltip.AddTool(m_pContinueButton->GetHandle(),"Click to continue, when 4 players 're in the game");

    // Create Chat Entry Box
    RECT rcChatEdit = {45,472,202,492};
    m_pChatEdit = new C_ChatEdit;
    m_pChatEdit->Create(rcChatEdit,this,ID_CHATEDIT);
    m_pChatEdit->SetFont(m_hFont,TRUE);
    m_Tooltip.AddTool(m_pChatEdit->GetHandle(),"Enter message here");

    // Create PlayerDetailWnd
    POINT ptPlayerDetail[NO_OF_JMPLAYER] = {{10,360},{10,10},{530,10},{530,360}};
    for ( i = 0 ; i<NO_OF_JMPLAYER ; i++ )
    {
		m_pPlayerDetailWnd[i] = new C_PlayerDetailWnd(this,SHOW_JMPLAYER_NORMAL,
        							ptPlayerDetail[i].x,
                                    ptPlayerDetail[i].y);
	}

    // set all card image
	char tmpbuf[128];
	JubmooApp.GetCustomCard(tmpbuf,128);
	C_CardWnd::SetCardImage(tmpbuf);

	// set bkg image of every component
	SetImage();

	// set delay time
	JubmooApp.OpenRegistry();
	SetGameSpeed(JubmooApp.GetAIDelay(),JubmooApp.GetPickDelay());
	JubmooApp.CloseRegistry();
}

int C_JubmooWnd::SetGameSpeed(int nAIDelay,int nPickDelay)
{
	m_nAIDelay = nAIDelay;
	m_nPickDelay = nPickDelay;
	return 0;
}

void C_JubmooWnd::SetImage()
{
	// ==============Set Background====================
    ClearImage();

    int bFading;
    int bCoolCtl;
    int bCustomImg;
    char szCustomBkg[48];
    memset(szCustomBkg,0x00,sizeof(szCustomBkg));
    char szCustomCard[48];
    memset(szCustomCard,0x00,sizeof(szCustomCard));
    char szCustomDlg[48];
    memset(szCustomDlg,0x00,sizeof(szCustomDlg));
    char szCustomNum[48];
    memset(szCustomNum,0x00,sizeof(szCustomNum));
    char szCustomPD[48];
    memset(szCustomPD,0x00,sizeof(szCustomPD));

	RECT rc;

    // read background source from registry
	JubmooApp.OpenRegistry();
    bFading = JubmooApp.GetUseFading();
    bCoolCtl = JubmooApp.GetUseCoolCtl();
    bCustomImg = JubmooApp.GetUseCustomImg();

   	C_CoolDlg::CoolDlgClearImage();
   	C_CoolDlg::CoolDlgEnableFading(bFading);

	if ( bCoolCtl )
    	CoolDlgAutoSubclass();
	else
    	CoolDlgTerminate();

    if ( bCustomImg )
    {
	    JubmooApp.GetCustomBkg(szCustomBkg,sizeof(szCustomBkg));
    	JubmooApp.GetCustomCard(szCustomCard,sizeof(szCustomCard));
	    JubmooApp.GetCustomDlg(szCustomDlg,sizeof(szCustomDlg));
    	JubmooApp.GetCustomNum(szCustomNum,sizeof(szCustomNum));
	    JubmooApp.GetCustomPD(szCustomPD,sizeof(szCustomPD));
    }

	JubmooApp.CloseRegistry();

    // set other component background
    C_CardWnd::SetBackCardImage();
    C_CardWnd::SetCardImage(szCustomCard);
    C_CoolDlg::CoolDlgSetImage(NULL,szCustomDlg);
    if ( m_pPlayerDetailWnd[0]->GetBmpDigit()->SetDigitImage(
				NULL,szCustomNum,USE_DELIMETER) == NULL )
	{
		m_pPlayerDetailWnd[0]->GetBmpDigit()->SetDigitImage(C_App::m_hInstance,MAKEINTRESOURCE(IDB_NUMBER),USE_DELIMETER);
		m_pPlayerDetailWnd[1]->GetBmpDigit()->SetDigitImage(C_App::m_hInstance,MAKEINTRESOURCE(IDB_NUMBER),USE_DELIMETER);
		m_pPlayerDetailWnd[2]->GetBmpDigit()->SetDigitImage(C_App::m_hInstance,MAKEINTRESOURCE(IDB_NUMBER),USE_DELIMETER);
		m_pPlayerDetailWnd[3]->GetBmpDigit()->SetDigitImage(C_App::m_hInstance,MAKEINTRESOURCE(IDB_NUMBER),USE_DELIMETER);
	}
	else
	{
	    m_pPlayerDetailWnd[1]->GetBmpDigit()->SetDigitImage(NULL,szCustomNum,USE_DELIMETER);
		m_pPlayerDetailWnd[2]->GetBmpDigit()->SetDigitImage(NULL,szCustomNum,USE_DELIMETER);
		m_pPlayerDetailWnd[3]->GetBmpDigit()->SetDigitImage(NULL,szCustomNum,USE_DELIMETER);
	}

    m_pPlayerDetailWnd[0]->SetImage(szCustomPD);
    m_pPlayerDetailWnd[1]->SetImage(szCustomPD);
    m_pPlayerDetailWnd[2]->SetImage(szCustomPD);
    m_pPlayerDetailWnd[3]->SetImage(szCustomPD);

    // set jubmoownd background
	GetClientRect(m_hWnd,&rc);

	m_hDC = GetDC(m_hWnd);
    m_hdcBkMem = CreateCompatibleDC(m_hDC);
	m_hbmpBkMem = CreateCompatibleBitmap(m_hDC,rc.right-rc.left,rc.bottom-rc.top);
    m_hbmpBkTmp = (HBITMAP)SelectObject(m_hdcBkMem,m_hbmpBkMem);

    int bLoadCustom = FALSE;
	if ( bCustomImg && szCustomBkg != NULL )
	{
		HDC hdcCustomBmp;
		HBITMAP hbmpTmpCustomBmp;
		HBITMAP hbmpCustomBmp = (HBITMAP)LoadImage(NULL,szCustomBkg,
							IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if ( hbmpCustomBmp != NULL )
		{
			int i;
		    hdcCustomBmp = CreateCompatibleDC(m_hDC);
			hbmpTmpCustomBmp = (HBITMAP)SelectObject(hdcCustomBmp,hbmpCustomBmp);

			BITMAP bm;
			GetObject(hbmpCustomBmp,sizeof(bm),&bm);
			for ( i = 0 ; i < JUBMOOWND_WIDTH ; i+=bm.bmWidth )
				for ( int j = 0 ; j < JUBMOOWND_HEIGHT ; j+=bm.bmHeight )
					BitBlt(m_hdcBkMem,i,j,bm.bmWidth,bm.bmHeight,
							hdcCustomBmp,0,0,SRCCOPY);

			DeleteObject(SelectObject(hdcCustomBmp,hbmpTmpCustomBmp));
			DeleteDC(hdcCustomBmp);
            bLoadCustom = TRUE;
        }
	}

    if ( !bLoadCustom )
    {
        if ( bFading )
        {
            // using default painting image , if not enter filename or load file fail
            C_DCEx * pDCEx = new C_DCEx(m_hdcBkMem);
            pDCEx->FillGradientRect(&rc,GetSysColor(COLOR_3DFACE),
                                        GetSysColor(COLOR_3DHILIGHT));
            delete pDCEx;
        }
        else
        {
            // paint solid background
            HBRUSH hb;
            hb = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
            HBRUSH hbtmp = (HBRUSH)SelectObject(m_hdcBkMem,hb);
            FillRect(m_hdcBkMem,&rc,hb);
            DeleteObject(SelectObject(m_hdcBkMem,hbtmp));
        }
    }
	// put small logo
	HDC hdcLogo = CreateCompatibleDC(m_hDC);
	HBITMAP hbmpLogo = LoadBitmap(C_App::m_hInstance,MAKEINTRESOURCE(IDB_JMLOGO));
	HBITMAP hbmpLogoTmp = (HBITMAP)SelectObject(hdcLogo,hbmpLogo);
    HFONT hfontTmp = (HFONT)SelectObject(hdcLogo,GetJmFont());
    // draw version number
    SetBkMode(hdcLogo,TRANSPARENT);
    TextOut(hdcLogo,98,19,JUBMOO_VERSION,strlen(JUBMOO_VERSION));
    SelectObject(hdcLogo,hfontTmp);

	BITMAP bm;
	GetObject(hbmpLogo,sizeof(BITMAP),&bm);
	HDC hdcLogoMsk = CreateCompatibleDC(m_hDC);
	C_DCEx * pDCEx = new C_DCEx(m_hdcBkMem);
	HBITMAP hbmpLogoMsk = pDCEx->CreateMaskBitmap(hdcLogo,bm.bmWidth,bm.bmHeight,TRANSPARENT_COLOR);
	HBITMAP hbmpLogoMskTmp = (HBITMAP)SelectObject(hdcLogoMsk,hbmpLogoMsk);
	pDCEx->TransparentBlt(596,464,bm.bmWidth,bm.bmHeight,hdcLogo,hdcLogoMsk,0,0);
	DeleteObject(SelectObject(hdcLogoMsk,hbmpLogoMskTmp));
	DeleteObject(SelectObject(hdcLogo,hbmpLogoTmp));
	DeleteDC(hdcLogoMsk);
	DeleteDC(hdcLogo);

	RECT rcShade;
	// put big logo
	HDC hdcBLogo = CreateCompatibleDC(m_hDC);
	HBITMAP hbmpBLogo = LoadBitmap(C_App::m_hInstance,MAKEINTRESOURCE(IDB_BIGLOGO));
	HBITMAP hbmpBLogoTmp = (HBITMAP)SelectObject(hdcBLogo,hbmpBLogo);
	GetObject(hbmpBLogo,sizeof(BITMAP),&bm);
	if ( m_pJubmoo->GetPlayMode() != IDLE_PLAYMODE )
	{
		SetRect(&rcShade,0,0,bm.bmWidth,bm.bmHeight);
		pDCEx->ShadeRect(hdcBLogo,&rcShade,TRANSPARENT_COLOR);
	}

	HDC hdcBLogoMsk = CreateCompatibleDC(m_hDC);
	HBITMAP hbmpBLogoMsk = pDCEx->CreateMaskBitmap(hdcBLogo,bm.bmWidth,bm.bmHeight,TRANSPARENT_COLOR);
	HBITMAP hbmpBLogoMskTmp = (HBITMAP)SelectObject(hdcBLogoMsk,hbmpBLogoMsk);

	pDCEx->TransparentBlt(210,130,bm.bmWidth,bm.bmHeight,hdcBLogo,hdcBLogoMsk,0,0);
	DeleteObject(SelectObject(hdcBLogoMsk,hbmpBLogoMskTmp));
	DeleteObject(SelectObject(hdcBLogo,hbmpBLogoTmp));
	DeleteDC(hdcBLogoMsk);
	DeleteDC(hdcBLogo);

	HRGN hrgn;
	for ( int i=0 ; i<4 ; i++ )
	{
		int x = m_pPlayerDetailWnd[i]->GetPosX();
		int y = m_pPlayerDetailWnd[i]->GetPosY();
		hrgn = CreateRoundRectRgn(x,y,x+PLAYERDETAIL_WIDTH-1,y+PLAYERDETAIL_HEIGHT-1,24,24);
		ExtSelectClipRgn(m_hdcBkMem,hrgn,RGN_AND );
		SetRect(&rcShade,x,y,x+PLAYERDETAIL_WIDTH-1,y+PLAYERDETAIL_HEIGHT-1);
		pDCEx->ShadeRect(m_hdcBkMem,&rcShade,RGB(128,128,128));
		SelectClipRgn(m_hdcBkMem,NULL);
		DeleteObject(hrgn);
	}
	delete pDCEx;
	ReleaseDC(m_hWnd,m_hDC);
}

int C_JubmooWnd::ClearImage()
{
	if ( m_hdcBkMem != NULL )
	{
		DeleteObject(SelectObject(m_hdcBkMem,m_hbmpBkTmp));
		DeleteDC(m_hdcBkMem);
		m_hdcBkMem = NULL;
	}
	return TRUE;
}

void C_JubmooWnd::CreateJmFont()
{
	LOGFONT lf;
	m_hDC = GetDC(m_hWnd);

    // set font size 8 ---v
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

	lf.lfHeight = -MulDiv(14,GetDeviceCaps(m_hDC,LOGPIXELSY),72);
    m_hBigFont = CreateFontIndirect(&lf);

	ReleaseDC(m_hWnd,m_hDC);
}

void C_JubmooWnd::MoveToCenter()
{
	RECT swp;
	int width;
	int height;

	width = GetSystemMetrics(SM_CXFULLSCREEN);
	height = GetSystemMetrics(SM_CYFULLSCREEN)+
				GetSystemMetrics(SM_CYCAPTION);

	GetWindowRect(m_hWnd,&swp);

	if ( (height-(swp.bottom-swp.top))/2 < 0 )
    	MoveWindow(m_hWnd,(width-(swp.right-swp.left))/2,0,
					swp.right-swp.left,
					swp.bottom-swp.top,FALSE);
	else if ( (width-(swp.right-swp.left))/2 < 0 )
		MoveWindow(m_hWnd,0,(height-(swp.bottom-swp.top))/2,
					swp.right-swp.left,
					swp.bottom-swp.top,FALSE);
	else
		MoveWindow(m_hWnd,(width-(swp.right-swp.left))/2,
					(height-(swp.bottom-swp.top))/2,
					swp.right-swp.left,
					swp.bottom-swp.top,FALSE);
}

// ==========================================================
// Interface
// ==========================================================
int C_JubmooWnd::ShowSettingGame(LPSTART_GAME_SETTING pGameSetting)
{
	if ( m_pStartGameWnd == NULL )
    {
		ShowIdleGame();
		// new screen with this setting
		m_pStartGameWnd = new C_StartGameWnd(this,pGameSetting);
    }
	else
	{
		if ( pGameSetting != NULL )
		{
			// set current screen this setting
			m_pStartGameWnd->SetGame(pGameSetting);
		}
	}
	return 0;
}

int C_JubmooWnd::ShowStartGame()
{
	int i;
	// set player to table by current setting , and check for my player
	// must be on lower-left side
	LockWindowUpdate(m_hWnd);

	for ( i=0; i<4 ; i++ )
	{
    	if ( m_pJubmoo->GetGameSetting()->nPlayerID[i] == NO_PLAYER ) continue;

		C_Player *pPlayer = m_pJubmoo->GetPlayer(m_pJubmoo->GetGameSetting()->nPlayerID[i]);
        if ( pPlayer == NULL )
        {
        	m_pJubmoo->GetComm()->LogMessage("#Err08\n");
			continue;
        }
        int nPos = pPlayer->GetTableOrder();

		switch ( pPlayer->GetStatus() )
		{
		case IAM_SPECTATOR:
			pPlayer->SetStatus(IAM_PLAYER);
		    SetPlayerUI(nPos,pPlayer);
			break;
		case IAM_PLAYER: case IAM_COMPUTER:
		    SetPlayerUI(nPos,pPlayer);
			break;
		}
	}
	delete m_pStartGameWnd;
	m_pStartGameWnd = NULL;
	LockWindowUpdate(NULL);
	return 0;
}

int C_JubmooWnd::ShowReplacedPlayer(int nOrder)
{
	C_Player *pPlayer = m_pJubmoo->GetPlayer(m_pJubmoo->GetIngameTable()->nPlayerID[nOrder]);
    if ( pPlayer == NULL )
    {
        m_pJubmoo->GetComm()->LogMessage("#Err08\n");
		return FALSE;
    }
    int nPos = pPlayer->GetTableOrder();
	pPlayer->SetStatus(IAM_PLAYER);
	SetPlayerUI(nPos,pPlayer);

	if ( pPlayer->GetPlayerID() == m_pJubmoo->GetMyPlayer()->GetPlayerID() )
		SetCardOnHand();
	else
		RefreshPlayersCard(nPos);

	RequestDiscard();

	return TRUE;
}

int C_JubmooWnd::ShowEndGame()
{
	delete m_pEndGameWnd;
	m_pEndGameWnd = new C_EndGameWnd(this);
	if ( JubmooApp.IsUseSound() )
		PlaySound(MAKEINTRESOURCE(IDR_SND_CLAPPING),
				C_App::m_hInstance,
				SND_RESOURCE|SND_ASYNC);
	return 0;
}

int C_JubmooWnd::ShowEndRound()
{
	if ( m_pJubmoo->GetMyPlayer()->GetStatus() == IAM_PLAYER )
	{
		C_RoundDlg RoundDlg(this);
		RoundDlg.DoModal();
		// unlock window update ,  in RoundDlg Closing
		LockWindowUpdate(NULL);
	}
	return 0;
}

int C_JubmooWnd::ShowIdleGame()
{
 	KillTimer(m_hWnd,AI_TIMER);
	KillTimer(m_hWnd,PICKUPCARD_TIMER);
	for ( int j=0 ; j<4 ; j++ )
	{
		delete m_pPickCardWnd[j];
		m_pPickCardWnd[j] = NULL;
	}
	m_bPickingUp = FALSE;

	ClearDropCard();
	ClearCardOnHand();
	delete m_pStartGameWnd; m_pStartGameWnd = NULL;
	delete m_pEndGameWnd; m_pEndGameWnd = NULL;

	// only quit from ingame playmode need to repaint
	if ( GetJubmoo()->GetPlayMode() == INGAME_PLAYMODE )
		for ( int i=0 ; i<4 ; i++ )
			RefreshPlayersCard(i);

	if ( GetJubmoo()->GetGameMode() == IDLE_GAMEMODE )
	{
		EnableWindow(m_pButton[START_BUTTON]->GetHandle(),TRUE);
		EnableWindow(m_pButton[JOIN_BUTTON]->GetHandle(),TRUE);
	}

	return 0;
}

int C_JubmooWnd::SetCardOnHand()
{
	LockWindowUpdate(m_hWnd);

	m_MyCardList.DeleteObjects();

	int i,nIndex = 0;
	for ( int nFace=0 ; nFace<4 ; nFace++ )
	{
		CARD_LIST * pcard = m_pJubmoo->GetMyPlayer()->GetCardList(nFace);
		for ( i=0 ; i<pcard->Count() ; i++ )
		{
    		C_PickCardWnd * pCardWnd = new C_PickCardWnd(this,*pcard->Items(i),212+nIndex*15,362,FACE_UP);
			nIndex++;
			m_MyCardList.Add(pCardWnd);
			BringWindowToTop(pCardWnd->GetHandle());
			pCardWnd->EnableKBMsgParentNotify(TRUE);
			pCardWnd->EnableClickNotify(FALSE);
		}
	}
	LockWindowUpdate(NULL);

	return 0;
}

int C_JubmooWnd::ClearCardOnHand()
{
	m_MyCardList.DeleteObjects();
	return 0;
}

int C_JubmooWnd::ClearDropCard()
{
	m_DropCardList.DeleteObjects();
	return 0;
}

int C_JubmooWnd::Discard(int nPos,int nCardNum,int nDiscardMode)
{
	if ( nDiscardMode != SUDDEN_DISCARDMODE )
	{
		m_pPlayerDetailWnd[m_pJubmoo->GetActivePlayerOrder()]->ShowInactive();
	}

	static RECT rcPlayersCard[4] = { {204,332,500,458},		// order # 1
		                        	{10,120,200,336},		// order # 2
        		                	{220,2,516,128},		// order # 3
									{520,132,710,348} };	// order # 4
	static POINT pntDropPos[4] = {
									{325,232},
									{250,182},
									{325,132},
									{400,182} };

	RECT rc = rcPlayersCard[nPos];
	int nCardPosX,nCardPosY;

    int i;
	int nCardCount = 0;
	for ( i=0 ; i<m_pJubmoo->GetPlayerList()->Count() ; i++ )
	{
		if ( nPos == m_pJubmoo->GetPlayerList()->Items(i)->GetTableOrder() )
		{
			for ( int nFace=0 ; nFace<4 ; nFace++ )
				nCardCount += m_pJubmoo->GetPlayerList()->Items(i)->GetCardList(nFace)->Count();
			break;
		}
	}

	switch( nPos )
    {
    case 0:
        nCardPosX = rc.left + 8 + 15*(nCardCount);
        nCardPosY = rc.top + 30;
    	break;
    case 1:
        nCardPosX = rc.left;
        nCardPosY = rc.top + 10*(nCardCount);
    	break;
    case 2:
    	nCardPosX = rc.right - CARD_WIDTH - 7 - 15*(nCardCount);
        nCardPosY = rc.top + 30;
    	break;
    case 3:
        nCardPosX = rc.right - CARD_WIDTH;
        nCardPosY = rc.bottom - CARD_HEIGHT - 10*(nCardCount);
    	break;
    }

    C_PickCardWnd * pCardWnd;
	// if my player, just remove from mycardlist
    if ( m_pJubmoo->GetMyPlayer()->GetTableOrder() == nPos )
    {
    	int nRemovedCard = -1;
    	for ( i=0 ; i<m_MyCardList.Count() ; i++ )
        {
        	m_MyCardList.Items(i)->EnableAutoMoveUp(FALSE);
        	m_MyCardList.Items(i)->EnableClickNotify(FALSE);
			if ( m_MyCardList.Items(i)->GetCardNumber() == nCardNum )
            	nRemovedCard = i;
        }
        if ( nRemovedCard > -1 ) // card found
        {
			pCardWnd = m_MyCardList.Items(nRemovedCard);
			m_MyCardList.Remove(nRemovedCard);
        }
    }
    else
    {
		pCardWnd = new C_PickCardWnd(this,nCardNum,nCardPosX,nCardPosY,FACE_UP);
        pCardWnd->EnableKBMsgParentNotify(TRUE);
	    pCardWnd->EnableClickNotify(FALSE);
    }

	m_DropCardList.Add(pCardWnd);
	BringWindowToTop(pCardWnd->GetHandle());
	if ( nDiscardMode == SUDDEN_DISCARDMODE )
		// suddenly move card , use to show drop card when client come in
		pCardWnd->MoveCardTo(pntDropPos[nPos].x,pntDropPos[nPos].y,0,999);
	else
	{
		if ( JubmooApp.IsUseSound() )
		{
			if ( nCardNum == SPADE_Q )
				PlaySound(MAKEINTRESOURCE(IDR_SND_CRASH),
						C_App::m_hInstance,
						SND_RESOURCE|SND_ASYNC);
			else if ( nCardNum == DIAMOND_J )
				PlaySound(MAKEINTRESOURCE(IDR_SND_POP),
						C_App::m_hInstance,
						SND_RESOURCE|SND_ASYNC);
			else
				PlaySound(MAKEINTRESOURCE(IDR_SND_DISCARD2),
						C_App::m_hInstance,
						SND_RESOURCE|SND_ASYNC);
		}
		pCardWnd->MoveCardTo(pntDropPos[nPos].x,pntDropPos[nPos].y,0,NORMAL_MOVECARD);
	}

#ifdef _DEBUG
	RefreshPlayersCard(nPos);
	if ( m_pJubmoo->GetMyPlayer()->GetStatus() == IAM_PLAYER )
	{
		for ( int k=0 ; k<GetMyCardList()->Count() ; k++ )
			InvalidateRect(GetMyCardList()->Items(k)->GetHandle(),NULL,FALSE);
	}
	InvalidateRect(pCardWnd->GetHandle(),NULL,FALSE);
#endif
	return 0;
}

// nOffset = 1-3 to rotate left
// nOffset = -1 - -3 to rotate right
int C_JubmooWnd::ShiftDropCard(int nOffset)
{
	static POINT pntDropPos[4] = {{325,232},{250,182},{325,132},{400,182} };

        int nOrder = 0;

        for ( int i=0 ; i<m_DropCardList.Count() ; i++ )
        {
                RECT rc;
                GetWindowRect(m_DropCardList.Items(i)->GetHandle(),&rc);
				ScreenToClient(m_hWnd,(LPPOINT)&rc.left);
                switch ( rc.left )
                {
                case 250:nOrder = 1;break;
                case 325:
                        if ( rc.top == 132 ) nOrder = 2; else nOrder = 0;
                        break;
                case 400:nOrder = 3;break;
                }
                nOrder += nOffset;
                if ( nOrder<0 )
                        nOrder+=4;
                else if ( nOrder>3 )
                        nOrder-=4;
                MoveWindow(m_DropCardList.Items(i)->GetHandle(),
                                pntDropPos[nOrder].x,pntDropPos[nOrder].y,
                                CARD_WIDTH,CARD_HEIGHT,TRUE);
        }
	return 0;
}

int C_JubmooWnd::PickUpCard(int nPos)
{
#ifdef _DEBUG
	if ( m_bPickingUp )
	{
		TRACE("error overlap calling of pickup card\n");
		// clear all last picking cards
		for ( int i=0 ; i<4 ; i++ )
		{
			if ( m_pPickCardWnd[i] != NULL )
			{
				delete m_pPickCardWnd[i];
				m_pPickCardWnd[i] = NULL;
			}
		}
	}
#endif

	m_bPickingUp = 1;
	for ( int j=0 ; j<4 ; j++ )
	{
		m_pPickCardWnd[j] = m_DropCardList.Items(0);
		m_DropCardList.Remove(0);
	}

	// save nPos to move card , to cancel, if overlap calling of pickupcard
	m_nPickToPos = nPos;
	SetTimer(m_hWnd,PICKUPCARD_TIMER,m_nPickDelay,NULL);
	return 0;
}

LRESULT C_JubmooWnd::OnPickUpCardTimer()
{
	static POINT pntDesPos[4] = {
									{-CARD_HEIGHT,360},
									{-CARD_HEIGHT,10},
									{JUBMOOWND_WIDTH+1,10},
									{JUBMOOWND_WIDTH+1,360} };
	static int i = 0;

	if ( m_bPickingUp == 1 )
	{
//		TRACE("first pick\n");
		i=0;
		m_bPickingUp = 2;
	}
	if ( JubmooApp.IsUseSound() )
		PlaySound(MAKEINTRESOURCE(IDR_SND_DISCARD1),
				C_App::m_hInstance,
				SND_RESOURCE|SND_ASYNC);
	m_pPickCardWnd[i]->MoveCardTo(pntDesPos[m_nPickToPos].x,pntDesPos[m_nPickToPos].y,
							PICKUPCARD_READY,FAST_MOVECARD);
	i++;
	if ( i==4 )
	{
		i=0;
		m_bPickingUp = FALSE;
		KillTimer(m_hWnd,PICKUPCARD_TIMER);

		// after pickupcard call EndTurn to continue jubmoo engine
		if ( m_pJubmoo->GetGameMode() == SERVER_GAMEMODE )
			m_pJubmoo->EndTurn();
	}
	else if ( i==1 )
	{
		KillTimer(m_hWnd,PICKUPCARD_TIMER);
		SetTimer(m_hWnd,PICKUPCARD_TIMER,100,NULL);
	}
	return 0;
}

int C_JubmooWnd::RequestDiscard()
{
	// if active player is my player, set card to autopopup
	if ( m_pJubmoo->GetActivePlayerID() == m_pJubmoo->GetMyPlayer()->GetPlayerID() )
	{
		int nCardCount = 0;
		for ( int i=0 ; i<GetMyCardList()->Count() ; i++ )
        {
			C_PickCardWnd * pCard = GetMyCardList()->Items(i);
			if ( m_pJubmoo->GetLeadedCard() != NO_LEADED )
				if ( GET_FACE(pCard->GetCardNumber()) != GET_FACE(m_pJubmoo->GetLeadedCard()) )
					continue;

        	pCard->EnableClickNotify(TRUE);
			pCard->EnableAutoMoveUp(TRUE);
			nCardCount++;
        }
		// if no specify leaded face ( void ) , enable all cards
		if ( nCardCount == 0 )
		{
			for ( int i=0 ; i<GetMyCardList()->Count() ; i++ )
		    {
				C_PickCardWnd * pCard = GetMyCardList()->Items(i);
		    	pCard->EnableClickNotify(TRUE);
				pCard->EnableAutoMoveUp(TRUE);
			}
		}
		m_pPlayerDetailWnd[m_pJubmoo->GetActivePlayerOrder()]->ShowActive();
		if ( JubmooApp.IsAlarmTurn() )
			if ( GetMyCardList()->Count() < 13 )
				PlaySound(MAKEINTRESOURCE(IDR_SND_MYTURN),
						C_App::m_hInstance,
						SND_RESOURCE|SND_ASYNC);
		return TRUE;
	}

	// if computer, set timer to get input from active player
	C_Player * pPlayer = m_pJubmoo->GetPlayer(m_pJubmoo->GetActivePlayerID());
	if ( pPlayer == NULL )
	{
       	m_pJubmoo->GetComm()->LogMessage("#Err09\n");
		return FALSE;
    }

    if ( pPlayer->GetStatus() == IAM_COMPUTER && m_pJubmoo->GetGameMode() == SERVER_GAMEMODE)
	{
		// jubmoo engine call this function to generate AI's discard
		SetTimer(m_hWnd,AI_TIMER,m_nAIDelay,NULL);
	}
	m_pPlayerDetailWnd[m_pJubmoo->GetActivePlayerOrder()]->ShowActive();
	return TRUE;
}

int C_JubmooWnd::WaitForLostPlayer(int nContinueType)
{
	m_nContinueType = nContinueType;
	ShowWindow(m_pContinueButton->GetHandle(),SW_SHOWNOACTIVATE);
	return TRUE;
}

LRESULT C_JubmooWnd::OnAIDiscardRequest()
{
	KillTimer(m_hWnd,AI_TIMER);

	C_Player * pPlayer = m_pJubmoo->GetPlayer(m_pJubmoo->GetActivePlayerID());

    // verify
	if ( pPlayer == NULL )
    {
       	m_pJubmoo->GetComm()->LogMessage("#Err10\n");
        return FALSE;
    }

	if ( pPlayer->GetStatus() != IAM_COMPUTER )
    {
       	m_pJubmoo->GetComm()->LogMessage("#Err11\n");
        return FALSE;
    }

	int nCardCount=0;
	for ( int nFace=0 ; nFace<4 ; nFace++ )
		nCardCount += pPlayer->GetCardList(nFace)->Count();
	if ( nCardCount > 0 )
		pPlayer->ActivateAI();

	return TRUE;
}

int C_JubmooWnd::AddSpectatorUI(C_Player * pPlayer)
{
	static POINT nPlayerPos[8] =
    { {10,465},{210,500},{320,500},{430,500},{540,500},{265,500},{375,500},{485,500} };
	//  0 xxxx  1  5  2  6  3  7  4

    if ( pPlayer == NULL ) return FALSE;

    // finding for empty slot
    C_PlayerWnd * pPlayerWnd;
    int nEmptyLot;

    if ( pPlayer == GetJubmoo()->GetMyPlayer() )
	{
    	// my player create at first position
    	nEmptyLot = 0;
        if ( m_pPlayerLotStatus[0] != NULL )
        	return FALSE;
    }
    else
    {
        int i;
        for ( i=1 ; i<MAX_NO_OF_PLAYER ; i++ )
		{
        	if ( m_pPlayerLotStatus[i] == NULL )
            	break;
		}
		// if no slot left
		if ( i == MAX_NO_OF_PLAYER )
        	return FALSE;
        else
        	nEmptyLot = i;
    }
    // create UI object at empty slot
   	pPlayerWnd = new C_PlayerWnd(this,SINGLE_SIZE_PLAYERWND,
									nPlayerPos[nEmptyLot].x,nPlayerPos[nEmptyLot].y,
									pPlayer );
	pPlayer->SetUI(pPlayerWnd);
    m_pPlayerLotStatus[nEmptyLot] = pPlayer;

    // if setting up create too
    if ( m_pStartGameWnd != NULL )
    	m_pStartGameWnd->AddPlayer(pPlayer);
    return TRUE;
}

int C_JubmooWnd::RemoveSpectatorUI(C_Player * pPlayer)
{
	// important :	when add -> create player , create UI
    //              when remove -> remove UI , remove player

    if ( pPlayer == NULL ) return FALSE;

    for ( int i = 0 ; i < GetJubmoo()->GetPlayerList()->Count() ; i++ )
    {
    	C_Player * pPy = GetJubmoo()->GetPlayerList()->Items(i);
    	if ( pPy == pPlayer )
        {
		    // if setting up remove from setting wnd too
		    if ( m_pStartGameWnd != NULL )
    			m_pStartGameWnd->RemovePlayer(pPy);
			// if playing remove from table too
			for ( int k=0 ; k<4 ; k++ )
			{
				if ( m_pPlayerDetailWnd[k]->GetPlayer() == pPy )
				{
					ClearPlayerUI(k);
					break;
				}
			}

            for ( int j = 0 ; j < MAX_NO_OF_PLAYER ; j++ )
			{
            	if ( m_pPlayerLotStatus[j] == pPy )
                {
                	m_pPlayerLotStatus[j] = NULL;
                    break;
                }
			}
			if ( pPy->GetStatus() != IAM_COMPUTER )
			{
				delete pPy->GetUI();
				pPy->SetUI(NULL);
			}
            return TRUE;
        }
    }
    return FALSE;
}

int C_JubmooWnd::SetPlayerUI(int nPos,C_Player * pPlayer)
{
	// delete old UI
    //delete pPlayer->GetUI();
    RemoveSpectatorUI(pPlayer);

    // create new UI
	m_pPlayerDetailWnd[nPos]->SetPlayer(pPlayer);
    return TRUE;
}

int C_JubmooWnd::ClearPlayerUI(int nPos)
{
	// clear player and set to be spectator again
    C_Player * pPlayer = m_pPlayerDetailWnd[nPos]->ClearPlayer();
	RefreshPlayersCard(nPos);

	if ( pPlayer == NULL ) return FALSE;

	if ( pPlayer->GetStatus() != IAM_COMPUTER )
		AddSpectatorUI( pPlayer );
    return TRUE;
}

void C_JubmooWnd::RefreshPlayersTable(int nPos)
{

	InvalidateRect(m_pPlayerDetailWnd[nPos]->GetHandle(),NULL,FALSE);

}

void C_JubmooWnd::RefreshPlayersCard(int nPos)
{
	static RECT rcPlayersCard[4] = { {204,332,500,458},		// order # 1
		                        	{10,120,200,336},		// order # 2
        		                	{220,2,516,128},		// order # 3
									{520,132,710,348} };	// order # 4
    RECT * prc = &rcPlayersCard[nPos];
    int nWidth = prc->right - prc->left;
    int nHeight = prc->bottom - prc->top;

	m_hDC = GetDC(m_hWnd);
	HDC hdc = CreateCompatibleDC(m_hDC);
    HBITMAP hbmp = CreateCompatibleBitmap(m_hDC,nWidth,nHeight);
	HBITMAP hbmpold = (HBITMAP)SelectObject(hdc,hbmp);

    BitBlt(hdc,0,0,nWidth,nHeight,m_hdcBkMem,prc->left,prc->top,SRCCOPY);
	
	C_Player * pPlayer;
	RECT rc;
	CARD_LIST pCardList;
    if ( (pPlayer = m_pPlayerDetailWnd[nPos]->GetPlayer()) != NULL )
	{
		rc.left = 0; rc.top = 0;
		rc.right = nWidth; rc.bottom = nHeight;
		for ( int nFace=0 ; nFace<4 ; nFace++ )
			for ( int i=0 ; i<pPlayer->GetCardList(nFace)->Count() ; i++ )
				pCardList.Add(pPlayer->GetCardList(nFace)->Items(i));
		PaintPlayersCard(hdc,nPos,rc,&pCardList,pPlayer->GetTakenCardList());
		pCardList.Clear();
	}
	BitBlt(m_hDC,prc->left,prc->top,nWidth,nHeight,hdc,0,0,SRCCOPY);

    DeleteObject(SelectObject(hdc,hbmpold));
    DeleteDC(hdc);
	ReleaseDC(m_hWnd,m_hDC);
}

int C_JubmooWnd::PaintPlayersCard(HDC hDC,int nPos,RECT& rc,
						CARD_LIST *pCardList,CARD_LIST * pTakenCardList)
{
	C_CardWnd *pCard = new C_CardWnd;
    int i;
    int nCardPosX,nCardPosY;

	switch( nPos )
    {
    case 0:
    	nCardPosX = rc.left;
        nCardPosY = rc.top;
		for ( i=0 ; i<pTakenCardList->Count() ; i++ )
        {
            pCard->PaintCard(hDC,nCardPosX,nCardPosY,
            				*(pTakenCardList->Items(i)));
			nCardPosX += 15;
        }
		// don't paint on my card list , ( if i'm player )
		if ( m_pJubmoo->GetMyPlayer()->GetStatus() == IAM_PLAYER ) break;
        nCardPosX = rc.left + 8;
        nCardPosY = rc.top + 30;
        for ( i=0 ; i<pCardList->Count() ; i++ )
        {
#ifdef _DEBUG
            pCard->PaintCard(hDC,nCardPosX,nCardPosY,
            				*(pCardList->Items(i)));
#else
            pCard->PaintCard(hDC,nCardPosX,nCardPosY,0);
#endif
			nCardPosX += 15;
        }
    	break;
    case 1:
        nCardPosX = rc.left;
        nCardPosY = rc.top;
        for ( i=0 ; i<pCardList->Count() ; i++ )
        {
#ifdef _DEBUG
            pCard->PaintCard(hDC,nCardPosX,nCardPosY,
            				*(pCardList->Items(i)));
#else
            pCard->PaintCard(hDC,nCardPosX,nCardPosY,0);
#endif
			nCardPosY += 10;
        }
    	nCardPosX = rc.left + CARD_WIDTH + 3;
        nCardPosY = rc.top;
		for ( i=0 ; i<pTakenCardList->Count() ; i++ )
        {
            pCard->PaintCard(hDC,nCardPosX,nCardPosY,
            				*(pTakenCardList->Items(i)));
			if ( i%4 == 3 )
            {
            	nCardPosY += 30;
                nCardPosX -= 60;
            }
			nCardPosX += 15;
        }
    	break;
    case 2:
    	nCardPosX = rc.right - CARD_WIDTH - 15*(pTakenCardList->Count()-1);
        nCardPosY = rc.top;
		for ( i=0 ; i<pTakenCardList->Count() ; i++ )
        {
            pCard->PaintCard(hDC,nCardPosX,nCardPosY,
            				*(pTakenCardList->Items(i)));
			nCardPosX += 15;
        }
    	nCardPosX = rc.right - CARD_WIDTH - 7 - 15*(pCardList->Count()-1);
        nCardPosY = rc.top + 30;
        for ( i=0 ; i<pCardList->Count() ; i++ )
        {
#ifdef _DEBUG
            pCard->PaintCard(hDC,nCardPosX,nCardPosY,
            				*(pCardList->Items(i)));
#else
            pCard->PaintCard(hDC,nCardPosX,nCardPosY,0);
#endif
			nCardPosX += 15;
        }
    	break;
    case 3:
        nCardPosX = rc.right - CARD_WIDTH;
        nCardPosY = rc.bottom - CARD_HEIGHT - 10*(pCardList->Count()-1);
        for ( i=0 ; i<pCardList->Count() ; i++ )
        {
#ifdef _DEBUG
            pCard->PaintCard(hDC,nCardPosX,nCardPosY,
            				*(pCardList->Items(i)));
#else
            pCard->PaintCard(hDC,nCardPosX,nCardPosY,0);
#endif
			nCardPosY += 10;
        }
    	nCardPosX = rc.left;
        nCardPosY = rc.bottom - CARD_HEIGHT - 30*((pTakenCardList->Count()-1)/4);
		for ( i=0 ; i<pTakenCardList->Count() ; i++ )
        {
            pCard->PaintCard(hDC,nCardPosX,nCardPosY,
            				*(pTakenCardList->Items(i)));
			if ( i%4 == 3 )
            {
            	nCardPosY += 30;
                nCardPosX -= 60;
            }
			nCardPosX += 15;
        }
    	break;
    }
    delete pCard;
	return 0;
}

void C_JubmooWnd::RefreshMyCardList()
{
	for ( int i=0; i<m_MyCardList.Count() ;i++)
		InvalidateRect(m_MyCardList.Items(i)->GetHandle(),NULL,FALSE);
}

int C_JubmooWnd::IsEndGame()
{
	if ( m_pEndGameWnd != NULL )
		return TRUE;
	else
		return FALSE;
}

void C_JubmooWnd::RefreshEndGame()
{
	m_pEndGameWnd->Refresh();
}


// =====================================================================
// Event Handler
// =====================================================================
LRESULT C_JubmooWnd::OnPaint(WPARAM ,LPARAM )
{
	static RECT rcPlayersCard[4] = { {204,332,500,458},		// order # 1
		                        	{10,120,200,336},		// order # 2
        		                	{220,2,516,128},		// order # 3
									{520,132,710,348} };	// order # 4

    PAINTSTRUCT ps;
    m_hDC = BeginPaint( m_hWnd, &ps );

	RECT rc;
	GetClientRect(m_hWnd,&rc);
	BitBlt(m_hDC,0,0,rc.right-rc.left,rc.bottom-rc.top,m_hdcBkMem,0,0,SRCCOPY);

	if ( ps.rcPaint.bottom == 532 && ps.rcPaint.top == 500 ) 
	{
		// if paint background for button command no need painting card.
		// to increase performance
		EndPaint( m_hWnd, &ps );
		return 0;
	}

    int i;
    C_Player * pPlayer;

	CARD_LIST pCardList;
	for ( i = 0 ; i<4 ; i++ )
	{
    	if ( ( pPlayer = m_pPlayerDetailWnd[i]->GetPlayer() ) == NULL ) continue;
		for ( int nFace=0 ; nFace<4 ; nFace++ )
			for ( int j=0 ; j<pPlayer->GetCardList(nFace)->Count() ; j++ )
				pCardList.Add(pPlayer->GetCardList(nFace)->Items(j));
        PaintPlayersCard(m_hDC,pPlayer->GetTableOrder(),
        				rcPlayersCard[i],
                        &pCardList,
                        pPlayer->GetTakenCardList());
		pCardList.Clear();
	}

	EndPaint( m_hWnd, &ps );
	return 0;
}

LRESULT C_JubmooWnd::OnNCLButtonDblClk(WPARAM ,LPARAM )
{
	if ( !IsIconic(m_hWnd) )
		ShowWindow(m_hWnd,SW_MINIMIZE);
	return 0;
}

LRESULT C_JubmooWnd::OnCommand(WPARAM wParam,LPARAM )
{
	switch ( wParam )
	{
    case ID_JM_START: OnStartButtonClick(); break;
    case ID_JM_JOIN: OnJoinButtonClick(); break;
    case ID_JM_QUIT: OnQuitButtonClick(); break;
    case ID_JM_SETTING:
    	{
        C_SettingDlg SettingDlg(this);
		if ( SettingDlg.DoModal() == IDCANCEL )
        	return 0;
        }
   	    InvalidateRect(m_hWnd,NULL,FALSE);
        break;
    case ID_JM_INFO:
		{
#ifdef _DEBUG
		TRACE("====================================================\n");
		int i;
		for ( i=0 ; i<GetJubmoo()->GetPlayerList()->Count() ; i++ )
		{
			C_Player * p = GetJubmoo()->GetPlayerList()->Items(i);
			TRACE("ID %d [%s] status %d, order %d , card[%d,%d,%d,%d] , TakenCard[%d] ,history->",
            		p->GetPlayerID(),p->GetName(),p->GetStatus(),
					p->GetTableOrder(),p->GetCardList(0)->Count(),
					p->GetCardList(1)->Count(),
					p->GetCardList(2)->Count(),
					p->GetCardList(3)->Count(),
					p->GetTakenCardList()->Count());
			for ( int j=0 ; j<p->GetHistoryScore()->Count() ; j++ )
				TRACE("%d,",*(p->GetHistoryScore()->Items(j)) );
			TRACE("\n");
		}
		TRACE("client list\n");
		for ( i=0 ; i<GetJubmoo()->GetComm()->GetClientList()->Count() ; i++ )
		{
			TRACE("JMCOMM#%d,",i);
			C_JMComm * p = (C_JMComm*)(GetJubmoo()->GetComm()->GetClientList()->Items(i));
			if ( p->GetPlayer() != NULL )
				TRACE("pid=%d,name=%s\n",p->GetPlayer()->GetPlayerID(),p->GetPlayer()->GetName());
			else
				TRACE("\n");
		}
		TRACE(" setting %d,%d,%d,%d score %d\n",
				GetJubmoo()->GetGameSetting()->nPlayerID[0],
				GetJubmoo()->GetGameSetting()->nPlayerID[1],
				GetJubmoo()->GetGameSetting()->nPlayerID[2],
				GetJubmoo()->GetGameSetting()->nPlayerID[3],
				GetJubmoo()->GetGameSetting()->nGameScore );
		TRACE(" table %d[%d],%d[%d],%d[%d],%d[%d] leaded card %d(face%d)\n",
				GetJubmoo()->GetIngameTable()->nPlayerID[0],
				GetJubmoo()->GetIngameTable()->nCardNum[0],
				GetJubmoo()->GetIngameTable()->nPlayerID[1],
				GetJubmoo()->GetIngameTable()->nCardNum[1],
				GetJubmoo()->GetIngameTable()->nPlayerID[2],
				GetJubmoo()->GetIngameTable()->nCardNum[2],
				GetJubmoo()->GetIngameTable()->nPlayerID[3],
				GetJubmoo()->GetIngameTable()->nCardNum[3],
				GetJubmoo()->GetLeadedCard(),
				GET_FACE(GetJubmoo()->GetLeadedCard())
				);
//		TRACE(" card on my hand ");
//		for ( int j=0 ; j<m_pJubmoo->GetMyPlayer()->GetCardList()->Count() ; j++ )
//			TRACE("%d,",*(m_pJubmoo->GetMyPlayer()->GetCardList()->Items(j)) );
//		TRACE("\n");
		TRACE(" activeplayerorder %d id=%d\n",
				GetJubmoo()->GetActivePlayerOrder(),
				GetJubmoo()->GetActivePlayerID());
		TRACE(" game %d , round %d , turn %d ====================\n",
				GetJubmoo()->GetGameNumber(),
				GetJubmoo()->GetRoundNumber(),
				GetJubmoo()->GetTurnNumber());
//		TRACE("IsPicking %d,  card = %d,%d,%d,%d DropCardCount=%d\n",m_bPickingUp,
//					m_pPickCardWnd[0],m_pPickCardWnd[1],
//					m_pPickCardWnd[2],m_pPickCardWnd[3],
//					m_DropCardList.Count());
#endif
        m_pDialog = new C_InfoDlg(this);
        m_pDialog->DoModal();
		delete m_pDialog;
		m_pDialog = NULL;
        }
        break;
	case ID_JM_HELP:
		{
        C_HelpDlg HelpDlg(this);
        HelpDlg.DoModal();
        }
		break;
	case ID_JM_ABOUT:
		{
		C_AboutDlg AboutDlg(this);
		AboutDlg.DoModal();
#ifdef _DEBUG
		// test ///////////////////////////////////
		HDC hdc = GetDC(m_hWnd);
		char tmpbuf[99];
		sprintf(tmpbuf,"Ingame %d[%d] %d[%d] %d[%d] %d[%d] - active=%d",
				m_pJubmoo->GetIngameTable()->nPlayerID[0],
				m_pJubmoo->GetIngameTable()->nCardNum[0],
				m_pJubmoo->GetIngameTable()->nPlayerID[1],
				m_pJubmoo->GetIngameTable()->nCardNum[1],
				m_pJubmoo->GetIngameTable()->nPlayerID[2],
				m_pJubmoo->GetIngameTable()->nCardNum[2],
				m_pJubmoo->GetIngameTable()->nPlayerID[3],
				m_pJubmoo->GetIngameTable()->nCardNum[3],
				m_pJubmoo->GetActivePlayerOrder()
				);
		TextOut(hdc,200,0,tmpbuf,strlen(tmpbuf));
		sprintf(tmpbuf,"setting [%d,%d,%d,%d] offset=%d",
				m_pJubmoo->GetGameSetting()->nPlayerID[0],
				m_pJubmoo->GetGameSetting()->nPlayerID[1],
				m_pJubmoo->GetGameSetting()->nPlayerID[2],
				m_pJubmoo->GetGameSetting()->nPlayerID[3],
				m_pJubmoo->GetIngameOffset()
				);
		TextOut(hdc,200,18,tmpbuf,strlen(tmpbuf));
		ReleaseDC(m_hWnd,hdc);
#endif
		}
		break;
	case ID_JM_EXIT:
        SendMessage(m_hWnd,WM_CLOSE,0,0);
		break;
	case IDI_CONTINUE:
		{
			int i;
			// if player ready , continue playing
			for ( i=0 ; i<4 ; i++ )
			{
				if ( m_pJubmoo->GetGameSetting()->nPlayerID[i] == NO_PLAYER )
				{
					MessageBox(m_hWnd,"Can continue, only when it has 4 players",
								C_JubmooApp::m_lpszAppName,MB_OK);
					return 0;
				}
			}
			ShowWindow(m_pContinueButton->GetHandle(),SW_HIDE);
			if ( m_pJubmoo->GetGameMode() == SERVER_GAMEMODE )
			{
				switch (m_nContinueType)
				{
				case CONTINUE_ENDTURN:
					m_pJubmoo->EndTurn();
					break;
				case CONTINUE_STARTROUND: 
					m_pJubmoo->StartRound();
					break;
				}
			}
		}
		break;
	}

	return 0;
}

LRESULT C_JubmooWnd::OnClose(WPARAM wParam,LPARAM lParam)
{
	if ( MessageBox(m_hWnd,"Exit JubMoo+ really?",C_JubmooApp::m_lpszAppName,
    				MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2) == IDOK )
	{
		KillTimer(m_hWnd,AI_TIMER);
		KillTimer(m_hWnd,PICKUPCARD_TIMER);

		return DefWindowProc( m_hWnd, WM_CLOSE, wParam, lParam );
	}
	return 0;
}

LRESULT C_JubmooWnd::OnDestroy(WPARAM ,LPARAM )
{
	DeleteObject(SelectObject(m_hdcBkMem,m_hbmpBkTmp));
	DeleteDC(m_hdcBkMem);
	DeleteObject(m_hFont);
	DeleteObject(m_hBigFont);

	PostQuitMessage( 0 );
	return 0;
}

LRESULT C_JubmooWnd::OnChar(WPARAM wParam,LPARAM lParam)
{
	// notify all WM_CHAR to chatedit
	SendMessage(m_pChatEdit->GetHandle(),WM_CHAR,wParam,lParam);
    return DefWindowProc(m_hWnd,WM_CHAR,wParam,lParam);
}
	
LRESULT C_JubmooWnd::OnTimer(WPARAM wParam,LPARAM )
{
	switch (wParam)
	{
	case PICKUPCARD_TIMER:
		return OnPickUpCardTimer();
    case AI_TIMER:
    	return OnAIDiscardRequest();
	}
	return 0;
}

LRESULT C_JubmooWnd::OnRButtonDown(WPARAM ,LPARAM )
{
	// create menu
	HMENU hMenu = CreatePopupMenu();

	switch ( m_pJubmoo->GetGameMode() )
	{
	case IDLE_GAMEMODE:
		AppendMenu(hMenu,MF_STRING,ID_JM_START,"Start");
		AppendMenu(hMenu,MF_STRING,ID_JM_JOIN,"Join");
		break;
	case SERVER_GAMEMODE:
	case CLIENT_GAMEMODE:
		AppendMenu(hMenu,MF_STRING,ID_JM_QUIT,"Quit");
		break;
	}
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_STRING,ID_JM_SETTING,"Setting");
	AppendMenu(hMenu,MF_STRING,ID_JM_INFO,"Info");
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_STRING,ID_JM_HELP,"Help");
	AppendMenu(hMenu,MF_STRING,ID_JM_ABOUT,"About");
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_STRING,ID_JM_EXIT,"Exit");
	
	POINT p;
	GetCursorPos(&p);
	TrackPopupMenuEx(hMenu,TPM_VERTICAL,p.x,p.y,m_hWnd,NULL);
	DestroyMenu(hMenu);
	
	return 0;
}

// ======================================================================
// User defined message handler
// ======================================================================
LRESULT C_JubmooWnd::OnCardWndLButtonClick(WPARAM ,LPARAM lParam)
{
	// if now picking card , can't drop the card
	if ( m_bPickingUp ) return 0;

	// click notify off , after discard
	for ( int i=0 ; i<GetMyCardList()->Count() ; i++ )
       	GetMyCardList()->Items(i)->EnableClickNotify(FALSE);

	C_PickCardWnd * pCardWnd = (C_PickCardWnd*)lParam;
	m_pJubmoo->GetMyPlayer()->Discard( pCardWnd->GetCardNumber() );

	return 0;
}

LRESULT C_JubmooWnd::OnPickCardReady(WPARAM wParam,LPARAM lParam)
{
	C_PickCardWnd * pCardWnd = (C_PickCardWnd*)lParam;
	switch ( wParam )
	{
	case PICKUPCARD_READY:
		for ( int i=0 ; i<4 ; i++ )
		{
			if ( pCardWnd == m_pPickCardWnd[i] )
			{
				//TRACE("delete pick card %d\n",i);
				delete pCardWnd;
				m_pPickCardWnd[i] = NULL;
			}
		}
		break;
	}
	return 0;
}

LRESULT C_JubmooWnd::OnStartButtonClick()
{
    // ============ Start waiting for connection =========
    if ( GetJubmoo()->GetGameMode() != IDLE_GAMEMODE ) return 0;

    int nRetVal = GetJubmoo()->StartServer();
    if ( nRetVal == FALSE )
    	MessageBox(GetActiveWindow(),"Can't start JubMoo+ network server. You still can play alone.",
                C_JubmooApp::m_lpszAppName,MB_OK|MB_ICONERROR);

	ShowSettingGame();
	EnableWindow(m_pButton[START_BUTTON]->GetHandle(),FALSE);
	EnableWindow(m_pButton[JOIN_BUTTON]->GetHandle(),FALSE);
	return 0;
}

LRESULT C_JubmooWnd::OnJoinButtonClick()
{
	if ( GetJubmoo()->GetGameMode() != IDLE_GAMEMODE ) return 0;

	char lpszJoinIP[64];

	JubmooApp.OpenRegistry();
	JubmooApp.GetJoinIP(lpszJoinIP,sizeof(lpszJoinIP));
	JubmooApp.CloseRegistry();

	C_JoinDlg JoinDlg(this,lpszJoinIP,sizeof(lpszJoinIP));
	if ( JoinDlg.DoModal() == IDOK )
	{
		// temporarily turn off cooldlg , it'll cause app error 
		// when system dialog appear to ask for connection
		int bUseCoolDlg = FALSE;
		if ( hCoolDlgHook != NULL )
		{
			CoolDlgTerminate();
			bUseCoolDlg = TRUE;
		}
	    if ( !GetJubmoo()->StartClient(lpszJoinIP) )
		{
			if ( bUseCoolDlg )
				CoolDlgAutoSubclass();
			// connect fail
			GetJubmoo()->GetComm()->PopupMessage("Joining to invalid address");
			return 0;
		}
		else
			if ( bUseCoolDlg )
				CoolDlgAutoSubclass();

		EnableWindow(m_pButton[START_BUTTON]->GetHandle(),FALSE);
		EnableWindow(m_pButton[JOIN_BUTTON]->GetHandle(),FALSE);
		JubmooApp.OpenRegistry();
		JubmooApp.SetJoinIP(lpszJoinIP);
		JubmooApp.CloseRegistry();
	}

    return 0;
}

LRESULT C_JubmooWnd::OnQuitButtonClick()
{
	switch ( GetJubmoo()->GetGameMode() )
	{
	case IDLE_GAMEMODE:
		return 0;
	case SERVER_GAMEMODE:
		// if playing , quit is to setting , else is to idle
		if ( GetJubmoo()->GetPlayMode() == INGAME_PLAYMODE )
		{
			if ( MessageBox(m_hWnd,"Quit game and Go back to game setting again?",C_JubmooApp::m_lpszAppName,
						MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2) != IDOK )
				return 0;
			// quit to setting again
			ShowWindow(m_pContinueButton->GetHandle(),SW_HIDE);
			GetJubmoo()->ResetGame();
			return 0;
		}
		else if ( GetJubmoo()->GetPlayMode() == SETTING_PLAYMODE )
		{
			if ( MessageBox(m_hWnd,"Quit game really?",C_JubmooApp::m_lpszAppName,
						MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2) != IDOK )
				return 0;
		}
		break;
	case CLIENT_GAMEMODE:
		if ( GetJubmoo()->GetPlayMode() == INGAME_PLAYMODE ||
				GetJubmoo()->GetPlayMode() == SETTING_PLAYMODE )
			if ( MessageBox(m_hWnd,"Quit game really?",C_JubmooApp::m_lpszAppName,
    					MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2) != IDOK )
				return 0;
		break;
	}

	// do quit here

	// if already idle, can quit again, it may help to clear unwant things
    LockWindowUpdate(m_hWnd);

	// remove player from table
	for ( int i=0 ; i<4 ; i++ )
		ClearPlayerUI(i);

	GetJubmoo()->QuitGame();

	LockWindowUpdate(NULL);

	return 0;
}

    /*/////////////////////// test rotate ////////////////////
        HDC hdc1 = CreateCompatibleDC(m_hDC);
        HBITMAP hbmp1 = LoadBitmap(C_App::m_hInstance,MAKEINTRESOURCE(53));
        HBITMAP hbmpt1 = SelectObject(hdc1,hbmp1);
        HBITMAP hbmpr = pDCEx->CreateRotatedBitmap(hdc1,71,96,45);
        BITMAP bm;
        GetObject(hbmpr,sizeof(BITMAP),&bm);
        // delete hbmp1
        DeleteObject(SelectObject(hdc1,hbmpt1));
        hbmpt1 = SelectObject(hdc1,hbmpr);
        RECT r1;
        r1.left = 0; r1.top = 200;
        r1.right = bm.bmWidth; r1.bottom = bm.bmHeight+200;

        HDC d2;
        HBITMAP b2,ob2;
        d2 = CreateCompatibleDC(m_hdcBkMem);
        b2 = (HBITMAP)pDCEx->CreateMaskBitmap(hdc1,bm.bmWidth,bm.bmHeight,RGB(0,128,128));
        ob2 = (HBITMAP)SelectObject(d2,b2);

        pDCEx->TransparentBlt(0,200,bm.bmWidth,bm.bmHeight,hdc1,d2,0,0);
        DeleteObject(SelectObject(d2,ob2));
        DeleteDC(d2);

        // delete hbmpr
        DeleteObject(SelectObject(hdc1,hbmpt1));
        DeleteDC(hdc1);
    *////////////////////////////////////////////////////////

