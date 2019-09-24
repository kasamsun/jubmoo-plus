#include "Player.h"

#include "CardNum.h"
#include "Jubmoo.h"
#include "JubmooWnd.h"
#include "PlayerWnd.h"
#include "JubmooAI.h"

C_Player::C_Player()
{
	m_pJubmoo = NULL;
    m_nPlayerID = 0;
    m_pPlayerWnd = NULL;
	m_pJmAI = NULL;

	m_nScore = 0;

   	m_nPlayerStatus = IAM_SPECTATOR;
    m_nCharacterID = 0;
	memset(m_szPlayerName,0x00,sizeof(m_szPlayerName));

	m_nNoOfChuan = 0;
	m_nNoOfBigChuan = 0;
	m_nNoOfPig = 0;
	m_nNoOfWinner = 0;
	m_nNoOfGamePlay = 0;

    m_TakenCard.Clear();
	for ( int nFace=0 ; nFace<4 ; nFace++ )
		m_Card[nFace].Clear();
    m_HistoryScore.Clear();
    m_nTableOrder = NOT_IN_TABLE;
}

C_Player::C_Player(C_Jubmoo * pJubmoo,int nPlayerID,LPJMPLAYER PlayerInfo)
{
	m_pJubmoo = pJubmoo;
    m_nPlayerID = nPlayerID;
    m_pPlayerWnd = NULL;
	m_pJmAI = NULL;

	m_nScore = 0;

   	m_nPlayerStatus = PlayerInfo->nPlayerStatus;
    m_nCharacterID = PlayerInfo->nCharacterID;
	strcpy(m_szPlayerName,PlayerInfo->szPlayerName);

	m_nNoOfChuan = PlayerInfo->nNoOfChuan;
	m_nNoOfBigChuan = PlayerInfo->nNoOfBigChuan;
	m_nNoOfPig = PlayerInfo->nNoOfPig;
	m_nNoOfWinner = PlayerInfo->nNoOfWinner;
	m_nNoOfGamePlay = PlayerInfo->nNoOfGamePlay;

    m_TakenCard.Clear();
	for ( int nFace=0 ; nFace<4 ; nFace++ )
	    m_Card[nFace].Clear();
    m_HistoryScore.Clear();
    m_nTableOrder = NOT_IN_TABLE;
}

C_Player::~C_Player()
{
	if ( m_TakenCard.Count() > 0 )
		m_TakenCard.DeleteObjects();
	for ( int nFace=0 ; nFace<4 ; nFace++ )
	{
	    if ( m_Card[nFace].Count() > 0 )
			m_Card[nFace].DeleteObjects();
	}
	if ( m_HistoryScore.Count() > 0 )
		m_HistoryScore.DeleteObjects();

	delete m_pJmAI;

    // delete UI Object
    if ( m_pPlayerWnd == NULL ) return;
	if ( m_nPlayerStatus == IAM_SPECTATOR )
	{
		if ( IsWindow(m_pPlayerWnd->GetHandle()) )
   			DestroyWindow(m_pPlayerWnd->GetHandle());
		delete m_pPlayerWnd;
	}

}

void C_Player::operator=(C_Player& p)
{
	m_pJubmoo = p.m_pJubmoo;
    m_nPlayerID = p.m_nPlayerID;

    // operator = will not copy pointer member
    m_pPlayerWnd = NULL;
	m_pJmAI = NULL;

	m_nScore = p.m_nScore;

   	m_nPlayerStatus = p.m_nPlayerStatus;
    m_nCharacterID = p.m_nCharacterID;
	strcpy(m_szPlayerName,p.m_szPlayerName);

	m_nNoOfChuan = p.m_nNoOfChuan;
	m_nNoOfBigChuan = p.m_nNoOfBigChuan;
	m_nNoOfPig = p.m_nNoOfPig;
	m_nNoOfWinner = p.m_nNoOfWinner;
	m_nNoOfGamePlay = p.m_nNoOfGamePlay;

    int i;
	int nFace;
	for ( nFace=0 ; nFace<4 ; nFace++ )
	{
	    if ( m_Card[nFace].Count() > 0 )
			m_Card[nFace].DeleteObjects();
	}
	for ( nFace=0 ; nFace<4 ; nFace++ )
	{
	    for ( i=0 ; i<p.m_Card[nFace].Count() ; i++ )
		{
	    	int nCard = *(p.m_Card[nFace].Items(i));
			m_Card[nFace].Add( new int( nCard ) );
		}
	}
    m_TakenCard.DeleteObjects();
    for ( i=0 ; i<p.m_TakenCard.Count() ; i++ )
    {
    	int nCard = *(p.m_TakenCard.Items(i));
    	m_TakenCard.Add( new int( nCard ) );
    }
    m_HistoryScore.DeleteObjects();
    for ( i=0 ; i<p.m_HistoryScore.Count() ; i++ )
    {
    	int nScore = *(p.m_HistoryScore.Items(i));
    	m_HistoryScore.Add( new int( nScore ) );
    }

    m_nTableOrder = p.m_nTableOrder;
}

void C_Player::SetName(LPCSTR lpszName)
{
    if ( strlen(lpszName) > MAX_PLAYER_NAME-1 )
        strncpy(m_szPlayerName,lpszName,MAX_PLAYER_NAME-1);
    else
        strcpy(m_szPlayerName,lpszName);
    if ( m_pPlayerWnd != NULL )
    	m_pPlayerWnd->PlayerUpdated();
}


int C_Player::Chat(char *pszMsg)
{
    m_pPlayerWnd->Chat(pszMsg);
	m_pJubmoo->Chat(GetPlayerID(),pszMsg);
	return 0;
}

int C_Player::Discard(int nCardNum)
{
	// remove card from card list
	int nFace = GET_FACE(nCardNum);
	for ( int i=0 ; i<GetCardList(nFace)->Count() ; i++ )
	{
		int ncard = *(GetCardList(nFace)->Items(i));
		if ( ncard == nCardNum )
		{
			delete GetCardList(nFace)->Items(i);
			GetCardList(nFace)->Remove(i);
			// must do UI's discard first , because in Jubmoo's discard
			// will call next turn that 'll call pickup card
			// it cause pickup undiscard card
			m_pJubmoo->GetUI()->Discard(GetTableOrder(),nCardNum);
			m_pJubmoo->Discard(GetPlayerID(),nCardNum);
			return TRUE;
		}
	}
	TRACE("can't discard, card not found\n");
	return FALSE;
}

int C_Player::ActivateAI()
{
	int nCardNum = m_pJmAI->Discard();
	return Discard(nCardNum);
}


