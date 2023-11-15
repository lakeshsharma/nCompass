#pragma once

#include <string>
#include "GlobalDefs.h"
#include "CommandMessage.h"
#include "Jackpot.h"

class CPromoGameCommand : public CCommandMessage
{
public:
	CPromoGameCommand(const std::string &paytableItemType, int64_t paytableItemValue, int64_t jackpotId, int64_t amount, const std::string& dynamicContentPath);
	virtual ~CPromoGameCommand();

	virtual void addCustomArgs(JsonWriter &writer) const;

private:
	std::string m_paytableItemType;
	int64_t m_paytableItemValue;
	int64_t m_jackpotId;
	int64_t m_amount;
};