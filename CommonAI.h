#ifndef _C_COMMONAI_H_
#define _C_COMMONAI_H_

#include "JubmooAI.h"

class C_Player;

class C_CommonAI : public C_JubmooAI
{
public:
	C_CommonAI(C_Player * pPlayer);
	~C_CommonAI();

	Discard();

};
#endif
