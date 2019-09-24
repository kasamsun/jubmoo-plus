#include "FaceSelectDlg.h"

#include "resource.h"

BEGIN_HANDLE_MSG(C_FaceSelectDlg,C_Dialog)
	HANDLE_MSG(WM_INITDIALOG,OnInitDialog)
	HANDLE_MSG(WM_COMMAND,OnCommand)
END_HANDLE_MSG()


C_FaceSelectDlg::C_FaceSelectDlg(C_Wnd * pParentWnd,HWND hFace)
{
	m_hFace = hFace;
	Create(IDD_FACESELECT,pParentWnd);
}

C_FaceSelectDlg::~C_FaceSelectDlg()
{
}

LRESULT C_FaceSelectDlg::OnInitDialog(WPARAM,LPARAM)
{
	int nCtlID[20] = { IDC_HEAD1,IDC_HEAD2,IDC_HEAD3,IDC_HEAD4,
						IDC_HEAD5,IDC_HEAD6,IDC_HEAD7,IDC_HEAD8,
						IDC_HEAD9,IDC_HEAD10,IDC_HEAD11,IDC_HEAD12,
						IDC_HEAD13,IDC_HEAD14,IDC_HEAD15,IDC_HEAD16,
						IDC_HEAD17,IDC_HEAD18,IDC_HEAD19,IDC_HEAD20,
						};
	int nResID[20] = { IDI_HEAD1,IDI_HEAD2,IDI_HEAD3,IDI_HEAD4,
						IDI_HEAD5,IDI_HEAD6,IDI_HEAD7,IDI_HEAD8,
						IDI_HEAD9,IDI_HEAD10,IDI_HEAD11,IDI_HEAD12,
						IDI_HEAD13,IDI_HEAD14,IDI_HEAD15,IDI_HEAD16,
						IDI_HEAD17,IDI_HEAD18,IDI_HEAD19,IDI_HEAD20 };
	for ( int i=0 ;i<20 ;i++ )
		SendMessage(GetDlgItem(m_hWnd,nCtlID[i]),
				BM_SETIMAGE,IMAGE_ICON,
				(LPARAM)LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResID[i]))
				);

	RECT rc;
	GetWindowRect(m_hFace,&rc);
	RECT myrc;
	GetWindowRect(m_hWnd,&myrc);
	MoveWindow(m_hWnd,rc.right,rc.top,
					myrc.right-myrc.left,myrc.bottom-myrc.top,TRUE);
	return TRUE;
}

LRESULT C_FaceSelectDlg::OnCommand(WPARAM wParam,LPARAM)
{
	switch ( LOWORD(wParam))
	{
	case IDC_HEAD1: EndDialog(m_hWnd,0); break;
	case IDC_HEAD2: EndDialog(m_hWnd,1); break;
	case IDC_HEAD3: EndDialog(m_hWnd,2); break;
	case IDC_HEAD4: EndDialog(m_hWnd,3); break;
	case IDC_HEAD5: EndDialog(m_hWnd,4); break;
	case IDC_HEAD6: EndDialog(m_hWnd,5); break;
	case IDC_HEAD7: EndDialog(m_hWnd,6); break;
	case IDC_HEAD8: EndDialog(m_hWnd,7); break;
	case IDC_HEAD9: EndDialog(m_hWnd,8); break;
	case IDC_HEAD10: EndDialog(m_hWnd,9); break;
	case IDC_HEAD11: EndDialog(m_hWnd,10); break;
	case IDC_HEAD12: EndDialog(m_hWnd,11); break;
	case IDC_HEAD13: EndDialog(m_hWnd,12); break;
	case IDC_HEAD14: EndDialog(m_hWnd,13); break;
	case IDC_HEAD15: EndDialog(m_hWnd,14); break;
	case IDC_HEAD16: EndDialog(m_hWnd,15); break;
	case IDC_HEAD17: EndDialog(m_hWnd,16); break;
	case IDC_HEAD18: EndDialog(m_hWnd,17); break;
	case IDC_HEAD19: EndDialog(m_hWnd,18); break;
	case IDC_HEAD20: EndDialog(m_hWnd,19); break;
	case IDCANCEL:
		EndDialog(m_hWnd,-1);
		break;
    default:
        return FALSE;
	}
	return TRUE;
}

