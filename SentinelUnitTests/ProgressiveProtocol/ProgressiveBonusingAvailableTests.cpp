#include "stdafx.h"
#include "AssertMsg.h"
#include "ProgressiveLevelTestHelpers.h"
#include "ProgressiveProtocol/ProgressiveBonusingAvailable.h"
//#include "Utility.h"

#include "gtest/gtest.h"

#define GetTickCount() CUtilities::GetTickCount() 
#define  GetMillisecondsSinceTimestamp CUtilities::GetMillisecondsSinceTimestamp

using namespace std;
namespace SentinelNativeUnitTests
{
	using namespace ProgressiveLevelTestHelpers;

	static const char* pJson =
		"{"
		"\"SessionId\":470,"
		"\"PatronInformation\":{"
		"\"CardNumber\":\"4BDC04FD39\","
		"\"PlayerId\":0,"
		"\"Tier\":\"Default\","
		"\"Name\":\"joe\""
		"},"
		"\"EligibilityDetermined\":true,"
		"\"EligibleLevels\":["
		"{"
		"\"LevelId\":24037,"
		"\"Name\":\"La MontaÃ±a\","
		"\"TriggerType\":\"CoinInTriggerOnly\","
		"\"Eligible\":true,"
		"\"ShowOdometer\":true,"
		"\"PercentEligible\":9,"
		"\"Minors\":["
		"{"
		"\"Sequence\":1,"
		"\"Eligible\":true,"
		"\"Name\":\"1\","
		"\"Min\":0.00,"
		"\"Max\":0.00"
		"}"
		"]"
		"},"
		"{"
		"\"LevelId\":12345,"
		"\"Name\":\"Buffalo\","
		"\"TriggerType\":\"PayLineTrigger\","
		"\"Eligible\":false,"
		"\"ShowOdometer\":false,"
		"\"PercentEligible\":2,"
		"\"Minors\":["
		"{"
		"\"Sequence\":2,"
		"\"Eligible\":false,"
		"\"Name\":\"3\","
		"\"Min\":0.04,"
		"\"Max\":0.05"
		"},"
		"{"
		"\"Sequence\":3,"
		"\"Eligible\":true,"
		"\"Name\":\"4\","
		"\"Min\":0.05,"
		"\"Max\":0.06"
		"}"
		"]"
		"}"
		"],"
		"\"SourceDeviceId\":1,"
		"\"SourceDeviceConfigVersion\":0,"
		"\"SourceAsset\":\"\","
		"\"Timestamp\":1433466405284"
		"}";

	class ProgressiveBonusingAvailableTests : public ::testing::Test
	{

	public:
		void f_ProgressiveBonusingAvailable_ctor(void)
		{
			const int sessionId = 8;
			const bool eligibilityDetermined = true;

			try
			{
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(pJson).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

				CProgressiveBonusingAvailable bonusingAvailable(sessionId, jsonDoc);

				const uint64_t MinutesTolerance = 5;
				long long int tick = GetTickCount();
				ASSERT_TRUE((tick - bonusingAvailable.m_receivedTickCount) < (DWORD)(MinutesTolerance * 60 * 1000)) << "m_receivedTickCount";
				DWORD time = GetMillisecondsSinceTimestamp(bonusingAvailable.m_receivedTime);
				ASSERT_TRUE(GetMillisecondsSinceTimestamp(bonusingAvailable.m_receivedTime) < (DWORD)(MinutesTolerance * 60 * 1000)) << "m_receivedTime";;
				ASSERT_EQ(sessionId, bonusingAvailable.GetSessionId()) << "GetSessionId";
				ASSERT_EQ(eligibilityDetermined, bonusingAvailable.m_eligibilityDetermined) << "m_eligibilityDetermined";
			}
			catch (JsonException& e)
			{
				FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %s: %s: %s", e.code(), e.codestring(), e.what(), e.filename());
			}
			catch (SentinelException& e)
			{
				FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %s", e.code(), e.what());
			}
		}

		/*void f_CProgressiveBonusingAvailable_LogStr(void)
{
			const SYSTEMTIME receivedTime = { 2007, 1, 2, 3, 4, 5, 6, 7 };
			const DWORD receivedTickCount = 9876;
			const int sessionId = 8;
			const bool eligibilityDetermined = true;

			rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(pJson).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

			CProgressiveBonusingAvailable bonusingAvailable(sessionId, jsonDoc);
			m_receivedTime=receivedTime;
			m_receivedTickCount=receivedTickCount;
			m_sessionId=sessionId;
			m_eligibilityDetermined=eligibilityDetermined;

			bonusing.AddEligibleLevel(1, true);
			string logStr = bonusing.LogStr();

			ASSERT_EQ(L"BonusingAvailable m_sessionId=8 m_eligibilityDetermined=1 eligibleLevels: 1",
				(LPCTSTR) logStr);
		}*/

		void f_CProgressiveBonusingAvailable_Log(void)
		{
			// Data.
			const SYSTEMTIME receivedTime = { 2007, 1, 2, 3, 4, 5, 6, 7 };
			const DWORD receivedTickCount = 9876;
			const int sessionId = 8;
			const bool eligibilityDetermined = true;

			rapidjson::Document jsonDoc;
			if (jsonDoc.Parse(pJson).HasParseError())
				throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

			CProgressiveBonusingAvailable bonusingAvailable(sessionId, jsonDoc);
			bonusingAvailable.m_receivedTime = receivedTime;
			bonusingAvailable.m_receivedTickCount = receivedTickCount;
			bonusingAvailable.m_sessionId = sessionId;
			bonusingAvailable.m_eligibilityDetermined = eligibilityDetermined;

			// Setup.
			ASSERT_EQ(eligibilityDetermined, bonusingAvailable.IsEligibilityDetermined());
			ASSERT_EQ(bonusingAvailable.m_sessionId, bonusingAvailable.GetSessionId());
			ASSERT_TRUE(!(CUtilities::CompareSysTimes(bonusingAvailable.m_receivedTime, bonusingAvailable.GetReceivedTime())));

		}
	};

	TEST_F(ProgressiveBonusingAvailableTests, ProgressiveBonusingAvailable_ctor)
	{
		f_ProgressiveBonusingAvailable_ctor();
	}

	/*TEST_F(ProgressiveBonusingAvailableTests, CProgressiveBonusingAvailable_LogStr)
	{
		f_CProgressiveBonusingAvailable_LogStr();

}*/

/// <summary>
/// Code coverage test for:
///     CProgressiveBonusingAvailable::Log()
/// </summary>
	TEST_F(ProgressiveBonusingAvailableTests, ProgressiveBonusingAvailable_Log)
	{
		f_CProgressiveBonusingAvailable_Log();
	}

}

