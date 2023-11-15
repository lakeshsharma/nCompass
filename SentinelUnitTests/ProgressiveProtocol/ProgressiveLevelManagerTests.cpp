
#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveLevelManager.h"

// Unit testing includes.
#include "AssertMsg.h"
#include "ProgressiveLevelTestHelpers.h"
#include <deque>

#include "gtest/gtest.h"

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
					"\"LevelId\":6,"
					"\"Name\":\"La Montaña\","
					"\"TriggerType\":\"CoinInTriggerOnly\","
					"\"Eligible\":true,"
					"\"ShowOdometer\":false,"
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
					"\"LevelId\":5,"
					"\"Name\":\"Buffalo\","
					"\"TriggerType\":\"PayLineTrigger\","
					"\"Eligible\":true,"
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
				"},"
				"{"
					"\"LevelId\":5009,"
					"\"Name\":\"Buffalo\","
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
					"\"LevelId\":61234,"
					"\"Name\":\"Joker Poker\","
					"\"TriggerType\":\"PayLineTrigger\","
					"\"Eligible\":true,"
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

	class ProgressiveLevelManagerTests : public ::testing::Test
	{
        public:
            void f_ProgressiveLevelManager_ctor( void )
            {
                ProgressiveLevelManager levelManager;

                ASSERT_TRUE(levelManager.m_levels.empty() ) << "m_levelManager->empty()";
                ASSERT_TRUE(levelManager.m_levelsBySAS.empty() ) << "m_levelsBySAS.empty()";

                ASSERT_EQ((DWORD) 0, levelManager.m_updatedTickCount) << "m_updatedTickCount";
                ASSERT_EQ((DWORD) 0, levelManager.m_forwardedTickCount)	  << "m_forwardedTickCount";
                ASSERT_EQ(        0, levelManager.m_progressiveGroupId)	 << "m_progressiveGroupId" ;
                ASSERT_FALSE(			levelManager.m_useAftLockOnTransfers)  << "m_useAftLockOnTransfers";
                ASSERT_FALSE(			levelManager.m_queryPokerCards)		  << "m_queryPokerCards";
                ASSERT_FALSE(			levelManager.m_isUpdated)			  << "m_isUpdated";
                ASSERT_FALSE(			levelManager.m_needPriorityForward) <<   "m_needPriorityForward";
                ASSERT_EQ(        0, levelManager.m_eligibilitySessionId) << "m_eligibilitySessionId";
                ASSERT_FALSE(			levelManager.m_eligibilityDetermined ) << "m_eligibilityDetermined";
                ASSERT_FALSE(			levelManager.m_sessionActive) << "m_sessionActive";
                
                ASSERT_EQ(0, (int) levelManager.m_sessionId) << "m_sessionId";
                ASSERT_EQ(DefaultMulticastActive, (bool) levelManager.m_multicastActive ) << "m_eligibilityDetermined";

                ASSERT_EQ((size_t) ProgressiveLevelManager::PriorityForwardWindowMaxUpdates, 
                    levelManager.m_lastForwardedTickCounts.size()) << "m_lastForwardedTickCounts.size()";

                for (size_t i = 0; i < ProgressiveLevelManager::PriorityForwardWindowMaxUpdates; ++i)
                {
                    ASSERT_EQ((DWORD) 0, levelManager.m_lastForwardedTickCounts[i] ) <<  AssertMsg::FormatOnly("m_lastForwardedTickCounts[%u]", i);
                }
            }

            void f_ProgressiveLevelManager_cctor_and_assignment()
            {
                try
                {
                    const SYSTEMTIME receivedTime = { 2001, 2, 0, 3, 12, 34, 56, 789 };
                    const DWORD receivedTickCount = 98765;
                    const bool useAftLockOnTransfers = true;

                    _ICardConfig5 cardConfig;
                    shared_ptr<ProgressiveLevelsInfo> levelsInfo;
                    cardConfig.CreateLevelsInfo(levelsInfo);

                    ProgressiveLevelManager levelManagerOrg;
                    levelManagerOrg.UpdateLevels(levelsInfo.get());

                    ProgressiveLevelManager levelManager(levelManagerOrg);
                    cardConfig.Verify(levelManager);
                }
                catch (SentinelException& e)
                {
                    FAIL() << AssertMsg::FormatOnly("ProcessICardConfiguration failed with exception: %S", e.what());
                }
            }

            void f_ProgressiveLevelManager_GetLevelIdFromSASLevel()
            {
                ProgressiveLevelManager levelManager;
                levelManager.m_updatedTickCount = 100;
                levelManager.m_forwardedTickCount = 100;
                levelManager.m_progressiveGroupId = 11;
                levelManager.m_useAftLockOnTransfers = true;

                LevelData data[] =
                {
                    //TickCount, id, type, pName, pUrl, numSasLevels, sasLevels[3], expectedNumSasLevels;

                    { __LINE__, 123, 1000, ProgressiveLevelType::Standard		, "name0", "url0", 2, { 2, 5 }, 2 },
                    { __LINE__, 456, 1001, ProgressiveLevelType::Bonus			, ""	 , "url1", 0, {      }, 0 },
                    { __LINE__, 789, 1002, ProgressiveLevelType::Standard		, nullptr, "url2", 1, { 31   }, 1 },
                    { __LINE__, 951, 1003, ProgressiveLevelType::AccountingOnly	, "ac"	 , "url3", 1, { 1    }, 1 }
                };

      			for (const auto& level : data)
    			{
                    shared_ptr<CProgressiveLevel> tempObj = level.CreateProgressiveLevel();
    				levelManager.AddLevel( tempObj );
    			}

                uint64_t levelId = (uint64_t) levelManager.GetLevelIdFromSASLevel(31);
                ASSERT_EQ((uint64_t) 1002, levelId ) << "GetLevelIdFromSASLevel(31)";

                levelId = levelManager.GetLevelIdFromSASLevel(5);		//Should Exist
                ASSERT_EQ((uint64_t) 1000, levelId) << "GetLevelIdFromSASLevel(31)";

                levelId = levelManager.GetLevelIdFromSASLevel(2);		//Should Exist
                ASSERT_EQ((uint64_t) 1000, levelId) << "GetLevelIdFromSASLevel(31)";

                levelId = levelManager.GetLevelIdFromSASLevel(1);		//Should Exist
                ASSERT_EQ((uint64_t) 1003, levelId) << "GetLevelIdFromSASLevel(1)";

                levelId = levelManager.GetLevelIdFromSASLevel(19);		//Should NOT Exist
                ASSERT_EQ((uint64_t) 0, levelId) << "GetLevelIdFromSASLevel(19)";
            }

            void f_ProgressiveLevelManager_multipleSasLevels()
            {
                _ICardConfig5 cardConfig;

                cardConfig.Games[1]->Levels[1]->LevelId = 5009;
                cardConfig.Games[1]->Levels[1]->LevelSequence = 5;

                shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager();

                ASSERT_EQ((size_t) 3, levelManager->m_levels.size());

                for (const auto& level : levelManager->m_levels)
                {
                    CProgressiveLevel* pLevel = level.second.get();
                    int64_t levelId = pLevel->GetID();
                    switch (levelId)
                    {
                        case 5:
                            ASSERT_EQ((size_t) 0, pLevel->m_sasLevels.size()) << AssertMsg::Format(__LINE__, nullptr,L"levelId=%lld m_sasLevels.size", levelId);
                            break;

                        case 5009:
                            ASSERT_EQ((size_t) 2,		  pLevel->m_sasLevels.size()) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_sasLevels.size", levelId);
                            ASSERT_EQ((size_t) 1, (size_t) pLevel->m_sasLevels[0]) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_sasLevels.size", levelId);
                            ASSERT_EQ((size_t) 5, (size_t) pLevel->m_sasLevels[1]) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_sasLevels.size", levelId);
                            break;

                        case 61234:
                            ASSERT_EQ((size_t) 1, pLevel->m_sasLevels.size()) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_sasLevels.size", levelId);
                            break;
                    }
                }
            }

            void f_ProgressiveLevelManager_HasLevelType_Standard_true()
            {
                _ICardConfig5 cardConfig;
                shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager();

                bool result = levelManager->HasLevelType(ProgressiveLevelType::Standard);
                ASSERT_TRUE(result);
            }

            void f_ProgressiveLevelManager_HasLevelType_Standard_false( void )
            {
    			_ICardConfig5 cardConfig;
    			cardConfig.CardLevels[1]->Type = ProgressiveLevelType::AccountingOnly;
    			cardConfig.CardLevels[2]->Type = ProgressiveLevelType::AccountingOnly;
    			shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager();

    			bool result = levelManager->HasLevelType(ProgressiveLevelType::Standard);
                ASSERT_FALSE(result);
            }

            void f_ProgressiveLevelManager_HasLevelType_Bonus_true()
            {
    			_ICardConfig5 cardConfig;
    			shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager();

    			bool result = levelManager->HasLevelType(ProgressiveLevelType::Bonus);
                ASSERT_TRUE(result);
            }

            void f_ProgressiveLevelManager_HasLevelType_Bonus_false( void )
            {
    			_ICardConfig5 cardConfig;
    			cardConfig.Games[0]->Levels[0]->LevelSequence = 2;
    			cardConfig.CardLevels[0]->Type = ProgressiveLevelType::Standard;
    			shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager();

    			bool result = levelManager->HasLevelType(ProgressiveLevelType::Bonus);
                ASSERT_FALSE(result);
            }

            void f_ProgressiveLevelManager_HasLevelType_AccountingOnly_true( void )
            {
    			_ICardConfig5 cardConfig;
    			rapidjson::Document jsonDoc;
                cardConfig.CreateJsonDoc(jsonDoc);
    			shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager();

    			bool result = levelManager->HasLevelType(ProgressiveLevelType::AccountingOnly);
                ASSERT_TRUE(result);
            }

            void f_ProgressiveLevelManager_HasLevelType_AccountingOnly_false (void )
            {
    			_ICardConfig5 cardConfig;
    			cardConfig.CardLevels[3]->Type = ProgressiveLevelType::Standard;
    			shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager();

    			bool result = levelManager->HasLevelType(ProgressiveLevelType::AccountingOnly);
                ASSERT_FALSE(result);
            }

            void f_ProgressiveLevelManager_ProcessBonusingAvailable( void )
            {
               _ICardConfig5 cardConfig;
               shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager();

               rapidjson::Document jsonDoc;
               if (jsonDoc.Parse(pJson).HasParseError())
                   throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

               CProgressiveBonusingAvailable progressiveBonusingAvailable(1, jsonDoc);
               CProgressiveEligibleLevelsInfo eligibleLevels(jsonDoc);

               levelManager->ProcessBonusingAvailable(&progressiveBonusingAvailable, &eligibleLevels);

               ASSERT_TRUE(levelManager->m_eligibilityDetermined) << "m_eligibilityDetermined";
               ASSERT_EQ(1, levelManager->m_eligibilitySessionId) << "m_eligibilitySessionId";

               for (const auto& level : levelManager->m_levels)
               {
                   const CProgressiveLevel* pLevel = level.second.get();
                   switch (pLevel->GetID())
                   {
                       case 5:
                           ASSERT_TRUE(pLevel->IsEligible()) <<  AssertMsg::Format(__LINE__, L"Level 5", L"m_eligible");
                           ASSERT_FALSE(pLevel->ShowOdometer()) <<  AssertMsg::Format(__LINE__, L"Level 5", L"m_showOdometer");
                           break;

                       case 5009:
                           ASSERT_TRUE(pLevel->IsEligible() ) <<  AssertMsg::Format(__LINE__, L"Level 5009", L"m_eligible");
                           ASSERT_FALSE(pLevel->ShowOdometer()) <<  AssertMsg::Format(__LINE__, L"Level 5009", L"m_showOdometer");
                           break;

                       case 61234:
                           ASSERT_TRUE(pLevel->IsEligible() ) <<  AssertMsg::Format(__LINE__, L"Level 61234", L"m_eligible");
                           ASSERT_FALSE(pLevel->ShowOdometer()) <<  AssertMsg::Format(__LINE__, L"Level 61234", L"m_showOdometer");
                           break;

                       case 5010:
                           ASSERT_TRUE(pLevel->IsEligible()) <<  AssertMsg::Format(__LINE__, L"Level 5010", L"m_eligible");
                           ASSERT_FALSE(pLevel->ShowOdometer())  <<  AssertMsg::Format(__LINE__, L"Level 5010", L"m_showOdometer");
                           break;
                   }
               }
           }

         void f_ProgressiveLevelManager_Copy_ProcessBonusingAvailable( void )
         {
               _ICardConfig5 cardConfig;
               shared_ptr<ProgressiveLevelManager> levelManagerOrig = cardConfig.CreateLevelManager();

               rapidjson::Document jsonDoc;
               if (jsonDoc.Parse(pJson).HasParseError())
                   throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

               CProgressiveBonusingAvailable progressiveBonusingAvailable(1, jsonDoc);
               CProgressiveEligibleLevelsInfo eligibleLevels(jsonDoc);

               levelManagerOrig->ProcessBonusingAvailable(&progressiveBonusingAvailable, &eligibleLevels);

               shared_ptr<ProgressiveLevelManager> levelManager(new ProgressiveLevelManager(*levelManagerOrig.get()));
               ASSERT_TRUE( levelManager->m_eligibilityDetermined ) <<  "m_eligibilityDetermined";
               ASSERT_EQ( 1, levelManager->m_eligibilitySessionId ) << "m_eligibilitySessionId";

               for (const auto& level : levelManager->m_levels)
               {
                   const CProgressiveLevel* pLevel = level.second.get();
                   switch (pLevel->GetID())
                   {
                       case 5:
                           ASSERT_TRUE(pLevel->IsEligible() ) <<  AssertMsg::Format(__LINE__, L"Level 5", L"m_eligible");
                           ASSERT_FALSE(pLevel->ShowOdometer()) <<  AssertMsg::Format(__LINE__, L"Level 5", L"m_showOdometer");
                           break;

                       case 5009:
                           ASSERT_TRUE(pLevel->IsEligible() ) <<  AssertMsg::Format(__LINE__, L"Level 5009", L"m_eligible");
                           ASSERT_FALSE(pLevel->ShowOdometer() ) << AssertMsg::Format(__LINE__, L"Level 5009", L"m_showOdometer");
                           break;

                       case 61234:
                           ASSERT_TRUE(pLevel->IsEligible() ) <<  AssertMsg::Format(__LINE__, L"Level 61234", L"m_eligible");
                           ASSERT_FALSE(pLevel->ShowOdometer() ) <<  AssertMsg::Format(__LINE__, L"Level 61234", L"m_showOdometer");
                           break;

                       case 5010:
                           ASSERT_TRUE(pLevel->IsEligible() ) <<  AssertMsg::Format(__LINE__, L"Level 5010", L"m_eligible");
                           ASSERT_FALSE(pLevel->ShowOdometer()) <<  AssertMsg::Format(__LINE__, L"Level 5010", L"m_showOdometer");
                           break;
                   }
               }
         }


         void f_ProgressiveLevelManager_Assign_ProcessBonusingAvailable( void )
         {
             _ICardConfig5 cardConfig;
             shared_ptr<ProgressiveLevelManager> levelManagerOrig = cardConfig.CreateLevelManager();

             rapidjson::Document jsonDoc;
             if (jsonDoc.Parse(pJson).HasParseError())
                 throw JsonException(jsonDoc.GetParseError(), nullptr, "Parse");

             CProgressiveBonusingAvailable progressiveBonusingAvailable(1, jsonDoc);
             CProgressiveEligibleLevelsInfo eligibleLevels(jsonDoc);

             levelManagerOrig->ProcessBonusingAvailable(&progressiveBonusingAvailable, &eligibleLevels);

             shared_ptr<ProgressiveLevelManager> levelManager(new ProgressiveLevelManager());
             ASSERT_FALSE(levelManager->m_eligibilityDetermined ) << "m_eligibilityDetermined before assignment";
             ASSERT_TRUE(levelManager->m_levels.empty()) << "m_levels.empty() before assignment";

             *levelManager = *levelManagerOrig;
             ASSERT_TRUE(levelManager->m_eligibilityDetermined) << "m_eligibilityDetermined";
             ASSERT_EQ(1, levelManager->m_eligibilitySessionId) << "m_eligibilitySessionId";

             for (const auto& level : levelManager->m_levels)
             {
                 const CProgressiveLevel* pLevel = level.second.get();
                 switch (pLevel->GetID())
                 {
                 case 5:
                     ASSERT_TRUE(pLevel->IsEligible()) << AssertMsg::Format(__LINE__, L"Level 5", L"m_eligible");
                     ASSERT_FALSE(pLevel->ShowOdometer()) <<  AssertMsg::Format(__LINE__, L"Level 5", L"m_showOdometer");
                     break;

                 case 5009:
                     ASSERT_TRUE(pLevel->IsEligible()) <<  AssertMsg::Format(__LINE__, L"Level 5009", L"m_eligible");
                     ASSERT_FALSE(pLevel->ShowOdometer()) <<  AssertMsg::Format(__LINE__, L"Level 5009", L"m_showOdometer");
                     break;

                 case 61234:
                     ASSERT_TRUE(pLevel->IsEligible()) <<  AssertMsg::Format(__LINE__, L"Level 61234", L"m_eligible");
                     ASSERT_FALSE(pLevel->ShowOdometer() ) <<  AssertMsg::Format(__LINE__, L"Level 61234", L"m_showOdometer");
                     break;

                 case 5010:
                     ASSERT_TRUE(pLevel->IsEligible()) <<  AssertMsg::Format(__LINE__, L"Level 5010", L"m_eligible");
                     ASSERT_FALSE(pLevel->ShowOdometer()) << AssertMsg::Format(__LINE__, L"Level 5010", L"m_showOdometer");
                     break;
                 }
             }
         }


         void  f_ProgressiveLevelManager_ProcessLevelBroadcast( void )
        {
			const DWORD receivedTickCount = 5000;
			const DWORD updatedTickCount = 8000;

			_ICardConfig5 cardConfig;
			shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(receivedTickCount);

			CProgressiveLevelBroadcast levelBroadcast(updatedTickCount);
			levelBroadcast.AddLevel(5, 1234, INVALID_VALUE, INVALID_VALUE, "Floorwide Value Mystery 001");
			levelBroadcast.AddLevel(61234, 5678, INVALID_VALUE, INVALID_VALUE, "z");

			levelManager->ProcessLevelBroadcast(&levelBroadcast);

			for (const auto& level : levelManager->m_levels)
			{
				CProgressiveLevel* pLevel = level.second.get();
				switch (pLevel->m_id)
				{
					case 5:
						ASSERT_EQ((int) CProgressiveLevel::Updated, (int) pLevel->m_state ) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_state", pLevel->m_id);
						ASSERT_EQ((long) 1234, (long) pLevel->m_value ) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_value", pLevel->m_id);
						ASSERT_STREQ(_T("Floorwide Value Mystery 001"), pLevel->m_name.c_str() ) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_name", pLevel->m_id);
						ASSERT_EQ(updatedTickCount, pLevel->m_updatedTickCount) << AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_updatedTickCount", pLevel->m_id);
						break;

					case 5009:
						ASSERT_EQ((int) CProgressiveLevel::Configured, (int) pLevel->m_state) << AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_state", pLevel->m_id);
						ASSERT_EQ((long) 0, (long) pLevel->m_value) << AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_value", pLevel->m_id);
						ASSERT_STREQ(_T(""), pLevel->m_name.c_str()) << AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_name", pLevel->m_id);
						ASSERT_EQ((long) receivedTickCount, (long) pLevel->m_updatedTickCount) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_updatedTickCount", pLevel->m_id);
						break;

					case 61234:
						ASSERT_EQ((int) CProgressiveLevel::Updated, (int) pLevel->m_state ) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_state", pLevel->m_id);
						ASSERT_EQ((long) 5678, (long) pLevel->m_value) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_value", pLevel->m_id);
						ASSERT_STREQ(_T("z"), pLevel->m_name.c_str()) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_name", pLevel->m_id);
						ASSERT_EQ((long) updatedTickCount, (long) pLevel->m_updatedTickCount) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_updatedTickCount", pLevel->m_id);
						break;

					case 5010:
						ASSERT_EQ((int) CProgressiveLevel::Configured, (int) pLevel->m_state ) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_state", pLevel->m_id);
						ASSERT_EQ((long) 0, (long) pLevel->m_value ) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_value", pLevel->m_id);
						ASSERT_STREQ(_T("x"), pLevel->m_name.c_str()) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_name", pLevel->m_id);
						ASSERT_EQ((long) receivedTickCount, (long) pLevel->m_updatedTickCount ) << AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_updatedTickCount", pLevel->m_id);
						break;
				}
			}

			const DWORD updatedTickCount2 = 11000;
			CProgressiveLevelBroadcast levelBroadcast2(updatedTickCount2);

			levelBroadcast2.AddLevel(5, 1235, INVALID_VALUE, INVALID_VALUE, "Floorwide Value Mystery 002");
			levelBroadcast2.AddLevel(5010, 9876, INVALID_VALUE, INVALID_VALUE, "x");

			levelManager->ProcessLevelBroadcast(&levelBroadcast2);

			for (const auto& level : levelManager->m_levels)
			{
				CProgressiveLevel* pLevel = level.second.get();
				switch (pLevel->m_id)
				{
					case 5:
						ASSERT_EQ((int) CProgressiveLevel::Updated, (int) pLevel->m_state ) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_state", pLevel->m_id);
						ASSERT_EQ((long) 1235, (long) pLevel->m_value) << AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_value", pLevel->m_id);
						ASSERT_STREQ(_T("Floorwide Value Mystery 002"), pLevel->m_name.c_str()) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_name", pLevel->m_id);
						ASSERT_EQ(updatedTickCount2, pLevel->m_updatedTickCount) << AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_updatedTickCount", pLevel->m_id);
						break;

					case 5009:
						ASSERT_EQ((int) CProgressiveLevel::Configured, (int) pLevel->m_state) << AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_state", pLevel->m_id);
						ASSERT_EQ((long) 0, (long) pLevel->m_value) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_value", pLevel->m_id);
						ASSERT_STREQ(_T(""), pLevel->m_name.c_str()) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_name", pLevel->m_id);
						ASSERT_EQ((long) receivedTickCount, (long) pLevel->m_updatedTickCount) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_updatedTickCount", pLevel->m_id);
						break;

					case 61234:
						ASSERT_EQ((int) CProgressiveLevel::Updated, (int) pLevel->m_state ) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_state", pLevel->m_id);
						ASSERT_EQ((long) 5678, (long) pLevel->m_value) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_value", pLevel->m_id);
						ASSERT_STREQ(_T("z"), pLevel->m_name.c_str()) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_name", pLevel->m_id);
						ASSERT_EQ((long) updatedTickCount, (long) pLevel->m_updatedTickCount) << AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_updatedTickCount", pLevel->m_id);
						break;

					case 5010:
						ASSERT_EQ((int) CProgressiveLevel::Updated, (int) pLevel->m_state) << AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_state", pLevel->m_id);
						ASSERT_EQ((long) 9876, (long) pLevel->m_value) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_value", pLevel->m_id);
						ASSERT_STREQ(_T("x"), pLevel->m_name.c_str()) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_name", pLevel->m_id);
						ASSERT_EQ((long) updatedTickCount2, (long) pLevel->m_updatedTickCount) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_updatedTickCount", pLevel->m_id);
						break;
				}
            }
        }

         void f_ProgressiveLevelManager_ProcessLevelBroadcast_nullptr( void )
         {
             const DWORD receivedTickCount = 0;

             _ICardConfig5 cardConfig;
             shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(&receivedTickCount);

             levelManager->ProcessLevelBroadcast(nullptr);

             for (const auto& level : levelManager->m_levels)
             {
                 CProgressiveLevel* pLevel = level.second.get();

                 ASSERT_EQ((int)CProgressiveLevel::Configured, (int) pLevel->m_state) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_state", pLevel->m_id);
                 ASSERT_EQ((long) 0, (long) pLevel->m_value) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_value", pLevel->m_id);
                 ASSERT_EQ((long) 0, (long) pLevel->m_updatedTickCount) <<  AssertMsg::Format(__LINE__, nullptr, L"levelId=%lld m_updatedTickCount", pLevel->m_id);
             }
         }

         void  f_ProgressiveLevelManager_ProcessLevelBroadcast_StandardLesser00()
        {
            const bool needForwardExpected1(false);
            const bool needForwardExpected2(false);

            const DWORD receivedTickCount = 5000;

            _ICardConfig5 cardConfig;
            shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(&receivedTickCount);

            ASSERT_FALSE(levelManager->m_needPriorityForward)<< "Need priority Forward true";

            // Perform operation(s) and test(s).
            {
                CProgressiveLevelBroadcast levelBroadcast(8000);
                levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
                levelBroadcast.AddLevel(61234, 5678, INVALID_VALUE, INVALID_VALUE, "z");

                bool needForward(levelManager->ProcessLevelBroadcast(&levelBroadcast));
                ASSERT_EQ(needForwardExpected1, needForward);
                ASSERT_TRUE(needForwardExpected1 == levelManager->m_needPriorityForward);
            }

            {
                CProgressiveLevelBroadcast levelBroadcast(10000);
                levelBroadcast.AddLevel(5009, 1235, INVALID_VALUE, INVALID_VALUE, nullptr);
                levelBroadcast.AddLevel(61234, 5679, INVALID_VALUE, INVALID_VALUE, "z");

                bool needForward(levelManager->ProcessLevelBroadcast(&levelBroadcast));
                ASSERT_EQ(needForwardExpected2, needForward);
                ASSERT_TRUE((needForwardExpected1 || needForwardExpected2)== levelManager->m_needPriorityForward);
            }
         }

        void f_ProgressiveLevelManager_ProcessLevelBroadcast_StandardLesser01( void )
        {
            DWORD receivedTickCount = 5000;

            _ICardConfig5 cardConfig;
            shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(&receivedTickCount);

            {
                CProgressiveLevelBroadcast levelBroadcast(8000);
                levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
                levelBroadcast.AddLevel(61234, 5678, INVALID_VALUE, INVALID_VALUE, "z");

                bool needForward = levelManager->ProcessLevelBroadcast(&levelBroadcast);
                ASSERT_FALSE( needForward ) << "return";
                ASSERT_FALSE(levelManager->m_needPriorityForward) << "m_needPriorityForward";
            }

            {
                CProgressiveLevelBroadcast levelBroadcast(10000);
                levelBroadcast.AddLevel(5009, 1235, INVALID_VALUE, INVALID_VALUE, nullptr);
                levelBroadcast.AddLevel(61234, 5677, INVALID_VALUE, INVALID_VALUE, "z");

                bool needForward = levelManager->ProcessLevelBroadcast(&levelBroadcast);
                ASSERT_TRUE(needForward) << "return";
                ASSERT_TRUE(levelManager->m_needPriorityForward) << "m_needPriorityForward";
            }
        }

        void f_ProgressiveLevelManager_ProcessLevelBroadcast_StandardLesser10( void )
        {
            const bool needForwardExpected1(false);
            const bool needForwardExpected2(true);

            DWORD receivedTickCount = 5000;

            _ICardConfig5 cardConfig;
            shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(&receivedTickCount);
            ASSERT_FALSE(levelManager->m_needPriorityForward); // Verify setup.

            // Perform operation(s) and test(s).
            {
                CProgressiveLevelBroadcast levelBroadcast(8000);
                levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
                levelBroadcast.AddLevel(61234, 5678, INVALID_VALUE, INVALID_VALUE, "z");

                bool needForward(levelManager->ProcessLevelBroadcast(&levelBroadcast));
                ASSERT_EQ(needForwardExpected1, needForward);
                ASSERT_TRUE(needForwardExpected1==levelManager->m_needPriorityForward);
            }

            {
                CProgressiveLevelBroadcast levelBroadcast(10000);
                levelBroadcast.AddLevel(5009, 1233, INVALID_VALUE, INVALID_VALUE, nullptr);
                levelBroadcast.AddLevel(61234, 5679, INVALID_VALUE, INVALID_VALUE, "z");

                bool needForward(levelManager->ProcessLevelBroadcast(&levelBroadcast));
                ASSERT_EQ(needForwardExpected2, needForward);
                ASSERT_TRUE((needForwardExpected1 || needForwardExpected2) == levelManager->m_needPriorityForward);
            }
        }

        void f_ProgressiveLevelManager_ProcessLevelBroadcast_StandardLesser11( void )
        {
            const bool needForwardExpected1(false);
            const bool needForwardExpected2(true);

            DWORD receivedTickCount = 5000;

            _ICardConfig5 cardConfig;
            shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(&receivedTickCount);
            ASSERT_FALSE(levelManager->m_needPriorityForward); // Verify setup.

            // Perform operation(s) and test(s).
            {
                CProgressiveLevelBroadcast levelBroadcast(8000);
                levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
                levelBroadcast.AddLevel(61234, 5678, INVALID_VALUE, INVALID_VALUE, "z");

                bool needForward(levelManager->ProcessLevelBroadcast(&levelBroadcast));
                ASSERT_EQ(needForwardExpected1, needForward);
                ASSERT_TRUE(needForwardExpected1 == levelManager->m_needPriorityForward);
            }

            {
                CProgressiveLevelBroadcast levelBroadcast(10000);
                levelBroadcast.AddLevel(5009, 1233, INVALID_VALUE, INVALID_VALUE, nullptr);
                levelBroadcast.AddLevel(61234, 5677, INVALID_VALUE, INVALID_VALUE, "z");

                bool needForward(levelManager->ProcessLevelBroadcast(&levelBroadcast));
                ASSERT_EQ(needForwardExpected2, needForward);
                ASSERT_TRUE((needForwardExpected1 || needForwardExpected2) == levelManager->m_needPriorityForward);
            }
        }

        void f_ProgressiveLevelManager_ProcessLevelBroadcast_BonusLesser( void )
        {
            const bool needForwardExpected1(false);
            const bool needForwardExpected2(false);

            // Setup.
            DWORD receivedTickCount = 5000;

            _ICardConfig5 cardConfig;
            shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(&receivedTickCount);
            ASSERT_FALSE(levelManager->m_needPriorityForward); // Verify setup.

            // Perform operation(s) and test(s).
            {
                CProgressiveLevelBroadcast levelBroadcast(8000);
                levelBroadcast.AddLevel(5, 1234, INVALID_VALUE, INVALID_VALUE, "Floorwide Value Mystery 001");

                bool needForward(levelManager->ProcessLevelBroadcast(&levelBroadcast));
                ASSERT_EQ(needForwardExpected1, needForward);
                ASSERT_TRUE(needForwardExpected1== levelManager->m_needPriorityForward);
            }

            {
                CProgressiveLevelBroadcast levelBroadcast(10000);
                levelBroadcast.AddLevel(5, 1233, INVALID_VALUE, INVALID_VALUE, "Floorwide Value Mystery 001");

                bool needForward(levelManager->ProcessLevelBroadcast(&levelBroadcast));
                ASSERT_EQ(needForwardExpected2, needForward);
                ASSERT_TRUE((needForwardExpected1 || needForwardExpected2) == levelManager->m_needPriorityForward);
            }
        }

        void f_ProgressiveLevelManager_ProcessLevelBroadcast_StandardLesserGreater( void )
        {
            const bool needForwardExpected1(false);
            const bool needForwardExpected2(true);

            // Setup.
            DWORD receivedTickCount = 5000;

            _ICardConfig5 cardConfig;
            shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(&receivedTickCount);
            ASSERT_FALSE(levelManager->m_needPriorityForward); // Verify setup.

            // Perform operation(s) and test(s).
            {
                CProgressiveLevelBroadcast levelBroadcast(8000);
                levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
                levelBroadcast.AddLevel(61234, 5678, INVALID_VALUE, INVALID_VALUE, "z");

                bool needForward(levelManager->ProcessLevelBroadcast(&levelBroadcast));
                ASSERT_EQ(needForwardExpected1, needForward);
                ASSERT_TRUE(needForwardExpected1 == levelManager->m_needPriorityForward);
            }

            {
                CProgressiveLevelBroadcast levelBroadcast(10000);
                levelBroadcast.AddLevel(5009, 1233, INVALID_VALUE, INVALID_VALUE, nullptr);
                levelBroadcast.AddLevel(61234, 5677, INVALID_VALUE, INVALID_VALUE, "z");

                bool needForward(levelManager->ProcessLevelBroadcast(&levelBroadcast));
                ASSERT_EQ(needForwardExpected2, needForward);
                ASSERT_TRUE((needForwardExpected1 || needForwardExpected2) == levelManager->m_needPriorityForward);
            }

            {
                CProgressiveLevelBroadcast levelBroadcast(12000);
                levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
                levelBroadcast.AddLevel(61234, 5678, INVALID_VALUE, INVALID_VALUE, "z");

                bool needForward(levelManager->ProcessLevelBroadcast(&levelBroadcast));
                ASSERT_EQ(needForwardExpected1, needForward);
                ASSERT_TRUE((needForwardExpected1 || needForwardExpected2 ) == levelManager->m_needPriorityForward);
            }
        }

       void f_ProgressiveLevelManager_ProcessLevelBroadcast_AccountingOnlyLesser( void )
        {
            const bool needForwardExpected1(false);
            const bool needForwardExpected2(false);

            // Setup.
            DWORD receivedTickCount = 5000;

            _ICardConfig5 cardConfig;
            shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(&receivedTickCount);
            ASSERT_FALSE(levelManager->m_needPriorityForward); // Verify setup.

            // Perform operation(s) and test(s).
            {
                CProgressiveLevelBroadcast levelBroadcast(8000);
                levelBroadcast.AddLevel(5010, 1234, INVALID_VALUE, INVALID_VALUE, "x");

                bool needForward(levelManager->ProcessLevelBroadcast(&levelBroadcast));
                ASSERT_EQ(needForwardExpected1, needForward);
                ASSERT_TRUE(needForwardExpected1 == levelManager->m_needPriorityForward);
            }

            {
                CProgressiveLevelBroadcast levelBroadcast(10000);
                levelBroadcast.AddLevel(5010, 1233, INVALID_VALUE, INVALID_VALUE, "x");

                bool needForward(levelManager->ProcessLevelBroadcast(&levelBroadcast));
                ASSERT_EQ(needForwardExpected2, needForward);
                ASSERT_TRUE((needForwardExpected1 || needForwardExpected2) == levelManager->m_needPriorityForward);
            }
        }

       void f_ProgressiveLevelManager_SetForwardedTickCount_Multiple( void )
       {
           deque<DWORD> expected(0);
           //deque_vector<DWORD, ProgressiveLevelManager::PriorityForwardWindowMaxUpdates> expected(0);

            ProgressiveLevelManager levelManager;

            for (size_t i = 0; i < 2 * expected.size(); ++i)
            {
                DWORD tickCountTest = (DWORD)(1000 + i);

                expected.push_front(tickCountTest);
                levelManager.SetForwardedTickCount(tickCountTest);

                ASSERT_EQ(expected.size(), levelManager.m_lastForwardedTickCounts.size()) <<  AssertMsg::FormatOnly("m_lastForwardedTickCounts.size(): Test %u", i);
                for (size_t k = 0; k < ProgressiveLevelManager::PriorityForwardWindowMaxUpdates; ++k)
                {
                    ASSERT_EQ(expected[k], levelManager.m_lastForwardedTickCounts[k]) << AssertMsg::FormatOnly("m_lastForwardedTickCounts[%u]", k);
                }
            }
        }

       void f_ProgressiveLevelManager_OkToSendPriorityForward_DefaultJson( void )
       {
           const DWORD receivedTickCount = 5000;

           _ICardConfig5 cardConfig;
           shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(&receivedTickCount);

           ASSERT_FALSE(levelManager->OkToSendPriorityForward((DWORD) -1)) <<  L"(DWORD) -1";
           ASSERT_FALSE(levelManager->OkToSendPriorityForward(0))<<  "0";
           ASSERT_FALSE(levelManager->OkToSendPriorityForward(ProgressiveLevelManager::PriorityForwardWindowTimeout - 1)) <<  "PriorityForwardWindowTimeout - 1";
           ASSERT_FALSE(levelManager->OkToSendPriorityForward(ProgressiveLevelManager::PriorityForwardWindowTimeout)) <<  "PriorityForwardWindowTimeout";
       }

       void f_ProgressiveLevelManager_OkToSendPriorityForward_DefaultAndLesser( void )
       {
           const DWORD receivedTickCount = 5000;

           _ICardConfig5 cardConfig;
           shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(&receivedTickCount);

           {
               CProgressiveLevelBroadcast levelBroadcast(8000);
               levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
               levelManager->ProcessLevelBroadcast(&levelBroadcast);
           }

           {
               CProgressiveLevelBroadcast levelBroadcast(10000);
               levelBroadcast.AddLevel(5009, 1233, INVALID_VALUE, INVALID_VALUE, nullptr);
               levelManager->ProcessLevelBroadcast(&levelBroadcast);
           }

           ASSERT_TRUE(levelManager->OkToSendPriorityForward((DWORD) -1)) << "(DWORD) -1";
           ASSERT_FALSE(levelManager->OkToSendPriorityForward(0)) << "0";
           ASSERT_FALSE(levelManager->OkToSendPriorityForward(ProgressiveLevelManager::PriorityForwardWindowTimeout - 1)) << "PriorityForwardWindowTimeout - 1";
           ASSERT_TRUE(levelManager->OkToSendPriorityForward(ProgressiveLevelManager::PriorityForwardWindowTimeout)) << "PriorityForwardWindowTimeout";
       }

       void f_ProgresiveLevelManager_OkToSendPriorityForward_Multiple( void )
        {
            const DWORD receivedTickCount = 5000;

            _ICardConfig5 cardConfig;
            shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(&receivedTickCount);

            {
                CProgressiveLevelBroadcast levelBroadcast(8000);
                levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
                levelManager->ProcessLevelBroadcast(&levelBroadcast);
            }

            {
                CProgressiveLevelBroadcast levelBroadcast(10000);
                levelBroadcast.AddLevel(5009, 1233, INVALID_VALUE, INVALID_VALUE, nullptr);
                levelManager->ProcessLevelBroadcast(&levelBroadcast);
            }

           deque<DWORD> expected(0);

            for (size_t i = 0; i < 2 * expected.size(); ++i)
            {
                const DWORD tickCountSet = 10000;
                DWORD tickCountTest = tickCountSet + 200 * (DWORD) i;

                expected.push_front(tickCountTest);
                levelManager->SetForwardedTickCount(tickCountTest);

                DWORD oldestForwardedTickCount = expected.back();

                ASSERT_TRUE(levelManager->OkToSendPriorityForward(oldestForwardedTickCount - 1)) << 
                    AssertMsg::Format(i, L"oldestForwardedTickCount - 1", L"tickCountTest: %u", tickCountTest);

                ASSERT_FALSE(levelManager->OkToSendPriorityForward(oldestForwardedTickCount)) <<
                    AssertMsg::Format(i, L"oldestForwardedTickCount", L"tickCountTest: %u", tickCountTest);

                ASSERT_FALSE(levelManager->OkToSendPriorityForward(oldestForwardedTickCount + ProgressiveLevelManager::PriorityForwardWindowTimeout - 1)) << 
                    AssertMsg::Format(i, L"oldestForwardedTickCount + PriorityForwardWindowTimeout - 1", L"tickCountTest: %u", tickCountTest);

                ASSERT_TRUE(levelManager->OkToSendPriorityForward(oldestForwardedTickCount + ProgressiveLevelManager::PriorityForwardWindowTimeout)) << 
                    AssertMsg::Format(i, L"oldestForwardedTickCount + PriorityForwardWindowTimeout", L"tickCountTest: %u", tickCountTest);
            }
        }

       void f_ProgressiveLevelManager_GetLevelForward_Initial( void )
       {
           _ICardConfig5 cardConfig;
           shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(5000);

           unique_ptr<CProgressiveLevelForward> levelForward(levelManager->GetLevelForward(5000));
           ASSERT_TRUE(levelForward.get()==nullptr) << "GetLevelForward(5000)";

           {
               CProgressiveLevelBroadcast levelBroadcast(8000);
               levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
               levelManager->ProcessLevelBroadcast(&levelBroadcast);
           }

           levelForward.reset(levelManager->GetLevelForward(8000));
           ASSERT_TRUE(levelForward.get()!=nullptr ) <<  "GetLevelForward(8000)";
           ASSERT_EQ((size_t) 1, levelForward->GetNumLevels() ) <<  "GetNumLevels()";
       }

       void f_ProgressiveLevelManager_GetLevelForward_StrobeInterval( void )
       {
           const DWORD configTickCount		= 5000;
           const DWORD broadcastTickCount	= 10000;
           const DWORD baseTickCount		= broadcastTickCount + 50;
           const DWORD strobeInterval		= CProgressiveLevelForward::StrobeInterval;

           _ICardConfig5 cardConfig;
           shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(configTickCount);

           bool multicastActiveChanged;
           unique_ptr<CProgressiveLevelForward> levelForward(levelManager->GetLevelForward(configTickCount, false, multicastActiveChanged));
           ASSERT_TRUE(levelForward.get()== nullptr) << AssertMsg::FormatOnly(L"pLevelForward(%u)", configTickCount);

           {
               CProgressiveLevelBroadcast levelBroadcast(broadcastTickCount);
               levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
               levelManager->ProcessLevelBroadcast(&levelBroadcast);
           }

           struct
           {
               int    line;
               DWORD  tickCount;
               bool   isDown;
               size_t expectedNumLevels;
               bool   expectedMulticastActive;
               bool   expectedMulticastActiveChanged;
           }
           data[] =
           {
               { __LINE__, baseTickCount							, false, 1,  true, false },
               { __LINE__, baseTickCount + strobeInterval - 1		, false, 0,  true, false },
               { __LINE__, baseTickCount + strobeInterval			, false, 1, false,  true },
               { __LINE__, baseTickCount + (2 * strobeInterval) - 1, false, 0, false, false },
               { __LINE__, baseTickCount + (2 * strobeInterval)	, false, 1, false, false }
           };

           for (size_t i = 0; i < _countof(data); i++)
           {
               bool multicastActiveChanged;
               levelForward.reset(levelManager->GetLevelForward(data[i].tickCount, data[i].isDown, multicastActiveChanged));
               if (data[i].expectedNumLevels)
               {
                   ASSERT_TRUE(levelForward.get()) <<  AssertMsg::Format(data[i].line ,  L"pLevelForward");
                   ASSERT_EQ(data[i].expectedNumLevels, levelForward->GetNumLevels()) <<  AssertMsg::Format(data[i].line, L"GetNumLevels()");
               }
               else
               {
                   ASSERT_TRUE(levelForward.get() ==nullptr) <<  AssertMsg::Format(data[i].line, L"pLevelForward");
               }
               ASSERT_EQ(data[i].expectedMulticastActive, levelManager->GetMulticastActive()) <<  AssertMsg::Format(data[i].line, L"GetMulticastActive()");
               ASSERT_EQ(data[i].expectedMulticastActiveChanged, multicastActiveChanged) <<  AssertMsg::Format(data[i].line, L"multicastActiveChanged");
           }
       }

        void f_ProgressiveLevelManager_GetLevelForward_PriorityForwardLimit()
        {
            const DWORD configTickCount		 = 5000;
            const DWORD broadcastTickCount1  = 10000;
            const DWORD broadcastTickCount2  = broadcastTickCount1 + (CProgressiveLevelForward::MinInterval / 2);
            const DWORD getForwardTickCount  = 10050;

            _ICardConfig5 cardConfig;
            shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(configTickCount);

            {
                CProgressiveLevelBroadcast levelBroadcast(broadcastTickCount1);
                levelBroadcast.AddLevel(5009,  1234, INVALID_VALUE, INVALID_VALUE, nullptr);
                levelBroadcast.AddLevel(61234, 5678, INVALID_VALUE, INVALID_VALUE, "z");
                bool needForward = levelManager->ProcessLevelBroadcast(&levelBroadcast);
                ASSERT_FALSE(needForward) << "ProcessLevelBroadcast: 1";
            }

            bool multicastActiveChanged;
            DWORD tickCountTest = broadcastTickCount1;
            unique_ptr<CProgressiveLevelForward> levelForward(levelManager->GetLevelForward(tickCountTest, false, multicastActiveChanged));
            ASSERT_TRUE(levelForward.get()!=nullptr) <<  "levelForward: 1";
            ASSERT_EQ((size_t) 2, levelForward->GetNumLevels()) << "levelForward: 1";
            ASSERT_TRUE(levelManager->GetMulticastActive()) << "GetMulticastActive(): 1";
            ASSERT_FALSE(multicastActiveChanged) << "multicastActiveChanged: 1";

            size_t levelForwardCount = 1; // Number of times that levels have been forwarded.
            DWORD lastForwardTickCount = tickCountTest;
            const DWORD testIntervaltickCount = CProgressiveLevelForward::MinInterval / ProgressiveLevelManager::PriorityForwardWindowMaxUpdates / 4;

            for (size_t i = 0; i < 2 * ProgressiveLevelManager::PriorityForwardWindowMaxUpdates; ++i)
            {
                tickCountTest += testIntervaltickCount;

                CProgressiveLevelBroadcast levelBroadcast(tickCountTest);
                if (0 == (i & 1))
                {
                    // Broadcast with first level amount smaller than previous value.
                    levelBroadcast.AddLevel( 5009, 1233, INVALID_VALUE, INVALID_VALUE, nullptr);
                    levelBroadcast.AddLevel(61234, 5679, INVALID_VALUE, INVALID_VALUE, "z");
                }
                else
                {
                    // Broadcast with second level amount smaller than previous value.
                    levelBroadcast.AddLevel( 5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
                    levelBroadcast.AddLevel(61234, 5678, INVALID_VALUE, INVALID_VALUE, "z");
                }
                bool needForward = levelManager->ProcessLevelBroadcast(&levelBroadcast);
                ASSERT_TRUE(needForward) << AssertMsg::Format(i, L"ProcessLevelBroadcast return");

                unique_ptr<CProgressiveLevelForward> levelForward(levelManager->GetLevelForward(tickCountTest, false, multicastActiveChanged));
                if (levelForwardCount < ProgressiveLevelManager::PriorityForwardWindowMaxUpdates)
                {
                    ASSERT_TRUE(levelForward.get()!=nullptr) <<  AssertMsg::Format(i, L"levelForward return");
                    ASSERT_EQ((size_t) 2, levelForward->GetNumLevels()) <<  AssertMsg::Format(i, L"levelForward->GetNumLevels()");
                    ++levelForwardCount;
                    lastForwardTickCount = tickCountTest;
                }
                else
                {
                    ASSERT_TRUE(levelForward.get()== nullptr) << AssertMsg::Format(i, L"levelForward return");
                }

                ASSERT_TRUE(levelManager->GetMulticastActive()) << AssertMsg::Format(i, L"GetMulticastActive()");
                ASSERT_FALSE(multicastActiveChanged ) << AssertMsg::Format(i, L"multicastActiveChanged");
            }

            // Test again before minimum interval has elapsed that a forward does not occur.
            levelForward.reset(levelManager->GetLevelForward(lastForwardTickCount + CProgressiveLevelForward::MinInterval - 1, false, multicastActiveChanged));
            ASSERT_TRUE(levelForward.get()==nullptr) << "levelForward return: Post loop 1";
            ASSERT_TRUE(levelManager->GetMulticastActive()) << "GetMulticastActive(): Post loop 1";
            ASSERT_FALSE(multicastActiveChanged) << "multicastActiveChanged: Post loop 1";

            // Test again after minimum interval has elapsed that a forward does occur.
            levelForward.reset(levelManager->GetLevelForward(lastForwardTickCount + CProgressiveLevelForward::MinInterval, false, multicastActiveChanged));
            ASSERT_TRUE(levelForward.get() !=nullptr) << "levelForward return: Post loop 2";
            ASSERT_EQ((size_t) 2, levelForward->GetNumLevels() ) <<  "levelForward->GetNumLevels(): Post loop 2";
            ASSERT_FALSE(levelManager->GetMulticastActive()) <<  "GetMulticastActive(): Post loop 2";
            ASSERT_TRUE(multicastActiveChanged) << "multicastActiveChanged: Post loop 2";

            // Test again after minimum interval has elapsed that a forward does not occur.
            levelForward.reset(levelManager->GetLevelForward(lastForwardTickCount + CProgressiveLevelForward::MinInterval, false, multicastActiveChanged));
            ASSERT_TRUE(levelForward.get() == nullptr) << "levelForward return: Post loop 3";
            ASSERT_FALSE(levelManager->GetMulticastActive() ) << "GetMulticastActive(): Post loop 3";
            ASSERT_FALSE(multicastActiveChanged ) << "multicastActiveChanged: Post loop 3";
        } 
  
        void f_ProgressiveLevelManager_GetLevelForward_MaxInterval(void)
        {
            const DWORD configTickCount		 = 5000;
            const DWORD broadcastTickCount	 = 10000;
            const DWORD tickCountMaxInterval = CProgressiveLevelForward::MaxInterval;

            _ICardConfig5 cardConfig;
            shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(configTickCount);

            bool multicastActiveChanged;
            unique_ptr<CProgressiveLevelForward> levelForward(levelManager->GetLevelForward(configTickCount, false, multicastActiveChanged));
            ASSERT_TRUE(levelForward.get() == nullptr) <<  AssertMsg::FormatOnly(L"pLevelForward(%u)", configTickCount);
            ASSERT_TRUE(levelManager->GetMulticastActive()) <<  "GetMulticastActive()";
            ASSERT_FALSE(multicastActiveChanged) << "multicastActiveChanged";

            {
                CProgressiveLevelBroadcast levelBroadcast(broadcastTickCount);
                levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
                levelManager->ProcessLevelBroadcast(&levelBroadcast);
            }

            levelForward.reset(levelManager->GetLevelForward(broadcastTickCount + tickCountMaxInterval, false, multicastActiveChanged));
            ASSERT_TRUE(levelForward.get()!=nullptr ) << "levelForward";
            ASSERT_EQ((size_t) 1, levelForward->GetNumLevels()) << "GetNumLevels()";
            ASSERT_FALSE(levelManager->GetMulticastActive()) << "GetMulticastActive()";
            ASSERT_TRUE( multicastActiveChanged ) << "multicastActiveChanged";
        } 
     
         void f_ProgressiveLevelManager_GetLevelForward_PriorityForwardTrue( void )
            {
			const DWORD configTickCount		 = 5000;
			const DWORD broadcastTickCount1  = 10000;
			const DWORD broadcastTickCount2  = broadcastTickCount1 + (CProgressiveLevelForward::MinInterval / 2);
			const DWORD getForwardTickCount  = 10050;

			_ICardConfig5 cardConfig;
			shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(configTickCount);

			{
				CProgressiveLevelBroadcast levelBroadcast(broadcastTickCount1);
				levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
				bool needForward = levelManager->ProcessLevelBroadcast(&levelBroadcast);
				ASSERT_FALSE(needForward) << "ProcessLevelBroadcast: 1";
			}

			bool multicastActiveChanged;
			unique_ptr<CProgressiveLevelForward> levelForward(levelManager->GetLevelForward(getForwardTickCount, false, multicastActiveChanged));
			ASSERT_TRUE(levelForward.get() !=nullptr) <<  "levelForward: 1";
			ASSERT_EQ((size_t) 1, levelForward->GetNumLevels()) << "GetNumLevels(): 1";
			ASSERT_TRUE(levelManager->GetMulticastActive()) << "GetMulticastActive(): 1";
			ASSERT_FALSE(multicastActiveChanged) << "multicastActiveChanged: 1";

			// Second broadcast. Level amount is greater than previous value.
			{
				CProgressiveLevelBroadcast levelBroadcast(broadcastTickCount2);
				levelBroadcast.AddLevel(5009, 1233, INVALID_VALUE, INVALID_VALUE, nullptr);
				bool needForward = levelManager->ProcessLevelBroadcast(&levelBroadcast);
				ASSERT_TRUE(needForward ) << "ProcessLevelBroadcast: 2";
			}

			levelForward.reset(levelManager->GetLevelForward(broadcastTickCount2, false, multicastActiveChanged));
			ASSERT_TRUE(levelForward.get()!=nullptr) <<  "levelForward: 2";
			ASSERT_EQ((size_t) 1, levelForward->GetNumLevels()) << "GetNumLevels(): 2";
			ASSERT_TRUE(levelManager->GetMulticastActive()) << "GetMulticastActive(): 2";
			ASSERT_FALSE(multicastActiveChanged) << "multicastActiveChanged: 2";

			levelForward.reset(levelManager->GetLevelForward(broadcastTickCount2, false, multicastActiveChanged));
			ASSERT_TRUE(levelForward.get()==nullptr) << "levelForward: 3";
			ASSERT_TRUE(levelManager->GetMulticastActive()) <<  "GetMulticastActive(): 3";
			ASSERT_FALSE(multicastActiveChanged ) << "multicastActiveChanged: 3";
        } 
        
         void f_ProgressiveLevelManager_GetLevelForward_MaxIntervalExceeded(void)
            {
    			const DWORD configTickCount		 = 5000;
    			const DWORD broadcastTickCount	 = 10000;
    			const DWORD tickCountMaxInterval = CProgressiveLevelForward::MaxInterval;

    			_ICardConfig5 cardConfig;
    			shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(configTickCount);

    			bool multicastActiveChanged;
    			unique_ptr<CProgressiveLevelForward> levelForward(levelManager->GetLevelForward(configTickCount, false, multicastActiveChanged));
    			ASSERT_TRUE(levelForward.get() ==nullptr) << AssertMsg::FormatOnly(L"pLevelForward(%u)", configTickCount);
    			ASSERT_TRUE(levelManager->GetMulticastActive()) <<  "GetMulticastActive()";
    			ASSERT_FALSE(multicastActiveChanged) << "multicastActiveChanged";

    			{
    				CProgressiveLevelBroadcast levelBroadcast(broadcastTickCount);
    				levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
    				levelManager->ProcessLevelBroadcast(&levelBroadcast);
    			}

    			levelForward.reset(levelManager->GetLevelForward(broadcastTickCount + tickCountMaxInterval + 1, false, multicastActiveChanged));
    			ASSERT_TRUE(levelForward.get() !=nullptr)   << "levelForward";
    			ASSERT_EQ((size_t) 0, levelForward->GetNumLevels()) <<  "GetNumLevels()";
    			ASSERT_FALSE(levelManager->GetMulticastActive()) << "GetMulticastActive()";
                ASSERT_TRUE(multicastActiveChanged) << "multicastActiveChanged";
            } 

			void f_ProgressiveLevelManager_GetLevelForward_MinInterval( void )
            {
    			const DWORD configTickCount		 = 5000;
    			const DWORD tickCountMinInterval = CProgressiveLevelForward::MinInterval;
    			const DWORD broadcastTickCount1	 = 10000;
    			const DWORD broadcastTickCount2	 = broadcastTickCount1 + (tickCountMinInterval / 2);
    			const DWORD broadcastTickCount3	 = broadcastTickCount1 + (3 * tickCountMinInterval / 2);
    			const DWORD getForwardTickCount	 = 10050;

    			_ICardConfig5 cardConfig;
    			shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(configTickCount);

    			bool multicastActiveChanged;
    			unique_ptr<CProgressiveLevelForward> levelForward(levelManager->GetLevelForward(configTickCount, false, multicastActiveChanged));
    			ASSERT_TRUE(levelForward.get() ==nullptr) << AssertMsg::FormatOnly(L"pLevelForward(%u)", configTickCount);
    			ASSERT_TRUE(levelManager->GetMulticastActive()) << "GetMulticastActive()";
    			ASSERT_FALSE(multicastActiveChanged) << "multicastActiveChanged";

    			{
    				CProgressiveLevelBroadcast levelBroadcast(broadcastTickCount1);
    				levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
    				levelManager->ProcessLevelBroadcast(&levelBroadcast);
    			}

    			levelForward.reset(levelManager->GetLevelForward(getForwardTickCount));
    			ASSERT_TRUE(levelForward.get() !=nullptr)  << "levelForward: 1";
    			ASSERT_EQ((size_t) 1, levelForward->GetNumLevels()) <<  "GetNumLevels(): 1";
    			ASSERT_TRUE(levelManager->GetMulticastActive()) << "GetMulticastActive(): 1";
    			ASSERT_FALSE(multicastActiveChanged) << "multicastActiveChanged: 1";

    			{
    				CProgressiveLevelBroadcast levelBroadcast(broadcastTickCount2);
    				levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
    				levelManager->ProcessLevelBroadcast(&levelBroadcast);
    			}

    			levelForward.reset(levelManager->GetLevelForward(getForwardTickCount + tickCountMinInterval - 1, false, multicastActiveChanged));
    			ASSERT_TRUE(levelForward.get()==nullptr) << "levelForward: 2";
    			ASSERT_TRUE(levelManager->GetMulticastActive()) << "GetMulticastActive(): 2";
    			ASSERT_FALSE(multicastActiveChanged) << "multicastActiveChanged: 2";

    			levelForward.reset(levelManager->GetLevelForward(getForwardTickCount + tickCountMinInterval, false, multicastActiveChanged));
    			ASSERT_TRUE(levelForward.get() !=nullptr) << "levelForward: 3";
    			ASSERT_EQ((size_t) 1, levelForward->GetNumLevels()) << "GetNumLevels(): 3";
    			ASSERT_FALSE(levelManager->GetMulticastActive()) << "GetMulticastActive(): 3";
    			ASSERT_TRUE(multicastActiveChanged) << "multicastActiveChanged: 3";

    			{
    				CProgressiveLevelBroadcast levelBroadcast(broadcastTickCount3);
    				levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
    				levelManager->ProcessLevelBroadcast(&levelBroadcast);
    			}

    			levelForward.reset(levelManager->GetLevelForward(getForwardTickCount + (2 * tickCountMinInterval) - 1, false, multicastActiveChanged));
    			ASSERT_TRUE(levelForward.get()==nullptr) <<  "levelForward: 4";
    			ASSERT_FALSE(levelManager->GetMulticastActive()) << "GetMulticastActive(): 4";
    			ASSERT_FALSE(multicastActiveChanged) << "multicastActiveChanged: 4";

    			levelForward.reset(levelManager->GetLevelForward(getForwardTickCount + (2 * tickCountMinInterval), false, multicastActiveChanged));
    			ASSERT_TRUE(levelForward.get() !=nullptr)  << "levelForward: 5";
    			ASSERT_EQ((size_t) 1, levelForward->GetNumLevels()) << "GetNumLevels(): 5";
    			ASSERT_FALSE(levelManager->GetMulticastActive())  << "GetMulticastActive(): 5";
                ASSERT_FALSE(multicastActiveChanged) << "multicastActiveChanged: 5";
            }
         
            void f_ProgressiveLevelManager_GetLevelForward_PriorityForwardFalse(void)
            {
    			const DWORD configTickCount		 = 5000;
    			const DWORD tickCountMinInterval = CProgressiveLevelForward::MinInterval;
    			const DWORD broadcastTickCount1  = 10000;
    			const DWORD broadcastTickCount2  = broadcastTickCount1 + (tickCountMinInterval / 2);
    			const DWORD getForwardTickCount  = 10050;

    			_ICardConfig5 cardConfig;
    			shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(configTickCount);

    			{
    				CProgressiveLevelBroadcast levelBroadcast(broadcastTickCount1);
    				levelBroadcast.AddLevel(5009, 1234, INVALID_VALUE, INVALID_VALUE, nullptr);
    				levelManager->ProcessLevelBroadcast(&levelBroadcast);
    			}

    			bool multicastActiveChanged;
    			unique_ptr<CProgressiveLevelForward> levelForward(levelManager->GetLevelForward(getForwardTickCount, false, multicastActiveChanged));
    			ASSERT_TRUE(levelForward.get()!=nullptr)  << "levelForward: 1";
    			ASSERT_EQ((size_t) 1, levelForward->GetNumLevels()) << "GetNumLevels(): 1";
    			ASSERT_TRUE(levelManager->GetMulticastActive()) << "GetMulticastActive(): 1";
    			ASSERT_FALSE(multicastActiveChanged) << "multicastActiveChanged: 1";

    			// Second broadcast. Level amount is greater than previous value.
    			{
    				CProgressiveLevelBroadcast levelBroadcast(broadcastTickCount2);
    				levelBroadcast.AddLevel(5009, 1235, INVALID_VALUE, INVALID_VALUE, nullptr);
    				levelManager->ProcessLevelBroadcast(&levelBroadcast);
    			}

    			// Test that a priority forward does not occur.
    			levelForward.reset(levelManager->GetLevelForward(broadcastTickCount2, false, multicastActiveChanged));
    			ASSERT_TRUE(levelForward.get()==nullptr)  << "levelForward: 2";
    			ASSERT_TRUE(levelManager->GetMulticastActive()) << "GetMulticastActive(): 2";
                ASSERT_FALSE(multicastActiveChanged) << "multicastActiveChanged: 2";
            }
    };
		// #pragma region ProgressiveLevelManager

		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_ctor)
		{
            f_ProgressiveLevelManager_ctor( );
		}

		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_cctor_and_assignment)
		{
            f_ProgressiveLevelManager_cctor_and_assignment();
        }

		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_GetLevelIdFromSASLevel)
		{
            f_ProgressiveLevelManager_GetLevelIdFromSASLevel();
        }

		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_multipleSasLevels)
		{
            f_ProgressiveLevelManager_multipleSasLevels();
        }

		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_HasLevelType_Standard_true)
        {
            f_ProgressiveLevelManager_HasLevelType_Standard_true();
        }

		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_HasLevelType_Standard_false)
        {
            f_ProgressiveLevelManager_HasLevelType_Standard_false();
		}

		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_HasLevelType_Bonus_true)
        {
            f_ProgressiveLevelManager_HasLevelType_Bonus_true();
		}

		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_HasLevelType_Bonus_false)
        {
            f_ProgressiveLevelManager_HasLevelType_Bonus_false();
		}

		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_HasLevelType_AccountingOnly_true)
        {
            f_ProgressiveLevelManager_HasLevelType_AccountingOnly_true();
		}

		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_HasLevelType_AccountingOnly_false)
        {
            f_ProgressiveLevelManager_HasLevelType_AccountingOnly_false ();
		}

		// #pragma endregion ProgressiveLevelManager

		// #pragma region ProcessBonusingAvailable

		// This test is slightly bogus since CProgressiveBonusingAvailable
		// will only include Bonus type levelManager->
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_ProcessBonusingAvailable)
        {
            f_ProgressiveLevelManager_ProcessBonusingAvailable();
        }

		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager(const ProgressiveLevelManager &)
		///     CProgressiveLevel::IsEligible()
		/// Test that m_levels IsEligible() results are correct with the ProgressiveLevelManager copy constructor.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_Copy_ProcessBonusingAvailable)
        {
            f_ProgressiveLevelManager_Copy_ProcessBonusingAvailable();
        }

		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager(const ProgressiveLevelManager &)
		///     CProgressiveLevel::IsEligible()
		/// Test that m_levels IsEligible() results are correct after using the ProgressiveLevelManager assignment operator.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_Assign_ProcessBonusingAvailable)
        {
            f_ProgressiveLevelManager_Assign_ProcessBonusingAvailable();
        }

// #pragma endregion ProcessBonusingAvailable

		// #pragma region CProgressiveLevelBroadcast

		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_ProcessLevelBroadcast)
        {
            f_ProgressiveLevelManager_ProcessLevelBroadcast();
		}

		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_ProcessLevelBroadcast_nullptr)
        {
            f_ProgressiveLevelManager_ProcessLevelBroadcast_nullptr();
        }

		// #pragma endregion "ProcessLevelBroadcast"

		// #pragma region ProgressiveLevelManager::ProcessLevelBroadcast_Return
		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::ProcessLevelBroadcast()
		/// Test the return value and m_needPriorityForward from ProcessLevelBroadcast().
		/// Test setting Standard levels where neither has a smaller value than the previous value.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_ProcessLevelBroadcast_StandardLesser00)
        {
            f_ProgressiveLevelManager_ProcessLevelBroadcast_StandardLesser00();
		}

		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::ProcessLevelBroadcast()
		/// Test the return value and m_needPriorityForward from ProcessLevelBroadcast().
		/// Test setting Standard levels where only the second level has a smaller value than the previous value.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_ProcessLevelBroadcast_StandardLesser01)
        {
            f_ProgressiveLevelManager_ProcessLevelBroadcast_StandardLesser01();
           
		}

		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::ProcessLevelBroadcast()
		/// Test the return value and m_needPriorityForward from ProcessLevelBroadcast().
		/// Test setting Standard levels where only the first level has a smaller value than the previous value.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_ProcessLevelBroadcast_StandardLesser10)
        {
            f_ProgressiveLevelManager_ProcessLevelBroadcast_StandardLesser10();
        }

		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::ProcessLevelBroadcast()
		/// Test the return value and m_needPriorityForward from ProcessLevelBroadcast().
		/// Test setting Standard levels where both have smaller values than the previous values.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_ProcessLevelBroadcast_StandardLesser11)
        {
            f_ProgressiveLevelManager_ProcessLevelBroadcast_StandardLesser11();
        }

		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::ProcessLevelBroadcast()
		/// Test the return value and m_needPriorityForward from ProcessLevelBroadcast().
		/// Test setting Standard levels to smaller values than the previous values and then to greater values.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_ProcessLevelBroadcast_StandardLesserGreater)
        {
            f_ProgressiveLevelManager_ProcessLevelBroadcast_StandardLesserGreater();
		}

		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::ProcessLevelBroadcast()
		/// Test the return value and m_needPriorityForward from ProcessLevelBroadcast().
		/// Test setting a Bonus level to a smaller value than the previous value.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_ProcessLevelBroadcast_BonusLesser)
        {
            f_ProgressiveLevelManager_ProcessLevelBroadcast_BonusLesser();
        }

		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::ProcessLevelBroadcast()
		/// Test the return value and m_needPriorityForward from ProcessLevelBroadcast().
		/// Test setting an AccountingOnly level to a smaller value than the previous value.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_ProcessLevelBroadcast_AccountingOnlyLesser)
        {
            f_ProgressiveLevelManager_ProcessLevelBroadcast_AccountingOnlyLesser();
		}
		// #pragma endregion ProgressiveLevelManager::ProcessLevelBroadcast_Return

		// #pragma region ProgressiveLevelManager::SetForwardedTickCount
		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::SetForwardedTickCount()
		/// Test setting multiple values.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_SetForwardedTickCount_Multiple)
        {
            f_ProgressiveLevelManager_SetForwardedTickCount_Multiple( );
		}
		// #pragma endregion ProgressiveLevelManager::SetForwardedTickCount

		// #pragma region ProgressiveLevelManager::OkToSendPriorityForward
		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager(DWORD, const rapidjson::Document &)
		///     ProgressiveLevelManager::OkToSendPriorityForward()
		/// Test default value(s) for JSON constructor.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_OkToSendPriorityForward_DefaultJson)
        {
            f_ProgressiveLevelManager_OkToSendPriorityForward_DefaultJson();
        }
  
		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::OkToSendPriorityForward()
		/// Test default value(s) after setting a Standard level to a smaller value than the previous value.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_OkToSendPriorityForward_DefaultAndLesser)
        {
             f_ProgressiveLevelManager_OkToSendPriorityForward_DefaultAndLesser( ); 
        }

		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::SetForwardedTickCount()
		///     ProgressiveLevelManager::OkToSendPriorityForward()
		/// Test after setting multiple values.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_OkToSendPriorityForward_Multiple)
        { 
            f_ProgresiveLevelManager_OkToSendPriorityForward_Multiple();
		}
		// #pragma endregion ProgressiveLevelManager::OkToSendPriorityForward

		// #pragma region ProgressiveLevelManager::GetLevelForward
		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::GetLevelForward()
		/// Test default value(s).
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_GetLevelForward_Initial)
        { 
            f_ProgressiveLevelManager_GetLevelForward_Initial();
        }

		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::GetLevelForward()
		/// Test strobe interval threshold.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_GetLevelForward_StrobeInterval)
        { 
            f_ProgressiveLevelManager_GetLevelForward_StrobeInterval();
         }
          
        /// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::GetLevelForward()
		/// Test maximum interval timeout not exceeded.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_GetLevelForward_MaxInterval)
        { 
            f_ProgressiveLevelManager_GetLevelForward_MaxInterval();
		}

		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::GetLevelForward()
		/// Test maximum interval timeout exceeded.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_GetLevelForward_MaxIntervalExceeded)
        { 
            f_ProgressiveLevelManager_GetLevelForward_MaxIntervalExceeded();
        }


		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::GetLevelForward()
		/// Test minimum interval threshold.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_GetLevelForward_MinInterval)
        { 
            f_ProgressiveLevelManager_GetLevelForward_MinInterval ();
        }

		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::GetLevelForward()
		/// Test for priority forward not occurring when level amount does not go backwards.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_GetLevelForward_PriorityForwardFalse)
        { 
            f_ProgressiveLevelManager_GetLevelForward_PriorityForwardFalse();
        }

		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::GetLevelForward()
		/// Test for priority forward occurring when level amount does go backwards.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_GetLevelForward_PriorityForwardTrue)
        { 
            f_ProgressiveLevelManager_GetLevelForward_PriorityForwardTrue();
        } 

		/// <summary>
		/// Functional test for:
		///     ProgressiveLevelManager::ProgressiveLevelManager()
		///     ProgressiveLevelManager::GetLevelForward()
		/// Test for that priority forwards stop occurring when a limit is reached.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests, ProgressiveLevelManager_GetLevelForward_PriorityForwardLimit)
        { 
            f_ProgressiveLevelManager_GetLevelForward_PriorityForwardLimit(); 
        }
		// #pragma endregion ProgressiveLevelManager::GetLevelForward
}
