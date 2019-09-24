#ifndef _C_JMComm_H_
#define _C_JMComm_H_

#include "MyWinsock.h"
#include "Jubmoo.h"
#include <stdio.h>

// ==========================================================
// MSG_VERSION
//	0x0200	- First Released
//	0x0201	- Add "GamePlay data" in class C_Player
//			- Add MsgID checking
//			- Add ErrCode when join fail
//			- Add KickOut Msg
//  0x0202	- Add more 3 faces of computer player
//			- Replace player after someone quit
//	0x0203	- Force remove player , when kick out
//			- Wait EndTurn until 4 players are in the game
//  0x0204  - Add 4 characters
//          - Jubmoo engine change - Remove player
//          - Fix replace player
// ==========================================================

#define MSG_VERSION				0x0204
#define ID_LOGON_MSG			0x0001
#define ID_LOGON_OK_MSG			0x0002
#define ID_LOGON_FAIL_MSG		0x0003
#define ID_CHAT_MSG				0x0010
#define ID_ADD_PLAYER_MSG		0x0020
#define ID_REMOVE_PLAYER_MSG	0x0021
#define ID_REPLACE_PLAYER_MSG	0x0022
#define ID_SET_GAME_MSG			0x0031
#define ID_START_GAME_MSG		0x0032
#define ID_RESET_GAME_MSG		0x0033
#define ID_SET_SPEED_MSG		0x0034
#define ID_SET_GAME_DATA_MSG	0x0040
#define ID_START_ROUND_MSG		0x0041
#define ID_END_TURN_MSG			0x0042
#define ID_END_ROUND_MSG		0x0043
#define ID_DISCARD_MSG			0x0011

// below is not implemented yet


// err code for logonfail handler
#define PLAYER_FULL_ERR			0x0001
#define INVALID_MSGVER_ERR		0x0002
#define KICK_OUT				0x0101

typedef struct _HEADER_MSG
{
	unsigned short Version;
	unsigned short MsgID;
} HEADER_MSG;

typedef struct _LOGON_MSG
{
	HEADER_MSG Header;
	char Name[16];
	unsigned short PlayerID;
	unsigned char Status;
	unsigned char CharacterID;
	unsigned short NoOfBigChuan;
	unsigned short NoOfChuan;
	unsigned short NoOfPig;
	unsigned short NoOfWinner;
	unsigned short NoOfGamePlay;
} LOGON_MSG;

typedef struct _LOGON_OK_MSG
{
	HEADER_MSG Header;
	unsigned short PlayerID;
} LOGON_OK_MSG;

typedef struct _LOGON_FAIL_MSG
{
	HEADER_MSG Header;
	unsigned short ErrCode;
} LOGON_FAIL_MSG;

typedef struct _CHAT_MSG
{
	HEADER_MSG Header;
	unsigned short PlayerID;
    char ChatMsg[41];					 // chatedit char limit = 40
} CHAT_MSG;

typedef struct _ADD_PLAYER_MSG
{
	HEADER_MSG Header;
	char Name[16];
	unsigned short PlayerID;
	unsigned char Status;
	unsigned char CharacterID;
	unsigned short NoOfBigChuan;
	unsigned short NoOfChuan;
	unsigned short NoOfPig;
	unsigned short NoOfWinner;
	unsigned short NoOfGamePlay;
} ADD_PLAYER_MSG;

typedef struct _REMOVE_PLAYER
{
	HEADER_MSG Header;
	unsigned short PlayerID;
} REMOVE_PLAYER_MSG;

typedef struct _SET_GAME_MSG
{
	HEADER_MSG Header;

	unsigned short PlayerID[4];
	short GameScore;
	unsigned char SwapJQ;
} SET_GAME_MSG, *LPSET_GAME_MSG;

typedef struct _START_GAME_MSG
{
	HEADER_MSG Header;

	unsigned short PlayerID[4];
	short GameScore;
	unsigned char SwapJQ;
} START_GAME_MSG, *LPSTART_GAME_MSG;

typedef struct _RESET_GAME_MSG
{
	HEADER_MSG Header;

} RESET_GAME_MSG, *LPRESET_GAME_MSG;

typedef struct _SET_SPEED_MSG
{
	HEADER_MSG Header;
	unsigned short AISpeed;
	unsigned short PickSpeed;
} SET_SPEED_MSG, *LPSET_SPEED_MSG;

typedef struct _START_ROUND_MSG
{
	HEADER_MSG Header;

    unsigned short PlayerID[4];
    unsigned char PlayersCard[52];
} START_ROUND_MSG, *LPSTART_ROUND_MSG;

typedef struct _END_TURN_MSG
{
	HEADER_MSG Header;

} END_TURN_MSG, *LPEND_TURN_MSG;

typedef struct _END_ROUND_MSG
{
	HEADER_MSG Header;

} END_ROUND_MSG, *LPEND_ROUND_MSG;

typedef struct _SET_GAME_DATA_MSG
{
	HEADER_MSG Header;

    unsigned short PlayerID[4];
	short Score[4];
    unsigned char DropCardNum[4];
	unsigned char ActivePlayerOrder; // order in gamesetting 
    unsigned char LeadedCard;
    unsigned short GameNum;
    unsigned char RoundNum;
    unsigned char TurnNum;
    unsigned char PlayersCardLen[4];
    unsigned char PlayersTakenCardLen[4];
	unsigned char PlayersScoreLen[4];
} SET_GAME_DATA_MSG, *LPSET_GAME_DATA_MSG;

typedef struct _DISCARD_MSG
{
	HEADER_MSG Header;
	unsigned short PlayerID;
    unsigned char CardNum;
} DISCARD_MSG, *LPDISCARD_MSG;

typedef struct _REPLACE_PLAYER_MSG
{
	HEADER_MSG Header;
	unsigned short PlayerID;
    unsigned char SettingOrder;
	short Score;
    unsigned char PlayersCardLen;
    unsigned char PlayersTakenCardLen;
	unsigned char PlayersScoreLen;
} REPLACE_PLAYER_MSG, *LPREPLACE_PLAYER_MSG;

class C_Jubmoo;
class C_Player;
class C_MsgPopupWnd;

class C_JMComm : public C_MyWinsock
{
public:
	C_JMComm(C_Jubmoo *pJubmoo);
	~C_JMComm();

    // interface
    int SendLogon(C_Player *pPlayer);
    int SendLogonOK(int nPlayerID);
    int SendLogonFail(int nErrCode);
	int SendChat(int nPlayerID,LPCSTR lpszMsg);
    int SendAddPlayer(C_Player *pPlayer);
    int SendRemovePlayer(int nPlayerID);
	int SendReplacePlayer(int nPlayerID,int nSettingOrder,LPPLAYER_BUFFER pPlayerBuf);
	int SendSetGame(LPSTART_GAME_SETTING pSetGame);
	int SendStartGame(LPSTART_GAME_SETTING pSetGame);
    int SendResetGame();
	int SendSetSpeed(int nAISpeed,int nPickSpeed);
	int SendStartRound();
	int SendEndTurn();
	int SendEndRound();
    int SendSetGameData();
    int SendDiscard(int nPlayerID,int nCardNum);

	// msg processing
	int OnLogon();
    int OnLogonOK();
    int OnLogonFail();
    int OnChat();
    int OnAddPlayer();
	int OnRemovePlayer();
	int OnReplacePlayer();
	int OnSetGame();
	int OnStartGame();
    int OnResetGame();
	int OnSetSpeed();
	int OnStartRound();
	int OnEndTurn();
	int OnEndRound();
    int OnSetGameData();
    int OnDiscard();

	BOOL CreateServerSocket(unsigned short nPortNum);
    BOOL CreateClientSocket(unsigned short nPortNum,char *szDotAddr);
	int Close();

	virtual void OnAccept(int nErrCode);
	virtual void OnConnect(int nErrCode);
	virtual void OnRecv(int nErrCode);
	virtual void OnSend(int nErrCode);
	virtual void OnClose(int nErrCode);
	virtual void OnOOB(int nErrCode) {};

	// Message declaration
    LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam );
    LRESULT OnPaint(WPARAM wParam,LPARAM lParam);
	LRESULT OnLButtonUp(WPARAM wParam,LPARAM lParam);
	LRESULT OnRButtonUp(WPARAM wParam,LPARAM lParam);
	LRESULT OnEraseBkgnd(WPARAM wParam,LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam,LPARAM lParam);

    C_Player * GetPlayer() { return m_pPlayer; }
	void SetPlayer(C_Player *pPlayer) { m_pPlayer = pPlayer; }
	C_Jubmoo * GetJubmoo() { return m_pJubmoo; }
	
	void PopupMessage(LPCTSTR pszMsg);
	int CreateMsgWnd();

	void CreateLogMessage();
	void OpenLogMessage();
	void CloseLogMessage();
	void LogMessage(LPCTSTR lpszFormat, ...);

private:
	C_Jubmoo * m_pJubmoo;
	C_Player * m_pPlayer;
    int m_bConnecting;
    C_MsgPopupWnd * m_pMsgWnd;
	FILE * m_fpNetLog;
};

#endif
