#include "stdafx.h"

#include "ConfigFeatureFlagField.h"
#include "../GlobalDefs.h"
#include "Logging/LogString.h"

CConfigFeatureFlagField::CConfigFeatureFlagField(void) :
	m_FeatureFlag(DEFAULT_FEATURE_FLAG),
	m_atomicRedemptions(DEFAULT_ATOMIC_REDEMPTIONS),
	m_jackpotExternalBonus(DEFAULT_JACKPOTEXTERNALBONUS),
	m_externalCreditToGame(DEFAULT_EXTERNALCREDITTOGAME),
	m_voucherSeparationEnabled(DEFAULT_VOUCHERSEPARATION),
	m_multiGameMultiDenom(DEFAULT_MULTIGAMEMULTIDENOM),
	m_bChunkedGamesList(DEFAULT_CHUNKED_GAMES_LIST)
{
	LogString(GENERAL, "Default external credit to game enabled : %d", m_externalCreditToGame);
	m_type = FEATURE_FLAGS;
}

CConfigFeatureFlagField::~CConfigFeatureFlagField(void)
{
}

void CConfigFeatureFlagField::SetFeatureFlags(byte * featureFlag, int length)
{
	// A Sentinel III feature uses this same config type on v11. The v11 field length is 2 bytes.
	// The v12 feature flag config type is 4 bytes.
	// The length field will be used to determine v11 vs v12. if v11 the feature flag will be set to 0.
	if (length == sizeof(WORD))
	{
		// v11
		m_FeatureFlag = 0;
	}
	else
	{
		length = max(0, length);

		memcpy_s(&m_FeatureFlag, sizeof(m_FeatureFlag), featureFlag, length);
	}
	
	LogString(POLLER_LOG, "Feature Flag received from Poller: %u", m_FeatureFlag);
	m_atomicRedemptions = (m_FeatureFlag & FF_ATOMIC_REDEMPTIONS) == FF_ATOMIC_REDEMPTIONS;
	m_jackpotExternalBonus = (m_FeatureFlag & FF_JACKPOT_EXTERNAL_BONUS) == FF_JACKPOT_EXTERNAL_BONUS;
	m_externalCreditToGame = (m_FeatureFlag & FF_EXTERNAL_CREDIT_TO_GAME) == FF_EXTERNAL_CREDIT_TO_GAME;
	m_voucherSeparationEnabled = (m_FeatureFlag & FF_VOUCHER_SEPARATION) == FF_VOUCHER_SEPARATION;
	m_multiGameMultiDenom = (m_FeatureFlag & FF_MULTI_GAME_MULTI_DENOM) == FF_MULTI_GAME_MULTI_DENOM;
	m_multiGameMultiDenom = (m_FeatureFlag & FF_MULTI_GAME_MULTI_DENOM) == FF_MULTI_GAME_MULTI_DENOM;
	m_bChunkedGamesList = (m_FeatureFlag & FF_CHUNKED_GAMES_LIST) == FF_CHUNKED_GAMES_LIST;
	
	LogString(POLLER_LOG, "Feature flags for: External credit - %u ChunkedGames - %u", m_externalCreditToGame, m_bChunkedGamesList);
}

DWORD CConfigFeatureFlagField::GetFeatureFlags()
{
	return m_FeatureFlag;
}

bool  CConfigFeatureFlagField::ExternalCreditToGame()
{
	return m_externalCreditToGame;
}

bool CConfigFeatureFlagField::VoucherSeparationEnabled(void)
{
	return m_voucherSeparationEnabled;
}

bool CConfigFeatureFlagField::IsChunkedGamesListEnabled(void)
{
	return m_bChunkedGamesList;
}

