#include "stdafx.h"
#include "PollerProtocol/PollerMGMDData.h"
#include "PollerProtocol/PollerGameListInfo.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

class PollerGameListInfoUnitTests : public ::testing::Test {};

TEST_F(PollerGameListInfoUnitTests, ProcessConfigFeatureFlagsRecvdTest)
{
	//Test the default values
	CPollerGameListInfo pollerGameListInfo(nullptr);
	CPollerMGMDData::Instance().SetChunkedGamesListFlag(1);
	ASSERT_EQ(pollerGameListInfo.m_configFeatureFlagsRecvd, false);
	pollerGameListInfo.ProcessConfigFeatureFlagsRecvd();
	ASSERT_EQ(pollerGameListInfo.m_configFeatureFlagsRecvd, true);
	ASSERT_EQ(pollerGameListInfo.m_gameListChunkSize, DEFAULT_GAME_LIST_CHUNK_SIZE);
	ASSERT_EQ(pollerGameListInfo.m_isChunkedGamesListFlagEnabled, true);

	//Test the case when implented games count is zero
	pollerGameListInfo.m_configFeatureFlagsRecvd = false;
	CPollerMGMDData::Instance().SetChunkedGamesListFlag(0);
	pollerGameListInfo.ProcessConfigFeatureFlagsRecvd();
	ASSERT_EQ(pollerGameListInfo.m_isChunkedGamesListFlagEnabled, false);	
	ASSERT_EQ(pollerGameListInfo.m_configFeatureFlagsRecvd, true);
	ASSERT_EQ(pollerGameListInfo.m_gameListChunkSize, DEFAULT_GAME_LIST_CHUNK_SIZE);

	//Test the case when implented games count is non-zero
	WORD IMPLEMENTED_GAMES_COUNT(9);
	pollerGameListInfo.m_implementedGamesCount = IMPLEMENTED_GAMES_COUNT;
	pollerGameListInfo.m_configFeatureFlagsRecvd = false;
	CPollerMGMDData::Instance().SetChunkedGamesListFlag(0);
	pollerGameListInfo.ProcessConfigFeatureFlagsRecvd();
	ASSERT_EQ(pollerGameListInfo.m_isChunkedGamesListFlagEnabled, false);
	ASSERT_EQ(pollerGameListInfo.m_configFeatureFlagsRecvd, true);
	ASSERT_EQ(pollerGameListInfo.m_gameListChunkSize, IMPLEMENTED_GAMES_COUNT);
	ASSERT_EQ(CPollerMGMDData::Instance().GetGameListChunkSizeOption(), IMPLEMENTED_GAMES_COUNT);
}

TEST_F(PollerGameListInfoUnitTests, ProcessAllGameListFromInternalsTest)
{
	//Test the default values
	CPollerGameListInfo pollerGameListInfo(nullptr);

	CGames* games(new CGames());
	CGame* pGame1 = new CGame(MEMORY_NONE, false);
	pGame1->SetGameNumber(1);
	pGame1->SetDenom(1, DenomCurrencyType::US_DOLLARS);
	pGame1->SetMaxBet(250);
	pGame1->SetTheoreticalPayback((WORD)8782);
	pGame1->SetMfgID("AT");
	pGame1->SetPaytableID("906   ");
	games->AddGame(pGame1);
	CGame* pGame2 = new CGame(MEMORY_NONE, false);
	pGame2->SetGameNumber(3);
	pGame2->SetDenom(5, DenomCurrencyType::US_DOLLARS);
	pGame2->SetMaxBet(250);
	pGame2->SetTheoreticalPayback((WORD)8782);
	pGame2->SetMfgID("AT");
	pGame2->SetPaytableID("906   ");
	games->AddGame(pGame2);

	CInternalMessage msg(INT_MSG_SEND_ALL_GAMES_LIST, games);
	pollerGameListInfo.ProcessAllGameListFromInternals(&msg);
	ASSERT_EQ(pollerGameListInfo.m_implementedGamesCount, 0);
	ASSERT_EQ(pollerGameListInfo.m_oldImplementedGamesCount, 0);	
	ASSERT_EQ(pollerGameListInfo.m_updateNumberOfChunks, true);

	WORD implementedGamesCount(10);
	msg.SetData(implementedGamesCount);
	pollerGameListInfo.ProcessAllGameListFromInternals(&msg);
	ASSERT_EQ(pollerGameListInfo.m_implementedGamesCount, implementedGamesCount);
	ASSERT_EQ(pollerGameListInfo.m_oldImplementedGamesCount, implementedGamesCount);
	ASSERT_EQ(pollerGameListInfo.m_updateNumberOfChunks, true);
}