#include "stdafx.h"

#include "InternalsFramework.h"
#include "PersonalBanker.h"

#include "Utilities.h"

#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{
	const time_t CTIME_DEFAULT_VALUE(0); // Arbitrary value when any valid value is okay.
	const byte MEMSET_DEFAULT_VALUE = 0xcd;

		/// Returns true if the auto upload timer for the pb instance is expired, returns false otherwise.
		/// This is a destructive test that alters the CPersonalBanker instance, including the auto upload timer.
		/// Not a test method.
		/// </summary>
		/// <param name="varPB">IN/OUT - The CPersonalBanker instance to test. This is a destructive test.</param>
		/// <param name="tickCount">IN - The tickcount for testing varPB.m_autoUpload.IsUploadTimerExpired(tickCount)</param>
		/// <returns>varPB.m_autoUpload.IsUploadTimerExpired(tickCount).</returns>
		bool IsAutoUploadTimerExpired(CPersonalBanker &varPB,
			DWORD tickCount)
		{
			// Test data - inputs.
			const CConfig config(false, "");

			// Test data - for determining results.
			const bool pbTransferForAutoUploadExpired(true); // Value that indicates timer is expired.

			// Setup to ensure that CPersonalBanker::PerformTimedEventsTransfer() does not call SetPersonalBankerAction().
			const bool waitingForTransferToBeCompleted(false);
			const PersonalBankerState pbState(PBStateUnknown);
			const DWORD sessionTimeoutValueInMilliseconds(0);
			varPB.DoneWithTransfer();
			varPB.SetWaitingForTransferToBeCompleted(waitingForTransferToBeCompleted);
			varPB.SetState(pbState);
			varPB.ClearPersonalBankerAction();

			// Setup to ensure that CPersonalBanker::PerformTimedEventsAuto() calls PerformTimedEventsAutoNotDefaulted().
			// if and only if varPB.m_autoUpload.IsUploadTimerExpired(tickCount) is true.
			varPB.MakeAutoUploadPending();
			const bool defaultedPlayer(false);
			const bool delayAutoNcepOutWhenNotDefaultedPlayer(false);

			// Perform operations to be tested.
			const bool haveEmployee1(false);
			varPB.CPersonalBanker::PerformTimedEvents(delayAutoNcepOutWhenNotDefaultedPlayer,
					haveEmployee1,
					config,
					tickCount,
					sessionTimeoutValueInMilliseconds);
			const CPersonalBanker &pb(varPB);

			// Test #1.
			const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();
			const bool autoUploadExpired = ((pbTransferResult != NULL) == pbTransferForAutoUploadExpired);

			// Cleanup.

			return autoUploadExpired;
		}

		/// <summary>
		/// Returns true if varPB.m_autoUpload.IsAutoInProcess() is true, returns false otherwise.
		/// This is a destructive test that alters the CPersonalBanker instance.
		/// Not a test method.
		/// </summary>
		/// <param name="varPB">IN/OUT - The CPersonalBanker instance to test. This is a destructive test.</param>
		/// <returns>varPB.m_autoUpload.IsAutoInProcess().</returns>
		bool IsAutoInProcess(CPersonalBanker &varPB)
		{
			// Test data - inputs.
			const CConfig config(false, "");

			// Test data - for determining results.
			const bool pbTransferForAutoInProcess(false); // Value that indicates varPB.m_autoUpload.IsAutoInProcess() == true.

			// Setup.
			varPB.CreateTransfer(CUtilities::GetTickCount());

			// Perform operations to be tested.
			const DWORD cashOnGame(0);
			const DWORD promoOnGame(0);
			const bool canTransferOff(true);
			const bool callToEGMFailed(true);
			varPB.ProcessBalancesFromEGM(cashOnGame,
				0,
				promoOnGame,
				canTransferOff,
				callToEGMFailed,
				config);
			const CPersonalBanker &pb(varPB);

			// Test #1.
			const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();
			const bool autoInProcess = ((pbTransferResult != NULL) == pbTransferForAutoInProcess);

			// Cleanup.

			return autoInProcess;
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::PerformTimedEventsAutoNotDefaulted()
		/// Code coverage and functional test with m_pbTransfer == NULL.
		/// Test with m_pbTransfer == NULL and set so CPersonalBanker::IsWaitingForTransferToBeCompleted() returns false.
		/// </summary>
		TEST(PersonalBanker5, PerformTimedEventsAutoNotDefaulted_TransferNullTest)
		{
			typedef struct
			{
				// Inputs.
				// m_pbTransfer->IsAutoUploadPIN() will always be true when called with m_pbTransfer == NULL.
				const bool m_haveEmployee1;
				const bool m_haveAutoUploadCard;

				// Expected results.
				const PersonalBankerAction m_pbActionExpected;
				const bool m_pbAutoTransferPendingExpected;
				const bool m_pbAutoNcepOutPendingExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{false, false, PBActionGetEGMBalances, false, false },
				{false, true, PBActionGetEGMBalancesWithMessage, true, false },
				{true, false, PBActionNone, false, true },
				{true, true, PBActionNone, true, true },
			};

			// Test data - more inputs, same regardless of testIndex.
			const bool waitingForTransferToBeCompleted(false);
			const PersonalBankerState pbState(PBStateUnknown);
			const CConfig config(false, "");
			const DWORD sessionTimeoutValueInMilliseconds(0);

			// Test data - more expected results, same regardless of testIndex
			const PersonalBankerState pbStateExpected(PBStateWaitingForUploadToStart);
			const bool pbTransferExpected(true);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBanker varPb;
				if (testIndex == 1)
				{
					varPb.SetToDelayAutoUpload();
				}

				if (TEST_DATA[testIndex].m_haveAutoUploadCard)
				{
					varPb.RebuildCard();
				}

				// Setup to ensure that CPersonalBanker::PerformTimedEventsTransfer() does not call SetPersonalBankerAction().
				varPb.DoneWithTransfer();
				varPb.SetWaitingForTransferToBeCompleted(waitingForTransferToBeCompleted);
				varPb.SetState(pbState);

				// Setup to ensure that CPersonalBanker::PerformTimedEventsAuto() calls PerformTimedEventsAutoNotDefaulted().
				varPb.MakeAutoUploadPending();
				const bool defaultedPlayer(false);
				const bool delayAutoNcepOutWhenNotDefaultedPlayer(false);
				varPb.SetAutoUploadTimerExpired();

				// Finish setup.
				varPb.ClearPersonalBankerAction();

				// Perform operations to be tested.
				varPb.CPersonalBanker::PerformTimedEvents(delayAutoNcepOutWhenNotDefaultedPlayer,
					TEST_DATA[testIndex].m_haveEmployee1,
					config,
					CUtilities::GetTickCount(),
					sessionTimeoutValueInMilliseconds);
				const CPersonalBanker &pb(varPb);

				// Test #1.
				const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();

				{
					bool areEqual = ((pbTransferResult != NULL) == pbTransferExpected);

					string failedMsg;
					failedMsg = FormatString("FAILED pbTransferResult=%p expected=%s testIndex=%d in %s(%d)",
						(const void *)pbTransferResult,
						(pbTransferExpected ? "NOT NULL":  _T("NULL")) ,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #2.
				{
					const PersonalBankerState pbStateResult = pb.GetState();

					const bool areEqual = pbStateResult == pbStateExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbStateResult=%d expected=%d testIndex=%d in %s(%d)",
						pbStateResult,
						pbStateExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #3.
				{
					const PersonalBankerAction pbActionResult = pb.GetAction();

					const bool areEqual = pbActionResult == TEST_DATA[testIndex].m_pbActionExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbActionResult=%d expected=%d testIndex=%d in %s(%d)",
						pbActionResult,
						TEST_DATA[testIndex].m_pbActionExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #4.
				{
					const bool pbAutoTransferPendingResult = varPb.IsAutoTransferPending();

					const bool areEqual = pbAutoTransferPendingResult == TEST_DATA[testIndex].m_pbAutoTransferPendingExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbAutoTransferPendingResult=%d expected=%d testIndex=%d in %s(%d)",
						pbAutoTransferPendingResult,
						TEST_DATA[testIndex].m_pbAutoTransferPendingExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #5.
				{
					const bool pbAutoNcepOutPendingResult = pb.IsAutoUploadPending();

					const bool areEqual = pbAutoNcepOutPendingResult == TEST_DATA[testIndex].m_pbAutoNcepOutPendingExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbAutoNcepOutPendingResult=%d expected=%d testIndex=%d in %s(%d)",
						pbAutoNcepOutPendingResult,
						TEST_DATA[testIndex].m_pbAutoNcepOutPendingExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::PerformTimedEventsAutoNotDefaulted()
		/// Code coverage and functional test with m_pbTransfer == NULL.
		/// Include functional testing for CPersonalBanker::m_autoUpload.IsUploadTimerExpired().
		/// Test with m_pbTransfer == NULL and set so CPersonalBanker::IsWaitingForTransferToBeCompleted() returns false.
		/// </summary>
		TEST(PersonalBanker5, PerformTimedEventsAutoNotDefaulted_TransferNull2Test)
		{
			typedef struct
			{
				// Inputs.
				// m_pbTransfer->IsAutoUploadPIN() will always be true when called with m_pbTransfer == NULL.
				const bool m_haveEmployee1;
				const bool m_haveAutoUploadCard;
				const DWORD m_tickCountStart;
				const DWORD m_tickCountExpired; // Tick count for expiration test.

				// Expected results.
				const bool m_pbAutoTransferPendingExpected;
				const bool m_pbAutoNcepOutPendingExpected;
				const PersonalBankerAction m_pbActionExpected;
				const bool m_autoUploadTimerExpiredExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Test the minimum time for non-expired timers.
				{false, false, 100000, 100000, false, false, PBActionGetEGMBalancesWithMessage, false },
				{false, true, 100000, 100000, true, false, PBActionGetEGMBalances, false },
				{true, false, 100000, 100000, false, true, PBActionNone, false },
				{true, true, 100000, 100000, true, true, PBActionNone, false },

				// Test the maximum time for non-expired timers.
				{false, false, 100000, 100000 + PBT_UPLOAD_DELAY_PLAYER_ACTIVE * MILLISECONDS_PER_SECOND - 1, false, false, PBActionGetEGMBalances, false },
				{false, true, 100000, 100000 + PBT_UPLOAD_DELAY_PLAYER_ACTIVE * MILLISECONDS_PER_SECOND - 1, true, false, PBActionGetEGMBalances, false },
				{true, false, 100000, 100000 + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND - 1, false, true, PBActionNone, false },
				{true, true, 100000, 100000 + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND - 1, true, true, PBActionNone, false },

				// Test the minimum time for expired timers.
				{false, false, 100000, 100000 + PBT_UPLOAD_DELAY_PLAYER_ACTIVE * MILLISECONDS_PER_SECOND, false, false, PBActionGetEGMBalances, true },
				{false, true, 100000, 100000 + PBT_UPLOAD_DELAY_PLAYER_ACTIVE * MILLISECONDS_PER_SECOND, true, false, PBActionGetEGMBalances, true },
				{true, false, 100000, 100000 + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND, false, true, PBActionNone, true },
				{true, true, 100000, 100000 + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND, true, true, PBActionNone, true },
			};

			// Test data - more inputs, same regardless of testIndex.
			const bool waitingForTransferToBeCompleted(false);
			const PersonalBankerState pbState(PBStateUnknown);
			const CConfig config(false, "");
			const DWORD sessionTimeoutValueInMilliseconds(0);

			// Test data - more expected results, same regardless of testIndex
			const PersonalBankerState pbStateExpected(PBStateWaitingForUploadToStart);
			const bool pbTransferExpected(true);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBanker varPb;

				if (testIndex == 0)
				{
					varPb.SetToDelayAutoUpload();
				}

				if (TEST_DATA[testIndex].m_haveAutoUploadCard)
				{
					varPb.RebuildCard();
				}

				// Setup to ensure that CPersonalBanker::PerformTimedEventsTransfer() does not call SetPersonalBankerAction().
				varPb.DoneWithTransfer();
				varPb.SetWaitingForTransferToBeCompleted(waitingForTransferToBeCompleted);
				varPb.SetState(pbState);
				varPb.ClearPersonalBankerAction();

				// Setup to ensure that CPersonalBanker::PerformTimedEventsAuto() calls PerformTimedEventsAutoNotDefaulted().
				varPb.MakeAutoUploadPending();
				const bool defaultedPlayer(false);
				const bool delayAutoNcepOutWhenNotDefaultedPlayer(false);

				// Perform operations to be tested.
				varPb.CPersonalBanker::PerformTimedEvents(delayAutoNcepOutWhenNotDefaultedPlayer,
					TEST_DATA[testIndex].m_haveEmployee1,
					config,
					TEST_DATA[testIndex].m_tickCountStart,
					sessionTimeoutValueInMilliseconds);
				const CPersonalBanker &pb(varPb);

				// Test #1.
				const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();

				{
					bool areEqual = ((pbTransferResult != NULL) == pbTransferExpected);

					string failedMsg;
					failedMsg = FormatString("FAILED pbTransferResult=%p expected=%s testIndex=%d in %s(%d)",
						(const void *)pbTransferResult,
						(pbTransferExpected ? "NOT NULL" :  _T("NULL")),
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #2.
				{
					const PersonalBankerState pbStateResult = pb.GetState();

					const bool areEqual = pbStateResult == pbStateExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbStateResult=%d expected=%d testIndex=%d in %s(%d)",
						pbStateResult,
						pbStateExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #3.
				{
					const PersonalBankerAction pbActionResult = pb.GetAction();

					const bool areEqual = pbActionResult == TEST_DATA[testIndex].m_pbActionExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbActionResult=%d expected=%d testIndex=%d in %s(%d)",
						pbActionResult,
						TEST_DATA[testIndex].m_pbActionExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #4.
				{
					const bool pbAutoTransferPendingResult = varPb.IsAutoTransferPending();

					const bool areEqual = pbAutoTransferPendingResult == TEST_DATA[testIndex].m_pbAutoTransferPendingExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbAutoTransferPendingResult=%d expected=%d testIndex=%d in %s(%d)",
						pbAutoTransferPendingResult,
						TEST_DATA[testIndex].m_pbAutoTransferPendingExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #5.
				{
					const bool pbAutoNcepOutPendingResult = pb.IsAutoUploadPending();

					const bool areEqual = pbAutoNcepOutPendingResult == TEST_DATA[testIndex].m_pbAutoNcepOutPendingExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbAutoNcepOutPendingResult=%d expected=%d testIndex=%d in %s(%d)",
						pbAutoNcepOutPendingResult,
						TEST_DATA[testIndex].m_pbAutoNcepOutPendingExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #6 -- destructive timer test, so do this last.
				{
					const bool autoUploadTimerExpiredResult = IsAutoUploadTimerExpired(varPb, TEST_DATA[testIndex].m_tickCountExpired);

					const bool areEqual = autoUploadTimerExpiredResult == TEST_DATA[testIndex].m_autoUploadTimerExpiredExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED autoUploadTimerExpiredResult=%d expected=%d testIndex=%d in %s(%d)",
						autoUploadTimerExpiredResult,
						TEST_DATA[testIndex].m_autoUploadTimerExpiredExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::PerformTimedEventsAutoNotDefaulted()
		/// Code coverage and functional test with m_pbTransfer != NULL.
		/// Include functional testing for CPersonalBanker::m_autoUpload.IsUploadTimerExpired().
		/// Test with m_pbTransfer != NULL and set so CPersonalBanker::IsWaitingForTransferToBeCompleted() returns false.
		/// </summary>
		TEST(PersonalBanker5, PerformTimedEventsAutoNotDefaulted_TransferTest)
		{
			// Test data inputs - same regardless of testIndex.
			const bool waitingForTransferToBeCompleted(false);
			const PersonalBankerState pbState(PBStateUnknown);
			const CConfig config(false, "");
			const DWORD sessionTimeoutValueInMilliseconds(0);

			// Test data - expected results, same regardless of testIndex
			const PersonalBankerState pbStateExpected(PBStateWaitingForUploadToStart);
			const bool pbTransferExpected(true);

			typedef struct
			{
				// Inputs.
				const bool m_haveAutoUploadPIN;
				const bool m_haveEmployee1;
				const bool m_haveAutoUploadCard;
				const DWORD m_tickCountStart;
				const DWORD m_tickCountExpired; // Tick count for expiration test.

				// Expected results.
				const bool m_pbAutoTransferPendingExpected;
				const bool m_pbAutoNcepOutPendingExpected;
				const PersonalBankerState m_pbStateExpected;
				const PersonalBankerAction m_pbActionExpected;
				const bool m_autoUploadTimerExpiredExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Test the minimum time for non-expired timers.
				{ false, false, false, 100000, 100000, false, true, pbState, PBActionNone, false },
				{ false, false, true, 100000, 100000, true, true, pbState, PBActionNone, false },
				{ false, true, false, 100000, 100000, false, true, pbState, PBActionNone, false },
				{ false, true, true, 100000, 100000, true, true, pbState, PBActionNone, false },
				{ true, false, false, 100000, 100000, false, false, pbState, PBActionGetEGMBalancesWithMessage, false },
				{ true, false, true, 100000, 100000, true, false, pbState, PBActionGetEGMBalances, false },
				{ true, true, false, 100000, 100000, false, true, pbState, PBActionNone, false },
				{ true, true, true, 100000, 100000, true, true, pbState, PBActionNone, false },

				// Test the maximum time for non-expired timers.
				{ false, false, false, 100000, 100000 + PBT_UPLOAD_DELAY_PLAYER_ACTIVE * MILLISECONDS_PER_SECOND - 1, false, true, pbState, PBActionNone, false },
				{ false, false, true, 100000, 100000 + PBT_UPLOAD_DELAY_PLAYER_ACTIVE * MILLISECONDS_PER_SECOND - 1, true, true, pbState, PBActionNone, false },
				{ false, true, false, 100000, 100000 + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND - 1, false, true, pbState, PBActionNone, false },
				{ false, true, true, 100000, 100000 + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND - 1, true, true, pbState, PBActionNone, false },
				{ true, false, false, 100000, 100000 + PBT_UPLOAD_DELAY_PLAYER_ACTIVE * MILLISECONDS_PER_SECOND - 1, false, false, pbState, PBActionGetEGMBalances, false },
				{ true, false, true, 100000, 100000 + PBT_UPLOAD_DELAY_PLAYER_ACTIVE * MILLISECONDS_PER_SECOND - 1, true, false, pbState, PBActionGetEGMBalances, false },
				{ true, true, false, 100000, 100000 + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND - 1, false, true, pbState, PBActionNone, false },
				{ true, true, true, 100000, 100000 + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND - 1, true, true, pbState, PBActionNone, false },

				// Test the minimum time for expired timers.
				{ false, false, false, 100000, 100000 + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND, false, true, pbState, PBActionNone, true },
				{ false, false, true, 100000, 100000 + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND, true, true, pbState, PBActionNone, true },
				{ false, true, false, 100000, 100000 + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND, false, true, pbState, PBActionNone, true },
				{ false, true, true, 100000, 100000 + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND, true, true, pbState, PBActionNone, true },
				{ true, false, false, 100000, 100000 + PBT_UPLOAD_DELAY_PLAYER_ACTIVE * MILLISECONDS_PER_SECOND, false, false, pbState, PBActionGetEGMBalances, true },
				{ true, false, true, 100000, 100000 + PBT_UPLOAD_DELAY_PLAYER_ACTIVE * MILLISECONDS_PER_SECOND, true, false, pbState, PBActionGetEGMBalances, true },
				{ true, true, false, 100000, 100000 + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND, false, true, pbState, PBActionNone, true },
				{ true, true, true, 100000, 100000 + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND, true, true, pbState, PBActionNone, true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBanker varPb;

				if (testIndex == 4)
				{
					varPb.SetToDelayAutoUpload();
				}

				const bool sendToPoller(false);
				varPb.BeginPersonalBankerAutoSession(sendToPoller,
					config, 
					TEST_DATA[testIndex].m_haveAutoUploadPIN ? PBT_AUTO_UPLOAD_PIN : ~PBT_AUTO_UPLOAD_PIN,
					CUtilities::GetTickCount());

				if (TEST_DATA[testIndex].m_haveAutoUploadCard)
				{
					varPb.RebuildCard();
				}

				// Setup to ensure that CPersonalBanker::PerformTimedEventsTransfer() does not call SetPersonalBankerAction().
				// BeginPersonalBankerAutoSession() already set m_pbTransfer != NULL.
				varPb.SetState(pbState);

				// Setup to ensure that CPersonalBanker::PerformTimedEventsAuto() calls PerformTimedEventsAutoNotDefaulted().
				varPb.MakeAutoUploadPending();
				const bool defaultedPlayer(false);
				const bool delayAutoNcepOutWhenNotDefaultedPlayer(false);

				// Finish setup.
				varPb.ClearPersonalBankerAction();

				// Perform operations to be tested.
				varPb.CPersonalBanker::PerformTimedEvents(delayAutoNcepOutWhenNotDefaultedPlayer,
					TEST_DATA[testIndex].m_haveEmployee1,
					config,
					TEST_DATA[testIndex].m_tickCountStart,
					sessionTimeoutValueInMilliseconds);
				const CPersonalBanker &pb(varPb);

				// Test #1.
				const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();

				{
					bool areEqual = ((pbTransferResult != NULL) == pbTransferExpected);

					string failedMsg;
					failedMsg = FormatString("FAILED pbTransferResult=%p expected=%s testIndex=%d in %s(%d)",
						(const void *)pbTransferResult,
						(pbTransferExpected ? "NOT NUL " :  _T("NULL")) ,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #2.
				{
					const PersonalBankerState pbStateResult = pb.GetState();

					const bool areEqual = pbStateResult == TEST_DATA[testIndex].m_pbStateExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbStateResult=%d expected=%d testIndex=%d in %s(%d)",
						pbStateResult,
						TEST_DATA[testIndex].m_pbStateExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #3.
				{
					const PersonalBankerAction pbActionResult = pb.GetAction();

					const bool areEqual = pbActionResult == TEST_DATA[testIndex].m_pbActionExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbActionResult=%d expected=%d testIndex=%d in %s(%d)",
						pbActionResult,
						TEST_DATA[testIndex].m_pbActionExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #4.
				{
					const bool pbAutoTransferPendingResult = varPb.IsAutoTransferPending();

					const bool areEqual = pbAutoTransferPendingResult == TEST_DATA[testIndex].m_pbAutoTransferPendingExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbAutoTransferPendingResult=%d expected=%d testIndex=%d in %s(%d)",
						pbAutoTransferPendingResult,
						TEST_DATA[testIndex].m_pbAutoTransferPendingExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #5.
				{
					const bool pbAutoNcepOutPendingResult = pb.IsAutoUploadPending();

					const bool areEqual = pbAutoNcepOutPendingResult == TEST_DATA[testIndex].m_pbAutoNcepOutPendingExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbAutoNcepOutPendingResult=%d expected=%d testIndex=%d in %s(%d)",
						pbAutoNcepOutPendingResult,
						TEST_DATA[testIndex].m_pbAutoNcepOutPendingExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #6 -- destructive timer test, so do this last.
				{
					const bool autoUploadTimerExpiredResult = IsAutoUploadTimerExpired(varPb, TEST_DATA[testIndex].m_tickCountExpired);

					const bool areEqual = autoUploadTimerExpiredResult == TEST_DATA[testIndex].m_autoUploadTimerExpiredExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED autoUploadTimerExpiredResult=%d expected=%d testIndex=%d in %s(%d)",
						autoUploadTimerExpiredResult,
						TEST_DATA[testIndex].m_autoUploadTimerExpiredExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::PerformTimedEventsAuto()
		/// Code coverage and functional test.
		/// Includes functional testing for CPersonalBanker::m_autoUpload.IsUploadTimerExpired().
		/// Test with m_pbTransfer != NULL and set so CPersonalBanker::IsWaitingForTransferToBeCompleted() returns false.
		/// </summary>
		TEST(PersonalBanker5, PerformTimedEventsAutoTest)
		{
			// Test data inputs - same regardless of testIndex.
			const bool waitingForTransferToBeCompleted(false);
			const PersonalBankerState pbState(PBStateUnknown);
			const bool haveEmployee1(true);
			const CConfig config(false, "");
			const DWORD sessionTimeoutValueInMilliseconds(0);
			const DWORD tickCountStart(100000); // Tick count start for timer expirations.
			const DWORD tcUpDlyExpired(tickCountStart + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND); // Minimum tick count for PBT_UPLOAD_DELAY timer expired.
			const DWORD tcUpDlyNotExpired(tcUpDlyExpired - 1); // Maximum tick count for PBT_UPLOAD_DELAY timer not expired.
			const DWORD tcUpDlyActiveExpired(tickCountStart + PBT_UPLOAD_DELAY_PLAYER_ACTIVE * MILLISECONDS_PER_SECOND); // Minimum tick count for PBT_UPLOAD_DELAY_PLAYER_ACTIVE timer expired.
			const DWORD tcUpDlyActiveNotExpired(tcUpDlyActiveExpired - 1); // Maximum tick count for PBT_UPLOAD_DELAY_PLAYER_ACTIVE timer not expired.

			// Test data - expected results, same regardless of testIndex
			const PersonalBankerState pbStateExpected(pbState);
			const bool pbTransferExpected(false);

			typedef struct
			{
				// Inputs.
				const bool m_haveAutoUploadPIN;
				const bool m_haveAutoUploadCard;
				const bool m_autoInProcess;
				const bool m_haveAutoStartedOnce;
				const DWORD m_tickCountExpired; // Tick count for expiration test -- generally either the maximum non-expired time or the minimum expired time.

				// Expected results.
				const bool m_pbAutoTransferPendingExpected;
				const bool m_pbAutoNcepOutPendingExpected;
				const PersonalBankerAction m_pbActionExpected;
				const bool m_autoUploadTimerExpiredExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// m_autoUpload.IsAutoUploadPending() && 
				// !defaultedPlayer && 
				// m_autoUpload.IsUploadTimerExpired(tickCount))
				// !delayAutoNcepOutWhenNotDefaultedPlayer)
				// Timers expire after 3 seconds, if active.
				{ false, false, true, true, tcUpDlyNotExpired, false, true, PBActionSendCardSessionInProgressToUI, false },
				{ false, true, true, true, tcUpDlyNotExpired, true, true, PBActionSendCardSessionInProgressToUI, false },
				{ true, false, true, true, tcUpDlyNotExpired, false, true, PBActionSendCardSessionInProgressToUI, false },
				{ true, true, true, true, tcUpDlyNotExpired, true, true, PBActionSendCardSessionInProgressToUI, false },
				{ false, false, true, false, tcUpDlyNotExpired, false, true, PBActionSendCardSessionInProgressToUI, false },
				{ false, true, true, false, tcUpDlyNotExpired, true, true, PBActionSendCardSessionInProgressToUI, false },
				{ true, false, true, false, tcUpDlyNotExpired, false, true, PBActionSendCardSessionInProgressToUI, false },
				{ true, true, true, false, tcUpDlyNotExpired, true, true, PBActionSendCardSessionInProgressToUI, false },

				{ false, false, true, true, tcUpDlyExpired, false, true, PBActionSendCardSessionInProgressToUI, true },
				{ false, true, true, true, tcUpDlyExpired, true, true, PBActionSendCardSessionInProgressToUI, true },
				{ true, false, true, true, tcUpDlyExpired, false, true, PBActionSendCardSessionInProgressToUI, true },
				{ true, true, true, true, tcUpDlyExpired, true, true, PBActionSendCardSessionInProgressToUI, true },
				{ false, false, true, false, tcUpDlyExpired, false, true, PBActionSendCardSessionInProgressToUI, true },
				{ false, true, true, false, tcUpDlyExpired, true, true, PBActionSendCardSessionInProgressToUI, true },
				{ true, false, true, false, tcUpDlyExpired, false, true, PBActionSendCardSessionInProgressToUI, true },
				{ true, true, true, false, tcUpDlyExpired, true, true, PBActionSendCardSessionInProgressToUI, true },

			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPersonalBanker varPb;


				if (TEST_DATA[testIndex].m_haveAutoStartedOnce)
				{
					varPb.SetAutoInProcess(true);
					const DWORD cashOnGame(0);
					const DWORD promoOnGame(1);
					const bool canTransferOff(true);
					const bool callToEGMFailed(false);
					varPb.ProcessBalancesFromEGM(cashOnGame,
						0,
						promoOnGame,
						canTransferOff,
						callToEGMFailed,
						config);
				}

				const bool sendToPoller(false);
				varPb.BeginPersonalBankerAutoSession(sendToPoller,
					config, 
					TEST_DATA[testIndex].m_haveAutoUploadPIN ? PBT_AUTO_UPLOAD_PIN : ~PBT_AUTO_UPLOAD_PIN,
					CUtilities::GetTickCount());

				if (TEST_DATA[testIndex].m_haveAutoUploadCard)
				{
					varPb.RebuildCard();
				}

				// Setup to ensure that CPersonalBanker::PerformTimedEventsTransfer() does not call SetPersonalBankerAction().
				// BeginPersonalBankerAutoSession() already set m_pbTransfer != NULL.
				varPb.RemoveSession();
				varPb.SetWaitingForTransferToBeCompleted(waitingForTransferToBeCompleted);
				varPb.SetState(pbState);

				// Setup to ensure that CPersonalBanker::PerformTimedEventsAuto() does NOT call PerformTimedEventsAutoNotDefaulted().
				varPb.MakeAutoUploadPending();
				const bool delayAutoNcepOutWhenNotDefaultedPlayer(true);

				// Finish setup.
				varPb.SetAutoInProcess(TEST_DATA[testIndex].m_autoInProcess);
				varPb.ClearPersonalBankerAction();

				// Perform operations to be tested.
				varPb.CPersonalBanker::PerformTimedEvents(delayAutoNcepOutWhenNotDefaultedPlayer,
					haveEmployee1,
					config,
					tickCountStart,
					sessionTimeoutValueInMilliseconds);
				const CPersonalBanker &pb(varPb);

				// Test #1.
				const CPersonalBankerTransfer *pbTransferResult = pb.GetTransferCopy();

				{
					bool areEqual = ((pbTransferResult != NULL) == pbTransferExpected);

					string failedMsg;
					failedMsg = FormatString("FAILED pbTransferResult=%p expected=%s testIndex=%d in %s(%d)",
						(const void *)pbTransferResult,
						(pbTransferExpected ? "NOT NUL" :  _T("NULL")) ,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #2.
				{
					const PersonalBankerState pbStateResult = pb.GetState();

					const bool areEqual = pbStateResult == pbStateExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbStateResult=%d expected=%d testIndex=%d in %s(%d)",
						pbStateResult,
						pbStateExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #3.
				{
					const PersonalBankerAction pbActionResult = pb.GetAction();

					const bool areEqual = pbActionResult == TEST_DATA[testIndex].m_pbActionExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbActionResult=%d expected=%d testIndex=%d in %s(%d)",
						pbActionResult,
						TEST_DATA[testIndex].m_pbActionExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #4.
				{
					const bool pbAutoTransferPendingResult = varPb.IsAutoTransferPending();

					const bool areEqual = pbAutoTransferPendingResult == TEST_DATA[testIndex].m_pbAutoTransferPendingExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbAutoTransferPendingResult=%d expected=%d testIndex=%d in %s(%d)",
						pbAutoTransferPendingResult,
						TEST_DATA[testIndex].m_pbAutoTransferPendingExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #5.
				{
					const bool pbAutoNcepOutPendingResult = pb.IsAutoUploadPending();

					const bool areEqual = pbAutoNcepOutPendingResult == TEST_DATA[testIndex].m_pbAutoNcepOutPendingExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED pbAutoNcepOutPendingResult=%d expected=%d testIndex=%d in %s(%d)",
						pbAutoNcepOutPendingResult,
						TEST_DATA[testIndex].m_pbAutoNcepOutPendingExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Test #6 -- destructive timer test, so do this last.
				{
					const bool autoUploadTimerExpiredResult = IsAutoUploadTimerExpired(varPb, TEST_DATA[testIndex].m_tickCountExpired);

					const bool areEqual = autoUploadTimerExpiredResult == TEST_DATA[testIndex].m_autoUploadTimerExpiredExpected;
					string failedMsg;
					failedMsg = FormatString("FAILED autoUploadTimerExpiredResult=%d expected=%d testIndex=%d in %s(%d)",
						autoUploadTimerExpiredResult,
						TEST_DATA[testIndex].m_autoUploadTimerExpiredExpected,
						testIndex,
						(__TFUNCTION__), __LINE__);
					ASSERT_TRUE(areEqual) <<  failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::PerformTimedEventsTransfer()
		/// Code coverage and functional test.
		/// </summary>
		TEST(PersonalBanker5, PerformTimedEventsTransferTest)
		{
			// Test data inputs - same regardless of testIndex.
			const bool haveEmployee1(true);
			const WORD configDenomination(25);
			const bool defaultedPlayer(true);
			const bool delayAutoNcepOutWhenNotDefaultedPlayer(true);
			const __int64 pendingAutoNCEPAmount(123);
			const string cardIDStr("1234567890");

			const DWORD tickCountStart(100000); // Tick count start for timer expirations.
			const DWORD tickCountStartPrior(tickCountStart - 500); // Tick count start for prior timer expirations.  Subtract an offset different than any expiration.
			const DWORD tickCountStartPrior2(tickCountStart - 750); // Tick count start for additional prior timer expirations.  Subtract an offset different than any expiration.
			const DWORD tcUpDlyExpired(tickCountStart + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND); // Minimum tick count for PBT_UPLOAD_DELAY timer expired.
			const DWORD tcUpDlyNotExpired(tcUpDlyExpired - 1); // Maximum tick count for PBT_UPLOAD_DELAY timer not expired.
			const DWORD tcUpDlyActiveExpired(tickCountStart + PBT_UPLOAD_DELAY_PLAYER_ACTIVE * MILLISECONDS_PER_SECOND); // Minimum tick count for PBT_UPLOAD_DELAY_PLAYER_ACTIVE timer expired.
			const DWORD tcUpDlyActiveNotExpired(tcUpDlyActiveExpired - 1); // Maximum tick count for PBT_UPLOAD_DELAY_PLAYER_ACTIVE timer not expired.
			const DWORD tcPriorUpDlyExpired(tickCountStartPrior + PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND); // Minimum tick count for PBT_UPLOAD_DELAY timer expired.
			const DWORD tcPriorUpDlyNotExpired(tcPriorUpDlyExpired - 1); // Maximum tick count for PBT_UPLOAD_DELAY timer not expired.

			// Tick counts for transfer session timer started at tickCountStart.
			const DWORD tcStartXferExpired(tickCountStart + CPersonalBanker::PB_SESSION_TIMEOUT);  // Minimum tick count for timer expired. 
			const DWORD tcStartXferNotExpired(tcStartXferExpired - 1); // Maximum tick count for timer not expired.

			const DWORD tickCountStartTransferSessionForExpired(tickCountStart - 2 * CPersonalBanker::PB_SESSION_TIMEOUT);
			const DWORD tcXferExpired2(tickCountStartTransferSessionForExpired + CPersonalBanker::PB_SESSION_TIMEOUT); // Minimum tick count for timer expired.
			const DWORD tcXferExpired1(tcXferExpired2 - 1); // Maximum tick count for timer not expired.

			const DWORD tickCountStartTransferSessionForNotExpired(tickCountStart - CPersonalBanker::PB_SESSION_TIMEOUT / 2);
			const DWORD tcXferNotExpired2(tickCountStartTransferSessionForNotExpired + CPersonalBanker::PB_SESSION_TIMEOUT); // Minimum tick count for timer expired.
			const DWORD tcXferNotExpired1(tcXferNotExpired2 - 1); // Maximum tick count for timer not expired.

			typedef struct
			{
				// Inputs.
				const bool m_haveTransfer;
				const bool m_haveTransferSessionExpired;
				const bool m_waitingForTransferToBeCompleted;
				const bool m_autoInProcess;
				const bool m_pbAutoNcepOutPending; // If true, also sets m_autoUpload.SetAutoInProcess(false) and m_autoUpload.IsUploadTimerExpired() to true.
				const PersonalBankerState m_pbState;

				// Expected results.
				const bool m_pbTransferExpected;
				const PersonalBankerState m_pbStateExpected;
				const PersonalBankerAction m_pbActionExpected;
				const bool m_pbWaitingForTransferToBeCompletedExpected;
				const bool m_autoInProcessExpected; // 2013-06-27 CPersonalBanker::m_autoUpload.m_pbtAutoInProcess cannot be monitored directly.
				const bool m_pbAutoTransferPendingExpected;
				const bool m_pbAutoNcepOutPendingExpected;
				const bool m_pbSessionReleasedDueToTimeoutExpected;
				// 2013-06-27 CPersonalBanker::m_pbTransfer.m_type (MEMORY_TYPE) cannot be monitored directly.
				const PollerCancellationType m_pbtCancelledTypeExpected;
				const PersonalBankerType m_pbtPersonalBankerTypeExpected;

				const __int64 m_pbtRestrictedAmountExpected;
				const DWORD m_pbtPINExpected;
				const WORD m_pbtDenominationExpected;
				const int m_pbtCardNumberLenExpected;
				const byte m_pbtCardNumberExpected[CARD_ID_LEN];
				const bool m_pbtUtcCardInTimeIsZero;
				const DWORD m_tickCountAutoUploadTimerExpired1; // Tick count for 1st auto upload expiration test -- generally the maximum non-expired time.
				const DWORD m_tickCountAutoUploadTimerExpired2; // Tick count for 2nd auto upload expiration test -- generally the minimum expired time.
				const bool m_autoUploadTimerExpired1Expected;
				const bool m_autoUploadTimerExpired2Expected;

				const DWORD m_tickCountTransferSessionTimerExpired1; // Tick count for 1st transfer session expiration test -- generally the maximum non-expired time.
				const DWORD m_tickCountTransferSessionTimerExpired2; // Tick count for 2nd transfer session expiration test -- generally the minimum expired time.
				const bool m_transferSessionTimerExpired1Expected;
				const bool m_transferSessionTimerExpired2Expected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// Permuatations with CPersonalBanker::m_state == PBStateUnknown.
				//#0
				{ false, false, false, false, true, PBStateUnknown,
					false, PBStateUnknown, PBActionNone, false, false, true, true, false, _Unknown, Unspecified,
					0, 0, 0, 0, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					0, 0, false, true },
				//#1
				{ false, false, false, true, true, PBStateUnknown,
					false, PBStateUnknown, PBActionNone, false, true, true, true, false, _Unknown, Unspecified,
					0, 0, 0, 0, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					0, 0, false, true },
				//#2
				{ false, false, true, false, false, PBStateUnknown,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, false, false, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, 0, 0, true, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#3
				{ false, false, true, false, true, PBStateUnknown,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, true, true, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#4
				{ false, false, true, true, false, PBStateUnknown,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, false, false, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, 0, 0, true, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#5			
				{ false, false, true, true, true, PBStateUnknown,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, true, true, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#6			
				{ false, true, false, false, true, PBStateUnknown,
					false, PBStateUnknown, PBActionNone, false, false, true, true, false, _Unknown, Unspecified,
					0, 0, 0, 0, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					0, 0, false, true },
				//#7			
				{ false, true, false, true, true, PBStateUnknown,
					false, PBStateUnknown, PBActionNone, false, true, true, true, false, _Unknown, Unspecified,
					0, 0, 0, 0, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					0, 0, false, true },
				//#8			
				{ false, true, true, false, false, PBStateUnknown,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, false, false, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, 0, 0, true, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#9
				{ false, true, true, false, true, PBStateUnknown,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, true, true, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#10
				{ false, true, true, true, false, PBStateUnknown,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, false, false, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, 0, 0, true, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#11
				{ false, true, true, true, true, PBStateUnknown,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, true, true, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#12
				{ true, false, false, false, true, PBStateUnknown,
					true, PBStateUnknown, PBActionNone, false, false, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferNotExpired1, tcXferNotExpired2, false, true },
				//#13
				{ true, false, false, true, true, PBStateUnknown,
					true, PBStateUnknown, PBActionNone, false, true, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferNotExpired1, tcXferNotExpired2, false, true },
				//#14
				{ true, false, true, false, true, PBStateUnknown,
					true, PBStateUnknown, PBActionNone, true, false, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferNotExpired1, tcXferNotExpired2, false, true },
				//#15
				{ true, false, true, true, true, PBStateUnknown,
					true, PBStateUnknown, PBActionNone, true, true, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferNotExpired1, tcXferNotExpired2, false, true },
				//#16
				{ true, true, false, false, true, PBStateUnknown,
					true, PBStateUnknown, PBActionNone, false, false, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#17
				{ true, true, false, true, true, PBStateUnknown,
					true, PBStateUnknown, PBActionNone, false, true, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#18
				{ true, true, true, false, true, PBStateUnknown,
					true, PBStateUnknown, PBActionNone, true, false, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#19
				{ true, true, true, true, true, PBStateUnknown,
					true, PBStateUnknown, PBActionNone, true, true, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },

				// Permuatations with CPersonalBanker::m_state == PBStateInquireSent.
				//#20
				{ false, false, false, false, true, PBStateInquireSent,
					false, PBStateInquireSent, PBActionNone, false, false, true, true, false, _Unknown, Unspecified,
					0, 0, 0, 0, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					0, 0, false, true },
				//#21
				{ false, false, false, true, true, PBStateInquireSent,
					false, PBStateInquireSent, PBActionNone, false, true, true, true, false, _Unknown, Unspecified,
					0, 0, 0, 0, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					0, 0, false, true },
				//#22
				{ false, false, true, false, false, PBStateInquireSent,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, false, false, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, 0, 0, true, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#23
				{ false, false, true, false, true, PBStateInquireSent,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, true, true, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#24
				{ false, false, true, true, false, PBStateInquireSent,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, false, false, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, 0, 0, true, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#25
				{ false, false, true, true, true, PBStateInquireSent,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, true, true, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#26
				{ false, true, false, false, true, PBStateInquireSent,
					false, PBStateInquireSent, PBActionNone, false, false, true, true, false, _Unknown, Unspecified,
					0, 0, 0, 0, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					0, 0, false, true },
				//#27
				{ false, true, false, true, true, PBStateInquireSent,
					false, PBStateInquireSent, PBActionNone, false, true, true, true, false, _Unknown, Unspecified,
					0, 0, 0, 0, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					0, 0, false, true },
				//#28
				{ false, true, true, false, false, PBStateInquireSent,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, false, false, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, 0, 0, true, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#29
				{ false, true, true, false, true, PBStateInquireSent,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, true, true, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#30
				{ false, true, true, true, false, PBStateInquireSent,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, false, false, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, 0, 0, true, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#31
				{ false, true, true, true, true, PBStateInquireSent,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, true, true, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#32
				{ true, false, false, false, true, PBStateInquireSent,
					true, PBStateInquireSent, PBActionNone, false, false, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferNotExpired1, tcXferNotExpired2, false, true },
				//#33
				{ true, false, false, true, true, PBStateInquireSent,
					true, PBStateInquireSent, PBActionNone, false, true, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferNotExpired1, tcXferNotExpired2, false, true },
				//#34
				{ true, false, true, false, true, PBStateInquireSent,
					true, PBStateInquireSent, PBActionNone, true, false, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferNotExpired1, tcXferNotExpired2, false, true },
				//#35
				{ true, false, true, true, true, PBStateInquireSent,
					true, PBStateInquireSent, PBActionNone, true, true, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferNotExpired1, tcXferNotExpired2, false, true },
				//#36
				{ true, true, false, false, false, PBStateInquireSent,
					true, PBStateInquireSent, PBActionReleaseSession, false, false, false, false, true, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, 0, 0, true, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#37
				{ true, true, false, false, true, PBStateInquireSent,
					true, PBStateInquireSent, PBActionReleaseSession, false, false, true, true, true, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#38
				{ true, true, false, true, false, PBStateInquireSent,
					true, PBStateInquireSent, PBActionSendTransferTransaction, false, false, true, true, false, AutomaticUploadCanceledBySystem, Unspecified,
					pendingAutoNCEPAmount, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcUpDlyNotExpired, tcUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#39
				{ true, true, false, true, true, PBStateInquireSent,
					true, PBStateInquireSent, PBActionSendTransferTransaction, false, false, true, true, false, AutomaticUploadCanceledBySystem, Unspecified,
					pendingAutoNCEPAmount, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcUpDlyNotExpired, tcUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#40
				{ true, true, true, false, false, PBStateInquireSent,
					true, PBStateInquireSent, PBActionReleaseSession, true, false, false, false, true, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, 0, 0, true, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#41
				{ true, true, true, false, true, PBStateInquireSent,
					true, PBStateInquireSent, PBActionReleaseSession, true, false, true, true, true, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#42
				{ true, true, true, true, false, PBStateInquireSent,
					true, PBStateInquireSent, PBActionSendTransferTransaction, true, false, true, true, false, AutomaticUploadCanceledBySystem, Unspecified,
					pendingAutoNCEPAmount, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcUpDlyNotExpired, tcUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#43
				{ true, true, true, true, true, PBStateInquireSent,
					true, PBStateInquireSent, PBActionSendTransferTransaction, true, false, true, true, false, AutomaticUploadCanceledBySystem, Unspecified,
					pendingAutoNCEPAmount, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcUpDlyNotExpired, tcUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },

				// Permuatations with CPersonalBanker::m_state == PBStateInfoReceived.
				//#44
				{ false, false, false, false, true, PBStateInfoReceived,
					false, PBStateInfoReceived, PBActionNone, false, false, true, true, false, _Unknown, CanceledPromoCreditsUploadAuto,
					0, 0, 0, 0, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					0, 0, false, true },
				//#45
				{ false, false, false, true, true, PBStateInfoReceived,
					false, PBStateInfoReceived, PBActionNone, false, true, true, true, false, _Unknown, Unspecified,
					0, 0, 0, 0, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					0, 0, false, true },
				//#46
				{ false, false, true, false, false, PBStateInfoReceived,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, false, false, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, 0, 0, true, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#47
				{ false, false, true, false, true, PBStateInfoReceived,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, true, true, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#48
				{ false, false, true, true, false, PBStateInfoReceived,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, false, false, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, 0, 0, true, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#49
				{ false, false, true, true, true, PBStateInfoReceived,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, true, true, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#50
				{ false, true, false, false, true, PBStateInfoReceived,
					false, PBStateInfoReceived, PBActionNone, false, false, true, true, false, _Unknown, Unspecified,
					0, 0, 0, 0, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					0, 0, false, true },
				//#51
				{ false, true, false, true, true, PBStateInfoReceived,
					false, PBStateInfoReceived, PBActionNone, false, true, true, true, false, _Unknown, Unspecified,
					0, 0, 0, 0, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					0, 0, false, true },
				//#52
				{ false, true, true, false, false, PBStateInfoReceived,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, false, false, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, 0, 0, true, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#53
				{ false, true, true, false, true, PBStateInfoReceived,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, true, true, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#54
				{ false, true, true, true, false, PBStateInfoReceived,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, false, false, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, 0, 0, true, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#55
				{ false, true, true, true, true, PBStateInfoReceived,
					true, PBStateTransferComplete, PBActionSendPersonalBankerNcepOutFail, false, false, true, true, false, AutomaticUploadCanceledByUser, Unspecified,
					pendingAutoNCEPAmount, PBT_AUTO_UPLOAD_PIN, configDenomination, CARD_ID_LEN, { 0x12, 0x34, 0x56, 0x78, 0x90 }, false, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcStartXferNotExpired, tcStartXferExpired, false, true },
				//#56
				{ true, false, false, false, true, PBStateInfoReceived,
					true, PBStateInfoReceived, PBActionNone, false, false, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferNotExpired1, tcXferNotExpired2, false, true },
				//#57
				{ true, false, false, true, true, PBStateInfoReceived,
					true, PBStateInfoReceived, PBActionNone, false, true, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferNotExpired1, tcXferNotExpired2, false, true },
				//#58
				{ true, false, true, false, true, PBStateInfoReceived,
					true, PBStateInfoReceived, PBActionNone, true, false, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferNotExpired1, tcXferNotExpired2, false, true },
				//#59
				{ true, false, true, true, true, PBStateInfoReceived,
					true, PBStateInfoReceived, PBActionNone, true, true, true, true, false, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferNotExpired1, tcXferNotExpired2, false, true },
				//#60
				{ true, true, false, false, false, PBStateInfoReceived,
					true, PBStateInfoReceived, PBActionReleaseSession, false, false, false, false, true, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, 0, 0, true, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#61
				{ true, true, false, false, true, PBStateInfoReceived,
					true, PBStateInfoReceived, PBActionReleaseSession, false, false, true, true, true, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#62
				{ true, true, false, true, false, PBStateInfoReceived,
					true, PBStateInfoReceived, PBActionSendTransferTransaction, false, false, true, true, false, AutomaticUploadCanceledBySystem, Unspecified,
					pendingAutoNCEPAmount, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcUpDlyNotExpired, tcUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#63
				{ true, true, false, true, true, PBStateInfoReceived,
					true, PBStateInfoReceived, PBActionSendTransferTransaction, false, false, true, true, false, AutomaticUploadCanceledBySystem, Unspecified,
					pendingAutoNCEPAmount, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcUpDlyNotExpired, tcUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#64
				{ true, true, true, false, false, PBStateInfoReceived,
					true, PBStateInfoReceived, PBActionReleaseSession, true, false, false, false, true, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, 0, 0, true, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#65
				{ true, true, true, false, true, PBStateInfoReceived,
					true, PBStateInfoReceived, PBActionReleaseSession, true, false, true, true, true, _Unknown, Unspecified,
					0, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcPriorUpDlyNotExpired, tcPriorUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#66
				{ true, true, true, true, false, PBStateInfoReceived,
					true, PBStateInfoReceived, PBActionSendTransferTransaction, true, false, true, true, false, AutomaticUploadCanceledBySystem, Unspecified,
					pendingAutoNCEPAmount, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcUpDlyNotExpired, tcUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },
				//#67
				{ true, true, true, true, true, PBStateInfoReceived,
					true, PBStateInfoReceived, PBActionSendTransferTransaction, true, false, true, true, false, AutomaticUploadCanceledBySystem, Unspecified,
					pendingAutoNCEPAmount, 0, 0, CARD_ID_LEN, { 0, 0, 0, 0, 0 }, true, tcUpDlyNotExpired, tcUpDlyExpired, false, true,
					tcXferExpired1, tcXferExpired2, false, true },
			};

			const int NUMBER_OF_DESTRUCTIVE_TESTS = 3; // Repeat the general test iterations, doing one destructive test with each iteration.

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				for (int destructiveIndex = 0; destructiveIndex < NUMBER_OF_DESTRUCTIVE_TESTS; ++destructiveIndex)
				{
					// Setup.
					CConfig varConfig(false, "");
					varConfig.SetAutoUploadCreditsGlobal(true);
					varConfig.SetSenOption2Raw(varConfig.GetSenOption2Raw() + SO2_NCEP_AUTO_UPLOAD);
					varConfig.SetSenOption2Raw(varConfig.GetSenOption2Raw() + SO2_NCEP_OUT_ENABLED);
					varConfig.SetPBTPromoEnabled_Global(true);
					varConfig.SetDenomination(configDenomination);
					const CConfig &config(varConfig);

					// Setup so CPersonalBanker::PerformTimedEvents() does NOT change CPersonalBanker state after calling PerformTimedEventsTransfer().
					// 2013-06-27 CPersonalBanker::m_autoUpload.m_pbtAutoUploadPending is false by default.
					CPersonalBanker varPb;

					// Set varPb.m_autoUpload.m_pendingAutoNCEPAmount.
					{
						DWORD cashOnGame(0);
						DWORD promoOnGame(pendingAutoNCEPAmount);
						bool canTransferOff(true);
						bool callToEGMFailed(false);
						varPb.SetAutoInProcess(true);
						varPb.ProcessBalancesFromEGM(cashOnGame,
							0,
							promoOnGame,
							canTransferOff,
							callToEGMFailed,
							config);
					}

					// If m_pbAutoNcepOutPending, also set m_autoUpload.SetAutoInProcess(false) and m_autoUpload.IsUploadTimerExpired() to true.
					if (TEST_DATA[testIndex].m_pbAutoNcepOutPending)
					{
						const bool sendToPoller(false);
						const DWORD autoUploadPin = PBT_AUTO_UPLOAD_PIN;
						varPb.CreateTransfer(tickCountStartPrior2);
						// Setup for CPersonalBanker::BeginPersonalBankerAutoSession() to set:
						//     m_autoUpload.SetAutoInProcess(false);
						//     m_autoUpload.SetAutoUploadPending(true);
						//     	m_autoUpload.SetUploadDelay(tickCount, PBT_UPLOAD_DELAY * MILLISECONDS_PER_SECOND);
						varPb.BeginPersonalBankerAutoSession(sendToPoller,
							config, 
							PBT_AUTO_UPLOAD_PIN,
							tickCountStartPrior);
					}

					if (TEST_DATA[testIndex].m_haveTransfer)
					{
						DWORD tickCount = TEST_DATA[testIndex].m_haveTransferSessionExpired ?
						tickCountStartTransferSessionForExpired :
						tickCountStartTransferSessionForNotExpired;
						varPb.CreateTransfer(tickCount);
					}
					else
					{
						varPb.DoneWithTransfer();
					}

					CCard varCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
					varCard.SetCardID(cardIDStr);
					const CCard &card(varCard);
					varPb.RebuildCard(&card);

					varPb.SetWaitingForTransferToBeCompleted(TEST_DATA[testIndex].m_waitingForTransferToBeCompleted);
					varPb.SetState(TEST_DATA[testIndex].m_pbState);
					varPb.SetAutoInProcess(TEST_DATA[testIndex].m_autoInProcess);
					varPb.ClearPersonalBankerAction();

					// Perform operations to be tested.
					varPb.CPersonalBanker::PerformTimedEvents(delayAutoNcepOutWhenNotDefaultedPlayer,
						haveEmployee1,
						config,
						tickCountStart);
					const CPersonalBanker &pb(varPb);

					// Only do the non-destructive tests once per testIndex iteratation, to reduce test execution time.
					if (destructiveIndex == 0)
					{
						// Test #1.
						{
							const PersonalBankerState pbStateResult = pb.GetState();

							const bool areEqual = pbStateResult == TEST_DATA[testIndex].m_pbStateExpected;
							string failedMsg;
							failedMsg = FormatString("FAILED pbStateResult=%d expected=%d testIndex=%d in %s(%d)",
								pbStateResult,
								TEST_DATA[testIndex].m_pbStateExpected,
								testIndex,
								(__TFUNCTION__), __LINE__);
							ASSERT_TRUE(areEqual) <<  failedMsg;
						}

						// Test #2.
						{
							const PersonalBankerAction pbActionResult = pb.GetAction();

							const bool areEqual = pbActionResult == TEST_DATA[testIndex].m_pbActionExpected;
							string failedMsg;
							failedMsg = FormatString("FAILED pbActionResult=%d expected=%d testIndex=%d in %s(%d)",
								pbActionResult,
								TEST_DATA[testIndex].m_pbActionExpected,
								testIndex,
								(__TFUNCTION__), __LINE__);
							ASSERT_TRUE(areEqual) <<  failedMsg;
						}

						// Test #3.
						{
							const bool pbWaitingForTransferToBeCompletedResult = pb.IsWaitingForTransferToBeCompleted();

							const bool areEqual = pbWaitingForTransferToBeCompletedResult == TEST_DATA[testIndex].m_pbWaitingForTransferToBeCompletedExpected;
							string failedMsg;
							failedMsg = FormatString("FAILED pbWaitingForTransferToBeCompletedResult=%d expected=%d testIndex=%d in %s(%d)",
								pbWaitingForTransferToBeCompletedResult,
								TEST_DATA[testIndex].m_pbWaitingForTransferToBeCompletedExpected,
								testIndex,
								(__TFUNCTION__), __LINE__);
							ASSERT_TRUE(areEqual) <<  failedMsg;
						}

						// Test #4.
						{
							const bool pbAutoTransferPendingResult = varPb.IsAutoTransferPending();

							const bool areEqual = pbAutoTransferPendingResult == TEST_DATA[testIndex].m_pbAutoTransferPendingExpected;
							string failedMsg;
							failedMsg = FormatString("FAILED pbAutoTransferPendingResult=%d expected=%d testIndex=%d in %s(%d)",
								pbAutoTransferPendingResult,
								TEST_DATA[testIndex].m_pbAutoTransferPendingExpected,
								testIndex,
								(__TFUNCTION__), __LINE__);
							ASSERT_TRUE(areEqual) <<  failedMsg;
						}

						// Test #5.
						{
							const bool pbAutoNcepOutPendingResult = pb.IsAutoUploadPending();

							const bool areEqual = pbAutoNcepOutPendingResult == TEST_DATA[testIndex].m_pbAutoNcepOutPendingExpected;
							string failedMsg;
							failedMsg = FormatString("FAILED pbAutoNcepOutPendingResult=%d expected=%d testIndex=%d in %s(%d)",
								pbAutoNcepOutPendingResult,
								TEST_DATA[testIndex].m_pbAutoNcepOutPendingExpected,
								testIndex,
								(__TFUNCTION__), __LINE__);
							ASSERT_TRUE(areEqual) <<  failedMsg;
						}

						// Test #6.
						{
							const bool pbSessionReleasedDueToTimeoutResult = pb.IsSessionReleasedDueToTimeout();

							const bool areEqual = pbSessionReleasedDueToTimeoutResult == TEST_DATA[testIndex].m_pbSessionReleasedDueToTimeoutExpected;
							string failedMsg;
							failedMsg = FormatString("FAILED pbSessionReleasedDueToTimeoutResult=%d expected=%d testIndex=%d in %s(%d)",
								pbSessionReleasedDueToTimeoutResult,
								TEST_DATA[testIndex].m_pbSessionReleasedDueToTimeoutExpected,
								testIndex,
								(__TFUNCTION__), __LINE__);
							ASSERT_TRUE(areEqual) <<  failedMsg;
						}

						// Test #T0.
						const CPersonalBankerTransfer * const pbTransferResult = pb.GetTransferCopy();

						{
							bool areEqual = ((pbTransferResult != NULL) == TEST_DATA[testIndex].m_pbTransferExpected);

							string failedMsg;
							failedMsg = FormatString("FAILED pbTransferResult=%p expected=%s testIndex=%d in %s(%d)",
								(const void *)pbTransferResult,
								(TEST_DATA[testIndex].m_pbTransferExpected ? "NOT NUL" :  _T("NULL")) ,
								testIndex,
								(__TFUNCTION__), __LINE__);
							ASSERT_TRUE(areEqual) <<  failedMsg;
						}

						if (TEST_DATA[testIndex].m_pbTransferExpected)
						{
							// Test #T1.
							{
								const PollerCancellationType pbtCancelledTypeResult = pbTransferResult->GetCancelledType();

								const bool areEqual = pbtCancelledTypeResult == TEST_DATA[testIndex].m_pbtCancelledTypeExpected;
								string failedMsg;
								failedMsg = FormatString("FAILED pbtCancelledTypeResult=%d expected=%d testIndex=%d in %s(%d)",
									pbtCancelledTypeResult,
									TEST_DATA[testIndex].m_pbtCancelledTypeExpected,
									testIndex,
									(__TFUNCTION__), __LINE__);
								ASSERT_TRUE(areEqual) <<  failedMsg;
							}

							// Test #T2.
							{
								const PersonalBankerType pbtPersonalBankerTypeResult = pbTransferResult->GetPersonalBankerType();

								const bool areEqual = pbtPersonalBankerTypeResult == TEST_DATA[testIndex].m_pbtPersonalBankerTypeExpected;
								string failedMsg;
								failedMsg = FormatString("FAILED pbtPersonalBankerTypeResult=%d expected=%d testIndex=%d in %s(%d)",
									pbtPersonalBankerTypeResult,
									TEST_DATA[testIndex].m_pbtPersonalBankerTypeExpected,
									testIndex,
									(__TFUNCTION__), __LINE__);
								ASSERT_TRUE(areEqual) <<  failedMsg;
							}


							// Test #T3.
							{
								const __int64 pbtRestrictedAmountResult = pbTransferResult->GetRestrictedAmount();

								const bool areEqual = pbtRestrictedAmountResult == TEST_DATA[testIndex].m_pbtRestrictedAmountExpected;
								string failedMsg;
								failedMsg = FormatString("FAILED pbtRestrictedAmountResult=%lld expected=%lld testIndex=%d in %s(%d)",
									(long long)pbtRestrictedAmountResult,
									(long long)TEST_DATA[testIndex].m_pbtRestrictedAmountExpected,
									testIndex,
									(__TFUNCTION__), __LINE__);
								ASSERT_TRUE(areEqual) <<  failedMsg;
							}

							// Test #T4.
							{
								const DWORD pbtPINResult = pbTransferResult->GetPIN();

								const bool areEqual = pbtPINResult == TEST_DATA[testIndex].m_pbtPINExpected;
								string failedMsg;
								failedMsg = FormatString("FAILED pbtPINResult=%d expected=%d testIndex=%d in %s(%d)",
									pbtPINResult,
									TEST_DATA[testIndex].m_pbtPINExpected,
									testIndex,
									(__TFUNCTION__), __LINE__);
								ASSERT_TRUE(areEqual) <<  failedMsg;
							}

							// Test #T5.
							{
								const WORD pbtDenominationResult = pbTransferResult->GetDenomination();

								const bool areEqual = pbtDenominationResult == TEST_DATA[testIndex].m_pbtDenominationExpected;
								string failedMsg;
								failedMsg = FormatString("FAILED pbtDenominationResult=%d expected=%d testIndex=%d in %s(%d)",
									pbtDenominationResult,
									TEST_DATA[testIndex].m_pbtDenominationExpected,
									testIndex,
									(__TFUNCTION__), __LINE__);
								ASSERT_TRUE(areEqual) <<  failedMsg;
							}

							// Test #T6.
							int varPbtCardNumberLenResult(-12345);
							const byte * const pbtCardNumberResult = pbTransferResult->GetCardNumber(varPbtCardNumberLenResult);
							const int pbtCardNumberLenResult(varPbtCardNumberLenResult);

							{
								const bool areEqual = pbtCardNumberLenResult == TEST_DATA[testIndex].m_pbtCardNumberLenExpected;
								string failedMsg;
								failedMsg = FormatString("FAILED pbtCardNumberLenResult=%d expected=%d testIndex=%d in %s(%d)",
									pbtCardNumberLenResult,
									TEST_DATA[testIndex].m_pbtCardNumberLenExpected,
									testIndex,
									(__TFUNCTION__), __LINE__);
								ASSERT_TRUE(areEqual) <<  failedMsg;
							}

							// Test #T7.
							{
								size_t memcmpSize = (size_t)TEST_DATA[testIndex].m_pbtCardNumberLenExpected;
								bool areEqual = 0 == memcmp(pbtCardNumberResult,
									TEST_DATA[testIndex].m_pbtCardNumberExpected,
									memcmpSize);

								string resultString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(pbtCardNumberResult),
									(DWORD)memcmpSize);
								string expectedString = 
									CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_pbtCardNumberExpected),
									(DWORD)memcmpSize);
								string failedMsg;
								failedMsg = FormatString("FAILED transferPbtID={%s} expected={%s} testIndex=%d in %s",
									(resultString).c_str(),
									(expectedString).c_str(),
									testIndex,
									static_cast<LPCTSTR>(__TFUNCTION__));
								ASSERT_TRUE(areEqual) <<  failedMsg;
							}

							// Test #T8.
							{
								time_t pbtUtcCardInTimeResult = pbTransferResult->GetUTCCardInTime();

								const bool areEqual = (0 == pbtUtcCardInTimeResult) == TEST_DATA[testIndex].m_pbtUtcCardInTimeIsZero;
								string failedMsg;
								failedMsg = FormatString("FAILED pbtDenominationResult=%lld expected=%s testIndex=%d in %s",
									(long long)pbtUtcCardInTimeResult,
									(TEST_DATA[testIndex].m_pbtUtcCardInTimeIsZero ? "ZERO" :  _T("NON-ZERO")),
									testIndex,
									(__TFUNCTION__));
								ASSERT_TRUE(areEqual) <<  failedMsg;
							}

							// Test #T9.
							{
								const bool pbtSessionExpired1 =
									pbTransferResult->HasSessionExpired(TEST_DATA[testIndex].m_tickCountTransferSessionTimerExpired1,
									CPersonalBanker::PB_SESSION_TIMEOUT);

								const bool areEqual = pbtSessionExpired1 == TEST_DATA[testIndex].m_transferSessionTimerExpired1Expected;
								string failedMsg;
								failedMsg = FormatString("FAILED pbtSessionExpired1=%d expected=%d testIndex=%d in %s",
									pbtSessionExpired1,
									TEST_DATA[testIndex].m_transferSessionTimerExpired1Expected,
									testIndex,
									(__TFUNCTION__));
								ASSERT_TRUE(areEqual) <<  failedMsg;
							}

							// Test #T10.
							{
								const bool pbtSessionExpired2 =
									pbTransferResult->HasSessionExpired(TEST_DATA[testIndex].m_tickCountTransferSessionTimerExpired2,
									CPersonalBanker::PB_SESSION_TIMEOUT);

								const bool areEqual = pbtSessionExpired2 == TEST_DATA[testIndex].m_transferSessionTimerExpired2Expected;
								string failedMsg;
								failedMsg = FormatString("FAILED pbtSessionExpired2=%d expected=%d testIndex=%d in %s",
									pbtSessionExpired2,
									TEST_DATA[testIndex].m_transferSessionTimerExpired2Expected,
									testIndex,
									(__TFUNCTION__));
								ASSERT_TRUE(areEqual) <<  failedMsg;
							}
						}

						// Cleanup.
						delete pbTransferResult;
					}

					// Destructive tests.  Do these last, 1 per interation.
					switch (destructiveIndex)
					{
					case 0:
						// Test #D0 -- destructive IsAutoInProgress test, so do this last.
						{
							const bool autoInProcessResult = IsAutoInProcess(varPb);

							const bool areEqual = autoInProcessResult == TEST_DATA[testIndex].m_autoInProcessExpected;
							string failedMsg;
							failedMsg = FormatString("FAILED autoInProcessResult=%d expected=%d testIndex=%d in %s(%d)",
								autoInProcessResult,
								TEST_DATA[testIndex].m_autoInProcessExpected,
								testIndex,
								(__TFUNCTION__), __LINE__);
							ASSERT_TRUE(areEqual) <<  failedMsg;
						}
						break;

					case 1:
						// Test #D1 -- destructive timer test, so do this last.
						{
							const bool autoUploadTimerExpired1Result = IsAutoUploadTimerExpired(varPb, TEST_DATA[testIndex].m_tickCountAutoUploadTimerExpired1);

							const bool areEqual = autoUploadTimerExpired1Result == TEST_DATA[testIndex].m_autoUploadTimerExpired1Expected;
							string failedMsg;
							failedMsg = FormatString("FAILED autoUploadTimerExpired1Result=%d expected=%d testIndex=%d in %s(%d)",
								autoUploadTimerExpired1Result,
								TEST_DATA[testIndex].m_autoUploadTimerExpired1Expected,
								testIndex,
								(__TFUNCTION__), __LINE__);
							ASSERT_TRUE(areEqual) <<  failedMsg;
						}
						break;

					case 2:
						// Test #D1 -- destructive timer test, so do this last.
						{
							const bool autoUploadTimerExpired2Result = IsAutoUploadTimerExpired(varPb, TEST_DATA[testIndex].m_tickCountAutoUploadTimerExpired2);

							const bool areEqual = autoUploadTimerExpired2Result == TEST_DATA[testIndex].m_autoUploadTimerExpired2Expected;
							string failedMsg;
							failedMsg = FormatString("FAILED autoUploadTimerExpired2Result=%d expected=%d testIndex=%d in %s(%d)",
								autoUploadTimerExpired2Result,
								TEST_DATA[testIndex].m_autoUploadTimerExpired2Expected,
								testIndex,
								(__TFUNCTION__), __LINE__);
							ASSERT_TRUE(areEqual) <<  failedMsg;
						}
						break;

					default:
						break;
					}

					// Cleanup.
				}
			}
		}

		TEST(PersonalBanker5, LimitSessionInProgressToUIExpiredActionTest)
		{
			CPersonalBanker *pb = new CPersonalBanker(MEMORY_NONE);
			DWORD current = CUtilities::GetTickCount();
			pb->SetLastCardedSessionInProgressToCurrent(current - CPersonalBanker::TELL_UI_OF_FAILED_PB_SLEEP_MILLISECONDS - 10);

			ASSERT_TRUE( pb->LimitSessionInProgressToUIExpiredAction() == PBActionSendCardSessionInProgressToUI);

			pb->SetLastCardedSessionInProgressToCurrent();
			ASSERT_TRUE(pb->LimitSessionInProgressToUIExpiredAction() == PBActionNone);

			delete pb;
		}

		TEST(PersonalBanker5, TestPlayerIdWaitingPinReset)
		{
			CPersonalBanker *pb = new CPersonalBanker(MEMORY_NONE);
			DWORD oldPlayerId = 1234;
			DWORD currentPlayerId = oldPlayerId;
			
			pb->SetPlayerIdWaitingPinReset(oldPlayerId);
			ASSERT_TRUE(pb->IsPlayerIdWaitingPinReset(currentPlayerId));

			currentPlayerId = 2324;
			ASSERT_FALSE(pb->IsPlayerIdWaitingPinReset(currentPlayerId));

			delete pb;
		}

		
		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::BeginPersonalBankerSession()
		///     CPersonalBanker::GetCurrnetPin()
		/// </summary>
		TEST(PersonalBanker5, UpdateCurrentPinTest)
		{
			// Test data.
			LPCTSTR pbPin = _T("1234");
			int pin = std::stoi(pbPin);
			
			// Setup.
			CPersonalBanker varPersonalBanker;
			CCard *card = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime());
			CPlayerInfo *playerInfo = new CPlayerInfo();
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
            CGames games;
			CPlayer player(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);

			// Perform operations to be tested.
			varPersonalBanker.BeginPersonalBankerSession(&player,
				pbPin,
				config);
			const CPersonalBanker &personalBanker(varPersonalBanker);
			ASSERT_EQ(pin, varPersonalBanker.GetCurrentPin());
		}

		/// <summary>
		/// Test the implementation of:
		///     CPersonalBanker::SetNewPin()
		/// </summary>
		TEST(PersonalBanker5, UpdateNewPinTest)
		{
			// Test data.
			DWORD pbPin = 3334;

			// Setup.
			CPersonalBanker varPersonalBanker;

			const CPersonalBanker &personalBanker(varPersonalBanker);
			varPersonalBanker.SetNewPin(pbPin);
			ASSERT_EQ(pbPin, varPersonalBanker.GetNewPin());
		}

		TEST(PersonalBanker5, GetSubscriptionDataResponse_PBInfoTest_AccessDenied)
		{
			// Test data - inputs.
			const int pbtIDLength = 1;
			const char pbtID[pbtIDLength] = { '1' };
			const CConfig config(false, "");
			const bool gameOnline(false);
			const bool bIsEFT(false);

			// Test data - expected results.
			string expectedResponse;
			expectedResponse = R"({"Successful":false,)";
			expectedResponse += R"("Error":"accessDenied"})";

			// Setup.
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
			pbInfo->SetErrorCode(PBError::AccessDenied);
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);

			// Perform operations to be tested.
			std::string response = varPb.GetSubscriptionDataResponse("PBInfo", "");
			ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
		}

		TEST(PersonalBanker5, GetSubscriptionDataResponse_PBInfoTest_AccessError)
		{
			// Test data - inputs.
			const int pbtIDLength = 1;
			const char pbtID[pbtIDLength] = { '1' };
			const CConfig config(false, "");
			const bool gameOnline(false);
			const bool bIsEFT(false);

			// Test data - expected results.
			string expectedResponse;
			expectedResponse = R"({"Successful":false,)";
			expectedResponse += R"("Error":"accessError"})";

			// Setup.
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
			pbInfo->SetErrorCode(PBError::AccessError);
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);

			// Perform operations to be tested.
			std::string response = varPb.GetSubscriptionDataResponse("PBInfo", "");
			ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
		}

		TEST(PersonalBanker5, GetSubscriptionDataResponse_PBInfoTest_AccountInUse)
		{
			// Test data - inputs.
			const int pbtIDLength = 1;
			const char pbtID[pbtIDLength] = { '1' };
			const CConfig config(false, "");
			const bool gameOnline(false);
			const bool bIsEFT(false);

			// Test data - expected results.
			string expectedResponse;
			expectedResponse = R"({"Successful":false,)";
			expectedResponse += R"("Error":"accountInUse"})";

			// Setup.
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
			pbInfo->SetErrorCode(PBError::AccountInUse);
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);

			// Perform operations to be tested.
			std::string response = varPb.GetSubscriptionDataResponse("PBInfo", "");
			ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
		}

		TEST(PersonalBanker5, GetSubscriptionDataResponse_PBInfoTest_AccountLocked)
		{
			// Test data - inputs.
			const int pbtIDLength = 1;
			const char pbtID[pbtIDLength] = { '1' };
			const CConfig config(false, "");
			const bool gameOnline(false);
			const bool bIsEFT(false);

			// Test data - expected results.
			string expectedResponse;
			expectedResponse = R"({"Successful":false,)";
			expectedResponse += R"("Error":"accountLocked"})";

			// Setup.
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
			pbInfo->SetErrorCode(PBError::AccountLocked);
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);

			// Perform operations to be tested.
			std::string response = varPb.GetSubscriptionDataResponse("PBInfo", "");
			ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
		}

		TEST(PersonalBanker5, GetSubscriptionDataResponse_PBInfoTest_AcctHasNoPin)
		{
			// Test data - inputs.
			const int pbtIDLength = 1;
			const char pbtID[pbtIDLength] = { '1' };
			const CConfig config(false, "");
			const bool gameOnline(false);
			const bool bIsEFT(false);

			// Test data - expected results.
			string expectedResponse;
			expectedResponse = R"({"Successful":false,)";
			expectedResponse += R"("Error":"acctHasNoPin"})";

			// Setup.
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
			pbInfo->SetErrorCode(PBError::AcctHasNoPin);
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);

			// Perform operations to be tested.
			std::string response = varPb.GetSubscriptionDataResponse("PBInfo", "");
			ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
		}

		TEST(PersonalBanker5, GetSubscriptionDataResponse_PBInfoTest_BannedPinEntered)
		{
			// Test data - inputs.
			const int pbtIDLength = 1;
			const char pbtID[pbtIDLength] = { '1' };
			const CConfig config(false, "");
			const bool gameOnline(false);
			const bool bIsEFT(false);

			// Test data - expected results.
			string expectedResponse;
			expectedResponse = R"({"Successful":false,)";
			expectedResponse += R"("Error":"bannedPinEntered"})";

			// Setup.
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
			pbInfo->SetErrorCode(PBError::BannedPinEntered);
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);

			// Perform operations to be tested.
			std::string response = varPb.GetSubscriptionDataResponse("PBInfo", "");
			ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
		}

		TEST(PersonalBanker5, GetSubscriptionDataResponse_PBInfoTest_CreditTransferCommserror)
		{
			// Test data - inputs.
			const int pbtIDLength = 1;
			const char pbtID[pbtIDLength] = { '1' };
			const CConfig config(false, "");
			const bool gameOnline(false);
			const bool bIsEFT(false);

			// Test data - expected results.
			string expectedResponse;
			expectedResponse = R"({"Successful":false,)";
			expectedResponse += R"("Error":"creditTransferCommserror"})";

			// Setup.
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
			pbInfo->SetErrorCode(PBError::CreditTransferCommserror);
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);

			// Perform operations to be tested.
			std::string response = varPb.GetSubscriptionDataResponse("PBInfo", "");
			ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
		}

		TEST(PersonalBanker5, GetSubscriptionDataResponse_PBInfoTest_CreditTransferInsufficientCredits)
		{
			// Test data - inputs.
			const int pbtIDLength = 1;
			const char pbtID[pbtIDLength] = { '1' };
			const CConfig config(false, "");
			const bool gameOnline(false);
			const bool bIsEFT(false);

			// Test data - expected results.
			string expectedResponse;
			expectedResponse = R"({"Successful":false,)";
			expectedResponse += R"("Error":"creditTransferInsufficientCredits"})";

			// Setup.
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
			pbInfo->SetErrorCode(PBError::CreditTransferInsufficientCredits);
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);

			// Perform operations to be tested.
			std::string response = varPb.GetSubscriptionDataResponse("PBInfo", "");
			ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
		}

		TEST(PersonalBanker5, GetSubscriptionDataResponse_PBInfoTest_CreditTransferOtherIssue)
		{
			// Test data - inputs.
			const int pbtIDLength = 1;
			const char pbtID[pbtIDLength] = { '1' };
			const CConfig config(false, "");
			const bool gameOnline(false);
			const bool bIsEFT(false);

			// Test data - expected results.
			string expectedResponse;
			expectedResponse = R"({"Successful":false,)";
			expectedResponse += R"("Error":"creditTransferOtherIssue"})";

			// Setup.
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
			pbInfo->SetErrorCode(PBError::CreditTransferOtherIssue);
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);

			// Perform operations to be tested.
			std::string response = varPb.GetSubscriptionDataResponse("PBInfo", "");
			ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
		}

		TEST(PersonalBanker5, GetSubscriptionDataResponse_PBInfoTest_InvalidID)
		{
			// Test data - inputs.
			const int pbtIDLength = 1;
			const char pbtID[pbtIDLength] = { '1' };
			const CConfig config(false, "");
			const bool gameOnline(false);
			const bool bIsEFT(false);

			// Test data - expected results.
			string expectedResponse;
			expectedResponse = R"({"Successful":false,)";
			expectedResponse += R"("Error":"invalidID"})";

			// Setup.
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
			pbInfo->SetErrorCode(PBError::InvalidID);
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);

			// Perform operations to be tested.
			std::string response = varPb.GetSubscriptionDataResponse("PBInfo", "");
			ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
		}

		TEST(PersonalBanker5, GetSubscriptionDataResponse_PBInfoTest_NewPinRequired)
		{
			// Test data - inputs.
			const int pbtIDLength = 1;
			const char pbtID[pbtIDLength] = { '1' };
			const CConfig config(false, "");
			const bool gameOnline(false);
			const bool bIsEFT(false);

			// Test data - expected results.
			string expectedResponse;
			expectedResponse = R"({"Successful":false,)";
			expectedResponse += R"("Error":"newPinRequired"})";

			// Setup.
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
			pbInfo->SetErrorCode(PBError::NewPinRequired);
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);

			// Perform operations to be tested.
			std::string response = varPb.GetSubscriptionDataResponse("PBInfo", "");
			ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
		}

		TEST(PersonalBanker5, GetSubscriptionDataResponse_PBInfoTest1)
		{
			// Test data - inputs.
			const int pbtIDLength = 1;
			const char pbtID[pbtIDLength] = { '1' };
			const CConfig config(false, "");
			const bool gameOnline(false);
			const bool bIsEFT(false);

			// Test data - expected results.
			string expectedResponse;
			expectedResponse = R"({"Successful":true,)";
			expectedResponse += R"("Error":"",)";
			expectedResponse += R"("CashUnavailable":false,)";
			expectedResponse += R"("IsTransferToGameEnabled":false,)";
			expectedResponse += R"("IsTransferFromGameEnabled":false,)";
			expectedResponse += R"("ShowPoints":true,)";
			expectedResponse += R"("Points":0,)";
			expectedResponse += R"("PointsAsCash":0.0,)";
			expectedResponse += R"("Cash":0.0,)";
			expectedResponse += R"("Promo":0.0,)";
			expectedResponse += R"("CashOnGame":0.0,)";
			expectedResponse += R"("PromoOnGame":0.0,)";
			expectedResponse += R"("IsTransferCashFromGame":false,)";
			expectedResponse += R"("IsTransferPromoFromGame":false,)";
			expectedResponse += R"("IsTransferCashToGame":false,)";
			expectedResponse += R"("IsTransferPromoToGame":false,)";
			expectedResponse += R"("IsTransferPointsToGame":false,)";
			expectedResponse += R"("IsCashInactive":false,)";
			expectedResponse += R"("IsPartialTransferCash":false,)";
			expectedResponse += R"("MinCashUpload":0,)";
			expectedResponse += R"("MaxCashUpload":0,)";
			expectedResponse += R"("MinCashDownload":0,)";
			expectedResponse += R"("MaxCashDownload":0,)";
			expectedResponse += R"("MinPromoDownload":0,)";
			expectedResponse += R"("MaxPromoDownload":0,)";
			expectedResponse += R"("MinPointsDownload":0,)";
			expectedResponse += R"("MaxPointsDownload":0,)";
			expectedResponse += R"("IsExternalCreditProviderEnabled":false,)";
			expectedResponse += R"("IsCreditFeeFixed":false,)";
			expectedResponse += R"("CreditFee":0})";

			// Setup.
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
			pbInfo->SetErrorCode(PBError::NoError);
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);
			varPb.ProcessBalancesFromEGM(0, 0, 0, false, false, config);

			// Perform operations to be tested.
			std::string response = varPb.GetSubscriptionDataResponse("PBInfo", "");
			ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
		}

		TEST(PersonalBanker5, GetSubscriptionDataResponse_PBInfoTest2)
		{
			// Test data - inputs.
			const int pbtIDLength = 1;
			const char pbtID[pbtIDLength] = { '1' };
			const CConfig config(false, "");
			const bool gameOnline(false);
			const bool bIsEFT(false);

			// Test data - expected results.
			string expectedResponse;
			expectedResponse = R"({"Successful":true,)";
			expectedResponse += R"("Error":"",)";
			expectedResponse += R"("CashUnavailable":false,)";
			expectedResponse += R"("IsTransferToGameEnabled":false,)";
			expectedResponse += R"("IsTransferFromGameEnabled":false,)";
			expectedResponse += R"("ShowPoints":true,)";
			expectedResponse += R"("Points":10,)";
			expectedResponse += R"("PointsAsCash":10.0,)";
			expectedResponse += R"("Cash":200.0,)";
			expectedResponse += R"("Promo":300.0,)";
			expectedResponse += R"("CashOnGame":2.0,)";
			expectedResponse += R"("PromoOnGame":3.0,)";
			expectedResponse += R"("IsTransferCashFromGame":false,)";
			expectedResponse += R"("IsTransferPromoFromGame":false,)";
			expectedResponse += R"("IsTransferCashToGame":false,)";
			expectedResponse += R"("IsTransferPromoToGame":false,)";
			expectedResponse += R"("IsTransferPointsToGame":false,)";
			expectedResponse += R"("IsCashInactive":false,)";
			expectedResponse += R"("IsPartialTransferCash":false,)";
			expectedResponse += R"("MinCashUpload":10,)";
			expectedResponse += R"("MaxCashUpload":1000,)";
			expectedResponse += R"("MinCashDownload":10,)";
			expectedResponse += R"("MaxCashDownload":200,)";
			expectedResponse += R"("MinPromoDownload":20,)";
			expectedResponse += R"("MaxPromoDownload":300,)";
			expectedResponse += R"("MinPointsDownload":1,)";
			expectedResponse += R"("MaxPointsDownload":10,)";
			expectedResponse += R"("IsExternalCreditProviderEnabled":false,)";
			expectedResponse += R"("IsCreditFeeFixed":false,)";
			expectedResponse += R"("CreditFee":0})";

			// Setup.
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
			pbInfo->SetErrorCode(PBError::NoError);
			pbInfo->SetPointsAvailable(10);
			pbInfo->SetPointsPerDollar(1);
			pbInfo->SetCashBalance(20000);
			pbInfo->SetPromo2Balance(30000);
			pbInfo->SetMinimumCashUp(10);
			pbInfo->SetMaximumCashUp(1000);
			pbInfo->SetMinimumCashDown(10);
			pbInfo->SetMaximumCashDown(200);
			pbInfo->SetMinimumPromoDown(20);
			pbInfo->SetMaximumPromoDown(300);
			pbInfo->SetMinimumPoints(1);
			pbInfo->SetMaximumPoints(10);
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);
			varPb.ProcessBalancesFromEGM(200, 0, 300, false, false, config);

			// Perform operations to be tested.
			std::string response = varPb.GetSubscriptionDataResponse("PBInfo", "");
			ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
		}

		TEST(PersonalBanker5, GetSubscriptionDataResponse_PBInfoTest3)
		{
			// Test data - inputs.
			const int pbtIDLength = 1;
			const char pbtID[pbtIDLength] = { '1' };
			CConfig config(false, "");
			config.SetShowPointsVsAccountBalance(true);
			const bool gameOnline(false);
			const bool bIsEFT(false);

			// Test data - expected results.
			string expectedResponse;
			expectedResponse = R"({"Successful":true,)";
			expectedResponse += R"("Error":"",)";
			expectedResponse += R"("CashUnavailable":false,)";
			expectedResponse += R"("IsTransferToGameEnabled":false,)";
			expectedResponse += R"("IsTransferFromGameEnabled":false,)";
			expectedResponse += R"("ShowPoints":false,)";
			expectedResponse += R"("Points":10,)";
			expectedResponse += R"("PointsAsCash":10.0,)";
			expectedResponse += R"("Cash":200.0,)";
			expectedResponse += R"("Promo":300.0,)";
			expectedResponse += R"("CashOnGame":0.0,)";
			expectedResponse += R"("PromoOnGame":0.0,)";
			expectedResponse += R"("IsTransferCashFromGame":false,)";
			expectedResponse += R"("IsTransferPromoFromGame":false,)";
			expectedResponse += R"("IsTransferCashToGame":false,)";
			expectedResponse += R"("IsTransferPromoToGame":false,)";
			expectedResponse += R"("IsTransferPointsToGame":false,)";
			expectedResponse += R"("IsCashInactive":false,)";
			expectedResponse += R"("IsPartialTransferCash":false,)";
			expectedResponse += R"("MinCashUpload":10,)";
			expectedResponse += R"("MaxCashUpload":1000,)";
			expectedResponse += R"("MinCashDownload":10,)";
			expectedResponse += R"("MaxCashDownload":200,)";
			expectedResponse += R"("MinPromoDownload":20,)";
			expectedResponse += R"("MaxPromoDownload":300,)";
			expectedResponse += R"("MinPointsDownload":1,)";
			expectedResponse += R"("MaxPointsDownload":10,)";
			expectedResponse += R"("IsExternalCreditProviderEnabled":false,)";
			expectedResponse += R"("IsCreditFeeFixed":false,)";
			expectedResponse += R"("CreditFee":0})";

			// Setup.
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetPersonalBankerID(pbtID, pbtIDLength);
			pbInfo->SetErrorCode(PBError::NoError);
			pbInfo->SetPointsAvailable(10);
			pbInfo->SetPointsPerDollar(1);
			pbInfo->SetCashBalance(20000);
			pbInfo->SetPromo2Balance(30000);
			pbInfo->SetMinimumCashUp(10);
			pbInfo->SetMaximumCashUp(1000);
			pbInfo->SetMinimumCashDown(10);
			pbInfo->SetMaximumCashDown(200);
			pbInfo->SetMinimumPromoDown(20);
			pbInfo->SetMaximumPromoDown(300);
			pbInfo->SetMinimumPoints(1);
			pbInfo->SetMaximumPoints(10);
			CPersonalBanker varPb;
			varPb.CreateTransfer(CUtilities::GetTickCount());
			varPb.ProcessPersonalBankerInfo(pbInfo, config, gameOnline, bIsEFT);
			varPb.ProcessBalancesFromEGM(0, 0, 0, false, false, config);

			// Perform operations to be tested.
			std::string response = varPb.GetSubscriptionDataResponse("PBInfo", "");
			ASSERT_EQ(response, expectedResponse) << "Incorrect response data";
		}
}
