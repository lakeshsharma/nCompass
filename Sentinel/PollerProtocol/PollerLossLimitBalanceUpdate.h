#pragma once
#include "PollerMessage.h"

typedef struct _lossLimitBalanceUpdate
{
	byte card_id[CARD_ID_LEN];
	WORD withdrawalAmount;
	WORD newBalance;
} LossLimitBalanceUpdate;

class CPollerLossLimitBalanceUpdate :
	public CPollerMessage
{
public:
	CPollerLossLimitBalanceUpdate(byte *message);
	~CPollerLossLimitBalanceUpdate(void);

	byte* GetCardID();
	WORD GetWithdrawalAmount();
	WORD GetNewBalance();

private:
	LossLimitBalanceUpdate m_buffer;

};
