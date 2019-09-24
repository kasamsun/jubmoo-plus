#include "AboutDlg.h"

#include <stdio.h>
#include "resource.h"
#include "JubmooApp.h"
#include "JMComm.h"

BEGIN_HANDLE_MSG(C_AboutDlg,C_Dialog)
	HANDLE_MSG(WM_INITDIALOG,OnInitDialog)
	HANDLE_MSG(WM_COMMAND,OnCommand)
END_HANDLE_MSG()


C_AboutDlg::C_AboutDlg(C_Wnd * pParentWnd)
{
	Create(IDD_ABOUT,pParentWnd);
}

C_AboutDlg::~C_AboutDlg()
{
}

LRESULT C_AboutDlg::OnInitDialog(WPARAM ,LPARAM )
{
	char tmpbuf[16];
	SetWindowText(GetDlgItem(m_hWnd,IDC_MY_EMAIL),"kasamsun@hotmail.com");
	SetWindowText(GetDlgItem(m_hWnd,IDC_VERSION),JUBMOO_VERSION);
	SetWindowText(GetDlgItem(m_hWnd,IDC_BUILDDATE),JUBMOO_BUILDDATE);
	sprintf(tmpbuf,"%04x",MSG_VERSION);
	SetWindowText(GetDlgItem(m_hWnd,IDC_MSGID),tmpbuf);
	return TRUE;
}

LRESULT C_AboutDlg::OnCommand(WPARAM wParam,LPARAM )
{
	switch ( LOWORD(wParam))
	{
    case IDC_MY_HOMEPAGE:
		{
		char tmpbuf[50];
		GetWindowText(GetDlgItem(m_hWnd,IDC_MY_HOMEPAGE),tmpbuf,sizeof(tmpbuf));
        ShellExecute(m_hWnd,"open",tmpbuf,"","",SW_SHOW);
		break;
		}
	case IDOK:
		EndDialog(m_hWnd,0);
		break;
    default:
        return FALSE;
	}
	return TRUE;
}

