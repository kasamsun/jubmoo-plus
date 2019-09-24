#include "ScoreChart.h"

#include "DCEx.h"
#include <stdio.h>
#include "resource.h"
#include "JubmooWnd.h"
#include "Jubmoo.h"
#include "Player.h"
#include "PlayerWnd.h"

BEGIN_HANDLE_MSG(C_ScoreChart,C_GenericWnd)
	HANDLE_MSG(WM_PAINT,OnPaint)
    HANDLE_MSG(WM_ERASEBKGND,OnEraseBkgnd)

END_HANDLE_MSG()

C_ScoreChart::C_ScoreChart()
{
}

C_ScoreChart::C_ScoreChart(C_Wnd *pParentWnd,const RECT& rc,C_Jubmoo *pJm,int nChartType) : C_GenericWnd(pParentWnd)
{
	m_hWnd = CreateWindow(
                    m_szClassName,
                    "",
                    WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS,
                    rc.left,rc.top,
                    rc.right-rc.left,rc.bottom-rc.top,
                    pParentWnd->GetHandle(),
                    NULL,
                    C_App::m_hInstance,
                    (LPSTR) this );

    if ( ! m_hWnd )
        exit( FALSE );

	BringWindowToTop(m_hWnd);
    //ShowWindow(m_hWnd,SW_SHOWNA);
	//UpdateWindow(m_hWnd);
    m_nChartType = nChartType;
    m_pJubmoo = pJm;
	m_hFont = pJm->GetUI()->GetJmFont();

    for ( int i=0 ; i<4 ; i++ )
    {
    	m_pPlayer[i] = NULL;
        m_pPlayerWnd[i] = NULL;
    }
	InitPlayerData();

}

C_ScoreChart::~C_ScoreChart()
{
	if ( IsWindow(m_hWnd) ) DestroyWindow(m_hWnd);
	ClearPlayerData();
}

void C_ScoreChart::InitPlayerData()
{
	ClearPlayerData();

	// copy players in the game to member player
	C_Player * pPlayer;
	for ( int i=0 ; i<4 ; i++ )
	{
    	if ( m_pJubmoo->GetIngameTable()->nPlayerID[i] == NO_PLAYER ) continue;
        pPlayer = m_pJubmoo->GetPlayer(m_pJubmoo->GetIngameTable()->nPlayerID[i]);
        if ( pPlayer == NULL ) continue;

        m_pPlayer[i] = new C_Player;
        *m_pPlayer[i] = *pPlayer;

        m_pPlayerWnd[i] = new C_PlayerWnd(this,SINGLE_SIZE_PLAYERWND,3,10+i*50,pPlayer);
	}
}

void C_ScoreChart::ClearPlayerData()
{
    for ( int i=0 ; i<4 ; i++ )
    {
    	delete m_pPlayer[i];m_pPlayer[i] = NULL;
        delete m_pPlayerWnd[i];m_pPlayerWnd[i] = NULL;
    }
}

void C_ScoreChart::Refresh()
{
	InitPlayerData();
	InvalidateRect(m_hWnd,NULL,FALSE);
}

int C_ScoreChart::GetChartType()
{
	return m_nChartType;
}

void C_ScoreChart::SetChartType(int nChartType)
{
	m_nChartType = nChartType;
    RECT rMyRect;
    GetClientRect(m_hWnd,&rMyRect);
    ClientToScreen(m_hWnd,(LPPOINT)&rMyRect.left);
    ClientToScreen(m_hWnd,(LPPOINT)&rMyRect.right);
    ScreenToClient(GetParent(m_hWnd),(LPPOINT)&rMyRect.left);
    ScreenToClient(GetParent(m_hWnd),(LPPOINT)&rMyRect.right);
    InvalidateRect(GetParent(m_hWnd),&rMyRect,TRUE);
}

void C_ScoreChart::PaintBar(HDC hDC,C_Player *pPlayer,
							int x,int y,int width,int height)
{
	int nPositive = 0;
    int nNegative = 0;
    int nSum = pPlayer->GetScore();
    int nMax = abs(m_pJubmoo->GetGameSetting()->nGameScore);

    for ( int i=0 ; i<pPlayer->GetHistoryScore()->Count() ; i++ )
    {
		if ( *(pPlayer->GetHistoryScore()->Items(i)) >= 0 )
			nPositive += *(pPlayer->GetHistoryScore()->Items(i));
		else
			nNegative += -(*(pPlayer->GetHistoryScore()->Items(i)));
	}

	C_DCEx * pDCEx = new C_DCEx(hDC);
	RECT rc;
	int right;
    if ( nPositive > 0 )
	{
		right = x+(width*nPositive)/nMax;
		Rectangle(hDC,x,y,right,y+10);
		rc.left = x+1;
		rc.top = y+1;
		rc.right = right-1;
		rc.bottom = y+9;
		pDCEx->FillGradientRect(&rc,RGB(0,192,0),RGB(255,255,255),8);
	}
    if ( nNegative > 0 )
	{
		right = x+(width*nNegative)/nMax;
    	Rectangle(hDC,x,y+10,right,y+20);
		rc.left = x+1;
		rc.top = y+11;
		rc.right = right-1;
		rc.bottom = y+19;
		pDCEx->FillGradientRect(&rc,RGB(255,0,0),RGB(255,255,255),8);
	}
    if ( -nSum > 0 )
	{
		right = x+(width*-nSum)/nMax;
    	Rectangle(hDC,x,y+20,right,y+35);
		rc.left = x+1;
		rc.top = y+21;
		rc.right = right-1;
		rc.bottom = y+34;
		pDCEx->FillGradientRect(&rc,RGB(0,0,255),RGB(255,255,255),8);
	}
	delete pDCEx;
}

void C_ScoreChart::PaintBarChart(HDC hDC)
{
	char tmpbuf[16];

	RECT rc;
    GetClientRect(m_hWnd,&rc);

	int nBarScale = ((rc.right-rc.left)-80)/5;

	DrawEdge(m_hDC,&rc,EDGE_ETCHED,BF_RECT);
	rc.left += 40;
	rc.right-= 40;
	rc.bottom-=20;
    DrawEdge(m_hDC,&rc,EDGE_ETCHED,BF_RECT);
	rc.left = 40+nBarScale;
	rc.right = rc.left + 3*nBarScale;
    DrawEdge(m_hDC,&rc,EDGE_ETCHED,BF_RECT);
	rc.left = 40+2*nBarScale;
	rc.right = rc.left + nBarScale;
    DrawEdge(m_hDC,&rc,EDGE_ETCHED,BF_RECT);

    GetClientRect(m_hWnd,&rc);
    HFONT hOldFont = (HFONT)SelectObject(hDC,m_hFont);

    TextOut(hDC,40,rc.bottom-18,"0",1);
    sprintf(tmpbuf,"%d",m_pJubmoo->GetGameSetting()->nGameScore/4);
    TextOut(hDC,40+nBarScale,rc.bottom-18,tmpbuf,strlen(tmpbuf));
    sprintf(tmpbuf,"%d",m_pJubmoo->GetGameSetting()->nGameScore/2);
    TextOut(hDC,40+nBarScale*2,rc.bottom-18,tmpbuf,strlen(tmpbuf));
    sprintf(tmpbuf,"%d",m_pJubmoo->GetGameSetting()->nGameScore*3/4);
    TextOut(hDC,40+nBarScale*3,rc.bottom-18,tmpbuf,strlen(tmpbuf));
    sprintf(tmpbuf,"%d",m_pJubmoo->GetGameSetting()->nGameScore);
    TextOut(hDC,40+nBarScale*4,rc.bottom-18,tmpbuf,strlen(tmpbuf));

	int nWidth = rc.right-rc.left-80-nBarScale;
	int nHeight = (rc.bottom-rc.top-20)/4;
	if ( m_pPlayer[0] != NULL )
		PaintBar(hDC,m_pPlayer[0],40,5,nWidth,nHeight);
	if ( m_pPlayer[1] != NULL )
		PaintBar(hDC,m_pPlayer[1],40,55,nWidth,nHeight);
	if ( m_pPlayer[2] != NULL )
		PaintBar(hDC,m_pPlayer[2],40,105,nWidth,nHeight);
	if ( m_pPlayer[3] != NULL )
		PaintBar(hDC,m_pPlayer[3],40,155,nWidth,nHeight);
    SelectObject(hDC,hOldFont);
}

void C_ScoreChart::PaintLine(HDC hDC,C_Player *pPlayer,
							int x,int y,int width,int height)
{
	if ( m_pJubmoo->GetGameNumber() <= 0 ) return;
	if ( pPlayer->GetHistoryScore()->Count() <= 0 ) return;
	int nScale = width/pPlayer->GetHistoryScore()->Count();
	
	int nScore = 0;
	int nMax = (-m_pJubmoo->GetGameSetting()->nGameScore);
	nMax = nMax*5/4;
	int nNegative = height/5;
	nNegative = nNegative * 4;
	MoveToEx(hDC,x,nNegative,NULL);
    for ( int i=0 ; i<pPlayer->GetHistoryScore()->Count() ; i++ )
    {
		nScore += *(pPlayer->GetHistoryScore()->Items(i));
		if ( nScore < 0 )
			LineTo(hDC,x+((i+1)*nScale),nNegative-(-nScore)*height/nMax);
		else
			LineTo(hDC,x+((i+1)*nScale),nNegative+(nScore)*height/nMax);
	}
}

void C_ScoreChart::PaintLineChart(HDC hDC)
{
	char tmpbuf[16];

	RECT rc;
    GetClientRect(m_hWnd,&rc);

	int nBarScale = (rc.bottom-rc.top)/5;

	DrawEdge(m_hDC,&rc,EDGE_ETCHED,BF_RECT);
	rc.left += 40;
	rc.right -= 40;
    DrawEdge(m_hDC,&rc,EDGE_ETCHED,BF_RECT);
	rc.top = nBarScale;
	rc.bottom = 4*nBarScale;
    DrawEdge(m_hDC,&rc,EDGE_ETCHED,BF_RECT);
	rc.top = 2*nBarScale;
	rc.bottom = 3*nBarScale;
    DrawEdge(m_hDC,&rc,EDGE_ETCHED,BF_RECT);

    GetClientRect(m_hWnd,&rc);
    HFONT hOldFont = (HFONT)SelectObject(hDC,m_hFont);

    sprintf(tmpbuf,"%d",m_pJubmoo->GetGameSetting()->nGameScore);
    TextOut(hDC,rc.right-40,rc.top,tmpbuf,strlen(tmpbuf));
    sprintf(tmpbuf,"%d",m_pJubmoo->GetGameSetting()->nGameScore*3/4);
    TextOut(hDC,rc.right-40,rc.top+nBarScale,tmpbuf,strlen(tmpbuf));
    sprintf(tmpbuf,"%d",m_pJubmoo->GetGameSetting()->nGameScore/2);
    TextOut(hDC,rc.right-40,rc.top+2*nBarScale,tmpbuf,strlen(tmpbuf));
    sprintf(tmpbuf,"%d",m_pJubmoo->GetGameSetting()->nGameScore/4);
    TextOut(hDC,rc.right-40,rc.top+3*nBarScale,tmpbuf,strlen(tmpbuf));
    TextOut(hDC,rc.right-40,rc.top+4*nBarScale,"0",1);

	int nWidth = rc.right-rc.left-80;
	int nHeight = rc.bottom-rc.top;
	HRGN hrgn = CreateRectRgn(0,0,rc.right-40,nHeight);
	SelectClipRgn(hDC,hrgn);
	if ( m_pPlayer[0] != NULL )
	{
		HPEN hp = CreatePen(PS_SOLID,2,RGB(255,0,0));
		HPEN hold = (HPEN)SelectObject(hDC,hp);
		Rectangle(hDC,5,43,35,46);
		PaintLine(hDC,m_pPlayer[0],40,0,nWidth,nHeight);
		DeleteObject(SelectObject(hDC,hold));
	}
	if ( m_pPlayer[1] != NULL )
	{
		HPEN hp = CreatePen(PS_SOLID,2,RGB(0,255,0));
		HPEN hold = (HPEN)SelectObject(hDC,hp);
		Rectangle(hDC,5,93,35,96);
		PaintLine(hDC,m_pPlayer[1],40,0,nWidth,nHeight);
		DeleteObject(SelectObject(hDC,hold));
	}
	if ( m_pPlayer[2] != NULL )
	{
		HPEN hp = CreatePen(PS_SOLID,2,RGB(0,0,255));
		HPEN hold = (HPEN)SelectObject(hDC,hp);
		Rectangle(hDC,5,143,35,146);
		PaintLine(hDC,m_pPlayer[2],40,0,nWidth,nHeight);
		DeleteObject(SelectObject(hDC,hold));
	}
	if ( m_pPlayer[3] != NULL )
	{
		HPEN hp = CreatePen(PS_SOLID,2,RGB(255,255,0));
		HPEN hold = (HPEN)SelectObject(hDC,hp);
		Rectangle(hDC,5,193,35,196);
		PaintLine(hDC,m_pPlayer[3],40,0,nWidth,nHeight);
		DeleteObject(SelectObject(hDC,hold));
	}
	SelectClipRgn(hDC,NULL);
	DeleteObject(hrgn);

    SelectObject(hDC,hOldFont);
}

///////////////////// Event Handler /////////////////////////
LRESULT C_ScoreChart::OnPaint(WPARAM ,LPARAM )
{
    PAINTSTRUCT ps;
    m_hDC = BeginPaint( m_hWnd, &ps );

    SetBkMode(m_hDC,TRANSPARENT);

    switch (m_nChartType)
    {
    case LINE_CHART:
        PaintLineChart(m_hDC);
        break;
    case BAR_CHART:
        PaintBarChart(m_hDC);
        break;
    }
	EndPaint(m_hWnd,&ps);
	return 0;
}

LRESULT C_ScoreChart::OnEraseBkgnd(WPARAM ,LPARAM )
{
	return 1;
}
