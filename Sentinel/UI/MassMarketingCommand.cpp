#include "MassMarketingCommand.h"

CMassMarketingCommand::CMassMarketingCommand(CProgressiveClaimableAward* claimableAward, DWORD displayDuration) :
	CCommandMessage("ShowMassMarketingScreen", displayDuration),
	m_awardAmount(claimableAward->m_amount), 
	m_url(claimableAward->m_acceptanceUrl),
	m_expiration(claimableAward->m_expirationMilliseconds),
	m_samePlayer(claimableAward->m_isSamePlayer)
{}

CMassMarketingCommand::~CMassMarketingCommand()
{
}

void CMassMarketingCommand::addCustomArgs(JsonWriter &writer) const
{	
	writer.Key("AwardAmount");	writer.String(m_awardAmount);
	writer.Key("Url");			writer.String(m_url);
	writer.Key("Expiration");	writer.Int64(m_expiration);
	writer.Key("SamePlayer");	writer.Bool(m_samePlayer);
}