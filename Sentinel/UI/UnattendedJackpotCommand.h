#pragma once

#include <string>
#include "GlobalDefs.h"
#include "CommandMessage.h"
#include "Jackpot.h"

class CUnattendedJackpotCommand : public CCommandMessage
{
public:
	CUnattendedJackpotCommand(CJackpot* jackpot, DWORD userActionTimeOut, DWORD displayDuration);
	virtual ~CUnattendedJackpotCommand();

	virtual void addCustomArgs(JsonWriter &writer) const;

private:
	std::string m_startupScreen;
	DWORD m_userActionTimeOut;
	bool  m_taxWithholdingEnabled;
	DWORD m_jackpotAmount;          // in pennies
	DWORD m_federalTaxAmount;       // in pennies
	DWORD m_stateTaxAmount;         // in pennies
};