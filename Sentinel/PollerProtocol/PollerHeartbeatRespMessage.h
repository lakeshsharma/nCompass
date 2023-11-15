#pragma once
#include "PollerMessage.h"
#define PMT_UP 1
#define PMT_TRANSACTION_UP 16

//#pragma pack (1)
typedef struct _HearbeatResp
{
	byte pollerID;
	bool pmtUp;
	bool mmtUp;
	bool voucherServiceUp;
}HeartbeatResp;

class CPollerHeartbeatRespMessage :
	public CPollerMessage
{
public:
	CPollerHeartbeatRespMessage(WORD  packetVersion);
public:
	~CPollerHeartbeatRespMessage(void);

	byte GetPollerID();
	bool IsPMTUp();
	bool IsMMTUp();
	bool IsPMTTransactionsUp();
	bool IsVoucherServiceUp();

private:
	WORD m_packetVersion;
};
