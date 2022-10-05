#ifndef _C_ABOUTDLG_H_
#define _C_ABOUTDLG_H_

#include "kswafx/Dialog.h"

class C_AboutDlg : public C_Dialog
{
public:
	C_AboutDlg(C_Wnd * pParentWnd);
	~C_AboutDlg();

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
	LRESULT OnInitDialog(WPARAM,LPARAM);
	LRESULT OnCommand(WPARAM,LPARAM);

private:
};

#endif