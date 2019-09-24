#ifndef _C_JUBMOO_H_
#define _C_JUBMOO_H_

#include "JubmooApp.h"
#include "Player.h"

#define MYID					1
#define NO_PLAYER				0

#define NO_OF_JMPLAYER			4
#define MAX_NO_OF_PLAYER		8
#define MAX_NO_OF_SPECTATOR		4

enum { SERVER_GAMEMODE,CLIENT_GAMEMODE,IDLE_GAMEMODE };
enum { INGAME_PLAYMODE,SETTING_PLAYMODE,IDLE_PLAYMODE,ENDGAME_PLAYMODE };

typedef MyList<C_Player> PLAYER_LIST;

enum { SWAP_Q_WITH_J = 1, NONSWAP_Q_WITH_J = 2 };

typedef struct _START_GAME_SETTTING
{
	int nPlayerID[4];
	int nGameScore;
	int bSwapJQ;
} START_GAME_SETTING,*LPSTART_GAME_SETTING;

typedef struct _INGAME_TABLE
{
	int nPlayerID[4];
    int nCardNum[4];
} INGAME_TABLE,*LPINGAME_TABLE;

typedef struct _PLAYER_BUFFER
{
	int nScore;
	int nCard[13];
	int nTakenCard[16];
	int nHistoryLen;
	int nHistoryScore[70];
} PLAYER_BUFFER,*LPPLAYER_BUFFER;

class C_JMKnowledgeBase;
class C_JMComm;
class C_JubmooWnd;
class C_JMPing;

class C_Jubmoo
{
public:
	C_Jubmoo();
	~C_Jubmoo();

    int Initialize();

	int StartServer();		// enter server mode ( include standalone machince )
    int StartClient(char *lpszJoinIP);		// enter client mode
    int QuitGame();			// quit to idle mode
	int ResetGame();		// quit to setting mode

	int StartGame();		// start game with lastest configuration
	int StartRound();					// start with reshuffle card
	int StartTurn(int nTurnNum);		// start turn with first player
	int NextTurn();			// start next turn and
							// check for last player
	int EndTurn();
	int EndRound();
	int EndGame();

	int Discard(int nPlayerNum,int nCardNum);
	int Chat(int nPlayerID,char *pszMsg);

	int CalculateScore(CARD_LIST *pCardList);

	// player interface
    C_Player * AddNewPlayer(LPJMPLAYER PlayerInfo,int nPlayerID = -1);
    int RemovePlayer(int nPlayerID);
	int ReplacePlayer(int nPlayerID,int nOrder);

	int UpdateSetting(LPSTART_GAME_SETTING pGameSetting);
	LPSTART_GAME_SETTING GetGameSetting() { return &m_GameSetting; }
    LPINGAME_TABLE GetIngameTable() { return &m_IngameTable; }
	int GetIngameOffset() { return m_nIngameOffset; }
    LPPLAYER_BUFFER GetPlayerBuffer(int nOrder) { return &m_PlayerBuffer[nOrder]; }

	C_JMKnowledgeBase * GetKnowledgeBase() { return m_pKnowledgeBase; }
	C_JMComm * GetComm() { return m_pComm; }

    int GetGameMode() { return m_nGameMode; }
    void SetGameMode(int nMode) { m_nGameMode = nMode; }
	int GetPlayMode() { return m_nPlayMode;	}
	void SetPlayMode(int nMode);
	int GetGameNumber() { return m_nGameNum; }
    void SetGameNumber(int nGameNum) { m_nGameNum = nGameNum; }
	int GetRoundNumber() { return m_nRoundNum; }
    void SetRoundNumber(int nRoundNum) { m_nRoundNum = nRoundNum; }
	int GetTurnNumber() { return m_nTurnNum; }
    void SetTurnNumber(int nTurnNum) { m_nTurnNum = nTurnNum; }
    int GetLeadedCard() { return m_nLeadedCard; }
    void SetLeadedCard(int nLeadedCard) { m_nLeadedCard = nLeadedCard; }

	int GetActivePlayerID() { return m_IngameTable.nPlayerID[m_nActivePlayerOrder]; }
	int GetActivePlayerOrder() { return m_nActivePlayerOrder; }
	void SetActivePlayerOrder(int nActivePlayerOrder) { m_nActivePlayerOrder = nActivePlayerOrder; }

	PLAYER_LIST * GetPlayerList() { return &m_PlayerList; }
	C_Player * GetPlayer(int nPlayerID);

	int AddMyPlayer(LPJMPLAYER PlayerInfo);
    C_Player * GetMyPlayer() { return m_pMyPlayer; }
//    void SetMyPlayer(C_Player * pMyPlayer) { m_pMyPlayer = pMyPlayer; }

    C_JubmooWnd * GetUI() { return m_pJubmooWnd; }
	void CheckResponse(int nPlayerID,C_JMComm * pComm);

private:
	int DistributeCard();
	int IsChuan(CARD_LIST *pCardList);
	int IsBigChuan(CARD_LIST *pCardList);

	// UI Object
	C_JubmooWnd * m_pJubmooWnd;

	int PickUpCard(int nFirstOrder);// check who take card in this turn

    // game info
	int m_nRunningPlayerID;
    int m_nGameMode;
	int m_nPlayMode;
	int m_nGameNum;			// 0->xx
	int m_nRoundNum;		// 0-12
	int m_nTurnNum;			// 0-3
	int m_nActivePlayerOrder;	// 0-3 (index of m_IngameTable)
	int m_nLeadedCard;
	int m_nIngameOffset;

	START_GAME_SETTING m_GameSetting;
    INGAME_TABLE m_IngameTable;

	PLAYER_LIST m_PlayerList;
    C_Player * m_pMyPlayer;

	C_JMKnowledgeBase * m_pKnowledgeBase;
    C_JMComm * m_pComm;
	C_JMPing * m_pPing;

	PLAYER_BUFFER m_PlayerBuffer[4];
};

#endif