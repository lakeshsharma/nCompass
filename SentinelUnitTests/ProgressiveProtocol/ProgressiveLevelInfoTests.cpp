#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveLevelInfo.h"
#include "ProgressiveLevelTestHelpers.h"
#include <Util/Convert.h>


#include "gtest/gtest.h"

using namespace std;

string levelId627("{\"Id\":627,\"Version\":250,\"Type\":\"Standard\",\"Channel\":{\"Destination\":\"@239.1.1.1:61613\",\"MulticastDestination\":\"stomp://239.1.1.1:61613\"},\"Info\":\"\",\"Name\":\"Name\",\"MediaMinSeconds\":0,\"MediaMaxSeconds\":0}");

namespace SentinelNativeUnitTests
{   

class ProgressiveLevelInfoTests : public ::testing::Test
    {
    public :
     void f_ProgressiveLevelInfo_ctor( void )
     {
			rapidjson::Document levelId627Json;
			if (levelId627Json.Parse(levelId627.c_str()).HasParseError())
				throw JsonException(levelId627Json.GetParseError(), nullptr, "Parse");
			ProgressiveLevelInfo level(627, ProgressiveLevelType::Bonus, levelId627Json);
			ASSERT_EQ((size_t) 627, (size_t) level.m_id) << "level.m_id";
			ASSERT_EQ((int) ProgressiveLevelType::Bonus, (int) level.m_type) << "level.m_type";
			ASSERT_STREQ("Name", level.m_name.c_str()) << "level.m_name";
			ASSERT_STREQ("stomp://239.1.1.1:61613", level.m_url.c_str()) << "level.m_url";
			ASSERT_EQ((size_t) 0, level.m_queryCardsGames.size()) << "level.m_queryCardsGames.size";
			ASSERT_EQ((size_t) 0, level.m_sasLevels.size()) << "level.m_sasLevels.size";                   
     }

     void ProgressiveLevelInfo_AddQueryCardsGame(void)
     {
		 rapidjson::Document levelId627Json;
		 if (levelId627Json.Parse(levelId627.c_str()).HasParseError())
			 throw JsonException(levelId627Json.GetParseError(), nullptr, "Parse");
		 ProgressiveLevelInfo level(627, ProgressiveLevelType::Bonus, levelId627Json);

         level.AddQueryCardsGame(5);
         level.AddQueryCardsGame(1000);
         level.AddQueryCardsGame(54321);

         ASSERT_EQ((size_t) 3, level.m_queryCardsGames.size()) << "level.m_queryCardsGames.size";
         ASSERT_EQ(5, (int) level.m_queryCardsGames[0]) << "level.m_queryCardsGames[0]";
         ASSERT_EQ(1000, (int) level.m_queryCardsGames[1]) << "level.m_queryCardsGames[1]";
         ASSERT_EQ(54321, (int) level.m_queryCardsGames[2]) << "level.m_queryCardsGames[2]";
     }

     void f_ProgressiveLevelInfo_AppendQueryCardsSet( void )
     {
		 rapidjson::Document levelId627Json;
		 if (levelId627Json.Parse(levelId627.c_str()).HasParseError())
			 throw JsonException(levelId627Json.GetParseError(), nullptr, "Parse");
		 ProgressiveLevelInfo level(627, ProgressiveLevelType::Bonus, levelId627Json);

         level.AddQueryCardsGame(5);
         level.AddQueryCardsGame(1000);
         level.AddQueryCardsGame(54321);

         set<DWORD> queryCardsSet;
         queryCardsSet.insert(1000);

         level.AppendQueryCardsSet(queryCardsSet);

         ASSERT_EQ((size_t) 3, queryCardsSet.size()) << "queryCardsSet.size";

         ASSERT_TRUE(queryCardsSet.find(5) != queryCardsSet.end()) << "queryCardsSet.find(5)";
         ASSERT_TRUE(queryCardsSet.find(1000) != queryCardsSet.end()) << "queryCardsSet.find(1000)";
         ASSERT_TRUE(queryCardsSet.find(54321) != queryCardsSet.end()) << "queryCardsSet.find(54321)";
     }

     void f_ProgressiveLevelInfo_AddSasLevel( void )
     {
		 rapidjson::Document levelId627Json;
		 if (levelId627Json.Parse(levelId627.c_str()).HasParseError())
			 throw JsonException(levelId627Json.GetParseError(), nullptr, "Parse");
		 ProgressiveLevelInfo level(627, ProgressiveLevelType::Bonus, levelId627Json);

         level.AddSasLevel(33);
         level.AddSasLevel(3);
         level.AddSasLevel(1);
         level.AddSasLevel(0);

         ASSERT_EQ((size_t) 3, level.m_sasLevels.size()) << "level.m_sasLevels.size";
         ASSERT_EQ(3, (int) level.m_sasLevels[0]) << "level.m_sasLevels[0]";
         ASSERT_EQ(1, (int) level.m_sasLevels[1]) << "level.m_sasLevels[1]";
         ASSERT_EQ(0, (int) level.m_sasLevels[2]) << "level.m_sasLevels[2]";
     }

     void f_ProgressiveLevelInfo_SortSasLevels( void )
     {
		 rapidjson::Document levelId627Json;
		 if (levelId627Json.Parse(levelId627.c_str()).HasParseError())
			 throw JsonException(levelId627Json.GetParseError(), nullptr, "Parse");
		 ProgressiveLevelInfo level(627, ProgressiveLevelType::Bonus, levelId627Json);

         level.AddSasLevel(3);
         level.AddSasLevel(1);
         level.AddSasLevel(0);

         level.SortSASLevels();

         ASSERT_EQ(0, (int) level.m_sasLevels[0]) << "level.m_sasLevels[0]";
         ASSERT_EQ(1, (int) level.m_sasLevels[1]) << "level.m_sasLevels[1]";
         ASSERT_EQ(3, (int) level.m_sasLevels[2]) << "level.m_sasLevels[2]";
     }
    };

TEST_F(ProgressiveLevelInfoTests, ProgressiveLevelInfo_ctor)
		{
            f_ProgressiveLevelInfo_ctor();
		}

		TEST_F(ProgressiveLevelInfoTests, ProgressiveLevelInfo_AddQueryCardsGame)
		{
            ProgressiveLevelInfo_AddQueryCardsGame();
        }

		TEST_F(ProgressiveLevelInfoTests, ProgressiveLevelInfo_AppendQueryCardsSet)
		{
            f_ProgressiveLevelInfo_AppendQueryCardsSet();
            		}

		TEST_F(ProgressiveLevelInfoTests, ProgressiveLevelInfo_AddSasLevel)
		{
            f_ProgressiveLevelInfo_AddSasLevel();
        }		

		TEST_F(ProgressiveLevelInfoTests, ProgressiveLevelInfo_SortSasLevels)
		{
            f_ProgressiveLevelInfo_SortSasLevels();
        }
}

