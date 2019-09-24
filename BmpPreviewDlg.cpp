#include "BmpPreviewDlg.h"

#include "resource.h"

BEGIN_HANDLE_MSG(C_BmpPreviewDlg,C_Dialog)
	HANDLE_MSG(WM_INITDIALOG,OnInitDialog)
	HANDLE_MSG(WM_COMMAND,OnCommand)
	HANDLE_MSG(WM_LBUTTONUP,OnLButtonUp)
	HANDLE_MSG(WM_RBUTTONUP,OnRButtonUp)
END_HANDLE_MSG()


C_BmpPreviewDlg::C_BmpPreviewDlg(C_Wnd * pParentWnd,HWND hSrc,LPSTR szFile)
{
	m_hSrc = hSrc;
	m_szFile = szFile;
	Create(IDD_BMPPREVIEW,pParentWnd);
}

C_BmpPreviewDlg::~C_BmpPreviewDlg()
{
}

LRESULT C_BmpPreviewDlg::OnInitDialog(WPARAM ,LPARAM )
{
	RECT rc;
	GetWindowRect(m_hSrc,&rc);
	RECT myrc;
	GetWindowRect(m_hWnd,&myrc);
	MoveWindow(m_hWnd,rc.right+25,rc.top-(myrc.bottom-myrc.top)/2,//(rc.bottom-rc.top)/2,
					myrc.right-myrc.left,myrc.bottom-myrc.top,TRUE);

	HBITMAP hbmp = (HBITMAP)LoadImage(NULL,m_szFile,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	SendMessage(GetDlgItem(m_hWnd,IDC_BMPPREVIEW),
				STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(HANDLE)hbmp);
	return TRUE;
} 

LRESULT C_BmpPreviewDlg::OnCommand(WPARAM wParam,LPARAM )
{
	switch ( LOWORD(wParam))
	{
	case IDOK:
		DeleteObject((HANDLE)SendMessage(GetDlgItem(m_hWnd,IDC_BMPPREVIEW),
						STM_GETIMAGE,IMAGE_BITMAP,0));
		EndDialog(m_hWnd,IDOK);
		break;
    default:
        return FALSE;
	}
	return TRUE;
}

LRESULT C_BmpPreviewDlg::OnLButtonUp(WPARAM ,LPARAM )
{
	EndDialog(m_hWnd,IDCANCEL);
	return TRUE;
}

LRESULT C_BmpPreviewDlg::OnRButtonUp(WPARAM ,LPARAM )
{
	EndDialog(m_hWnd,IDCANCEL);
	return TRUE;
}