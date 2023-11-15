#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveLevelInfo.h"
#include "ProgressiveLevelTestHelpers.h"
#include <Util/Convert.h>
#include "AssertMsg.h"

using namespace std;

#include "gtest/gtest.h"

using namespace SentinelNativeUnitTests::ProgressiveLevelTestHelpers;

namespace SentinelNativeUnitTests
{
    using namespace ProgressiveLevelTestHelpers;
    static const char* pJsonLevels = R"({
    "Levels":
    [
      {
        "Id": 627,
        "Version": 250,
        "Type": "Standard",
        "Channel":
        {
          "Destination": "@239.1.1.1:61613",
    	  "MulticastDestination": "stomp://239.1.1.1:61613"
        },
        "Info": "",
        "Name": "Jacks 1",
        "MediaMinSeconds": 0,
        "MediaMaxSeconds": 0
      },
      {
        "Id": 628,
        "Version": 251,
        "Type": "Bonus",
        "Channel":
        {
          "Destination": "@239.1.1.1:61613",
    	  "MulticastDestination": "stomp://239.1.1.1:61614"
        },
        "Info": "",
        "Name": "Jacks 2",
        "MediaMinSeconds": 0,
        "MediaMaxSeconds": 0
      },
      {
        "Id": 0,
        "Version": 251,
        "Type": "Standard",
        "Channel":
        {
          "Destination": "@239.1.1.1:61614",
          "MulticastDestination": "stomp://239.1.1.1:61615"
        },
        "Info": "",
        "Name": "Jacks 3",
        "MediaMinSeconds": 0,
        "MediaMaxSeconds": 0
      },
      {
        "Id": 23,
        "Version": 251,
        "Type": "PalcomX",
        "Channel":
        {
          "Destination": "@239.1.1.1:61614",
          "MulticastDestination": "stomp://239.1.1.1:61613"
        },
        "Info": "",
        "Name": "Jacks 4",
        "MediaMinSeconds": 0,
        "MediaMaxSeconds": 0
      }
    ]})";

    static const char* pJsonGames = R"({
    "Games":
    [
      {
        "GameNumber": 9,
        "Denomination": 0.05,
        "MaxBetCredits": 5,
        "TheoPercentage": 99.17,
        "GameName": "GK",
        "Paytable": "P1003A",
        "QueryCards": false
      },
      {
        "GameNumber": 2,
        "Denomination": 0.25,
        "MaxBetCredits": 5,
        "TheoPercentage": 99.54,
        "GameName": "Jacks Or Better",
        "Paytable": "P1041A",
        "QueryCards": true,
        "Levels":
        [
          {
            "LevelId": 627,
            "LevelSequence": 5
          },
          {
            "LevelId": 627,
            "LevelSequence": 3
          },
          {
            "LevelId": 628,
            "LevelSequence": 1
          }
        ]
      }
    ]})";

    static const char* pMultiLevelMultiGame = R"({"Games":[{"GameNumber":2,"Denomination":0.02,"MaxBetCredits":250,"TheoPercentage":87.82,"GameName":"Aristocrat - N/A","Paytable":"906   ","QueryCards":false,"Levels":[{"LevelId":24071,"LevelSequence":0}]},{"GameNumber":3,"Denomination":0.05,"MaxBetCredits":250,"TheoPercentage":87.82,"GameName":"Aristocrat - N/A","Paytable":"906   ","QueryCards":false,"Levels":[{"LevelId":24243,"LevelSequence":0},{"LevelId":24071,"LevelSequence":0}]},{"GameNumber":1,"Denomination":0.01,"MaxBetCredits":250,"TheoPercentage":87.82,"GameName":"Aristocrat - N/A","Paytable":"906   ","QueryCards":false}],"Levels":[{"Id":24243,"Version":352,"Type":"Bonus","Channel":{"Destination":"@239.1.1.1:61613","MulticastDestination":"stomp://239.1.1.1:61613"},"Info":"","Name":"Joe Mystery","MediaMinSeconds":0,"MediaMaxSeconds":0},{"Id":24071,"Version":360,"Type":"Bonus","Channel":{"Destination":"@239.1.1.1:61613","MulticastDestination":"stomp://239.1.1.1:61613"},"Info":"","Name":"Joe Standard","MediaMinSeconds":1,"MediaMaxSeconds":3}],"ProtocolType":"sas","ProgressiveGroupId":0,"PaymentMethod":"SasAFTBonus","UseAFTLockOnTransfers":false,"MeterBankLevel":false,"MeterMachineOnly":false,"ContentHttpUrl":"http://10.138.98.10:8080/onelink/content/original/","DeviceId":24157,"DeviceConfigVersion":64404,"DeviceAddress":"@device.ICARD.206FEC00414F","EventChannel":{"Destination":"@239.3.0.1:61613","MulticastDestination":"stomp://239.3.0.1:61613"},"AssetNumber":"55553","BankName":"Default Bank","TimeZone":"America/Los_Angeles","SourceDeviceId":1,"SourceDeviceConfigVersion":0,"SourceAsset":"","Timestamp":1448506563025,"MessageId":"1372","OneLinkSystemId":"ceae0730-bbef-46ec-9cf1-292cab5fd4ca"})";
    class ProgressiveLevelsInfoTests : public ::testing::Test
    {

        public:
            void f_ProgressiveLevelsInfo_Ctor_AddLevels(void)
            {
                try
                {
                    rapidjson::Document jsonDoc;
                    if (jsonDoc.Parse(pJsonLevels).HasParseError())
                        throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

                    ProgressiveLevelsInfo levels;
                    levels.Ctor_AddLevels(jsonDoc);

                    ASSERT_EQ((size_t) 2, levels.m_levels.size()) << "m_levels.size()";

                    ProgressiveLevelInfo* pLevel = levels.m_levels[627].get();

                    ASSERT_TRUE(pLevel != NULL) << "m_levels[627]";
                    ASSERT_EQ((size_t) 627, (size_t) pLevel->m_id) << "m_levels[627]->m_id";
                    ASSERT_STREQ("Jacks 1", pLevel->m_name.c_str()) << "m_levels[627]->m_name";
                    ASSERT_STREQ("stomp://239.1.1.1:61613", pLevel->m_url.c_str()) << "m_levels[627]->m_url";
                    ASSERT_EQ((int) ProgressiveLevelType::Standard, (int) pLevel->m_type) << "m_levels[627]->m_type";

                    pLevel = levels.m_levels[628].get();

                    ASSERT_TRUE(pLevel != NULL) << "m_levels[628]";
                    ASSERT_EQ((size_t) 628, (size_t) pLevel->m_id) << "m_levels[628]->m_id";
                    ASSERT_STREQ("Jacks 2", pLevel->m_name.c_str()) << "m_levels[628]->m_name";
                    ASSERT_STREQ("stomp://239.1.1.1:61614", pLevel->m_url.c_str()) << "m_levels[628]->m_url";
                    ASSERT_EQ((int) ProgressiveLevelType::Bonus, (int) pLevel->m_type) << "m_levels[628]->m_type";
                }
                catch (JsonException& e)
                {
                    FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %S: %S: %S: %S", e.code(), e.codestring(), e.what(), e.filename());
                }
            }

            void f_ProgressiveLevelsInfo_Ctor_AssignSasLevels(void)
            {
                try
                {
#if 000 //TODO-LV
                    rapidjson::Document jsonDoc;
                    if (jsonDoc.Parse(pJsonGames).HasParseError())
                        throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

                    ProgressiveLevelsInfo levels;
                    levels.m_levels[627].reset(new ProgressiveLevelInfo(627, ProgressiveLevelType::Standard, "", ""));
                    levels.m_levels[628].reset(new ProgressiveLevelInfo(628, ProgressiveLevelType::Bonus, "", ""));

                    levels.Ctor_AssignSasLevels(jsonDoc);

                    ASSERT_EQ((size_t) 1, levels.m_levels.size()) << "m_levels.size()";

                    ProgressiveLevelInfo* pLevel = levels.m_levels[627].get();

                    ASSERT_EQ((size_t) 2, pLevel->m_sasLevels.size()) << "m_levels[627].m_sasLevels.size()";
                    ASSERT_EQ((uint8_t) 5, pLevel->m_sasLevels[0]) << "m_levels[627].m_sasLevels[0]";
                    ASSERT_EQ((uint8_t) 3, pLevel->m_sasLevels[1]) << "m_levels[627].m_sasLevels[1]";
#endif
                }
                catch (JsonException& e)
                {
                    FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %S: %S: %S: %S", e.code(), e.codestring(), e.what(), e.filename());
                }
            }

            void f_ProgressiveLevelsInfo_Ctor_Cleanup(void)
            {
#if 000 //TODO-LV
                ProgressiveLevelsInfo levels;

                ProgressiveLevelInfo* pLevel = new ProgressiveLevelInfo(1, ProgressiveLevelType::Standard, "", "");
                pLevel->m_sasLevels.push_back(5);
                pLevel->m_sasLevels.push_back(3);
                levels.m_levels[1].reset(pLevel);

                pLevel = new ProgressiveLevelInfo(2, ProgressiveLevelType::Standard, "", "");
                levels.m_levels[2].reset(pLevel);

                pLevel = new ProgressiveLevelInfo(3, ProgressiveLevelType::Bonus, "", "");
                pLevel->m_sasLevels.push_back(0);
                levels.m_levels[3].reset(pLevel);

                levels.Ctor_Cleanup();

                ASSERT_EQ((size_t) 2, levels.m_levels.size()) << "m_levels.size()";

                pLevel = levels.m_levels[1].get();
                ASSERT_EQ((size_t) 2, pLevel->m_sasLevels.size()) << "m_levels[1].m_sasLevels.size()";
                ASSERT_EQ((uint8_t) 3, pLevel->m_sasLevels[0]) << "m_levels[1].m_sasLevels[0]";
                ASSERT_EQ((uint8_t) 5, pLevel->m_sasLevels[1]) << "m_levels[1].m_sasLevels[1]";

                pLevel = levels.m_levels[3].get();
                ASSERT_EQ((size_t) 1, pLevel->m_sasLevels.size()) << "m_levels[3].m_sasLevels.size()";
                ASSERT_EQ((uint8_t) 0, pLevel->m_sasLevels[0]) << "m_levels[3].m_sasLevels[0]";

                pLevel = levels.m_levels[2].get();
                ASSERT_TRUE(pLevel == NULL) << "m_levels[2].get()";
#endif
            }

            void f_ProgressiveLevelsInfo_ctor_multiLevelmultigame(void)
            {
                try
                {
                    rapidjson::Document jsonDoc;
                    if (jsonDoc.Parse(pMultiLevelMultiGame).HasParseError())
                        throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

                    ProgressiveLevelsInfo levels(jsonDoc);

                    ASSERT_FALSE(levels.QueryPokerCards()) << "levels.QueryPokerCards()";

                    vector<string> urls = levels.GetUrls();
                    ASSERT_EQ((size_t) 1, urls.size()) << "levels.GetUrls()";
                    ASSERT_STREQ("stomp://239.1.1.1:61613", urls[0].c_str()) << "levels.GetUrls()[0]";
                }
                catch (JsonException& e)
                {
                    FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %S: %S: %S: %S", e.code(), e.codestring(), e.what(), e.filename());
                }
            }

             void f_ProgressiveLevelsInfo_ctor(void)
             {
                try
                {
                    _ICardConfig5 cardConfig;
                    rapidjson::Document jsonDoc;
                    cardConfig.CreateJsonDoc(jsonDoc);

                    ProgressiveLevelsInfo levels(jsonDoc);
                    cardConfig.Verify(levels);
                }
                catch (JsonException& e)
                {
                    FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %S: %S: %S: %S", e.code(), e.codestring(), e.what(), e.filename());
                }
            }

            void f_ProgressiveLevelsInfo_duplicatesaslevels(void)
            {
                try
                {
                    _ICardConfig5 cardConfig;
                    cardConfig.Games[1]->Levels[0]->LevelSequence = 1;
                    rapidjson::Document jsonDoc;
                    cardConfig.CreateJsonDoc(jsonDoc);

                    ProgressiveLevelsInfo levelsInfo(jsonDoc);
                    FAIL() << "Should have thrown an exception.";
                }
                catch (SentinelException& msg)
                {
                    ASSERT_STREQ("ERROR: Level ID 61234: SAS level 1 is already being used by level ID 5009.", msg.what());
                }
            }

            void f_ProgressiveLevelsInfo_ctor2(void)
            {
                try
                {
                   _ICardConfig5 cardConfig(2);
                    rapidjson::Document jsonDoc;
                    cardConfig.CreateJsonDoc(jsonDoc);

                    ProgressiveLevelsInfo levels(jsonDoc);
                    cardConfig.Verify(levels);
                }
                catch (JsonException& e)
                {
                    FAIL() << AssertMsg::FormatOnly(L"Unexpected exception: %S: %S: %S: %S", e.code(), e.codestring(), e.what(), e.filename());
                }
            }

            void f_ProgressiveLevelsInfo_duplicatelevelids(void)
            {
                try
                {
                    _ICardConfig5 cardConfig;
                    cardConfig.CardLevels[3]->Id = 5009;
                    rapidjson::Document jsonDoc;
                    cardConfig.CreateJsonDoc(jsonDoc);

                    ProgressiveLevelsInfo levelsInfo(jsonDoc);
                    FAIL() << "Should have thrown an exception.";
                }
                catch (SentinelException& msg)
                {
                    ASSERT_STREQ("Level Id 5009 is a duplicate.", msg.what());
                }
            }

            void f_ProgressiveLevelsInfo_sasleveloutofbounds(void)
            {
                try
                {
                    _ICardConfig5 cardConfig;
                    cardConfig.Games[1]->Levels[1]->LevelSequence = 33;
                    rapidjson::Document jsonDoc;
                    cardConfig.CreateJsonDoc(jsonDoc);

                    ProgressiveLevelsInfo levelsInfo(jsonDoc);
                    FAIL()<<"Should have thrown an exception.";
                }
                catch (SentinelException& msg)
                {
                    ASSERT_STREQ("Level Id 5010: SAS level 33 is out-of-bounds.", msg.what());
                }
            }
    };

    TEST_F(ProgressiveLevelsInfoTests, ProgressiveLevelsInfo_Ctor_AddLevels)
    {
        f_ProgressiveLevelsInfo_Ctor_AddLevels();
    }

    TEST_F(ProgressiveLevelsInfoTests, ProgressiveLevelsInfo_Ctor_AssignSasLevels)
    {
        f_ProgressiveLevelsInfo_Ctor_AssignSasLevels();
    }

    TEST_F(ProgressiveLevelsInfoTests, ProgressiveLevelsInfo_Ctor_Cleanup)
    {
        f_ProgressiveLevelsInfo_Ctor_Cleanup();
    }

    TEST_F(ProgressiveLevelsInfoTests, ProgressiveLevelsInfo_ctor_multiLevelmultigame)
    {
        f_ProgressiveLevelsInfo_ctor_multiLevelmultigame();
        
    }

    TEST_F(ProgressiveLevelsInfoTests, ProgressiveLevelsInfo_ctor)
    {
        f_ProgressiveLevelsInfo_ctor();
    }


    TEST_F(ProgressiveLevelsInfoTests, ProgressiveLevelsInfo_ctor2)
    {
        f_ProgressiveLevelsInfo_ctor2();

    }

    TEST_F(ProgressiveLevelsInfoTests, ProgressiveLevelsInfo_duplicatelevelids)
    {
        f_ProgressiveLevelsInfo_duplicatelevelids();

    }

    TEST_F(ProgressiveLevelsInfoTests, ProgressiveLevelsInfo_sasleveloutofbounds)
    {
        f_ProgressiveLevelsInfo_sasleveloutofbounds();

    }

    TEST_F(ProgressiveLevelsInfoTests, ProgressiveLevelsInfo_duplicatesaslevels)
    {
        f_ProgressiveLevelsInfo_duplicatesaslevels();
       
    }
}
