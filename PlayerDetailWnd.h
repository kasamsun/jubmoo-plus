#ifndef _C_PLAYERDETAILWND_H_
#define _C_PLAYERDETAILWND_H_

#define PLAYERDETAIL_WIDTH	180
#define PLAYERDETAIL_HEIGHT	100

enum { SHOW_JMPLAYER_NORMAL=0, SHOW_JMPLAYER_INFO=1, SHOW_JMPLAYER_SCOREONHAND=2 };

#include "kswafx/GenericClickWnd.h"
class C_BmpDigit;
class C_PlayerWnd;
class C_Player;
class C_Tooltip;

class C_PlayerDetailWnd : public C_GenericClickWnd
{
public:
	C_PlayerDetailWnd(C_Wnd *,int nDisplayMode,int PosX,int PosY);
	~C_PlayerDetailWnd();

	int GetDisplayMode() { return m_nDisplayMode; }
	void SetDisplayMode(int nMode);
	void SetImage(LPCSTR lpszImage = NULL);
	int ClearImage();
    C_BmpDigit * GetBmpDigit() { return m_pBmpDigit; }

    C_Player * SetPlayer(C_Player *);
    C_Player * ClearPlayer();
	C_Player * GetPlayer() { return m_pPlayer; }
	void Refresh();

	void Float();
	void Sink();
	void ShowActive();
	void ShowInactive();
	void FlashCaption(int bFlag);

	int GetPosX() { return m_nPosX; }
	int GetPosY() { return m_nPosY; }

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    LRESULT OnPaint(WPARAM wParam,LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam,LPARAM lParam);
    LRESULT OnLButtonUp(WPARAM wParam,LPARAM lParam);
	LRESULT OnRButtonDown(WPARAM wParam,LPARAM lParam);
	LRESULT OnRButtonUp(WPARAM wParam,LPARAM lParam);
	LRESULT OnEraseBkgnd(WPARAM wParam,LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam,LPARAM lParam);

private:
	static char szDisplayModeText[3][24];

	HDC m_hdcBkMem;
	HBITMAP m_hbmpBkMem,m_hbmpBkTmp;

	int PaintNormal(HDC);
    int PaintInfo(HDC);
    int PaintScoreOnHand(HDC);

    C_PlayerWnd * m_pPlayerWnd;
	C_Player * m_pPlayer;
    C_BmpDigit * m_pBmpDigit;

    int m_nDisplayMode;
	C_Tooltip * m_pTooltip;
	int m_nPosX,m_nPosY;
	int m_nMoveTick;
	int m_bActivate;
};

#endif