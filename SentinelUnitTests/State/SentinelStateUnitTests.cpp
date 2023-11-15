#include "stdafx.h"
#include "State/SentinelState.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "UnitTestingUtils.h"

using namespace std;

// TEST_FIX_CDM_DATA: For compatibility with the nCompass v1.18.0.90 implementation of time_t CConfigFile::m_lastConfigFileLoaded,
// change the expected data to match the actual results.
// Future versions of the source code can be updated so that CConfigFile::LoadConfigFile() calls m_operatingSystem.GetCurrentTime()
// instead of calling CUtilities::GetCurrentTime() directly.
// Also CSentinelState::CSentinelState(time_t, LPCTSTR) should initialize m_lastPackageListUpdate to 0 instead of CUtilities::GetCurrentTime().
#define TEST_FIX_CDM_DATA

namespace SentinelNativeUnitTests
{
	class SentinelState : public ::testing::Test
	{
	public:

		// Default expected result(s).
		static const time_t m_lastConfigFileLoadedExpected;

		// Default expected result(s) for CProgressiveConfig.
		static const bool m_progConfig_hasConfigFileExpected = false;
		static const bool m_progConfig_activeExpected = false;
		static const bool m_progConfig_dnsLookupExpected = true;
		static const int m_progConfig_endpointCountExpected = 2;
		static const string m_progConfig_endpointsExpected[m_progConfig_endpointCountExpected];
		static const DWORD m_progConfig_pingIntervalExpected = 15000;
        static const WORD PACKET_VERSION_MULTI_GAME_MULTI_DENOM = 0x0B;

		void AssertEqual(const CProgressiveConfig &obj,
			bool hasConfigFileExpected,
			bool activeExpected,
			bool dnsLookupExpected,
			DWORD pingIntervalExpected,
			const string endpointsExpected[],
			int endpointCountExpected,
			int testIndex,
			LPCTSTR functionName,
			int lineNumber,
			const int* extraIndex = NULL);

		// Default expected result(s) for CProgressiveConnectionInfo.
		static const CIPAddress m_progInfo_lastMessageIPExpected;
		static const SYSTEMTIME m_progInfo_lastMessageTimestampExpected;

		static const SYSTEMTIME m_progInfo_lastConnectAttemptTimestampExpected;
		static const SYSTEMTIME m_progInfo_lastConnectionTimestampExpected;
		static const ProgressiveConnectionState m_progInfo_connectionStateExpected = NotStarted;
		static const ProgressiveConnectionError m_progInfo_lastConnectionErrorExpected = ProgressiveConnectionErrorNone;
		static const string m_progInfo_connectionStateStringExpected;
		static const int m_progInfo_connectAttemptCountExpected = 0;

		static const SYSTEMTIME m_progInfo_lastAddressUpdateTimestampExpected;

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
			const int* extraIndex = NULL);

		// Default expected result(s) for CProgressiveHost.
		static const std::string m_progHost_nameExpected;
		static const bool m_progHost_hasAddressExpected = false;
		static const CIPAddress m_progHost_ipAddressExpected;
		static const size_t m_progHost_currentAddressIndexExpected = 0;
		static const CIPAddress* const m_progHost_ipAddressesExpected;
		static const size_t m_progHost_ipAddressCountExpected = 0;
		static const DWORD m_progHost_hostPortExpected = 0;
		static const std::string m_progHost_urlExpected;

		static const CIPAddress m_defaultIPAddress;

		void AssertEqual(const CProgressiveHost &obj,
			const std::string& nameExpected,
			bool hasAddressExpected,
			const CIPAddress& ipAddressExpected,
			size_t currentAddressIndexExpected,
			const CIPAddress ipAddressesExpected[],
			size_t ipAddressCountExpected,
			DWORD hostPortExpected,
			const std::string& urlExpected,
			int testIndex,
			LPCTSTR functionName,
			int lineNumber,
			const int* extraIndex = NULL);

		void SentinelState_ResponseForSentinelState_ConnectedTCP()
		{
			// Expected Response Data
			std::string expectedResponse;
			expectedResponse = R"({"PollerPacketVersion":12,)";
			expectedResponse += R"("TimeZoneOffset":0,)";
			expectedResponse += R"("ConnectivityStatus":{)";
			expectedResponse +=		R"("EGM":"Connected",)";
			expectedResponse +=		R"("Errors":5,)";
			expectedResponse +=		R"("Poller":"Connected",)";
			expectedResponse +=		R"("TCPIP":"Connected",)";
			expectedResponse +=		R"("PMT":"Connected",)";
			expectedResponse +=		R"("MMT":"Connected",)";
			expectedResponse +=		R"("VS":"Connected",)";
			expectedResponse +=		R"("DisplayYellowCommDown":false,)";
			expectedResponse +=		R"("DisplayRedCommDown":false},)";
			expectedResponse += R"("VirtualPlayer":false,)";
			expectedResponse += R"("IsOneLinkEnabled":true,)";
			expectedResponse += R"("IsMobileConnectEnabled":true,)";
			expectedResponse += R"("IsEmployeeGoodbyeEnabled":true})";
			// Setup.
			CSentinelState obj(time_t(CUtilities::GetTimeFromFields(2020, 11, 04, 10, 11, 12)), _T("2.10.0.1"));
			CUtilities::SetTzOffset(0);

			obj.m_doorOpen = false;
			obj.m_pollerPacketVersion = 12;
			obj.m_gameOnline = true;
			obj.m_egmErrorCount = 5;
			obj.m_connectivityStatus.flags.isPollerOnline = true;
			obj.m_connectivityStatus.flags.isTcpConnected = true;
			obj.m_connectivityStatus.flags.isPMTUp = true;
			obj.m_connectivityStatus.flags.isMMTUp = true;
			obj.m_connectivityStatus.flags.isVoucherServiceUp = true;
			obj.m_virtualPlayer = false;
			obj.m_oneLinkProgressivesEnabled = true;
			obj.m_mobileConnectCommEnabled = true;
			obj.m_employeeGoodbyeEnabled = true;

			obj.m_progressiveConnectionInfo.SetState(ProgressiveConnectionState::Connected);
			CIPAddress ipAddress;
			ipAddress.SetIP("192.168.1.10");
			obj.m_progressiveConnectionInfo.SetLastMessageIP(ipAddress);
			CIPAddressArray ipAddressArray;
			ipAddressArray.Add("192.168.10.1", 100);
			CProgressiveHost progressiveHost(ipAddressArray);
			obj.m_progressiveHost = progressiveHost;
			obj.m_progressiveConfig.SetDNSLookup(false);

			// Perform operation(s) to be tested.
			std::string responseStr = obj.ResponseForSentinelState();
			ASSERT_TRUE(strcmp(responseStr.c_str(), expectedResponse.c_str()) == 0) << "Response string is different from excepted string.";
		}

		void SentinelState_ResponseForSentinelState_DisconnectedTCP()
		{
			// Expected Response Data
			std::string expectedResponse;
			expectedResponse = R"({"PollerPacketVersion":12,)";
			expectedResponse += R"("TimeZoneOffset":0,)";
			expectedResponse += R"("ConnectivityStatus":{)";
			expectedResponse +=		R"("EGM":"Connected",)";
			expectedResponse +=		R"("Errors":5,)";
			expectedResponse +=		R"("Poller":"Connected",)";
			expectedResponse +=		R"("TCPIP":"Not connected",)";
			expectedResponse +=		R"("PMT":"N/A",)";
			expectedResponse +=		R"("MMT":"N/A",)";
			expectedResponse +=		R"("VS":"N/A",)";
			expectedResponse +=		R"("DisplayYellowCommDown":false,)";
			expectedResponse +=		R"("DisplayRedCommDown":true},)";
			expectedResponse += R"("VirtualPlayer":false,)";
			expectedResponse += R"("IsOneLinkEnabled":true,)";
			expectedResponse += R"("IsMobileConnectEnabled":true,)";
			expectedResponse += R"("IsEmployeeGoodbyeEnabled":true})";
			// Setup.
			CSentinelState obj(time_t(CUtilities::GetTimeFromFields(2020, 11, 04, 10, 11, 12)), _T("2.10.0.1"));
			CUtilities::SetTzOffset(0);

			obj.m_doorOpen = false;
			obj.m_pollerPacketVersion = 12;
			obj.m_gameOnline = true;
			obj.m_egmErrorCount = 5;
			obj.m_connectivityStatus.flags.isPollerOnline = true;
			obj.m_connectivityStatus.flags.isTcpConnected = false;
			obj.m_connectivityStatus.flags.isVoucherServiceUp = true;
			obj.m_virtualPlayer = false;
			obj.m_oneLinkProgressivesEnabled = true;
			obj.m_mobileConnectCommEnabled = true;
			obj.m_employeeGoodbyeEnabled = true;

			obj.m_progressiveConnectionInfo.SetState(ProgressiveConnectionState::Connected);
			CIPAddress ipAddress;
			ipAddress.SetIP("192.168.1.10");
			obj.m_progressiveConnectionInfo.SetLastMessageIP(ipAddress);
			CIPAddressArray ipAddressArray;
			ipAddressArray.Add("192.168.10.1", 100);
			CProgressiveHost progressiveHost(ipAddressArray);
			obj.m_progressiveHost = progressiveHost;
			obj.m_progressiveConfig.SetDNSLookup(false);

			// Perform operation(s) to be tested.
			std::string responseStr = obj.ResponseForSentinelState();

			ASSERT_TRUE(strcmp(responseStr.c_str(), expectedResponse.c_str()) == 0) << "Response string is different from excepted response string.";
		}

		void SentinelState_SetMeterRolloverConfigTest()
		{
			// Setup.
			CSentinelState varSentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)), _T("1.12.2.0"));

			// Test(s).
			ASSERT_TRUE(varSentinelState.m_canPerformMeterRollover == false) << _T("m_canPerformMeterRollover");
			ASSERT_TRUE(varSentinelState.m_meterRolloverSyncEnabled == false) << _T("m_meterRolloverSyncEnabled");
			ASSERT_TRUE(varSentinelState.m_meterRolloverTimeHour == INVALID_TIME) << _T("m_meterRolloverTimeHour");
			ASSERT_TRUE(varSentinelState.m_meterRolloverTimeMinute == INVALID_TIME) << _T("m_meterRolloverTimeMinute");
			ASSERT_TRUE(varSentinelState.m_meterRolloverTimeSecond == INVALID_TIME) << _T("m_meterRolloverTimeSecond");
			varSentinelState.SetMeterRolloverConfig(true, 1, 2, 3);
			ASSERT_TRUE(varSentinelState.m_meterRolloverSyncEnabled == true) << _T("m_meterRolloverSyncEnabled");
			ASSERT_TRUE(varSentinelState.m_meterRolloverTimeHour == 1) << _T("m_meterRolloverTimeHour");
			ASSERT_TRUE(varSentinelState.m_meterRolloverTimeMinute == 2) << _T("m_meterRolloverTimeMinute");
			ASSERT_TRUE(varSentinelState.m_meterRolloverTimeSecond == 3) << _T("m_meterRolloverTimeSecond");
		}

		void SentinelState_PerformRolloverMeterCheckTest()
		{
			// Setup.
			CSentinelState varSentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)), _T("1.12.2.0"));

			// Test(s).
			ASSERT_FALSE(varSentinelState.PerformRolloverMeterCheck()) << _T("PerformRolloverMeterCheck");
			ASSERT_FALSE(varSentinelState.m_canPerformMeterRollover) << _T("m_canPerformMeterRollover");
			ASSERT_FALSE(varSentinelState.m_meterRolloverSyncEnabled) << _T("m_meterRolloverSyncEnabled");
			varSentinelState.SetMeterRolloverConfig(true, 23, 59, 59);
			ASSERT_TRUE(varSentinelState.m_meterRolloverSyncEnabled) << _T("m_meterRolloverSyncEnabled");
			ASSERT_FALSE(varSentinelState.PerformRolloverMeterCheck()) << _T("PerformRolloverMeterCheck");
			SYSTEMTIME localTime;
			CUtilities::GetLocalSystemTime(&localTime);
			varSentinelState.m_meterRolloverTimeHour = localTime.wHour;
			varSentinelState.m_meterRolloverTimeMinute = localTime.wMinute;
			varSentinelState.m_meterRolloverTimeSecond = localTime.wSecond;
			ASSERT_TRUE(varSentinelState.PerformRolloverMeterCheck()) << _T("PerformRolloverMeterCheck");
			ASSERT_FALSE(varSentinelState.m_canPerformMeterRollover) << _T("m_canPerformMeterRollover");
			CUtilities::GetLocalSystemTime(&localTime);
			time_t localTimeT = CUtilities::SysTimeToUlarge(localTime);
			localTimeT += 1;
			SYSTEMTIME rolloverTime = CUtilities::GetUTCSystemTime(localTimeT);
			varSentinelState.m_meterRolloverTimeHour = rolloverTime.wHour;
			varSentinelState.m_meterRolloverTimeMinute = rolloverTime.wMinute;
			varSentinelState.m_meterRolloverTimeSecond = rolloverTime.wSecond;
			ASSERT_FALSE(varSentinelState.PerformRolloverMeterCheck()) << _T("PerformRolloverMeterCheck");
			ASSERT_TRUE(varSentinelState.m_canPerformMeterRollover) << _T("m_canPerformMeterRollover");
		}		

		
		void SentinelState_GetSubscriptionDataResponseTest()
		{
			// Setup.
			CSentinelState varSentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)), _T("1.12.2.0"));

			string expResult = "{\"PollerPacketVersion\":0,\"TimeZoneOffset\":0,\"ConnectivityStatus\":{\"EGM\":\"Not connected\",\"Errors\":0,\"Poller\":\"Not connected\",\"TCPIP\":\"Not connected\",\"PMT\":\"N/A\",\"MMT\":\"N/A\",\"VS\":\"N/A\",\"DisplayYellowCommDown\":true,\"DisplayRedCommDown\":true},\"VirtualPlayer\":false,\"IsOneLinkEnabled\":false,\"IsMobileConnectServiceEnabled\":false,\"IsEmployeeGoodbyeEnabled\":false}";
			string result = varSentinelState.GetSubscriptionDataResponse("SentinelState", "");
			ASSERT_EQ(result, expResult) << _T("SentinelState");
			
			expResult = "{\"Version\":\"1.12.2.0\",\"Door\":\"Closed\",\"LastCoinsPlayed\":0}";
			result = varSentinelState.GetSubscriptionDataResponse("Status", "");
			ASSERT_EQ(result, expResult) << _T("Status");

			expResult = "{\"Info\":{\"ConnectionState\":\"Not Started\",\"LastMessageIp\":\"0.0.0.0\",\"LastMessageTime\":\"N/A\",\"HostName\":\"\",\"CurrentHostIp\":\"0.0.0.0\",\"HostList\":[{\"Host\":\"onelinkdiscossl:61614\"},{\"Host\":\"onelinkdisco:61613\"}],\"HostIpAddresses\":[]}}";
			result = varSentinelState.GetSubscriptionDataResponse("OneLinkConnection", "");
			ASSERT_EQ(result, expResult) << _T("OneLinkConnection");

			expResult = "{\"DiskSpace\":[]}";
			result = varSentinelState.GetSubscriptionDataResponse("DiskSpace", "");
			ASSERT_EQ(result, expResult) << _T("DiskSpace");

			expResult = "{\"FileExists\":false,\"Successful\":false}";
			result = varSentinelState.GetSubscriptionDataResponse("MemoryClear", "");
			ASSERT_EQ(result, expResult) << _T("MemoryClear");

			expResult = "{\"Type\":\"none\",\"Checksum\":\"\",\"Percent\":0,\"LastReceivedTime\":\"\",\"LastReceivedFile\":\"\",\"PendingAction\":\"NO_PENDING_ACTION\",\"Status\":\"\"}";
			result = varSentinelState.GetSubscriptionDataResponse("DownloadState", "");
			ASSERT_EQ(result, expResult) << _T("DownloadState");

			expResult = "{\"InProgress\":false,\"Status\":\"Okay\",\"Percentage\":0,\"Message\":\"Capturing machine meters\",\"Warning\":\"This machine will not accept cards during this process. Please do not reboot or play until all meters are captured.\"}";
			result = varSentinelState.GetSubscriptionDataResponse("MeterCapture", "");
			ASSERT_EQ(result, expResult) << _T("MeterCapture");

			expResult = "{\"SkinName\":\"\",\"SkinRevision\":0,\"CampaignName\":\"\",\"CampaignRevision\":0}";
			result = varSentinelState.GetSubscriptionDataResponse("GraphicCardContent", "");
			ASSERT_EQ(result, expResult) << _T("GraphicCardContent");
		}

		void SentinelState_PendingAction()
		{
			// Setup.
			CSentinelState varSentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)), _T("1.12.2.0"));

			varSentinelState.SetPendingAction(NonePending);
			string result = varSentinelState.GetPendingActionAsString();
			string expResult = "NO_PENDING_ACTION";
			ASSERT_EQ(result, expResult) << _T("NonePending");

			varSentinelState.SetPendingAction(RestartUI);
			result = varSentinelState.GetPendingActionAsString();
			expResult = "RESTART_UI";
			ASSERT_EQ(result, expResult) << _T("RestartUI");

			varSentinelState.SetPendingAction(RebootAfterDrop);
			result = varSentinelState.GetPendingActionAsString();
			expResult = "REBOOT_AFTER_DROP";
			ASSERT_EQ(result, expResult) << _T("RebootAfterDrop");

			varSentinelState.SetPendingAction(RebootAsap);
			result = varSentinelState.GetPendingActionAsString();
			expResult = "REBOOT_ASAP";
			ASSERT_EQ(result, expResult) << _T("RebootAsap");

			varSentinelState.SetPendingAction(OsUpgrade);
			result = varSentinelState.GetPendingActionAsString();
			expResult = "OS_UPGRADE";
			ASSERT_EQ(result, expResult) << _T("OsUpgrade");

			varSentinelState.SetPendingAction(RemoteReboot);
			result = varSentinelState.GetPendingActionAsString();
			expResult = "REMOTE_REBOOT";
			ASSERT_EQ(result, expResult) << _T("RemoteReboot");

			varSentinelState.SetPendingAction(RefreshUI);
			result = varSentinelState.GetPendingActionAsString();
			expResult = "REFRESH_UI";
			ASSERT_EQ(result, expResult) << _T("RefreshUI");
		}	
};

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::CSentinelState()
	///		 CSentinelState::SetProgressiveHost()
	///		 CSentinelState::GetProgressiveHostCopy()
	/// Construct the source CProgressiveHost instance using a hostName and hostPort.
	/// </summary>
	TEST_F(SentinelState, SetProgressiveHost_HostNameTest)
	{
		typedef struct {
			LPCTSTR const m_ipAddressString;
			int m_port;
		} TIPAddressStringPort;

		typedef struct
		{
			// Input(s).
			LPCTSTR const m_hostName;
			const DWORD m_hostPort;

			// Expected result(s).
			// IP addresses are calculated at runtime.
			const std::string m_nameExpected;
			const bool m_hasAddressExpected;
			const TIPAddressStringPort m_ipAddressExpected;
			const size_t m_ipAddressCountExpected;
			const DWORD m_hostPortExpected;
			const std::string m_urlExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			{ _T("invalid."), (DWORD)-1, "invalid.", false, { _T("0.0.0.0"), 0 }, 0, (DWORD)-1, "" },
			{ _T("localhost."), (DWORD)-1, "localhost.", true, { _T("127.0.0.1"), -1 }, 1, (DWORD)-1, "ssl://127.0.0.1:-1?wireFormat=stomp" },

			{ _T("invalid."), 0, "invalid.", false, { _T("0.0.0.0"), 0 }, 0, 0, "" },
			{ _T("localhost."), 0, "localhost.", true, { _T("127.0.0.1"), 0 }, 1, 0, "ssl://127.0.0.1:0?wireFormat=stomp" },

			{ _T("invalid."), 1, "invalid.", false, { _T("0.0.0.0"), 0 }, 0, 1, "" },
			{ _T("localhost."), 1, "localhost.", true, { _T("127.0.0.1"), 1 }, 1, 1, "ssl://127.0.0.1:1?wireFormat=stomp" },

			{ _T("invalid."), WORD_MAX, "invalid.", false, { _T("0.0.0.0"), 0 }, 0, WORD_MAX, "" },
			{ _T("localhost."), WORD_MAX, "localhost.", true, { _T("127.0.0.1"), WORD_MAX }, 1, WORD_MAX, "ssl://127.0.0.1:65535?wireFormat=stomp" },

			{ _T("invalid."), WORD_MAX + 1, "invalid.", false, { _T("0.0.0.0"), 0 }, 0, WORD_MAX + 1, "" },
			{ _T("localhost."), WORD_MAX + 1, "localhost.", true, { _T("127.0.0.1"), WORD_MAX + 1 }, 1, WORD_MAX + 1, "ssl://127.0.0.1:65536?wireFormat=stomp" },

			{ _T("invalid."), INT_MAX, "invalid.", false, { _T("0.0.0.0"), 0 }, 0, INT_MAX, "" },
			{ _T("localhost."), INT_MAX, "localhost.", true, { _T("127.0.0.1"), INT_MAX }, 1, INT_MAX, "ssl://127.0.0.1:2147483647?wireFormat=stomp" },

			{ _T("invalid."), INT_MAX, "invalid.", false, { _T("0.0.0.0"), 0 }, 0, INT_MAX, "" },
			{ _T("localhost."), (DWORD)INT_MIN, "localhost.", true, { _T("127.0.0.1"), INT_MIN }, 1, (DWORD)INT_MIN, "ssl://127.0.0.1:-2147483648?wireFormat=stomp" },

			{ _T("invalid."), DWORD_MAX, "invalid.", false, { _T("0.0.0.0"), 0 }, 0, DWORD_MAX, "" },
			{ _T("localhost."), DWORD_MAX, "localhost.", true, { _T("127.0.0.1"), (int)DWORD_MAX }, 1, DWORD_MAX, "ssl://127.0.0.1:-1?wireFormat=stomp" },
		};

		// Setup. Perform each test with the same instance.
		CSentinelState varSentinelStateA(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Setup. Calculate ipAddressExpected.
			CIPAddress ipAddressExpected;
			ipAddressExpected.SetIP(TEST_DATA[testIndex].m_ipAddressExpected.m_ipAddressString);
			ipAddressExpected.SetPort(TEST_DATA[testIndex].m_ipAddressExpected.m_port);

			// Setup. Construct the source CProgressiveHost instance.
			const CProgressiveHost progHost(TEST_DATA[testIndex].m_hostName, TEST_DATA[testIndex].m_hostPort);

			// Setup. Perform each test with a new object.
			CSentinelState varSentinelStateB(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
				_T("1.12.2.0"));

			// Perform operation(s) to be tested.
			varSentinelStateA.SetProgressiveHost(progHost);
			const CSentinelState& sentinelStateA(varSentinelStateA);
			const CProgressiveHost* const objA(sentinelStateA.GetProgressiveHostCopy());

			varSentinelStateB.SetProgressiveHost(progHost);
			const CSentinelState& sentinelStateB(varSentinelStateB);
			const CProgressiveHost* const objB(sentinelStateB.GetProgressiveHostCopy());

			// Test(s).
			AssertEqual(*objA,
				TEST_DATA[testIndex].m_nameExpected,
				TEST_DATA[testIndex].m_hasAddressExpected,
				ipAddressExpected,
				m_progHost_currentAddressIndexExpected,
				&ipAddressExpected,
				TEST_DATA[testIndex].m_ipAddressCountExpected,
				TEST_DATA[testIndex].m_hostPortExpected,
				TEST_DATA[testIndex].m_urlExpected,
				testIndex, __TFUNCTION__, __LINE__);
			AssertEqual(*objA,
				TEST_DATA[testIndex].m_nameExpected,
				TEST_DATA[testIndex].m_hasAddressExpected,
				ipAddressExpected,
				m_progHost_currentAddressIndexExpected,
				&ipAddressExpected,
				TEST_DATA[testIndex].m_ipAddressCountExpected,
				TEST_DATA[testIndex].m_hostPortExpected,
				TEST_DATA[testIndex].m_urlExpected,
				testIndex, __TFUNCTION__, __LINE__);

			// Cleanup.
			delete objA;
			delete objB;
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::CSentinelState()
	///		 CSentinelState::SetProgressiveHost()
	///		 CSentinelState::GetProgressiveHostCopy()
	/// Use the same IP address and port for each element of the IP address array so the elements are in a known "order" even after they have been randomized.
	/// </summary>
	TEST_F(SentinelState, SetProgressiveHost_OrderedTest)
	{
		typedef struct {
			LPCTSTR const m_ipAddressString;
			int m_port;
		} TIPAddressStringPort;

		typedef struct
		{
			// Input(s).
			const TIPAddressStringPort* const m_ipAddressStringPort;
			const int m_ipAddressCount;

			// Expected result(s).
			// IP addresses are calculated at runtime.
			const bool m_hasAddressExpected;
			const std::string m_urlExpected;
		} TestDataType;

		TIPAddressStringPort addr1[] = {
			{ _T("1.2.3.4"), 12345 },
		};

		TIPAddressStringPort addr2[] = {
			{ _T("1.2.3.4"), 12345 },
			{ _T("1.2.3.4"), 12345 },
			{ _T("1.2.3.4"), 12345 },
		};

		TIPAddressStringPort addr3[] = {
			{ _T("1.2.3.4"), 12345 },
			{ _T("1.2.3.4"), 12345 },
			{ _T("1.2.3.4"), 12345 },
			{ _T("1.2.3.4"), 12345 },
			{ _T("1.2.3.4"), 12345 },
		};

		const TestDataType TEST_DATA[] =
		{
			{ NULL, 0, false, "" },
			{ addr1, _countof(addr1), true, "ssl://1.2.3.4:12345?wireFormat=stomp" },
			{ addr2, _countof(addr2), true, "ssl://1.2.3.4:12345?wireFormat=stomp" },
			{ addr3, _countof(addr3), true, "ssl://1.2.3.4:12345?wireFormat=stomp" },
		};

		// Setup. Perform each test with the same instance.
		CSentinelState varSentinelStateA(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			CIPAddress* const ipAddressesExpected(new CIPAddress[TEST_DATA[testIndex].m_ipAddressCount]);
			CIPAddressArray ipAddressArray;
			for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
			{
				ipAddressesExpected[index].SetIP(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString);
				ipAddressesExpected[index].SetPort(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
				ipAddressArray.Add(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString,
					TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
			}
			const CProgressiveHost progHost(ipAddressArray);

			const CIPAddress &ipAddressExpected(
				TEST_DATA[testIndex].m_ipAddressCount > 0 ? ipAddressesExpected[0] : m_defaultIPAddress);

			// Setup. Perform each test with a new object.
			CSentinelState varSentinelStateB(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
				_T("1.12.2.0"));

			// Perform operation(s) to be tested.
			varSentinelStateA.SetProgressiveHost(progHost);
			const CSentinelState& sentinelStateA(varSentinelStateA);
			const CProgressiveHost* const objA(sentinelStateA.GetProgressiveHostCopy());

			varSentinelStateB.SetProgressiveHost(progHost);
			const CSentinelState& sentinelStateB(varSentinelStateB);
			const CProgressiveHost* const objB(sentinelStateB.GetProgressiveHostCopy());

			// Test(s).
			AssertEqual(*objA,
				m_progHost_nameExpected,
				TEST_DATA[testIndex].m_hasAddressExpected,
				ipAddressExpected,
				m_progHost_currentAddressIndexExpected,
				ipAddressesExpected,
				TEST_DATA[testIndex].m_ipAddressCount,
				m_progHost_hostPortExpected,
				TEST_DATA[testIndex].m_urlExpected,
				testIndex, __TFUNCTION__, __LINE__);
			AssertEqual(*objB,
				m_progHost_nameExpected,
				TEST_DATA[testIndex].m_hasAddressExpected,
				ipAddressExpected,
				m_progHost_currentAddressIndexExpected,
				ipAddressesExpected,
				TEST_DATA[testIndex].m_ipAddressCount,
				m_progHost_hostPortExpected,
				TEST_DATA[testIndex].m_urlExpected,
				testIndex, __TFUNCTION__, __LINE__);

			// Cleanup.
			delete[] ipAddressesExpected;
			delete objA;
			delete objB;
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::CSentinelState()
	///		 CSentinelState::SetProgressiveHost()
	///		 CSentinelState::GetProgressiveHostCopy()
	/// Use a CProgressiveHost instance with a randomized list of IP addresses.
	/// </summary>
	TEST_F(SentinelState, SetProgressiveHost_RandomTest)
	{
		typedef struct {
			LPCTSTR const m_ipAddressString;
			int m_port;
		} TIPAddressStringPort;

		typedef struct
		{
			// Input(s).
			const TIPAddressStringPort* const m_ipAddressStringPort;
			const int m_ipAddressCount;

			// Expected result(s).
			// IP addresses and URL are calculated at runtime.
			const bool m_hasAddressExpected;
		} TestDataType;

		TIPAddressStringPort addr1[] = {
			{ _T("1.2.3.40"), 12340 },
		};

		TIPAddressStringPort addr2[] = {
			{ _T("1.2.3.41"), 12341 },
			{ _T("1.2.3.42"), 12342 },
			{ _T("1.2.3.43"), 12343 },
		};

		TIPAddressStringPort addr3[] = {
			{ _T("1.2.3.44"), 12344 },
			{ _T("1.2.3.45"), 12345 },
			{ _T("1.2.3.46"), 12346 },
			{ _T("1.2.3.47"), 12347 },
			{ _T("1.2.3.48"), 12348 },
		};

		TIPAddressStringPort addr4[] = {
			{ _T("1.2.3.44"), 12344 },
			{ _T("0.0.0.0"), 0 },
			{ _T("1.2.3.45"), 12345 },
			{ _T("0.0.0.0"), 0 },
			{ _T("0.0.0.0"), 0 },
			{ _T("1.2.3.46"), 12346 },
			{ _T("123.234.56.78"), 0x12345678 },
			{ _T("123.234.56.78"), 0x12345678 },
			{ _T("1.2.3.47"), 12347 },
			{ _T("255.255.255.255"), 1234567890 },
			{ _T("255.255.255.255"), 1234567890 },
			{ _T("1.2.3.48"), 12348 },
		};

		const TestDataType TEST_DATA[] =
		{
			{ NULL, 0, false },
			{ addr1, _countof(addr1), true },
			{ addr2, _countof(addr2), true },
			{ addr3, _countof(addr3), true },
			{ addr4, _countof(addr4), true },
		};

		// Setup. Perform each test with the same instance.
		CSentinelState varSentinelStateA(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			CIPAddress* const ipAddressesExpected(new CIPAddress[TEST_DATA[testIndex].m_ipAddressCount]);
			CIPAddressArray ipAddressArray;
			for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
			{
				ipAddressesExpected[index].SetIP(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString);
				ipAddressesExpected[index].SetPort(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
				ipAddressArray.Add(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString,
					TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
			}
			const CProgressiveHost progHost(ipAddressArray);

			// Reorder ipAddressesExpected as needed.
			// Reorder our trusted list, instead of overwriting our list with elements from an instance under test.
			bool reorderOkay(true);
			ASSERT_TRUE(progHost.GetIPAddressCount() == TEST_DATA[testIndex].m_ipAddressCount);
			for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
			{
				CIPAddress hostIPAddress = progHost.GetIPAddress(index);
				if (ipAddressesExpected[index] != hostIPAddress)
				{
					reorderOkay = false;
					for (int swapIndex(index + 1); swapIndex < TEST_DATA[testIndex].m_ipAddressCount; ++swapIndex)
					{
						if (ipAddressesExpected[swapIndex] == hostIPAddress)
						{
							// Found an element to swap.
							CIPAddress temp = ipAddressesExpected[index];
							ipAddressesExpected[index] = ipAddressesExpected[swapIndex];
							ipAddressesExpected[swapIndex] = temp;
							reorderOkay = true;
						}
					}
				}
			}
			ASSERT_TRUE(reorderOkay == true);

			const CIPAddress &ipAddressExpected(
				TEST_DATA[testIndex].m_ipAddressCount > 0 ? ipAddressesExpected[0] : m_defaultIPAddress);

			std::ostringstream stringStream;
			if (TEST_DATA[testIndex].m_ipAddressCount > 0)
			{
				BYTE address[4];
				ipAddressExpected.GetIP(address, _countof(address));
				stringStream << "ssl://" <<
					(unsigned)address[0] << '.' << (unsigned)address[1] << '.' << (unsigned)address[2] << '.' << (unsigned)address[3] <<
					':' << ipAddressExpected.GetPort() << "?wireFormat=stomp";
			}
			const std::string urlExpected(stringStream.str());

			// Setup. Perform each test with a new object.
			CSentinelState varSentinelStateB(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
				_T("1.12.2.0"));

			// Perform operation(s) to be tested.
			varSentinelStateA.SetProgressiveHost(progHost);
			const CSentinelState& sentinelStateA(varSentinelStateA);
			const CProgressiveHost* const objA(sentinelStateA.GetProgressiveHostCopy());

			varSentinelStateB.SetProgressiveHost(progHost);
			const CSentinelState& sentinelStateB(varSentinelStateB);
			const CProgressiveHost* const objB(sentinelStateB.GetProgressiveHostCopy());

			// Test(s).
			AssertEqual(*objA,
				m_progHost_nameExpected,
				TEST_DATA[testIndex].m_hasAddressExpected,
				ipAddressExpected,
				m_progHost_currentAddressIndexExpected,
				ipAddressesExpected,
				TEST_DATA[testIndex].m_ipAddressCount,
				m_progHost_hostPortExpected,
				urlExpected,
				testIndex, __TFUNCTION__, __LINE__);
			AssertEqual(*objB,
				m_progHost_nameExpected,
				TEST_DATA[testIndex].m_hasAddressExpected,
				ipAddressExpected,
				m_progHost_currentAddressIndexExpected,
				ipAddressesExpected,
				TEST_DATA[testIndex].m_ipAddressCount,
				m_progHost_hostPortExpected,
				urlExpected,
				testIndex, __TFUNCTION__, __LINE__);

			// Cleanup.
			delete[] ipAddressesExpected;
			delete objA;
			delete objB;
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::CSentinelState()
	///		 CSentinelState::GetReportedEgmProtocolVersion()
	/// Test default value.
	/// </summary>
	TEST_F(SentinelState, GetReportedEgmProtocolVersion_DefaultTest)
	{
		// Data.
		const string expected;

		// Perform operation(s) and test(s).
		const CSentinelState sentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));
		ASSERT_TRUE(expected == sentinelState.GetReportedEgmProtocolVersion()) << L"GetReportedEgmProtocolVersion";
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::SetReportedEgmProtocolVersion()
	///		 CSentinelState::GetReportedEgmProtocolVersion()
	/// Test setting a non-empty value and then an empty value.
	/// </summary>
	TEST_F(SentinelState, GetReportedEgmProtocolVersion_NonEmpty_EmptyTest)
	{
		// Data.
		const string expected1(_T("123"));
		const string expected2;

		// Setup.
		CSentinelState sentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)), _T("1.12.2.0"));

		// Perform operation(s) and test(s).
		sentinelState.SetReportedEgmProtocolVersion(expected1);
		ASSERT_TRUE(expected1 == sentinelState.GetReportedEgmProtocolVersion()) << L"GetReportedEgmProtocolVersion1";

		sentinelState.SetReportedEgmProtocolVersion(expected2);
		ASSERT_TRUE(expected2 == sentinelState.GetReportedEgmProtocolVersion()) << L"GetReportedEgmProtocolVersion2";
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::SetReportedEgmProtocolVersion()
	///		 CSentinelState::GetReportedEgmProtocolVersion()
	/// Test setting a large value.
	/// </summary>
	TEST_F(SentinelState, GetReportedEgmProtocolVersion_LargeTest)
	{
		// Data.
		const string expected(
			_T("12345678901234567890123456789012345678901234567890") _T("12345678901234567890123456789012345678901234567890")
			_T("12345678901234567890123456789012345678901234567890") _T("12345678901234567890123456789012345678901234567890")
			_T("12345678901234567890123456789012345678901234567890") _T("12345678901234567890123456789012345678901234567890")
			);

		// Setup.
		CSentinelState sentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)), _T("1.12.2.0"));

		// Perform operation(s) and test(s).
		sentinelState.SetReportedEgmProtocolVersion(expected);
		ASSERT_TRUE(expected == sentinelState.GetReportedEgmProtocolVersion()) << L"GetReportedEgmProtocolVersion";
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::CSentinelState()
	///		 CSentinelState::GetEgmSupportsLP7B_Available()
	/// Test default value.
	/// </summary>
	TEST_F(SentinelState, GetEgmSupportsLP7B_Available_DefaultTest)
	{
		// Data.
		const bool expected(false);

		// Perform operation(s) and test(s).
		const CSentinelState sentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));
		ASSERT_TRUE(expected == sentinelState.GetEgmSupportsLP7B_Available()) << L"GetEgmSupportsLP7B_Available";
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::SetEgmSupportsLP7B_Available()
	///		 CSentinelState::GetEgmSupportsLP7B_Available()
	/// Test setting value to false.
	/// </summary>
	TEST_F(SentinelState, GetEgmSupportsLP7B_Available_FalseTest)
	{
		// Data.
		const bool expected(false);

		// Setup.
		CSentinelState sentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)), _T("1.12.2.0"));

		// Perform operation(s) and test(s).
		sentinelState.SetEgmSupportsLP7B_Available(expected);
		ASSERT_TRUE(expected == sentinelState.GetEgmSupportsLP7B_Available()) << L"GetEgmSupportsLP7B_Available";
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::SetEgmSupportsLP7B_Available()
	///		 CSentinelState::GetEgmSupportsLP7B_Available()
	/// Test setting value to true.
	/// </summary>
	TEST_F(SentinelState, GetEgmSupportsLP7B_Available_TrueTest)
	{
		// Data.
		const bool expected(true);

		// Setup.
		CSentinelState sentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)), _T("1.12.2.0"));

		// Perform operation(s) and test(s).
		sentinelState.SetEgmSupportsLP7B_Available(expected);
		ASSERT_TRUE(expected == sentinelState.GetEgmSupportsLP7B_Available()) << L"GetEgmSupportsLP7B_Available";
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::CSentinelState()
	///		 CSentinelState::GetEgmSupportsLP7B()
	/// Test default value.
	/// </summary>
	TEST_F(SentinelState, GetEgmSupportsLP7B_DefaultTest)
	{
		// Data.
		const bool expected(false);

		// Perform operation(s) and test(s).
		const CSentinelState sentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));
		ASSERT_TRUE(expected == sentinelState.GetEgmSupportsLP7B()) << L"GetEgmSupportsLP7B";
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::SetEgmSupportsLP7B()
	///		 CSentinelState::GetEgmSupportsLP7B()
	/// Test setting value to false.
	/// </summary>
	TEST_F(SentinelState, GetEgmSupportsLP7B_FalseTest)
	{
		// Data.
		const bool expected(false);

		// Setup.
		CSentinelState sentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)), _T("1.12.2.0"));

		// Perform operation(s) and test(s).
		sentinelState.SetEgmSupportsLP7B(expected);
		ASSERT_TRUE(expected == sentinelState.GetEgmSupportsLP7B()) << L"GetEgmSupportsLP7B";
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::SetEgmSupportsLP7B()
	///		 CSentinelState::GetEgmSupportsLP7B()
	/// Test setting value to true.
	/// </summary>
	TEST_F(SentinelState, GetEgmSupportsLP7B_TrueTest)
	{
		// Data.
		const bool expected(true);

		// Setup.
		CSentinelState sentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)), _T("1.12.2.0"));

		// Perform operation(s) and test(s).
		sentinelState.SetEgmSupportsLP7B(expected);
		ASSERT_TRUE(expected == sentinelState.GetEgmSupportsLP7B()) << L"GetEgmSupportsLP7B";
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::CSentinelState()
	/// Code coverage for:
	///     CSentinelState::~CSentinelState()
	/// </summary>
	TEST_F(SentinelState, CSentinelStateTest)
	{
		// Setup.
		const int testIndex(0);

		// Perform operation(s) to be tested.
		const CSentinelState sentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));

		// Test(s).
		ASSERT_TRUE(sentinelState.m_startupTime == time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12))) << _T("m_startupTime");
		ASSERT_TRUE(sentinelState.m_sentinelVersion == _T("1.12.2.0")) << _T("m_sentinelVersion");
		ASSERT_TRUE(sentinelState.m_availablePhysicalMemory == (DWORD)-1) << _T("m_availablePhysicalMemory");

		ASSERT_TRUE(sentinelState.m_tickCountOfLastFloorLogixTimeUpdateAvailable == false) << _T("m_tickCountOfLastFloorLogixTimeUpdateAvailable");
		ASSERT_TRUE(sentinelState.m_tickCountOfLastFloorLogixTimeUpdate == 0) << _T("m_tickCountOfLastFloorLogixTimeUpdate");
		ASSERT_TRUE(sentinelState.m_timeOfLastFloorLogixConfigAvailable == false) << _T("m_timeOfLastFloorLogixConfigAvailable");
		ASSERT_TRUE(sentinelState.m_timeOfLastFloorLogixConfig == 0) << _T("m_timeOfLastFloorLogixConfig");

		ASSERT_TRUE(sentinelState.m_lastUIRestartTime == 0) << _T("m_lastUIRestartTime");
		ASSERT_TRUE(sentinelState.m_uiRestartCount == 0) << _T("m_uiRestartCount");

		ASSERT_TRUE(sentinelState.m_lastUIRefreshTime == 0) << _T("m_lastUIRefreshTime");
		ASSERT_TRUE(sentinelState.m_uiRefreshCount == 0) << _T("m_uiRefreshCount");

		ASSERT_TRUE(sentinelState.m_rebootTilt == tltRebootUnknown) << _T("m_rebootTilt");
		ASSERT_TRUE(sentinelState.m_pollerPacketSentVerifyErrors == 0) << _T("m_pollerPacketSentVerifyErrors");
		ASSERT_EQ((WORD)0, sentinelState.m_pollerPacketVersion) << _T("m_pollerPacketVersion");
		ASSERT_FALSE(sentinelState.m_oneLinkProgressivesSenOptionSupported) << _T("m_oneLinkProgressivesSenOptionSupported");
	}

	void SentinelState::AssertEqual(const CProgressiveConfig &obj,
		bool hasConfigFileExpected,
		bool activeExpected,
		bool dnsLookupExpected,
		DWORD pingIntervalExpected,
		const string endpointsExpected[],
		int endpointCountExpected,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex)
	{
		ASSERT_EQ(obj.HasConfigFile(), hasConfigFileExpected);
		ASSERT_EQ(obj.GetActive(), activeExpected);
		ASSERT_EQ(obj.GetDNSLookup(), dnsLookupExpected);
		ASSERT_EQ(obj.GetPingInterval(), pingIntervalExpected);

		ASSERT_EQ(obj.GetEndpointCount(), endpointCountExpected);
		string emptyString;
		ASSERT_EQ(obj.GetEndpoint(endpointCountExpected), emptyString);

		for (int index = 0; index < endpointCountExpected; ++index)
		{
			string resultName;
			resultName = FormatString(_T("obj.GetEndpoint(index=%d)"), index);
			ASSERT_EQ(obj.GetEndpoint(index), endpointsExpected[index]) << resultName;
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::CSentinelState()
	///		 CSentinelState::GetProgressiveConfigCopy()
	/// </summary>
	TEST_F(SentinelState, CSentinelState_ProgressiveConfigDefaultTest)
	{
		// Setup.
		const int testIndex(0);
		const CSentinelState sentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));

		// Perform operation(s) to be tested.
		const CProgressiveConfig* const obj(sentinelState.GetProgressiveConfigCopy());

		// Test(s).
		AssertEqual(*obj,
			m_progConfig_hasConfigFileExpected,
			m_progConfig_activeExpected,
			m_progConfig_dnsLookupExpected,
			m_progConfig_pingIntervalExpected,
			m_progConfig_endpointsExpected,
			m_progConfig_endpointCountExpected,
			testIndex, __TFUNCTION__, __LINE__);

		// Cleanup.
		delete obj;
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::CSentinelState()
	///		 CSentinelState::SetProgressiveConfig()
	///		 CSentinelState::GetProgressiveConfigCopy()
	/// Use a default CProgressiveConfig instance.
	/// </summary>
	TEST_F(SentinelState, SetProgressiveConfig_DefaultTest)
	{
		// Setup.
		const int testIndex(0);
		CSentinelState varSentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));
		const CProgressiveConfig progConfig;

		// Verify setup.
		AssertEqual(progConfig,
			m_progConfig_hasConfigFileExpected,
			m_progConfig_activeExpected,
			m_progConfig_dnsLookupExpected,
			m_progConfig_pingIntervalExpected,
			m_progConfig_endpointsExpected,
			m_progConfig_endpointCountExpected,
			testIndex, __TFUNCTION__, __LINE__);

		// Perform operation(s) to be tested.
		varSentinelState.SetProgressiveConfig(progConfig);
		const CSentinelState& sentinelState(varSentinelState);
		const CProgressiveConfig* const obj(sentinelState.GetProgressiveConfigCopy());

		// Test(s).
		AssertEqual(*obj,
			m_progConfig_hasConfigFileExpected,
			m_progConfig_activeExpected,
			m_progConfig_dnsLookupExpected,
			m_progConfig_pingIntervalExpected,
			m_progConfig_endpointsExpected,
			m_progConfig_endpointCountExpected,
			testIndex, __TFUNCTION__, __LINE__);

		// Cleanup.
		delete obj;
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::CSentinelState()
	///		 CSentinelState::SetProgressiveConfig()
	///		 CSentinelState::GetProgressiveConfigCopy()
	/// </summary>
	TEST_F(SentinelState, SetProgressiveConfigTest)
	{
		// Input(s).
		LPCTSTR endpoint1(_T("abcde.xyz:12345"));
		LPCTSTR endpoint2(_T("123.234.56.78:23456"));

		typedef struct
		{
			// Input(s).
			const bool m_hasConfigFile;
			const bool m_active;
			const bool m_dnsLookup;
			const DWORD m_pingInterval;
			const string* const m_endpoints;
			const int m_endpointCount;

			// Expected result(s).
			const bool m_hasConfigFileExpected;
			const bool m_activeExpected;
			const bool m_dnsLookupExpected;
			const DWORD m_pingIntervalExpected;
			const string* const m_endpointsExpected;
			const int m_endpointCountExpected;
		} TestDataType;

		string endpoints1[] = {
			_T("abcde.xyz:12345"),
		};
		string endpoints2[] = {
			_T("abcde.xyz:12345"),
			_T("   abcdefghijk.lmnop.xyz:1234567890   "),
		};
		string endpoints3[] = {
			_T("abcde.xyz:12345"),
			_T("abcdefghijk.lmnop.xyz:1234567890"),
			_T(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 "),
		};

		const TestDataType TEST_DATA[] =
		{
			{ false, false, false, 0, NULL, 0,
			false, false, false, 15000, m_progConfig_endpointsExpected, m_progConfig_endpointCountExpected,
			},

			{ false, false, true, 6000, endpoints1, _countof(endpoints1),
			false, false, true, 6000, endpoints1, _countof(endpoints1),
			},

			{ false, true, false, 1234567890, endpoints2, _countof(endpoints2),
			false, true, false, 1234567890, endpoints2, _countof(endpoints2),
			},

			{ false, true, true, 4294967295, endpoints3, _countof(endpoints3),
			false, true, true, 4294967295, endpoints3, _countof(endpoints3),
			},

			{ true, false, false, 0, NULL, 0,
			true, false, false, 15000, m_progConfig_endpointsExpected, m_progConfig_endpointCountExpected,
			},

			{ true, false, true, 6000, endpoints1, _countof(endpoints1),
			true, false, true, 6000, endpoints1, _countof(endpoints1),
			},

			{ true, true, false, 1234567890, endpoints2, _countof(endpoints2),
			true, true, false, 1234567890, endpoints2, _countof(endpoints2),
			},

			{ true, true, true, 4294967295, endpoints3, _countof(endpoints3),
			true, true, true, 4294967295, endpoints3, _countof(endpoints3),
			},
		};

		// Setup. Perform each test with the same instance.
		CSentinelState varSentinelStateA(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));

		for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Setup. Perform each test with a new object.
			CSentinelState varSentinelStateB(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
				_T("1.12.2.0"));

			// Setup.
			CProgressiveConfig varProgConfig;
			varProgConfig.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
			varProgConfig.SetActive(TEST_DATA[testIndex].m_active);
			varProgConfig.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
			varProgConfig.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
			for (int index(0); index < TEST_DATA[testIndex].m_endpointCount; ++index)
			{
				varProgConfig.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
			}
			const CProgressiveConfig& progConfig(varProgConfig);

			// Perform operation(s) to be tested.
			varSentinelStateA.SetProgressiveConfig(progConfig);
			const CSentinelState& sentinelStateA(varSentinelStateA);
			const CProgressiveConfig* const objA(sentinelStateA.GetProgressiveConfigCopy());

			varSentinelStateB.SetProgressiveConfig(progConfig);
			const CSentinelState& sentinelStateB(varSentinelStateB);
			const CProgressiveConfig* const objB(sentinelStateB.GetProgressiveConfigCopy());

			// Test(s).
			AssertEqual(*objA,
				TEST_DATA[testIndex].m_hasConfigFileExpected,
				TEST_DATA[testIndex].m_activeExpected,
				TEST_DATA[testIndex].m_dnsLookupExpected,
				TEST_DATA[testIndex].m_pingIntervalExpected,
				TEST_DATA[testIndex].m_endpointsExpected,
				TEST_DATA[testIndex].m_endpointCountExpected,
				testIndex, __TFUNCTION__, __LINE__);
			AssertEqual(*objB,
				TEST_DATA[testIndex].m_hasConfigFileExpected,
				TEST_DATA[testIndex].m_activeExpected,
				TEST_DATA[testIndex].m_dnsLookupExpected,
				TEST_DATA[testIndex].m_pingIntervalExpected,
				TEST_DATA[testIndex].m_endpointsExpected,
				TEST_DATA[testIndex].m_endpointCountExpected,
				testIndex, __TFUNCTION__, __LINE__);

			// Cleanup.
			delete objA;
			delete objB;
		}
	}

	void SentinelState::AssertEqual(const CProgressiveConnectionInfo &obj,
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
		const int* extraIndex)
	{
		// Calculate the expected timestamp string(s) at runtime, because the result depends on the time zone.
		// If the time zone changes during a test then the test could fail.
		// Use time_t and CUtilities specific-behaviors to calculate the expected timestamp string(s), because the range handling is undefined.
		string lastMessageTimestampStringExpected(_T("N/A"));
		if ((lastMessageTimestampExpected.wYear != 0 || lastMessageTimestampExpected.wMonth != 0) &&
			!CUtilities::Is1970Time(lastMessageTimestampExpected))
		{
			lastMessageTimestampStringExpected = CUnitTestingUtils::FormatLocalMilitaryDateTimeString(lastMessageTimestampExpected);
		}
		string lastConnectionTimestampStringExpected(_T("N/A"));
		if ((lastConnectionTimestampExpected.wYear != 0 || lastConnectionTimestampExpected.wMonth != 0) &&
			!CUtilities::Is1970Time(lastConnectionTimestampExpected))
		{
			lastConnectionTimestampStringExpected = CUnitTestingUtils::FormatLocalMilitaryDateTimeString(lastConnectionTimestampExpected);
		}

		ASSERT_EQ(obj.GetLastMessageIP(), lastMessageIPExpected);
		CUnitTestingUtils::AssertEqual(obj.GetLastMessageTimestamp(), lastMessageTimestampExpected, _T("obj.GetLastMessageTimestamp()"), 0, __TFUNCTION__, __LINE__, nullptr);
		ASSERT_EQ(obj.GetLastMessageTimestampString(), lastMessageTimestampStringExpected);
		CUnitTestingUtils::AssertEqual(obj.GetLastConnectAttemptTimestamp(), lastConnectAttemptTimestampExpected, _T("obj.GetLastConnectAttemptTimestamp()"), 0, __TFUNCTION__, __LINE__, nullptr);
		CUnitTestingUtils::AssertEqual(obj.GetLastConnectTimestamp(), lastConnectionTimestampExpected, _T("obj.GetLastConnectTimestamp()"), 0, __TFUNCTION__, __LINE__, nullptr);
		ASSERT_EQ(obj.GetLastConnectTimestampString(), lastConnectionTimestampStringExpected);
		ASSERT_EQ(obj.GetState(), m_connectionStateExpected);
		ASSERT_EQ(obj.GetLastError(), lastConnectionErrorExpected);
		ASSERT_EQ(obj.GetConnectionStateString(), connectionStateStringExpected);
		ASSERT_EQ(obj.GetConnectAttemptCount(), connectAttemptCountExpected);
		CUnitTestingUtils::AssertEqual(obj.GetLastAddressUpdateTimestamp(), lastAddressUpdateTimestampExpected, _T("obj.GetLastAddressUpdateTimestamp()"), 0, __TFUNCTION__, __LINE__, nullptr);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::CSentinelState()
	///		 CSentinelState::GetProgressiveConnectionInfoCopy()
	/// </summary>
	TEST_F(SentinelState, CSentinelState_ProgressiveConnectionInfoDefaultTest)
	{
		// Setup.
		const int testIndex(0);
		const CSentinelState sentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));

		// Perform operation(s) to be tested.
		const CProgressiveConnectionInfo* const obj(sentinelState.GetProgressiveConnectionInfoCopy());

		// Test(s).
		AssertEqual(*obj,
			m_progInfo_lastMessageIPExpected,
			m_progInfo_lastMessageTimestampExpected,
			m_progInfo_lastConnectAttemptTimestampExpected,
			m_progInfo_lastConnectionTimestampExpected,
			m_progInfo_connectionStateExpected,
			m_progInfo_lastConnectionErrorExpected,
			m_progInfo_connectionStateStringExpected,
			m_progInfo_connectAttemptCountExpected,
			m_progInfo_lastAddressUpdateTimestampExpected,
			testIndex, __TFUNCTION__, __LINE__);

		// Cleanup.
		delete obj;
	}

	/// <summary>
	/// Code coverage and functional test for:
	///		 CSentinelState::SetProgressiveConnectionInfo()
	///		 CSentinelState::GetProgressiveConnectionInfoCopy()
	/// Use a default CProgressiveConnectionInfo instance.
	/// </summary>
	TEST_F(SentinelState, SetProgressiveConnectionInfo_DefaultTest)
	{
		// Setup.
		const int testIndex(0);
		CSentinelState varSentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));
		const CProgressiveConnectionInfo progInfo;

		// Verify setup.
		AssertEqual(progInfo,
			m_progInfo_lastMessageIPExpected,
			m_progInfo_lastMessageTimestampExpected,
			m_progInfo_lastConnectAttemptTimestampExpected,
			m_progInfo_lastConnectionTimestampExpected,
			m_progInfo_connectionStateExpected,
			m_progInfo_lastConnectionErrorExpected,
			m_progInfo_connectionStateStringExpected,
			m_progInfo_connectAttemptCountExpected,
			m_progInfo_lastAddressUpdateTimestampExpected,
			testIndex, __TFUNCTION__, __LINE__);

		// Perform operation(s) to be tested.
		varSentinelState.SetProgressiveConnectionInfo(progInfo);
		const CSentinelState& sentinelState(varSentinelState);
		const CProgressiveConnectionInfo* const obj(sentinelState.GetProgressiveConnectionInfoCopy());

		// Test(s).
		AssertEqual(*obj,
			m_progInfo_lastMessageIPExpected,
			m_progInfo_lastMessageTimestampExpected,
			m_progInfo_lastConnectAttemptTimestampExpected,
			m_progInfo_lastConnectionTimestampExpected,
			m_progInfo_connectionStateExpected,
			m_progInfo_lastConnectionErrorExpected,
			m_progInfo_connectionStateStringExpected,
			m_progInfo_connectAttemptCountExpected,
			m_progInfo_lastAddressUpdateTimestampExpected,
			testIndex, __TFUNCTION__, __LINE__);

		// Cleanup.
		delete obj;
	}

	/// <summary>
	/// Code coverage and functional test for:
	///		 CSentinelState::SetProgressiveConnectionInfo()
	///		 CSentinelState::GetProgressiveConnectionInfoCopy()
	/// </summary>
	TEST_F(SentinelState, SetProgressiveConnectionInfoTest)
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
			{ { _T(""), -1 }, { 2011, 2, 0, 3, 14, 25, 0, 789 }, { 2011, 1, 2, 3, 4, 5, 0, 999 }, { 2012, 1, 2, 3, 4, 5, 0, 000 }, (ProgressiveConnectionState)-1, CouldNotFindServer, 0, { 2013, 1, 2, 3, 4, 55, 0, 555 },
			{ _T(""), -1 }, { 2011, 2, 0, 3, 14, 25, 0, 789 }, { 2011, 1, 2, 3, 4, 5, 0, 999 }, { 2012, 1, 2, 3, 4, 5, 0, 000 }, (ProgressiveConnectionState)-1, CouldNotFindServer, _T("N/A"), 0, { 2013, 1, 2, 3, 4, 55, 0, 555 }
			},
			{ { _T("*"), -999999 }, { 2011, 2, 0, 3, 14, 25, 1, 789 }, { 2011, 1, 2, 3, 4, 5, 1, 999 }, { 2012, 1, 2, 3, 4, 5, 1, 000 }, (ProgressiveConnectionState)(NotStarted - 1), CouldNotFindServer, 1, { 2013, 1, 2, 3, 4, 55, 1, 555 },
			{ _T("*"), -999999 }, { 2011, 2, 0, 3, 14, 25, 1, 789 }, { 2011, 1, 2, 3, 4, 5, 1, 999 }, { 2012, 1, 2, 3, 4, 5, 1, 000 }, (ProgressiveConnectionState)(NotStarted - 1), CouldNotFindServer, _T("N/A"), 1, { 2013, 1, 2, 3, 4, 55, 1, 555 }
			},
			{ { _T("n/a"), -1234567890 }, { 2011, 2, 0, 3, 14, 25, 2, 789 }, { 2011, 1, 2, 3, 4, 5, 2, 999 }, { 2012, 1, 2, 3, 4, 5, 2, 000 }, NotStarted, CouldNotFindServer, 2, { 2013, 1, 2, 3, 4, 55, 2, 555 },
			{ _T("n/a"), -1234567890 }, { 2011, 2, 0, 3, 14, 25, 2, 789 }, { 2011, 1, 2, 3, 4, 5, 2, 999 }, { 2012, 1, 2, 3, 4, 5, 2, 000 }, NotStarted, CouldNotFindServer, _T("Not Started"), 2, { 2013, 1, 2, 3, 4, 55, 2, 555 }
			},
			{ { _T(" !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 "), INT_MIN }, { 2011, 2, 0, 3, 14, 25, 3, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 3, 000 }, Disabled, CouldNotFindServer, 1234567890, { 2013, 1, 2, 3, 4, 55, 3, 555 },
			{ _T(" !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~:123456789012345678901234567890 "), INT_MIN }, { 2011, 2, 0, 3, 14, 25, 3, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 3, 000 }, Disabled, CouldNotFindServer, _T("Disabled"), 1234567890, { 2013, 1, 2, 3, 4, 55, 3, 555 }
			},
			{ { _T("abcdefghijklmnopqrstuvwxyz"), INT_MAX }, { 2011, 2, 0, 3, 14, 25, 4, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 4, 000 }, WaitingForInfo, CouldNotFindServer, INT_MAX - 1, { 2013, 1, 2, 3, 4, 55, 4, 555 },
			{ _T("abcdefghijklmnopqrstuvwxyz"), INT_MAX }, { 2011, 2, 0, 3, 14, 25, 4, 789 }, { 2011, 1, 2, 3, 4, 5, 4, 999 }, { 2012, 1, 2, 3, 4, 5, 4, 000 }, WaitingForInfo, CouldNotFindServer, _T("Waiting For Info"), INT_MAX - 1, { 2013, 1, 2, 3, 4, 55, 4, 555 }
			},
			{ { _T("0.0.0.0"), INT_MAX - 1 }, { 2011, 2, 0, 3, 14, 25, 5, 789 }, { 2011, 1, 2, 3, 4, 5, 5, 999 }, { 2012, 1, 2, 3, 4, 5, 5, 000 }, InDiscovery, CouldNotFindServer, INT_MAX, { 2013, 1, 2, 3, 4, 55, 5, 555 },
			{ _T("0.0.0.0"), INT_MAX - 1 }, { 2011, 2, 0, 3, 14, 25, 5, 789 }, { 2011, 1, 2, 3, 4, 5, 5, 999 }, { 2012, 1, 2, 3, 4, 5, 5, 000 }, InDiscovery, CouldNotFindServer, _T("In Discovery"), INT_MAX, { 2013, 1, 2, 3, 4, 55, 5, 555 }
			},
			{ { _T("255.255.255.255"), INT_MIN + 1 }, { 2011, 2, 0, 3, 14, 25, 6, 789 }, { 2011, 1, 2, 3, 4, 5, 6, 999 }, { 2012, 1, 2, 3, 4, 5, 6, 000 }, Connected, CouldNotFindServer, INT_MIN, { 2013, 1, 2, 3, 4, 55, 6, 555 },
			{ _T("255.255.255.255"), INT_MIN + 1 }, { 2011, 2, 0, 3, 14, 25, 6, 789 }, { 2011, 1, 2, 3, 4, 5, 6, 999 }, { 2012, 1, 2, 3, 4, 5, 6, 000 }, Connected, CouldNotFindServer, _T("Connected"), INT_MIN, { 2013, 1, 2, 3, 4, 55, 6, 555 }
			},
			{ { _T("1.2.3.107"), 20007 }, { 2011, 2, 0, 3, 14, 25, 7, 789 }, { 2011, 1, 2, 3, 4, 5, 7, 999 }, { 2012, 1, 2, 3, 4, 5, 7, 000 }, Disconnected, CouldNotFindServer, INT_MIN + 1, { 2013, 1, 2, 3, 4, 55, 7, 555 },
			{ _T("1.2.3.107"), 20007 }, { 2011, 2, 0, 3, 14, 25, 7, 789 }, { 2011, 1, 2, 3, 4, 5, 7, 999 }, { 2012, 1, 2, 3, 4, 5, 7, 000 }, Disconnected, CouldNotFindServer, _T("Disconnected"), INT_MIN + 1, { 2013, 1, 2, 3, 4, 55, 7, 555 }
			},

			{ { _T("1.2.3.108"), 20008 }, { 2011, 2, 0, 3, 14, 25, 8, 789 }, { 2011, 1, 2, 3, 4, 5, 8, 999 }, { 2012, 1, 2, 3, 4, 5, 8, 000 }, Error, (ProgressiveConnectionError)-1, -1234567890, { 2013, 1, 2, 3, 4, 55, 8, 555 },
			{ _T("1.2.3.108"), 20008 }, { 2011, 2, 0, 3, 14, 25, 8, 789 }, { 2011, 1, 2, 3, 4, 5, 8, 999 }, { 2012, 1, 2, 3, 4, 5, 8, 000 }, Error, (ProgressiveConnectionError)-1, _T("Error"), -1234567890, { 2013, 1, 2, 3, 4, 55, 8, 555 }
			},
			{ { _T("1.2.3.109"), 20009 }, { 2011, 2, 0, 3, 14, 25, 9, 789 }, { 2011, 1, 2, 3, 4, 5, 9, 999 }, { 2012, 1, 2, 3, 4, 5, 9, 000 }, Error, (ProgressiveConnectionError)(ProgressiveConnectionErrorNone - 1), (int)(DWORD_MAX - 1), { 2013, 1, 2, 3, 4, 55, 9, 555 },
			{ _T("1.2.3.109"), 20009 }, { 2011, 2, 0, 3, 14, 25, 9, 789 }, { 2011, 1, 2, 3, 4, 5, 9, 999 }, { 2012, 1, 2, 3, 4, 5, 9, 000 }, Error, (ProgressiveConnectionError)(ProgressiveConnectionErrorNone - 1), _T("Error"), (int)(DWORD_MAX - 1), { 2013, 1, 2, 3, 4, 55, 9, 555 }
			},
			{ { _T("1.2.3.110"), 20010 }, { 2011, 2, 0, 3, 14, 25, 10, 789 }, { 2011, 1, 2, 3, 4, 5, 10, 999 }, { 2012, 1, 2, 3, 4, 5, 10, 000 }, Error, ProgressiveConnectionErrorNone, (int)DWORD_MAX, { 2013, 1, 2, 3, 4, 55, 10, 555 },
			{ _T("1.2.3.110"), 20010 }, { 2011, 2, 0, 3, 14, 25, 10, 789 }, { 2011, 1, 2, 3, 4, 5, 10, 999 }, { 2012, 1, 2, 3, 4, 5, 10, 000 }, Error, ProgressiveConnectionErrorNone, _T("Error"), (int)DWORD_MAX, { 2013, 1, 2, 3, 4, 55, 10, 555 }
			},
			{ { _T("1.2.3.111"), 20011 }, { 2011, 2, 0, 3, 14, 25, 11, 789 }, { 2011, 1, 2, 3, 4, 5, 11, 999 }, { 2012, 1, 2, 3, 4, 5, 11, 000 }, Error, DiscoveryConnectionError, 0x12345678, { 2013, 1, 2, 3, 4, 55, 11, 555 },
			{ _T("1.2.3.111"), 20011 }, { 2011, 2, 0, 3, 14, 25, 11, 789 }, { 2011, 1, 2, 3, 4, 5, 11, 999 }, { 2012, 1, 2, 3, 4, 5, 11, 000 }, Error, DiscoveryConnectionError, _T("Error: Discovery Connection"), 0x12345678, { 2013, 1, 2, 3, 4, 55, 11, 555 }
			},
			{ { _T("1.2.3.112"), 20012 }, { 2011, 2, 0, 3, 14, 25, 12, 789 }, { 2011, 1, 2, 3, 4, 5, 12, 999 }, { 2012, 1, 2, 3, 4, 5, 12, 000 }, Error, MessageTransmissionError, -0x12345678, { 2013, 1, 2, 3, 4, 55, 12, 555 },
			{ _T("1.2.3.112"), 20012 }, { 2011, 2, 0, 3, 14, 25, 12, 789 }, { 2011, 1, 2, 3, 4, 5, 12, 999 }, { 2012, 1, 2, 3, 4, 5, 12, 000 }, Error, MessageTransmissionError, _T("Error: Message Transmission"), -0x12345678, { 2013, 1, 2, 3, 4, 55, 12, 555 }
			},
			{ { _T("1.2.3.113"), 20013 }, { 2011, 2, 0, 3, 14, 25, 13, 789 }, { 2011, 1, 2, 3, 4, 5, 13, 999 }, { 2012, 1, 2, 3, 4, 5, 13, 000 }, Error, ConnectFailed, 1313, { 2013, 1, 2, 3, 4, 55, 13, 555 },
			{ _T("1.2.3.113"), 20013 }, { 2011, 2, 0, 3, 14, 25, 13, 789 }, { 2011, 1, 2, 3, 4, 5, 13, 999 }, { 2012, 1, 2, 3, 4, 5, 13, 000 }, Error, ConnectFailed, _T("Error: Connect Failed"), 1313, { 2013, 1, 2, 3, 4, 55, 13, 555 }
			},
			{ { _T("1.2.3.114"), 20014 }, { 2011, 2, 0, 3, 14, 25, 14, 789 }, { 2011, 1, 2, 3, 4, 5, 14, 999 }, { 2012, 1, 2, 3, 4, 5, 14, 000 }, Error, DisconnectFailed, 1414, { 2013, 1, 2, 3, 4, 55, 14, 555 },
			{ _T("1.2.3.114"), 20014 }, { 2011, 2, 0, 3, 14, 25, 14, 789 }, { 2011, 1, 2, 3, 4, 5, 14, 999 }, { 2012, 1, 2, 3, 4, 5, 14, 000 }, Error, DisconnectFailed, _T("Error: Disconnect Failed"), 1414, { 2013, 1, 2, 3, 4, 55, 14, 555 }
			},
			{ { _T("1.2.3.115"), 20015 }, { 2011, 2, 0, 3, 14, 25, 15, 789 }, { 2011, 1, 2, 3, 4, 5, 15, 999 }, { 2012, 1, 2, 3, 4, 5, 15, 000 }, Error, (ProgressiveConnectionError)(DisconnectFailed + 1), 1515, { 2013, 1, 2, 3, 4, 55, 15, 555 },
			{ _T("1.2.3.115"), 20015 }, { 2011, 2, 0, 3, 14, 25, 15, 789 }, { 2011, 1, 2, 3, 4, 5, 15, 999 }, { 2012, 1, 2, 3, 4, 5, 15, 000 }, Error, (ProgressiveConnectionError)(DisconnectFailed + 1), _T("Error"), 1515, { 2013, 1, 2, 3, 4, 55, 15, 555 }
			},
			{ { _T("1.2.3.116"), 20016 }, { 2011, 2, 0, 3, 14, 25, 16, 789 }, { 2011, 1, 2, 3, 4, 5, 16, 999 }, { 2012, 1, 2, 3, 4, 5, 16, 000 }, Error, (ProgressiveConnectionError)INT_MAX, 1616, { 2013, 1, 2, 3, 4, 55, 16, 555 },
			{ _T("1.2.3.116"), 20016 }, { 2011, 2, 0, 3, 14, 25, 16, 789 }, { 2011, 1, 2, 3, 4, 5, 16, 999 }, { 2012, 1, 2, 3, 4, 5, 16, 000 }, Error, (ProgressiveConnectionError)INT_MAX, _T("Error"), 1616, { 2013, 1, 2, 3, 4, 55, 16, 555 }
			},
			{ { _T("1.2.3.117"), 20017 }, { 2011, 2, 0, 3, 14, 25, 17, 789 }, { 2011, 1, 2, 3, 4, 5, 17, 999 }, { 2012, 1, 2, 3, 4, 5, 17, 000 }, Error, (ProgressiveConnectionError)DWORD_MAX, 1717, { 2013, 1, 2, 3, 4, 55, 17, 555 },
			{ _T("1.2.3.117"), 20017 }, { 2011, 2, 0, 3, 14, 25, 17, 789 }, { 2011, 1, 2, 3, 4, 5, 17, 999 }, { 2012, 1, 2, 3, 4, 5, 17, 000 }, Error, (ProgressiveConnectionError)DWORD_MAX, _T("Error"), 1717, { 2013, 1, 2, 3, 4, 55, 17, 555 }
			},

			{ { _T("1.2.3.118"), 20018 }, { 2011, 2, 0, 3, 14, 25, 18, 789 }, { 2011, 1, 2, 3, 4, 5, 18, 999 }, { 2012, 1, 2, 3, 4, 5, 18, 000 }, (ProgressiveConnectionState)(Error + 1), CouldNotFindServer, 1818, { 2013, 1, 2, 3, 4, 55, 18, 555 },
			{ _T("1.2.3.118"), 20018 }, { 2011, 2, 0, 3, 14, 25, 18, 789 }, { 2011, 1, 2, 3, 4, 5, 18, 999 }, { 2012, 1, 2, 3, 4, 5, 18, 000 }, (ProgressiveConnectionState)(Error + 1), CouldNotFindServer, _T("N/A"), 1818, { 2013, 1, 2, 3, 4, 55, 18, 555 }
			},
			{ { _T("1.2.3.119"), 20019 }, { 2011, 2, 0, 3, 14, 25, 19, 789 }, { 2011, 1, 2, 3, 4, 5, 19, 999 }, { 2012, 1, 2, 3, 4, 5, 19, 000 }, (ProgressiveConnectionState)INT_MAX, CouldNotFindServer, 1919, { 2013, 1, 2, 3, 4, 55, 19, 555 },
			{ _T("1.2.3.119"), 20019 }, { 2011, 2, 0, 3, 14, 25, 19, 789 }, { 2011, 1, 2, 3, 4, 5, 19, 999 }, { 2012, 1, 2, 3, 4, 5, 19, 000 }, (ProgressiveConnectionState)INT_MAX, CouldNotFindServer, _T("N/A"), 1919, { 2013, 1, 2, 3, 4, 55, 19, 555 }
			},
			{ { _T("1.2.3.120"), 20020 }, { 2011, 2, 0, 3, 14, 25, 20, 789 }, { 2011, 1, 2, 3, 4, 5, 20, 999 }, { 2012, 1, 2, 3, 4, 5, 20, 000 }, (ProgressiveConnectionState)INT_MIN, CouldNotFindServer, 2020, { 2013, 1, 2, 3, 4, 55, 20, 555 },
			{ _T("1.2.3.120"), 20020 }, { 2011, 2, 0, 3, 14, 25, 20, 789 }, { 2011, 1, 2, 3, 4, 5, 20, 999 }, { 2012, 1, 2, 3, 4, 5, 20, 000 }, (ProgressiveConnectionState)INT_MIN, CouldNotFindServer, _T("N/A"), 2020, { 2013, 1, 2, 3, 4, 55, 20, 555 }
			},
			{ { _T("1.2.3.121"), 20021 }, { 2011, 2, 0, 3, 14, 25, 21, 789 }, { 2011, 1, 2, 3, 4, 5, 21, 999 }, { 2012, 1, 2, 3, 4, 5, 21, 000 }, (ProgressiveConnectionState)DWORD_MAX, CouldNotFindServer, 2121, { 2013, 1, 2, 3, 4, 55, 21, 555 },
			{ _T("1.2.3.121"), 20021 }, { 2011, 2, 0, 3, 14, 25, 21, 789 }, { 2011, 1, 2, 3, 4, 5, 21, 999 }, { 2012, 1, 2, 3, 4, 5, 21, 000 }, (ProgressiveConnectionState)DWORD_MAX, CouldNotFindServer, _T("N/A"), 2121, { 2013, 1, 2, 3, 4, 55, 21, 555 }
			},
		};

		// Setup. Perform each test with the same instance.
		CSentinelState varSentinelStateA(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));

		for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Setup. Perform each test with a new object.
			CSentinelState varSentinelStateB(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
				_T("1.12.2.0"));

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

			// Verify setup.
			AssertEqual(progInfo,
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

			// Perform operation(s) to be tested.
			varSentinelStateA.SetProgressiveConnectionInfo(progInfo);
			const CSentinelState& sentinelStateA(varSentinelStateA);
			const CProgressiveConnectionInfo* const objA(sentinelStateA.GetProgressiveConnectionInfoCopy());

			varSentinelStateB.SetProgressiveConnectionInfo(progInfo);
			const CSentinelState& sentinelStateB(varSentinelStateB);
			const CProgressiveConnectionInfo* const objB(sentinelStateB.GetProgressiveConnectionInfoCopy());

			// Test(s).
			AssertEqual(*objA,
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
			AssertEqual(*objB,
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

			// Cleanup.
			delete objA;
			delete objB;
		}
	}

	void SentinelState::AssertEqual(const CProgressiveHost &obj,
		const std::string& nameExpected,
		bool hasAddressExpected,
		const CIPAddress& ipAddressExpected,
		size_t currentAddressIndexExpected,
		const CIPAddress ipAddressesExpected[],
		size_t ipAddressCountExpected,
		DWORD hostPortExpected,
		const std::string& urlExpected,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex)
	{
		ASSERT_EQ(obj.GetName(), nameExpected) << FormatString("testIndex=%d", (int)testIndex);
		ASSERT_EQ(obj.HasAddress(), hasAddressExpected) << FormatString("testIndex=%d", (int)testIndex);
		ASSERT_EQ(obj.GetIPAddress(), ipAddressExpected);
		ASSERT_EQ(obj.GetCurrentAddressIndex(), currentAddressIndexExpected);

		ASSERT_EQ(obj.GetIPAddressCount(), ipAddressCountExpected);

		ASSERT_EQ(obj.GetIPAddress(-1), m_defaultIPAddress);
		for (size_t index(0); index < ipAddressCountExpected; ++index)
		{
			string resultName;
			resultName = FormatString(_T("obj.GetIPAddress(index=%d)"), (int)index);
			ASSERT_EQ(obj.GetIPAddress(index), ipAddressesExpected[index]) << resultName;
		}
		ASSERT_EQ(obj.GetIPAddress(obj.GetIPAddressCount()), m_defaultIPAddress);

		ASSERT_EQ(obj.GetHostPort(), hostPortExpected);
		ASSERT_EQ(obj.GetURL(), urlExpected);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::CSentinelState()
	///		 CSentinelState::GetProgressiveHostCopy()
	/// </summary>
	TEST_F(SentinelState, CSentinelState_ProgressiveHostDefaultTest)
	{
		// Setup.
		const int testIndex(0);
		const CSentinelState sentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));

		// Perform operation(s) to be tested.
		const CProgressiveHost* const obj(sentinelState.GetProgressiveHostCopy());

		// Test(s).
		AssertEqual(*obj,
			m_progHost_nameExpected,
			m_progHost_hasAddressExpected,
			m_progHost_ipAddressExpected,
			m_progHost_currentAddressIndexExpected,
			m_progHost_ipAddressesExpected,
			m_progHost_ipAddressCountExpected,
			m_progHost_hostPortExpected,
			m_progHost_urlExpected,
			testIndex, __TFUNCTION__, __LINE__);

		// Cleanup.
		delete obj;
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::CSentinelState()
	///		 CSentinelState::SetProgressiveHost()
	///		 CSentinelState::GetProgressiveHostCopy()
	/// Use a "default" CProgressiveHost instance with an empty host name and port zero.
	/// </summary>
	TEST_F(SentinelState, SetProgressiveHost_Default0Test)
	{
		// Setup.
		const int testIndex(0);
		CSentinelState varSentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));
		const CProgressiveHost progHost(_T(""), 0);

		// Perform operation(s) to be tested.
		varSentinelState.SetProgressiveHost(progHost);
		const CSentinelState& sentinelState(varSentinelState);
		const CProgressiveHost* const obj(sentinelState.GetProgressiveHostCopy());

		// Test(s).
		AssertEqual(*obj,
			m_progHost_nameExpected,
			m_progHost_hasAddressExpected,
			m_progHost_ipAddressExpected,
			m_progHost_currentAddressIndexExpected,
			m_progHost_ipAddressesExpected,
			m_progHost_ipAddressCountExpected,
			m_progHost_hostPortExpected,
			m_progHost_urlExpected,
			testIndex, __TFUNCTION__, __LINE__);

		// Cleanup.
		delete obj;
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::CSentinelState()
	///		 CSentinelState::SetProgressiveHost()
	///		 CSentinelState::GetProgressiveHostCopy()
	/// Use a "default" CProgressiveHost instance with an empty IP address array.
	/// </summary>
	TEST_F(SentinelState, SetProgressiveHost_Default1Test)
	{
		// Setup.
		const int testIndex(0);
		CSentinelState varSentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));
		CIPAddressArray ipAddressArray;
		const CProgressiveHost progHost(ipAddressArray);

		// Perform operation(s) to be tested.
		varSentinelState.SetProgressiveHost(progHost);
		const CSentinelState& sentinelState(varSentinelState);
		const CProgressiveHost* const obj(sentinelState.GetProgressiveHostCopy());

		// Test(s).
		AssertEqual(*obj,
			m_progHost_nameExpected,
			m_progHost_hasAddressExpected,
			m_progHost_ipAddressExpected,
			m_progHost_currentAddressIndexExpected,
			m_progHost_ipAddressesExpected,
			m_progHost_ipAddressCountExpected,
			m_progHost_hostPortExpected,
			m_progHost_urlExpected,
			testIndex, __TFUNCTION__, __LINE__);

		// Cleanup.
		delete obj;
	}
	
	/// <summary>
	/// Code coverage and functional test for:
	/// 	CSentinelState::CSentinelState()
	/// 	 CSentinelState::IsSystemOnline()
	/// </summary>
	TEST_F(SentinelState, IsSystemOnline_Test)
	{
		CSentinelState varSentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)),
			_T("1.12.2.0"));
		u_ConnectivityStatus connectivityStatus;
		// Nothing enabled
		ASSERT_FALSE(varSentinelState.IsSystemOnline());
		// TCP conected
		connectivityStatus.flags.isTcpConnected = true;
		varSentinelState.SetConnectivityStatus(connectivityStatus.uValue);
		ASSERT_FALSE(varSentinelState.IsSystemOnline());
		//TCP,PMT up
		connectivityStatus.flags.isPMTUp = true;
		varSentinelState.SetConnectivityStatus(connectivityStatus.uValue);
		ASSERT_FALSE(varSentinelState.IsSystemOnline());
		//TCP,PMT,MMT up
		connectivityStatus.flags.isMMTUp = true;
		varSentinelState.SetConnectivityStatus(connectivityStatus.uValue);
		ASSERT_FALSE(varSentinelState.IsSystemOnline());
		
		//TCP,PMT,MMT, Poller Online, Packet version defaulted to 0, VS has no significance
		varSentinelState.SetPollerOnline(true);
		ASSERT_TRUE(varSentinelState.IsSystemOnline());

		//Only MMT down
		connectivityStatus.flags.isMMTUp = false;
		varSentinelState.SetConnectivityStatus(connectivityStatus.uValue);
		ASSERT_FALSE(varSentinelState.IsSystemOnline());
		//Only PMT down
		connectivityStatus.flags.isMMTUp = true;
		connectivityStatus.flags.isPMTUp = false;
		varSentinelState.SetConnectivityStatus(connectivityStatus.uValue);
		ASSERT_FALSE(varSentinelState.IsSystemOnline());
			
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::ResponseForSentinelState()
	/// Test the ResponseForSentinelState with TCP as connected.
	/// </summary>
	TEST_F(SentinelState, CSentinelState_ResponseForSentinelState_ConnectedTCP)
	{
		SentinelState_ResponseForSentinelState_ConnectedTCP();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::ResponseForSentinelState()
	/// Test the ResponseForSentinelState with TCP as disconnected.
	/// </summary>
	TEST_F(SentinelState, CSentinelState_ResponseForSentinelState_DisconnectedTCP)
	{
		SentinelState_ResponseForSentinelState_DisconnectedTCP();
	}

	/// <summary>
	/// Code coverage and functional test for:
	/// 	CSentinelState::CSentinelState()
	/// 	 CSentinelState::IsBackgroundSnapshotInProgress()
	/// </summary>
	TEST_F(SentinelState, IsBackgroundSnapshotInProgress_Test)
	{
		CSentinelState varSentinelState(time_t(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12)), _T("1.12.2.0"));
		int meterCapturePercentage(0);
		ASSERT_FALSE(varSentinelState.IsBackgroundSnapshotInProgress(meterCapturePercentage));
		ASSERT_TRUE(meterCapturePercentage == 0);
		varSentinelState.BeginMeterCapture(InitialMeters, true);
		varSentinelState.SetMeterCapturePercentage(2, 10, 10);
		ASSERT_TRUE(varSentinelState.IsBackgroundSnapshotInProgress(meterCapturePercentage));
		ASSERT_TRUE(meterCapturePercentage == 100);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::SetMeterRolloverConfig()
	/// </summary>
	TEST_F(SentinelState, CSentinelState_SetMeterRolloverConfigTest)
	{
		SentinelState_SetMeterRolloverConfigTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::PerformRolloverMeterCheck()
	/// </summary>
	TEST_F(SentinelState, CSentinelState_PerformRolloverMeterCheckTest)
	{
		SentinelState_PerformRolloverMeterCheckTest();
	}	

	const time_t SentinelState::m_lastConfigFileLoadedExpected(0);

	const string SentinelState::m_progConfig_endpointsExpected[] = { _T("onelinkdiscossl:61614"), _T("onelinkdisco:61613") };

	// Connection Data
	const CIPAddress SentinelState::m_progInfo_lastMessageIPExpected;
	const SYSTEMTIME SentinelState::m_progInfo_lastMessageTimestampExpected = { 1970, 1, 4, 1, 0, 0, 0, 0 };
	const SYSTEMTIME SentinelState::m_progInfo_lastConnectAttemptTimestampExpected = { 1970, 1, 4, 1, 0, 0, 0, 0 };
	const SYSTEMTIME SentinelState::m_progInfo_lastConnectionTimestampExpected = { 1970, 1, 4, 1, 0, 0, 0, 0 };
	const string SentinelState::m_progInfo_connectionStateStringExpected(_T("Not Started"));
	const SYSTEMTIME SentinelState::m_progInfo_lastAddressUpdateTimestampExpected = { 1970, 1, 4, 1, 0, 0, 0, 0 };

	const std::string SentinelState::m_progHost_nameExpected;
	const CIPAddress SentinelState::m_progHost_ipAddressExpected;
	const CIPAddress* const SentinelState::m_progHost_ipAddressesExpected(NULL);
	const std::string SentinelState::m_progHost_urlExpected;

	const CIPAddress SentinelState::m_defaultIPAddress;

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::GetSubscriptionDataResponse()
	/// </summary>
	TEST_F(SentinelState, CSentinelState_GetSubscriptionDataResponseTest)
	{
		SentinelState_GetSubscriptionDataResponseTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelState::SetPendingAction/GetPendingActionAsString()
	/// </summary>
	TEST_F(SentinelState, CSentinelState_PendingAction)
	{
		SentinelState_PendingAction();
	}
	
}
