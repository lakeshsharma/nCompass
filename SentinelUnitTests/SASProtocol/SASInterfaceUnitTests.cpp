#include "stdafx.h"
#include "SASProtocol/SASInterface.h"
#include "SASProtocol/SASSerialWrapper.h"
#include "SASProtocol/SASGeneralPoll.h"
#include "SASProtocol/LongPollDMessage.h"
#include "SASProtocol/LongPoll38Message.h"
#include "SASProtocol/LongPoll6FMessage.h"
#include "SASProtocol/LongPoll8AMessage.h"
#include "gtest/gtest.h"
#include "SASProtocol/SASSerialWrapperMock.h"

// Unit testing includes.
////#include "UnitTestingToString.h"
//#include "UnitTestingUtils.h"
#include "Utility/UnitTestingMatchers.h"

//using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::DoAll;
using ::testing::_;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;
using ::testing::Return;
using ::testing::InSequence;
using ::testing::ElementsAre;
using ::testing::Pointee;
using ::testing::Matcher;
using ::testing::SetArgReferee;
using ::testing::Ne;
using ::testing::WithoutArgs;
using ::testing::WithArgs;

namespace SentinelNativeUnitTests
{
	const byte GAMEADDRESS = 1;

	class SASInterfaceUnitTests : public ::testing::Test
	{
	public:
		CSASConsumer *cons;
  CQueueList *gcf;
  CQueueList *in;
  NiceMock<CSASSerialWrapperMock> *mockSerial;

		/// <summary>
		/// Mock some of S5 API calls so they fail.
		/// Do not mock GetTickCount32() or Sleep(), so we still test actual elapsed tick counts.
		/// </summary>
		class COperatingSystemMockFailedPolls : public COperatingSystem
		{
			static const UINT SASREADINTERVAL = 24;
			static const DWORD m_millisecondsPerSecond = 1000;
			byte m_lastGameAddress;
		public:
			DWORD m_lastReadPerfCount; // In milliseconds.
			DWORD m_lastWritePerfCount; // In milliseconds.
			DWORD m_minPerfCountBetweenPolls; // In milliseconds.
	
			COperatingSystemMockFailedPolls() :
				m_lastGameAddress(1),
				m_lastReadPerfCount(0),
				m_lastWritePerfCount(0),
				m_minPerfCountBetweenPolls(0)
			{
			}
	
			bool VerifyMinTicks(DWORD pollingSpeed)
			{
				return m_minPerfCountBetweenPolls >= pollingSpeed;
			}
	
			// Mock S5 API calls.
			S7_Result S7LITE_UART_ClearBuffers(UINT uart, USHORT mask)
			{
				return S7DLL_STATUS_WARNING;
			}
	
			// Get performance counter in milliseconds.
			/*
			double GetPerfCount()
			{
				LARGE_INTEGER performanceFrequency;
				::QueryPerformanceFrequency(&performanceFrequency);
				LARGE_INTEGER performanceCounter;
				::QueryPerformanceCounter(&performanceCounter);
	
				return performanceCounter.QuadPart * (double)m_millisecondsPerSecond / performanceFrequency.QuadPart;
			} */
	
			S7_Result S7LITE_UART_SendBuffer(UINT uart, USHORT *buffer, UINT length)
			{
				if (buffer != nullptr && length > 0)
				{
					// Only check timing for polls to the EGM.
					m_lastGameAddress = (*buffer) & 0x7f;
					if (m_lastGameAddress == GAMEADDRESS)
					{
						m_lastWritePerfCount = CUtilities::GetTickCount();
						double perfCountBetweenPolls = m_lastWritePerfCount - m_lastReadPerfCount;
						if (perfCountBetweenPolls < m_minPerfCountBetweenPolls)
						{
							m_minPerfCountBetweenPolls = perfCountBetweenPolls;
						}
					}
				}
	
				return S7DLL_STATUS_WARNING;
			}
	
			S7_Result S7LITE_UART_GetBuffer(UINT uart, USHORT *buffer, UINT& length)
			{
				// Only check timing for polls to the EGM.
				if (m_lastGameAddress == GAMEADDRESS)
				{
					// 2015-06-01 CSASSerialWrapper implementation notes:
					// S7LITE_UART_GetBuffer() calls do not affect the polling cycle delay if zero bytes are returned.
					// If S7LITE_UART_GetBuffer() returns less than the requested bytes then the polling cycle delay is calculated based on the UART read timeout.
					// If less than the requested bytes are returned then there should be a delay of at least the UART read timeout.
					length /= 2;
					if (length > 0)
					{
						m_lastReadPerfCount = CUtilities::GetTickCount(); // Last buffer element is read before the UART read timeout starts.
					}
					CUtilities::Sleep(SASREADINTERVAL);
				}
	
				return S7DLL_STATUS_WARNING;
			}
		};
	
		/// <summary>
		/// Self-contained wrapper class for CSASInterface.
		/// Uses the CSASInterface(CSASSerialWrapper *, ...) constructor.
		/// </summary>
		class CSASInterfaceWrapper :
			public CSASInterface
		{
		public:
			CSASConsumer *cons;
			CQueueList *gcf;
			CQueueList *in;
	
			CSASInterfaceWrapper() :
				  CSASInterface(new CSASSerialWrapper(), cons, gcf, in),
				  cons(),
				  gcf(nullptr),
				  in(nullptr)
			{
			}
	
			virtual ~CSASInterfaceWrapper()
			{
			}
	
		private:
			// Disable default copy constructor, and default assignment operator.
			CSASInterfaceWrapper(const CSASInterfaceWrapper& rhs);
			CSASInterfaceWrapper& operator=(const CSASInterfaceWrapper& rhs);
			
		};
	
		virtual void SetUp()
		{
			cons = new CSASConsumer();
			mockSerial = new NiceMock<CSASSerialWrapperMock>();
			gcf = new CQueueList("");
			in = new CQueueList("");
		}

		virtual void TearDown()
		{
			delete cons;
			delete gcf;
			delete in;
		}

		/// <summary>
		/// Helper method to test:
		///     CSASInterface::SetTicketExpirationDuration()
		///     CSASInterface::GetTicketExpirationDuration()
		/// <param name="setValue">Value to set.</param>
		/// <param name="expectedValue">Value expected when getter is called.</param>
		/// </summary>
		void SetTicketExpirationDurationTest(WORD setValue, WORD expectedValue)
		{
			// Setup.
			CSASInterfaceWrapper obj;
		
			// Perform operation(s) and test(s).
			obj.SetTicketExpirationDuration(setValue);
			EXPECT_EQ(expectedValue, obj.GetTicketExpirationDuration()) << _T("GetTicketExpirationDuration()");
		}
	
		/// <summary>
		/// Helper method to test:
		///     CSASInterface::SetTicketExpirationDuration()
		///     CSASInterface::GetTicketExpirationDuration()
		/// <param name="value">Value to set. Also value expected when getter is called.</param>
		/// </summary>
		void SetTicketExpirationDurationTest(WORD value)
		{
			SetTicketExpirationDurationTest(value, value);
		}

	};

	TEST_F(SASInterfaceUnitTests, SASInterfaceGeneralPollSuccess)
	{
		// Should give a successful GPE_CASHBOX_OPENED general poll
		EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(1);
		EXPECT_CALL(*mockSerial, ReadSAS(_, 1, _)).Times(1)
			.WillOnce(DoAll(SetArgPointee<0>(GPE_CASHBOX_OPENED),
								 SetArgReferee<2>(1)));
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		CSASGeneralPoll *gp = new CSASGeneralPoll(GAMEADDRESS);
		gp->Fill();
		sas.SendToSAS(gp, 0);
		gp->ResponseReceived();
	
		EXPECT_TRUE(gp->GetReturnValue() == SASPOLL_SUCCESS);
		EXPECT_TRUE(gp->GetExceptionValue() == GPE_CASHBOX_OPENED);
		EXPECT_TRUE(Mock::VerifyAndClearExpectations(&mockSerial));
	
		delete gp;
	}
	
	TEST_F(SASInterfaceUnitTests, SASInterfaceGeneralPollFailure)
	{
		// Should give a failed general poll
		EXPECT_CALL(*mockSerial, ReadSAS(_, 1, _)).Times(3) // Do bother with Read on the 3rd call to SendToSAS ( ie NACK )
			.WillRepeatedly(
			SetArgReferee<2>(0));
		EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(4); // Includes 3 tries plus a automatic resync poll
	
		CSASInterface sas(mockSerial, cons, gcf, in);
	
		// Start with functioning game_comm
		CInternalMessage *iMsg = sas.SetGameComStatus(true);
		delete iMsg;
		CSASGeneralPoll *gp = new CSASGeneralPoll(GAMEADDRESS);
		gp->Fill();
		sas.SendToSAS(gp, 0);
		gp->ResponseReceived();
	
		EXPECT_TRUE(gp->GetReturnValue() == SASPOLL_NACK);
		EXPECT_TRUE(Mock::VerifyAndClearExpectations(&mockSerial));
	
		delete gp;
	}
	
	TEST_F(SASInterfaceUnitTests, SASInterfaceGeneralPollSuccessOn2ndAttempt)
	{
		// Having InSequence causes all EXPECT_CALLs in block to having in order
		InSequence s;
	
		// Should give a failed general poll
		EXPECT_CALL(*mockSerial, ReadSAS(_, 1, _)).Times(1)
			.WillRepeatedly(
			SetArgReferee<2>(0));
	
		// Should give a successful GPE_CASHBOX_OPENED general poll
		EXPECT_CALL(*mockSerial, ReadSAS(_, 1, _)).Times(1)
			.WillOnce(DoAll(SetArgPointee<0>(GPE_CASHBOX_OPENED),
								 SetArgReferee<2>(1)));
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		CSASGeneralPoll *gp = new CSASGeneralPoll(GAMEADDRESS);
		gp->Fill();
		sas.SendToSAS(gp, 0);
		gp->ResponseReceived();
	
		EXPECT_TRUE(gp->GetReturnValue() == SASPOLL_SUCCESS);
		EXPECT_TRUE(Mock::VerifyAndClearExpectations(&mockSerial));
	
		delete gp;
	}
	
// TODO: broken unit teset
//	TEST_F(SASInterfaceUnitTests, SASInterfaceEFTFailedRetryCount)
//	{
//		// Should give a failed long poll
//		EXPECT_CALL(*mockSerial, ReadSAS(_, XD_RESPONSESIZE, _)).Times(9)
//			.WillRepeatedly(
//			SetArgReferee<2>(0));
//		EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(9);
//
//		CSASInterface sas(mockSerial, cons, gcf, in);
//
//		// Start with functioning game_comm
//		CInternalMessage *iMsg = sas.SetGameComStatus(true);
//		delete iMsg;
//
//		CLongPollDMessage *lp = new CLongPollDMessage(GAMEADDRESS);
//		lp->SetAmount(100);
//		lp->SetAck(0);
//		lp->Fill();
//		sas.SendToSAS(lp, 0);
//		lp->ResponseReceived();
//
//		EXPECT_TRUE(lp->GetReturnValue() == SASPOLL_NACK);
//		EXPECT_TRUE(Mock::VerifyAndClearExpectations(&mockSerial));
//
//		delete lp;
//	}
//
	TEST_F(SASInterfaceUnitTests, SASInterfaceBadThenGoodChecksum)
	{
	
		byte sendData[2] = { 01, 0x38 };
		byte receiveDataBadChecksum[8] = { 01, 0x38, 00, 0x10, 00, 00, 0xDA, 0x8A };
		byte receiveDataGoodChecksum[8] = { 01, 0x38, 00, 00, 00, 00, 0xDA, 0x8A };
		InSequence s;
	
		EXPECT_CALL(*mockSerial, ReadSAS(_, 8, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveDataBadChecksum, receiveDataBadChecksum + 8),
							 SetArgReferee<2>(8)));
		EXPECT_CALL(*mockSerial, ReadSAS(_, 8, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveDataGoodChecksum, receiveDataGoodChecksum + 8),
							 SetArgReferee<2>(8)));
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		CMeters *meters = new CMeters(MEMORY_NONE);
		meters->SetMeter(mD500);
		CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
	
		EXPECT_TRUE(lp->GetReturnValue() == SASPOLL_SUCCESS);
		EXPECT_TRUE(meters->GetMeterByIndex(0)->GetValue() == 0);
		EXPECT_TRUE(Mock::VerifyAndClearExpectations(&mockSerial));
	
		delete meters;
		delete lp;
	}
	
	TEST_F(SASInterfaceUnitTests, SASInterfaceGoodBillMeter)
	{
	
		byte sendData[2] = { 01, 0x38 };
		byte receiveData[8] = { 01, 0x38, 00, 00, 00, 00, 0xDA, 0x8A };
	
		EXPECT_CALL(*mockSerial, ReadSAS(_, 8, _)).Times(1)
			.WillRepeatedly(DoAll(
									 SetArrayArgument<0>(receiveData, receiveData + 8),
									 SetArgReferee<2>(8)));
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		CMeters *meters = new CMeters(MEMORY_NONE);
		meters->SetMeter(mD500);
		CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
	
		EXPECT_TRUE(lp->GetReturnValue() == SASPOLL_SUCCESS);
		EXPECT_TRUE(meters->GetMeterByIndex(0)->GetValue() == 0);
		EXPECT_TRUE(Mock::VerifyAndClearExpectations(&mockSerial));
	
		delete meters;
		delete lp;
	}
	
	TEST_F(SASInterfaceUnitTests, SASInterfaceSamePollBackToBack)
	{
		byte sendData[2] = { 01, 0x38 };
		byte receiveData[8] = { 01, 0x38, 00, 00, 00, 00, 0xDA, 0x8A };
		byte sendData1A[2] = { 01, 0x1A };
		byte receiveData1A[8] = { 01, 0x1A, 00, 00, 00, 00, 0xC3, 0xFC };
	
		InSequence s;
	
		// Send 38 long poll
		EXPECT_CALL(*mockSerial, WriteSAS(_, 2, true)).Times(1);
		EXPECT_CALL(*mockSerial, ReadSAS(_, 8, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveData, receiveData + 8),
							 SetArgReferee<2>(8)));
		// Send 1A poll
		EXPECT_CALL(*mockSerial, WriteSAS(CheckPollValue(1, 0x1A), 2, true)).Times(1);
	
		EXPECT_CALL(*mockSerial, ReadSAS(_, 8, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveData1A, receiveData1A + 8),
							 SetArgReferee<2>(8)));
		// Send 2nd 38 long poll
		EXPECT_CALL(*mockSerial, WriteSAS(_, 2, true)).Times(1);
		EXPECT_CALL(*mockSerial, ReadSAS(_, 8, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveData, receiveData + 8),
							 SetArgReferee<2>(8)));
	
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		CMeters *meters = new CMeters(MEMORY_NONE);
		meters->SetMeter(mD500);
		CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
		delete lp;
	
		lp = new CLongPoll38Message(GAMEADDRESS, meters);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
	
		EXPECT_TRUE(Mock::VerifyAndClearExpectations(&mockSerial));
	
		delete meters;
		delete lp;
	}
	
	TEST_F(SASInterfaceUnitTests, SASInterfaceSamePoll6FBackToBack)
	{
		byte sendData6F[17] = { 01, 0x6F, 0x0C, 0x00, 0x00, 0x09, 0x00, 0x19, 0x00, 0x1A, 0x00, 0x7F, 0x00, 0x24, 0x00, 0xE6, 0x7F };
		byte receiveData6F[62] = { 01, 0x6F, 0x39, 0x00, 0x00, 0x09, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19,
			0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x56, 0x00, 0x1A, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x7F, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x30,
			0x29, 0x52, 0x06, 0x89, 0x1A };
		byte sendData1A[2] = { 01, 0x1A };
		byte receiveData1A[8] = { 01, 0x1A, 00, 00, 00, 00, 0xC3, 0xFC };
	
		byte sendDataAF[13] = { 01, 0xAF, 0x08, 0x00, 0x00, 0x33, 0x00, 0x19, 0x00, 0x1B, 0x00, 0x92, 0x09 };
		byte receiveDataAF[40] = { 01, 0xAF, 0x23, 0x00, 0x00, 0x33, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x81, 0x00, 0x19,
			0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x56, 0x00, 0x1B, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x4F, 0x40 };
	
		InSequence s;
	
		// Send 6F long poll
		EXPECT_CALL(*mockSerial, WriteSAS(_, _, true)).Times(1);
		EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveData6F, receiveData6F + 62),
							 SetArgReferee<2>(3)));
		EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveData6F + 3, receiveData6F + 62),
							 SetArgReferee<2>(59)));
	
		// Send 2nd 6F long poll, should be converted to AF
		EXPECT_CALL(*mockSerial, WriteSAS(CheckPollValue(1, 0xAF), _, true)).Times(1);
		EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveDataAF, receiveDataAF + 40),
							 SetArgReferee<2>(3)));
	
		EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveDataAF + 3, receiveDataAF + 40),
							 SetArgReferee<2>(37)));
	
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		sas.SetAfSupported(true);
		CMeters *meters6F = new CMeters(MEMORY_NONE);
	
		// Need space for 5 metercodes
		meters6F->SetMeter(mTCo);
		meters6F->SetMeter(gGW);
		meters6F->SetMeter(gCC);
		meters6F->SetMeter(mHCC);
		meters6F->SetMeter(mD200);
	
		CLongPoll6FMessage *lp = new CLongPoll6FMessage(GAMEADDRESS, meters6F);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
		delete lp;
	
		CMeters *metersAF = new CMeters(MEMORY_NONE);
		metersAF->SetMeter(mTCo);
		metersAF->SetMeter(gCC);
		metersAF->SetMeter(mHCC);
		lp = new CLongPoll6FMessage(GAMEADDRESS, metersAF);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
	
		EXPECT_TRUE(Mock::VerifyAndClearExpectations(&mockSerial));
	
		delete meters6F;
		delete metersAF;
		delete lp;
	}
	
	TEST_F(SASInterfaceUnitTests, SASInterfaceSamePoll6FBackToBackAFNotSupported)
	{
		byte sendData6F[17] = { 01, 0x6F, 0x0C, 0x00, 0x00, 0x09, 0x00, 0x19, 0x00, 0x1A, 0x00, 0x7F, 0x00, 0x24, 0x00, 0xE6, 0x7F };
		byte receiveData6F[62] = { 01, 0x6F, 0x39, 0x00, 0x00, 0x09, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19,
			0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x56, 0x00, 0x1A, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x7F, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x30,
			0x29, 0x52, 0x06, 0x89, 0x1A };
		byte sendData1A[2] = { 01, 0x1A };
		byte receiveData1A[8] = { 01, 0x1A, 00, 00, 00, 00, 0xC3, 0xFC };
	
		InSequence s;
	
		// Send 6F long poll
		EXPECT_CALL(*mockSerial, WriteSAS(_, _, true)).Times(1);
		EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveData6F, receiveData6F + 62),
							 SetArgReferee<2>(3)));
		EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveData6F + 3, receiveData6F + 62),
							 SetArgReferee<2>(59)));
		// Send 1A poll
		EXPECT_CALL(*mockSerial, WriteSAS(CheckPollValue(1, 0x1A), 2, true)).Times(1);
		EXPECT_CALL(*mockSerial, ReadSAS(_, 8, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveData1A, receiveData1A + 8),
							 SetArgReferee<2>(8)));
		// Send 2nd 6F long poll, should stay 6F
		EXPECT_CALL(*mockSerial, WriteSAS(CheckPollValue(1, 0x6F), _, true)).Times(1);
		EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveData6F, receiveData6F + 62),
							 SetArgReferee<2>(3)));
		EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveData6F + 3, receiveData6F + 62),
							 SetArgReferee<2>(59)));
	
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		sas.SetAfSupported(false);
	
		CMeters *meters6F = new CMeters(MEMORY_NONE);
	
		// Need space for 5 metercodes
		meters6F->SetMeter(mTCo);
		meters6F->SetMeter(gGW);
		meters6F->SetMeter(gCC);
		meters6F->SetMeter(mHCC);
		meters6F->SetMeter(mD200);
	
		CLongPoll6FMessage *lp = new CLongPoll6FMessage(GAMEADDRESS, meters6F);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
		delete lp;
	
		lp = new CLongPoll6FMessage(GAMEADDRESS, meters6F);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
	
		EXPECT_TRUE(Mock::VerifyAndClearExpectations(&mockSerial));
	
		delete meters6F;
		delete lp;
	}
	
//	#pragma region GetGameComStatus
	/// <summary>
	/// Code coverage and functional test for:
	///     CSASInterface::CSASInterface(CSASSerialWrapper * ...)
	///     CSASInterface::GetGameComStatus()
	/// Test default value after using the CSASSerialWrapper constructor.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, SetGameComStatus_Default2Test)
	{
		// Setup.
		CSASSerialWrapper *sasSerialWrapper(new CSASSerialWrapper());
	
		// Perform operation(s) and test(s).
		CSASInterface obj(sasSerialWrapper, nullptr, nullptr, nullptr);
		EXPECT_FALSE(obj.GameComStatus()) << _T("msg->GetGameComStatus()");
	}
	
	/// <summary>
	/// Code coverage and functional test for:
	///     CSASInterface::SetGameComStatus()
	///     CSASInterface::GetGameComStatus()
	/// Set to false.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, SetGameComStatus_FalseTest)
	{
		// Setup.
		CSASSerialWrapper *sasSerialWrapper(new CSASSerialWrapper());
		CSASInterface obj(sasSerialWrapper, nullptr, nullptr, nullptr);
	
		// Perform operation(s) and test(s).
		CInternalMessage *msg(obj.SetGameComStatus(false));
		EXPECT_NE(nullptr, msg) << _T("msg");
		EXPECT_EQ(INT_MSG_GAME_COMM_STATUS, msg->GetMessageType()) << _T("msg->GetMessageType()");
		EXPECT_EQ((DWORD)false, msg->GetDWordData()) << _T("msg->GetDWordData() gameComStatus");
		EXPECT_EQ((DWORD)false, msg->GetDWordData(true)) << _T("msg->GetDWordData(true) sasStartupQueueEmpty");
		EXPECT_FALSE(obj.GameComStatus()) << _T("msg->GetGameComStatus()");
	
		// Cleanup.
		delete msg;
	}
	
	/// <summary>
	/// Code coverage and functional test for:
	///     CSASInterface::SetGameComStatus()
	///     CSASInterface::GetGameComStatus()
	/// Set to true.  sasStartupQueueEmpty values is undefined.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, SetGameComStatus_TrueTest)
	{
		// Setup.
		CSASSerialWrapper *sasSerialWrapper(new CSASSerialWrapper());
		CSASInterface obj(sasSerialWrapper, nullptr, nullptr, nullptr);
	
		// Perform operation(s) and test(s).
		CInternalMessage *msg(obj.SetGameComStatus(true));
		EXPECT_NE(nullptr, msg) << _T("msg");
		EXPECT_EQ(INT_MSG_GAME_COMM_STATUS, msg->GetMessageType()) << _T("msg->GetMessageType()");
		EXPECT_EQ((DWORD)true, msg->GetDWordData()) << _T("msg->GetDWordData() gameComStatus");
		EXPECT_EQ(true, obj.GameComStatus()) << _T("msg->GetGameComStatus()");
	
		// Cleanup.
		delete msg;
	}
	
	/// <summary>
	/// Code coverage and functional test for:
	///     CSASInterface::SetGameComStatus()
	/// Set to false then set to true.  sasStartupQueueEmpty value MUST be false.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, SetGameComStatus_FalseTrueTest)
	{
		// Setup.
		CSASSerialWrapper *sasSerialWrapper(new CSASSerialWrapper());
		CSASInterface obj(sasSerialWrapper, nullptr, nullptr, nullptr);
	
		// Perform operation(s) and test(s).
		delete obj.SetGameComStatus(false);
		CInternalMessage *msg(obj.SetGameComStatus(true));
		EXPECT_NE(nullptr, msg) << _T("msg");
		EXPECT_EQ(INT_MSG_GAME_COMM_STATUS, msg->GetMessageType()) << _T("msg->GetMessageType()");
		EXPECT_EQ((DWORD)true, msg->GetDWordData()) << _T("msg->GetDWordData() gameComStatus");
		EXPECT_EQ((DWORD)false, msg->GetDWordData(true)) << _T("msg->GetDWordData(true) sasStartupQueueEmpty");
	
		// Cleanup.
		delete msg;
	}
	// #pragma endregion GetGameComStatus
	
	// #pragma region ProcessReadQueueIsEmpty
	/// <summary>
	/// Code coverage and functional test for:
	///     CSASInterface::SetGameComStatus()
	///     CSASInterface::ProcessReadQueueIsEmpty()
	/// Set game com status to false then call ProcessReadQueueIsEmpty(). Verify that returned msg is NULL.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, ProcessReadQueueIsEmpty_SetGameComStatusFalseTest)
	{
		// Setup.
		CSASSerialWrapper *sasSerialWrapper(new CSASSerialWrapper());
		CSASInterface obj(sasSerialWrapper, nullptr, nullptr, nullptr);
	
		// Perform operation(s) and test(s).
		delete obj.SetGameComStatus(false);
		CInternalMessage *msg(obj.ProcessReadQueueIsEmpty());
		EXPECT_EQ(nullptr, msg) << _T("msg");
	
		// Cleanup.
		delete msg;
	}
	
	/// <summary>
	/// Code coverage and functional test for:
	///     CSASInterface::SetGameComStatus()
	///     CSASInterface::ProcessReadQueueIsEmpty()
	/// Set game com status to false then true and then call ProcessReadQueueIsEmpty(). sasStartupQueueEmpty value MUST be true.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, ProcessReadQueueIsEmpty_SetGameComStatusFalseTrueTest)
	{
		// Setup.
		CSASSerialWrapper *sasSerialWrapper(new CSASSerialWrapper());
		CSASInterface obj(sasSerialWrapper, nullptr, nullptr, nullptr);
	
		// Perform operation(s) and test(s).
		delete obj.SetGameComStatus(false);
		delete obj.SetGameComStatus(true);
		CInternalMessage *msg(obj.ProcessReadQueueIsEmpty());
		EXPECT_NE(nullptr, msg) << _T("msg");
		EXPECT_EQ(INT_MSG_GAME_COMM_STATUS, msg->GetMessageType()) << _T("msg->GetMessageType()");
		EXPECT_EQ((DWORD)true, msg->GetDWordData()) << _T("msg->GetDWordData() gameComStatus");
		EXPECT_EQ((DWORD)true, msg->GetDWordData(true)) << _T("msg->GetDWordData(true) sasStartupQueueEmpty");
	
		// Cleanup.
		delete msg;
	}
	// #pragma endregion ProcessReadQueueIsEmpty
	
	// #pragma region GetPollingSpeed
	/// <summary>
	/// Functional test for:
	///     CSASSerialWrapper
	/// Test static const members.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, GetPollingSpeed_StaticConst)
	{
		EXPECT_EQ((short)200, CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS);
		EXPECT_EQ((short)40, CSASSerialWrapper::MS40_POLLING_SPEED_MILLISECONDS);
		EXPECT_EQ((short)40, CSASSerialWrapper::DEFAULT_POLLING_SPEED_MILLISECONDS);
	}
	
	/// <summary>
	/// Functional test for:
	///     CSASInterface::CSASInterface(CSASConsumer *, ...)
	///     CSASInterface::GetPollingSpeed()
	/// Test the standard constructor for default value(s).
	TEST_F(SASInterfaceUnitTests, GetPollingSpeed_StandardDefault)
	{
		// Setup.
		CSASSerialWrapper *serial(new CSASSerialWrapper());
	
		// Perform operation(s) and test(s).
		CSASInterface sas(serial, cons, gcf, in);
		EXPECT_EQ(CSASSerialWrapper::DEFAULT_POLLING_SPEED_MILLISECONDS, sas.GetPollingSpeed());
	}
	
	/// <summary>
	/// Functional test for:
	///     CSASInterface::CSASInterface(CSASSerialWrapper *, ...)
	///     CSASInterface::GetPollingSpeed()
	/// Test the mock constructor for default value(s).
	TEST_F(SASInterfaceUnitTests, GetPollingSpeed_MockDefault)
	{
		// Setup.
	
		// Perform operation(s) and test(s).
		CSASInterface sas(mockSerial, cons, gcf, in);
		EXPECT_EQ(CSASSerialWrapper::DEFAULT_POLLING_SPEED_MILLISECONDS, sas.GetPollingSpeed());
	}
	
	/// <summary>
	/// Functional test for:
	///     CSASInterface::SetPollingSpeed()
	///     CSASInterface::GetPollingSpeed()
	/// Test the setter with non-default value(s).
	TEST_F(SASInterfaceUnitTests, GetPollingSpeed_Set)
	{
		CSASInterface sas(mockSerial, cons, gcf, in);
	
		// Perform operation(s) and test(s).
		sas.SetPollingSpeed(CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS);
		EXPECT_EQ(CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS, sas.GetPollingSpeed());
	
		sas.SetPollingSpeed(CSASSerialWrapper::MS40_POLLING_SPEED_MILLISECONDS);
		EXPECT_EQ(CSASSerialWrapper::MS40_POLLING_SPEED_MILLISECONDS, sas.GetPollingSpeed());
	
		sas.SetPollingSpeed(CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS);
		EXPECT_EQ(CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS, sas.GetPollingSpeed());
	}
	// #pragma endregion GetPollingSpeed
	
	TEST_F(SASInterfaceUnitTests, SASInterfacePollingSpeedFailedPolls)
	{
		for (int testCount = 0; testCount < 2; ++testCount)
		{
	
			short pollingSpeed;
			byte sendData[2] = { 01, 0x38 };
			COperatingSystemMockFailedPolls os;
			os.m_lastReadPerfCount = CUtilities::GetTickCount(); // Same as mockSerial construction or earlier.
	
			CSASSerialWrapper *mock(new CSASSerialWrapper(os));
			CSASInterface sas(mock, cons, gcf, in);
			CInternalMessage *iMsg = sas.SetGameComStatus(true);
			delete iMsg;
			os.m_minPerfCountBetweenPolls = MAXDWORD;
			pollingSpeed = 40;
			sas.SetPollingSpeed(pollingSpeed);
			CMeters *meters = new CMeters(MEMORY_NONE);
			meters->SetMeter(mD500);
			CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();
			EXPECT_TRUE(os.VerifyMinTicks(pollingSpeed));
			delete lp;
	
			pollingSpeed = 200;
			sas.SetPollingSpeed(pollingSpeed);
			os.m_minPerfCountBetweenPolls = MAXDWORD;
			lp = new CLongPoll38Message(GAMEADDRESS, meters);
			lp->Fill();
			sas.SendToSAS(lp, 0);
			lp->ResponseReceived();
	
			EXPECT_TRUE(os.VerifyMinTicks(pollingSpeed));
			EXPECT_TRUE(Mock::VerifyAndClearExpectations(&mock));
			delete meters;
			delete lp;
		}
	}
	
	TEST_F(SASInterfaceUnitTests, SASInterfaceVariableLengthTooMuchData)
	{
		byte sendData6F[17] = { 01, 0x6F, 0x0C, 0x00, 0x00, 0x09, 0x00, 0x19, 0x00, 0x1A, 0x00, 0x7F, 0x00, 0x24, 0x00, 0xE6, 0x7F };
		byte receiveData6F[62] = { 01, 0x6F, 0xff, 0x00, 0x00, 0x09, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19,
			0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x56, 0x00, 0x1A, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x7F, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x30,
			0x29, 0x52, 0x06, 0x89, 0x1A };
	
		InSequence s;
	
		// Send 6F long poll, and following resync
		ON_CALL(*mockSerial, ReadSAS(_, _, _))
			.WillByDefault(DoAll(
									SetArrayArgument<0>(receiveData6F, receiveData6F + 3),
									SetArgReferee<2>(3)));
	
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		sas.SetAfSupported(true);
		CMeters *meters6F = new CMeters(MEMORY_NONE);
	
		// Need space for 5 metercodes
		meters6F->SetMeter(mTCo);
		meters6F->SetMeter(gGW);
		meters6F->SetMeter(gCC);
		meters6F->SetMeter(mHCC);
		meters6F->SetMeter(mD200);
	
		CLongPoll6FMessage *lp = new CLongPoll6FMessage(GAMEADDRESS, meters6F);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
		EXPECT_TRUE(lp->GetReturnValue() == SASPOLL_DATA_TOO_LARGE);
		delete lp;
	
	
		delete meters6F;
	}
	
	TEST_F(SASInterfaceUnitTests, SASInterfaceVariableLengthShortReadNoLengthByte)
	{
		byte sendData6F[17] = { 01, 0x6F, 0x0C, 0x00, 0x00, 0x09, 0x00, 0x19, 0x00, 0x1A, 0x00, 0x7F, 0x00, 0x24, 0x00, 0xE6, 0x7F };
		byte receiveData6F[62] = { 01, 0x6F, 0x39, 0x00, 0x00, 0x09, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19,
			0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x56, 0x00, 0x1A, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x7F, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x30,
			0x29, 0x52, 0x06, 0x89, 0x1A };
	
		// Send 6F long poll, and following resync
		//EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(4);
		EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(3)
			.WillRepeatedly(
			SetArgReferee<2>(2));
	
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		sas.SetAfSupported(true);
		CMeters *meters6F = new CMeters(MEMORY_NONE);
	
		// Need space for 5 metercodes
		meters6F->SetMeter(mTCo);
		meters6F->SetMeter(gGW);
		meters6F->SetMeter(gCC);
		meters6F->SetMeter(mHCC);
		meters6F->SetMeter(mD200);
	
		CLongPoll6FMessage *lp = new CLongPoll6FMessage(GAMEADDRESS, meters6F);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
		EXPECT_TRUE(lp->GetReturnValue() == SASPOLL_NACK);
		delete lp;
	
	
		delete meters6F;
	}
	
	TEST_F(SASInterfaceUnitTests, SASInterfaceVariableLengthNoResponse)
	{
		byte sendData6F[17] = { 01, 0x6F, 0x0C, 0x00, 0x00, 0x09, 0x00, 0x19, 0x00, 0x1A, 0x00, 0x7F, 0x00, 0x24, 0x00, 0xE6, 0x7F };
		byte receiveData6F[62] = { 01, 0x6F, 0x39, 0x00, 0x00, 0x09, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19,
			0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x56, 0x00, 0x1A, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x7F, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x30,
			0x29, 0x52, 0x06, 0x89, 0x1A };
	
		// Send 6F long poll, and following resync
		//EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(4);
		EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(3)
			.WillRepeatedly(
			SetArgReferee<2>(0));
	
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		sas.SetAfSupported(true);
		CMeters *meters6F = new CMeters(MEMORY_NONE);
	
		// Need space for 5 metercodes
		meters6F->SetMeter(mTCo);
		meters6F->SetMeter(gGW);
		meters6F->SetMeter(gCC);
		meters6F->SetMeter(mHCC);
		meters6F->SetMeter(mD200);
	
		CLongPoll6FMessage *lp = new CLongPoll6FMessage(GAMEADDRESS, meters6F);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
		EXPECT_TRUE(lp->GetReturnValue() == SASPOLL_NACK);
		delete lp;
	
	
		delete meters6F;
	}
	
	TEST_F(SASInterfaceUnitTests, SASInterfaceVariableLengthRecievedLengthByteWithNACK)
	{
		byte sendData6F[17] = { 01, 0x6F, 0x0C, 0x00, 0x00, 0x09, 0x00, 0x19, 0x00, 0x1A, 0x00, 0x7F, 0x00, 0x24, 0x00, 0xE6, 0x7F };
		byte receiveData6F[62] = { 0x81, 0x6F, 0x39, 0x00, 0x00, 0x09, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19,
			0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x56, 0x00, 0x1A, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x7F, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x30,
			0x29, 0x52, 0x06, 0x89, 0x1A };
	
		// Send 6F long poll, and following resync
		//EXPECT_CALL(*mockSerial, WriteSAS(_, _, _)).Times(4);
		EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(4) // 3 lengh byte NACK reads and 1 remainder reads
			.WillRepeatedly(DoAll(
									 SetArrayArgument<0>(receiveData6F, receiveData6F + 3),
									 SetArgReferee<2>(3)));
	
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		sas.SetAfSupported(true);
		CMeters *meters6F = new CMeters(MEMORY_NONE);
	
		// Need space for 5 metercodes
		meters6F->SetMeter(mTCo);
		meters6F->SetMeter(gGW);
		meters6F->SetMeter(gCC);
		meters6F->SetMeter(mHCC);
		meters6F->SetMeter(mD200);
	
		CLongPoll6FMessage *lp = new CLongPoll6FMessage(GAMEADDRESS, meters6F);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
		EXPECT_TRUE(lp->GetReturnValue() == SASPOLL_NACK);
		delete lp;
	
	
		delete meters6F;
	}
	
	TEST_F(SASInterfaceUnitTests, SASInterfaceVariableLengthShortReadNoLengthByteValidDataOn2nd)
	{
		byte sendData6F[17] = { 01, 0x6F, 0x0C, 0x00, 0x00, 0x09, 0x00, 0x19, 0x00, 0x1A, 0x00, 0x7F, 0x00, 0x24, 0x00, 0xE6, 0x7F };
		byte receiveData6F[62] = { 01, 0x6F, 0x39, 0x00, 0x00, 0x09, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19,
			0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x56, 0x00, 0x1A, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x7F, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x30,
			0x29, 0x52, 0x06, 0x89, 0x1A };
	
		InSequence s;
	
		// Send 6F long poll, and following resync
		EXPECT_CALL(*mockSerial, ReadSAS(_, 3, _)).Times(1)
			.WillRepeatedly(
			SetArgReferee<2>(2));
		EXPECT_CALL(*mockSerial, ReadSAS(_, 3, _)).Times(1)
			.WillRepeatedly(DoAll(
									 SetArrayArgument<0>(receiveData6F, receiveData6F + 3),
									 SetArgReferee<2>(3)));
		EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveData6F + 3, receiveData6F + 62),
							 SetArgReferee<2>(59)));
	
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		sas.SetAfSupported(true);
		CMeters *meters6F = new CMeters(MEMORY_NONE);
	
		// Need space for 5 metercodes
		meters6F->SetMeter(mTCo);
		meters6F->SetMeter(gGW);
		meters6F->SetMeter(gCC);
		meters6F->SetMeter(mHCC);
		meters6F->SetMeter(mD200);
	
		CLongPoll6FMessage *lp = new CLongPoll6FMessage(GAMEADDRESS, meters6F);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
		EXPECT_TRUE(lp->GetReturnValue() == SASPOLL_SUCCESS);
		delete lp;
	
	
		delete meters6F;
	}

#if 000//#if 000//TODO-PORT-UNIT-TESTS
	TEST_F(SASInterfaceUnitTests, SASInterfaceVariableLengthNoResponseValidOn2nd)
	{
		byte sendData6F[17] = { 01, 0x6F, 0x0C, 0x00, 0x00, 0x09, 0x00, 0x19, 0x00, 0x1A, 0x00, 0x7F, 0x00, 0x24, 0x00, 0xE6, 0x7F };
		byte receiveData6F[62] = { 01, 0x6F, 0x39, 0x00, 0x00, 0x09, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19,
			0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x56, 0x00, 0x1A, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x7F, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x30,
			0x29, 0x52, 0x06, 0x89, 0x1A };
	
		InSequence s;
	
		// Send 6F long poll, and following resync
		EXPECT_CALL(*mockSerial, ReadSAS(_, 3, _)).Times(1)
			.WillRepeatedly(
			SetArgReferee<2>(0));
		EXPECT_CALL(*mockSerial, ReadSAS(_, 3, _)).Times(1)
			.WillRepeatedly(DoAll(
									 SetArrayArgument<0>(receiveData6F, receiveData6F + 3),
									 SetArgReferee<2>(3)));
		EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(1)
			.WillOnce(DoAll(
							 SetArrayArgument<0>(receiveData6F + 3, receiveData6F + 62),
							 SetArgReferee<2>(59)));
	
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		sas.SetAfSupported(true);
		CMeters *meters6F = new CMeters(MEMORY_NONE);
	
		// Need space for 5 metercodes
		meters6F->SetMeter(mTCo);
		meters6F->SetMeter(gGW);
		meters6F->SetMeter(gCC);
		meters6F->SetMeter(mHCC);
		meters6F->SetMeter(mD200);
	
		CLongPoll6FMessage *lp = new CLongPoll6FMessage(GAMEADDRESS, meters6F);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
		EXPECT_TRUE(lp->GetReturnValue() == SASPOLL_SUCCESS);
		delete lp;
	
	
		delete meters6F;
	}
#endif
	
	TEST_F(SASInterfaceUnitTests, SASInterfaceFixedLengthNoResponseValid2ndAttempt)
	{
	
		byte sendData[2] = { 01, 0x38 };
		byte receiveData[8] = { 01, 0x38, 00, 00, 00, 00, 0xDA, 0x8A };
		InSequence s;
	
		EXPECT_CALL(*mockSerial, ReadSAS(_, 8, _)).Times(1)
			.WillRepeatedly(
			SetArgReferee<2>(0));
		EXPECT_CALL(*mockSerial, ReadSAS(_, 8, _)).Times(1)
			.WillRepeatedly(DoAll(
									 SetArrayArgument<0>(receiveData, receiveData + 8),
									 SetArgReferee<2>(8)));
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		CMeters *meters = new CMeters(MEMORY_NONE);
		meters->SetMeter(mD500);
		CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
	
		EXPECT_TRUE(lp->GetReturnValue() == SASPOLL_SUCCESS);
		EXPECT_TRUE(meters->GetMeterByIndex(0)->GetValue() == 0);
		EXPECT_TRUE(Mock::VerifyAndClearExpectations(&mockSerial));
	
		delete meters;
		delete lp;
	}
	
	TEST_F(SASInterfaceUnitTests, SASInterfaceFixedLengthShortResponseValid2ndAttempt)
	{
	
		byte sendData[2] = { 01, 0x38 };
		byte receiveData[8] = { 01, 0x38, 00, 00, 00, 00, 0xDA, 0x8A };
	
		InSequence s;
	
		EXPECT_CALL(*mockSerial, ReadSAS(_, 8, _)).Times(1)
			.WillRepeatedly(DoAll(
									 SetArrayArgument<0>(receiveData, receiveData + 5),
									 SetArgReferee<2>(5)));
		EXPECT_CALL(*mockSerial, ReadSAS(_, 8, _)).Times(1)
			.WillRepeatedly(DoAll(
									 SetArrayArgument<0>(receiveData, receiveData + 8),
									 SetArgReferee<2>(8)));
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		CMeters *meters = new CMeters(MEMORY_NONE);
		meters->SetMeter(mD500);
		CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
	
		EXPECT_TRUE(lp->GetReturnValue() == SASPOLL_SUCCESS);
		EXPECT_TRUE(meters->GetMeterByIndex(0)->GetValue() == 0);
		EXPECT_TRUE(Mock::VerifyAndClearExpectations(&mockSerial));
	
		delete meters;
		delete lp;
	}
	
	TEST_F(SASInterfaceUnitTests, SASInterfaceFixedLengthNACK)
	{
	
		byte sendData[2] = { 01, 0x38 };
		byte receiveData[8] = { 0x81, 0x38, 00, 00, 00, 00, 0xDA, 0x8A };
	
		EXPECT_CALL(*mockSerial, ReadSAS(_, 8, _)).Times(3)
			.WillRepeatedly(DoAll(
									 SetArrayArgument<0>(receiveData, receiveData + 8),
									 SetArgReferee<2>(8)));
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		CMeters *meters = new CMeters(MEMORY_NONE);
		meters->SetMeter(mD500);
		CLongPoll38Message *lp = new CLongPoll38Message(GAMEADDRESS, meters);
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
	
		EXPECT_TRUE(lp->GetReturnValue() == SASPOLL_NACK);
		EXPECT_TRUE(Mock::VerifyAndClearExpectations(&mockSerial));
	
		delete meters;
		delete lp;
	}
	
	TEST_F(SASInterfaceUnitTests, SASInterface8ANoResponse)
	{
	
		byte sendData[9] = { 01, 0x8A, 00, 00, 06, 0x15, 00, 0x89, 0x8E };
		byte receiveData[1] = { 0x81 };
	
		EXPECT_CALL(*mockSerial, ReadSAS(_, _, _)).Times(3)
			.WillRepeatedly(
			SetArgReferee<2>(0));
	
		CSASInterface sas(mockSerial, cons, gcf, in);
		CLongPoll8AMessage *lp = new CLongPoll8AMessage(GAMEADDRESS);
		lp->m_legacyBonusAmount = 615;
		lp->Fill();
		sas.SendToSAS(lp, 0);
		lp->ResponseReceived();
	
		EXPECT_TRUE(lp->GetReturnValue() == SASPOLL_NACK);
		EXPECT_TRUE(Mock::VerifyAndClearExpectations(&mockSerial));
	
		delete lp;
	}
	
	/// <summary>
	/// Code coverage and functional test for:
	///     CSASInterface::CSASInterface(CSASSerialWrapper * ...)
	///     CSASSerialWrapper::GetSasStartupQueueEmpty()
	/// Test default value after using the CSASSerialWrapper constructor.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, SasStartupQueueEmpty_Default2Test)
	{
		// Setup.
		CSASSerialWrapper *sasSerialWrapper(new CSASSerialWrapper());
	
		// Perform operation(s) and test(s).
		CSASInterface obj(sasSerialWrapper, nullptr, nullptr, nullptr);
		EXPECT_FALSE(obj.GetSasStartupQueueEmpty()) << _T("GetSasStartupQueueEmpty() error");
	}
	
	/// <summary>
	/// Code coverage and functional test for:
	///     CSASInterface::CSASInterface(CSASSerialWrapper * ...)
	///     CSASSerialWrapper::GetSasStartupQueueEmpty()
	///     CSASSerialWrapper::SetGameComStatus()
	///		CSASSerialWrapper::ProcessReadQueueIsEmpty()
	/// Test m_sasStartupQueueEmpty value after calling SetGameComStatus and ProcessReadQueueIsEmpty.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, SasStartupQueueEmptyTest)
	{
		// Setup.
		CSASSerialWrapper *sasSerialWrapper(new CSASSerialWrapper());
		CInternalMessage *msg(NULL);
		// Perform operation(s) and test(s).
		CSASInterface obj(sasSerialWrapper, nullptr, nullptr, nullptr);
		EXPECT_FALSE(obj.GetSasStartupQueueEmpty()) << _T("GetSasStartupQueueEmpty() error");
	
		// Perform operation(s) and test(s).
		delete obj.SetGameComStatus(false);
		EXPECT_FALSE(obj.GetSasStartupQueueEmpty()) << _T("GetSasStartupQueueEmpty() error");
		msg = obj.ProcessReadQueueIsEmpty();
		EXPECT_EQ(nullptr, msg) << _T("msg");
		delete obj.SetGameComStatus(true);
		EXPECT_FALSE(obj.GetSasStartupQueueEmpty()) << _T("GetSasStartupQueueEmpty() error");
		delete msg;
		msg = obj.ProcessReadQueueIsEmpty();
		EXPECT_TRUE(obj.GetSasStartupQueueEmpty()) << _T("GetSasStartupQueueEmpty() error");
		EXPECT_NE(nullptr, msg) << _T("msg");
		bool areEqual(INT_MSG_GAME_COMM_STATUS == msg->GetMessageType());
		EXPECT_TRUE(areEqual) << _T("GetSasStartupQueueEmpty() error");
	
		// Cleanup.
		delete msg;
	}
	
	// #pragma region GetTicketExpirationDuration
	/// <summary>
	/// Functional test for:
	///     CSASInterface::CSASInterface(CSASSerialWrapper * ...)
	///     CSASInterface::GetGameComStatus()
	/// Test default value after using the CSASSerialWrapper constructor.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, GetTicketExpirationDuration_Default2Test)
	{
		// Data.
		const WORD expectedValue(0);
	
		// Perform operation(s) and test(s).
		CSASInterfaceWrapper obj;
		EXPECT_EQ(expectedValue, obj.GetTicketExpirationDuration()) << _T("GetTicketExpirationDuration()");
	}
	
	/// <summary>
	/// Functional test for:
	///     CSASInterface::SetTicketExpirationDuration()
	///     CSASInterface::GetTicketExpirationDuration()
	/// Test with a range of values.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, GetTicketExpirationDuration_RangeTest)
	{
		SetTicketExpirationDurationTest(0);
		SetTicketExpirationDurationTest(1);
		SetTicketExpirationDurationTest(2);
		SetTicketExpirationDurationTest(9998);
		SetTicketExpirationDurationTest(9999);
		SetTicketExpirationDurationTest(10000);
		SetTicketExpirationDurationTest(0x7ffe);
		SetTicketExpirationDurationTest(0x7fff);
		SetTicketExpirationDurationTest(0x8000);
		SetTicketExpirationDurationTest(0x8001);
		SetTicketExpirationDurationTest(0xfffe);
		SetTicketExpirationDurationTest(0xffff);
	}
	// #pragma endregion GetTicketExpirationDuration
	
	// #pragma region SetEgmSupportsLP7B
	/// <summary>
	/// Code coverage and functional test for:
	///     CSASInterface::SetEgmSupportsLP7B()
	/// Set EGM supports long poll 7B to false.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, SetEgmSupportsLP7B_False)
	{
		// Data.
		bool expected(false);
	
		// Setup.
		CSASInterfaceWrapper obj;
	
		// Perform operation(s) and test(s).
		CInternalMessage *msg(obj.SetEgmSupportsLP7B(expected));
		EXPECT_NE(nullptr, msg) << _T("msg");
		EXPECT_EQ(INT_MSG_EGM_SUPPORTS_LP_7B, msg->GetMessageType()) << _T("msg->GetMessageType()");
		EXPECT_EQ((WORD)expected, msg->GetWordData()) << _T("msg->GetWordData()");
		EXPECT_EQ(expected, (bool)msg->GetWordData()) << _T("(bool)msg->GetWordData()");
	
		// Cleanup.
		delete msg;
	}
	
	/// <summary>
	/// Code coverage and functional test for:
	///     CSASInterface::SetEgmSupportsLP7B()
	/// Set EGM supports long poll 7B to true.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, SetEgmSupportsLP7B_True)
	{
		// Data.
		bool expected(true);
	
		// Setup.
		CSASInterfaceWrapper obj;
	
		// Perform operation(s) and test(s).
		CInternalMessage *msg(obj.SetEgmSupportsLP7B(expected));
		EXPECT_NE(nullptr, msg) << _T("msg");
		EXPECT_EQ(INT_MSG_EGM_SUPPORTS_LP_7B, msg->GetMessageType()) << _T("msg->GetMessageType()");
		EXPECT_EQ((WORD)expected, msg->GetWordData()) << _T("msg->GetWordData()");
		EXPECT_EQ(expected, (bool)msg->GetWordData()) << _T("(bool)msg->GetWordData()");
	
		// Cleanup.
		delete msg;
	}
	// #pragma endregion SetEgmSupportsLP7B
	
	// #pragma region SetSASVersion
	static const size_t m_versionLengthExpected = 3;
	const byte m_versionDefaultExpected[m_versionLengthExpected] = { '0' };
	//const byte m_versionDefaultExpected[m_versionLengthExpected];
	
	// Remove all messages and assert that they are the correct default message(s).
	static void AssertRemoveMessages_Default(CQueueList& gcfToInternalsQueue)
	{
		// No messages present by default.
	
		// Verify that there are no more messages.
		EXPECT_EQ(nullptr, gcfToInternalsQueue.Remove()) << "Remove default final";
	}
	
	// Remove all messages and assert that they are the correct version message(s).
	void AssertRemoveMessages_GetSASVersion(CQueueList& gcfToInternalsQueue, string sasVersion, string desc)
	{
		CQueueItem *item(gcfToInternalsQueue.Remove());
		CInternalMessage *msg(dynamic_cast<CInternalMessage *>(item));
		EXPECT_NE(nullptr, msg) << "msg " << desc;
		EXPECT_EQ(INT_MSG_EGM_PROTOCOL_VERSION, msg->GetMessageType()) << "msg->GetMessageType " << desc;
		EXPECT_FALSE(strcmp(sasVersion.c_str(), msg->GetStringData().c_str())) << "msg->GetStringData " << desc;

		// Verify that there are no more messages.
		EXPECT_EQ(nullptr, gcfToInternalsQueue.Remove()) << "Remove default final";
	}
	
	void AssertSASVersion(CSASInterface& obj, LPCSTR version, string desc)
	{
		EXPECT_EQ(m_versionLengthExpected, (size_t)CSASInterface::VERSION_LENGTH) << "VERSION_LENGTH " << desc;
		EXPECT_EQ(0, strncmp(version, (char *)obj.GetVersion(), (size_t)CSASInterface::VERSION_LENGTH)) << "GetVersion " << desc;
		EXPECT_EQ((byte)version[0], obj.GetMajorVersion()) << "GetMajorVersion " << desc;
		EXPECT_EQ(string(version), obj.GetVersionString().c_str()) << "GetVersionString " << desc;
	}
	
	/// <summary>
	/// Functional test for:
	///     CSASInterface
	/// Verify const values.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, SetVersion_ConstTest)
	{
		EXPECT_EQ(m_versionLengthExpected, (size_t)CSASInterface::VERSION_LENGTH) << "VERSION_LENGTH";
		EXPECT_TRUE(0 == memcmp(m_versionDefaultExpected, CSASInterface::DEFAULT_VERSION, CSASInterface::VERSION_LENGTH)) << "DEFAULT_VERSION";
	}
	
	/// <summary>
	/// Functional test for:
	///     CSASInterface::CSASInterface()
	///     CSASInterface::GetVersion()
	///     CSASInterface::MajorVersion()
	///     CSASInterface::GetVersionString()
	/// Verify default values after calling the constructor.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, SetVersion_DefaultTest)
	{
		// Data.
		const LPCSTR versionSetup("0");

		// Setup.
		NiceMock<CSASSerialWrapperMock> *serial(new NiceMock<CSASSerialWrapperMock>());
		CSASConsumer sasProtocol;
		CQueueList gcfToInternalsQueue;
		CQueueList internalsToGCFQueue;

		// Perform operation(s) and test(s).
		CSASInterface obj(serial, &sasProtocol, &gcfToInternalsQueue, &internalsToGCFQueue);
		AssertSASVersion(obj, versionSetup, "setup");
		AssertRemoveMessages_Default(gcfToInternalsQueue); // Verify setup.
	}
	
	/// <summary>
	/// Functional test for:
	///     CSASInterface::SetVersion()
	///     CSASInterface::GetVersion()
	///     CSASInterface::MajorVersion()
	///     CSASInterface::GetVersionString()
	/// Verify default values when there is no response from the EGM.
	/// </summary>

	TEST_F(SASInterfaceUnitTests, SetVersion_NoReponseTest)
	{
		// Data.
		const LPCSTR versionSetup("0");
		const LPCSTR version1(nullptr);

		// Setup.
		NiceMock<CSASSerialWrapperMock> *serial(new NiceMock<CSASSerialWrapperMock>());
		CSASConsumer sasProtocol;
		CQueueList gcfToInternalsQueue;
		CQueueList internalsToGCFQueue;

		CSASInterface obj(serial, &sasProtocol, &gcfToInternalsQueue, &internalsToGCFQueue);
		AssertSASVersion(obj, versionSetup, "setup");
		AssertRemoveMessages_Default(gcfToInternalsQueue); // Verify setup.

		// Perform operation(s) and test(s).
		obj.SetVersion(version1);
		AssertSASVersion(obj, versionSetup, "first response");
		AssertRemoveMessages_GetSASVersion(gcfToInternalsQueue, versionSetup, "first response");
	}

	/// <summary>
	/// Functional test for:
	///     CSASInterface::SetVersion()
	///     CSASInterface::GetVersion()
	///     CSASInterface::MajorVersion()
	///     CSASInterface::GetVersionString()
	/// Verify correct non-default values when there is a response from the EGM.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, SetVersion_ReponseTest)
	{
		// Data.
		const LPCSTR versionSetup("0");
		const LPCSTR version1("601");

		// Setup.
		NiceMock<CSASSerialWrapperMock> *serial(new NiceMock<CSASSerialWrapperMock>());
		CSASConsumer sasProtocol;
		CQueueList gcfToInternalsQueue;
		CQueueList internalsToGCFQueue;

		CSASInterface obj(serial, &sasProtocol, &gcfToInternalsQueue, &internalsToGCFQueue);
		AssertSASVersion(obj, versionSetup, "setup");
		AssertRemoveMessages_Default(gcfToInternalsQueue); // Verify setup.

		// Perform operation(s) and test(s).
		obj.SetVersion(version1);
		AssertSASVersion(obj, version1, "first response");
		AssertRemoveMessages_GetSASVersion(gcfToInternalsQueue, version1, "first response");
	}
	
	/// <summary>
	/// Functional test for:
	///     CSASInterface::SetVersion()
	///     CSASInterface::GetVersion()
	///     CSASInterface::MajorVersion()
	///     CSASInterface::GetVersionString()
	/// Verify correct non-default values when a response is followed by the same response from the EGM.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, SetVersion_ReponseSameResponseTest)
	{
		// Data.
		const LPCSTR versionSetup("0");
		const LPCSTR version1("601");

		// Setup.
		NiceMock<CSASSerialWrapperMock> *serial(new NiceMock<CSASSerialWrapperMock>());
		CSASConsumer sasProtocol;
		CQueueList gcfToInternalsQueue;
		CQueueList internalsToGCFQueue;

		CSASInterface obj(serial, &sasProtocol, &gcfToInternalsQueue, &internalsToGCFQueue);
		AssertSASVersion(obj, versionSetup, "setup");
		AssertRemoveMessages_Default(gcfToInternalsQueue); // Verify setup.

		// Perform operation(s) and test(s).
		obj.SetVersion(version1);
		AssertSASVersion(obj, version1, "first response");
		AssertRemoveMessages_GetSASVersion(gcfToInternalsQueue, version1, "first response");

		// Perform operation(s) and test(s).
		obj.SetVersion(version1);
		AssertSASVersion(obj, version1, "second response");
		AssertRemoveMessages_GetSASVersion(gcfToInternalsQueue, version1, "second response");
	}
	
	/// <summary>
	/// Functional test for:
	///     CSASInterface::SetVersion()
	///     CSASInterface::GetVersion()
	///     CSASInterface::MajorVersion()
	///     CSASInterface::GetVersionString()
	/// Verify correct non-default values when a response is followed by a different response from the EGM.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, SetVersion_ReponseDiffResponseTest)
	{
		// Data.
		const LPCSTR versionSetup("0");
		const LPCSTR version1("601");
		const LPCSTR version2("345");

		// Setup.
		NiceMock<CSASSerialWrapperMock> *serial(new NiceMock<CSASSerialWrapperMock>());
		CSASConsumer sasProtocol;
		CQueueList gcfToInternalsQueue;
		CQueueList internalsToGCFQueue;

		CSASInterface obj(serial, &sasProtocol, &gcfToInternalsQueue, &internalsToGCFQueue);
		AssertSASVersion(obj, versionSetup, "setup");
		AssertRemoveMessages_Default(gcfToInternalsQueue); // Verify setup.

		// Perform operation(s) and test(s).
		obj.SetVersion(version1);
		AssertSASVersion(obj, version1, "first response");
		AssertRemoveMessages_GetSASVersion(gcfToInternalsQueue, version1, "first response");

		// Perform operation(s) and test(s).
		obj.SetVersion(version2);
		AssertSASVersion(obj, version2, "second response");
		AssertRemoveMessages_GetSASVersion(gcfToInternalsQueue, version2, "second response");
	}
	
	/// <summary>
	/// Functional test for:
	///     CSASInterface::SetVersion()
	///     CSASInterface::GetVersion()
	///     CSASInterface::MajorVersion()
	///     CSASInterface::GetVersionString()
	/// Verify correct non-default values when a response is followed by no response from the EGM.
	/// </summary>
	TEST_F(SASInterfaceUnitTests, SetVersion_ReponseNoResponseTest)
	{
		// Data.
		const LPCSTR versionSetup("0");
		const LPCSTR version1("601");
		const LPCSTR version2(nullptr);

		// Setup.
		NiceMock<CSASSerialWrapperMock> *serial(new NiceMock<CSASSerialWrapperMock>());
		CSASConsumer sasProtocol;
		CQueueList gcfToInternalsQueue;
		CQueueList internalsToGCFQueue;

		CSASInterface obj(serial, &sasProtocol, &gcfToInternalsQueue, &internalsToGCFQueue);
		AssertSASVersion(obj, versionSetup, "setup");
		AssertRemoveMessages_Default(gcfToInternalsQueue); // Verify setup.

		// Perform operation(s) and test(s).
		obj.SetVersion(version1);
		AssertSASVersion(obj, version1, "first response");
		AssertRemoveMessages_GetSASVersion(gcfToInternalsQueue, version1, "first response");

		// Perform operation(s) and test(s).
		obj.SetVersion(version2);
		AssertSASVersion(obj, version1, "second response");
		AssertRemoveMessages_GetSASVersion(gcfToInternalsQueue, version1, "second response");
	}
// #pragma endregion SetSASVersion
}
