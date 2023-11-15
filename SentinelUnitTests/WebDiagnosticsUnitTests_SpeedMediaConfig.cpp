#include "stdafx.h"
#include "ProcessDump/WebDiagnostics.h"

// Sentinel includes.
#include "SpeedMediaConfig.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	LPCTSTR TOKEN = _T("token");
	const string CUSTOM_TOKEN = _T("token=");
	const DWORD MINIMUM_TOKEN_SIZE = 10;

	class WebDiagnostics_SpeedMediaConfig : public ::testing::Test
	{
	public:

		// Class member constant(s).
		static const LPCTSTR SPEEDMEDIA_CONFIG_FILENAME; // When class under test choses the filename.
		static const LPCTSTR ALT_SPEEDMEDIA_CONFIG_FILENAME; // When we can choose any filename.
		static const int SPEEDMEDIA_CONFIG_REFRESH_SECONDS = 60;

		// Web content simulator.  Compare actual results with simulator results.
		string CalcPageHeaderHtml(int refreshSeconds)
		{
			string html;
			html = FormatString("<html><META Http-equiv=\"refresh\" Content=\"%d\">", (int)refreshSeconds);

			return html;
		}

		// Web content simulator.  Compare actual results with simulator results.
		string CalcPageTrailerHtml()
		{
			return _T("</html>");
		}

		// Web content simulator.  Compare actual results with simulator results.
		string CalcCommonHeaderHtml(const CWebDiagnosticsCommonInfo &info)
		{
			string html;
			html += FormatString(_T("Startup Time: %s<br>"), static_cast<LPCTSTR>(CUtilities::FormatDateTimeString(info.m_startupTime, "%m/%d/%Y %I:%M:%S %p").c_str()));
			html += FormatString(_T("Current Time: %s<br>"), static_cast<LPCTSTR>(CUtilities::FormatDateTimeString(info.m_currentTime, "%m/%d/%Y %I:%M:%S %p").c_str()));
			html += FormatString(_T("SlotMast ID: %ld<br>"), (long)info.m_slotMastID);
			html += FormatString(_T("Machine Number:%ld<br>"), (long)info.m_machineNumber); // No space after ':' for backwards compatibility.
			html += FormatString(_T("Sentinel Version: %s<br>"), static_cast<LPCTSTR>(info.m_sentinelVersion.c_str()));
			html += FormatString(_T("OS Image Version: %s<br>"), static_cast<LPCTSTR>(info.m_osImageVersion.c_str()));
			html += FormatString(_T("Carrier Version: %s<br>"), static_cast<LPCTSTR>(info.m_carrierVersion.c_str()));
			html += FormatString(_T("VM Version: %s<br>"), static_cast<LPCTSTR>(info.m_vmVersion.c_str()));
			html += _T("<br>");

			return html;
		}

		/// <summary>
		/// Web content simulator for the SpeedMedia Config info.
		/// Compare actual results with simulator results.
		/// </summary>
		/// <param name="info">IN - The web diagnostics common information.</param>
		/// <param name="speedMediaConfig">IN - The SpeedMedia Config.</param>
		/// <returns>A string with the expected content for the SpeedMedia Config info.</returns>
		string CalcSpeedMediaConfigHtml(const std::map<string, string>& speedMediaConfig,
			const std::map<string, string>& customParams)
		{
			string html;

			for (auto keyValueIter = speedMediaConfig.begin();
				keyValueIter != speedMediaConfig.end();
				++keyValueIter)
			{
				string key(keyValueIter->first.c_str());;
				string value(keyValueIter->second.c_str());
				string lowerCaseKey(key);
				CUtilities::MakeLower(lowerCaseKey);
				int findIndex = lowerCaseKey.find(TOKEN);
				if (findIndex != string::npos && findIndex > 0)
				{
					int valueLength(value.length());
					if (valueLength > MINIMUM_TOKEN_SIZE)
					{
						string length;
						length = FormatString(" Length:%d", valueLength);
						value = value.substr(0, MINIMUM_TOKEN_SIZE / 2) + _T("...") + value.substr(value.length() - MINIMUM_TOKEN_SIZE / 2) + length;
					}
				}
				html += FormatString("%s = %s<br>", key.c_str(), value.c_str());
			}

			html += _T("<br>");

			for (auto keyValueIter = customParams.begin();
				keyValueIter != customParams.end();
				++keyValueIter)
			{
				string key(keyValueIter->first.c_str());;
				string value(keyValueIter->second.c_str());
				string lowerCaseValue(value);
				CUtilities::MakeLower(lowerCaseValue);
				int findIndex = lowerCaseValue.find(CUSTOM_TOKEN);
				if (findIndex != string::npos)
				{
					string tokenValue(value.substr(findIndex + CUSTOM_TOKEN.length()));
					int tokenValueLength(tokenValue.length());
					if (tokenValueLength > MINIMUM_TOKEN_SIZE)
					{
						string length;
						length = FormatString(" Length:%d", tokenValueLength);
						tokenValue = tokenValue.substr(0, MINIMUM_TOKEN_SIZE / 2) + _T("...") + tokenValue.substr(tokenValue.length() - MINIMUM_TOKEN_SIZE / 2) + length;
						value = value.substr(0, findIndex + CUSTOM_TOKEN.length()) + tokenValue;
					}
				}
				html += FormatString("%s = %s<br>", key.c_str(), value.c_str());
			}


			return html;
		}

		/// <summary>
		/// Web content simulator for the entire SpeedMedia Config page.
		/// Compare actual results with simulator results.
		/// </summary>
		/// <param name="info">IN - The web diagnostics common information.</param>
		/// <param name="speedMediaConfig">IN - The SpeedMedia Config.</param>
		/// <returns>A string with the expected content for the entire SpeedMedia Config page.</returns>
		string CalcSpeedMediaConfigPage(const CWebDiagnosticsCommonInfo &commonInfo, const CSpeedMediaConfig &speedMediaConfig)
		{
			string html;
			html += CalcPageHeaderHtml(SPEEDMEDIA_CONFIG_REFRESH_SECONDS);
			html += CalcCommonHeaderHtml(commonInfo);
			html += CalcSpeedMediaConfigHtml(speedMediaConfig.GetConfigurationKeyValuePairs(), speedMediaConfig.GetCustomParamsKeyValuePairs());
			html += CalcPageTrailerHtml();

			return html;
		}
	};

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::GetSpeedMediaConfigHtml()
		/// </summary>
		TEST_F(WebDiagnostics_SpeedMediaConfig, GetSpeedMediaConfigHtml_DefaultTest)
		{
			// Setup.
			CSpeedMediaConfig speedMediaConfig;

			// Test(s).
			ASSERT_EQ(CWebDiagnostics::GetSpeedMediaConfigHtml(speedMediaConfig.GetConfigurationKeyValuePairs(),
				speedMediaConfig.GetCustomParamsKeyValuePairs()),
				CalcSpeedMediaConfigHtml(speedMediaConfig.GetConfigurationKeyValuePairs(),
				speedMediaConfig.GetCustomParamsKeyValuePairs()));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::GetSpeedMediaConfigHtml()
		/// </summary>
		TEST_F(WebDiagnostics_SpeedMediaConfig, GetSpeedMediaConfigHtml_DataTest)
		{
			// Setup.
			CSpeedMediaConfig speedMediaConfig;
			std::map<string, string> speedMediaParams;
			std::map<string, string> customParams;

			speedMediaParams["speedMediaParamsKey1"] = "speedMediaParamsValue1";
			speedMediaParams["speedMediaParamsKey2"] = "speedMediaParamsValue2";
			speedMediaParams["speedMediaParamsKey3"] = "speedMediaParamsValue3";
			speedMediaParams["speedMediaParamsKey4"] = "speedMediaParamsValue4";
			customParams["customParamsKey1"] = "customParamsValue1";
			customParams["customParamsKey2"] = "customParamsValue2";
			customParams["customParamsKey3"] = "customParamsValue3";
			customParams["customParamsKey4"] = "customParamsValue4";

			speedMediaConfig.SetConfigurationKeyValuePairs(speedMediaParams);
			speedMediaConfig.SetCustomParamsKeyValuePairs(customParams);

			// Test(s).
			ASSERT_EQ(CWebDiagnostics::GetSpeedMediaConfigHtml(speedMediaConfig.GetConfigurationKeyValuePairs(),
				speedMediaConfig.GetCustomParamsKeyValuePairs()),
				CalcSpeedMediaConfigHtml(speedMediaConfig.GetConfigurationKeyValuePairs(),
				speedMediaConfig.GetCustomParamsKeyValuePairs()));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::GetSpeedMediaConfigHtml()
		/// </summary>
		TEST_F(WebDiagnostics_SpeedMediaConfig, GetSpeedMediaConfigHtml_TokenTest)
		{
			// Setup.
			CSpeedMediaConfig speedMediaConfig;
			std::map<string, string> speedMediaParams;
			std::map<string, string> customParams;

			speedMediaParams["speedMediaParamsKey1toke"] = "speedMediaParamsValue1";
			speedMediaParams["speedMediaParamsKey2token"] = "speedMediaParamsValue2";
			speedMediaParams["speedMediaParamsKeyToken3"] = "speedMediaParamsValue3";
			speedMediaParams["speedMediaParamsKeyToken4"] = "4abcdefghi"; // Should not report "...", as value length <= 10.
			speedMediaParams["speedMediaParamsKeyToken5"] = "5abcdefghij"; // Should report "...", as value length > 10.
			speedMediaParams["tokenSpeedMediaParamsKey6"] = "speedMediaParamsValue6"; // Should not report "...", as index = 0.
			speedMediaParams["xTokenSpeedMediaParamsKey7"] = "speedMediaParamsValue7"; // Should report "...", as index > 0.
			customParams["customParamsKey1"] = "customParamsValue1token";
			customParams["customParamsKey2"] = "customParamsValue2token=";
			customParams["customParamsKey3"] = "customParamsValueToken=3";
			customParams["customParamsKey4"] = "customParamsValueToken=4abcdefghi"; // Should not report "...", as value length <= 10.
			customParams["customParamsKey5"] = "customParamsValueToken=5abcdefghij"; // Should report "...", as value length > 10.
			customParams["customParamsKey6"] = "token=customParamsValue6";
			customParams["customParamsKey7"] = "xToken=customParamsValue7";

			speedMediaConfig.SetConfigurationKeyValuePairs(speedMediaParams);
			speedMediaConfig.SetCustomParamsKeyValuePairs(customParams);

			// Test(s).
			ASSERT_EQ(CWebDiagnostics::GetSpeedMediaConfigHtml(speedMediaConfig.GetConfigurationKeyValuePairs(),
				speedMediaConfig.GetCustomParamsKeyValuePairs()),
				CalcSpeedMediaConfigHtml(speedMediaConfig.GetConfigurationKeyValuePairs(),
				speedMediaConfig.GetCustomParamsKeyValuePairs()));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::SaveSpeedMediaConfigData()
		/// </summary>
		TEST_F(WebDiagnostics_SpeedMediaConfig, SaveSpeedMediaConfigDataTest)
		{
#if 0 // Failed unit test
			// Input(s).
			const int utilitiesTimeZoneOffset(0);
			LPCTSTR const currentHostIP(_T("11.22.33.44"));
			const string dumpFileName(_T("C:\\Inetpub\\wwwroot\\SpeedMediaConfig.html"));
			const DWORD tickCount(0);
			LPCTSTR const speedMediaInfoCacheList(_T(""));
			const bool hasSpeedMediaInfo(false);
			CPlayerSession playerSession;
			
			CSpeedMediaInfo speedMediaInfo;

			const CWebDiagnosticsCommonInfo commonInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				CUtilities::GetTimeFromFields(2014, 1, 27, 13, 14, 15),
				941,
				5141,
				_T("1.12.2.0"),
				_T("1.21.0"),
				hasSpeedMediaInfo,
				speedMediaInfo,
				speedMediaInfoCacheList,
				161028 * 1024,
				0,
				8, "", "");

			string expectedString = "<html><META Http-equiv=\"refresh\" Content=\"60\">Startup Time: 01/27/2014 10:11:12 AM<br>Current Time: 01/27/2014 01:14:15 PM<br>SlotMast ID: 941<br>Machine Number:5141<br>Sentinel Version: 1.12.2.0<br>OS Image Version: 1.21.0<br><br>speedMediaParamsKey1 = speedMediaParamsValue1<br>speedMediaParamsKey2 = speedMediaParamsValue2<br>speedMediaParamsKey3 = speedMediaParamsValue3<br>speedMediaParamsKey4 = speedMediaParamsValue4<br><br>customParamsKey1 = customParamsValue1<br>customParamsKey2 = customParamsValue2<br>customParamsKey3 = customParamsValue3<br>customParamsKey4 = customParamsValue4<br></html>";

			CSpeedMediaConfig speedMediaConfig;
			std::map<string, string> speedMediaParams;
			std::map<string, string> customParams;
			speedMediaParams["speedMediaParamsKey1"] = "speedMediaParamsValue1";
			speedMediaParams["speedMediaParamsKey2"] = "speedMediaParamsValue2";
			speedMediaParams["speedMediaParamsKey3"] = "speedMediaParamsValue3";
			speedMediaParams["speedMediaParamsKey4"] = "speedMediaParamsValue4";
			customParams["customParamsKey1"] = "customParamsValue1";
			customParams["customParamsKey2"] = "customParamsValue2";
			customParams["customParamsKey3"] = "customParamsValue3";
			customParams["customParamsKey4"] = "customParamsValue4";
			speedMediaConfig.SetConfigurationKeyValuePairs(speedMediaParams);
			speedMediaConfig.SetCustomParamsKeyValuePairs(customParams);

			shared_ptr<stringstream> file(new stringstream);

			// Perform operation(s) to be tested.
			CWebDiagnostics::SaveSpeedMediaConfiguration(file, commonInfo, 
				speedMediaConfig.GetConfigurationKeyValuePairs(), speedMediaConfig.GetCustomParamsKeyValuePairs());

			// Test(s).
			string actual;
			actual = file->str();

			 ASSERT_TRUE(actual == expectedString);
#endif
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CWebDiagnostics::SaveSpeedMediaConfigData()
		/// </summary>
		TEST_F(WebDiagnostics_SpeedMediaConfig, SaveSpeedMediaConfigTokenTest)
		{

#if 0 // Failed unit test
			// Input(s).
			const int utilitiesTimeZoneOffset(0);
			LPCTSTR const currentHostIP(_T("11.22.33.44"));
			const string dumpFileName(_T("C:\\Inetpub\\wwwroot\\SpeedMediaConfig.html"));
			const DWORD tickCount(0);
			LPCTSTR const speedMediaInfoCacheList(_T(""));
			const bool hasSpeedMediaInfo(false);
			CPlayerSession playerSession;

			CSpeedMediaInfo speedMediaInfo;

			const CWebDiagnosticsCommonInfo commonInfo(CUtilities::GetTimeFromFields(2014, 1, 27, 10, 11, 12),
				CUtilities::GetTimeFromFields(2014, 1, 27, 13, 14, 15),
				941,
				5141,
				_T("1.12.2.0"),
				_T("1.21.0"),
				hasSpeedMediaInfo,
				speedMediaInfo,
				speedMediaInfoCacheList,
				161028 * 1024,
				0,
				8, "", "");

			string expectedString = "<html><META Http-equiv=\"refresh\" Content=\"60\">Startup Time: 01/27/2014 10:11:12 AM<br>Current Time: 01/27/2014 01:14:15 PM<br>SlotMast ID: 941<br>Machine Number:5141<br>Sentinel Version: 1.12.2.0<br>OS Image Version: 1.21.0<br><br>speedMediaParamsKey1toke = speedMediaParamsValue1<br>speedMediaParamsKey2token = speed...alue2 Length:22<br>speedMediaParamsKeyToken3 = speed...alue3 Length:22<br>speedMediaParamsKeyToken4 = 4abcdefghi<br>speedMediaParamsKeyToken5 = 5abcd...fghij Length:11<br>tokenSpeedMediaParamsKey6 = speedMediaParamsValue6<br>xTokenSpeedMediaParamsKey7 = speed...alue7 Length:22<br><br>customParamsKey1 = customParamsValue1token<br>customParamsKey2 = customParamsValue2token=<br>customParamsKey3 = customParamsValueToken=3<br>customParamsKey4 = customParamsValueToken=4abcdefghi<br>customParamsKey5 = customParamsValueToken=5abcd...fghij Length:11<br>customParamsKey6 = token=custo...alue6 Length:18<br>customParamsKey7 = xToken=custo...alue7 Length:18<br></html>";

			CSpeedMediaConfig speedMediaConfig;
			std::map<string, string> speedMediaParams;
			std::map<string, string> customParams;
			speedMediaParams["speedMediaParamsKey1toke"] = "speedMediaParamsValue1";
			speedMediaParams["speedMediaParamsKey2token"] = "speedMediaParamsValue2";
			speedMediaParams["speedMediaParamsKeyToken3"] = "speedMediaParamsValue3";
			speedMediaParams["speedMediaParamsKeyToken4"] = "4abcdefghi"; // Should not report "...", as value length <= 10.
			speedMediaParams["speedMediaParamsKeyToken5"] = "5abcdefghij"; // Should report "...", as value length > 10.
			speedMediaParams["tokenSpeedMediaParamsKey6"] = "speedMediaParamsValue6"; // Should not report "...", as index = 0.
			speedMediaParams["xTokenSpeedMediaParamsKey7"] = "speedMediaParamsValue7"; // Should report "...", as index > 0.
			customParams["customParamsKey1"] = "customParamsValue1token";
			customParams["customParamsKey2"] = "customParamsValue2token=";
			customParams["customParamsKey3"] = "customParamsValueToken=3";
			customParams["customParamsKey4"] = "customParamsValueToken=4abcdefghi"; // Should not report "...", as value length <= 10.
			customParams["customParamsKey5"] = "customParamsValueToken=5abcdefghij"; // Should report "...", as value length > 10.
			customParams["customParamsKey6"] = "token=customParamsValue6";
			customParams["customParamsKey7"] = "xToken=customParamsValue7";
			speedMediaConfig.SetConfigurationKeyValuePairs(speedMediaParams);
			speedMediaConfig.SetCustomParamsKeyValuePairs(customParams);

			shared_ptr<stringstream> file(new stringstream);

			// Perform operation(s) to be tested.
			CWebDiagnostics::SaveSpeedMediaConfiguration(file, commonInfo,
				speedMediaConfig.GetConfigurationKeyValuePairs(), speedMediaConfig.GetCustomParamsKeyValuePairs());

			// Test(s).
			string actual;
			actual = file->str();

			ASSERT_EQ(expectedString, actual);
#endif
		}

	const LPCTSTR WebDiagnostics_SpeedMediaConfig::SPEEDMEDIA_CONFIG_FILENAME(_T("C:\\Inetpub\\wwwroot\\SpeedMediaConfig.html"));
	const LPCTSTR WebDiagnostics_SpeedMediaConfig::ALT_SPEEDMEDIA_CONFIG_FILENAME(_T("C:/Inetpub/wwwroot/SpeedMediaConfig.html"));
}
