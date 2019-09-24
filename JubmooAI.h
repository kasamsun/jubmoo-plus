#ifndef _C_JUBMOOAI_H_
#define _C_JUBMOOAI_H_

#define AI_ERROR		-1

class C_Player;
class C_JMKnowledgeBase;

class C_JubmooAI
{
public:
	C_JubmooAI(C_Player * pPlayer);
	virtual ~C_JubmooAI();

	virtual Discard() = 0;

	C_Player * GetPlayer() { return m_pPlayer; }

    // Knowledgebase Service
	int CountGotCard(int nPlayerID,int face);
	int CountLeaded(int face);
	int CurrentOrder();
	int FaceLeaded();
	int GameCardLeft(int face);
	int GameMaxCard();
    int GameMinCard();
	int GameScore(int nPlayerID);
	int HandFaceLeft(int face);
	int HandLessCard(int card);
	int HandLessEqCard(int card);
	int HandMaxCard(int face);
	int HandMinCard(int face);
	int HandMoreCard(int card);
	int HandMoreEqCard(int card);
	int HandNextCard(int card);
	int HandPrevCard(int card);
	int IsBigChuanBroken();
	int IsCardOnGame(int nCard);
	int IsCardOnHand(int nCard);
	int IsCardOutGame(int card);
	int IsChuanBroken();
	int RandomAnyCard();
	int RandomCard(int nFace);
	int RandomFace();


protected:
	C_Player *m_pPlayer;
    C_JMKnowledgeBase *m_pKB;
};
#endif
