#ifndef _C_CARDWND_H_
#define _C_CARDWND_H_

#define CARD_WIDTH	71
#define CARD_HEIGHT	96

#define FACE_UP		0
#define FACE_DOWN	1

#include "kswafx/GenericClickWnd.h"

class C_MsgPopupWnd;

class C_CardWnd : public C_GenericClickWnd
{
public:
	C_CardWnd();
	C_CardWnd(C_Wnd *,int CardNum,int PosX,int PosY,int CardStatus=0);
	~C_CardWnd();

    static int C_CardWnd::SetBackCardImage(LPCTSTR lpszName=NULL);
	static int C_CardWnd::ClearBackCardImage();
	static int C_CardWnd::SetCardImage(LPCTSTR lpszName=NULL);
    static int C_CardWnd::ClearCardImage();

    // CardWnd Service
    int PaintCard(HDC hDC,int x,int y,int nCardNum);

    // CardWnd Interface
	int GetCardNumber() { return m_CardNum; }
    int GetCardStatus() { return m_CardStatus; }

	void FaceUp();
	void FaceDown();
	int TurnFace();

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    LRESULT OnPaint(WPARAM wParam,LPARAM lParam);
    LRESULT OnMouseMove(WPARAM wParam,LPARAM lParam);
    LRESULT OnTimer(WPARAM wParam,LPARAM lParam);
protected:
	static HDC m_hdcBackCardImage;
	static HBITMAP m_hbmpBackCardImage,m_hbmpBackCardTmp;
	static HDC m_hdcCardImage;
	static HBITMAP m_hbmpCardImage,m_hbmpCardTmp;

	int m_CardStatus;    // up=0 / down=1
	int m_CardNum;
private:
};

#endif