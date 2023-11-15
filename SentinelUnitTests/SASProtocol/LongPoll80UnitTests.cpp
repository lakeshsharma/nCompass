#include "stdafx.h"
#include "SASProtocol/LongPoll80Message.h"

#include "SASProtocol/SASLimits.h"

// Unit testing includes.
#include "UnitTestingUtils.h"
#include "gtest/gtest.h"
//#include "UnitTestingXmlData.h"
//#include "UnitTestingXmlLite.h"
//
// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

#define AssertResult5(result, resultLength, expected, expectedLength, resultName) \
	CUnitTestingUtils::AssertEqual(result, resultLength, expected, expectedLength, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult4(result, resultLength, expected, expectedLength) AssertResult5(result, resultLength, expected, expectedLength, _T(#result))

// AssertResult macro(s) for calling CUnitTestingUtils from a method with a "testValue".
#define AssertResult2t(result, expected) AssertResult3(result, expected, string("for testValue=").c_str() + to_string(testValue).c_str() + _T(" ") + _T(#result))
#define AssertResult4t(result, resultLength, expected, expectedLength) \
	AssertResult5(result, resultLength, expected, expectedLength, (string("for testValue=") + to_string(testValue) + string(" ") + #result).c_str())

// AssertResult macros for calling CUnitTestingUtils from a class initialization method.
#define AssertResult3i(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, 0, __TFUNCTION__, __LINE__);
#define AssertResult2i(result, expected) AssertResult3i(result, expected, _T(#result))

using namespace std;

namespace SentinelUnitTests
{
// #pragma region ClassLevel
	// Expected value(s).
	static const BYTE m_pollValueExpected = 0x80;
	static const DWORD m_maxReceivedDelayTickCountExpected = 6000;
	static const int m_minLevelExpected = 1;
	static const int m_maxLevelExpected = 32;
	static const int m_minGroupExpected = 1;
	static const int m_maxGroupExpected = 255;
	static const int m_levelCountExpected = 1;

	static const UINT m_sendBufferSizeDefaultExpected = 0;
	static const UINT m_responseBufferSizeDefaultExpected = 1;
	static const bool m_swapPollValueExpected = false;
	static const bool m_isGeneralPollExpected = false;
	static const bool m_fillWithTickCountBeforeSendingExpected = true;
	static const int m_returnValueDefaultExpected = SASPOLL_SUCCESS;

	static const BYTE m_nackOrValue = 0x80;
	class LongPoll80Message : public ::testing::Test
	{
	public: 

		// Class member variable(s).
		int m_testIndex;

		/// <summary>
		/// Test static class members.
		/// </summary>
		void TestStaticMembers()
		{
			AssertResult2i(CLongPoll80Message::POLL_VALUE, m_pollValueExpected);
			AssertResult2i(CProgressiveLevelForward::MaxInterval, m_maxReceivedDelayTickCountExpected);
			AssertResult2i(CSASLimits::MIN_SAS_PROGRESSIVE_LEVEL, m_minLevelExpected);
			AssertResult2i(CSASLimits::MAX_SAS_PROGRESSIVE_LEVEL, m_maxLevelExpected);
			AssertResult2i(CSASLimits::MIN_SAS_PROGRESSIVE_GROUP, m_minGroupExpected);
			AssertResult2i(CSASLimits::MAX_SAS_PROGRESSIVE_GROUP, m_maxGroupExpected);
			AssertResult2i(CLongPoll80Message::LEVEL_COUNT, m_levelCountExpected);

			AssertResult2i(CSASPollMessage::GAME_ADDRESS_NACK_OR_VALUE, m_nackOrValue);
		}

		/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		virtual void SetUp()
		{
			// Test method initialization code.
			m_testIndex = 0;

			// Perform static tests here so each test method gets a useful error message.
			TestStaticMembers();
		}

		/// <summary>
		/// Cleanup class member(s) before each test method is run.
		/// </summary>
		virtual void TearDown()
		{
			// Test method cleanup code.
		}

	};
	// #pragma endregion ClassLevel

	/// <summary>
	/// Code coverage and functional test for:
	///     CLongPoll80Message::CLongPoll80Message()
	/// Code coverage:
	///     CLongPoll80Message::~CLongPoll80Message()
	/// Test for correct default values.
	/// </summary>
	TEST_F(LongPoll80Message, CLongPoll80Message_DefaultTest)
	{
		// Data.
		const byte gameAddress = 1;
		const int testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testReceivedTickCount);

		// Perform operation(s) and test(s).
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
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
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::Fill(DWORD tickCount)
	///     CLongPoll80Message::GetSendBuffer()
	///     CLongPoll80Message::GetSendBufferSize()
	///     CLongPoll80Message::GetResponseBufferSize()
	/// Test with good progressive info.
	/// </summary>
	TEST_F(LongPoll80Message, CLongPoll80Message_FillTest)
	{
		// Data.
		const byte gameAddress = 1;
		const int testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);
		const DWORD tickCount = testReceivedTickCount;

		BYTE sendBufferExpected[] = { 0x01, 0x80, testGroupId, testLevel, 0x00, 0x00, 0x00, 0x04, 0x56, 0xcc, 0xcc };
		CUnitTestingUtils::SetSasCrc(sendBufferExpected, sizeof(sendBufferExpected));

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testReceivedTickCount);

		// Perform operation(s) and test(s).
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
		msg.Fill();
		msg.Fill(tickCount);

		AssertResult4(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, sizeof(sendBufferExpected));
		AssertResult2(msg.GetResponseBufferSize(), m_responseBufferSizeDefaultExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::Fill(DWORD tickCount)
	///     CLongPoll80Message::GetSendBuffer()
	///     CLongPoll80Message::GetSendBufferSize()
	///     CLongPoll80Message::GetResponseBufferSize()
	/// Test with good progressive info, where amount is too big to fit in the message.
	/// </summary>
	TEST_F(LongPoll80Message, CLongPoll80Message_FillAmoutTooBigTest)
	{
		// Data.
		const byte gameAddress = 1;
		const int testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(10000000001); // Also test that leading digits are not truncated, by using non-zero trailing digits.
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);
		const DWORD tickCount = testReceivedTickCount;

		BYTE sendBufferExpected[] = { 0x01, 0x80, testGroupId, testLevel, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcc };
		CUnitTestingUtils::SetSasCrc(sendBufferExpected, sizeof(sendBufferExpected));

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testReceivedTickCount);

		// Perform operation(s) and test(s).
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
		msg.Fill();
		msg.Fill(tickCount);

		AssertResult4(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, sizeof(sendBufferExpected));
		AssertResult2(msg.GetResponseBufferSize(), m_responseBufferSizeDefaultExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::Fill(DWORD tickCount)
	///     CLongPoll80Message::GetSendBuffer()
	///     CLongPoll80Message::GetSendBufferSize()
	///     CLongPoll80Message::GetResponseBufferSize()
	/// Test with good progressive info, where amount is too small to fit in the message.
	/// </summary>
	TEST_F(LongPoll80Message, CLongPoll80Message_FillAmoutTooSmallTest)
	{
		// Data.
		const byte gameAddress = 1;
		const int testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(-1);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);
		const DWORD tickCount = testReceivedTickCount;

		BYTE sendBufferExpected[] = { 0x01, 0x80, testGroupId, testLevel, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcc };
		CUnitTestingUtils::SetSasCrc(sendBufferExpected, sizeof(sendBufferExpected));

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testReceivedTickCount);

		// Perform operation(s) and test(s).
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
		msg.Fill();
		msg.Fill(tickCount);

		AssertResult4(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, sizeof(sendBufferExpected));
		AssertResult2(msg.GetResponseBufferSize(), m_responseBufferSizeDefaultExpected);
	}

	/// <summary>
	/// Test:
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::Fill(DWORD tickCount)
	///     CLongPoll80Message::GetSendBuffer()
	///     CLongPoll80Message::GetSendBufferSize()
	///     CLongPoll80Message::GetResponseBufferSize()
	/// Test various progressiveGroup values and otherwise good progressive info.
	/// </summary>
	TEST_F(LongPoll80Message, CLongPoll80Message_FillGroupTest)
	{
		// Data.
		const byte gameAddress = 1;
		const BYTE testLevel(3);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);
		const DWORD tickCount = testReceivedTickCount;

		const int minValidValue = 1;
		const int maxValidValue = 0xff;
		BYTE testValue = 0x0a;

		// Setup, valid group id.
		BYTE sendBufferExpected[] = { 0x01, 0x80, testValue, 0x03, 0x00, 0x00, 0x00, 0x04, 0x56, 0xcc, 0xcc };
		CUnitTestingUtils::SetSasCrc(sendBufferExpected, sizeof(sendBufferExpected));
		int sendBufferLenExpected = minValidValue <= testValue && testValue <= maxValidValue ? sizeof(sendBufferExpected) : 0;

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testReceivedTickCount);

		// Perform operation(s) and test(s).
		CLongPoll80Message msg(gameAddress, testValue, levelRecord);
		msg.Fill();
		msg.Fill(tickCount);

		ASSERT_EQ(msg.GetSendBufferSize(), sendBufferLenExpected);
		ASSERT_EQ(msg.GetResponseBufferSize(), m_responseBufferSizeDefaultExpected);

		// Setup invalid group id
		testValue = 0;
		sendBufferExpected[2] = testValue;
		sendBufferLenExpected = minValidValue <= testValue && testValue <= maxValidValue ? sizeof(sendBufferExpected) : 0;
		CLongPoll80Message msg1(gameAddress, testValue, levelRecord);
		msg1.Fill();
		msg1.Fill(tickCount);

		ASSERT_EQ(msg1.GetSendBufferSize(), sendBufferLenExpected);
		ASSERT_EQ(msg1.GetResponseBufferSize(), 1);
	}

	/// <summary>
	/// Test:
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::Fill(DWORD tickCount)
	///     CLongPoll80Message::GetSendBuffer()
	///     CLongPoll80Message::GetSendBufferSize()
	///     CLongPoll80Message::GetResponseBufferSize()
	/// Test various progressiveLevel values and otherwise good progressive info.
	/// </summary>
	TEST_F(LongPoll80Message, CLongPoll80Message_FillLevelTest)
	{
		// Data.
		const byte gameAddress = 1;
		const BYTE testGroupId(0xab);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);
		const DWORD tickCount = testReceivedTickCount;

		typedef BYTE TestDataType;
		const TestDataType minValidValue(1);
		const TestDataType maxValidValue(20);
		BYTE testValue = 0x0a;


		// Setup valid level.
		BYTE sendBufferExpected[] = { 0x01, 0x80, 0x01, testValue, 0x00, 0x00, 0x00, 0x04, 0x56, 0xcc, 0xcc };
		CUnitTestingUtils::SetSasCrc(sendBufferExpected, sizeof(sendBufferExpected));
		auto sendBufferLenExpected = minValidValue <= testValue && testValue <= maxValidValue ? sizeof(sendBufferExpected) : 0;

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testValue, testAmount, testType, testReceivedTickCount);

		// Perform operation(s) and test(s).
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
		msg.Fill();
		msg.Fill(tickCount);

		ASSERT_EQ(msg.GetSendBufferSize(), sendBufferLenExpected);
		ASSERT_EQ(msg.GetResponseBufferSize(), m_responseBufferSizeDefaultExpected);

		// Setup invalid level
		testValue = 35;
		sendBufferExpected[3] = testValue;
		sendBufferLenExpected = minValidValue <= testValue && testValue <= maxValidValue ? sizeof(sendBufferExpected) : 0;
		const CProgressiveLevelForward::Level levelRecord1(testValue, testAmount, testType, testReceivedTickCount);
		CLongPoll80Message msg1(gameAddress, testGroupId, levelRecord1);
		msg1.Fill();
		msg1.Fill(tickCount);

		ASSERT_EQ(msg1.GetSendBufferSize(), sendBufferLenExpected);
		ASSERT_EQ(msg1.GetResponseBufferSize(), 1);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::Fill(DWORD tickCount)
	///     CLongPoll80Message::GetSendBuffer()
	///     CLongPoll80Message::GetSendBufferSize()
	///     CLongPoll80Message::GetResponseBufferSize()
	/// Test with one progressive level and amount within maximum allowed delay.
	/// </summary>
	TEST_F(LongPoll80Message, CLongPoll80Message_FillMaxReceivedTest)
	{
		// Data.
		const byte gameAddress = 1;
		const BYTE testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);
		const DWORD tickCount = testReceivedTickCount + m_maxReceivedDelayTickCountExpected;

		BYTE sendBufferExpected[] = { 0x01, 0x80, testGroupId, testLevel, 0x00, 0x00, 0x00, 0x04, 0x56, 0xcc, 0xcc };
		CUnitTestingUtils::SetSasCrc(sendBufferExpected, sizeof(sendBufferExpected));

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testReceivedTickCount);

		// Perform operation(s) and test(s).
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
		msg.Fill();
		msg.Fill(tickCount);

		AssertResult4(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, sizeof(sendBufferExpected));
		AssertResult2(msg.GetResponseBufferSize(), m_responseBufferSizeDefaultExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::Fill(DWORD tickCount)
	///     CLongPoll80Message::GetSendBuffer()
	///     CLongPoll80Message::GetSendBufferSize()
	///     CLongPoll80Message::GetResponseBufferSize()
	/// Test with one progressive level and amount, but not within maximum allowed delay.
	/// </summary>
	TEST_F(LongPoll80Message, CLongPoll80Message_FillMaxReceivedPlus1Test)
	{
		// Data.
		const byte gameAddress = 1;
		const BYTE testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);
		const DWORD tickCount = testReceivedTickCount + m_maxReceivedDelayTickCountExpected + 1;

		BYTE *sendBufferExpected(NULL);

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testReceivedTickCount);

		// Perform operation(s) and test(s).
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
		msg.Fill();
		msg.Fill(tickCount);

		AssertResult4(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, m_sendBufferSizeDefaultExpected);
		AssertResult2(msg.GetResponseBufferSize(), m_responseBufferSizeDefaultExpected);
	}

	/// <summary>
	/// Test:
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::Fill(DWORD tickCount)
	///     CLongPoll80Message::GetSendBuffer()
	///     CLongPoll80Message::GetSendBufferSize()
	/// Test various receivedTickCount values and otherwise good progressive info.
	/// </summary>
	TEST_F(LongPoll80Message, CLongPoll80Message_FillReceivedTickCountTest)
	{
		// Data.
		const byte gameAddress = 1;
		const BYTE testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);

		BYTE sendBufferExpected[] = { 0x01, 0x80, testGroupId, testLevel, 0x00, 0x00, 0x00, 0x04, 0x56, 0xcc, 0xcc };
		CUnitTestingUtils::SetSasCrc(sendBufferExpected, sizeof(sendBufferExpected));

		typedef DWORD TestDataType;
		const TestDataType maxValidElapsedTickCount(m_maxReceivedDelayTickCountExpected);
		TestDataType testValue = 1000;

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testValue);

		// Perform operation(s) and test(s).
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
		msg.Fill();

		msg.Fill(testValue - 1);
		AssertResult4t(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, 0);

		msg.Fill(testValue + 0);
		AssertResult4t(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, sizeof(sendBufferExpected));

		msg.Fill(testValue + 1);
		AssertResult4t(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, sizeof(sendBufferExpected));

		msg.Fill(testValue + maxValidElapsedTickCount - 1);
		AssertResult4t(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, sizeof(sendBufferExpected));

		msg.Fill(testValue + maxValidElapsedTickCount + 0);
		AssertResult4t(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, sizeof(sendBufferExpected));

		msg.Fill(testValue + maxValidElapsedTickCount + 1);
		AssertResult4t(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, 0);

	}

	/// <summary>
	/// Test:
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::Fill(DWORD tickCount)
	///     CLongPoll80Message::GetSendBuffer()
	///     CLongPoll80Message::GetSendBufferSize()
	/// Test various (current) tickCount values and otherwise good progressive info.
	/// </summary>
	TEST_F(LongPoll80Message, CLongPoll80Message_FillCurrentTickCountTest)
	{
		// Data.
		const byte gameAddress = 1;
		const BYTE testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);

		BYTE sendBufferExpected[] = { 0x01, 0x80, testGroupId, testLevel, 0x00, 0x00, 0x00, 0x04, 0x56, 0xcc, 0xcc };
		CUnitTestingUtils::SetSasCrc(sendBufferExpected, sizeof(sendBufferExpected));

		typedef DWORD TestDataType;
		const TestDataType maxValidElapsedTickCount(m_maxReceivedDelayTickCountExpected);

		TestDataType testValue = 1000;

		// Perform setup, operation(s) and test(s).
		{
			const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, (testValue - maxValidElapsedTickCount) - 1);
			CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
			msg.Fill();
			msg.Fill(testValue);
			AssertResult4t(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, 0);
		}

		{
			const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, (testValue - maxValidElapsedTickCount) + 0);
			CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
			msg.Fill();
			msg.Fill(testValue);
			AssertResult4t(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, sizeof(sendBufferExpected));
		}

		{
			const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, (testValue - maxValidElapsedTickCount) + 1);
			CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
			msg.Fill();
			msg.Fill(testValue);
			AssertResult4t(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, sizeof(sendBufferExpected));
		}

		{
			const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testValue - 1);
			CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
			msg.Fill();
			msg.Fill(testValue);
			AssertResult4t(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, sizeof(sendBufferExpected));
		}

		{
			const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testValue + 0);
			CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
			msg.Fill();
			msg.Fill(testValue);
			AssertResult4t(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, sizeof(sendBufferExpected));
		}

		{
			const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testValue + 1);
			CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
			msg.Fill();
			msg.Fill(testValue);
			AssertResult4t(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, 0);
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::ResponseReceived()
	///     CLongPoll80Message::GetReturnValue()
	/// Test with typical default state.
	/// </summary>
	TEST_F(LongPoll80Message, ResponseReceived_DefaultTest)
	{
		// Data.
		const byte gameAddress = 1;
		const int testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);

		const int returnValueExpected(SASPOLL_NACK);

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testReceivedTickCount);
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
		msg.Fill();

		// Perform operation(s) and test(s).
		msg.ResponseReceived();
		AssertResult2(msg.GetReturnValue(), returnValueExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::ResponseReceived()
	///     CLongPoll80Message::GetReturnValue()
	/// Test with broadcast game address and default response.
	/// </summary>
	TEST_F(LongPoll80Message, ResponseReceived_BroadcastDefaultTest)
	{
		// Data.
		const byte gameAddress(0);
		const int testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);

		const int returnValueExpected(SASPOLL_SUCCESS);

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testReceivedTickCount);
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
		msg.Fill();

		// Perform operation(s) and test(s).
		msg.ResponseReceived();
		AssertResult2(msg.GetReturnValue(), returnValueExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::ResponseReceived()
	///     CLongPoll80Message::GetReturnValue()
	/// Test with broadcast game address and no response.
	/// </summary>
	TEST_F(LongPoll80Message, ResponseReceived_BroadcastNoResponseTest)
	{
		// Data.
		const byte gameAddress(0);
		const int testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);
		const DWORD tickCount = testReceivedTickCount;

		const int returnValueExpected(SASPOLL_SUCCESS);

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testReceivedTickCount);
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
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
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::ResponseReceived()
	///     CLongPoll80Message::GetReturnValue()
	/// Test typical response with incomplete data.
	/// </summary>
	TEST_F(LongPoll80Message, ResponseReceived_IncompleteDataTest)
	{
		// Data.
		const byte gameAddress(1);
		const int testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);
		const DWORD tickCount = testReceivedTickCount;

		const int returnValueExpected(SASPOLL_INCOMPLETE_DATA);

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testReceivedTickCount);
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
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
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::ResponseReceived()
	///     CLongPoll80Message::GetReturnValue()
	/// Test with game address and no response.
	/// </summary>
	TEST_F(LongPoll80Message, ResponseReceived_NoResponseTest)
	{
		// Data.
		const byte gameAddress(1);
		const int testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);
		const DWORD tickCount = testReceivedTickCount;

		const int returnValueExpected(SASPOLL_INCOMPLETE_DATA);

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testReceivedTickCount);
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
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
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::ResponseReceived()
	///     CLongPoll80Message::GetReturnValue()
	/// Test NACK response return value.
	/// </summary>
	TEST_F(LongPoll80Message, ResponseReceived_NackResponseTest)
	{
		// Data.
		const byte gameAddress(1);
		const int testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);
		const DWORD tickCount = testReceivedTickCount;

		const int returnValueExpected(SASPOLL_NACK);

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testReceivedTickCount);
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
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
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::ResponseReceived()
	///     CLongPoll80Message::GetReturnValue()
	/// Test successful response with NACK data.
	/// </summary>
	TEST_F(LongPoll80Message, ResponseReceived_NackDataTest)
	{
		// Data.
		const byte gameAddress(1);
		const int testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);
		const DWORD tickCount = testReceivedTickCount;

		const int returnValueExpected(SASPOLL_NACK);

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testReceivedTickCount);
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
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
	///     CLongPoll80Message::CLongPoll80Message()
	///     CLongPoll80Message::ResponseReceived()
	///     CLongPoll80Message::GetReturnValue()
	/// Test successful response with ACK data.
	/// </summary>
	TEST_F(LongPoll80Message, ResponseReceived_AckTest)
	{
		// Data.
		const byte gameAddress(1);
		const int testGroupId(0xab);
		const BYTE testLevel(3);
		const __int64 testAmount(456);
		const ProgressiveLevelType::Type testType(ProgressiveLevelType::Standard);
		const DWORD testReceivedTickCount(10000);
		const DWORD tickCount = testReceivedTickCount;

		const int returnValueExpected(SASPOLL_SUCCESS);

		// Setup.
		const CProgressiveLevelForward::Level levelRecord(testLevel, testAmount, testType, testReceivedTickCount);
		CLongPoll80Message msg(gameAddress, testGroupId, levelRecord);
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
