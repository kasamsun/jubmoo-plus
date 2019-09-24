#ifndef _C_INFODLG_H_
#define _C_INFODLG_H_

#include "Dialog.h"

class C_ScoreChart;

class C_InfoDlg : public C_Dialog
{
public:
	C_InfoDlg(C_Wnd * pParentWnd);
	~C_InfoDlg();

	virtual void Refresh();
	C_ScoreChart * GetScoreChart() { return m_pScoreChart; }

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
	LRESULT OnInitDialog(WPARAM,LPARAM);
	LRESULT OnCommand(WPARAM,LPARAM);

private:
	C_ScoreChart * m_pScoreChart;
};

#endif