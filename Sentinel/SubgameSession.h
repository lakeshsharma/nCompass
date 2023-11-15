#pragma once

#include "SubgameSessionParameters.h"
#include "Meters.h"

enum class SubgameSessionStatus
{
	ActiveWaitingForParameters,
	Active,
	Closed,
	ClosedWaitingForParameters
};



class CSubgameSession
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	/// <param name="subgameSessionParameters">IN - subgame session parameters</param>
	CSubgameSession(const SubgameSessionParameters& subgameSessionParameters);
	
	/// <summary>
	/// Destructor.
	/// </summary>
	~CSubgameSession();
	
	/// <summary>
	/// Apply the subgame session parameters.
	/// </summary>
	/// <param name="subgameSessionParameters">IN - subgame session parameters.</param>
	/// <returns>true if subgame session parameters are applied, false otherwise.</returns>
	bool ApplySubgameSessionParameters(const SubgameSessionParameters& subgameSessionParameters);

	/// <summary>
	/// Apply the subgame session parameters.
	/// </summary>
	/// <param name="meters">IN - current meters.</param> 
	/// <param name="origMeters">IN - original meters.</param>
	/// <returns>true if meter change is applied, false otherwise.</returns>
	bool ApplyMeterChange(CMeters *meters, CMeters *origMeters);
	
	/// <summary>
	/// Close session.
	/// </summary>
	/// <param name="gameSessionSequenceNumber">IN - gameSession sequence number.</param>
	/// <returns>true if sequence number matches the session's sequence number, false otherwise.</returns>
	bool CloseSession(DWORD gameSessionSequenceNumber);

	/// <summary>
	/// Increment session game start count.
	/// </summary>
	/// <returns>true if first game start and session is active, false otherwise.</returns>
	bool IncrementGameStart();

	/// <summary>
	/// Get meter delta.
	/// </summary>
	/// <param name="tilted">OUT - invalid meter deltas require tilt to be sent to poller.</param>
	/// <param name="coinInOverride">IN - consider CoinIn as meter whose delta is to be returned.</param>
	/// <returns>meter delta</returns>
	DWORD GetMeterDelta(bool &tilted, bool coinInOverride = false) const;

	/// <summary>
	/// Get subgame session parameters.
	/// </summary>
	/// <returns>subgame session parameters</returns>
	const SubgameSessionParameters & GetSubgameSessionParameters();

	/// <summary>
	/// Get subgame session status.
	/// </summary>
	/// <returns>subgame session status</returns>
	void SetStatus(SubgameSessionStatus status);

	/// <summary>
	/// Get subgame session status.
	/// </summary>
	/// <returns>subgame session status</returns>
	SubgameSessionStatus GetStatus() const;

	/// <summary> Get game start count. 
	/// </summary>
	/// <returns>subgame start count</returns>
	int GetGameStartCount() const;

	/// <summary>
	/// Is same game number and denom as params
	/// </summary>
	/// <param name="params">IN - SubgameSessionparameters 
	/// containing game number and denom.</param> 
	/// <returns>true if same game.</returns>
	bool IsSameGameNumberAndDenom(CSubgameSession *params) const;

protected:
	SubgameSessionStatus m_status;

	SubgameSessionParameters m_subgameSessionParameters;

	std::map<byte,DWORD> m_subgameSessionMeterDeltas;

	int m_sessionGameStartCount;
};




