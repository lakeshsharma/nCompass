#include "stdafx.h"
#include "SASProtocol/LongPollB5Message.h"

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
		///     CLongPollB5Message::CLongPollB5Message()
		/// Code coverage:
		///     CLongPollB5Message::~CLongPollB5Message()
		/// Test for correct default values.
		/// </summary>
		TEST(LongPollB5Message, CLongPollB5MessageTest)
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
			const BYTE pollValueExpected(0xb5);
			const bool swapPollValueExpected(false);
			const UINT responseBufferSizeExpected(92);
			const BYTE responseBufferExpected[responseBufferSizeExpected] = {
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0 };
			const UINT sendBufferSizeExpected(6);
			const BYTE sendBufferExpected[sendBufferSizeExpected] = { 0, 0, 0, 0, 0, 0 };
			const __int64 maxBetExpected(0);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CLongPollB5Message varMsg(TEST_DATA[testIndex].m_gameAddress);
				const CLongPollB5Message &msg(varMsg);

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

				// Test #9.  Verify that the response is not already received.
/*
                {                                                                                                  
                    const HANDLE hResponseEventHandle = msg.GetResponseEventHandle();                              
                    DWORD waitForSingleObjectResult = WaitForSingleObject(hResponseEventHandle, 0);                
                    const bool success(waitForSingleObjectResult == waitForSingleObjectExpected);                  
                    if (!success)                                                                                  
                    {                                                                                              
                        string failedMsg;                                                                          
                        failedMsg = FormatString("waitForSingleObjectResult=%u expected=%u testIndex=%d in %s(%d)",
                            (unsigned)waitForSingleObjectResult,                                                   
                            (unsigned)waitForSingleObjectExpected,                                                 
                            testIndex,                                                                             
                            (__TFUNCTION__), __LINE__);                                                            
                        ASSERT_TRUE(success) <<  failedMsg;                                                        
                    }                                                                                              
                }                                                                                                  
                                                                                                                   
*/
				// Test #10.
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
		///     CLongPollB5Message::CLongPollB5Message()
		///     CLongPollB5Message::Fill()
		/// Test for correct default values.
		/// </summary>
		TEST(LongPollB5Message, Fill_DefaultTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(6);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0xff, 0xb5, 0, 0, 0x2d, 0x76 };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 0x00, 0xb5, 0, 0, 0xff, 0xb3 };
			const BYTE sendBuffer2[sendBufferSizeExpected] = { 0x01, 0xb5, 0, 0, 0x44, 0xaf };
			const BYTE sendBuffer3[sendBufferSizeExpected] = { 0x02, 0xb5, 0, 0, 0x89, 0x8a };
			const BYTE sendBuffer4[sendBufferSizeExpected] = { 0x7e, 0xb5, 0, 0, 0xf8, 0x47 };
			const BYTE sendBuffer5[sendBufferSizeExpected] = { 0x7f, 0xb5, 0, 0, 0x43, 0x5b };
			const BYTE sendBuffer6[sendBufferSizeExpected] = { 0x80, 0xb5, 0, 0, 0x91, 0x9e };
			const BYTE sendBuffer7[sendBufferSizeExpected] = { 0x81, 0xb5, 0, 0, 0x2a, 0x82 };
			const BYTE sendBuffer8[sendBufferSizeExpected] = { 0xfe, 0xb5, 0, 0, 0x96, 0x6a };
			const BYTE sendBuffer9[sendBufferSizeExpected] = { 0xff, 0xb5, 0, 0, 0x2d, 0x76 };

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
				CLongPollB5Message varMsg(TEST_DATA[testIndex].m_gameAddress);
				varMsg.Fill();
				const CLongPollB5Message &msg(varMsg);

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
		///     CLongPollB5Message::CLongPollB5Message()
		///     CLongPollB5Message::SetGameNumber()
		///     CLongPollB5Message::Fill()
		/// Test for correct default values.
		/// </summary>
		TEST(LongPollB5Message, Fill_GameNumberTest)
		{
			// Expected results -- same for all loop iterations.
			const UINT sendBufferSizeExpected(6);

			// Expected results -- per iteration.
			const BYTE sendBuffer0[sendBufferSizeExpected] = { 0xff, 0xb5, 0, 0, 0x2d, 0x76 };
			const BYTE sendBuffer1[sendBufferSizeExpected] = { 0x00, 0xb5, 0, 0, 0xff, 0xb3 };
			const BYTE sendBuffer2[sendBufferSizeExpected] = { 0x01, 0xb5, 0, 1, 0xcd, 0xbe };
			const BYTE sendBuffer3[sendBufferSizeExpected] = { 0x02, 0xb5, 0, 0x99, 0xc1, 0x83 };
			const BYTE sendBuffer4[sendBufferSizeExpected] = { 0x03, 0xb5, 1, 0, 0xea, 0x8f };
			const BYTE sendBuffer5[sendBufferSizeExpected] = { 0x04, 0xb5, 0x99, 0x99, 0x1e, 0x06 };
			const BYTE sendBuffer6[sendBufferSizeExpected] = { 0x05, 0xb5, 0, 0, 0xa8, 0xdd };
			const BYTE sendBuffer7[sendBufferSizeExpected] = { 0x06, 0xb5, 0, 0, 0x65, 0xf8 };
			const BYTE sendBuffer8[sendBufferSizeExpected] = { 0x7e, 0xb5, 0, 0, 0xf8, 0x47 };
			const BYTE sendBuffer9[sendBufferSizeExpected] = { 0x7f, 0xb5, 0, 0, 0x43, 0x5b };
			const BYTE sendBuffer10[sendBufferSizeExpected] = { 0x80, 0xb5, 0, 0, 0x91, 0x9e };
			const BYTE sendBuffer11[sendBufferSizeExpected] = { 0x81, 0xb5, 0, 0, 0x2a, 0x82 };
			const BYTE sendBuffer12[sendBufferSizeExpected] = { 0xfe, 0xb5, 0, 0, 0x96, 0x6a };
			const BYTE sendBuffer13[sendBufferSizeExpected] = { 0xff, 0xb5, 0, 0, 0x2d, 0x76 };

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
				{ 5, 10000, sendBuffer6 },
				{ 6, 10001, sendBuffer7 },
                { (byte)(_I8_MAX - 1), INT_MAX - 1, sendBuffer8 },    
                { (byte)_I8_MAX, INT_MAX, sendBuffer9 },              
                { (byte)_I8_MIN, INT_MIN, sendBuffer10 },             
                { (byte)(_I8_MIN + 1), INT_MIN + 1, sendBuffer11 },   
                { (byte)(_UI8_MAX - 1), (int)(UINT_MAX -1), sendBuffer12 },
                { (byte)_UI8_MAX, (int)(UINT_MAX), sendBuffer13 },           
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CLongPollB5Message varMsg(TEST_DATA[testIndex].m_gameAddress);
				varMsg.SetGameNumber(TEST_DATA[testIndex].m_gameNumber);
				varMsg.Fill();
				const CLongPollB5Message &msg(varMsg);

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
		/// CLongPollB5Message::ResponseReceived()
		/// </summary>
		TEST(LongPollB5Message, ResponseReceivedTest)
		{
			// Input(s) -- same for all loop iterations.
			const byte gameAddress(1);
			const byte pollValue(0xb5);

			// Expected result(s) -- same for all loop iterations.
			const UINT responseBufferSizeExpected(92);

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
			const byte responseBuffer2a[] = { gameAddress, 0x00 };
			const byte responseBuffer2b[] = { gameAddress, 0x01 };
			const byte responseBuffer2c[] = { gameAddress, 0xb4 };
			const byte responseBuffer2d[] = { gameAddress, pollValue };
			const byte responseBuffer2e[] = { gameAddress, 0xb6 };
			const byte responseBuffer2f[] = { gameAddress, 0x7f };
			const byte responseBuffer2g[] = { gameAddress, 0x80 };
			const byte responseBuffer2h[] = { gameAddress, 0xff };

			// Input(s): Short responses with variable length field set to 0.
			const byte responseBuffer3a[] = { gameAddress, pollValue, 0x00 };
			const byte responseBuffer3b[] = { gameAddress, pollValue, 0x00, 0x00 };
			const byte responseBuffer3c[] = { gameAddress, pollValue, 0x00, 0x00, 0x00 };
			const byte responseBuffer3d[] = { gameAddress, pollValue, 0x00, 0x00, 0x00, 0xcc };
			const byte responseBuffer3e[] = { gameAddress, pollValue, 0x00, 0x00, 0x00, 0xcc, 0xcd }; // Minimum length to include the max bet field.
			const byte responseBuffer3f[] = { gameAddress, pollValue, 0x00, 0x00, 0x00, 0x03, 0xcc, 0xcd };
			const byte responseBuffer3g[] = { gameAddress, pollValue, 0x00, 0x00, 0x00, 0x03, 0x04, 0xcc, 0xcd }; // Minimum length to include the max bet field and crc.
			const byte responseBuffer3h[] = { gameAddress, pollValue, 0x00, 0x00, 0x00, 0x03, 0x04, 0x05, 0x03,0x00 ,0x00, 0x00, 0x00, 0xcc, 0xcd }; // Minimum length to include the progressive group field and crc.
			const byte responseBuffer3i[] = { gameAddress, pollValue, 0x00, 0x00, 0x00, 0x03, 0x04, 0x05, 0x03,0x00 ,0x00, 0x00, 0x06, 0xcc, 0xcd };
			const byte responseBuffer3j[] = { gameAddress, pollValue, 0x00, 0x00, 0x00, 0x03, 0x04, 0x05, 0x03,0x00 ,0x00, 0x06, 0x07, 0xcc, 0xcd };
			const byte responseBuffer3k[] = { gameAddress, pollValue, 0x00, 0x00, 0x00, 0x03, 0x04, 0x05, 0x03 ,0x00, 0x06, 0x07, 0x08, 0xcc, 0xcd };
			const byte responseBuffer3L[15] = { gameAddress, pollValue,0x00, 0x00, 0x00, 0x03, 0x04, 0x05, 0x03, 0x06, 0x07, 0x08, 0x09, 0xcc, 0xcd }; // Minimum length for a valid B5 response (14 bytes total).

			// Input(s): Short responses with variable length field set to 0xff.
			const byte responseBuffer4a[] = { gameAddress, pollValue, 0xff };
			const byte responseBuffer4b[] = { gameAddress, pollValue, 0xff, 0x00 };
			const byte responseBuffer4c[] = { gameAddress, pollValue, 0xff, 0x00, 0x00 };
			const byte responseBuffer4d[] = { gameAddress, pollValue, 0xff, 0x00, 0x00, 0x03 };
			const byte responseBuffer4e[] = { gameAddress, pollValue, 0xff, 0x00, 0x00, 0x03, 0xcc }; // Minimum length to include the max bet field.
			const byte responseBuffer4f[] = { gameAddress, pollValue, 0xff, 0x00, 0x00, 0x03, 0xcc, 0xcd };
			const byte responseBuffer4g[] = { gameAddress, pollValue, 0xff, 0x00, 0x00, 0x03, 0x04, 0xcc, 0xcd }; // Minimum length to include the max bet field and crc.
			const byte responseBuffer4h[] = { gameAddress, pollValue, 0xff, 0x00, 0x00, 0x03, 0x04, 0x05, 0x04,0x00 ,0x00, 0x00, 0x00, 0x00, 0xcc, 0xcd }; // Minimum length to include the progressive group field and crc.
			const byte responseBuffer4i[] = { gameAddress, pollValue, 0xff, 0x00, 0x00, 0x03, 0x04, 0x05, 0x04,0x00 ,0x00, 0x00, 0x00, 0x06, 0xcc, 0xcd };
			const byte responseBuffer4j[] = { gameAddress, pollValue, 0xff, 0x00, 0x00, 0x03, 0x04, 0x05, 0x04,0x00 ,0x00, 0x00, 0x06, 0x07, 0xcc, 0xcd };
			const byte responseBuffer4k[] = { gameAddress, pollValue, 0xff, 0x00, 0x00, 0x03, 0x04, 0x05, 0x04,0x00 ,0x00, 0x06, 0x07, 0x08, 0xcc, 0xcd };
			const byte responseBuffer4L[15] = { gameAddress, pollValue,0xff,0x00, 0x00, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xcc,0x00, 0xcd }; // Minimum length for a valid B5 response (14 bytes total).

			// Input(s): Minimum length (for a valid B5 response),  with variable length field set to 0x00.
			// 2 BCD bytes for game number, 2 BCD bytes for max bet, 1 byte for progressive group, 4 bytes for progressive level flags.
			const byte responseBuffer5a[15] = { gameAddress, pollValue, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcd };
			const byte responseBuffer5b[15] = { gameAddress, pollValue, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x01, 0x00, 0xff, 0xff, 0x00, 0xcc, 0xcd };
			const byte responseBuffer5c[15] = { gameAddress, pollValue, 0x00, 0x00, 0x01, 0x00, 0x99, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcd };
			const byte responseBuffer5d[15] = { gameAddress, pollValue, 0x00, 0x00, 0x99, 0x01, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcd };
			const byte responseBuffer5e[15] = { gameAddress, pollValue, 0x00, 0x01, 0x00, 0x99, 0x99, 0x7f, 0x04, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcd };
			const byte responseBuffer5f[15] = { gameAddress, pollValue, 0x00, 0x99, 0x99, 0x99, 0x9a, 0x80, 0x05, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcd };
			const byte responseBuffer5g[15] = { gameAddress, pollValue, 0x00, 0x99, 0x9a, 0x99, 0xff, 0x81, 0x06, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcd };
			const byte responseBuffer5h[15] = { gameAddress, pollValue, 0x00, 0x99, 0xff, 0xff, 0xff, 0xfd, 0x07, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcd };
			const byte responseBuffer5i[15] = { gameAddress, pollValue, 0x00, 0xff, 0xff, 0x00, 0x00, 0xfe, 0x08, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcd };

			// Input(s): Minimum length (for a valid B5 response),  with variable length field set to 0xff.
			// 2 BCD bytes for game number, 2 BCD bytes for max bet, 1 byte for progressive group, 4 bytes for progressive level flags.
			const byte responseBuffer6a[15] = { gameAddress, pollValue, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcd };
			const byte responseBuffer6b[15] = { gameAddress, pollValue, 0xff, 0x00, 0x00, 0x00, 0x01, 0xff, 0x01, 0x00, 0xff, 0xff, 0xcc, 0xcd };
			const byte responseBuffer6c[15] = { gameAddress, pollValue, 0xff, 0x00, 0x01, 0x00, 0x99, 0x00, 0x02, 0x00, 0x00, 0x00, 0xcc, 0xcd };
			const byte responseBuffer6d[15] = { gameAddress, pollValue, 0xff, 0x00, 0x99, 0x01, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0xcc, 0xcd };
			const byte responseBuffer6e[15] = { gameAddress, pollValue, 0xff, 0x01, 0x00, 0x99, 0x99, 0x7f, 0x04, 0x00, 0x00, 0x00, 0xcc, 0xcd };
			const byte responseBuffer6f[15] = { gameAddress, pollValue, 0xff, 0x99, 0x99, 0x99, 0x9a, 0x80, 0x05, 0x00, 0x00, 0x00, 0xcc, 0xcd };
			const byte responseBuffer6g[15] = { gameAddress, pollValue, 0xff, 0x99, 0x9a, 0x99, 0xff, 0x81, 0x06, 0x00, 0x00, 0x00, 0xcc, 0xcd };
			const byte responseBuffer6h[15] = { gameAddress, pollValue, 0xff, 0x99, 0xff, 0xff, 0xff, 0xfd, 0x07, 0x00, 0x00, 0x00, 0xcc, 0xcd };
			const byte responseBuffer6i[15] = { gameAddress, pollValue, 0xff, 0xff, 0xff, 0x00, 0x00, 0xfe, 0x08, 0x00, 0x00, 0x00, 0xcc, 0xcd };

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
				{ 0, responseBuffer3b, sizeof(responseBuffer3b), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer3c, sizeof(responseBuffer3c), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer3d, sizeof(responseBuffer3d), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer3e, sizeof(responseBuffer3e), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer3f, sizeof(responseBuffer3f), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer3g, sizeof(responseBuffer3g), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer3h, sizeof(responseBuffer3h), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 5, 3 },
				{ 0, responseBuffer3i, sizeof(responseBuffer3i), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 5, 3 },
				{ 0, responseBuffer3j, sizeof(responseBuffer3j), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 5, 3 },
				{ 0, responseBuffer3k, sizeof(responseBuffer3k), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 5, 3 },
				{ 0, responseBuffer3L, sizeof(responseBuffer3L), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 5, 3 },
				
				{ 0, responseBuffer4a, sizeof(responseBuffer4a), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer4b, sizeof(responseBuffer4b), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer4c, sizeof(responseBuffer4c), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer4d, sizeof(responseBuffer4d), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer4e, sizeof(responseBuffer4e), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer4f, sizeof(responseBuffer4f), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer4g, sizeof(responseBuffer4g), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer4h, sizeof(responseBuffer4h), SASPOLL_SUCCESS, SASPOLL_SUCCESS, 304, 5, 4 },
				{ 0, responseBuffer4i, sizeof(responseBuffer4i), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 5, 4 },
				{ 0, responseBuffer4j, sizeof(responseBuffer4j), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 5, 4 },
				{ 0, responseBuffer4k, sizeof(responseBuffer4k), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 5, 4 },
				{ 0, responseBuffer4L, sizeof(responseBuffer4L), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 304, 5, 6 },

				{ 0, responseBuffer5a, sizeof(responseBuffer5a), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer5b, sizeof(responseBuffer5b), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 1, 255, 1 },
				{ 1, responseBuffer5c, sizeof(responseBuffer5c), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 99, 0, 2 },
				{ 99, responseBuffer5d, sizeof(responseBuffer5d), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 100, 1, 3 },
				{ 100, responseBuffer5e, sizeof(responseBuffer5e), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 9999, 127, 4 },
				{ 9999, responseBuffer5f, sizeof(responseBuffer5f), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 10000, 128, 5 },
				{ 10000, responseBuffer5g, sizeof(responseBuffer5g), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 10065, 129, 6 },
				{ 10065, responseBuffer5h, sizeof(responseBuffer5h), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 16665, 253, 7 },
				{ 16665, responseBuffer5i, sizeof(responseBuffer5i), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 254, 8 },

				{ 0, responseBuffer6a, sizeof(responseBuffer6a), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 0, 0 },
				{ 0, responseBuffer6b, sizeof(responseBuffer6b), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 1, 255, 1 },
				{ 1, responseBuffer6c, sizeof(responseBuffer6c), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 99, 0, 2 },
				{ 99, responseBuffer6d, sizeof(responseBuffer6d), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 100, 1, 3 },
				{ 100, responseBuffer6e, sizeof(responseBuffer6e), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 9999, 127, 4 },
				{ 9999, responseBuffer6f, sizeof(responseBuffer6f), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 10000, 128, 5 },
				{ 10000, responseBuffer6g, sizeof(responseBuffer6g), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 10065, 129, 6 },
				{ 10065, responseBuffer6h, sizeof(responseBuffer6h), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 16665, 253, 7 },
				{ 16665, responseBuffer6i, sizeof(responseBuffer6i), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, 0, 254, 8 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPollB5Message varMsg(gameAddress);
				varMsg.SetGameNumber(TEST_DATA[testIndex].m_gameNumber);
				varMsg.Fill();
				const CLongPollB5Message &msg(varMsg);
				BYTE * const responseBuffer(varMsg.GetResponseBuffer());
				const UINT responseBufferSize(varMsg.GetResponseBufferSize());

				// Verify setup.
				{
/*
                    const HANDLE hResponseEventHandle = msg.GetResponseEventHandle();            
                    ASSERT_TRUE(WAIT_TIMEOUT == WaitForSingleObject(hResponseEventHandle) <<  0);
*/

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

				// Test #1.  Verify that the response is already received.
/*
                {                                                                                                  
                    const HANDLE hResponseEventHandle = msg.GetResponseEventHandle();                              
                    DWORD waitForSingleObjectResult = WaitForSingleObject(hResponseEventHandle, 0);                
                    const bool success(waitForSingleObjectResult == waitForSingleObjectExpected);                  
                    if (!success)                                                                                  
                    {                                                                                              
                        string failedMsg;                                                                          
                        failedMsg = FormatString("waitForSingleObjectResult=%u expected=%u testIndex=%d in %s(%d)",
                            (unsigned)waitForSingleObjectResult,                                                   
                            (unsigned)waitForSingleObjectExpected,                                                 
                            testIndex,                                                                             
                            (__TFUNCTION__), __LINE__);                                                            
                        ASSERT_TRUE(success) <<  failedMsg;                                                        
                    }                                                                                              
                }                                                                                                  
*/

				const int returnValueResult = varMsg.GetReturnValue();

				// Test #2.
				{
                    const bool success(returnValueResult == TEST_DATA[testIndex].m_returnValueExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("returnValueResult=%d expected=%d testIndex=%d in %s(%d)",
							(unsigned)returnValueResult,
							(unsigned)TEST_DATA[testIndex].m_returnValueExpected,
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #3.
				{
					const __int64 maxBetResult = msg.GetMaxBet();
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

				// Test #3.
				{
					const byte progressiveGroupResult = msg.GetProgressiveGroup();
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
				// Cleanup.
			}

		}

		/// Code coverage and functional test for:
		/// CLongPollB5Message::ResponseReceived()
		/// </summary>
		TEST(LongPollB5Message, ResponseReceivedTest02)
		{
			BYTE gameNumber = 1;
			BYTE gameAddress = 1;
			BYTE paytableIDLength = 10;
			BYTE msg_buffer[] =
			{
				gameNumber, 181, 30, 0, 1, 1, 37, 0, 0, 0, 0, 0,
				7, 87, 79, 67, 32, 45, 32, 49,              /*Game name and length*/
				paytableIDLength, 77, 86, 52, 49, 55, 52, 45, 76, 48, 49, /*Paytable ID and length*/
				0, 1
			};

			UINT msg_size = sizeof(msg_buffer) / sizeof(BYTE);
			BYTE *buffer = new BYTE[msg_size];
			memcpy_s(buffer, msg_size, &msg_buffer, msg_size);

			CLongPollB5Message lpb5_message(gameAddress);
			lpb5_message.SetGameNumber(gameNumber);
			lpb5_message.SetResponseBuffer(buffer, msg_size);
			lpb5_message.SetReturnValue(SASPOLL_SUCCESS);
			lpb5_message.ResponseReceived();

			ASSERT_EQ(lpb5_message.GetReturnValue(), SASPOLL_SUCCESS);

			char * gameName = nullptr;
			ASSERT_EQ(7, lpb5_message.GetGameName(&gameName));
			ASSERT_NE(gameName, nullptr);
			ASSERT_STREQ(gameName, "WOC - 1");

			char * paytableName = nullptr;
			ASSERT_EQ(paytableIDLength, lpb5_message.GetPaytableName(&paytableName));
			ASSERT_NE(paytableName, nullptr);
			ASSERT_EQ(strncmp(paytableName, "MV4174-L01K", paytableIDLength), 0);


		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CLongPollB5Message::ResponseReceived() if game numbers don't match
		/// </summary>
		TEST(LongPollB5Message, ResponseReceivedTest03)
		{
			BYTE gameNumber = 1;
			BYTE gameAddress = 1;
			BYTE paytableIDLength = 10;
			BYTE msg_buffer[] =
			{
				gameNumber, 181, 30, 0, 1, 1, 37, 0, 0, 0, 0, 0,
				7, 87, 79, 67, 32, 45, 32, 49,              /*Game name and length*/
				paytableIDLength, 77, 86, 52, 49, 55, 52, 45, 76, 48, 49, /*Paytable ID and length*/
				0, 1
			};

			UINT msg_size = sizeof(msg_buffer) / sizeof(BYTE);
			BYTE *buffer = new BYTE[msg_size];
			memcpy_s(buffer, msg_size, &msg_buffer, msg_size);

			CLongPollB5Message lpb5_message(gameAddress);
			lpb5_message.SetGameNumber(gameNumber + 1);
			lpb5_message.SetResponseBuffer(buffer, msg_size);
			lpb5_message.SetReturnValue(SASPOLL_SUCCESS);
			lpb5_message.ResponseReceived();

			ASSERT_EQ(lpb5_message.GetReturnValue(), SASPOLL_INCOMPLETE_DATA);

		}


		/// <summary>
		/// Code coverage and functional test for:
		/// CLongPollB5Message::ResponseReceived() if insufficient length like paytableid is shorter
		/// </summary>
		TEST(LongPollB5Message, ResponseReceivedTest04)
		{
			BYTE gameNumber = 1;
			BYTE gameAddress = 1;
			BYTE paytableIDLength = 10;
			BYTE msg_buffer[] =
			{
				gameNumber, 181, 30, 0, 1, 1, 37, 0, 0, 0, 0, 0,
				7, 87, 79, 67, 32, 45, 32, 49, 			 /*Game name and length*/
				paytableIDLength, 77, 86, 52, 49, 55, 52,  /*Paytable ID and length*/
				0, 1
			};

			UINT msg_size = sizeof(msg_buffer) / sizeof(BYTE);
			BYTE *buffer = new BYTE[msg_size];
			memcpy_s(buffer, msg_size, &msg_buffer, msg_size);

			CLongPollB5Message lpb5_message(gameAddress);
			lpb5_message.SetGameNumber(gameNumber);
			lpb5_message.SetResponseBuffer(buffer, msg_size);
			lpb5_message.SetReturnValue(SASPOLL_SUCCESS);
			lpb5_message.ResponseReceived();

			ASSERT_EQ(lpb5_message.GetReturnValue(), SASPOLL_INCOMPLETE_DATA);

			char * gameName = nullptr;
			ASSERT_EQ(7, lpb5_message.GetGameName(&gameName));
			ASSERT_NE(gameName, nullptr);
			ASSERT_STREQ(gameName, "WOC - 1");

			char * paytableName = nullptr;
			ASSERT_EQ(0, lpb5_message.GetPaytableName(&paytableName));
			ASSERT_EQ(nullptr, paytableName);
		}

		/// <summary>
		/// Code coverage and functional test for:
		/// CLongPollB5Message::ResponseReceived() if insufficient length like paytableid is shorter
		/// </summary>
		TEST(LongPollB5Message, ResponseReceivedTest05)
		{
			BYTE gameNumber = 1;
			BYTE gameAddress = 1;
			BYTE paytableIDLength = 10;
			BYTE msg_buffer[] =
			{
				gameNumber, 181, 30, 0, 1, 1, 37, 0, 0, 0, 0, 0,
				7, 87, 79, 67, 32, 45, 32 			 /* Incomplete Game name and length*/
													 /*No Paytable ID and length*/
													 /*NO CRC*/
			};

			UINT msg_size = sizeof(msg_buffer) / sizeof(BYTE);
			BYTE *buffer = new BYTE[msg_size];
			memcpy_s(buffer, msg_size, &msg_buffer, msg_size);

			CLongPollB5Message lpb5_message(gameAddress);
			lpb5_message.SetGameNumber(gameNumber);
			lpb5_message.SetResponseBuffer(buffer, msg_size);
			lpb5_message.SetReturnValue(SASPOLL_SUCCESS);
			lpb5_message.ResponseReceived();

			ASSERT_EQ(lpb5_message.GetReturnValue(), SASPOLL_INCOMPLETE_DATA);

			char * gameName = nullptr;
			ASSERT_EQ(0, lpb5_message.GetGameName(&gameName));
			ASSERT_EQ(gameName, nullptr);

			char * paytableName = nullptr;
			ASSERT_EQ(0, lpb5_message.GetPaytableName(&paytableName));
			ASSERT_EQ(nullptr, paytableName);

		}
}
