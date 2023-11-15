#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveConnectionInfo.h"

// Sentinel includes.
#include "ProcessDump/OperatingSystem.h"

// Unit testing includes.
#include "UnitTestingUtils.h"
#include "gtest/gtest.h"

namespace SentinelNativeUnitTests
{
// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, #result)

// AssertResult macros for calling CUnitTestingUtils from a test method using m_testIndex.
#define AssertResult3m(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2m(result, expected) AssertResult3m(result, expected, #result)

// AssertResult macros for calling CUnitTestingUtils from a helper method.
#define AssertResult3h(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, functionName, lineNumber, extraIndex);
#define AssertResult2h(result, expected) AssertResult3h(result, expected, #result)

// AssertResult macro for calling CUnitTestingUtils (with an extra index) from a test method.
#define AssertResult3x(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__, &extraIndex);
#define AssertResult2x(result, expected) AssertResult3x(result, expected, #result)
	class ProgressiveConnectionInfo : public ::testing::Test
	{
    public:
		// Default expected result(s).
		static const CIPAddress m_lastMessageIPExpected;
		static const SYSTEMTIME m_lastMessageTimestampExpected;
		static const DWORD m_pingIntervalExpected = 15000;
		static const DWORD m_pingIntervalMinimumExpected = 5000;
		static const DWORD m_pingIntervalMaximumExpected = DWORD_MAX;

		static const SYSTEMTIME m_lastConnectAttemptTimestampExpected;
		static const SYSTEMTIME m_lastConnectionTimestampExpected;
		static const ProgressiveConnectionState m_connectionStateExpected = NotStarted;
		static const ProgressiveConnectionError m_lastConnectionErrorExpected = ProgressiveConnectionErrorNone;
		static const string m_connectionStateStringExpected;
		static const int m_connectAttemptCountExpected = 0;

		static const SYSTEMTIME m_lastAddressUpdateTimestampExpected;

		// Class member variable(s).
		int m_testIndex;

		/// <summary>
		/// Test static class members.
		/// </summary>
		void TestStaticMembers()
		{
			// Test static default value(s).
			AssertResult2m(CProgressiveConfig::DEFAULT_PING_INTERVAL, m_pingIntervalExpected);
			AssertResult2m(CProgressiveConfig::MINIMUM_PING_INTERVAL, m_pingIntervalMinimumExpected);
			AssertResult2m(CProgressiveConfig::MAXIMUM_PING_INTERVAL, m_pingIntervalMaximumExpected);
		}

		void AssertEqual(const CProgressiveConnectionInfo &obj,
			const CIPAddress& lastMessageIPExpected,
			const SYSTEMTIME& lastMessageTimestampExpected,
			const SYSTEMTIME& lastConnectAttemptTimestampExpected,
			const SYSTEMTIME& lastConnectionTimestampExpected,
			ProgressiveConnectionState m_connectionStateExpected,
			ProgressiveConnectionError lastConnectionErrorExpected,
			const string& connectionStateStringExpected,
			int connectAttemptCountExpected,
			const SYSTEMTIME& lastAddressUpdateTimestampExpected,
			int testIndex,
			LPCTSTR functionName,
			int lineNumber,
			const int* extraIndex = NULL)
		{
			// Calculate the expected timestamp string(s) at runtime, because the result depends on the time zone.
			// If the time zone changes during a test then the test could fail.
			// Use CTime and CUtilities specific-behaviors to calculate the expected timestamp string(s), because the range handling is undefined.
			string lastMessageTimestampStringExpected("N/A");
			if ((lastMessageTimestampExpected.wYear != 0 || lastMessageTimestampExpected.wMonth != 0) &&
				!CUtilities::Is1970Time(lastMessageTimestampExpected))
			{
				lastMessageTimestampStringExpected = CUnitTestingUtils::FormatLocalMilitaryDateTimeString(lastMessageTimestampExpected);
			}
			string lastConnectionTimestampStringExpected("N/A");
			if ((lastConnectionTimestampExpected.wYear != 0 || lastConnectionTimestampExpected.wMonth != 0) &&
				!CUtilities::Is1970Time(lastConnectionTimestampExpected))
			{
				lastConnectionTimestampStringExpected = CUnitTestingUtils::FormatLocalMilitaryDateTimeString(lastConnectionTimestampExpected);
			}

			AssertResult2h(obj.GetLastMessageIP(), lastMessageIPExpected);
			AssertResult2h(obj.GetLastMessageTimestamp(), lastMessageTimestampExpected);
			AssertResult2h(obj.GetLastMessageTimestampString(), lastMessageTimestampStringExpected);
			AssertResult2h(obj.GetLastConnectAttemptTimestamp(), lastConnectAttemptTimestampExpected);
			AssertResult2h(obj.GetLastConnectTimestamp(), lastConnectionTimestampExpected);
			AssertResult2h(obj.GetLastConnectTimestampString(), lastConnectionTimestampStringExpected);
			AssertResult2h(obj.GetState(), m_connectionStateExpected);
			AssertResult2h(obj.GetLastError(), lastConnectionErrorExpected);
			AssertResult2h(obj.GetConnectionStateString(), connectionStateStringExpected);
			AssertResult2h(obj.GetConnectAttemptCount(), connectAttemptCountExpected);
			AssertResult2h(obj.GetLastAddressUpdateTimestamp(), lastAddressUpdateTimestampExpected);
		}

		/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		void f_Initialize( void )
		{
			// Test method initialization code.
			m_testIndex = 0;

			// Perform static tests here so each test method gets a useful error message.
			TestStaticMembers();
		}

		/// <summary>
		/// Cleanup class member(s) after each test method is run.
		/// </summary>
		void f_Cleanup(void)
		{
			// Test method cleanup code.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConnectionInfo::CProgressiveConnectionInfo()
		///     CProgressiveConnectionInfo::GetConnectionStateString()
		/// </summary>
		void f_CProgressiveConnectionInfoTest(void)
		{
			// Setup.
			const int testIndex(0);

			// Perform operation(s) to be tested.
			const CProgressiveConnectionInfo obj;

			// Test(s).
			AssertEqual(obj,
				m_lastMessageIPExpected,
				m_lastMessageTimestampExpected,
				m_lastConnectAttemptTimestampExpected,
				m_lastConnectionTimestampExpected,
				m_connectionStateExpected,
				m_lastConnectionErrorExpected,
				m_connectionStateStringExpected,
				m_connectAttemptCountExpected,
				m_lastAddressUpdateTimestampExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConnectionInfo::CProgressiveConnectionInfo()
		///     CProgressiveConnectionInfo::GetConnectionStateString()
		/// </summary>
		void f_GetConnectionStateStringDefaultTest(void)
		{
			// Expected result(s).
			const string connectionStateStringExpected("Not Started");

			// Setup.
			const int testIndex(0);

			// Perform operation(s) to be tested.
			const CProgressiveConnectionInfo obj;

			// Test(s).
			AssertResult2(obj.GetConnectionStateString(), connectionStateStringExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConnectionInfo::SetState()
		///     CProgressiveConnectionInfo::SetLastError()
		///     CProgressiveConnectionInfo::GetConnectionStateString()
		/// Test by setting the state first and then the error code.
		/// </summary>
		void f_GetConnectionStateStringTest(void)
		{
			typedef struct
			{
				// Input(s).
				const ProgressiveConnectionState m_state;
				const ProgressiveConnectionError m_error;

				// Expected result(s).
				const string m_connectionStateStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (ProgressiveConnectionState)-1, CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)(NotStarted - 1), CouldNotFindServer, "N/A" },
				{ NotStarted, CouldNotFindServer, "Not Started" },
				{ Disabled, CouldNotFindServer, "Disabled" },
				{ WaitingForInfo, CouldNotFindServer, "Waiting For Info" },
				{ InDiscovery, CouldNotFindServer, "In Discovery" },
				{ Connected, CouldNotFindServer, "Connected" },
				{ Disconnected, CouldNotFindServer, "Disconnected" },

				{ Error, (ProgressiveConnectionError)-1, "Error" },
				{ Error, (ProgressiveConnectionError)(ProgressiveConnectionErrorNone - 1), "Error" },
				{ Error, ProgressiveConnectionErrorNone, "Error" },
				{ Error, DiscoveryConnectionError, "Error: Discovery Connection" },
				{ Error, MessageTransmissionError, "Error: Message Transmission" },
				{ Error, ConnectFailed, "Error: Connect Failed" },
				{ Error, DisconnectFailed, "Error: Disconnect Failed" },
				{ Error, (ProgressiveConnectionError)(DisconnectFailed + 1), "Error" },
				{ Error, (ProgressiveConnectionError)INT_MAX, "Error" },
				{ Error, (ProgressiveConnectionError)DWORD_MAX, "Error" },

				{ (ProgressiveConnectionState)(Error + 1), CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)INT_MAX, CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)INT_MIN, CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)DWORD_MAX, CouldNotFindServer, "N/A" },
			};

			// Setup. Perform each test with the same instance.
			CProgressiveConnectionInfo objA;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new object.
				CProgressiveConnectionInfo objB;

				// Perform operation(s) to be tested.
				objA.SetState(TEST_DATA[testIndex].m_state);
				objA.SetLastError(TEST_DATA[testIndex].m_error);
				objB.SetState(TEST_DATA[testIndex].m_state);
				objB.SetLastError(TEST_DATA[testIndex].m_error);

				// Test(s).
				AssertResult2(objA.GetConnectionStateString(), TEST_DATA[testIndex].m_connectionStateStringExpected);
				AssertResult2(objB.GetConnectionStateString(), TEST_DATA[testIndex].m_connectionStateStringExpected);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConnectionInfo::SetState()
		///     CProgressiveConnectionInfo::SetLastError()
		///     CProgressiveConnectionInfo::GetConnectionStateString()
		/// Test by setting the error code first and then the state.
		/// </summary>
		void f_GetConnectionStateString2Test(void)
		{
			typedef struct
			{
				// Input(s).
				const ProgressiveConnectionState m_state;
				const ProgressiveConnectionError m_error;

				// Expected result(s).
				const string m_connectionStateStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (ProgressiveConnectionState)-1, CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)(NotStarted - 1), CouldNotFindServer, "N/A" },
				{ NotStarted, CouldNotFindServer, "Not Started" },
				{ Disabled, CouldNotFindServer, "Disabled" },
				{ WaitingForInfo, CouldNotFindServer, "Waiting For Info" },
				{ InDiscovery, CouldNotFindServer, "In Discovery" },
				{ Connected, CouldNotFindServer, "Connected" },
				{ Disconnected, CouldNotFindServer, "Disconnected" },

				{ Error, (ProgressiveConnectionError)-1, "Error" },
				{ Error, (ProgressiveConnectionError)(ProgressiveConnectionErrorNone - 1), "Error" },
				{ Error, ProgressiveConnectionErrorNone, "Error" },
				{ Error, DiscoveryConnectionError, "Error: Discovery Connection" },
				{ Error, MessageTransmissionError, "Error: Message Transmission" },
				{ Error, ConnectFailed, "Error: Connect Failed" },
				{ Error, DisconnectFailed, "Error: Disconnect Failed" },
				{ Error, (ProgressiveConnectionError)(DisconnectFailed + 1), "Error" },
				{ Error, (ProgressiveConnectionError)INT_MAX, "Error" },
				{ Error, (ProgressiveConnectionError)DWORD_MAX, "Error" },

				{ (ProgressiveConnectionState)(Error + 1), CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)INT_MAX, CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)INT_MIN, CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)DWORD_MAX, CouldNotFindServer, "N/A" },
			};

			// Setup. Perform each test with the same instance.
			CProgressiveConnectionInfo objA;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new object.
				CProgressiveConnectionInfo objB;

				// Perform operation(s) to be tested.
				objA.SetLastError(TEST_DATA[testIndex].m_error);
				objA.SetState(TEST_DATA[testIndex].m_state);
				objB.SetLastError(TEST_DATA[testIndex].m_error);
				objB.SetState(TEST_DATA[testIndex].m_state);

				// Test(s).
				AssertResult2(objA.GetConnectionStateString(), TEST_DATA[testIndex].m_connectionStateStringExpected);
				AssertResult2(objB.GetConnectionStateString(), TEST_DATA[testIndex].m_connectionStateStringExpected);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConnectionInfo::SetState()
		///     CProgressiveConnectionInfo::SetLastError()
		///     CProgressiveConnectionInfo::GetConnectionStateString(bool)
		/// Test by setting the state first and then the error code.
		/// </summary>
		void f_GetConnectionStateStringShortFalseTest(void)
		{
			typedef struct
			{
				// Input(s).
				const ProgressiveConnectionState m_state;
				const ProgressiveConnectionError m_error;

				// Expected result(s).
				const string m_connectionStateStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (ProgressiveConnectionState)-1, CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)(NotStarted - 1), CouldNotFindServer, "N/A" },
				{ NotStarted, CouldNotFindServer, "Not Started" },
				{ Disabled, CouldNotFindServer, "Disabled" },
				{ WaitingForInfo, CouldNotFindServer, "Waiting For Info" },
				{ InDiscovery, CouldNotFindServer, "In Discovery" },
				{ Connected, CouldNotFindServer, "Connected" },
				{ Disconnected, CouldNotFindServer, "Disconnected" },

				{ Error, (ProgressiveConnectionError)-1, "Error" },
				{ Error, (ProgressiveConnectionError)(ProgressiveConnectionErrorNone - 1), "Error" },
				{ Error, ProgressiveConnectionErrorNone, "Error" },
				{ Error, DiscoveryConnectionError, "Error: Discovery Connection" },
				{ Error, MessageTransmissionError, "Error: Message Transmission" },
				{ Error, ConnectFailed, "Error: Connect Failed" },
				{ Error, DisconnectFailed, "Error: Disconnect Failed" },
				{ Error, (ProgressiveConnectionError)(DisconnectFailed + 1), "Error" },
				{ Error, (ProgressiveConnectionError)INT_MAX, "Error" },
				{ Error, (ProgressiveConnectionError)DWORD_MAX, "Error" },

				{ (ProgressiveConnectionState)(Error + 1), CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)INT_MAX, CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)INT_MIN, CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)DWORD_MAX, CouldNotFindServer, "N/A" },
			};

			// Setup. Perform each test with the same instance.
			CProgressiveConnectionInfo objA;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new object.
				CProgressiveConnectionInfo objB;

				// Perform operation(s) to be tested.
				objA.SetState(TEST_DATA[testIndex].m_state);
				objA.SetLastError(TEST_DATA[testIndex].m_error);
				objB.SetState(TEST_DATA[testIndex].m_state);
				objB.SetLastError(TEST_DATA[testIndex].m_error);

				// Test(s).
				AssertResult2(objA.GetConnectionStateString(false), TEST_DATA[testIndex].m_connectionStateStringExpected);
				AssertResult2(objB.GetConnectionStateString(false), TEST_DATA[testIndex].m_connectionStateStringExpected);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConnectionInfo::SetState()
		///     CProgressiveConnectionInfo::SetLastError()
		///     CProgressiveConnectionInfo::GetConnectionStateString(bool)
		/// Test by setting the state first and then the error code.
		/// </summary>
		void f_GetConnectionStateStringShortTrueTest(void)
		{
			typedef struct
			{
				// Input(s).
				const ProgressiveConnectionState m_state;
				const ProgressiveConnectionError m_error;

				// Expected result(s).
				const string m_connectionStateStringExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (ProgressiveConnectionState)-1, CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)(NotStarted - 1), CouldNotFindServer, "N/A" },
				{ NotStarted, CouldNotFindServer, "Not Started" },
				{ Disabled, CouldNotFindServer, "Disabled" },
				{ WaitingForInfo, CouldNotFindServer, "Waiting For Info" },
				{ InDiscovery, CouldNotFindServer, "In Discovery" },
				{ Connected, CouldNotFindServer, "Connected" },
				{ Disconnected, CouldNotFindServer, "Disconnected" },

				{ Error, (ProgressiveConnectionError)-1, "Error" },
				{ Error, (ProgressiveConnectionError)(ProgressiveConnectionErrorNone - 1), "Error" },
				{ Error, ProgressiveConnectionErrorNone, "Error" },
				{ Error, DiscoveryConnectionError, "Error: Discovery Conn." },
				{ Error, MessageTransmissionError, "Error: Message Trans." },
				{ Error, ConnectFailed, "Error: Connect Failed" },
				{ Error, DisconnectFailed, "Error: Disconnect Fail" },
				{ Error, (ProgressiveConnectionError)(DisconnectFailed + 1), "Error" },
				{ Error, (ProgressiveConnectionError)INT_MAX, "Error" },
				{ Error, (ProgressiveConnectionError)DWORD_MAX, "Error" },

				{ (ProgressiveConnectionState)(Error + 1), CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)INT_MAX, CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)INT_MIN, CouldNotFindServer, "N/A" },
				{ (ProgressiveConnectionState)DWORD_MAX, CouldNotFindServer, "N/A" },
			};

			// Setup. Perform each test with the same instance.
			CProgressiveConnectionInfo objA;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new object.
				CProgressiveConnectionInfo objB;

				// Perform operation(s) to be tested.
				objA.SetState(TEST_DATA[testIndex].m_state);
				objA.SetLastError(TEST_DATA[testIndex].m_error);
				objB.SetState(TEST_DATA[testIndex].m_state);
				objB.SetLastError(TEST_DATA[testIndex].m_error);

				// Test(s).
				AssertResult2(objA.GetConnectionStateString(true), TEST_DATA[testIndex].m_connectionStateStringExpected);
				AssertResult2(objB.GetConnectionStateString(true), TEST_DATA[testIndex].m_connectionStateStringExpected);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///		 CProgressiveConnectionInfo::SetLastMessageIP()
		///		 CProgressiveConnectionInfo::SetLastMessageTimestamp()
		///		 CProgressiveConnectionInfo::SetLastConnectAttemptTimestamp()
		///		 CProgressiveConnectionInfo::SetLastConnectTimestamp()
		///		 CProgressiveConnectionInfo::SetState()
		///		 CProgressiveConnectionInfo::SetLastError()
		///		 CProgressiveConnectionInfo::SetConnectAttemptCount()
		///		 CProgressiveConnectionInfo::SetLastAddressUpdateTimestamp()
		///		 CProgressiveConnectionInfo::GetLastMessageIP()
		///		 CProgressiveConnectionInfo::GetLastMessageTimestamp()
		///		 CProgressiveConnectionInfo::GetLastMessageTimestampString()
		///		 CProgressiveConnectionInfo::GetLastConnectAttemptTimestamp()
		///		 CProgressiveConnectionInfo::GetLastConnectTimestamp()
		///		 CProgressiveConnectionInfo::GetLastConnectTimestampString()
		///		 CProgressiveConnectionInfo::GetState()
		///		 CProgressiveConnectionInfo::GetLastError()
		///		 CProgressiveConnectionInfo::GetConnectionStateString()
		///		 CProgressiveConnectionInfo::GetConnectAttemptCount()
		///		 CProgressiveConnectionInfo::GetLastAddressUpdateTimestamp()
		/// </summary>
		void f_SetGetTest(void)
		{
			typedef struct {
				LPCTSTR const m_ipAddressString;
				int m_port;
			} TIPAddressStringPort;

			typedef struct
			{
				// Input(s).
				const TIPAddressStringPort m_lastMessageIP;
				const SYSTEMTIME m_lastMessageTimestamp;
				const SYSTEMTIME m_lastConnectAttemptTimestamp;
				const SYSTEMTIME m_lastConnectionTimestamp;
				const ProgressiveConnectionState m_state;
				const ProgressiveConnectionError m_error;
				const int m_connectAttemptCount;
				const SYSTEMTIME m_lastAddressUpdateTimestamp;

				// Expected result(s).
				const TIPAddressStringPort m_lastMessageIPExpected;
				const SYSTEMTIME m_lastMessageTimestampExpected;
				const SYSTEMTIME m_lastConnectAttemptTimestampExpected;
				const SYSTEMTIME m_lastConnectionTimestampExpected;
				const ProgressiveConnectionState m_connectionStateExpected;
				const ProgressiveConnectionError m_lastConnectionErrorExpected;
				const string m_connectionStateStringExpected;
				const int m_connectAttemptCountExpected;
				const SYSTEMTIME m_lastAddressUpdateTimestampExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ { "", -1 }, { 2011, 2, 0, 3, 14, 25, 0, 789 }, { 2011, 1, 2, 3, 4, 5, 0, 999 }, { 2012, 1, 2, 3, 4, 5, 0, 000 }, (ProgressiveConnectionState)-1, CouldNotFindServer, 0, { 2013, 1, 2, 3, 4, 55, 0, 555 },
				{ "", -1 }, { 2011, 2, 0, 3, 14, 25, 0, 789 }, { 2011, 1, 2, 3, 4, 5, 0, 999 }, { 2012, 1, 2, 3, 4, 5, 0, 000 }, (ProgressiveConnectionState)-1, CouldNotFindServer, "N/A", 0, { 2013, 1, 2, 3, 4, 55, 0, 555 }
				},
				{ { "*", -999999 }, { 2011, 2, 0, 3, 14, 25, 1, 789 }, { 2011, 1, 2, 3, 4, 5, 1, 999 }, { 2012, 1, 2, 3, 4, 5, 1, 000 }, (ProgressiveConnectionState)(NotStarted - 1), CouldNotFindServer, 1, { 2013, 1, 2, 3, 4, 55, 1, 555 },
				{ "*", -999999 }, { 2011, 2, 0, 3, 14, 25, 1, 789 }, { 2011, 1, 2, 3, 4, 5, 1, 999 }, { 2012, 1, 2, 3, 4, 5, 1, 000 }, (ProgressiveConnectionState)(NotStarted - 1), CouldNotFindServer, "N/A", 1, { 2013, 1, 2, 3, 4, 55, 1, 555 }
				},
				{ { "n/a", -1234567890 }, { 2011, 2, 0, 3, 14, 25, 2, 789 }, { 2011, 1, 2, 3, 4, 5, 2, 999 }, { 2012, 1, 2, 3, 4, 5, 2, 000 }, NotStarted, CouldNotFindServer, 2, { 2013, 1, 2, 3, 4, 55, 2, 555 },
				{ "n/a", -1234567890 }, { 2011, 2, 0, 3, 14, 25, 2, 789 }, { 2011, 1, 2, 3, 4, 5, 2, 999 }, { 2012, 1, 2, 3, 4, 5, 2, 000 }, NotStarted, CouldNotFindServer, "Not Started", 2, { 2013, 1, 2, 3, 4, 55, 2, 555 }
				},
				{ { " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 ", INT_MIN }, { 2011, 2, 0, 3, 14, 25, 3, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 3, 000 }, Disabled, CouldNotFindServer, 1234567890, { 2013, 1, 2, 3, 4, 55, 3, 555 },
				{ " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 ", INT_MIN }, { 2011, 2, 0, 3, 14, 25, 3, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 3, 000 }, Disabled, CouldNotFindServer, "Disabled", 1234567890, { 2013, 1, 2, 3, 4, 55, 3, 555 }
				},
				{ { "abcdefghijklmnopqrstuvwxyz", INT_MAX }, { 2011, 2, 0, 3, 14, 25, 4, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 4, 000 }, WaitingForInfo, CouldNotFindServer, INT_MAX - 1, { 2013, 1, 2, 3, 4, 55, 4, 555 },
				{ "abcdefghijklmnopqrstuvwxyz", INT_MAX }, { 2011, 2, 0, 3, 14, 25, 4, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 4, 000 }, WaitingForInfo, CouldNotFindServer, "Waiting For Info", INT_MAX - 1, { 2013, 1, 2, 3, 4, 55, 4, 555 }
				},
				{ { "0.0.0.0", INT_MAX - 1 }, { 2011, 2, 0, 3, 14, 25, 5, 789 }, { 2011, 1, 2, 3, 4, 5, 5, 999 }, { 2012, 1, 2, 3, 4, 5, 5, 000 }, InDiscovery, CouldNotFindServer, INT_MAX, { 2013, 1, 2, 3, 4, 55, 5, 555 },
				{ "0.0.0.0", INT_MAX - 1 }, { 2011, 2, 0, 3, 14, 25, 5, 789 }, { 2011, 1, 2, 3, 4, 5, 5, 999 }, { 2012, 1, 2, 3, 4, 5, 5, 000 }, InDiscovery, CouldNotFindServer, "In Discovery", INT_MAX, { 2013, 1, 2, 3, 4, 55, 5, 555 }
				},
				{ { "255.255.255.255", INT_MIN + 1 }, { 2011, 2, 0, 3, 14, 25, 6, 789 }, { 2011, 1, 2, 3, 4, 5, 6, 999 }, { 2012, 1, 2, 3, 4, 5, 6, 000 }, Connected, CouldNotFindServer, INT_MIN, { 2013, 1, 2, 3, 4, 55, 6, 555 },
				{ "255.255.255.255", INT_MIN + 1 }, { 2011, 2, 0, 3, 14, 25, 6, 789 }, { 2011, 1, 2, 3, 4, 5, 6, 999 }, { 2012, 1, 2, 3, 4, 5, 6, 000 }, Connected, CouldNotFindServer,"Connected", INT_MIN, { 2013, 1, 2, 3, 4, 55, 6, 555 }
				},
				{ { "1.2.3.107", 20007 }, { 2011, 2, 0, 3, 14, 25, 7, 789 }, { 2011, 1, 2, 3, 4, 5, 7, 999 }, { 2012, 1, 2, 3, 4, 5, 7, 000 }, Disconnected, CouldNotFindServer, INT_MIN + 1, { 2013, 1, 2, 3, 4, 55, 7, 555 },
				{ "1.2.3.107", 20007 }, { 2011, 2, 0, 3, 14, 25, 7, 789 }, { 2011, 1, 2, 3, 4, 5, 7, 999 }, { 2012, 1, 2, 3, 4, 5, 7, 000 }, Disconnected, CouldNotFindServer, "Disconnected", INT_MIN + 1, { 2013, 1, 2, 3, 4, 55, 7, 555 }
				},
				{ { "1.2.3.111", 20011 }, { 2011, 2, 0, 3, 14, 25, 11, 789 }, { 2011, 1, 2, 3, 4, 5, 11, 999 }, { 2012, 1, 2, 3, 4, 5, 11, 000 }, Error, DiscoveryConnectionError, 0x12345678, { 2013, 1, 2, 3, 4, 55, 11, 555 },
				{ "1.2.3.111", 20011 }, { 2011, 2, 0, 3, 14, 25, 11, 789 }, { 2011, 1, 2, 3, 4, 5, 11, 999 }, { 2012, 1, 2, 3, 4, 5, 11, 000 }, Error, DiscoveryConnectionError, "Error: Discovery Connection", 0x12345678, { 2013, 1, 2, 3, 4, 55, 11, 555 }
				},
				{ { "1.2.3.112", 20012 }, { 2011, 2, 0, 3, 14, 25, 12, 789 }, { 2011, 1, 2, 3, 4, 5, 12, 999 }, { 2012, 1, 2, 3, 4, 5, 12, 000 }, Error, MessageTransmissionError, -0x12345678, { 2013, 1, 2, 3, 4, 55, 12, 555 },
				{ "1.2.3.112", 20012 }, { 2011, 2, 0, 3, 14, 25, 12, 789 }, { 2011, 1, 2, 3, 4, 5, 12, 999 }, { 2012, 1, 2, 3, 4, 5, 12, 000 }, Error, MessageTransmissionError, "Error: Message Transmission", -0x12345678, { 2013, 1, 2, 3, 4, 55, 12, 555 }
				},
				{ { "1.2.3.113", 20013 }, { 2011, 2, 0, 3, 14, 25, 13, 789 }, { 2011, 1, 2, 3, 4, 5, 13, 999 }, { 2012, 1, 2, 3, 4, 5, 13, 000 }, Error, ConnectFailed, 1313, { 2013, 1, 2, 3, 4, 55, 13, 555 },
				{ "1.2.3.113", 20013 }, { 2011, 2, 0, 3, 14, 25, 13, 789 }, { 2011, 1, 2, 3, 4, 5, 13, 999 }, { 2012, 1, 2, 3, 4, 5, 13, 000 }, Error, ConnectFailed, "Error: Connect Failed", 1313, { 2013, 1, 2, 3, 4, 55, 13, 555 }
				}
			};

			// Setup. Perform each test with the same instance.
			CProgressiveConnectionInfo varProgInfoA;

			for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new object.
				CProgressiveConnectionInfo varProgInfoB;

				// Setup. Calculate lastMessageIPExpected.
				CIPAddress lastMessageIPExpected;
				lastMessageIPExpected.SetIP(TEST_DATA[testIndex].m_lastMessageIPExpected.m_ipAddressString);
				lastMessageIPExpected.SetPort(TEST_DATA[testIndex].m_lastMessageIPExpected.m_port);

				// Perform operation(s) to be tested.
				CIPAddress ipAddress;
				ipAddress.SetIP(TEST_DATA[testIndex].m_lastMessageIP.m_ipAddressString);
				ipAddress.SetPort(TEST_DATA[testIndex].m_lastMessageIP.m_port);
				varProgInfoA.SetLastMessageIP(ipAddress);
				varProgInfoA.SetLastMessageTimestamp(TEST_DATA[testIndex].m_lastMessageTimestamp);
				varProgInfoA.SetLastConnectAttemptTimestamp(TEST_DATA[testIndex].m_lastConnectAttemptTimestampExpected);
				varProgInfoA.SetLastConnectTimestamp(TEST_DATA[testIndex].m_lastConnectionTimestamp);
				varProgInfoA.SetState(TEST_DATA[testIndex].m_state);
				varProgInfoA.SetLastError(TEST_DATA[testIndex].m_error);
				varProgInfoA.SetConnectAttemptCount(TEST_DATA[testIndex].m_connectAttemptCount);
				varProgInfoA.SetLastAddressUpdateTimestamp(TEST_DATA[testIndex].m_lastAddressUpdateTimestamp);
				const CProgressiveConnectionInfo& progInfoA(varProgInfoA);

				// Perfrom operations(s) to be tested in the opposite order.
				varProgInfoB.SetLastAddressUpdateTimestamp(TEST_DATA[testIndex].m_lastAddressUpdateTimestamp);
				varProgInfoB.SetConnectAttemptCount(TEST_DATA[testIndex].m_connectAttemptCount);
				varProgInfoB.SetLastError(TEST_DATA[testIndex].m_error);
				varProgInfoB.SetState(TEST_DATA[testIndex].m_state);
				varProgInfoB.SetLastConnectTimestamp(TEST_DATA[testIndex].m_lastConnectionTimestamp);
				varProgInfoB.SetLastConnectAttemptTimestamp(TEST_DATA[testIndex].m_lastConnectAttemptTimestampExpected);
				varProgInfoB.SetLastMessageTimestamp(TEST_DATA[testIndex].m_lastMessageTimestamp);
				varProgInfoB.SetLastMessageIP(ipAddress);
				const CProgressiveConnectionInfo& progInfoB(varProgInfoB);

				// Test(s).
				AssertEqual(progInfoA,
					lastMessageIPExpected,
					TEST_DATA[testIndex].m_lastMessageTimestampExpected,
					TEST_DATA[testIndex].m_lastConnectAttemptTimestampExpected,
					TEST_DATA[testIndex].m_lastConnectionTimestampExpected,
					TEST_DATA[testIndex].m_connectionStateExpected,
					TEST_DATA[testIndex].m_lastConnectionErrorExpected,
					TEST_DATA[testIndex].m_connectionStateStringExpected,
					TEST_DATA[testIndex].m_connectAttemptCountExpected,
					TEST_DATA[testIndex].m_lastAddressUpdateTimestampExpected,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(progInfoB,
					lastMessageIPExpected,
					TEST_DATA[testIndex].m_lastMessageTimestampExpected,
					TEST_DATA[testIndex].m_lastConnectAttemptTimestampExpected,
					TEST_DATA[testIndex].m_lastConnectionTimestampExpected,
					TEST_DATA[testIndex].m_connectionStateExpected,
					TEST_DATA[testIndex].m_lastConnectionErrorExpected,
					TEST_DATA[testIndex].m_connectionStateStringExpected,
					TEST_DATA[testIndex].m_connectAttemptCountExpected,
					TEST_DATA[testIndex].m_lastAddressUpdateTimestampExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///		 CProgressiveConnectionInfo::CProgressiveConnectionInfo()
		///		 CProgressiveConnectionInfo::CProgressiveConnectionInfo(const CProgressiveConnectionInfo &rhs)
		/// Use a default CProgressiveConnectionInfo instance.
		/// </summary>
		void f_CopyConstructor_DefaultTest(void)
		{
			// Setup.
			const int testIndex(0);
			const CProgressiveConnectionInfo progInfo;

			// Verify setup.
			AssertEqual(progInfo,
				m_lastMessageIPExpected,
				m_lastMessageTimestampExpected,
				m_lastConnectAttemptTimestampExpected,
				m_lastConnectionTimestampExpected,
				m_connectionStateExpected,
				m_lastConnectionErrorExpected,
				m_connectionStateStringExpected,
				m_connectAttemptCountExpected,
				m_lastAddressUpdateTimestampExpected,
				testIndex, __TFUNCTION__, __LINE__);

			// Perform operation(s) to be tested.
			const CProgressiveConnectionInfo progInfoA(progInfo);

			// Test(s).
			AssertEqual(progInfoA,
				m_lastMessageIPExpected,
				m_lastMessageTimestampExpected,
				m_lastConnectAttemptTimestampExpected,
				m_lastConnectionTimestampExpected,
				m_connectionStateExpected,
				m_lastConnectionErrorExpected,
				m_connectionStateStringExpected,
				m_connectAttemptCountExpected,
				m_lastAddressUpdateTimestampExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///		 CProgressiveConnectionInfo::SetLastMessageIP()
		///		 CProgressiveConnectionInfo::SetLastMessageTimestamp()
		///		 CProgressiveConnectionInfo::SetLastConnectAttemptTimestamp()
		///		 CProgressiveConnectionInfo::SetLastConnectTimestamp()
		///		 CProgressiveConnectionInfo::SetState()
		///		 CProgressiveConnectionInfo::SetLastError()
		///		 CProgressiveConnectionInfo::SetConnectAttemptCount()
		///		 CProgressiveConnectionInfo::SetLastAddressUpdateTimestamp()
		///		 CProgressiveConnectionInfo::GetLastMessageIP()
		///		 CProgressiveConnectionInfo::GetLastMessageTimestamp()
		///		 CProgressiveConnectionInfo::GetLastMessageTimestampString()
		///		 CProgressiveConnectionInfo::GetLastConnectAttemptTimestamp()
		///		 CProgressiveConnectionInfo::GetLastConnectTimestamp()
		///		 CProgressiveConnectionInfo::GetLastConnectTimestampString()
		///		 CProgressiveConnectionInfo::GetState()
		///		 CProgressiveConnectionInfo::GetLastError()
		///		 CProgressiveConnectionInfo::GetConnectionStateString()
		///		 CProgressiveConnectionInfo::GetConnectAttemptCount()
		///		 CProgressiveConnectionInfo::GetLastAddressUpdateTimestamp()
		///		 CProgressiveConnectionInfo::CProgressiveConnectionInfo(const CProgressiveConnectionInfo &rhs)
		/// </summary>
		void f_CopyConstructor_SetGetTest(void)
		{
			typedef struct {
				LPCTSTR const m_ipAddressString;
				int m_port;
			} TIPAddressStringPort;

			typedef struct
			{
				// Input(s).
				const TIPAddressStringPort m_lastMessageIP;
				const SYSTEMTIME m_lastMessageTimestamp;
				const SYSTEMTIME m_lastConnectAttemptTimestamp;
				const SYSTEMTIME m_lastConnectionTimestamp;
				const ProgressiveConnectionState m_state;
				const ProgressiveConnectionError m_error;
				const int m_connectAttemptCount;
				const SYSTEMTIME m_lastAddressUpdateTimestamp;

				// Expected result(s).
				const TIPAddressStringPort m_lastMessageIPExpected;
				const SYSTEMTIME m_lastMessageTimestampExpected;
				const SYSTEMTIME m_lastConnectAttemptTimestampExpected;
				const SYSTEMTIME m_lastConnectionTimestampExpected;
				const ProgressiveConnectionState m_connectionStateExpected;
				const ProgressiveConnectionError m_lastConnectionErrorExpected;
				const string m_connectionStateStringExpected;
				const int m_connectAttemptCountExpected;
				const SYSTEMTIME m_lastAddressUpdateTimestampExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ { "", -1 }, { 2011, 2, 0, 3, 14, 25, 0, 789 }, { 2011, 1, 2, 3, 4, 5, 0, 999 }, { 2012, 1, 2, 3, 4, 5, 0, 000 }, (ProgressiveConnectionState)-1, CouldNotFindServer, 0, { 2013, 1, 2, 3, 4, 55, 0, 555 },
				{ "", -1 }, { 2011, 2, 0, 3, 14, 25, 0, 789 }, { 2011, 1, 2, 3, 4, 5, 0, 999 }, { 2012, 1, 2, 3, 4, 5, 0, 000 }, (ProgressiveConnectionState)-1, CouldNotFindServer, "N/A", 0, { 2013, 1, 2, 3, 4, 55, 0, 555 }
				},
				{ { "*", -999999 }, { 2011, 2, 0, 3, 14, 25, 1, 789 }, { 2011, 1, 2, 3, 4, 5, 1, 999 }, { 2012, 1, 2, 3, 4, 5, 1, 000 }, (ProgressiveConnectionState)(NotStarted - 1), CouldNotFindServer, 1, { 2013, 1, 2, 3, 4, 55, 1, 555 },
				{ "*", -999999 }, { 2011, 2, 0, 3, 14, 25, 1, 789 }, { 2011, 1, 2, 3, 4, 5, 1, 999 }, { 2012, 1, 2, 3, 4, 5, 1, 000 }, (ProgressiveConnectionState)(NotStarted - 1), CouldNotFindServer, "N/A", 1, { 2013, 1, 2, 3, 4, 55, 1, 555 }
				},
				{ { "n/a", -1234567890 }, { 2011, 2, 0, 3, 14, 25, 2, 789 }, { 2011, 1, 2, 3, 4, 5, 2, 999 }, { 2012, 1, 2, 3, 4, 5, 2, 000 }, NotStarted, CouldNotFindServer, 2, { 2013, 1, 2, 3, 4, 55, 2, 555 },
				{ "n/a", -1234567890 }, { 2011, 2, 0, 3, 14, 25, 2, 789 }, { 2011, 1, 2, 3, 4, 5, 2, 999 }, { 2012, 1, 2, 3, 4, 5, 2, 000 }, NotStarted, CouldNotFindServer, "Not Started", 2, { 2013, 1, 2, 3, 4, 55, 2, 555 }
				},
				{ { " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 ", INT_MIN }, { 2011, 2, 0, 3, 14, 25, 3, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 3, 000 }, Disabled, CouldNotFindServer, 1234567890, { 2013, 1, 2, 3, 4, 55, 3, 555 },
				{ " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 ", INT_MIN }, { 2011, 2, 0, 3, 14, 25, 3, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 3, 000 }, Disabled, CouldNotFindServer, "Disabled", 1234567890, { 2013, 1, 2, 3, 4, 55, 3, 555 }
				},
				{ { "abcdefghijklmnopqrstuvwxyz", INT_MAX }, { 2011, 2, 0, 3, 14, 25, 4, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 4, 000 }, WaitingForInfo, CouldNotFindServer, INT_MAX - 1, { 2013, 1, 2, 3, 4, 55, 4, 555 },
				{ "abcdefghijklmnopqrstuvwxyz", INT_MAX }, { 2011, 2, 0, 3, 14, 25, 4, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 4, 000 }, WaitingForInfo, CouldNotFindServer, "Waiting For Info", INT_MAX - 1, { 2013, 1, 2, 3, 4, 55, 4, 555 }
				},
				{ { "0.0.0.0", INT_MAX - 1 }, { 2011, 2, 0, 3, 14, 25, 5, 789 }, { 2011, 1, 2, 3, 4, 5, 5, 999 }, { 2012, 1, 2, 3, 4, 5, 5, 000 }, InDiscovery, CouldNotFindServer, INT_MAX, { 2013, 1, 2, 3, 4, 55, 5, 555 },
				{ "0.0.0.0", INT_MAX - 1 }, { 2011, 2, 0, 3, 14, 25, 5, 789 }, { 2011, 1, 2, 3, 4, 5, 5, 999 }, { 2012, 1, 2, 3, 4, 5, 5, 000 }, InDiscovery, CouldNotFindServer, "In Discovery", INT_MAX, { 2013, 1, 2, 3, 4, 55, 5, 555 }
				},
				{ { "255.255.255.255", INT_MIN + 1 }, { 2011, 2, 0, 3, 14, 25, 6, 789 }, { 2011, 1, 2, 3, 4, 5, 6, 999 }, { 2012, 1, 2, 3, 4, 5, 6, 000 }, Connected, CouldNotFindServer, INT_MIN, { 2013, 1, 2, 3, 4, 55, 6, 555 },
				{ "255.255.255.255", INT_MIN + 1 }, { 2011, 2, 0, 3, 14, 25, 6, 789 }, { 2011, 1, 2, 3, 4, 5, 6, 999 }, { 2012, 1, 2, 3, 4, 5, 6, 000 }, Connected, CouldNotFindServer, "Connected", INT_MIN, { 2013, 1, 2, 3, 4, 55, 6, 555 }
				},
				{ { "1.2.3.107", 20007 }, { 2011, 2, 0, 3, 14, 25, 7, 789 }, { 2011, 1, 2, 3, 4, 5, 7, 999 }, { 2012, 1, 2, 3, 4, 5, 7, 000 }, Disconnected, CouldNotFindServer, INT_MIN + 1, { 2013, 1, 2, 3, 4, 55, 7, 555 },
				{ "1.2.3.107", 20007 }, { 2011, 2, 0, 3, 14, 25, 7, 789 }, { 2011, 1, 2, 3, 4, 5, 7, 999 }, { 2012, 1, 2, 3, 4, 5, 7, 000 }, Disconnected, CouldNotFindServer, "Disconnected", INT_MIN + 1, { 2013, 1, 2, 3, 4, 55, 7, 555 }
				},
				{ { "1.2.3.109", 20009 }, { 2011, 2, 0, 3, 14, 25, 9, 789 }, { 2011, 1, 2, 3, 4, 5, 9, 999 }, { 2012, 1, 2, 3, 4, 5, 9, 000 }, Error, (ProgressiveConnectionError)(ProgressiveConnectionErrorNone - 1), (int)DWORD_MAX - 1, { 2013, 1, 2, 3, 4, 55, 9, 555 },
				{ "1.2.3.109", 20009 }, { 2011, 2, 0, 3, 14, 25, 9, 789 }, { 2011, 1, 2, 3, 4, 5, 9, 999 }, { 2012, 1, 2, 3, 4, 5, 9, 000 }, Error, (ProgressiveConnectionError)(ProgressiveConnectionErrorNone - 1), "Error", (int)DWORD_MAX - 1, { 2013, 1, 2, 3, 4, 55, 9, 555 }
				},
				{ { "1.2.3.110", 20010 }, { 2011, 2, 0, 3, 14, 25, 10, 789 }, { 2011, 1, 2, 3, 4, 5, 10, 999 }, { 2012, 1, 2, 3, 4, 5, 10, 000 }, Error, ProgressiveConnectionErrorNone, (int)DWORD_MAX, { 2013, 1, 2, 3, 4, 55, 10, 555 },
				{ "1.2.3.110", 20010 }, { 2011, 2, 0, 3, 14, 25, 10, 789 }, { 2011, 1, 2, 3, 4, 5, 10, 999 }, { 2012, 1, 2, 3, 4, 5, 10, 000 }, Error, ProgressiveConnectionErrorNone,"Error", (int)DWORD_MAX, { 2013, 1, 2, 3, 4, 55, 10, 555 }
				},
				{ { "1.2.3.111", 20011 }, { 2011, 2, 0, 3, 14, 25, 11, 789 }, { 2011, 1, 2, 3, 4, 5, 11, 999 }, { 2012, 1, 2, 3, 4, 5, 11, 000 }, Error, DiscoveryConnectionError, 0x12345678, { 2013, 1, 2, 3, 4, 55, 11, 555 },
				{ "1.2.3.111", 20011 }, { 2011, 2, 0, 3, 14, 25, 11, 789 }, { 2011, 1, 2, 3, 4, 5, 11, 999 }, { 2012, 1, 2, 3, 4, 5, 11, 000 }, Error, DiscoveryConnectionError, "Error: Discovery Connection", 0x12345678, { 2013, 1, 2, 3, 4, 55, 11, 555 }
				},
				{ { "1.2.3.112", 20012 }, { 2011, 2, 0, 3, 14, 25, 12, 789 }, { 2011, 1, 2, 3, 4, 5, 12, 999 }, { 2012, 1, 2, 3, 4, 5, 12, 000 }, Error, MessageTransmissionError, -0x12345678, { 2013, 1, 2, 3, 4, 55, 12, 555 },
				{ "1.2.3.112", 20012 }, { 2011, 2, 0, 3, 14, 25, 12, 789 }, { 2011, 1, 2, 3, 4, 5, 12, 999 }, { 2012, 1, 2, 3, 4, 5, 12, 000 }, Error, MessageTransmissionError, "Error: Message Transmission", -0x12345678, { 2013, 1, 2, 3, 4, 55, 12, 555 }
				},
				{ { "1.2.3.113", 20013 }, { 2011, 2, 0, 3, 14, 25, 13, 789 }, { 2011, 1, 2, 3, 4, 5, 13, 999 }, { 2012, 1, 2, 3, 4, 5, 13, 000 }, Error, ConnectFailed, 1313, { 2013, 1, 2, 3, 4, 55, 13, 555 },
				{ "1.2.3.113", 20013 }, { 2011, 2, 0, 3, 14, 25, 13, 789 }, { 2011, 1, 2, 3, 4, 5, 13, 999 }, { 2012, 1, 2, 3, 4, 5, 13, 000 }, Error, ConnectFailed, "Error: Connect Failed", 1313, { 2013, 1, 2, 3, 4, 55, 13, 555 }
				},
				{ { "1.2.3.114", 20014 }, { 2011, 2, 0, 3, 14, 25, 14, 789 }, { 2011, 1, 2, 3, 4, 5, 14, 999 }, { 2012, 1, 2, 3, 4, 5, 14, 000 }, Error, DisconnectFailed, 1414, { 2013, 1, 2, 3, 4, 55, 14, 555 },
				{ "1.2.3.114", 20014 }, { 2011, 2, 0, 3, 14, 25, 14, 789 }, { 2011, 1, 2, 3, 4, 5, 14, 999 }, { 2012, 1, 2, 3, 4, 5, 14, 000 }, Error, DisconnectFailed, "Error: Disconnect Failed", 1414, { 2013, 1, 2, 3, 4, 55, 14, 555 }
				},
				{ { "1.2.3.115", 20015 }, { 2011, 2, 0, 3, 14, 25, 15, 789 }, { 2011, 1, 2, 3, 4, 5, 15, 999 }, { 2012, 1, 2, 3, 4, 5, 15, 000 }, Error, (ProgressiveConnectionError)(DisconnectFailed + 1), 1515, { 2013, 1, 2, 3, 4, 55, 15, 555 },
				{ "1.2.3.115", 20015 }, { 2011, 2, 0, 3, 14, 25, 15, 789 }, { 2011, 1, 2, 3, 4, 5, 15, 999 }, { 2012, 1, 2, 3, 4, 5, 15, 000 }, Error, (ProgressiveConnectionError)(DisconnectFailed + 1), "Error", 1515, { 2013, 1, 2, 3, 4, 55, 15, 555 }
				},
				{ { "1.2.3.116", 20016 }, { 2011, 2, 0, 3, 14, 25, 16, 789 }, { 2011, 1, 2, 3, 4, 5, 16, 999 }, { 2012, 1, 2, 3, 4, 5, 16, 000 }, Error, (ProgressiveConnectionError)INT_MAX, 1616, { 2013, 1, 2, 3, 4, 55, 16, 555 },
				{ "1.2.3.116", 20016 }, { 2011, 2, 0, 3, 14, 25, 16, 789 }, { 2011, 1, 2, 3, 4, 5, 16, 999 }, { 2012, 1, 2, 3, 4, 5, 16, 000 }, Error, (ProgressiveConnectionError)INT_MAX, "Error", 1616, { 2013, 1, 2, 3, 4, 55, 16, 555 }
				},
				{ { "1.2.3.117", 20017 }, { 2011, 2, 0, 3, 14, 25, 17, 789 }, { 2011, 1, 2, 3, 4, 5, 17, 999 }, { 2012, 1, 2, 3, 4, 5, 17, 000 }, Error, (ProgressiveConnectionError)DWORD_MAX, 1717, { 2013, 1, 2, 3, 4, 55, 17, 555 },
				{ "1.2.3.117", 20017 }, { 2011, 2, 0, 3, 14, 25, 17, 789 }, { 2011, 1, 2, 3, 4, 5, 17, 999 }, { 2012, 1, 2, 3, 4, 5, 17, 000 }, Error, (ProgressiveConnectionError)DWORD_MAX, "Error", 1717, { 2013, 1, 2, 3, 4, 55, 17, 555 }
				},

				{ { "1.2.3.118", 20018 }, { 2011, 2, 0, 3, 14, 25, 18, 789 }, { 2011, 1, 2, 3, 4, 5, 18, 999 }, { 2012, 1, 2, 3, 4, 5, 18, 000 }, (ProgressiveConnectionState)(Error + 1), CouldNotFindServer, 1818, { 2013, 1, 2, 3, 4, 55, 18, 555 },
				{ "1.2.3.118", 20018 }, { 2011, 2, 0, 3, 14, 25, 18, 789 }, { 2011, 1, 2, 3, 4, 5, 18, 999 }, { 2012, 1, 2, 3, 4, 5, 18, 000 }, (ProgressiveConnectionState)(Error + 1), CouldNotFindServer, "N/A", 1818, { 2013, 1, 2, 3, 4, 55, 18, 555 }
				},
				{ { "1.2.3.119", 20019 }, { 2011, 2, 0, 3, 14, 25, 19, 789 }, { 2011, 1, 2, 3, 4, 5, 19, 999 }, { 2012, 1, 2, 3, 4, 5, 19, 000 }, (ProgressiveConnectionState)(INT_MAX), CouldNotFindServer, 1919, { 2013, 1, 2, 3, 4, 55, 19, 555 },
				{ "1.2.3.119", 20019 }, { 2011, 2, 0, 3, 14, 25, 19, 789 }, { 2011, 1, 2, 3, 4, 5, 19, 999 }, { 2012, 1, 2, 3, 4, 5, 19, 000 }, (ProgressiveConnectionState)INT_MAX, CouldNotFindServer, "N/A", 1919, { 2013, 1, 2, 3, 4, 55, 19, 555 }
				},
				{ { "1.2.3.120", 20020 }, { 2011, 2, 0, 3, 14, 25, 20, 789 }, { 2011, 1, 2, 3, 4, 5, 20, 999 }, { 2012, 1, 2, 3, 4, 5, 20, 000 }, (ProgressiveConnectionState)(INT_MIN), CouldNotFindServer, 2020, { 2013, 1, 2, 3, 4, 55, 20, 555 },
				{ "1.2.3.120", 20020 }, { 2011, 2, 0, 3, 14, 25, 20, 789 }, { 2011, 1, 2, 3, 4, 5, 20, 999 }, { 2012, 1, 2, 3, 4, 5, 20, 000 }, (ProgressiveConnectionState)INT_MIN, CouldNotFindServer, "N/A", 2020, { 2013, 1, 2, 3, 4, 55, 20, 555 }
				},
				{ { "1.2.3.121", 20021 }, { 2011, 2, 0, 3, 14, 25, 21, 789 }, { 2011, 1, 2, 3, 4, 5, 21, 999 }, { 2012, 1, 2, 3, 4, 5, 21, 000 }, (ProgressiveConnectionState)DWORD_MAX, CouldNotFindServer, 2121, { 2013, 1, 2, 3, 4, 55, 21, 555 },
				{ "1.2.3.121", 20021 }, { 2011, 2, 0, 3, 14, 25, 21, 789 }, { 2011, 1, 2, 3, 4, 5, 21, 999 }, { 2012, 1, 2, 3, 4, 5, 21, 000 }, (ProgressiveConnectionState)DWORD_MAX, CouldNotFindServer, "N/A", 2121, { 2013, 1, 2, 3, 4, 55, 21, 555 }
				},
			};

			for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CIPAddress ipAddress;
				ipAddress.SetIP(TEST_DATA[testIndex].m_lastMessageIP.m_ipAddressString);
				ipAddress.SetPort(TEST_DATA[testIndex].m_lastMessageIP.m_port);
				CProgressiveConnectionInfo varProgInfo;
				varProgInfo.SetLastMessageIP(ipAddress);
				varProgInfo.SetLastMessageTimestamp(TEST_DATA[testIndex].m_lastMessageTimestamp);
				varProgInfo.SetLastConnectAttemptTimestamp(TEST_DATA[testIndex].m_lastConnectAttemptTimestampExpected);
				varProgInfo.SetLastConnectTimestamp(TEST_DATA[testIndex].m_lastConnectionTimestamp);
				varProgInfo.SetState(TEST_DATA[testIndex].m_state);
				varProgInfo.SetLastError(TEST_DATA[testIndex].m_error);
				varProgInfo.SetConnectAttemptCount(TEST_DATA[testIndex].m_connectAttemptCount);
				varProgInfo.SetLastAddressUpdateTimestamp(TEST_DATA[testIndex].m_lastAddressUpdateTimestamp);
				const CProgressiveConnectionInfo& progInfo(varProgInfo);

				CIPAddress lastMessageIPExpected;
				lastMessageIPExpected.SetIP(TEST_DATA[testIndex].m_lastMessageIPExpected.m_ipAddressString);
				lastMessageIPExpected.SetPort(TEST_DATA[testIndex].m_lastMessageIPExpected.m_port);

				// Perfrom operations(s) to be tested.
				const CProgressiveConnectionInfo progInfoA(progInfo);

				// Test(s).
				AssertEqual(progInfoA,
					lastMessageIPExpected,
					TEST_DATA[testIndex].m_lastMessageTimestampExpected,
					TEST_DATA[testIndex].m_lastConnectAttemptTimestampExpected,
					TEST_DATA[testIndex].m_lastConnectionTimestampExpected,
					TEST_DATA[testIndex].m_connectionStateExpected,
					TEST_DATA[testIndex].m_lastConnectionErrorExpected,
					TEST_DATA[testIndex].m_connectionStateStringExpected,
					TEST_DATA[testIndex].m_connectAttemptCountExpected,
					TEST_DATA[testIndex].m_lastAddressUpdateTimestampExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///		 CProgressiveConnectionInfo::CProgressiveConnectionInfo()
		///		 CProgressiveConnectionInfo::CProgressiveConnectionInfo& operator=(const CProgressiveConnectionInfo &rhs)
		/// Use a default CProgressiveConnectionInfo instance.
		/// </summary>
		void f_AssignmentOperator_DefaultTest(void)
		{
			// Setup.
			const int testIndex(0);
			const CProgressiveConnectionInfo progInfo;

			// Verify setup.
			AssertEqual(progInfo,
				m_lastMessageIPExpected,
				m_lastMessageTimestampExpected,
				m_lastConnectAttemptTimestampExpected,
				m_lastConnectionTimestampExpected,
				m_connectionStateExpected,
				m_lastConnectionErrorExpected,
				m_connectionStateStringExpected,
				m_connectAttemptCountExpected,
				m_lastAddressUpdateTimestampExpected,
				testIndex, __TFUNCTION__, __LINE__);

			CProgressiveConnectionInfo varProgInfoB;
			varProgInfoB.SetConnectAttemptCount(99999);

			// Perform operation(s) to be tested.
			varProgInfoB = progInfo;
			const CProgressiveConnectionInfo& progInfoB(varProgInfoB);

			// Test(s).
			AssertEqual(progInfoB,
				m_lastMessageIPExpected,
				m_lastMessageTimestampExpected,
				m_lastConnectAttemptTimestampExpected,
				m_lastConnectionTimestampExpected,
				m_connectionStateExpected,
				m_lastConnectionErrorExpected,
				m_connectionStateStringExpected,
				m_connectAttemptCountExpected,
				m_lastAddressUpdateTimestampExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///		 CProgressiveConnectionInfo::SetLastMessageIP()
		///		 CProgressiveConnectionInfo::SetLastMessageTimestamp()
		///		 CProgressiveConnectionInfo::SetLastConnectAttemptTimestamp()
		///		 CProgressiveConnectionInfo::SetLastConnectTimestamp()
		///		 CProgressiveConnectionInfo::SetState()
		///		 CProgressiveConnectionInfo::SetLastError()
		///		 CProgressiveConnectionInfo::SetConnectAttemptCount()
		///		 CProgressiveConnectionInfo::SetLastAddressUpdateTimestamp()
		///		 CProgressiveConnectionInfo::GetLastMessageIP()
		///		 CProgressiveConnectionInfo::GetLastMessageTimestamp()
		///		 CProgressiveConnectionInfo::GetLastMessageTimestampString()
		///		 CProgressiveConnectionInfo::GetLastConnectAttemptTimestamp()
		///		 CProgressiveConnectionInfo::GetLastConnectTimestamp()
		///		 CProgressiveConnectionInfo::GetLastConnectTimestampString()
		///		 CProgressiveConnectionInfo::GetState()
		///		 CProgressiveConnectionInfo::GetLastError()
		///		 CProgressiveConnectionInfo::GetConnectionStateString()
		///		 CProgressiveConnectionInfo::GetConnectAttemptCount()
		///		 CProgressiveConnectionInfo::GetLastAddressUpdateTimestamp()
		///		 CProgressiveConnectionInfo::CProgressiveConnectionInfo& operator=(const CProgressiveConnectionInfo &rhs)
		/// </summary>
		void f_AssignmentOperator_SetGetTest(void)
		{
			typedef struct {
				LPCTSTR const m_ipAddressString;
				int m_port;
			} TIPAddressStringPort;

			typedef struct
			{
				// Input(s).
				const TIPAddressStringPort m_lastMessageIP;
				const SYSTEMTIME m_lastMessageTimestamp;
				const SYSTEMTIME m_lastConnectAttemptTimestamp;
				const SYSTEMTIME m_lastConnectionTimestamp;
				const ProgressiveConnectionState m_state;
				const ProgressiveConnectionError m_error;
				const int m_connectAttemptCount;
				const SYSTEMTIME m_lastAddressUpdateTimestamp;

				// Expected result(s).
				const TIPAddressStringPort m_lastMessageIPExpected;
				const SYSTEMTIME m_lastMessageTimestampExpected;
				const SYSTEMTIME m_lastConnectAttemptTimestampExpected;
				const SYSTEMTIME m_lastConnectionTimestampExpected;
				const ProgressiveConnectionState m_connectionStateExpected;
				const ProgressiveConnectionError m_lastConnectionErrorExpected;
				const string m_connectionStateStringExpected;
				const int m_connectAttemptCountExpected;
				const SYSTEMTIME m_lastAddressUpdateTimestampExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ { "*", -999999 }, { 2011, 2, 0, 3, 14, 25, 1, 789 }, { 2011, 1, 2, 3, 4, 5, 1, 999 }, { 2012, 1, 2, 3, 4, 5, 1, 000 }, (ProgressiveConnectionState)(NotStarted - 1), CouldNotFindServer, 1, { 2013, 1, 2, 3, 4, 55, 1, 555 },
				{ "*", -999999 }, { 2011, 2, 0, 3, 14, 25, 1, 789 }, { 2011, 1, 2, 3, 4, 5, 1, 999 }, { 2012, 1, 2, 3, 4, 5, 1, 000 }, (ProgressiveConnectionState)(NotStarted - 1), CouldNotFindServer, "N/A", 1, { 2013, 1, 2, 3, 4, 55, 1, 555 }
				},
				{ { "n/a", -1234567890 }, { 2011, 2, 0, 3, 14, 25, 2, 789 }, { 2011, 1, 2, 3, 4, 5, 2, 999 }, { 2012, 1, 2, 3, 4, 5, 2, 000 }, NotStarted, CouldNotFindServer, 2, { 2013, 1, 2, 3, 4, 55, 2, 555 },
				{ "n/a", -1234567890 }, { 2011, 2, 0, 3, 14, 25, 2, 789 }, { 2011, 1, 2, 3, 4, 5, 2, 999 }, { 2012, 1, 2, 3, 4, 5, 2, 000 }, NotStarted, CouldNotFindServer, "Not Started", 2, { 2013, 1, 2, 3, 4, 55, 2, 555 }
				},
				{ { " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 ", INT_MIN }, { 2011, 2, 0, 3, 14, 25, 3, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 3, 000 }, Disabled, CouldNotFindServer, 1234567890, { 2013, 1, 2, 3, 4, 55, 3, 555 },
				{ " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 ", INT_MIN }, { 2011, 2, 0, 3, 14, 25, 3, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 3, 000 }, Disabled, CouldNotFindServer, "Disabled", 1234567890, { 2013, 1, 2, 3, 4, 55, 3, 555 }
				},
				{ { "abcdefghijklmnopqrstuvwxyz", INT_MAX }, { 2011, 2, 0, 3, 14, 25, 4, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 4, 000 }, WaitingForInfo, CouldNotFindServer, INT_MAX - 1, { 2013, 1, 2, 3, 4, 55, 4, 555 },
				{ "abcdefghijklmnopqrstuvwxyz", INT_MAX }, { 2011, 2, 0, 3, 14, 25, 4, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 4, 000 }, WaitingForInfo, CouldNotFindServer, "Waiting For Info", INT_MAX - 1, { 2013, 1, 2, 3, 4, 55, 4, 555 }
				},
				{ { "0.0.0.0", INT_MAX - 1 }, { 2011, 2, 0, 3, 14, 25, 5, 789 }, { 2011, 1, 2, 3, 4, 5, 5, 999 }, { 2012, 1, 2, 3, 4, 5, 5, 000 }, InDiscovery, CouldNotFindServer, INT_MAX, { 2013, 1, 2, 3, 4, 55, 5, 555 },
				{ "0.0.0.0", INT_MAX - 1 }, { 2011, 2, 0, 3, 14, 25, 5, 789 }, { 2011, 1, 2, 3, 4, 5, 5, 999 }, { 2012, 1, 2, 3, 4, 5, 5, 000 }, InDiscovery, CouldNotFindServer, "In Discovery", INT_MAX, { 2013, 1, 2, 3, 4, 55, 5, 555 }
				},
				{ { "255.255.255.255", INT_MIN + 1 }, { 2011, 2, 0, 3, 14, 25, 6, 789 }, { 2011, 1, 2, 3, 4, 5, 6, 999 }, { 2012, 1, 2, 3, 4, 5, 6, 000 }, Connected, CouldNotFindServer, INT_MIN, { 2013, 1, 2, 3, 4, 55, 6, 555 },
				{ "255.255.255.255", INT_MIN + 1 }, { 2011, 2, 0, 3, 14, 25, 6, 789 }, { 2011, 1, 2, 3, 4, 5, 6, 999 }, { 2012, 1, 2, 3, 4, 5, 6, 000 }, Connected, CouldNotFindServer,"Connected", INT_MIN, { 2013, 1, 2, 3, 4, 55, 6, 555 }
				},
				{ { "1.2.3.107", 20007 }, { 2011, 2, 0, 3, 14, 25, 7, 789 }, { 2011, 1, 2, 3, 4, 5, 7, 999 }, { 2012, 1, 2, 3, 4, 5, 7, 000 }, Disconnected, CouldNotFindServer, INT_MIN + 1, { 2013, 1, 2, 3, 4, 55, 7, 555 },
				{ "1.2.3.107", 20007 }, { 2011, 2, 0, 3, 14, 25, 7, 789 }, { 2011, 1, 2, 3, 4, 5, 7, 999 }, { 2012, 1, 2, 3, 4, 5, 7, 000 }, Disconnected, CouldNotFindServer,"Disconnected", INT_MIN + 1, { 2013, 1, 2, 3, 4, 55, 7, 555 }
				},
				{ { "1.2.3.109", 20009 }, { 2011, 2, 0, 3, 14, 25, 9, 789 }, { 2011, 1, 2, 3, 4, 5, 9, 999 }, { 2012, 1, 2, 3, 4, 5, 9, 000 }, Error, (ProgressiveConnectionError)(ProgressiveConnectionErrorNone - 1), (int)DWORD_MAX - 1, { 2013, 1, 2, 3, 4, 55, 9, 555 },
				{ "1.2.3.109", 20009 }, { 2011, 2, 0, 3, 14, 25, 9, 789 }, { 2011, 1, 2, 3, 4, 5, 9, 999 }, { 2012, 1, 2, 3, 4, 5, 9, 000 }, Error, (ProgressiveConnectionError)(ProgressiveConnectionErrorNone - 1), "Error", (int)DWORD_MAX - 1, { 2013, 1, 2, 3, 4, 55, 9, 555 }
				},
				{ { "1.2.3.110", 20010 }, { 2011, 2, 0, 3, 14, 25, 10, 789 }, { 2011, 1, 2, 3, 4, 5, 10, 999 }, { 2012, 1, 2, 3, 4, 5, 10, 000 }, Error, ProgressiveConnectionErrorNone, (int)DWORD_MAX, { 2013, 1, 2, 3, 4, 55, 10, 555 },
				{ "1.2.3.110", 20010 }, { 2011, 2, 0, 3, 14, 25, 10, 789 }, { 2011, 1, 2, 3, 4, 5, 10, 999 }, { 2012, 1, 2, 3, 4, 5, 10, 000 }, Error, ProgressiveConnectionErrorNone,"Error", (int)DWORD_MAX, { 2013, 1, 2, 3, 4, 55, 10, 555 }
				},
				{ { "1.2.3.111", 20011 }, { 2011, 2, 0, 3, 14, 25, 11, 789 }, { 2011, 1, 2, 3, 4, 5, 11, 999 }, { 2012, 1, 2, 3, 4, 5, 11, 000 }, Error, DiscoveryConnectionError, 0x12345678, { 2013, 1, 2, 3, 4, 55, 11, 555 },
				{ "1.2.3.111", 20011 }, { 2011, 2, 0, 3, 14, 25, 11, 789 }, { 2011, 1, 2, 3, 4, 5, 11, 999 }, { 2012, 1, 2, 3, 4, 5, 11, 000 }, Error, DiscoveryConnectionError, "Error: Discovery Connection", 0x12345678, { 2013, 1, 2, 3, 4, 55, 11, 555 }
				},
				{ { "1.2.3.112", 20012 }, { 2011, 2, 0, 3, 14, 25, 12, 789 }, { 2011, 1, 2, 3, 4, 5, 12, 999 }, { 2012, 1, 2, 3, 4, 5, 12, 000 }, Error, MessageTransmissionError, -0x12345678, { 2013, 1, 2, 3, 4, 55, 12, 555 },
				{ "1.2.3.112", 20012 }, { 2011, 2, 0, 3, 14, 25, 12, 789 }, { 2011, 1, 2, 3, 4, 5, 12, 999 }, { 2012, 1, 2, 3, 4, 5, 12, 000 }, Error, MessageTransmissionError, "Error: Message Transmission", -0x12345678, { 2013, 1, 2, 3, 4, 55, 12, 555 }
				},
				{ { "1.2.3.113", 20013 }, { 2011, 2, 0, 3, 14, 25, 13, 789 }, { 2011, 1, 2, 3, 4, 5, 13, 999 }, { 2012, 1, 2, 3, 4, 5, 13, 000 }, Error, ConnectFailed, 1313, { 2013, 1, 2, 3, 4, 55, 13, 555 },
				{ "1.2.3.113", 20013 }, { 2011, 2, 0, 3, 14, 25, 13, 789 }, { 2011, 1, 2, 3, 4, 5, 13, 999 }, { 2012, 1, 2, 3, 4, 5, 13, 000 }, Error, ConnectFailed,"Error: Connect Failed", 1313, { 2013, 1, 2, 3, 4, 55, 13, 555 }
				},
				{ { "1.2.3.114", 20014 }, { 2011, 2, 0, 3, 14, 25, 14, 789 }, { 2011, 1, 2, 3, 4, 5, 14, 999 }, { 2012, 1, 2, 3, 4, 5, 14, 000 }, Error, DisconnectFailed, 1414, { 2013, 1, 2, 3, 4, 55, 14, 555 },
				{ "1.2.3.114", 20014 }, { 2011, 2, 0, 3, 14, 25, 14, 789 }, { 2011, 1, 2, 3, 4, 5, 14, 999 }, { 2012, 1, 2, 3, 4, 5, 14, 000 }, Error, DisconnectFailed,"Error: Disconnect Failed", 1414, { 2013, 1, 2, 3, 4, 55, 14, 555 }
				},
				{ { "1.2.3.115", 20015 }, { 2011, 2, 0, 3, 14, 25, 15, 789 }, { 2011, 1, 2, 3, 4, 5, 15, 999 }, { 2012, 1, 2, 3, 4, 5, 15, 000 }, Error, (ProgressiveConnectionError)(DisconnectFailed + 1), 1515, { 2013, 1, 2, 3, 4, 55, 15, 555 },
				{ "1.2.3.115", 20015 }, { 2011, 2, 0, 3, 14, 25, 15, 789 }, { 2011, 1, 2, 3, 4, 5, 15, 999 }, { 2012, 1, 2, 3, 4, 5, 15, 000 }, Error, (ProgressiveConnectionError)(DisconnectFailed + 1), "Error", 1515, { 2013, 1, 2, 3, 4, 55, 15, 555 }
				},
				{ { "1.2.3.116", 20016 }, { 2011, 2, 0, 3, 14, 25, 16, 789 }, { 2011, 1, 2, 3, 4, 5, 16, 999 }, { 2012, 1, 2, 3, 4, 5, 16, 000 }, Error, (ProgressiveConnectionError)INT_MAX, 1616, { 2013, 1, 2, 3, 4, 55, 16, 555 },
				{ "1.2.3.116", 20016 }, { 2011, 2, 0, 3, 14, 25, 16, 789 }, { 2011, 1, 2, 3, 4, 5, 16, 999 }, { 2012, 1, 2, 3, 4, 5, 16, 000 }, Error, (ProgressiveConnectionError)INT_MAX, "Error", 1616, { 2013, 1, 2, 3, 4, 55, 16, 555 }
				},
				{ { "1.2.3.117", 20017 }, { 2011, 2, 0, 3, 14, 25, 17, 789 }, { 2011, 1, 2, 3, 4, 5, 17, 999 }, { 2012, 1, 2, 3, 4, 5, 17, 000 }, Error, (ProgressiveConnectionError)DWORD_MAX, 1717, { 2013, 1, 2, 3, 4, 55, 17, 555 },
				{ "1.2.3.117", 20017 }, { 2011, 2, 0, 3, 14, 25, 17, 789 }, { 2011, 1, 2, 3, 4, 5, 17, 999 }, { 2012, 1, 2, 3, 4, 5, 17, 000 }, Error, (ProgressiveConnectionError)DWORD_MAX, "Error", 1717, { 2013, 1, 2, 3, 4, 55, 17, 555 }
				},

				{ { "1.2.3.118", 20018 }, { 2011, 2, 0, 3, 14, 25, 18, 789 }, { 2011, 1, 2, 3, 4, 5, 18, 999 }, { 2012, 1, 2, 3, 4, 5, 18, 000 }, (ProgressiveConnectionState)(Error + 1), CouldNotFindServer, 1818, { 2013, 1, 2, 3, 4, 55, 18, 555 },
				{ "1.2.3.118", 20018 }, { 2011, 2, 0, 3, 14, 25, 18, 789 }, { 2011, 1, 2, 3, 4, 5, 18, 999 }, { 2012, 1, 2, 3, 4, 5, 18, 000 }, (ProgressiveConnectionState)(Error + 1), CouldNotFindServer, "N/A", 1818, { 2013, 1, 2, 3, 4, 55, 18, 555 }
				},
				{ { "1.2.3.119", 20019 }, { 2011, 2, 0, 3, 14, 25, 19, 789 }, { 2011, 1, 2, 3, 4, 5, 19, 999 }, { 2012, 1, 2, 3, 4, 5, 19, 000 }, (ProgressiveConnectionState)INT_MAX, CouldNotFindServer, 1919, { 2013, 1, 2, 3, 4, 55, 19, 555 },
				{ "1.2.3.119", 20019 }, { 2011, 2, 0, 3, 14, 25, 19, 789 }, { 2011, 1, 2, 3, 4, 5, 19, 999 }, { 2012, 1, 2, 3, 4, 5, 19, 000 }, (ProgressiveConnectionState)INT_MAX, CouldNotFindServer,"N/A", 1919, { 2013, 1, 2, 3, 4, 55, 19, 555 }
				},
				{ { "1.2.3.120", 20020 }, { 2011, 2, 0, 3, 14, 25, 20, 789 }, { 2011, 1, 2, 3, 4, 5, 20, 999 }, { 2012, 1, 2, 3, 4, 5, 20, 000 }, (ProgressiveConnectionState)INT_MIN, CouldNotFindServer, 2020, { 2013, 1, 2, 3, 4, 55, 20, 555 },
				{ "1.2.3.120", 20020 }, { 2011, 2, 0, 3, 14, 25, 20, 789 }, { 2011, 1, 2, 3, 4, 5, 20, 999 }, { 2012, 1, 2, 3, 4, 5, 20, 000 }, (ProgressiveConnectionState)INT_MIN, CouldNotFindServer, "N/A", 2020, { 2013, 1, 2, 3, 4, 55, 20, 555 }
				},
				{ { "1.2.3.121", 20021 }, { 2011, 2, 0, 3, 14, 25, 21, 789 }, { 2011, 1, 2, 3, 4, 5, 21, 999 }, { 2012, 1, 2, 3, 4, 5, 21, 000 }, (ProgressiveConnectionState)DWORD_MAX, CouldNotFindServer, 2121, { 2013, 1, 2, 3, 4, 55, 21, 555 },
				{ "1.2.3.121", 20021 }, { 2011, 2, 0, 3, 14, 25, 21, 789 }, { 2011, 1, 2, 3, 4, 5, 21, 999 }, { 2012, 1, 2, 3, 4, 5, 21, 000 }, (ProgressiveConnectionState)DWORD_MAX, CouldNotFindServer,"N/A", 2121, { 2013, 1, 2, 3, 4, 55, 21, 555 }
				},
			};

			// Setup. Perform each test with the same instance.
			CProgressiveConnectionInfo varProgInfoA;

			for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new object.
				CProgressiveConnectionInfo varProgInfoB;
				varProgInfoB.SetConnectAttemptCount(99999);

				// Setup. Calculate lastMessageIPExpected.
				CIPAddress lastMessageIPExpected;
				lastMessageIPExpected.SetIP(TEST_DATA[testIndex].m_lastMessageIPExpected.m_ipAddressString);
				lastMessageIPExpected.SetPort(TEST_DATA[testIndex].m_lastMessageIPExpected.m_port);

				// Setup.
				CProgressiveConnectionInfo varProgInfo;
				CIPAddress ipAddress;
				ipAddress.SetIP(TEST_DATA[testIndex].m_lastMessageIP.m_ipAddressString);
				ipAddress.SetPort(TEST_DATA[testIndex].m_lastMessageIP.m_port);
				varProgInfo.SetLastMessageIP(ipAddress);
				varProgInfo.SetLastMessageTimestamp(TEST_DATA[testIndex].m_lastMessageTimestamp);
				varProgInfo.SetLastConnectAttemptTimestamp(TEST_DATA[testIndex].m_lastConnectAttemptTimestampExpected);
				varProgInfo.SetLastConnectTimestamp(TEST_DATA[testIndex].m_lastConnectionTimestamp);
				varProgInfo.SetState(TEST_DATA[testIndex].m_state);
				varProgInfo.SetLastError(TEST_DATA[testIndex].m_error);
				varProgInfo.SetConnectAttemptCount(TEST_DATA[testIndex].m_connectAttemptCount);
				varProgInfo.SetLastAddressUpdateTimestamp(TEST_DATA[testIndex].m_lastAddressUpdateTimestamp);
				const CProgressiveConnectionInfo& progInfo(varProgInfo);

				// Perfrom operations(s) to be tested.
				varProgInfoA = progInfo;
				varProgInfoB = progInfo;
				const CProgressiveConnectionInfo& progInfoA(varProgInfoA);
				const CProgressiveConnectionInfo& progInfoB(varProgInfoB);

				// Test(s).
				AssertEqual(progInfoA,
					lastMessageIPExpected,
					TEST_DATA[testIndex].m_lastMessageTimestampExpected,
					TEST_DATA[testIndex].m_lastConnectAttemptTimestampExpected,
					TEST_DATA[testIndex].m_lastConnectionTimestampExpected,
					TEST_DATA[testIndex].m_connectionStateExpected,
					TEST_DATA[testIndex].m_lastConnectionErrorExpected,
					TEST_DATA[testIndex].m_connectionStateStringExpected,
					TEST_DATA[testIndex].m_connectAttemptCountExpected,
					TEST_DATA[testIndex].m_lastAddressUpdateTimestampExpected,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(progInfoB,
					lastMessageIPExpected,
					TEST_DATA[testIndex].m_lastMessageTimestampExpected,
					TEST_DATA[testIndex].m_lastConnectAttemptTimestampExpected,
					TEST_DATA[testIndex].m_lastConnectionTimestampExpected,
					TEST_DATA[testIndex].m_connectionStateExpected,
					TEST_DATA[testIndex].m_lastConnectionErrorExpected,
					TEST_DATA[testIndex].m_connectionStateStringExpected,
					TEST_DATA[testIndex].m_connectAttemptCountExpected,
					TEST_DATA[testIndex].m_lastAddressUpdateTimestampExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}
		}

		void f_ProgressiveConnectInfoConnectAttempt(void)
		{
			CProgressiveConnectionInfo connectInfo;

			// Make a timestamp that is in the past
			SYSTEMTIME currentTime;
            currentTime = CUtilities::Get1970Time();
			time_t pastTimeInt = CUtilities::SysTimeToUlarge(currentTime);
			pastTimeInt--;
			SYSTEMTIME pastTime = CUtilities::UlargeToSysTime(pastTimeInt);

			// Set the last connection attempt time and last connect time to the past timestamp
			connectInfo.SetLastConnectTimestamp(pastTime);
			connectInfo.SetLastConnectAttemptTimestamp(pastTime);

			// Indicate we have done a connection attempt
			connectInfo.ConnectAttempt();

			// Test the affected member after a connect attemp
			SYSTEMTIME connectAttemptTime = connectInfo.GetLastConnectAttemptTimestamp();
			SYSTEMTIME connectTime = connectInfo.GetLastConnectTimestamp();
			ASSERT_EQ(connectInfo.GetConnectAttemptCount(), 1) << "There should be one connection attempt";
			ASSERT_TRUE(CUtilities::CompareSysTimes(connectAttemptTime, pastTime) > 0) << "The connection attempt timestamp should have been updated";
			ASSERT_TRUE(CUtilities::CompareSysTimes(connectTime, pastTime) == 0) << "The last connection timestamp should have remained the same";

			// Now indicate we are connected.
			connectInfo.SetConnected();

			// Test the affected member after a connect
			connectTime = connectInfo.GetLastConnectTimestamp();
			ASSERT_EQ((int)connectInfo.GetState(), (int)Connected) << "The state should reflect connected";
			ASSERT_TRUE(CUtilities::CompareSysTimes(connectTime, pastTime) > 0) << "The last connection timestamp should have been updated";
		}

// #pragma region CProgressiveConnectionInfo::m_pingInterval
		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		/// Test with the default value.
		/// </summary>
		void f_PingInterval_Default(void)
		{
			// Perform operation(s) and test(s).
			CProgressiveConnectionInfo connectInfo;
			AssertResult2m(connectInfo.m_pingInterval, m_pingIntervalExpected);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::LoadProgressiveConfig()
		/// Test with a non-default value.
		/// </summary>
		void f_PingInterval_LoadProgressiveConfig(void)
		{
			// Data.
			static const DWORD pingIntervalExpected(m_pingIntervalExpected + 4567);
			AssertResult2m(pingIntervalExpected != m_pingIntervalExpected, true); // Verify data.

			// Setup.
			CProgressiveConnectionInfo connectInfo;
			CProgressiveConfig varProgConfig;
			varProgConfig.SetPingInterval(pingIntervalExpected);
			const CProgressiveConfig &progConfig(varProgConfig);
			AssertResult2m(progConfig.GetPingInterval(), pingIntervalExpected); // Verify setup.

			// Perform operation(s) and test(s).
			connectInfo.SetPingInterval(progConfig.GetPingInterval());
			AssertResult2m(connectInfo.m_pingInterval, pingIntervalExpected);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::LoadProgressiveConfig()
		/// Test with a range of values.
		/// </summary>
		void f_PingInterval_LoadProgressiveConfig_Data(void)
		{
			// Data.
			typedef DWORD TestDataType;
			const TestDataType thresholdA(m_pingIntervalMinimumExpected);
			const TestDataType thresholdB(m_pingIntervalMaximumExpected);

			// Setup.  Use the same object for each test.
            DWORD pingIntervalExpected = m_pingIntervalMaximumExpected-1;
			CProgressiveConfig varProgConfig;
			varProgConfig.SetPingInterval(pingIntervalExpected);
			const CProgressiveConfig &progConfig(varProgConfig);

			AssertResult2m(progConfig.GetPingInterval(), pingIntervalExpected); // Verify setup.

            varProgConfig.SetPingInterval(m_pingIntervalMaximumExpected+2);
            CProgressiveConnectionInfo connectInfoA;

			// Perform operation(s) and test(s).
			connectInfoA.SetPingInterval(progConfig.GetPingInterval());
			AssertResult2m(connectInfoA.m_pingInterval, pingIntervalExpected);

			// Setup.  Use a different object  for each test.
            pingIntervalExpected = progConfig.GetPingInterval();
			CProgressiveConnectionInfo connectInfoB;

			// Perform operation(s) and test(s).
			connectInfoB.SetPingInterval(progConfig.GetPingInterval());
			AssertResult2m(connectInfoB.m_pingInterval, pingIntervalExpected);

            connectInfoA = connectInfoB;
			// Perform operation(s) and test(s).
			AssertResult2m(connectInfoA.m_pingInterval, pingIntervalExpected);
		}


// #pragma endregion CProgressiveConnectionInfo::m_pingInterval

// #pragma region CProgressiveConnectionInfo::m_lastHeartbeatMessageTickCount
		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::GetLastHeartbeatMessageTickCount()
		/// Test with the default value.
		/// </summary>
		void f_LastHeartbeatMessageTickCount_Default(void)
		{
			// Perform operation(s) and test(s).
			CProgressiveConnectionInfo connectInfo;
			AssertResult2m(connectInfo.m_lastHeartbeatMessageTickCount, (DWORD)0);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatMessageTickCount()
		///     CProgressiveConnection::GetLastHeartbeatMessageTickCount()
		/// Test with a range of values.
		/// </summary>
		void f_LastHeartbeatMessageTickCount_Data(void)
		{
			// Data.
			typedef DWORD TestDataType;

			// Setup.  Use the same object for each test.
			CProgressiveConnectionInfo connectInfoA;
            TestDataType testValue = CUtilities::GetTickCount();

			// Perform operation(s) and test(s).
			connectInfoA.SetLastHeartbeatMessageTickCount(testValue);
			AssertResult2m(connectInfoA.m_lastHeartbeatMessageTickCount, testValue);

			// Setup.  Use a different object  for each test.
			CProgressiveConnectionInfo connectInfoB;

			// Perform operation(s) and test(s).
			connectInfoB.SetLastHeartbeatMessageTickCount(testValue);
			AssertResult2m(connectInfoB.m_lastHeartbeatMessageTickCount, testValue);
		}

// #pragma endregion CProgressiveConnectionInfo::m_lastHeartbeatMessageTickCount

// #pragma region CProgressiveConnectionInfo::IsTimeForPing
		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetState()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test with a range of m_connectionState values.
		/// </summary>
		void f_IsTimeForPing_SetState(void)
		{
			// Data.
			ProgressiveConnectionState expirationState(Connected);
			DWORD lastHeartbeat(100000);
			DWORD pingInterval(m_pingIntervalMinimumExpected * 2);
			AssertResult2m(m_pingIntervalMinimumExpected <= pingInterval && pingInterval <= m_pingIntervalMaximumExpected, true); // Verify data.

			// Setup.
			CProgressiveConnectionInfo connectInfo;
			connectInfo.SetLastHeartbeatMessageTickCount(lastHeartbeat);
			AssertResult2m(connectInfo.m_lastHeartbeatMessageTickCount, lastHeartbeat); // Verify setup.

			CProgressiveConfig varProgConfig;
			varProgConfig.SetPingInterval(pingInterval);
			const CProgressiveConfig &progConfig(varProgConfig);
			connectInfo.SetPingInterval(progConfig.GetPingInterval());
			AssertResult2m(connectInfo.m_pingInterval, pingInterval); // Verify setup.

			for (m_testIndex = (int)NotStarted - 1; m_testIndex <= (int)Error + 5; ++m_testIndex)
			{
				// Data
				ProgressiveConnectionState connectionState((ProgressiveConnectionState)m_testIndex);

				// Perform operation(s) and test(s).
				connectInfo.SetState(connectionState);

				AssertResult2m(connectInfo.IsTimeForPing(lastHeartbeat - 1), Connected == connectionState);
				AssertResult2m(connectInfo.IsTimeForPing(lastHeartbeat), false);
				AssertResult2m(connectInfo.IsTimeForPing(lastHeartbeat + pingInterval - 1), false);
				AssertResult2m(connectInfo.IsTimeForPing(lastHeartbeat + pingInterval), Connected == connectionState);
			}
		}



		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatMessageTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test with a range of m_lastHeartbeatMessageTickCount values.
		/// </summary>
		void f_IsTimeForPing_SetLastHeartbeatMessageTickCount(void)
		{
			// Data.
			ProgressiveConnectionState expirationState(Connected);
			DWORD pingInterval(m_pingIntervalMinimumExpected * 2);
			AssertResult2m(m_pingIntervalMinimumExpected <= pingInterval && pingInterval <= m_pingIntervalMaximumExpected, true); // Verify data.

			// Setup.
			CProgressiveConnectionInfo connectInfo;
			connectInfo.SetState(expirationState);
			AssertResult2m(connectInfo.GetState(), expirationState); // Verify setup.

			CProgressiveConfig varProgConfig;
			varProgConfig.SetPingInterval(pingInterval);
			const CProgressiveConfig &progConfig(varProgConfig);
			connectInfo.SetPingInterval(progConfig.GetPingInterval());
			AssertResult2m(connectInfo.m_pingInterval, pingInterval); // Verify setup.

			DWORD testValue;
			testValue = CUtilities::GetTickCount();
			DWORD lastHeartbeat(testValue);

			// Perform operation(s) and test(s).
			connectInfo.SetLastHeartbeatMessageTickCount(lastHeartbeat);

			AssertResult2m(connectInfo.IsTimeForPing(lastHeartbeat - 1), true);
			AssertResult2m(connectInfo.IsTimeForPing(lastHeartbeat), false);
			AssertResult2m(connectInfo.IsTimeForPing(lastHeartbeat + pingInterval - 1), false);
			AssertResult2m(connectInfo.IsTimeForPing(lastHeartbeat + pingInterval), true);

		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test with a range of m_pingInterval values.
		/// </summary>
		void f_IsTimeForPing_PingInterval(void)
		{
			// Data.
			ProgressiveConnectionState expirationState(Connected);
			DWORD lastHeartbeat(100000);
			DWORD pingIntervalIncrement(m_pingIntervalMinimumExpected + 1357);
			DWORD numberOfPingTests(20);
			AssertResult2m((pingIntervalIncrement * numberOfPingTests) / numberOfPingTests, pingIntervalIncrement); // Verify no overflow.

			// Setup.
			CProgressiveConnectionInfo connectInfo;
			connectInfo.SetState(expirationState);
			AssertResult2m(connectInfo.GetState(), expirationState); // Verify setup.
			connectInfo.SetLastHeartbeatMessageTickCount(lastHeartbeat);
			AssertResult2m(connectInfo.m_lastHeartbeatMessageTickCount, lastHeartbeat); // Verify setup.

			DWORD pingInterval(m_pingIntervalMinimumExpected);
			for (; m_testIndex < (int)numberOfPingTests; ++m_testIndex)
			{
				// Perform operation(s) and test(s).
				CProgressiveConfig varProgConfig;
				varProgConfig.SetPingInterval(pingInterval);
				const CProgressiveConfig &progConfig(varProgConfig);
				connectInfo.SetPingInterval(progConfig.GetPingInterval());

				AssertResult2m(connectInfo.IsTimeForPing(lastHeartbeat - 1), true);
				AssertResult2m(connectInfo.IsTimeForPing(lastHeartbeat), false);
				AssertResult2m(connectInfo.IsTimeForPing(lastHeartbeat + pingInterval - 1), false);
				AssertResult2m(connectInfo.IsTimeForPing(lastHeartbeat + pingInterval), true);

				// Setup for next loop iteration.
				pingInterval += pingIntervalIncrement;
			}
		}
// #pragma endregion CProgressiveConnectionInfo::IsTimeForPing
	};

		/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, Initialize)
		{
			f_Initialize();
		}

		/// <summary>
		/// Cleanup class member(s) after each test method is run.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, Cleanup)
		{
			f_Cleanup();
			// Test method cleanup code.
		}


		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConnectionInfo::CProgressiveConnectionInfo()
		///     CProgressiveConnectionInfo::GetConnectionStateString()
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, CProgressiveConnectionInfoTest)
		{
			f_CProgressiveConnectionInfoTest();
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConnectionInfo::CProgressiveConnectionInfo()
		///     CProgressiveConnectionInfo::GetConnectionStateString()
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, GetConnectionStateStringDefaultTest)
		{
			f_GetConnectionStateStringDefaultTest();
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConnectionInfo::SetState()
		///     CProgressiveConnectionInfo::SetLastError()
		///     CProgressiveConnectionInfo::GetConnectionStateString()
		/// Test by setting the state first and then the error code.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, GetConnectionStateStringTest)
		{
			f_GetConnectionStateStringTest();
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConnectionInfo::SetState()
		///     CProgressiveConnectionInfo::SetLastError()
		///     CProgressiveConnectionInfo::GetConnectionStateString()
		/// Test by setting the error code first and then the state.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, GetConnectionStateString2Test)
		{
			f_GetConnectionStateString2Test();
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConnectionInfo::SetState()
		///     CProgressiveConnectionInfo::SetLastError()
		///     CProgressiveConnectionInfo::GetConnectionStateString(bool)
		/// Test by setting the state first and then the error code.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, GetConnectionStateStringShortFalseTest)
		{
			f_GetConnectionStateStringShortFalseTest();
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConnectionInfo::SetState()
		///     CProgressiveConnectionInfo::SetLastError()
		///     CProgressiveConnectionInfo::GetConnectionStateString(bool)
		/// Test by setting the state first and then the error code.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, GetConnectionStateStringShortTrueTest)
		{
			f_GetConnectionStateStringShortTrueTest();
		}

		/// <summary>
		/// Code coverage and functional test for:
		///		 CProgressiveConnectionInfo::SetLastMessageIP()
		///		 CProgressiveConnectionInfo::SetLastMessageTimestamp()
		///		 CProgressiveConnectionInfo::SetLastConnectAttemptTimestamp()
		///		 CProgressiveConnectionInfo::SetLastConnectTimestamp()
		///		 CProgressiveConnectionInfo::SetState()
		///		 CProgressiveConnectionInfo::SetLastError()
		///		 CProgressiveConnectionInfo::SetConnectAttemptCount()
		///		 CProgressiveConnectionInfo::SetLastAddressUpdateTimestamp()
		///		 CProgressiveConnectionInfo::GetLastMessageIP()
		///		 CProgressiveConnectionInfo::GetLastMessageTimestamp()
		///		 CProgressiveConnectionInfo::GetLastMessageTimestampString()
		///		 CProgressiveConnectionInfo::GetLastConnectAttemptTimestamp()
		///		 CProgressiveConnectionInfo::GetLastConnectTimestamp()
		///		 CProgressiveConnectionInfo::GetLastConnectTimestampString()
		///		 CProgressiveConnectionInfo::GetState()
		///		 CProgressiveConnectionInfo::GetLastError()
		///		 CProgressiveConnectionInfo::GetConnectionStateString()
		///		 CProgressiveConnectionInfo::GetConnectAttemptCount()
		///		 CProgressiveConnectionInfo::GetLastAddressUpdateTimestamp()
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, SetGetTest)
		{
			f_SetGetTest();
		}

		/// <summary>
		/// Code coverage and functional test for:
		///		 CProgressiveConnectionInfo::CProgressiveConnectionInfo()
		///		 CProgressiveConnectionInfo::CProgressiveConnectionInfo(const CProgressiveConnectionInfo &rhs)
		/// Use a default CProgressiveConnectionInfo instance.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, CopyConstructor_DefaultTest)
		{
			f_CopyConstructor_DefaultTest();
		}

		/// <summary>
		/// Code coverage and functional test for:
		///		 CProgressiveConnectionInfo::SetLastMessageIP()
		///		 CProgressiveConnectionInfo::SetLastMessageTimestamp()
		///		 CProgressiveConnectionInfo::SetLastConnectAttemptTimestamp()
		///		 CProgressiveConnectionInfo::SetLastConnectTimestamp()
		///		 CProgressiveConnectionInfo::SetState()
		///		 CProgressiveConnectionInfo::SetLastError()
		///		 CProgressiveConnectionInfo::SetConnectAttemptCount()
		///		 CProgressiveConnectionInfo::SetLastAddressUpdateTimestamp()
		///		 CProgressiveConnectionInfo::GetLastMessageIP()
		///		 CProgressiveConnectionInfo::GetLastMessageTimestamp()
		///		 CProgressiveConnectionInfo::GetLastMessageTimestampString()
		///		 CProgressiveConnectionInfo::GetLastConnectAttemptTimestamp()
		///		 CProgressiveConnectionInfo::GetLastConnectTimestamp()
		///		 CProgressiveConnectionInfo::GetLastConnectTimestampString()
		///		 CProgressiveConnectionInfo::GetState()
		///		 CProgressiveConnectionInfo::GetLastError()
		///		 CProgressiveConnectionInfo::GetConnectionStateString()
		///		 CProgressiveConnectionInfo::GetConnectAttemptCount()
		///		 CProgressiveConnectionInfo::GetLastAddressUpdateTimestamp()
		///		 CProgressiveConnectionInfo::CProgressiveConnectionInfo(const CProgressiveConnectionInfo &rhs)
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, CopyConstructor_SetGetTest)
		{
			f_CopyConstructor_SetGetTest();
		}

		/// <summary>
		/// Code coverage and functional test for:
		///		 CProgressiveConnectionInfo::CProgressiveConnectionInfo()
		///		 CProgressiveConnectionInfo::CProgressiveConnectionInfo& operator=(const CProgressiveConnectionInfo &rhs)
		/// Use a default CProgressiveConnectionInfo instance.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, AssignmentOperator_DefaultTest)
		{
			f_AssignmentOperator_DefaultTest();
		}

		/// <summary>
		/// Code coverage and functional test for:
		///		 CProgressiveConnectionInfo::SetLastMessageIP()
		///		 CProgressiveConnectionInfo::SetLastMessageTimestamp()
		///		 CProgressiveConnectionInfo::SetLastConnectAttemptTimestamp()
		///		 CProgressiveConnectionInfo::SetLastConnectTimestamp()
		///		 CProgressiveConnectionInfo::SetState()
		///		 CProgressiveConnectionInfo::SetLastError()
		///		 CProgressiveConnectionInfo::SetConnectAttemptCount()
		///		 CProgressiveConnectionInfo::SetLastAddressUpdateTimestamp()
		///		 CProgressiveConnectionInfo::GetLastMessageIP()
		///		 CProgressiveConnectionInfo::GetLastMessageTimestamp()
		///		 CProgressiveConnectionInfo::GetLastMessageTimestampString()
		///		 CProgressiveConnectionInfo::GetLastConnectAttemptTimestamp()
		///		 CProgressiveConnectionInfo::GetLastConnectTimestamp()
		///		 CProgressiveConnectionInfo::GetLastConnectTimestampString()
		///		 CProgressiveConnectionInfo::GetState()
		///		 CProgressiveConnectionInfo::GetLastError()
		///		 CProgressiveConnectionInfo::GetConnectionStateString()
		///		 CProgressiveConnectionInfo::GetConnectAttemptCount()
		///		 CProgressiveConnectionInfo::GetLastAddressUpdateTimestamp()
		///		 CProgressiveConnectionInfo::CProgressiveConnectionInfo& operator=(const CProgressiveConnectionInfo &rhs)
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, AssignmentOperator_SetGetTest)
		{
			f_AssignmentOperator_SetGetTest();
		}

		TEST_F(ProgressiveConnectionInfo, ProgressiveConnectInfoConnectAttempt)
		{
			f_ProgressiveConnectInfoConnectAttempt();
		}

// #pragma region CProgressiveConnectionInfo::m_pingInterval
		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		/// Test with the default value.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, PingInterval_Default)
		{
			f_PingInterval_Default();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::LoadProgressiveConfig()
		/// Test with a non-default value.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, PingInterval_LoadProgressiveConfig)
		{
			f_PingInterval_LoadProgressiveConfig();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::LoadProgressiveConfig()
		/// Test with a range of values.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, PingInterval_LoadProgressiveConfig_Data)
		{
			f_PingInterval_LoadProgressiveConfig_Data();
		}
// #pragma endregion CProgressiveConnectionInfo::m_pingInterval

// #pragma region CProgressiveConnectionInfo::m_lastHeartbeatMessageTickCount
		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::CProgressiveConnection()
		///     CProgressiveConnection::GetLastHeartbeatMessageTickCount()
		/// Test with the default value.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, LastHeartbeatMessageTickCount_Default)
		{
			f_LastHeartbeatMessageTickCount_Default();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatMessageTickCount()
		///     CProgressiveConnection::GetLastHeartbeatMessageTickCount()
		/// Test with a range of values.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, LastHeartbeatMessageTickCount_Data)
		{
			f_LastHeartbeatMessageTickCount_Data();
		}
// #pragma endregion CProgressiveConnectionInfo::m_lastHeartbeatMessageTickCount

// #pragma region CProgressiveConnectionInfo::IsTimeForPing
		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetState()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test with a range of m_connectionState values.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, IsTimeForPing_SetState)
		{
			f_IsTimeForPing_SetState();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::SetLastHeartbeatMessageTickCount()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test with a range of m_lastHeartbeatMessageTickCount values.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, IsTimeForPing_SetLastHeartbeatMessageTickCount)
		{
			f_IsTimeForPing_SetLastHeartbeatMessageTickCount();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveConnection::LoadProgressiveConfig()
		///     CProgressiveConnection::IsTimeForPing()
		/// Test with a range of m_pingInterval values.
		/// </summary>
		TEST_F(ProgressiveConnectionInfo, IsTimeForPing_PingInterval)
		{
			f_IsTimeForPing_PingInterval();
		}

	// Connection Data
	const CIPAddress ProgressiveConnectionInfo::m_lastMessageIPExpected;
	const SYSTEMTIME ProgressiveConnectionInfo::m_lastMessageTimestampExpected = { 1970, 1, 4, 1, 0, 0, 0, 0 };
	const SYSTEMTIME ProgressiveConnectionInfo::m_lastConnectAttemptTimestampExpected = { 1970, 1, 4, 1, 0, 0, 0, 0 };
	const SYSTEMTIME ProgressiveConnectionInfo::m_lastConnectionTimestampExpected = { 1970, 1, 4, 1, 0, 0, 0, 0 };
	const string ProgressiveConnectionInfo::m_connectionStateStringExpected("Not Started");
	const SYSTEMTIME ProgressiveConnectionInfo::m_lastAddressUpdateTimestampExpected = { 1970, 1, 4, 1, 0, 0, 0, 0 };

}
