#include "JMPing.h"

#include "Jubmoo.h"
#include "JMComm.h"
#include "Player.h"
#include <winsock.h>

C_JMPing::C_JMPing(C_Jubmoo *pJubmoo)
{
	m_pJubmoo = pJubmoo;
}

void C_JMPing::Ping(int nPlayerID,char *szIP)
{
	m_nPlayerID = nPlayerID;

    DWORD dwAddr;

    dwAddr = inet_addr(szIP);
    if ( dwAddr == INADDR_NONE )
	{
		TRACE("convert inet_addr fail\n");
    	return;
	}

	C_IcmpPing::Ping(dwAddr,5000);
}

void C_JMPing::OnResponse(int nErrCode,LPIP_ECHO_REPLY pEchoReply)
{
	C_Player * p = m_pJubmoo->GetPlayer(m_nPlayerID);

	if ( p == NULL )
    	return;

	char tmpbuf[64];

	switch (nErrCode)
	{
	case ICMP_PINGOK:
    	sprintf(tmpbuf,"/sys %d.%d.%d.%d  RTT=%dms TTL=%d",
        		int(LOBYTE(LOWORD(pEchoReply->Address))),
				int(HIBYTE(LOWORD(pEchoReply->Address))),
				int(LOBYTE(HIWORD(pEchoReply->Address))),
				int(HIBYTE(HIWORD(pEchoReply->Address))),
				int(pEchoReply->RoundTripTime),
				int(pEchoReply->Options.Ttl));
		p->Chat(tmpbuf);
    	break;
	case ICMP_NOTINITIALIZED:
    	sprintf(tmpbuf,"/sys Icmp not initialized");
		p->Chat(tmpbuf);
    	break;
	case ICMP_OPENSERVICEFAIL:
    	sprintf(tmpbuf,"/sys Open ping service fail");
		p->Chat(tmpbuf);
    	break;
	case ICMP_ALLOCATEBUFFERFAIL:
    	sprintf(tmpbuf,"/sys Alloc buffer fail");
		p->Chat(tmpbuf);
    	break;
	case ICMP_PINGTIMEOUT:
    	sprintf(tmpbuf,"/sys Lag/ Not response");
		p->Chat(tmpbuf);
    	break;
	}
}
