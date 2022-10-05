#ifndef _C_ENDGAMEWND_H_
#define _C_ENDGAMEWND_H_

#define ENDGAMEWND_WIDTH	640
#define ENDGAMEWND_HEIGHT	240

#include "kswafx/GenericWnd.h"
#include "kswafx/MyTempl.h"


class C_Jubmoo;
class C_JubmooWnd;
class C_CoolButton;
#include "Player.h"
#include "PlayerWnd.h"

class C_EndGameWnd : public C_GenericWnd
{
public:
	C_EndGameWnd(C_JubmooWnd * pJubmooWnd);
	~C_EndGameWnd();

	C_Jubmoo * GetJubmoo() { return m_pJubmoo; }
	void CreatePlayerWnd();
    void Refresh();

    void SetPigImg();
    void ClearPigImg();

	// Message declaration
    LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    LRESULT OnPaint(WPARAM wParam,LPARAM lParam);
	LRESULT OnEraseBkgnd(WPARAM wParam,LPARAM lParam);
	LRESULT OnCommand(WPARAM wParam,LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam,LPARAM lParam);
	LRESULT OnRButtonDown(WPARAM wParam,LPARAM lParam);

private:
	HDC m_hdcBkMem;
	HBITMAP m_hbmpBkMem,m_hbmpBkTmp;
    HDC m_hdcPig;
    HBITMAP m_hbmpPig,m_hbmpPigTmp;
    HDC m_hdcPigMsk;
    HBITMAP m_hbmpPigMsk,m_hbmpPigMskTmp;

	BOOL m_bInitBkg;
	// Main jubmoo engine
	C_Jubmoo * m_pJubmoo;

    C_CoolButton * m_pContinueButton;

	MyList<C_PlayerWnd> m_pPigPlayerWnd;
	MyList<C_PlayerWnd> m_pPlayerWnd;

    int m_nPigFrame;

};

#endif