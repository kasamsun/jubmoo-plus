#include "resource.h"
#include "JubmooApp.h"
#include "kswafx/GenericWnd.h"
#include "Jubmoo.h"
#include "JubmooWnd.h"
#include "kswafx/RegistryKey.h"
#include <stdio.h>

#ifndef _MSC_VER
#include <vcl/vcl.h>
USEUNIT("AboutDlg.cpp");
USEUNIT("CardWnd.cpp");
USEUNIT("ChatEdit.cpp");
USEUNIT("HelpDlg.cpp");
USEUNIT("JMComm.cpp");
USEUNIT("JMKnowledgeBase.cpp");
USEUNIT("Jubmoo.cpp");
USEUNIT("JubmooAI.cpp");
USEUNIT("JubmooWnd.cpp");
USEUNIT("Player.cpp");
USEUNIT("PlayerDetailWnd.cpp");
USEUNIT("..\kswafx\src\Button.cpp");
USEUNIT("..\kswafx\src\Control.cpp");
USEUNIT("..\kswafx\src\CoolButton.cpp");
USEUNIT("..\kswafx\src\CoolStatic.cpp");
USEUNIT("..\kswafx\src\DCEx.cpp");
USEUNIT("..\kswafx\src\Dialog.cpp");
USEUNIT("..\kswafx\src\Edit.cpp");
USEUNIT("..\kswafx\src\GenericClickWnd.cpp");
USEUNIT("..\kswafx\src\GenericWnd.cpp");
USEUNIT("..\kswafx\src\MainAfx.cpp");
USEUNIT("MsgPopupWnd.cpp");
USEUNIT("..\kswafx\src\MyWinsock.cpp");
USEUNIT("..\kswafx\src\Static.cpp");
USEUNIT("..\kswafx\src\Tooltip.cpp");
USERC("jubmoo.rc");
USEUNIT("PlayerWnd.cpp");
USEUNIT("..\kswafx\src\CoolDlg.cpp");
USEUNIT("PickCardWnd.cpp");
USEUNIT("BmpDigit.cpp");
USEUNIT("StartGameWnd.cpp");
USEUNIT("EndGameWnd.cpp");
USEUNIT("..\kswafx\src\RegistryKey.cpp");
USEUNIT("InfoDlg.cpp");
USEUNIT("SettingDlg.cpp");
USEUNIT("NetStatDlg.cpp");
USEUNIT("JoinDlg.cpp");
USEUNIT("FaceSelectDlg.cpp");
USEUNIT("BmpPreviewDlg.cpp");
USEUNIT("RoundDlg.cpp");
USEUNIT("CommonAI.cpp");
USEUNIT("ScoreChart.cpp");
USEUNIT("..\kswafx\src\MyThread.cpp");
USEUNIT("..\kswafx\src\IcmpPing.cpp");
USEUNIT("JMPing.cpp");
//---------------------------------------------------------------------------
#endif

char C_JubmooApp::m_lpszAppName[] = "JubMoo+";
C_JubmooApp JubmooApp;

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine,
                    int nCmdShow )
{
    C_App::m_hInstance = hInstance;
    C_App::m_hPrevInstance = hPrevInstance;
	C_App::m_lpszCmdLine = lpszCmdLine;
    C_App::m_nCmdShow = nCmdShow;

    if ( ! C_App::m_hPrevInstance )
	{
		// created common class here
		C_GenericWnd::Register(
				CS_VREDRAW|CS_HREDRAW,				// style
				LoadIcon(C_App::m_hInstance,MAKEINTRESOURCE(IDI_JUBMOO)),
				LoadCursor(NULL,IDC_ARROW),
				(HBRUSH)GetStockObject(WHITE_BRUSH),// hBkgBrush
				NULL,								// menu name
				"JubMoo+");
    }

    if ( !JubmooApp.Initialize() )
    	return 0;

    C_Jubmoo Jubmoo;
    // set Jubmoo's UI Object
	JubmooApp.SetMainWnd( Jubmoo.GetUI() );

	Jubmoo.Initialize();

    return C_App::MessageLoop();
}

C_JubmooApp::C_JubmooApp()
{
	OpenRegistry();
    m_nPlayerSlot = GetLastPlayer();
	m_bUseSound = GetUseSound();
	m_bAlarmTurn = GetAlarmTurn();
	CloseRegistry();
}

C_JubmooApp::~C_JubmooApp()
{
	OpenRegistry();
    SetLastPlayer(m_nPlayerSlot);
	SetUseSound(m_bUseSound);
	SetAlarmTurn(m_bAlarmTurn);
	CloseRegistry();
}

int C_JubmooApp::Initialize()
{
	// check screen size
	if ( GetSystemMetrics(SM_CXSCREEN) < 800 )
    {
    	MessageBox(GetActiveWindow(),"800x600 Display mode require to run JubMoo+",
        			C_JubmooApp::m_lpszAppName,MB_OK);
        return FALSE;
    }

	// check color mode
	HDC hdc = GetDC(GetDesktopWindow());
	int nBits = GetDeviceCaps( hdc,BITSPIXEL );
	ReleaseDC(GetDesktopWindow(),hdc);
	if ( nBits <= 8 )
    {
    	MessageBox(GetActiveWindow(),"Hi/True color mode recommended to run JubMoo+",
        			C_JubmooApp::m_lpszAppName,MB_OK);
    }

    // check registry and prepare them if not existing
    CheckJubmooRegistry();

    RANDOMIZE();

    return TRUE;
}

int C_JubmooApp::UpdatePlayerSlot(int nSlot)
{
	m_nPlayerSlot = nSlot;
	return nSlot;
}

void C_JubmooApp::CheckJubmooRegistry()
{
	// make default registry if it does not exist

	char tmpbuf[256];

	OpenRegistry();

    GetAIDelay();
    GetPickDelay();
    GetCustomBkg(tmpbuf,sizeof(tmpbuf));
    GetCustomCard(tmpbuf,sizeof(tmpbuf));
    GetCustomDlg(tmpbuf,sizeof(tmpbuf));
    GetCustomNum(tmpbuf,sizeof(tmpbuf));
    GetCustomPD(tmpbuf,sizeof(tmpbuf));
	GetDefChatMsg(tmpbuf,sizeof(tmpbuf));
    GetJoinIP(tmpbuf,sizeof(tmpbuf));
	GetPort();
    GetOpponent(1,tmpbuf,sizeof(tmpbuf));
    GetOpponent(2,tmpbuf,sizeof(tmpbuf));
    GetOpponent(3,tmpbuf,sizeof(tmpbuf));
    GetOpponent(4,tmpbuf,sizeof(tmpbuf));
    GetLastPlayer();
    GetCharID(1);
    GetCharID(2);
    GetCharID(3);
    GetCharID(4);
    GetCharID(5);
    GetName(1,tmpbuf,sizeof(tmpbuf));
    GetName(2,tmpbuf,sizeof(tmpbuf));
    GetName(3,tmpbuf,sizeof(tmpbuf));
    GetName(4,tmpbuf,sizeof(tmpbuf));
    GetName(5,tmpbuf,sizeof(tmpbuf));
    UINT a,b,c,d,e;
    GetStat(1,a,b,c,d,e);
    GetStat(2,a,b,c,d,e);
    GetStat(3,a,b,c,d,e);
    GetStat(4,a,b,c,d,e);
    GetStat(5,a,b,c,d,e);
    GetOpponentStat(1,a,b,c,d,e);
    GetOpponentStat(2,a,b,c,d,e);
    GetOpponentStat(3,a,b,c,d,e);
    GetOpponentStat(4,a,b,c,d,e);
    GetUseCoolCtl();
    GetUseCustomImg();
    GetUseFading();
    GetUseNetLog();

	GetUseSound();
	GetAlarmTurn();

	CloseRegistry();
}


void C_JubmooApp::OpenRegistry()
{
    m_RegKey.Open(HKEY_CURRENT_USER,JUBMOO_REGISTRY);
}

void C_JubmooApp::CloseRegistry()
{
	m_RegKey.Close();
}

#define DEFAULT_AIDELAY			600
#define DEFAULT_PICKDELAY		1800
#define DEFAULT_CUSTOMBKG		"wood4.bmp"
#define DEFAULT_CUSTOMCARD		"card01.bmp"
#define DEFAULT_CUSTOMDLG		"wood2.bmp"
#define DEFAULT_CUSTOMNUM		""
#define DEFAULT_CUSTOMPD		"pl02.bmp"
//#define DEFAULT_DEFCHATMSG		""
#define DEFAULT_JOINIP			"0.0.0.0"
#define DEFAULT_PORT			6400
#define DEFAULT_OPPONENT1		"เจ้าหมู"
#define DEFAULT_OPPONENT2		"หมูน้อย"
#define DEFAULT_OPPONENT3		"หมูหวาน"
#define DEFAULT_OPPONENT4		"หมูดุ๊กดิ๊ก"
#define DEFAULT_LASTPLAYER		1
#define DEFAULT_CHARID1			0
#define DEFAULT_CHARID2			1
#define DEFAULT_CHARID3			2
#define DEFAULT_CHARID4			3
#define DEFAULT_CHARID5			7
#define DEFAULT_NAME1			"ต้อม"
#define DEFAULT_NAME2			"เนเน่จัง"
#define DEFAULT_NAME3			"พี่ชาย"
#define DEFAULT_NAME4			"หยองกี้"
#define DEFAULT_NAME5			"หญิง"
#define DEFAULT_USECOOLCTL		TRUE
#define DEFAULT_USECUSTOMIMG	TRUE
#define DEFAULT_USEFADING		FALSE
#define DEFAULT_USENETLOG		FALSE
#define DEFAULT_USESOUND		TRUE
#define DEFAULT_ALARMTURN		FALSE

UINT C_JubmooApp::GetAIDelay()
{
    DWORD dwData;
	if ( m_RegKey.Read("AIDelay",dwData) != ERROR_SUCCESS )
    {
    	SetAIDelay(DEFAULT_AIDELAY);
        return DEFAULT_AIDELAY;
    }
    return (UINT)dwData;
}
void C_JubmooApp::SetAIDelay(UINT nAIDelay)
{
	m_RegKey.Write("AIDelay",(DWORD)nAIDelay);
}

UINT C_JubmooApp::GetPickDelay()
{
    DWORD dwData;
	if ( m_RegKey.Read("PickDelay",dwData) != ERROR_SUCCESS )
    {
    	SetPickDelay(DEFAULT_PICKDELAY);
        return DEFAULT_PICKDELAY;
    }
    return (UINT)dwData;
}
void C_JubmooApp::SetPickDelay(UINT nPickDelay)
{
	m_RegKey.Write("PickDelay",(DWORD)nPickDelay);
}

void C_JubmooApp::GetCustomBkg(LPSTR lpszPath,int nBufLen)
{
	if ( m_RegKey.Read("CustomBkg",lpszPath,nBufLen) != ERROR_SUCCESS )
    {
    	SetCustomBkg(DEFAULT_CUSTOMBKG);
        strcpy(lpszPath,DEFAULT_CUSTOMBKG);
    }
}
void C_JubmooApp::SetCustomBkg(LPCSTR lpszPath)
{
	m_RegKey.Write("CustomBkg",lpszPath);
}

void C_JubmooApp::GetCustomCard(LPSTR lpszPath,int nBufLen)
{
	if ( m_RegKey.Read("CustomCard",lpszPath,nBufLen) != ERROR_SUCCESS )
    {
    	SetCustomCard(DEFAULT_CUSTOMCARD);
        strcpy(lpszPath,DEFAULT_CUSTOMCARD);
    }
}
void C_JubmooApp::SetCustomCard(LPCSTR lpszPath)
{
	m_RegKey.Write("CustomCard",lpszPath);
}

void C_JubmooApp::GetCustomDlg(LPSTR lpszPath,int nBufLen)
{
	if ( m_RegKey.Read("CustomDlg",lpszPath,nBufLen) != ERROR_SUCCESS )
    {
    	SetCustomDlg(DEFAULT_CUSTOMDLG);
        strcpy(lpszPath,DEFAULT_CUSTOMDLG);
    }
}
void C_JubmooApp::SetCustomDlg(LPCSTR lpszPath)
{
	m_RegKey.Write("CustomDlg",lpszPath);
}

void C_JubmooApp::GetCustomNum(LPSTR lpszPath,int nBufLen)
{
	if ( m_RegKey.Read("CustomNum",lpszPath,nBufLen) != ERROR_SUCCESS )
    {
    	SetCustomNum(DEFAULT_CUSTOMNUM);
        strcpy(lpszPath,DEFAULT_CUSTOMNUM);
    }
}
void C_JubmooApp::SetCustomNum(LPCSTR lpszPath)
{
	m_RegKey.Write("CustomNum",lpszPath);
}

void C_JubmooApp::GetCustomPD(LPSTR lpszPath,int nBufLen)
{
	if ( m_RegKey.Read("CustomPD",lpszPath,nBufLen) != ERROR_SUCCESS )
    {
    	SetCustomPD(DEFAULT_CUSTOMPD);
        strcpy(lpszPath,DEFAULT_CUSTOMPD);
    }
}
void C_JubmooApp::SetCustomPD(LPCSTR lpszPath)
{
	m_RegKey.Write("CustomPD",lpszPath);
}

void C_JubmooApp::GetDefChatMsg(LPSTR lpszPath,int nBufLen)
{
	if ( m_RegKey.Read("DefChatMsg",lpszPath,nBufLen) != ERROR_SUCCESS )
    {
		char tmpbuf[32];
		sprintf(tmpbuf,":o)%c%c:oP%c%c:~<%c%c(*^_^*)\0",13,10,13,10,13,10);
    	SetDefChatMsg(tmpbuf);
        strcpy(lpszPath,tmpbuf);
    }
}
void C_JubmooApp::SetDefChatMsg(LPCSTR lpszPath)
{
	m_RegKey.Write("DefChatMsg",lpszPath);
}

void C_JubmooApp::GetJoinIP(LPSTR lpszPath,int nBufLen)
{
	if ( m_RegKey.Read("JoinIP",lpszPath,nBufLen) != ERROR_SUCCESS )
    {
    	SetJoinIP(DEFAULT_JOINIP);
        strcpy(lpszPath,DEFAULT_JOINIP);
    }
}
void C_JubmooApp::SetJoinIP(LPCSTR lpszPath)
{
	m_RegKey.Write("JoinIP",lpszPath);
}

UINT C_JubmooApp::GetPort()
{
	DWORD dwData;
	if ( m_RegKey.Read("Port",dwData) != ERROR_SUCCESS )
    {
    	SetPort(DEFAULT_PORT);
        return DEFAULT_PORT;
    }
    return (UINT)dwData;
}
void C_JubmooApp::SetPort(UINT nPort)
{
	m_RegKey.Write("Port",(DWORD)nPort);
}

void C_JubmooApp::GetOpponent(int nOrder,LPSTR lpszPath,int nBufLen)
{
	char tmpbuf[16];
    sprintf(tmpbuf,"Opponent%d",nOrder);
	if ( m_RegKey.Read(tmpbuf,lpszPath,nBufLen) != ERROR_SUCCESS )
    {
    	switch ( nOrder )
        {
        case 1:
        	SetOpponent(nOrder,DEFAULT_OPPONENT1);
            strcpy(lpszPath,DEFAULT_OPPONENT1);
            break;
        case 2:
        	SetOpponent(nOrder,DEFAULT_OPPONENT2);
            strcpy(lpszPath,DEFAULT_OPPONENT2);
            break;
        case 3:
        	SetOpponent(nOrder,DEFAULT_OPPONENT3);
            strcpy(lpszPath,DEFAULT_OPPONENT3);
            break;
        case 4:
        	SetOpponent(nOrder,DEFAULT_OPPONENT4);
            strcpy(lpszPath,DEFAULT_OPPONENT4);
            break;
		}
    }
}
void C_JubmooApp::SetOpponent(int nOrder,LPCSTR lpszPath)
{
	char tmpbuf[16];
    sprintf(tmpbuf,"Opponent%d",nOrder);
	m_RegKey.Write(tmpbuf,lpszPath);
}

void C_JubmooApp::GetOpponentStat(int nOrder,UINT &nGamePlay,UINT &nWin,UINT &nPig,UINT &nBigChuan,UINT &nChuan)
{
	char tmpbuf[16];
    sprintf(tmpbuf,"o%dStat",nOrder);
    unsigned char uData[10];
	memset(uData,0x00,10);
	if ( m_RegKey.Read(tmpbuf,uData,10) != ERROR_SUCCESS )
    {
        SetOpponentStat(nOrder,0,0,0,0,0);
        nGamePlay = 0;
        nWin = 0;
        nPig = 0;
        nBigChuan = 0;
        nChuan = 0;
        return ;
    }
	unsigned short *pShort;
	pShort = (unsigned short*)&uData[0];
    nChuan = (UINT)*pShort;
	pShort = (unsigned short*)&uData[2];
    nBigChuan = (UINT)*pShort;
	pShort = (unsigned short*)&uData[4];
    nPig = (UINT)*pShort;
	pShort = (unsigned short*)&uData[6];
    nWin = (UINT)*pShort;
	pShort = (unsigned short*)&uData[8];
    nGamePlay = (UINT)*pShort;
}
void C_JubmooApp::SetOpponentStat(int nOrder,UINT nGamePlay,UINT nWin,UINT nPig,UINT nBigChuan,UINT nChuan)
{
	char tmpbuf[16];
    sprintf(tmpbuf,"o%dStat",nOrder);
    unsigned char uData[10];
	unsigned short *pShort;
	pShort = (unsigned short*)&uData[0];
    *pShort = (unsigned short)nChuan;
	pShort = (unsigned short*)&uData[2];
    *pShort = (unsigned short)nBigChuan;
	pShort = (unsigned short*)&uData[4];
    *pShort = (unsigned short)nPig;
	pShort = (unsigned short*)&uData[6];
    *pShort = (unsigned short)nWin;
	pShort = (unsigned short*)&uData[8];
    *pShort = (unsigned short)nGamePlay;
	m_RegKey.Write(tmpbuf,uData,10);
}

UINT C_JubmooApp::GetLastPlayer()
{
	unsigned char uData;
	if ( m_RegKey.Read("LastPlayer",&uData,1) != ERROR_SUCCESS )
    {
    	SetLastPlayer(DEFAULT_LASTPLAYER);
        return DEFAULT_LASTPLAYER;
    }
    return (UINT)uData;
}
void C_JubmooApp::SetLastPlayer(UINT nLastPlayer)
{
	unsigned char uData = (unsigned char)nLastPlayer;
	m_RegKey.Write("LastPlayer",&uData,1);
}

UINT C_JubmooApp::GetCharID(int nOrder)
{
	char tmpbuf[16];
    sprintf(tmpbuf,"p%dCharID",nOrder);
	unsigned char uData;
	if ( m_RegKey.Read(tmpbuf,&uData,1) != ERROR_SUCCESS )
    {
    	switch ( nOrder )
        {
        case 1:
	    	SetCharID(nOrder,DEFAULT_CHARID1);
            return DEFAULT_CHARID1;
        case 2:
	    	SetCharID(nOrder,DEFAULT_CHARID2);
            return DEFAULT_CHARID2;
        case 3:
	    	SetCharID(nOrder,DEFAULT_CHARID3);
            return DEFAULT_CHARID3;
        case 4:
	    	SetCharID(nOrder,DEFAULT_CHARID4);
            return DEFAULT_CHARID4;
        case 5:
	    	SetCharID(nOrder,DEFAULT_CHARID5);
            return DEFAULT_CHARID5;
        }
    }
    return (UINT)uData;
}
void C_JubmooApp::SetCharID(int nOrder,UINT nCharID)
{
	char tmpbuf[16];
    sprintf(tmpbuf,"p%dCharID",nOrder);
	unsigned char uData = (unsigned char)nCharID;
	m_RegKey.Write(tmpbuf,&uData,1);
}

void C_JubmooApp::GetName(int nOrder,LPSTR lpszPath,int nBufLen)
{
	char tmpbuf[16];
    sprintf(tmpbuf,"p%dName",nOrder);
	if ( m_RegKey.Read(tmpbuf,lpszPath,nBufLen) != ERROR_SUCCESS )
    {
    	switch ( nOrder )
        {
        case 1:
        	SetName(nOrder,DEFAULT_NAME1);
            strcpy(lpszPath,DEFAULT_NAME1);
            break;
        case 2:
        	SetName(nOrder,DEFAULT_NAME2);
            strcpy(lpszPath,DEFAULT_NAME2);
            break;
        case 3:
        	SetName(nOrder,DEFAULT_NAME3);
            strcpy(lpszPath,DEFAULT_NAME3);
            break;
        case 4:
        	SetName(nOrder,DEFAULT_NAME4);
            strcpy(lpszPath,DEFAULT_NAME4);
            break;
        case 5:
        	SetName(nOrder,DEFAULT_NAME5);
            strcpy(lpszPath,DEFAULT_NAME5);
            break;
		}
    }
}
void C_JubmooApp::SetName(int nOrder,LPCSTR lpszPath)
{
	char tmpbuf[16];
    sprintf(tmpbuf,"p%dName",nOrder);
	m_RegKey.Write(tmpbuf,lpszPath);
}

void C_JubmooApp::GetStat(int nOrder,UINT &nGamePlay,UINT &nWin,UINT &nPig,UINT &nBigChuan,UINT &nChuan)
{
	char tmpbuf[16];
    sprintf(tmpbuf,"p%dStat",nOrder);
    unsigned char uData[10];
	memset(uData,0x00,10);
	if ( m_RegKey.Read(tmpbuf,uData,10) != ERROR_SUCCESS )
    {
        SetStat(nOrder,0,0,0,0,0);
        nGamePlay = 0;
        nWin = 0;
        nPig = 0;
        nBigChuan = 0;
        nChuan = 0;
        return ;
    }
	unsigned short *pShort;
	pShort = (unsigned short*)&uData[0];
    nChuan = (UINT)*pShort;
	pShort = (unsigned short*)&uData[2];
    nBigChuan = (UINT)*pShort;
	pShort = (unsigned short*)&uData[4];
    nPig = (UINT)*pShort;
	pShort = (unsigned short*)&uData[6];
    nWin = (UINT)*pShort;
	pShort = (unsigned short*)&uData[8];
    nGamePlay = (UINT)*pShort;
}
void C_JubmooApp::SetStat(int nOrder,UINT nGamePlay,UINT nWin,UINT nPig,UINT nBigChuan,UINT nChuan)
{
	char tmpbuf[16];
    sprintf(tmpbuf,"p%dStat",nOrder);
    unsigned char uData[10];
	unsigned short *pShort;
	pShort = (unsigned short*)&uData[0];
    *pShort = (unsigned short)nChuan;
	pShort = (unsigned short*)&uData[2];
    *pShort = (unsigned short)nBigChuan;
	pShort = (unsigned short*)&uData[4];
    *pShort = (unsigned short)nPig;
	pShort = (unsigned short*)&uData[6];
    *pShort = (unsigned short)nWin;
	pShort = (unsigned short*)&uData[8];
    *pShort = (unsigned short)nGamePlay;
	m_RegKey.Write(tmpbuf,uData,10);
}

int C_JubmooApp::GetUseCoolCtl()
{
	unsigned char uData;
	if ( m_RegKey.Read("UseCoolCtl",&uData,1) != ERROR_SUCCESS )
    {
    	SetUseCoolCtl(DEFAULT_USECOOLCTL);
        return DEFAULT_USECOOLCTL;
    }
    return (UINT)uData;
}
void C_JubmooApp::SetUseCoolCtl(int bUseCoolCtl)
{
	unsigned char uData;
	if ( bUseCoolCtl )
    	uData = 1;
    else
    	uData = 0;
	m_RegKey.Write("UseCoolCtl",&uData,1);
}

int C_JubmooApp::GetUseCustomImg()
{
	unsigned char uData;
	if ( m_RegKey.Read("UseCustomImg",&uData,1) != ERROR_SUCCESS )
    {
    	SetUseCustomImg(DEFAULT_USECUSTOMIMG);
        return DEFAULT_USECUSTOMIMG;
    }
    return (UINT)uData;
}
void C_JubmooApp::SetUseCustomImg(int bUseCustomImg)
{
	unsigned char uData;
	if ( bUseCustomImg )
    	uData = 1;
    else
    	uData = 0;
	m_RegKey.Write("UseCustomImg",&uData,1);
}

int C_JubmooApp::GetUseFading()
{
	unsigned char uData;
	if ( m_RegKey.Read("UseFading",&uData,1) != ERROR_SUCCESS )
    {
    	SetUseFading(DEFAULT_USEFADING);
        return DEFAULT_USEFADING;
    }
    return (UINT)uData;
}
void C_JubmooApp::SetUseFading(int bUseFading)
{
	unsigned char uData;
	if ( bUseFading )
    	uData = 1;
    else
    	uData = 0;
	m_RegKey.Write("UseFading",&uData,1);
}

int C_JubmooApp::GetUseNetLog()
{
	unsigned char uData;
	if ( m_RegKey.Read("UseNetLog",&uData,1) != ERROR_SUCCESS )
    {
    	SetUseNetLog(DEFAULT_USENETLOG);
        return DEFAULT_USENETLOG;
    }
    return (UINT)uData;
}
void C_JubmooApp::SetUseNetLog(int bUseNetLog)
{
	unsigned char uData;
	if ( bUseNetLog )
    	uData = 1;
    else
    	uData = 0;
	m_RegKey.Write("UseNetLog",&uData,1);
}

int C_JubmooApp::GetUseSound()
{
	unsigned char uData;
	if ( m_RegKey.Read("UseSound",&uData,1) != ERROR_SUCCESS )
    {
    	SetUseSound(DEFAULT_USESOUND);
        return DEFAULT_USESOUND;
    }
    return (UINT)uData;
}
void C_JubmooApp::SetUseSound(int bUseSound)
{
	unsigned char uData;
	if ( bUseSound )
    	uData = 1;
    else
    	uData = 0;
	m_RegKey.Write("UseSound",&uData,1);
}

int C_JubmooApp::GetAlarmTurn()
{
	unsigned char uData;
	if ( m_RegKey.Read("AlarmTurn",&uData,1) != ERROR_SUCCESS )
    {
    	SetUseSound(DEFAULT_ALARMTURN);
        return DEFAULT_ALARMTURN;
    }
    return (UINT)uData;
}
void C_JubmooApp::SetAlarmTurn(int bAlarmTurn)
{
	unsigned char uData;
	if ( bAlarmTurn )
    	uData = 1;
    else
    	uData = 0;
	m_RegKey.Write("AlarmTurn",&uData,1);
}


