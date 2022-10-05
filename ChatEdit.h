#ifndef _C_CHATEDIT_H_
#define _C_CHATEDIT_H_

#include "kswafx/Edit.h"
#include "kswafx/MyTempl.h"

#define MAX_CHATLIMITTEXT		40
class C_Wnd;

class C_ChatEdit : public C_Edit
{
public:
	C_ChatEdit();
	~C_ChatEdit();

	BOOL Create(const RECT& rect,C_Wnd* pParentWnd,UINT nID);
    int SubclassControl(HWND hWnd,C_Wnd *pParentWnd);
	void SetDefChatMsg();

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    LRESULT OnChar(WPARAM,LPARAM);
    LRESULT OnLButtonDblClk(WPARAM,LPARAM);
    LRESULT OnCommand(WPARAM,LPARAM);

protected:

private:
	MyList<char> m_pszMsgList;
};

#endif