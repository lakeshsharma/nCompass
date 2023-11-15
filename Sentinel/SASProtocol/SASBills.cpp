#include "stdafx.h"
#include "SASBills.h"
#include "Logging/LogString.h"
#include "Sentinel.h"
#include "GlobalDefs.h"

CSASBills::CSASBills(CQueueList &gcfToInternalsQueue)
	:
	m_gcfToInternalsQueue(gcfToInternalsQueue),
	m_totalBills(INVALID_METER_VALUE),
	m_billDropCreditsFromInternals(INVALID_METER_VALUE)
{
}

CSASBills::~CSASBills()
{
}

void CSASBills::SetBillDropCreditsFromInternals(__int64 billDropCreditsFromInternals)
{
	m_billDropCreditsFromInternals = billDropCreditsFromInternals;
	LogString("SASGENERAL", "SASBills: m_billDropCreditsFromInternals=%lld", (long long)m_billDropCreditsFromInternals);
}

void CSASBills::SetDenomMult(WORD denomMult)
{
	if (INVALID_METER_VALUE == m_totalBills &&
		m_billDropCreditsFromInternals != INVALID_METER_VALUE &&
		denomMult != INVALID_DENOM)
	{
		// Initialize dollar value of bill meters once and only once.
		// Restore m_totalBills from m_billDropCreditsFromInternals. Also see void CLongPoll20Message::ResponseReceived() for totalBills calculation.
		m_totalBills = m_billDropCreditsFromInternals * denomMult / ONE_DOLLAR_IN_CENTS;
		LogString("SASGENERAL", "SASBills: Restoring m_totalBills=%lld from m_billDropCreditsFromInternals=%lld denomMult=%u",
			(long long)m_totalBills, (long long)m_billDropCreditsFromInternals, (unsigned)denomMult);
	}
}

void CSASBills::SetTotalBills(__int64 totalBills)
{
	ProcessIndividualBillsFromTotalBills(m_gcfToInternalsQueue, m_totalBills, totalBills);
}

__int64 CSASBills::GetTotalBills()
{
	return m_totalBills;
}

void CSASBills::ProcessIndividualBillsFromTotalBills(CQueueList &gcfToInternalsQueue, __int64 &totalBills, __int64 newTotalBills)
{
	// Need a message queue in order to process changes.
	if (totalBills != newTotalBills)
	{
		LogString("SASGENERAL", "SASBills: Processing totalBills=%lld newTotalBills=%lld delta=%lld",
			(long long)totalBills,
			(long long)newTotalBills,
			(long long)(newTotalBills - totalBills));

		// Only process the new value if it is valid.
		if (newTotalBills != INVALID_METER_VALUE)
		{
			// Don't process the delta if the previous value is unknown (or the delta is too big), but do keep the new value.
			__int64 delta = newTotalBills - totalBills;
			if (totalBills != INVALID_METER_VALUE && delta <= MAXIMUM_TOTAL_BILLS_DELTA)
			{
				ProcessSingleBillFromTotalBills(gcfToInternalsQueue, delta, mD100, BILL_DENOM_100);

				// If delta > 50 then calculate the delta using $20 bills, as $20 bills are generally more popular than $50 bills in the U.S.
				if (BILL_DENOM_50 == delta)
				{
					ProcessSingleBillFromTotalBills(gcfToInternalsQueue, delta, mD50, BILL_DENOM_50);
				}

				ProcessSingleBillFromTotalBills(gcfToInternalsQueue, delta, mD20, BILL_DENOM_20);
				ProcessSingleBillFromTotalBills(gcfToInternalsQueue, delta, mD10, BILL_DENOM_10);
				ProcessSingleBillFromTotalBills(gcfToInternalsQueue, delta, mD5, BILL_DENOM_5);
				ProcessSingleBillFromTotalBills(gcfToInternalsQueue, delta, mD1, BILL_DENOM_1);
			}

			totalBills = newTotalBills;
		}
	}
}

void CSASBills::ProcessSingleBillFromTotalBills(CQueueList &gcfToInternalsQueue, __int64 &delta, enum oasisMeters meterNumber, __int64 billDenom)
{
	if (delta > 0 && billDenom > 0 && delta >= billDenom)
	{
		__int64 incAmount(delta / billDenom);
		LogString("SASGENERAL", "SASBills: Calculated bill meterNumber=%d billDenom=%lld incAmount=%lld",
			(int)meterNumber, (long long)billDenom, (long long)(incAmount));
		SendBillIncrementToInternals(gcfToInternalsQueue, meterNumber, incAmount);
		delta %= billDenom;
	}
}

void CSASBills::SendBillIncrementToInternals(CQueueList &gcfToInternalsQueue, enum oasisMeters meterNumber, __int64 incAmount)
{
	if (incAmount > 0)
	{
		gcfToInternalsQueue.Add(
			new CInternalMessage(INT_MSG_INC_BILL_METER, (DWORD)meterNumber, (DWORD)incAmount),
			QUEUE_PRIORITY_NORMAL);
	}
}

