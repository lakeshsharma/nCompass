#pragma once

#include "BucketSessionPoints.h"

// Forward declare class to declare friend for unit test access.
namespace SentinelUnitTests { class SessionPointsUnitTests; }

// Structure to pass bucket display information to web diagnostics.
struct BucketDisplayData
{
	DWORD id;
	std::string bucketName;
	std::string displayName;
	std::string currency;
	DWORD countdown;
	DWORD sessionPoints;

	BucketDisplayData()
	{
		id = 0;
		countdown = 0;
		sessionPoints = 0;
	}
};

// Class responsible for player cardin session tracking
// Derive from CBucketSessionPoints to implement and 
// enhance for legacy points bucket of OASIS 
class CSessionPoints : public CBucketSessionPoints
{
public:
	// friend class declarations for unit test access.
	friend class SentinelUnitTests::SessionPointsUnitTests;

	/// <summary>
	/// Default constructor.
	/// </summary>
	CSessionPoints();

	/// <summary>
	/// Copy Constructor
	/// SessionPoints constructor that builds from another SessionPoints object
	/// </summary>
	/// <param name="sessionPoints">IN - SessionPoint object.</param>
	CSessionPoints(const CSessionPoints &sessionPoints);

	/// <summary>
	/// Assignment operator.
	/// </summary>
	/// <param name="sessionPointsRHS">IN - The source for the assignment.</param>
	/// <returns>A reference to the target of the assigment.</returns>
	CSessionPoints & operator=(const CSessionPoints &sessionPointsRHS);

	/// <summary>
	/// Destructor.
	/// </summary>
	~CSessionPoints();

	/// <summary>
	/// Set Current Session Points for the player.
	/// </summary>
	/// <param name="points">IN - Points to be set.</param>
	/// <returns>void.</returns>
	void SetSessionPointsFlushed(const DWORD points);

	/// <summary>
	/// Get Current Session Points for the player.
	/// </summary>
	/// <param>void.</param>
	/// <returns>DWORD.</returns>
	DWORD GetSessionPointsFlushed() const;

	/// <summary>
	/// Get Bonus Events for the player.
	/// </summary>
	/// <param>void.</param>
	/// <returns>CBonusEvents pointer</returns>
	CBonusEvents * GetBonusEvents() const;

	/// <summary>
	/// Set Bonus Events for the player.
	/// </summary>
	/// <param name="events">CBonusEvents pointer</param>
	/// <returns>void</returns>
	void SetBonusEvents(CBonusEvents *events);

	/// <summary>
	/// Build session start meters from NVRAM, called by CPlayer class.
	/// </summary>
	/// <param>void</param>
	/// <returns>void</returns>
	void BuildSessionStartMeters();

	/// <summary>
	/// Get Session Start Meters Copy.
	/// </summary>
	/// <param>void</param>
	/// <returns>Return a copy of Session Start Meters.</returns>
	CMeters *GetSessionStartMetersCopy(void) const;

	/// <summary>
	/// Get Session Start Meters.
	/// </summary>
	/// <param>void</param>
	/// <returns>Return Session Start Meters.</returns>
	CMeters* GetSessionStartMeters(void) const;

	/// <summary>
	/// Set Session Start Meters.
	/// </summary>
	/// <param name="meters">IN - Meters to be set.</param>
	/// <returns>void.</returns>
	void SetSessionStartMeters(CMeters* meters);

	/// <summary>
	/// Get Current Session Start Meters.
	/// </summary>
	/// <param>void</param>
	/// <returns>Return Current Session Start Meters.</returns>
	CMeters* GetSessionStartMetersCurrent() const;

	/// <summary>
	/// Set Current Session Start Meters.
	/// </summary>
	/// <param name="meter">IN - Meter to be set.</param>
	/// <returns>void.</returns>
	void SetSessionStartMetersCurrent(CMeters* meter);

	/// <summary>
	/// Reset current version of "BonusEvents "
	/// </summary>
	/// <param name="meter">IN - Meter to be set.</param>
	/// <returns>void.</returns>
	void ResetCurrentBonusData();

	/// <summary>
	/// Get total Current Bonus Points.
	/// </summary>
	/// <param>void</param>
	/// <returns>Return total Current Bonus Points.</returns>
	DWORD GetBonusPointsCurrent() const;

	/// <summary>
	/// Get total Session Bonus Points.
	/// </summary>
	/// <param>void</param>
	/// <returns>Return total Bonus Points.</returns>
	DWORD GetBonusPoints() const;

	/// <summary>
	/// Sets the subgame session parameters when game/denom is changed.
	/// </summary>
	/// <param name="params">The subgame session parameters to be set with.</param>
	/// <returns>True if old SubgameSession was closed and new was created  succesfully otherwise false .</returns>
	bool SetGameSession(SubgameSessionParameters &params);

	/// <summary>
	/// Apply the subgame session parameters when response is received from poller 
	//  after changing game/denom.
	/// </summary>
	/// <param name="params">The subgame session parameters to be set with.</param>
	/// <param name="isPartialBonusing">Is partial bonusing supported</param>
	/// <param name="pointsAdded">whether points are added</param>	
	/// <returns>true if subgame params could be applied successfully otherwise false.</returns>
	bool ApplySubgameSessionParams(SubgameSessionParameters &params, bool isPartialBonusing, bool& pointsAdded);

	/// <summary>
	/// Game start event.
	/// </summary>
	/// <returns>True if it's first game start else false.</returns>
	bool GameStart();

	/// <summary>
	/// Apply the meter changes.
	/// </summary>
	/// <param name="meters">IN - current meters.</param> 
	/// <param name="origMeters">IN - original meters.</param>
	/// <param name="isPartialBonusing">Is partial bonusing supported</param>
	/// <param name="isMGMDEnabled">IN: Is MGMD enabled</param> 
	/// <param name="isBonusPointsAwarded">OUT: whether bonus points awarded</param>
	/// <param name="pointsAdded">OUT: whether points are added</param>
	/// <returns>true if applying meter delta successful else false.</returns>
	bool ApplyMeterChange(CMeters *meters, CMeters *origMeters, bool isPartialBonusing, bool isMGMDEnabled, bool &isBonusPointsAwarded, bool& pointsAdded);

	/// <summary>
	/// Get the current Subgame Session.
	/// </summary>
	/// <returns>Pointer to  the current Subgame Session</returns>
	CSubgameSession* GetCurrentSubgameSession();

	/// <summary>
	/// Gets the number of SubgameSession objects stored in the map.
	/// </summary>
	/// <returns>The number of SubgameSession objects stored in the map.</returns>
	DWORD GetSubgameSessionCount() const;

	/// <summary>
	/// Deactivate MGMD for this session. 
	/// </summary>
	void DeactivateMGMD();

	/// <summary>
	/// Create new subgame session.
	/// </summary>
	/// <param name="oldSubgameSession">Pointer to old subgame session</param>
	/// <returns>True if new game session created and added to map succesfully, otherwise false.</returns>
	bool CreateNewSubgameSession(SubgameSessionParameters &params);

	/// <summary>
	/// Award bonus points based upon the new session points.
	/// </summary>
	/// <param name="baseSessionPoints">base session points</param>
	/// <returns> Returns true if any bonus points have been awarded, otherwise false.</returns>	
	bool AwardBonusPoints(DWORD baseSessionPoints);

	/// <summary>
	/// Initializes countdown tracking for the passed bucket by creating CBucketSessionPoint instance for it,
	/// if not already created for this bucket.
	/// </summary>
	/// <param name="ebCountdownData">countdown data</param>
	/// <returns> Returns true if initialized, otherwise false.</returns>	
	bool InitializeExtendedBucketCountdownTracking(const ExtendedBucketsCountdownData *ebCountdownData);

	/// <summary>
	/// Calculate the session points the parameters and session meter delta from each session object.
	/// </summary>
	/// <param name="isPartialBonusing">Is partial bonusing supported</param>
	/// <param name="isBonusPointsAwarded">OUT: whether bonus points awarded</param>
	/// <param name="pointsAdded">OUT: whether points are added</param>
	/// <returns>None.</returns>
	void CalculateSessionPointsAndCountdown(bool isPartialBonusing, bool &isBonusPointsAwarded, bool& pointsAdded);

	/// <summary>
	/// Get active multiplier and divider.
	/// </summary>
	/// <param name="activeMultiplier"> OUT: Multiplier from active subgame session else starting multiplier</param>
	/// <param name="activeDivider"> OUT: Divider from active subgame session else starting divider</param>
	/// <returns>None.</returns>	
	void GetActiveMultiplierDivider(DWORD &activeMultiplier, DWORD &activeDivider);

	/// <summary>
	/// Get current bucket details from the bucket-level tracking list.
	/// </summary>
	/// <param name="id"> OUT: current bucket id</param>
	/// <param name="name"> OUT: current bucket name</param>
	/// <param name="currency"> OUT: current bucket currency</param>
	/// <param name="countdown"> OUT: current bucket countdown</param>
	/// <param name="sessionPoints"> OUT: current bucket session points</param>
	/// <returns>True on success. Otherwise returns false.</returns>	
	bool GetCurrentBucketDetails(DWORD &id, std::string &name, std::string &currency, DWORD &countdown, DWORD &sessionPoints) const;

	/// <summary>
	/// Move the index to next entry in the bucket-level tracking list.
	/// It resets itself when reaches end of list.
	/// </summary>
	void MoveToNextBucket();

	/// <summary>
	/// Return the number of extended buckets.
	/// </summary>
	/// <returns>The number of extended buckets available.</returns>
	WORD GetBucketCount() const;

	/// <summary>
	/// Gets the loyalty extended bucket display information from bucket-tracking list.
	/// </summary>
	/// <param name="data"> OUT: bucket display info</param>
	/// <param name="gameNumber"> OUT: game number of display information</param>
	/// <param name="denom"> OUT: denom of diaplay information</param>
	void GetBucketsDisplayData(std::vector<std::unique_ptr<BucketDisplayData>> &data, DWORD &gameNumber, int &denom) const;

	/// <summary>
	/// Get Session Points for the given bucket ID.
	/// </summary>
	/// <param name="bucketId"> IN: bucket id</param>
	/// <returns>DWORD.</returns>
	DWORD GetEBSessionPoints(const DWORD bucketId) const;

	/// <summary>
	/// Get countdown value for the given bucket ID.
	/// </summary>
	/// <param name="bucketId"> IN: bucket id</param>
	/// <returns>DWORD: Countdown value.</returns>
	DWORD GetEBSessionCountdown(const DWORD bucketId) const;

private:
	void CopyObj(const CSessionPoints &sessionPointsRHS);
	void CleanUp();

	/// <summary>
	/// Updates the bucket-level tracking list for current subgame.
	/// </summary>
	void RefreshSessionBucketsList();

	DWORD m_sessionPointsFlushed;
	CBonusEvents *m_bonusEvents;
	CMeters * volatile m_sessionStartMeters;
	CMeters * volatile m_sessionStartMetersCurrent;
	mutable std::recursive_mutex m_sessionStartMetersCritSection;
	bool m_firstPlay;
	bool m_deactivated;

	DWORD m_lastPlayedSubgameNumber;
	DWORD m_lastPlayedSubgameDenom;

	// map to hold references to all the subgame sessions: active or others
	std::map<DWORD, CSubgameSession*> m_subgameSessionMap;  // key is game session sequence number

	// pointer to hold the current active game session
	CSubgameSession *m_currentSubgameSession;

	// map to hold references to session points object for loyalty extended buckets
	std::map<DWORD, CBucketSessionPoints*> m_ebSessionPointsMap;  // key is bucket ID

	// bucket-level tracking for current subgame, includes legacy points bucket if applicable
	vector<CBucketSessionPoints*> m_sessionBucketsList;
	DWORD m_sessionBucketsListIdx;
	mutable std::recursive_mutex m_sessionBucketsListCriticalSection;
};




