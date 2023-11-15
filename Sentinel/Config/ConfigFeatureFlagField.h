#pragma once

#include "ConfigField.h"

class CConfigFeatureFlagField :
	public CConfigField
{
public:
	CConfigFeatureFlagField(void);
public:
	~CConfigFeatureFlagField(void);

public:

	void SetFeatureFlags(byte * featureFlag, int length);
	DWORD GetFeatureFlags();

	bool AtomicRedemptions()
	{
		return m_atomicRedemptions;
	}

	bool JackpotExternalBonus()
	{
		return m_jackpotExternalBonus;
	}


	bool MultiGameMultiDenom()
	{
		return m_multiGameMultiDenom;
	}

	/// <summary>
	/// Gets the  the voucher separation flag.
	/// </summary>
	bool VoucherSeparationEnabled(void);

	/// <summary>
	/// Gets External Credit To Game flag
	/// </summary>
	/// <returns> External Credit To Game flag</returns>
	bool ExternalCreditToGame();

	/// <summary>
	/// Gets whether to send game list in chunks or whole at once
	/// </summary>
	/// <returns> flag whether to send game list in chunks or whole at once </returns>
	bool IsChunkedGamesListEnabled(void);
	
private:
	static const DWORD DEFAULT_FEATURE_FLAG = 0;
	static const bool DEFAULT_ATOMIC_REDEMPTIONS = false;
	static const bool DEFAULT_JACKPOTEXTERNALBONUS = false;
	static const bool DEFAULT_EXTERNALCREDITTOGAME = false;
	static const bool DEFAULT_VOUCHERSEPARATION = false;
	static const bool DEFAULT_MULTIGAMEMULTIDENOM = false;
	static const bool DEFAULT_CHUNKED_GAMES_LIST = false;
	
	DWORD m_FeatureFlag;	
	bool m_atomicRedemptions;		
	bool m_jackpotExternalBonus;
	bool m_externalCreditToGame;
	bool m_voucherSeparationEnabled;
	bool m_multiGameMultiDenom;
	bool m_bChunkedGamesList;

};
