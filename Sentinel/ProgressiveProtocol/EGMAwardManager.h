#pragma once

#include "InternalMessage.h"
#include "EGMAwardManagerState.h"
#include "EGMAwardMessageSource.h"
#include "IStandardProgressiveWins.h"
#include "StandardProgressiveWins.h"
#include "CumulativeProgressiveManager.h"
#include "ProgressiveLevelManager.h"
#include "CountHistory.h"
#include "MsgTargetPair.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class EGMAwardManager; }

class CEGMAwardManager
{
public:
	/// <summary>
	/// Standard/default constructor. Initializes a new instance of the <see cref="CEGMAward"/> class.
	/// <param name="progressiveLevelManager">IN - A reference to a caller provided volatile ProgressiveLevelManager instance.</param>
	/// <param name="memoryType">IN - The type of persistent memory to use for the instance.</param>
	/// <param name="bBuildFromNVRAM">IN - true to build the instance from NVRAM, false otherwise.</param>
	/// </summary>
	CEGMAwardManager(const ProgressiveLevelManager& progressiveLevelManager, MEMORY_TYPE memoryType = MEMORY_NVRAM, bool buildFromNvram = true);

	/// <summary>
	/// Starts the instance.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	void Start(DWORD tickCount);

	/// <summary>
	/// Gets a copy of the EGM award.
	/// </summary>
	/// <returns>A pointer to a new CEGMAward instance.  MUST NOT return NULL.</returns>
	/// <remarks>
	/// The caller is responsible for deleting the returned instance.
	/// </remarks>
	CEGMAward* GetEGMAwardCopy() const;

	/// <summary>
	/// Gets a copy of the EGM award manager state.
	/// </summary>
	/// <returns>A copy of the EGM award manager state.</returns>
	CEGMAwardManagerState GetEGMAwardManagerState() const;

	/// <summary>
	/// Processes an incoming message or event and optionally generates an outgoing message.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="systemTime">IN - The current system time.</param>
	/// <param name="games">Games information, denomination, transfer capabilities, etc.</param>
	/// <param name="progressiveDown">IN - true if we are displaying a progressive down message to players, false otherwise.</param>
	/// <param name="msg">IN - The incoming message or event.</param>
	/// <param name="source">IN - the message queue source for msg.  Use mqNone to perform timed events.</param>
	/// <param name="targetMsg">OUT - The target message.  NULL if no target message.</param>
	/// <param name="target">OUT - The message queue target for targetMsg.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	/// <param name="gameOnline">IN - true if the egm is currently online, false otherwise.</param>
	void ProcessMessage(DWORD tickCount,
		CGames& games,
		bool progressiveDown,
		CInternalMessage* msg,
		MessageQueueId source,
		CInternalMessage*& targetMsg,
		MessageQueueId& target,
		std::vector<MsgTargetPair*> *returnMessages,
		bool gameOnline = true);

	/// <summary>
	/// Sets the jackpot external bonus feature.
	/// </summary>
	/// <param name="enabled">if set to <c>true</c> [enabled].</param>
	void SetJackpotExternalBonusFeature(bool enabled);

	/// <summary>
	/// Returns the unlock in progress flag.
	/// </summary>
	bool UnlockInProgress() const;

	/// <summary>
	/// Sets the unlock in progress flag.
	/// </summary>
	/// <param name="enabled">if set to <c>true</c> [enabled].</param>
	void SetUnlockInProgress(bool enabled);

	/// <summary>
	/// Returns the PrizeClearedByPbt flag.
	/// </summary>
	bool PrizeClearedByPbt() const;

	/// <summary>
	/// Sets the PrizeClearedByPbt flag.
	/// </summary>
	/// <param name="value">if set to <c>true</c> [value].</param>
	void SetPrizeClearedByPbt(bool value);

	/// <summary>
	/// Sets the Progressive Wins list.
	/// </summary>
	/// <param name="progressiveWins">IN - The progressive wins list.</param>
	/// <remarks>
	/// The Progressive wins is automatically setup in the constructor this is to override that.
	/// </remarks>
	void SetProgressiveWins(std::shared_ptr<IStandardProgressiveWins> progressiveWins);

	/// <summary>
	/// Gets information about Standard SAS Progressive Wins.
	/// </summary>
	/// <param name="lastStandardWin">OUT - The last Standard SAS progressive win processed.</param>
	/// <param name="standardWins">OUT - The list of Standard SAS progressive wins currently being processed.</param>
	void GetStandardWins(std::shared_ptr<CEGMAward> &lastStandardWin,
		std::vector<std::shared_ptr<CEGMAward>> &standardWins);

	/// <summary>
	/// Sets the cumulative progressive manager
	/// </summary>
	/// <param name="cumulativeProgressiveManager">IN - The Cumulative progressive Manager.</param>
	/// <remarks>
	/// The cumulative progressive manager is automatically setup in the constructor this is to override that.
	/// </remarks>
	void SetCumulativeProgressiveManager(std::shared_ptr<ICumulativeProgressiveManager> cumulativeProgressiveManager);

	/// <summary>
	/// Checks to see if Prize Award is active.
	/// </summary>
	/// <param name="jackpotId">OUT - The jackpot ID.</param>
	/// <returns>Returns true if prize award is active, false otherwise.</returns>
	bool IsPrizeAwardActive(int64_t& jackpotId) const;
	  
	/// <summary>
	/// Adds pending promo jackpot id
	/// </summary>
	/// <param name="jackpotId">OUT - The jackpot ID.</param>
	void AddPendingPromoJackpotId(int64_t jackpotId);

private:
	CEGMAwardManager(const CEGMAwardManager &rhs) = delete;
	void operator=(const CEGMAwardManager &rhs) = delete;

	static const __int64 SAS_ROLLOVER_VALUES[];

	std::shared_ptr<IStandardProgressiveWins> m_standardProgressiveWins;			//stores the IStandardProgressiveWins class
	std::shared_ptr<ICumulativeProgressiveManager> m_cumulativeProgressiveManager;	//stores the cumulative progressive manager

	CEGMAward m_egmAward;
	CEGMAwardManagerState m_state;

	// From features flag
	bool m_jackpotExternalBonus;

	bool m_unlockInProgress;
	bool m_prizeClearedByPbt;
	std::unordered_set<int64_t> m_pendingPromoJackpotIds;

	static const DWORD LOG_TICK_COUNT_EXPIRED; // For periodic diagnostic logging.
	DWORD m_logTickCountStart; // For periodic diagnostic logging.

	/// <summary>
	/// Gets the message.
	/// Gets a message with the current state/status to send to the progressive protocol.
	/// </summary>
	/// <returns>A non-NULL pointer to a message with the current state/status to send to the progressive protocol.</returns>
	CInternalMessage* GetMessage();

	/// <summary>
	/// Processes an incoming message from the progressive system and optionally generates an outgoing message.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="games">Games information, denomination, transfer capabilities, etc.</param>
	/// <param name="progressiveDown">IN - true if we are displaying a progressive down message to players, false otherwise.</param>
	/// <param name="msg">IN - The incoming message or event.</param>
	/// <param name="targetMsg">IN/OUT - The target message.  NULL if no target message.  Should be NULL on entry.</param>
	/// <param name="target">IN/OUT - The message queue target for targetMsg.  Should be mqNone on entry.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	/// <param name="gameOnline">IN - true if the egm is currently online, false otherwise.</param>
	void ProcessMessageFromProgressive(DWORD tickCount,
		CGames& games,
		bool progressiveDown,
		CInternalMessage* msg,
		CInternalMessage*& targetMsg,
		MessageQueueId& target,
		std::vector<MsgTargetPair*> *returnMessages,
		bool gameOnline);

	/// <summary>
	/// Processes a CEGMAward from the progressive protocol and optionally generates an outgoing message.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="games">Games information, denomination, transfer capabilities, etc.</param>
	/// <param name="progressiveDown">IN - true if we are displaying a progressive down message to players, false otherwise.</param>
	/// <param name="egmAwardHit">IN/OUT - The incoming award instance.</param>
	/// <param name="targetMsg">IN/OUT - The target message.  NULL if no target message.  Should be NULL on entry.</param>
	/// <param name="target">IN/OUT - The message queue target for targetMsg.  Should be mqNone on entry.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	/// <param name="gameOnline">IN - true if the egm is currently online, false otherwise.</param>
	void ProcessEGMAwardFromProgressive(DWORD tickCount,
		CGames& games,
		bool progressiveDown,
		CEGMAward& egmAwardHit,
		CInternalMessage*& targetMsg, 
		MessageQueueId& target,
		std::vector<MsgTargetPair*> *returnMessages,
		bool gameOnline);

	/// <summary>
	/// Processes an bonus award from the progressive protocol and optionally generates an outgoing message.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="games">Games information, denomination, transfer capabilities, etc.</param>
	/// <param name="progressiveDown">IN - true if we are displaying a progressive down message to players, false otherwise.</param>
	/// <param name="egmAwardHit">IN/OUT - The incoming award instance.</param>
	/// <param name="targetMsg">IN/OUT - The target message.  NULL if no target message.  Should be NULL on entry.</param>
	/// <param name="target">IN/OUT - The message queue target for targetMsg.  Should be mqNone on entry.</param>
	/// <param name="gameOnline">IN - true if the egm is currently online, false otherwise.</param>
	void ProcessEGMAwardBonusFromProgressive(DWORD tickCount,
		CGames &games,
		bool progressiveDown,
		CEGMAward &egmAwardHit,
		CInternalMessage *&targetMsg,
		MessageQueueId &target,
		bool gameOnline);

	/// <summary>
	/// Processes an incoming message from the GCF and optionally generates an outgoing message.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="games">Games information, denomination, transfer capabilities, etc.</param>
	/// <param name="msg">IN - The incoming message or event.</param>
	/// <param name="targetMsg">IN/OUT - The target message.  NULL if no target message.  Should be NULL on entry.</param>
	/// <param name="target">IN/OUT - The message queue target for targetMsg.  Should be mqNone on entry.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	/// <param name="progressiveDown">IN - Status of the progressive system we are talking to.</param>
	void ProcessMessageFromGCF(DWORD tickCount,
		CGames& games,
		CInternalMessage* msg,
		CInternalMessage*& targetMsg,
		MessageQueueId& target,
		std::vector<MsgTargetPair*> *returnMessages,
		bool progressiveDown);

	/// <summary>
	/// Returns a non-negative meter value delta for an unknown SAS BCD meter size.
	/// </summary>
	/// <param name="currentMeterValue">IN - The current meter value.</param>
	/// <param name="previousMeterValue">IN - The previous meter value.</param>
	/// <returns>A non-negative meter value delta.</returns>
	static __int64 GetSasMeterDelta(
		__int64 currentMeterValue,
		__int64 previousMeterValue);

	/// <summary>
	/// Processes the progressive win hit. Which is really just telling GCF to poll
	/// </summary>
	/// <param name="msg">The MSG.</param>
	/// <param name="targetMsg">The target MSG.</param>
	/// <param name="target">The target.</param>
	void ProcessProgressiveWinHit(
		CInternalMessage *msg,
		std::vector<MsgTargetPair*> *returnMessages);
	
	/// <summary>
	/// Processes the progressive win hit paid to EGM
	/// </summary>
	/// <param name="msg">The MSG.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	void ProcessProgressiveWinHitPaidToEGM(
		CInternalMessage *msg,
		std::vector<MsgTargetPair*> *returnMessages);

	/// <summary>
	/// Processes the progressive win. Which is really just telling GCF to poll
	/// </summary>
	/// <param name="msg">The MSG.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	void ProcessProgressiveWin(
		CInternalMessage *msg,
		std::vector<MsgTargetPair*> *returnMessages);

	/// <summary>
	/// Processes the hand pay reset.
	/// </summary>
	/// <param name="msg">The MSG.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	/// <param name="fromGameStart">IN - Used when a gamestart causes a reset</param>
	void ProcessHandPayReset(
		std::vector<MsgTargetPair*> *returnMessages,
		bool fromGameStart = false);

	/// <summary>
	/// Processes the cumulative progressive win  meter from GCF.
	/// </summary>
	/// <param name="msg">IN - The incoming message or event.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	void ProcessCumulativeProgressiveWinFromGCF(
		CInternalMessage *msg,
		std::vector<MsgTargetPair*> *returnMessages);

	/// <summary>
	/// Processes an incoming set meters message from the GCF and optionally generates an outgoing message.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="games">Games information, denomination, transfer capabilities, etc.</param>
	/// <param name="msg">IN - The incoming message or event.</param>
	/// <param name="targetMsg">IN/OUT - The target message.  NULL if no target message.  Should be NULL on entry.</param>
	/// <param name="target">IN/OUT - The message queue target for targetMsg.  Should be mqNone on entry.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	/// <remarks>
	/// Caller requirements: msg != NULL && msg->GetMessageType() == INT_MSG_SET_METERS.
	/// </remarks>
	void ProcessSetMetersMessageFromGCF(DWORD tickCount,
		CGames& games,
		CInternalMessage* msg,
		CInternalMessage*& targetMsg,
		MessageQueueId& target,
		std::vector<MsgTargetPair*> *returnMessages);

	/// <summary>
	/// Processes an incoming jackpot info message from the GCF.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="games">Games information, denomination, transfer capabilities, etc.</param>
	/// <param name="msg">IN - The incoming message or event.</param>
	/// <param name="targetMsg">IN/OUT - The target message.  NULL if no target message.  Should be NULL on entry.</param>
	/// <param name="target">IN/OUT - The message queue target for targetMsg.  Should be mqNone on entry.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	/// <remarks>
	/// Caller requirements: msg != NULL && msg->GetMessageType() == INT_MSG_SET_METERS.
	/// </remarks>
	void ProcessJackpotInfoMessageFromGCF(DWORD tickCount,
		CGames& games,
		CInternalMessage* msg,
		CInternalMessage*& targetMsg,
		MessageQueueId& target,
		std::vector<MsgTargetPair*> *returnMessages);

	/// <summary>
	/// Gets the transfer type for the next transfer attempt.
	/// </summary>
	/// <param name="games">Games information, denomination, transfer capabilities, etc.</param>
	/// <returns>The transfer type for the next transfer attempt.</returns>
	/// <remarks>
	/// A return value of EGMAwardTransferType_None indicates that a transfer should not be attempted (at this time).
	/// </remarks>
	static EGMAwardTransferType GetNextTransferType(CGames& games);

	/// <summary>
	/// Processes a CEGMAward from the GCF and optionally generates an outgoing message.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="games">Games information, denomination, transfer capabilities, etc.</param>
	/// <param name="egmAwardUpdate">IN - The incoming award instance.</param>
	/// <param name="targetMsg">IN/OUT - The target message.  NULL if no target message.  Should be NULL on entry.</param>
	/// <param name="target">IN/OUT - The message queue target for targetMsg.</param>
	void ProcessEGMAwardFromGCF(DWORD tickCount,
		CGames& games,
		const CEGMAward& egmAwardUpdate,
		CInternalMessage*& targetMsg,
		MessageQueueId& target);

	/// <summary>
	/// Performs timed events and optionally generates an outgoing message.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="games">Games information, denomination, transfer capabilities, etc.</param>
	/// <param name="targetMsg">IN/OUT - The target message.  NULL if no target message.  Should be NULL on entry.</param>
	/// <param name="target">IN/OUT - The message queue target for targetMsg.  Should be mqNone on entry.</param>
	/// <param name="gameOnline">IN - true if the egm is currently online, false otherwise.</param>
	void PerformTimedEvents(DWORD tickCount,
		CGames& games,
		CInternalMessage*& targetMsg,
		MessageQueueId& target,
		bool gameOnline);

	/// <summary>
	/// Performs timed events and optionally generates an outgoing message.
	/// Call only when not waiting for GCF.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="games">Games information, denomination, transfer capabilities, etc.</param>
	/// <param name="targetMsg">IN/OUT - The target message.  NULL if no target message.  Should be NULL on entry.</param>
	/// <param name="target">IN/OUT - The message queue target for targetMsg.  Should be mqNone on entry.</param>
	/// <param name="gameOnline">IN - true if the egm is currently online, false otherwise.</param>
	void PerformTimedEventsNotWaiting(DWORD tickCount,
		CGames& games,
		CInternalMessage*& targetMsg,
		MessageQueueId& target,
		bool gameOnline);

	/// <summary>
	/// Determines whether an EGM award is in progress.
	/// </summary>
	/// <returns>true if an EGM award is in progress, false otherwise.</returns>
	bool IsEGMAwardInProgress() const;

	/// <summary>
	/// Determines whether current bonus level information matches the current session information.
	/// </summary>
	/// <returns>true if current bonus level information matches the current session information, false otherwise.</returns>
	/// <remarks>
	/// Note: An active session is not needed for a match.
	/// </remarks>
	//bool DoBonusLevelsMatchSession() const;
    const ProgressiveLevelManager& m_progressiveLevelManager;
	void ProcessGameStartFromGCF(DWORD tickCount,
		CGames& games,
		CInternalMessage*& targetMsg,
		MessageQueueId& target);
	
	void CheckIfPrizeAwardLockTimeNeedsToBeRefreshed(CInternalMessage*& targetMsg, MessageQueueId& target);

	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::EGMAwardManager;
};

