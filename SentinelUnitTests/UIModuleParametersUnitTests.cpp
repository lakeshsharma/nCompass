#include "stdafx.h"
#include "UnitTestingUtils.h"
#include "ProgressiveProtocol/ProgressiveLevelTestHelpers.h"
#include "ProgressiveProtocol/ProgressiveClaimableAward.h"
#include "UI/UIModuleParameters.h"
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;
using std::string;

namespace SentinelUnitTests
{
        const string DefaultConnection = "AttractorConnection";


		TEST(UIModuleParametersUnitTests, UIModuleParametersConstruction)
		{
			CUIModuleParameters *flashParams = new CUIModuleParameters();
            string expectedParams = "Connection=" + DefaultConnection;
			string params = flashParams->GetParametersString();
			ASSERT_TRUE(params.compare(expectedParams)==0) << L"Parameter string expected to be empty";
			delete flashParams;
		}

		TEST(UIModuleParametersUnitTests, UIModuleParametersPrefixOnly)
		{
			CUIModuleParameters *flashParams = new CUIModuleParameters();

			string Prefix = "TestPrefix";
			flashParams->SetSkinnedPrefix(Prefix);
			string expectedParams = Prefix + "&Connection=" + DefaultConnection;
			string params = flashParams->GetParametersString();
			ASSERT_TRUE(params.compare(expectedParams)==0) << L"Parameter string expected to match the passed-in prefix";
			string Prefix2 = "TestPrefix2";
			flashParams->SetSkinnedPrefix(Prefix2);
            expectedParams = Prefix2 + "&Connection=" + DefaultConnection;
            params = flashParams->GetParametersString();
			ASSERT_TRUE(params.compare(expectedParams)==0)<< L"Parameter string expected to match the changed prefix";

			delete flashParams;
		}

		TEST(UIModuleParametersUnitTests, UIModuleParametersPrefixDevice)
		{
			CUIModuleParameters *flashParams = new CUIModuleParameters();

			string Prefix = "TestPrefix";
			const long Slotmast = 1234;
			const long Slotnum = 5678;

			flashParams->SetSkinnedPrefix(Prefix);
			flashParams->SetDeviceInfo(Slotmast, Slotnum);

			string expectedParams = Prefix + "&MachineID=" + to_string(Slotmast)+ "&SlotNumber=" + to_string(Slotnum) + "&Connection=" + DefaultConnection;
            string params = flashParams->GetParametersString();
			ASSERT_TRUE(params.compare(expectedParams)==0) << L"Parameter string expected to contain prefix and device info";

			delete flashParams;
		}

        TEST(UIModuleParametersUnitTests, UIModuleParametersPlayer)
		{
			CUIModuleParameters *flashParams = new CUIModuleParameters();

			const string PlayerName="PlayerName";
			const int   RankID = 1234;
			const string RankName="RankName";
			const string Prefix="TestPrefix";
			const long Slotmast = 1234;
			const long Slotnum = 5678;
			const long PlayerID = 9876;
			const string CardNumber="0123456789";

            CPlayerInfo *playerInfo(new CPlayerInfo());
			playerInfo->SetName(PlayerName);
			playerInfo->SetRankId(RankID);
			playerInfo->SetRankName(RankName);
			playerInfo->SetPlayerId(PlayerID);

			CConfig *config = new CConfig(true,"1.5");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CCard *card = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime());
			card->SetCardID(CardNumber);

            CGames *games = new CGames();
			CPlayer *player = new CPlayer(card, playerInfo, *config, egmConfig, CUtilities::GetCurrentTime(), false, games);
			//player->SetPlayerInfo(playerInfo);

			flashParams->SetSkinnedPrefix(Prefix);
			flashParams->SetDeviceInfo(Slotmast, Slotnum);
			flashParams->SetPlayer(player, false);

			string expectedParams = Prefix + "&MachineID="+to_string(Slotmast)+"&PlayerID="+to_string(PlayerID)+"&PlayerName="+PlayerName+"&TierID="+to_string(RankID)+"&TierName=" +RankName+ "&SlotNumber="+to_string(Slotnum)+"&CardNumber="+CardNumber+"&Connection="+DefaultConnection;
            string params = flashParams->GetParametersString();
			ASSERT_TRUE(params.compare(expectedParams)==0) << L"Parameter string expected to contain prefix, device and player info";

			delete player;
			delete flashParams;
		}
}

