#include "JMComm.h"

#include "resource.h"
#include <stdio.h>
#include "Jubmoo.h"
#include "JubmooWnd.h"
#include "NetStatDlg.h"
#include "StartGameWnd.h"
#include "CardNum.h"
#include "MsgPopupWnd.h"

#define CONNECTION_TIMERID		5002
#define CONNECTING_BLINK_RATE	300
#define JMNETLOG_FILENAME		"JmNet.log"

BEGIN_HANDLE_MSG(C_JMComm,C_MyWinsock)
	HANDLE_MSG(WM_PAINT,OnPaint)
	HANDLE_MSG(WM_LBUTTONUP,OnLButtonUp)
	HANDLE_MSG(WM_RBUTTONUP,OnRButtonUp)
	HANDLE_MSG(WM_ERASEBKGND,OnEraseBkgnd)
	HANDLE_MSG(WM_TIMER,OnTimer)
END_HANDLE_MSG()


C_JMComm::C_JMComm(C_Jubmoo *pJubmoo) : C_MyWinsock()
{
	m_pJubmoo = pJubmoo;
	m_pPlayer = NULL;
    m_bConnecting = FALSE;
    m_pMsgWnd = NULL;
	m_fpNetLog = NULL;
}

C_JMComm::~C_JMComm()
{
	if ( IsWindow(m_hWnd) )
		KillTimer(m_hWnd,CONNECTION_TIMERID);
	delete m_pMsgWnd;
	CloseLogMessage();
}

void C_JMComm::CreateLogMessage()
{
	if ( m_fpNetLog == NULL )
	{
		// open in overwrite mode
		m_fpNetLog = fopen(JMNETLOG_FILENAME,"w+");
	}
}

void C_JMComm::OpenLogMessage()
{
	if ( m_fpNetLog == NULL )
	{
		// open in append mode
		m_fpNetLog = fopen(JMNETLOG_FILENAME,"a+");
	}
}

void C_JMComm::CloseLogMessage()
{
	if ( m_fpNetLog != NULL )
	{
		fclose(m_fpNetLog);
		m_fpNetLog = NULL;
	}
}

void C_JMComm::LogMessage(LPCTSTR lpszFormat, ...)
{
	if ( m_fpNetLog == NULL && m_pParentSock == NULL ) return;

	va_list Args;
	va_start(Args,lpszFormat);

	char szMsgBuf[256];
	vsprintf(szMsgBuf,lpszFormat,Args);
	va_end(Args);

	if ( m_pParentSock != NULL )
    {
    	((C_JMComm*)m_pParentSock)->LogMessage(szMsgBuf);
    }
    else
    {
		fputs(szMsgBuf,m_fpNetLog);
		fflush(m_fpNetLog);
    }
}

BOOL C_JMComm::CreateServerSocket(unsigned short nPortNum)
{
	m_bConnecting = FALSE;
	int nRetVal = C_MyWinsock::CreateServerSocket(nPortNum);
	InvalidateRect(m_hWnd,NULL,FALSE);
	LogMessage("CreateSrvSock(%d)=%d\n",nPortNum,nRetVal);
    return nRetVal;
}

BOOL C_JMComm::CreateClientSocket(unsigned short nPortNum,char *szDotAddr)
{
	m_bConnecting = TRUE;
	SetTimer(m_hWnd,CONNECTION_TIMERID,CONNECTING_BLINK_RATE,NULL);
	int nRetVal = C_MyWinsock::CreateClientSocket(nPortNum,szDotAddr);
	InvalidateRect(m_hWnd,NULL,FALSE);
	LogMessage("CreateCliSock(%d,%s)=%d\n",nPortNum,szDotAddr,nRetVal);
    return nRetVal;
}

int C_JMComm::Close()
{
	CloseAllSocket();
	if ( IsWindow(m_hWnd) )
    {
    	KillTimer(m_hWnd,CONNECTION_TIMERID);
   		InvalidateRect(m_hWnd,NULL,FALSE);
    }
	LogMessage("CloseSock\n");
	return TRUE;
}

void C_JMComm::PopupMessage(LPCTSTR pszMsg)
{
	if ( m_pParentSock == NULL )
	{
		if ( m_pMsgWnd != NULL )
			m_pMsgWnd->PopupMessage(pszMsg);
	}
	else
		((C_JMComm*)m_pParentSock)->PopupMessage(pszMsg);
}

int C_JMComm::CreateMsgWnd()
{
    m_pMsgWnd = new C_MsgPopupWnd(this,5,10000,8000);
	return 0;
}

// ================= Communication API =====================
int C_JMComm::SendLogon(C_Player *pPlayer)
{
	LOGON_MSG Data;
	memset(&Data,0x00,sizeof(LOGON_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_LOGON_MSG;

	strcpy(Data.Name,pPlayer->GetName());
	Data.PlayerID = (unsigned short) pPlayer->GetPlayerID();
	Data.Status = (unsigned char) pPlayer->GetStatus();
	Data.CharacterID = (unsigned char) pPlayer->GetCharacterID();
	Data.NoOfBigChuan = (unsigned short) pPlayer->GetNoOfBigChuan();
	Data.NoOfChuan = (unsigned short) pPlayer->GetNoOfChuan();
	Data.NoOfPig = (unsigned short) pPlayer->GetNoOfPig();
	Data.NoOfWinner = (unsigned short) pPlayer->GetNoOfWinner();
	Data.NoOfGamePlay = (unsigned short) pPlayer->GetNoOfGamePlay();

	SocketSend((char*)&Data,sizeof(LOGON_MSG));
	LogMessage("SendLogon(%s)\n",pPlayer->GetName());
	return 0;
}

int C_JMComm::SendLogonOK(int nPlayerID)
{
	LOGON_OK_MSG Data;
	memset(&Data,0x00,sizeof(LOGON_OK_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_LOGON_OK_MSG;

	Data.PlayerID = (unsigned short) nPlayerID;

	SocketSend((char*)&Data,sizeof(LOGON_OK_MSG));
	LogMessage("SendLogonOK(%d)\n",nPlayerID);
	return 0;
}

int C_JMComm::SendLogonFail(int nErrCode)
{
	LOGON_FAIL_MSG Data;
	memset(&Data,0x00,sizeof(LOGON_FAIL_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_LOGON_FAIL_MSG;

	Data.ErrCode = (unsigned short) nErrCode;

	SocketSend((char*)&Data,sizeof(LOGON_FAIL_MSG));
	LogMessage("SendLogonFail(%d)\n",nErrCode);
	return 0;
}

int C_JMComm::SendChat(int nPlayerID,LPCSTR lpszMsg)
{
	CHAT_MSG Data;
	memset(&Data,0x00,sizeof(CHAT_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_CHAT_MSG;

	Data.PlayerID = (unsigned short) nPlayerID;
    strcpy(Data.ChatMsg,lpszMsg);

	SocketSend((char*)&Data,sizeof(CHAT_MSG));
	LogMessage("SendChat(%d,%s)\n",nPlayerID,lpszMsg);
	return 0;
}

int C_JMComm::SendAddPlayer(C_Player *pPlayer)
{
	ADD_PLAYER_MSG Data;
	memset(&Data,0x00,sizeof(ADD_PLAYER_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_ADD_PLAYER_MSG;

	strcpy(Data.Name,pPlayer->GetName());
	Data.PlayerID = (unsigned short) pPlayer->GetPlayerID();
	Data.Status = (unsigned char) pPlayer->GetStatus();
	Data.CharacterID = (unsigned char) pPlayer->GetCharacterID();
	Data.NoOfBigChuan = (unsigned short) pPlayer->GetNoOfBigChuan();
	Data.NoOfChuan = (unsigned short) pPlayer->GetNoOfChuan();
	Data.NoOfPig = (unsigned short) pPlayer->GetNoOfPig();
	Data.NoOfWinner = (unsigned short) pPlayer->GetNoOfWinner();
	Data.NoOfGamePlay = (unsigned short) pPlayer->GetNoOfGamePlay();

	SocketSend((char*)&Data,sizeof(ADD_PLAYER_MSG));
	LogMessage("SendAddPlayer(%d,%s)\n",pPlayer->GetPlayerID(),pPlayer->GetName());
	return 0;
}

int C_JMComm::SendRemovePlayer(int nPlayerID)
{
	REMOVE_PLAYER_MSG Data;
	memset(&Data,0x00,sizeof(REMOVE_PLAYER_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_REMOVE_PLAYER_MSG;

	Data.PlayerID = (unsigned short)nPlayerID;

	SocketSend((char*)&Data,sizeof(REMOVE_PLAYER_MSG));
	LogMessage("SendRemovePlayer(%d)\n",nPlayerID);
	return 0;
}

int C_JMComm::SendReplacePlayer(int nPlayerID,int nSettingOrder,LPPLAYER_BUFFER pPlayerBuf)
{
	REPLACE_PLAYER_MSG Data;
	memset(&Data,0x00,sizeof(REPLACE_PLAYER_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_REPLACE_PLAYER_MSG;

	Data.PlayerID = (unsigned short)nPlayerID;
	Data.SettingOrder = (unsigned char)nSettingOrder;
	Data.Score = (short)pPlayerBuf->nScore;

    C_Player * p = m_pJubmoo->GetPlayer(Data.PlayerID);
    if ( p == NULL )
    {
    	LogMessage("#Err23\n");
        return 0;
    }
    else
    {
		// get data length
		int i;
		int nLen = 0 ;
		for ( i=0 ; pPlayerBuf->nCard[i]!=0 ; i++ )
			nLen++;
	    Data.PlayersCardLen = (unsigned char)nLen;

		nLen = 0;
		for ( i=0 ; pPlayerBuf->nTakenCard[i]!=0 ; i++ )
			nLen++;
    	Data.PlayersTakenCardLen = (unsigned char)nLen;

	    // short = 2 bytes
    	Data.PlayersScoreLen = (unsigned char)((pPlayerBuf->nHistoryLen)*2);
    }

	// create card buffer
    int j;
	int nBufLen = 0;
    nBufLen += Data.PlayersCardLen;
    nBufLen += Data.PlayersTakenCardLen;
    nBufLen += Data.PlayersScoreLen;
	char * pBuf = new char[nBufLen];
	short * pShort;
	int idx = 0;
    for ( j=0 ; j<Data.PlayersCardLen ; j++ )
    {
        *(pBuf+idx) = (unsigned char)pPlayerBuf->nCard[j];
        idx++;
    }
    for ( j=0 ; j<Data.PlayersTakenCardLen ; j++ )
    {
        *(pBuf+idx) = (unsigned char)pPlayerBuf->nTakenCard[j];
        idx++;
    }
    pShort = (short*)(pBuf+idx);
    for ( j=0 ; j<Data.PlayersScoreLen/2 ; j++ )
    {
        *(pShort+j) = (short)pPlayerBuf->nHistoryScore[j];
        idx+=2;
    }

	char * pSumBuf = new char[sizeof(REPLACE_PLAYER_MSG)+nBufLen];
	memcpy(pSumBuf,&Data,sizeof(REPLACE_PLAYER_MSG));
	memcpy(pSumBuf+sizeof(REPLACE_PLAYER_MSG),pBuf,nBufLen);
	SocketSend(pSumBuf,sizeof(REPLACE_PLAYER_MSG)+nBufLen);
	
//	SocketSend((char*)&Data,sizeof(REPLACE_PLAYER_MSG));
//	SocketSend(pBuf,nBufLen);
	delete[] pBuf;
	delete[] pSumBuf;
	LogMessage("SendReplacePlayer(%d,%d)\n",nPlayerID,nSettingOrder);
	return 0;
}

int C_JMComm::SendSetGame(LPSTART_GAME_SETTING pSetGame)
{
	SET_GAME_MSG Data;
	memset(&Data,0x00,sizeof(SET_GAME_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_SET_GAME_MSG;

	for ( int i=0 ; i<4 ; i++ )
		Data.PlayerID[i] = (unsigned short)pSetGame->nPlayerID[i];
	Data.GameScore = (unsigned short)abs(pSetGame->nGameScore);
	Data.SwapJQ = (unsigned char)pSetGame->bSwapJQ;

	SocketSend((char*)&Data,sizeof(SET_GAME_MSG));
	LogMessage("SendSetGame(%d,%d,%d,%d,%d,%d)\n",
				pSetGame->nPlayerID[0],
				pSetGame->nPlayerID[1],
				pSetGame->nPlayerID[2],
				pSetGame->nPlayerID[3],
				pSetGame->nGameScore,
				pSetGame->bSwapJQ);
	return 0;
}

int C_JMComm::SendStartGame(LPSTART_GAME_SETTING pSetGame)
{
	START_GAME_MSG Data;
	memset(&Data,0x00,sizeof(START_GAME_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_START_GAME_MSG;

	for ( int i=0 ; i<4 ; i++ )
		Data.PlayerID[i] = (unsigned short)pSetGame->nPlayerID[i];
	Data.GameScore = (unsigned short)abs(pSetGame->nGameScore);
	Data.SwapJQ = (unsigned char)pSetGame->bSwapJQ;

	SocketSend((char*)&Data,sizeof(START_GAME_MSG));
	LogMessage("SendStartGame(%d,%d,%d,%d,%d,%d)\n",
				pSetGame->nPlayerID[0],
				pSetGame->nPlayerID[1],
				pSetGame->nPlayerID[2],
				pSetGame->nPlayerID[3],
				pSetGame->nGameScore,
				pSetGame->bSwapJQ);
	return 0;
}

int C_JMComm::SendResetGame()
{
	RESET_GAME_MSG Data;
	memset(&Data,0x00,sizeof(RESET_GAME_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_RESET_GAME_MSG;

	SocketSend((char*)&Data,sizeof(RESET_GAME_MSG));
	LogMessage("SendResetGame\n");
	return 0;
}

int C_JMComm::SendSetSpeed(int nAISpeed,int nPickSpeed)
{
	SET_SPEED_MSG Data;
	memset(&Data,0x00,sizeof(SET_SPEED_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_SET_SPEED_MSG;

	Data.AISpeed = (unsigned short)nAISpeed;
	Data.PickSpeed = (unsigned short)nPickSpeed;

	SocketSend((char*)&Data,sizeof(SET_SPEED_MSG));
	LogMessage("SendSetSpeed(%d,%d)\n",nAISpeed,nPickSpeed);
	return 0;
}

int C_JMComm::SendStartRound()
{
	START_ROUND_MSG Data;
	memset(&Data,0x00,sizeof(START_ROUND_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_START_ROUND_MSG;

	int idx = 0;
	for ( int i=0 ; i<4 ; i++ )
	{
		int nPlayerID = m_pJubmoo->GetGameSetting()->nPlayerID[i];
		if ( nPlayerID == NO_PLAYER )
			continue;
		C_Player * pPlayer = m_pJubmoo->GetPlayer(nPlayerID);
		if ( pPlayer == NULL )
        {
        	LogMessage("#Err21\n");
			continue;
        }
		Data.PlayerID[i] = (unsigned short) nPlayerID;
		for ( int nFace=0 ; nFace<4 ; nFace++ )
		{
			for ( int j=0 ; j<pPlayer->GetCardList(nFace)->Count() ; j++ )
			{
				Data.PlayersCard[idx] = (unsigned char)*(pPlayer->GetCardList(nFace)->Items(j));
				idx++;
			}
		}
	}

	SocketSend((char*)&Data,sizeof(START_ROUND_MSG));
	LogMessage("SendStartRound\n");
	return 0;
}

int C_JMComm::SendEndTurn()
{
	END_TURN_MSG Data;
	memset(&Data,0x00,sizeof(END_TURN_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_END_TURN_MSG;

	SocketSend((char*)&Data,sizeof(END_TURN_MSG));
	LogMessage("SendEndTurn\n");
	return 0;
}

int C_JMComm::SendEndRound()
{
	END_TURN_MSG Data;
	memset(&Data,0x00,sizeof(END_ROUND_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_END_ROUND_MSG;

	SocketSend((char*)&Data,sizeof(END_ROUND_MSG));
	LogMessage("SendEndRound\n");
	return 0;
}

int C_JMComm::SendSetGameData()
{
	SET_GAME_DATA_MSG Data;
	memset(&Data,0x00,sizeof(SET_GAME_DATA_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_SET_GAME_DATA_MSG;

	int i,j;
	// v.1.60 send order of gamesetting instead of pid
	int nOrder = m_pJubmoo->GetActivePlayerOrder()+m_pJubmoo->GetIngameOffset();
	if ( nOrder >=4 ) nOrder-=4;
	Data.ActivePlayerOrder = (unsigned char)nOrder;
	// ===
	Data.LeadedCard = (unsigned char)m_pJubmoo->GetLeadedCard();
	int nOffset;
    for ( i=0 ; i<4 ; i++ )
    {
        nOffset = i - m_pJubmoo->GetIngameOffset();
        if ( nOffset < 0 ) nOffset +=4;
		Data.PlayerID[i] = (unsigned short)m_pJubmoo->GetGameSetting()->nPlayerID[i];
       	Data.DropCardNum[i] = (unsigned char)m_pJubmoo->GetIngameTable()->nCardNum[nOffset];

		C_Player * p = m_pJubmoo->GetPlayer(Data.PlayerID[i]);
		if ( p == NULL )
		{
			Data.Score[i] = 0;
			Data.PlayersCardLen[i] = 0;
			Data.PlayersTakenCardLen[i] = 0;
			Data.PlayersScoreLen[i] = 0;
		}
		else
		{
			Data.Score[i] = (short)p->GetScore();
			Data.PlayersCardLen[i] = (unsigned char)( p->GetCardList(CLUB)->Count()+
														p->GetCardList(DIAMOND)->Count()+
														p->GetCardList(SPADE)->Count()+
														p->GetCardList(HEART)->Count());
			Data.PlayersTakenCardLen[i] = (unsigned char)p->GetTakenCardList()->Count();
			// short = 2 bytes
			Data.PlayersScoreLen[i] = (unsigned char)(p->GetHistoryScore()->Count()*2);
		}
    }
    Data.GameNum = (unsigned short)m_pJubmoo->GetGameNumber();
	Data.RoundNum = (unsigned char)m_pJubmoo->GetRoundNumber();
	Data.TurnNum = (unsigned char)m_pJubmoo->GetTurnNumber();

	// create card buffer
	int nBufLen = 0;
	for ( i=0 ; i<4 ; i++ )
	{
		nBufLen += Data.PlayersCardLen[i];
		nBufLen += Data.PlayersTakenCardLen[i];
		nBufLen += Data.PlayersScoreLen[i];
	}
	char * pBuf = new char[nBufLen];
	short * pShort;
	int idx = 0;
    for ( i=0 ; i<4 ; i++ )
    {
		C_Player * p = m_pJubmoo->GetPlayer(Data.PlayerID[i]);
		if ( p != NULL )
		{
			for ( int nFace=0 ; nFace<4 ; nFace++ )
			{
				for ( j=0 ; j<p->GetCardList(nFace)->Count() ; j++ )
				{
					*(pBuf+idx) = (unsigned char)*(p->GetCardList(nFace)->Items(j));
					idx++;
				}
			}
			for ( j=0 ; j<p->GetTakenCardList()->Count() ; j++ )
			{
				*(pBuf+idx) = (unsigned char)*(p->GetTakenCardList()->Items(j));
				idx++;
			}
			pShort = (short*)(pBuf+idx);
			for ( j=0 ; j<p->GetHistoryScore()->Count() ; j++ )
			{
				*(pShort+j) = (short)*(p->GetHistoryScore()->Items(j));
				idx+=2;
			}
		}
    }

	char * pSumBuf = new char[sizeof(SET_GAME_DATA_MSG)+nBufLen];
	memcpy(pSumBuf,&Data,sizeof(SET_GAME_DATA_MSG));
	memcpy(pSumBuf+sizeof(SET_GAME_DATA_MSG),pBuf,nBufLen);
	SocketSend(pSumBuf,sizeof(SET_GAME_DATA_MSG)+nBufLen);
	
//	SocketSend((char*)&Data,sizeof(SET_GAME_DATA_MSG));
//	SocketSend(pBuf,nBufLen);
	delete[] pBuf;
	delete[] pSumBuf;
	LogMessage("SendSetGameData\n");
	return 0;
}

int C_JMComm::SendDiscard(int nPlayerID,int nCardNum)
{
	DISCARD_MSG Data;
	memset(&Data,0x00,sizeof(DISCARD_MSG));

	Data.Header.Version = MSG_VERSION;
	Data.Header.MsgID = ID_DISCARD_MSG;

	Data.PlayerID = (unsigned short) nPlayerID;
    Data.CardNum = (unsigned char) nCardNum;

	SocketSend((char*)&Data,sizeof(DISCARD_MSG));
	LogMessage("SendDiscard(%d,%d)\n",nPlayerID,nCardNum);
	return 0;
}

// ==============================================================
//         game event handler
// ==============================================================

// Client -> Server
// first logon to server
int C_JMComm::OnLogon()
{
	LOGON_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(LOGON_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(LOGON_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnLogon msglen err %d\n",nRetVal);
		LogMessage("OnLogon msglen err %d\n",nRetVal);
		return FALSE;
	}

	char tmpbuf[50];
    // if no space left for additional player , close this
    if ( m_pJubmoo->GetComm()->GetClientList()->Count() > MAX_NO_OF_PLAYER-1 )
    {
		LogMessage("OnLogon playerfull\n");
		SendLogonFail(PLAYER_FULL_ERR);
		sprintf(tmpbuf,"/c4%s connection being deny",Data.Name);
		PopupMessage(tmpbuf);
		return FALSE;
    }

	sprintf(tmpbuf,"/c4%s come to join",Data.Name);
	PopupMessage(tmpbuf);

    JMPLAYER JmPlayer;
	JmPlayer.nPlayerStatus = Data.Status;
    JmPlayer.nCharacterID = Data.CharacterID;
	strcpy(JmPlayer.szPlayerName,Data.Name);
	JmPlayer.nNoOfChuan = Data.NoOfChuan;
	JmPlayer.nNoOfBigChuan = Data.NoOfBigChuan;
	JmPlayer.nNoOfPig = Data.NoOfPig;
	JmPlayer.nNoOfWinner = Data.NoOfWinner;
	JmPlayer.nNoOfGamePlay = Data.NoOfGamePlay;

    // create player 
	C_Player * pPlayer = m_pJubmoo->AddNewPlayer(&JmPlayer);
	int nPlayerID = pPlayer->GetPlayerID();

	// if there are available space for new player
    // send Player ID back to client
	TRACE("[%s] logon, assigned id %d\n",Data.Name,nPlayerID);
	LogMessage("OnLogon assign %d to %s\n",nPlayerID,Data.Name);

    SendLogonOK(nPlayerID);
	SendSetSpeed(m_pJubmoo->GetUI()->GetAIDelay(),m_pJubmoo->GetUI()->GetPickDelay());

	// also send addplayer to other client
    for ( int i = 0 ; i < m_pJubmoo->GetPlayerList()->Count(); i++ )
    	if ( m_pJubmoo->GetPlayerList()->Items(i)->GetPlayerID() != nPlayerID )
        	SendAddPlayer(m_pJubmoo->GetPlayerList()->Items(i));

	if ( m_pJubmoo->GetPlayMode() == SETTING_PLAYMODE )
		SendSetGame(m_pJubmoo->GetGameSetting());
	else if ( m_pJubmoo->GetPlayMode() == INGAME_PLAYMODE )
	{
		SendStartGame(m_pJubmoo->GetGameSetting());
		// and game data too ......................
		SendSetGameData();
	}

	return TRUE;
}

// Server -> Client
// response of logon msg, return player ID of current game
int C_JMComm::OnLogonOK()
{
	LOGON_OK_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(LOGON_OK_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(LOGON_OK_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnLogonOK msglen err %d\n",nRetVal);
		LogMessage("OnLogonOK msglen err %d\n",nRetVal);
		return FALSE;
	}
	TRACE("logonok pid=%d\n",(int)Data.PlayerID);
	LogMessage("OnLogonOK %d\n",(int)Data.PlayerID);

	// set my Player ID given from server
	m_pJubmoo->GetMyPlayer()->SetPlayerID((int)Data.PlayerID);
	PopupMessage("/c4Connection accepted. enjoy");
	return TRUE;
}

// Server -> Client
// logon fail with errcode
int C_JMComm::OnLogonFail()
{
	LOGON_FAIL_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(LOGON_FAIL_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(LOGON_FAIL_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnLogonFail msglen err %d\n",nRetVal);
		LogMessage("OnLogonFail msglen err %d\n",nRetVal);
		return FALSE;
	}
	TRACE("logonfail ok [%d]\n",(int)Data.ErrCode);
	LogMessage("OnLogonFail %d\n",(int)Data.ErrCode);

	m_pJubmoo->QuitGame();
	switch ( Data.ErrCode )
	{
	case PLAYER_FULL_ERR:
		PopupMessage("/c12JubMoo+ Server is now full");
		break;
	case INVALID_MSGVER_ERR:
		PopupMessage("/c12You uses wrong version with dealer");
		PopupMessage("/c12Get the latest from my homepage");
		break;
	case KICK_OUT:
		PopupMessage("/c12You've been kicked out");
		break;
	}
	return TRUE;
}

// Server -> Client , Client -> Server
// tell about who is speaking and what they speak
int C_JMComm::OnChat()
{
	CHAT_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(CHAT_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(CHAT_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnChat msglen err %d\n",nRetVal);
		LogMessage("OnChat msglen err %d\n",nRetVal);
		return FALSE;
	}
	TRACE("chat ok [%s]\n",(char*)Data.ChatMsg);

	// other player send this msg , do that
	C_Player * pPlayer = m_pJubmoo->GetPlayer((int)Data.PlayerID);
	if ( pPlayer != NULL )
	{
		LogMessage("OnChat %d,%s\n",(int)Data.PlayerID,Data.ChatMsg);
		pPlayer->Chat((char*)Data.ChatMsg);
	}
	else
		LogMessage("OnChat fail togetplayer %d\n",(int)Data.PlayerID);
	return TRUE;
}

// Server -> Client
// Server notify client about player who have to be created on client
int C_JMComm::OnAddPlayer()
{
	ADD_PLAYER_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(ADD_PLAYER_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(ADD_PLAYER_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnAddPlayer msglen err %d\n",nRetVal);
		LogMessage("OnAddPlayer msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("add player [%s]\n",Data.Name);

	JMPLAYER JmPlayer;
	JmPlayer.nPlayerStatus = Data.Status;
	JmPlayer.nCharacterID = Data.CharacterID;
	strcpy(JmPlayer.szPlayerName,Data.Name);
	JmPlayer.nNoOfChuan = Data.NoOfChuan;
	JmPlayer.nNoOfBigChuan = Data.NoOfBigChuan;
	JmPlayer.nNoOfPig = Data.NoOfPig;
	JmPlayer.nNoOfWinner = Data.NoOfWinner;
	JmPlayer.nNoOfGamePlay = Data.NoOfGamePlay;

    // create player with ID
	C_Player * pPlayer = m_pJubmoo->AddNewPlayer(&JmPlayer,Data.PlayerID);
	LogMessage("OnAddPlayer addplayer %d\n",(int)Data.PlayerID);
	return TRUE;
}

// Server -> Client
// Server notify client about player who have to be removed on client
int C_JMComm::OnRemovePlayer()
{
	REMOVE_PLAYER_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(REMOVE_PLAYER_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(REMOVE_PLAYER_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnRemovePlayer msglen err %d\n",nRetVal);
		LogMessage("OnRemovePlayer msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("remove player [%d]\n",(int)Data.PlayerID);

    // remove player
	if ( m_pJubmoo->RemovePlayer((int)Data.PlayerID) )
		LogMessage("OnRemovePlayer %d\n",(int)Data.PlayerID);
	else
		LogMessage("OnRemovePlayer fail togetpid %d\n",(int)Data.PlayerID);

	return TRUE;
}

// Server -> Client
// Server notify client about replacing player on specified location
int C_JMComm::OnReplacePlayer()
{
	REPLACE_PLAYER_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(REPLACE_PLAYER_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(REPLACE_PLAYER_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnReplacePlayer msglen err %d\n",nRetVal);
		LogMessage("OnReplacePlayer msglen err %d\n",nRetVal);
		return FALSE;
	}

	int nCardBufLen = 0;
	int j;
    nCardBufLen += Data.PlayersCardLen;
    nCardBufLen += Data.PlayersTakenCardLen;
    nCardBufLen += Data.PlayersScoreLen;
    TRACE("replace data cardlen[%d],takenlen[%d],scorelen[%d]\n",
            Data.PlayersCardLen,
            Data.PlayersTakenCardLen,
            Data.PlayersScoreLen
            );

	char * pBuf = new char[nCardBufLen];
	nRetVal = 0;
	if ( nCardBufLen > 0 )
	{
		while ( nRetVal == 0 )
			nRetVal = SocketRecv((char*)pBuf,nCardBufLen);
	}
	if ( nRetVal != nCardBufLen )
	{
		TRACE("OnReplacePlayer msglen2 err %d\n",nRetVal);
		LogMessage("OnReplacePlayer msglen2 err %d\n",nRetVal);
		return FALSE;
	}

	// set player's card
	short * pShort;
	int idx = 0;
    int nSettingOrder = (int)Data.SettingOrder;
    LPPLAYER_BUFFER pb = m_pJubmoo->GetPlayerBuffer(nSettingOrder);
    memset(pb,0x00,sizeof(PLAYER_BUFFER));
    pb->nScore = (int)Data.Score;
    for ( j=0 ; j<Data.PlayersCardLen ; j++ )
    {
//        int * nCard = new int( *(pBuf+idx) );
        pb->nCard[j] = *(pBuf+idx);
//        p->GetCardList(GET_FACE(*nCard))->Add(nCard);
        idx++;
    }
    for ( j=0 ; j<Data.PlayersTakenCardLen ; j++ )
    {
//        int * nCard = new int( *(pBuf+idx) );
        pb->nTakenCard[j] = *(pBuf+idx);
//        p->GetTakenCardList()->Add(nCard);
        idx++;
    }
    pShort = (short*)(pBuf+idx);
	pb->nHistoryLen = Data.PlayersScoreLen/2;
    for ( j=0 ; j<Data.PlayersScoreLen/2 ; j++ )
    {
//        int * nScore = new int( *(pShort+j) );
        pb->nHistoryScore[j] = *(pShort+j);
//        p->GetHistoryScore()->Add(nScore);
        idx+=2;
    }

	C_Player * pPlayer = m_pJubmoo->GetPlayer((int)Data.PlayerID);
	if ( pPlayer != NULL )
	{
		char tmpbuf[64];
		sprintf(tmpbuf,"/c4%s join current game",pPlayer->GetName());
		PopupMessage(tmpbuf);
	}
	else
	{
		TRACE("Replace fail###\n");
		LogMessage("#err replace fail\n");
	}

	TRACE("replace player [%d,%d]\n",(int)Data.PlayerID,(int)Data.SettingOrder);
    // replace player
	if ( m_pJubmoo->ReplacePlayer((int)Data.PlayerID,(int)Data.SettingOrder) )
		LogMessage("OnReplacePlayer %d\n",(int)Data.PlayerID);
	else
		LogMessage("OnReplacePlayer fail pid %d\n",(int)Data.PlayerID);

	return TRUE;
}

// Server -> Client
// Server notify client that is now setting up game, information also
int C_JMComm::OnSetGame()
{
	SET_GAME_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(SET_GAME_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(SET_GAME_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnSetGame msglen err %d\n",nRetVal);
		LogMessage("OnSetGame msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("Set game [%d] swap[%d]\n",-((int)Data.GameScore),(int)Data.SwapJQ);

    // setting game
	START_GAME_SETTING GameSetting;
	for ( int i=0 ; i<4 ; i++ )
		GameSetting.nPlayerID[i] = Data.PlayerID[i];
	GameSetting.nGameScore = -((int)Data.GameScore);
	GameSetting.bSwapJQ = Data.SwapJQ;

	// this message intend to update config changing to all clients.
	m_pJubmoo->UpdateSetting(&GameSetting);
	LogMessage("OnSetGame %d,%d\n",(int)Data.GameScore,(int)Data.SwapJQ);
	return TRUE;
}

// Server -> Client
// Server notify client about info of start game setting
int C_JMComm::OnStartGame()
{
	SET_GAME_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(START_GAME_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(START_GAME_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnStartGame msglen err %d\n",nRetVal);
		LogMessage("OnStartGame msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("start game\n");

    // start game
	START_GAME_SETTING GameSetting;
	for ( int i=0 ; i<4 ; i++ )
		GameSetting.nPlayerID[i] = Data.PlayerID[i];
	GameSetting.nGameScore = -((int)Data.GameScore);
	GameSetting.bSwapJQ = Data.SwapJQ;
	
	m_pJubmoo->UpdateSetting(&GameSetting);
	m_pJubmoo->StartGame();
	PopupMessage("/c4Start!");
	LogMessage("OnStartGame %d,%d,%d,%d,%d,%d\n",
				(int)Data.PlayerID[0],
				(int)Data.PlayerID[1],
				(int)Data.PlayerID[2],
				(int)Data.PlayerID[3],
				(int)Data.GameScore,
				(int)Data.SwapJQ);
	return TRUE;
}

// Server -> Client
// Server notify client that is now setting up game, information also
int C_JMComm::OnResetGame()
{
	RESET_GAME_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(RESET_GAME_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(RESET_GAME_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnResetGame msglen err %d\n",nRetVal);
		LogMessage("OnResetGame msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("reset game\n");

	m_pJubmoo->ResetGame();
	PopupMessage("Game Reset");
	LogMessage("OnResetGame\n");
	return TRUE;
}

// Server -> Client
// Server notify client about game speed
int C_JMComm::OnSetSpeed()
{
	SET_SPEED_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(SET_SPEED_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(SET_SPEED_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnSetSpeed msglen err %d\n",nRetVal);
		LogMessage("OnSetSpeed msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("set speed of game [%d][%d]\n",(int)Data.AISpeed,(int)Data.PickSpeed);

	m_pJubmoo->GetUI()->SetGameSpeed((int)Data.AISpeed,(int)Data.PickSpeed);
	PopupMessage("/c4Dealer set game speed");
	LogMessage("OnSetSpeed %d,%d\n",(int)Data.AISpeed,(int)Data.PickSpeed);
	return TRUE;
}

// Server -> Client
// Server send start round data , cards of each player
int C_JMComm::OnStartRound()
{
	START_ROUND_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(START_ROUND_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(START_ROUND_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnStartRound msglen err %d\n",nRetVal);
		LogMessage("OnStartRound msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("start round ok\n");

	int idx = 0;
	for ( int i=0 ; i<4 ; i++ )
	{
		int nPlayerID = (int)Data.PlayerID[i];
		if ( nPlayerID == NO_PLAYER )
			continue;
		C_Player * pPlayer = m_pJubmoo->GetPlayer(nPlayerID);
		if ( pPlayer == NULL )
        {
        	LogMessage("#Err22\n");
			continue;
        }
		for ( int nFace=0 ; nFace<4 ; nFace++ )
       		pPlayer->GetCardList(nFace)->DeleteObjects();
		pPlayer->GetTakenCardList()->DeleteObjects();
		for ( int j=0 ; j<13 ; j++ )
		{
            int * nCard = new int( (int)Data.PlayersCard[idx] );
        	pPlayer->GetCardList(GET_FACE(*nCard))->Add(nCard);
			idx++;
		}
	}

    m_pJubmoo->StartRound();
	LogMessage("OnStartRound\n");
	return TRUE;
}

// Server -> Client
// Server notify client to end turn 
int C_JMComm::OnEndTurn()
{
	END_TURN_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(END_TURN_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(END_TURN_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnEndTurn msglen err %d\n",nRetVal);
		LogMessage("OnEndTurn msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("end turn\n");

	m_pJubmoo->EndTurn();
	LogMessage("OnEndTurn\n");
	return TRUE;
}

// Server -> Client
// Server notify client to end turn 
int C_JMComm::OnEndRound()
{
	END_ROUND_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(END_ROUND_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(END_ROUND_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnEndRound msglen err %d\n",nRetVal);
		LogMessage("OnEndRound msglen err %d\n",nRetVal);
		return FALSE;
	}

	TRACE("end round\n");

	m_pJubmoo->EndRound();
	LogMessage("OnEndRound\n");
	return TRUE;
}

// Server -> Client
// Server send current game data
int C_JMComm::OnSetGameData()
{
	SET_GAME_DATA_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(SET_GAME_DATA_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(SET_GAME_DATA_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnSetGameData msglen err %d\n",nRetVal);
		LogMessage("OnSetGameData msglen err %d\n",nRetVal);
		return FALSE;
	}

	int nCardBufLen = 0;
	int i,j;
	for ( i=0 ; i<4 ; i++ )
	{
		nCardBufLen += Data.PlayersCardLen[i];
		nCardBufLen += Data.PlayersTakenCardLen[i];
		nCardBufLen += Data.PlayersScoreLen[i];
		TRACE("set game data %d cardlen[%d],takenlen[%d],scorelen[%d]\n",
				i,Data.PlayersCardLen[i],
				Data.PlayersTakenCardLen[i],
				Data.PlayersScoreLen[i]
				);
	}

	char * pBuf = new char[nCardBufLen];
	nRetVal = 0;
	if ( nCardBufLen > 0 )
	{
		while ( nRetVal == 0 )
			nRetVal = SocketRecv((char*)pBuf,nCardBufLen);
	}
	if ( nRetVal != nCardBufLen )
	{
		TRACE("OnSetGameData msglen2 err %d\n",nRetVal);
		LogMessage("OnSetGameData msglen2 err %d\n",nRetVal);
		return FALSE;
	}
	TRACE("set game data ok\n");

	// set player's card
	short * pShort;
	int idx = 0;
	for ( i=0 ; i<4 ; i++ )
	{
		C_Player * p = m_pJubmoo->GetPlayer(Data.PlayerID[i]);
		if ( p != NULL )
		{
			p->SetScore( (int)Data.Score[i] );
			for ( j=0 ; j<Data.PlayersCardLen[i] ; j++ )
			{
				int * nCard = new int( *(pBuf+idx) );
				p->GetCardList(GET_FACE(*nCard))->Add(nCard);
				idx++;
			}
			for ( j=0 ; j<Data.PlayersTakenCardLen[i] ; j++ )
			{
				int * nCard = new int( *(pBuf+idx) );
				p->GetTakenCardList()->Add(nCard);
				idx++;
			}
			pShort = (short*)(pBuf+idx);
			for ( j=0 ; j<Data.PlayersScoreLen[i]/2 ; j++ )
			{
				int * nScore = new int( *(pShort+j) );
				p->GetHistoryScore()->Add(nScore);
				idx+=2;
			}
		}
	}

	// set game data
	m_pJubmoo->SetLeadedCard((int)Data.LeadedCard);

	// v.1.60 fix order setting
	m_pJubmoo->SetActivePlayerOrder(Data.ActivePlayerOrder);


	m_pJubmoo->SetGameNumber((int)Data.GameNum);
	m_pJubmoo->SetRoundNumber((int)Data.RoundNum);
	m_pJubmoo->SetTurnNumber((int)Data.TurnNum);

	// update Screen
	for ( i=0 ; i<4 ; i++ )
	{
		m_pJubmoo->GetUI()->RefreshPlayersCard(i);
	}

	// set drop card to screen
	int nDropCardCount = 0;
	for ( i=0 ; i<4 ; i++ )
	{
		if ( Data.DropCardNum[i] == 0 ) continue;
		nDropCardCount++;
	}

	if ( m_pJubmoo->GetLeadedCard() != NO_LEADED )
	{
		for ( i=0 ; i<4 ; i++ )
		{
			// set drop card , because ingameoffset 'll be 0 here
			// so just put it
			m_pJubmoo->GetIngameTable()->nCardNum[i] = Data.DropCardNum[i];
			if ( m_pJubmoo->GetIngameTable()->nCardNum[i] == 0 )
				continue;
/*            for ( j=0 ; j<4 ; j++ )
            {
	        	if ( Data.PlayerID[i] == m_pJubmoo->GetIngameTable()->nPlayerID[j] )
                {
                	// assume must be found only
                	m_pJubmoo->GetIngameTable()->nCardNum[j] = Data.DropCardNum[i];
                    break;
                }
            }*/
			// create drop card only in turn no 0,1,2 
			// turn 3 will just wait for end turn
			if ( nDropCardCount < 4 )
			{
				m_pJubmoo->GetUI()->Discard(i,m_pJubmoo->GetIngameTable()->nCardNum[i],
											SUDDEN_DISCARDMODE);
			}
		}
	}

	LogMessage("OnSetGameData\n");
	return TRUE;
}

// Server -> Client , Client -> Server
// Discard action
int C_JMComm::OnDiscard()
{
	DISCARD_MSG Data;
	int nRetVal;

	nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
							sizeof(DISCARD_MSG)-sizeof(HEADER_MSG));

	if ( nRetVal != sizeof(DISCARD_MSG)-sizeof(HEADER_MSG) )
	{
		TRACE("OnDiscard msglen err %d\n",nRetVal);
		LogMessage("OnDiscard msglen err %d\n",nRetVal);
		return FALSE;
	}
	TRACE("discard ok pid[%d],card[%d]\n",(int)Data.PlayerID,(int)Data.CardNum);

	// other player send this msg , do that
	C_Player * pPlayer = m_pJubmoo->GetPlayer((int)Data.PlayerID);
	if ( pPlayer != NULL )
	{
		if ( m_pJubmoo->GetPlayMode() == INGAME_PLAYMODE )
		{
			pPlayer->Discard((int)Data.CardNum);
			LogMessage("OnDiscard %d,%d\n",(int)Data.PlayerID,(int)Data.CardNum);
		}
		else
			LogMessage("#err com#01");
	}
	else
		LogMessage("OnDiscard fail togetpid %d\n",(int)Data.PlayerID);

	return TRUE;
}


// ===========================================================
//  base communication event handler
// ===========================================================
void C_JMComm::OnAccept(int nErrCode)
{
	if ( nErrCode == WSAENETDOWN )
	{
		TRACE("## Error On Accept\n");
		return;
	}

	TRACE("Incoming Connection\n");
	// child create
	C_JMComm * pActSock = new C_JMComm(m_pJubmoo);
	RECT rc = {0,0,10,10};
	pActSock->Create("",0,WS_CHILD,
						rc,(C_Wnd*)m_pJubmoo->GetUI(),this);

	if ( AcceptSocket(pActSock) )
	{
		// accept ok
		// checking connection from the same ip
		//
		// continue if yes
		TRACE("   @:oP~~ ready to send/receive data\n");
	}
	else
	{
		delete pActSock;
		TRACE("  @:o(  can't accept\n");
	}
}

void C_JMComm::OnConnect(int nErrCode)
{
    m_bConnecting = FALSE;
	KillTimer(m_hWnd,CONNECTION_TIMERID);
	switch ( nErrCode )
	{
	case WSAEADDRINUSE:		//The specified address is already in use.
	case WSAEADDRNOTAVAIL:	//The specified address is not available from the local machine.
	case WSAEAFNOSUPPORT:	//Addresses in the specified family cannot be used with this socket.
	case WSAECONNREFUSED:	//The attempt to connect was forcefully rejected.
	case WSAENETUNREACH:	//The network cannot be reached from this host at this time.
	case WSAEFAULT:			//The namelen argument is incorrect.
	case WSAEINVAL:			//The socket is already bound to an address.
	case WSAEISCONN:		//The socket is already connected.
	case WSAEMFILE:			//No more file descriptors are available.
	case WSAENOBUFS:		//No buffer space is available. The socket cannot be connected.
	case WSAENOTCONN:		//The socket is not connected.
	case WSAETIMEDOUT:		//Attempt to connect timed out without establishing a connection.
		{
		char tmpbuf[50];
		TRACE("error on connect\n");
		InvalidateRect(m_hWnd,NULL,FALSE);
        // now command quit game 'll blink wnd by unnecessary.
		m_pJubmoo->QuitGame();
//		Close();
		sprintf(tmpbuf,"/c12Can't join to dealer [err#%d]",nErrCode);
		PopupMessage(tmpbuf);
		}
		return;
	}
	TRACE("   @:oP~~ Connection ready to send/receive data\n");

	// connect to server OK , send my player's info.
    InvalidateRect(m_hWnd,NULL,FALSE);
	SendLogon(m_pJubmoo->GetMyPlayer());
}

void C_JMComm::OnRecv(int nErrCode)
{
	if ( nErrCode == WSAENETDOWN )
	{
		TRACE("Error On Recv\n");
		return;
	}

	HEADER_MSG Header;
	memset(&Header,0x00,sizeof(HEADER_MSG));
	int nRetVal;

	nRetVal = SocketRecv((char*)&Header,sizeof(HEADER_MSG));
	// recv unspecify msg length
	if ( nRetVal != sizeof(HEADER_MSG) ) return;

	TRACE("header %04x ,msg %d  | ",Header.Version,Header.MsgID);

	if ( Header.Version == MSG_VERSION )
	{
		switch ( Header.MsgID )
		{
		case ID_LOGON_MSG: OnLogon(); break;
		case ID_LOGON_OK_MSG: OnLogonOK(); break;
		case ID_LOGON_FAIL_MSG: OnLogonFail(); break;
		case ID_CHAT_MSG: OnChat(); break;
		case ID_ADD_PLAYER_MSG: OnAddPlayer(); break;
		case ID_REMOVE_PLAYER_MSG: OnRemovePlayer(); break;
		case ID_REPLACE_PLAYER_MSG: OnReplacePlayer(); break;
		case ID_SET_GAME_MSG: OnSetGame(); break;
		case ID_START_GAME_MSG: OnStartGame(); break;
		case ID_RESET_GAME_MSG: OnResetGame(); break;
		case ID_SET_SPEED_MSG: OnSetSpeed(); break;
		case ID_START_ROUND_MSG: OnStartRound(); break;
		case ID_END_TURN_MSG: OnEndTurn(); break;
		case ID_END_ROUND_MSG: OnEndRound(); break;
		case ID_SET_GAME_DATA_MSG: OnSetGameData(); break;
		case ID_DISCARD_MSG: OnDiscard(); break;
		default:
			break;
		}
	}
	else
	{
		// message version mismatch handler
		if ( Header.MsgID == ID_LOGON_MSG )
		{
            LOGON_MSG Data;
            int nRetVal;

            nRetVal = SocketRecv((char*)&Data + sizeof(HEADER_MSG),
                                sizeof(LOGON_MSG)-sizeof(HEADER_MSG));
			// version number mismatch.

			char tmpbuf[50];
			sprintf(tmpbuf,"/c12%s's version mismatch",Data.Name);
			PopupMessage(tmpbuf);
			sprintf(tmpbuf,"/c12%s connection being deny",Data.Name);
			PopupMessage(tmpbuf);
			TRACE("deny incoming connect ** version mismatch **\n");

            {
            	// send logonfail with the same version of incoming connection
            	LOGON_FAIL_MSG Data;
                memset(&Data,0x00,sizeof(LOGON_FAIL_MSG));

                Data.Header.Version = Header.Version;
                Data.Header.MsgID = ID_LOGON_FAIL_MSG;

                Data.ErrCode = (unsigned short) INVALID_MSGVER_ERR;

                SocketSend((char*)&Data,sizeof(LOGON_FAIL_MSG));
            }
		}
	}

}

void C_JMComm::OnSend(int nErrCode)
{
	if ( nErrCode == WSAENETDOWN )
	{
		TRACE("Error On Send\n");
		return;
	}
}

void C_JMComm::OnClose(int nErrCode)
{
	switch ( nErrCode )
	{
	case WSAENETDOWN:		//The network subsystem has failed.
	case WSAECONNRESET:		//The connection was reset by the remote side.
	case WSAECONNABORTED:	//The connection was terminated due to a time-out or other failure.
		TRACE("Error On Close [%d]\n",nErrCode);
		CloseSocket();
		break;
	}

	KillTimer(m_hWnd,CONNECTION_TIMERID);
	m_bConnecting = FALSE;
	m_fdSock = INVALID_SOCKET;
    if ( IsWindow(m_hWnd) ) InvalidateRect(m_hWnd,NULL,FALSE);
	if ( m_pParentSock == NULL )
	{
		// parent socket , go back to idle mode
		// both server and client mode
//        MessageBox(m_hWnd,"close event","",MB_OK);
		CloseSocket();
	    if ( IsWindow(m_hWnd) ) InvalidateRect(m_hWnd,NULL,FALSE);
		TRACE("Close Server Socket\n");
		m_pJubmoo->QuitGame();
		PopupMessage("/c4Quit from the game");
	}
	else
	{
		// passive close
		CloseSocket();
	    if ( IsWindow(m_hWnd) ) InvalidateRect(m_hWnd,NULL,FALSE);
		// child socket only in server mode
		// remove this player out
		TRACE("Close Client Socket\n");
		if ( m_pPlayer != NULL )
		{
			char tmpbuf[50];
			sprintf(tmpbuf,"/c4%s quit. bye",m_pPlayer->GetName());
			PopupMessage(tmpbuf);
			m_pJubmoo->RemovePlayer(m_pPlayer->GetPlayerID());
		}

		// this child socket is automaticly deleted by parent socket
	}
}

// ===================================================
// Event handler
// ===================================================
LRESULT C_JMComm::OnPaint(WPARAM ,LPARAM )
{
	static int bBlink = TRUE;
	PAINTSTRUCT ps;
	m_hDC = BeginPaint( m_hWnd, &ps );

	if ( m_fdSock == INVALID_SOCKET )
		DrawIcon(m_hDC,0,0,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_JMOFFLINE)));
	else
	{
		if ( m_bConnecting )
		{
			if ( bBlink )
				DrawIcon(m_hDC,0,0,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_JMCONNECT)));
			else
				DrawIcon(m_hDC,0,0,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_JMCONNECT1)));
			bBlink = !bBlink;
		}
		else
			DrawIcon(m_hDC,0,0,LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_JMONLINE)));
	}
	EndPaint( m_hWnd, &ps );
	return 0;
}

LRESULT C_JMComm::OnLButtonUp(WPARAM ,LPARAM )
{
	PopupMessage(NULL);
	return 0;
}

LRESULT C_JMComm::OnRButtonUp(WPARAM ,LPARAM )
{
	C_NetStatDlg NetStatDlg(this);
	NetStatDlg.DoModal();
	return 0;
}

LRESULT C_JMComm::OnEraseBkgnd(WPARAM ,LPARAM )
{
	return 1;
}

LRESULT C_JMComm::OnTimer(WPARAM ,LPARAM )
{
/*	RECT rc;
	GetClientRect(m_hWnd,&rc);
	ClientToScreen(m_hWnd,(LPPOINT)&rc.left);
	ClientToScreen(m_hWnd,(LPPOINT)&rc.right);
	ScreenToClient(GetParent(m_hWnd),(LPPOINT)&rc.left);
	ScreenToClient(GetParent(m_hWnd),(LPPOINT)&rc.right);
	InvalidateRect(GetParent(m_hWnd),&rc,FALSE);
	*/
	InvalidateRect(m_hWnd,NULL,FALSE);
	return 0;
}
