#include "HelpDlg.h"

#include "resource.h"
#include "CardNum.h"
#include "JubmooWnd.h"
#include "Jubmoo.h"
#include "JubmooApp.h"

#include <stdio.h>

extern C_JubmooApp JubmooApp;

BEGIN_HANDLE_MSG(C_HelpDlg,C_Dialog)
	HANDLE_MSG(WM_INITDIALOG,OnInitDialog)
	HANDLE_MSG(WM_COMMAND,OnCommand)
	HANDLE_MSG(WM_GENERICCLICKWND_LBUTTONCLICK,OnCardWndLButtonClick)
END_HANDLE_MSG()


C_HelpDlg::C_HelpDlg(C_Wnd * pParentWnd)
{
	Create(IDD_HELP,pParentWnd);
}

C_HelpDlg::~C_HelpDlg()
{
    pCardWndList.DeleteObjects();
}

LRESULT C_HelpDlg::OnInitDialog(WPARAM ,LPARAM )
{
    // create card window list
    pCardWndList.Add( new C_CardWnd(this,SPADE_Q,90,200,FACE_UP) );
    pCardWndList.Add( new C_CardWnd(this,DIAMOND_J,60,180,FACE_UP) );
    pCardWndList.Add( new C_CardWnd(this,CLUB_10,30,160,FACE_UP) );
    pCardWndList.Add( new C_CardWnd(this,HEART_2,200,190,FACE_UP) );
    pCardWndList.Add( new C_CardWnd(this,HEART_3,185,180,FACE_UP) );
    pCardWndList.Add( new C_CardWnd(this,HEART_4,170,170,FACE_UP) );
	pCardWndList.Add( new C_CardWnd(this,HEART_5,155,160,FACE_UP) );
    pCardWndList.Add( new C_CardWnd(this,HEART_6,140,150,FACE_UP) );
    pCardWndList.Add( new C_CardWnd(this,HEART_7,125,140,FACE_UP) );
    pCardWndList.Add( new C_CardWnd(this,HEART_8,110,130,FACE_UP) );
    pCardWndList.Add( new C_CardWnd(this,HEART_9,95,120,FACE_UP) );
    pCardWndList.Add( new C_CardWnd(this,HEART_10,80,110,FACE_UP) );
    pCardWndList.Add( new C_CardWnd(this,HEART_J,65,100,FACE_UP) );
	pCardWndList.Add( new C_CardWnd(this,HEART_Q,50,90,FACE_UP) );
    pCardWndList.Add( new C_CardWnd(this,HEART_K,35,80,FACE_UP) );
    pCardWndList.Add( new C_CardWnd(this,HEART_A,20,70,FACE_UP) );

    // Set Tip Text
    m_Tooltip.Create(this);

    char tmpbuf[32];
    for ( int i = 0 ;i< pCardWndList.Count() ; i++ )
    {
        LoadString(C_App::m_hInstance,pCardWndList.Items(i)->GetCardNumber(),tmpbuf,sizeof(tmpbuf));
        m_Tooltip.AddTool(pCardWndList.Items(i)->GetHandle(),tmpbuf);
        pCardWndList.Items(i)->EnableClickNotify(TRUE);
    }

	C_JubmooWnd * pJmWnd = (C_JubmooWnd*)m_pParentWnd;
	SendMessage(GetDlgItem(m_hWnd,IDC_TESTSCORE),WM_SETFONT,
				(WPARAM)pJmWnd->GetBigJmFont(),TRUE);

	return TRUE;
}

LRESULT C_HelpDlg::OnCommand(WPARAM wParam,LPARAM )
{
	switch ( LOWORD(wParam))
	{
	case IDOK:
		EndDialog(m_hWnd,0);
		break;
    default:
        return FALSE;
	}
	return TRUE;
}

LRESULT C_HelpDlg::OnCardWndLButtonClick(WPARAM ,LPARAM lParam)
{
	C_CardWnd * pCardWnd = (C_CardWnd*)lParam;
    pCardWnd->TurnFace();

	CARD_LIST CardList;
	for ( int i = 0 ; i<16 ; i++ )
	{
		if ( pCardWndList.Items(i)->GetCardStatus() == FACE_UP )
		{
			CARDNUM * cn = new CARDNUM;
			*cn = pCardWndList.Items(i)->GetCardNumber();
			CardList.Add(cn);
		}
	}

	C_Jubmoo * pJm = ((C_JubmooWnd *)m_pParentWnd)->GetJubmoo();
	int nScore = pJm->CalculateScore(&CardList);
	char tmpbuf[8];
	sprintf(tmpbuf,"%d",nScore);
	HWND hTestScore = GetDlgItem(m_hWnd,IDC_TESTSCORE);
	SetWindowText(hTestScore,tmpbuf);

	// update control text
    RECT rMyRect;
    GetClientRect(hTestScore,&rMyRect);
    ClientToScreen(hTestScore,(LPPOINT)&rMyRect.left);
    ClientToScreen(hTestScore,(LPPOINT)&rMyRect.right);
    ScreenToClient(GetParent(hTestScore),(LPPOINT)&rMyRect.left);
    ScreenToClient(GetParent(hTestScore),(LPPOINT)&rMyRect.right);
    InvalidateRect(GetParent(hTestScore),&rMyRect,FALSE);

    CardList.DeleteObjects();

	return TRUE;
}
