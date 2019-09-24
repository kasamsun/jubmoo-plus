#ifndef _C_HELPDLG_H_
#define _C_HELPDLG_H_

#include "Dialog.h"
#include "MyTempl.h"

#include "Tooltip.h"

#include "CardWnd.h"//class C_CardWnd;
// MyList need to know defined structure
// otherwise compiler can't call destructor

typedef MyList<C_CardWnd> CARDWND_LIST;

class C_HelpDlg : public C_Dialog
{
public:
	C_HelpDlg(C_Wnd * pParentWnd);
	~C_HelpDlg();

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
	LRESULT OnInitDialog(WPARAM,LPARAM);
	LRESULT OnCommand(WPARAM,LPARAM);
	LRESULT OnCardWndLButtonClick(WPARAM wParam,LPARAM lParam);

private:
    C_Tooltip m_Tooltip;

    CARDWND_LIST pCardWndList;

};

#endif