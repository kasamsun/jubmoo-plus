#include "InfoDlg.h"

#include <stdio.h>
#include "resource.h"
#include "Jubmoo.h"
#include "JubmooWnd.h"
#include "ScoreChart.h"

BEGIN_HANDLE_MSG(C_InfoDlg,C_Dialog)
	HANDLE_MSG(WM_INITDIALOG,OnInitDialog)
	HANDLE_MSG(WM_COMMAND,OnCommand)
END_HANDLE_MSG()


C_InfoDlg::C_InfoDlg(C_Wnd * pParentWnd)
{
	Create(IDD_INFO,pParentWnd);
}

C_InfoDlg::~C_InfoDlg()
{
	delete m_pScoreChart;
}

LRESULT C_InfoDlg::OnInitDialog(WPARAM ,LPARAM )
{
	C_JubmooWnd * pJmWnd = (C_JubmooWnd*)m_pParentWnd;
	C_Jubmoo * pJm = pJmWnd->GetJubmoo();

	char tmpbuf[64];
	sprintf(tmpbuf,"%d",pJm->GetGameNumber()+1);
	SetWindowText(GetDlgItem(m_hWnd,IDC_GAMENUM),tmpbuf);
	SendMessage(GetDlgItem(m_hWnd,IDC_GAMENUM),WM_SETFONT,
				(WPARAM)pJmWnd->GetBigJmFont(),TRUE);
	sprintf(tmpbuf,"%d",pJm->GetGameSetting()->nGameScore);
	SetWindowText(GetDlgItem(m_hWnd,IDC_GAMESCORE),tmpbuf);
	SendMessage(GetDlgItem(m_hWnd,IDC_GAMESCORE),WM_SETFONT,
				(WPARAM)pJmWnd->GetBigJmFont(),TRUE);
	if ( pJm->GetGameSetting()->bSwapJQ )
		ShowWindow(GetDlgItem(m_hWnd,IDC_SWAPJQ),SW_HIDE);

    RECT rc;
	HWND hCtl = GetDlgItem(m_hWnd,IDC_CHART);
	GetClientRect(hCtl,&rc);
	ClientToScreen(hCtl,(LPPOINT)&rc.left);
	ClientToScreen(hCtl,(LPPOINT)&rc.right);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.left);
	ScreenToClient(GetParent(hCtl),(LPPOINT)&rc.right);
	ShowWindow(hCtl,SW_HIDE);

    // use IDC_CHART as dummy for chart's size
	if ( HIBYTE(GetKeyState(VK_RBUTTON)) )
		m_pScoreChart = new C_ScoreChart(this,rc,pJm,BAR_CHART);
	else
		m_pScoreChart = new C_ScoreChart(this,rc,pJm,LINE_CHART);

	return TRUE;
}

void C_InfoDlg::Refresh()
{
	C_JubmooWnd * pJmWnd = (C_JubmooWnd*)m_pParentWnd;
	C_Jubmoo * pJm = pJmWnd->GetJubmoo();

	// update game number
	char tmpbuf[64];
	sprintf(tmpbuf,"%d",pJm->GetGameNumber()+1);
	SetWindowText(GetDlgItem(m_hWnd,IDC_GAMENUM),tmpbuf);

	// update chart
	InvalidateRect(m_hWnd,NULL,FALSE);
	m_pScoreChart->Refresh();
}

LRESULT C_InfoDlg::OnCommand(WPARAM wParam,LPARAM )
{
	switch ( LOWORD(wParam))
	{
	case IDC_BARCHART:
		if ( m_pScoreChart->GetChartType() == BAR_CHART ) break;
		m_pScoreChart->SetChartType(BAR_CHART);
		break;
	case IDC_LINECHART:
		if ( m_pScoreChart->GetChartType() == LINE_CHART ) break;
		m_pScoreChart->SetChartType(LINE_CHART);
		break;
	case IDOK:
		EndDialog(m_hWnd,0);
		break;
    default:
        return FALSE;
	}
	return TRUE;
}
