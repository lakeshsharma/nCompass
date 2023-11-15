#pragma once

#include <string>
#include "GlobalDefs.h"
#include "CommandMessage.h"
#include "Jackpot.h"
#include "../ProgressiveProtocol/ProgressiveClaimableAward.h"

class CMassMarketingCommand : public CCommandMessage
{
public:
	CMassMarketingCommand(CProgressiveClaimableAward* claimableAward, DWORD displayDuration);
	virtual ~CMassMarketingCommand();

	virtual void addCustomArgs(JsonWriter &writer) const;

private:
	std::string m_awardAmount;
	std::string m_url;
	int64_t m_expiration;
	bool m_samePlayer;
};