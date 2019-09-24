#ifndef _C_PICKCARDWND_H_
#define _C_PICKCARDWND_H_

#define WM_PICKCARD_MOVED	WM_USER+300
// WM_PICKCARD_MOVED   this message will be sent to parent to notify this card
// 					   have already moved.
//	WPARAM - return flag value from bRetFlag of MoveCardTo function
//	LPARAM - return pointer to this card

#define VERYSLOW_MOVECARD	10
#define SLOW_MOVECARD	20
#define NORMAL_MOVECARD 40
#define FAST_MOVECARD	80

#include "CardWnd.h"

class C_PickCardWnd : public C_CardWnd
{
public:
	C_PickCardWnd(C_Wnd *,int CardNum,int PosX,int PosY,int CardStatus=0);
	~C_PickCardWnd();

	// interface
    void EnableAutoMoveUp(int bFlag);
    int IsAutoMoveUp() { return m_bAutoMoveUp; }
	int IsMoving() { return m_bMoving; }
	int IsMovedUp() { return m_bMovedUp; }
	int MoveCardTo(int nX,int nY,int bRetFlag=0,int nSpeed=NORMAL_MOVECARD);

	// to freeze automoveup card which we select
	int FreezeCard(int nFlag = TRUE);

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    LRESULT OnMouseMove(WPARAM wParam,LPARAM lParam);
    LRESULT OnTimer(WPARAM wParam,LPARAM lParam);
protected:
	int m_nCardSpeed;
private:
    int m_bAutoMoveUp;
    int m_bMovedUp;
    POINT m_OrgPoint;	// source of moving up point
	POINT m_SrcPoint;	// source of moving point
	POINT m_DesPoint;	// destination of moving point
	int m_bMoving;
	int m_nStepX,m_nStepY;
	double m_M,m_C;		// Y = mX + c;
	int m_bRetFlag;
	int m_bFreeze;
};

#endif