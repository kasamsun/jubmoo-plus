#ifndef _C_JMPING_H_
#define _C_JMPING_H_

#include "kswafx/IcmpPing.h"

// ==========================================
// IcmpPing need WSAStartup before use 
// ==========================================
class C_Jubmoo;

class C_JMPing : public C_IcmpPing
{
public:
	C_JMPing(C_Jubmoo *pJubmoo);

    void Ping(int nPlayerID,char * szIP);
protected:
	virtual void OnResponse(int nErrCode,LPIP_ECHO_REPLY pEchoReply);
private:
	C_Jubmoo * m_pJubmoo;
	int m_nPlayerID;
};

#endif