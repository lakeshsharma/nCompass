#pragma once

#include "IStandardProgressiveWins.h"
#include "Logging/LogString.h"
#include "MsgTargetPair.h"
#include "Utilities.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class StandardProgressiveWinsTests; }

class StandardProgressiveWins : public IStandardProgressiveWins
{
public:
	// NVRAM memory storage parameters for m_lastAwardProcessed.
	static const DWORD NVRAM_PREV_AWARD_DATA_OFFSET = NVRAM_LAST_STD_AWARD_DATA_OFFSET;
	static const DWORD NVRAM_PREV_AWARD_DATA_SIZE = NVRAM_LAST_STD_AWARD_DATA_SIZE;
	static const DWORD NVRAM_PREV_AWARD_ACTIVE_OFFSET = NVRAM_LAST_STD_AWARD_CHECKBYTE_OFFSET;
	static const BYTE NVRAM_PREV_AWARD_ACTIVE_VALUE = NVRAM_LAST_STD_AWARD_CHECKBYTE_VALUE;

	/// <summary>
	/// Initializes a new instance of the <see cref="StandardProgressiveWins"/> class.
	/// <param name="bBuildFromNVRAM">IN - true to build the instance from NVRAM, false otherwise.</param>
	/// </summary>
	StandardProgressiveWins(bool buildFromNvram = false);

	/// <summary>
	/// Finalizes an instance of the <see cref="StandardProgressiveWins"/> class.
	/// </summary>
	~StandardProgressiveWins();

	/// <summary>
	/// Sets the check byte locations to indicate that there are no active records.
	/// </summary>
	/// <returns></returns>
	static void SetInactiveLocation();

	/// <summary>
	/// Determines whether [has unsent progressive wins].
	/// </summary>
	/// <returns>true if there are unsent items in the list.</returns>
	bool HasUnsentProgressiveWins();

	/// <summary>
	/// Adds the progressive win to the list of awards.
	/// </summary>
	/// <param name="award">The award.</param>
	/// <returns>true if the award was able to go on the list.</returns>
	bool AddProgressiveWin(std::shared_ptr<CEGMAward> award);

	/// <summary>
	/// Marks the state.
	/// </summary>
	/// <param name="award">The award.</param>
	/// <param name="state">The state.</param>
	/// <returns>true if it found the award and marked it state</returns>
	bool MarkState(std::shared_ptr<CEGMAward> award, EGMAwardState state);

	/// <summary>
	/// Processes an standard (SAS) progressive hit acknowledgment from the progressive protocol and optionally generates an outgoing message.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="games">Games information, denomination, transfer capabilities, etc.</param>
	/// <param name="progressiveDown">IN - true if we are displaying a progressive down message to players, false otherwise.</param>
	/// <param name="egmAwardHit">IN - IN - The incoming award instance.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	void ProcessEGMAwardStandardFromProgressive(DWORD tickCount,
		CGames& games,
		bool progressiveDown,
		const CEGMAward& egmAwardHit, 
		std::vector<MsgTargetPair*> *returnMessages);

	/// <summary>
	/// This tells the system that the progressive was paid kind of.
	/// </summary>
	/// <param name="egmAwardHit">The egm award hit.</param>
	void ProgressivePaidSentReceived(std::shared_ptr<CEGMAward> egmAwardHit);

	/// <summary>
	/// Gets a copy of the last standard SAS progressive win processed.
	/// </summary>
	/// <param name="lastStandardWin">OUT - A copy of the last standard SAS progressive win processed.</param>
	/// <remarks>
	/// Creates a deep copy, so the original values can be modifed while the copy is being used.
	/// </remarks>
	virtual void GetLastStandardWin(std::shared_ptr<CEGMAward> &lastStandardWin);

	/// <summary>
	/// Gets a copy of the list of standard SAS progressive win currently being processed.
	/// </summary>
	/// <param name="standardWins">OUT - A copy of the list of standard SAS progressive wins currently being processed.</param>
	/// <remarks>
	/// Creates a deep copy, so the original values can be modifed while the copy is being used.
	/// </remarks>
	virtual void GetStandardWins(std::vector<std::shared_ptr<CEGMAward>> &standardWins);

	/// <summary>
	/// Processes the egm award with no egm paid signaled.
	/// </summary>
	/// <param name="pendingAward">The pending award.</param>
	/// <param name="egmAwardHit">The egm award hit.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	void ProcessEGMAwardWithNoEGMPaidSignaled(std::shared_ptr<CEGMAward> pendingAward, const CEGMAward &egmAwardHit, std::vector<MsgTargetPair*> *returnMessages);

	/// <summary>
	/// Processes the egm award with egm paid signaled.
	/// </summary>
	/// <param name="pendingAward">The pending award.</param>
	/// <param name="egmAwardHit">The egm award hit.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	void ProcessEGMAwardWithEGMPaidSignaled(std::shared_ptr<CEGMAward> pendingAward, const CEGMAward &egmAwardHit, std::vector<MsgTargetPair*> *returnMessages);
	
	/// <summary>
	/// Sets the hand pay pending.
	/// </summary>
	/// <param name="awardAmount">The award amount.</param>
	/// <param name="levelId">The level identifier.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	void SetHandPayPending(__int64 awardAmount, int levelId, std::vector<MsgTargetPair*> *returnMessages);

	/// <summary>
	/// Determines whether [is this a duplicate] [the specified award amount].
	/// </summary>
	/// <param name="awardAmount">The award amount.</param>
	/// <param name="sasLevelId">The sas level identifier.</param>
	/// <returns>true if it is a duplicate</returns>
	bool IsThisADuplicate(__int64 awardAmount, int sasLevelId);

	/// <summary>
	/// Sets the Hand Pay to Reset
	/// </summary>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	/// <param name="fromGameStart">IN - Used when a gamestart causes a reset</param>
	void SetHandPayReset(std::vector<MsgTargetPair*> *returnMessages, bool fromGameStart = false);

	/// <summary>
	/// Sets the progressive paid by egm.
	/// </summary>
	/// <param name="Award">The award.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	void SetProgressivePaidByEGM(std::shared_ptr<CEGMAward> Award, std::vector<MsgTargetPair*> *returnMessages);

	/// <summary>
	/// Sets the progressive win. Usually from a long poll 84 or 85
	/// </summary>
	/// <param name="msg">The MSG.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	/// <param name="progressiveLevelId">The SAS level.</param>
	void SetProgressiveWin(CInternalMessage *msg, std::vector<MsgTargetPair*> *returnMessages, int64_t progressiveLevelId);

	/// <summary>
	/// Pulses the specified tick count for the standard progressive.
	/// </summary>
	/// <param name="tickCount">The tick count.</param>
	/// <param name="progressiveDown">if set to <c>true</c> [progressive down].</param>
	/// <param name="returnMessages">The return messages.</param>
	void Pulse(DWORD tickCount, bool progressiveDown, std::vector<MsgTargetPair*> *returnMessages);

	/// <summary>
	/// Builds the award resend list.
	/// </summary>
	/// <param name="returnMessages">The return messages.</param>
	void BuildAwardResendList(std::vector<MsgTargetPair*> * returnMessages);

protected:
	std::shared_ptr<CEGMAward> m_lastAwardProcessed;

	/// <summary>
	/// Holds the awards.
	/// </summary>
	std::vector<std::shared_ptr<CEGMAward>> m_Awards;

	/// <summary>
	/// Holds the available NVRAM active offset for each persisted award instance.
	/// Use a queue (for reusing offsets) to maximize the amount of persisted history for post-mortems.
	/// 2015-07-08 Allow up to NVRAM_EGM_AWARD_STANDARD_DATA_COUNT instances persisted at one time.
	/// </summary>
	std::queue<DWORD> m_availableNvramActiveOffsets;

	/// <summary>
	/// Builds the instance from NVRAM.
	/// </summary>
	void BuildYourself(bool buildFromNvram);

	/// <summary>
	/// Builds the m_lastAwardProcessed instance from NVRAM.
	/// </summary>
	void BuildLastAwardProcessed(bool buildFromNvram);

	/// <summary>
	/// Finds the award.
	/// </summary>
	/// <param name="awardAmount">The award amount.</param>
	/// <param name="levelId">The level identifier.</param>
	/// <returns>if it finds it will return the shared pointer to an egm award</returns>
	virtual std::shared_ptr<CEGMAward> FindAward(__int64 awardAmount, int levelId);

	/// <summary>
	/// Finds the award by SAS level id.
	/// </summary>
	/// <param name="awardAmount">The award amount.</param>
	/// <param name="sasLevelId">The level identifier.</param>
	/// <returns>if it finds it will return the shared pointer to an egm award</returns>
	std::shared_ptr<CEGMAward> FindAwardBySasLevelID(__int64 awardAmount, int levelId);

	/// <summary>
	/// Removes the award.
	/// </summary>
	/// <param name="awardAmount">The award amount.</param>
	/// <param name="levelId">The level identifier.</param>
	virtual void RemoveAward(__int64 awardAmount, int levelId);

	/// <summary>
	/// Removes the award.
	/// </summary>
	/// <param name="it">IN - An m_Award iterator.</param>
	/// <returns>The m_Award iterator returned by m_Award.erase(), or m_Award.cend().</returns>
	std::vector<std::shared_ptr<CEGMAward>>::const_iterator RemoveAward(std::vector<std::shared_ptr<CEGMAward>>::const_iterator it);

	/// <summary>
	/// Gets the next NVRAM active offset for storing CEGMAward instances.
	/// </summary>
	/// <returns>The next NVRAM active offset for storing CEGMAward instances.
	/// Returns CMemoryStorage::NVRAM_DEFAULT_OFFSET if there are no NVRAM active offsets available.</returns>
	DWORD GetNextNvramActiveOffset();

	/// <summary>
	/// Recovers the NVRAM active offset (if any) from the egmAward instance.
	/// </summary>
	/// <param name="egmAward">IN/OUT - The EGM award.  MAY contain a nullptr.</param>
	/// <remarks>
	/// If the award has an NVRAM active offset:
	///     The award record in NVRAM is marked as inactive.
	///     The recovered NVRAM active offset is added back to m_nvramActiveOffsets.
	/// </remarks>
	void RecoverNvramActiveOffset(std::shared_ptr<CEGMAward> egmAward);

	/// <summary>
	/// The length of time we wait to retry/resend queue items.
	/// </summary>
	static const DWORD QUEUE_RETRY_EXPIRED;

	/// <summary>
	/// the current retry tick count for testing if elapsed or not.
	/// </summary>
	DWORD m_queueRetryTickCountStart;

	/// <summary>
	/// Gets the retry tick count start.
	/// </summary>
	/// <returns></returns>
	DWORD GetRetryTickCountStart();

	/// <summary>
	/// Sets the retry tick count start.
	/// </summary>
	/// <param name="retryCountStart">The retry count start.</param>
	void SetRetryTickCountStart(DWORD retryCountStart);

	/// <summary>
	/// The length of time we wait to retry/resend queue items that have been hit and not returned.
	/// </summary>
	static const DWORD QUEUE_HIT_PUNT_EXPIRED;

	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::StandardProgressiveWinsTests;
};

