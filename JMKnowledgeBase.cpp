#include "JMKnowledgeBase.h"

#include "CardNum.h"
#include "Jubmoo.h"

C_JMKnowledgeBase::C_JMKnowledgeBase(C_Jubmoo * pJubmoo)
{
	m_pJubmoo = pJubmoo;
}

C_JMKnowledgeBase::~C_JMKnowledgeBase()
{
}

int C_JMKnowledgeBase::GetOrder(int nPlayerID)
{
	for ( int i=0 ; i<4 ; i++ )
		if ( m_pJubmoo->GetIngameTable()->nPlayerID[i] == nPlayerID )
			return i;
	return -1;
}

void C_JMKnowledgeBase::Reset()
{
	int i,j;
	for ( i = 0 ; i<4 ; i++ )
    	for ( j = 0 ; j<13 ; j++ )
		    m_nCardStatus[i][j] = CARD_ONHAND;
	for ( i = 0 ; i<4 ; i++ )
    	for ( j = 0 ; j<4 ; j++ )
        	m_nChkVoid[i][j] = FALSE;
	for ( i = 0 ; i<4 ; i++ )
	{
		m_nCountLeaded[i] = 0;
		m_nGameScore[i] = -50;
	}

	m_nChkChuan = FALSE;
    m_nChkBigChuan = FALSE;
	m_nChkScoreCard = FALSE;
    m_nMaxCard = 0;
    m_nMinCard = 0;
}

void C_JMKnowledgeBase::OnDiscard(int nPlayerID,int nCard)
{
	if ( m_pJubmoo->GetTurnNumber() == 0 )
    {
    	// first turn
		m_nCountLeaded[GET_FACE(nCard)]++;

        m_nMaxCard = nCard;
        m_nMinCard = nCard;
    }
    else
    {
        // update chkvoid
		int nOrder = GetOrder(nPlayerID);
		int nFace = GET_FACE(m_pJubmoo->GetLeadedCard());
        if ( m_nChkVoid[nOrder][nFace] == FALSE )
            if ( m_pJubmoo->GetLeadedCard() != GET_FACE(nCard) )
                m_nChkVoid[nOrder][nFace] = TRUE;

		if ( GET_FACE(nCard) == GET_FACE(m_pJubmoo->GetLeadedCard()) )
        {
			if ( nCard > m_nMaxCard ) m_nMaxCard = nCard;
            if ( nCard < m_nMinCard ) m_nMinCard = nCard;
        }
    }

	// update card status
	m_nCardStatus[GET_FACE(nCard)][GET_NUM(nCard)] = CARD_INGAME;
}

void C_JMKnowledgeBase::OnTakenCard(int nPlayerID)
{
    if ( m_nChkBigChuan == FALSE )
    {
		int nCount = 0;
		for ( int i=0 ; i<4 ; i++ )
		{
			C_Player * pPlayer = m_pJubmoo->GetPlayer(m_pJubmoo->GetIngameTable()->nPlayerID[i]);
            if ( pPlayer == NULL ) continue;
			if ( pPlayer->GetTakenCardList()->Count() > 0 )
			{
				nCount++;
				if ( nCount >= 2 )
				{
					m_nChkBigChuan = TRUE;
					break;
				}
			}
		}
    }
    if ( m_nChkChuan == FALSE )
    {
		int nCount = 0;
		for ( int i=0 ; i<4 ; i++ )
		{
			C_Player * pPlayer = m_pJubmoo->GetPlayer(m_pJubmoo->GetIngameTable()->nPlayerID[i]);
            if ( pPlayer == NULL ) continue;
			for ( int k=0 ; k<pPlayer->GetTakenCardList()->Count() ; k++ )
			{
				if ( GET_FACE( *(pPlayer->GetTakenCardList()->Items(k)) ) == HEART )
				{
					nCount++;
					break;
				}
			}
			if ( nCount >= 2 )
			{
				m_nChkChuan = TRUE;
				break;
			}
		}
    }
	if ( m_nChkScoreCard == FALSE )
	{
		if ( m_nCardStatus[CLUB][CARD_10]==CARD_OUT && 
				m_nCardStatus[DIAMOND][CARD_J]==CARD_OUT &&
				m_nCardStatus[SPADE][CARD_Q]==CARD_OUT &&
				m_nCardStatus[HEART][CARD_2]==CARD_OUT &&
				m_nCardStatus[HEART][CARD_3]==CARD_OUT &&
				m_nCardStatus[HEART][CARD_4]==CARD_OUT &&
				m_nCardStatus[HEART][CARD_5]==CARD_OUT &&
				m_nCardStatus[HEART][CARD_6]==CARD_OUT &&
				m_nCardStatus[HEART][CARD_7]==CARD_OUT && 
				m_nCardStatus[HEART][CARD_8]==CARD_OUT &&
				m_nCardStatus[HEART][CARD_9]==CARD_OUT && 
				m_nCardStatus[HEART][CARD_10]==CARD_OUT &&
				m_nCardStatus[HEART][CARD_J]==CARD_OUT && 
				m_nCardStatus[HEART][CARD_Q]==CARD_OUT &&
				m_nCardStatus[HEART][CARD_K]==CARD_OUT && 
				m_nCardStatus[HEART][CARD_A]==CARD_OUT )
			m_nChkScoreCard = TRUE;
	}
	C_Player * pPlayer = m_pJubmoo->GetPlayer(nPlayerID);
    if ( pPlayer != NULL )
		m_nGameScore[GetOrder(nPlayerID)] = m_pJubmoo->CalculateScore(pPlayer->GetTakenCardList());

    m_nMaxCard = 0;
    m_nMinCard = 0;
}




