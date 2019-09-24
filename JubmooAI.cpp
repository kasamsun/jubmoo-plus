#include "JubmooAI.h"

#include "CardNum.h"
#include "Jubmoo.h"
#include "Player.h"
#include "JMKnowledgeBase.h"

C_JubmooAI::C_JubmooAI(C_Player * pPlayer)
{
	m_pPlayer = pPlayer;
    m_pKB = pPlayer->GetJubmoo()->GetKnowledgeBase();
}

C_JubmooAI::~C_JubmooAI()
{
}

// ===================================================================



// ===================================================================

/*
int CardPlayer(int player)
{
	if (HistActCard[player] > 0)
		return HistActCard[player];
	else
		return -1;
}
int CountAllLeaded()
{
	return 13-Player[MyPlayerID].Card[0];
}
*/
// ==================================================================
int C_JubmooAI::CountGotCard(int nPlayerID,int face)
{
	CARD_LIST * pCard = m_pPlayer->GetJubmoo()->GetPlayer(nPlayerID)->GetTakenCardList();

	int nSum = 0;
	for ( int i=0 ; i<pCard->Count() ; i++ )
	{
		if ( GET_FACE(*(pCard->Items(i))) == face )
			nSum++;
	}
	return nSum;
}

// ==================================================================
int C_JubmooAI::CountLeaded(int face)
{
	return m_pKB->m_nCountLeaded[face];
}

// ==================================================================
int C_JubmooAI::CurrentOrder()
{
	return m_pPlayer->GetJubmoo()->GetTurnNumber()+1;
}

// ==================================================================
int C_JubmooAI::FaceLeaded()
{
	if ( m_pPlayer->GetJubmoo()->GetTurnNumber()==0 ) return AI_ERROR;
	return GET_FACE(m_pPlayer->GetJubmoo()->GetLeadedCard());
}

// ==================================================================
int C_JubmooAI::GameCardLeft(int face)
{
	int nSum = 0 ;
	for ( int i=0 ; i<4 ; i++ )
	{
		int nPlayerID = m_pPlayer->GetJubmoo()->GetIngameTable()->nPlayerID[i];
		C_Player * pPlayer = m_pPlayer->GetJubmoo()->GetPlayer(nPlayerID);
		nSum += pPlayer->GetCardList(face)->Count();
	}
	return nSum;
}

// ==================================================================
int C_JubmooAI::GameMaxCard()
{
    if ( m_pKB->m_nMaxCard == 0 ) return AI_ERROR;
    return m_pKB->m_nMaxCard;
}

// ==================================================================
int C_JubmooAI::GameMinCard()
{
    if ( m_pKB->m_nMinCard == 0 ) return AI_ERROR;
    return m_pKB->m_nMinCard;
}

/*
int GameMaxPlayer()
{
}
int GameMinPlayer()
{
}
*/

// ==================================================================
int C_JubmooAI::GameScore(int nPlayerID)
{
	return m_pKB->m_nGameScore[m_pKB->GetOrder(nPlayerID)];
}

// ==================================================================
int C_JubmooAI::HandFaceLeft(int face)
{
	return m_pPlayer->GetCardList(face)->Count();
}

// ==================================================================
int C_JubmooAI::HandLessCard(int card)
{
	int nFace = GET_FACE(card);
	CARD_LIST * pCard = m_pPlayer->GetCardList(nFace);

	int nSum=0;
	for ( int i=0 ; i<pCard->Count() ; i++ )
	{
		if ( *(pCard->Items(i)) < card )
			nSum++;
		else
			return nSum;
	}
	return nSum;
}

// ==================================================================
int C_JubmooAI::HandLessEqCard(int card)
{
	int nFace = GET_FACE(card);
	CARD_LIST * pCard = m_pPlayer->GetCardList(nFace);

	int nSum=0;
	for ( int i=0 ; i<pCard->Count() ; i++ )
	{
		if ( *(pCard->Items(i)) <= card )
			nSum++;
		else
			return nSum;
	}
	return nSum;
}

// =return card num==================================================
int C_JubmooAI::HandMaxCard(int face)
{
	CARD_LIST * pCard = m_pPlayer->GetCardList(face);
	if ( pCard->Count() <= 0 )
		return AI_ERROR;
	return *( pCard->Items(pCard->Count()-1) );
}

// =return card num==================================================
int C_JubmooAI::HandMinCard(int face)
{
	CARD_LIST * pCard = m_pPlayer->GetCardList(face);
	if ( pCard->Count() <= 0 )
		return AI_ERROR;
	return *( pCard->Items(0) );
}

// ==================================================================
int C_JubmooAI::HandMoreCard(int card)
{
	int nFace = GET_FACE(card);
	CARD_LIST * pCard = m_pPlayer->GetCardList(nFace);

	int nSum=0;
	for ( int i=pCard->Count()-1 ; i>=0 ; i-- )
	{
		if ( *(pCard->Items(i)) > card )
			nSum++;
		else
			return nSum;
	}
	return nSum;
}

// ==================================================================
int C_JubmooAI::HandMoreEqCard(int card)
{
	int nFace = GET_FACE(card);
	CARD_LIST * pCard = m_pPlayer->GetCardList(nFace);

	int nSum=0;
	for ( int i=pCard->Count()-1 ; i>=0 ; i-- )
	{
		if ( *(pCard->Items(i)) >= card )
			nSum++;
		else
			return nSum;
	}
	return nSum;
}

// =return card num==================================================
int C_JubmooAI::HandNextCard(int card)
{
	int nFace = GET_FACE(card);
	CARD_LIST * pCard = m_pPlayer->GetCardList(nFace);

	for ( int i=0 ; i<pCard->Count() ; i++ )
	{
		if ( *(pCard->Items(i)) > card )
			return *(pCard->Items(i));
	}
	return AI_ERROR;
}

// =return card num==================================================
int C_JubmooAI::HandPrevCard(int card)
{
	int nFace = GET_FACE(card);
	CARD_LIST * pCard = m_pPlayer->GetCardList(nFace);

	for ( int i=pCard->Count()-1 ; i>=0 ; i-- )
	{
		if ( *(pCard->Items(i)) < card )
			return *(pCard->Items(i));
	}
	return AI_ERROR;
}

// ==================================================================
int C_JubmooAI::IsBigChuanBroken()
{
	return m_pKB->m_nChkBigChuan;
}

// ==================================================================
int C_JubmooAI::IsCardOnGame(int nCard)
{
	int i;
	for ( i=0 ; i<4 ; i++)
	{
		if (m_pPlayer->GetJubmoo()->GetIngameTable()->nCardNum[i] == nCard)
			return TRUE;
	}
	return FALSE;
}

// ==================================================================
int C_JubmooAI::IsCardOnHand(int nCard)
{
	int i;
	int nFace=GET_FACE(nCard);
	for ( i=0 ; i < m_pPlayer->GetCardList(nFace)->Count() ; i++)
	{
		if ( *(m_pPlayer->GetCardList(nFace)->Items(i)) == nCard)
			return TRUE;
	}
	return FALSE;
}

// ==================================================================
int C_JubmooAI::IsCardOutGame(int card)
{
	if ( m_pKB->m_nCardStatus[GET_FACE(card)][GET_NUM(card)] == CARD_OUT )
		return TRUE;
	else
		return FALSE;
}

// ==================================================================
int C_JubmooAI::IsChuanBroken()
{
	return m_pKB->m_nChkChuan;
}

/*
int IsFaceOnGame(int face)
{
	int i;
	for (i=0;i<=3;i++)
	{
		if (HistActCard[i]>0)
			if ( ((HistActCard[i]-1)/13) == face)
				return TRUE;
	}
	return FALSE;
}
int IsGotCard(int player,int card)
{
	int i;

	for (i=1;i<=RecvCard[player][0];i++)
	{
		if ( RecvCard[player][i] == card )
			return TRUE;
	}
	return FALSE;
}

int IsNoScoreCard()
{
	return m_pKB->m_nChkScore;
}

int IsVoid(int player,int face)
{
	return ChkVoid[player][face];
}
int LastScore(int player)
{
	return Player[player].nScore;
}
int NextPlayer(int seq)
{
	int i;
	if (CurOrder==4) return UNKNOWN;
	i = MyPlayerID+seq;
	return (i>3)?i%4:i;
}
int PrevPlayer(int seq)
{
	int i;
	if (CurOrder==1) return UNKNOWN;
	i = MyPlayerID-seq;
	return (i<0)?-(i%4):i;
}

*/
// ==================================================================
int C_JubmooAI::RandomAnyCard()
{
	return RandomCard(RandomFace());
}

// ==================================================================
int C_JubmooAI::RandomCard(int nFace)
{
	if ( m_pPlayer->GetCardList(nFace)->Count() <= 0 ) return AI_ERROR;
	return *(m_pPlayer->GetCardList(nFace)->Items(
				RANDOM(m_pPlayer->GetCardList(nFace)->Count())));
}

// ==================================================================
int C_JubmooAI::RandomFace()
{
	int i,j;
	int nface[4] = { 0,0,0,0 };

	j = 0;
	for ( i=0;i<=3;i++ )
	{
		if (m_pPlayer->GetCardList(i)->Count() > 0)
		{
			nface[j]=i;
			j++;
		}
	}
	if (j==0) return AI_ERROR;
	return nface[RANDOM(j)];
}

/*
int TotalScore(int player)
{
	return Player[player].nScore+CurScore[player];
}
  */

