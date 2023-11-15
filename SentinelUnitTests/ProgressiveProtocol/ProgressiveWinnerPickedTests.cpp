#include "stdafx.h"
#include "TestUtil.h"
#include "ProgressiveLevelTestHelpers.h"
#include "ProgressiveProtocol/ProgressiveWinnerPicked.h"
#include "AssertMsg.h"

#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	using namespace ProgressiveLevelTestHelpers;

	static const char* pJson =
		"{"
			"\"LevelId\":1,"
			"\"LevelName\":\"2\","
			"\"WinAmount\":3,"
			"\"WinnerVenue\":\"4\","
			"\"WinnerLocation\":\"5\","
			"\"WinnerAssetNumber\":\"6\","
			"\"WinningHand\":\"7\","
			"\"WinnerDeviceId\":8,"
			"\"JackpotId\":9,"
			"\"MediaMinSeconds\":10,"
			"\"MediaMaxSeconds\":11,"
			"\"SessionId\":12,"
			"\"JackpotTriggerType\":\"PokerFrenzyTriggerMinor\""
		"}";

	static const char* pJson2 =
		"{"
			"\"LevelId\":24015,"
			"\"LevelName\":\"José\","
			"\"MediaMinSeconds\":1,"
			"\"MediaMaxSeconds\":10,"
			"\"WinnerDeviceId\":8,"
			"\"WinAmount\":100.38,"
			"\"JackpotId\":602,"
			"\"WinnerVenue\":\"\","
			"\"WinnerBank\":\"\","
			"\"WinnerLocation\":\"Joe's\","
			"\"WinnerAssetNumber\":\"55553\","
			"\"JackpotTriggerType\":\"CoinInHitTrigger\","
			"\"MethodOfPayment\":\"PayToGameWithHandPay\","
			"\"AwardType\":\"RestrictedCredits\","
			"\"SourceDeviceId\":1,"
			"\"SourceDeviceConfigVersion\":0,"
			"\"SourceAsset\":\"\","
			"\"Timestamp\":1432249767399"
		"}";

		class ProgressiveWinnerPickedTests : public ::testing::Test{

			// Minimal JSON for WinnerPicked.
			static LPCSTR const pJsonMinimalBegin;
			static LPCSTR const pJsonEnd;
			
		public:
			
			void TestMethodOfPayment(LPCSTR pJsonAdd, MethodOfPayment::Type methodOfPaymentExpected, 
				string methodOfPaymentNameExpected)
			{
				string pJson(string(pJsonMinimalBegin) + pJsonAdd + pJsonEnd);
	
				try
				{
					rapidjson::Document jsonDoc;
					if (jsonDoc.Parse(pJson.c_str()).HasParseError())
						throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");
					CProgressiveWinnerPicked picked(0, jsonDoc);
					ASSERT_EQ(methodOfPaymentExpected, picked.m_methodOfPayment) << "m_methodOfPayment";
					CProgressiveWinnerPicked picked2(picked); // test copy constructor
					ASSERT_EQ(methodOfPaymentNameExpected, picked2.MethodOfPaymentName()) << "MethodOfPaymentName";
				}
				catch (JsonException& e)
				{
					FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename());
				}
				catch (SentinelException& e)
				{
					FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what());
				}
			}
			
			void TestAwardType(LPCSTR pJsonAdd, AwardType::Type awardTypeExpected, string awardTypeNameExpected)
			{
				string pJson(string(pJsonMinimalBegin) + pJsonAdd + pJsonEnd);

				try
				{
					rapidjson::Document jsonDoc;
					if (jsonDoc.Parse(pJson.c_str()).HasParseError())
						throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");
					CProgressiveWinnerPicked picked(0, jsonDoc);
					ASSERT_EQ(awardTypeExpected, picked.m_awardType) << "m_awardType";
					ASSERT_EQ(awardTypeNameExpected, picked.AwardTypeName()) << "AwardTypeName";
				}
				catch (JsonException& e)
				{
					FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename());
				}
				catch (SentinelException& e)
				{
					FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what());
				}


			}

			OptionalVal<int> GetSessionId(CProgressiveWinnerPicked& picked)
			{
				return picked.m_sessionId;
			}

		};

		
		LPCSTR const ProgressiveWinnerPickedTests::pJsonMinimalBegin =
			"{"
			"\"LevelId\":1,"
			"\"LevelName\":\"2\","
			"\"WinAmount\":3,"
			"\"WinnerVenue\":\"4\","
			"\"WinnerLocation\":\"5\","
			"\"WinnerAssetNumber\":\"6\","
			"\"WinningHand\":\"7\","
			"\"WinnerDeviceId\":8,"
			"\"JackpotId\":9,"
			"\"MediaMinSeconds\":10,"
			"\"MediaMaxSeconds\":11,"
			"\"JackpotTriggerType\":\"CoinInTrigger\"";
		LPCSTR const ProgressiveWinnerPickedTests::pJsonEnd =
			"}";
		
		TEST_F(ProgressiveWinnerPickedTests, ProgressiveWinnerPicked_Constructor)
		{
			#if 000//TODO-PORT
			OutputDebugStringA("### pJson ###\n");
			OutputDebugStringA(pJson);
			OutputDebugStringA("\n### pJson ###\n");
			#endif
			
			try
			{
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(pJson).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

				CProgressiveWinnerPicked picked(1000, jsonDoc);

				ASSERT_EQ(1000LU		, picked.GetReceivedTickCount() ) << "m_receivedTickCount";
				ASSERT_EQ(1			, picked.GetLevelId()			) << "m_levelId";
				ASSERT_EQ(string("2")		, picked.GetLevelName()			) << "m_levelName";
				ASSERT_EQ(string("3.00")	, picked.GetWinAmount()			) << "m_winAmount";
				ASSERT_EQ(300				, picked.GetWinAmountInt64()    ) << "GetWinAmountInt64";
				ASSERT_EQ(string("4")		, picked.GetWinnerVenue()		) << "m_winnerVenue";
				ASSERT_EQ(string("5")		, picked.GetWinnerLocation()	) << "m_winnerLocation";
				ASSERT_EQ(string("6")		, picked.GetWinnerAssetNumber()	) << "m_winnerAssetNumber";
				ASSERT_EQ(string("7")		, picked.GetWinningHand()		) << "m_winningHand";
				ASSERT_EQ((int) 8	, (int)picked.GetWinnerDeviceId()) << "m_winnerDeviceId";
				ASSERT_EQ((int) 9	, (int)picked.GetJackpotId()	) << "m_jackpotId";
				ASSERT_EQ(10			, picked.GetMediaMinSeconds()	) << "m_mediaMinSeconds";
				ASSERT_EQ(11			, picked.GetMediaMaxSeconds()	) << "m_mediaMaxSeconds";
				ASSERT_TRUE(GetSessionId(picked).isValid					) << "GetSessionId().isValid";
				ASSERT_EQ(12			, GetSessionId(picked).val		) << "m_sessionId.val";
				ASSERT_EQ((int) JackpotTrigger::PokerFrenzyMinor, (int) picked.GetJackpotTriggerType()) << "m_jackpotTriggerType";
				ASSERT_EQ(MethodOfPayment::Invalid, picked.GetMethodOfPayment()) << "m_methodOfPayment";
				ASSERT_EQ(AwardType::Invalid, picked.GetAwardType()) << "m_awardType";
			}
			catch (JsonException& e)
			{
				FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename());
			}
			catch (SentinelException& e)
			{
				FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what());
			}
		}

		TEST_F(ProgressiveWinnerPickedTests, ProgressiveWinnerPicked_Constructor2)
		{
			#if 000//TODO-PORT
			OutputDebugStringA("### pJson2 ###\n");
			OutputDebugStringA(pJson2);
			OutputDebugStringA("\n### pJson2 ###\n");
			#endif
			
			try
			{
				rapidjson::Document jsonDoc;
				if (jsonDoc.Parse(pJson2).HasParseError())
					throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

				CProgressiveWinnerPicked picked(1000, jsonDoc);

				ASSERT_EQ(1000LU, picked.GetReceivedTickCount()	) << "m_receivedTickCount";
				ASSERT_EQ(24015, picked.GetLevelId()) << "m_levelId";
				string name(Convert::Utf8ToString("José"));
				ASSERT_EQ(name, picked.GetLevelName()) << "m_levelName";
				ASSERT_EQ(string("100.38"), picked.GetWinAmount()) << "m_winAmount";
				ASSERT_EQ(string(""), picked.GetWinnerVenue()) << "m_winnerVenue";
				ASSERT_EQ(string("Joe's"), picked.GetWinnerLocation()) << "m_winnerLocation";
				ASSERT_EQ(string("55553"), picked.GetWinnerAssetNumber()) << "m_winnerAssetNumber";
				ASSERT_EQ(string(""), picked.GetWinningHand()) << "m_winningHand";
				ASSERT_EQ((int)8, (int)picked.GetWinnerDeviceId()) << "m_winnerDeviceId";
				ASSERT_EQ((int)602, (int)picked.GetJackpotId()) << "m_jackpotId";
				ASSERT_EQ(1, picked.GetMediaMinSeconds()) << "m_mediaMinSeconds";
				ASSERT_EQ(10, picked.GetMediaMaxSeconds()) << "m_mediaMaxSeconds";
				ASSERT_FALSE(GetSessionId(picked).isValid					) << "m_sessionId.isValid";
				ASSERT_EQ((int)JackpotTrigger::CoinInHit, (int)picked.GetJackpotTriggerType()) << "m_jackpotTriggerType";
				ASSERT_EQ(MethodOfPayment::PayToGameWithHandPay, picked.GetMethodOfPayment()) << "m_methodOfPayment";
				ASSERT_EQ(AwardType::RestrictedCredits, picked.GetAwardType()) << "m_awardType";
				ASSERT_EQ("", picked.GetMinorAwardId()) << "GetMinorAwardId";
				ASSERT_EQ("", picked.GetExternalAwardName()) << "GetExternalAwardName";
				ASSERT_EQ("0", picked.GetExternalAwardValue()) << "GetExternalAwardValue";
				ASSERT_EQ("", picked.GetExternalAwardCurrency()) << "GetExternalAwardCurrency";
				ASSERT_EQ("", picked.GetExternalAwardId()) << "GetExternalAwardId";
				ASSERT_EQ(true, picked.ForwardToUI()) << "ForwardToUI";
			}
			catch (JsonException& e)
			{
				FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename());
			}
			catch (SentinelException& e)
			{
				FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what());
			}
		}

// #pragma region MethodOfPayment
     	TEST_F(ProgressiveWinnerPickedTests, MethodOfPayment_Default)
		{
			TestMethodOfPayment("", MethodOfPayment::Invalid, "");
		}

		TEST_F(ProgressiveWinnerPickedTests, MethodOfPayment_PayToGame)
		{
			TestMethodOfPayment(",\"MethodOfPayment\":\"PayToGame\"", MethodOfPayment::PayToGame, "PayToGame");
		}

		TEST_F(ProgressiveWinnerPickedTests, MethodOfPayment_PayToGameWithHandPay)
		{
			TestMethodOfPayment(",\"MethodOfPayment\":\"PayToGameWithHandPay\"", MethodOfPayment::PayToGameWithHandPay, 
				"PayToGameWithHandPay");
		}

		TEST_F(ProgressiveWinnerPickedTests, MethodOfPayment_PayToPlayerAccount)
		{
			TestMethodOfPayment(",\"MethodOfPayment\":\"PayToPlayerAccount\"", MethodOfPayment::PayToPlayerAccount, 
				"PayToPlayerAccount");
		}

		TEST_F(ProgressiveWinnerPickedTests, MethodOfPayment_TriggerOnly)
		{
			TestMethodOfPayment(",\"MethodOfPayment\":\"TriggerOnly\"", MethodOfPayment::TriggerOnly,
				"TriggerOnly");
		}

		TEST_F(ProgressiveWinnerPickedTests, MethodOfPayment_PayToPromoGameDevice)
		{
			TestMethodOfPayment(",\"MethodOfPayment\":\"PayToPromoGameDevice\"", MethodOfPayment::PayToPromoGameDevice,
				"PayToPromoGameDevice");
		}

		TEST_F(ProgressiveWinnerPickedTests, MethodOfPayment_Claimable)
		{
			TestMethodOfPayment(",\"MethodOfPayment\":\"Claimable\"", MethodOfPayment::Claimable,
				"Claimable");
		}
// #pragma endregion MethodOfPayment

// #pragma region AwardType
		TEST_F(ProgressiveWinnerPickedTests, AwardType_Default)
		{
			TestAwardType("", AwardType::Invalid, "");
		}

		TEST_F(ProgressiveWinnerPickedTests, AwardType_CashableCredits)
		{
			TestAwardType(",\"AwardType\":\"CashableCredits\"", AwardType::CashableCredits, "CashableCredits");
		}

		TEST_F(ProgressiveWinnerPickedTests, AwardType_RestrictedCredits)
		{
			TestAwardType(",\"AwardType\":\"RestrictedCredits\"", AwardType::RestrictedCredits, "RestrictedCredits");
		}

		TEST_F(ProgressiveWinnerPickedTests, AwardType_NonRestrictedCredits)
		{
			TestAwardType(",\"AwardType\":\"NonRestrictedCredits\"", AwardType::NonRestrictedCredits, "NonRestrictedCredits");
		}

		TEST_F(ProgressiveWinnerPickedTests, AwardType_Points)
		{
			TestAwardType(",\"AwardType\":\"Points\"", AwardType::Points, "Points");
		}

		TEST_F(ProgressiveWinnerPickedTests, AwardType_TriggerOnly)
		{
			TestAwardType(",\"AwardType\":\"TriggerOnly\"", AwardType::TriggerOnly, "TriggerOnly");
		}

		TEST_F(ProgressiveWinnerPickedTests, AwardType_Freeplay)
		{
			TestAwardType(",\"AwardType\":\"Freeplay\"", AwardType::Freeplay, "Freeplay");
		}

		TEST_F(ProgressiveWinnerPickedTests, AwardType_Comps)
		{
			TestAwardType(",\"AwardType\":\"Comps\"", AwardType::Comps, "Comps");
		}
		
		TEST_F(ProgressiveWinnerPickedTests, AwardType_Prize)
		{
			TestAwardType(",\"AwardType\":\"Prize\"", AwardType::Prize, "Prize");
		}

		TEST_F(ProgressiveWinnerPickedTests, AwardType_Offer)
		{
			TestAwardType(",\"AwardType\":\"Offer\"", AwardType::Offer, "Offer");
		}

		TEST_F(ProgressiveWinnerPickedTests, AwardType_ExtendedBucket)
		{
			TestAwardType(",\"AwardType\":\"ExtendedBucket\"", AwardType::ExtendedBucket, "ExtendedBucket");
		}
		// #pragma endregion AwardType
	};

