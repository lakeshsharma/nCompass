#include "stdafx.h"
#include <ProgressiveProtocol/ProgressiveEligibleLevelInfo.h>
#include "AssertMsg.h"
#include "ProgressiveLevelTestHelpers.h"

#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
    void CheckMinor(const ProgressiveEligibleMinorInfo& minor)
    {
        struct
        {
            LPCTSTR pName;
            double  percentEligible;
            int64_t _min;
            int64_t _max;
            int     sequence;
            bool    isEligible;
        }
        static data[] =
        {
            { "9", 3.21, 650, 870, 1, false },
            { "4", 1.23, 560, 780, 2,  true }
        };

        for (size_t i = 0; i < _countof(data); i++)
        {
            if (minor.GetSequence() == data[i].sequence)
            {
                ASSERT_STREQ(data[i].pName          , minor.GetName().c_str()           ) << "GetName()";
                ASSERT_EQ(data[i].percentEligible, minor.GetPercentEligible()) << "GetPercentEligible()";
                ASSERT_EQ((int)data[i]._min      , (int)minor.GetMin()       ) << "GetMin()"            ;
                ASSERT_EQ((int)data[i]._max      , (int)minor.GetMax()       ) << "GetMax()"            ;
                ASSERT_EQ(data[i].isEligible     , minor.IsEligible()        ) << "IsEligible()"        ;
                return;
            }
        }

        FAIL()<< AssertMsg::FormatOnly(L"sequence == %llu", minor.GetSequence());
    }


    TEST(ProgressiveEligibleLevelInfoTests, ProgressiveEligibleLevelInfo_ctor)
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
                        "\"Name\":\"45\","
                        "\"TriggerType\":\"CoinInTriggerOnly\","
                        "\"Eligible\":true,"
                        "\"ShowOdometer\":true,"
                        "\"PercentEligible\":0.17,"
                        "\"Info\":\"info\","
                        "\"Value\":3.14,"
                        "\"LinkName\":\"link\","
                        "\"StartTimeUtc\":598,"
                        "\"EndTimeUtc\":192837,"
                        "\"TotalAwardsToPlayer\":23.45,"
                        "\"Pte\":true,"
                        "\"ShowInfo\":true,"
                        "\"LeaderboardUrl\":\"hello\","
                        "\"Status\":\"Active\","
                        "\"ImageUrl\":\"world\","
                        "\"Name\":\"4\","
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

        try
        {
            rapidjson::Document jsonDoc;
            if (jsonDoc.Parse(pJson).HasParseError())
                throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

            const rapidjson::Value& levels = JsonGetArrayEx(jsonDoc, "EligibleLevels");
            auto it = levels.Begin();
            ASSERT_TRUE(it != levels.End()) << "it != levels.End()";

            ProgressiveEligibleLevelInfo level(123, *it);

            ASSERT_EQ((int) 123                 , (int) level.GetLevelId()) << "GetLevelId()"        ;
            ASSERT_EQ(      true                ,       level.ShowInfo()) << "ShowInfo()"          ;
            ASSERT_EQ(      true                ,       level.IsEligible()) << "IsEligible()"        ;
            ASSERT_EQ(      true                ,       level.ShowOdometer()) << "ShowOdometer()"      ;
            ASSERT_EQ(      true                ,       level.IsPte()) << "IsPte()"             ;
            ASSERT_STREQ(      "45"               ,       level.GetName()) << "GetName()"           ;
            ASSERT_STREQ(      "info"             ,       level.GetInfo()) << "GetInfo()"           ;
            ASSERT_STREQ(      "CoinInTriggerOnly",       level.GetTriggerType()) << "GetTriggerType()"    ;
            ASSERT_STREQ(      "hello"            ,       level.GetLeaderboardUrl()) << "GetLeaderboardUrl()" ;
            ASSERT_STREQ(      "world"            ,       level.GetImageUrl()) << "GetImageUrl()"       ;
            ASSERT_EQ((int) 314                 , (int) level.GetValue()) << "GetValue()"          ;
            ASSERT_EQ((int) 598                 , (int) level.GetStartTimeUtc()) << "GetStartTimeUtc()"   ;
            ASSERT_EQ((int) 192837              , (int) level.GetEndTimeUtc()) << "GetEndTimeUtc()"     ;
            ASSERT_EQ(      0.17                ,	   level.GetPercentEligible()) << "GetPercentEligible()";
            ASSERT_EQ((int) 2                   , (int) level.GetNumMinors()) << "GetNumMinors()"      ;
            ASSERT_EQ((int) PteStatus::Active   , (int) level.GetPteStatus()) << "GetPteStatus()"      ;

            const ProgressiveEligibleMinorInfo* pMinor;

            pMinor = level[0];
            ASSERT_TRUE(pMinor != NULL) << "level[0]";
            CheckMinor(*pMinor);

            pMinor = level[1];
            ASSERT_TRUE(pMinor != NULL) << "level[1]";
            CheckMinor(*pMinor);

            pMinor = level[2];
            ASSERT_TRUE(pMinor == NULL) << "level[2]";

            int count = 0;
            level.ForEachMinor([&](const ProgressiveEligibleMinorInfo& minor)->bool
            {
                CheckMinor(minor);
                count++;
                return true;
            });

            ASSERT_EQ(2, count) << "count";
        }
        catch (JsonException& e)
        {
            FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%S: %S: %S: %S", e.code(), e.codestring(), e.what(), e.filename());
        }
        catch (SentinelException& e)
        {
            FAIL() << AssertMsg::Format(0, L"Unexpected exception", L"%S: %S", e.code(), e.what());
        }
    }

    TEST(ProgressiveEligibleLevelInfoTests, ProgressiveEligibleLevelInfo_ctor_prepte)
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
                        "\"Name\":\"second\","
                        "\"TriggerType\":\"PayLineTrigger\","
                        "\"Eligible\":false,"
                        "\"ShowOdometer\":false,"
                        "\"PercentEligible\":7.65,"
                        "\"LinkName\":\"link\","
                        "\"StartTimeUtc\":598,"
                        "\"EndTimeUtc\":7654321,"
                        "\"TotalAwardsToPlayer\":98.01,"
                        "\"Minors\":["
                            "{"
                                "\"Name\":\"second\","
                                "\"PercentEligible\":7.65,"
                                "\"Eligible\":true,"
                                "\"Min\":5.6,"
                                "\"Max\":7.8,"
                                "\"Sequence\":2"
                            "},"
                            "{"
                                "\"Name\":\"first\","
                                "\"PercentEligible\":7.65,"
                                "\"Eligible\":true,"
                                "\"Min\":5.6,"
                                "\"Max\":7.8,"
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

        try
        {
            rapidjson::Document jsonDoc;
            if (jsonDoc.Parse(pJson).HasParseError())
                throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

            const rapidjson::Value& levels = JsonGetArrayEx(jsonDoc, "EligibleLevels");
            auto it = levels.Begin();
            ASSERT_TRUE(it != levels.End()) << "it != levels.End()";

            ProgressiveEligibleLevelInfo level(123, *it);

            ASSERT_EQ((int) 123                 , (int) level.GetLevelId()) << "GetLevelId()"        ;
            ASSERT_FALSE(level.ShowInfo()) << "ShowInfo()"          ;
            ASSERT_FALSE(level.IsEligible()) << "IsEligible()"        ;
            ASSERT_FALSE(level.ShowOdometer()) << "ShowOdometer()"      ;
            ASSERT_FALSE( level.IsPte()) << "IsPte()"             ;
            ASSERT_STREQ(      "second"           ,       level.GetName()) << "GetName()"           ;
            ASSERT_STREQ(      ""                 ,       level.GetInfo()) << "GetInfo()"           ;
            ASSERT_STREQ(      "PayLineTrigger"   ,       level.GetTriggerType()) << "GetTriggerType()"    ;
            ASSERT_STREQ(      ""                 ,       level.GetLeaderboardUrl()) << "GetLeaderboardUrl()" ;
            ASSERT_STREQ(      ""                 ,       level.GetImageUrl()) << "GetImageUrl()"       ;
            ASSERT_EQ((int) 0                   , (int) level.GetValue()) << "GetValue()"          ;
            ASSERT_EQ((int) 598                 , (int) level.GetStartTimeUtc()) << "GetStartTimeUtc()"   ;
            ASSERT_EQ((int) 7654321             , (int) level.GetEndTimeUtc()) << "GetEndTimeUtc()"     ;
            ASSERT_EQ(      7.65                ,	   level.GetPercentEligible()) << "GetPercentEligible()";
            ASSERT_EQ((int) 0                   , (int) level.GetNumMinors()) << "GetNumMinors()"      ;
            ASSERT_EQ((int) PteStatus::Invalid  , (int) level.GetPteStatus()) << "GetPteStatus()"      ;

            const ProgressiveEligibleMinorInfo* pMinor;

            pMinor = level[0];
            ASSERT_TRUE(pMinor == NULL) << "level[0]";

            int count = 0;
            level.ForEachMinor([&](const ProgressiveEligibleMinorInfo& minor)->bool
            {
                count++;
                return true;
            });

            ASSERT_EQ(0, count) << "count";
        }
        catch (JsonException& e)
        {
            FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %S: %S: %S: %S", e.code(), e.codestring(), e.what(), e.filename());
        }
        catch (SentinelException& e)
        {
            FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %S: %S", e.code(), e.what());
        }
    }

    TEST(ProgressiveEligibleLevelInfoTests, ProgressiveEligibleLevelInfo_GetLogString)
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
            "\"Name\":\"45\","
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
        string expectedString("EligibleLevelInfo: levelId=123 pteStatus=2 showInfo=1 isEligible=1 isPlayerQualified=0"
            " showOdometer=1 pte=1 name=\"45\" info=\"info\" triggerType=\"CoinInTriggerOnly\" value=314"
            " startTime=02/11/1970 00:00:00 endTime=02/11/1970 00:03:12 countdownStartTime=-7766279631452241719 countdownEndTime=-7766279631452241719 percentEligible=0.17"
			" prizeDescription= awardType=");

        try
        {
            rapidjson::Document jsonDoc;
            if (jsonDoc.Parse(pJson).HasParseError())
                throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

            const rapidjson::Value& levels = JsonGetArrayEx(jsonDoc, "EligibleLevels");
            auto it = levels.Begin();
            ASSERT_TRUE(it != levels.End()) << "it != levels.End()";

            ProgressiveEligibleLevelInfo level(123, *it);
            CUtilities::SetTzOffset(tzOffsetMinutes);

            // Perform operation(s) and test(s).
            string resultString(level.GetLogString());
            ASSERT_EQ(expectedString, resultString) << "GetLogString()";
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

    TEST(ProgressiveEligibleLevelInfoTests, ProgressiveEligibleLevelInfo_GetSubscriptionOdometerData)
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
            "\"Name\":\"45\","
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
        string expectedString("{\"BonusLevelType\":\"CoinInTriggerOnly\",\"BonusLevelData\":{\"LevelName\":\"45\",\"Value\":\"3.14\",\"PrizeDescription\":\"\"}}");

        try
        {
            rapidjson::Document jsonDoc;
            if (jsonDoc.Parse(pJson).HasParseError())
                throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

            const rapidjson::Value& levels = JsonGetArrayEx(jsonDoc, "EligibleLevels");
            auto it = levels.Begin();
            ASSERT_TRUE(it != levels.End()) << "it != levels.End()";

            ProgressiveEligibleLevelInfo level(123, *it);
            CUtilities::SetTzOffset(tzOffsetMinutes);

            // Perform operation(s) and test(s).
            string resultString(level.GetSubscriptionOdometerData(""));
            ASSERT_EQ(expectedString, resultString) << "GetSubscriptionOdometerData()";
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

    TEST(ProgressiveEligibleLevelInfoTests, ProgressiveEligibleLevelInfo_GetLogString_prepte)
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
            "\"Name\":\"second\","
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
            "\"Name\":\"second\","
            "\"Eligible\":true,"
            "\"PercentEligible\":7.65,"
            "\"Min\":5.6,"
            "\"Max\":7.8,"
            "\"Sequence\":2"
            "},"
            "{"
            "\"Name\":\"first\","
            "\"Eligible\":true,"
            "\"PercentEligible\":7.65,"
            "\"Min\":5.6,"
            "\"Max\":7.8,"
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
		string expectedString("EligibleLevelInfo: levelId=123 pteStatus=-1 showInfo=0 isEligible=0 isPlayerQualified=0"
			" showOdometer=0 pte=0 name=\"second\" info=\"\" triggerType=\"PayLineTrigger\" value=0"
			" startTime=02/21/1971 00:00:00 endTime=02/21/1971 02:07:34 countdownStartTime=-7766279631452241719 countdownEndTime=-7766279631452241719 percentEligible=7.65"
			" prizeDescription= awardType=");

        try
        {
            rapidjson::Document jsonDoc;
            if (jsonDoc.Parse(pJson).HasParseError())
                throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

            const rapidjson::Value& levels = JsonGetArrayEx(jsonDoc, "EligibleLevels");
            auto it = levels.Begin();
            ASSERT_TRUE(it != levels.End()) << "it != levels.End()";

            ProgressiveEligibleLevelInfo level(123, *it);
            CUtilities::SetTzOffset(tzOffsetMinutes);

            // Perform operation(s) and test(s).
            string resultString(level.GetLogString());
            ASSERT_STREQ(expectedString.c_str(), resultString.c_str()) << "GetLogString()";
        }
        catch (JsonException& e)
        {
            FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %S: %S: %S: %S", e.code(), e.codestring(), e.what(), e.filename());
        }
        catch (SentinelException& e)
        {
            FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %S: %S", e.code(), e.what());
        }
    }

    TEST(ProgressiveEligibleLevelInfoTests, GetSubscriptionOdometerData_prepte)
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
            "\"Name\":\"second\","
            "\"TriggerType\":\"PayLineTrigger\","
            "\"Eligible\":false,"
            "\"Value\":43.100000000,"
            "\"ShowOdometer\":false,"
            "\"PercentEligible\":7.65,"
            "\"LinkName\":\"link\","
            "\"StartTimeUtc\":36000000598,"
            "\"EndTimeUtc\":36007654321,"
            "\"TotalAwardsToPlayer\":98.01,"
            "\"Minors\":["
            "{"
            "\"Name\":\"second\","
            "\"Eligible\":true,"
            "\"PercentEligible\":7.65,"
            "\"Min\":5.6,"
            "\"Max\":7.8,"
            "\"Sequence\":2"
            "},"
            "{"
            "\"Name\":\"first\","
            "\"Eligible\":true,"
            "\"PercentEligible\":7.65,"
            "\"Min\":5.6,"
            "\"Max\":7.8,"
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
        string expectedString("{\"BonusLevelType\":\"\"}");

        try
        {
            rapidjson::Document jsonDoc;
            if (jsonDoc.Parse(pJson).HasParseError())
                throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

            const rapidjson::Value& levels = JsonGetArrayEx(jsonDoc, "EligibleLevels");
            auto it = levels.Begin();
            ASSERT_TRUE(it != levels.End()) << "it != levels.End()";

            ProgressiveEligibleLevelInfo level(123, *it);
            CUtilities::SetTzOffset(tzOffsetMinutes);

            // Perform operation(s) and test(s).
            string resultString(level.GetSubscriptionOdometerData(""));
            ASSERT_STREQ(expectedString.c_str(), resultString.c_str()) << "GetSubscriptionOdometerData()";
        }
        catch (JsonException& e)
        {
            FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %S: %S: %S: %S", e.code(), e.codestring(), e.what(), e.filename());
        }
        catch (SentinelException& e)
        {
            FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %S: %S", e.code(), e.what());
        }
    }
    
    TEST(ProgressiveEligibleLevelInfoTests, GetSubscriptionOdometerData_scatter)
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
            "\"Name\":\"second\","
            "\"TriggerType\":\"Scatter\","
            "\"Pte\":true,"
            "\"Info\":\"info\","
            "\"Value\":43.100000000,"
            "\"LeaderboardUrl\":\"hello\","
            "\"Eligible\":false,"
            "\"ShowOdometer\":false,"
            "\"Status\":\"Active\","
            "\"PercentEligible\":7.65,"
            "\"LinkName\":\"link\","
            "\"StartTimeUtc\":36000000598,"
            "\"EndTimeUtc\":36007654321,"
            "\"TotalAwardsToPlayer\":98.01,"
            "\"Minors\":["
            "{"
            "\"Name\":\"second\","
            "\"Eligible\":true,"
            "\"PercentEligible\":7.65,"
            "\"Min\":5.6,"
            "\"Max\":7.8,"
            "\"Sequence\":2"
            "},"
            "{"
            "\"Name\":\"first\","
            "\"Eligible\":true,"
            "\"PercentEligible\":7.65,"
            "\"Min\":5.6,"
            "\"Max\":7.8,"
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
        string expectedString("{\"BonusLevelType\":\"Scatter\",\"BonusLevelData\":{\"LevelName\":\"second\",\"Value\":");

        try
        {
            rapidjson::Document jsonDoc;
            if (jsonDoc.Parse(pJson).HasParseError())
                throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

            const rapidjson::Value& levels = JsonGetArrayEx(jsonDoc, "EligibleLevels");
            auto it = levels.Begin();
            ASSERT_TRUE(it != levels.End()) << "it != levels.End()";
            
            ProgressiveEligibleLevelInfo level(123, *it);

            CUtilities::SetTzOffset(tzOffsetMinutes);

            // Perform operation(s) and test(s).
            string resultString(level.GetSubscriptionOdometerData(""));
            
            ASSERT_EQ(resultString.find(expectedString), 0);
        }
        catch (JsonException& e)
        {
            FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %S: %S: %S: %S", e.code(), e.codestring(), e.what(), e.filename());
        }
        catch (SentinelException& e)
        {
            FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %S: %S", e.code(), e.what());
        }
    }

    TEST(ProgressiveEligibleLevelInfoTests, GetSubscriptionOdometerData_PlayXGetY)
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
            "\"Name\":\"second\","
            "\"TriggerType\":\"PlayXGetY\","
            "\"Pte\":true,"
            "\"Info\":\"info\","
            "\"Value\":43.100000000,"
            "\"Eligible\":false,"
            "\"LeaderboardUrl\":\"hello\","
            "\"ShowOdometer\":false,"
            "\"Status\":\"Active\","
            "\"PercentEligible\":7.65,"
            "\"LinkName\":\"link\","
            "\"StartTimeUtc\":36000000598,"
            "\"EndTimeUtc\":36007654321,"
            "\"TotalAwardsToPlayer\":98.01,"
            "\"Minors\":["
            "{"
            "\"Name\":\"second\","
            "\"PercentEligible\":7.65,"
            "\"Eligible\":true,"
            "\"Min\":5.6,"
            "\"Max\":7.8,"
            "\"Sequence\":2"
            "},"
            "{"
            "\"Name\":\"first\","
            "\"PercentEligible\":7.65,"
            "\"Eligible\":true,"
            "\"Min\":5.6,"
            "\"Max\":7.8,"
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
        string expectedString("{\"BonusLevelType\":\"PlayXGetY\",\"BonusLevelData\":{\"LevelName\":\"second\",\"Value\":{\"Value\":\"43.10\",\"Minors\":[]}}}");

        try
        {
            rapidjson::Document jsonDoc;
            if (jsonDoc.Parse(pJson).HasParseError())
                throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

            const rapidjson::Value& levels = JsonGetArrayEx(jsonDoc, "EligibleLevels");
            auto it = levels.Begin();
            ASSERT_TRUE(it != levels.End()) << "it != levels.End()";

            ProgressiveEligibleLevelInfo level(123, *it);
            CUtilities::SetTzOffset(tzOffsetMinutes);

            // Perform operation(s) and test(s).
            string resultString(level.GetSubscriptionOdometerData(""));
            ASSERT_STREQ(expectedString.c_str(), resultString.c_str()) << "GetSubscriptionOdometerData()";
        }
        catch (JsonException& e)
        {
            FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %S: %S: %S: %S", e.code(), e.codestring(), e.what(), e.filename());
        }
        catch (SentinelException& e)
        {
            FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %S: %S", e.code(), e.what());
        }
    }

    TEST(ProgressiveEligibleLevelInfoTests, ProgressiveEligibleLevelInfo_PlayXGetY)
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
			"\"Name\":\"45\","
			"\"TriggerType\":\"PlayXGetY\","
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
			"\"PercentEligible\":0.23,"
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
		string expectedString("EligibleLevelInfo: levelId=123 pteStatus=2 showInfo=1 isEligible=1 isPlayerQualified=0"
			" showOdometer=1 pte=1 name=\"45\" info=\"info\" triggerType=\"PlayXGetY\" value=314"
			" startTime=02/11/1970 00:00:00 endTime=02/11/1970 00:03:12 countdownStartTime=-7766279631452241719 countdownEndTime=-7766279631452241719 percentEligible=0.17"
			" prizeDescription= awardType=");		
		try
		{
			rapidjson::Document jsonDoc;
			if (jsonDoc.Parse(pJson).HasParseError())
				throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

			const rapidjson::Value& levels = JsonGetArrayEx(jsonDoc, "EligibleLevels");
			auto it = levels.Begin();
			ASSERT_TRUE(it != levels.End()) << "it != levels.End()";

			ProgressiveEligibleLevelInfo level(123, *it);
			CUtilities::SetTzOffset(tzOffsetMinutes);

			// Perform operation(s) and test(s).
			string resultString(level.GetLogString());
			ASSERT_EQ(expectedString, resultString) << "GetLogString()";
			ASSERT_EQ(1, level.GetNumMinors()) << "GetNumMinors()";
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
}
