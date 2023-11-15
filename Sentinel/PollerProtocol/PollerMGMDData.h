#pragma once
#include "PollerMessage.h"
#include "InternalMessage.h"

class CPollerMGMDData
{

public:
	///	<summary>
	/// Creates a global instance of CPollerMGMDData.
	///	</summary>
	///	<returns>A reference to the created CPollerMGMDData instance.</returns>
	static CPollerMGMDData & Instance();

	/// <summary>
	/// Returns if MGMD is available from internals
	/// </summary>
	byte GetMGMDAvailable();

	/// <summary>
	/// Returns if MGMD is Global Option from internals
	/// </summary>
	byte GetMGMDGlobalOption();

	/// <summary>
	/// Returns is game start is send by game. This will return the same value which internals has provided to scf.
	/// </summary>
	byte GetMGMDGameStart();

	/// <summary>
	/// Returns is game end is send by game. This will return the same value which internals has provided to scf.
	/// </summary>
	byte GetMGMDGameEnd();

	/// <summary>
	/// Returns is game polling speed. This will return the same value which internals has provided to scf.
	/// </summary>
	byte GetMGMDPollingSpeed();

	/// <summary>
	/// Returns the sentinel mgmd option. This will return the same value which internals has provided to scf.
	/// </summary>
	byte GetMGMDSenOption();

	/// <summary>
	/// Set Game Start
	/// </summary>
	void SetMGMDGameStart(byte mgmdGameStart);

	/// <summary>
	/// Set Game End
	/// </summary>
	void SetMGMDGameEnd(byte mgmdGameEnd);

	/// <summary>
	/// Set Game Polling Speed
	/// </summary>
	void SetMGMDPollingSpeed(byte mgmdPollingSpeed);

	/// <summary>
	/// Set the MGMD Sentinel Option
	/// </summary>
	void SetMGMDSenOption(byte mgmdSenOption);

	/// <summary>
	/// Set the Global MGMD Option
	/// </summary>
	void SetMGMDGlobalOption(byte mgmdGlobalOption);

	/// <summary>
	/// Returns the Game List Chunk Size Option
	/// </summary>
	WORD GetGameListChunkSizeOption();

	/// <summary>
	/// Set the Game List Chunk Size Option
	/// </summary>
	void SetGameListChunkSizeOption(WORD        gameListChunkSize);

	/// <summary>
	/// Returns whether the ChunkedGamesList flag enabled
	/// </summary>
	/// <return> whether the ChunkedGamesList flag enabled.</return>
	bool IsChunkedGamesListFlagEnabled();
	
	/// <summary>
	/// Sets the ChunkedGamesList enabled flag.
	/// </summary>
	void SetChunkedGamesListFlag(WORD	chunkedGamesListFlag);

	/// <summary>
	/// Returns whether the config Feature Flags Recvd. This will
	/// be set when ChunkedGamesListFlag is received
	/// </summary>
	/// <return> whether config Feature Flags Recvd.</return>
	bool IsConfigFeatureFlagsRecvd();

private:
	CPollerMGMDData();
	~CPollerMGMDData(void);
	static CPollerMGMDData *m_instance;
	static std::recursive_mutex m_instanceCS;

	void CheckandSetMGMDAvailable();

	byte m_mgmdAvailable;
	byte m_mgmdGameStart;
	byte m_mgmdGameEnd;
	byte m_mgmdPollingSpeed;
	byte m_mgmdGlobalOption;
	byte m_mgmdSenOption;
	WORD m_gameListChunkSizeOption;
	bool m_chunkedGamesListFlag;
	bool m_configFeatureFlagsRecvd;
};
