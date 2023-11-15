#pragma once
#include <InternalMsgContent.h>
#include <GlobalDefs.h>
#include <Jackpot.h>
class CPayout :
	public CInternalMsgContent<INT_MSG_CONTENT_PAYOUT>
{
public:
	/// <summary>
	/// Creates the CPayout class used to send jackpot information to OneLink
	/// </summary>
	/// <param name="jackpotAmount">IN - The jackpot amount in pennies</param>
	/// <param name="jackpotType">IN - The jackpot type</param>
	CPayout(long jackpotAmount,
		TriggerClassType jackpotType);

	/// <summary>
	/// Create the CPayout class used to send jackpot information to OneLink
	/// </summary>
	/// <param name="jackpot">IN - The Jackpot</param>
	CPayout(CJackpot *jackpot);

	/// <summary>
	/// Gets the jackpot amount in whole dollars
	/// </summary>
	/// <returns>Returns the jackpot amount in whole dollars</returns>
	long GetJackpotAmount() const;

	/// <summary>
	/// Gets Jackpot type
	/// </summary>
	/// <returns>Returns the jackpot type</returns>
	TriggerClassType GetJackpotType() const;

private:
	long m_jackpotAmount;
	TriggerClassType m_jackpotType;
};

