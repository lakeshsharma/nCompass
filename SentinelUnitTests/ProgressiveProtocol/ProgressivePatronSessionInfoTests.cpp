#include "stdafx.h"
#include "AssertMsg.h"
#include "ProgressiveLevelTestHelpers.h"
#include "ProgressiveProtocol/ProgressivePatronSessionInfo.h"
#include "Util/Convert.h"


#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	static const char*	 pCardNumber	  = "1";
	static const char*	 pName			  = "2";
	static const char*	 pTier			  = "3";
	static const int64_t playerId		  = 5;
	static const int	 sessionPts		  = 6;

	class ProgressivePatronInfoTests: public ::testing::Test
	{
		public:
			string CreateJsonString(
				bool useCardNumber,
				bool useName,
				bool useTier,
				bool usePlayerId,
				bool useSessionPts)
			{
				JsonWriteStreamString writeStream;
				JsonWriter writer(writeStream);

				writer.StartObject();
				writer.Key("PatronInformation");
				writer.StartObject();

				if (useCardNumber)
				{
					writer.Key("CardNumber");
					writer.String(pCardNumber);
				}

				if (useName)
				{
					writer.Key("Name");
					writer.String(pName);
				}

				if (useTier)
				{
					writer.Key("Tier");
					writer.String(pTier);
				}

				if (usePlayerId)
				{
					writer.Key("PlayerId");
					writer.Int64(playerId);
				}

				if (useSessionPts)
				{
					writer.Key("SessionPts");
					writer.Int(sessionPts);
				}

				writer.EndObject();
				writer.EndObject();

				return writeStream;
			}

			void VerifyString(int line, bool use, const char* pElement, const char* pExpected, const char* pActual)
			{
				if (use)
				{
					ASSERT_STREQ(pExpected, pActual) << AssertMsg::Format(line, nullptr, L"%S=true", pElement);
				}
				else
				{
					ASSERT_STREQ("", pActual) << AssertMsg::Format(line, nullptr, L"%S=false", pElement);
				}
			}

			void VerifyVal(int line, bool use, const char* pElement, int64_t expected, int64_t actual, bool isValid)
			{
				if (use)
				{
					ASSERT_TRUE(isValid) << AssertMsg::Format(line, L"IsValid", L"%S=true", pElement);
					ASSERT_EQ((int) expected, (int) actual) << AssertMsg::Format(line, L"Value", L"%S=true", pElement);
				}
				else
				{
					ASSERT_FALSE(isValid) << AssertMsg::Format(line, L"IsValid", L"%S=false", pElement);
				}
			}

			void VerifyVal(int line, bool use, const char* pElement, int expected, int actual, bool isValid)
			{
				if (use)
				{
					ASSERT_TRUE(isValid) << AssertMsg::Format(line, L"IsValid", L"%S=true", pElement);
					ASSERT_EQ(expected, actual) << AssertMsg::Format(line, L"Value", L"%S=true", pElement);
				}
				else
				{
					ASSERT_FALSE(isValid) << AssertMsg::Format(line, L"IsValid", L"%S=false", pElement);
				}
			}

			void Verify(
				int line,
				const ProgressivePatronSessionInfo& info,
				bool useCardNumber,
				bool useName,
				bool useTier,
				bool usePlayerId,
				bool useSessionPts)
			{
				VerifyString(line, useCardNumber,		"useCardNumber",		pCardNumber,	  info.GetCardNumber());
				VerifyString(line, useName,				"useName",				pName,			  info.GetName());
				VerifyString(line, useTier,				"useTier",				pTier,			  info.GetTier());

				bool isValid;
				int64_t actual64 = info.GetPlayerId(isValid);
				VerifyVal(line, usePlayerId, "usePlayerId", playerId, actual64, isValid);

				int actual = info.GetSessionPoints(isValid);
				VerifyVal(line, useSessionPts, "useSessionPts", sessionPts, actual, isValid);
			}

			void RunCtorTest(
				int line,
				bool useCardNumber,
				bool useName,
				bool useTier,
				bool usePlayerId,
				bool useSessionPts)
			{
				const SYSTEMTIME receivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
				const DWORD receivedTickCount = 100;
				const int sessionId = 99;

				string s  = CreateJsonString(useCardNumber, useName, useTier, usePlayerId, useSessionPts);
				rapidjson::Document jsonDoc;
				bool result = jsonDoc.Parse(s.c_str()).HasParseError();
				ASSERT_FALSE(result) << AssertMsg::Format(line, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

				ProgressivePatronSessionInfo info(sessionId, jsonDoc);
				ASSERT_EQ(sessionId, info.GetSessionId()) << "GetSessionId()";
				Verify(line, info, useCardNumber, useName, useTier, usePlayerId, useSessionPts);
			}
	};
				
	TEST_F(ProgressivePatronInfoTests, ProgressivePatronSessionInfoTests)
	{
		int test = 0;
		
		for (int useCardNumber = 0; useCardNumber < 3; useCardNumber++)
		{
			for (int useName = 0; useName < 3; useName++)
			{
				for (int useTier = 0; useTier < 3; useTier++)
				{
					for (int usePlayerId = 0; usePlayerId < 3; usePlayerId++)
					{
						for (int useSessionPts = 0; useSessionPts < 3; useSessionPts++)
						{
							RunCtorTest(test++, useCardNumber == 0, useName == 0, useTier == 0, usePlayerId == 0, useSessionPts == 0);
						}
					}
				}
			}
		}
	}
}
