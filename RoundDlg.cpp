#include "RoundDlg.h"

#include <stdio.h>
#include "resource.h"
#include "BmpDigit.h"
#include "PlayerWnd.h"
#include "CardWnd.h"
#include "Jubmoo.h"
#include "JubmooWnd.h"

BEGIN_HANDLE_MSG(C_RoundDlg,C_Dialog)
	HANDLE_MSG(WM_INITDIALOG,OnInitDialog)
	HANDLE_MSG(WM_COMMAND,OnCommand)
END_HANDLE_MSG()


C_RoundDlg::C_RoundDlg(C_Wnd * pParentWnd)
{
	int i;
	for ( i=0 ; i<4 ; i++ )
		m_pPlayerWnd[i] = NULL;
	for ( i=0 ; i<16 ; i++ )
		m_pCardWnd[i] = NULL;
	Create(IDD_ROUNDRESULT,pParentWnd);
}

C_RoundDlg::~C_RoundDlg()
{
	int i;
	for ( i=0 ; i<4 ; i++ )
		delete m_pPlayerWnd[i];
	for ( i=0 ; i<16 ; i++ )
		delete m_pCardWnd[i];
}

LRESULT C_RoundDlg::OnInitDialog(WPARAM ,LPARAM )
{
	int nScoreResID[4] = { IDC_SCORE1,IDC_SCORE2,IDC_SCORE3,IDC_SCORE4 };
	int nRScoreResID[4] = { IDC_RSCORE1,IDC_RSCORE2,IDC_RSCORE3,IDC_RSCORE4 };
	C_Jubmoo * pJubmoo = ((C_JubmooWnd*)m_pParentWnd)->GetJubmoo();

	C_Player * pPlayer;
	int i,x,y,z;

	x = 10; y = 10; z = 0;
	for ( i=0 ; i<4 ; i++ )
	{
		// set score font
		HWND hCtl = GetDlgItem(m_hWnd,nScoreResID[i]);
		HWND hRCtl = GetDlgItem(m_hWnd,nRScoreResID[i]);
		HFONT hFont = ((C_JubmooWnd*)m_pParentWnd)->GetBigJmFont();
		SendMessage(hCtl,WM_SETFONT,(WPARAM)hFont,MAKELPARAM(TRUE,0));
		SendMessage(hRCtl,WM_SETFONT,(WPARAM)hFont,MAKELPARAM(TRUE,0));

		// create player wnd
		pPlayer = pJubmoo->GetPlayer(pJubmoo->GetIngameTable()->nPlayerID[i]);
		if ( pPlayer != NULL )
		{
			m_pPlayerWnd[i] = new C_PlayerWnd(this,SINGLE_SIZE_PLAYERWND,
										x,y,pPlayer );
			for ( int j=0 ; j<pPlayer->GetTakenCardList()->Count() ; j++ )
			{
				m_pCardWnd[z] = new C_CardWnd(this,
								*(pPlayer->GetTakenCardList()->Items(j)),
								x+95+15*j,y,FACE_UP);
				BringWindowToTop( m_pCardWnd[z++]->GetHandle() );
			}
			char tmpbuf[16];
			sprintf(tmpbuf,"%d",pJubmoo->CalculateScore(pPlayer->GetTakenCardList()));
			SetWindowText(hRCtl,tmpbuf);
			sprintf(tmpbuf,"%d",pJubmoo->CalculateScore(pPlayer->GetTakenCardList())+
								pPlayer->GetScore() );
			SetWindowText(hCtl,tmpbuf);
		}
		else
		{
			SetWindowText(hCtl,"");
			SetWindowText(hRCtl,"");
		}
		y+=50;
	}
	return TRUE;
}

LRESULT C_RoundDlg::OnCommand(WPARAM wParam,LPARAM )
{
	switch ( LOWORD(wParam))
	{
	case IDOK:
		// lock update , prevent screen flashing when close
		LockWindowUpdate(GetParent(m_hWnd));
		EndDialog(m_hWnd,0);
		if ( ((C_JubmooWnd*)m_pParentWnd)->IsEndGame() ) ((C_JubmooWnd*)m_pParentWnd)->RefreshEndGame();
		break;
    default:
        return FALSE;
	}
	return TRUE;
}

