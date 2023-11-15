#pragma once

#include <string>
#include "GlobalDefs.h"
#include "FullScreenAttractorCommand.h"

class CWinnerPickAndPickedCommand : public CFullScreenAttractorCommand
{
public:
	CWinnerPickAndPickedCommand(DWORD displayDuration, 
		const std::string& assetTag,
		const std::string& dynamicContentPath,
		const std::string& levelName, 
		const std::string& awardAmount = "",
		const std::string& prizeDescription = "",
		const std::string& paymentMethod = "",
		const std::string& awardType = "",
		const std::string& externalAwardName = "",
		const std::string& externalAwardValue = "",
		const std::string& externalAwardCurrency = "");
	virtual ~CWinnerPickAndPickedCommand();

	virtual void AddDynamicTags(JsonWriter &writer) const;

private:
	std::string GetAwardDescription() const;
	std::string GetAwardCurrency() const;
	std::string GetPaymentMode() const;
	std::string GetPaymentModePrefix() const;

private:
	std::string m_levelName;
	std::string m_awardAmount;
	std::string m_prizeDescription;
	std::string m_paymentMethod;
	std::string m_awardType;
	std::string m_externalAwardName;
	std::string m_externalAwardValue;
	std::string m_externalAwardCurrency;
};