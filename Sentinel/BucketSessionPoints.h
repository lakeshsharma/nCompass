#pragma once

#include "BonusEvents.h"
#include "Meters.h"
#include "SubgameSession.h"
#include "SubgameSessionParameters.h"
#include "PollerProtocol/PollerPlayerResponseMessage.h"

class CBucketSessionPoints
{
public:
	// The multiplier used in order to not do floating point arithmetic.
	static const DWORD COUNTDOWN_PRECISION_MULTIPLIER = 100000;

	/// <summary>
	/// Default constructor.
	/// </summary>
	CBucketSessionPoints();

	/// <summary>
	/// Destructor.
	/// </summary>
	virtual ~CBucketSessionPoints() {}

	/// <summary>
	/// Set Session Points for the player.
	/// </summary>
	/// <param name="points">IN - Points to be set.</param>
	/// <returns>void.</returns>
	void SetSessionPoints(const DWORD points);

	/// <summary>
	/// Set Session Points for the player read during NVRAM read.
	/// </summary>
	/// <param name="points">IN - Points to be set.</param>
	/// <returns>void.</returns>
	void SetInitialSessionPoints(const DWORD points);

	/// <summary>
	/// Get Session Points for the player.
	/// </summary>
	/// <param>void.</param>
	/// <returns>DWORD.</returns>
	DWORD GetSessionPoints() const;

	/// <summary>
	/// Get countdown value.
	/// </summary>
	/// <param>void.</param>
	/// <returns>DWORD: Countdown value.</returns>
	DWORD GetSessionCountdown() const;

	/// <summary>
	/// Set countdown value.
	/// </summary>
	/// <param>void.</param>
	/// <returns>DWORD: Countdown value.</returns>
	void SetSessionCountdown(const DWORD sessionCountDown);

	/// <summary>
	/// Get countdown value.
	/// </summary>
	/// <param>void.</param>
	/// <returns>DWORD: Countdown value.</returns>
	DWORD GetBaseSessionPoints() const;

	/// <summary>
	/// Set the starting Multiplier, Divider and  countdown on card in
	/// </summary>
	/// <param name="startingMultiplier"> IN: starting the multiplier</param> 
	/// <param name="startingDivider"> IN: starting the divider</param> 
	/// <param name="startingCountdown"> IN: starting the countdown</param> 
	/// <param name="startingCountdownHighPrecision"> IN: starting the high precision countdown</param> 
    /// <param name="precisionMultiplier"> IN: the precision multiplier for the supllied h1gh precision countdown</param>
	/// <returns>None.</returns>
	void SetStartingMultiplierDividerCountdown(DWORD startingMultiplier, DWORD startingDivider, DWORD startingCountdown, 
												DWORD startingCountdownHighPrecision, DWORD precisionMultilier);

	/// <summary>
	/// Get active multiplier and divider.
	/// </summary>
	/// <param name="currentSubgameSession"> IN: pointer to current CSubgameSession instance.</param>
	/// <param name="activeMultiplier"> OUT: Multiplier from active subgame session else starting multiplier</param>
	/// <param name="activeDivider"> OUT: Divider from active subgame session else starting divider</param>
	/// <returns>None.</returns>	
	void GetActiveMultiplierDivider(CSubgameSession *currentSubgameSession, DWORD &activeMultiplier, DWORD &activeDivider);

	/// <summary>
	/// Set starting countdown for session. 
	/// </summary>
	/// <param name="startingCountdown"> IN: starting countdown for session.</param> 
	/// <param name="startingCountdownHighPrecision"> IN: starting the high precision countdown</param> 
	/// <param name="precisionMultiplier"> IN: the precision multiplier for the supllied h1gh precision countdown</param>
	/// <returns>None.</returns>	
	void SetStartingCountdown(DWORD startingCountdown, DWORD startingCountdownHighPrecision, DWORD precisionMultilier);

	/// <summary>
	/// Sets the display name.
	/// </summary>
	/// <param name="displayName">IN : the display name.</param>
	/// <returns>None.</returns>
	void SetDisplayName(const std::string displayName);

	/// <summary>
	/// Get the display name of bucket.
	/// </summary>
	/// <param>None</param>
	/// <returns>the display name of bucket.</returns>
	std::string GetDisplayName() const;

	/// <summary>
	/// Sets the bucket ID.
	/// </summary>
	/// <param name="displayName">IN : the bucket ID.</param>
	/// <returns>None.</returns>
	void SetBucketId(const DWORD id);

	/// <summary>
	/// Get the bucket ID.
	/// </summary>
	/// <param>None</param>
	/// <returns>the bucket ID.</returns>
	DWORD GetBucketId() const;

	/// <summary>
	/// Sets the bucket currency.
	/// </summary>
	/// <param name="displayName">IN : the bucket currency.</param>
	/// <returns>None.</returns>
	void SetBucketCurrency(const std::string bucketCurrency);

	/// <summary>
	/// Get the bucket currency.
	/// </summary>
	/// <param>None</param>
	/// <returns>the bucket currency.</returns>
	std::string GetBucketCurrency() const;

	/// <summary>
	/// Calculate the session points the parameters and session meter delta from each session object.
	/// </summary>
	/// <param name="subgameSessionMap">IN : Reference to SubGameSessionMap</param>
	/// <param name="currentGameSessionSeqNo">IN : the current game sequence number</param>
	/// <param name="isPartialBonusing">Is partial bonusing supported</param>
	/// <param name="pointsAdded">OUT: whether points are added</param>
	/// <returns>None.</returns>
	void CalculateSessionPointsAndCountdown(std::map<DWORD, CSubgameSession*> &subgameSessionMap,
											DWORD currentGameSessionSeqNo, bool isPartialBonusing, bool &pointsAdded);

protected:
	/// <summary>
	/// Calcualte session points from all subgame session.
	/// </summary>
	/// <param name="subgameSessionMap">IN : Reference to SubGameSessionMap</param>
	/// <param name="residualDelta"> IN/OUT: Residual delta calculated initially</param>
	/// <param name="residualMultiplier"> IN: The initial multiplier</param> 
	/// <param name="baseSessionPoints"> IN/OUT: base session points</param>
	/// <returns>None.</returns>
	void CalculateSessionPtsFromSubgameSessions(std::map<DWORD, CSubgameSession*> &subgameSessionMap,
		uint64_t &residualDelta, DWORD residualMultiplier, DWORD &baseSessionPoints);

	DWORD m_sessionPoints;       	// session points calculated
	DWORD m_baseSessionPoints;   	// Base session points calculated from the subgames
	DWORD m_initialSessionPoints;   // Initial session points read from NVRAM
	DWORD m_countdown;
	DWORD m_startingCountdown;
	DWORD m_startingCountdownPrecision;
	DWORD m_startingMultiplier;
	DWORD m_startingDivider;

	DWORD m_bucketId;
	std::string m_displayName;
	std::string m_bucketCurrency;
};
