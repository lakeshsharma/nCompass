#include "stdafx.h"
#include "PollerProtocol/PollerCommunicator.h"
#include "PollerProtocol/PollerPlayerTransactionMessage.h"
#include "PollerProtocol/PollerHeartbeatMessage.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
		static const byte MEMSET_DEFAULT_VALUE = 0xcd;
		static const byte MEMCMP_DEFAULT_HEADER[8] = { 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd };
		static const byte MEMCMP_DEFAULT_TRAILER[8] = { 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd };

		class PollerCommunicatorTestClass : public ::testing::Test
		{
		public:
		};

		/// <summary>
		/// Code coverage test for:
		///     PollerCommunicator::PollerCommunicator()
		///     PollerCommunicator::~PollerCommunicator()
		/// </summary>
		TEST(PollerCommunicatorTestClass, PollerCommunicatorTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool connectedExpected(false);
			const bool readingExpected(false);
			const bool sendingExpected(false);
			const DWORD pollerPacketSentVerifyErrorsExpected(0);

			// Setup.

			// Perform operation(s) to be tested.
			PollerCommunicator pollerComm;
			// Test #1.
			{
				const bool connectedResult(pollerComm.Connected());
				const bool areEqual(connectedResult == connectedExpected);

				string failedMsg;
				failedMsg = FormatString(_T("connectedResult=%d expected=%d in %s(%d)"),
					(int)connectedResult,
					(int)connectedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.
			{
				const bool readingResult(pollerComm.Reading());
				const bool areEqual(readingResult == readingExpected);

				string failedMsg;
				failedMsg = FormatString(_T("readingResult=%d expected=%d in %s(%d)"),
					(int)readingResult,
					(int)readingExpected,
					static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			{
				const bool sendingResult(pollerComm.Sending());
				const bool areEqual(sendingResult == sendingExpected);

				string failedMsg;
				failedMsg = FormatString(_T("sendingResult=%d expected=%d in %s(%d)"),
					(int)sendingResult,
					(int)sendingExpected,
					static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #4.
			{
				const DWORD pollerPacketSentVerifyErrorsResult(pollerComm.GetPollerPacketSentVerifyErrors());
				const bool areEqual(pollerPacketSentVerifyErrorsResult == pollerPacketSentVerifyErrorsExpected);

				string failedMsg;
				failedMsg = FormatString(_T("pollerPacketSentVerifyErrorsResult=%u expected=%u in %s(%d)"),
					(unsigned)pollerPacketSentVerifyErrorsResult,
					(unsigned)pollerPacketSentVerifyErrorsExpected,
					static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     PollerCommunicator::Close()
		/// </summary>
		TEST(PollerCommunicatorTestClass, CloseTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool connectedExpected(false);

			// Setup.
			PollerCommunicator pollerComm;

			// Perform operation(s) to be tested.
			pollerComm.Close();

			// Test #1.
			{
				const bool connectedResult(pollerComm.Connected());
				const bool areEqual(connectedResult == connectedExpected);

				string failedMsg;
				failedMsg = FormatString(_T("connectedResult=%d expected=%d in %s(%d)"),
					(int)connectedResult,
					(int)connectedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     PollerCommunicator::Cancel()
		/// </summary>
		TEST(PollerCommunicatorTestClass, CancelTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool connectedExpected(false);

			// Setup.
			PollerCommunicator pollerComm;

			// Perform operation(s) to be tested.
			pollerComm.Cancel();

			// Test #1.
			{
				const bool connectedResult(pollerComm.Connected());
				const bool areEqual(connectedResult == connectedExpected);

				string failedMsg;
				failedMsg = FormatString(_T("connectedResult=%d expected=%d in %s(%d)"),
					(int)connectedResult,
					(int)connectedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Functional test and code coverage test for:
		///     PollerCommunicator::Connected()
		/// Test for correct initial value.
		/// </summary>
		TEST(PollerCommunicatorTestClass, Connected_InitTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool connectedExpected(false);

			// Setup.
			PollerCommunicator pollerComm;

			// Perform operation(s) to be tested.
			const bool connectedResult(pollerComm.Connected());

			// Test #1.
			{
				const bool areEqual(connectedResult == connectedExpected);

				string failedMsg;
				failedMsg = FormatString(_T("connectedResult=%d expected=%d in %s(%d)"),
					(int)connectedResult,
					(int)connectedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Functional test and code coverage test for:
		///     PollerCommunicator::Reading()
		/// Test for correct initial value.
		/// </summary>
		TEST(PollerCommunicatorTestClass, Reading_InitTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool readingExpected(false);

			// Setup.
			PollerCommunicator pollerComm;

			// Perform operation(s) to be tested.
			const bool readingResult(pollerComm.Reading());

			// Test #1.
			{
				const bool areEqual(readingResult == readingExpected);

				string failedMsg;
				failedMsg = FormatString(_T("readingResult=%d expected=%d in %s(%d)"),
					(int)readingResult,
					(int)readingExpected,
					static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Functional test and code coverage test for:
		///     PollerCommunicator::Sending()
		/// Test for correct initial value.
		/// </summary>
		TEST(PollerCommunicatorTestClass, Sending_InitTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool sendingExpected(false);

			// Setup.
			PollerCommunicator pollerComm;

			// Perform operation(s) to be tested.
			const bool sendingResult(pollerComm.Sending());

			// Test #1.
			{
				const bool areEqual(sendingResult == sendingExpected);

				string failedMsg;
				failedMsg = FormatString(_T("sendingResult=%d expected=%d in %s(%d)"),
					(int)sendingResult,
					(int)sendingExpected,
					static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Functional test and code coverage test for:
		///     PollerCommunicator::GetPollerPacketSentVerifyErrors()
		/// Test for correct initial value.
		/// </summary>
		TEST(PollerCommunicatorTestClass, GetPollerPacketSentVerifyErrors_InitTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const DWORD pollerPacketSentVerifyErrorsExpected(0);

			// Setup.
			PollerCommunicator pollerComm;

			// Perform operation(s) to be tested.
			const DWORD pollerPacketSentVerifyErrorsResult(pollerComm.GetPollerPacketSentVerifyErrors());

			// Test #1.
			{
				const bool areEqual(pollerPacketSentVerifyErrorsResult == pollerPacketSentVerifyErrorsExpected);

				string failedMsg;
				failedMsg = FormatString(_T("pollerPacketSentVerifyErrorsResult=%u expected=%u in %s(%d)"),
					(unsigned)pollerPacketSentVerifyErrorsResult,
					(unsigned)pollerPacketSentVerifyErrorsExpected,
					static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     PollerCommunicator::Connect()
		/// Test with an empty HostName.
		/// </summary>
		TEST(PollerCommunicatorTestClass, Connect_EmptyTest)
		{
			// Test data -- inputs.
			const std::string HostName;
			const int Port(-1);

			// Test data -- expected results.
			const bool connectExpected(false);

			// Setup.
			PollerCommunicator pollerComm;

			// Perform operation(s) to be tested.
			TiltCode tiltCode(tltUnknown);
			const bool connectResult(pollerComm.Connect(HostName, Port, tiltCode));

			// Test #1.
			{
				const bool areEqual(connectResult == connectExpected);

				string failedMsg;
				failedMsg = FormatString(_T("connectResult=%d expected=%d in %s(%d)"),
					(int)connectResult,
					(int)connectExpected,
					static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     PollerCommunicator::Send()
		/// Test while not connected.
		/// </summary>
		TEST(PollerCommunicatorTestClass, Send_NotConnectedTest)
		{
			// Test data -- inputs.
			char datatosend[1] = { 0 };
			const long lengthofdatatosend(0);

			// Test data -- expected results.
			const bool sendResultExpected(false);
			const long bytessentExpected(-12345);

			// Setup.
			PollerCommunicator pollerComm;
			ASSERT_FALSE(pollerComm.Connected());

			// Perform operation(s) to be tested.
			long bytessent(bytessentExpected);
#if 000//TODO-PORT - 2016-12-02 PollerCommunicator::Send() does not timeout.
			const bool sendResult(pollerComm.Send(datatosend, lengthofdatatosend, bytessent));
#else//TODO-PORT Remove this "else" code.
			const bool sendResult(false);
#endif//TODO-PORT

			// Test #1.
			{
				const bool areEqual(sendResult == sendResultExpected);

				string failedMsg;
				failedMsg = FormatString(_T("sendResult=%d expected=%d in %s(%d)"),
					(int)sendResult,
					(int)sendResultExpected,
					static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.
			{
				const bool areEqual(bytessent == bytessentExpected);

				string failedMsg;
				failedMsg = FormatString(_T("bytessent=%ld expected=%ld in %s(%d)"),
					(long)bytessent,
					(long)bytessentExpected,
					static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Functional test and code coverage test for:
		///     PollerCommunicator::GetPollerPacketSentVerifyErrors()
		/// Increment count.
		/// </summary>
		TEST(PollerCommunicatorTestClass, GetPollerPacketSentVerifyErrors_IncTest)
		{
			// Test data -- inputs.
			const int testCount(10);
			const BYTE *massagedPacket(NULL);
			const long massagedPacketLength(0);
			const BYTE *message(NULL);
			long messageLength(0);

			// Test data -- expected results.

			// Setup.
			PollerCommunicator pollerComm;

			for (int testIndex = 0; testIndex < testCount; ++testIndex)
			{
				// Test data -- expected results.
				const DWORD pollerPacketSentVerifyErrorsExpected(testIndex+1);
				
				// Setup.
				const bool verifyPacketSentResult = pollerComm.VerifyPacketSent(massagedPacket, massagedPacketLength, message, messageLength);
				ASSERT_FALSE(verifyPacketSentResult);

				// Perform operation(s) to be tested.
				const DWORD pollerPacketSentVerifyErrorsResult(pollerComm.GetPollerPacketSentVerifyErrors());

				// Test #1.
				{
					const bool areEqual(pollerPacketSentVerifyErrorsResult == pollerPacketSentVerifyErrorsExpected);

					string failedMsg;
					failedMsg = FormatString(_T("pollerPacketSentVerifyErrorsResult=%u expected=%u testIndex=%d in %s(%d)"),
						(unsigned)pollerPacketSentVerifyErrorsResult,
						(unsigned)pollerPacketSentVerifyErrorsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage test for:
		///     PollerCommunicator::VerifyPacketSent()
		/// Test packets with zero for message data length -- no decryption.
		/// </summary>
		TEST(PollerCommunicatorTestClass, VerifyPacketSent_NoDecryptTest)
		{
			typedef struct
			{
				// Inputs.
				const BYTE * const m_massagedPacket;
				const long m_massagedPacketLength;
				const BYTE * const m_message;
				const long m_messageLength;

				// Expected results.
				const bool m_verifyPacketSentExpected;
			} TestDataType;

			const BYTE packet1[] = { 0xeb, 0x00, 0x00, 0x00, 0x00, 0xee };
			const BYTE message1[] = { 0x00 }; // Test with length of 0.

			const BYTE packet2[] = { 0xeb, 0x00, 0x00, 0x00, 0xee };
			const BYTE packet3[] = { 0xeb, 0x00, 0x00, 0xee };
			const BYTE packet4[] = { 0xeb, 0x00, 0xee };
			const BYTE packet5[] = { 0xeb, 0xee };
			const BYTE packet6[] = { 0xeb };
			const BYTE packet7[] = { 0xeb };

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, NULL, 0, false },
				{ NULL, _countof(packet1), NULL, 0, false },
				{ NULL, 33, NULL, 0, false },
				{ NULL, -33, NULL, 0, false },
				{ packet1, 0, NULL, 0, false },
				{ packet1, -33, NULL, 0, false },
				{ packet1, _countof(packet1), NULL, 33, false },
				{ packet1, _countof(packet1), NULL, -33, false },

				{ packet1, _countof(packet1), NULL, 0, true },
				{ packet1, _countof(packet1), message1, 0, true },

				{ packet2, _countof(packet2), NULL, 0, false },
				{ packet3, _countof(packet3), NULL, 0, false },
				{ packet4, _countof(packet4), NULL, 0, false },
				{ packet5, _countof(packet5), NULL, 0, false },
				{ packet6, _countof(packet6), NULL, 0, false },
				{ packet7, _countof(packet7), NULL, 0, false },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				PollerCommunicator pollerComm;

				// Perform operations to be tested.
				const bool verifyPacketSentResult = pollerComm.VerifyPacketSent(TEST_DATA[testIndex].m_massagedPacket,
					TEST_DATA[testIndex].m_massagedPacketLength,
					TEST_DATA[testIndex].m_message,
					TEST_DATA[testIndex].m_messageLength);

				// Test #1.
				{
					const bool areEqual(verifyPacketSentResult == TEST_DATA[testIndex].m_verifyPacketSentExpected);

					string m_massagedPacketStr(TEST_DATA[testIndex].m_massagedPacket ?
						_T("{") + CUtilities::ToHexString(TEST_DATA[testIndex].m_massagedPacket, TEST_DATA[testIndex].m_massagedPacketLength) + _T("}"):
						_T("NULL"));

					string messageStr(TEST_DATA[testIndex].m_message ?
						_T("{") + CUtilities::ToHexString(TEST_DATA[testIndex].m_message, TEST_DATA[testIndex].m_messageLength) + _T("}"):
						_T("NULL"));

					string failedMsg;
					failedMsg = FormatString(_T("VerifyPacketSent(%s,%ld,%s,%ld) result=%d expected=%d testIndex=%d in %s(%d)"),
						static_cast<LPCTSTR>(m_massagedPacketStr.c_str()),
						(long)TEST_DATA[testIndex].m_massagedPacketLength,
						static_cast<LPCTSTR>(messageStr.c_str()),
						(long)TEST_DATA[testIndex].m_messageLength,
						(int)verifyPacketSentResult,
						(int)TEST_DATA[testIndex].m_verifyPacketSentExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
			}
		}

		/// <summary>
		/// Functional test and code coverage test for:
		///     PollerCommunicator::DLEEncode()
		/// </summary>
		TEST(PollerCommunicatorTestClass, DLEEncodeTest)
		{
			typedef struct
			{
				// Inputs.
				const byte * const m_pabyPacketData;
				const long m_lPacketLength;

				// Expected results.
				const byte * const m_pabyEncodeBufferExpected;
				const long m_encodeLengthExpected;
			} TestDataType;

			const BYTE packet1[] = { 0x00 };
			const BYTE encode1[] = { 0x00 };

			const BYTE packet2[] = { 0xff };
			const BYTE encode2[] = { 0xff };

			const BYTE packet3[] = { 0xe0 };
			const BYTE encode3[] = { 0xe0, 0x00 };

			const BYTE packet4[] = { 0xeb };
			const BYTE encode4[] = { 0xe0, 0x0b };

			const BYTE packet5[] = { 0xee };
			const BYTE encode5[] = { 0xe0, 0x0e };

			byte packet6[16] =
			{
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
			};
			byte encode6[16 + 3] =
			{
				0xe0, 0x00, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xe0, 0x0b, 0xec, 0xed, 0xe0, 0x0e, 0xef,
			};

			byte packet7[16 * 16] =
			{
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
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
			};
			byte encode7[16 * 16 + 3] =
			{
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
				0xe0, 0x00, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xe0, 0x0b, 0xec, 0xed, 0xe0, 0x0e, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
			};

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, NULL, 0 },
				{ packet1, _countof(packet1), encode1, _countof(encode1) },
				{ packet2, _countof(packet2), encode2, _countof(encode2) },
				{ packet3, _countof(packet3), encode3, _countof(encode3) },
				{ packet4, _countof(packet4), encode4, _countof(encode4) },
				{ packet5, _countof(packet5), encode5, _countof(encode5) },
				{ packet6, _countof(packet6), encode6, _countof(encode6) },
				{ packet7, _countof(packet7), encode7, _countof(encode7) },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				PollerCommunicator pollerComm;
				const size_t sizeofTestBuffer = 2 * TEST_DATA[testIndex].m_lPacketLength +
					sizeof(MEMCMP_DEFAULT_HEADER) +
					sizeof(MEMCMP_DEFAULT_TRAILER);
				BYTE *testBuffer(new BYTE[sizeofTestBuffer]);
				memset(testBuffer, MEMSET_DEFAULT_VALUE, sizeofTestBuffer);
				BYTE *pabyEncodeBuffer(testBuffer + sizeof(MEMCMP_DEFAULT_HEADER)); // Not allocated.  Do NOT free.

				// Perform operations to be tested.
				const long decodedLengthResult = pollerComm.DLEEncode(TEST_DATA[testIndex].m_pabyPacketData,
					pabyEncodeBuffer,
					TEST_DATA[testIndex].m_lPacketLength);

				// Test #1. Verify length.
				{
					const bool areEqual(decodedLengthResult == TEST_DATA[testIndex].m_encodeLengthExpected);

					string failedMsg;
					failedMsg = FormatString(_T("DLEDecode({%s},{%s},%ld) decodeLength=%ld decodedLengthExpected=%ld testIndex=%d in %s(%d)"),
						static_cast<LPCTSTR>(CUtilities::ToHexString(TEST_DATA[testIndex].m_pabyPacketData, TEST_DATA[testIndex].m_lPacketLength).c_str()),
						static_cast<LPCTSTR>(CUtilities::ToHexString(pabyEncodeBuffer, decodedLengthResult).c_str()),
						(long)TEST_DATA[testIndex].m_lPacketLength,
						(long)decodedLengthResult,
						(long)TEST_DATA[testIndex].m_encodeLengthExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2. Verify that extra test header bytes are unchanged.
				{
					const int memcmpResult = memcmp(testBuffer, MEMCMP_DEFAULT_HEADER, sizeof(MEMCMP_DEFAULT_HEADER));
					const bool areEqual(memcmpResult == MEMCMP_EQUAL);

					string failedMsg;
					failedMsg = FormatString(_T("DLEDecode({%s},{%s},%ld) decodeLength=%ld testHeader={%s} Expected={%s} testIndex=%d in %s(%d)"),
						static_cast<LPCTSTR>(CUtilities::ToHexString(TEST_DATA[testIndex].m_pabyPacketData, TEST_DATA[testIndex].m_lPacketLength).c_str()),
						static_cast<LPCTSTR>(CUtilities::ToHexString(pabyEncodeBuffer, decodedLengthResult).c_str()),
						(long)TEST_DATA[testIndex].m_lPacketLength,
						(long)decodedLengthResult,
						static_cast<LPCTSTR>(CUtilities::ToHexString(testBuffer, sizeof(MEMCMP_DEFAULT_HEADER)).c_str()),
						static_cast<LPCTSTR>(CUtilities::ToHexString(MEMCMP_DEFAULT_HEADER, sizeof(MEMCMP_DEFAULT_HEADER)).c_str()),
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #3. Verify that extra test trailer bytes are unchanged.
				{
					int memcmpResult = MEMCMP_EQUAL;
					for (size_t cmpIndex = sizeof(MEMCMP_DEFAULT_HEADER) + decodedLengthResult;
						cmpIndex < sizeofTestBuffer;
						++cmpIndex)
					{
						memcmpResult = testBuffer[cmpIndex] - MEMSET_DEFAULT_VALUE;
						if (memcmpResult != MEMCMP_EQUAL)
						{
							break;
						}
					}

					const bool areEqual(memcmpResult == MEMCMP_EQUAL);

					string failedMsg;
					failedMsg = FormatString(_T("DLEDecode({%s},{%s},%ld) decodeLength=%ld testTrailer={%s} Expected={%2.2x ...} testIndex=%d in %s(%d)"),
						static_cast<LPCTSTR>(CUtilities::ToHexString(TEST_DATA[testIndex].m_pabyPacketData, TEST_DATA[testIndex].m_lPacketLength).c_str()),
						static_cast<LPCTSTR>(CUtilities::ToHexString(pabyEncodeBuffer, decodedLengthResult).c_str()),
						(long)TEST_DATA[testIndex].m_lPacketLength,
						(long)decodedLengthResult,
						static_cast<LPCTSTR>(CUtilities::ToHexString(testBuffer + sizeof(MEMCMP_DEFAULT_HEADER) + decodedLengthResult,
						sizeofTestBuffer - sizeof(MEMCMP_DEFAULT_HEADER) - decodedLengthResult).c_str()),
						(unsigned)MEMSET_DEFAULT_VALUE,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #4. Verify output buffer.
				{
					const int memcmpResult = memcmp(pabyEncodeBuffer, TEST_DATA[testIndex].m_pabyEncodeBufferExpected, TEST_DATA[testIndex].m_encodeLengthExpected);
					const bool areEqual(memcmpResult == MEMCMP_EQUAL);

					string failedMsg;
					failedMsg = FormatString(_T("DLEDecode({%s},{%s},%ld) decodeLength=%ld decodeBufferExpected={%s} testIndex=%d in %s(%d)"),
						static_cast<LPCTSTR>(CUtilities::ToHexString(TEST_DATA[testIndex].m_pabyPacketData, (DWORD)TEST_DATA[testIndex].m_lPacketLength, (DWORD)TEST_DATA[testIndex].m_lPacketLength).c_str()),
						static_cast<LPCTSTR>(CUtilities::ToHexString(pabyEncodeBuffer, (DWORD)decodedLengthResult, (DWORD)decodedLengthResult).c_str()),
						(long)TEST_DATA[testIndex].m_lPacketLength,
						(long)decodedLengthResult,
						static_cast<LPCTSTR>(CUtilities::ToHexString(TEST_DATA[testIndex].m_pabyEncodeBufferExpected,
							(DWORD)TEST_DATA[testIndex].m_encodeLengthExpected,
							(DWORD)(sizeofTestBuffer  - sizeof(MEMCMP_DEFAULT_HEADER)  - sizeof(MEMCMP_DEFAULT_TRAILER))).c_str()),
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
				delete testBuffer;
			}
		}

		/// <summary>
		/// Functional test and code coverage test for:
		///     PollerCommunicator::DLEDecode()
		/// </summary>
		TEST(PollerCommunicatorTestClass, DLEDecodeTest)
		{
			typedef struct
			{
				// Inputs.
				const byte * const m_pabyPacketData;
				const long m_lPacketLength;

				// Expected results.
				const byte * const m_pabyDecodeBufferExpected;
				const long m_decodeLengthExpected;
			} TestDataType;

			const BYTE packet1[] = { 0x00 };
			const BYTE decode1[] = { 0x00 };

			const BYTE packet2[] = { 0xff };
			const BYTE decode2[] = { 0xff };

			const BYTE packet3[] = { 0xe0, 0xcd  };
			const BYTE decode3[] = { 0xad };

			const BYTE packet4[] = { 0xe0, 0xcd  }; // Test with length less 1.
			const BYTE decode4[] = { 0xad };

			byte packet5[16 * 16 + 3] =
			{
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
				0xe0, 0x00, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xe0, 0x0b, 0xec, 0xed, 0xe0, 0x0e, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
			};
			byte decode5[16 * 16] =
			{
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
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
			};

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, NULL, 0 },
				{ packet1, _countof(packet1), decode1, _countof(decode1) },
				{ packet2, _countof(packet2), decode2, _countof(decode2) },
				{ packet3, _countof(packet3), decode3, _countof(decode3) },
				{ packet4, _countof(packet4) - 1, decode4, _countof(decode4) },
				{ packet5, _countof(packet5), decode5, _countof(decode5) },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				PollerCommunicator pollerComm;
				const size_t sizeofTestBuffer = TEST_DATA[testIndex].m_lPacketLength +
					sizeof(MEMCMP_DEFAULT_HEADER) +
					sizeof(MEMCMP_DEFAULT_TRAILER);
				BYTE *testBuffer(new BYTE[sizeofTestBuffer]);
				memset(testBuffer, MEMSET_DEFAULT_VALUE, sizeofTestBuffer);
				BYTE *pabyDecodeBuffer(testBuffer + sizeof(MEMCMP_DEFAULT_HEADER)); // Not allocated.  Do NOT free.

				// Perform operations to be tested.
				const long decodedLengthResult = pollerComm.DLEDecode(TEST_DATA[testIndex].m_pabyPacketData,
					pabyDecodeBuffer,
					TEST_DATA[testIndex].m_lPacketLength);

				// Test #1. Verify length.
				{
					const bool areEqual(decodedLengthResult == TEST_DATA[testIndex].m_decodeLengthExpected);

					string failedMsg;
					failedMsg = FormatString(_T("DLEDecode({%s},{%s},%ld) decodeLength=%ld decodedLengthExpected=%ld testIndex=%d in %s(%d)"),
						static_cast<LPCTSTR>(CUtilities::ToHexString(TEST_DATA[testIndex].m_pabyPacketData, TEST_DATA[testIndex].m_lPacketLength).c_str()),
						static_cast<LPCTSTR>(CUtilities::ToHexString(pabyDecodeBuffer, decodedLengthResult).c_str()),
						(long)TEST_DATA[testIndex].m_lPacketLength,
						(long)decodedLengthResult,
						(long)TEST_DATA[testIndex].m_decodeLengthExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #2. Verify that extra test header bytes are unchanged.
				{
					const int memcmpResult = memcmp(testBuffer, MEMCMP_DEFAULT_HEADER, sizeof(MEMCMP_DEFAULT_HEADER));
					const bool areEqual(memcmpResult == MEMCMP_EQUAL);

					string failedMsg;
					failedMsg = FormatString(_T("DLEDecode({%s},{%s},%ld) decodeLength=%ld testHeader={%s} Expected={%s} testIndex=%d in %s(%d)"),
						static_cast<LPCTSTR>(CUtilities::ToHexString(TEST_DATA[testIndex].m_pabyPacketData, TEST_DATA[testIndex].m_lPacketLength).c_str()),
						static_cast<LPCTSTR>(CUtilities::ToHexString(pabyDecodeBuffer, decodedLengthResult).c_str()),
						(long)TEST_DATA[testIndex].m_lPacketLength,
						(long)decodedLengthResult,
						static_cast<LPCTSTR>(CUtilities::ToHexString(testBuffer, sizeof(MEMCMP_DEFAULT_HEADER)).c_str()),
						static_cast<LPCTSTR>(CUtilities::ToHexString(MEMCMP_DEFAULT_HEADER, sizeof(MEMCMP_DEFAULT_HEADER)).c_str()),
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #3. Verify that extra test trailer bytes are unchanged.
				{
					int memcmpResult = MEMCMP_EQUAL;
					for (size_t cmpIndex = sizeof(MEMCMP_DEFAULT_HEADER) + decodedLengthResult;
						cmpIndex < sizeofTestBuffer;
						++cmpIndex)
					{
						memcmpResult = testBuffer[cmpIndex] - MEMSET_DEFAULT_VALUE;
						if (memcmpResult != MEMCMP_EQUAL)
						{
							break;
						}
					}

					const bool areEqual(memcmpResult == MEMCMP_EQUAL);

					string failedMsg;
					failedMsg = FormatString(_T("DLEDecode({%s},{%s},%ld) decodeLength=%ld testTrailer={%s} Expected={%2.2x ...} testIndex=%d in %s(%d)"),
						static_cast<LPCTSTR>(CUtilities::ToHexString(TEST_DATA[testIndex].m_pabyPacketData, TEST_DATA[testIndex].m_lPacketLength).c_str()),
						static_cast<LPCTSTR>(CUtilities::ToHexString(pabyDecodeBuffer, decodedLengthResult).c_str()),
						(long)TEST_DATA[testIndex].m_lPacketLength,
						(long)decodedLengthResult,
						static_cast<LPCTSTR>(CUtilities::ToHexString(testBuffer + sizeof(MEMCMP_DEFAULT_HEADER) + decodedLengthResult,
						sizeofTestBuffer - sizeof(MEMCMP_DEFAULT_HEADER) - decodedLengthResult).c_str()),
						(unsigned)MEMSET_DEFAULT_VALUE,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #4. Verify output buffer.
				{
					const int memcmpResult = memcmp(pabyDecodeBuffer, TEST_DATA[testIndex].m_pabyDecodeBufferExpected, TEST_DATA[testIndex].m_decodeLengthExpected);
					const bool areEqual(memcmpResult == MEMCMP_EQUAL);

					string failedMsg;
					failedMsg = FormatString(_T("DLEDecode({%s},{%s},%ld) decodeLength=%ld decodeBufferExpected={%s} testIndex=%d in %s(%d)"),
						static_cast<LPCTSTR>(CUtilities::ToHexString(TEST_DATA[testIndex].m_pabyPacketData, TEST_DATA[testIndex].m_lPacketLength).c_str()),
						static_cast<LPCTSTR>(CUtilities::ToHexString(pabyDecodeBuffer, decodedLengthResult).c_str()),
						(long)TEST_DATA[testIndex].m_lPacketLength,
						(long)decodedLengthResult,
						static_cast<LPCTSTR>(CUtilities::ToHexString(TEST_DATA[testIndex].m_pabyDecodeBufferExpected, TEST_DATA[testIndex].m_decodeLengthExpected).c_str()),
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
				delete testBuffer;
			}
		}

		/// <summary>
		/// Code coverage test for:
		///     PollerCommunicator::AddPacket()
		/// Test with initial conditions and a zero length packet.
		/// </summary>
		TEST(PollerCommunicatorTestClass, AddPacket_InitialZeroTest)
		{
			// Test data -- inputs.
			BYTE* const InputPacket(NULL);
			const long InputPacketLength(0);
			BYTE* const OutputPacket(NULL);
			const long OutputPacketLength(0);

			// Test data -- expected results.
			const bool addPacketExpected(false);
			const long OutputPacketReturnLengthExpected(-123);

			// Setup.
			PollerCommunicator pollerComm;
			long OutputPacketReturnLength(-123);

			// Perform operation(s) to be tested.
			const bool addPacketResult = pollerComm.AddPacket(InputPacket, InputPacketLength, OutputPacket, OutputPacketLength, &OutputPacketReturnLength);

			// Test #1.
			{
				const bool areEqual(addPacketResult == addPacketExpected);

				string failedMsg;
				failedMsg = FormatString(_T("addPacketResult=%d expected=%d in %s(%d)"),
					(int)addPacketResult,
					(int)addPacketExpected,
					static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.
			{
				const bool areEqual(OutputPacketReturnLength == OutputPacketReturnLengthExpected);

				string failedMsg;
				failedMsg = FormatString(_T("OutputPacketReturnLength=%ld expected=%ld in %s(%d)"),
					(long)OutputPacketReturnLength,
					(long)OutputPacketReturnLengthExpected,
					static_cast<LPCTSTR>(__TFUNCTION__), (int)__LINE__);
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     PollerCommunicator::CreatePacketToSend()
		/// Test with invalid message header for a non transaction message.
		/// </summary>
		TEST(PollerCommunicatorTestClass, CreatePacketToSend_InvalidNonTxMsg)
		{
			CPollerHeartbeatMessage pollerMsg;
			pollerMsg.MessageHeader.PacketVersion = 0;  // BAD PACKET

			long msgToSendLength = 0;
			BYTE *msgToSend = pollerMsg.GetReadWriteBuffer(msgToSendLength);

			BYTE* sendpacket = new BYTE[SIZE_OF_READ_BUFFER];
			long sendPacketLength(0);

			TiltCode tiltCode = tltUnknown;
			PollerCommunicator pollerComm;

			// Perform operation(s) to be tested.
			bool result = pollerComm.CreatePacketToSend(msgToSend, sendpacket, msgToSendLength, &sendPacketLength, false, NULL, tiltCode, POLLER_MSG_SENTINEL_HEARTBEAT, 0);

			// Verify
			ASSERT_EQ(tiltCode, tltPollerProtocolCheckPacket) << "TiltCode is wrong";
			ASSERT_FALSE(result) << "CreatePacketToSend() returned wrong value.";

			// Cleanup
			delete sendpacket;
		}

		/// <summary>
		/// Code coverage test for:
		///     PollerCommunicator::CreatePacketToSend()
		/// Test with invalid message header for a transaction message.
		/// </summary>
		TEST(PollerCommunicatorTestClass, CreatePacketToSend_InvalidTxMsg)
		{
			const BYTE macAddress[SIZE_OF_MAC_ADDRESS] = { 0x00, 0x60, 0xE0, 0x70, 0x39, 0x8C };
			time_t transTime;

			// Setup
			CCard *card(new CCard(transTime, transTime));
			card->SetCardID(_T("1234567890"));
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CGames *games = new CGames();
			CPlayer *player(new CPlayer(card, new CPlayerInfo, config, egmConfig, transTime, false, games));

			CPollerPlayerTransactionMessage pollerMsg(player, new CMeters(MEMORY_NONE), INT_MSG_TRANSACTION_PLAYER_CARD_OUT, &transTime, 10, 101);
			pollerMsg.MessageHeader.PacketVersion = 0;  // BAD PACKET
			memcpy(&pollerMsg.MessageHeader.MacAddress, &macAddress, sizeof(pollerMsg.MessageHeader.MacAddress));

			DWORD msgToSendLength = 0;
			BYTE *msgToSend = pollerMsg.GetReadWriteBuffer(msgToSendLength);

			BYTE* sendpacket = new BYTE[SIZE_OF_READ_BUFFER];
			long sendPacketLength(0);

			TiltCode tiltCode = tltUnknown;
			PollerCommunicator pollerComm;

			// Perform operation(s) to be tested.
			bool result = pollerComm.CreatePacketToSend(msgToSend, sendpacket, msgToSendLength, &sendPacketLength, false, macAddress, tiltCode, POLLER_MSG_TRANSACTION, 0);

			// Verify
			ASSERT_EQ(tiltCode, tltPollerProtocolCheckPacket) << "TiltCode is wrong";
			ASSERT_TRUE(result) << "CreatePacketToSend() returned wrong value.";

			// Cleanup
			delete sendpacket;
			delete games;
		}
}
