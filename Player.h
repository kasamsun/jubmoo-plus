#ifndef _C_PLAYER_H_
#define _C_PLAYER_H_

#include "kswafx/MyTempl.h"
#include "JubmooApp.h"

#define MAX_PLAYER_NAME	16
#define NOT_IN_TABLE	-1

enum { IAM_PLAYER,IAM_SPECTATOR,IAM_COMPUTER };

typedef struct _JMPLAYER
{
	int nPlayerStatus;
    int nCharacterID;
	char szPlayerName[MAX_PLAYER_NAME];

	int nNoOfChuan;
	int nNoOfBigChuan;
	int nNoOfPig;
	int nNoOfWinner;
	int nNoOfGamePlay;
} JMPLAYER, *LPJMPLAYER;


class C_Jubmoo;
class C_JubmooAI;
class C_PlayerWnd;

class C_Player
{
public:
	C_Player();
	C_Player(C_Jubmoo * pJumboo,int nPlayerID,LPJMPLAYER PlayerInfo);
	~C_Player();

    // now can't use chain of operator = ,i'm not sure in pointer member of C_Player
    // default constructor and operator = are intended to
    //	 	use only in creating copy of C_Player in Jubmoo Object
    void operator=(C_Player&);

	int Chat(char *lpszMsg);
	int Discard(int nCardNum);

	CARD_LIST * GetCardList(int nFace) { return &m_Card[nFace]; }
	CARD_LIST * GetTakenCardList() { return &m_TakenCard; }
	int GetScore() { return m_nScore; }
    void SetScore(int nScore) { m_nScore = nScore; }
    int GetCharacterID() { return m_nCharacterID; }
    void SetCharacterID(int nCharID) { m_nCharacterID = nCharID; }
	int GetStatus() { return m_nPlayerStatus; }
    void SetStatus(int nPlayerStatus) { m_nPlayerStatus = nPlayerStatus; }
	char * GetName() { return m_szPlayerName; }
    void SetName(LPCSTR lpszName);
	int GetNoOfChuan() { return m_nNoOfChuan; }
    void SetNoOfChuan(int nNoOfChuan) { m_nNoOfChuan = nNoOfChuan; }
	int GetNoOfBigChuan() { return m_nNoOfBigChuan; }
    void SetNoOfBigChuan(int nNoOfBigChuan) { m_nNoOfBigChuan = nNoOfBigChuan; }
	int GetNoOfPig() { return m_nNoOfPig; }
    void SetNoOfPig(int nNoOfPig) { m_nNoOfPig = nNoOfPig; }
	int GetNoOfWinner() { return m_nNoOfWinner; }
    void SetNoOfWinner(int nNoOfWinner) { m_nNoOfWinner = nNoOfWinner; }
	int GetNoOfGamePlay() { return m_nNoOfGamePlay; }
    void SetNoOfGamePlay(int nNoOfGamePlay) { m_nNoOfGamePlay = nNoOfGamePlay; }

    int GetTableOrder() { return m_nTableOrder; }
    void SetTableOrder(int nTableOrder) { m_nTableOrder = nTableOrder; }

	SCORE_LIST * GetHistoryScore() { return &m_HistoryScore; }
	C_Jubmoo * GetJubmoo() { return m_pJubmoo; }
    int GetPlayerID() { return m_nPlayerID; }
	void SetPlayerID(int nPlayerID) { m_nPlayerID = nPlayerID; }

	int ActivateAI();
	C_JubmooAI * GetJubmooAI() { return m_pJmAI; }
	void SetJubmooAI(C_JubmooAI *pJmAI) { m_pJmAI = pJmAI; }

    C_PlayerWnd * GetUI() { return m_pPlayerWnd; }
    void SetUI(C_PlayerWnd *pPlayerWnd) { m_pPlayerWnd = pPlayerWnd; }

protected:

private:
	CARD_LIST m_TakenCard;
	CARD_LIST m_Card[4];

	// personel data
	int m_nScore;
	int m_nPlayerStatus;			// Player, Computer

    int m_nCharacterID;
	char m_szPlayerName[MAX_PLAYER_NAME];

	int m_nNoOfChuan;
	int m_nNoOfBigChuan;
	int m_nNoOfPig;
	int m_nNoOfWinner;
	int m_nNoOfGamePlay;

	// history data
	SCORE_LIST m_HistoryScore;

    // table position of this player ( if i'm player )
    int m_nTableOrder;

	C_Jubmoo * m_pJubmoo;
    int m_nPlayerID;

    // AI Object
	C_JubmooAI * m_pJmAI;
    // UI Object
    C_PlayerWnd * m_pPlayerWnd;
};

#endif
