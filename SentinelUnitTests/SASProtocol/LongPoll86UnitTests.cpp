#include "stdafx.h"
#include "SASProtocol/LongPoll86Message.h"

#include "SASProtocol/SASLimits.h"

// Unit testing includes.
#include "UnitTestingUtils.h"
#include "gtest/gtest.h"

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, #result)

#define AssertResult5(result, resultLength, expected, expectedLength, resultName) \
	CUnitTestingUtils::AssertEqual(result, resultLength, expected, expectedLength, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult4(result, resultLength, expected, expectedLength) AssertResult5(result, resultLength, expected, expectedLength, #result)

// AssertResult macros for calling CUnitTestingUtils from a class initialization method.
#define AssertResult3i(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, 0, __TFUNCTION__, __LINE__);
#define AssertResult2i(result, expected) AssertResult3i(result, expected, #result)

namespace SentinelUnitTests
{
        class LongPoll86Message : public ::testing::Test
        {
        public: 
            // Expected value(s).
    		static const BYTE m_pollValueExpected = 0x86;
    		static const DWORD m_maxReceivedDelayTickCountExpected = 6000;
    		static const int m_minLevelExpected = 1;
    		static const int m_maxLevelExpected = 32;
    		static const int m_minGroupIdExpected = 1;
    		static const int m_maxGroupIdExpected = 255;
    		static const int m_maxLevelCountExpected = 32;

    		static const UINT m_sendBufferSizeDefaultExpected = 0;
    		static const UINT m_responseBufferSizeDefaultExpected = 1;
    		static const bool m_swapPollValueExpected = false;
    		static const bool m_isGeneralPollExpected = false;
    		static const bool m_fillWithTickCountBeforeSendingExpected = true;
    		static const int m_returnValueDefaultExpected = SASPOLL_SUCCESS;

    		static const BYTE m_nackOrValue = 0x80;

    		// Class member variable(s).
    		int m_testIndex;

            int GetMAX_LEVEL_COUNT(CLongPoll86Message& obj)
            {
                return obj.MAX_LEVEL_COUNT;
            }

    		/// <summary>
    		/// Test static class members.
    		/// </summary>
    		void TestStaticMembers()
    		{
                // Data.
                const byte gameAddress = 1;
                const CProgressiveLevelForward progressiveData(0, false);
                // Perform operation(s) and test(s).
                CLongPoll86Message msg(gameAddress, progressiveData);

    			AssertResult2i(CLongPoll86Message::POLL_VALUE, m_pollValueExpected);
    			AssertResult2i(CProgressiveLevelForward::MaxInterval, m_maxReceivedDelayTickCountExpected);
    			AssertResult2i(CSASLimits::MIN_SAS_PROGRESSIVE_LEVEL, m_minLevelExpected);
    			AssertResult2i(CSASLimits::MAX_SAS_PROGRESSIVE_LEVEL, m_maxLevelExpected);
    			AssertResult2i(CSASLimits::MIN_SAS_PROGRESSIVE_GROUP, m_minGroupIdExpected);
    			AssertResult2i(CSASLimits::MAX_SAS_PROGRESSIVE_GROUP, m_maxGroupIdExpected);
    			AssertResult2i(GetMAX_LEVEL_COUNT(msg), m_maxLevelCountExpected);

    			AssertResult2i(CSASPollMessage::GAME_ADDRESS_NACK_OR_VALUE, m_nackOrValue);
    		}
        protected:
            virtual void SetUp() 
            {
                // Test method initialization code.
                m_testIndex = 0;

                // Perform static tests here so each test method gets a useful error message.
                TestStaticMembers();
            }

            virtual void TearDown() 
            {
            }
                
        };
// #pragma endregion ClassLevel

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll86Message::CLongPoll86Message()
		/// Code coverage:
		///     CLongPoll86Message::~CLongPoll86Message()
		/// Test for correct default values.
		/// </summary>
		TEST_F(LongPoll86Message, CLongPoll86Message_DefaultTest)
		{
			// Data.
			const byte gameAddress = 1;
			const CProgressiveLevelForward progressiveData(0, false);

			// Perform operation(s) and test(s).
			CLongPoll86Message msg(gameAddress, progressiveData);
			msg.Fill();
			AssertResult2(msg.GetSendBufferSize(), m_sendBufferSizeDefaultExpected);
			AssertResult2(msg.GetResponseBufferSize(), m_responseBufferSizeDefaultExpected);
			AssertResult2(msg.GetPollValue(), m_pollValueExpected);
			AssertResult2(msg.SwapPollValue(), m_swapPollValueExpected);
			AssertResult2(msg.IsGeneralPoll(), m_isGeneralPollExpected);
			AssertResult2(msg.FillWithTickCountBeforeSending(), m_fillWithTickCountBeforeSendingExpected);
			AssertResult2(msg.GetReturnValue(), m_returnValueDefaultExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll86Message::CLongPoll86Message()
		///     CLongPoll86Message::Fill(DWORD tickCount)
		///     CLongPoll86Message::GetSendBuffer()
		///     CLongPoll86Message::GetSendBufferSize()
		///     CLongPoll86Message::GetResponseBufferSize()
		/// Test with one progressive level and amount.
		/// </summary>
		TEST_F(LongPoll86Message, CLongPoll86Message_Add1Test)
		{
			// Data.
			const DWORD testReceivedTickCount(10000);
			const BYTE testGroupId(0xab);
			const BYTE testLevel(3);

			const __int64 testAmount(456);
			const ProgressiveLevelType::Type  testType(ProgressiveLevelType::Standard);
			CProgressiveLevelForward varProgressiveData(testGroupId, false);
			varProgressiveData.AddLevel(testLevel, testAmount, testType, testReceivedTickCount);
			const CProgressiveLevelForward &progressiveData(varProgressiveData);

			BYTE sendBufferExpected[] = { 0x01, 0x86, 0xee, testGroupId, testLevel, 0x00, 0x00, 0x00, 0x04, 0x56, 0xcc, 0xcc };
			CUnitTestingUtils::SetSasLengthAndCrc(sendBufferExpected, sizeof(sendBufferExpected));

			const byte gameAddress = 1;
			const DWORD tickCount = testReceivedTickCount;

			// Perform operation(s) and test(s).
			CLongPoll86Message msg(gameAddress, progressiveData);
			msg.Fill();
			msg.Fill(tickCount);
			AssertResult4(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, sizeof(sendBufferExpected));
			AssertResult2(msg.GetResponseBufferSize(), m_responseBufferSizeDefaultExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll86Message::CLongPoll86Message()
		///     CLongPoll86Message::Fill(DWORD tickCount)
		///     CLongPoll86Message::GetSendBuffer()
		///     CLongPoll86Message::GetSendBufferSize()
		///     CLongPoll86Message::GetResponseBufferSize()
		/// Test with one progressive level and amount within maximum allowed delay.
		/// </summary>
		TEST_F(LongPoll86Message, CLongPoll86Message_Add1MaxReceivedTest)
		{
			// Data.
			const DWORD testReceivedTickCount(10000);
			const BYTE testGroupId(0xab);
			const BYTE testLevel(3);

			const __int64 testAmount(456);
			const ProgressiveLevelType::Type  testType(ProgressiveLevelType::Standard);
			CProgressiveLevelForward varProgressiveData(testGroupId, false);
			varProgressiveData.AddLevel(testLevel, testAmount, testType, testReceivedTickCount);
			const CProgressiveLevelForward &progressiveData(varProgressiveData);

			BYTE sendBufferExpected[] = { 0x01, 0x86, 0xee, testGroupId, testLevel, 0x00, 0x00, 0x00, 0x04, 0x56, 0xcc, 0xcc };
			CUnitTestingUtils::SetSasLengthAndCrc(sendBufferExpected, sizeof(sendBufferExpected));

			const byte gameAddress = 1;
			const DWORD tickCount = testReceivedTickCount + m_maxReceivedDelayTickCountExpected;

			// Perform operation(s) and test(s).
			CLongPoll86Message msg(gameAddress, progressiveData);
			msg.Fill();
			msg.Fill(tickCount);
			AssertResult4(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, sizeof(sendBufferExpected));
			AssertResult2(msg.GetResponseBufferSize(), m_responseBufferSizeDefaultExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll86Message::CLongPoll86Message()
		///     CLongPoll86Message::Fill(DWORD tickCount)
		///     CLongPoll86Message::GetSendBuffer()
		///     CLongPoll86Message::GetSendBufferSize()
		///     CLongPoll86Message::GetResponseBufferSize()
		/// Test with one progressive level and amount, but not within maximum allowed delay.
		/// </summary>
		TEST_F(LongPoll86Message, CLongPoll86Message_Add1MaxReceived1Test)
		{
			// Data.
			const DWORD testReceivedTickCount(10000);
			const BYTE testGroupId(0xab);
			const BYTE testLevel(3);

			const __int64 testAmount(456);
			const ProgressiveLevelType::Type  testType(ProgressiveLevelType::Standard);
			CProgressiveLevelForward varProgressiveData(testGroupId, false);
			varProgressiveData.AddLevel(testLevel, testAmount, testType, testReceivedTickCount);
			const CProgressiveLevelForward &progressiveData(varProgressiveData);

			BYTE *sendBufferExpected(NULL);

			const byte gameAddress = 1;
			const DWORD tickCount = testReceivedTickCount + m_maxReceivedDelayTickCountExpected + 1;

			// Perform operation(s) and test(s).
			CLongPoll86Message msg(gameAddress, progressiveData);
			msg.Fill();
			msg.Fill(tickCount);
			AssertResult4(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, m_sendBufferSizeDefaultExpected);
			AssertResult2(msg.GetResponseBufferSize(), m_responseBufferSizeDefaultExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll86Message::CLongPoll86Message()
		///     CLongPoll86Message::Fill(DWORD tickCount)
		///     CLongPoll86Message::GetSendBuffer()
		///     CLongPoll86Message::GetSendBufferSize()
		///     CLongPoll86Message::GetResponseBufferSize()
		/// Test with one progressive level and amount.
		/// </summary>
		TEST_F(LongPoll86Message, CLongPoll86Message_Add2Test)
		{
			// Data.
			const DWORD testReceivedTickCount(10000);
			const BYTE testGroupId(0xab);
			const BYTE testLevel1(3);
			const BYTE testLevel2(4);

			const __int64 testAmount1(456);
			const __int64 testAmount2(1234567890);
			const ProgressiveLevelType::Type  testType(ProgressiveLevelType::Standard);
			CProgressiveLevelForward varProgressiveData(testGroupId, false);
			varProgressiveData.AddLevel(testLevel1, testAmount1, testType, testReceivedTickCount);
			varProgressiveData.AddLevel(testLevel2, testAmount2, testType, testReceivedTickCount);
			const CProgressiveLevelForward &progressiveData(varProgressiveData);

			BYTE sendBufferExpected[] = { 0x01, 0x86, 0xee, testGroupId,
				testLevel1, 0x00, 0x00, 0x00, 0x04, 0x56,
				testLevel2, 0x12, 0x34, 0x56, 0x78, 0x90,
				0xcc, 0xcc };
			CUnitTestingUtils::SetSasLengthAndCrc(sendBufferExpected, sizeof(sendBufferExpected));

			const byte gameAddress = 1;
			const DWORD tickCount = testReceivedTickCount;

			// Perform operation(s) and test(s).
			CLongPoll86Message msg(gameAddress, progressiveData);
			msg.Fill();
			msg.Fill(tickCount);
			AssertResult4(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, sizeof(sendBufferExpected));
			AssertResult2(msg.GetResponseBufferSize(), m_responseBufferSizeDefaultExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll86Message::CLongPoll86Message()
		///     CLongPoll86Message::ResponseReceived()
		///     CLongPoll86Message::GetReturnValue()
		/// Test with typical default state.
		/// </summary>
		TEST_F(LongPoll86Message, ResponseReceived_DefaultTest)
		{
			// Data.
			const byte gameAddress(1);
			const DWORD tickCount(0);
			const int returnValueExpected(SASPOLL_NACK);

			// Setup.
			const CProgressiveLevelForward progressiveData(0, false);
			CLongPoll86Message msg(gameAddress, progressiveData);
			msg.Fill();
			msg.Fill(tickCount);

			// Perform operation(s) and test(s).
			msg.ResponseReceived();
			AssertResult2(msg.GetReturnValue(), returnValueExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll86Message::CLongPoll86Message()
		///     CLongPoll86Message::ResponseReceived()
		///     CLongPoll86Message::GetReturnValue()
		/// Test with broadcast game address and default response.
		/// </summary>
		TEST_F(LongPoll86Message, ResponseReceived_BroadcastDefaultTest)
		{
			// Data.
			const byte gameAddress(0);
			const DWORD tickCount(0);
			const int returnValueExpected(SASPOLL_SUCCESS);

			// Setup.
			const CProgressiveLevelForward progressiveData(0, false);
			CLongPoll86Message msg(gameAddress, progressiveData);
			msg.Fill();
			msg.Fill(tickCount);

			// Perform operation(s) and test(s).
			msg.ResponseReceived();
			AssertResult2(msg.GetReturnValue(), returnValueExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll86Message::CLongPoll86Message()
		///     CLongPoll86Message::ResponseReceived()
		///     CLongPoll86Message::GetReturnValue()
		/// Test with broadcast game address and no response.
		/// </summary>
		TEST_F(LongPoll86Message, ResponseReceived_BroadcastNoResponseTest)
		{
			// Data.
			const byte gameAddress(0);
			const DWORD tickCount(0);
			const int returnValueExpected(SASPOLL_SUCCESS);

			// Setup.
			const CProgressiveLevelForward progressiveData(0, false);
			CLongPoll86Message msg(gameAddress, progressiveData);
			msg.Fill();
			msg.Fill(tickCount);
			msg.SetResponseBuffer(msg.GetResponseBuffer(), 0);
			msg.SetReturnValue(SASPOLL_SUCCESS);

			// Perform operation(s) and test(s).
			msg.ResponseReceived();
			AssertResult2(msg.GetReturnValue(), returnValueExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll86Message::CLongPoll86Message()
		///     CLongPoll86Message::ResponseReceived()
		///     CLongPoll86Message::GetReturnValue()
		/// Test typical response with incomplete data.
		/// </summary>
		TEST_F(LongPoll86Message, ResponseReceived_IncompleteDataTest)
		{
			// Data.
			const byte gameAddress(1);
			const DWORD tickCount(0);
			const int returnValueExpected(SASPOLL_INCOMPLETE_DATA);

			// Setup.
			const CProgressiveLevelForward progressiveData(0, false);
			CLongPoll86Message msg(gameAddress, progressiveData);
			msg.Fill();
			msg.Fill(tickCount);
			msg.SetResponseBuffer(msg.GetResponseBuffer(), 0);
			msg.SetReturnValue(SASPOLL_INCOMPLETE_DATA);

			// Perform operation(s) and test(s).
			msg.ResponseReceived();
			AssertResult2(msg.GetReturnValue(), returnValueExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll86Message::CLongPoll86Message()
		///     CLongPoll86Message::ResponseReceived()
		///     CLongPoll86Message::GetReturnValue()
		/// Test with game address and no response.
		/// </summary>
		TEST_F(LongPoll86Message, ResponseReceived_NoResponseTest)
		{
			// Data.
			const byte gameAddress(1);
			const DWORD tickCount(0);
			const int returnValueExpected(SASPOLL_INCOMPLETE_DATA);

			// Setup.
			const CProgressiveLevelForward progressiveData(0, false);
			CLongPoll86Message msg(gameAddress, progressiveData);
			msg.Fill();
			msg.Fill(tickCount);
			msg.SetResponseBuffer(msg.GetResponseBuffer(), 0);
			msg.SetReturnValue(SASPOLL_SUCCESS);

			// Perform operation(s) and test(s).
			msg.ResponseReceived();
			AssertResult2(msg.GetReturnValue(), returnValueExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll86Message::CLongPoll86Message()
		///     CLongPoll86Message::ResponseReceived()
		///     CLongPoll86Message::GetReturnValue()
		/// Test NACK response return value.
		/// </summary>
		TEST_F(LongPoll86Message, ResponseReceived_NackResponseTest)
		{
			// Data.
			const byte gameAddress(1);
			const DWORD tickCount(0);
			const int returnValueExpected(SASPOLL_NACK);

			// Setup.
			const CProgressiveLevelForward progressiveData(0, false);
			CLongPoll86Message msg(gameAddress, progressiveData);
			msg.Fill();
			msg.Fill(tickCount);
			AssertResult2(msg.GetResponseBufferSize() >= 1, true);
			*msg.GetResponseBuffer() = (BYTE)(gameAddress | m_nackOrValue);
			msg.SetReturnValue(SASPOLL_NACK);

			// Perform operation(s) and test(s).
			msg.ResponseReceived();
			AssertResult2(msg.GetReturnValue(), returnValueExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll86Message::CLongPoll86Message()
		///     CLongPoll86Message::ResponseReceived()
		///     CLongPoll86Message::GetReturnValue()
		/// Test successful response with NACK data.
		/// </summary>
		TEST_F(LongPoll86Message, ResponseReceived_NackDataTest)
		{
			// Data.
			const byte gameAddress(1);
			const DWORD tickCount(0);
			const int returnValueExpected(SASPOLL_NACK);

			// Setup.
			const CProgressiveLevelForward progressiveData(0, false);
			CLongPoll86Message msg(gameAddress, progressiveData);
			msg.Fill();
			msg.Fill(tickCount);
			AssertResult2(msg.GetResponseBufferSize() >= 1, true);
			*msg.GetResponseBuffer() = (BYTE)(gameAddress | m_nackOrValue);
			msg.SetReturnValue(SASPOLL_SUCCESS);

			// Perform operation(s) and test(s).
			msg.ResponseReceived();
			AssertResult2(msg.GetReturnValue(), returnValueExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll86Message::CLongPoll86Message()
		///     CLongPoll86Message::ResponseReceived()
		///     CLongPoll86Message::GetReturnValue()
		/// Test successful response with ACK data.
		/// </summary>
		TEST_F(LongPoll86Message, ResponseReceived_AckTest)
		{
			// Data.
			const byte gameAddress(1);
			const DWORD tickCount(0);
			const int returnValueExpected(SASPOLL_SUCCESS);

			// Setup.
			const CProgressiveLevelForward progressiveData(0, false);
			CLongPoll86Message msg(gameAddress, progressiveData);
			msg.Fill();
			msg.Fill(tickCount);
			AssertResult2(msg.GetResponseBufferSize() >= 1, true);
			*msg.GetResponseBuffer() = (BYTE)(gameAddress);
			msg.SetReturnValue(SASPOLL_SUCCESS);

			// Perform operation(s) and test(s).
			msg.ResponseReceived();
			AssertResult2(msg.GetReturnValue(), returnValueExpected);
		}
}
