#pragma once
#include "QueueList.h"

// Declare friend class(es) for unit test access.
namespace SentinelUnitTests { class SASBillsTests; }

class CSASBills
{
public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CSASBills"/> class.
	/// <param name="gcfToInternalsQueue">IN/OUT - The GCF to Internals queue.</param>
	/// </summary>
	CSASBills(CQueueList &gcfToInternalsQueue);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CSASBills"/> class.
	/// </summary>
	~CSASBills();

	/// <summary>
	/// Sets the bill drop credits from Internals.
	/// </summary>
	/// <param name="billDropCreditsFromInternals">IN - The bill drop credits from Internals.</param>
	void SetBillDropCreditsFromInternals(__int64 billDropCreditsFromInternals);

	/// <summary>
	/// Sets the SAS communications denomination multiplier.
	/// </summary>
	/// <param name="denomMult">IN - The SAS communications denomination multiplier.</param>
	void SetDenomMult(WORD denomMult);

	/// <summary>
	/// Sets the total bills accepted in dollars.
	/// </summary>
	/// <param name="totalBills">IN - The total bills accepted in dollars.</param>
	void SetTotalBills(__int64 totalBills);

	/// <summary>
	/// Gets the total bills accepted in dollars.
	/// </summary>
	/// <returns>The total bills accepted in dollars.</returns>
	__int64 GetTotalBills();

	/// <summary>
	/// Process individual bills from a change in total bills.
	/// </summary>
	/// <param name="gcfToInternalsQueue">IN/OUT - The GCF to internals queue.</param>
	/// <param name="totalBills">IN/OUT - The current/previous value for total bills in dollars.</param>
	/// <param name="newTotalBills">IN - The new value for total bills in dollars.</param>
	/// <remarks>
	/// Sends messages to Internals for individual bill meter changes.
	/// Updates totalBills to the newTotalBills value.
	/// 2016-07-05 Only checks for 1, 5, 10, 20, 50, 100 denomination bills.
	/// </remarks>
	static void ProcessIndividualBillsFromTotalBills(CQueueList &gcfToInternalsQueue, __int64 &totalBills, __int64 newTotalBills);

private:
	// Make default constructor, copy constructor, and assignment operator (generally) inaccessible.
	CSASBills();
	CSASBills(const CSASBills &rhs);
	void operator=(const CSASBills &rhs);

	static const __int64 BILL_DENOM_1 = 1;
	static const __int64 BILL_DENOM_5 = 5;
	static const __int64 BILL_DENOM_10 = 10;
	static const __int64 BILL_DENOM_20 = 20;
	static const __int64 BILL_DENOM_50 = 50;
	static const __int64 BILL_DENOM_100 = 100;

	static const __int64 MAXIMUM_TOTAL_BILLS_DELTA = 1000; // Don't calculate individual bills for deltas greater than this.

	CQueueList &m_gcfToInternalsQueue;
	__int64 m_totalBills;
	__int64 m_billDropCreditsFromInternals;

	static void ProcessSingleBillFromTotalBills(CQueueList &gcfToInternalsQueue, 	__int64 &delta, enum oasisMeters meterNumber, __int64 billDenom);
	static void SendBillIncrementToInternals(CQueueList &gcfToInternalsQueue, enum oasisMeters meterNumber, __int64 incAmount);

	// Grant "friend" access for testing.
	friend class SentinelUnitTests::SASBillsTests;

};

