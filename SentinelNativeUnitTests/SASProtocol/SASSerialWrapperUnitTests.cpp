#if 000//TODO-PORT
#include "stdafx.h"
#include "gmock/gmock.h" // Include before <setupapi.h>.
#include "SASProtocol/SASSerialWrapper.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingUtils.h"
#include "UnitTestingXmlData.h"
#include "UnitTestingXmlLite.h"

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

#define AssertResult5(result, resultLength, expected, expectedLength, resultName) \
	CUnitTestingUtils::AssertEqual(result, resultLength, expected, expectedLength, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult4(result, resultLength, expected, expectedLength) AssertResult5(result, resultLength, expected, expectedLength, _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a class initialization method.
#define AssertResult3i(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, 0, __TFUNCTION__, __LINE__);
#define AssertResult2i(result, expected) AssertResult3i(result, expected, _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a helper method.
#define AssertResult3h(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, functionName, lineNumber, extraIndex);
#define AssertResult2hd(result, expected, dataIndexString) AssertResult3h(result, expected, (dataIndexString + _T(#result)))
#define AssertResult2h(result, expected) AssertResult3h(result, expected, _T(#result))

// For compilers that do not support the C++11 <math.h> round function, provide a rough approximation.
static double round(double value) { return (double)floor((long double)value + 0.5); }

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using ::testing::_;
using ::testing::Invoke;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(SASSerialWrapper)
	{
// #pragma region NonGmockTests
		/// <summary>
		/// Ensure that the first test method of each module does NOT generate a gmock error.
		/// The build server can stop reporting before indicating that a gmock test failed, but indicate that the prior method passed.
		/// Therefore, it is very useful to ensure that the prior method is in the same module.
		/// </summary>
		TEST_METHOD(SASSerialWrapper_NotNullTest)
		{
			// Setup.
			CSASSerialWrapper obj;

			// Test.
			Assert::IsNotNull(&obj, _T("&obj"));
			Assert::IsNotNull(&obj.m_operatingSystem, _T("&obj.m_operatingSystem"));
		}
// #pragma endregion NonGmockTests

		class COperatingSystemTestMock : public COperatingSystemTest
		{
		public:
			COperatingSystemTestMock(UINT sasUartNumber) : COperatingSystemTest(sasUartNumber) {}
			MOCK_METHOD2(S5_UART_ClearBuffers, S5_Result(UINT, USHORT));
			MOCK_METHOD3(S5_UART_SendBuffer, S5_Result(UINT, USHORT *, UINT));
			MOCK_METHOD3(S5_UART_GetBuffer, S5_Result(UINT, USHORT *, UINT &));

			// Use this to call base (parent) methods.
			S5_Result BaseS5_UART_ClearBuffers(UINT uart, USHORT mask) { return COperatingSystemTest::S5_UART_ClearBuffers(uart, mask); }
			S5_Result BaseS5_UART_SendBuffer(UINT uart, USHORT *buffer, UINT length) { return COperatingSystemTest::S5_UART_SendBuffer(uart, buffer, length); }
			S5_Result BaseS5_UART_GetBuffer(UINT uart, USHORT *buffer, UINT& length) { return COperatingSystemTest::S5_UART_GetBuffer(uart, buffer, length); }
		};

		// Expected value(s).
		static const DWORD m_pollingSpeedDefault = CSASSerialWrapper::DEFAULT_POLLING_SPEED_MILLISECONDS;
		static const DWORD m_pollingSpeedOther = CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS;
		static const DWORD m_pollingSpeedMax = CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS; // Typical expected value.

		static const DWORD m_sasUart = 1;
		static const USHORT m_serial9BitNoMark = 0x0000;
		static const USHORT m_serial9BitMark = 0xff00;
#define READ_TIMEOUT_TICK_COUNT 0 // "#define" we can disable tests that would cause compiler error "C4296: '>' : expression is always false".
		static const DWORD m_readTimeoutTickCount = READ_TIMEOUT_TICK_COUNT; // Use 0 for perfCount implementation without 1ms multimedia timers.
		static const DWORD m_sasReadInterval = 24;
		static const USHORT m_maximumReadTickCount = 200;

		static const DWORD m_globalBroadcastAddress = 0;
		static const DWORD m_lastGameAddressDefault = 1;
		static const DWORD m_lastTransmitDurationTickCountDefault = 0;
		static const byte m_gameAddressMask = 0x7f; // The lower 7-bits of the first byte of the buffer is treated as the game address.
		static const UINT m_messageLengthMaximum = 260; // 5 for message header/trailer overhead, plus 255 for the maximum extra length (in an 8-bit length field).

		// Data.
		static const bool m_txMark = true; // Normally, send messages with the first word set to mark parity.

		// Class member(s) used by test (data) methods.
		int m_testIndex;
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
		/// Test static class members.
		/// </summary>
		void TestStaticMembers()
		{
			// Test static default value(s).
			EXPECT_EQ(m_pollingSpeedDefault, CSASSerialWrapper::DEFAULT_POLLING_SPEED_MILLISECONDS);

			EXPECT_EQ(m_sasUart, CSASSerialWrapper::SASUART);
			EXPECT_EQ(COperatingSystemTest::m_sasBitsPerWord, CSASSerialWrapper::SAS_BITS_PER_WORD);
			EXPECT_EQ(m_serial9BitNoMark, CSASSerialWrapper::SER9BIT_NOMARK);
			EXPECT_EQ(m_serial9BitMark, CSASSerialWrapper::SER9BIT_MARK);
			EXPECT_EQ(m_sasReadInterval, CSASSerialWrapper::SASREADINTERVAL);
			EXPECT_EQ(m_maximumReadTickCount, CSASSerialWrapper::MAXREADTIME);

			EXPECT_EQ(m_globalBroadcastAddress, CSASSerialWrapper::GLOBAL_BROADCAST_ADDRESS);
			EXPECT_EQ(m_lastGameAddressDefault, CSASSerialWrapper::DEFAULT_LAST_GAME_ADDRESS);
			EXPECT_EQ(m_lastTransmitDurationTickCountDefault, CSASSerialWrapper::DEFAULT_LAST_TRANSMIT_DURATION_TICK_COUNT);
			EXPECT_EQ(m_gameAddressMask, CSASSerialWrapper::GAME_ADDRESS_MASK);
			EXPECT_EQ(m_messageLengthMaximum, CSASSerialWrapper::MAXIMUM_MESSAGE_LENGTH);
		}

		/// <summary>
		/// Helper function to calculate the polling speed for a transmit to a game address different that the last game address.
		/// </summary>
		DWORD PollingSpeedToDifferentAddress(DWORD pollingSpeed)
		{
			return (pollingSpeed + 1) / 2;
		}

		/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		TEST_METHOD_INITIALIZE(Initialize)
		{
			// Test method initialization code.
			m_testIndex = 0;
			m_gmockFailure.Empty();

			// Perform static tests here so each test method gets a useful error message.
			TestStaticMembers();
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

		void VerifyUartCalls(const COperatingSystemTest &os,
			const COperatingSystemTest::UartDataArray &txDataExpected,
			DWORD writeCalls,
			DWORD readCalls,
			DWORD currentTickCount, // Verify that the expected time has elapsed.
			LPCTSTR functionName,
			int lineNumber)
		{
			// Use extraIndex for the m_uartTxData/txDataExpected index.
			int index;
			int *extraIndex(&index);

			// Report size mismatches after comparing data, as it is probably the less informative.
			int count((int)min(os.m_uartTxData.size(), txDataExpected.size()));
			for (index = 0; index < count; ++index)
			{
				AssertResult2h(os.m_uartTxData[index].m_tickCount, txDataExpected[index].m_tickCount);

				int dataSize((int)min(os.m_uartTxData[index].m_data.size(), txDataExpected[index].m_data.size()));
				for (int dataIndex(0); dataIndex < dataSize; ++dataIndex)
				{
					if (os.m_uartTxData[index].m_data[dataIndex], txDataExpected[index].m_data[dataIndex])
					{
						CString dataIndexString;
						dataIndexString.Format(_T("dataIndex=%d "), dataIndex); // Trailing space.
						AssertResult2hd(os.m_uartTxData[index].m_data[dataIndex], txDataExpected[index].m_data[dataIndex], dataIndexString);
					}
				}

				AssertResult2h(os.m_uartTxData[index].m_data.size(), txDataExpected[index].m_data.size());
			}

			extraIndex = NULL;
			AssertResult2h(os.m_uartTxData.size(), txDataExpected.size());
			AssertResult2h(os.m_tickCount32, currentTickCount);

			// Some or all of these could be reported using gmock.
			AssertResult2h(os.m_uartNumberOtherCount, (DWORD)0);
			AssertResult2h(os.m_uartClearTxCount, (DWORD)0);
			AssertResult2h(os.m_uartClearRxCount, (DWORD)writeCalls);
			AssertResult2h(os.m_uartClearBothCount, (DWORD)0);
			AssertResult2h(os.m_uartClearNoneCount, (DWORD)0);
			AssertResult2h(os.m_uartWriteCount, (DWORD)writeCalls);
			AssertResult2h(os.m_uartReadCount, (DWORD)readCalls);
			AssertResult2h(os.m_uartRxUnexpected, false);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::CSASSerialWrapper()
		/// Test m_operatingSystem initialization.
		/// </summary>
		TEST_METHOD(CSASSerialWrapper_DefaultTest)
		{
			// Perform operation(s) and test(s).
			CSASSerialWrapper obj;
			EXPECT_EQ(&COperatingSystem::m_operatingSystem, &obj.m_operatingSystem);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::CSASSerialWrapper()
		/// Test m_operatingSystem initialization.
		/// </summary>
		TEST_METHOD(CSASSerialWrapper_TestTest)
		{
			// Setup.
			COperatingSystemTestMock os(m_sasUart);

			// Perform operation(s) and test(s).
			CSASSerialWrapper obj(os);
			EXPECT_EQ(&os, &obj.m_operatingSystem);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::WriteSAS()
		/// Call the WriteSAS() function with a NULL buffer.
		/// </summary>
		TEST_METHOD(WriteSAS_NullTest)
		{
			// Data.
			const byte * const message(NULL);
			const DWORD messageLength(1);

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			ON_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_ClearBuffers));
			ON_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_SendBuffer));
			ON_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_GetBuffer));

			// Expect 0 calls with any (other) conditions.
			EXPECT_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).Times(0);

			CSASSerialWrapper obj(os);

			// Perform operation(s) and test(s).
			obj.OpenSAS();
			obj.WriteSAS(message, messageLength, m_txMark);
			obj.CloseSAS();
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::WriteSAS()
		/// Call the WriteSAS() function with a zero length buffer.
		/// </summary>
		TEST_METHOD(WriteSAS_ZeroLengthTest)
		{
			// Data.
			const byte message[1] = { 1 };
			const DWORD messageLength(0);

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			ON_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_ClearBuffers));
			ON_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_SendBuffer));
			ON_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_GetBuffer));

			// Expect 0 calls with any (other) conditions.
			EXPECT_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).Times(0);

			CSASSerialWrapper obj(os);

			// Perform operation(s) and test(s).
			obj.OpenSAS();
			obj.WriteSAS(message, messageLength, m_txMark);
			obj.CloseSAS();
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::WriteSAS()
		/// Call the WriteSAS() function with a 1-byte message.
		/// </summary>
		TEST_METHOD(WriteSAS1Test)
		{
			// Data.
			const byte message[] = { 0x81 }; // Typical general poll.

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			ON_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_ClearBuffers));
			ON_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_SendBuffer));
			ON_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_GetBuffer));

			// Expect 0 calls with any (other) conditions.
			EXPECT_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).Times(0);

			EXPECT_CALL(os, S5_UART_ClearBuffers(m_sasUart, CLR_RX_BUFFER)).Times(1);
			EXPECT_CALL(os, S5_UART_SendBuffer(m_sasUart, testing::_, _countof(message))).Times(1);

			CSASSerialWrapper obj(os);

			// Perform operation(s) and test(s).
			obj.OpenSAS();
			obj.WriteSAS(message, _countof(message), m_txMark);
			obj.CloseSAS();
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::WriteSAS()
		/// Call the WriteSAS() function with a 1-byte message, with the mark turned off.
		/// </summary>
		TEST_METHOD(WriteSAS_NoMark1Test)
		{
			// Data.
			const bool txMark(false);
			const DWORD tickCountInit(0);
			const DWORD tickCountStart(1000); // At least tickCountInit plus the polling speed period.
			const byte message[] = { 0x81 }; // Typical general poll.
			const USHORT uartTxDataExpected[1] = { 0x0081 };

			COperatingSystemTest::UartDataArray txDataExpected;
			txDataExpected.push_back(COperatingSystemTest::UartDataRecord(tickCountStart,
				COperatingSystemTest::UartData(uartTxDataExpected, uartTxDataExpected + _countof(uartTxDataExpected))));

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountInit;
			ON_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_ClearBuffers));
			ON_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_SendBuffer));
			ON_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_GetBuffer));

			// Expect 0 calls with any (other) conditions.
			EXPECT_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).Times(0);

			EXPECT_CALL(os, S5_UART_ClearBuffers(m_sasUart, CLR_RX_BUFFER)).Times(1);
			EXPECT_CALL(os, S5_UART_SendBuffer(m_sasUart, testing::_, _countof(message))).Times(1);

			CSASSerialWrapper obj(os);
			os.m_perfCount = os.m_tickCount32 = tickCountStart;

			// Perform operation(s) and test(s).
			obj.OpenSAS();
			obj.WriteSAS(message, _countof(message), txMark);
			obj.CloseSAS();

			VerifyUartCalls(os, txDataExpected, 1, 0, tickCountStart, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::WriteSAS()
		/// Call the WriteSAS() function with a 2-byte message, with the mark turned off.
		/// </summary>
		TEST_METHOD(WriteSAS_NoMark2Test)
		{
			// Data.
			const bool txMark(false);
			const DWORD tickCountInit(0);
			const DWORD tickCountStart(1000); // At least tickCountInit plus the polling speed period.
			const byte message[] = { 0x01, 0x19 }; // Typical 2-byte long poll.
			const USHORT uartTxDataExpected[] = { 0x0001, 0x0019 };

			COperatingSystemTest::UartDataArray txDataExpected;
			txDataExpected.push_back(COperatingSystemTest::UartDataRecord(tickCountStart,
				COperatingSystemTest::UartData(uartTxDataExpected, uartTxDataExpected + _countof(uartTxDataExpected))));

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountInit;
			ON_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_ClearBuffers));
			ON_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_SendBuffer));
			ON_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_GetBuffer));

			// Expect 0 calls with any (other) conditions.
			EXPECT_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).Times(0);

			EXPECT_CALL(os, S5_UART_ClearBuffers(m_sasUart, CLR_RX_BUFFER)).Times(1);
			EXPECT_CALL(os, S5_UART_SendBuffer(m_sasUart, testing::_, _countof(message))).Times(1);

			CSASSerialWrapper obj(os);
			os.m_perfCount = os.m_tickCount32 = tickCountStart;

			// Perform operation(s) and test(s).
			obj.OpenSAS();
			obj.WriteSAS(message, _countof(message), txMark);
			obj.CloseSAS();

			VerifyUartCalls(os, txDataExpected, 1, 0, tickCountStart, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::WriteSAS()
		/// Call the WriteSAS() function with a 1-byte message, with the mark turned on.
		/// Call the WriteSAS() function with a 2-byte message, with the mark turned off.
		/// Test with polling speed set to 40.
		/// </summary>
		TEST_METHOD(WriteSAS_MarkNoMark40Test)
		{
			// Data.
			const short pollingSpeed(40);
			const bool txMark(false);
			const DWORD tickCountInit(0);
			const DWORD tickCountStart(1000); // At least tickCountInit plus the polling speed period.
			const byte message1[] = { 0x81 }; // Typical general poll.
			const USHORT uartTxDataExpected1[1] = { 0x0081 };
			const byte message2[] = { 0x01, 0x19 }; // Typical 2-byte long poll.
			const USHORT uartTxDataExpected2[] = { 0x0001, 0x0019 };

			COperatingSystemTest::UartDataArray txDataExpected;
			DWORD tickCountCurrent(tickCountStart);
			txDataExpected.push_back(COperatingSystemTest::UartDataRecord(tickCountCurrent,
				COperatingSystemTest::UartData(uartTxDataExpected1, uartTxDataExpected1 + _countof(uartTxDataExpected1))));
			tickCountCurrent += COperatingSystemTest::CalcSasMessageTickCountDuration(_countof(message1)) + pollingSpeed;
			txDataExpected.push_back(COperatingSystemTest::UartDataRecord(tickCountCurrent,
				COperatingSystemTest::UartData(uartTxDataExpected2, uartTxDataExpected2 + _countof(uartTxDataExpected2))));

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountInit;
			ON_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_ClearBuffers));
			ON_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_SendBuffer));
			ON_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_GetBuffer));

			// Expect 0 calls with any (other) conditions.
			EXPECT_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).Times(0);

			EXPECT_CALL(os, S5_UART_ClearBuffers(m_sasUart, CLR_RX_BUFFER)).Times(2);
			EXPECT_CALL(os, S5_UART_SendBuffer(m_sasUart, testing::_, _countof(message1))).Times(1);
			EXPECT_CALL(os, S5_UART_SendBuffer(m_sasUart, testing::_, _countof(message2))).Times(1);

			CSASSerialWrapper obj(os);
			os.m_perfCount = os.m_tickCount32 = tickCountStart;
			obj.SetPollingSpeed(pollingSpeed);

			// Perform operation(s) and test(s).
			obj.OpenSAS();
			obj.WriteSAS(message1, _countof(message1), txMark);
			obj.WriteSAS(message2, _countof(message2), txMark);
			obj.CloseSAS();

			VerifyUartCalls(os, txDataExpected, 2, 0, tickCountCurrent, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::WriteSAS()
		/// Call the WriteSAS() function with a 1-byte message, with the mark turned on.
		/// Call the WriteSAS() function with a 2-byte message, with the mark turned off.
		/// Test with polling speed set to 200.
		/// </summary>
		TEST_METHOD(WriteSAS_MarkNoMark200Test)
		{
			// Data.
			const short pollingSpeed(200);
			const bool txMark(false);
			const DWORD tickCountInit(0);
			const DWORD tickCountStart(1000); // At least tickCountInit plus the polling speed period.
			const byte message1[] = { 0x81 }; // Typical general poll.
			const USHORT uartTxDataExpected1[1] = { 0x0081 };
			const byte message2[] = { 0x01, 0x19 }; // Typical 2-byte long poll.
			const USHORT uartTxDataExpected2[] = { 0x0001, 0x0019 };

			COperatingSystemTest::UartDataArray txDataExpected;
			DWORD tickCountCurrent(tickCountStart);
			txDataExpected.push_back(COperatingSystemTest::UartDataRecord(tickCountCurrent,
				COperatingSystemTest::UartData(uartTxDataExpected1, uartTxDataExpected1 + _countof(uartTxDataExpected1))));
			tickCountCurrent += COperatingSystemTest::CalcSasMessageTickCountDuration(_countof(message1)) + pollingSpeed;
			txDataExpected.push_back(COperatingSystemTest::UartDataRecord(tickCountCurrent,
				COperatingSystemTest::UartData(uartTxDataExpected2, uartTxDataExpected2 + _countof(uartTxDataExpected2))));

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountInit;
			ON_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_ClearBuffers));
			ON_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_SendBuffer));
			ON_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_GetBuffer));

			// Expect 0 calls with any (other) conditions.
			EXPECT_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).Times(0);

			EXPECT_CALL(os, S5_UART_ClearBuffers(m_sasUart, CLR_RX_BUFFER)).Times(2);
			EXPECT_CALL(os, S5_UART_SendBuffer(m_sasUart, testing::_, _countof(message1))).Times(1);
			EXPECT_CALL(os, S5_UART_SendBuffer(m_sasUart, testing::_, _countof(message2))).Times(1);

			CSASSerialWrapper obj(os);
			os.m_perfCount = os.m_tickCount32 = tickCountStart;
			obj.SetPollingSpeed(pollingSpeed);

			// Perform operation(s) and test(s).
			obj.OpenSAS();
			obj.WriteSAS(message1, _countof(message1), txMark);
			obj.WriteSAS(message2, _countof(message2), txMark);
			obj.CloseSAS();

			VerifyUartCalls(os, txDataExpected, 2, 0, tickCountCurrent, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::WriteSAS()
		/// Call the WriteSAS() function with a 1-byte message.
		/// </summary>
		TEST_METHOD(WriteSAS_Mark1Test)
		{
			// Data.
			const DWORD tickCountInit(0);
			const DWORD tickCountStart(1000); // At least tickCountInit plus the polling speed period.
			const byte message[] = { 0x81 }; // Typical general poll.
			const USHORT uartTxDataExpected[] = { 0xff81 };

			COperatingSystemTest::UartDataArray txDataExpected;
			txDataExpected.push_back(COperatingSystemTest::UartDataRecord(tickCountStart,
				COperatingSystemTest::UartData(uartTxDataExpected, uartTxDataExpected + _countof(uartTxDataExpected))));

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountInit;
			ON_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_ClearBuffers));
			ON_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_SendBuffer));
			ON_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_GetBuffer));

			// Expect 0 calls with any (other) conditions.
			EXPECT_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).Times(0);

			EXPECT_CALL(os, S5_UART_ClearBuffers(m_sasUart, CLR_RX_BUFFER)).Times(1);
			EXPECT_CALL(os, S5_UART_SendBuffer(m_sasUart, testing::_, _countof(message))).Times(1);

			CSASSerialWrapper obj(os);
			os.m_perfCount = os.m_tickCount32 = tickCountStart;

			// Perform operation(s) and test(s).
			obj.OpenSAS();
			obj.WriteSAS(message, _countof(message), m_txMark);
			obj.CloseSAS();

			VerifyUartCalls(os, txDataExpected, 1, 0, tickCountStart, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::ReadSAS()
		/// Call the ReadSAS() function with a NULL buffer.
		/// </summary>
		TEST_METHOD(ReadSAS_NullTest)
		{
			// Data.
			const DWORD tickCountStart(1000);

			COperatingSystemTest::UartDataArray txDataExpected;

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountStart;
			ON_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_ClearBuffers));
			ON_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_SendBuffer));
			ON_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_GetBuffer));

			// Expect 0 calls with any (other) conditions.
			EXPECT_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).Times(0);

			CSASSerialWrapper obj(os);

			// Perform operation(s) and test(s).
			obj.OpenSAS();
			byte *message(NULL);
			DWORD messageLength(1);
			DWORD numberBytesRead;
			obj.ReadSAS(message, messageLength, numberBytesRead);
			obj.CloseSAS();

			VerifyUartCalls(os, txDataExpected, 0, 0, tickCountStart, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::ReadSAS()
		/// Call the ReadSAS() function with a zero-length buffer.
		/// </summary>
		TEST_METHOD(ReadSAS_ZeroLengthTest)
		{
			// Data.
			const DWORD tickCountStart(1000);

			COperatingSystemTest::UartDataArray txDataExpected;

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountStart;
			ON_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_ClearBuffers));
			ON_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_SendBuffer));
			ON_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_GetBuffer));

			// Expect 0 calls with any (other) conditions.
			EXPECT_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).Times(0);

			CSASSerialWrapper obj(os);

			// Perform operation(s) and test(s).
			obj.OpenSAS();
			byte message[1];
			DWORD messageLength(0);
			DWORD numberBytesRead;
			obj.ReadSAS(message, messageLength, numberBytesRead);
			obj.CloseSAS();

			VerifyUartCalls(os, txDataExpected, 0, 0, tickCountStart, __TFUNCTION__, __LINE__);
		}

		DWORD CalcReadTickCountTimeout(DWORD &getBufferCount)
		{
			DWORD getBufferTickCountTimeout(COperatingSystemTest::m_rxTickCountNoDataTimeout);
			getBufferCount = (m_maximumReadTickCount + getBufferTickCountTimeout - 1) / getBufferTickCountTimeout;
			return getBufferTickCountTimeout * getBufferCount;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::ReadSAS()
		/// Call the ReadSAS() function, asking for 1 byte, but expecting 0 bytes returned.
		/// </summary>
		TEST_METHOD(ReadSAS10Test)
		{
			// Data.
			const DWORD tickCountStart(1000);
			const DWORD numberBytesReadExpected(0);
			DWORD getBufferCountExpected;
			const DWORD tickCountEnd(tickCountStart + CalcReadTickCountTimeout(getBufferCountExpected));

			COperatingSystemTest::UartDataArray txDataExpected;

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountStart;
			ON_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_ClearBuffers));
			ON_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_SendBuffer));
			ON_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_GetBuffer));

			// Expect 0 calls with any (other) conditions.
			EXPECT_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).Times(0);

			EXPECT_CALL(os, S5_UART_GetBuffer(m_sasUart, testing::_, testing::_)).Times(getBufferCountExpected);

			CSASSerialWrapper obj(os);

			// Perform operation(s) and test(s).
			obj.OpenSAS();
			byte rxMessage[1];
			DWORD numberBytesRead;
			obj.ReadSAS(rxMessage, _countof(rxMessage), numberBytesRead);
			AssertResult2(numberBytesRead, numberBytesReadExpected);
			obj.CloseSAS();

			VerifyUartCalls(os, txDataExpected, 0, getBufferCountExpected, tickCountEnd, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::ReadSAS()
		/// Call the ReadSAS() function, asking for 1 byte and expecting 1 bytes returned on the first GetBuffer call.
		/// </summary>
		TEST_METHOD(ReadSAS11Test)
		{
			// Data.
			const DWORD tickCountStart(1000);

			const DWORD rxTickCountDelay(10);
			const USHORT uartRxData[] = { 0x1f }; // Typical general poll response.
			const byte rxMessageExpected[] = { 0x1f }; // Typical general poll response.
			const DWORD tickCountEnd(tickCountStart + rxTickCountDelay + COperatingSystemTest::CalcSasMessageTickCountDuration(_countof(rxMessageExpected)));

			COperatingSystemTest::UartDataArray txDataExpected;

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountStart;
			os.m_uartRxData.push_back(COperatingSystemTest::UartDataRecord(rxTickCountDelay,
				COperatingSystemTest::UartData(uartRxData, uartRxData + _countof(uartRxData))));
			ON_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_ClearBuffers));
			ON_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_SendBuffer));
			ON_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_GetBuffer));

			// Expect 0 calls with any (other) conditions.
			EXPECT_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).Times(0);

			EXPECT_CALL(os, S5_UART_GetBuffer(m_sasUart, testing::_, testing::_)).Times(os.m_uartRxData.size());

			CSASSerialWrapper obj(os);

			// Perform operation(s) and test(s).
			obj.OpenSAS();
			byte rxMessage[1];
			DWORD numberBytesRead;
			obj.ReadSAS(rxMessage, _countof(rxMessage), numberBytesRead);
			AssertResult4(rxMessage, numberBytesRead, rxMessageExpected, _countof(rxMessageExpected));
			obj.CloseSAS();

			VerifyUartCalls(os, txDataExpected, 0, os.m_uartRxData.size(), tickCountEnd, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::ReadSAS()
		/// Call the ReadSAS() function, asking for 1 byte
		/// and expecting 0 bytes returned on the first GetBuffer call
		/// and expecting 1 bytes returned on the second GetBuffer call.
		/// </summary>
		TEST_METHOD(ReadSAS101Test)
		{
			// Data.
			const DWORD tickCountStart(1000);

			const DWORD rxTickCountDelay(10);
			const USHORT uartRxData[] = { 0x1f }; // Typical general poll response.
			const byte rxMessageExpected[] = { 0x1f }; // Typical general poll response.
			const DWORD tickCountEnd(tickCountStart + rxTickCountDelay + COperatingSystemTest::m_rxTickCountNoDataTimeout +
				COperatingSystemTest::CalcSasMessageTickCountDuration(_countof(rxMessageExpected)));

			COperatingSystemTest::UartDataArray txDataExpected;

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountStart;
			os.m_uartRxData.push_back(COperatingSystemTest::UartDataRecord(COperatingSystemTest::m_rxTickCountNoDataTimeout,
				COperatingSystemTest::UartData(NULL, NULL)));
			os.m_uartRxData.push_back(COperatingSystemTest::UartDataRecord(rxTickCountDelay,
				COperatingSystemTest::UartData(uartRxData, uartRxData + _countof(uartRxData))));
			ON_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_ClearBuffers));
			ON_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_SendBuffer));
			ON_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_GetBuffer));

			// Expect 0 calls with any (other) conditions.
			EXPECT_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).Times(0);

			EXPECT_CALL(os, S5_UART_GetBuffer(m_sasUart, testing::_, testing::_)).Times(os.m_uartRxData.size());

			CSASSerialWrapper obj(os);

			// Perform operation(s) and test(s).
			obj.OpenSAS();
			byte rxMessage[1];
			DWORD numberBytesRead;
			obj.ReadSAS(rxMessage, _countof(rxMessage), numberBytesRead);
			AssertResult4(rxMessage, numberBytesRead, rxMessageExpected, _countof(rxMessageExpected));
			obj.CloseSAS();

			VerifyUartCalls(os, txDataExpected, 0, os.m_uartRxData.size(), tickCountEnd, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::WriteSAS()
		///     CSASSerialWrapper::ReadSAS()
		/// Call the Write() function with a 1-byte message and get a 1-byte response.
		/// </summary>
		TEST_METHOD(WriteSAS_Read11Test)
		{
			// Data.
			const DWORD tickCountInit(0);
			const DWORD tickCountStart(1000); // At least tickCountInit plus the polling speed period.

			const DWORD rxTickCountDelay(10);
			const USHORT uartRxData[] = { 0x1f }; // Typical general poll response.
			const byte rxMessageExpected[] = { 0x1f }; // Typical general poll response.

			const byte txMessage[] = { 0x81 }; // Typical general poll.
			const USHORT uartTxDataExpected[] = { 0xff81 };
			COperatingSystemTest::UartDataArray txDataExpected;
			txDataExpected.push_back(COperatingSystemTest::UartDataRecord(tickCountStart,
				COperatingSystemTest::UartData(uartTxDataExpected, uartTxDataExpected + _countof(uartTxDataExpected))));

			const DWORD tickCountEnd(tickCountStart +
				rxTickCountDelay +
				COperatingSystemTest::CalcSasMessageTickCountDuration(_countof(txMessage)) +
				COperatingSystemTest::CalcSasMessageTickCountDuration(_countof(rxMessageExpected)));

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountInit;
			os.m_uartRxData.push_back(COperatingSystemTest::UartDataRecord(rxTickCountDelay,
				COperatingSystemTest::UartData(uartRxData, uartRxData + _countof(uartRxData))));
			ON_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_ClearBuffers));
			ON_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_SendBuffer));
			ON_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_GetBuffer));

			EXPECT_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).Times(0);

			EXPECT_CALL(os, S5_UART_ClearBuffers(m_sasUart, CLR_RX_BUFFER)).Times(1);
			EXPECT_CALL(os, S5_UART_SendBuffer(m_sasUart, testing::_, _countof(txMessage))).Times(1);
			EXPECT_CALL(os, S5_UART_GetBuffer(m_sasUart, testing::_, testing::_)).Times(os.m_uartRxData.size());

			CSASSerialWrapper obj(os);
			os.m_perfCount = os.m_tickCount32 = tickCountStart;

			// Perform operation(s) and test(s).
			obj.OpenSAS();
			obj.WriteSAS(txMessage, _countof(txMessage), m_txMark);
			BYTE rxMessage[1];
			DWORD numberBytesRead;
			obj.ReadSAS(rxMessage, _countof(rxMessage), numberBytesRead);
			AssertResult4(rxMessage, numberBytesRead, rxMessageExpected, _countof(rxMessageExpected));
			obj.CloseSAS();

			VerifyUartCalls(os, txDataExpected, txDataExpected.size(), os.m_uartRxData.size(), tickCountEnd, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::WriteSAS()
		///     CSASSerialWrapper::ReadSAS()
		/// Call the Write() function with a 1-byte message.
		/// </summary>
		TEST_METHOD(WriteSAS_ReadNTest)
		{
			// Data.
			const DWORD tickCountInit(0);
			const DWORD tickCountStart(1000); // At least tickCountInit plus the polling speed period.

			const byte txMessage[] = { 0x01, 0x19 }; // Typical 2-byte long poll.
			const USHORT uartTxDataExpected[] = { 0xff01, 0x0019 };
			COperatingSystemTest::UartDataArray txDataExpected;
			txDataExpected.push_back(COperatingSystemTest::UartDataRecord(tickCountStart,
				COperatingSystemTest::UartData(uartTxDataExpected, uartTxDataExpected + _countof(uartTxDataExpected))));

			const DWORD rxTickCountDelay(10);
			byte rxMessageExpected[24] = { 0x01, 0x19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xcc, 0xcc };
			CUnitTestingUtils::SetSasCrc(rxMessageExpected, sizeof(rxMessageExpected));
			USHORT uartRxData[sizeof(rxMessageExpected)];
			for (size_t index(0); index < _countof(uartRxData); ++index)
			{
				uartRxData[index] = rxMessageExpected[index];
			}

			const DWORD tickCountEnd(tickCountStart +
				rxTickCountDelay +
				COperatingSystemTest::CalcSasMessageTickCountDuration(_countof(txMessage)) +
				COperatingSystemTest::CalcSasMessageTickCountDuration(_countof(rxMessageExpected)));

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountInit;
			os.m_uartRxData.push_back(COperatingSystemTest::UartDataRecord(rxTickCountDelay,
				COperatingSystemTest::UartData(uartRxData, uartRxData + _countof(uartRxData))));
			ON_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_ClearBuffers));
			ON_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_SendBuffer));
			ON_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).WillByDefault(Invoke(&os, &COperatingSystemTestMock::BaseS5_UART_GetBuffer));

			EXPECT_CALL(os, S5_UART_ClearBuffers(testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_SendBuffer(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(os, S5_UART_GetBuffer(testing::_, testing::_, testing::_)).Times(0);

			EXPECT_CALL(os, S5_UART_ClearBuffers(m_sasUart, CLR_RX_BUFFER)).Times(1);
			EXPECT_CALL(os, S5_UART_SendBuffer(m_sasUart, testing::_, _countof(txMessage))).Times(1);
			EXPECT_CALL(os, S5_UART_GetBuffer(m_sasUart, testing::_, testing::_)).Times(os.m_uartRxData.size());

			CSASSerialWrapper obj(os);
			os.m_perfCount = os.m_tickCount32 = tickCountStart;

			// Perform operation(s) and test(s).
			obj.OpenSAS();
			obj.WriteSAS(txMessage, _countof(txMessage), m_txMark);
			BYTE rxMessage[sizeof(rxMessageExpected)];
			DWORD numberBytesRead;
			obj.ReadSAS(rxMessage, _countof(rxMessage), numberBytesRead);
			AssertResult4(rxMessage, numberBytesRead, rxMessageExpected, _countof(rxMessageExpected));
			obj.CloseSAS();

			VerifyUartCalls(os, txDataExpected, txDataExpected.size(), os.m_uartRxData.size(), tickCountEnd, __TFUNCTION__, __LINE__);
		}

// #pragma region CSASSerialWrapper::m_pollingSpeed
		/// <summary>
		/// Functional test for:
		///     CSASSerialWrapper
		/// Test static const members.
		/// </summary>
		TEST_METHOD(SetPollingSpeed_StaticConst)
		{
			Assert::AreEqual((short)200, CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS, L"MS200");
			Assert::AreEqual((short)40, CSASSerialWrapper::MS40_POLLING_SPEED_MILLISECONDS, L"MS40");
			Assert::AreEqual((short)40, CSASSerialWrapper::DEFAULT_POLLING_SPEED_MILLISECONDS, L"DEFAULT");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::CSASSerialWrapper()
		/// Test m_pollingSpeed initialization with default operating environment.
		/// </summary>
		TEST_METHOD(SetPollingSpeed_DefaultTest)
		{
			// Perform operation(s) and test(s).
			CSASSerialWrapper obj;
			EXPECT_EQ(m_pollingSpeedDefault, obj.m_pollingSpeed);
			EXPECT_EQ(m_pollingSpeedDefault, obj.GetPollingSpeed());
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::CSASSerialWrapper()
		/// Test m_pollingSpeed initialization with operating environment for testing.
		/// </summary>
		TEST_METHOD(SetPollingSpeed_DefaultTestTest)
		{
			// Setup.
			COperatingSystemTestMock os(m_sasUart);

			// Perform operation(s) and test(s).
			CSASSerialWrapper obj(os);
			EXPECT_EQ(m_pollingSpeedDefault, obj.m_pollingSpeed);
			EXPECT_EQ(m_pollingSpeedDefault, obj.GetPollingSpeed());
		}

		/// <summary>
		/// Functional test for:
		///     CSASSerialWrapper::SetPollingSpeed()
		///     CSASSerialWrapper::GetPollingSpeed()
		/// Test non-default value(s).
		TEST_METHOD(SetPollingSpeed)
		{
			// Setup.
			CSASSerialWrapper obj;

			// Perform operation(s) and test(s).
			obj.SetPollingSpeed(CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS);
			Assert::AreEqual(CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS, obj.GetPollingSpeed());

			obj.SetPollingSpeed(CSASSerialWrapper::MS40_POLLING_SPEED_MILLISECONDS);
			Assert::AreEqual(CSASSerialWrapper::MS40_POLLING_SPEED_MILLISECONDS, obj.GetPollingSpeed());

			obj.SetPollingSpeed(CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS);
			Assert::AreEqual(CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS, obj.GetPollingSpeed());
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::SetPollingSpeed()
		///     CSASSerialWrapper::GetPollingSpeed()
		/// Test with a variety of values.
		/// </summary>
		TEST_METHOD(SetPollingSpeed_DataTest)
		{
			// Data.
			typedef short TestDataType;
			const TestDataType thresholdA(m_pollingSpeedDefault);
			const TestDataType thresholdB(m_pollingSpeedOther);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(thresholdA, thresholdB), &m_testIndex);

			// Setup.  Use the same object for each test.
			CSASSerialWrapper obj;

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Perform operation(s) and test(s).
				obj.SetPollingSpeed(testValue);
				Assert::AreEqual((DWORD)testValue, obj.m_pollingSpeed, L"m_pollingSpeed");
				Assert::AreEqual(testValue, obj.GetPollingSpeed(), L"GetPollingSpeed");
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}
// #pragma endregion CSASSerialWrapper::m_pollingSpeed

// #pragma region CSASSerialWrapper::SetLastTransmittedByteTickCount
		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::SetLastTransmittedByteTickCount()
		/// Test initialization.
		/// </summary>
		TEST_METHOD(SetLastTransmittedByteTickCount_DefaultTest)
		{
			const DWORD tickCountInit(1000);

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_perfCount = os.m_tickCount32 = tickCountInit;

			// Perform operation(s) and test(s).
			CSASSerialWrapper obj(os);
			EXPECT_EQ(m_lastGameAddressDefault, obj.m_lastGameAddress);
			EXPECT_EQ(tickCountInit, (DWORD)round(obj.m_lastTransmitStartPerfCount));
			EXPECT_EQ(m_lastTransmitDurationTickCountDefault, (DWORD)ceil(obj.m_lastTransmitDurationPerfCount));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::SetLastTransmittedByteTickCount()
		/// Test simple case.
		/// </summary>
		TEST_METHOD(SetLastTransmittedByteTickCount1Test)
		{
			const DWORD tickCountInit(1000);
			const DWORD tickCountStart(3000);
			const byte gameAddress(0x0c);
			const UINT bufferLen(1);

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountInit;
			CSASSerialWrapper obj(os);
			os.m_perfCount = os.m_tickCount32 = tickCountStart;

			// Perform operation(s) and test(s).
			obj.SetLastTransmittedByteTickCount(gameAddress, bufferLen);
			EXPECT_EQ(gameAddress, obj.m_lastGameAddress);
			EXPECT_EQ(tickCountStart, (DWORD)round(obj.m_lastTransmitStartPerfCount));
			EXPECT_EQ(COperatingSystemTest::CalcSasMessageTickCountDuration(bufferLen), (DWORD)ceil(obj.m_lastTransmitDurationPerfCount));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::SetLastTransmittedByteTickCount()
		/// Test with various game address values.
		/// </summary>
		TEST_METHOD(SetLastTransmittedByteTickCount_GameAddressTest)
		{
			// Data.
			const DWORD tickCountInit(1000);
			const DWORD tickCountStart(3000);
			const UINT bufferLen(1);

			typedef byte TestDataType;
			const TestDataType thresholdA(m_pollingSpeedDefault);
			const TestDataType thresholdB(m_pollingSpeedOther);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(thresholdA, thresholdB), &m_testIndex);

			// Setup.  Use the same object for each test.
			COperatingSystemTestMock osA(m_sasUart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountInit;
			CSASSerialWrapper objA(osA);
			osA.m_perfCount = osA.m_tickCount32 = tickCountStart;

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Perform operation(s) and test(s).
				objA.SetLastTransmittedByteTickCount(testValue, bufferLen);
				EXPECT_EQ(testValue, objA.m_lastGameAddress);
				EXPECT_EQ(tickCountStart, (DWORD)round(objA.m_lastTransmitStartPerfCount));
				EXPECT_EQ(COperatingSystemTest::CalcSasMessageTickCountDuration(bufferLen), (DWORD)ceil(objA.m_lastTransmitDurationPerfCount));

				// Setup.  Use a different object for each test sequence.
				COperatingSystemTestMock osB(m_sasUart);
				osB.m_perfCount = osB.m_tickCount32 = tickCountInit;
				CSASSerialWrapper objB(osB);
				osB.m_perfCount = osB.m_tickCount32 = tickCountStart;

				// Perform operation(s) and test(s).
				objB.SetLastTransmittedByteTickCount(testValue, bufferLen);
				EXPECT_EQ(testValue, objB.m_lastGameAddress);
				EXPECT_EQ(tickCountStart, (DWORD)round(objB.m_lastTransmitStartPerfCount));
				EXPECT_EQ(COperatingSystemTest::CalcSasMessageTickCountDuration(bufferLen), (DWORD)ceil(objB.m_lastTransmitDurationPerfCount));
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::SetLastTransmittedByteTickCount()
		/// Test with various buffer length values.
		/// </summary>
		TEST_METHOD(SetLastTransmittedByteTickCount_BufferLenTest)
		{
			// Data.
			const DWORD tickCountInit(1000);
			const DWORD tickCountStart(3000);
			const byte gameAddress(55);
			UINT minimumMessageLength(0); // For normal use, a minimum length of 1 is expected, but test here with a minimum length of 0 for completeness.

			typedef UINT TestDataType;

			// Setup.  Use the same object for each test.
			COperatingSystemTestMock osA(m_sasUart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountInit;
			CSASSerialWrapper objA(osA);
			osA.m_perfCount = osA.m_tickCount32 = tickCountStart;

			for (m_testIndex = (int)minimumMessageLength; m_testIndex <= (int)m_messageLengthMaximum; ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue(m_testIndex);

				// Perform operation(s) and test(s).
				objA.SetLastTransmittedByteTickCount(gameAddress, testValue);
				AssertResult2(objA.m_lastGameAddress, gameAddress);
				AssertResult2((DWORD)round(objA.m_lastTransmitStartPerfCount), tickCountStart);
				AssertResult2(objA.m_lastTransmitDurationPerfCount, COperatingSystemTest::CalcSasMessagePerfCountDuration(testValue));

				// Setup.  Use a different object for each test sequence.
				COperatingSystemTestMock osB(m_sasUart);
				osB.m_perfCount = osB.m_tickCount32 = tickCountInit;
				CSASSerialWrapper objB(osB);
				osB.m_perfCount = osB.m_tickCount32 = tickCountStart;

				// Perform operation(s) and test(s).
				objB.SetLastTransmittedByteTickCount(gameAddress, testValue);
				AssertResult2(objB.m_lastGameAddress, gameAddress);
				AssertResult2((DWORD)round(objB.m_lastTransmitStartPerfCount), tickCountStart);
				AssertResult2(objB.m_lastTransmitDurationPerfCount, COperatingSystemTest::CalcSasMessagePerfCountDuration(testValue));

			}
		}
// #pragma endregion CSASSerialWrapper::SetLastTransmittedByteTickCount

// #pragma region CSASSerialWrapper::SetLastReceivedByteTickCount
		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::SetLastReceivedByteTickCount()
		/// Test initialization.
		/// </summary>
		TEST_METHOD(SetLastReceivedByteTickCount_DefaultTest)
		{
			const DWORD tickCountInit(1000);

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountInit;

			// Perform operation(s) and test(s).
			CSASSerialWrapper obj(os);
			EXPECT_EQ(tickCountInit, (DWORD)round(obj.m_lastReceivedBytePerfCount));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::SetLastReceivedByteTickCount()
		/// Test partial read.
		/// </summary>
		TEST_METHOD(SetLastReceivedByteTickCount_ReadPartialTest)
		{
			const DWORD tickCountInit(1000);
			const DWORD tickCountStart(3000);

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountInit;
			CSASSerialWrapper obj(os);
			os.m_perfCount = os.m_tickCount32 = tickCountStart;

			// Perform operation(s) and test(s).
			obj.SetLastReceivedByteTickCount(false);
			EXPECT_EQ(tickCountStart - m_readTimeoutTickCount, (DWORD)round(obj.m_lastReceivedBytePerfCount));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::SetLastReceivedByteTickCount()
		/// Test read all.
		/// </summary>
		TEST_METHOD(SetLastReceivedByteTickCount_ReadAllTest)
		{
			const DWORD tickCountInit(1000);
			const DWORD tickCountStart(3000);

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountInit;
			CSASSerialWrapper obj(os);
			os.m_perfCount = os.m_tickCount32 = tickCountStart;

			// Perform operation(s) and test(s).
			obj.SetLastReceivedByteTickCount(true);
			EXPECT_EQ(tickCountStart, (DWORD)round(obj.m_lastReceivedBytePerfCount));
		}
// #pragma endregion CSASSerialWrapper::SetLastReceivedByteTickCount

// #pragma region CSASSerialWrapper::PerformPollingCycleDelay
		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::PerformPollingCycleDelay()
		/// Test initialization and delay for first poll to the same address.
		/// </summary>
		TEST_METHOD(PerformPollingCycleDelay_DefaultSameAddressTest)
		{
			const DWORD tickCountInit(1000);
			const byte gameAddress(m_lastGameAddressDefault);

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountInit;
			CSASSerialWrapper obj(os);

			// Perform operation(s) and test(s).
			obj.PerformPollingCycleDelay(gameAddress);
			EXPECT_EQ(tickCountInit + m_pollingSpeedDefault, os.m_tickCount32);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::PerformPollingCycleDelay()
		/// Test initialization and delay for first poll to a differet address.
		/// </summary>
		TEST_METHOD(PerformPollingCycleDelay_DefaultDifferentAddressTest)
		{
			const DWORD tickCountInit(1000);
			const byte gameAddress((byte)(m_lastGameAddressDefault + 11));

			// Setup.
			COperatingSystemTestMock os(m_sasUart);
			os.m_perfCount = os.m_tickCount32 = tickCountInit;
			CSASSerialWrapper obj(os);

			// Perform operation(s) and test(s).
			obj.PerformPollingCycleDelay(gameAddress);
			EXPECT_EQ(tickCountInit + PollingSpeedToDifferentAddress(m_pollingSpeedDefault), os.m_tickCount32);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::PerformPollingCycleDelay()
		/// Test with various polling speeds to the same address.
		/// </summary>
		TEST_METHOD(PerformPollingCycleDelay_PollingSpeedSameAddressTest)
		{
			const DWORD tickCountInit(1000);
			const byte gameAddress(m_lastGameAddressDefault);

			typedef short TestDataType;
			const TestDataType minimumPollingSpeed(0);
			const TestDataType maximumPollingSpeed(255);

			// Setup.  Use the same object for each test.
			COperatingSystemTestMock osA(m_sasUart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountInit;
			CSASSerialWrapper objA(osA);

			for (m_testIndex = (int)minimumPollingSpeed; m_testIndex <= (int)maximumPollingSpeed; ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue((TestDataType)m_testIndex);

				// Perform operation(s) and test(s).
				objA.SetPollingSpeed(testValue);
				objA.PerformPollingCycleDelay(gameAddress);
				AssertResult2(osA.m_tickCount32, tickCountInit + (DWORD)testValue);

				// Setup.  Use a different object for each test sequence.
				COperatingSystemTestMock osB(m_sasUart);
				osB.m_perfCount = osB.m_tickCount32 = tickCountInit;
				CSASSerialWrapper objB(osB);

				// Perform operation(s) and test(s).
				objB.SetPollingSpeed(testValue);
				objB.PerformPollingCycleDelay(gameAddress);
				AssertResult2(osB.m_tickCount32, tickCountInit + (DWORD)testValue);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::PerformPollingCycleDelay()
		/// Test with various polling speeds to a different address.
		/// </summary>
		TEST_METHOD(PerformPollingCycleDelay_PollingSpeedDifferentAddressTest)
		{
			const DWORD tickCountInit(1000);
			const byte gameAddress((byte)(m_lastGameAddressDefault + 11));

			typedef short TestDataType;
			const TestDataType minimumPollingSpeed(0);
			const TestDataType maximumPollingSpeed(255);

			// Setup.  Use the same object for each test.
			COperatingSystemTestMock osA(m_sasUart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountInit;
			CSASSerialWrapper objA(osA);

			for (m_testIndex = (int)minimumPollingSpeed; m_testIndex <= (int)maximumPollingSpeed; ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue((TestDataType)m_testIndex);

				// Perform operation(s) and test(s).
				objA.SetPollingSpeed(testValue);
				objA.PerformPollingCycleDelay(gameAddress);
				AssertResult2(osA.m_tickCount32, tickCountInit + PollingSpeedToDifferentAddress((DWORD)testValue));

				// Setup.  Use a different object for each test sequence.
				COperatingSystemTestMock osB(m_sasUart);
				osB.m_perfCount = osB.m_tickCount32 = tickCountInit;
				CSASSerialWrapper objB(osB);

				// Perform operation(s) and test(s).
				objB.SetPollingSpeed(testValue);
				objB.PerformPollingCycleDelay(gameAddress);
				AssertResult2(osB.m_tickCount32, tickCountInit + PollingSpeedToDifferentAddress((DWORD)testValue));

			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::PerformPollingCycleDelay()
		/// Test with various polling speeds after transmitting to the same address.
		/// </summary>
		TEST_METHOD(PerformPollingCycleDelay_AfterTransmitSameAddressTest)
		{
			const DWORD tickCountInit(1000);
			const DWORD tickCountStart(3000);
			const byte gameAddress(m_lastGameAddressDefault);
			const UINT bufferLen(1);

			typedef short TestDataType;
			const TestDataType minimumPollingSpeed(0);
			const TestDataType maximumPollingSpeed(255);

			// Setup.  Use the same object for each test.
			COperatingSystemTestMock osA(m_sasUart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountInit;
			CSASSerialWrapper objA(osA);
			DWORD tickCountCurrentA(tickCountStart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountCurrentA;

			for (m_testIndex = (int)minimumPollingSpeed; m_testIndex <= (int)maximumPollingSpeed; ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue((TestDataType)m_testIndex);

				// Perform operation(s) and test(s).
				objA.SetPollingSpeed(testValue);
				objA.SetLastTransmittedByteTickCount(gameAddress, bufferLen);
				objA.PerformPollingCycleDelay(gameAddress);
				tickCountCurrentA += (DWORD)testValue + COperatingSystemTest::CalcSasMessageTickCountDuration(bufferLen);

				// Setup.  Use a different object for each test sequence.
				COperatingSystemTestMock osB(m_sasUart);
				osB.m_perfCount = osB.m_tickCount32 = tickCountInit;
				CSASSerialWrapper objB(osB);
				osB.m_perfCount = osB.m_tickCount32 = tickCountStart;

				// Perform operation(s) and test(s).
				objB.SetPollingSpeed(testValue);
				objB.SetLastTransmittedByteTickCount(gameAddress, bufferLen);
				objB.PerformPollingCycleDelay(gameAddress);
				AssertResult2(osB.m_tickCount32, tickCountStart + (DWORD)testValue + COperatingSystemTest::CalcSasMessageTickCountDuration(bufferLen));
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::PerformPollingCycleDelay()
		/// Test with various polling speeds after transmitting to a different address.
		/// </summary>
		TEST_METHOD(PerformPollingCycleDelay_AfterTransmitDifferentAddressTest)
		{
			const DWORD tickCountInit(1000);
			const DWORD tickCountStart(3000);
			byte gameAddressA((byte)(m_lastGameAddressDefault + 1));
			const byte gameAddressB((byte)(m_lastGameAddressDefault + 11));
			const UINT bufferLen(1);

			typedef short TestDataType;
			const TestDataType minimumPollingSpeed(0);
			const TestDataType maximumPollingSpeed(255);

			// Setup.  Use the same object for each test.
			COperatingSystemTestMock osA(m_sasUart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountInit;
			CSASSerialWrapper objA(osA);
			DWORD tickCountCurrentA(tickCountStart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountCurrentA;

			for (m_testIndex = (int)minimumPollingSpeed; m_testIndex <= (int)maximumPollingSpeed; ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue((TestDataType)m_testIndex);

				// Perform operation(s) and test(s).
				objA.SetPollingSpeed(testValue);
				objA.SetLastTransmittedByteTickCount(gameAddressA, bufferLen);
				objA.PerformPollingCycleDelay(++gameAddressA);
				tickCountCurrentA += PollingSpeedToDifferentAddress((DWORD)testValue) +
					COperatingSystemTest::CalcSasMessageTickCountDuration(bufferLen);

				// Setup.  Use a different object for each test sequence.
				COperatingSystemTestMock osB(m_sasUart);
				osB.m_perfCount = osB.m_tickCount32 = tickCountInit;
				CSASSerialWrapper objB(osB);
				osB.m_perfCount = osB.m_tickCount32 = tickCountStart;

				// Perform operation(s) and test(s).
				objB.SetPollingSpeed(testValue);
				objB.SetLastTransmittedByteTickCount(m_lastGameAddressDefault, bufferLen);
				objB.PerformPollingCycleDelay(gameAddressB);
				AssertResult2(osB.m_tickCount32,
					tickCountStart + PollingSpeedToDifferentAddress((DWORD)testValue) +
					COperatingSystemTest::CalcSasMessageTickCountDuration(bufferLen));
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::PerformPollingCycleDelay()
		/// Test with various polling speeds after receiving partial data using the same address.
		/// </summary>
		TEST_METHOD(PerformPollingCycleDelay_AfterReadPartialSameAddressTest)
		{
			const DWORD tickCountInit(1000);
			const DWORD tickCountStart(3000);
			const byte gameAddress(m_lastGameAddressDefault);

			typedef short TestDataType;
			const TestDataType minimumPollingSpeed(0);
			const TestDataType maximumPollingSpeed(255);

			// Setup.  Use the same object for each test.
			COperatingSystemTestMock osA(m_sasUart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountInit;
			CSASSerialWrapper objA(osA);
			DWORD tickCountCurrentA(tickCountStart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountCurrentA;

			for (m_testIndex = (int)minimumPollingSpeed; m_testIndex <= (int)maximumPollingSpeed; ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue((TestDataType)m_testIndex);

				// Perform operation(s) and test(s).
				objA.SetPollingSpeed(testValue);
				objA.SetLastReceivedByteTickCount(false);
				objA.PerformPollingCycleDelay(gameAddress);
#if READ_TIMEOUT_TICK_COUNT
				if (m_readTimeoutTickCount > (DWORD)testValue)
				{
					AssertResult2(osA.m_tickCount32, tickCountStart);
				}
				else
#endif // READ_TIMEOUT_TICK_COUNT.
				{
					tickCountCurrentA += (DWORD)testValue - m_readTimeoutTickCount;
					AssertResult2(osA.m_tickCount32, tickCountCurrentA);
				}

				// Setup.  Use a different object for each test sequence.
				COperatingSystemTestMock osB(m_sasUart);
				osB.m_perfCount = osB.m_tickCount32 = tickCountInit;
				CSASSerialWrapper objB(osB);
				osB.m_perfCount = osB.m_tickCount32 = tickCountStart;

				// Perform operation(s) and test(s).
				objB.SetPollingSpeed(testValue);
				objB.SetLastReceivedByteTickCount(false);
				objB.PerformPollingCycleDelay(gameAddress);
#if READ_TIMEOUT_TICK_COUNT
				if (m_readTimeoutTickCount > (DWORD)testValue)
				{
					AssertResult2(osB.m_tickCount32, tickCountStart);
				}
				else
#endif // READ_TIMEOUT_TICK_COUNT.
				{
					AssertResult2(osB.m_tickCount32, tickCountStart - m_readTimeoutTickCount + (DWORD)testValue);
				}
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::PerformPollingCycleDelay()
		/// Test with various polling speeds after receiving partial data using a different address.
		/// </summary>
		TEST_METHOD(PerformPollingCycleDelay_AfterReadPartialDifferentAddressTest)
		{
			const DWORD tickCountInit(1000);
			const DWORD tickCountStart(3000);
			const byte gameAddress((byte)(m_lastGameAddressDefault + 11));

			typedef short TestDataType;
			const TestDataType minimumPollingSpeed(0);
			const TestDataType maximumPollingSpeed(255);

			// Setup.  Use the same object for each test.
			COperatingSystemTestMock osA(m_sasUart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountInit;
			CSASSerialWrapper objA(osA);
			DWORD tickCountCurrentA(tickCountStart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountCurrentA;

			for (m_testIndex = (int)minimumPollingSpeed; m_testIndex <= (int)maximumPollingSpeed; ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue((TestDataType)m_testIndex);

				// Perform operation(s) and test(s).
				objA.SetPollingSpeed(testValue);
				objA.SetLastReceivedByteTickCount(false);
				objA.PerformPollingCycleDelay(gameAddress);
#if READ_TIMEOUT_TICK_COUNT
				if (m_readTimeoutTickCount > PollingSpeedToDifferentAddress((DWORD)testValue))
				{
					AssertResult2(osA.m_tickCount32, tickCountStart);
				}
				else
#endif // READ_TIMEOUT_TICK_COUNT.
				{
					tickCountCurrentA += PollingSpeedToDifferentAddress((DWORD)testValue) - m_readTimeoutTickCount;
					AssertResult2(osA.m_tickCount32, tickCountCurrentA);
				}

				// Setup.  Use a different object for each test sequence.
				COperatingSystemTestMock osB(m_sasUart);
				osB.m_perfCount = osB.m_tickCount32 = tickCountInit;
				CSASSerialWrapper objB(osB);
				osB.m_perfCount = osB.m_tickCount32 = tickCountStart;

				// Perform operation(s) and test(s).
				objB.SetPollingSpeed(testValue);
				objB.SetLastReceivedByteTickCount(false);
				objB.PerformPollingCycleDelay(gameAddress);
#if READ_TIMEOUT_TICK_COUNT
				if (m_readTimeoutTickCount > PollingSpeedToDifferentAddress((DWORD)testValue))
				{
					AssertResult2(osB.m_tickCount32, tickCountStart);
				}
				else
#endif // READ_TIMEOUT_TICK_COUNT.
				{
					AssertResult2(osB.m_tickCount32, tickCountStart - m_readTimeoutTickCount + PollingSpeedToDifferentAddress((DWORD)testValue));
				}
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::PerformPollingCycleDelay()
		/// Test with various polling speeds after receiving alll data using the same address.
		/// </summary>
		TEST_METHOD(PerformPollingCycleDelay_AfterReadAllSameAddressTest)
		{
			const DWORD tickCountInit(1000);
			const DWORD tickCountStart(3000);
			const byte gameAddress(m_lastGameAddressDefault);

			typedef short TestDataType;
			const TestDataType minimumPollingSpeed(0);
			const TestDataType maximumPollingSpeed(255);

			// Setup.  Use the same object for each test.
			COperatingSystemTestMock osA(m_sasUart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountInit;
			CSASSerialWrapper objA(osA);
			DWORD tickCountCurrentA(tickCountStart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountCurrentA;

			for (m_testIndex = (int)minimumPollingSpeed; m_testIndex <= (int)maximumPollingSpeed; ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue((TestDataType)m_testIndex);

				// Perform operation(s) and test(s).
				objA.SetPollingSpeed(testValue);
				objA.SetLastReceivedByteTickCount(true);
				objA.PerformPollingCycleDelay(gameAddress);
				tickCountCurrentA += (DWORD)testValue;
				AssertResult2(osA.m_tickCount32, tickCountCurrentA);

				// Setup.  Use a different object for each test sequence.
				COperatingSystemTestMock osB(m_sasUart);
				osB.m_perfCount = osB.m_tickCount32 = tickCountInit;
				CSASSerialWrapper objB(osB);
				osB.m_perfCount = osB.m_tickCount32 = tickCountStart;

				// Perform operation(s) and test(s).
				objB.SetPollingSpeed(testValue);
				objB.SetLastReceivedByteTickCount(true);
				objB.PerformPollingCycleDelay(gameAddress);
				AssertResult2(osB.m_tickCount32, tickCountStart + (DWORD)testValue);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::PerformPollingCycleDelay()
		/// Test with various polling speeds after receiving all data using a different address.
		/// </summary>
		TEST_METHOD(PerformPollingCycleDelay_AfterReadAllDifferentAddressTest)
		{
			const DWORD tickCountInit(1000);
			const DWORD tickCountStart(3000);
			const byte gameAddress((byte)(m_lastGameAddressDefault + 11));

			typedef short TestDataType;
			const TestDataType minimumPollingSpeed(0);
			const TestDataType maximumPollingSpeed(255);

			// Setup.  Use the same object for each test.
			COperatingSystemTestMock osA(m_sasUart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountInit;
			CSASSerialWrapper objA(osA);
			DWORD tickCountCurrentA(tickCountStart);
			osA.m_perfCount = osA.m_tickCount32 = tickCountCurrentA;

			for (m_testIndex = (int)minimumPollingSpeed; m_testIndex <= (int)maximumPollingSpeed; ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue((TestDataType)m_testIndex);

				// Perform operation(s) and test(s).
				objA.SetPollingSpeed(testValue);
				objA.SetLastReceivedByteTickCount(true);
				objA.PerformPollingCycleDelay(gameAddress);
				tickCountCurrentA += PollingSpeedToDifferentAddress((DWORD)testValue);
				AssertResult2(osA.m_tickCount32, tickCountCurrentA);

				// Setup.  Use a different object for each test sequence.
				COperatingSystemTestMock osB(m_sasUart);
				osB.m_perfCount = osB.m_tickCount32 = tickCountInit;
				CSASSerialWrapper objB(osB);
				osB.m_perfCount = osB.m_tickCount32 = tickCountStart;

				// Perform operation(s) and test(s).
				objB.SetPollingSpeed(testValue);
				objB.SetLastReceivedByteTickCount(true);
				objB.PerformPollingCycleDelay(gameAddress);
				AssertResult2(osB.m_tickCount32, tickCountStart + PollingSpeedToDifferentAddress((DWORD)testValue));
			}
		}
// #pragma endregion CSASSerialWrapper::PerformPollingCycleDelay

// #pragma region CSASSerialWrapper::CalcSasMessagePerfCountDurationTest
		/// <summary>
		/// Code coverage and functional test for:
		///     CSASSerialWrapper::CalcSasMessagePerfCountDurationTest()
		/// Test with a full range of values.
		/// </summary>
		TEST_METHOD(CalcSasMessagePerfCountDurationTest)
		{
			// For normal use, a minimum length of 1 is expected, but test here with a minimum length of 0 for completeness.
			UINT minimumMessageLength(0);

			for (m_testIndex = (int)minimumMessageLength; m_testIndex <= (int)m_messageLengthMaximum; ++m_testIndex)
			{
				UINT messageLength(m_testIndex);
				double expectedPerfCount(COperatingSystemTest::CalcSasMessagePerfCountDuration(messageLength));
				double actualPerfCount(CSASSerialWrapper::CalcSasMessagePerfCountDuration(messageLength));
				AssertResult2(actualPerfCount, expectedPerfCount);
			}
		}
// #pragma endregion CSASSerialWrapper::CalcSasMessageTickCountDurationTest
	};

	CString SASSerialWrapper::m_gmockFailure;
	SASSerialWrapper::CTersePrinter SASSerialWrapper::m_tersePrinter;
}
#endif //TODO-PORT