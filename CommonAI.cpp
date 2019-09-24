#include "CommonAI.h"

#include "Jubmoo.h"
#include "Player.h"
#include "CardNum.h"
#include "JMKnowledgeBase.h"

C_CommonAI::C_CommonAI(C_Player * pPlayer) : C_JubmooAI(pPlayer)
{
}

C_CommonAI::~C_CommonAI()
{
}

int C_CommonAI::Discard()
{
#define TESTKNOWLEDGEBASEx
#ifdef TESTKNOWLEDGEBASE
	// test knowledge base
    char tmpbuf[512];
    int pid = m_pPlayer->GetPlayerID();
	if ( m_pKB->GetOrder(pid) == 2 )
	{
		// show only in #1 position
		sprintf(tmpbuf,"CountGotCard[%d,%d,%d,%d] CountLeaded[%d,%d,%d,%d] CurrentOrder[%d]" \
    		" FaceLeaded[%d] GameCardLeft[%d,%d,%d,%d] GameMaxCard[%d] GameMinCard[%d] GameScore[%d]" \
            " IsBigChuanBroken[%d] IsChuanBroken[%d] Is-Q-OnGame[%d] Is-J-OnGame[%d]" \
            " Is-Q-OnHand[%d] Is-J-OnHand[%d] Is-Q-OutGame[%d] Is-J-OutGame[%d]",
    		CountGotCard(pid,0),CountGotCard(pid,1),CountGotCard(pid,2),CountGotCard(pid,3),
            CountLeaded(0),CountLeaded(1),CountLeaded(2),CountLeaded(3),
            CurrentOrder(),
            FaceLeaded(),
            GameCardLeft(0),GameCardLeft(1),GameCardLeft(2),GameCardLeft(3),
            GameMaxCard(),GameMinCard(),
            GameScore(pid),
            IsBigChuanBroken(),IsChuanBroken(),
            IsCardOnGame(SPADE_Q),IsCardOnGame(DIAMOND_J),
            IsCardOnHand(SPADE_Q),IsCardOnHand(DIAMOND_J),
			IsCardOutGame(SPADE_Q),IsCardOutGame(DIAMOND_J)
            );
		MessageBox(GetForegroundWindow(),tmpbuf,"",MB_OK);
		sprintf(tmpbuf,"HandFaceLeft[%d,%d,%d,%d] HandLessCard-10h-[%d] " \
    		"HandLessEqCard-10h-[%d] HandMaxCard[%d,%d,%d,%d] " \
            "HandMinCard[%d,%d,%d,%d] HandMoreCard-10h-[%d] " \
            "HandMoreEqCard-10h-[%d] HandNextCard-10c-[%d] HandPrevCard-10c-[%d]",
			HandFaceLeft(0),HandFaceLeft(1),HandFaceLeft(2),HandFaceLeft(3),
            HandLessCard(HEART_10),HandLessEqCard(HEART_10),
            HandMaxCard(0),HandMaxCard(1),HandMaxCard(2),HandMaxCard(3),
            HandMinCard(0),HandMinCard(1),HandMinCard(2),HandMinCard(3),
            HandMoreCard(HEART_10),HandMoreEqCard(HEART_10),
            HandNextCard(CLUB_10),HandPrevCard(CLUB_10)
            );
		MessageBox(GetForegroundWindow(),tmpbuf,"",MB_OK);
	}
    // ============================================================
#endif

	int tmp1,tmp2,tmp3;

	switch ( CurrentOrder() )
	{
		// 1111111111111111111111111111111111111111111111111111111111111111
		case 1:
			if ( !IsCardOnHand(SPADE_Q) && (HandLessCard(SPADE_Q)>3) )
				return HandPrevCard(SPADE_Q);
			if ( IsCardOnHand(SPADE_Q) ||
					( (IsCardOnHand(SPADE_A)||IsCardOnHand(SPADE_K)) &&
						!IsCardOutGame(SPADE_Q)) )
			{
				if ( HandLessCard(SPADE_Q)<=3 )
				{  // search for min no of each face
					tmp1 = HandFaceLeft(CLUB);
					tmp2 = HandFaceLeft(DIAMOND);
					tmp3 = HandFaceLeft(HEART);
					if ( tmp1>0 || tmp2>0 || tmp3>0 )
					{
						if (tmp1==0) tmp1+=13;
						if (tmp2==0) tmp2+=13;
						if (tmp3==0) tmp3+=13;
						if ( tmp1<=tmp2 && tmp1<=tmp3 )
							return RandomCard(CLUB);
						if ( tmp2<=tmp1 && tmp2<=tmp3 )
							return RandomCard(DIAMOND);
						if ( tmp3<=tmp1 && tmp3<=tmp2 )
							return RandomCard(HEART);
					}
					else
					{	// on other card
						if ( HandLessCard(SPADE_Q)>0 ) return HandPrevCard(SPADE_Q);
						if ( HandMoreCard(SPADE_Q)>0 ) return HandNextCard(SPADE_Q);
					}
				}
				else
					return HandPrevCard(SPADE_Q);
			}
			else
			{	// don't afraid about Q
				if ( !IsCardOutGame(SPADE_Q) && HandFaceLeft(SPADE)>0 )
					return RandomCard(SPADE);
				if ( IsCardOnHand(DIAMOND_J) )
				{
					if ( (IsCardOutGame(DIAMOND_Q)||IsCardOnHand(DIAMOND_Q)) &&
							(IsCardOutGame(DIAMOND_K)||IsCardOnHand(DIAMOND_K)) &&
							(IsCardOutGame(DIAMOND_A)||IsCardOnHand(DIAMOND_A)) )
						return DIAMOND_J;
				}
				if ( (HandLessEqCard(HEART_J))>=2 && HandMoreCard(HEART_J)<=1 )
					return HandPrevCard(HEART_Q);
				if ( GameScore(m_pPlayer->GetPlayerID())>0 && !IsCardOutGame(CLUB_10) &&
						IsCardOutGame(SPADE_Q) && HandMoreCard(CLUB_10)>0 )
					return HandNextCard(CLUB_10);
				if ( GameScore(m_pPlayer->GetPlayerID())<(-50) && HandLessCard(CLUB_10)>0)
					return HandPrevCard(CLUB_10);
				if ( IsCardOnHand(DIAMOND_J) )
				{
					if ( HandFaceLeft(CLUB)>0 ) return RandomCard(CLUB);
					if ( HandFaceLeft(HEART)>0 )
					{
						if ( HandLessEqCard(HEART_J)>0 )
							return HandPrevCard(HEART_Q);
						return RandomCard(HEART);
					}
				}
			}
			return RandomAnyCard();
		// 22222222222222222222222222222222222222222222222222222222222222222
		case 2:
			if ( HandFaceLeft(FaceLeaded()) > 0 )
			{
				if ( IsCardOnGame(SPADE_Q) )
					return HandMinCard(FaceLeaded());
				if ( IsCardOnGame(DIAMOND_J) && HandMoreCard(DIAMOND_J)>0 )
					return HandMaxCard(DIAMOND);
				switch ( FaceLeaded() )
				{
					case SPADE:
						if ( !IsCardOutGame(SPADE_Q) )
							if ( IsCardOnHand(SPADE_Q) )
								if ( GameMaxCard()==SPADE_A || GameMaxCard()==SPADE_K )
									return SPADE_Q;
								else
								{
									if ( HandLessCard(SPADE_Q) > 0 )
										return HandPrevCard(SPADE_Q);
									if ( HandMoreCard(SPADE_Q) > 0 )
										return HandNextCard(SPADE_Q);
								}
							else
								if ( HandLessCard(SPADE_Q) > 0 )
									return HandPrevCard(SPADE_Q);
						break;
					case DIAMOND:
						if ( !IsCardOutGame(DIAMOND_J) )
						{
							if ( IsCardOnHand(DIAMOND_J) && GameMaxCard() < DIAMOND_J &&
									(IsCardOutGame(DIAMOND_Q)||IsCardOnHand(DIAMOND_Q)) &&
									(IsCardOutGame(DIAMOND_K)||IsCardOnHand(DIAMOND_K)) &&
									(IsCardOutGame(DIAMOND_A)||IsCardOnHand(DIAMOND_A)) )
								return DIAMOND_J;
							if ( HandMoreCard(DIAMOND_J)>=2 && CountLeaded(DIAMOND)>=2 )
								return HandNextCard(DIAMOND_J);
							if ( HandMoreCard(DIAMOND_J)>=1 && CountLeaded(DIAMOND)>=3 )
								return HandNextCard(DIAMOND_J);
							if ( HandLessCard(DIAMOND_J)>0 ) return HandPrevCard(DIAMOND_J);
							if ( HandMoreCard(DIAMOND_J)>0 )	return HandNextCard(DIAMOND_J);
						}
						break;
					case CLUB:
						if ( IsCardOutGame(CLUB_10) ) break;
						if ( GameScore(m_pPlayer->GetPlayerID())<(-50) && HandLessCard(GameMaxCard())>0 )
							return HandPrevCard(GameMaxCard());
						else
						{
							if ( CountGotCard(m_pPlayer->GetPlayerID(),HEART)>0 && HandLessCard(GameMaxCard())>0 )
								return HandPrevCard(GameMaxCard());
						}
						break;
					case HEART:
						if ( GameMaxCard()>HEART_J && HandLessCard(GameMaxCard())>0 )
							return HandPrevCard(GameMaxCard());
						else
							if ( HandMoreCard(GameMaxCard())>0 )
								if ( HandNextCard(GameMaxCard())>HEART_J )
								{
									if ( HandLessCard(GameMaxCard())>0 )
										return HandPrevCard(GameMaxCard());
								}
								else
									return HandNextCard(GameMaxCard());
						break;
				}
				return RandomCard(FaceLeaded());
			}
			else
			{  // yud of current order  = 2
				if ( IsCardOnHand(SPADE_Q) )
					return SPADE_Q;
				else
					if ( !IsCardOutGame(SPADE_Q) )
					{
						if ( IsCardOnHand(SPADE_A) ) return SPADE_A;
						if ( IsCardOnHand(SPADE_K) ) return SPADE_K;
					}
				if ( IsCardOnHand(HEART_A) ) return HEART_A;
				if ( IsCardOnHand(HEART_K) ) return HEART_K;
				if ( IsCardOnHand(DIAMOND_J) )
				{
					if ( HandFaceLeft(CLUB)>0 ) return RandomCard(CLUB);
					if ( HandFaceLeft(SPADE)>0 ) return RandomCard(SPADE);
					if ( HandFaceLeft(HEART)>0 ) return RandomCard(HEART);
					if ( HandMoreCard(DIAMOND_J)>0 ) return HandNextCard(DIAMOND_J);
					if ( HandLessCard(DIAMOND_J)>0 ) return HandPrevCard(DIAMOND_J);
				}
				return RandomAnyCard();
			}
		// 3333333333333333333333333333333333333333333333333333333333333333
		case 3:
			if ( HandFaceLeft(FaceLeaded()) > 0 )
			{
				if ( IsCardOnGame(SPADE_Q) )
					return HandMinCard(FaceLeaded());
				if ( IsCardOnGame(DIAMOND_J) && HandMoreCard(DIAMOND_J)>0 )
					return HandMaxCard(DIAMOND);
				switch ( FaceLeaded() )
				{
					case SPADE:
						if ( !IsCardOutGame(SPADE_Q) )
							if ( IsCardOnHand(SPADE_Q) )
								if ( GameMaxCard()==SPADE_A || GameMaxCard()==SPADE_K )
									return SPADE_Q;
								else
								{
									if ( HandMoreCard(SPADE_Q) > 0 )
										return HandNextCard(SPADE_Q);
									if ( HandLessCard(SPADE_Q) > 0 )
										return HandPrevCard(SPADE_Q);
								}
							else
								if ( HandLessCard(SPADE_Q) > 0 )
									return HandPrevCard(SPADE_Q);
						break;
					case DIAMOND:
						if ( !IsCardOutGame(DIAMOND_J) )
						{
							if ( IsCardOnHand(DIAMOND_J) && GameMaxCard()<DIAMOND_J &&
									(IsCardOutGame(DIAMOND_Q)||IsCardOnHand(DIAMOND_Q)) &&
									(IsCardOutGame(DIAMOND_K)||IsCardOnHand(DIAMOND_K)) &&
									(IsCardOutGame(DIAMOND_A)||IsCardOnHand(DIAMOND_A)) )
								return DIAMOND_J;
							if ( HandMoreCard(DIAMOND_J)>=2 && CountLeaded(DIAMOND)>=2 )
								return HandNextCard(DIAMOND_J);
							if ( HandMoreCard(DIAMOND_J)>=1 && CountLeaded(DIAMOND)>=3 )
								return HandNextCard(DIAMOND_J);
							if ( HandLessCard(DIAMOND_J)>0 ) return HandPrevCard(DIAMOND_J);
							if ( HandMoreCard(DIAMOND_J)>0 ) return HandNextCard(DIAMOND_J);
						}
						break;
					case CLUB:
						if ( IsCardOutGame(CLUB_10) ) break;
						if ( GameScore(m_pPlayer->GetPlayerID())<(-50) && HandLessCard(GameMaxCard())>0 )
							return HandPrevCard(GameMaxCard());
						else
						{
							if ( CountGotCard(m_pPlayer->GetPlayerID(),HEART)>0 && HandLessCard(GameMaxCard())>0 )
								return HandPrevCard(GameMaxCard());
						}
						break;
					case HEART:
						if ( GameMaxCard()>HEART_J && HandLessCard(GameMaxCard())>0 )
							return HandPrevCard(GameMaxCard());
						else
							if ( HandMoreCard(GameMaxCard())>0 )
								if ( HandNextCard(GameMaxCard())>HEART_J )
								{
									if ( HandLessCard(GameMaxCard())>0 )
										return HandPrevCard(GameMaxCard());
								}
								else
									return HandNextCard(GameMaxCard());
						break;
				}
				return RandomCard(FaceLeaded());
			}
			else
			{	// yud of current order  = 3
				if ( IsCardOnHand(SPADE_Q) )
					return SPADE_Q;
				else
					if ( !IsCardOutGame(SPADE_Q) )
					{
						if ( IsCardOnHand(SPADE_A) ) return SPADE_A;
						if ( IsCardOnHand(SPADE_K) ) return SPADE_K;
					}
				if ( IsCardOnHand(HEART_A) ) return HEART_A;
				if ( IsCardOnHand(HEART_K) ) return HEART_K;
				if ( IsCardOnHand(DIAMOND_J) )
				{
					if ( HandFaceLeft(CLUB)>0 ) return RandomCard(CLUB);
					if ( HandFaceLeft(SPADE)>0 ) return RandomCard(SPADE);
					if ( HandFaceLeft(HEART)>0 ) return RandomCard(HEART);
					if ( HandMoreCard(DIAMOND_J)>0 ) return HandNextCard(DIAMOND_J);
					if ( HandLessCard(DIAMOND_J)>0 ) return HandPrevCard(DIAMOND_J);
				}
				return RandomAnyCard();
			}
		// 44444444444444444444444444444444444444444444444444444444444444444
		case 4:
			if ( HandFaceLeft(FaceLeaded()) > 0 )
			{
				if ( IsCardOnGame(SPADE_Q) )
					return HandMinCard(FaceLeaded());
				if ( IsCardOnGame(DIAMOND_J) && HandMoreCard(DIAMOND_J)>0 )
					return HandMaxCard(DIAMOND);
				switch ( FaceLeaded() )
				{
					case SPADE:
						if ( !IsCardOutGame(SPADE_Q) )
							if ( IsCardOnHand(SPADE_Q) )
								if ( GameMaxCard()==SPADE_A || GameMaxCard()==SPADE_K )
									return SPADE_Q;
								else
								{
									if ( HandMoreCard(SPADE_Q) > 0 )
										return HandNextCard(SPADE_Q);
									if ( HandLessCard(SPADE_Q) > 0 )
										return HandPrevCard(SPADE_Q);
								}
							else
								if ( HandMoreEqCard(SPADE_K) > 0 )
									return HandNextCard(SPADE_Q);
						break;
					case DIAMOND:
						if ( IsCardOnHand(DIAMOND_J) )
							if ( GameMaxCard()<DIAMOND_J )
								return DIAMOND_J;
							else
							{
								if ( HandLessCard(DIAMOND_J)>0 )
									return HandPrevCard(DIAMOND_J);
								if ( HandMoreCard(DIAMOND_J)>0 )
									return HandNextCard(DIAMOND_J);
							}
						break;
					case CLUB:
						if ( IsCardOutGame(CLUB_10) ) break;
						if ( GameScore(m_pPlayer->GetPlayerID())<(-50) && HandLessCard(GameMaxCard())>0 )
							return HandPrevCard(GameMaxCard());
						else
						{
							if ( CountGotCard(m_pPlayer->GetPlayerID(),HEART)>0 && HandLessCard(GameMaxCard())>0 )
								return HandPrevCard(GameMaxCard());
						}
						break;
					case HEART:
						if ( GameMaxCard()>HEART_J && HandLessCard(GameMaxCard())>0 )
							return HandPrevCard(GameMaxCard());
						else
							if ( HandMoreCard(GameMaxCard())>0 )
								if ( HandNextCard(GameMaxCard())>HEART_J )
								{
									if ( HandLessCard(GameMaxCard())>0 )
										return HandPrevCard(GameMaxCard());
								}
								else
									return HandNextCard(GameMaxCard());
						break;
				}
				return RandomCard(FaceLeaded());
			}
			else
			{	// yud
				if ( IsCardOnHand(SPADE_Q) )
					return SPADE_Q;
				else
					if ( !IsCardOutGame(SPADE_Q) )
					{
						if ( IsCardOnHand(SPADE_A) ) return SPADE_A;
						if ( IsCardOnHand(SPADE_K) ) return SPADE_K;
					}
				if ( IsCardOnHand(HEART_A) ) return HEART_A;
				if ( IsCardOnHand(HEART_K) ) return HEART_K;
				if ( IsCardOnHand(DIAMOND_J) )
				{
					if ( HandFaceLeft(CLUB)>0 ) return RandomCard(CLUB);
					if ( HandFaceLeft(SPADE)>0 ) return RandomCard(SPADE);
					if ( HandFaceLeft(HEART)>0 ) return RandomCard(HEART);
					if ( HandMoreCard(DIAMOND_J)>0 ) return HandNextCard(DIAMOND_J);
					if ( HandLessCard(DIAMOND_J)>0 ) return HandPrevCard(DIAMOND_J);
				}
				return RandomAnyCard();
			}
	}
	if ( CurrentOrder()!=1 )
		if ( HandFaceLeft(FaceLeaded())>0 )
			return RandomCard(FaceLeaded());
	return RandomAnyCard();
}
