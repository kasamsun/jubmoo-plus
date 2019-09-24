#ifndef _C_ROUNDDLG_H_
#define _C_ROUNDDLG_H_

#include "Dialog.h"

class C_BmpDigit;
class C_PlayerWnd;
class C_CardWnd;

class C_RoundDlg : public C_Dialog
{
public:
	C_RoundDlg(C_Wnd * pParentWnd);
	~C_RoundDlg();

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
	LRESULT OnInitDialog(WPARAM,LPARAM);
	LRESULT OnCommand(WPARAM,LPARAM);

private:
	C_PlayerWnd * m_pPlayerWnd[4];
	C_CardWnd * m_pCardWnd[16];
};

#endif