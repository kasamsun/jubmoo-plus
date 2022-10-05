#ifndef _C_MSGPOPUPWINDOW_H_
#define _C_MSGPOPUPWINDOW_H_

#include "kswafx/GenericClickWnd.h"
#include "kswafx/MyTempl.h"

class C_MsgPopupWnd : public C_GenericWnd
{
public:
	C_MsgPopupWnd(C_Wnd *pParentWnd,int nMaxLine=5,int nMsgTimeout=8000,int nIdleTimeout=5000);
	~C_MsgPopupWnd();

	void PopupMessage(LPCTSTR szMsg=NULL);
	void PutTextMessage(HDC,int xPos,int yPos,LPCSTR pszMsg,int nMsgLen);

    int GetMaxLine() { return m_nMaxLine; }
    void SetMaxLine(int nMaxLine) { m_nMaxLine = nMaxLine; }
    int GetMsgTimeout() { return m_nMsgTimeout; }
    void SetMsgTimeout(int nMsgTimeout) { m_nMsgTimeout = nMsgTimeout; }
    int GetIdleTimeout() { return m_nIdleTimeout; }
    void SetIdleTimeout(int nIdleTimeout) { m_nIdleTimeout = nIdleTimeout; }

    LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    LRESULT OnPaint(WPARAM wParam,LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam,LPARAM lParam);
	LRESULT OnDestroy(WPARAM wParam,LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam,LPARAM lParam);

protected:
	void CreateMsgPopupWndFont();
	void SetWndRegion();

private:
	HFONT m_hFont,m_hOldFont;
    POINT m_pDirPoint[3];

	MyList<char> m_pszMsgList;

	int m_nLeftMargin;
    int m_nTopMargin;
    int m_nFloatHeight;
	int m_nDirHeight;
    int m_nDirWidth;
    int m_nDirMargin;


	int m_nMsgTimeout;
    int m_nIdleTimeout;
    int m_nMaxLine;
	int m_nFontHeight;

};

#endif