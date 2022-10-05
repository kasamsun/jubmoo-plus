#ifndef _C_BMPPREVIEWDLG_H_
#define _C_BMPPREVIEWDLG_H_

#include "kswafx/Dialog.h"

class C_BmpPreviewDlg : public C_Dialog
{
public:
	C_BmpPreviewDlg(C_Wnd * pParentWnd,HWND hSrc,LPSTR szFile);
	~C_BmpPreviewDlg();

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
	LRESULT OnInitDialog(WPARAM,LPARAM);
	LRESULT OnCommand(WPARAM,LPARAM);
	LRESULT OnLButtonUp(WPARAM wParam,LPARAM lParam);
	LRESULT OnRButtonUp(WPARAM wParam,LPARAM lParam);


private:
	HWND m_hSrc;
	LPSTR m_szFile;
};

#endif