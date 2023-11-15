#pragma once

#include "InternalMessage.h"
#include "EGMAwardMessageSource.h"
#include "EGMAward.h"
#include "MsgTargetPair.h"
#include "ProgressiveLevel.h"

class IStandardProgressiveWins
{
public:

	/// <summary>
	/// Determines whether [has unsent progressive wins].
	/// </summary>
	/// <returns>true if there are unsent items in the list.</returns>
	virtual bool HasUnsentProgressiveWins() = 0;

	/// <summary>
	/// Adds the progressive win to the list of awards.
	/// </summary>
	/// <param name="award">The award.</param>
	/// <returns>true if the award was able to go on the list.</returns>
	virtual bool AddProgressiveWin(std::shared_ptr<CEGMAward>award) = 0;

	/// <summary>
	/// Marks the state.
	/// </summary>
	/// <param name="award">The award.</param>
	/// <param name="state">The state.</param>
	/// <returns>true if it found the award and marked it state</returns>
	virtual bool MarkState(std::shared_ptr<CEGMAward> award, EGMAwardState state) = 0;

	/// <summary>
	/// Determines whether [is this a duplicate] [the specified award amount].
	/// </summary>
	/// <param name="awardAmount">The award amount.</param>
	/// <param name="sasLevelId">The sas level identifier.</param>
	/// <returns>true if it is a duplicate</returns>
	virtual bool IsThisADuplicate(__int64 awardAmount, int sasLevelId) = 0;

	/// <summary>
	/// Sets the Hand Pay to Reset
	/// </summary>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	/// <param name="fromGameStart">IN - Used when a gamestart causes a reset</param>
	virtual void SetHandPayReset(std::vector<MsgTargetPair*> *returnMessages, bool fromGameStart) = 0;

	/// <summary>
	/// Pulses the specified tick count for the standard progressive.
	/// </summary>
	/// <param name="tickCount">The tick count.</param>
	/// <param name="progressiveDown">if set to <c>true</c> [progressive down].</param>
	/// <param name="returnMessages">The return messages.</param>
	virtual void Pulse(
		DWORD tickCount, 
		bool progressiveDown, 
		std::vector<MsgTargetPair*> *returnMessages) = 0;

	/// <summary>
	/// Sets the progressive paid by EGM.
	/// </summary>
	/// <param name="Award">The award.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	virtual void SetProgressivePaidByEGM(
		std::shared_ptr<CEGMAward> Award, 
		std::vector<MsgTargetPair*> *returnMessages) = 0;

	/// <summary>
	/// Sets the hand pay pending.
	/// </summary>
	/// <param name="awardAmount">The award amount.</param>
	/// <param name="levelId">The level identifier.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	virtual void SetHandPayPending(
		__int64 awardAmount,
		int levelId,
		std::vector<MsgTargetPair*> *returnMessages) = 0;

	/// <summary>
	/// Processes an standard (SAS) progressive hit acknowledgment from the progressive protocol and optionally generates an outgoing message.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <param name="games">Games information, denomination, transfer capabilities, etc.</param>
	/// <param name="progressiveDown">IN - true if we are displaying a progressive down message to players, false otherwise.</param>
	/// <param name="egmAwardHit">IN - IN - The incoming award instance.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	virtual void ProcessEGMAwardStandardFromProgressive(
		DWORD tickCount,
		CGames& games,
		bool progressiveDown,
		const CEGMAward& egmAwardHit,
		std::vector<MsgTargetPair*> *returnMessages) = 0;

	/// <summary>
	/// Sets the progressive win. Usually from a long poll 84 or 85
	/// </summary>
	/// <param name="msg">The MSG.</param>
	/// <param name="returnMessages">IN - Used to send multiple message back from callers</param>
	/// <param name="level">The SAS level.</param>
	virtual void SetProgressiveWin(
		CInternalMessage *msg, 
		std::vector<MsgTargetPair*> *returnMessages,
		int64_t progressiveLevelId) = 0;

	/// <summary>
	/// This tells the system that the progressive was paid kind of.
	/// </summary>
	/// <param name="egmAwardHit">The egm award hit.</param>
	virtual void ProgressivePaidSentReceived(std::shared_ptr<CEGMAward> egmAwardHit) = 0;

	/// <summary>
	/// Gets a copy of the last standard SAS progressive win processed.
	/// </summary>
	/// <param name="lastStandardWin">OUT - A copy of the last standard SAS progressive win processed.</param>
	virtual void GetLastStandardWin(std::shared_ptr<CEGMAward> &lastStandardWin) = 0;

	/// <summary>
	/// Gets a copy of the list of standard SAS progressive win currently being processed.
	/// </summary>
	/// <param name="standardWins">OUT - A copy of the list of standard SAS progressive wins currently being processed.</param>
	virtual void GetStandardWins(std::vector<std::shared_ptr<CEGMAward>> &standardWins) = 0;
};

