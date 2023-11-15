#pragma once

#include "GlobalDefs.h"
#include "InternalMsgContent.h"
#include "Config/Config.h"
#include "SubgameSessionParameters.h"
#include "PollerProtocol/PollerPlayerResponseMessage.h"
#include <memory>

struct SubgameSelectionResponse : public CInternalMsgContent<INT_MSG_CONTENT_SUBGAME_SELECTION_RESPONSE>
{
	SubgameSessionParameters m_subgameSessionParams;
	std::vector<std::unique_ptr<ExtendedBucketsCountdownData>> m_ebCountdowns;
};
