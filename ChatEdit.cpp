#include "ChatEdit.h"

#include <stdio.h>

#include "resource.h"
#include "JubmooWnd.h"
#include "Jubmoo.h"

extern C_JubmooApp JubmooApp;

BEGIN_HANDLE_MSG(C_ChatEdit,C_Edit)
    HANDLE_MSG(WM_CHAR,OnChar)
    HANDLE_MSG(WM_LBUTTONDBLCLK,OnLButtonDblClk)
    HANDLE_MSG(WM_COMMAND,OnCommand)
END_HANDLE_MSG()

C_ChatEdit::C_ChatEdit() : C_Edit()
{
	SetDefChatMsg();
}

C_ChatEdit::~C_ChatEdit()
{
	if ( m_pszMsgList.Count() > 0 )
		m_pszMsgList.DeleteObjects();
}

BOOL C_ChatEdit::Create(const RECT& rect,C_Wnd* pParentWnd,
						UINT nID)
{
	DWORD dwExStyle = WS_EX_CLIENTEDGE;
    DWORD dwStyle = WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|ES_AUTOHSCROLL;
	int nRetVal = C_Edit::Create("",dwExStyle,dwStyle,rect,pParentWnd,nID);

    if ( nRetVal == FALSE )
        return FALSE;
    else
    {
    	SetLimitText(MAX_CHATLIMITTEXT);
    	return C_ChatEdit::SubclassControl(m_hWnd,pParentWnd);
    }
}

int C_ChatEdit::SubclassControl(HWND hWnd,C_Wnd *pParentWnd)
{
	int nRetVal = C_Control::SubclassControl(hWnd,pParentWnd);
	if ( nRetVal )
	{
		LONG lStyle = GetWindowLong(hWnd,GWL_STYLE);
		lStyle |= WS_CLIPSIBLINGS;
		SetWindowLong(hWnd,GWL_STYLE,lStyle);
	}
	return nRetVal;
}

void C_ChatEdit::SetDefChatMsg()
{
	char pszMsg[256];
    memset(pszMsg,0x00,sizeof(pszMsg));

	JubmooApp.OpenRegistry();
	JubmooApp.GetDefChatMsg(pszMsg,sizeof(pszMsg));
	JubmooApp.CloseRegistry();

	m_pszMsgList.DeleteObjects();

	int i = 0;
	int nLineLen = 0;
	char * pszStartLine = pszMsg;
	do 
	{
		char tmpbuf[MAX_CHATLIMITTEXT];
		if ( pszMsg[i] == 0x0a || pszMsg[i+1] == '\0' )
		{
			if ( pszMsg[i] != 0x0a ) nLineLen++;
			if ( pszMsg[i-1] == 0x0d ) nLineLen--;
			memset(tmpbuf,0x00,MAX_CHATLIMITTEXT);
			if ( nLineLen > MAX_CHATLIMITTEXT )
				strncpy(tmpbuf,pszStartLine,MAX_CHATLIMITTEXT);
			else
				strncpy(tmpbuf,pszStartLine,nLineLen);

			pszStartLine = pszMsg + i + 1;
			if ( nLineLen > 0 )
			{
				char * pszNewMsg = new char[strlen(tmpbuf)+1];
				strcpy(pszNewMsg,tmpbuf);
				pszNewMsg[strlen(tmpbuf)] = 0x00;
				m_pszMsgList.Add(pszNewMsg);
			}
			nLineLen = 0;
		}
		else
			nLineLen++;
		i++;
	} while ( pszMsg[i] != '\0' && i < sizeof(pszMsg)-2);

}

LRESULT C_ChatEdit::OnChar(WPARAM wParam,LPARAM lParam)
{
    TCHAR cCharCode = (TCHAR) wParam;    // character code
//    TRACE("%c = %d",cCharCode,cCharCode);
    //lKeyData = lParam;              // key data
    if ( cCharCode == 13 )
    {
        if ( GetWindowTextLength(m_hWnd) > 0 )
        {
            int nBufLen = GetWindowTextLength(m_hWnd);
            char * pszBuf = new char[nBufLen+1];
            GetWindowText(m_hWnd,pszBuf,nBufLen+1);

            C_Jubmoo * pJubmoo = ((C_JubmooWnd*)GetParentWnd())->GetJubmoo();
            pJubmoo->GetMyPlayer()->Chat(pszBuf);

            SetWindowText(m_hWnd,"");
            delete[] pszBuf;
        }
		return 0;
    }
    else
        return CallWindowProc(m_lpfnDefWndProc,m_hWnd,WM_CHAR,wParam,lParam);
}

LRESULT C_ChatEdit::OnLButtonDblClk(WPARAM ,LPARAM )
{
	if ( m_pszMsgList.Count() > 0 )
    {
    	int i;
        HMENU hMenu = CreatePopupMenu();
        for ( i=0 ; i<m_pszMsgList.Count() ; i++ )
	        AppendMenu(hMenu,MF_STRING,10000+i,m_pszMsgList.Items(i));

		POINT p;
		GetCursorPos(&p);
        TrackPopupMenuEx(hMenu,TPM_VERTICAL,p.x,p.y,m_hWnd,NULL);
        DestroyMenu(hMenu);
    }
	return 0;
}

LRESULT C_ChatEdit::OnCommand(WPARAM wParam,LPARAM )
{
    C_Jubmoo * pJubmoo = ((C_JubmooWnd*)GetParentWnd())->GetJubmoo();
    pJubmoo->GetMyPlayer()->Chat(m_pszMsgList.Items(wParam-10000));
	return 0;
}
