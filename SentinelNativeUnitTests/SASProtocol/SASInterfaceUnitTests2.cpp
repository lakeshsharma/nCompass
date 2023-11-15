#if 000//TODO-PORT
#include "stdafx.h"
#include "SASProtocol/SASInterface.h"
#include "SASProtocol/LongPoll38Message.h"
#include "SASProtocol/LongPollUMessage.h"
#include "SASProtocol/SASSerialWrapperMock.h"

// Unit testing includes.
#include "TestUtil/UnitTestingMatchers.h"
#include "UnitTestingToString.h"
#include "UnitTestingUtils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using ::testing::_;
using ::testing::SetArrayArgument;
using ::testing::InSequence;
using ::testing::SetArgReferee;
using ::testing::Ne;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(SASInterfaceUnitTests2)
	{
		// Class member(s) used by test (data) methods.
		static const byte GAMEADDRESS = 1;

		static CString m_gmockFailure;

		class CTersePrinter : public ::testing::EmptyTestEventListener {
			// Called after a failed assertion or a SUCCEED() invocation.
			virtual void OnTestPartResult(const ::testing::TestPartResult& test_part_result) {
				if (test_part_result.failed())
				{
					CStringA failure;
					failure.Format("FAILED in %s:%d\n%s",
						test_part_result.file_name(),
						test_part_result.line_number(),
						test_part_result.summary());
					m_gmockFailure = failure;
				}
			}
		};

		static CTersePrinter m_tersePrinter;

		/// <summary>
		/// Initialize and/or test static class members.
		/// </summary>
		TEST_CLASS_INITIALIZE(ClassInitialize)
		{
			// Class initialization code.
			::testing::UnitTest::GetInstance()->listeners().Append(&m_tersePrinter);
		}

		/// <summary>
		/// Cleanup static class member(s).
		/// </summary>
		TEST_CLASS_CLEANUP(ClassCleanup)
		{
			// Class cleanup code.
			::testing::UnitTest::GetInstance()->listeners().Release(&m_tersePrinter);
		}

		/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		TEST_METHOD_INITIALIZE(Initialize)
		{
			// Test method initialization code.
			m_gmockFailure.Empty();
		}

		/// <summary>
		/// Cleanup class member(s) before each test method is run.
		/// </summary>
		TEST_METHOD_CLEANUP(Cleanup)
		{
			// Report a gmock failure reported by our event listener, if one occurred.
			if (!m_gmockFailure.IsEmpty())
			{
				Assert::IsTrue(false, m_gmockFailure);
			}

			// Test method cleanup code.
		}

		void Init(CSASConsumer *&sasProtocol, CQueueList *&gcfToInternalsQueue, CQueueList *&internalsToGCFQueue)
		{
			sasProtocol = new CSASConsumer();
			gcfToInternalsQueue = new CQueueList(false);
			internalsToGCFQueue = new CQueueList(false);
		}

		void Cleanup(CSASConsumer *sasProtocol, CQueueList *gcfToInternalsQueue, CQueueList *internalsToGCFQueue)
		{
			delete gcfToInternalsQueue;
			delete internalsToGCFQueue;
			delete sasProtocol;

		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when the fixed length response contains a good response.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Good)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Good response.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual((__int64)0, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Try to get a GetComStatus() result of true,
		/// when a fixed length poll gets a bad response
		/// followed a  fixed length poll that gets a good response.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Bad_Good)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.
			const byte fakeData[2] = { 1, 0x1a }; // Expect "fake" 0x1A poll.

			byte receiveData1[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			byte *receiveData3(nullptr); // No response.
			DWORD receiveData3Len(0);

			byte receiveData4[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Good response.
			CUnitTestingUtils::SetSasCrc(receiveData4, _countof(receiveData4));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(fakeData, sizeof(fakeData)), sizeof(fakeData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData4, (byte *)(receiveData4 + _countof(receiveData4))),
				SetArgReferee<2>(_countof(receiveData4))));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			// First, get a bad response, to get GetConsecutiveCommErrorCount() > 0.
			{

				CMeters *meters = new CMeters(MEMORY_NONE);
				meters->SetMeter(mD500);
				CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
				lp->Fill();
				sas.SendToSAS(lp, 0);
				lp->ResponseReceived();

				Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
				Assert::AreEqual(0, lp->GetRetryCount());
				Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
				Assert::AreEqual(false, sas.GameComStatus());
				Assert::AreEqual(false, sas.HasNoErrors());
				Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

				delete meters;
				delete lp;
			}

			// Next, get a good response, so GameComStatus() returns true.
			{
				CMeters *meters = new CMeters(MEMORY_NONE);
				meters->SetMeter(mD500);
				CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
				lp->Fill();
				sas.SendToSAS(lp, 0);
				lp->ResponseReceived();

				Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
				Assert::AreEqual(0, lp->GetRetryCount());
				Assert::AreEqual((__int64)0, meters->GetMeterByIndex(0)->GetValue());
				Assert::AreEqual(true, sas.GameComStatus());
				Assert::AreEqual(true, sas.HasNoErrors());
				Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

				delete meters;
				delete lp;
			}

			Cleanup(cons, gcf, in);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Try to get a GetComStatus() result of true,
		/// when a fixed length poll gets a bad response
		/// followed a  fixed length poll that gets a good response.
		/// Then try to GetComStatus() result of false by getting enough bad responses.
		/// 2015-06-21 Note: The number of sync polls after a failed poll depends on GameCommStatus().
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Bad_Good_Down)
		{
			// Expect SASGAMECOMDOWNERRORCOUNT == 30.
			const int maxConsecutiveErrorCountForCommUp(30);
			const int consecutiveBadPollsForComDown(15); // 3 consecutive bad response per poll. Plus extra.

			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.
			const byte fakeData[2] = { 1, 0x1a }; // Expect "fake" 0x1A poll.

			byte receiveData1[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			byte *receiveData3(nullptr); // No response.
			DWORD receiveData3Len(0);

			byte receiveData4[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Good response.
			CUnitTestingUtils::SetSasCrc(receiveData4, _countof(receiveData4));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			// First poll, bad responses.
			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(fakeData, sizeof(fakeData)), sizeof(fakeData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			// Second poll, good resonse.
			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData4, (byte *)(receiveData4 + _countof(receiveData4))),
				SetArgReferee<2>(_countof(receiveData4))));

			// More polls, with consecutive bad responses.
			for (int count(0); count < consecutiveBadPollsForComDown; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(fakeData, sizeof(fakeData)), sizeof(fakeData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
					SetArgReferee<2>(receiveData3Len)));

				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
					SetArgReferee<2>(_countof(receiveData1))));

				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
					SetArgReferee<2>(_countof(receiveData2))));

				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

				if ((count + 1) * 3 <= maxConsecutiveErrorCountForCommUp)
				{
					EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(1);
				}
				else
				{
					EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);
				}
			}

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			// First, get a bad response, to get GetConsecutiveCommErrorCount() > 0.
			{

				CMeters *meters = new CMeters(MEMORY_NONE);
				meters->SetMeter(mD500);
				CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
				lp->Fill();
				sas.SendToSAS(lp, 0);
				lp->ResponseReceived();

				Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
				Assert::AreEqual(0, lp->GetRetryCount());
				Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
				Assert::AreEqual(false, sas.GameComStatus());
				Assert::AreEqual(false, sas.HasNoErrors());
				Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

				delete meters;
				delete lp;
			}

			// Next, get a good response, so GameComStatus() returns true.
			{
				CMeters *meters = new CMeters(MEMORY_NONE);
				meters->SetMeter(mD500);
				CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
				lp->Fill();
				sas.SendToSAS(lp, 0);
				lp->ResponseReceived();

				Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
				Assert::AreEqual(0, lp->GetRetryCount());
				Assert::AreEqual((__int64)0, meters->GetMeterByIndex(0)->GetValue());
				Assert::AreEqual(true, sas.GameComStatus());
				Assert::AreEqual(true, sas.HasNoErrors());
				Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

				delete meters;
				delete lp;
			}

			// Now more polls with consecutive bad responses, to get GetConsecutiveCommErrorCount() > 30.
			for (int count(0); count < consecutiveBadPollsForComDown; ++count)
			{
				CMeters *meters = new CMeters(MEMORY_NONE);
				meters->SetMeter(mD500);
				CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
				lp->Fill();
				sas.SendToSAS(lp, 0);
				lp->ResponseReceived();

				Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
				Assert::AreEqual(0, lp->GetRetryCount());
				Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
				Assert::AreEqual(sas.GetConsecutiveCommErrorCount() <= maxConsecutiveErrorCountForCommUp, sas.GameComStatus());
				Assert::AreEqual(false, sas.HasNoErrors());
				Assert::AreEqual((WORD)(3 * (count + 1)), sas.GetConsecutiveCommErrorCount());

				delete meters;
				delete lp;
			}

			Assert::AreEqual(false, sas.GameComStatus()); // Verify that we had enough consecutive bad responses to go comm down.

			Cleanup(cons, gcf, in);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad poll value followed by a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadPollValue_Good)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Good response.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual((__int64)0, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad poll value followed by a bad poll value.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadPollValue_BadPollValue)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad poll value followed by a bad CRC.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadPollValue_BadCrc)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));
			++receiveData2[_countof(receiveData2) - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad poll value followed by a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadPollValue_Short)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[7] = { 1, 0x38, 0, 0, 0, 0, 0 }; // Short length.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad poll value followed by a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadPollValue_FullLenNack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[8] = { 0x81, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad poll value followed by a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadPollValue_LongNack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad poll value followed by a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadPollValue_Nack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[1] = { 0x81 }; // Normal NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad poll value followed by no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadPollValue_NoResponse)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte *receiveData2(nullptr); // No response.
			DWORD receiveData2Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)4, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad CRC followed by a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadCrc_Good)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));
			++receiveData1[_countof(receiveData1) - 1];

			byte receiveData2[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Good response.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual((__int64)0, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad CRC followed by a bad poll value.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadCrc_BadPollValue)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));
			++receiveData1[_countof(receiveData1) - 1];

			byte receiveData2[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad CRC followed by a bad CRC.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadCrc_BadCrc)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));
			++receiveData1[_countof(receiveData1) - 1];

			byte receiveData2[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));
			++receiveData2[_countof(receiveData2) - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad CRC followed by a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadCrc_Short)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));
			++receiveData1[_countof(receiveData1) - 1];

			byte receiveData2[7] = { 1, 0x38, 0, 0, 0, 0, 0 }; // Short length.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad CRC followed by a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadCrc_FullLenNack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));
			++receiveData1[_countof(receiveData1) - 1];

			byte receiveData2[8] = { 0x81, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad CRC followed by a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadCrc_LongNack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));
			++receiveData1[_countof(receiveData1) - 1];

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad CRC followed by a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadCrc_Nack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));
			++receiveData1[_countof(receiveData1) - 1];

			byte receiveData2[1] = { 0x81 }; // Normal NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a bad CRC followed by no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_BadCrc_NoResponse)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));
			++receiveData1[_countof(receiveData1) - 1];

			byte *receiveData2(nullptr); // No response.
			DWORD receiveData2Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)4, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a short response followed by a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Short_Good)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[7] = { 1, 0x00, 0, 0, 0, 0, 0 }; // Short length.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Good response.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)0, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a short response followed by a bad poll value.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Short_BadPollValue)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[7] = { 1, 0x00, 0, 0, 0, 0, 0 }; // Short length.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a short reponse followed by a bad CRC.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Short_BadCrc)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[7] = { 1, 0x00, 0, 0, 0, 0, 0 }; // Short length.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));
			++receiveData2[_countof(receiveData2) - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a short reponse followed by a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Short_Short)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[7] = { 1, 0x00, 0, 0, 0, 0, 0 }; // Short length.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[7] = { 1, 0x38, 0, 0, 0, 0, 0 }; // Short length.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a short reponse followed by a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Short_FullLenNack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[7] = { 1, 0x00, 0, 0, 0, 0, 0 }; // Short length.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[8] = { 0x81, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a short reponse followed by a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Short_LongNack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[7] = { 1, 0x00, 0, 0, 0, 0, 0 }; // Short length.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a short reponse followed by a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Short_Nack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[7] = { 1, 0x00, 0, 0, 0, 0, 0 }; // Short length.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte receiveData2[1] = { 0x81 }; // Normal NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a short reponse followed by no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Short_NoResponse)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[7] = { 1, 0x00, 0, 0, 0, 0, 0 }; // Short length.
			CUnitTestingUtils::SetSasCrc(receiveData1, _countof(receiveData1));

			byte *receiveData2(nullptr); // No response.
			DWORD receiveData2Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)4, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a full-length NACK followed by a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_FullLenNack_Good)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 0x81, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.

			byte receiveData2[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Good response.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)0, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a full-length NACK followed by a bad poll value.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_FullLenNack_BadPollValue)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 0x81, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.

			byte receiveData2[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a full-length NACK followed by a bad CRC.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_FullLenNack_BadCrc)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 0x81, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.

			byte receiveData2[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));
			++receiveData2[_countof(receiveData2) - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a full-length NACK followed by a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_FullLenNack_Short)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 0x81, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.

			byte receiveData2[7] = { 1, 0x38, 0, 0, 0, 0, 0 }; // Short length.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a full-length NACK followed by a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_FullLenNack_FullLenNack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 0x81, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.

			byte receiveData2[8] = { 0x81, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a full-length NACK followed by a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_FullLenNack_LongNack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 0x81, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a full-length NACK followed by a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_FullLenNack_Nack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 0x81, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.

			byte receiveData2[1] = { 0x81 }; // Normal NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a full-length NACK followed by no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_FullLenNack_NoResponse)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[8] = { 0x81, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.

			byte *receiveData2(nullptr); // No response.
			DWORD receiveData2Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)4, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a longer than normal NACK followed by a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_LongNack_Good)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.

			byte receiveData2[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Good response.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)0, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a longer than normal NACK followed by a bad poll value.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_LongNack_BadPollValue)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.

			byte receiveData2[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a longer than normal NACK followed by a bad CRC.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_LongNack_BadCrc)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.

			byte receiveData2[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));
			++receiveData2[_countof(receiveData2) - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a longer than normal NACK followed by a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_LongNack_Short)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.

			byte receiveData2[7] = { 1, 0x38, 0, 0, 0, 0, 0 }; // Short length.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a longer than normal NACK followed by a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_LongNack_FullLenNack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.

			byte receiveData2[8] = { 0x81, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a longer than normal NACK followed by a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_LongNack_LongNack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a longer than normal NACK followed by a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_LongNack_Nack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.

			byte receiveData2[1] = { 0x81 }; // Normal NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a longer than normal NACK followed by no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_LongNack_NoResponse)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.

			byte *receiveData2(nullptr); // No response.
			DWORD receiveData2Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)4, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a normal NACK followed by a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Nack_Good)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[1] = { 0x81 }; // Normal NACK.

			byte receiveData2[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Good response.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)0, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a normal NACK followed by a bad poll value.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Nack_BadPollValue)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[1] = { 0x81 }; // Normal NACK.

			byte receiveData2[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a normal NACK followed by a bad CRC.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Nack_BadCrc)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[1] = { 0x81 }; // Normal NACK.

			byte receiveData2[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));
			++receiveData2[_countof(receiveData2) - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a normal NACK followed by a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Nack_Short)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[1] = { 0x81 }; // Normal NACK.

			byte receiveData2[7] = { 1, 0x38, 0, 0, 0, 0, 0 }; // Short length.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a normal NACK followed by a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Nack_FullLenNack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[1] = { 0x81 }; // Normal NACK.

			byte receiveData2[8] = { 0x81, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a normal NACK followed by a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Nack_LongNack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[1] = { 0x81 }; // Normal NACK.

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a normal NACK followed by a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Nack_Nack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[1] = { 0x81 }; // Normal NACK.

			byte receiveData2[1] = { 0x81 }; // Normal NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain a normal NACK followed by no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_Nack_NoResponse)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[1] = { 0x81 }; // Normal NACK.

			byte *receiveData2(nullptr); // No response.
			DWORD receiveData2Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)4, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain no response followed by a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_NoResponse_Good)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[1] = { 0x81 }; // Normal NACK.

			byte receiveData2[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Good response.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)0, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain no response followed by a bad poll value.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_NoResponse_BadPollValue)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[1] = { 0x81 }; // Normal NACK.

			byte receiveData2[8] = { 1, 0x00, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain no response followed by a bad CRC.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_NoResponse_BadCrc)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[1] = { 0x81 }; // Normal NACK.

			byte receiveData2[8] = { 1, 0x38, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));
			++receiveData2[_countof(receiveData2) - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), _countof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain no response followed by a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_NoResponse_Short)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[1] = { 0x81 }; // Normal NACK.

			byte receiveData2[7] = { 1, 0x38, 0, 0, 0, 0, 0 }; // Short length.
			CUnitTestingUtils::SetSasCrc(receiveData2, _countof(receiveData2));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain no response followed by a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_NoResponse_FullLenNack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[1] = { 0x81 }; // Normal NACK.

			byte receiveData2[8] = { 0x81, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain no response followed by a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_NoResponse_LongNack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[1] = { 0x81 }; // Normal NACK.

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain no response followed by a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_NoResponse_Nack)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[1] = { 0x81 }; // Normal NACK.

			byte receiveData2[1] = { 0x81 }; // Normal NACK.

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + _countof(receiveData1))),
				SetArgReferee<2>(_countof(receiveData1))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + _countof(receiveData2))),
				SetArgReferee<2>(_countof(receiveData2))));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)3, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test when fixed length responses contain no response followed by no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASFixedLen_NoResponse_NoResponse)
		{
			const DWORD expectedReadLen(8);
			const unsigned char pollData[2] = { 1, 0x38 }; // Expect long poll 38.
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte *receiveData2(nullptr); // No response.
			DWORD receiveData2Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
				SetArgReferee<2>(receiveData1Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(4);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual((__int64)INVALID_METER_VALUE, meters->GetMeterByIndex(0)->GetValue());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)5, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete meters;
			delete lp;
		}

// #pragma region CSASInterface::SendToSAS_Upload
		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when the first fixed length response contains a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_Good)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));
			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte receiveData1[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Good.
			DWORD receiveData1Len(_countof(receiveData1));
			CUnitTestingUtils::SetSasCrc(receiveData1, receiveData1Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
				SetArgReferee<2>(receiveData1Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain no responses.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse9x)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 9; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(9);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)9, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}
// #pragma endregion CSASInterface::SendToSAS_Upload

// #pragma region CSASInterface::SendToSAS_Upload_NoResponse8x
		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Good)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Good.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadPollValue_Good)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Good.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and a bad poll response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadPollValue_BadPollValue)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and a bad CRC response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadPollValue_BadCrc)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);
			++receiveData3[receiveData3Len - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadPollValue_Short)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadPollValue_FullLenNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadPollValue_LongNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadPollValue_Nack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadPollValue_NoResponse)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte *receiveData3(nullptr); // No response.
			DWORD receiveData3Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(10);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)10, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadCrc_Good)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Good.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and a bad poll response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadCrc_BadPollValue)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte receiveData3[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and a bad CRC response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadCrc_BadCrc)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);
			++receiveData3[receiveData3Len - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadCrc_Short)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte receiveData3[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadCrc_FullLenNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte receiveData3[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadCrc_LongNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte receiveData3[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadCrc_Nack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte receiveData3[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_BadCrc_NoResponse)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte *receiveData3(nullptr); // No response.
			DWORD receiveData3Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(10);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)10, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Short_Good)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Good.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and a bad poll response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Short_BadPollValue)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and a bad CRC response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Short_BadCrc)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);
			++receiveData3[receiveData3Len - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Short_Short)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Short_FullLenNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Short_LongNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Short_Nack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Short_NoResponse)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte *receiveData3(nullptr); // No response.
			DWORD receiveData3Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(10);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)10, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_FullLenNack_Good)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Good.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and a bad poll response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_FullLenNack_BadPollValue)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and a bad CRC response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_FullLenNack_BadCrc)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);
			++receiveData3[receiveData3Len - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_FullLenNack_Short)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_FullLenNack_FullLenNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_FullLenNack_LongNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_FullLenNack_Nack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_FullLenNack_NoResponse)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte *receiveData3(nullptr); // No response.
			DWORD receiveData3Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(10);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)10, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_LongNack_Good)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Good.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and a bad poll response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_LongNack_BadPollValue)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and a bad CRC response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_LongNack_BadCrc)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);
			++receiveData3[receiveData3Len - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_LongNack_Short)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_LongNack_FullLenNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_LongNack_LongNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_LongNack_Nack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_LongNack_NoResponse)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte *receiveData3(nullptr); // No response.
			DWORD receiveData3Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(10);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)10, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Nack_Good)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Good.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and a bad poll response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Nack_BadPollValue)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and a bad CRC response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Nack_BadCrc)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);
			++receiveData3[receiveData3Len - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Nack_Short)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Nack_FullLenNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Nack_LongNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Nack_Nack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(12);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)11, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse8x_Nack_NoResponse)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte *receiveData3(nullptr); // No response.
			DWORD receiveData3Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 8; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(10);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)10, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}
// #pragma endregion CSASInterface::SendToSAS_Upload_NoResponse8x

// #pragma region CSASInterface::SendToSAS_Upload_NoResponse4x
		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Good)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Good.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadPollValue_Good)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Good.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and a bad poll response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadPollValue_BadPollValue)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and a bad CRC response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadPollValue_BadCrc)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);
			++receiveData3[receiveData3Len - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadPollValue_Short)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadPollValue_FullLenNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadPollValue_LongNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadPollValue_Nack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad poll value response and no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadPollValue_NoResponse)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte *receiveData3(nullptr); // No response.
			DWORD receiveData3Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
					SetArgReferee<2>(receiveData3Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(9);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)9, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadCrc_Good)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Good.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and a bad poll response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadCrc_BadPollValue)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte receiveData3[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and a bad CRC response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadCrc_BadCrc)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);
			++receiveData3[receiveData3Len - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadCrc_Short)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte receiveData3[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadCrc_FullLenNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte receiveData3[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadCrc_LongNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte receiveData3[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadCrc_Nack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte receiveData3[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a bad CRC response and no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_BadCrc_NoResponse)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);
			++receiveData2[receiveData2Len - 1];

			byte *receiveData3(nullptr); // No response.
			DWORD receiveData3Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
					SetArgReferee<2>(receiveData3Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(9);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(0, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)9, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Short_Good)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Good.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and a bad poll response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Short_BadPollValue)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and a bad CRC response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Short_BadCrc)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);
			++receiveData3[receiveData3Len - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Short_Short)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Short_FullLenNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Short_LongNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Short_Nack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte receiveData3[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a short response and no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Short_NoResponse)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData2Len(_countof(receiveData2));
			CUnitTestingUtils::SetSasCrc(receiveData2, receiveData2Len);

			byte *receiveData3(nullptr); // No response.
			DWORD receiveData3Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
					SetArgReferee<2>(receiveData3Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(9);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)9, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_FullLenNack_Good)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Good.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and a bad poll response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_FullLenNack_BadPollValue)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and a bad CRC response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_FullLenNack_BadCrc)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);
			++receiveData3[receiveData3Len - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_FullLenNack_Short)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_FullLenNack_FullLenNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_FullLenNack_LongNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_FullLenNack_Nack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a full-length NACK and no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_FullLenNack_NoResponse)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte *receiveData3(nullptr); // No response.
			DWORD receiveData3Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
					SetArgReferee<2>(receiveData3Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(9);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)9, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_LongNack_Good)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Good.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and a bad poll response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_LongNack_BadPollValue)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and a bad CRC response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_LongNack_BadCrc)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);
			++receiveData3[receiveData3Len - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_LongNack_Short)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_LongNack_FullLenNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_LongNack_LongNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_LongNack_Nack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a longer than normal NACK and no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_LongNack_NoResponse)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte *receiveData3(nullptr); // No response.
			DWORD receiveData3Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
					SetArgReferee<2>(receiveData3Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(9);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)9, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and a good response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Nack_Good)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Good.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_SUCCESS, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and a bad poll response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Nack_BadPollValue)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect poll value.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and a bad CRC response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Nack_BadCrc)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, incorrect CRC.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);
			++receiveData3[receiveData3Len - 1];

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and a short response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Nack_Short)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen - 1] = { 1, EFTCREDITSUP, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Short length.
			DWORD receiveData3Len(_countof(receiveData3));
			CUnitTestingUtils::SetSasCrc(receiveData3, receiveData3Len);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and a full-length NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Nack_FullLenNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[expectedReadLen] = { 0x81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Correct length, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and a longer than normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Nack_LongNack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[2] = { 0x81, 0 }; // Longer than a normal NACK, first byte is a NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and a normal NACK.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Nack_Nack)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte receiveData3[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData3Len(_countof(receiveData3));

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
				SetArgReferee<2>(receiveData3Len)));

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(8);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(2, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)7, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASInterface::SendToSAS()
		/// Test an upload poll when fixed length responses contain 8 no responses followed by a normal NACK and no response.
		/// This test may need to be updated when CSASInterface::SendToSAS() is improved.
		/// </summary>
		TEST_METHOD(SendToSASUpload_NoResponse4x_Nack_NoResponse)
		{
			const byte eftNumber(1);
			const byte EFTACKPHASE1 = 0;
			const byte EFTCREDITSUP = 0X64;
			const DWORD expectedReadLen(12);
			unsigned char pollData[6] = { 1, EFTCREDITSUP, 1, EFTACKPHASE1, 0, 0 }; // Expect long poll U.
			CUnitTestingUtils::SetSasCrc(pollData, _countof(pollData));

			const byte syncData[1] = { 0x8c }; // Expect general poll to address 12.

			byte *receiveData1(nullptr); // No response.
			DWORD receiveData1Len(0);

			byte receiveData2[1] = { 0x81 }; // Normal NACK.
			DWORD receiveData2Len(_countof(receiveData2));

			byte *receiveData3(nullptr); // No response.
			DWORD receiveData3Len(0);

			CSASSerialWrapperMock *mockSerial(new CSASSerialWrapperMock());
			InSequence s;

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData1, (byte *)(receiveData1 + receiveData1Len)),
					SetArgReferee<2>(receiveData1Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
			EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
				.WillRepeatedly(DoAll(
				SetArrayArgument<0>((byte *)receiveData2, (byte *)(receiveData2 + receiveData2Len)),
				SetArgReferee<2>(receiveData2Len)));

			for (int count(0); count < 4; ++count)
			{
				EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(pollData, sizeof(pollData)), sizeof(pollData), true)).Times(1);
				EXPECT_CALL(*mockSerial, ReadSAS(Ne(nullptr), expectedReadLen, _)).Times(1)
					.WillRepeatedly(DoAll(
					SetArrayArgument<0>((byte *)receiveData3, (byte *)(receiveData3 + receiveData3Len)),
					SetArgReferee<2>(receiveData3Len)));
			}

			EXPECT_CALL(*mockSerial, WriteSAS(CheckBuffer(syncData, sizeof(syncData)), sizeof(syncData), true)).Times(9);

			EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(0);
			EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(0);

			CSASConsumer *cons = NULL;
			CQueueList *gcf = NULL;
			CQueueList *in = NULL;

			Init(cons, gcf, in);

			CSASInterface sas(mockSerial, cons, gcf, in);
			Assert::AreEqual(false, sas.GameComStatus()); // Verify setup.
			Assert::AreEqual(true, sas.HasNoErrors());
			Assert::AreEqual((WORD)0, sas.GetConsecutiveCommErrorCount());

			CLongPollUMessage *lp(new CLongPollUMessage(GAMEADDRESS));
			lp->SetPollValue(EFTCREDITSUP);
			lp->SetTransactionNumber(eftNumber);
			lp->SetAck(EFTACKPHASE1);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();

			Assert::AreEqual(SASPOLL_NACK, lp->GetReturnValue());
			Assert::AreEqual(1, lp->GetRetryCount());
			Assert::AreEqual(false, sas.GameComStatus());
			Assert::AreEqual(false, sas.HasNoErrors());
			Assert::AreEqual((WORD)9, sas.GetConsecutiveCommErrorCount());

			Cleanup(cons, gcf, in);
			delete lp;
		}
// #pragma endregion CSASInterface::SendToSAS_Upload_NoResponse4x
	};

	CString SASInterfaceUnitTests2::m_gmockFailure;
	SASInterfaceUnitTests2::CTersePrinter SASInterfaceUnitTests2::m_tersePrinter;
}
#endif //TODO-PORT
