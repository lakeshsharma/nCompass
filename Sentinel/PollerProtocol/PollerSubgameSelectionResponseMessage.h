#pragma once
#include "PollerMessage.h"
#include "SubgameSelectionResponse.h"
#include "PollerPlayerResponseMessage.h"

class CPollerSubgameSelectionResponseMessage :
	public CPollerMessage
{
public:
	CPollerSubgameSelectionResponseMessage(const byte *response, WORD packetVersion, WORD dataLength);
	~CPollerSubgameSelectionResponseMessage();

	SubgameSelectionResponse* GetSubgameSelectionResponse();

protected:
	SubgameSelectionResponse m_subgameSelectionResponse;
};

