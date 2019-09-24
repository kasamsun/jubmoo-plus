#include "SettingDlg.h"

#include <stdio.h>

#include "resource.h"
#include "JubmooApp.h"
#include "JubmooWnd.h"
#include "Jubmoo.h"
#include "JMComm.h"
#include "FaceSelectDlg.h"
#include "BmpPreviewDlg.h"
#include "StartGameWnd.h"
#include "EndGameWnd.h"
#include "ChatEdit.h"

extern C_JubmooApp JubmooApp;

BEGIN_HANDLE_MSG(C_SettingDlg,C_Dialog)
	HANDLE_MSG(WM_INITDIALOG,OnInitDialog)
	HANDLE_MSG(WM_COMMAND,OnCommand)
END_HANDLE_MSG()


C_SettingDlg::C_SettingDlg(C_JubmooWnd * pParentWnd)
{
	m_nSelectedSlot = 1;
	m_nSelectedFace = 1;
	m_nSelectedAISpeed = 1;
	m_nSelectedPickSpeed = 1;
	m_nOldAISpeed = 1;
	m_nOldPickSpeed = 1;
	Create(IDD_SETTING,pParentWnd);
}

C_SettingDlg::~C_SettingDlg()
{
}

void C_SettingDlg::UpdateCtlBkg(HWND hCtl)
{
	RECT rc;
	GetClientRect(hCtl,&rc);
	ClientToScreen(hCtl,(LPPOINT)&rc.left);
	ClientToScreen(hCtl,(LPPOINT)&rc.right);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.left);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.right);
	InvalidateRect(GetParent(hCtl),&rc,FALSE);
}

LRESULT C_SettingDlg::OnInitDialog(WPARAM ,LPARAM )
{
	// set save slot reference table
	m_nSlotID[0] = IDC_SLOT1;
	m_nSlotID[1] = IDC_SLOT2;
	m_nSlotID[2] = IDC_SLOT3;
	m_nSlotID[3] = IDC_SLOT4;
	m_nSlotID[4] = IDC_SLOT5;

	// set limit text to all entry box
	SendMessage(GetDlgItem(m_hWnd,IDC_NAME),EM_SETLIMITTEXT,15,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_OPPONENT1),EM_SETLIMITTEXT,15,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_OPPONENT2),EM_SETLIMITTEXT,15,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_OPPONENT3),EM_SETLIMITTEXT,15,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_OPPONENT4),EM_SETLIMITTEXT,15,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_BKGFILE),EM_SETLIMITTEXT,45,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_CARDFILE),EM_SETLIMITTEXT,45,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_DLGFILE),EM_SETLIMITTEXT,45,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_NUMFILE),EM_SETLIMITTEXT,45,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_PDFILE),EM_SETLIMITTEXT,45,0);
	SendMessage(GetDlgItem(m_hWnd,IDC_DEFCHATMSG),EM_SETLIMITTEXT,255,0);

	if ( ((C_JubmooWnd*)m_pParentWnd)->GetJubmoo()->GetGameMode() != IDLE_GAMEMODE )
	{
		// can't change player data , when game start/join
		EnableWindow(GetDlgItem(m_hWnd,IDC_NAME),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_SELECTFACE),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_RESETSCORE),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_SLOT1),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_SLOT2),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_SLOT3),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_SLOT4),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_SLOT5),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_RESETCOMPSCORE),FALSE);
	}
	if ( ((C_JubmooWnd*)m_pParentWnd)->GetJubmoo()->GetGameMode() == CLIENT_GAMEMODE )
	{
		// can't change game 
		EnableWindow(GetDlgItem(m_hWnd,IDC_AIFAST),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_AINORMAL),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_AISLOW),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_PICKFAST),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_PICKNORMAL),FALSE);
		EnableWindow(GetDlgItem(m_hWnd,IDC_PICKSLOW),FALSE);
	}

	// load data from registry
    char tmpbuf[256];
	RECT rc;
	HWND hCtl;

    JubmooApp.OpenRegistry();

	// last active slot
	m_nSelectedSlot = (int)JubmooApp.GetPlayerSlot();
	hCtl = GetDlgItem(m_hWnd,m_nSlotID[m_nSelectedSlot-1]);
	GetClientRect(hCtl,&rc);
	ClientToScreen(hCtl,(LPPOINT)&rc.left);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.left);
	MoveWindow(GetDlgItem(m_hWnd,IDC_ARROWB),
				rc.left-32,rc.top-5,32,32,TRUE);
	// player name
	JubmooApp.GetName(m_nSelectedSlot,tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_NAME),tmpbuf);
	// player character
	OnFaceSelect(JubmooApp.GetCharID(m_nSelectedSlot));
	// player statistic
    UINT nGamePlay,nWin,nPig,nBigChuan,nChuan;
    JubmooApp.GetStat(m_nSelectedSlot,nGamePlay,nWin,nPig,nBigChuan,nChuan);
	sprintf(tmpbuf,"%d",nChuan);
	SetWindowText(GetDlgItem(m_hWnd,IDC_NOOFCHUAN),tmpbuf);
	sprintf(tmpbuf,"%d",nBigChuan);
	SetWindowText(GetDlgItem(m_hWnd,IDC_NOOFBIGCHUAN),tmpbuf);
	sprintf(tmpbuf,"%d",nPig);
	SetWindowText(GetDlgItem(m_hWnd,IDC_NOOFPIG),tmpbuf);
	sprintf(tmpbuf,"%d",nWin);
	SetWindowText(GetDlgItem(m_hWnd,IDC_NOOFWINNER),tmpbuf);
	sprintf(tmpbuf,"%d",nGamePlay);
	SetWindowText(GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY),tmpbuf);
	SetWindowText(GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY2),tmpbuf);

	// opponent
    JubmooApp.GetOpponent(1,tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT1),tmpbuf);
    JubmooApp.GetOpponent(2,tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT2),tmpbuf);
    JubmooApp.GetOpponent(3,tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT3),tmpbuf);
    JubmooApp.GetOpponent(4,tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT4),tmpbuf);

	// environment
	if ( JubmooApp.GetUseCoolCtl() )
		SendMessage(GetDlgItem(m_hWnd,IDC_USECOOLCTL),BM_SETCHECK,TRUE,0);
	if ( JubmooApp.GetUseFading() )
		SendMessage(GetDlgItem(m_hWnd,IDC_USEFADEBKG),BM_SETCHECK,TRUE,0);
	if ( JubmooApp.GetUseCustomImg() )
		SendMessage(GetDlgItem(m_hWnd,IDC_USECUSTOMBKG),BM_SETCHECK,TRUE,0);

    JubmooApp.GetCustomBkg(tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_BKGFILE),tmpbuf);
    JubmooApp.GetCustomCard(tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_CARDFILE),tmpbuf);
    JubmooApp.GetCustomDlg(tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_DLGFILE),tmpbuf);
    JubmooApp.GetCustomNum(tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_NUMFILE),tmpbuf);
    JubmooApp.GetCustomPD(tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_PDFILE),tmpbuf);

	if ( JubmooApp.IsUseSound() )
		SendMessage(GetDlgItem(m_hWnd,IDC_USESOUND),BM_SETCHECK,TRUE,0);
	if ( JubmooApp.IsAlarmTurn() )
		SendMessage(GetDlgItem(m_hWnd,IDC_ALARMTURN),BM_SETCHECK,TRUE,0);

	// game speed

	switch ( JubmooApp.GetAIDelay() )
	{
	case 300: hCtl = GetDlgItem(m_hWnd,IDC_AIFAST); m_nSelectedAISpeed = 300; break;
	case 600: hCtl = GetDlgItem(m_hWnd,IDC_AINORMAL); m_nSelectedAISpeed = 600; break;
	case 1200: hCtl = GetDlgItem(m_hWnd,IDC_AISLOW); m_nSelectedAISpeed = 1200; break;
	default: hCtl = GetDlgItem(m_hWnd,IDC_AINORMAL); m_nSelectedAISpeed = 600; break;
	}
	m_nOldAISpeed = m_nSelectedAISpeed;
	GetClientRect(hCtl,&rc);
	ClientToScreen(hCtl,(LPPOINT)&rc.left);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.left);
	MoveWindow(GetDlgItem(m_hWnd,IDC_ARROWB2),
					rc.left-32,rc.top-5,32,32,TRUE);

	switch ( JubmooApp.GetPickDelay() )
	{
	case 1000: hCtl = GetDlgItem(m_hWnd,IDC_PICKFAST); m_nSelectedPickSpeed = 1000; break;
	case 1800: hCtl = GetDlgItem(m_hWnd,IDC_PICKNORMAL); m_nSelectedPickSpeed = 1800; break;
	case 2500: hCtl = GetDlgItem(m_hWnd,IDC_PICKSLOW); m_nSelectedPickSpeed = 2500; break;
	default: hCtl = GetDlgItem(m_hWnd,IDC_PICKNORMAL); m_nSelectedPickSpeed = 1800; break;
	}
	m_nOldPickSpeed = m_nSelectedPickSpeed;
	GetClientRect(hCtl,&rc);
	ClientToScreen(hCtl,(LPPOINT)&rc.left);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.left);
	MoveWindow(GetDlgItem(m_hWnd,IDC_ARROWB3),
					rc.left-32,rc.top-5,32,32,TRUE);

    JubmooApp.GetDefChatMsg(tmpbuf,sizeof(tmpbuf));
	SetWindowText(GetDlgItem(m_hWnd,IDC_DEFCHATMSG),tmpbuf);

	JubmooApp.CloseRegistry();

	return TRUE;
}

LRESULT C_SettingDlg::OnCommand(WPARAM wParam,LPARAM )
{
	switch ( LOWORD(wParam))
	{
	case IDC_SLOT1: case IDC_SLOT2: case IDC_SLOT3:
	case IDC_SLOT4:	case IDC_SLOT5:
		OnChangeSaveSlot(LOWORD(wParam));
		break;
	case IDC_SELECTFACE: OnFaceSelect(); break;
	case IDC_RESETSCORE: OnResetScore(); break;
	case IDC_RESETCOMPSCORE: OnResetComputerScore(); break;
	case IDC_SELECTBKG: OnBmpSelect(IDC_BKGFILE); break;
	case IDC_SELECTCARD: OnBmpSelect(IDC_CARDFILE); break;
	case IDC_SELECTDLG: OnBmpSelect(IDC_DLGFILE); break;
	case IDC_SELECTNUM: OnBmpSelect(IDC_NUMFILE); break;
	case IDC_SELECTPD: OnBmpSelect(IDC_PDFILE); break;
	case IDC_AIFAST: case IDC_AINORMAL: case IDC_AISLOW:
		OnChangeAISpeed(LOWORD(wParam));
		break;
	case IDC_PICKFAST: case IDC_PICKNORMAL: case IDC_PICKSLOW:
		OnChangePickSpeed(LOWORD(wParam));
		break;
	case IDOK:
		OnOK();
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

void C_SettingDlg::OnChangeAISpeed(int nSlotID)
{
	HWND hCtl;
	RECT rc;

	switch ( nSlotID )
	{
	case IDC_AIFAST: m_nSelectedAISpeed = 300; break;
	case IDC_AINORMAL: m_nSelectedAISpeed = 600; break;
	case IDC_AISLOW: m_nSelectedAISpeed = 1200; break;
	}
	hCtl = GetDlgItem(m_hWnd,nSlotID);
	GetClientRect(hCtl,&rc);
	ClientToScreen(hCtl,(LPPOINT)&rc.left);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.left);
	MoveWindow(GetDlgItem(m_hWnd,IDC_ARROWB2),
					rc.left-32,rc.top-5,32,32,TRUE);
	UpdateCtlBkg(GetDlgItem(m_hWnd,IDC_ARROWB2));
}

void C_SettingDlg::OnChangePickSpeed(int nSlotID)
{
	HWND hCtl;
	RECT rc;

	switch ( nSlotID )
	{
	case IDC_PICKFAST: m_nSelectedPickSpeed = 1000; break;
	case IDC_PICKNORMAL: m_nSelectedPickSpeed = 1800; break;
	case IDC_PICKSLOW: m_nSelectedPickSpeed = 2500; break;
	}
	hCtl = GetDlgItem(m_hWnd,nSlotID);
	GetClientRect(hCtl,&rc);
	ClientToScreen(hCtl,(LPPOINT)&rc.left);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.left);
	MoveWindow(GetDlgItem(m_hWnd,IDC_ARROWB3),
					rc.left-32,rc.top-5,32,32,TRUE);
	UpdateCtlBkg(GetDlgItem(m_hWnd,IDC_ARROWB3));
}

void C_SettingDlg::OnChangeSaveSlot(int nSlotID)
{
	RECT rc;
	HWND hCtl;
	char tmpbuf[32];

	int nSlot = 0;
	switch ( nSlotID )
	{
	case IDC_SLOT1: nSlot = 1; break;
	case IDC_SLOT2: nSlot = 2; break;
	case IDC_SLOT3: nSlot = 3; break;
	case IDC_SLOT4: nSlot = 4; break;
	case IDC_SLOT5: nSlot = 5; break;
	}

	if ( m_nSelectedSlot == nSlot ) return;

	m_nSelectedSlot = nSlot;
	LockWindowUpdate(m_hWnd);
	// arrow pointer move 
	hCtl = GetDlgItem(m_hWnd,m_nSlotID[nSlot-1]);
	GetClientRect(hCtl,&rc);
	ClientToScreen(hCtl,(LPPOINT)&rc.left);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.left);
	hCtl = GetDlgItem(m_hWnd,IDC_ARROWB);
	MoveWindow(hCtl,rc.left-32,rc.top-5,32,32,TRUE);
	UpdateCtlBkg(hCtl);

	//////////////////////////
	JubmooApp.OpenRegistry();
	// player data
	JubmooApp.GetName(m_nSelectedSlot,tmpbuf,sizeof(tmpbuf));
	hCtl = GetDlgItem(m_hWnd,IDC_NAME);
	SetWindowText(hCtl,tmpbuf);
	//
	OnFaceSelect(JubmooApp.GetCharID(m_nSelectedSlot));
	//
	UINT nGamePlay,nWin,nPig,nBigChuan,nChuan;
	JubmooApp.GetStat(m_nSelectedSlot,nGamePlay,nWin,nPig,nBigChuan,nChuan);
	sprintf(tmpbuf,"%d\0",nChuan);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFCHUAN);
	SetWindowText(hCtl,tmpbuf);
	UpdateCtlBkg(hCtl);
	sprintf(tmpbuf,"%d\0",nBigChuan);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFBIGCHUAN);
	SetWindowText(hCtl,tmpbuf);
	UpdateCtlBkg(hCtl);
	sprintf(tmpbuf,"%d\0",nPig);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFPIG);
	SetWindowText(hCtl,tmpbuf);
	UpdateCtlBkg(hCtl);
	sprintf(tmpbuf,"%d\0",nWin);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFWINNER);
	SetWindowText(hCtl,tmpbuf);
	UpdateCtlBkg(hCtl);
	sprintf(tmpbuf,"%d\0",nGamePlay);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY);
	SetWindowText(hCtl,tmpbuf);
	UpdateCtlBkg(hCtl);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY2);
	SetWindowText(hCtl,tmpbuf);
	UpdateCtlBkg(hCtl);

	JubmooApp.CloseRegistry();
	///////////////////////////

	LockWindowUpdate(NULL);
}

void C_SettingDlg::OnFaceSelect(int id)
{
	if ( id < 0 )
	{
		C_FaceSelectDlg FaceSelect(this,GetDlgItem(m_hWnd,IDC_SELECTFACE));
		id = FaceSelect.DoModal();
		if ( id < 0 ) return ;
	}

	int nResID;
	switch ( id )
    {
    case 0: nResID = IDI_HEAD1; break;
    case 1: nResID = IDI_HEAD2; break;
    case 2: nResID = IDI_HEAD3; break;
    case 3: nResID = IDI_HEAD4; break;
    case 4: nResID = IDI_HEAD5; break;
    case 5: nResID = IDI_HEAD6; break;
    case 6: nResID = IDI_HEAD7; break;
    case 7: nResID = IDI_HEAD8; break;
    case 8: nResID = IDI_HEAD9; break;
    case 9: nResID = IDI_HEAD10; break;
    case 10: nResID = IDI_HEAD11; break;
    case 11: nResID = IDI_HEAD12; break;
    case 12: nResID = IDI_HEAD13; break;
    case 13: nResID = IDI_HEAD14; break;
    case 14: nResID = IDI_HEAD15; break;
    case 15: nResID = IDI_HEAD16; break;
    case 16: nResID = IDI_HEAD17; break;
    case 17: nResID = IDI_HEAD18; break;
    case 18: nResID = IDI_HEAD19; break;
    case 19: nResID = IDI_HEAD20; break;
    case 128: nResID = IDI_HEADCOM1; break;
    case 129: nResID = IDI_HEADCOM2; break;
    case 130: nResID = IDI_HEADCOM3; break;
    case 131: nResID = IDI_HEADCOM4; break;
    default: nResID = IDI_NOBODY; break;
    }
	m_nSelectedFace = id;
	UpdateCtlBkg(GetDlgItem(m_hWnd,IDC_SELECTFACE));
	SendMessage(GetDlgItem(m_hWnd,IDC_SELECTFACE),
				BM_SETIMAGE,IMAGE_ICON,
				(LPARAM)LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(nResID))
				);
}

void C_SettingDlg::OnBmpSelect(int ctlid)
{
	char szFilename[50];

	HWND hCtl = GetDlgItem(m_hWnd,ctlid);

	GetWindowText(hCtl,szFilename,sizeof(szFilename));

	C_BmpPreviewDlg BmpPreviewDlg(this,hCtl,szFilename);
	
	if ( BmpPreviewDlg.DoModal() == IDOK )
		SetWindowText(hCtl,szFilename);
}

void C_SettingDlg::OnResetScore()
{
	HWND hCtl;
	if ( MessageBox(m_hWnd,"Delete player's score?",
				C_JubmooApp::m_lpszAppName,
				MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2) != IDOK )
		return;
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFCHUAN);
	SetWindowText(hCtl,"0");
	UpdateCtlBkg(hCtl);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFBIGCHUAN);
	SetWindowText(hCtl,"0");
	UpdateCtlBkg(hCtl);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFPIG);
	SetWindowText(hCtl,"0");
	UpdateCtlBkg(hCtl);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFWINNER);
	SetWindowText(hCtl,"0");
	UpdateCtlBkg(hCtl);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY);
	SetWindowText(hCtl,"0");
	UpdateCtlBkg(hCtl);
	hCtl = GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY2);
	SetWindowText(hCtl,"0");
	UpdateCtlBkg(hCtl);
}

void C_SettingDlg::OnResetComputerScore()
{
	if ( MessageBox(m_hWnd,"Delete computer's score?",
				C_JubmooApp::m_lpszAppName,
				MB_OKCANCEL|MB_ICONQUESTION|MB_DEFBUTTON2) != IDOK )
		return;
	JubmooApp.OpenRegistry();
	for ( int i=1 ; i<=4 ; i++ )
		JubmooApp.SetOpponentStat(i,0,0,0,0,0);
	JubmooApp.CloseRegistry();
}

void C_SettingDlg::OnOK()
{
	C_JubmooWnd * pJmWnd = (C_JubmooWnd*)m_pParentWnd;

	char tmpbuf[256];

	/////////////////////////
	JubmooApp.OpenRegistry();

    // save all value to registry
	JubmooApp.UpdatePlayerSlot(m_nSelectedSlot);

    GetWindowText(GetDlgItem(m_hWnd,IDC_NAME),tmpbuf,sizeof(tmpbuf));
	JubmooApp.SetName(m_nSelectedSlot,tmpbuf);

	JubmooApp.SetCharID(m_nSelectedSlot,m_nSelectedFace);

	UINT nGamePlay,nWin,nPig,nBigChuan,nChuan;

    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFCHUAN),tmpbuf,sizeof(tmpbuf));
    nChuan = atoi(tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFBIGCHUAN),tmpbuf,sizeof(tmpbuf));
	nBigChuan = atoi(tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFPIG),tmpbuf,sizeof(tmpbuf));
	nPig = atoi(tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFWINNER),tmpbuf,sizeof(tmpbuf));
	nWin = atoi(tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY),tmpbuf,sizeof(tmpbuf));
	nGamePlay = atoi(tmpbuf);
	JubmooApp.SetStat(m_nSelectedSlot,nGamePlay,nWin,nPig,nBigChuan,nChuan);

    GetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT1),tmpbuf,sizeof(tmpbuf));
	JubmooApp.SetOpponent(1,tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT2),tmpbuf,sizeof(tmpbuf));
	JubmooApp.SetOpponent(2,tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT3),tmpbuf,sizeof(tmpbuf));
	JubmooApp.SetOpponent(3,tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_OPPONENT4),tmpbuf,sizeof(tmpbuf));
	JubmooApp.SetOpponent(4,tmpbuf);

    GetWindowText(GetDlgItem(m_hWnd,IDC_BKGFILE),tmpbuf,sizeof(tmpbuf));
	JubmooApp.SetCustomBkg(tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_CARDFILE),tmpbuf,sizeof(tmpbuf));
    JubmooApp.SetCustomCard(tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_DLGFILE),tmpbuf,sizeof(tmpbuf));
    JubmooApp.SetCustomDlg(tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_NUMFILE),tmpbuf,sizeof(tmpbuf));
    JubmooApp.SetCustomNum(tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_PDFILE),tmpbuf,sizeof(tmpbuf));
    JubmooApp.SetCustomPD(tmpbuf);

    if ( SendMessage(GetDlgItem(m_hWnd,IDC_USECOOLCTL),BM_GETCHECK,0,0) == BST_CHECKED )
    	JubmooApp.SetUseCoolCtl(TRUE);
    else
    	JubmooApp.SetUseCoolCtl(FALSE);

    if ( SendMessage(GetDlgItem(m_hWnd,IDC_USEFADEBKG),BM_GETCHECK,0,0) == BST_CHECKED )
    	JubmooApp.SetUseFading(TRUE);
    else
    	JubmooApp.SetUseFading(FALSE);

    if ( SendMessage(GetDlgItem(m_hWnd,IDC_USECUSTOMBKG),BM_GETCHECK,0,0) == BST_CHECKED )
    	JubmooApp.SetUseCustomImg(TRUE);
    else
    	JubmooApp.SetUseCustomImg(FALSE);

    if ( SendMessage(GetDlgItem(m_hWnd,IDC_USESOUND),BM_GETCHECK,0,0) == BST_CHECKED )
    	JubmooApp.UseSound(TRUE);
    else
    	JubmooApp.UseSound(FALSE);
    if ( SendMessage(GetDlgItem(m_hWnd,IDC_ALARMTURN),BM_GETCHECK,0,0) == BST_CHECKED )
    	JubmooApp.AlarmTurn(TRUE);
    else
    	JubmooApp.AlarmTurn(FALSE);

	if ( m_nOldAISpeed != m_nSelectedAISpeed )
	{
		JubmooApp.SetAIDelay(m_nSelectedAISpeed);
		pJmWnd->SetGameSpeed(m_nSelectedAISpeed,m_nSelectedPickSpeed);
	}

	if ( m_nOldPickSpeed != m_nSelectedPickSpeed )
	{
		JubmooApp.SetPickDelay(m_nSelectedPickSpeed);
		pJmWnd->SetGameSpeed(m_nSelectedAISpeed,m_nSelectedPickSpeed);

		// pick speed change , set to all client
		C_JMComm * pComm = ((C_JubmooWnd*)m_pParentWnd)->GetJubmoo()->GetComm();
		for ( int i=0 ; i<pComm->GetClientList()->Count() ; i++ )
		{
			((C_JMComm*)pComm->GetClientList()->Items(i))->SendSetSpeed(m_nSelectedAISpeed,
														m_nSelectedPickSpeed);
		}
	}

	GetWindowText(GetDlgItem(m_hWnd,IDC_DEFCHATMSG),tmpbuf,sizeof(tmpbuf));
	JubmooApp.SetDefChatMsg(tmpbuf);

	JubmooApp.CloseRegistry();

    // set img follow this setting
    pJmWnd->GetJubmoo()->GetMyPlayer()->SetCharacterID(m_nSelectedFace);
    GetWindowText(GetDlgItem(m_hWnd,IDC_NAME),tmpbuf,sizeof(tmpbuf));
    pJmWnd->GetJubmoo()->GetMyPlayer()->SetName(tmpbuf);
    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFCHUAN),tmpbuf,sizeof(tmpbuf));
    pJmWnd->GetJubmoo()->GetMyPlayer()->SetNoOfChuan(atoi(tmpbuf));
    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFBIGCHUAN),tmpbuf,sizeof(tmpbuf));
    pJmWnd->GetJubmoo()->GetMyPlayer()->SetNoOfBigChuan(atoi(tmpbuf));
    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFPIG),tmpbuf,sizeof(tmpbuf));
    pJmWnd->GetJubmoo()->GetMyPlayer()->SetNoOfPig(atoi(tmpbuf));
    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFWINNER),tmpbuf,sizeof(tmpbuf));
    pJmWnd->GetJubmoo()->GetMyPlayer()->SetNoOfWinner(atoi(tmpbuf));
    GetWindowText(GetDlgItem(m_hWnd,IDC_NOOFGAMEPLAY),tmpbuf,sizeof(tmpbuf));
    pJmWnd->GetJubmoo()->GetMyPlayer()->SetNoOfGamePlay(atoi(tmpbuf));
   	pJmWnd->SetImage();
	if ( pJmWnd->m_pStartGameWnd != NULL ) pJmWnd->m_pStartGameWnd->Refresh();
	if ( pJmWnd->m_pEndGameWnd != NULL ) pJmWnd->m_pEndGameWnd->Refresh();
	pJmWnd->m_pChatEdit->SetDefChatMsg();
}
