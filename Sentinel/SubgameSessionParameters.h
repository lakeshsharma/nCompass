#pragma once

#include "GlobalDefs.h"
#include "InternalMsgContent.h"
#include "Config/Config.h"

struct ExtendedBucketSessionParameters
{
	DWORD m_multiplier;
	DWORD m_divider;

	ExtendedBucketSessionParameters()
	{
		m_multiplier = 0;
		m_divider = 0;
	}
	ExtendedBucketSessionParameters(DWORD multiplier, DWORD divider)
	{
		m_multiplier = multiplier;
		m_divider = divider;
	}
};

struct SubgameSessionParameters : public CInternalMsgContent<INT_MSG_CONTENT_SUBGAME_SESSION_PARAMS>
{
public:
	SubgameSessionParameters()
	{
		m_gameSessionSequenceNumber = SESSION_ID_DEFAULT;
		m_multiplier = DEFAULT_MULTIPLIER;
		m_divider = DEFAULT_COUNTDOWN;
		m_playSessionID = GAME_SESSION_ID_DEFAULT;
		m_denom = 0;
		m_gameNumber = 0;
		m_accountingType = ACCOUNTING_COINS_IN;
	}

	// data members
	DWORD m_gameSessionSequenceNumber;
	DWORD m_multiplier;
	DWORD m_divider;
	int m_playSessionID;
	int m_denom;
	DWORD m_gameNumber;
	AccountingType m_accountingType;

	std::map<DWORD, ExtendedBucketSessionParameters> m_ebSessionParams;  // here key is bucket ID
};
