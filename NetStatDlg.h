#ifndef _C_NETSTATDLG_H_
#define _C_NETSTATDLG_H_

#include "kswafx/Dialog.h"
class C_JMComm;
class C_PlayerWnd;

class C_NetStatDlg : public C_Dialog
{
public:
	C_NetStatDlg(C_JMComm * pParentWnd);
	~C_NetStatDlg();

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
	LRESULT OnInitDialog(WPARAM,LPARAM);
	LRESULT OnCommand(WPARAM,LPARAM);
	void OnOk();

private:
	C_PlayerWnd * m_pPlayerWnd[9];
};

#endif