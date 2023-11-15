#include "UnattendedJackpotCommand.h"
#include "Logging/LogString.h"

CUnattendedJackpotCommand::CUnattendedJackpotCommand(CJackpot* jackpot, DWORD userActionTimeOut, DWORD displayDuration) :
	CCommandMessage("ShowUnattendedJackpotScreen", displayDuration)
{
	if (jackpot)
	{
		m_startupScreen = jackpot->GetStartupScreen();
		m_userActionTimeOut = userActionTimeOut;
		m_taxWithholdingEnabled = jackpot->IsTaxWithholdingEnabled();
		m_jackpotAmount = jackpot->GetJackpotAmount();//pennies
		float fedTaxWithholdPercentDecimal(jackpot->GetFederalTaxWithholdingPercent() / (float)ONE_HUNDRED_PERCENT);
		float stateTaxWithholdPercentDecimal(jackpot->GetStateTaxWithholdingPercent() / (float)ONE_HUNDRED_PERCENT);
		uint64_t jackpotAmountInDollars(m_jackpotAmount / ONE_DOLLAR_IN_CENTS);
		m_federalTaxAmount = (ceil(jackpotAmountInDollars * fedTaxWithholdPercentDecimal) * ONE_DOLLAR_IN_CENTS);
		if (m_federalTaxAmount >= m_jackpotAmount)
		{
			m_federalTaxAmount = floor(jackpotAmountInDollars * fedTaxWithholdPercentDecimal) * ONE_DOLLAR_IN_CENTS;
		}
		m_stateTaxAmount = (ceil(jackpotAmountInDollars * stateTaxWithholdPercentDecimal) * ONE_DOLLAR_IN_CENTS);
		if ((m_stateTaxAmount + m_federalTaxAmount) >= m_jackpotAmount)
		{
			m_stateTaxAmount = floor(jackpotAmountInDollars * stateTaxWithholdPercentDecimal) * ONE_DOLLAR_IN_CENTS;
		}
		LogString(JACKPOT, "CUnattendedJackpotCommand m_startupScreen=%s TaxWithholdingEnabled=%u m_jackpotAmount=%u m_federalTaxAmount=%u m_stateTaxAmount=%u userActionTimeOut=%u",
			m_startupScreen.c_str(), m_taxWithholdingEnabled, m_jackpotAmount, m_federalTaxAmount, m_stateTaxAmount, userActionTimeOut);
	}
	else
	{
		LogString(ERROR_LOG, "CUnattendedJackpotCommand jackpot is null.");
	}
}

CUnattendedJackpotCommand::~CUnattendedJackpotCommand()
{
}

void CUnattendedJackpotCommand::addCustomArgs(JsonWriter &writer) const
{	
	writer.Key("StartupScreen");			writer.String(m_startupScreen);
	writer.Key("TaxWithholdingEnabled");	writer.Bool(m_taxWithholdingEnabled);
	writer.Key("JackpotAmount");			writer.Uint64(m_jackpotAmount);
	writer.Key("FederalTaxAmount");			writer.Uint64(m_federalTaxAmount);
	writer.Key("StateTaxAmount");			writer.Uint64(m_stateTaxAmount);
	writer.Key("UserActionTimeOut");		writer.Uint64(m_userActionTimeOut);
}