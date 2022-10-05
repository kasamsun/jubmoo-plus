#include "PickCardWnd.h"

#define CARD_MOVE_TIMERID	1120

#define CARD_MOVEUP_TIMER	50
#define CARD_MOVE_TIMER		50

#define NOTINMOVING	0
#define MOVEBYX	1
#define MOVEBYY 2

BEGIN_HANDLE_MSG(C_PickCardWnd,C_CardWnd)
	HANDLE_MSG(WM_MOUSEMOVE,OnMouseMove)
	HANDLE_MSG(WM_TIMER,OnTimer)
	
END_HANDLE_MSG()

C_PickCardWnd::C_PickCardWnd(C_Wnd *pParentWnd,int CardNum,int PosX,int PosY,int CardStatus)
			: C_CardWnd(pParentWnd,CardNum,PosX,PosY,CardStatus)
{
    m_bAutoMoveUp = FALSE;
    m_bMovedUp = FALSE;
	m_bMoving = FALSE;
	m_nCardSpeed = NORMAL_MOVECARD;
	m_bRetFlag = FALSE;
	m_bDragWindow = FALSE;
	m_bFreeze = FALSE;
}

C_PickCardWnd::~C_PickCardWnd()
{
	if ( m_bMoving || m_bMovedUp )
   		KillTimer(m_hWnd,CARD_MOVE_TIMERID);
}

void C_PickCardWnd::EnableAutoMoveUp(int bFlag)
{
	m_bAutoMoveUp = bFlag;
    if ( !m_bAutoMoveUp && m_bMovedUp )
    {
    	KillTimer(m_hWnd,CARD_MOVE_TIMERID);
        MoveWindow(m_hWnd,m_OrgPoint.x,m_OrgPoint.y,CARD_WIDTH,CARD_HEIGHT,TRUE);
        m_bMovedUp = FALSE;
    }
}

int C_PickCardWnd::MoveCardTo(int nX,int nY,int bRetFlag,int nSpeed)
{
	if ( m_bMovedUp )
	{
//		TRACE("Stop moveUP %d\n",m_CardNum);
		KillTimer(m_hWnd,CARD_MOVE_TIMERID);
//			m_bMovedUp = FALSE;
	}

	RECT rc;
	GetWindowRect(m_hWnd,&rc);
	ScreenToClient(GetParent(m_hWnd),(LPPOINT)&rc.left);

	m_bRetFlag = bRetFlag;
	m_nCardSpeed = nSpeed;
	m_DesPoint.x = nX;
	m_DesPoint.y = nY;
	m_SrcPoint.x = rc.left;
	m_SrcPoint.y = rc.top;

	int nDifX = nX-rc.left;
	int nDifY = nY-rc.top;

//	TRACE("dify=%d, difx=%d , nY=%d,nX=%d\n",nDifY,nDifX,nY,nX);
	m_M = ((double)nDifY) / ((double)nDifX);
	m_C = nY - (m_M * nX);
//	TRACE("m=%f,c=%f\n",m_M,m_C);
	if ( abs(nDifX) > abs(nDifY) )
	{
		m_bMoving = MOVEBYX;
		if ( abs(nDifX) <= m_nCardSpeed )
		{
			m_nStepX = nX;
			m_nStepY = nY; 
			MoveWindow(m_hWnd,nX,nY,CARD_WIDTH,CARD_HEIGHT,TRUE);
			m_bMoving = FALSE;
			PostMessage(m_pParentWnd->GetHandle(),WM_PICKCARD_MOVED,
							(WPARAM)m_bRetFlag,(LPARAM)this);
			m_bMovedUp = FALSE;
			return 0;
		}
		else
		{
			// check move left/right
			m_nStepX = (nDifX > 0)? m_nCardSpeed:-m_nCardSpeed;
			m_nStepY = (int)((m_M*(rc.left+m_nStepX))+m_C);
			MoveWindow(m_hWnd,rc.left+m_nStepX,m_nStepY,CARD_WIDTH,CARD_HEIGHT,TRUE);
			// calculate stepY at each timer
			//m_nStepY = (nDifY*m_nStepX)/nDifX;
		}
	}
	else
	{
		m_bMoving = MOVEBYY;
		if ( abs(nDifY) <= m_nCardSpeed )
		{
			m_nStepX = nX;
			m_nStepY = nY; 
			MoveWindow(m_hWnd,nX,nY,CARD_WIDTH,CARD_HEIGHT,TRUE);
			m_bMoving = FALSE;
			PostMessage(m_pParentWnd->GetHandle(),WM_PICKCARD_MOVED,
							(WPARAM)m_bRetFlag,(LPARAM)this);
			m_bMovedUp = FALSE;
			return 0;
		}
		else
		{
			// check move up/down
			m_nStepY = (nDifY > 0)? m_nCardSpeed:-m_nCardSpeed;
			m_nStepX = (int)(((rc.top+m_nStepY)-m_C)/m_M);
			MoveWindow(m_hWnd,m_nStepX,rc.top+m_nStepY,CARD_WIDTH,CARD_HEIGHT,TRUE);
			// calculate StepX at each timer
			//m_nStepX = (nDifX*m_nStepY)/nDifY;
		}
	}

	SetTimer(m_hWnd,CARD_MOVE_TIMERID,CARD_MOVE_TIMER,NULL);
//	TRACE("start MOVE CARD %d\n",m_CardNum);
	return 0;
}

int C_PickCardWnd::FreezeCard(int nFlag)
{
	if ( nFlag )
	{
		// freeze
		m_bFreeze = TRUE;
		KillTimer(m_hWnd,CARD_MOVE_TIMERID);
        MoveWindow(m_hWnd,m_OrgPoint.x,m_OrgPoint.y-25,CARD_WIDTH,CARD_HEIGHT,TRUE);
	}
	else
	{
		// unfreeze
		m_bFreeze = FALSE;
	}
	return 0;
}

///////////////////// Event Handler /////////////////////////
LRESULT C_PickCardWnd::OnMouseMove(WPARAM wParam,LPARAM lParam)
{
	if ( m_bAutoMoveUp && !m_bMoving && !m_bFreeze)
    {
		// let mouse move process at least one time before
		// start moveup , prevent recursive moveingup sometimes
	    if ( m_bMouseAbove && !m_bMovedUp)
    	{
            WORD yPos = HIWORD(lParam);
            if ( yPos < CARD_HEIGHT-22 )
            {
//				TRACE("Start MovingUp %d",m_CardNum);
                RECT rc;
                GetClientRect(m_hWnd,&rc);
                ClientToScreen(m_hWnd,(LPPOINT)&rc.left);
                ScreenToClient(GetParent(m_hWnd),(LPPOINT)&rc.left);
                SetTimer(m_hWnd,CARD_MOVE_TIMERID,CARD_MOVEUP_TIMER,NULL);
                MoveWindow(m_hWnd,rc.left,rc.top-5,CARD_WIDTH,CARD_HEIGHT,TRUE);
                m_OrgPoint.x = rc.left;
                m_OrgPoint.y = rc.top;
                m_bMovedUp = TRUE;
            }
	    }
    }

	LRESULT lRetVal = C_GenericClickWnd::OnMouseMove(wParam,lParam);

    if ( m_bAutoMoveUp && !m_bMoving && !m_bMouseAbove && m_bMovedUp && !m_bFreeze)
    {
//		TRACE("Stop MoveUP %d\n",m_CardNum);
    	KillTimer(m_hWnd,CARD_MOVE_TIMERID);
        MoveWindow(m_hWnd,m_OrgPoint.x,m_OrgPoint.y,CARD_WIDTH,CARD_HEIGHT,TRUE);
        m_bMovedUp = FALSE;
    }
    return lRetVal;
}

LRESULT C_PickCardWnd::OnTimer(WPARAM,LPARAM)
{
	RECT rc;

	if ( m_bMoving )
	{
		// handle timer of Moving Card
		GetWindowRect(m_hWnd,&rc);
		ScreenToClient(GetParent(m_hWnd),(LPPOINT)&rc.left);
		if ( (m_bMoving == MOVEBYX && abs(m_DesPoint.x-rc.left) <= m_nCardSpeed) ||
				(m_bMoving == MOVEBYY && abs(m_DesPoint.y-rc.top) <= m_nCardSpeed) )
		{
			MoveWindow(m_hWnd,m_DesPoint.x,m_DesPoint.y,CARD_WIDTH,CARD_HEIGHT,TRUE);
	    	KillTimer(m_hWnd,CARD_MOVE_TIMERID);
			m_bMoving = FALSE;
			m_bMovedUp = FALSE;
			// parent notify when finish moving
			PostMessage(m_pParentWnd->GetHandle(),WM_PICKCARD_MOVED,
							(WPARAM)m_bRetFlag,(LPARAM)this);
		}
		else
		{
			if ( m_bMoving == MOVEBYX )
			{
				m_nStepY = (int)((m_M*(rc.left+m_nStepX))+m_C);
//				TRACE("moveX ,%d,%d\n",m_nStepX,rc.top+m_nStepY);
				MoveWindow(m_hWnd,rc.left+m_nStepX,m_nStepY,CARD_WIDTH,CARD_HEIGHT,TRUE);
			}
			else
			{
				m_nStepX = (int)(((rc.top+m_nStepY)-m_C)/m_M);
//				TRACE("moveY ,%d,%d\n",m_nStepX,rc.top+m_nStepY);
				MoveWindow(m_hWnd,m_nStepX,rc.top+m_nStepY,CARD_WIDTH,CARD_HEIGHT,TRUE);
			}
//			MoveWindow(m_hWnd,rc.left+m_nStepX,rc.top+m_nStepY,CARD_WIDTH,CARD_HEIGHT,TRUE);
		}
	}
	else if ( m_bMovedUp )
	{
		// handle timer of AutoMoveUp Card
		GetClientRect(m_hWnd,&rc);
		ClientToScreen(m_hWnd,(LPPOINT)&rc.left);
		ScreenToClient(GetParent(m_hWnd),(LPPOINT)&rc.left);

		static int nCount = 0;
		int step[5] = {4,3,2,1,1};
		if ( abs(m_OrgPoint.y-rc.top) > 18 ) // move up height
		{
    		KillTimer(m_hWnd,CARD_MOVE_TIMERID);
			nCount = 0;
//			TRACE("Finish MovingUp %d\n",m_CardNum);
		}
		else
		{
			MoveWindow(m_hWnd,rc.left,rc.top-step[nCount],CARD_WIDTH,CARD_HEIGHT,TRUE);
			nCount++;
			if ( nCount >= 5 ) nCount = 0;
		}
	}
	return 0;
}

