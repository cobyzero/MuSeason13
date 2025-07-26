#pragma once

#include "Protocol.h"

struct HARDWARE_ID_RECV
{
	PBMSG_HEAD header;
	char HarwareId[45];
};

struct HARDWARE_ID_SEND_DB
{
	PSBMSG_HEAD header;
	char Account[11];
	char HardwareId[45];
};

class CheatGuard
{
public:
	CheatGuard();
	~CheatGuard();
	void	HardIdRecv(int aIndex, HARDWARE_ID_RECV * pResult);

public:
	void PingStack(int aIndex, int ping);
	void GCSendPing(int aIndex);
	void GCPingSendRecv(PMSG_PING* aRecv, int aIndex);
};

extern CheatGuard gCheatGuard;
