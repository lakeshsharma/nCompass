#include "stdafx.h"
#include "AssertMsg.h"
#include "ProgressiveLevelTestHelpers.h"
#include "ProgressiveProtocol/ProgressiveConnection.h"
#include "ProgressiveProtocol/ProgressiveMessageFactory.h"


#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
    using namespace ProgressiveLevelTestHelpers;

    TEST(ProgressiveLevelTestHelper, _ICardConfig5_ctor1)
    {
        _ICardConfig5 cardConfig5(1);
        ASSERT_EQ((size_t) 2, cardConfig5.Games.size());
        ASSERT_EQ((size_t) 4, cardConfig5.CardLevels.size());
    }

    TEST(ProgressiveLevelTestHelper, _ICardConfig5_ctor2)
    {
        _ICardConfig5 cardConfig5(2);
        ASSERT_EQ((size_t) 2, cardConfig5.Games.size());
        ASSERT_EQ((size_t) 4, cardConfig5.CardLevels.size());
    }

    TEST(ProgressiveLevelTestHelper, _ICardConfig5_CreateJson1)
    {
        _ICardConfig5 cardConfig5(1);
        string jsonStr = cardConfig5.CreateJson();
        //OutputDebugStringA("### _ICardConfig5_CreateJson1 ###\n");
        //OutputDebugStringA(jsonStr.c_str());
        //OutputDebugStringA("\n### _ICardConfig5_CreateJson1 ###\n");

        rapidjson::Document jsonDoc;
        bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
        ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));
    }

    TEST(ProgressiveLevelTestHelper, _ICardConfig5_CreateJson2)
    {
        _ICardConfig5 cardConfig5(2);
        string jsonStr = cardConfig5.CreateJson();
        //OutputDebugStringA("### _ICardConfig5_CreateJson2 ###\n");
        //OutputDebugStringA(jsonStr.c_str());
        //OutputDebugStringA("\n### _ICardConfig5_CreateJson2 ###\n");

        rapidjson::Document jsonDoc;
        bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
        ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));
    }

    TEST(ProgressiveLevelTestHelper, _ICardConfig5_CreateJsonDoc)
    {
        _ICardConfig5 cardConfig5;
        rapidjson::Document jsonDoc;
        cardConfig5.CreateJsonDoc(jsonDoc);


        int progressiveGroupId = JsonGetInt(jsonDoc, pKeyProgressiveGroupId);
        ASSERT_EQ(cardConfig5.ProgressiveGroupId, progressiveGroupId) << "progressiveGroupId";
    }
};

namespace SentinelNativeUnitTests
{

namespace ProgressiveLevelTestHelpers
{
    _ICardConfig5::_ICardConfig5(int dataSet)
    {
        switch (dataSet)
        {
            case 1:
                Create1();
                break;

            case 2:
                Create2();
                break;
        }
    };

    _ICardConfig5::_ICardConfig5(int ProgressiveGroupId, bool useAftOnTransfers)
    {
        ProgressiveGroupId = ProgressiveGroupId;
        SetUseAftOnTransfers(useAftOnTransfers);
    }

    void _ICardConfig5::SetUseAftOnTransfers(bool useAftOnTransfers)
    {
        UseAFTLockOnTransfers = useAftOnTransfers;
    }

    void _ICardConfig5::Verify(ProgressiveLevelsInfo& levels)
    {
        const uint64_t MinutesTolerance = 5;

        ASSERT_TRUE((CUtilities::GetTickCount() - levels.m_receivedTickCount) < (DWORD)(MinutesTolerance * MillisecondsPerSecond));
        //ASSERT_TRUE(FileTime::Elapsed(levels.m_receivedTime) < (MinutesTolerance * Intervals::PerMinute));

        ASSERT_EQ(ProgressiveGroupId, levels.m_progressiveGroupId) << "m_progressiveGroupId";

        bool useAFTLockOnTransfers = UseAFTLockOnTransfers ? UseAFTLockOnTransfers.val : false;
        ASSERT_EQ(useAFTLockOnTransfers, levels.m_useAftLockOnTransfers) << "m_useAftLockOnTransfers()";

        map<LONG64, set<BYTE>>		gameLevelMap;
        map<LONG64, _ICardLevel*>	cardLevelMap;
        set<string>					expectedUrlSet;

        vector<string>				urls = levels.GetUrls();
        set<DWORD>					queryPokerCardsSet = levels.GetQueryPokerCardsSet();

        bool expectedQueryPokerCards = false;

        for (size_t i = 0; i < Games.size(); i++)
        {
            bool expectedQueryCards = Games[i]->QueryCards;
            if (expectedQueryCards)
                expectedQueryPokerCards = true;

            bool queryCards = queryPokerCardsSet.find(Games[i]->GameNumber) != queryPokerCardsSet.end();
            ASSERT_EQ(expectedQueryCards, queryCards) << AssertMsg::FormatOnly(L"Games[%d] pQueryCards", i);

            for (auto it = Games[i]->Levels.cbegin(); it != Games[i]->Levels.cend(); ++it)
            {
                ASSERT_TRUE((*it)->LevelSequence >= 0 && (*it)->LevelSequence <= 32) <<
                    AssertMsg::FormatOnly(L"Games[%d]->LevelSequence is invalid (%d)", i, (*it)->LevelSequence);

                auto gameLevelMapIt = gameLevelMap.find((*it)->LevelId);
                if (gameLevelMapIt != gameLevelMap.end())
                {
                    gameLevelMapIt->second.insert((BYTE) (*it)->LevelSequence);
                }
                else
                {
                    set<BYTE> levelSequences;
                    levelSequences.insert((BYTE) (*it)->LevelSequence);
                    gameLevelMap[(*it)->LevelId] = levelSequences;
                }
            }
        }

        ASSERT_EQ(expectedQueryPokerCards, levels.QueryPokerCards()) << "levels.QueryPokerCards()";

        for (auto it = CardLevels.cbegin(); it != CardLevels.cend(); ++it)
        {
            cardLevelMap.insert(make_pair((*it)->Id, (*it).get()));
            expectedUrlSet.insert((*it)->Channel.MulticastDestination);
        }

        ASSERT_EQ(expectedUrlSet.size(), urls.size()) << "urls.size()";

        for (size_t i = 0; i < urls.size(); i++)
        {
            ASSERT_TRUE(expectedUrlSet.find(urls.at(i)) != expectedUrlSet.end()) << AssertMsg::FormatOnly(L"urls[%u]: %S", i, urls.at(i).c_str());
        }

        ASSERT_EQ((size_t) CardLevels.size(), levels.m_levels.size()) << "levels.m_levels.size()";

        for (auto& level : levels.m_levels)
        {
            ASSERT_TRUE(
                    ProgressiveLevelType::Standard == level.second->m_type ||
                    ProgressiveLevelType::Bonus == level.second->m_type ||
                    ProgressiveLevelType::AccountingOnly == level.second->m_type) <<
                AssertMsg::FormatOnly(L"m_levels[%u]->m_type is invalid", level.second->m_id);

            auto gameLevelIt = gameLevelMap.find(level.second->m_id);
            ASSERT_TRUE(gameLevelIt != gameLevelMap.end()) << AssertMsg::FormatOnly(L"m_levels[%lld]->m_id is not in _paltronics1__ICardConfiguration5::Games", level.second->m_id);

            auto cardLevelIt = cardLevelMap.find(level.second->m_id);
            ASSERT_TRUE(cardLevelIt != cardLevelMap.end()) << AssertMsg::FormatOnly(L"m_levels[%lld]->m_id is not in _paltronics1__ICardConfiguration5::Levels", level.second->m_id);

            const _ICardLevel* cardLevel = cardLevelIt->second;

            ASSERT_EQ((int) cardLevel->Type, (int) level.second->m_type) << AssertMsg::FormatOnly(L"m_levels[%lld]->m_type", level.second->m_id);
            ASSERT_STREQ(cardLevel->Name ? cardLevel->Name : "", level.second->m_name.c_str()) << AssertMsg::FormatOnly(L"m_levels[%lld]->m_name", level.second->m_id);
            ASSERT_STREQ(cardLevel->Channel.MulticastDestination, level.second->m_url.c_str()) << AssertMsg::FormatOnly(L"m_levels[%lld]->m_url", level.second->m_id);

            const auto levelSequences = gameLevelIt->second;

            if (level.second->m_type == ProgressiveLevelType::Bonus)
            {
                ASSERT_EQ((size_t) 0, level.second->m_sasLevels.size()) << AssertMsg::FormatOnly(L"m_levels[%lld]->m_sasLevels.size()", level.second->m_id);
            }
            else
            {
                ASSERT_EQ(levelSequences.size(), level.second->m_sasLevels.size()) << AssertMsg::FormatOnly(L"m_levels[%lld]->m_sasLevels.size()", level.second->m_id);

                for (size_t k = 0; k < level.second->m_sasLevels.size(); k++)
                {
                    ASSERT_TRUE(levelSequences.find(level.second->m_sasLevels[k]) != levelSequences.end()) <<
                        AssertMsg::FormatOnly(L"m_levels[%u]->m_sasLevels[%u]", level.second->m_id, k);
                }
            }
        }
    }

    void _ICardConfig5::Verify(ProgressiveLevelManager& levels)
    {
        const uint64_t MinutesTolerance = 5;

        ASSERT_TRUE((CUtilities::GetTickCount() - levels.m_updatedTickCount) < (DWORD) (MinutesTolerance * MillisecondsPerSecond));
        //ASSERT_TRUE(FileTime::Elapsed(levels.m_receivedTime) < (MinutesTolerance * FileTime::Intervals::PerMinute));

        ASSERT_EQ(ProgressiveGroupId, levels.m_progressiveGroupId) << "m_progressiveGroupId";

        bool useAFTLockOnTransfers = UseAFTLockOnTransfers ? UseAFTLockOnTransfers.val : false;
        ASSERT_EQ(useAFTLockOnTransfers, levels.m_useAftLockOnTransfers) << "m_useAftLockOnTransfers()";

        map<LONG64, set<BYTE>>		gameLevelMap;
        map<LONG64, _ICardLevel*>	cardLevelMap;

        bool expectedQueryPokerCards = false;

        for (size_t i = 0; i < Games.size(); i++)
        {
            bool expectedQueryCards = Games[i]->QueryCards;
            if (expectedQueryCards)
                expectedQueryPokerCards = true;

            for (const auto& gameLevel : Games[i]->Levels)
            {
                ASSERT_TRUE(gameLevel->LevelSequence >= 0 && gameLevel->LevelSequence <= 32) << 
                    AssertMsg::FormatOnly(L"Games[%d]->LevelSequence is invalid (%d)", i, gameLevel->LevelSequence);

                auto gameLevelMapIt = gameLevelMap.find(gameLevel->LevelId);
                if (gameLevelMapIt != gameLevelMap.end())
                {
                    gameLevelMapIt->second.insert((BYTE) gameLevel->LevelSequence);
                }
                else
                {
                    set<BYTE> levelSequences;
                    levelSequences.insert((BYTE) gameLevel->LevelSequence);
                    gameLevelMap[gameLevel->LevelId] = levelSequences;
                }
            }
        }

        ASSERT_EQ(expectedQueryPokerCards, levels.QueryPokerCards()) << "levels.QueryPokerCards()";

        for (const auto& cardLevel : CardLevels)
        {
            cardLevelMap.insert(make_pair(cardLevel->Id, cardLevel.get()));
        }

        ASSERT_EQ((size_t) CardLevels.size(), levels.m_levels.size()) << "levels.m_levels.size()";

        for (const auto& level : levels.m_levels)
        {
            ASSERT_TRUE(
                    ProgressiveLevelType::Standard == level.second->m_type ||
                    ProgressiveLevelType::Bonus == level.second->m_type ||
                    ProgressiveLevelType::AccountingOnly == level.second->m_type) << 
                AssertMsg::FormatOnly(L"m_levels[%u]->m_type is invalid", level.second->m_id);

            auto gameLevelIt = gameLevelMap.find(level.second->m_id);
            ASSERT_TRUE(gameLevelIt != gameLevelMap.end()) << AssertMsg::FormatOnly(L"m_levels[%lld]->m_id is not in _paltronics1__ICardConfiguration5::Games", level.second->m_id);

            auto cardLevelIt = cardLevelMap.find(level.second->m_id);
            ASSERT_TRUE(cardLevelIt != cardLevelMap.end()) << AssertMsg::FormatOnly(L"m_levels[%lld]->m_id is not in _paltronics1__ICardConfiguration5::Levels", level.second->m_id);

            const _ICardLevel* cardLevel = cardLevelIt->second;

            ASSERT_EQ((int) cardLevel->Type, (int) level.second->m_type) << AssertMsg::FormatOnly(L"m_levels[%lld]->m_type", level.second->m_id);
            ASSERT_EQ(string(cardLevel->Name ? cardLevel->Name : ""), level.second->m_name) << AssertMsg::FormatOnly(L"m_levels[%lld]->m_name", level.second->m_id);
            ASSERT_EQ(string(cardLevel->Channel.MulticastDestination), level.second->m_url) << AssertMsg::FormatOnly(L"m_levels[%lld]->m_url", level.second->m_id);

            const auto levelSequences = gameLevelIt->second;

            if (level.second->m_type == ProgressiveLevelType::Bonus)
            {
                ASSERT_EQ((size_t) 0, level.second->m_sasLevels.size()) << AssertMsg::FormatOnly(L"m_levels[%lld]->m_sasLevels.size()", level.second->m_id);
            }
            else
            {
                ASSERT_EQ(levelSequences.size(), level.second->m_sasLevels.size()) << AssertMsg::FormatOnly(L"m_levels[%lld]->m_sasLevels.size()", level.second->m_id);

                for (size_t k = 0; k < level.second->m_sasLevels.size(); k++)
                {
                    ASSERT_TRUE(levelSequences.find(level.second->m_sasLevels[k]) != levelSequences.end()) <<
                        AssertMsg::FormatOnly(L"m_levels[%u]->m_sasLevels[%u]", level.second->m_id, k);
                }
            }
        }
    }

    void _ICardConfig5::Create1()
    {
        SourceDeviceId				= 1;
        SourceDeviceConfigVersion	= 0;
        SourceAsset					= "";
        Timestamp					= 1431663528254;
        MessageId					= "discovery1431663528254";
        DeviceId					= 8;
        DeviceConfigVersion			= 987654321;
        DeviceAddress				= "@device.ICARD.0060E053E4F0";
        OneLinkSystemId				= "ceae0730-bbef-46ec-9cf1-292cab5fd4ca";
        EventChannel.Destination	= "@239.3.0.1:61613";
        EventChannel.MulticastDestination = "stomp://239.3.0.1:61613";
        AssetNumber					= "55554";
        BankName					= "Default Bank";
        TimeZone					= "America/Los_Angeles";

        unique_ptr<_ICardGame5>		game;
        unique_ptr<_ICardGameLevel> gameLevel;

        game.reset(new _ICardGame5);
        game->GameNumber		= 1;
        game->Denomination		= 1.00;
        game->MaxBetCredits		= 5;
        game->TheoPercentage	= 91.23;
        game->GameName			= "Buffalo";
        game->Paytable			= "098765";
        game->QueryCards		= false;

        gameLevel.reset(new _ICardGameLevel);
        gameLevel->LevelId		 = 5;
        gameLevel->LevelSequence = 0;
        game->Levels.push_back(move(gameLevel));

        gameLevel.reset(new _ICardGameLevel);
        gameLevel->LevelId		 = 5009;
        gameLevel->LevelSequence = 1;
        game->Levels.push_back(move(gameLevel));

        Games.push_back(move(game));

        game.reset(new _ICardGame5);
        game->GameNumber		= 2;
        game->Denomination		= 0.50;
        game->MaxBetCredits		= 6;
        game->TheoPercentage	= 12.34;
        game->GameName			= "Joker Poker";
        game->Paytable			= "56789";
        game->QueryCards		= true;

        gameLevel.reset(new _ICardGameLevel);
        gameLevel->LevelId		 = 61234;
        gameLevel->LevelSequence = 31;
        game->Levels.push_back(move(gameLevel));

        gameLevel.reset(new _ICardGameLevel);
        gameLevel->LevelId		 = 5010;
        gameLevel->LevelSequence = 15;
        game->Levels.push_back(move(gameLevel));

        Games.push_back(move(game));

        unique_ptr<_ICardLevel>	cardLevel;

        cardLevel.reset(new _ICardLevel);
        cardLevel->Id					= 5;
        cardLevel->Version				= 1;
        cardLevel->Type					= ProgressiveLevelType::Bonus;
        cardLevel->pCardLevelType		= ProgressiveLevelType::pBonus;
        cardLevel->Channel.Destination	= "@239.1.1.10:61613"; 
        cardLevel->Channel.MulticastDestination = "stomp://239.1.1.10:61613"; 
        cardLevel->Name					= "Floorwide Value Mystery 001";
        cardLevel->MediaMinSeconds		= 0;
        cardLevel->MediaMaxSeconds		= 0;
        CardLevels.push_back(move(cardLevel));

        cardLevel.reset(new _ICardLevel);
        cardLevel->Id					= 5009;
        cardLevel->Version				= 2;
        cardLevel->Type					= ProgressiveLevelType::Standard;
        cardLevel->pCardLevelType		= ProgressiveLevelType::pStandard;
        cardLevel->Channel.Destination	= "@239.1.1.10:61613"; 
        cardLevel->Channel.MulticastDestination = "stomp://239.1.1.1:61613"; 
        cardLevel->Name					= nullptr;
        cardLevel->MediaMinSeconds		= 2;
        cardLevel->MediaMaxSeconds		= 6;
        CardLevels.push_back(move(cardLevel));

        cardLevel.reset(new _ICardLevel);
        cardLevel->Id					= 61234;
        cardLevel->Version				= 2;
        cardLevel->Type					= ProgressiveLevelType::Standard;
        cardLevel->pCardLevelType		= ProgressiveLevelType::pStandard;
        cardLevel->Channel.Destination	= "@239.1.1.10:61613"; 
        cardLevel->Channel.MulticastDestination = "stomp://239.1.1.1:61613"; 
        cardLevel->Name					= "z";
        cardLevel->MediaMinSeconds		= 1;
        cardLevel->MediaMaxSeconds		= 5;
        CardLevels.push_back(move(cardLevel));

        cardLevel.reset(new _ICardLevel);
        cardLevel->Id					= 5010;
        cardLevel->Version				= 2;
        cardLevel->Type					= ProgressiveLevelType::AccountingOnly;
        cardLevel->pCardLevelType		= ProgressiveLevelType::pAccountingOnly;
        cardLevel->Channel.Destination	= "@239.1.1.10:61613"; 
        cardLevel->Channel.MulticastDestination = "stomp://239.1.1.2:61613"; 
        cardLevel->Name					= "x";
        cardLevel->MediaMinSeconds		= 4;
        cardLevel->MediaMaxSeconds		= 8;
        CardLevels.push_back(move(cardLevel));

        ProtocolType		 = pValSAS;
        ProgressiveGroupId	 = 1;
        PaymentMethod		 = BonusPaymentMethod::pSasAFTBonus;
        ContentHttpUrl		 = "<TEMP>/";

        Playlist.reset(new _Playlist);

        Playlist->Name			= "SLOT_PLAYLIST";
        Playlist->Shuffle		= false;

        Playlist->Items.reset(new vector<unique_ptr<_PlaylistItem>>);

        unique_ptr<_PlaylistItem> playlistItem;

        playlistItem.reset(new _PlaylistItem);
        playlistItem->Content.FileName	= "File1.txt";
        playlistItem->Content.MD5		= "MD51";
        playlistItem->Enabled			= true;
        playlistItem->BeginDate			= 1;
        playlistItem->EndDate			= 6;
        playlistItem->BeginTime			= "0000";
        playlistItem->EndTime			= "1259";
        playlistItem->ActiveSunday		= true;
        playlistItem->ActiveMonday		= false;
        playlistItem->ActiveTuesday		= true;
        playlistItem->ActiveWednesday	= false;
        playlistItem->ActiveThursday	= true;
        playlistItem->ActiveFriday		= true;
        playlistItem->ActiveSaturday	= true;
        playlistItem->SequenceId		= 15;
        playlistItem->Duration			= 16;
        playlistItem->AudioAdjustment	= "0.97";
        playlistItem->Mute				= true;
        playlistItem->Image				= true;

        Playlist->Items->push_back(move(playlistItem));

        playlistItem.reset(new _PlaylistItem);
        playlistItem->Content.FileName	= "File2.txt";
        playlistItem->Content.MD5		= "MD52";
        playlistItem->Enabled			= false;
        playlistItem->BeginDate			= 0;
        playlistItem->EndDate			= 0xffffffff;
        playlistItem->BeginTime			= "0000";
        playlistItem->EndTime			= "2359";
        playlistItem->ActiveSunday		= true;
        playlistItem->ActiveMonday		= true;
        playlistItem->ActiveTuesday		= true;
        playlistItem->ActiveWednesday	= true;
        playlistItem->ActiveThursday	= true;
        playlistItem->ActiveFriday		= true;
        playlistItem->ActiveSaturday	= true;
        playlistItem->SequenceId		= 25;
        playlistItem->Duration			= 26;
        playlistItem->AudioAdjustment	= "0.79";
        playlistItem->Mute				= true;
        playlistItem->Image				= true;

        Playlist->Items->push_back(move(playlistItem));

        playlistItem.reset(new _PlaylistItem);
        playlistItem->Content.FileName	= "File3.txt";
        playlistItem->Content.MD5		= "MD53";
        playlistItem->Enabled			= true;
        playlistItem->BeginDate			= 3;
        playlistItem->EndDate			= 4;
        playlistItem->BeginTime			= "1200";
        playlistItem->EndTime			= "2359";
        playlistItem->ActiveSunday		= false;
        playlistItem->ActiveMonday		= true;
        playlistItem->ActiveTuesday		= false;
        playlistItem->ActiveWednesday	= true;
        playlistItem->ActiveThursday	= false;
        playlistItem->ActiveFriday		= true;
        playlistItem->ActiveSaturday	= false;
        playlistItem->SequenceId		= 25;
        playlistItem->Duration			= 26;
        playlistItem->AudioAdjustment	= "0.79";
        playlistItem->Mute				= false;
        playlistItem->Image				= false;

        Playlist->Items->push_back(move(playlistItem));
    }

    void _ICardConfig5::Create2()
    {
        SourceDeviceId				= 1;
        SourceDeviceConfigVersion	= 0;
        SourceAsset					= "";
        Timestamp					= 1431675639365;
        MessageId					= "discovery1431663528254";
        DeviceId					= 8;
        DeviceConfigVersion			= 987654321;
        DeviceAddress				= "@device.ICARD.0060E053E4F0";
        OneLinkSystemId				= "ceae0730-bbef-46ec-9cf1-292cab5fd4ca";
        EventChannel.Destination	= "@239.3.0.1:61613";
        EventChannel.MulticastDestination = "stomp://239.3.0.1:61613";
        AssetNumber					= "55554";
        BankName					= "Default Bank";
        TimeZone					= "America/Los_Angeles";

        unique_ptr<_ICardGame5>		game;
        unique_ptr<_ICardGameLevel> gameLevel;

        game.reset(new _ICardGame5);
        game->GameNumber		= 1;
        game->Denomination		= 1.11;
        game->MaxBetCredits		= 6;
        game->TheoPercentage	= 19.23;
        game->GameName			= "Cow";
        game->Paytable			= "098765";
        game->QueryCards		= false;

        gameLevel.reset(new _ICardGameLevel);
        gameLevel->LevelId		 = 6;
        gameLevel->LevelSequence = 31;
        game->Levels.push_back(move(gameLevel));

        gameLevel.reset(new _ICardGameLevel);
        gameLevel->LevelId		 = 5010;
        gameLevel->LevelSequence = 8;
        game->Levels.push_back(move(gameLevel));

        Games.push_back(move(game));

        game.reset(new _ICardGame5);
        game->GameNumber		= 4;
        game->Denomination		= 2.50;
        game->MaxBetCredits		= 9;
        game->TheoPercentage	= 43.21;
        game->GameName			= "Poker Joker";
        game->Paytable			= "187649";
        game->QueryCards		= true;

        gameLevel.reset(new _ICardGameLevel);
        gameLevel->LevelId		 = 61234;
        gameLevel->LevelSequence = 30;
        game->Levels.push_back(move(gameLevel));

        gameLevel.reset(new _ICardGameLevel);
        gameLevel->LevelId		 = 5009;
        gameLevel->LevelSequence = 0;
        game->Levels.push_back(move(gameLevel));

        Games.push_back(move(game));

        unique_ptr<_ICardLevel>	cardLevel;

        cardLevel.reset(new _ICardLevel);
        cardLevel->Id					= 6;
        cardLevel->Version				= 2;
        cardLevel->Type					= ProgressiveLevelType::Standard;
        cardLevel->pCardLevelType		= ProgressiveLevelType::pStandard;
        cardLevel->Channel.Destination	= "@239.1.1.9:61613"; 
        cardLevel->Channel.MulticastDestination = "stomp://239.1.1.9:61613"; 
        cardLevel->Name					= "Floorwide Value Mystery 002";
        cardLevel->MediaMinSeconds		= 0;
        cardLevel->MediaMaxSeconds		= 0;
        CardLevels.push_back(move(cardLevel));

        cardLevel.reset(new _ICardLevel);
        cardLevel->Id					= 5009;
        cardLevel->Version				= 2;
        cardLevel->Type					= ProgressiveLevelType::Bonus;
        cardLevel->pCardLevelType		= ProgressiveLevelType::pBonus;
        cardLevel->Channel.Destination	= "@239.1.1.9:61613"; 
        cardLevel->Channel.MulticastDestination = "stomp://239.1.1.1:61613"; 
        cardLevel->Name					= nullptr;
        cardLevel->MediaMinSeconds		= 12;
        cardLevel->MediaMaxSeconds		= 16;
        CardLevels.push_back(move(cardLevel));

        cardLevel.reset(new _ICardLevel);
        cardLevel->Id					= 61234;
        cardLevel->Version				= 2;
        cardLevel->Type					= ProgressiveLevelType::Standard;
        cardLevel->pCardLevelType		= ProgressiveLevelType::pStandard;
        cardLevel->Channel.Destination	= "@239.1.1.9:61613"; 
        cardLevel->Channel.MulticastDestination = "stomp://239.1.1.1:61613"; 
        cardLevel->Name					= "zz";
        cardLevel->MediaMinSeconds		= 11;
        cardLevel->MediaMaxSeconds		= 15;
        CardLevels.push_back(move(cardLevel));

        cardLevel.reset(new _ICardLevel);
        cardLevel->Id					= 5010;
        cardLevel->Version				= 2;
        cardLevel->Type					= ProgressiveLevelType::AccountingOnly;
        cardLevel->pCardLevelType		= ProgressiveLevelType::pAccountingOnly;
        cardLevel->Channel.Destination	= "@239.1.1.10:61613"; 
        cardLevel->Channel.MulticastDestination = "stomp://239.1.1.2:61613"; 
        cardLevel->Name					= "x";
        cardLevel->MediaMinSeconds		= 14;
        cardLevel->MediaMaxSeconds		= 18;
        CardLevels.push_back(move(cardLevel));

        ProtocolType		 = pValSAS;
        ProgressiveGroupId	 = 2;
        PaymentMethod		 = "SasAFTBonus";
        MeterBankLevel		 = false;
        MeterMachineOnly	 = false;
        ContentHttpUrl		 = "<TEMP>/";

        Playlist.reset(new _Playlist);

        Playlist->Name			= "SLOT_PLAYLIST";
        Playlist->Shuffle		= true;

        Playlist->Items.reset(new vector<unique_ptr<_PlaylistItem>>);

        unique_ptr<_PlaylistItem> playlistItem;

        playlistItem.reset(new _PlaylistItem);
        playlistItem->Content.FileName	= "File3.txt";
        playlistItem->Content.MD5		= "MD53";
        playlistItem->Enabled			= true;
        playlistItem->BeginDate			= 3;
        playlistItem->EndDate			= 4;
        playlistItem->BeginTime			= "1200";
        playlistItem->EndTime			= "2359";
        playlistItem->ActiveSunday		= false;
        playlistItem->ActiveMonday		= true;
        playlistItem->ActiveTuesday		= false;
        playlistItem->ActiveWednesday	= true;
        playlistItem->ActiveThursday	= false;
        playlistItem->ActiveFriday		= true;
        playlistItem->ActiveSaturday	= false;
        playlistItem->SequenceId		= 25;
        playlistItem->Duration			= 26;
        playlistItem->AudioAdjustment	= "0.79";
        playlistItem->Mute				= false;
        playlistItem->Image				= false;

        Playlist->Items->push_back(move(playlistItem));

        playlistItem.reset(new _PlaylistItem);
        playlistItem->Content.FileName	= "File4.txt";
        playlistItem->Content.MD5		= "MD54";
        playlistItem->Enabled			= true;
        playlistItem->BeginDate			= 0;
        playlistItem->EndDate			= 0xffffffff;
        playlistItem->BeginTime			= "0000";
        playlistItem->EndTime			= "0000";
        playlistItem->ActiveSunday		= false;
        playlistItem->ActiveMonday		= true;
        playlistItem->ActiveTuesday		= true;
        playlistItem->ActiveWednesday	= true;
        playlistItem->ActiveThursday	= true;
        playlistItem->ActiveFriday		= true;
        playlistItem->ActiveSaturday	= true;
        playlistItem->SequenceId		= 98;
        playlistItem->Duration			= 99;
        playlistItem->AudioAdjustment	= "1.0";
        playlistItem->Mute				= true;
        playlistItem->Image				= true;

        Playlist->Items->push_back(move(playlistItem));
    }

    void _ICardConfig5::CreateJsonDoc(rapidjson::Document& jsonDoc) 
    {
        string jsonStr = CreateJson();
        bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		
		
		ASSERT_FALSE(result) << AssertMsg::Format(0,
						L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

        //ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));
        //return move(jsonDoc);
    }

    void _ICardConfig5::CreateLevelsInfo(shared_ptr<ProgressiveLevelsInfo>& levelsInfo, const DWORD* pReceivedTickCount, const SYSTEMTIME* pReceivedTime) 
    {
        try
        {
        	bool result = false;
            rapidjson::Document jsonDoc;
			CreateJsonDoc(jsonDoc);

			levelsInfo = make_shared<ProgressiveLevelsInfo>(jsonDoc);

            if (pReceivedTickCount)
            {
                levelsInfo->m_receivedTickCount = *pReceivedTickCount;
            }

            if (pReceivedTime)
            {
                levelsInfo->m_receivedTime = *pReceivedTime;
            }
        }
        catch (SentinelException& e)
        {
            EXPECT_TRUE(false) << AssertMsg::FormatOnly(L"ProcessICardConfiguration failed with exception: %S", e.what());
        }

        //return levelsInfo;
    }

    shared_ptr<ProgressiveLevelManager> _ICardConfig5::CreateLevelManager(const DWORD* pReceivedTickCount, const SYSTEMTIME* pReceivedTime)
    {
        shared_ptr<ProgressiveLevelManager> levelManager = make_shared<ProgressiveLevelManager>();

        if (pReceivedTickCount)
        {
            levelManager->m_updatedTickCount = *pReceivedTickCount;
            levelManager->m_forwardedTickCount = *pReceivedTickCount;
        }

        if (pReceivedTime)
        {
            levelManager->m_receivedTime = *pReceivedTime;
        }

        try
        {
            shared_ptr<ProgressiveLevelsInfo> levelsInfo;
            CreateLevelsInfo(levelsInfo, pReceivedTickCount, pReceivedTime);
            levelManager->UpdateLevels(levelsInfo.get());
        }
        catch (SentinelException& e)
        {
        	bool pass = false;
            EXPECT_TRUE(pass) << AssertMsg::FormatOnly(L"ProcessICardConfiguration failed with exception: %S", e.what());
        }
        return levelManager;
    }

    string _ICardConfig5::CreateJson() const
    {
        ostringstream json;
        
        json << '{';
        
        json << "\"SkinId\":123,";
        json << "\"SkinRevision\":456,";
        json << "\"CampaignId\":123,";
        json << "\"CampaignRevision\":456,";
        json << "\"SourceDeviceId\":" << SourceDeviceId << ',';
        json << "\"SourceDeviceConfigVersion\":" << SourceDeviceConfigVersion << ',';
        json << "\"SourceAsset\":\"" << SourceAsset << "\",";
        json << "\"Timestamp\":" << Timestamp << ',';
        json << "\"MessageId\":\"" << MessageId << "\",";
        json << "\"DeviceId\":" << DeviceId << ",";
        json << "\"DeviceConfigVersion\":" << DeviceConfigVersion << ',';
        json << "\"DeviceAddress\":\"" << DeviceAddress << "\",";
        json << "\"OneLinkSystemId\":\"" << OneLinkSystemId << "\",";
        json << "\"EventChannel\":{";
        json << "\"Destination\":\"" << EventChannel.Destination << "\",";
        json << "\"MulticastDestination\":\"" << EventChannel.MulticastDestination << "\"},";
        json << "\"AssetNumber\":\"" << AssetNumber << "\",";
        json << "\"BankName\":\"" << BankName << "\",";
        json << "\"TimeZone\":\"" << TimeZone << "\",";
        json << "\"Games\":[";
        bool gamesTrailingComma = false;
        for_each(Games.cbegin(), Games.cend(), [&](const unique_ptr<_ICardGame5>& game)
                {
                if (gamesTrailingComma)
                json << ',';

                json << '{';
                json << "\"GameNumber\":" << game->GameNumber << ',';
                json << "\"Denomination\":" << game->Denomination << ',';
                json << "\"MaxBetCredits\":" << game->MaxBetCredits << ',';
                json << "\"TheoPercentage\":" << game->TheoPercentage << ',';
                json << "\"GameName\":\"" << game->GameName << "\",";
                json << "\"Paytable\":\"" << game->Paytable << "\",";
                json << "\"QueryCards\":" << (game->QueryCards ? "true" : "false");

                if (game->Levels.size())
                {
                json << ",\"Levels\":[";

                bool levelsTrailingComma = false;
                for_each(game->Levels.cbegin(), game->Levels.cend(), [&](const unique_ptr<_ICardGameLevel>& level)
                        {
                        if (levelsTrailingComma)
                        json << ',';

                        json << '{';
                        json << "\"LevelId\":" << level->LevelId << ',';
                        json << "\"LevelSequence\":" << level->LevelSequence;
                        json << '}';
                        levelsTrailingComma = true;
                        });

                json << ']';
                }

                json << '}';
                gamesTrailingComma = true;

                });
        json << "],";

        if (CardLevels.size())
        {
            json << "\"Levels\":[";

            bool levelsTrailingComma = false;
            for_each(CardLevels.cbegin(), CardLevels.cend(), [&](const unique_ptr<_ICardLevel>& level)
                    {
                    if (levelsTrailingComma)
                    json << ',';

                    json << '{';
                    json << "\"Id\":" << level->Id << ',';
                    json << "\"Version\":" << level->Version << ',';
                    json << "\"Type\":";
                    switch (level->Type)
                    {
                    case ProgressiveLevelType::Standard:
                    json << "\"Standard\",";
                    EXPECT_STREQ("Standard", ProgressiveLevelType::pStandard);
                    break;

                    case ProgressiveLevelType::Bonus:
                    json << "\"Bonus\",";
                    EXPECT_STREQ("Bonus", ProgressiveLevelType::pBonus);
                    break;

                    case ProgressiveLevelType::AccountingOnly:
                    json << "\"AccountingOnly\",";
                    EXPECT_STREQ("AccountingOnly", ProgressiveLevelType::pAccountingOnly);
                    break;
					case ProgressiveLevelType::PalcomX:
					case ProgressiveLevelType::Invalid:
						break;
                    };

                    json << "\"Channel\":{";
                    json << "\"Destination\":\"" << level->Channel.Destination << "\",";
                    json << "\"MulticastDestination\":\"" << level->Channel.MulticastDestination << "\"},";

                    if (level->Name)
                        json << "\"Name\":\"" << level->Name << "\",";

                    json << "\"MediaMinSeconds\":" << level->MediaMinSeconds << ',';
                    json << "\"MediaMaxSeconds\":" << level->MediaMaxSeconds;
                    json << '}';
                    levelsTrailingComma = true;
                    });

            json << "],";
        }

        json << "\"ProtocolType\":\"" << ProtocolType << "\",";
        json << "\"ProgressiveGroupId\":" << ProgressiveGroupId << ',';

        json << "\"PaymentMethod\":\"" << PaymentMethod << "\"";

        if (Playlist.get())
        {
            if (ContentHttpUrl)
                json << ",\"ContentHttpUrl\":\"" << ContentHttpUrl.val << "\",";

            json << "\"Playlist\":{";
            json << "\"Name\":\"" << Playlist->Name << "\",";
            json << "\"Shuffle\":" << (Playlist->Shuffle ? "true" : "false");

            if (Playlist->Items->size())
            {
                json << ",\"Items\":[";

                bool itemsTrailingComma = false;
                for_each(Playlist->Items->cbegin(), Playlist->Items->cend(), [&](const unique_ptr<_PlaylistItem>& item)
                        {
                        if (itemsTrailingComma)
                        json << ',';

                        json << '{';
                        json << "\"Content\":{";
                        json << "\"FileName\":\"" << item->Content.FileName << "\",";
                        json << "\"MD5\":\"" << item->Content.MD5 << "\"";
                        json << "},";
                        json << "\"Enabled\":" << (item->Enabled ? "true" : "false") << ',';
                        json << "\"Duration\":" << item->Duration;
                        json << '}';

                        itemsTrailingComma = true;
                        });

                json << ']';
            }

            json << '}';
        }


        json << '}';

        string jsonStr = json.str();
        //OutputDebugStringA("***** Config 5 *****\n");
        //OutputDebugStringA(jsonStr.c_str());
        //OutputDebugStringA("\n");
        //OutputDebugStringA("***** Config 5 *****\n");
        return jsonStr;
    }

    void LevelData::Verify(int line, CProgressiveLevel* pLevel) const
    {
        ASSERT_TRUE(pLevel != NULL) << AssertMsg::Format(line,L"pLevel");
        ASSERT_TRUE(pName != NULL) <<  AssertMsg::Format(line, L"pName");
        ASSERT_TRUE(pUrl != NULL) <<   AssertMsg::Format(line, L"pUrl");

        ASSERT_EQ(			updatedTickCount	,			pLevel->m_updatedTickCount	) << AssertMsg::Format(line, nullptr) << "m_receivedTickCount";
        ASSERT_EQ((long)		id					, (long)	pLevel->m_id				) << AssertMsg::Format(line, nullptr) << "m_id";
        ASSERT_EQ((int)		type				, (int)		pLevel->m_type				) << AssertMsg::Format(line, nullptr) << "m_type";
        ASSERT_EQ(			expectedNumSasLevels,			pLevel->m_sasLevels.size()	) << AssertMsg::Format(line, nullptr) << "m_sasLevels.size()";

        ASSERT_EQ(Convert::Utf8ToString(pName)	,		pLevel->m_name	) << AssertMsg::Format(line, nullptr) << "m_name";
        ASSERT_EQ(Convert::Utf8ToString(pUrl)	,		pLevel->m_url	) << AssertMsg::Format(line, nullptr) << "m_url";

        for (size_t i = 0; i < expectedNumSasLevels; i++)
        {
            ASSERT_EQ((int) sasLevels[i], (int) pLevel->m_sasLevels[i]) << AssertMsg::Format(line, nullptr, L"m_sasLevels[i]", i);
        }
    }

    shared_ptr<CProgressiveLevel> LevelData::CreateProgressiveLevel() const
    {
        shared_ptr<CProgressiveLevel> level(new CProgressiveLevel());

        level->m_id = id;
        level->m_type = type;
        level->m_updatedTickCount = updatedTickCount;
        level->m_name = Convert::Utf8ToString(pName);
        level->m_url = Convert::Utf8ToString(pUrl);

        for (size_t i = 0; i < numSasLevels; i++)
        {
            level->m_sasLevels.push_back(sasLevels[i]);
        }

        sort(level->m_sasLevels.begin(), level->m_sasLevels.end());

        return level;
    }
}}
