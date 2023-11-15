#include "stdafx.h"
#include "SASProtocol/LongPollB0B5Message.h"

#include "GlobalDefs.h"
#include "Utilities.h"

#include "gtest/gtest.h"

#define _I8_MIN     (-127 - 1)    // minimum signed 8 bit value
#define _I8_MAX       127         // maximum signed 8 bit value
#define _UI8_MAX      0xffu       // maximum unsigned 8 bit value

using namespace std;

namespace SentinelUnitTests
{
		/// Code coverage and functional test for:
		///     CLongPollB0B5MessageTest::CLongPollB0B5MessageTest()
		/// Code coverage:
		///     CLongPollB0B5MessageTest::~CLongPollB0B5MessageTest()
		/// Test for correct default values.
		/// </summary>
		TEST(LongPollB0B5Message, CLongPollB0B5MessageTest)
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
			const BYTE pollValueExpected(0xb0);
			const bool swapPollValueExpected(false);
			const UINT responseBufferSizeExpected(95);
			const BYTE responseBufferExpected[responseBufferSizeExpected] = {
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			const UINT sendBufferSizeExpected(9);
			const BYTE sendBufferExpected[sendBufferSizeExpected] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			const __int64 maxBetExpected(0);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CLongPollB0B5Message varMsg(TEST_DATA[testIndex].m_gameAddress, 1);
				const CLongPollB0B5Message&msg(varMsg);

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
					const int memcmpResult = memcmp(sendBufferResult, sendBufferExpected, sendBufferSizeExpected);
					const bool success(memcmpResult == MEMCMP_EQUAL);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("sendBufferResult={%s} expected={%s} testIndex=%d in %s(%d)",
							(CUtilities::ToHexString(sendBufferResult, sendBufferSizeResult)).c_str(),
							(CUtilities::ToHexString(sendBufferExpected, sendBufferSizeExpected)).c_str(),
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #9.
				{
					const __int64 maxBetResult = msg.GetMaxBet();
					const bool success(maxBetResult == maxBetExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("assetNumberResult=%lld expected=%lld testIndex=%d in %s(%d)",
							(long long)maxBetResult,
							(long long)maxBetExpected,
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
		///     CLongPollB0B5Message::CLongPollB0B5Message()
		///     CLongPollB0B5Message::Fill()
		/// Test for correct default values.
		/// </summary>
		TEST(LongPollB0B5Message, Fill_DefaultTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(9);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0xff, 0xb0, 4, 1, 0xb5, 0, 1, 0x0a, 0xdf };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 0x00, 0xb0, 4, 1, 0xb5, 0, 1, 0xaf, 0x51 };
			const BYTE sendBuffer2[sendBufferSizeExpected] = { 0x01, 0xb0, 4, 1, 0xb5, 0, 1, 0x7a, 0xce };
			const BYTE sendBuffer3[sendBufferSizeExpected] = { 0x02, 0xb0, 4, 1, 0xb5, 0, 1, 0x89, 0x8a };
			const BYTE sendBuffer4[sendBufferSizeExpected] = { 0x7e, 0xb0, 4, 1, 0xb5, 0, 1, 0xf8, 0x47 };
			const BYTE sendBuffer5[sendBufferSizeExpected] = { 0x7f, 0xb0, 4, 1, 0xb5, 0, 1, 0x43, 0x5b };
			const BYTE sendBuffer6[sendBufferSizeExpected] = { 0x80, 0xb0, 4, 1, 0xb5, 0, 1, 0x91, 0x9e };
			const BYTE sendBuffer7[sendBufferSizeExpected] = { 0x81, 0xb0, 4, 1, 0xb5, 0, 1, 0x2a, 0x82 };
			const BYTE sendBuffer8[sendBufferSizeExpected] = { 0xfe, 0xb0, 4, 1, 0xb5, 0, 1, 0x96, 0x6a };
			const BYTE sendBuffer9[sendBufferSizeExpected] = { 0xff, 0xb0, 4, 1, 0xb5, 0, 1, 0x2d, 0x76 };

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
				CLongPollB0B5Message varMsg(TEST_DATA[testIndex].m_gameAddress, 1);
				varMsg.SetDenom(1);
				varMsg.Fill();
				const CLongPollB0B5Message &msg(varMsg);

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
					const int memcmpResult = memcmp(sendBufferResult, TEST_DATA[testIndex].m_sendBufferExpected, sendBufferSizeExpected - 2);
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
		///     CLongPollB0B5Message::CLongPollB0B5Message()
		///     CLongPollB0B5Message::SetGameNumber()
		///     CLongPollB0B5Message::Fill()
		/// Test for correct default values.
		/// </summary>
		TEST(LongPollB0B5Message, Fill_GameNumberTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(9);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0xff, 0xb0, 4, 1, 0xb5, 0, 0, 0x83, 0xce };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 0x00, 0xb0, 4, 1, 0xb5, 0, 0, 0x83, 0xce };
			const BYTE sendBuffer2[sendBufferSizeExpected] = { 0x01, 0xb0, 4, 1, 0xb5, 0, 1, 0xcd, 0xbe };
			const BYTE sendBuffer3[sendBufferSizeExpected] = { 0x02, 0xb0, 4, 1, 0xb5, 0, 0x99, 0xc1, 0x83 };
			const BYTE sendBuffer4[sendBufferSizeExpected] = { 0x03, 0xb0, 4, 1, 0xb5, 1, 0, 0xea, 0x8f };
			const BYTE sendBuffer5[sendBufferSizeExpected] = { 0x04, 0xb0, 4, 1, 0xb5, 0x99, 0x99, 0x1e, 0x06 };

			typedef struct
			{
				// Inputs.
				const byte m_gameAddress;
				const int m_gameNumber;

				// Expected results.
				const BYTE * const m_sendBufferExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (byte)-1, -1, sendBuffer0 },
				{ 0, 0, sendBuffer1 },
				{ 1, 1, sendBuffer2 },
				{ 2, 99, sendBuffer3 },
				{ 3, 100, sendBuffer4 },
				{ 4, 9999, sendBuffer5 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CLongPollB0B5Message varMsg(TEST_DATA[testIndex].m_gameAddress, TEST_DATA[testIndex].m_gameNumber);
				varMsg.SetDenom(1);
				varMsg.Fill();
				const CLongPollB0B5Message &msg(varMsg);

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
					const int memcmpResult = memcmp(sendBufferResult, TEST_DATA[testIndex].m_sendBufferExpected, sendBufferSizeExpected - 2);
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
		/// CLongPollB0B5Message::ResponseReceived()
		/// </summary>
		TEST(LongPollB0B5Message, ResponseReceivedTest)
		{
			// Input(s) -- same for all loop iterations.
			const byte gameAddress(1);
			const byte pollValue(0xb5);

			// Expected result(s) -- same for all loop iterations.
			const UINT responseBufferSizeExpected(95);

			// Input(s): 1-byte responses.
			const byte responseBuffer1a[] = { 0x00 };
			const byte responseBuffer1b[] = { 0x01 };
			const byte responseBuffer1c[] = { 0x02 };
			const byte responseBuffer1d[] = { 0x7f };
			const byte responseBuffer1e[] = { 0x80 };
			const byte responseBuffer1f[] = { 0x81 };
			const byte responseBuffer1g[] = { 0x82 };
			const byte responseBuffer1h[] = { 0xfe };
			const byte responseBuffer1i[] = { 0xff };

			// Input(s): 2-byte responses.
			const byte responseBuffer2a[] = { gameAddress, 0xb0, 1, 1, 0x00 };
			const byte responseBuffer2b[] = { gameAddress, 0xb0, 1, 1, 0x01 };
			const byte responseBuffer2c[] = { gameAddress, 0xb0, 1, 1, 0xb4 };
			const byte responseBuffer2d[] = { gameAddress, 0xb0, 1, 1, pollValue };
			const byte responseBuffer2e[] = { gameAddress, 0xb0, 1, 1, 0xb6 };
			const byte responseBuffer2f[] = { gameAddress, 0xb0, 1, 1, 0x7f };
			const byte responseBuffer2g[] = { gameAddress, 0xb0, 1, 1, 0x80 };
			const byte responseBuffer2h[] = { gameAddress, 0xb0, 1, 1, 0xff };

			// Input(s): Short responses with variable length field set to 0.
			const byte responseBuffer3a[] = { gameAddress, 0xb0, 1, 1, pollValue, 0x00 };
			const byte responseBuffer3d[] = { gameAddress, 0xb0, 4, 1, pollValue, 0x00, 0x00, 0x00, 0xcc };
			const byte responseBuffer3e[] = { gameAddress, 0xb0, 5, 1, pollValue, 0x00, 0x00, 0x00, 0xcc, 0xcd }; // Minimum length to include the max bet field.
			const byte responseBuffer3f[] = { gameAddress, 0xb0, 6, 1, pollValue, 0x00, 0x00, 0x00, 0x03, 0xcc, 0xcd };
			const byte responseBuffer3g[] = { gameAddress, 0xb0, 7, 1, pollValue, 0x00, 0x00, 0x00, 0x03, 0x04, 0xcc, 0xcd }; // Minimum length to include the max bet field and crc.
			const byte responseBuffer3h[] = { gameAddress, 0xb0, 13, 1, pollValue, 0x00, 0x00, 0x00, 0x03, 0x04, 0x03, 0x04, 0x00 ,0x00, 0x00, 0x00, 0xcc, 0xcd }; // Minimum length to include the progressive group field and crc.
			const byte responseBuffer3i[] = { gameAddress, 0xb0, 13, 1, pollValue, 0x00, 0x00, 0x00, 0x03, 0x04, 0x03, 0x05, 0x00 ,0x00, 0x00, 0x06, 0xcc, 0xcd };
			const byte responseBuffer3j[] = { gameAddress, 0xb0, 13, 1, pollValue, 0x00, 0x00, 0x00, 0x03, 0x04, 0x03, 0x06, 0x00 ,0x00, 0x00, 0x07, 0xcc, 0xcd };
			const byte responseBuffer3k[] = { gameAddress, 0xb0, 13, 1, pollValue, 0x00, 0x00, 0x00, 0x03, 0x04, 0x03, 0x07, 0x00, 0x00, 0x00, 0x08, 0xcc, 0xcd };
			const byte responseBuffer3L[18] = { gameAddress, 0xb0, 13, 1, pollValue,0x00, 0x00, 0x00, 0x03, 0x04, 0x03, 0x08, 0x00, 0x00, 0x00, 0x09, 0xcc, 0xcd }; // Minimum length for a valid B5 response (14 bytes total).

			// Input(s): Short responses with variable length field set to 0xff.
			const byte responseBuffer4a[] = { gameAddress, 0xb0, 1, 1, pollValue, 0xff };
			const byte responseBuffer4d[] = { gameAddress, 0xb0, 4, 1, pollValue, 0xff, 0x00, 0x00, 0x03 };
			const byte responseBuffer4e[] = { gameAddress, 0xb0, 5, 1, pollValue, 0xff, 0x00, 0x00, 0x03, 0xcc }; // Minimum length to include the max bet field.
			const byte responseBuffer4f[] = { gameAddress, 0xb0, 6, 1, pollValue, 0xff, 0x00, 0x00, 0x03, 0xcc, 0xcd };
			const byte responseBuffer4g[] = { gameAddress, 0xb0, 7, 1, pollValue, 0xff, 0x00, 0x00, 0x03, 0x04, 0xcc, 0xcd }; // Minimum length to include the max bet field and crc.
			const byte responseBuffer4h[] = { gameAddress, 0xb0, 14, 1, pollValue, 0xff, 0x00, 0x00, 0x03, 0x04, 0x05, 0x04, 0x00 ,0x00, 0x00, 0x00, 0x00, 0xcc, 0xcd }; // Minimum length to include the progressive group field and crc.
			const byte responseBuffer4i[] = { gameAddress, 0xb0, 14, 1, pollValue, 0xff, 0x00, 0x00, 0x03, 0x04, 0x05, 0x05, 0x00 ,0x00, 0x00, 0x00, 0x06, 0xcc, 0xcd };
			const byte responseBuffer4j[] = { gameAddress, 0xb0, 14, 1, pollValue, 0xff, 0x00, 0x00, 0x03, 0x04, 0x05, 0x06, 0x00 ,0x00, 0x00, 0x00, 0x07, 0xcc, 0xcd };
			const byte responseBuffer4k[] = { gameAddress, 0xb0, 14, 1, pollValue, 0xff, 0x00, 0x00, 0x03, 0x04, 0x05, 0x07, 0x00 ,0x00, 0x00, 0x00, 0x08, 0xcc, 0xcd };
			const byte responseBuffer4L[18] = { gameAddress, 0xb0, 13, 1, pollValue, 0xff,0x00, 0x00, 0x03, 0x04, 0x05, 0x08, 0x00, 0x00, 0x00, 0x00,0x00, 0xcd }; // Minimum length for a valid B5 response (14 bytes total).


			typedef struct
			{
				// Input(s).
				const int m_gameNumber;
				const byte * const m_responseBuffer;
				const UINT m_responseBufferSize;
				const int m_returnValue;

				// Expected result(s).
				const int m_returnValueExpected;
				const __int64 m_maxBetExpected;
				const byte m_progressiveGroupExpected;
				const DWORD m_levelIds;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ 0, NULL, 0, SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },
				{ 0, NULL, 0, SASPOLL_INCOMPLETE_DATA, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, NULL, 0, SASPOLL_NACK, SASPOLL_NACK, 0, 0, 0 },
				{ 0, NULL, 0, SASPOLL_DATA_TOO_LARGE, SASPOLL_DATA_TOO_LARGE, 0, 0, 0 },

				{ 0, responseBuffer1a, sizeof(responseBuffer1a), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },
				{ 0, responseBuffer1b, sizeof(responseBuffer1b), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },
				{ 0, responseBuffer1c, sizeof(responseBuffer1c), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },
				{ 0, responseBuffer1d, sizeof(responseBuffer1d), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },
				{ 0, responseBuffer1e, sizeof(responseBuffer1e), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },
				{ 0, responseBuffer1f, sizeof(responseBuffer1f), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },
				{ 0, responseBuffer1g, sizeof(responseBuffer1g), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },
				{ 0, responseBuffer1h, sizeof(responseBuffer1h), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },
				{ 0, responseBuffer1i, sizeof(responseBuffer1i), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },

				{ 0, responseBuffer2a, sizeof(responseBuffer2a), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },
				{ 0, responseBuffer2b, sizeof(responseBuffer2b), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },
				{ 0, responseBuffer2c, sizeof(responseBuffer2c), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },
				{ 0, responseBuffer2d, sizeof(responseBuffer2d), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer2e, sizeof(responseBuffer2e), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },
				{ 0, responseBuffer2f, sizeof(responseBuffer2f), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },
				{ 0, responseBuffer2g, sizeof(responseBuffer2g), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },
				{ 0, responseBuffer2h, sizeof(responseBuffer2h), SASPOLL_SUCCESS, SASPOLL_NACK, 0, 0, 0 },

				{ 0, responseBuffer3a, sizeof(responseBuffer3a), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer3d, sizeof(responseBuffer3d), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer3e, sizeof(responseBuffer3e), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer3f, sizeof(responseBuffer3f), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer3g, sizeof(responseBuffer3g), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer3h, sizeof(responseBuffer3h), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 3, 4 },
				{ 0, responseBuffer3i, sizeof(responseBuffer3i), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 3, 5 },
				{ 0, responseBuffer3j, sizeof(responseBuffer3j), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 3, 6 },
				{ 0, responseBuffer3k, sizeof(responseBuffer3k), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 3, 7 },
				{ 0, responseBuffer3L, sizeof(responseBuffer3L), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 3, 8 },
				
				{ 0, responseBuffer4a, sizeof(responseBuffer4a), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer4d, sizeof(responseBuffer4d), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer4e, sizeof(responseBuffer4e), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer4f, sizeof(responseBuffer4f), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer4g, sizeof(responseBuffer4g), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer4h, sizeof(responseBuffer4h), SASPOLL_SUCCESS, SASPOLL_SUCCESS, 304, 5, 4 },
				{ 0, responseBuffer4i, sizeof(responseBuffer4i), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 5, 5 },
				{ 0, responseBuffer4j, sizeof(responseBuffer4j), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 5, 6 },
				{ 0, responseBuffer4k, sizeof(responseBuffer4k), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 5, 7 },
				{ 0, responseBuffer4L, sizeof(responseBuffer4L), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 5, 8 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPollB0B5Message varMsg(gameAddress, TEST_DATA[testIndex].m_gameNumber);
				varMsg.SetDenom(1);
				varMsg.Fill();
				BYTE * const responseBuffer(varMsg.GetResponseBuffer());
				const UINT responseBufferSize(varMsg.GetResponseBufferSize());

				// Verify setup.
				{

					ASSERT_TRUE(responseBuffer != nullptr);
					ASSERT_TRUE(responseBufferSize == responseBufferSizeExpected);
				}

				// Perform operation(s) to be tested.
				memcpy(responseBuffer,
					TEST_DATA[testIndex].m_responseBuffer,
					min(responseBufferSizeExpected, TEST_DATA[testIndex].m_responseBufferSize));
				varMsg.SetResponseBuffer(responseBuffer, TEST_DATA[testIndex].m_responseBufferSize);
				varMsg.SetReturnValue(TEST_DATA[testIndex].m_returnValue);
				ASSERT_TRUE(varMsg.GetResponseBufferSize() == TEST_DATA[testIndex].m_responseBufferSize) <<  "varMsg.GetResponseBufferSize()";
				ASSERT_TRUE(varMsg.GetReturnValue() == TEST_DATA[testIndex].m_returnValue) <<  "varMsg.GetReturnValue()";
				varMsg.ResponseReceived();

				const int returnValueResult = varMsg.GetReturnValue();

				// Test #1.
				{
					const __int64 maxBetResult = varMsg.GetMaxBet();
					const bool success(maxBetResult == TEST_DATA[testIndex].m_maxBetExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("maxBetResult=%lld expected=%lld testIndex=%d in %s(%d)",
							(long long)maxBetResult,
							(long long)TEST_DATA[testIndex].m_maxBetExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #2.
				{
					const byte progressiveGroupResult = varMsg.GetProgressiveGroup();
					const bool success(progressiveGroupResult == TEST_DATA[testIndex].m_progressiveGroupExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("progressiveGroupResult=%d expected=%d testIndex=%d in %s(%d)",
							progressiveGroupResult,
							TEST_DATA[testIndex].m_progressiveGroupExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #3.
				{
					DWORD levelIdsResult = varMsg.GetLevelIds();
					const bool success(levelIdsResult == TEST_DATA[testIndex].m_levelIds);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("levelIdsResult=%d expected=%d testIndex=%d in %s(%d)",
							levelIdsResult,
							TEST_DATA[testIndex].m_levelIds,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) << failedMsg;
					}
				}
				// Cleanup.
			}

		}

}
