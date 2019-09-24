#ifndef _C_STARTGAMEWND_H_
#define _C_STARTGAMEWND_H_

#define STARTGAMEWND_WIDTH	320
#define STARTGAMEWND_HEIGHT	320

#include "GenericWnd.h"
#include "Jubmoo.h"
#include "Tooltip.h"

class C_JubmooWnd;
class C_CoolButton;
class C_PlayerWnd;
class C_Player;

class C_StartGameWnd : public C_GenericWnd
{
public:
	C_StartGameWnd(C_JubmooWnd * pJubmooWnd,LPSTART_GAME_SETTING pSetting=NULL);
	~C_StartGameWnd();

	C_Jubmoo * GetJubmoo() { return m_pJubmoo; }

	int AddPlayer(C_Player * pPlayer);
    int RemovePlayer(C_Player *pPlayer);

	int SetPlayer(int nPlayerID,int nOrder);
	int ClearPlayer(int nOrder);
	int MovePlayer(int nSlotID,int nOrder,int nOldOrder);

	int SetGame(LPSTART_GAME_SETTING pGameSetting);
	int SendUpdateSetting();

    void Refresh();

	// Message declaration
    LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    LRESULT OnPaint(WPARAM wParam,LPARAM lParam);
	LRESULT OnEraseBkgnd(WPARAM wParam,LPARAM lParam);
	LRESULT OnCommand(WPARAM wParam,LPARAM lParam);
	LRESULT OnRButtonDown(WPARAM wParam,LPARAM lParam);

	LRESULT OnPlayerWndBeginDrag(WPARAM wParam,LPARAM lParam);
	LRESULT OnPlayerWndEndDrag(WPARAM wParam,LPARAM lParam);
	LRESULT OnPlayerWndRButtonClick(WPARAM wParam,LPARAM lParam);
private:
	HDC m_hdcBkMem;
	HBITMAP m_hbmpBkMem,m_hbmpBkTmp;
	C_Tooltip m_Tooltip;

	BOOL m_bInitBkg;
	// Main jubmoo engine
	C_Jubmoo * m_pJubmoo;

	C_PlayerWnd * m_pPlayerWndLotStatus[8];
	int m_nLotStatus[8];
	int m_nDragSlot;

	int m_nSelectedSlotID[4];

	// command button
    C_CoolButton * m_pContinueButton;
	C_CoolButton * m_pGroup;
	C_CoolButton * m_pScore500;
	C_CoolButton * m_pScore1000;
	C_CoolButton * m_pScore2000;
	C_CoolButton * m_pSwapJQ;

	HRGN m_hrgnPos[4];
	POINT m_pntLastPos;
};

#endif