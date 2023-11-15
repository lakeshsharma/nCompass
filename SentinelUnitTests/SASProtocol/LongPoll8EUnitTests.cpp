#include "stdafx.h"
#include "SASProtocol/LongPoll8EMessage.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "UnitTestingUtils.h"

using namespace std;

	class LongPoll8EMessageTests : public ::testing::Test
	{
	public:

// #pragma region ClassLevel
		// Expected value(s).
		static const BYTE m_pollValueExpected;
		static const UINT m_sendBufferSizeExpected;
		static const UINT m_responseBufferSizeExpected;
		static const UINT m_handTypeLocationExpected;
		static const UINT m_cardLocationExpected; // Location of the first card.

		static const bool m_swapPollValueExpected;
		static const bool m_isGeneralPollExpected;
		static const bool m_fillWithTickCountBeforeSendingExpected;
		static const int m_returnValueDefaultExpected;

		/// <summary>
		/// Test static class members.
		/// </summary>
		void TestStaticMembers()
		{
			ASSERT_EQ(CLongPoll8EMessage::COMMAND, m_pollValueExpected);
			ASSERT_EQ(CLongPoll8EMessage::SENDBUFFERLENGTH, m_sendBufferSizeExpected);
			ASSERT_EQ(CLongPoll8EMessage::RECBUFFERLENGTH, m_responseBufferSizeExpected);
			ASSERT_EQ(CLongPoll8EMessage::HAND_TYPE_LOCATION, m_handTypeLocationExpected);
			ASSERT_EQ(CLongPoll8EMessage::CARDLOC, m_cardLocationExpected);
		}

		/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		void SetUp()
		{
			// Perform static tests here so each test method gets a useful error message.
			TestStaticMembers();
		}

		/// <summary>
		/// Cleanup class member(s) before each test method is run.
		/// </summary>
		void TearDown()
		{
			// Test method cleanup code.
		}

		/// <summary>
		/// Test a function that takes a "BYTE" parameter with various values.
		/// </summary>
		typedef void(&TestFuncByteType)(byte testValue);
		void Test(TestFuncByteType TestFunc, BYTE extraThresholdA = 0, BYTE extraThresholdB = 0)
		{
			if (extraThresholdA != 0)
			{
				// Add extra threshold-1, threshold+0, threshold+1 values.
				for (int index = -1; index <= 1; ++index)
				{
					TestFunc((BYTE)(extraThresholdA + index));
				}
			}

			if (extraThresholdB != 0)
			{
				// Add extra threshold-1, threshold+0, threshold+1 values.
				for (int index = -1; index <= 1; ++index)
				{
					TestFunc((BYTE)(extraThresholdB + index));
				}
			}

			// Smaller values.
			TestFunc(0);
			TestFunc(1);
			TestFunc(123);

			// Signed data thresholds.
			TestFunc(126); // SCHAR_MAX - 1.
			TestFunc(127); // SCHAR_MAX.
			TestFunc(128); // SCHAR_MIN.
			TestFunc(129); // SCHAR_MIN + 1.

			// Larger values.
			TestFunc(210);
			TestFunc(254); // UCHAR_MAX - 1.
			TestFunc(255); // UCHAR_MAX.
		}

		/// <summary>
		/// Test a function that takes an "int" parameter with various values.
		/// </summary>
		typedef void(&TestFuncIntType)(int testValue);
		void Test(TestFuncIntType TestFunc, int extraThresholdA = 0, int extraThresholdB = 0)
		{
			if (extraThresholdA != 0)
			{
				// Add extra threshold-1, threshold+0, threshold+1 values.
				for (int index = -1; index <= 1; ++index)
				{
					TestFunc(extraThresholdA + index);
				}
			}

			if (extraThresholdB != 0)
			{
				// Add extra threshold-1, threshold+0, threshold+1 values.
				for (int index = -1; index <= 1; ++index)
				{
					TestFunc(extraThresholdB + index);
				}
			}

			// Smaller values.
			TestFunc(0);
			TestFunc(1);
			TestFunc(2);
			TestFunc(1234567890);

			// Signed data thresholds.
			TestFunc(2147483645); // INT_MAX - 2.
			TestFunc(2147483646); // INT_MAX - 1.
			TestFunc(2147483647); // INT_MAX.
			TestFunc(2147483648); // INT_MIN.
			TestFunc(2147483649); // INT_MIN + 1.
			TestFunc(2147483650); // INT_MIN + 2.

			// Smaller negative values.
			TestFunc(-1234567890);
			TestFunc(-3); // DWORD_MAX - 2.
			TestFunc(-2); // DWORD_MAX - 1.
			TestFunc(-1); // DWORD_MAX.
		}

		void CLongPoll80Message_GameAddressX(byte testValue);
		static void CLongPoll80Message_GameAddress(byte testValue);
		static void CLongPoll80Message_PokerHandType(int testData);
		static void Fill_GameAddress(byte testValue);
		static void ResponseReceived_DealtHandFalse(int testData);
		static void ResponseReceived_PokerHandType(int testData);
// #pragma endregion ClassLevel
	};

		/// <summary>
		/// Functional test for:
		///     CLongPoll8EMessage::CLongPoll8EMessage()
		///     CLongPoll8EMessage::GetSendBufferSize()
		///     CLongPoll8EMessage::GetResponseBufferSize()
		///     CLongPoll8EMessage::GetPollValue()
		///     CLongPoll8EMessage::SwapPollValue()
		///     CLongPoll8EMessage::IsGeneralPoll()
		///     CLongPoll8EMessage::FillWithTickCountBeforeSending()
		///     CLongPoll8EMessage::GetReturnValue()
		/// Code coverage for:
		///     CLongPoll8EMessage::~CLongPoll8EMessage()
		/// Test for correct default values.
		/// </summary>
		TEST_F(LongPoll8EMessageTests, CLongPoll8EMessage_Default)
		{
			// Data.
			const byte gameAddress = 0x17;
			const CLongPoll8EMessage::PokerHandType pokerHandType(CLongPoll8EMessage::FinalHand);

			// Perform operation(s) and test(s).
			CLongPoll8EMessage msg(gameAddress, pokerHandType);
			msg.Fill();

			ASSERT_EQ(msg.GetSendBufferSize(), m_sendBufferSizeExpected);
			ASSERT_EQ(msg.GetResponseBufferSize(), m_responseBufferSizeExpected);
			ASSERT_EQ(msg.GetPollValue(), m_pollValueExpected);
			ASSERT_EQ(msg.SwapPollValue(), m_swapPollValueExpected);
			ASSERT_EQ(msg.IsGeneralPoll(), m_isGeneralPollExpected);
			ASSERT_EQ(msg.FillWithTickCountBeforeSending(), m_fillWithTickCountBeforeSendingExpected);
			ASSERT_EQ(msg.GetReturnValue(), m_returnValueDefaultExpected);
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll8EMessage::CLongPoll8EMessage()
		/// Test various gameAddress values.
		/// </summary>
		void LongPoll8EMessageTests::CLongPoll80Message_GameAddressX(byte testValue)
		{
			// Data.
			const CLongPoll8EMessage::PokerHandType pokerHandType(CLongPoll8EMessage::FinalHand);

			{
				// Perform operation(s) and test(s).
				CLongPoll8EMessage msg(testValue, pokerHandType);
				ASSERT_EQ(msg.m_gameAddress, testValue);
			}
		}

		void LongPoll8EMessageTests::CLongPoll80Message_GameAddress(byte testValue)
		{
			// Data.
			const CLongPoll8EMessage::PokerHandType pokerHandType(CLongPoll8EMessage::FinalHand);

			{
				// Perform operation(s) and test(s).
				CLongPoll8EMessage msg(testValue, pokerHandType);
				ASSERT_EQ(msg.m_gameAddress, testValue);
			}
		}

		TEST_F(LongPoll8EMessageTests, CLongPoll80Message_GameAddress)
		{
			Test(LongPoll8EMessageTests::CLongPoll80Message_GameAddress);
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll8EMessage::CLongPoll8EMessage()
		/// Test various pokerHandType values.
		/// </summary>
		void LongPoll8EMessageTests::CLongPoll80Message_PokerHandType(int testData)
		{
			// Data.
			const byte gameAddress = 1;

			{
				// Get the test data.
				CLongPoll8EMessage::PokerHandType testValue((CLongPoll8EMessage::PokerHandType)testData);

				// Perform operation(s) and test(s).
				CLongPoll8EMessage msg(gameAddress, testValue);
				ASSERT_EQ(msg.m_pokerHandType, testValue);
			}
		}

		TEST_F(LongPoll8EMessageTests, CLongPoll80Message_PokerHandType)
		{
			Test(LongPoll8EMessageTests::CLongPoll80Message_PokerHandType, CLongPoll8EMessage::DealtHand, CLongPoll8EMessage::FinalHand);
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll8EMessage::CLongPoll8EMessage()
		///     CLongPoll8EMessage::Fill()
		///     CLongPoll8EMessage::GetSendBuffer()
		///     CLongPoll8EMessage::GetSendBufferSize()
		/// Test various gameAddress values.
		/// </summary>
		void LongPoll8EMessageTests::Fill_GameAddress(byte testValue)
		{
			// Data.
			const CLongPoll8EMessage::PokerHandType pokerHandType(CLongPoll8EMessage::FinalHand);

			{
				// Setup.
				const BYTE sendBufferExpected[m_sendBufferSizeExpected] = { testValue, m_pollValueExpected };

				// Perform operation(s) and test(s).
				CLongPoll8EMessage msg(testValue, pokerHandType);
				msg.Fill();
				CUnitTestingUtils::AssertEqual(msg.GetSendBuffer(), msg.GetSendBufferSize(), sendBufferExpected, m_sendBufferSizeExpected,
					_T("GetSendBuffer"), (int)testValue, __TFUNCTION__, __LINE__);
			}
		}

		TEST_F(LongPoll8EMessageTests, Fill_GameAddress)
		{
			Test(LongPoll8EMessageTests::Fill_GameAddress);
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll8EMessage::CLongPoll8EMessage()
		///     CLongPoll8EMessage::ResponseReceived()
		///     CLongPoll8EMessage::PokerHand()
		/// Test with DealtHand required, but FinalHand received.
		/// </summary>
		TEST_F(LongPoll8EMessageTests, ResponseReceived_DealtHandFalse)
		{
			// Data.
			const byte gameAddress = 1;
			CLongPoll8EMessage::PokerHandType handTypeRequired(CLongPoll8EMessage::DealtHand);
			CLongPoll8EMessage::PokerHandType handTypeReceived(CLongPoll8EMessage::FinalHand);

			// Setup.
			BYTE receiveBuffer[m_responseBufferSizeExpected] = 	{
				0, m_pollValueExpected, (BYTE)handTypeReceived, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xcc, 0xcc };

			// Perform operation(s) and test(s).
			CLongPoll8EMessage msg(gameAddress, handTypeRequired);
			msg.Fill();
			memcpy_s(msg.GetResponseBuffer(), msg.GetResponseBufferSize(), receiveBuffer, sizeof(receiveBuffer));
			msg.ResponseReceived();
			ASSERT_EQ(msg.GetPokerHand().IsValid(), false);
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll8EMessage::CLongPoll8EMessage()
		///     CLongPoll8EMessage::ResponseReceived()
		///     CLongPoll8EMessage::PokerHand()
		/// Test with DealtHand required and DealtHand received.
		/// </summary>
		TEST_F(LongPoll8EMessageTests, ResponseReceived_DealtHandTrue)
		{
			// Data.
			const byte gameAddress = 1;
			CLongPoll8EMessage::PokerHandType handTypeRequired(CLongPoll8EMessage::DealtHand);
			CLongPoll8EMessage::PokerHandType handTypeReceived(CLongPoll8EMessage::DealtHand);

			// Setup.
			BYTE receiveBuffer[m_responseBufferSizeExpected] = {
				0, m_pollValueExpected, (BYTE)handTypeReceived, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xcc, 0xcc };

			// Perform operation(s) and test(s).
			CLongPoll8EMessage msg(gameAddress, handTypeRequired);
			msg.Fill();
			memcpy_s(msg.GetResponseBuffer(), msg.GetResponseBufferSize(), receiveBuffer, sizeof(receiveBuffer));
			msg.ResponseReceived();
			ASSERT_EQ(msg.GetPokerHand().IsValid(), true);

			ASSERT_EQ(PokerHand::CardsInHand, 5);
			ASSERT_EQ(msg.GetPokerHand()[0], receiveBuffer[m_cardLocationExpected + 0]);
			ASSERT_EQ(msg.GetPokerHand()[1], receiveBuffer[m_cardLocationExpected + 1]);
			ASSERT_EQ(msg.GetPokerHand()[2], receiveBuffer[m_cardLocationExpected + 2]);
			ASSERT_EQ(msg.GetPokerHand()[3], receiveBuffer[m_cardLocationExpected + 3]);
			ASSERT_EQ(msg.GetPokerHand()[4], receiveBuffer[m_cardLocationExpected + 4]);
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll8EMessage::CLongPoll8EMessage()
		///     CLongPoll8EMessage::ResponseReceived()
		///     CLongPoll8EMessage::PokerHand()
		/// Test with FinalHand required, but DealtHand received.
		/// </summary>
		TEST_F(LongPoll8EMessageTests, ResponseReceived_FinalHandFalse)
		{
			// Data.
			const byte gameAddress = 1;
			CLongPoll8EMessage::PokerHandType handTypeRequired(CLongPoll8EMessage::FinalHand);
			CLongPoll8EMessage::PokerHandType handTypeReceived(CLongPoll8EMessage::DealtHand);

			// Setup.
			BYTE receiveBuffer[m_responseBufferSizeExpected] = {
				0, m_pollValueExpected, (BYTE)handTypeReceived, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xcc, 0xcc };

			// Perform operation(s) and test(s).
			CLongPoll8EMessage msg(gameAddress, handTypeRequired);
			msg.Fill();
			memcpy_s(msg.GetResponseBuffer(), msg.GetResponseBufferSize(), receiveBuffer, sizeof(receiveBuffer));
			msg.ResponseReceived();
			ASSERT_EQ(msg.GetPokerHand().IsValid(), false);
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll8EMessage::CLongPoll8EMessage()
		///     CLongPoll8EMessage::ResponseReceived()
		///     CLongPoll8EMessage::PokerHand()
		/// Test with FinalHand required and FinalHand received.
		/// </summary>
		TEST_F(LongPoll8EMessageTests, ResponseReceived_FinalHandTrue)
		{
			// Data.
			const byte gameAddress = 1;
			CLongPoll8EMessage::PokerHandType handTypeRequired(CLongPoll8EMessage::FinalHand);
			CLongPoll8EMessage::PokerHandType handTypeReceived(CLongPoll8EMessage::FinalHand);

			// Setup.
			BYTE receiveBuffer[m_responseBufferSizeExpected] = {
				0, m_pollValueExpected, (BYTE)handTypeReceived, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xcc, 0xcc };

			// Perform operation(s) and test(s).
			CLongPoll8EMessage msg(gameAddress, handTypeRequired);
			msg.Fill();
			memcpy_s(msg.GetResponseBuffer(), msg.GetResponseBufferSize(), receiveBuffer, sizeof(receiveBuffer));
			msg.ResponseReceived();
			ASSERT_EQ(msg.GetPokerHand().IsValid(), true);

			ASSERT_EQ(PokerHand::CardsInHand, 5);
			ASSERT_EQ(msg.GetPokerHand()[0], receiveBuffer[m_cardLocationExpected + 0]);
			ASSERT_EQ(msg.GetPokerHand()[1], receiveBuffer[m_cardLocationExpected + 1]);
			ASSERT_EQ(msg.GetPokerHand()[2], receiveBuffer[m_cardLocationExpected + 2]);
			ASSERT_EQ(msg.GetPokerHand()[3], receiveBuffer[m_cardLocationExpected + 3]);
			ASSERT_EQ(msg.GetPokerHand()[4], receiveBuffer[m_cardLocationExpected + 4]);
		}

		/// <summary>
		/// Functional test for:
		///     CLongPoll8EMessage::CLongPoll8EMessage()
		///     CLongPoll8EMessage::ResponseReceived()
		///     CLongPoll8EMessage::PokerHand()
		/// Test various pokerHandType values.
		/// </summary>
		void LongPoll8EMessageTests::ResponseReceived_PokerHandType(int testData)
		{
			// Data.
			const byte gameAddress = 1;

			{
				// Get the test data.
				CLongPoll8EMessage::PokerHandType testValue((CLongPoll8EMessage::PokerHandType)testData);

				// Setup.
				BYTE receiveBuffer[m_responseBufferSizeExpected] = {
					0, m_pollValueExpected, (BYTE)testValue, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xcc, 0xcc };
				bool isValidExpected((CLongPoll8EMessage::PokerHandType)(BYTE)testValue == testValue);

				// Perform operation(s) and test(s).
				CLongPoll8EMessage msg(gameAddress, testValue);
				msg.Fill();
				memcpy_s(msg.GetResponseBuffer(), msg.GetResponseBufferSize(), receiveBuffer, sizeof(receiveBuffer));
				msg.ResponseReceived();
				ASSERT_EQ(msg.GetPokerHand().IsValid(), isValidExpected);
				if (isValidExpected)
				{
					ASSERT_EQ(msg.GetPokerHand()[0], receiveBuffer[m_cardLocationExpected + 0]);
					ASSERT_EQ(msg.GetPokerHand()[1], receiveBuffer[m_cardLocationExpected + 1]);
					ASSERT_EQ(msg.GetPokerHand()[2], receiveBuffer[m_cardLocationExpected + 2]);
					ASSERT_EQ(msg.GetPokerHand()[3], receiveBuffer[m_cardLocationExpected + 3]);
					ASSERT_EQ(msg.GetPokerHand()[4], receiveBuffer[m_cardLocationExpected + 4]);
				}
			}
		}

		TEST_F(LongPoll8EMessageTests, ResponseReceived_PokerHandType)
		{
			Test(ResponseReceived_PokerHandType, CLongPoll8EMessage::DealtHand, CLongPoll8EMessage::FinalHand);
		}

	const BYTE LongPoll8EMessageTests::m_pollValueExpected = 0x8e;
	const UINT LongPoll8EMessageTests::m_sendBufferSizeExpected = 2;
	const UINT LongPoll8EMessageTests::m_responseBufferSizeExpected = 10;
	const UINT LongPoll8EMessageTests::m_handTypeLocationExpected = 2;
	const UINT LongPoll8EMessageTests::m_cardLocationExpected = 3;

	const bool LongPoll8EMessageTests::m_swapPollValueExpected = false;
	const bool LongPoll8EMessageTests::m_isGeneralPollExpected = false;
	const bool LongPoll8EMessageTests::m_fillWithTickCountBeforeSendingExpected = false;
	const int LongPoll8EMessageTests::m_returnValueDefaultExpected = SASPOLL_SUCCESS;
