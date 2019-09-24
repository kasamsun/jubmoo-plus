#include "JoinDlg.h"

#include "resource.h"

BEGIN_HANDLE_MSG(C_JoinDlg,C_Dialog)
	HANDLE_MSG(WM_INITDIALOG,OnInitDialog)
	HANDLE_MSG(WM_COMMAND,OnCommand)
END_HANDLE_MSG()


C_JoinDlg::C_JoinDlg(C_Wnd * pParentWnd,LPSTR lpszJoinIP,int nJoinIPLen)
{
	Create(IDD_JOIN,pParentWnd);
	m_lpszJoinIP = lpszJoinIP;
	m_nJoinIPLen = nJoinIPLen;
}

C_JoinDlg::~C_JoinDlg()
{
}

LRESULT C_JoinDlg::OnInitDialog(WPARAM ,LPARAM )
{
	HWND hEdit = GetDlgItem(m_hWnd,IDC_JOINIP);
	SetWindowText(hEdit,m_lpszJoinIP);
	SendMessage(hEdit,EM_SETLIMITTEXT,(WPARAM)m_nJoinIPLen-1,0);
	SetFocus(hEdit);
	return TRUE;
}

LRESULT C_JoinDlg::OnCommand(WPARAM wParam,LPARAM )
{
	switch ( LOWORD(wParam))
	{
	case IDOK:
		GetWindowText(GetDlgItem(m_hWnd,IDC_JOINIP),m_lpszJoinIP,m_nJoinIPLen);
		EndDialog(m_hWnd,IDOK);
		break;
	case IDCANCEL:
		EndDialog(m_hWnd,IDCANCEL);
		break;
    default:
        return FALSE;
	}
	return TRUE;
}
