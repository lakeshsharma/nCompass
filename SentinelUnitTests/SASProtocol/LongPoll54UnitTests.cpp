#include "stdafx.h"
#include "SASProtocol/LongPoll54Message.h"

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
		///     CLongPoll54Message::CLongPoll54Message()
		/// Code coverage:
		///     CLongPoll54Message::~CLongPoll54Message()
		/// Test for correct default values.
		/// </summary>
		TEST(LongPoll54Message, CLongPoll54MessageTest)
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
			const BYTE pollValueExpected(0x54);
			const bool swapPollValueExpected(false);
			const UINT responseBufferSizeExpected(48);
			const BYTE responseBufferExpected[responseBufferSizeExpected] = {
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0 };
			const UINT sendBufferSizeExpected(2);
			const BYTE sendBufferExpected[sendBufferSizeExpected] = { 0, 0 };
			const string serialNumberExpected;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CLongPoll54Message varMsg(TEST_DATA[testIndex].m_gameAddress);
				const CLongPoll54Message &msg(varMsg);

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

				// Test #10.
				{
					const string serialNumberResult(msg.GetSerialNumber());
					const bool success(serialNumberResult == serialNumberExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("serialNumberResult=\"%s\" expected=\"%s\" testIndex=%d in %s(%d)",
							(serialNumberResult).c_str(),
							(serialNumberExpected).c_str(),
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
		/// CLongPoll54Message::ResponseReceived()
		/// </summary>
		TEST(LongPoll54Message, ResponseReceivedTest)
		{
			// Input(s) -- same for all loop iterations.
			const byte gameAddress(1);
			const byte pollValue(0x54);

			// Expected result(s) -- same for all loop iterations.
			const UINT responseBufferSizeExpected(48);

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
			const byte responseBuffer2c[] = { gameAddress, 0x53 };
			const byte responseBuffer2d[] = { gameAddress, pollValue };
			const byte responseBuffer2e[] = { gameAddress, 0x55 };
			const byte responseBuffer2f[] = { gameAddress, 0x7f };
			const byte responseBuffer2g[] = { gameAddress, 0x80 };
			const byte responseBuffer2h[] = { gameAddress, 0xff };

			// Input(s): Short responses with variable length field set to 0.
			const byte responseBuffer3a[] = { gameAddress, pollValue, 0x00 };
			const byte responseBuffer3b[] = { gameAddress, pollValue, 0x00, 0x01 };
			const byte responseBuffer3c[] = { gameAddress, pollValue, 0x00, 0x01, 0x02 };
			const byte responseBuffer3d[] = { gameAddress, pollValue, 0x00, 0x01, 0x02, 0x03 };
			const byte responseBuffer3e[] = { gameAddress, pollValue, 0x00, 0x01, 0x02, 0x03, 0xcc };
			const byte responseBuffer3f[] = { gameAddress, pollValue, 0x00, 0x01, 0x02, 0x03, 0xcc, 0xcd };
			const byte responseBuffer3g[] = { gameAddress, pollValue, 0x00, 0x01, 0x02, 0x03, 'A', 0xcc, 0xcd };
			const byte responseBuffer3h[] = { gameAddress, pollValue, 0x00, 0xfd, 0xfe, 0xff, 'A', 'B', 0xcc, 0xcd };

			// Input(s): Short responses with variable length field set to 0xff.
			const byte responseBuffer4a[] = { gameAddress, pollValue, 0xff };
			const byte responseBuffer4b[] = { gameAddress, pollValue, 0xff, 0x01 };
			const byte responseBuffer4c[] = { gameAddress, pollValue, 0xff, 0x01, 0x02 };
			const byte responseBuffer4d[] = { gameAddress, pollValue, 0xff, 0x01, 0x02, 0x03 };
			const byte responseBuffer4e[] = { gameAddress, pollValue, 0xff, 0x01, 0x02, 0x03, 0xcc };
			const byte responseBuffer4f[] = { gameAddress, pollValue, 0xff, 0x01, 0x02, 0x03, 0xcc, 0xcd };
			const byte responseBuffer4g[] = { gameAddress, pollValue, 0xff, 0x01, 0x02, 0x03, 'A', 0xcc, 0xcd };
			const byte responseBuffer4h[] = { gameAddress, pollValue, 0xff, 0xfd, 0xfe, 0xff, 'A', 'B', 0xcc, 0xcd };

			// Input(s): Long responses with variable length field set to 0.
			const byte responseBuffer5a[responseBufferSizeExpected - 2] = {
				gameAddress, pollValue, 0x00, 0x7f, (byte)0x80, (byte)0x81,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8',
				0xcc, 0xcd };
			const byte responseBuffer5b[responseBufferSizeExpected - 1] = {
				gameAddress, pollValue, 0x00, 0x01, 0x02, 0x03,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
				0xcc, 0xcd };
			const byte responseBuffer5c[responseBufferSizeExpected] = {
				gameAddress, pollValue, 0x00, 0x01, 0x02, 0x03,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				0xcc, 0xcd };
			const byte responseBuffer5d[responseBufferSizeExpected + 1] = {
				gameAddress, pollValue, 0x00, 0x01, 0x02, 0x03,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'1',
				0xcc, 0xcd };
			const byte responseBuffer5e[responseBufferSizeExpected + 2] = {
				gameAddress, pollValue, 0x00, 0x01, 0x02, 0x03,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'1', '2',
				0xcc, 0xcd };
			const byte responseBuffer5f[responseBufferSizeExpected + 26] = {
				gameAddress, pollValue, 0x00, 0x01, 0x02, 0x03,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				0xcc, 0xcd };

			// Input(s): Long responses with variable length field set to 0xff.
			const byte responseBuffer6a[responseBufferSizeExpected - 2] = {
				gameAddress, pollValue, 0xff, 0x7f, (byte)0x80, (byte)0x81,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8',
				0xcc, 0xcd };
			const byte responseBuffer6b[responseBufferSizeExpected - 1] = {
				gameAddress, pollValue, 0xff, 0x01, 0x02, 0x03,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
				0xcc, 0xcd };
			const byte responseBuffer6c[responseBufferSizeExpected] = {
				gameAddress, pollValue, 0xff, 0x01, 0x02, 0x03,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				0xcc, 0xcd };
			const byte responseBuffer6d[responseBufferSizeExpected + 1] = {
				gameAddress, pollValue, 0xff, 0x01, 0x02, 0x03,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'1',
				0xcc, 0xcd };
			const byte responseBuffer6e[responseBufferSizeExpected + 2] = {
				gameAddress, pollValue, 0xff, 0x01, 0x02, 0x03,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'1', '2',
				0xcc, 0xcd };
			const byte responseBuffer6f[responseBufferSizeExpected + 26] = {
				gameAddress, pollValue, 0xff, 0x01, 0x02, 0x03,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				0xcc, 0xcd };

			// Input(s): Short responses with variable length field set correctly.
			const byte responseBuffer7a[] = { gameAddress, pollValue, 0 };
			const byte responseBuffer7b[] = { gameAddress, pollValue, 0, 0x01 };
			const byte responseBuffer7c[] = { gameAddress, pollValue, 0, 0x01, 0x02 };
			const byte responseBuffer7d[] = { gameAddress, pollValue, 1, 0x01, 0x02, 0x03 };
			const byte responseBuffer7e[] = { gameAddress, pollValue, 2, 0x01, 0x02, 0x03, 0xcc };
			const byte responseBuffer7f[] = { gameAddress, pollValue, 3, 0x01, 0x02, 0x03, 0xcc, 0xcd };
			const byte responseBuffer7g[] = { gameAddress, pollValue, 4, 0x01, 0x02, 0x03, 'A', 0xcc, 0xcd };
			const byte responseBuffer7h[] = { gameAddress, pollValue, 5, 0xfd, 0xfe, 0xff, 'A', 'B', 0xcc, 0xcd };
			const byte responseBuffer7i[] = { gameAddress, pollValue, 6, 0xfd, 0xfe, 0xff, 'A', 'B', 'C', 0xcc, 0xcd };
			const byte responseBuffer7j[] = { gameAddress, pollValue, 6, 0xfd, 0xfe, 0xff, 'A', 0, 'C', 0xcc, 0xcd }; // Embedded NUL character.
			const byte responseBuffer7k[] = { gameAddress, pollValue, 6, 0xfd, 0xfe, 0xff, 'A', 1, 'C', 0xcc, 0xcd }; // Embedded control character.
			const byte responseBuffer7L[] = { gameAddress, pollValue, 6, 0xfd, 0xfe, 0xff, 'A', ' ', 'C', 0xcc, 0xcd }; // Minimum displayable ASCII character.
			const byte responseBuffer7m[] = { gameAddress, pollValue, 6, 0xfd, 0xfe, 0xff, 'A', '~', 'C', 0xcc, 0xcd }; // Maximum displayable ASCII character.
			const byte responseBuffer7n[] = { gameAddress, pollValue, 6, 0xfd, 0xfe, 0xff, 'A', 0x7f, 'C', 0xcc, 0xcd }; // Maximum ASCII character.
			const byte responseBuffer7o[] = { gameAddress, pollValue, 6, 0xfd, 0xfe, 0xff, 'A', 0x80, 'C', 0xcc, 0xcd }; // Non-ASCII character.
			const byte responseBuffer7p[] = { gameAddress, pollValue, 6, 0xfd, 0xfe, 0xff, 'A', 0x81, 'C', 0xcc, 0xcd }; // Non-ASCII character.
			const byte responseBuffer7q[] = { gameAddress, pollValue, 6, 0xfd, 0xfe, 0xff, 'A', 0xfe, 'C', 0xcc, 0xcd }; // Non-ASCII character.
			const byte responseBuffer7r[] = { gameAddress, pollValue, 6, 0xfd, 0xfe, 0xff, 'A', 0xff, 'C', 0xcc, 0xcd }; // Non-ASCII character.

			// Input(s): Short responses with variable length field set (mostly) incorrectly.
			const byte responseBuffer8a[] = { gameAddress, pollValue, 4, 0xfd, 0xfe, 0xff, 'A', 'B', 'C', 0xcc, 0xcd };
			const byte responseBuffer8b[] = { gameAddress, pollValue, 5, 0xfd, 0xfe, 0xff, 'A', 'B', 'C', 0xcc, 0xcd };
			const byte responseBuffer8c[] = { gameAddress, pollValue, 6, 0xfd, 0xfe, 0xff, 'A', 'B', 'C', 0xcc, 0xcd }; // Correct length field.
			const byte responseBuffer8d[] = { gameAddress, pollValue, 7, 0xfd, 0xfe, 0xff, 'A', 'B', 'C', 0xcc, 0xcd };
			const byte responseBuffer8e[] = { gameAddress, pollValue, 8, 0xfd, 0xfe, 0xff, 'A', 'B', 'C', 0xcc, 0xcd };
			const byte responseBuffer8f[] = { gameAddress, pollValue, 9, 0xfd, 0xfe, 0xff, 'A', 'B', 'C', 0xcc, 0xcd };
			const byte responseBuffer8g[] = { gameAddress, pollValue, 10, 0xfd, 0xfe, 0xff, 'A', 'B', 'C', 0xcc, 0xcd };


			// Input(s): Longer responses with the variable length field set correctly.
			const byte responseBuffer9a[responseBufferSizeExpected - 2] = {
				gameAddress, pollValue, 41, 0x7f, (byte)0x80, (byte)0x81,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8',
				0xcc, 0xcd };
			const byte responseBuffer9b[responseBufferSizeExpected - 1] = {
				gameAddress, pollValue, 42, 0x01, 0x02, 0x03,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
				0xcc, 0xcd };
			const byte responseBuffer9c[responseBufferSizeExpected] = {
				gameAddress, pollValue, 43, 0x01, 0x02, 0x03,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				0xcc, 0xcd };
			const byte responseBuffer9d[responseBufferSizeExpected + 1] = {
				gameAddress, pollValue, 44, 0x01, 0x02, 0x03,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'1',
				0xcc, 0xcd };
			const byte responseBuffer9e[responseBufferSizeExpected + 2] = {
				gameAddress, pollValue, 45, 0x01, 0x02, 0x03,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'1', '2',
				0xcc, 0xcd };
			const byte responseBuffer9f[responseBufferSizeExpected + 26] = {
				gameAddress, pollValue, 69, 0x01, 0x02, 0x03,
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				'7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
				'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
				0xcc, 0xcd };

			typedef struct
			{
				// Input(s).
				const byte * const m_responseBuffer;
				const UINT m_responseBufferSize;
				const int m_returnValue;

				// Expected result(s).
				const int m_returnValueExpected;
				const char m_versionExpected[3]; // Undefined unless CSASPollMessage::GetReturnValue() == SASPOLL_SUCCESS.
				const string m_serialNumberExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ nullptr, 0, SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ nullptr, 0, SASPOLL_INCOMPLETE_DATA, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ nullptr, 0, SASPOLL_NACK, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ nullptr, 0, SASPOLL_DATA_TOO_LARGE, SASPOLL_DATA_TOO_LARGE, { 0, 0, 0 }, "" },

				{ responseBuffer1a, sizeof(responseBuffer1a), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ responseBuffer1b, sizeof(responseBuffer1b), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ responseBuffer1c, sizeof(responseBuffer1c), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ responseBuffer1d, sizeof(responseBuffer1d), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ responseBuffer1e, sizeof(responseBuffer1e), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ responseBuffer1f, sizeof(responseBuffer1f), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ responseBuffer1g, sizeof(responseBuffer1g), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ responseBuffer1h, sizeof(responseBuffer1h), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ responseBuffer1i, sizeof(responseBuffer1i), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },

				{ responseBuffer2a, sizeof(responseBuffer2a), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ responseBuffer2b, sizeof(responseBuffer2b), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ responseBuffer2c, sizeof(responseBuffer2c), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ responseBuffer2d, sizeof(responseBuffer2d), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer2e, sizeof(responseBuffer2e), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ responseBuffer2f, sizeof(responseBuffer2f), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ responseBuffer2g, sizeof(responseBuffer2g), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },
				{ responseBuffer2h, sizeof(responseBuffer2h), SASPOLL_SUCCESS, SASPOLL_NACK, { 0, 0, 0 }, "" },

				{ responseBuffer3a, sizeof(responseBuffer3a), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer3b, sizeof(responseBuffer3b), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer3c, sizeof(responseBuffer3c), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer3d, sizeof(responseBuffer3d), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer3e, sizeof(responseBuffer3e), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer3f, sizeof(responseBuffer3f), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "" },
				{ responseBuffer3g, sizeof(responseBuffer3g), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "" },
				{ responseBuffer3h, sizeof(responseBuffer3h), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "" },

				{ responseBuffer4a, sizeof(responseBuffer4a), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer4b, sizeof(responseBuffer4b), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer4c, sizeof(responseBuffer4c), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer4d, sizeof(responseBuffer4d), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer4e, sizeof(responseBuffer4e), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer4f, sizeof(responseBuffer4f), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "" },
				{ responseBuffer4g, sizeof(responseBuffer4g), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "" },
				{ responseBuffer4h, sizeof(responseBuffer4h), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "" },

				{ responseBuffer5a, sizeof(responseBuffer5a), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x7f, (char)0x80, (char)0x81 }, "" },
				{ responseBuffer5b, sizeof(responseBuffer5b), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "" },
				{ responseBuffer5c, sizeof(responseBuffer5c), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "" },
				{ responseBuffer5d, sizeof(responseBuffer5d), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "" },
				{ responseBuffer5e, sizeof(responseBuffer5e), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "" },
				{ responseBuffer5f, sizeof(responseBuffer5f), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "" },

				{ responseBuffer6a, sizeof(responseBuffer6a), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x7f, (char)0x80, (char)0x81 }, "" },
				{ responseBuffer6b, sizeof(responseBuffer6b), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "" },
				{ responseBuffer6c, sizeof(responseBuffer6c), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "" },
				{ responseBuffer6d, sizeof(responseBuffer6d), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "" },
				{ responseBuffer6e, sizeof(responseBuffer6e), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "" },
				{ responseBuffer6f, sizeof(responseBuffer6f), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "" },

				{ responseBuffer7a, sizeof(responseBuffer7a), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer7b, sizeof(responseBuffer7b), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer7c, sizeof(responseBuffer7c), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer7d, sizeof(responseBuffer7d), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer7e, sizeof(responseBuffer7e), SASPOLL_SUCCESS, SASPOLL_INCOMPLETE_DATA, { 0, 0, 0 }, "" },
				{ responseBuffer7f, sizeof(responseBuffer7f), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "" },
				{ responseBuffer7g, sizeof(responseBuffer7g), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { 0x01, 0x02, 0x03 }, "A" },
				{ responseBuffer7h, sizeof(responseBuffer7h), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "AB" },
				{ responseBuffer7i, sizeof(responseBuffer7i), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "ABC" },
				{ responseBuffer7j, sizeof(responseBuffer7j), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "A" },
				{ responseBuffer7k, sizeof(responseBuffer7k), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "A\x01" "C"},
				{ responseBuffer7L, sizeof(responseBuffer7L), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "A C" },
				{ responseBuffer7m, sizeof(responseBuffer7m), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "A~C" },
				{ responseBuffer7n, sizeof(responseBuffer7n), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "A\x7f" "C"},

				// Locale-dependent result(s).
				{ responseBuffer7o, sizeof(responseBuffer7o), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "A\x80" "C"},
				{ responseBuffer7p, sizeof(responseBuffer7p), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "A\x81" "C"},
				{ responseBuffer7q, sizeof(responseBuffer7q), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "A\xfe" "C"},
				{ responseBuffer7r, sizeof(responseBuffer7r), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "A\xff" "C"},

				{ responseBuffer8a, sizeof(responseBuffer8a), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "A" },
				{ responseBuffer8b, sizeof(responseBuffer8b), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "AB" },
				{ responseBuffer8c, sizeof(responseBuffer8c), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "ABC" },

				// Locale-dependent result(s).
				{ responseBuffer8d, sizeof(responseBuffer8d), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "ABC\xcc" },
				{ responseBuffer8e, sizeof(responseBuffer8e), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "ABC\xcc" "\xcd" },
				{ responseBuffer8f, sizeof(responseBuffer8f), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "ABC\xcc" "\xcd" },
				{ responseBuffer8g, sizeof(responseBuffer8g), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0xfd, (char)0xfe, (char)0xff }, "ABC\xcc" "\xcd" },

				{ responseBuffer9a, sizeof(responseBuffer9a), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0x7f, (char)0x80, (char)0x81 }, "ABCDEFGHIJKLMNOPQRSTUVWXYZ789012345678" },
				{ responseBuffer9b, sizeof(responseBuffer9b), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0x01, (char)0x02, (char)0x03 }, "ABCDEFGHIJKLMNOPQRSTUVWXYZ7890123456789" },
				{ responseBuffer9c, sizeof(responseBuffer9c), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0x01, (char)0x02, (char)0x03 }, "ABCDEFGHIJKLMNOPQRSTUVWXYZ78901234567890" },
				{ responseBuffer9d, sizeof(responseBuffer9d), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0x01, (char)0x02, (char)0x03 }, "" },
				{ responseBuffer9e, sizeof(responseBuffer9e), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0x01, (char)0x02, (char)0x03 }, "" },
				{ responseBuffer9f, sizeof(responseBuffer9f), SASPOLL_SUCCESS, SASPOLL_SUCCESS, { (char)0x01, (char)0x02, (char)0x03 }, "" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CLongPoll54Message varMsg(gameAddress);
				const CLongPoll54Message &msg(varMsg);
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

				// 2014-06-22 Version number result is uninitialized unless returnValueResult == SASPOLL_SUCCESS.
				if (returnValueResult == SASPOLL_SUCCESS)
				{
					// Test #3.
					const bool success = MEMCMP_EQUAL == memcmp(msg.version,
						TEST_DATA[testIndex].m_versionExpected,
						sizeof(TEST_DATA[testIndex].m_versionExpected));
					if (!success)
					{
						string versionResultStr = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(msg.version), sizeof(msg.version));
						string versionExpectedStr = CUtilities::ToHexString(
							reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_versionExpected),
							sizeof(TEST_DATA[testIndex].m_versionExpected));
						string failedMsg;
						failedMsg = FormatString("versionResult={%s} expected={%s} testIndex=%d in %s(%d)",
							(versionResultStr).c_str(),
							(versionExpectedStr).c_str(),
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Test #4.
				{
					const string serialNumberResult(msg.GetSerialNumber());
					const bool success(serialNumberResult == TEST_DATA[testIndex].m_serialNumberExpected);
					if (!success)
					{
						string failedMsg;
						failedMsg = FormatString("serialNumberResult=\"%s\" expected=\"%s\" testIndex=%d in %s(%d)",
							(serialNumberResult).c_str(),
							(TEST_DATA[testIndex].m_serialNumberExpected).c_str(),
							testIndex,
							(__TFUNCTION__), __LINE__);
						ASSERT_TRUE(success) <<  failedMsg;
					}
				}

				// Cleanup.
			}
		}
}
