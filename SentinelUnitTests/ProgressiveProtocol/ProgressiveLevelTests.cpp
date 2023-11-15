#include "stdafx.h"
#include "ProgressiveLevelTestHelpers.h"
#include "ProgressiveProtocol/ProgressiveLevelBroadcast.h"
#include "Util/Convert.h"
#include "Utility/AssertMsg.h"
#include <limits>

#include "UnitTestingUtils.h"
#include "AssertMsg.h"
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	using namespace ProgressiveLevelTestHelpers;
    class ProgressiveLevelTests :  public ::testing::Test
    {
		// Class member variable(s).
		int m_testIndex;

	public:
            /// <summary>
    		/// Initialize class member(s) before each test method is run.
    		/// </summary>
            void f_Initialize(void)
            {
                // Test method initialization code.
                m_testIndex = 0;
            }

            void f_ProgressiveLevel_ctor(void)
            {
				string jsonLevelData("{\"Id\":361,\"Version\":404,\"Type\":\"Bonus\",\"Channel\":{\"Destination\":\"@239.1.1.1:61613\",\"MulticastDestination\":\"stomp://239.1.1.1:61613\"},\"Info\":\"\",\"Name\":\"LevelName\",\"MediaMinSeconds\":0,\"MediaMaxSeconds\":0,\
				\"CelebrationContent\":{\"Id\":15,\"Type\":\"Celebration\",\
				\"CompactContent\":{\"Id\":31,\"FileName\":\"Test1.swf\",\"MD5\":\"c7ac044ad45d6a2a1491cdf3b57d12ba\"},\
				\"LcdContent\":{\"Id\":31,\"FileName\":\"Test2.swf\",\"MD5\":\"c7ac044ad45d6a2a1491cdf3b57d12ba\"},\
				\"MediaWindowSideContent\":{\"Id\":31,\"FileName\":\"Test3.swf\",\"MD5\":\"c7ac044ad45d6a2a1491cdf3b57d12ba\"},\
				\"MediaWindowBottomContent\":{\"Id\":31,\"FileName\":\"Test4.swf\",\"MD5\":\"1234044ad45d6a2a1491cdf3157d12ba\"}},\"MinorAwards\":[]}");
				rapidjson::Document levelJson;
				levelJson.Parse(jsonLevelData.c_str()).HasParseError();
				int64_t id = JsonGetInt64Ex(levelJson, pKeyId);
				const char* pType = JsonGetStringEx(levelJson, pKeyType);
				ProgressiveLevelType::Type type = ProgressiveLevelType::emap.At(pType);
				ProgressiveLevelInfo levelInfo(id, type, levelJson);
				CProgressiveLevel level(levelInfo, 9876);
                ASSERT_TRUE(361 == levelInfo.m_id);
				ASSERT_TRUE(ProgressiveLevelType::Bonus == levelInfo.m_type);
				ASSERT_TRUE("LevelName" == levelInfo.m_name);
				ASSERT_TRUE("stomp://239.1.1.1:61613" == levelInfo.m_url);
				ASSERT_TRUE(levelInfo.m_minorAwards.size() == 0);
				ASSERT_TRUE(levelInfo.m_slotsContentPackages.size() == 1);
				auto slotContentPackage = levelInfo.m_slotsContentPackages.begin();
				//ASSERT_TRUE(SlotsContentType::Celebration == slotContentPackage->first);  // to correct
				ASSERT_TRUE(9876 == level.m_updatedTickCount);
            }

            void f_ProgressiveLevel_ctor2(void)
            {
				string jsonLevelData("{\"Id\":266,\"Version\":414,\"Type\":\"Bonus\",\"Channel\":{\"Destination\":\"@239.1.1.2:61613\",\"MulticastDestination\":\"stomp://239.1.1.2:61613\"},\
				\"Info\":\"Created by Bonus Model\",\"Name\":\"LevelWithMinor\",\"MediaMinSeconds\":0,\"MediaMaxSeconds\":0,\
				\"AnticipationContent\":{\"Id\":29,\"Type\":\"Anticipation\",\
					\"CompactContent\":{\"Id\":78,\"FileName\":\"Diamonds_LCD_Winner_Pick.swf\",\"MD5\":\"53b3343f48e150324b69316ecaedeef2\"},\
					\"LcdContent\":{\"Id\":78,\"FileName\":\"Diamonds_LCD_Winner_Pick.swf\",\"MD5\":\"53b3343f48e150324b69316ecaedeef2\"},\
					\"MediaWindowSideContent\":{\"Id\":67,\"FileName\":\"Diamonds_MW_Winner_Pick.swf\",\"MD5\":\"b2d181336658364350af1ae7b4723220\"},\
					\"MediaWindowBottomContent\":{\"Id\":78,\"FileName\":\"Diamonds_LCD_Winner_Pick.swf\",\"MD5\":\"53b3343f48e150324b69316ecaedeef2\"}},\
				\"CelebrationContent\":{\"Id\":30,\"Type\":\"Celebration\",\
					\"CompactContent\":{\"Id\":77,\"FileName\":\"Diamonds_LCD_Winner_Paid.swf\",\"MD5\":\"c96078cdb453d5d76422bbd18c856ff3\"},\
					\"LcdContent\":{\"Id\":77,\"FileName\":\"Diamonds_LCD_Winner_Paid.swf\",\"MD5\":\"c96078cdb453d5d76422bbd18c856ff3\"},\
					\"MediaWindowSideContent\":{\"Id\":66,\"FileName\":\"Diamonds_MW_Winner_Paid.swf\",\"MD5\":\"5253afe66a7ed1ac1262a1879367e4fc\"},\
					\"MediaWindowBottomContent\":{\"Id\":77,\"FileName\":\"Diamonds_LCD_Winner_Paid.swf\",\"MD5\":\"c96078cdb453d5d76422bbd18c856ff3\"}},\
				\"OdometerContent\":{\"Id\":31,\"Type\":\"Odometer\",\
					\"CompactContent\":{\"Id\":68,\"FileName\":\"Diamonds-Odometer.swf\",\"MD5\":\"f4bd178300f050904d3a2e28124b7711\"},\
					\"LcdContent\":{\"Id\":68,\"FileName\":\"Diamonds-Odometer.swf\",\"MD5\":\"f4bd178300f050904d3a2e28124b7711\"},\
					\"MediaWindowSideContent\":{\"Id\":68,\"FileName\":\"Diamonds-Odometer.swf\",\"MD5\":\"f4bd178300f050904d3a2e28124b7711\"},\
					\"MediaWindowBottomContent\":{\"Id\":68,\"FileName\":\"Diamonds-Odometer.swf\",\"MD5\":\"f4bd178300f050904d3a2e28124b7711\"}},\
				\"MinorAwards\":[{\"Id\":164,\"Name\":\"RandomRewards\",\"Sequence\":1,\"MeetAllCriteria\":false,\"ClaimableDuration\":0,\
				\"PromoGameContent\":{\"Id\":34,\"Type\":\"PromoGame\",\
					\"CompactContent\":{\"Id\":76,\"FileName\":\"Diamonds_LCD_Game.swf\",\"MD5\":\"391db772433d40a9e0667bbc8f1996ad\"},\
					\"LcdContent\":{\"Id\":76,\"FileName\":\"Diamonds_LCD_Game.swf\",\"MD5\":\"391db772433d40a9e0667bbc8f1996ad\"},\
					\"MediaWindowSideContent\":{\"Id\":65,\"FileName\":\"Diamonds_MW_Game.swf\",\"MD5\":\"b8b451e5f121b6e41b22b63ebe0aef0f\"},\
					\"MediaWindowBottomContent\":{\"Id\":76,\"FileName\":\"Diamonds_LCD_Game.swf\",\"MD5\":\"391db772433d40a9e0667bbc8f1996ad\"}},\
				\"CelebrationContent\":{\"Id\":30,\"Type\":\"Celebration\",\
					\"CompactContent\":{\"Id\":77,\"FileName\":\"Diamonds_LCD_Winner_Paid.swf\",\"MD5\":\"c96078cdb453d5d76422bbd18c856ff3\"},\
					\"LcdContent\":{\"Id\":77,\"FileName\":\"Diamonds_LCD_Winner_Paid.swf\",\"MD5\":\"c96078cdb453d5d76422bbd18c856ff3\"},\
					\"MediaWindowSideContent\":{\"Id\":66,\"FileName\":\"Diamonds_MW_Winner_Paid.swf\",\"MD5\":\"5253afe66a7ed1ac1262a1879367e4fc\"},\
					\"MediaWindowBottomContent\":{\"Id\":77,\"FileName\":\"Diamonds_LCD_Winner_Paid.swf\",\"MD5\":\"c96078cdb453d5d76422bbd18c856ff3\"}}}]}");

				rapidjson::Document levelJson;
				levelJson.Parse(jsonLevelData.c_str()).HasParseError();
				int64_t id = JsonGetInt64Ex(levelJson, pKeyId);
				const char* pType = JsonGetStringEx(levelJson, pKeyType);
				ProgressiveLevelType::Type type = ProgressiveLevelType::emap.At(pType);
				ProgressiveLevelInfo levelInfo(id, type, levelJson);
				CProgressiveLevel level(levelInfo, 6789);
				ASSERT_TRUE(266 == levelInfo.m_id);
				ASSERT_TRUE(ProgressiveLevelType::Bonus == levelInfo.m_type);
				ASSERT_TRUE("LevelWithMinor" == levelInfo.m_name);
				ASSERT_TRUE("stomp://239.1.1.2:61613" == levelInfo.m_url);
				ASSERT_TRUE(levelInfo.m_minorAwards.size() == 1);
				ASSERT_TRUE(levelInfo.m_slotsContentPackages.size() == 3);
				auto slotContentPackage = levelInfo.m_slotsContentPackages.begin();
				//ASSERT_TRUE(SlotsContentType::Anticipation == slotContentPackage->first);  // to correct
				ASSERT_TRUE(6789 == level.m_updatedTickCount);
            }

            void f_ProgressiveLevel_GetType(void)
            {
                CProgressiveLevel level;
                level.m_type = ProgressiveLevelType::Bonus;

                ASSERT_EQ((BYTE) ProgressiveLevelType::Bonus, (BYTE) level.GetType());
            }

            void f_ProgressiveLevel_GetID(void)
            {
                CProgressiveLevel level;
                level.m_id = 12345;

                ASSERT_EQ((uint64_t) 12345, (uint64_t) level.GetID());
            }
            void f_ProgressiveLevel_GetLogString(void)
            {
                CProgressiveLevel level;

                level.m_id = 1;
                level.m_name = "Name1";
                level.m_url = "b";
                level.m_sasLevels.push_back(2);
                level.m_value = 3;
                level.m_updatedTickCount = 5;
                level.m_state = CProgressiveLevel::Updated;
                level.m_type = ProgressiveLevelType::Standard;

                string result = level.GetLogString("Log test 1: ", nullptr, nullptr);
                ASSERT_STREQ("Log test 1: id=1 name=\"Name1\" value=3 type=0 eligible=1 show=0 url=\"b\" sasIds=2",
                result.c_str()) << "GetLogString";

                __int64 newValue = 999;
                result = level.GetLogString("Log test 2: ", nullptr, &newValue);
                ASSERT_STREQ("Log test 2: id=1 name=\"Name1\" value=3->999 type=0 eligible=1 show=0 url=\"b\" sasIds=2",
                    result.c_str()) << "GetLogString";

                level.m_value = 999;
                level.m_sasLevels.push_back(31);
                result = level.GetLogString("Log test 3: ", nullptr, nullptr);
                ASSERT_STREQ("Log test 3: id=1 name=\"Name1\" value=999 type=0 eligible=1 show=0 url=\"b\" sasIds=2,31",
                result.c_str()) << "GetLogString";

                newValue = 50;
                result = level.GetLogString("Log test 4: ", nullptr, &newValue);
                ASSERT_STREQ("Log test 4: id=1 name=\"Name1\" value=999->50 type=0 eligible=1 show=0 url=\"b\" sasIds=2,31",
                result.c_str()) << "GetLogString";

                LPCTSTR pNewName = "b";
                result = level.GetLogString("Log test 5: ", pNewName, nullptr);
                ASSERT_STREQ("Log test 5: id=1 name=\"Name1\"->\"b\" value=999 type=0 eligible=1 show=0 url=\"b\" sasIds=2,31",
                result.c_str()) << "GetLogString";

                pNewName = "Name2";
                newValue = 12345;
                result = level.GetLogString("Log test 6: ", pNewName, &newValue);
                ASSERT_STREQ("Log test 6: id=1 name=\"Name1\"->\"Name2\" value=999->12345 type=0 eligible=1 show=0 url=\"b\" sasIds=2,31",
                result.c_str()) << "GetLogString";

                level.m_state = CProgressiveLevel::State::Stale;
                level.m_name = "Johnny_)(*&^%$#@!!@@##$$%%^^&&**(())}{[]\":; '?><, -";
                result = level.GetLogString("Log test 7: ", nullptr, nullptr, CProgressiveLevel::StaleState::GoneStale);
                ASSERT_STREQ("Log test 7: id=1 name=\"Johnny_)(*&^%$#@!!@@##$$%%^^&&**(())}{[]\":; '?><, -\" value=999 type=0 eligible=1 show=0 url=\"b\" sasIds=2,31 GoneStale - Last Updated TickCount=5",
                result.c_str()) << "GetLogString";
            }

            void f_ProgressiveLevel_GetLogString_EligibilityInfo(void)
            {
                static const char* pJson =
                    "{"
                    "\"SessionId\":470,"
                    "\"PatronInformation\":{"
                    "\"CardNumber\":\"4BDC04FD39\","
                    "\"PlayerId\":1,"
                    "\"Tier\":\"Default\","
                    "\"Name\":\"patron\""
                    "},"
                    "\"EligibilityDetermined\":true,"
                    "\"EligibleLevels\":["
                    "{"
                    "\"LevelId\":123,"
                    "\"Name\":\"Pots\","
                    "\"TriggerType\":\"CoinInTriggerOnly\","
                    "\"Eligible\":true,"
                    "\"ShowOdometer\":true,"
                    "\"PercentEligible\":0.17,"
                    "\"Info\":\"info\","
                    "\"Value\":3.14,"
                    "\"LinkName\":\"link\","
                    "\"StartTimeUtc\":3600000598,"
                    "\"EndTimeUtc\":3600192837,"
                    "\"TotalAwardsToPlayer\":23.45,"
                    "\"Pte\":true,"
                    "\"ShowInfo\":true,"
                    "\"LeaderboardUrl\":\"hello\","
                    "\"Status\":\"Active\","
                    "\"ImageUrl\":\"world\","
                    "\"Minors\":["
                    "{"
                    "\"Sequence\":2,"
                    "\"Eligible\":true,"
                    "\"PercentEligible\":1.23,"
                    "\"Name\":\"4\","
                    "\"Min\":5.6,"
                    "\"Max\":7.8"
                    "},"
                    "{"
                    "\"Sequence\":1,"
                    "\"Eligible\":false,"
                    "\"PercentEligible\":3.21,"
                    "\"Name\":\"9\","
                    "\"Min\":6.5,"
                    "\"Max\":8.7"
                    "}"
                    "]"
                    "}"
                    "],"
                    "\"SourceDeviceId\":1,"
                    "\"SourceDeviceConfigVersion\":0,"
                    "\"SourceAsset\":\"\","
                    "\"Timestamp\":1433466405284"
                    "}";

                const int tzOffsetMinutes(-8 * 60);
                string expectedString1("id=123 name=\"Pots\" value=314 type=1 eligible=1 show=1 url=\"https://urly\" sasIds="
                    " EligibleLevelInfo: levelId=123 pteStatus=2 showInfo=1 isEligible=1 isPlayerQualified=0"
                    " showOdometer=1 pte=1 name=\"Pots\" info=\"info\" triggerType=\"CoinInTriggerOnly\" value=314"
                    " startTime=02/11/1970 00:00:00 endTime=02/11/1970 00:03:12 countdownStartTime=-7766279631452241719 countdownEndTime=-7766279631452241719 percentEligible=0.17 prizeDescription= awardType=");

                string expectedString2("id=123 name=\"Pots\" value=314 type=1 eligible=1 show=1 url=\"https://urly\" sasIds="
                    " EligibleLevelInfo: levelId=123 pteStatus=2 showInfo=1 isEligible=1 isPlayerQualified=0"
                    " showOdometer=1 pte=1 name=\"Pots\" info=\"info\" triggerType=\"CoinInTriggerOnly\" value=314"
                    " startTime=02/11/1970 00:00:00 endTime=02/11/1970 00:03:12 countdownStartTime=-7766279631452241719 countdownEndTime=-7766279631452241719 percentEligible=0.17 prizeDescription= awardType=");

                string expectedString3("abc: id=123 name=\"Pots\" value=314 type=1 eligible=1 show=1 url=\"https://urly\" sasIds="
                    " EligibleLevelInfo: levelId=123 pteStatus=2 showInfo=1 isEligible=1 isPlayerQualified=0"
                    " showOdometer=1 pte=1 name=\"Pots\" info=\"info\" triggerType=\"CoinInTriggerOnly\" value=314"
                    " startTime=02/11/1970 00:00:00 endTime=02/11/1970 00:03:12 countdownStartTime=-7766279631452241719 countdownEndTime=-7766279631452241719 percentEligible=0.17 prizeDescription= awardType=");

                string expectedString4("abc xyz: id=123 name=\"Pots\" value=314 type=1 eligible=1 show=1 url=\"https://urly\" sasIds="
                    " EligibleLevelInfo: levelId=123 pteStatus=2 showInfo=1 isEligible=1 isPlayerQualified=0"
                    " showOdometer=1 pte=1 name=\"Pots\" info=\"info\" triggerType=\"CoinInTriggerOnly\" value=314"
                    " startTime=02/11/1970 00:00:00 endTime=02/11/1970 00:03:12 countdownStartTime=-7766279631452241719 countdownEndTime=-7766279631452241719 percentEligible=0.17 prizeDescription= awardType=");
                try
                {
                    rapidjson::Document jsonDoc;
                    if (jsonDoc.Parse(pJson).HasParseError())
                        throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

                    const rapidjson::Value& levels = JsonGetArrayEx(jsonDoc, "EligibleLevels");
                    auto it = levels.Begin();
                    ASSERT_TRUE(it != levels.End()) << "it != levels.End()";

                    ProgressiveEligibleLevelInfo eligibleLevelInfo(123, *it);
                    CUtilities::SetTzOffset(tzOffsetMinutes);

                    CProgressiveLevel level;
                    level.m_id = 123;
                    level.m_name = "Pots";
                    level.m_url = "https://urly";
                    level.m_value = 314;
                    level.m_updatedTickCount = 5;
                    level.m_state = CProgressiveLevel::Updated;
                    level.m_type = ProgressiveLevelType::Bonus;
                    level.ProcessEligibleLevelInfo(&eligibleLevelInfo);

                    // Perform operation(s) and test(s).
                    string resultString1(level.GetLogString(nullptr));
                    ASSERT_EQ(expectedString1, resultString1) << "GetLogString() 1";

                    string resultString2(level.GetLogString(""));
                    ASSERT_EQ(expectedString2, resultString2) << "GetLogString() 2";

                    string resultString3(level.GetLogString("abc: "));
                    ASSERT_EQ(expectedString3, resultString3) << "GetLogString() 3";

                    string resultString4(level.GetLogString("abc xyz: "));
                    ASSERT_EQ(expectedString4, resultString4) << "GetLogString() 4";
                }
                catch (JsonException& e)
                {
                    FAIL() << (AssertMsg::Format(0, L"Unexpected exception", L"%u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename()));
                }
                catch (SentinelException& e)
                {
                   FAIL() << (AssertMsg::Format(0, L"Unexpected exception", L"%u: %S", e.code(), e.what()));
                }
            }


            void f_ProgressiveLevel_GetLogString_EligibilityInfo_prepte(void)
            {
                static const char* pJson =
                    "{"
                    "\"SessionId\":470,"
                    "\"PatronInformation\":{"
                    "\"CardNumber\":\"4BDC04FD39\","
                    "\"PlayerId\":1,"
                    "\"Tier\":\"Default\","
                    "\"Name\":\"patron\""
                    "},"
                    "\"EligibilityDetermined\":true,"
                    "\"EligibleLevels\":["
                    "{"
                    "\"LevelId\":987654321,"
                    "\"Name\":\"Pans\","
                    "\"TriggerType\":\"PayLineTrigger\","
                    "\"Eligible\":false,"
                    "\"ShowOdometer\":false,"
                    "\"PercentEligible\":7.65,"
                    "\"LinkName\":\"link\","
                    "\"StartTimeUtc\":36000000598,"
                    "\"EndTimeUtc\":36007654321,"
                    "\"TotalAwardsToPlayer\":98.01,"
                    "\"Minors\":["
                    "{"
                    "\"Sequence\":2"
                    "},"
                    "{"
                    "\"Sequence\":1"
                    "}"
                    "]"
                    "}"
                    "],"
                    "\"SourceDeviceId\":1,"
                    "\"SourceDeviceConfigVersion\":0,"
                    "\"SourceAsset\":\"\","
                    "\"Timestamp\":1433466405284"
                    "}";

                const int tzOffsetMinutes(-8 * 60);
                string expectedString1("id=123 name=\"Pans\" value=314 type=1 eligible=0 show=0 url=\"https://burly\" sasIds="
                    " EligibleLevelInfo: levelId=123 pteStatus=-1 showInfo=0 isEligible=0 isPlayerQualified=0"
                    " showOdometer=0 pte=0 name=\"Pans\" info=\"\" triggerType=\"PayLineTrigger\" value=0"
                    " startTime=02/21/1971 00:00:00 endTime=02/21/1971 02:07:34 countdownStartTime=-7766279631452241719 countdownEndTime=-7766279631452241719 percentEligible=7.65 prizeDescription= awardType=");

                string expectedString2("id=123 name=\"Pans\" value=314 type=1 eligible=0 show=0 url=\"https://burly\" sasIds="
                    " EligibleLevelInfo: levelId=123 pteStatus=-1 showInfo=0 isEligible=0 isPlayerQualified=0"
                    " showOdometer=0 pte=0 name=\"Pans\" info=\"\" triggerType=\"PayLineTrigger\" value=0"
                    " startTime=02/21/1971 00:00:00 endTime=02/21/1971 02:07:34 countdownStartTime=-7766279631452241719 countdownEndTime=-7766279631452241719 percentEligible=7.65 prizeDescription= awardType=");

                string expectedString3("abc: id=123 name=\"Pans\" value=314 type=1 eligible=0 show=0 url=\"https://burly\" sasIds="
                    " EligibleLevelInfo: levelId=123 pteStatus=-1 showInfo=0 isEligible=0 isPlayerQualified=0"
                    " showOdometer=0 pte=0 name=\"Pans\" info=\"\" triggerType=\"PayLineTrigger\" value=0"
                    " startTime=02/21/1971 00:00:00 endTime=02/21/1971 02:07:34 countdownStartTime=-7766279631452241719 countdownEndTime=-7766279631452241719 percentEligible=7.65 prizeDescription= awardType=");

                string expectedString4("abc xyz: id=123 name=\"Pans\" value=314 type=1 eligible=0 show=0 url=\"https://burly\" sasIds="
                    " EligibleLevelInfo: levelId=123 pteStatus=-1 showInfo=0 isEligible=0 isPlayerQualified=0"
                    " showOdometer=0 pte=0 name=\"Pans\" info=\"\" triggerType=\"PayLineTrigger\" value=0"
                    " startTime=02/21/1971 00:00:00 endTime=02/21/1971 02:07:34 countdownStartTime=-7766279631452241719 countdownEndTime=-7766279631452241719 percentEligible=7.65 prizeDescription= awardType=");

                try
                {
                    rapidjson::Document jsonDoc;
                    if (jsonDoc.Parse(pJson).HasParseError())
                        throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

                    const rapidjson::Value& levels = JsonGetArrayEx(jsonDoc, "EligibleLevels");
                    auto it = levels.Begin();
                    ASSERT_TRUE(it != levels.End()) << "it != levels.End()";

                    ProgressiveEligibleLevelInfo eligibleLevelInfo(123, *it);
                    CUtilities::SetTzOffset(tzOffsetMinutes);

                    CProgressiveLevel level;
                    level.m_id = 123;
                    level.m_name = "Pans";
                    level.m_url = "https://burly";
                    level.m_value = 314;
                    level.m_updatedTickCount = 5;
                    level.m_state = CProgressiveLevel::Updated;
                    level.m_type = ProgressiveLevelType::Bonus;
                    level.ProcessEligibleLevelInfo(&eligibleLevelInfo);

                    // Perform operation(s) and test(s).
                    string resultString1(level.GetLogString(nullptr));
                    ASSERT_EQ(expectedString1, resultString1) << "GetLogString() 1";

                    string resultString2(level.GetLogString(""));
                    ASSERT_EQ(expectedString2, resultString2) << "GetLogString() 2";

                    string resultString3(level.GetLogString("abc: "));
                    ASSERT_EQ(expectedString3, resultString3) << "GetLogString() 3";

                    string resultString4(level.GetLogString("abc xyz: "));
                    ASSERT_EQ(expectedString4, resultString4) << "GetLogString() 4";
                }
                catch (JsonException& e)
                {
                    FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %u: %S: %S: %s", e.code(), e.codestring(), e.what(), e.filename());
                }
                catch (SentinelException& e)
                {
                    FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %u: %S", e.code(), e.what());
                }
            }

			CProgressiveLevelBroadcast::Level* CreateBroadcastLevel(int64_t value) const
			{
				CProgressiveLevelBroadcast::Level *pLevel = nullptr;
				stringstream ss;
				ss << R"({"LevelValues":[{"LevelId":1,"Value":)" << value / 100 << "." << value % 100 << R"(,"Name":""}]})";

                try 
                {
                 
                    rapidjson::Document jsonDoc;
    				if (jsonDoc.Parse(ss.str().c_str()).HasParseError())
    					throw JsonException(jsonDoc.GetParseError(), nullptr, "CreateBroadcastLevel Parse");
    
    
    				pLevel = new CProgressiveLevelBroadcast::Level(JsonGetArrayEx(jsonDoc, pKeyLevelValues).Begin());
                    return pLevel;
              
                }
                catch (JsonException& e)
                {
                    cout  << " CreateBroadcastLevel  "  <<" e.code()" <<  e.code() << "  e.what()  " << e.what()  << "  e.codestring()  " << e.codestring() << "  e.filename()  " << e.filename() << endl;
                    return pLevel;
                }
			}

			void f_ProgressiveLevel_ProcessLevelBroadcast(void)
			{
				struct
				{
					int line;
					ProgressiveLevelType::Type type;
					int64_t value1;
					int64_t value2;
					bool    result1;
					bool    result2;
				}
				data[] =
				{
					{ __LINE__, ProgressiveLevelType::Standard		, 45, 44, false,  true },
					{ __LINE__, ProgressiveLevelType::Standard		, 45, 45, false, false },
					{ __LINE__, ProgressiveLevelType::Standard		, 45, 46, false, false },
					{ __LINE__, ProgressiveLevelType::Bonus			, 45, 44, false, false },
					{ __LINE__, ProgressiveLevelType::Bonus			, 45, 45, false, false },
					{ __LINE__, ProgressiveLevelType::Bonus			, 45, 46, false, false },
					{ __LINE__, ProgressiveLevelType::AccountingOnly, 45, 44, false, false },
					{ __LINE__, ProgressiveLevelType::AccountingOnly, 45, 45, false, false },
					{ __LINE__, ProgressiveLevelType::AccountingOnly, 45, 46, false, false }
				};

				for (const auto& item : data)
				{
					CProgressiveLevel level;
					level.m_type = item.type;

					CProgressiveLevelBroadcast::Level *broadcastLevel = CreateBroadcastLevel(item.value1);

					ASSERT_EQ(item.result1, level.ProcessLevelBroadcast(*broadcastLevel, 1000)) << "return 1";
					ASSERT_EQ((uint64_t)item.value1, (uint64_t)level.m_value) << "level.m_value 1";

					broadcastLevel = CreateBroadcastLevel(item.value2);

					ASSERT_EQ(item.result2, level.ProcessLevelBroadcast(*broadcastLevel, 1000)) << "return 2";
					ASSERT_EQ((uint64_t)item.value2, (uint64_t)level.m_value) << "level.m_value 2";
				}
			}

			CProgressiveLevel::State CalcStateAfterCallingProcessForward(
				CProgressiveLevel &level,
				CProgressiveLevel::State stateExpected, // State expected prior to calling ProcessForward().
				CProgressiveLevelForward *progressiveLevelForward)
			{
				CProgressiveLevel::State forwardedStateExpected = stateExpected;

				if (progressiveLevelForward != nullptr && CProgressiveLevel::Updated == stateExpected)
				{
					if (ProgressiveLevelType::Bonus == level.GetType() ||
						(ProgressiveLevelType::Standard == level.GetType() && (level.m_sasLevels.size() > 0)))
					{
						forwardedStateExpected = CProgressiveLevel::Forwarded;
					}
				}

				return forwardedStateExpected;
			}

			void VerifyMulticastActive(
				CProgressiveLevel &level,
				DWORD updatedTickCount,
				CProgressiveLevel::State stateExpected,
				CProgressiveLevelForward *progressiveLevelForward)
			{
				ASSERT_EQ(updatedTickCount, level.m_updatedTickCount) << "m_updatedTickCount";
				ASSERT_EQ((int)stateExpected, (int)level.m_state) << "m_state";

				CProgressiveLevel::State forwardedStateExpected =
					CalcStateAfterCallingProcessForward(level, stateExpected, progressiveLevelForward);

				struct
				{
					int   line;
					DWORD tickCountToAdd;
					int   state;
					bool  multicastActive;
				}
				data[] =
				{
					{ __LINE__, CProgressiveLevelForward::MaxInterval / 3, -1, true },
					{ __LINE__, CProgressiveLevelForward::MaxInterval / 2, -1, true },
					{ __LINE__, CProgressiveLevelForward::MaxInterval, -1, true },
					{ __LINE__, CProgressiveLevelForward::MaxInterval + 1, CProgressiveLevel::Stale, false },
					{ __LINE__, CProgressiveLevelForward::MaxInterval + CProgressiveLevelForward::MinInterval, CProgressiveLevel::Stale, false },
					{ __LINE__, CProgressiveLevelForward::MaxInterval + CProgressiveLevelForward::MinInterval * 2, CProgressiveLevel::Stale, false },
					{ __LINE__, 0, CProgressiveLevel::Stale, false },
					{ __LINE__, CProgressiveLevelForward::MaxInterval / 2, CProgressiveLevel::Stale, false }
				};

				for (const auto& item : data)
				{
					bool multicastActive = level.ProcessForward(updatedTickCount + item.tickCountToAdd, progressiveLevelForward);
					ASSERT_EQ(item.multicastActive, multicastActive) << AssertMsg::Format(item.line) << "multicastActive (return)";

					if (item.state < 0)
					{
						ASSERT_EQ((int)forwardedStateExpected, (int)level.m_state) << AssertMsg::Format(item.line) << "level.m_state";
					}
					else
					{
						ASSERT_EQ((int)item.state, (int)level.m_state) << AssertMsg::Format(item.line) << "level.m_state";
					}
				}
			}

			void f_ProgressiveLevel_ProcessForward_nullptr(void)
			{
				struct
				{
					int line;
					ProgressiveLevelType::Type type;
					DWORD updatedTickCount;
					CProgressiveLevel::State stateExpected;

				}
				data[] =
				{
					{ __LINE__, ProgressiveLevelType::Standard		, 10000, CProgressiveLevel::Configured },
					{ __LINE__, ProgressiveLevelType::Bonus			, 10000, CProgressiveLevel::Configured },
					{ __LINE__, ProgressiveLevelType::AccountingOnly, 10000, CProgressiveLevel::Configured },
					{ __LINE__, ProgressiveLevelType::Invalid		, 10000, CProgressiveLevel::Configured }
				};

				for (const auto& item : data)
				{
					CProgressiveLevel level;
					level.m_id = 1;
					level.m_type = item.type;
					level.m_updatedTickCount = item.updatedTickCount;

					VerifyMulticastActive(level, item.updatedTickCount, item.stateExpected, nullptr);
				}
			}

			void f_ProgressiveLevel_ProcessForward(void)
			{
				struct
				{
					int line;
					ProgressiveLevelType::Type type;
					DWORD updatedTickCount;
					CProgressiveLevel::State stateExpected;

				}
				data[] =
				{
					{ __LINE__, ProgressiveLevelType::Standard		, 10000, CProgressiveLevel::Configured },
					{ __LINE__, ProgressiveLevelType::Bonus			, 10000, CProgressiveLevel::Configured },
					{ __LINE__, ProgressiveLevelType::AccountingOnly, 10000, CProgressiveLevel::Configured },
					{ __LINE__, ProgressiveLevelType::Invalid		, 10000, CProgressiveLevel::Configured }
				};

				for (const auto& item : data)
				{
					CProgressiveLevel level;
					level.m_id = 1;
					level.m_type = item.type;
					level.m_updatedTickCount = item.updatedTickCount;
					level.m_sasLevels.push_back(4);

					CProgressiveLevelForward levelForward(1, false);

					VerifyMulticastActive(level, item.updatedTickCount, item.stateExpected, &levelForward);
				}
			}

			void f_ProgressiveLevel_ProcessForward_nullptr2(void)
			{
				struct
				{
					int line;
					ProgressiveLevelType::Type type;
					DWORD updatedTickCount;
					CProgressiveLevel::State stateExpected;

				}
				data[] =
				{
					{ __LINE__, ProgressiveLevelType::Standard		, CProgressiveLevelForward::MinInterval, CProgressiveLevel::Updated },
					{ __LINE__, ProgressiveLevelType::Bonus			, CProgressiveLevelForward::MinInterval, CProgressiveLevel::Updated },
					{ __LINE__, ProgressiveLevelType::AccountingOnly, CProgressiveLevelForward::MinInterval, CProgressiveLevel::Updated },
					{ __LINE__, ProgressiveLevelType::Invalid		, CProgressiveLevelForward::MinInterval, CProgressiveLevel::Updated }
				};

				for (const auto& item : data)
				{
					CProgressiveLevel level;
					level.m_id = 1;
					level.m_type = item.type;
					level.m_updatedTickCount = 10000;

					DWORD updatedTickCount = level.m_updatedTickCount + item.updatedTickCount;

					unique_ptr<CProgressiveLevelBroadcast::Level> broadcastLevel(CreateBroadcastLevel(12345));
					level.ProcessLevelBroadcast(*broadcastLevel, updatedTickCount);
					VerifyMulticastActive(level, updatedTickCount, item.stateExpected, nullptr);
				}
			}

			void f_ProgressiveLevel_ProcessForward_2(void)
			{
				struct
				{
					int line;
					ProgressiveLevelType::Type type;
					DWORD updatedTickCount;
					CProgressiveLevel::State stateExpected;

				}
				data[] =
				{
					{ __LINE__, ProgressiveLevelType::Standard		, CProgressiveLevelForward::MinInterval, CProgressiveLevel::Updated },
					{ __LINE__, ProgressiveLevelType::Bonus			, CProgressiveLevelForward::MinInterval, CProgressiveLevel::Updated },
					{ __LINE__, ProgressiveLevelType::AccountingOnly, CProgressiveLevelForward::MinInterval, CProgressiveLevel::Updated },
					{ __LINE__, ProgressiveLevelType::Invalid		, CProgressiveLevelForward::MinInterval, CProgressiveLevel::Updated }
				};

				for (const auto& item : data)
				{
					CProgressiveLevel level;
					level.m_id = 1;
					level.m_type = item.type;
					level.m_updatedTickCount = 10000;

					DWORD updatedTickCount = level.m_updatedTickCount + item.updatedTickCount;

					unique_ptr<CProgressiveLevelBroadcast::Level> broadcastLevel(CreateBroadcastLevel(12345));
					level.ProcessLevelBroadcast(*broadcastLevel, updatedTickCount);

					CProgressiveLevelForward levelForward(1, false);
					VerifyMulticastActive(level, updatedTickCount, item.stateExpected, &levelForward);
				}
			}

			void f_ProgressiveLevel_ProcessForward_mixednullptr(void)
			{
				struct
				{
					int line;
					ProgressiveLevelType::Type type;
					DWORD updatedTickCount;
					DWORD ticksToAdd;
					CProgressiveLevel::State stateExpected;

				}
				data[] =
				{
					// { __LINE__, ProgressiveLevelType::Standard		, CProgressiveLevelForward::MinInterval, CProgressiveLevelForward::MaxInterval / 5, CProgressiveLevel::Updated },
					{ __LINE__, ProgressiveLevelType::Bonus			, CProgressiveLevelForward::MinInterval, CProgressiveLevelForward::MaxInterval / 5, CProgressiveLevel::Updated }
					//{ __LINE__, ProgressiveLevelType::AccountingOnly, CProgressiveLevelForward::MinInterval, CProgressiveLevelForward::MaxInterval / 5, CProgressiveLevel::Updated },
					//  { __LINE__, ProgressiveLevelType::Invalid		, CProgressiveLevelForward::MinInterval, CProgressiveLevelForward::MaxInterval / 5, CProgressiveLevel::Updated }
				};

				for (const auto& item : data)
				{
					CProgressiveLevel level;
					level.m_id = 1;
					level.m_type = item.type;
					level.m_updatedTickCount = 10000;

					DWORD updatedTickCount = level.m_updatedTickCount + item.updatedTickCount;

					unique_ptr<CProgressiveLevelBroadcast::Level> broadcastLevel(CreateBroadcastLevel(12345));
					level.ProcessLevelBroadcast(*broadcastLevel, updatedTickCount);

					CProgressiveLevelForward progressiveLevelForward(1, false);

					level.ProcessLevelBroadcast(*broadcastLevel, updatedTickCount);

					bool multicastActive = level.ProcessForward(updatedTickCount + item.ticksToAdd, &progressiveLevelForward);
					ASSERT_EQ(true, multicastActive) << "ProcessForward";

					CProgressiveLevel::State stateExpected = item.stateExpected;
					stateExpected = CalcStateAfterCallingProcessForward(level, stateExpected, &progressiveLevelForward);
					ASSERT_EQ((int)stateExpected, (int)level.m_state) << "level.m_state";

					VerifyMulticastActive(level, updatedTickCount, stateExpected, nullptr);
				}
			}

			void f_ProgressiveLevel_ProcessForward_mixed2(void)
			{
				struct
				{
					int line;
					ProgressiveLevelType::Type type;
					DWORD updatedTickCount;
					DWORD ticksToAdd;
					CProgressiveLevel::State stateExpected;

				}
				data[] =
				{
					{ __LINE__, ProgressiveLevelType::Standard		, CProgressiveLevelForward::MinInterval, CProgressiveLevelForward::MaxInterval / 5, CProgressiveLevel::Updated },
					{ __LINE__, ProgressiveLevelType::Bonus			, CProgressiveLevelForward::MinInterval, CProgressiveLevelForward::MaxInterval / 5, CProgressiveLevel::Updated },
					{ __LINE__, ProgressiveLevelType::AccountingOnly, CProgressiveLevelForward::MinInterval, CProgressiveLevelForward::MaxInterval / 5, CProgressiveLevel::Updated },
					{ __LINE__, ProgressiveLevelType::Invalid		, CProgressiveLevelForward::MinInterval, CProgressiveLevelForward::MaxInterval / 5, CProgressiveLevel::Updated }
				};

				for (const auto& item : data)
				{
					CProgressiveLevel level;
					level.m_id = 1;
					level.m_type = item.type;
					level.m_updatedTickCount = 10000;

					DWORD updatedTickCount = level.m_updatedTickCount + item.updatedTickCount;

					unique_ptr<CProgressiveLevelBroadcast::Level> broadcastLevel(CreateBroadcastLevel(12345));
					level.ProcessLevelBroadcast(*broadcastLevel, updatedTickCount);

					CProgressiveLevelForward progressiveLevelForward(1, false);
					level.ProcessLevelBroadcast(*broadcastLevel, updatedTickCount);

					bool multicastActive = level.ProcessForward(updatedTickCount + item.ticksToAdd, &progressiveLevelForward);
					ASSERT_EQ(true, multicastActive) << "ProcessForward";

					CProgressiveLevel::State stateExpected = item.stateExpected;
					stateExpected = CalcStateAfterCallingProcessForward(level, stateExpected, &progressiveLevelForward);
					ASSERT_EQ((int)stateExpected, (int)level.m_state) << "level.m_state";

					CProgressiveLevelForward progressiveLevelForward2(1, false);
					VerifyMulticastActive(level, updatedTickCount, stateExpected, &progressiveLevelForward2);
				}
			}

#if 000
			void f_ProcessForward_AddLevel_LevelId(void)
			{
				// Data.
				LPCSTR const levelName(" abc xyz ");
				const __int64 amount(12345);
				const ProgressiveLevelType::Type type(ProgressiveLevelType::Bonus);
				const DWORD createdTickCount(10000 - CProgressiveLevelForward::StrobeInterval);
				const DWORD updatedTickCount(10000);
				const DWORD forwardTickCount(10000 + CProgressiveLevelForward::StrobeInterval);
				const int progressiveGroupId(2);
				const bool useAftLockOnTransfers(false);

                vector<int64_t> testReader = {0,1,2,6,8,-6,-8 ,255,1000000, 10000000};
                for(int testValue : testReader)
                {
                    const int64_t levelId(testValue);
    				CProgressiveLevel level;

                    level.m_id = testValue;
                    level.m_type = type;
                    level.m_updatedTickCount = createdTickCount;

                    CProgressiveLevelBroadcast::Level* broadcastLevel(CreateBroadcastLevel(amount));
                    level.ProcessLevelBroadcast(*broadcastLevel, updatedTickCount);

                    CProgressiveLevelForward levelForward(progressiveGroupId, useAftLockOnTransfers);

                    level.ProcessForward(forwardTickCount, &levelForward);
                    auto it = levelForward.cbegin();
    				ASSERT_EQ(true, it != levelForward.cend());
                    ASSERT_EQ((uint64_t) levelId, (uint64_t) it->levelId);

                    level.m_id = testValue+1;
    				level.m_type = ProgressiveLevelType::Standard;
    				level.m_updatedTickCount = createdTickCount + 5;
    				level.m_sasLevels.push_back(6);

    				broadcastLevel = CreateBroadcastLevel(amount);
    				level.ProcessLevelBroadcast(*broadcastLevel, updatedTickCount);

    				level.ProcessForward(forwardTickCount, &levelForward);
    				it = levelForward.cbegin();
    				it++;
    				ASSERT_EQ(true, it != levelForward.cend());
                }
			}

			void f_ProcessForward_AddLevel_LevelName(void)
			{
				// Data.
				const __int64 levelId(11);
				const __int64 amount(12345);
				const ProgressiveLevelType::Type type(ProgressiveLevelType::Bonus);
				const DWORD createdTickCount(10000 - CProgressiveLevelForward::StrobeInterval);
				const DWORD updatedTickCount(10000);
				const DWORD forwardTickCount(10000 + CProgressiveLevelForward::StrobeInterval);
				const int progressiveGroupId(2);
				const bool useAftLockOnTransfers(false);

				string jsonLevelData("{\"Id\":361,\"Version\":404,\"Type\":\"Bonus\",\"Channel\":{\"Destination\":\"@239.1.1.1:61613\",\"MulticastDestination\":\"stomp://239.1.1.1:61613\"},\"Info\":\"\",\"Name\":\"LevelName\",\"MediaMinSeconds\":0,\"MediaMaxSeconds\":0,\
				\"CelebrationContent\":{\"Id\":15,\"Type\":\"Celebration\",\
				\"CompactContent\":{\"Id\":31,\"FileName\":\"Test1.swf\",\"MD5\":\"c7ac044ad45d6a2a1491cdf3b57d12ba\"},\
				\"LcdContent\":{\"Id\":31,\"FileName\":\"Test2.swf\",\"MD5\":\"c7ac044ad45d6a2a1491cdf3b57d12ba\"},\
				\"MediaWindowSideContent\":{\"Id\":31,\"FileName\":\"Test3.swf\",\"MD5\":\"c7ac044ad45d6a2a1491cdf3b57d12ba\"},\
				\"MediaWindowBottomContent\":{\"Id\":31,\"FileName\":\"Test4.swf\",\"MD5\":\"1234044ad45d6a2a1491cdf3157d12ba\"}},\"MinorAwards\":[]}");
				rapidjson::Document levelJson;
				levelJson.Parse(jsonLevelData.c_str()).HasParseError();
				int64_t id = JsonGetInt64Ex(levelJson, pKeyId);
				const char* pType = JsonGetStringEx(levelJson, pKeyType);
				ProgressiveLevelType::Type type = ProgressiveLevelType::emap.At(pType);
				ProgressiveLevelInfo levelInfo(id, type, levelJson);
					std::map<std::string, CSlotsContentFile> slotsContentFiles;
    				CProgressiveLevel    level(levelInfo, createdTickCount, slotsContentFiles);
    				unique_ptr<CProgressiveLevelBroadcast::Level> broadcastLevel(CreateBroadcastLevel(12));
    				level.ProcessLevelBroadcast(*broadcastLevel, updatedTickCount);
    				ASSERT_STREQ(level.m_name.c_str(), levelName.c_str()); // Verify setup.
    
    				CProgressiveLevelForward levelForward(progressiveGroupId, useAftLockOnTransfers);
    				ASSERT_EQ(levelForward.cbegin() == levelForward.cend(), true); // Verify setup.
    															   // Perform operation(s) and test(s).
    				level.ProcessForward(forwardTickCount, &levelForward);
    				auto it = levelForward.cbegin();
    				ASSERT_EQ(it != levelForward.cend(), true);
    				ASSERT_STREQ(it->levelName.c_str(), levelName.c_str());
			}
#endif
			void f_ProcessForward_AddLevel_Eligible(void)
			{
				// Data.
				string jsonLevelData("{\"LevelId\":11,\"Value\":0.02,\"Name\":\"abc xyz\",\"CountdownStartTime\":1557864000000,\"CountdownEndTime\":1557907200000}");
				rapidjson::Document levelJson;
				levelJson.Parse(jsonLevelData.c_str()).HasParseError();
				const __int64 levelId(11);
				LPCTSTR levelName = " abc xyz ";
				const int64_t amount(12345);
				const ProgressiveLevelType::Type type(ProgressiveLevelType::Bonus);
				const DWORD createdTickCount(10000 - CProgressiveLevelForward::StrobeInterval);
				const DWORD updatedTickCount(10000);
				const DWORD forwardTickCount(10000 + CProgressiveLevelForward::StrobeInterval);
				const int progressiveGroupId(2);
				const bool useAftLockOnTransfers(false);
				const int sessionId(3456789);

				CProgressiveLevel level;
				level.m_id = levelId;
				level.m_type = type;
				level.m_updatedTickCount = createdTickCount;
				level.m_sasLevels.push_back(6);

				bool eligible = level.IsEligible();

				CProgressiveLevelBroadcast::Level broadcastLevel(&levelJson);
				CProgressiveLevelForward levelForward(progressiveGroupId, useAftLockOnTransfers);
				level.ProcessLevelBroadcast(broadcastLevel, updatedTickCount);

				// Perform operation(s) and test(s).
				level.ProcessForward(forwardTickCount, &levelForward);
				auto it = levelForward.cbegin();
				ASSERT_EQ(true, it != levelForward.cend());
				ASSERT_EQ(eligible, it->eligible);

			}

			void f_ProcessForward_AddLevel_ShowOdometer(void)
			{
				// Data.
				string jsonLevelData("{\"LevelId\":11,\"Value\":0.02,\"Name\":\"abc xyz\",\"CountdownStartTime\":1557864000000,\"CountdownEndTime\":1557907200000}");
				rapidjson::Document levelJson;
				levelJson.Parse(jsonLevelData.c_str()).HasParseError();
				const __int64 levelId(11);
				LPCTSTR levelName = " abc xyz ";
				const int64_t amount(12345);
				const ProgressiveLevelType::Type type(ProgressiveLevelType::Bonus);
				const DWORD createdTickCount(10000 - CProgressiveLevelForward::StrobeInterval);
				const DWORD updatedTickCount(10000);
				const DWORD forwardTickCount(10000 + CProgressiveLevelForward::StrobeInterval);
				const int progressiveGroupId(2);
				const bool useAftLockOnTransfers(false);
				const int sessionId(3456789);

				CProgressiveLevel level;
				level.m_id = levelId;
				level.m_type = type;
				level.m_updatedTickCount = createdTickCount;

				bool showOdometer = level.IsEligible();

				CProgressiveLevelBroadcast::Level broadcastLevel(&levelJson);
				CProgressiveLevelForward levelForward(progressiveGroupId, useAftLockOnTransfers);
				level.ProcessLevelBroadcast(broadcastLevel, updatedTickCount);

				// Perform operation(s) and test(s).
				level.ProcessForward(forwardTickCount, &levelForward);
				auto it = levelForward.cbegin();
				ASSERT_EQ(true, it != levelForward.cend());
				ASSERT_EQ(showOdometer, it->showOdometer);

			}

			void f_ProcessForward_AddLevel_Amount(void)
			{
				// Data.
				string jsonLevelData("{\"LevelId\":11,\"Value\":5.2,\"Name\":\"abc xyz\",\"CountdownStartTime\":1557864000000,\"CountdownEndTime\":1557907200000}");
				rapidjson::Document levelJson;
				levelJson.Parse(jsonLevelData.c_str()).HasParseError();
				const int64_t levelId(11);
				LPCTSTR levelName = " abc xyz ";
				const ProgressiveLevelType::Type type(ProgressiveLevelType::Bonus);
				const DWORD createdTickCount(10000 - CProgressiveLevelForward::StrobeInterval);
				const DWORD updatedTickCount(10000);
				const DWORD forwardTickCount(10000 + CProgressiveLevelForward::StrobeInterval);
				const int progressiveGroupId(2);
				const bool useAftLockOnTransfers(false);

				const __int64 amount(520);
    			CProgressiveLevel level;
    			level.m_id = levelId;
    			level.m_type = type;
    			level.m_updatedTickCount = createdTickCount;
    			level.m_sasLevels.push_back(6);
    
				CProgressiveLevelBroadcast::Level broadcastLevel(&levelJson);
    			CProgressiveLevelForward levelForward(progressiveGroupId, useAftLockOnTransfers);
    			level.ProcessLevelBroadcast(broadcastLevel, updatedTickCount);
    
    			// Perform operation(s) and test(s).
    			level.ProcessForward(forwardTickCount, &levelForward);
    			auto it = levelForward.cbegin();
    			ASSERT_EQ(true, it != levelForward.cend());
    			ASSERT_EQ((uint64_t)amount, (uint64_t)it->amount);
			}

			void f_ProcessForward_AddLevel_Type(void)
			{
				// Data.
				string jsonLevelData("{\"LevelId\":11,\"Value\":0.02,\"Name\":\"abc xyz\",\"CountdownStartTime\":1557864000000,\"CountdownEndTime\":1557907200000}");
				rapidjson::Document levelJson;
				levelJson.Parse(jsonLevelData.c_str()).HasParseError();
				const __int64 levelId(11);
				LPCTSTR levelName = " abc xyz ";
				const int64_t amount(12345);
				const ProgressiveLevelType::Type type(ProgressiveLevelType::Bonus);
				const DWORD createdTickCount(10000 - CProgressiveLevelForward::StrobeInterval);
				const DWORD updatedTickCount(10000);
				const DWORD forwardTickCount(10000 + CProgressiveLevelForward::StrobeInterval);
				const int progressiveGroupId(2);
				const bool useAftLockOnTransfers(false);
				const int sessionId(3456789);

                CProgressiveLevel level;
                level.m_id = levelId;
                level.m_type = type;
                level.m_updatedTickCount = createdTickCount;
                level.m_sasLevels.push_back(6);

				CProgressiveLevelBroadcast::Level broadcastLevel(&levelJson);
                level.ProcessLevelBroadcast(broadcastLevel, updatedTickCount);

                CProgressiveLevelForward levelForward(progressiveGroupId, useAftLockOnTransfers);

                // Perform operation(s) and test(s).
                level.ProcessForward(forwardTickCount, &levelForward);
                auto it = levelForward.cbegin();
                ASSERT_EQ(true, it != levelForward.cend());
                ASSERT_EQ((int)type, (int)it->type);

			}

			void f_ProcessForward_AddLevel_UpdatedTickCount(void)
			{
				// Data.
				string jsonLevelData("{\"LevelId\":11,\"Value\":0.02,\"Name\":\"abc xyz\",\"CountdownStartTime\":1557864000000,\"CountdownEndTime\":1557907200000}");
				rapidjson::Document levelJson;
				levelJson.Parse(jsonLevelData.c_str()).HasParseError();
				const __int64 levelId(11);
				LPCTSTR levelName = " abc xyz ";
				const int64_t amount(12345);
				const ProgressiveLevelType::Type type(ProgressiveLevelType::Bonus);
				const int progressiveGroupId(2);
				const bool useAftLockOnTransfers(false);

                vector<DWORD> testReader = {0, 1, 2, 6, 8, 255, 1000000, 10000000, 01000, 000011111,  UINT_MAX, UINT_MAX + 2};
                for(auto testValue : testReader)
                {
    				const DWORD createdTickCount(testValue - CProgressiveLevelForward::StrobeInterval);
    				const DWORD updatedTickCount(testValue);
    				const DWORD forwardTickCount(testValue + CProgressiveLevelForward::StrobeInterval);
    
    				// Setup.
    				CProgressiveLevel level;
    				level.m_id = levelId;
    				level.m_type = type;
    				level.m_updatedTickCount = createdTickCount;
    				level.m_sasLevels.push_back(6);
    
					CProgressiveLevelBroadcast::Level broadcastLevel(&levelJson);
    				level.ProcessLevelBroadcast(broadcastLevel, updatedTickCount);
    
    				CProgressiveLevelForward levelForward(progressiveGroupId, useAftLockOnTransfers);
    
    				// Perform operation(s) and test(s).
    				level.ProcessForward(forwardTickCount, &levelForward);
    				auto it = levelForward.cbegin();
    				ASSERT_EQ(true, it != levelForward.cend());
    				ASSERT_EQ(updatedTickCount, it->updatedTickCount);
                }

			}
    };


		TEST_F(ProgressiveLevelTests, Initialize)
		{
            f_Initialize();
		}

		TEST_F(ProgressiveLevelTests, ProgressiveLevel_ctor)
		{
            f_ProgressiveLevel_ctor();
        }

		TEST_F(ProgressiveLevelTests, ProgressiveLevel_ctor2)
		{
            f_ProgressiveLevel_ctor2();
        }

		TEST_F(ProgressiveLevelTests, ProgressiveLevel_GetType)
		{
            f_ProgressiveLevel_GetType();
        }

		TEST_F(ProgressiveLevelTests, ProgressiveLevel_GetID)
		{
            f_ProgressiveLevel_GetID();
		}

		TEST_F(ProgressiveLevelTests, ProgressiveLevel_GetLogString)
		{
            f_ProgressiveLevel_GetLogString();
        }


		TEST_F(ProgressiveLevelTests, ProgressiveLevel_GetLogString_EligibilityInfo)
		{
			f_ProgressiveLevel_GetLogString_EligibilityInfo();
		}

		TEST_F(ProgressiveLevelTests, ProgressiveLevel_GetLogString_EligibilityInfo_prepte)
		{
			f_ProgressiveLevel_GetLogString_EligibilityInfo_prepte();
		}

		TEST_F(ProgressiveLevelTests, ProgressiveLevel_ProcessForward_nullptr)
		{
			f_ProgressiveLevel_ProcessForward_nullptr();
		}

		TEST_F(ProgressiveLevelTests, ProgressiveLevel_ProcessForward)
		{
			f_ProgressiveLevel_ProcessForward();
		}

		TEST_F(ProgressiveLevelTests, ProgressiveLevel_ProcessForward_nullptr2)
		{
			f_ProgressiveLevel_ProcessForward_nullptr2();
		}

		TEST_F(ProgressiveLevelTests, ProgressiveLevel_ProcessForward_2)
		{
			f_ProgressiveLevel_ProcessForward_2();
		}

		TEST_F(ProgressiveLevelTests, ProgressiveLevel_ProcessForward_mixednullptr)
		{
			f_ProgressiveLevel_ProcessForward_mixednullptr();
		}

		TEST_F(ProgressiveLevelTests, ProgressiveLevel_ProcessForward_mixed2)
		{
			f_ProgressiveLevel_ProcessForward_mixed2();
		}

		TEST_F(ProgressiveLevelTests, ProgressiveLevel_ProcessLevelBroadcast)
		{
			f_ProgressiveLevel_ProcessLevelBroadcast();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevel::ProcessForward()
		///     CProgressiveLevelForward::AddLevel(__int64, ...)
		/// Test for various eligible values.
		/// </summary>
		TEST_F(ProgressiveLevelTests, ProcessForward_AddLevel_Eligible)
		{
			f_ProcessForward_AddLevel_Eligible();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevel::ProcessForward()
		///     CProgressiveLevelForward::AddLevel(__int64, ...)
		/// Test for various showOdometer values.
		/// </summary>
		TEST_F(ProgressiveLevelTests, ProcessForward_AddLevel_ShowOdometer)
		{
			f_ProcessForward_AddLevel_ShowOdometer();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevel::ProcessForward()
		///     CProgressiveLevelForward::AddLevel(__int64, ...)
		/// Test for various amount values.
		/// </summary>
		TEST_F(ProgressiveLevelTests, ProcessForward_AddLevel_Amount)
		{
			f_ProcessForward_AddLevel_Amount();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevel::ProcessForward()
		///     CProgressiveLevelForward::AddLevel(__int64, ...)
		/// Test for the Bonus type value.
		/// </summary>
		TEST_F(ProgressiveLevelTests, ProcessForward_AddLevel_Type)
		{
			f_ProcessForward_AddLevel_Type();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevel::ProcessForward()
		///     CProgressiveLevelForward::AddLevel(__int64, ...)
		/// Test for various updatedTickCount values.
		/// </summary>
		TEST_F(ProgressiveLevelTests, ProcessForward_AddLevel_UpdatedTickCount)
		{
			f_ProcessForward_AddLevel_UpdatedTickCount();
		}

#if 000
		TEST_F(ProgressiveLevelTests, ProcessForward_AddLevel_LevelId)
		{
			f_ProcessForward_AddLevel_LevelId();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevel::ProcessForward()
		///     CProgressiveLevelForward::AddLevel(__int64, ...)
		/// Test for various updatedTickCount values.
		/// </summary>
		TEST_F(ProgressiveLevelTests, ProcessForward_AddLevel_LevelName)
		{
			f_ProcessForward_AddLevel_LevelName();
		}
#endif
};

