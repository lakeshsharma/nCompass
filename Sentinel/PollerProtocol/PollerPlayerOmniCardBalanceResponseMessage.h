#pragma once
#include "PollerMessage.h"

typedef struct _playerOminCardBalanceResp
{
	byte card_id[ 5 ];
	LONGLONG totalRedeemablePoints;
	LONGLONG totalRedeemableComps;
	LONGLONG totalPoints;
	LONGLONG totalComps;
} PlayerOmniCardBalanceResponse;


class CPollerPlayerOmniCardBalanceResponseMessage : public CPollerMessage
{
public:
	CPollerPlayerOmniCardBalanceResponseMessage( byte *response);

	PlayerOmniCardBalanceResponse GetPlayerResponse();

public:
	~CPollerPlayerOmniCardBalanceResponseMessage();

private:
	PlayerOmniCardBalanceResponse m_playerResponse;

};
