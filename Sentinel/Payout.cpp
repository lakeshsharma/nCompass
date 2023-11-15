#include "stdafx.h"
#include "Payout.h"
#include <Jackpot.h>

CPayout::CPayout(long jackpotAmount,
	TriggerClassType jackpotType) :
	m_jackpotAmount(jackpotAmount),
	m_jackpotType(jackpotType)
{}

CPayout::CPayout(CJackpot *jackpot) 
{
	JackpotType jType = jackpot->GetJackpotType();
	JackpotTransactionType jTransType = jackpot->GetJackpotTransactionType();
	m_jackpotAmount = jackpot->GetJackpotAmount();

	if (jTransType == JTRANS_COMPOSITE &&
		(jType == JT_HANDPAY || jType == JT_TOPAWARD))
	{
		m_jackpotType = AttendantPaidJackpot;
	}
	else if ((jTransType == JTRANS_COMPOSITE && jType == JT_NONE) ||
		jTransType == JTRANS_PROGRESSIVE_HOPPER_PAID ||
		jTransType == JTRANS_PROGRESSIVE_SCANNER_ONLY)
	{
		m_jackpotType = AttendantPaidProgressiveJackpot;
	}
	else
	{
		m_jackpotType = AllJackpots;
	}
}

long CPayout::GetJackpotAmount() const
{
	return m_jackpotAmount;
}

TriggerClassType CPayout::GetJackpotType() const
{
	return m_jackpotType;
}