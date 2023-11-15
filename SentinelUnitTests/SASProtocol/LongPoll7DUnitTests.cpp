#include "stdafx.h"
#include "SASProtocol/LongPoll7DMessage.h"

#include "GlobalDefs.h"
#include "Utilities.h"


#include "gtest/gtest.h"
using namespace std;

#define _I8_MIN     (-127 - 1)    // minimum signed 8 bit value
#define _I8_MAX       127         // maximum signed 8 bit value
#define _UI8_MAX      0xffu       // maximum unsigned 8 bit value

namespace SentinelUnitTests
{

		/// Code coverage and functional test for:
		///     CLongPoll7DMessage::CLongPoll7DMessage()
		/// Code coverage for:
		///     CLongPoll7DMessage::~CLongPoll7DMessage() -- skip delete for NULL members.
		/// Test for correct default values.
		/// </summary>
		TEST(LongPoll7DMessage, CLongPoll7DMessageTest)
		{
			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;

				// Expected results.
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (byte)-1 },
				{ 0 },
				{ 1 },
				{ 2 },
				{ (byte)(_I8_MAX - 1) },
				{ (byte)_I8_MAX },
				{ (byte)_I8_MIN },
				{ (byte)(_I8_MIN + 1) },
				{ (byte)(_UI8_MAX - 1) },
				{ (byte)_UI8_MAX },
			};

			// Expected results -- same for all loop iterations.
			const BYTE pollValueExpected(0x7D);
			const bool swapPollValueExpected(false);
			const UINT responseBufferSizeExpected(5);
			const BYTE responseBufferExpected[responseBufferSizeExpected] = { 0, 0, 0, 0, 0 };
			const UINT sendBufferSizeExpected(0);
			const BYTE sendBufferExpected = 0;
			//const DWORD waitForSingleObjectExpected(WAIT_TIMEOUT);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CLongPoll7DMessage varMsg(TEST_DATA[testIndex].m_gameAddress);
				const CLongPoll7DMessage &msg(varMsg);

				// Test #1.
				{
					const BYTE pollValueResult(varMsg.GetPollValue());
					const bool success(pollValueResult == pollValueExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("pollValueResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)pollValueResult,
							(unsigned)pollValueExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #2.
				{
					const bool swapPollValueResult(varMsg.SwapPollValue());
					const bool success(swapPollValueResult == swapPollValueExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("swapPollValueResult=%d expected=%d testIndex=%d in %s(%d)",
							swapPollValueResult,
							swapPollValueExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #3.
				const BYTE * const responseBufferResult(varMsg.GetResponseBuffer());

				{
					const bool success(responseBufferResult != NULL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("responseBufferResult=%p expected=NOT NULL testIndex=%d in %s(%d)",
							(void *)responseBufferResult,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #4.
				const UINT responseBufferSizeResult(varMsg.GetResponseBufferSize());

				{
					const bool success(responseBufferSizeResult == responseBufferSizeExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("responseBufferSizeResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)responseBufferSizeResult,
							(unsigned)responseBufferSizeExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #5.
				{
					const int memcmpResult = memcmp(responseBufferResult, responseBufferExpected, responseBufferSizeExpected);
					const bool success(memcmpResult == MEMCMP_EQUAL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("responseBufferResult={%s} expected={%s} testIndex=%d in %s(%d)",
							(CUtilities::ToHexString(responseBufferResult, responseBufferSizeResult)).c_str(),
							(CUtilities::ToHexString(responseBufferExpected, responseBufferSizeExpected)).c_str(),
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #6.
				const BYTE * const sendBufferResult(varMsg.GetSendBuffer());

				{
					const bool success(sendBufferResult == NULL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult=%p expected=NULL testIndex=%d in %s(%d)",
							(void *)sendBufferResult,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #7.
				const UINT sendBufferSizeResult(varMsg.GetSendBufferSize());

				{
					const bool success(sendBufferSizeResult == sendBufferSizeExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferSizeResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)sendBufferSizeResult,
							(unsigned)sendBufferSizeExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #8.
				{
					const int memcmpResult = memcmp(sendBufferResult, &sendBufferExpected, sendBufferSizeExpected);
					const bool success(memcmpResult == MEMCMP_EQUAL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult={%s} expected={%s} testIndex=%d in %s(%d)",
							(CUtilities::ToHexString(sendBufferResult, sendBufferSizeResult)).c_str(),
							(CUtilities::ToHexString(&sendBufferExpected, sendBufferSizeExpected)).c_str(),
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASPollMessage::GetResponseEventHandle()
		///     CLongPoll7DMessage::ResponseReceived()
		/// Test for correct default values.
		/// </summary>
		TEST(LongPoll7DMessage, ResponseReceivedTest)
		{
			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;

				// Expected results.
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (byte)-1 },
				{ 0 },
				{ 1 },
				{ 2 },
				{ (byte)(_I8_MAX - 1) },
				{ (byte)_I8_MAX },
				{ (byte)_I8_MIN },
				{ (byte)(_I8_MIN + 1) },
				{ (byte)(_UI8_MAX - 1) },
				{ (byte)_UI8_MAX },
			};

			// Expected results -- same for all loop iterations.
			//const DWORD waitForSingleObjectExpected(0);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll7DMessage varMsg(TEST_DATA[testIndex].m_gameAddress);
				const CLongPoll7DMessage &msg(varMsg);

				// Perform operation(s) to be tested.
				varMsg.ResponseReceived();

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7DMessage::CLongPoll7DMessage()
		///     CLongPoll7DMessage::Fill()
		/// Test for correct default values.
		/// </summary>
		TEST(LongPoll7DMessage, Fill_DefaultTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(11);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0xff, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x58, 0xd6 };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 0x00, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x69, 0x6a };
			const BYTE sendBuffer2[sendBufferSizeExpected] = { 0x01, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x94, 0x27 };
			const BYTE sendBuffer3[sendBufferSizeExpected] = { 0x02, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x93, 0xf1 };
			const BYTE sendBuffer4[sendBufferSizeExpected] = { 0x7e, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x72, 0x5f };
			const BYTE sendBuffer5[sendBufferSizeExpected] = { 0x7f, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x8f, 0x12 };
			const BYTE sendBuffer6[sendBufferSizeExpected] = { 0x80, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0xbe, 0xae };
			const BYTE sendBuffer7[sendBufferSizeExpected] = { 0x81, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x43, 0xe3 };
			const BYTE sendBuffer8[sendBufferSizeExpected] = { 0xfe, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0xa5, 0x9b };
			const BYTE sendBuffer9[sendBufferSizeExpected] = { 0xff, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x58, 0xd6 };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (byte)-1, sendBuffer0 },
				{ 0, sendBuffer1 },
				{ 1, sendBuffer2 },
				{ 2, sendBuffer3 },
				{ (byte)(_I8_MAX - 1), sendBuffer4 },
				{ (byte)_I8_MAX, sendBuffer5 },
				{ (byte)_I8_MIN, sendBuffer6 },
				{ (byte)(_I8_MIN + 1), sendBuffer7 },
				{ (byte)(_UI8_MAX - 1), sendBuffer8 },
				{ (byte)_UI8_MAX, sendBuffer9 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CLongPoll7DMessage varMsg(TEST_DATA[testIndex].m_gameAddress);
				varMsg.Fill();
				const CLongPoll7DMessage &msg(varMsg);

				// Test #1.
				const BYTE * const sendBufferResult(varMsg.GetSendBuffer());

				{
					const bool success(sendBufferResult != NULL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult=%p expected=NOT NULL testIndex=%d in %s(%d)",
							(void *)sendBufferResult,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #2.
				const UINT sendBufferSizeResult(varMsg.GetSendBufferSize());

				{
					const bool success(sendBufferSizeResult == sendBufferSizeExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferSizeResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)sendBufferSizeResult,
							(unsigned)sendBufferSizeExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #3.
				{
					const int memcmpResult = memcmp(sendBufferResult, TEST_DATA[testIndex].m_sendBufferExpected, sendBufferSizeExpected);
					const bool success(memcmpResult == MEMCMP_EQUAL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult={%s} expected={%s} testIndex=%d in %s(%d)",
							(CUtilities::ToHexString(sendBufferResult, sendBufferSizeResult)).c_str(),
							(CUtilities::ToHexString(TEST_DATA[testIndex].m_sendBufferExpected, sendBufferSizeExpected)).c_str(),
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7DMessage::CLongPoll7DMessage()
		///     CLongPoll7DMessage::SetHostID()
		///     CLongPoll7DMessage::Fill()
		/// </summary>
		TEST(LongPoll7DMessage, SetHostIdTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(11);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0xff, 0x7d, 6, 0x00, 0x00, 0, 0, 0, 0, 0x58, 0xd6 };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 0x00, 0x7d, 6, 0x01, 0x00, 0, 0, 0, 0, 0x42, 0x6e };
			const BYTE sendBuffer2[sendBufferSizeExpected] = { 0x01, 0x7d, 6, 0x02, 0x00, 0, 0, 0, 0, 0xc2, 0x2f };
			const BYTE sendBuffer3[sendBufferSizeExpected] = { 0x02, 0x7d, 6, 0xfe, 0x7f, 0, 0, 0, 0, 0x9d, 0x77 };
			const BYTE sendBuffer4[sendBufferSizeExpected] = { 0x7e, 0x7d, 6, 0xff, 0x7f, 0, 0, 0, 0, 0x57, 0xdd };
			const BYTE sendBuffer5[sendBufferSizeExpected] = { 0x7f, 0x7d, 6, 0x00, 0x80, 0, 0, 0, 0, 0xda, 0x98 };
			const BYTE sendBuffer6[sendBufferSizeExpected] = { 0x80, 0x7d, 6, 0x01, 0x80, 0, 0, 0, 0, 0xc0, 0x20 };
			const BYTE sendBuffer7[sendBufferSizeExpected] = { 0x81, 0x7d, 6, 0xfe, 0xff, 0, 0, 0, 0, 0x18, 0xef };
			const BYTE sendBuffer8[sendBufferSizeExpected] = { 0xfe, 0x7d, 6, 0xff, 0xff, 0, 0, 0, 0, 0xd5, 0x93 };
			const BYTE sendBuffer9[sendBufferSizeExpected] = { 0xff, 0x7d, 6, 0xff, 0xff, 0, 0, 0, 0, 0x28, 0xde };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const WORD m_hostId;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (byte)-1, 0, sendBuffer0 },
				{ 0, 1, sendBuffer1 },
				{ 1, 2, sendBuffer2 },
				{ 2, (WORD)(SHRT_MAX - 1), sendBuffer3 },
				{ (byte)(_I8_MAX - 1), (WORD)(SHRT_MAX), sendBuffer4 },
				{ (byte)_I8_MAX, (WORD)(SHRT_MIN), sendBuffer5 },
				{ (byte)_I8_MIN, (WORD)(SHRT_MIN + 1), sendBuffer6 },
				{ (byte)(_I8_MIN + 1), WORD_MAX - 1, sendBuffer7 },
				{ (byte)(_UI8_MAX - 1), WORD_MAX, sendBuffer8 },
				{ (byte)_UI8_MAX, (WORD)-1, sendBuffer9 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CLongPoll7DMessage varMsg(TEST_DATA[testIndex].m_gameAddress);
				varMsg.SetHostID(TEST_DATA[testIndex].m_hostId);
				varMsg.Fill();
				const CLongPoll7DMessage &msg(varMsg);

				// Test #1.
				const BYTE * const sendBufferResult(varMsg.GetSendBuffer());

				{
					const bool success(sendBufferResult != NULL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult=%p expected=NOT NULL testIndex=%d in %s(%d)",
							(void *)sendBufferResult,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #2.
				const UINT sendBufferSizeResult(varMsg.GetSendBufferSize());

				{
					const bool success(sendBufferSizeResult == sendBufferSizeExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferSizeResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)sendBufferSizeResult,
							(unsigned)sendBufferSizeExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #3.
				{
					const int memcmpResult = memcmp(sendBufferResult, TEST_DATA[testIndex].m_sendBufferExpected, sendBufferSizeExpected);
					const bool success(memcmpResult == MEMCMP_EQUAL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult={%s} expected={%s} testIndex=%d in %s(%d)",
							(CUtilities::ToHexString(sendBufferResult, sendBufferSizeResult)).c_str(),
							(CUtilities::ToHexString(TEST_DATA[testIndex].m_sendBufferExpected, sendBufferSizeExpected)).c_str(),
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7DMessage::CLongPoll7DMessage()
		///     CLongPoll7DMessage::SetExpiration()
		///     CLongPoll7DMessage::Fill()
		/// </summary>
		TEST(LongPoll7DMessage, SetExpirationTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(11);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0xff, 0x7d, 6, 0, 0, 0x01, 0, 0, 0, 0xe3, 0xca };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 0x00, 0x7d, 6, 0, 0, 0x02, 0, 0, 0, 0x1f, 0x53 };
			const BYTE sendBuffer2[sendBufferSizeExpected] = { 0x01, 0x7d, 6, 0, 0, 0x7e, 0, 0, 0, 0x93, 0xd3 };
			const BYTE sendBuffer3[sendBufferSizeExpected] = { 0x02, 0x7d, 6, 0, 0, 0x7f, 0, 0, 0, 0x2f, 0x19 };
			const BYTE sendBuffer4[sendBufferSizeExpected] = { 0x7e, 0x7d, 6, 0, 0, 0x80, 0, 0, 0, 0x1c, 0x72 };
			const BYTE sendBuffer5[sendBufferSizeExpected] = { 0x7f, 0x7d, 6, 0, 0, 0x81, 0, 0, 0, 0x5a, 0x23 };
			const BYTE sendBuffer6[sendBufferSizeExpected] = { 0x80, 0x7d, 6, 0, 0, 0xfe, 0, 0, 0, 0xd7, 0x77 };
			const BYTE sendBuffer7[sendBufferSizeExpected] = { 0x81, 0x7d, 6, 0, 0, 0xff, 0, 0, 0, 0x91, 0x26 };
			const BYTE sendBuffer8[sendBufferSizeExpected] = { 0xfe, 0x7d, 6, 0, 0, 0xff, 0, 0, 0, 0x77, 0x5e };
			const BYTE sendBuffer9[sendBufferSizeExpected] = { 0xff, 0x7d, 6, 0, 0, 0x00, 0, 0, 0, 0x58, 0xd6 };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const byte m_expiration;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (byte)-1, 1, sendBuffer0 },
				{ 0, 2, sendBuffer1 },
				{ 1, (byte)(_I8_MAX - 1), sendBuffer2 },
				{ 2, (byte)(_I8_MAX), sendBuffer3 },
				{ (byte)(_I8_MAX - 1), (byte)(_I8_MIN), sendBuffer4 },
				{ (byte)_I8_MAX, (byte)(_I8_MIN + 1), sendBuffer5 },
				{ (byte)_I8_MIN, (byte)(_UI8_MAX - 1), sendBuffer6 },
				{ (byte)(_I8_MIN + 1), (byte)_UI8_MAX, sendBuffer7 },
				{ (byte)(_UI8_MAX - 1), (byte)-1, sendBuffer8 },
				{ (byte)_UI8_MAX, 0, sendBuffer9 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CLongPoll7DMessage varMsg(TEST_DATA[testIndex].m_gameAddress);
				varMsg.SetExpiration(TEST_DATA[testIndex].m_expiration);
				varMsg.Fill();
				const CLongPoll7DMessage &msg(varMsg);

				// Test #1.
				const BYTE * const sendBufferResult(varMsg.GetSendBuffer());

				{
					const bool success(sendBufferResult != NULL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult=%p expected=NOT NULL testIndex=%d in %s(%d)",
							(void *)sendBufferResult,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #2.
				const UINT sendBufferSizeResult(varMsg.GetSendBufferSize());

				{
					const bool success(sendBufferSizeResult == sendBufferSizeExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferSizeResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)sendBufferSizeResult,
							(unsigned)sendBufferSizeExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #3.
				{
					const int memcmpResult = memcmp(sendBufferResult, TEST_DATA[testIndex].m_sendBufferExpected, sendBufferSizeExpected);
					const bool success(memcmpResult == MEMCMP_EQUAL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult={%s} expected={%s} testIndex=%d in %s(%d)",
							(CUtilities::ToHexString(sendBufferResult, sendBufferSizeResult)).c_str(),
							(CUtilities::ToHexString(TEST_DATA[testIndex].m_sendBufferExpected, sendBufferSizeExpected)).c_str(),
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7DMessage::CLongPoll7DMessage()
		///     CLongPoll7DMessage::SetAddressData()
		///     CLongPoll7DMessage::Fill()
		/// Code coverage for:
		///     CLongPoll7DMessage::~CLongPoll7DMessage()
		/// Test SetAddressData() for correct location values.
		/// </summary>
		TEST(LongPoll7DMessage, SetLocationTest)
		{
			const byte location2[] = { 0 }; // Use length of 0.
			const byte location3[] = { 0 };
			const byte location4[] = { 0xff };
			const byte location5[] = { (byte)'a', 0x00, (byte)'b' }; // Array with embedded null.
			const byte location6[] = { (byte)'a', (byte)'b', (byte)'c', 0x00 }; // Array with trailing null.
			const byte location7[] = { (byte)'D', (byte)'E', (byte)'F', (byte)'G' };
			const byte location8[39] = {
				1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39 }; // 1 less than maximum stored length.
			const byte location9[40] = {
				1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40 }; // Maximum stored length.
			const byte location10[41] = {
				1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 }; // 1 more than maximum stored length.
			const byte location11[255] = {
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, // Maximum parameter length.
			};

			// Expected results -- per iteration.
			const BYTE sendBuffer0[] = { 0, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x69, 0x6a };
			const BYTE sendBuffer1[] = { 1, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x94, 0x27 };
			const BYTE sendBuffer2[] = { 2, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x93, 0xf1 };
			const BYTE sendBuffer3[] = { 3, 0x7d, 7, 0, 0, 0, 1, 0, 0, 0, 0xc0, 0x17 };
			const BYTE sendBuffer4[] = { 4, 0x7d, 7, 0, 0, 0, 1, 0xff, 0, 0, 0xc6, 0x15 };
			const BYTE sendBuffer5[] = { 5, 0x7d, 9, 0, 0, 0, 3, (byte)'a', 0, (byte)'b', 0, 0, 0xf6, 0x3a };
			const BYTE sendBuffer6[] = { 6, 0x7d, 10, 0, 0, 0, 4, (byte)'a', (byte)'b', (byte)'c', 0, 0, 0, 0x5f, 0x0e };
			const BYTE sendBuffer7[] = { 7, 0x7d, 10, 0, 0, 0, 4, (byte)'D', (byte)'E', (byte)'F', (byte)'G', 0, 0, 0x2f, 0xa1 };
			const BYTE sendBuffer8[] = { 8, 0x7d, 45, 0, 0, 0,
				39, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
				0, 0, 0x11, 0x8c };
			const BYTE sendBuffer9[] = { 9, 0x7d, 46, 0, 0, 0,
				40, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				0, 0, 0x72, 0x72 };
			const BYTE sendBuffer10[] = { 10, 0x7d, 46, 0, 0, 0,
				40, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				0, 0, 0xd3, 0x1f };
			const BYTE sendBuffer11[] = { 11, 0x7d, 46, 0, 0, 0,
				40, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
				0, 0, 0x2f, 0xbe };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const byte * const m_location;
				const byte m_locationLength;
				const byte * const m_address1;
				const byte m_address1Length;
				const byte * const m_address2;
				const byte m_address2Length;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
				const UINT m_sendBufferSizeExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, NULL, 0, NULL, 0, NULL, 0, sendBuffer0, sizeof(sendBuffer0) },
				{ 1, NULL, 33, NULL, 0, NULL, 0, sendBuffer1, sizeof(sendBuffer1) },
				{ 2, location2, 0, NULL, 0, NULL, 0, sendBuffer2, sizeof(sendBuffer2) },
				{ 3, location3, (byte)sizeof(location3), NULL, 0, NULL, 0, sendBuffer3, sizeof(sendBuffer3) },
				{ 4, location4, (byte)sizeof(location4), NULL, 0, NULL, 0, sendBuffer4, sizeof(sendBuffer4) },
				{ 5, location5, (byte)sizeof(location5), NULL, 0, NULL, 0, sendBuffer5, sizeof(sendBuffer5) },
				{ 6, location6, (byte)sizeof(location6), NULL, 0, NULL, 0, sendBuffer6, sizeof(sendBuffer6) },
				{ 7, location7, (byte)sizeof(location7), NULL, 0, NULL, 0, sendBuffer7, sizeof(sendBuffer7) },
				{ 8, location8, (byte)sizeof(location8), NULL, 0, NULL, 0, sendBuffer8, sizeof(sendBuffer8) },
				{ 9, location9, (byte)sizeof(location9), NULL, 0, NULL, 0, sendBuffer9, sizeof(sendBuffer9) },
				{ 10, location10, (byte)sizeof(location10), NULL, 0, NULL, 0, sendBuffer10, sizeof(sendBuffer10) },
				{ 11, location11, (byte)sizeof(location11), NULL, 0, NULL, 0, sendBuffer11, sizeof(sendBuffer11) },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CLongPoll7DMessage varMsg(TEST_DATA[testIndex].m_gameAddress);
				varMsg.SetAddressData(TEST_DATA[testIndex].m_location,
					TEST_DATA[testIndex].m_locationLength,
					TEST_DATA[testIndex].m_address1,
					TEST_DATA[testIndex].m_address1Length,
					TEST_DATA[testIndex].m_address2,
					TEST_DATA[testIndex].m_address2Length);
				varMsg.Fill();
				const CLongPoll7DMessage &msg(varMsg);

				// Test #1.
				const BYTE * const sendBufferResult(varMsg.GetSendBuffer());

				{
					const bool success(sendBufferResult != NULL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult=%p expected=NOT NULL testIndex=%d in %s(%d)",
							(void *)sendBufferResult,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #2.
				const UINT sendBufferSizeResult(varMsg.GetSendBufferSize());

				{
					const bool success(sendBufferSizeResult == TEST_DATA[testIndex].m_sendBufferSizeExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferSizeResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)sendBufferSizeResult,
							(unsigned)TEST_DATA[testIndex].m_sendBufferSizeExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #3.
				{
					const int memcmpResult = memcmp(sendBufferResult, TEST_DATA[testIndex].m_sendBufferExpected, TEST_DATA[testIndex].m_sendBufferSizeExpected);
					const bool success(memcmpResult == MEMCMP_EQUAL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult={%s} expected={%s} testIndex=%d in %s(%d)",
							(CUtilities::ToHexString(sendBufferResult, sendBufferSizeResult)).c_str(),
							(CUtilities::ToHexString(TEST_DATA[testIndex].m_sendBufferExpected, TEST_DATA[testIndex].m_sendBufferSizeExpected)).c_str(),
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7DMessage::CLongPoll7DMessage()
		///     CLongPoll7DMessage::SetAddressData()
		///     CLongPoll7DMessage::Fill()
		/// Code coverage for:
		///     CLongPoll7DMessage::~CLongPoll7DMessage()
		/// Test SetAddressData() for correct address1 values.
		/// </summary>
		TEST(LongPoll7DMessage, SetAddress1Test)
		{
			const byte address1_2[] = { 0 }; // Use length of 0.
			const byte address1_3[] = { 0 };
			const byte address1_4[] = { 0xff };
			const byte address1_5[] = { (byte)'a', 0x00, (byte)'b' }; // Array with embedded null.
			const byte address1_6[] = { (byte)'a', (byte)'b', (byte)'c', 0x00 }; // Array with trailing null.
			const byte address1_7[] = { (byte)'D', (byte)'E', (byte)'F', (byte)'G' };
			const byte address1_8[39] = {
				1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39 }; // 1 less than maximum stored length.
			const byte address1_9[40] = {
				1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40 }; // Maximum stored length.
			const byte address1_10[41] = {
				1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 }; // 1 more than maximum stored length.
			const byte address1_11[255] = {
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, // Maximum parameter length.
			};

			// Expected results -- per iteration.
			const BYTE sendBuffer0[] = { 0, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x69, 0x6a };
			const BYTE sendBuffer1[] = { 1, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x94, 0x27 };
			const BYTE sendBuffer2[] = { 2, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x93, 0xf1 };
			const BYTE sendBuffer3[] = { 3, 0x7d, 7, 0, 0, 0, 0, 1, 0, 0, 0xa7, 0x51 };
			const BYTE sendBuffer4[] = { 4, 0x7d, 7, 0, 0, 0, 0, 1, 0xff, 0, 0x92, 0x6a };
			const BYTE sendBuffer5[] = { 5, 0x7d, 9, 0, 0, 0, 0, 3, (byte)'a', 0, (byte)'b', 0, 0xff, 0xe2 };
			const BYTE sendBuffer6[] = { 6, 0x7d, 10, 0, 0, 0, 0, 4, (byte)'a', (byte)'b', (byte)'c', 0, 0, 0x41, 0x19 };
			const BYTE sendBuffer7[] = { 7, 0x7d, 10, 0, 0, 0, 0, 4, (byte)'D', (byte)'E', (byte)'F', (byte)'G', 0, 0xb3, 0xb3 };
			const BYTE sendBuffer8[] = { 8, 0x7d, 45, 0, 0, 0, 0,
				39, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
				0, 0x0a, 0xf8 };
			const BYTE sendBuffer9[] = { 9, 0x7d, 46, 0, 0, 0, 0,
				40, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				0, 0x42, 0xdd };
			const BYTE sendBuffer10[] = { 10, 0x7d, 46, 0, 0, 0, 0,
				40, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				0, 0xe3, 0xb0 };
			const BYTE sendBuffer11[] = { 11, 0x7d, 46, 0, 0, 0, 0,
				40, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
				0, 0xed, 0xf7 };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const byte * const m_location;
				const byte m_locationLength;
				const byte * const m_address1;
				const byte m_address1Length;
				const byte * const m_address2;
				const byte m_address2Length;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
				const UINT m_sendBufferSizeExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, NULL, 0, NULL, 0, NULL, 0, sendBuffer0, sizeof(sendBuffer0) },
				{ 1, NULL, 0, NULL, 33, NULL, 0, sendBuffer1, sizeof(sendBuffer1) },
				{ 2, NULL, 0, address1_2, 0, NULL, 0, sendBuffer2, sizeof(sendBuffer2) },
				{ 3, NULL, 0, address1_3, (byte)sizeof(address1_3), NULL, 0, sendBuffer3, sizeof(sendBuffer3) },
				{ 4, NULL, 0, address1_4, (byte)sizeof(address1_4), NULL, 0, sendBuffer4, sizeof(sendBuffer4) },
				{ 5, NULL, 0, address1_5, (byte)sizeof(address1_5), NULL, 0, sendBuffer5, sizeof(sendBuffer5) },
				{ 6, NULL, 0, address1_6, (byte)sizeof(address1_6), NULL, 0, sendBuffer6, sizeof(sendBuffer6) },
				{ 7, NULL, 0, address1_7, (byte)sizeof(address1_7), NULL, 0, sendBuffer7, sizeof(sendBuffer7) },
				{ 8, NULL, 0, address1_8, (byte)sizeof(address1_8), NULL, 0, sendBuffer8, sizeof(sendBuffer8) },
				{ 9, NULL, 0, address1_9, (byte)sizeof(address1_9), NULL, 0, sendBuffer9, sizeof(sendBuffer9) },
				{ 10, NULL, 0, address1_10, (byte)sizeof(address1_10), NULL, 0, sendBuffer10, sizeof(sendBuffer10) },
				{ 11, NULL, 0, address1_11, (byte)sizeof(address1_11), NULL, 0, sendBuffer11, sizeof(sendBuffer11) },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CLongPoll7DMessage varMsg(TEST_DATA[testIndex].m_gameAddress);
				varMsg.SetAddressData(TEST_DATA[testIndex].m_location,
					TEST_DATA[testIndex].m_locationLength,
					TEST_DATA[testIndex].m_address1,
					TEST_DATA[testIndex].m_address1Length,
					TEST_DATA[testIndex].m_address2,
					TEST_DATA[testIndex].m_address2Length);
				varMsg.Fill();
				const CLongPoll7DMessage &msg(varMsg);

				// Test #1.
				const BYTE * const sendBufferResult(varMsg.GetSendBuffer());

				{
					const bool success(sendBufferResult != NULL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult=%p expected=NOT NULL testIndex=%d in %s(%d)",
							(void *)sendBufferResult,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #2.
				const UINT sendBufferSizeResult(varMsg.GetSendBufferSize());

				{
					const bool success(sendBufferSizeResult == TEST_DATA[testIndex].m_sendBufferSizeExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferSizeResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)sendBufferSizeResult,
							(unsigned)TEST_DATA[testIndex].m_sendBufferSizeExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #3.
				{
					const int memcmpResult = memcmp(sendBufferResult, TEST_DATA[testIndex].m_sendBufferExpected, TEST_DATA[testIndex].m_sendBufferSizeExpected);
					const bool success(memcmpResult == MEMCMP_EQUAL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult={%s} expected={%s} testIndex=%d in %s(%d)",
							(CUtilities::ToHexString(sendBufferResult, sendBufferSizeResult)).c_str(),
							(CUtilities::ToHexString(TEST_DATA[testIndex].m_sendBufferExpected, TEST_DATA[testIndex].m_sendBufferSizeExpected)).c_str(),
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7DMessage::CLongPoll7DMessage()
		///     CLongPoll7DMessage::SetAddressData()
		///     CLongPoll7DMessage::Fill()
		/// Code coverage for:
		///     CLongPoll7DMessage::~CLongPoll7DMessage()
		/// Test SetAddressData() for correct address2 values.
		/// </summary>
		TEST(LongPoll7DMessage, SetAddress2Test)
		{
			const byte address2_2[] = { 0 }; // Use length of 0.
			const byte address2_3[] = { 0 };
			const byte address2_4[] = { 0xff };
			const byte address2_5[] = { (byte)'a', 0x00, (byte)'b' }; // Array with embedded null.
			const byte address2_6[] = { (byte)'a', (byte)'b', (byte)'c', 0x00 }; // Array with trailing null.
			const byte address2_7[] = { (byte)'D', (byte)'E', (byte)'F', (byte)'G' };
			const byte address2_8[39] = {
				1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39 }; // 1 less than maximum stored length.
			const byte address2_9[40] = {
				1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40 }; // Maximum stored length.
			const byte address2_10[41] = {
				1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 }; // 1 more than maximum stored length.
			const byte address2_11[255] = {
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, // Maximum parameter length.
			};

			// Expected results -- per iteration.
			const BYTE sendBuffer0[] = { 0, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x69, 0x6a };
			const BYTE sendBuffer1[] = { 1, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x94, 0x27 };
			const BYTE sendBuffer2[] = { 2, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x93, 0xf1 };
			const BYTE sendBuffer3[] = { 3, 0x7d, 7, 0, 0, 0, 0, 0, 1, 0, 0xa3, 0x12 };
			const BYTE sendBuffer4[] = { 4, 0x7d, 7, 0, 0, 0, 0, 0, 1, 0xff, 0x2e, 0xd9 };
			const BYTE sendBuffer5[] = { 5, 0x7d, 9, 0, 0, 0, 0, 0, 3, (byte)'a', 0, (byte)'b', 0xc1, 0x16 };
			const BYTE sendBuffer6[] = { 6, 0x7d, 10, 0, 0, 0, 0, 0, 4, (byte)'a', (byte)'b', (byte)'c', 0, 0x26, 0xbe };
			const BYTE sendBuffer7[] = { 7, 0x7d, 10, 0, 0, 0, 0, 0, 4, (byte)'D', (byte)'E', (byte)'F', (byte)'G', 0x81, 0xbe };
			const BYTE sendBuffer8[] = { 8, 0x7d, 45, 0, 0, 0, 0, 0,
				39, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
				0x0d, 0x5c };
			const BYTE sendBuffer9[] = { 9, 0x7d, 46, 0, 0, 0, 0, 0,
				40, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				0x48, 0xb7 };
			const BYTE sendBuffer10[] = { 10, 0x7d, 46, 0, 0, 0, 0, 0,
				40, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				0xe9, 0xda };
			const BYTE sendBuffer11[] = { 11, 0x7d, 46, 0, 0, 0, 0, 0,
				40, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
				0x16, 0x69 };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const byte * const m_location;
				const byte m_locationLength;
				const byte * const m_address1;
				const byte m_address1Length;
				const byte * const m_address2;
				const byte m_address2Length;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
				const UINT m_sendBufferSizeExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, NULL, 0, NULL, 0, NULL, 0, sendBuffer0, sizeof(sendBuffer0) },
				{ 1, NULL, 0, NULL, 0, NULL, 33, sendBuffer1, sizeof(sendBuffer1) },
				{ 2, NULL, 0, NULL, 0, address2_2, 0, sendBuffer2, sizeof(sendBuffer2) },
				{ 3, NULL, 0, NULL, 0, address2_3, (byte)sizeof(address2_3), sendBuffer3, sizeof(sendBuffer3) },
				{ 4, NULL, 0, NULL, 0, address2_4, (byte)sizeof(address2_4), sendBuffer4, sizeof(sendBuffer4) },
				{ 5, NULL, 0, NULL, 0, address2_5, (byte)sizeof(address2_5), sendBuffer5, sizeof(sendBuffer5) },
				{ 6, NULL, 0, NULL, 0, address2_6, (byte)sizeof(address2_6), sendBuffer6, sizeof(sendBuffer6) },
				{ 7, NULL, 0, NULL, 0, address2_7, (byte)sizeof(address2_7), sendBuffer7, sizeof(sendBuffer7) },
				{ 8, NULL, 0, NULL, 0, address2_8, (byte)sizeof(address2_8), sendBuffer8, sizeof(sendBuffer8) },
				{ 9, NULL, 0, NULL, 0, address2_9, (byte)sizeof(address2_9), sendBuffer9, sizeof(sendBuffer9) },
				{ 10, NULL, 0, NULL, 0, address2_10, (byte)sizeof(address2_10), sendBuffer10, sizeof(sendBuffer10) },
				{ 11, NULL, 0, NULL, 0, address2_11, (byte)sizeof(address2_11), sendBuffer11, sizeof(sendBuffer11) },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CLongPoll7DMessage varMsg(TEST_DATA[testIndex].m_gameAddress);
				varMsg.SetAddressData(TEST_DATA[testIndex].m_location,
					TEST_DATA[testIndex].m_locationLength,
					TEST_DATA[testIndex].m_address1,
					TEST_DATA[testIndex].m_address1Length,
					TEST_DATA[testIndex].m_address2,
					TEST_DATA[testIndex].m_address2Length);
				varMsg.Fill();
				const CLongPoll7DMessage &msg(varMsg);

				// Test #1.
				const BYTE * const sendBufferResult(varMsg.GetSendBuffer());

				{
					const bool success(sendBufferResult != NULL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult=%p expected=NOT NULL testIndex=%d in %s(%d)",
							(void *)sendBufferResult,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #2.
				const UINT sendBufferSizeResult(varMsg.GetSendBufferSize());

				{
					const bool success(sendBufferSizeResult == TEST_DATA[testIndex].m_sendBufferSizeExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferSizeResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)sendBufferSizeResult,
							(unsigned)TEST_DATA[testIndex].m_sendBufferSizeExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #3.
				{
					const int memcmpResult = memcmp(sendBufferResult, TEST_DATA[testIndex].m_sendBufferExpected, TEST_DATA[testIndex].m_sendBufferSizeExpected);
					const bool success(memcmpResult == MEMCMP_EQUAL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult={%s} expected={%s} testIndex=%d in %s(%d)",
							(CUtilities::ToHexString(sendBufferResult, sendBufferSizeResult)).c_str(),
							(CUtilities::ToHexString(TEST_DATA[testIndex].m_sendBufferExpected, TEST_DATA[testIndex].m_sendBufferSizeExpected)).c_str(),
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7DMessage::CLongPoll7DMessage()
		///     CLongPoll7DMessage::SetAddressData()
		///     CLongPoll7DMessage::Fill()
		/// Code coverage for:
		///     CLongPoll7DMessage::~CLongPoll7DMessage() -- delete non-NULL members.
		/// Test SetAddressData() for correct values.
		/// </summary>
		TEST(LongPoll7DMessage, SetAddressDataTest)
		{
			// Expected results.
			const byte location2[] = { 0 }; // Use length of 0.
			const byte address1_2[] = { 0 }; // Use length of 0.
			const byte address2_2[] = { 0 }; // Use length of 0.

			const byte location3[] = { 1 };
			const byte address1_3[] = { 2 };
			const byte address2_3[] = { 3 };

			const byte location4[] = { 0xfc };
			const byte address1_4[] = { 0xfd };
			const byte address2_4[] = { 0xfe };

			const byte location5[] = { (byte)'a', 0x00, (byte)'b' }; // Array with embedded null.
			const byte address1_5[] = { (byte)'c', (byte)'d', 0x00, (byte)'e' }; // Array with embedded null.
			const byte address2_5[] = { (byte)'f', (byte)'g', 0x00, (byte)'h', (byte)'i' }; // Array with embedded null.

			const byte location6[] = { (byte)'a', (byte)'b', (byte)'c', 0x00 }; // Array with trailing null.
			const byte address1_6[] = { (byte)'d', (byte)'e', (byte)'f', (byte)'g', 0x00 }; // Array with trailing null.
			const byte address2_6[] = { (byte)'h', (byte)'i', (byte)'j', (byte)'k', (byte)'l', 0x00 }; // Array with trailing null.

			const byte location7[] = { (byte)'L', (byte)'M', (byte)'N', (byte)'O' };
			const byte address1_7[] = { (byte)'P', (byte)'Q', (byte)'R', (byte)'S', (byte)'T' };
			const byte address2_7[] = { (byte)'U', (byte)'V', (byte)'W', (byte)'X', (byte)'Y', (byte)'Z' };

			const byte location8[39] = {
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49 }; // 1 less than maximum stored length.
			const byte address1_8[39] = {
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				51, 52, 53, 54, 55, 56, 57, 58, 59 }; // 1 less than maximum stored length.
			const byte address2_8[39] = {
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
				61, 62, 63, 64, 65, 66, 67, 68, 69 }; // 1 less than maximum stored length.

			const byte location9[40] = {
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50 }; // Maximum stored length.
			const byte address1_9[40] = {
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				51, 52, 53, 54, 55, 56, 57, 58, 59, 60 }; // Maximum stored length.
			const byte address2_9[40] = {
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
				61, 62, 63, 64, 65, 66, 67, 68, 69, 70 }; // Maximum stored length.

			const byte location10[41] = {
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 }; // 1 more than maximum stored length.
			const byte address1_10[41] = {
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61 }; // 1 more than maximum stored length.
			const byte address2_10[41] = {
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
				61, 62, 63, 64, 65, 66, 67, 68, 69, 70,71 }; // 1 more than maximum stored length.

			const byte location11[255] = {
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e }; // Maximum parameter length.
			const byte address1_11[255] = {
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0xf,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e }; // Maximum parameter length.
			const byte address2_11[255] = {
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0xf,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e }; // Maximum parameter length.

			// Expected results -- per iteration.
			const BYTE sendBuffer0[] = { 0, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x69, 0x6a };
			const BYTE sendBuffer1[] = { 1, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x94, 0x27 };
			const BYTE sendBuffer2[] = { 2, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x93, 0xf1 };
			const BYTE sendBuffer3[] = { 3, 0x7d, 9, 0, 0, 0, 1, 1, 1, 2, 1, 3, 0x6b, 0xa6 };
			const BYTE sendBuffer4[] = { 4, 0x7d, 9, 0, 0, 0, 1, 0xfc, 1, 0xfd, 1, 0xfe, 0xb8, 0x2d };
			const BYTE sendBuffer5[] = { 5, 0x7d, 18, 0, 0, 0, 3, (byte)'a', 0, (byte)'b', 4, (byte)'c', (byte)'d', 0, (byte)'e', 5, (byte)'f', (byte)'g', 0, (byte)'h', (byte)'i', 0xc2, 0x86 };
			const BYTE sendBuffer6[] = { 6, 0x7d, 21, 0, 0, 0, 4, (byte)'a', (byte)'b', (byte)'c', 0, 5, (byte)'d', (byte)'e', (byte)'f', (byte)'g', 0, 6, (byte)'h', (byte)'i', (byte)'j', (byte)'k', (byte)'l', 0, 0x67, 0xc8 };
			const BYTE sendBuffer7[] = { 7, 0x7d, 21, 0, 0, 0, 4, (byte)'L', (byte)'M', (byte)'N', (byte)'O', 5, (byte)'P', (byte)'Q', (byte)'R', (byte)'S', (byte)'T', 6, (byte)'U', (byte)'V', (byte)'W', (byte)'X', (byte)'Y', (byte)'Z', 0x2f, 0xab };
			const BYTE sendBuffer8[] = { 8, 0x7d, 123, 0, 0, 0,
				39, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
				39, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
				39, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
				0x63, 0x6e };
			const BYTE sendBuffer9[] = { 9, 0x7d, 126, 0, 0, 0,
				40, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				40, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
				40, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
				0x1a, 0x07 };
			const BYTE sendBuffer10[] = { 10, 0x7d, 126, 0, 0, 0,
				40, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				40, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
				40, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
				0x97, 0xea };
			const BYTE sendBuffer11[] = { 11, 0x7d, 126, 0, 0, 0,
				40, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
				40, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
				40, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 
				0x2b, 0x7f };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const byte * const m_location;
				const byte m_locationLength;
				const byte * const m_address1;
				const byte m_address1Length;
				const byte * const m_address2;
				const byte m_address2Length;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
				const UINT m_sendBufferSizeExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, NULL, 0, NULL, 0, NULL, 0, sendBuffer0, sizeof(sendBuffer0) },
				{ 1, NULL, 33, NULL, 33, NULL, 33, sendBuffer1, sizeof(sendBuffer1) },
				{ 2, location2, 0, address1_2, 0, address2_2, 0, sendBuffer2, sizeof(sendBuffer2) },
				{ 3, location3, (byte)sizeof(location3), address1_3, sizeof(address1_3), address2_3, sizeof(address2_3), sendBuffer3, sizeof(sendBuffer3) },
				{ 4, location4, (byte)sizeof(location4), address1_4, sizeof(address1_4), address2_4, sizeof(address2_4), sendBuffer4, sizeof(sendBuffer4) },
				{ 5, location5, (byte)sizeof(location5), address1_5, sizeof(address1_5), address2_5, sizeof(address2_5), sendBuffer5, sizeof(sendBuffer5) },
				{ 6, location6, (byte)sizeof(location6), address1_6, sizeof(address1_6), address2_6, sizeof(address2_6), sendBuffer6, sizeof(sendBuffer6) },
				{ 7, location7, (byte)sizeof(location7), address1_7, sizeof(address1_7), address2_7, sizeof(address2_7), sendBuffer7, sizeof(sendBuffer7) },
				{ 8, location8, (byte)sizeof(location8), address1_8, sizeof(address1_8), address2_8, sizeof(address2_8), sendBuffer8, sizeof(sendBuffer8) },
				{ 9, location9, (byte)sizeof(location9), address1_9, sizeof(address1_9), address2_9, sizeof(address2_9), sendBuffer9, sizeof(sendBuffer9) },
				{ 10, location10, (byte)sizeof(location10), address1_10, sizeof(address1_10), address2_10, sizeof(address2_10), sendBuffer10, sizeof(sendBuffer10) },
				{ 11, location11, (byte)sizeof(location11), address1_11, sizeof(address1_11), address2_11, sizeof(address2_11), sendBuffer11, sizeof(sendBuffer11) },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CLongPoll7DMessage varMsg(TEST_DATA[testIndex].m_gameAddress);
				varMsg.SetAddressData(TEST_DATA[testIndex].m_location,
					TEST_DATA[testIndex].m_locationLength,
					TEST_DATA[testIndex].m_address1,
					TEST_DATA[testIndex].m_address1Length,
					TEST_DATA[testIndex].m_address2,
					TEST_DATA[testIndex].m_address2Length);
				varMsg.Fill();
				const CLongPoll7DMessage &msg(varMsg);

				// Test #1.
				const BYTE * const sendBufferResult(varMsg.GetSendBuffer());

				{
					const bool success(sendBufferResult != NULL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult=%p expected=NOT NULL testIndex=%d in %s(%d)",
							(void *)sendBufferResult,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #2.
				const UINT sendBufferSizeResult(varMsg.GetSendBufferSize());

				{
					const bool success(sendBufferSizeResult == TEST_DATA[testIndex].m_sendBufferSizeExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferSizeResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)sendBufferSizeResult,
							(unsigned)TEST_DATA[testIndex].m_sendBufferSizeExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #3.
				{
					const int memcmpResult = memcmp(sendBufferResult, TEST_DATA[testIndex].m_sendBufferExpected, TEST_DATA[testIndex].m_sendBufferSizeExpected);
					const bool success(memcmpResult == MEMCMP_EQUAL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult={%s} expected={%s} testIndex=%d in %s(%d)",
							(CUtilities::ToHexString(sendBufferResult, sendBufferSizeResult)).c_str(),
							(CUtilities::ToHexString(TEST_DATA[testIndex].m_sendBufferExpected, TEST_DATA[testIndex].m_sendBufferSizeExpected)).c_str(),
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CLongPoll7DMessage::CLongPoll7DMessage()
		///     CLongPoll7DMessage::SetAddressData()
		///     CLongPoll7DMessage::Fill()
		/// Code coverage for:
		///     CLongPoll7DMessage::~CLongPoll7DMessage() -- delete non-NULL members.
		/// Set address data twice, to perform cleanup of prior values.
		/// Test SetAddressData() for correct values overriding prior values.
		/// </summary>
		TEST(LongPoll7DMessage, SetAddressData2Test)
		{
			// Expected results.
			const byte location2[] = { 0 }; // Use length of 0.
			const byte address1_2[] = { 0 }; // Use length of 0.
			const byte address2_2[] = { 0 }; // Use length of 0.

			const byte location3[] = { 1 };
			const byte address1_3[] = { 2 };
			const byte address2_3[] = { 3 };

			const byte location4[] = { 0xfc };
			const byte address1_4[] = { 0xfd };
			const byte address2_4[] = { 0xfe };

			const byte location5[] = { (byte)'a', 0x00, (byte)'b' }; // Array with embedded null.
			const byte address1_5[] = { (byte)'c', (byte)'d', 0x00, (byte)'e' }; // Array with embedded null.
			const byte address2_5[] = { (byte)'f', (byte)'g', 0x00, (byte)'h', (byte)'i' }; // Array with embedded null.

			const byte location6[] = { (byte)'a', (byte)'b', (byte)'c', 0x00 }; // Array with trailing null.
			const byte address1_6[] = { (byte)'d', (byte)'e', (byte)'f', (byte)'g', 0x00 }; // Array with trailing null.
			const byte address2_6[] = { (byte)'h', (byte)'i', (byte)'j', (byte)'k', (byte)'l', 0x00 }; // Array with trailing null.

			const byte location7[] = { (byte)'L', (byte)'M', (byte)'N', (byte)'O' };
			const byte address1_7[] = { (byte)'P', (byte)'Q', (byte)'R', (byte)'S', (byte)'T' };
			const byte address2_7[] = { (byte)'U', (byte)'V', (byte)'W', (byte)'X', (byte)'Y', (byte)'Z' };

			const byte location8[39] = {
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49 }; // 1 less than maximum stored length.
			const byte address1_8[39] = {
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				51, 52, 53, 54, 55, 56, 57, 58, 59 }; // 1 less than maximum stored length.
			const byte address2_8[39] = {
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
				61, 62, 63, 64, 65, 66, 67, 68, 69 }; // 1 less than maximum stored length.

			const byte location9[40] = {
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50 }; // Maximum stored length.
			const byte address1_9[40] = {
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				51, 52, 53, 54, 55, 56, 57, 58, 59, 60 }; // Maximum stored length.
			const byte address2_9[40] = {
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
				61, 62, 63, 64, 65, 66, 67, 68, 69, 70 }; // Maximum stored length.

			const byte location10[41] = {
				11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 }; // 1 more than maximum stored length.
			const byte address1_10[41] = {
				21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61 }; // 1 more than maximum stored length.
			const byte address2_10[41] = {
				31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
				41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
				61, 62, 63, 64, 65, 66, 67, 68, 69, 70,71 }; // 1 more than maximum stored length.

			const byte location11[255] = {
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e }; // Maximum parameter length.
			const byte address1_11[255] = {
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0xf,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e }; // Maximum parameter length.
			const byte address2_11[255] = {
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0xf,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e }; // Maximum parameter length.

			// Expected results -- per iteration.
			const BYTE sendBuffer0[] = { 0, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x69, 0x6a };
			const BYTE sendBuffer1[] = { 1, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x94, 0x27 };
			const BYTE sendBuffer2[] = { 2, 0x7d, 6, 0, 0, 0, 0, 0, 0, 0x93, 0xf1 };
			const BYTE sendBuffer3[] = { 3, 0x7d, 9, 0, 0, 0, 1, 1, 1, 2, 1, 3, 0x6b, 0xa6 };
			const BYTE sendBuffer4[] = { 4, 0x7d, 9, 0, 0, 0, 1, 0xfc, 1, 0xfd, 1, 0xfe, 0xb8, 0x2d };
			const BYTE sendBuffer5[] = { 5, 0x7d, 18, 0, 0, 0, 3, (byte)'a', 0, (byte)'b', 4, (byte)'c', (byte)'d', 0, (byte)'e', 5, (byte)'f', (byte)'g', 0, (byte)'h', (byte)'i', 0xc2, 0x86 };
			const BYTE sendBuffer6[] = { 6, 0x7d, 21, 0, 0, 0, 4, (byte)'a', (byte)'b', (byte)'c', 0, 5, (byte)'d', (byte)'e', (byte)'f', (byte)'g', 0, 6, (byte)'h', (byte)'i', (byte)'j', (byte)'k', (byte)'l', 0, 0x67, 0xc8 };
			const BYTE sendBuffer7[] = { 7, 0x7d, 21, 0, 0, 0, 4, (byte)'L', (byte)'M', (byte)'N', (byte)'O', 5, (byte)'P', (byte)'Q', (byte)'R', (byte)'S', (byte)'T', 6, (byte)'U', (byte)'V', (byte)'W', (byte)'X', (byte)'Y', (byte)'Z', 0x2f, 0xab };
			const BYTE sendBuffer8[] = { 8, 0x7d, 123, 0, 0, 0,
				39, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
				39, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
				39, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
				0x63, 0x6e };
			const BYTE sendBuffer9[] = { 9, 0x7d, 126, 0, 0, 0,
				40, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				40, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
				40, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
				0x1a, 0x07 };
			const BYTE sendBuffer10[] = { 10, 0x7d, 126, 0, 0, 0,
				40, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
				40, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
				40, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
				0x97, 0xea };
			const BYTE sendBuffer11[] = { 11, 0x7d, 126, 0, 0, 0,
				40, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
				40, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
				40, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 
				0x2b, 0x7f };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const byte * const m_location;
				const byte m_locationLength;
				const byte * const m_address1;
				const byte m_address1Length;
				const byte * const m_address2;
				const byte m_address2Length;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
				const UINT m_sendBufferSizeExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, NULL, 0, NULL, 0, NULL, 0, sendBuffer0, sizeof(sendBuffer0) },
				{ 1, NULL, 33, NULL, 33, NULL, 33, sendBuffer1, sizeof(sendBuffer1) },
				{ 2, location2, 0, address1_2, 0, address2_2, 0, sendBuffer2, sizeof(sendBuffer2) },
				{ 3, location3, (byte)sizeof(location3), address1_3, sizeof(address1_3), address2_3, sizeof(address2_3), sendBuffer3, sizeof(sendBuffer3) },
				{ 4, location4, (byte)sizeof(location4), address1_4, sizeof(address1_4), address2_4, sizeof(address2_4), sendBuffer4, sizeof(sendBuffer4) },
				{ 5, location5, (byte)sizeof(location5), address1_5, sizeof(address1_5), address2_5, sizeof(address2_5), sendBuffer5, sizeof(sendBuffer5) },
				{ 6, location6, (byte)sizeof(location6), address1_6, sizeof(address1_6), address2_6, sizeof(address2_6), sendBuffer6, sizeof(sendBuffer6) },
				{ 7, location7, (byte)sizeof(location7), address1_7, sizeof(address1_7), address2_7, sizeof(address2_7), sendBuffer7, sizeof(sendBuffer7) },
				{ 8, location8, (byte)sizeof(location8), address1_8, sizeof(address1_8), address2_8, sizeof(address2_8), sendBuffer8, sizeof(sendBuffer8) },
				{ 9, location9, (byte)sizeof(location9), address1_9, sizeof(address1_9), address2_9, sizeof(address2_9), sendBuffer9, sizeof(sendBuffer9) },
				{ 10, location10, (byte)sizeof(location10), address1_10, sizeof(address1_10), address2_10, sizeof(address2_10), sendBuffer10, sizeof(sendBuffer10) },
				{ 11, location11, (byte)sizeof(location11), address1_11, sizeof(address1_11), address2_11, sizeof(address2_11), sendBuffer11, sizeof(sendBuffer11) },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CLongPoll7DMessage varMsg(TEST_DATA[testIndex].m_gameAddress);

				// Set values to be cleaned up by next call to SetAddressData().
				varMsg.SetAddressData(TEST_DATA[testIndex].m_address1,
					TEST_DATA[testIndex].m_address1Length,
					TEST_DATA[testIndex].m_address2,
					TEST_DATA[testIndex].m_address2Length,
					TEST_DATA[testIndex].m_location,
					TEST_DATA[testIndex].m_locationLength);

				// Set values to test.
				varMsg.SetAddressData(TEST_DATA[testIndex].m_location,
					TEST_DATA[testIndex].m_locationLength,
					TEST_DATA[testIndex].m_address1,
					TEST_DATA[testIndex].m_address1Length,
					TEST_DATA[testIndex].m_address2,
					TEST_DATA[testIndex].m_address2Length);
				varMsg.Fill();
				const CLongPoll7DMessage &msg(varMsg);

				// Test #1.
				const BYTE * const sendBufferResult(varMsg.GetSendBuffer());

				{
					const bool success(sendBufferResult != NULL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult=%p expected=NOT NULL testIndex=%d in %s(%d)",
							(void *)sendBufferResult,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #2.
				const UINT sendBufferSizeResult(varMsg.GetSendBufferSize());

				{
					const bool success(sendBufferSizeResult == TEST_DATA[testIndex].m_sendBufferSizeExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferSizeResult=%u expected=%u testIndex=%d in %s(%d)",
							(unsigned)sendBufferSizeResult,
							(unsigned)TEST_DATA[testIndex].m_sendBufferSizeExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #3.
				{
					const int memcmpResult = memcmp(sendBufferResult, TEST_DATA[testIndex].m_sendBufferExpected, TEST_DATA[testIndex].m_sendBufferSizeExpected);
					const bool success(memcmpResult == MEMCMP_EQUAL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult={%s} expected={%s} testIndex=%d in %s(%d)",
							(CUtilities::ToHexString(sendBufferResult, sendBufferSizeResult)).c_str(),
							(CUtilities::ToHexString(TEST_DATA[testIndex].m_sendBufferExpected, TEST_DATA[testIndex].m_sendBufferSizeExpected)).c_str(),
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}
}
