#pragma once
#include "Meters.h"
#include "SASPollMessage.h"
#include "InternalMessage.h"
#include "MeterStepdown.h"
#include "SASInterface.h"

class CSASMeterCollection
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CSASMeterCollection" /> class.
	/// </summary>
	/// <param name="meters">The meters to collect.</param>
	/// <param name="gameNumber">The game number.</param>
	/// <param name="bInSnapshot">if set to <c>true</c> [b in snapshot].</param>
	/// <param name="tryOnce">if set to <c>true</c> [try once].</param>
	CSASMeterCollection(CSASInterface *sas, CMeters *meters, WORD gameNumber, WORD denom, bool bInSnapshot, bool tryOnce, CMeterStepdown *stepdown, bool isIgtS2000);

	~CSASMeterCollection();

	/// <summary>
	/// Collects the meters
	/// </summary>
	/// <param name="sasCallback">Callback function to send poll to SAS and wait for response</param>
	/// <param name="intCallback">Callback function to send messages to internals</param>
	/// <returns>Number of meters collected</returns>
	/// <remarks>
	/// Ananymous callback function will need access to "this" in capture list
	/// </remarks>
	int CollectMeters(const std::function<void(CSASPollMessage *, int priority)>& sasCallback, const std::function<void(CInternalMessage *msg, int priority)>& intCallback);

private:
	CMeters *m_meters;
	CMeters *m_currentPollMeters;
	int m_numberOfMeters;
	WORD m_targetMeterCount;
	WORD m_gameNumber;
	WORD m_denomMult;
	bool m_inSnapshot;
	bool m_tryOnce;
	std::vector<byte> m_triedMeters;
	CMeterStepdown *m_stepdown;
	bool m_isIgtS2000;
	int m_tryCount;
	CSASInterface *m_sas;
	int m_meterIndexToStart;
	std::vector<int> m_failedPolls;
	int m_consecutiveFailedAttempts;

	std::function<void(CSASPollMessage *, int priority)> m_sasCallback;
	std::function<void(CInternalMessage *, int priority)> m_intCallback;

	bool DoNextLongPoll();
	int GetMetersMessageForLongPoll(int longpoll, CSASPollMessage *&msg, CMeters *meters);
	void ParseResults(CSASPollMessage *msg);
	bool IsIndividualBillLP(BYTE lp);
};

