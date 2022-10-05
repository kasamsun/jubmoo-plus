#ifndef _C_PLAYERWND_H_
#define _C_PLAYERWND_H_

#define PLAYERWND_WIDTH		32
#define PLAYERWND_HEIGHT	32

enum { SINGLE_SIZE_PLAYERWND=1, DOUBLE_SIZE_PLAYERWND=2 };

#include "kswafx/GenericClickWnd.h"
#include "kswafx/Tooltip.h"
class C_Player;
class C_MsgPopupWnd;

class C_PlayerWnd : public C_GenericClickWnd
{
public:

	C_PlayerWnd(C_Wnd *pParentWnd,int nDisplayMode,int PosX,int PosY,C_Player *pPlayer);
	~C_PlayerWnd();

    void ShowSize(int nDisplayMode);
    UINT GetCharacterResID(int id);
    C_Player * GetPlayer() { return m_pPlayer; }
    int PlayerUpdated();
	int Chat(char *pszMsg);

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    LRESULT OnPaint(WPARAM wParam,LPARAM lParam);
    LRESULT OnLButtonUp(WPARAM wParam,LPARAM lParam);
    LRESULT OnEraseBkgnd(WPARAM,LPARAM);
    LRESULT OnRButtonDown(WPARAM wParam,LPARAM lParam);
    LRESULT OnCommand(WPARAM wParam,LPARAM lParam);

	void Paint(HDC hDC,int x,int y);
private:
    int m_nDisplayMode;
    C_Tooltip m_Tooltip;

    C_Player * m_pPlayer;
    C_MsgPopupWnd * m_pMsgWnd;
};

#endif