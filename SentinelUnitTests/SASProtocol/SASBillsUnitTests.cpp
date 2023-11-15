#include "stdafx.h"
#include "SASProtocol/SASBills.h"

// Unit testing includes.
#include "gtest/gtest.h"
//#include "UnitTestingToString.h"

namespace SentinelUnitTests
{
	class SASBillsTests : public ::testing::Test
	{
	public:   
		virtual void SetUp()  
		{
		} 
		virtual void TearDown()
		{
		}
	};

// #pragma region SetBillDropCreditsFromInternals

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test setting a range of values.
		/// </summary>
// #pragma endregion SetBillDropCreditsFromInternals
// #pragma region SetBillDropCreditsFromInternals
		/// <summary>
		/// Functional test for:
		///     CSASBills::SetDenomMult()
		/// Test with default value for m_billDropCreditsFromInternals, default value for m_totalBills, and invalid denom.
		/// </summary>
		TEST_F(SASBillsTests, SetDenomMult_Default_Default_Invalid)
		{
			// Data.
			const WORD denomMult(INVALID_DENOM);
			const __int64 billDropCreditsFromInternals(INVALID_METER_VALUE);
			const __int64 totalBills(INVALID_METER_VALUE);
			const __int64 totalBillsExpected(INVALID_METER_VALUE);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetDenomMult(denomMult);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			////ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetDenomMult()
		/// Test with default value for m_billDropCreditsFromInternals, default value for m_totalBills, and valid denom.
		/// </summary>
		TEST_F(SASBillsTests, SetDenomMult_Default_Default_Valid)
		{
			// Data.
			const WORD denomMult(25);
			const __int64 billDropCreditsFromInternals(INVALID_METER_VALUE);
			const __int64 totalBills(INVALID_METER_VALUE);
			const __int64 totalBillsExpected(INVALID_METER_VALUE);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetDenomMult(denomMult);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			////ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetDenomMult()
		/// Test with default value for m_billDropCreditsFromInternals, valid value for m_totalBills, and invalid denom.
		/// </summary>
		TEST_F(SASBillsTests, SetDenomMult_Default_Valid_Invalid)
		{
			// Data.
			const WORD denomMult(INVALID_DENOM);
			const __int64 billDropCreditsFromInternals(INVALID_METER_VALUE);
			const __int64 totalBills(100);
			const __int64 totalBillsExpected(totalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetDenomMult(denomMult);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			////ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetDenomMult()
		/// Test with default value for m_billDropCreditsFromInternals, valid value for m_totalBills, and valid denom.
		/// </summary>
		TEST_F(SASBillsTests, SetDenomMult_Default_Valid_Valid)
		{
			// Data.
			const WORD denomMult(25);
			const __int64 billDropCreditsFromInternals(INVALID_METER_VALUE);
			const __int64 totalBills(100);
			const __int64 totalBillsExpected(totalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetDenomMult(denomMult);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			////ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetDenomMult()
		/// Test with valid value for m_billDropCreditsFromInternals, default value for m_totalBills, and invalid denom.
		/// </summary>
		TEST_F(SASBillsTests, SetDenomMult_Valid_Default_Invalid)
		{
			// Data.
			const WORD denomMult(INVALID_DENOM);
			const __int64 billDropCreditsFromInternals(1000);
			const __int64 totalBills(INVALID_METER_VALUE);
			const __int64 totalBillsExpected(INVALID_METER_VALUE);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetBillDropCreditsFromInternals(billDropCreditsFromInternals);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetDenomMult(denomMult);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			////ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetDenomMult()
		/// Test with valid value for m_billDropCreditsFromInternals, default value for m_totalBills, and valid denom.
		/// </summary>
		TEST_F(SASBillsTests, SetDenomMult_Valid_Default_Valid)
		{
			// Data.
			const WORD denomMult(25);
			const __int64 billDropCreditsFromInternals(1000);
			const __int64 totalBills(INVALID_METER_VALUE);
			const __int64 totalBillsExpected(billDropCreditsFromInternals * denomMult / ONE_DOLLAR_IN_CENTS);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetBillDropCreditsFromInternals(billDropCreditsFromInternals);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetDenomMult(denomMult);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			////ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetDenomMult()
		/// Test with valid value for m_billDropCreditsFromInternals, valid value for m_totalBills, and invalid denom.
		/// </summary>
		TEST_F(SASBillsTests, SetDenomMult_Valid_Valid_Invalid)
		{
			// Data.
			const WORD denomMult(INVALID_DENOM);
			const __int64 billDropCreditsFromInternals(1000);
			const __int64 totalBills(100);
			const __int64 totalBillsExpected(totalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetBillDropCreditsFromInternals(billDropCreditsFromInternals);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetDenomMult(denomMult);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			////ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetDenomMult()
		/// Test with valid value for m_billDropCreditsFromInternals, valid value for m_totalBills, and valid denom.
		/// </summary>
		TEST_F(SASBillsTests, SetDenomMult_Valid_Valid_Valid)
		{
			// Data.
			const WORD denomMult(25);
			const __int64 billDropCreditsFromInternals(1000);
			const __int64 totalBills(100);
			const __int64 totalBillsExpected(totalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetBillDropCreditsFromInternals(billDropCreditsFromInternals);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetDenomMult(denomMult);
			//ASSERT_EQ(billDropCreditsFromInternals, obj.m_billDropCreditsFromInternals);
			////ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());
		}
// #pragma endregion SetBillDropCreditsFromInternals

// #pragma region SetTotalBills
		/// <summary>
		/// Functional test for:
		///     CSASBills::CSASBills()
		/// Test for default value.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Default)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");

			// Perform operation(s) and test(s).
			CSASBills obj(gcfToInternalsQueue);
			//ASSERT_EQ((__int64)INVALID_METER_VALUE, obj.m_totalBills);
			ASSERT_EQ((__int64)INVALID_METER_VALUE, obj.GetTotalBills());
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test setting a range of values.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Range)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);

			// Perform operation(s) and test(s).
			obj.SetTotalBills(INT64_MIN);
			//ASSERT_EQ((__int64)INT64_MIN, obj.m_totalBills);
			ASSERT_EQ((__int64)INT64_MIN, obj.GetTotalBills());

			obj.SetTotalBills(INT_MIN);
			//ASSERT_EQ((__int64)INT_MIN, obj.m_totalBills);
			ASSERT_EQ((__int64)INT_MIN, obj.GetTotalBills());

			obj.SetTotalBills(-1);
			//ASSERT_EQ((__int64)INT_MIN, obj.m_totalBills);
			ASSERT_EQ((__int64)INT_MIN, obj.GetTotalBills());

			obj.SetTotalBills(0);
			//ASSERT_EQ((__int64)0, obj.m_totalBills);
			ASSERT_EQ((__int64)0, obj.GetTotalBills());

			obj.SetTotalBills(1);
			//ASSERT_EQ((__int64)1, obj.m_totalBills);
			ASSERT_EQ((__int64)1, obj.GetTotalBills());

			obj.SetTotalBills(INT_MAX);
			//ASSERT_EQ((__int64)INT_MAX, obj.m_totalBills);
			ASSERT_EQ((__int64)INT_MAX, obj.GetTotalBills());

			obj.SetTotalBills(DWORD_MAX);
			//ASSERT_EQ((__int64)DWORD_MAX, obj.m_totalBills);
			ASSERT_EQ((__int64)DWORD_MAX, obj.GetTotalBills());

			obj.SetTotalBills(INT64_MAX);
			//ASSERT_EQ((__int64)INT64_MAX, obj.m_totalBills);
			ASSERT_EQ((__int64)INT64_MAX, obj.GetTotalBills());
		}
// #pragma endregion SetTotalBills

		// Remove all messages and assert that they are the correct default message(s).
		static void AssertQueueEmpty(CQueueList &gcfToInternalsQueue)
		{
			// Verify that the queue is empty.
			ASSERT_EQ(gcfToInternalsQueue.Remove(), nullptr);
		}

		static void AssertIncBillMeter(CQueueList &gcfToInternalsQueue, DWORD meterNumber, DWORD incAmount)
		{
			// Verify that the queue is empty.
			CQueueItem *item(gcfToInternalsQueue.Remove());
			ASSERT_NE(item,nullptr);
			CInternalMessage *msg(dynamic_cast<CInternalMessage *>(item));
			ASSERT_NE(msg, nullptr);

			ASSERT_EQ(INT_MSG_INC_BILL_METER, msg->GetMessageType());
			ASSERT_EQ(meterNumber, msg->GetDWordData());
			ASSERT_EQ(incAmount, msg->GetDWordData(true));
		}

// #pragma region SetTotalBills_QueuedItems
		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with old value invalid.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_OldInvalid)
		{
			// Data.
			__int64 totalBills(INVALID_METER_VALUE);
			const __int64 newTotalBills(20);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			////ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with new value invalid.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_NewInvalid)
		{
			// Data.
			__int64 totalBills(123);
			const __int64 newTotalBills(INVALID_METER_VALUE);
			const __int64 totalBillsExpected(totalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with both old and new values invalid.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_BothInvalid)
		{
			// Data.
			__int64 totalBills(INVALID_METER_VALUE);
			const __int64 newTotalBills(INVALID_METER_VALUE);
			const __int64 totalBillsExpected(totalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a delta of zero.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_ZeroDelta)
		{
			// Data.
			__int64 totalBills(3);
			const __int64 newTotalBills(totalBills);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a delta less than zero.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_NegDelta)
		{
			// Data.
			__int64 totalBills(5);
			const __int64 newTotalBills(totalBills - 1);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Delta1)
		{
			// Data.
			__int64 totalBills(6);
			const __int64 newTotalBills(totalBills + 1);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 1);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Delta4)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 4);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 4);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Delta5)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 5);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD5, 1);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Delta7)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 7);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD5, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 2);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Delta10)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 10);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD10, 1);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Delta18)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 18);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD10, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD5, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 3);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Delta20)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 20);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD20, 1);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Delta29)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 29);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD20, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD5, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 4);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Delta50)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 50);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD50, 1);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Delta51)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 51);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD20, 2);
			AssertIncBillMeter(gcfToInternalsQueue, mD10, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 1);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Delta60)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 60);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD20, 3);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Delta99)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 99);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD20, 4);
			AssertIncBillMeter(gcfToInternalsQueue, mD10, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD5, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 4);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Delta100)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 100);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD100, 1);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Delta123)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 123);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD100, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD20, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 3);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_Delta789)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 789);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD100, 7);
			AssertIncBillMeter(gcfToInternalsQueue, mD20, 4);
			AssertIncBillMeter(gcfToInternalsQueue, mD5, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 4);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with maximum limit threshold delta for $100 bills.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_LimitMax)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 1000);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertIncBillMeter(gcfToInternalsQueue, mD100, 10);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with maximum limit threshold delta for $100 bills, plus $1.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_LimitMaxPlus1)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 1001);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a maximum delta for $100 bills.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_DeltaMax)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 100 * (__int64)DWORD_MAX);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a maximum delta for $100 bills, plus other bills.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_DeltaMax_Rem)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 100 * (__int64)DWORD_MAX + 45);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a excessive delta for $100 bills.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_DeltaExcess)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 100 * (__int64)0x34567890);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::SetTotalBills()
		/// Test queued items with a excessive delta for $100 bills, plus other bills.
		/// </summary>
		TEST_F(SASBillsTests, SetTotalBills_DeltaExcess_Rem)
		{
			// Data.
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 100 * (__int64)0x1234567890 + 45);
			const __int64 totalBillsExpected(newTotalBills);

			// Setup.
			CQueueList gcfToInternalsQueue("");
			CSASBills obj(gcfToInternalsQueue);
			obj.SetTotalBills(totalBills);
			//ASSERT_EQ(totalBills, obj.m_totalBills);
			ASSERT_EQ(totalBills, obj.GetTotalBills());

			// Perform operation(s) and test(s).
			obj.SetTotalBills(newTotalBills);
			//ASSERT_EQ(totalBillsExpected, obj.m_totalBills);
			ASSERT_EQ(totalBillsExpected, obj.GetTotalBills());
			AssertQueueEmpty(gcfToInternalsQueue);
		}
// #pragma endregion SetTotalBills_QueuedItems

// #pragma region ProcessIndividualBillsFromTotalBills
		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with old value invalid.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_OldInvalid)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(INVALID_METER_VALUE);
			const __int64 newTotalBills(20);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with new value invalid.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_NewInvalid)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(123);
			const __int64 newTotalBills(INVALID_METER_VALUE);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)totalBills, totalBills);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with both old and new values invalid.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_BothInvalid)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(INVALID_METER_VALUE);
			const __int64 newTotalBills(totalBills);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a delta of zero.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_ZeroDelta)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(3);
			const __int64 newTotalBills(totalBills);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a delta less than zero.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_NegDelta)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(5);
			const __int64 newTotalBills(totalBills - 1);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_Delta1)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(6);
			const __int64 newTotalBills(totalBills + 1);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 1);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_Delta4)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 4);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 4);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_Delta5)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 5);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD5, 1);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_Delta7)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 7);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD5, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 2);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_Delta10)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 10);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD10, 1);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_Delta18)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 18);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD10, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD5, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 3);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_Delta20)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 20);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD20, 1);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_Delta29)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 29);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD20, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD5, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 4);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_Delta50)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 50);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD50, 1);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_Delta51)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 51);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD20, 2);
			AssertIncBillMeter(gcfToInternalsQueue, mD10, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 1);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_Delta60)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 60);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD20, 3);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_Delta99)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 99);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD20, 4);
			AssertIncBillMeter(gcfToInternalsQueue, mD10, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD5, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 4);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_Delta100)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 100);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD100, 1);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_Delta123)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 123);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD100, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD20, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 3);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a postive delta.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_Delta789)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 789);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD100, 7);
			AssertIncBillMeter(gcfToInternalsQueue, mD20, 4);
			AssertIncBillMeter(gcfToInternalsQueue, mD5, 1);
			AssertIncBillMeter(gcfToInternalsQueue, mD1, 4);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with maximum limit threshold delta for $100 bills.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_LimitMax)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 1000);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertIncBillMeter(gcfToInternalsQueue, mD100, 10);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with maximum limit threshold delta for $100 bills, plus $1.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_LimitMaxPlus1)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 1001);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a maximum delta for $100 bills.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_DeltaMax)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 100 * (__int64)DWORD_MAX);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a maximum delta for $100 bills, plus other bills.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_DeltaMax_Rem)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 100 * (__int64)DWORD_MAX + 45);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a excessive delta for $100 bills.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_DeltaExcess)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 100 * (__int64)0x34567890);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertQueueEmpty(gcfToInternalsQueue);
		}

		/// <summary>
		/// Functional test for:
		///     CSASBills::ProcessIndividualBillsFromTotalBills()
		/// Test with a excessive delta for $100 bills, plus other bills.
		/// </summary>
		TEST_F(SASBillsTests, ProcessIndividualBillsFromTotalBills_DeltaExcess_Rem)
		{
			// Setup.
			CQueueList gcfToInternalsQueue("");
			__int64 totalBills(1000);
			const __int64 newTotalBills(totalBills + 100 * (__int64)0x1234567890 + 45);

			// Perform operation(s) and test(s).
			CSASBills::ProcessIndividualBillsFromTotalBills(gcfToInternalsQueue, totalBills, newTotalBills);
			ASSERT_EQ((__int64)newTotalBills, totalBills);
			AssertQueueEmpty(gcfToInternalsQueue);
		}
// #pragma endregion ProcessIndividualBillsFromTotalBills

}
