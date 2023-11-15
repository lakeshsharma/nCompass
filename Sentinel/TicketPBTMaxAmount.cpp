#include "stdafx.h"
#include "TicketPBTMaxAmount.h"

// Sentinel includes.
#include "Logging/LogString.h"
#include "Utilities.h"

using namespace std;

CTicketPBTMaxAmount::~CTicketPBTMaxAmount()
{
}

CTicketPBTMaxAmount::CTicketPBTMaxAmount(DWORD startupTickcount) : CConfigFile(startupTickcount),
m_TicketMax(TICKETMAX),
m_PBTMax(PBTMAX)
{
}

bool CTicketPBTMaxAmount::Start()
{
	bool success(LoadConfigFile(TICKETPBTMAXAMOUNTSFILE));
	return success;
}

void CTicketPBTMaxAmount::SetDefaults()
{
	m_PBTMax = PBTMAX;
	m_TicketMax = TICKETMAX;
}

bool CTicketPBTMaxAmount::InterpretNameValue(const std::string& name, const std::string& value)
{
	bool retVal = false;

    try
    {
        if (PBTMAXNAME == name)
        {
            long longVal = stol(value.c_str());
            if (longVal > 0 && longVal < LONG_MAX)
            {
                m_PBTMax = longVal;
                retVal = true;
            }
        }
        else if (TICKETMAXNAME == name)
        {
            long longVal = stol(value.c_str());
            if (longVal > 0 && longVal < LONG_MAX)
            {
                m_TicketMax = longVal;
                retVal = true;
            }
        }
    }
    catch (...) 
    {
        LogString(ERROR_LOG, _T("%s error. %s"),
                      static_cast<LPCTSTR>(__TFUNCTION__), name.c_str());
        return false;
    }
    return retVal;
}

void CTicketPBTMaxAmount::LogConfigValues(LPCTSTR sourceDesc, bool valid)
{
    UNREFERENCED_PARAMETER(&sourceDesc); 
    UNREFERENCED_PARAMETER(&valid); 

	bool pbtValid = m_PBTMax != LONG_MAX;
	bool ticketValid = m_TicketMax != LONG_MAX;

	// Log PBT max value
	LPCTSTR statusDesc = pbtValid ? _T("valid") : _T("invalid");
	LogString(GENERAL, _T("PBT maximum amount (%s) %ld"),
		static_cast<LPCTSTR>(statusDesc),
		(long)m_PBTMax);

	// Log Ticket max value
	statusDesc = ticketValid ? _T("valid") : _T("invalid");
	LogString(GENERAL, _T("Ticket maximum amount (%s) %ld"),
		static_cast<LPCTSTR>(statusDesc),
		(long)m_TicketMax);
}

void CTicketPBTMaxAmount::GetMaxAmounts(long &pbtMax, long &ticketMax)
{
	double difference = difftime(CUtilities::GetCurrentTime(), m_lastConfigFileLoaded);

	if (difference / SECONDS_PER_MINUTE >= CONFIG_READ_PERIOD)
	{
		LoadConfigFile(TICKETPBTMAXAMOUNTSFILE);
	}

	pbtMax = m_PBTMax;
	ticketMax = m_TicketMax;
}

long CTicketPBTMaxAmount::GetMaxPBTAmount()
{
	long pbtMax = 0;
	long ticketMax = 0;
	GetMaxAmounts(pbtMax, ticketMax);
	return pbtMax;
}

long CTicketPBTMaxAmount::GetMaxTicketAmount()
{
	long pbtMax = 0;
	long ticketMax = 0;
	GetMaxAmounts(pbtMax, ticketMax);
	return ticketMax;
}
