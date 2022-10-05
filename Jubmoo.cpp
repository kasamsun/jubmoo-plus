#include "Jubmoo.h"

#include <stdio.h>
#include "CardNum.h"
#include "resource.h"
#include "PlayerWnd.h"
#include "JubmooWnd.h"
#include "JMComm.h"
#include "JMKnowledgeBase.h"
#include "kswafx/Dialog.h"
#include "JMPing.h"

#include "CommonAI.h"


extern C_JubmooApp JubmooApp;

C_Jubmoo::C_Jubmoo()
{
	m_nGameMode = IDLE_GAMEMODE;
    m_nPlayMode = IDLE_PLAYMODE;
    m_pMyPlayer = NULL;

	// default game setting
	for ( int i=0 ; i<4 ; i++ )
	{
		m_IngameTable.nPlayerID[i] = NO_PLAYER;
        m_IngameTable.nCardNum[i] = 0;

		m_GameSetting.nPlayerID[i] = NO_PLAYER;
	}
	m_GameSetting.nGameScore = -1000;
	m_GameSetting.bSwapJQ = FALSE;

	m_nGameNum = 0;
	m_nRoundNum = 0;
	m_nTurnNum = 0;
	m_nActivePlayerOrder = 0;
	m_nLeadedCard = NO_LEADED;
	m_nIngameOffset = 0;

	m_nRunningPlayerID = MYID;
	// create UI Object
    m_pJubmooWnd = new C_JubmooWnd(this);
    // create knowledgebase object
    m_pKnowledgeBase = new C_JMKnowledgeBase(this);
    // create communication object
	m_pComm = new C_JMComm(this);
   	RECT rc = {560,465,592,497};
    m_pComm->Create("",0,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
    					rc,m_pJubmooWnd,NULL);
	m_pComm->CreateMsgWnd();

	m_pPing = new C_JMPing(this);

	memset(m_PlayerBuffer,0x00,sizeof(PLAYER_BUFFER)*4);
}

C_Jubmoo::~C_Jubmoo()
{
	GetPlayerList()->DeleteObjects();
	delete m_pJubmooWnd;
    delete m_pKnowledgeBase;
    delete m_pComm;
    delete m_pPing;
}

int C_Jubmoo::Initialize()
{
    // create last player
    JMPLAYER JmPlayer;

	UINT nLastPlayer;
	UINT nGamePlay,nWin,nPig,nBigChuan,nChuan;

	char szName[32];
	memset(szName,0x00,sizeof(szName));

	// get last player from registry 
	JubmooApp.OpenRegistry();
	nLastPlayer = JubmooApp.GetPlayerSlot();
	JmPlayer.nPlayerStatus = IAM_SPECTATOR;
    JmPlayer.nCharacterID = (int)JubmooApp.GetCharID(nLastPlayer);
	JubmooApp.GetName(nLastPlayer,JmPlayer.szPlayerName,sizeof(JmPlayer.szPlayerName));
	JubmooApp.GetStat(nLastPlayer,nGamePlay,nWin,nPig,nBigChuan,nChuan);
	JmPlayer.nNoOfGamePlay = nGamePlay;
	JmPlayer.nNoOfWinner = nWin;
	JmPlayer.nNoOfPig = nPig;
	JmPlayer.nNoOfBigChuan = nBigChuan;
	JmPlayer.nNoOfChuan = nChuan;

	if ( JubmooApp.GetUseNetLog() )
		m_pComm->CreateLogMessage();
	
	JubmooApp.CloseRegistry();

	AddMyPlayer(&JmPlayer);

	// main communicating object bind to MyPlayer
	m_pComm->SetPlayer(GetMyPlayer());

    // update UI object only when finish initializing
	ShowWindow(m_pJubmooWnd->GetHandle(),SW_SHOW);
	UpdateWindow(m_pJubmooWnd->GetHandle());
	return TRUE;
}

void C_Jubmoo::SetPlayMode(int nMode)
{
	if ( nMode == m_nPlayMode ) return;
	int nLastPlayMode = m_nPlayMode;
	m_nPlayMode = nMode;

	switch( nMode )
	{
	case IDLE_PLAYMODE:
		GetUI()->SetImage();
		break;
	case INGAME_PLAYMODE:
	case SETTING_PLAYMODE:
	case ENDGAME_PLAYMODE:
		if ( nLastPlayMode == IDLE_PLAYMODE )
			GetUI()->SetImage();
		break;
	}
}

int C_Jubmoo::StartServer()
{
	m_nGameMode = SERVER_GAMEMODE;
	// reset player running id
	m_nRunningPlayerID = GetMyPlayer()->GetPlayerID();

	// get port to activate
    DWORD dwPort;

	JubmooApp.OpenRegistry();
	dwPort = (DWORD)JubmooApp.GetPort();
	JubmooApp.CloseRegistry();

    return m_pComm->CreateServerSocket((unsigned short)dwPort);
}

int C_Jubmoo::StartClient(char * lpszJoinIP)
{
	m_nGameMode = CLIENT_GAMEMODE;
	// reset player running id
	m_nRunningPlayerID = GetMyPlayer()->GetPlayerID();

	// get Port for connecting
    DWORD dwPort;

	JubmooApp.OpenRegistry();
	dwPort = (DWORD)JubmooApp.GetPort();
	JubmooApp.CloseRegistry();

	if ( m_pComm->CreateClientSocket((unsigned short)dwPort,lpszJoinIP) )
		return TRUE;
	else
	{
		m_nGameMode = IDLE_GAMEMODE;
		return FALSE;
	}
}

int C_Jubmoo::QuitGame()
{
	LockWindowUpdate(m_pJubmooWnd->GetHandle());

	m_nGameMode = IDLE_GAMEMODE;
	m_pComm->Close();
	m_pJubmooWnd->ShowIdleGame();
	SetPlayMode(IDLE_PLAYMODE);

	C_Player *pPlayer;
	int i;

	// remove all other player
	for ( i=0 ; i<GetPlayerList()->Count() ; i++ )
	{
		pPlayer = GetPlayerList()->Items(i);
		if ( GetMyPlayer()->GetPlayerID() != pPlayer->GetPlayerID() )
		{
			RemovePlayer(pPlayer->GetPlayerID());
			i--;
		}
		else
		{
			if ( pPlayer->GetStatus() == IAM_PLAYER )
			{
				m_pJubmooWnd->ClearPlayerUI(pPlayer->GetTableOrder());
                pPlayer->SetTableOrder(NOT_IN_TABLE);
				pPlayer->SetStatus(IAM_SPECTATOR);
			}
		}
	}

	// clear player setting
	m_nLeadedCard = NO_LEADED;
	for ( i=0 ; i<4 ; i++ )
    {
		m_GameSetting.nPlayerID[i] = NO_PLAYER;
        m_IngameTable.nPlayerID[i] = NO_PLAYER;
        m_IngameTable.nCardNum[i] = 0;
    }

	// v.1.60 clear my card too
	for ( i=0 ; i<4 ; i++ )
	{
		GetMyPlayer()->GetCardList(i)->DeleteObjects();
	}
	GetMyPlayer()->GetHistoryScore()->DeleteObjects();
	GetMyPlayer()->GetTakenCardList()->DeleteObjects();
	// =====

	LockWindowUpdate(NULL);
	return 0;
}

int C_Jubmoo::ResetGame()
{
	LockWindowUpdate(m_pJubmooWnd->GetHandle());
	// resetting game
	SetPlayMode(SETTING_PLAYMODE);

	C_Player *pPlayer;
	int i;

	// send set game message to all client
    if ( m_nGameMode == SERVER_GAMEMODE )
    {
		// checking client list and player list, remove unknown player
		for ( i=0 ; i < GetPlayerList()->Count() ; i++ )
		{
			C_Player * pPlayer = GetPlayerList()->Items(i);
			if ( pPlayer->GetPlayerID() == GetMyPlayer()->GetPlayerID() )
				continue;
			if ( pPlayer->GetStatus() == IAM_COMPUTER )
				continue;
			int j;
			C_Player * pLostPlayer;
			for ( j=0; j < m_pComm->GetClientList()->Count() ; j++ )
			{
				pLostPlayer = ((C_JMComm *)m_pComm->GetClientList()->Items(j))->GetPlayer();
				if ( pLostPlayer->GetPlayerID() == pPlayer->GetPlayerID() )
					break;
			}
			if ( j == m_pComm->GetClientList()->Count() )
			{
				// player not found
				RemovePlayer(pLostPlayer->GetPlayerID());
			}
		}

    	// send msg to other players
        for ( i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
        {
            C_JMComm * pJMComm = (C_JMComm *)m_pComm->GetClientList()->Items(i);
            pJMComm->SendResetGame();
        }
    }

	// remove computer player , set player back to spectator

	// clear table
	for ( i=0 ; i<4 ; i++ )
	{
		int pid = GetGameSetting()->nPlayerID[i];
		if ( pid == NO_PLAYER ) continue;
		pPlayer = GetPlayer(pid);
        if ( pPlayer == NULL )
        {
        	m_pComm->LogMessage("#Err01\n");
            continue;
        }
        int nPos = pPlayer->GetTableOrder();
		switch ( pPlayer->GetStatus() )
		{
		case IAM_COMPUTER:
//			TRACE("clear UI %s %d,%d\n",pPlayer->GetName(),j,pPlayer->GetPlayerID());
            // change setting to empty
           	m_GameSetting.nPlayerID[i] = NO_PLAYER;
            m_IngameTable.nPlayerID[pPlayer->GetTableOrder()] = NO_PLAYER;
			m_pJubmooWnd->ClearPlayerUI(nPos);
			break;
		case IAM_PLAYER:
//			TRACE("clear UI %s %d,%d\n",pPlayer->GetName(),j,pPlayer->GetPlayerID());
			pPlayer->SetStatus(IAM_SPECTATOR);
			m_pJubmooWnd->ClearPlayerUI(nPos);
			break;
		}
		pPlayer->SetTableOrder(NOT_IN_TABLE);
	}

	// remove computer player
	for ( i=0 ; i<GetPlayerList()->Count() ; i++ )
	{
		pPlayer = GetPlayerList()->Items(i);
		if ( pPlayer->GetStatus() == IAM_COMPUTER )
		{
//			TRACE("del %s %d,%d\n",pPlayer->GetName(),i,pPlayer->GetPlayerID());
			pPlayer->SetUI(NULL);
			delete pPlayer->GetJubmooAI();
			pPlayer->SetJubmooAI(NULL);
			delete pPlayer;
			GetPlayerList()->Remove(i);
			i--;
			continue;
		}
	}

	// clear table setting
	for ( i=0 ; i<4 ; i++ )
    {
        m_IngameTable.nPlayerID[i] = NO_PLAYER;
        m_IngameTable.nCardNum[i] = 0;
    }

	GetUI()->ShowSettingGame(&m_GameSetting);

	LockWindowUpdate(NULL);
	return TRUE;
}

int C_Jubmoo::StartGame()
{
	LockWindowUpdate(m_pJubmooWnd->GetHandle());

	// enter ingame playmode
	SetPlayMode(INGAME_PLAYMODE);

	int i;

	// send start game message to all client
    if ( m_nGameMode == SERVER_GAMEMODE )
    {
		// generate computer player at empty slot
		JMPLAYER AIPlayer;

		JubmooApp.OpenRegistry();
		int nComputerID = RANDOM(4)+1;
		for ( i=0 ; i<4 ; i++ )
		{
			// 0 specify that is empty and ready to add ai player
			if ( m_GameSetting.nPlayerID[i] == NO_PLAYER )
			{
				nComputerID++; if ( nComputerID > 4 ) nComputerID = 1;
				AIPlayer.nPlayerStatus = IAM_COMPUTER;
				JubmooApp.GetOpponent(nComputerID,AIPlayer.szPlayerName,sizeof(AIPlayer.szPlayerName));
				// AI player res id is always start with 128;
				AIPlayer.nCharacterID = 127+nComputerID;
				UINT c,bc,p,w,g;
				JubmooApp.GetOpponentStat(nComputerID,g,w,p,bc,c);
				AIPlayer.nNoOfChuan = c;
				AIPlayer.nNoOfBigChuan = bc;
				AIPlayer.nNoOfPig = p;
				AIPlayer.nNoOfWinner = w;
				AIPlayer.nNoOfGamePlay = g;
				C_Player * pNewPlayer = AddNewPlayer(&AIPlayer);
				m_GameSetting.nPlayerID[i] = pNewPlayer->GetPlayerID();
				pNewPlayer->SetJubmooAI( new C_CommonAI(pNewPlayer) );
			}
		}
		JubmooApp.CloseRegistry();

		// send msg to other players
        for ( i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
        {
            C_JMComm * pJMComm = (C_JMComm *)m_pComm->GetClientList()->Items(i);
			pJMComm->SendStartGame(&m_GameSetting);
        }
    }

	// clear player data
	C_Player * pPlayer;
	for ( i=0 ; i<GetPlayerList()->Count() ; i++ )
	{
		pPlayer = GetPlayerList()->Items(i);
		pPlayer->SetScore(0);
		pPlayer->GetHistoryScore()->DeleteObjects();
	}

	// set player order , and start game
	m_nGameNum = 0;
    int nFirstOrder = 0;
	m_nActivePlayerOrder = 0;

	for ( i=0 ; i<4 ; i++ )
	{
		if ( m_GameSetting.nPlayerID[i] == GetMyPlayer()->GetPlayerID() )
		{
        	nFirstOrder = i;
			break;
		}
	}
    m_nActivePlayerOrder -= nFirstOrder;
	m_nIngameOffset = nFirstOrder;
    if ( m_nActivePlayerOrder<0 ) m_nActivePlayerOrder+=4;

    for ( i=0 ; i<4 ; i++ )
    {
		C_Player * pPlayer = GetPlayer(m_GameSetting.nPlayerID[nFirstOrder]);
        m_IngameTable.nPlayerID[i] = m_GameSetting.nPlayerID[nFirstOrder];
        m_IngameTable.nCardNum[i] = 0;
        nFirstOrder++ ; if ( nFirstOrder == 4 ) nFirstOrder = 0;
		if ( pPlayer == NULL )
        {
        	m_pComm->LogMessage("#Err02\n");
	        m_IngameTable.nPlayerID[i] = NO_PLAYER;
            continue;
        }
	    pPlayer->SetTableOrder(i);
    }

	m_pJubmooWnd->ShowStartGame();

    // server continue start game, client just wait for start round
    if ( m_nGameMode == SERVER_GAMEMODE )
		StartRound();

	LockWindowUpdate(NULL);
	return TRUE;
}

int C_Jubmoo::StartRound()
{
	int i;

	// v.1.60 add checking all players
	for ( i=0 ; i<4 ; i++ )
	{
		C_Player * pPlayer = GetPlayer(m_GameSetting.nPlayerID[i]);
		if ( pPlayer == NULL )
		{
			// if this player has quited, just wait , try end turn later
			m_pComm->LogMessage("#Err04\n");
			GetUI()->WaitForLostPlayer(CONTINUE_STARTROUND);
    		return FALSE; // player may leave , so check it
		}
	}

	m_nRoundNum = 0;

	// send gamedata round message to all client
    if ( m_nGameMode == SERVER_GAMEMODE )
    {
    	// clear cards
        for ( i=0 ; i<GetPlayerList()->Count() ; i++ )
        {
            C_Player * pPlayer = GetPlayerList()->Items(i);
			for ( int nFace=0 ; nFace<4 ; nFace++ )
				pPlayer->GetCardList(nFace)->DeleteObjects();
            pPlayer->GetTakenCardList()->DeleteObjects();
        }

		// checking still have 4 players in game
		for ( i=0 ; i<4 ; i++ )
		{
			C_Player * pPlayer = GetPlayer(m_GameSetting.nPlayerID[i]);
			if ( pPlayer == NULL )
			{
	        	m_pComm->LogMessage("#Err03\n");
				// update player's card on screen
				for ( i=0 ; i<4 ; i++ )
					m_pJubmooWnd->RefreshPlayersCard(i);
				// don't continue.
				return FALSE;
			}
		}

		// random distribute card to all players
		if ( !DistributeCard() )
        {
        	m_pComm->LogMessage("#Err07\n");
            return FALSE;
        }
    	// send msg to other players
        for ( i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
        {
            C_JMComm * pJMComm = (C_JMComm *)m_pComm->GetClientList()->Items(i);
			pJMComm->SendStartRound();
        }
	}

	if ( JubmooApp.IsUseSound() )
		PlaySound(MAKEINTRESOURCE(IDR_SND_SHUFFLE),
				C_App::m_hInstance,
				SND_RESOURCE|SND_ASYNC);

	// update player's card on screen
	for ( i=0 ; i<4 ; i++ )
	{
		m_pJubmooWnd->RefreshPlayersCard(i);
		m_pJubmooWnd->RefreshPlayersTable(i);
	}

	// create card if my player is being player
	if ( GetMyPlayer()->GetStatus() == IAM_PLAYER )
	{
		GetUI()->SetCardOnHand();
	}

    // reset knowledgebase
    m_pKnowledgeBase->Reset();

	StartTurn(0);
	return TRUE;
}

int C_Jubmoo::StartTurn(int nTurnNum)
{
	m_nTurnNum = nTurnNum;
	if ( nTurnNum == 0 )
	{
	    // clear dropped card data
		m_nLeadedCard = NO_LEADED;
		for ( int i=0 ; i<4 ; i++ )
		    m_IngameTable.nCardNum[i] = 0;
	}

	m_pJubmooWnd->RequestDiscard();
	return 0;
}

int C_Jubmoo::NextTurn()
{
	static int nFirstOrder = 0;

    if ( m_nTurnNum == 0 )
    {
        // set leaded face
        m_nLeadedCard = m_IngameTable.nCardNum[m_nActivePlayerOrder];
		nFirstOrder = m_nActivePlayerOrder;
    }

	m_nTurnNum++;
	m_nActivePlayerOrder++;
    if ( m_nActivePlayerOrder == 4 ) m_nActivePlayerOrder = 0;

	// if end turn start turn
	if ( m_nTurnNum == 4 )
    {
		// check for who pick card , then set first player on the next turn
        PickUpCard(nFirstOrder);
    }
    else
    {
		// do next turn
    	StartTurn(m_nTurnNum);
    }
	return TRUE;
}

int C_Jubmoo::EndTurn()
{	
	int i;
	C_Player * pPlayer;

	// v.1.60 add checking all players
	for ( i=0 ; i<4 ; i++ )
	{
		pPlayer = GetPlayer(m_GameSetting.nPlayerID[i]);
		if ( pPlayer == NULL )
		{
			// if this player has quited, just wait , try end turn later
			m_pComm->LogMessage("#Err04\n");
			GetUI()->WaitForLostPlayer(CONTINUE_ENDTURN);
    		return FALSE; // player may leave , so check it
		}
	}

/*	pPlayer = GetPlayer(m_IngameTable.nPlayerID[m_nActivePlayerOrder]);
	if ( pPlayer == NULL )
    {
		// if this player has quited, just wait , try end turn later
		m_pComm->LogMessage("#Err04\n");
		GetUI()->WaitForLostPlayer();
    	return FALSE; // player may leave , so check it
    }*/
	
	pPlayer = GetPlayer(m_IngameTable.nPlayerID[m_nActivePlayerOrder]);

	// send message to all client here
    for ( i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
    {
        C_JMComm * pJMComm = (C_JMComm *)m_pComm->GetClientList()->Items(i);
		pJMComm->SendEndTurn();
    }
	
	// add dropcard to takencard of activeplayer
	int k;
	int bUpdateFlag = FALSE;
	for ( i=0 ; i<4 ; i++ )
	{
		int * nCardNum;
		switch ( m_IngameTable.nCardNum[i] )
		{
		case CLUB_10: case DIAMOND_J: case SPADE_Q:
		case HEART_2: case HEART_3: case HEART_4:
		case HEART_5: case HEART_6: case HEART_7:
		case HEART_8: case HEART_9: case HEART_10:
		case HEART_J: case HEART_Q: case HEART_K: case HEART_A:
			nCardNum = new int( m_IngameTable.nCardNum[i] );
			bUpdateFlag = TRUE;
			for ( k=0 ; k < pPlayer->GetTakenCardList()->Count() ; k++ )
			{
				if ( *nCardNum < *(pPlayer->GetTakenCardList()->Items(k)) )
				{
					pPlayer->GetTakenCardList()->Insert(k,nCardNum);
					break;
				}
			}
			// add at last position
			if ( k == pPlayer->GetTakenCardList()->Count() )
				pPlayer->GetTakenCardList()->Add(nCardNum);
			break;
		}
	}

	if ( bUpdateFlag )
	{
		GetUI()->RefreshPlayersCard(m_nActivePlayerOrder);
		GetUI()->RefreshPlayersTable(m_nActivePlayerOrder);
		if ( pPlayer->GetPlayerID() == GetMyPlayer()->GetPlayerID() )
			GetUI()->RefreshMyCardList();
	}
	
	// update knowledgebase
    m_pKnowledgeBase->OnTakenCard(GetActivePlayerID());


	// check end round and end game
	m_nRoundNum++;
	if ( m_nRoundNum == 13 )
	{
		GetUI()->ShowEndRound();
		// server continue , client wait for endround msg
		if ( GetGameMode() == SERVER_GAMEMODE )
			EndRound();
		return TRUE;
	}

	// if not , start new turn
	StartTurn(0);
	return TRUE;
}

int C_Jubmoo::EndRound()
{
	int i;
	// send message to all client here
    for ( i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
    {
        C_JMComm * pJMComm = (C_JMComm *)m_pComm->GetClientList()->Items(i);
		pJMComm->SendEndRound();
    }

	// update player score
	int bIsEndGame = FALSE;
	int bIsAnyChuan = FALSE;
	for ( i=0 ; i<4 ; i++ )
	{
		C_Player * pPlayer = GetPlayer(m_IngameTable.nPlayerID[i]);
		if ( pPlayer == NULL )
        {
        	m_pComm->LogMessage("#Err05\n");
			continue;
        }
		int nScore = CalculateScore(pPlayer->GetTakenCardList());
		int * pScore = new int(nScore);
		pPlayer->SetScore(pPlayer->GetScore()+nScore);
		pPlayer->GetHistoryScore()->Add(pScore);

		// if chuan , update data
		if ( IsBigChuan(pPlayer->GetTakenCardList()) )
		{
			pPlayer->SetNoOfBigChuan(pPlayer->GetNoOfBigChuan()+1);
			if ( pPlayer->GetPlayerID() == GetMyPlayer()->GetPlayerID() )
				bIsAnyChuan = TRUE;
		}
		else if ( IsChuan(pPlayer->GetTakenCardList()) )
		{
			pPlayer->SetNoOfChuan(pPlayer->GetNoOfChuan()+1);
			if ( pPlayer->GetPlayerID() == GetMyPlayer()->GetPlayerID() )
				bIsAnyChuan = TRUE;
		}
		if ( pPlayer->GetScore() <= m_GameSetting.nGameScore )
			bIsEndGame = TRUE;

		// update player screen
		m_pJubmooWnd->RefreshPlayersTable(i);
	}

	// if my player chuan , update data 
	if ( bIsAnyChuan )
	{
		// update my player data to registry
		JubmooApp.OpenRegistry();
		JubmooApp.SetStat(JubmooApp.GetPlayerSlot(),
							GetMyPlayer()->GetNoOfGamePlay(),
							GetMyPlayer()->GetNoOfWinner(),
							GetMyPlayer()->GetNoOfPig(),
							GetMyPlayer()->GetNoOfBigChuan(),
							GetMyPlayer()->GetNoOfChuan());
		JubmooApp.CloseRegistry();
	}

	// end game reach
	if ( bIsEndGame )
	{
		EndGame();
	}
	else
	{
		m_nGameNum++;
		// refresh UI's dialog , if it's opened
		C_Dialog * pJmDialog = m_pJubmooWnd->GetJmDialog();
		if ( pJmDialog != NULL ) pJmDialog->Refresh();
		// server continue , client wait for startround msg
		if ( GetGameMode() == SERVER_GAMEMODE )
			StartRound();
	}
	return TRUE;
}

int C_Jubmoo::EndGame()
{
	int i;
	C_Player * pWinPlayer;
	int nHighestScore = 0;
	for ( i=0 ; i<4 ; i++ )
	{
		C_Player * pPlayer = GetPlayer(m_IngameTable.nPlayerID[i]);
		if ( pPlayer == NULL )
        {
        	m_pComm->LogMessage("#Err06\n");
        	continue;
        }

		// first reference player
		if ( i==0 )
		{
			nHighestScore = pPlayer->GetScore();
			pWinPlayer = pPlayer;
		}

		if ( pPlayer->GetScore() > nHighestScore )
		{
			nHighestScore = pPlayer->GetScore();
			pWinPlayer = pPlayer;
		}

		// if pig , update player's data
		if ( pPlayer->GetScore() <= m_GameSetting.nGameScore )
			pPlayer->SetNoOfPig(pPlayer->GetNoOfPig()+1);
		// update no of game play
		pPlayer->SetNoOfGamePlay(pPlayer->GetNoOfGamePlay()+1);

		// clear all cards on hand
		for ( int nFace=0 ; nFace<4 ; nFace++ )
			pPlayer->GetCardList(nFace)->DeleteObjects();
		pPlayer->GetTakenCardList()->DeleteObjects();
	}
	// win player must not be pig .
	if ( pWinPlayer->GetScore() > m_GameSetting.nGameScore )
		pWinPlayer->SetNoOfWinner(pWinPlayer->GetNoOfWinner()+1);

	for ( i=0 ; i<4 ; i++ )
		m_pJubmooWnd->RefreshPlayersCard(i);
	// show end screen
	SetPlayMode(ENDGAME_PLAYMODE);
	GetUI()->ShowEndGame();
	
	// update my player data to registry
	JubmooApp.OpenRegistry();
	JubmooApp.SetStat(JubmooApp.GetPlayerSlot(),
						GetMyPlayer()->GetNoOfGamePlay(),
						GetMyPlayer()->GetNoOfWinner(),
						GetMyPlayer()->GetNoOfPig(),
						GetMyPlayer()->GetNoOfBigChuan(),
						GetMyPlayer()->GetNoOfChuan());
	// update computer player data
	if ( m_nGameMode == SERVER_GAMEMODE )
	{
		for ( int i=0 ; i<4 ; i++ )
		{
			C_Player * pPlayer = GetPlayer(m_IngameTable.nPlayerID[i]);
			if ( pPlayer != NULL )
			{
				if ( pPlayer->GetStatus() == IAM_COMPUTER )
					JubmooApp.SetOpponentStat(pPlayer->GetCharacterID()-127,
						pPlayer->GetNoOfGamePlay(),
						pPlayer->GetNoOfWinner(),
						pPlayer->GetNoOfPig(),
						pPlayer->GetNoOfBigChuan(),
						pPlayer->GetNoOfChuan());
			}
		}
	}

	JubmooApp.CloseRegistry();
	// then do nothing

	return TRUE;
}

int C_Jubmoo::PickUpCard(int nFirstOrder)
{
	int nBiggestOrder = nFirstOrder;
	int nNextOrder = nFirstOrder;

	nNextOrder++; if ( nNextOrder == 4 ) nNextOrder = 0;

	for ( int i=1 ; i<4 ; i++ )
	{
		if ( GET_FACE(m_IngameTable.nCardNum[nNextOrder]) == GET_FACE(m_nLeadedCard) )
			if ( m_IngameTable.nCardNum[nNextOrder] >
					m_IngameTable.nCardNum[nBiggestOrder] )
				nBiggestOrder = nNextOrder;
		nNextOrder++; if ( nNextOrder == 4 ) nNextOrder = 0;
	}

	m_nActivePlayerOrder = nBiggestOrder;

	TRACE("biggest order = %d\n",nBiggestOrder);
	// UI->pickupcard will break jubmoo engine for awhile until picking finish
	m_pJubmooWnd->PickUpCard(nBiggestOrder);


	return TRUE;
}

int C_Jubmoo::Discard(int nPlayerID,int nCardNum)
{
	m_IngameTable.nCardNum[m_nActivePlayerOrder] = nCardNum;

    // dispatch this action to other player here
    if ( m_nGameMode == SERVER_GAMEMODE )
    {
    	// send msg to every other player , except owner
        for ( int i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
        {
        	// if server discard , send to all client
            C_JMComm * pJMComm = (C_JMComm *)m_pComm->GetClientList()->Items(i);
        	if ( GetMyPlayer()->GetPlayerID() == nPlayerID )
            {
				// send discard
	    		pJMComm->SendDiscard(nPlayerID,nCardNum);
            }
            else
            {
            	// if others discard , send to all other client , except owner
				if ( pJMComm->GetPlayer() != NULL )
					if ( pJMComm->GetPlayer()->GetPlayerID() != nPlayerID )
			    		pJMComm->SendDiscard(nPlayerID,nCardNum);
            }
        }
    }
    // action that can use by client must check ID to prevent recursive
    else if ( m_nGameMode == CLIENT_GAMEMODE && nPlayerID == m_pMyPlayer->GetPlayerID() )
    {
		// client send discard to server
    	m_pComm->SendDiscard(nPlayerID,nCardNum);
    }

	// update knowledgebase
    m_pKnowledgeBase->OnDiscard(nPlayerID,nCardNum);

	NextTurn();
	return TRUE;
}

int C_Jubmoo::Chat(int nPlayerID,char *pszMsg)
{
    // dispatch this action to other player here
    if ( m_nGameMode == SERVER_GAMEMODE )
    {
    	// send msg to every other player , except owner
        for ( int i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
        {
        	// if server chat , send to all client
            C_JMComm * pJMComm = (C_JMComm *)m_pComm->GetClientList()->Items(i);
        	if ( GetMyPlayer()->GetPlayerID() == nPlayerID )
            {
	    		pJMComm->SendChat(nPlayerID,pszMsg);
            }
            else
            {
            	// if others chat , send to all other client , except owner
				if ( pJMComm->GetPlayer() != NULL )
					if ( pJMComm->GetPlayer()->GetPlayerID() != nPlayerID )
			    		pJMComm->SendChat(nPlayerID,pszMsg);
            }
        }
    }
    // action that can use by client must check ID to prevent recursive
    else if ( m_nGameMode == CLIENT_GAMEMODE && nPlayerID == m_pMyPlayer->GetPlayerID() )
    {
    	m_pComm->SendChat(nPlayerID,pszMsg);
    }
	return TRUE;
}

// ==============================================
//  remark : Player ID of my player  ** alway 
//			 start from MYID , MYID 'll be lowest possible ID
//           if server, other client start id from MYID+1
//			 if client, my id upon what server give
// ==============================================
int C_Jubmoo::AddMyPlayer(LPJMPLAYER PlayerInfo)
{
    m_pMyPlayer = new C_Player(this,MYID,PlayerInfo);
	GetPlayerList()->Add(m_pMyPlayer);

    // create UI of my player
    m_pJubmooWnd->AddSpectatorUI(m_pMyPlayer);
	return 0;
}

// ==============================================
//  parameter : nPlayerID default is -1
//  return : Player ID of this player
// ==============================================
C_Player * C_Jubmoo::AddNewPlayer(LPJMPLAYER PlayerInfo,int nPlayerID)
{
	m_nRunningPlayerID++;

    C_Player * pPlayer;
    if ( nPlayerID < 0 )
    	// server manipulate running player id
	    pPlayer = new C_Player(this,m_nRunningPlayerID,PlayerInfo);
    else
    	// create player with player ID
	    pPlayer = new C_Player(this,nPlayerID,PlayerInfo);
	GetPlayerList()->Add(pPlayer);

    // create UI of player
	if ( PlayerInfo->nPlayerStatus != IAM_COMPUTER )
	{
	    LockWindowUpdate(m_pJubmooWnd->GetHandle());
		m_pJubmooWnd->AddSpectatorUI(pPlayer);
		LockWindowUpdate(NULL);
	}

	// dispatch message to other client
    if ( m_nGameMode == SERVER_GAMEMODE )
    {
    	// send msg to every other player , except owner
        for ( int i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
        {
        	// if server do , send to all client
            C_JMComm * pJMComm = (C_JMComm *)m_pComm->GetClientList()->Items(i);
        	if ( GetMyPlayer()->GetPlayerID() == nPlayerID )
            {
	    		pJMComm->SendAddPlayer(pPlayer);
            }
            else
            {
				// if new player log on ,JMComm.m_pPlayer is still NULL, so set it also
				if ( pJMComm->GetPlayer() == NULL )
				{
					pJMComm->SetPlayer(pPlayer);
				}
            	// if others do , send to all other client , except owner
				if ( pJMComm->GetPlayer()->GetPlayerID() != pPlayer->GetPlayerID() )
		    		pJMComm->SendAddPlayer(pPlayer);
            }
        }
    }

	return pPlayer;
}

// ===============================================
//  return : TRUE - Remove successful
//           FALSE - Remove fail
// ===============================================
int C_Jubmoo::RemovePlayer(int nPlayerID)
{
	for ( int i = 0 ; i<GetPlayerList()->Count() ; i++ )
	{
		if ( GetPlayerList()->Items(i)->GetPlayerID() == nPlayerID )
		{
        	// remove process
			C_Player * p = GetPlayerList()->Items(i);
			// remove game setting flag if he is player
            for ( int k=0 ; k<4 ; k++ )
                if ( m_GameSetting.nPlayerID[k] == p->GetPlayerID() )
                {
					// save last data of quit player
					// clear table
					int nTableOrder = p->GetTableOrder();
                    m_GameSetting.nPlayerID[k] = NO_PLAYER;
					// v.1.60 fix , nTableOrder may be -1 
					if ( nTableOrder >= 0 )
						m_IngameTable.nPlayerID[nTableOrder] = NO_PLAYER;
					memset(&m_PlayerBuffer[k],0x00,sizeof(PLAYER_BUFFER));
					m_PlayerBuffer[k].nScore = p->GetScore();
					int j;
					int offset = 0;
                    for ( int nFace=0 ; nFace<4 ; nFace++ )
					{
						for ( j=0 ; j<p->GetCardList(nFace)->Count() ; j++ )
							m_PlayerBuffer[k].nCard[offset++] = *(p->GetCardList(nFace)->Items(j));
                        p->GetCardList(nFace)->DeleteObjects();
					}
					for ( j=0 ; j<p->GetTakenCardList()->Count() ; j++ )
						m_PlayerBuffer[k].nTakenCard[j] = *(p->GetTakenCardList()->Items(j));
                    p->GetTakenCardList()->DeleteObjects();
					for ( j=0 ; j<p->GetHistoryScore()->Count() ; j++ )
						m_PlayerBuffer[k].nHistoryScore[j] = *(p->GetHistoryScore()->Items(j));
					m_PlayerBuffer[k].nHistoryLen = p->GetHistoryScore()->Count();
                    break;
                }

		    LockWindowUpdate(m_pJubmooWnd->GetHandle());
			GetUI()->RemoveSpectatorUI(p);
			delete p;
			GetPlayerList()->Remove(i);

		    LockWindowUpdate(NULL);

			// dispatch message to other client
			if ( m_nGameMode == SERVER_GAMEMODE )
			{
    			// send msg to every other player , except owner
				for ( int i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
				{
        			// if server do , send to all client
					C_JMComm * pJMComm = (C_JMComm *)m_pComm->GetClientList()->Items(i);
        			if ( GetMyPlayer()->GetPlayerID() == nPlayerID )
					{
	    				pJMComm->SendRemovePlayer(nPlayerID);
					}
					else
					{
            			// if others do , send to all other client , except owner
						if ( pJMComm->GetPlayer()->GetPlayerID() != nPlayerID )
		    				pJMComm->SendRemovePlayer(nPlayerID);
					}
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

int C_Jubmoo::ReplacePlayer(int nPlayerID,int nSettingOrder)
{
	// check condition

	// dispatch message to other client
	if ( m_nGameMode == SERVER_GAMEMODE )
	{
    	// send msg to every other player
		for ( int i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
		{
			C_JMComm * pJMComm = (C_JMComm *)m_pComm->GetClientList()->Items(i);
			pJMComm->SendReplacePlayer(nPlayerID,nSettingOrder,&m_PlayerBuffer[nSettingOrder]);
		}
	}

	// do replace
	C_Player *pPlayer = GetPlayer(nPlayerID);

	if ( pPlayer == NULL ) return FALSE;

	for ( int nFace=0 ; nFace<4 ; nFace++ )
		if ( pPlayer->GetCardList(nFace)->Count() > 0 )
			pPlayer->GetCardList(nFace)->DeleteObjects();
	if ( pPlayer->GetTakenCardList()->Count() > 0 )
		pPlayer->GetTakenCardList()->DeleteObjects();
	if ( pPlayer->GetHistoryScore()->Count() > 0 )
		pPlayer->GetHistoryScore()->DeleteObjects();

	pPlayer->SetScore(m_PlayerBuffer[nSettingOrder].nScore);
	int i;

	for ( i=0; m_PlayerBuffer[nSettingOrder].nCard[i] != 0 ; i++ )
	{
		int *nCard = new int( m_PlayerBuffer[nSettingOrder].nCard[i] );
		pPlayer->GetCardList(GET_FACE(*nCard))->Add(nCard);
	}
	for ( i=0; m_PlayerBuffer[nSettingOrder].nTakenCard[i] != 0 ; i++ )
	{
		int *nCard = new int( m_PlayerBuffer[nSettingOrder].nTakenCard[i] );
		pPlayer->GetTakenCardList()->Add(nCard);
	}
	for ( i=0; i<m_PlayerBuffer[nSettingOrder].nHistoryLen ; i++ )
	{
		int *nScore = new int( m_PlayerBuffer[nSettingOrder].nHistoryScore[i] );
		pPlayer->GetHistoryScore()->Add(nScore);
	}

	m_GameSetting.nPlayerID[nSettingOrder] = nPlayerID;
	// change to screen position
	int nOrder = nSettingOrder - m_nIngameOffset;
	if ( nOrder < 0 ) nOrder += 4;
	m_IngameTable.nPlayerID[nOrder] = nPlayerID;
	pPlayer->SetTableOrder(nOrder);

	// update UI
	if ( GetMyPlayer()->GetPlayerID() == nPlayerID )
	{
		// clear table
		int i;
		for ( i=0 ; i<4 ; i++ )
		{
			m_pJubmooWnd->ClearPlayerUI(i);
		}
		// set player order , and start game
		int nFirstOrder = 0;

		for ( i=0 ; i<4 ; i++ )
		{
			if ( m_GameSetting.nPlayerID[i] == GetMyPlayer()->GetPlayerID() )
			{
        		nFirstOrder = i;
				break;
			}
		}
		m_nIngameOffset = nFirstOrder;
		// set active player order here
		// old ingameoffset is always 0 ( because it's spectator )
		// so set new activeplayerorder by new offset
		m_nActivePlayerOrder -= m_nIngameOffset;
		if ( m_nActivePlayerOrder<0 ) m_nActivePlayerOrder+=4;
		// =============================
		// save last drop card
		int nCardNumTmp[4];
		for ( i=0 ; i<4 ; i++ )
			nCardNumTmp[i] = m_IngameTable.nCardNum[i];
		for ( i=0 ; i<4 ; i++ )
		{
			// fill in Ingametable data
			C_Player * pPlayer = GetPlayer(m_GameSetting.nPlayerID[nFirstOrder]);
			m_IngameTable.nPlayerID[i] = m_GameSetting.nPlayerID[nFirstOrder];
			m_IngameTable.nCardNum[i] = nCardNumTmp[nFirstOrder];
			nFirstOrder++ ; if ( nFirstOrder == 4 ) nFirstOrder = 0;
			if ( pPlayer == NULL )
			{
        		m_pComm->LogMessage("#Err02\n");
				m_IngameTable.nPlayerID[i] = NO_PLAYER;
				continue;
			}
			pPlayer->SetTableOrder(i);
		}

		m_pJubmooWnd->ShowStartGame();
		m_pJubmooWnd->SetCardOnHand();
		if ( m_nIngameOffset > 0 )
        {
        	for ( i=0 ; i<4 ; i++ )
	        	m_pJubmooWnd->RefreshPlayersCard(i);
			m_pJubmooWnd->ShiftDropCard(-m_nIngameOffset);
        }
		m_pJubmooWnd->RequestDiscard();
	}
	else
	{
		m_pJubmooWnd->ShowReplacedPlayer(nOrder);
	}

	return TRUE;
}


int C_Jubmoo::UpdateSetting(LPSTART_GAME_SETTING pGameSetting)
{
	for ( int i=0 ; i<4 ; i++ )
		m_GameSetting.nPlayerID[i] = pGameSetting->nPlayerID[i];
	m_GameSetting.nGameScore = pGameSetting->nGameScore;
	m_GameSetting.bSwapJQ = pGameSetting->bSwapJQ;

	if ( m_nGameMode ==	SERVER_GAMEMODE )
	{
		// server only can make change setting
		// just send msg to every other clientr
		for ( int i=0 ; i < m_pComm->GetClientList()->Count() ; i++ )
		{
			TRACE("send setting to client\n");
			C_JMComm * pJMComm = (C_JMComm*)m_pComm->GetClientList()->Items(i);
    		pJMComm->SendSetGame(&m_GameSetting);
		}
	}
	else if ( m_nGameMode == CLIENT_GAMEMODE )
	{
		// client get setting from server and just do it.
		GetUI()->ShowSettingGame(pGameSetting);
	}
	return 0;
}

C_Player * C_Jubmoo::GetPlayer(int nPlayerID)
{
	for ( int i = 0 ; i < GetPlayerList()->Count() ; i++ )
	{
		if ( GetPlayerList()->Items(i)->GetPlayerID() == nPlayerID )
			return GetPlayerList()->Items(i);
	}
	return NULL;
}

int C_Jubmoo::DistributeCard()
{
	// distribute card to all players
	// all players must have no card
	// all players must have at least a heart card
	CARD_LIST card;

	int i,j;

	for ( i=1 ; i<=61 ; i++ )
	{
		if ( i==14 ) i=17; if ( i==30 ) i=33;
		if ( i==46 ) i=49;
		int * c = new int(i);
		card.Add(c);
	}

	// random pick heart card to each player
	for ( i=0 ; i<4 ; i++ )
	{
		j = RANDOM(card.Count()-40)+40;
        C_Player * pPlayer = GetPlayer(m_GameSetting.nPlayerID[i]);
        if ( pPlayer == NULL )
            return FALSE;
        pPlayer->GetCardList(GET_FACE(*(card.Items(j))))->Add(card.Items(j));
		card.Remove(j);
	}
	// random other cards
	for ( i=0 ; i<4 ; i++ )
	{
		C_Player * pPlayer = GetPlayer(m_GameSetting.nPlayerID[i]);
        if ( pPlayer == NULL )
            return FALSE;

		for  ( int n=0 ; n<12 ; n++ )
		{
			// random 12 card to player
			j = RANDOM( card.Count() );
			int nFace = GET_FACE(*(card.Items(j)));
            int k;
			for ( k=0 ; k < pPlayer->GetCardList(nFace)->Count() ; k++ )
			{
				if ( *(card.Items(j)) < *(pPlayer->GetCardList(nFace)->Items(k)) )
				{
					pPlayer->GetCardList(nFace)->Insert(k,card.Items(j));
					card.Remove(j);
					break;
				}
			}
			if ( k == pPlayer->GetCardList(nFace)->Count() )
			{
				pPlayer->GetCardList(nFace)->Add(card.Items(j));
				card.Remove(j);
			}
		}
	}

	card.Clear();
	return TRUE;
}

int C_Jubmoo::IsChuan(CARD_LIST *pCardList)
{
	int nCardCount = 0;
	for ( int i=0 ; i<pCardList->Count() ; i++ )
	{
		if ( GET_FACE(*(pCardList->Items(i))) == HEART )
			nCardCount++;
	}
	if ( nCardCount == 13 ) return TRUE;
	return FALSE;
}

int C_Jubmoo::IsBigChuan(CARD_LIST *pCardList)
{
	// card list must contain only score card
	if ( pCardList->Count() == 16 )
		return TRUE;
	return FALSE;
}

int C_Jubmoo::CalculateScore(CARD_LIST *pCardList)
{
    // ==================== Restriction ==========================
    //  1. Don't have duplicated card
    // ===========================================================

    BOOL b10ClubCard = FALSE;
    BOOL bJDiamondCard = FALSE;
    BOOL bQSpadeCard = FALSE;
    int nNoOfHeart = 0;
    int nHeartScore = 0;

    for ( int i=0 ; i< pCardList->Count() ; i++ )
    {
        switch ( (CARDNUM)*(pCardList->Items(i)) )
        {
            case HEART_2:
                nNoOfHeart++;nHeartScore+=HEART_2_VAL;break;
            case HEART_3:
                nNoOfHeart++;nHeartScore+=HEART_3_VAL;break;
            case HEART_4:
                nNoOfHeart++;nHeartScore+=HEART_4_VAL;break;
            case HEART_5:
                nNoOfHeart++;nHeartScore+=HEART_5_VAL;break;
            case HEART_6:
                nNoOfHeart++;nHeartScore+=HEART_6_VAL;break;
            case HEART_7:
                nNoOfHeart++;nHeartScore+=HEART_7_VAL;break;
            case HEART_8:
                nNoOfHeart++;nHeartScore+=HEART_8_VAL;break;
            case HEART_9:
                nNoOfHeart++;nHeartScore+=HEART_9_VAL;break;
            case HEART_10:
                nNoOfHeart++;nHeartScore+=HEART_10_VAL;break;
            case HEART_J:
                nNoOfHeart++;nHeartScore+=HEART_J_VAL;break;
            case HEART_Q:
                nNoOfHeart++;nHeartScore+=HEART_Q_VAL;break;
            case HEART_K:
                nNoOfHeart++;nHeartScore+=HEART_K_VAL;break;
            case HEART_A:
                nNoOfHeart++;nHeartScore+=HEART_A_VAL;break;
            case CLUB_10:
				b10ClubCard=TRUE;break;
            case DIAMOND_J:
				bJDiamondCard=TRUE;break;
            case SPADE_Q:
				bQSpadeCard=TRUE;break;
        }

    }

	if ( b10ClubCard && bJDiamondCard && bQSpadeCard && nNoOfHeart >= 13 )
		return BIG_CHUAN_VAL;
    if ( !b10ClubCard && !bJDiamondCard && !bQSpadeCard && nNoOfHeart == 0 )
        return NO_CARD_VAL;        

	int nSumScore = 0;
	int b10ClubX2 = FALSE;
	int bSwapJQ = m_GameSetting.bSwapJQ;                  // implement later
	int bChuan = FALSE;

	if ( bSwapJQ )
	{
		// look for chuan
		for ( int i=0 ; i<GetPlayerList()->Count() ; i++ )
		{
			C_Player * p = GetPlayerList()->Items(i);
			if ( p->GetTakenCardList()->Count() < 13 ) continue;
			if ( IsBigChuan(p->GetTakenCardList()) ) 
			{
				break;
			}
			else if ( IsChuan(p->GetTakenCardList()) )
			{
				bChuan = TRUE;
				break;
			}
		}
	}

	// ==========================
	if ( nNoOfHeart > 0 )
	{
		if ( nNoOfHeart >= 13 )
			nSumScore += -nHeartScore;
		else
			nSumScore += nHeartScore;
		b10ClubX2 = TRUE;
	}
	
	if ( bJDiamondCard )
	{
		if ( bSwapJQ && bChuan )
			nSumScore += SPADE_Q_VAL;
		else
			nSumScore += DIAMOND_J_VAL;
		b10ClubX2 = TRUE;
	}

	if ( bQSpadeCard )
	{
		if ( bSwapJQ && bChuan )
			nSumScore += DIAMOND_J_VAL;
		else
			nSumScore += SPADE_Q_VAL;
		b10ClubX2 = TRUE;
	}

	if ( b10ClubCard )
	{
		if ( b10ClubX2 )
			nSumScore *= 2;
		else
			nSumScore = CLUB_10_VAL;
	}
    return nSumScore;
}

void C_Jubmoo::CheckResponse(int nPlayerID,C_JMComm * pComm)
{
	char szIPBuf[64];
	UINT nPort;

	if ( pComm->GetPeerName(szIPBuf,&nPort) )
		m_pPing->Ping(nPlayerID,szIPBuf);
}



