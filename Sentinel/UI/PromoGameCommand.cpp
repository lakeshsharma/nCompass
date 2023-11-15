#include "PromoGameCommand.h"

using namespace std;

CPromoGameCommand::CPromoGameCommand(const string &paytableItemType,
	int64_t paytableItemValue,
	int64_t jackpotId,
	int64_t amount,
	const string& dynamicContentPath) :
	CCommandMessage("ShowPromoGame", 0, dynamicContentPath),
	m_paytableItemType(paytableItemType),
	m_paytableItemValue(paytableItemValue),
	m_jackpotId(jackpotId),
	m_amount(amount)
{}

CPromoGameCommand::~CPromoGameCommand()
{
}

void CPromoGameCommand::addCustomArgs(JsonWriter &writer) const
{	
	writer.Key("PaytableItemType");		writer.String(m_paytableItemType);
	writer.Key("PaytableItemValue");	writer.Int64(m_paytableItemValue);
	writer.Key("JackpotId");			writer.Int64(m_jackpotId);
	writer.Key("Amount");				writer.Int64(m_amount);
}