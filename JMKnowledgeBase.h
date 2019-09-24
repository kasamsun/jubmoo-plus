#ifndef _C_JMKNOWLEDGE_H_
#define _C_JMKNOWLEDGE_H_

#include "JubmooApp.h"

class C_Player;
class C_Jubmoo;
class C_JubmooAI;

enum { CARD_OUT,CARD_INGAME,CARD_ONHAND };

class C_JMKnowledgeBase
{
	friend C_JubmooAI;
public:
	C_JMKnowledgeBase(C_Jubmoo * pJubmoo);
    ~C_JMKnowledgeBase();

	int GetOrder(int nPlayerID);
    void Reset();
    void OnDiscard(int nPlayerID,int nCard);
    void OnTakenCard(int nPlayerID);

private:
	C_Jubmoo * m_pJubmoo;

    int m_nCardStatus[4][13];     	// [ face 0-3 ] [ index 1-13 ]
    int m_nChkVoid[4][4];			// [ player ] [ face 0 - 3 ]
    int m_nCountLeaded[4];			// [ face 0-3 ]
    int m_nChkChuan;				// chuan is broken?
    int m_nChkBigChuan;				// big chuan is broken?
    int m_nChkScoreCard;			// is there any score card?
	int m_nGameScore[4];			// [ player ]
    int m_nMaxCard;					// max card in this turn
    int m_nMinCard;					// min card in this turn
};
#endif