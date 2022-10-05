#include "NetStatDlg.h"

#include <stdio.h>

#include "resource.h"
#include "JMComm.h"
#include "PlayerWnd.h"
#include "JubmooWnd.h"
#include "Jubmoo.h"
#include "kswafx/CoolDlg.h"

extern HHOOK hCoolDlgHook;
extern C_JubmooApp JubmooApp;

BEGIN_HANDLE_MSG(C_NetStatDlg,C_Dialog)
	HANDLE_MSG(WM_INITDIALOG,OnInitDialog)
	HANDLE_MSG(WM_COMMAND,OnCommand)
END_HANDLE_MSG()


C_NetStatDlg::C_NetStatDlg(C_JMComm * pParentWnd)
{
	Create(IDD_NETSTAT,pParentWnd);
	for ( int i=0 ;i<8 ;i++ )
		m_pPlayerWnd[i] = NULL;
}

C_NetStatDlg::~C_NetStatDlg()
{
	for ( int i=0 ;i<8 ;i++ )
		delete m_pPlayerWnd[i];
}

LRESULT C_NetStatDlg::OnInitDialog(WPARAM ,LPARAM )
{
	char tmpbuf[128];
	char szMyIP[128];
	char szIPBuf[16];
	UINT nPort;
	UINT nAddrCount = 0;
	int nResID[7] = { IDC_CLIENT1,IDC_CLIENT2,IDC_CLIENT3,
					IDC_CLIENT4,IDC_CLIENT5,IDC_CLIENT6,IDC_CLIENT7 };

	C_JMComm * pComm = (C_JMComm*)m_pParentWnd;

	memset(szMyIP,0x00,sizeof(szMyIP));
	memset(tmpbuf,0x00,sizeof(tmpbuf));

	// temporarily turn off cooldlg , it'll cause app error 
	// when system dialog appear to ask for connection
	int bUseCoolDlg = FALSE;
	if ( hCoolDlgHook != NULL )
	{
		CoolDlgTerminate();
		bUseCoolDlg = TRUE;
	}

	if ( pComm->GetMyHostAddr(szMyIP,&nAddrCount) )
	{
		int nOffset = 0;
		for ( UINT i=0 ; i<nAddrCount ; i++ )
		{
			if ( nOffset != 0 )
			{
				tmpbuf[nOffset-1] = ',';
				tmpbuf[nOffset] = 0x00;
				strcat(tmpbuf+nOffset,szMyIP+nOffset);
			}
			else
				strcpy(tmpbuf,szMyIP);
			nOffset = strlen(szMyIP+nOffset)+1;
		}
		SetWindowText(GetDlgItem(m_hWnd,IDC_MYIP),tmpbuf);
	}
	RECT rc;
	HWND hEdit = GetDlgItem(m_hWnd,IDC_MYIP);
	GetClientRect(hEdit,&rc);
	ClientToScreen(hEdit,(LPPOINT)&rc.left);
	ScreenToClient(GetParent(hEdit),(LPPOINT)&rc.left);
	rc.left -= 40;
	rc.top -=5;
   	m_pPlayerWnd[0] = new C_PlayerWnd(this,
					SINGLE_SIZE_PLAYERWND,rc.left,rc.top,
					pComm->GetPlayer());

	if ( bUseCoolDlg )
		CoolDlgAutoSubclass();

	if ( pComm->GetPeerName(szIPBuf,&nPort) )
	{
		sprintf(tmpbuf,"%s:%u",szIPBuf,nPort);
		SetWindowText(GetDlgItem(m_hWnd,IDC_STATUS),tmpbuf);
	}
	else
	{
		if ( pComm->GetSockName(szIPBuf,&nPort) )
			sprintf(tmpbuf,"%s:%u",szIPBuf,nPort);
		else
			sprintf(tmpbuf,"Closed");
		SetWindowText(GetDlgItem(m_hWnd,IDC_STATUS),tmpbuf);
	}

	for ( int i=0 ; i<7 ; i++ )
	{
		if ( i < pComm->GetClientList()->Count() )
		{
			C_JMComm * pCliJMComm = (C_JMComm*)pComm->GetClientList()->Items(i);
			if ( pCliJMComm->GetPeerName(szIPBuf,&nPort) )
				SetWindowText(GetDlgItem(m_hWnd,nResID[i]),szIPBuf);
			RECT rc;
			HWND hEdit = GetDlgItem(m_hWnd,nResID[i]);
			GetClientRect(hEdit,&rc);
			ClientToScreen(hEdit,(LPPOINT)&rc.left);
			ScreenToClient(GetParent(hEdit),(LPPOINT)&rc.left);
			rc.left -= 40;
			rc.top -=5;
			// v.1.60 add checking 'JMComm::GetPlayer == NULL'
			if ( pCliJMComm->GetPlayer() != NULL )
   				m_pPlayerWnd[i+1] = new C_PlayerWnd(this,
							SINGLE_SIZE_PLAYERWND,
							rc.left,rc.top,
							pCliJMComm->GetPlayer());
		}
		else
		{
			SetWindowText(GetDlgItem(m_hWnd,nResID[i]),"[No Connection]");
		}
	}


	// data setting
	SendMessage(GetDlgItem(m_hWnd,IDC_PORT),EM_SETLIMITTEXT,5,0);

	JubmooApp.OpenRegistry();

	sprintf(tmpbuf,"%lu",JubmooApp.GetPort());
	SetWindowText(GetDlgItem(m_hWnd,IDC_PORT),tmpbuf);

    if ( JubmooApp.GetUseNetLog() )
		SendMessage(GetDlgItem(m_hWnd,IDC_NETLOG),BM_SETCHECK,TRUE,0);

	JubmooApp.CloseRegistry();
	return TRUE;
}

LRESULT C_NetStatDlg::OnCommand(WPARAM wParam,LPARAM )
{
	switch ( LOWORD(wParam))
	{
	case IDOK:
		OnOk();
		EndDialog(m_hWnd,0);
		break;
    default:
        return FALSE;
	}
	return TRUE;
}

void C_NetStatDlg::OnOk()
{
	char tmpbuf[16];

	JubmooApp.OpenRegistry();

    memset(tmpbuf,0x00,sizeof(tmpbuf));
    GetWindowText(GetDlgItem(m_hWnd,IDC_PORT),tmpbuf,sizeof(tmpbuf));
	JubmooApp.SetPort((DWORD)atol(tmpbuf));

	C_JMComm * pComm = (C_JMComm*)m_pParentWnd;
    if ( SendMessage(GetDlgItem(m_hWnd,IDC_NETLOG),BM_GETCHECK,0,0) == BST_CHECKED )
	{
		JubmooApp.SetUseNetLog(TRUE);
		pComm->OpenLogMessage();
	}
    else
	{
    	JubmooApp.SetUseNetLog(FALSE);
		pComm->CloseLogMessage();
	}

	JubmooApp.CloseRegistry();
}
