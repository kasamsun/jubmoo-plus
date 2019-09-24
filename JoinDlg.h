#ifndef _C_JOINDLG_H_
#define _C_JOINDLG_H_

#include <Dialog.h>

class C_JoinDlg : public C_Dialog
{
public:
	C_JoinDlg(C_Wnd * pParentWnd,LPSTR lpszJoinIP,int nJoinIPLen);
	~C_JoinDlg();

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
	LRESULT OnInitDialog(WPARAM,LPARAM);
	LRESULT OnCommand(WPARAM,LPARAM);

private:
	char * m_lpszJoinIP;
	int m_nJoinIPLen;
};

#endif