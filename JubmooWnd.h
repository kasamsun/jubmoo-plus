#ifndef _C_JUBMOOWND_H_
#define _C_JUBMOOWND_H_

#define JUBMOOWND_WIDTH		720
#define JUBMOOWND_HEIGHT	540

enum { BUTTON_START,BUTTON_JOIN,BUTTON_QUIT,BUTTON_SETTING,BUTTON_INFO,
        BUTTON_HELP,BUTTON_ABOUT,BUTTON_EXIT };

#define CONTINUE_ENDTURN	0
#define CONTINUE_STARTROUND	1

#define MAX_JUBMOO_BUTTON   	8
#define SUDDEN_DISCARDMODE		1

#include "GenericWnd.h"
#include "MyTempl.h"

class C_Jubmoo;
class C_CoolButton;
class C_Button;
class C_ChatEdit;
class C_Player;
class C_PlayerDetailWnd;
class C_PlayerWnd;
class C_EndGameWnd;
class C_SettingDlg;
class C_Dialog;

#include <Tooltip.h>
#include "PickCardWnd.h"
#include "StartGameWnd.h"

typedef MyList<C_PickCardWnd> MYCARD_LIST;
typedef MyList<C_PickCardWnd> DROPCARD_LIST;

class C_JubmooWnd : public C_GenericWnd
{
	friend C_SettingDlg;
public:
	C_JubmooWnd(C_Jubmoo * pJubmoo);
	~C_JubmooWnd();

	C_Jubmoo * GetJubmoo() { return m_pJubmoo; }

	void InitScreen();
	void SetImage();
    int ClearImage();
	void CreateJmFont();
    HFONT GetJmFont() { return m_hFont; }
    HFONT GetBigJmFont() { return m_hBigFont; }

	// Utility functions
	void MoveToCenter();

    // Interface
    int ShowSettingGame(LPSTART_GAME_SETTING pGameSetting=NULL);
	int ShowStartGame();
	int ShowReplacedPlayer(int nOrder);
    int ShowEndGame();
    int ShowEndRound();
    int ShowIdleGame();

	int IsEndGame();
	void RefreshEndGame();

	int SetCardOnHand();
	int ClearCardOnHand();
	int ClearDropCard();

    int AddSpectatorUI(C_Player * pPlayer);
    int RemoveSpectatorUI(C_Player * pPlayer);
    int SetPlayerUI(int nPos,C_Player * pPlayer);
    int ClearPlayerUI(int nPos);

	void RefreshPlayersCard(int nPos);
	void RefreshMyCardList();
	void RefreshPlayersTable(int nPos);
	int PaintPlayersCard(HDC hDC,int nPos,RECT& rc,
						CARD_LIST *pCardList,CARD_LIST * pTakenCardList);
	MYCARD_LIST * GetMyCardList() { return &m_MyCardList; }
	DROPCARD_LIST * GetDropCardList() { return &m_DropCardList; }

	int Discard(int nPos,int nCardNum,int nDiscardMode=0);
	int ShiftDropCard(int nOffset);
	int PickUpCard(int nPos);
	int RequestDiscard();

	int WaitForLostPlayer(int nContinueType);

	int SetGameSpeed(int nAIDelay,int nPickDelay);
	int GetAIDelay() { return m_nAIDelay; }
	int GetPickDelay() { return m_nPickDelay; }

	C_Dialog * GetJmDialog() { return m_pDialog; }
	C_PlayerDetailWnd * GetPlayerDetailWnd(int nOrder) { return m_pPlayerDetailWnd[nOrder]; }

	// Message declaration
    LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    LRESULT OnPaint(WPARAM wParam,LPARAM lParam);
	LRESULT OnNCLButtonDblClk(WPARAM wParam,LPARAM lParam);
	LRESULT OnCommand(WPARAM wParam,LPARAM lParam);
	LRESULT OnClose(WPARAM wParam,LPARAM lParam);
	LRESULT OnDestroy(WPARAM wParam,LPARAM lParam);
    LRESULT OnChar(WPARAM,LPARAM);
	LRESULT OnRButtonDown(WPARAM wParam,LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam,LPARAM lParam);

	// user message handler
	LRESULT OnCardWndLButtonClick(WPARAM wParam,LPARAM lParam);
	LRESULT OnPickCardReady(WPARAM wParam,LPARAM lParam);
	LRESULT OnPickUpCardTimer();
    LRESULT OnAIDiscardRequest();

    LRESULT OnStartButtonClick();
    LRESULT OnJoinButtonClick();
    LRESULT OnQuitButtonClick();

private:
	HDC m_hdcBkMem;
	HBITMAP m_hbmpBkMem,m_hbmpBkTmp;
	HFONT m_hFont,m_hBigFont;
	C_Tooltip m_Tooltip;

	// Main jubmoo engine
	C_Jubmoo * m_pJubmoo;

	// Command Button
    C_CoolButton * m_pButton[MAX_JUBMOO_BUTTON];
	// player label
    C_PlayerDetailWnd * m_pPlayerDetailWnd[4];
    // player lot status
    C_Player * m_pPlayerLotStatus[8];
	// continue button ( in case of player lost during the game )
    C_CoolButton * m_pContinueButton;
	int m_nContinueType;

	C_StartGameWnd * m_pStartGameWnd;
    C_EndGameWnd * m_pEndGameWnd;
	
	// dialog that currently displayed
	C_Dialog * m_pDialog;

	// input windows
    C_ChatEdit * m_pChatEdit;
	MYCARD_LIST m_MyCardList;
	DROPCARD_LIST m_DropCardList;

	int m_bPickingUp;	// flag is to check picking up , prevent drop card
	// picking card to be moving
	C_PickCardWnd * m_pPickCardWnd[4];
	int m_nPickToPos;

	int m_nPickDelay;
	int m_nAIDelay;
};

#endif