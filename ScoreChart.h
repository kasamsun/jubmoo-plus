#ifndef _C_SCORECHART_H_
#define _C_SCORECHART_H_

#include "GenericWnd.h"

#define BAR_CHART	1
#define LINE_CHART	2

class C_Player;
class C_Jubmoo;
class C_PlayerWnd;

class C_ScoreChart : public C_GenericWnd
{
public:
	C_ScoreChart();
	C_ScoreChart(C_Wnd *,const RECT& rc,C_Jubmoo *,int nChartType=BAR_CHART);
	~C_ScoreChart();

	void InitPlayerData();
	void ClearPlayerData();
	void Refresh();

	int GetChartType();
	void SetChartType(int nChartType);

	void PaintBar(HDC,C_Player*,int x,int y,int witdth,int height);
	void PaintLine(HDC,C_Player*,int x,int y,int witdth,int height);

    void PaintBarChart(HDC hDC);
    void PaintLineChart(HDC hDC);

	LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    LRESULT OnPaint(WPARAM wParam,LPARAM lParam);
	LRESULT OnEraseBkgnd(WPARAM wParam,LPARAM lParam);
private:
	C_Player * m_pPlayer[4];
    C_PlayerWnd * m_pPlayerWnd[4];
    HFONT m_hFont;

    int m_nChartType;
    C_Jubmoo * m_pJubmoo;
};

#endif