#include "stdafx.h"
#include "PlayerInfo.h"

// Sentinel includes.
#include "GlobalDefs.h"
#include "Card.h"
#include "Meters.h"
#include "NVRAMConfig.h"
#include "Player.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "AssertMsg.h"
#include "UnitTestingUtils.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class PlayerInfoUnitTests : public ::testing::Test
	{
	public:
		CCard* card;
		string name;
		string responseText;
		string rankName;

		DWORD multiplier;
		DWORD divider;
		DWORD lastPoints;
		DWORD lastCountdown;
		DWORD compValue;
		DWORD dailyPoints;
		DWORD pointsTillNextTier;
		DWORD tierMultiplier;
		DWORD rankId;
		DWORD playerId;
		DWORD tierPoints;

		bool bBirthdayToday;
		bool bAnniversaryToday;
		bool bBannedFromPlay;		
		bool bMaxCardLimitIndicator;
		bool bAllowPBT;

		AccountingType accountingType;

		bool bEBAvailable;
		std::string ebJSONData;
		DWORD ebPointsId;
		DWORD ebPromosId;
		DWORD ebCompsId;
		DWORD ebTierPointsId;
		std::vector<ExtendedBucketsCountdownData*> ebCountdowns;
	public:
		PlayerInfoUnitTests()
		{
			time_t cardTime(CUtilities::GetTimeFromFields(2013, 03, 07, 12, 12, 12));
			card = new CCard(cardTime,cardTime);
			card->SetCardID("A531000002");
			name = DEFAULT_PLAYER_NAME;
			responseText = _T("Response");
			rankName = _T("RankName");

			multiplier = 1;
			divider = 2;
			lastPoints = 3;
			lastCountdown = 4;
			compValue = 5;

			bBirthdayToday = true;
			bAnniversaryToday = false;
			bBannedFromPlay = true;
			bAllowPBT = true;
			bMaxCardLimitIndicator = false;

			dailyPoints = 6;			
			pointsTillNextTier = 7;
			tierMultiplier = 8;
			rankId = 9;
			playerId = 10;
			tierPoints = 11;

			accountingType = AccountingType::ACCOUNTING_COINS_IN;

			bEBAvailable = true;
			ebJSONData = "Extended buckets JSON data";
			ebPointsId = 10;
			ebPromosId = 20;
			ebCompsId = 30;
			ebTierPointsId = 40;

			ExtendedBucketsCountdownData *countdownData1 = new ExtendedBucketsCountdownData();
			countdownData1->id = 101;
			countdownData1->displayName = "Bucket 1";
			countdownData1->bucketCurrency = "$";
			countdownData1->countdown = 100;
			countdownData1->multiplier = 10;
			countdownData1->divider = 100;
			ebCountdowns.push_back(countdownData1);

			ExtendedBucketsCountdownData *countdownData2 = new ExtendedBucketsCountdownData();
			countdownData1->id = 102;
			countdownData1->displayName = "Bucket 2";
			countdownData1->bucketCurrency = "";
			countdownData1->countdown = 200;
			countdownData1->multiplier = 20;
			countdownData1->divider = 200;
			ebCountdowns.push_back(countdownData2);
		}

		CPlayerInfo* CreatePlayerInfo()
		{
			CPlayerInfo* playerInfo(new CPlayerInfo());
			int cardNumberLen = 0;
			const byte* cardNumber = card->GetCardID(cardNumberLen);
			playerInfo->SetCardNumber(cardNumber, cardNumberLen);
			playerInfo->SetName(name);
			playerInfo->SetResponseText(responseText);
			playerInfo->SetRankName(rankName);
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(lastPoints, lastCountdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			playerInfo->SetFlags(bBirthdayToday, bAnniversaryToday, bBannedFromPlay);
			playerInfo->SetCompValue(compValue);
			playerInfo->AllowPBT(bAllowPBT);
			playerInfo->SetDailyPoints(dailyPoints);
			playerInfo->MaxCardLimitReached(bMaxCardLimitIndicator);
			playerInfo->SetPointsTillNextTier(pointsTillNextTier);
			playerInfo->SetTierMultiplier(tierMultiplier);
			playerInfo->SetRankId(rankId);
			playerInfo->SetPlayerId(playerId);
			playerInfo->SetTierPoints(tierPoints);
			playerInfo->SetAccountingType(accountingType);
			playerInfo->m_bEBAvailable = bEBAvailable;
			playerInfo->m_ebJSONData = ebJSONData;
			playerInfo->m_ebPointsId = ebPointsId;
			playerInfo->m_ebPromosId = ebPromosId;
			playerInfo->m_ebCompsId = ebCompsId;
			playerInfo->m_ebTierPointsId = ebTierPointsId;
			playerInfo->m_ExtendedBucketsCountdowns = ebCountdowns;
			return playerInfo;
		}
		
		void TestToComparePlayerInfo(CPlayerInfo* playerInfo1, CPlayerInfo* playerInfo2, string nameOfTest, bool compareEBParams = true)
		{
			int cardNumberLen1(0), cardNumberLen2(0);
			ASSERT_TRUE((playerInfo1->GetName() == playerInfo2->GetName())) << 
				 _T("CPlayerInfo - m_name was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE((playerInfo1->GetResponseText() == playerInfo2->GetResponseText())) << 
				_T("CPlayerInfo - m_responseText was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE((playerInfo1->GetRankName() == playerInfo2->GetRankName())) <<
				_T("CPlayerInfo - m_rankName was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(!memcmp(playerInfo1->GetCardNumber(cardNumberLen1), playerInfo2->GetCardNumber(cardNumberLen2), cardNumberLen1)) <<
				_T("CPlayerInfo - m_cardNumber was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(cardNumberLen1 == cardNumberLen2) <<
				_T("CPlayerInfo - m_cardNumberLen was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->GetMultiplier() == playerInfo2->GetMultiplier()) <<
				_T("CplayerInfo - m_multiplier was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->GetDivider() == playerInfo2->GetDivider()) << 
				_T("CplayerInfo - m_divider was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->GetPoints() == playerInfo2->GetPoints()) << 
				_T("CPlayerInfo - m_lastPoints was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->GetCountdown() == playerInfo2->GetCountdown()) <<
				_T("CPlayerInfo - m_lastCountdown was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->GetCompValue() == playerInfo2->GetCompValue()) <<
				_T("CPlayerInfo - m_compValue was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->IsBirthdayToday() == playerInfo2->IsBirthdayToday()) <<
				_T("CPlayerInfo - m_bBirthdayToday was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->IsAnniversaryToday() == playerInfo2->IsAnniversaryToday()) <<
				_T("CPlayerInfo - m_bAnniversaryToday was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->IsBannedFromPlay() == playerInfo2->IsBannedFromPlay()) <<
				_T("CPlayerInfo - m_bBannedFromPlay was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->IsPBTAllowed() == playerInfo2->IsPBTAllowed()) <<
				_T("CPlayerInfo - m_bAllowPBT was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->IsMaxCardLimitReached() == playerInfo2->IsMaxCardLimitReached()) << 
				_T("CPlayerInfo - m_bMaxCardLimitIndicator was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->GetDailyPoints() == playerInfo2->GetDailyPoints()) <<
				_T("CPlayerInfo - m_dailyPoints was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->GetPointsTillNextTier() == playerInfo2->GetPointsTillNextTier()) <<
				_T("CPlayerInfo - m_pointsTillNextTier was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->GetTierMultiplier() == playerInfo2->GetTierMultiplier()) <<
				_T("CPlayerInfo - m_tierMultiplier was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->GetRankId() == playerInfo2->GetRankId()) <<
				_T("CPlayerInfo - m_rankId was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->GetPlayerId() == playerInfo2->GetPlayerId()) <<
				_T("CPlayerInfo - m_playerId was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->GetTierPoints() == playerInfo2->GetTierPoints()) <<
				_T("CPlayerInfo - m_tierPoints was not properly setup by the ") + nameOfTest;
			ASSERT_TRUE(playerInfo1->GetAccountingType() == playerInfo2->GetAccountingType()) <<
				_T("CPlayerInfo - m_accountingType was not properly setup by the ") + nameOfTest;

			if (compareEBParams)
			{
				ASSERT_TRUE(playerInfo1->IsExtendedBucketsAvailable() == playerInfo2->IsExtendedBucketsAvailable()) <<
					_T("CPlayerInfo - m_bEBAvailable was not properly setup by the ") + nameOfTest;
				ASSERT_TRUE(playerInfo1->GetExtendedBucketsJSONData() == playerInfo2->GetExtendedBucketsJSONData()) <<
					_T("CPlayerInfo - m_ebJSONData was not properly setup by the ") + nameOfTest;
				ASSERT_TRUE(playerInfo1->GetEBPointsId() == playerInfo2->GetEBPointsId()) <<
					_T("CPlayerInfo - m_ebPointsId was not properly setup by the ") + nameOfTest;
				ASSERT_TRUE(playerInfo1->GetEBPromosId() == playerInfo2->GetEBPromosId()) <<
					_T("CPlayerInfo - m_ebPromosId was not properly setup by the ") + nameOfTest;
				ASSERT_TRUE(playerInfo1->GetEBCompsId() == playerInfo2->GetEBCompsId()) <<
					_T("CPlayerInfo - m_ebCompsId was not properly setup by the ") + nameOfTest;
				ASSERT_TRUE(playerInfo1->GetEBTierPointsId() == playerInfo2->GetEBTierPointsId()) <<
					_T("CPlayerInfo - m_ebTierPointsId was not properly setup by the ") + nameOfTest;

				const std::vector<ExtendedBucketsCountdownData*> countdownData1 = playerInfo1->GetExtendedBucketsCountdownData();
				const std::vector<ExtendedBucketsCountdownData*> countdownData2 = playerInfo2->GetExtendedBucketsCountdownData();
				ASSERT_TRUE(countdownData1.size() == countdownData2.size()) <<
					_T("CPlayerInfo - m_ExtendedBucketsCountdowns was not properly setup by the ") + nameOfTest;

				for (int i = 0; i < countdownData1.size(); i++)
				{
					ASSERT_TRUE(countdownData1[i]->id == countdownData2[i]->id) <<
						_T("CPlayerInfo - m_ExtendedBucketsCountdowns[]->id was not properly setup by the ") + nameOfTest;
					ASSERT_TRUE(countdownData1[i]->displayName == countdownData2[i]->displayName) <<
						_T("CPlayerInfo - m_ExtendedBucketsCountdowns[]->displayName was not properly setup by the ") + nameOfTest;
					ASSERT_TRUE(countdownData1[i]->bucketCurrency == countdownData2[i]->bucketCurrency) <<
						_T("CPlayerInfo - m_ExtendedBucketsCountdowns[]->bucketCurrency was not properly setup by the ") + nameOfTest;
					ASSERT_TRUE(countdownData1[i]->countdown == countdownData2[i]->countdown) <<
						_T("CPlayerInfo - m_ExtendedBucketsCountdowns[]->countdown was not properly setup by the ") + nameOfTest;
					ASSERT_TRUE(countdownData1[i]->multiplier == countdownData2[i]->multiplier) <<
						_T("CPlayerInfo - m_ExtendedBucketsCountdowns[]->multiplier was not properly setup by the ") + nameOfTest;
					ASSERT_TRUE(countdownData1[i]->divider == countdownData2[i]->divider) <<
						_T("CPlayerInfo - m_ExtendedBucketsCountdowns[]->divider was not properly setup by the ") + nameOfTest;
				}
			}
		}
	};

	TEST_F(PlayerInfoUnitTests, PlayerInfoConstructorTest)
		{
			//Setup
			CPlayerInfo* playerInfo(new CPlayerInfo());
			int cardNumberLen(0);

			//Tests
			ASSERT_TRUE((playerInfo->GetName() == DEFAULT_PLAYER_NAME)) <<
				L"CPlayerInfo - m_name was not properly setup by the default constructor";
			ASSERT_TRUE((playerInfo->GetResponseText() == _T(""))) <<
				L"CPlayerInfo - m_responseText was not properly setup by the default constructor";
			ASSERT_TRUE((playerInfo->GetRankName() == _T(""))) <<
				L"CPlayerInfo - m_rankName was not properly setup by the default constructor";
			ASSERT_EQ((void *)nullptr, (void *)playerInfo->GetCardNumber(cardNumberLen)) <<
				L"CPlayerInfo - m_cardNumber was not properly setup by the default constructor";
			ASSERT_TRUE(cardNumberLen == 0) <<
				L"CPlayerInfo - cardNumberLen was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetMultiplier() == DEFAULT_MULTIPLIER) <<
				L"CPlayerInfo - m_multiplier was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetDivider() == DEFAULT_COUNTDOWN) <<
				L"CPlayerInfo - m_divider was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetPoints() == 0) <<
				L"CPlayerInfo - m_lastPoints was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetCountdown() == DEFAULT_COUNTDOWN) <<
				L"CPlayerInfo - m_lastCountdown was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetCompValue() == 0) <<
				L"CPlayerInfo - m_compValue was not properly setup by the default constructor";
			ASSERT_FALSE(playerInfo->IsBirthdayToday()) <<
				L"CPlayerInfo - m_bBirthdayToday was not properly setup by the default constructor";
			ASSERT_FALSE(playerInfo->IsAnniversaryToday()) <<
				L"CPlayerInfo - m_bAnniversaryToday was not properly setup by the default constructor";
			ASSERT_FALSE(playerInfo->IsBannedFromPlay()) <<
				L"CPlayerInfo - m_bBannedFromPlay was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->IsPBTAllowed()) <<
				L"CPlayerInfo - m_bAllowPBT was not properly setup by the default constructor";
			ASSERT_FALSE(playerInfo->IsMaxCardLimitReached()) <<
				L"CPlayerInfo - m_bMaxCardLimitIndicator was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetDailyPoints() == 0) <<
				L"CPlayerInfo - m_dailyPoints was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetPointsTillNextTier() == 0) <<
				L"CPlayerInfo - m_pointsTillNextTier was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetTierMultiplier() == 0) <<
				L"CPlayerInfo - m_tierMultiplier was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetRankId() == (DWORD)-1) <<
				L"CPlayerInfo - m_rankId was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetPlayerId() == CPollerPlayerResponseMessage::DEFAULT_PLAYER_ID) <<
				L"CPlayerInfo - m_playerId was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetTierPoints() == CPollerPlayerResponseMessage::DEFAULT_TIER_POINTS) <<
				L"CPlayerInfo - m_tierPoints was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetAccountingType() == AccountingType::ACCOUNTING_COINS_IN) <<
				L"CPlayerInfo - m_accountingType was not properly setup by the default constructor";
			ASSERT_FALSE(playerInfo->IsExtendedBucketsAvailable()) <<
				L"CPlayerInfo - m_bEBAvailable was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetExtendedBucketsJSONData() == _T("")) <<
				L"CPlayerInfo - m_ebJSONData was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetEBPointsId() == 0) <<
				L"CPlayerInfo - m_ebPointsId was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetEBPromosId() == 0) <<
				L"CPlayerInfo - m_ebPromosId was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetEBCompsId() == 0) <<
				L"CPlayerInfo - m_ebCompsId was not properly setup by the default constructor";
			ASSERT_TRUE(playerInfo->GetEBTierPointsId() == 0) <<
				L"CPlayerInfo - m_ebTierPointsId was not properly setup by the default constructor";

			//Cleanup
			delete playerInfo;
		}

	TEST_F(PlayerInfoUnitTests, PlayerInfoCopyConstructorTest)
		{
			//Setup
			int cardNumberLen1(0), cardNumberLen2(0);
			CPlayerInfo* playerInfo1(CreatePlayerInfo());			
			CPlayerInfo* playerInfo2(new CPlayerInfo(*playerInfo1));//Calling copy constructor
			
			//Tests
			TestToComparePlayerInfo(playerInfo1, playerInfo2, "Copy Constructor");

			//Cleanup
			delete playerInfo1;
			delete playerInfo2;
		}

		TEST_F(PlayerInfoUnitTests, PlayerInfoAssignmentOperatorTest)
		{
			//Setup
			CPlayerInfo* playerInfo1(CreatePlayerInfo());
			CPlayerInfo* playerInfo2(new CPlayerInfo());
			*playerInfo2 = *playerInfo1;//Calling Assignment Operator

			//Tests
			TestToComparePlayerInfo(playerInfo1, playerInfo2, "Assignment Operator");

			//Cleanup
			delete playerInfo1;
			delete playerInfo2;
		}

		TEST_F(PlayerInfoUnitTests, PlayerInfoNvramTest)
		{
			//Setup
			CPlayerInfo* playerInfo1(CreatePlayerInfo());
			byte* nvramBuffer1 = new byte[NVRAM_PLAYER_INFO_SIZE];
			byte* nvramBuffer2 = new byte[NVRAM_PLAYER_INFO_SIZE];
			ZeroMemory(nvramBuffer1, NVRAM_PLAYER_INFO_SIZE);
			ZeroMemory(nvramBuffer2, NVRAM_PLAYER_INFO_SIZE);
			playerInfo1->GetNvramBuffer( nvramBuffer1, NVRAM_PLAYER_INFO_SIZE );

			CPlayerInfo* playerInfo2(new CPlayerInfo(nvramBuffer1));

			//Tests
			//Compare the values of the two player information
			bool compareEBParams = false; // don't compare EB params as it is not saved in NVRAM
			TestToComparePlayerInfo(playerInfo1, playerInfo2, "CPlayerInfo(byte *nvramStream, int nvramBufferLength) function", compareEBParams);

			//Compare NVRAM contents of the two player info
			playerInfo2->GetNvramBuffer( nvramBuffer2, NVRAM_PLAYER_INFO_SIZE );
			ASSERT_TRUE(!memcmp(nvramBuffer1, nvramBuffer2, NVRAM_PLAYER_INFO_SIZE)) << _T("NVRAM values are not equal ");

			//Cleanup
			delete playerInfo1;
			delete playerInfo2;
			delete[] nvramBuffer1;
			delete[] nvramBuffer2;
		}

		TEST_F(PlayerInfoUnitTests, PlayerInfoFunctionsTest)
		{
			//Setup
			CPlayerInfo* playerInfo(CreatePlayerInfo());
			int cardNumberLen1(0), cardNumberLen2(0);			
			
			//Tests
			ASSERT_TRUE(!_tcscmp(playerInfo->GetName().c_str(), name.c_str())) << 
				L"CPlayerInfo - m_name was not properly setup by the SetName function";
			ASSERT_TRUE(!_tcscmp(playerInfo->GetResponseText().c_str(), responseText.c_str())) <<
				L"CPlayerInfo - m_responseText was not properly setup SetResponseText function";
			ASSERT_TRUE(!_tcscmp(playerInfo->GetRankName().c_str(), rankName.c_str())) <<
				L"CPlayerInfo - m_rankName was not properly setup by the SetRankName function";
			ASSERT_TRUE(cardNumberLen1 == cardNumberLen2) <<
				L"CPlayerInfo - m_cardNumber was not properly setup by the SetCardNumber function";

			const byte* cardNumber1(playerInfo->GetCardNumber(cardNumberLen1));
			const byte* cardNumber2(card->GetCardID(cardNumberLen2));
			ASSERT_TRUE(cardNumberLen1 == cardNumberLen2) << L"Length of card numbers in bytes are not equal.";
			ASSERT_TRUE(!memcmp(cardNumber1, cardNumber2, cardNumberLen1)) << L"CPlayerInfo - m_cardNumber was not properly setup by the SetCardNumber function";

			ASSERT_TRUE(playerInfo->GetMultiplier() == multiplier) <<
				L"CplayerInfo - m_multiplier was not properly setup by the SetMultiplierDivider function";
			ASSERT_TRUE(playerInfo->GetDivider() == divider) << 
				L"CplayerInfo - m_divider was not properly setup by the SetMultiplierDivider function";
			ASSERT_TRUE(playerInfo->GetPoints() == lastPoints) << 
				L"CPlayerInfo - m_lastPoints was not properly setup by the SetPointsAndCountdown function";
			ASSERT_TRUE(playerInfo->GetCountdown() == lastCountdown) <<
				L"CPlayerInfo - m_lastCountdown was not properly setup by the SetPointsAndCountdown function";
			ASSERT_TRUE(playerInfo->GetCompValue() == compValue) <<
				L"CPlayerInfo - m_compValue was not properly setup by the SetCompValue function";
			ASSERT_TRUE(playerInfo->IsBirthdayToday() == bBirthdayToday) <<
				L"CPlayerInfo - m_bBirthdayToday was not properly setup by the SetFlags function";
			ASSERT_TRUE(playerInfo->IsAnniversaryToday() == bAnniversaryToday) <<
				L"CPlayerInfo - m_bAnniversaryToday was not properly setup by the  SetFlags function";
			ASSERT_TRUE(playerInfo->IsBannedFromPlay() == bBannedFromPlay) <<
				L"CPlayerInfo - m_bBannedFromPlay was not properly setup by the SetFlags function";
			ASSERT_TRUE(playerInfo->IsPBTAllowed() == bAllowPBT) <<
				L"CPlayerInfo - m_bAllowPBT was not properly setup by the AllowPBT function";
			ASSERT_TRUE(playerInfo->IsMaxCardLimitReached() == bMaxCardLimitIndicator) << 
				L"CPlayerInfo - m_bMaxCardLimitIndicator was not properly setup by the MaxCardLimitReached function";
			ASSERT_TRUE(playerInfo->GetDailyPoints() == dailyPoints) <<
				L"CPlayerInfo - m_dailyPoints was not properly setup by the SetDailyPoints function";
			ASSERT_TRUE(playerInfo->GetPointsTillNextTier() == pointsTillNextTier) <<
				L"CPlayerInfo - m_pointsTillNextTier was not properly setup by the SetPointsTillNextTier function";
			ASSERT_TRUE(playerInfo->GetTierMultiplier() == tierMultiplier) <<
				L"CPlayerInfo - m_tierMultiplier was not properly setup by the SetTierMultiplier function";
			ASSERT_TRUE(playerInfo->GetRankId() == rankId) <<
				L"CPlayerInfo - m_rankId was not properly setup by the SetRankId function";
			ASSERT_TRUE(playerInfo->GetPlayerId() == playerId) <<
				L"CPlayerInfo - m_playerId was not properly setup by the SetRankId function";
			ASSERT_TRUE(playerInfo->GetTierPoints() == tierPoints) <<
				L"CPlayerInfo - m_tierPoints was not properly setup by the SetRankId function";
			ASSERT_TRUE(playerInfo->GetAccountingType() == accountingType) <<
				L"CPlayerInfo - m_accountingType was not properly setup by the SetAccountingType function";

			//Cleanup
			delete playerInfo;
		}

		void SetFlagsTest(CPlayerInfo* playerInfo, bool bBirthdayToday, bool bAnniversaryToday, bool bBannedFromPlay)
		{
			playerInfo->SetFlags(bBirthdayToday, bAnniversaryToday, bBannedFromPlay);
			ASSERT_TRUE(playerInfo->IsBirthdayToday() == bBirthdayToday) <<
				L"CPlayerInfo - m_bBirthdayToday was not properly setup by the SetFlags function";
			ASSERT_TRUE(playerInfo->IsAnniversaryToday() == bAnniversaryToday) <<
				L"CPlayerInfo - m_bAnniversaryToday was not properly setup by the  SetFlags function";
			ASSERT_TRUE(playerInfo->IsBannedFromPlay() == bBannedFromPlay) <<
				L"CPlayerInfo - m_bBannedFromPlay was not properly setup by the SetFlags function";
		}

		TEST_F(PlayerInfoUnitTests, PlayerInfoSetFlagsTest)
		{
			//Setup
			CPlayerInfo* playerInfo(CreatePlayerInfo());
			
			//Tests
			SetFlagsTest(playerInfo, true, true, true);
			SetFlagsTest(playerInfo, false, false, false);

			SetFlagsTest(playerInfo, true, false, false);
			SetFlagsTest(playerInfo, true, true, false);
			SetFlagsTest(playerInfo, true, false, true);
			
			SetFlagsTest(playerInfo, false, true, true);
			SetFlagsTest(playerInfo, false, true, false);
			SetFlagsTest(playerInfo, false, false, true);			

			//Cleanup
			delete playerInfo;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerInfo::CPlayerInfo()
		///     CPlayerInfo::SetRankId()
		///     CPlayerInfo::GetRankId()
		/// Code coverage for:
		///     CPlayerInfo::LogRankInfo()
		/// Get code coverage for LogRankInfo() with a variety of values.
		/// </summary>
		TEST_F(PlayerInfoUnitTests, SetRankIdTest)
		{
			// Test data.
			typedef struct
			{
				// Input(s).
				const DWORD m_rankId;

				// Expected result(s).
				const DWORD m_rankIdExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (DWORD)-1, (DWORD)-1 },

				{ 0, 0 },
				{ 1, 1 },
				{ 2, 2 },
				{ 1234567890, 1234567890 },
				{ 0x12345678, 0x12345678 },
				{ INT_MAX - 1, INT_MAX - 1 },
				{ INT_MAX, INT_MAX },

				{ (DWORD)INT_MIN, (DWORD)INT_MIN },
				{ (DWORD)(INT_MIN + 1), (DWORD)(INT_MIN + 1) },
				{ (DWORD)-1234567890, (DWORD)-1234567890 },

				{ DWORD_MAX - 1, DWORD_MAX - 1 },
				{ DWORD_MAX, DWORD_MAX },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPlayerInfo varObj;

				// Perform operation(s) to be tested.
				varObj.SetRankId(TEST_DATA[testIndex].m_rankId);
				const CPlayerInfo &obj(varObj);
				obj.LogRankInfo();

				// Test(s).
				ASSERT_EQ(obj.GetRankId(), TEST_DATA[testIndex].m_rankIdExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerInfo::CPlayerInfo()
		///     CPlayerInfo::SetPlayerId()
		///     CPlayerInfo::GetPlayerId()
		/// Code coverage for:
		///     CPlayerInfo::LogRankInfo()
		/// Get code coverage for LogRankInfo() with a variety of values.
		/// </summary>
		TEST_F(PlayerInfoUnitTests, SetPlayerIdTest)
		{
			// Test data.
			typedef struct
			{
				// Input(s).
				const DWORD m_playerId;

				// Expected result(s).
				const DWORD m_playerIdExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (DWORD)-1, (DWORD)-1 },

				{ 0, 0 },
				{ 1, 1 },
				{ 2, 2 },
				{ 1234567890, 1234567890 },
				{ 0x12345678, 0x12345678 },
				{ INT_MAX - 1, INT_MAX - 1 },
				{ INT_MAX, INT_MAX },

				{ (DWORD)INT_MIN, (DWORD)INT_MIN },
				{ (DWORD)(INT_MIN + 1), (DWORD)(INT_MIN + 1) },
				{ (DWORD)-1234567890, (DWORD)-1234567890 },

				{ DWORD_MAX - 1, DWORD_MAX - 1 },
				{ DWORD_MAX, DWORD_MAX },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPlayerInfo varObj;

				// Perform operation(s) to be tested.
				varObj.SetPlayerId(TEST_DATA[testIndex].m_playerId);
				const CPlayerInfo &obj(varObj);
				obj.LogRankInfo();

				// Test(s).
				ASSERT_EQ(obj.GetPlayerId(), TEST_DATA[testIndex].m_playerIdExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerInfo::CPlayerInfo()
		///     CPlayerInfo::SetTierPoints()
		///     CPlayerInfo::GetTierPoints()
		/// Code coverage for:
		///     CPlayerInfo::LogRankInfo()
		/// Get code coverage for LogRankInfo() with a variety of values.
		/// </summary>
		TEST_F(PlayerInfoUnitTests, SetTierPointsTest)
		{
			// Test data.
			typedef struct
			{
				// Input(s).
				const DWORD m_tierPoints;

				// Expected result(s).
				const DWORD m_tierPointsExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (DWORD)-1, (DWORD)-1 },

				{ 0, 0 },
				{ 1, 1 },
				{ 2, 2 },
				{ 1234567890, 1234567890 },
				{ 0x12345678, 0x12345678 },
				{ INT_MAX - 1, INT_MAX - 1 },
				{ INT_MAX, INT_MAX },

				{ (DWORD)INT_MIN, (DWORD)INT_MIN },
				{ (DWORD)(INT_MIN + 1), (DWORD)(INT_MIN + 1) },
				{ (DWORD)-1234567890, (DWORD)-1234567890 },

				{ DWORD_MAX - 1, DWORD_MAX - 1 },
				{ DWORD_MAX, DWORD_MAX },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPlayerInfo varObj;

				// Perform operation(s) to be tested.
				varObj.SetTierPoints(TEST_DATA[testIndex].m_tierPoints);
				const CPlayerInfo &obj(varObj);
				obj.LogRankInfo();

				// Test(s).
				ASSERT_EQ(obj.GetTierPoints(), TEST_DATA[testIndex].m_tierPointsExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerInfo::CPlayerInfo()
		///     CPlayerInfo::SetRankName()
		///     CPlayerInfo::GetRankName()
		/// Code coverage for:
		///     CPlayerInfo::LogRankInfo()
		/// Get code coverage for LogRankInfo() with a variety of values.
		/// </summary>
		TEST_F(PlayerInfoUnitTests, SetRankNameTest)
		{
			// Test data.
			typedef struct
			{
				// Input(s).
				const LPCTSTR m_rankName;

				// Expected result(s).
				const LPCTSTR m_rankNameExpected;
			} TestDataType;

			const LPCTSTR name1(_T(""));
			const LPCTSTR name2(_T(" "));
			const LPCTSTR name3(_T("a"));
			const LPCTSTR name4(_T("aB"));
			const LPCTSTR name5(_T("  aBc  "));
			const LPCTSTR name6(_T("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
			const LPCTSTR name7(_T("123456789012345678901234567890"));

			// All displayable ASCII characters, with imbedded quote. Only the first 50 characters are expected.
			const LPCTSTR name8(
				_T(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"));
			const LPCTSTR name8Expected(
				_T(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQ"));
			// All displayable ASCII characters, with imbedded quote. Only the first 50 characters are expected.
			const LPCTSTR name9(
				_T("QRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOP"));
			const LPCTSTR name9Expected(
				_T("QRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ !\"#"));

			// Maximum length less one, maximum length, maximum length plus one.
			const LPCTSTR name10(
				_T("a23456789.123456789.123456789.123456789.123456789"));
			const LPCTSTR name11(
				_T("b23456789.123456789.123456789.123456789.123456789."));
			const LPCTSTR name12(
				_T("c23456789.123456789.123456789.123456789.123456789.1"));
			const LPCTSTR name12Expected(
				_T("c23456789.123456789.123456789.123456789.123456789."));

			// 300 characters.
			const LPCTSTR name13(
				_T("d23456789.123456789.123456789.123456789.123456789.")
				_T("123456789.123456789.123456789.123456789.123456789.")
				_T("123456789.123456789.123456789.123456789.123456789.")
				_T("123456789.123456789.123456789.123456789.123456789.")
				_T("123456789.123456789.123456789.123456789.123456789.")
				_T("123456789.123456789.123456789.123456789.123456789."));
			const LPCTSTR name13Expected(
				_T("d23456789.123456789.123456789.123456789.123456789."));

			const TestDataType TEST_DATA[] =
			{
				{ _T(""), _T("") },
				{ name1, name1 },
				{ name2, name2 },
				{ name3, name3 },
				{ name4, name4 },
				{ name5, name5 },
				{ name6, name6 },
				{ name7, name7 },
				{ name8, name8Expected },
				{ name9, name9Expected },
				{ name10, name10 },
				{ name11, name11 },
				{ name12, name12Expected },
				{ name13, name13Expected },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CPlayerInfo varObj;

				// Perform operation(s) to be tested.
				varObj.SetRankName(TEST_DATA[testIndex].m_rankName);
				const CPlayerInfo &obj(varObj);
				obj.LogRankInfo();

				// Test(s).
				ASSERT_EQ(obj.GetRankName(), TEST_DATA[testIndex].m_rankNameExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			}
		}

		/// <summary>
		/// Code coverage for:
		///     CPlayerInfo::CPlayerInfo()
		///     CPlayerInfo::LogRankInfo()
		///     CPlayerInfo::~CPlayerInfo()
		/// Get code coverage for LogRankInfo() with default values.
		/// </summary>
		TEST_F(PlayerInfoUnitTests, LogRankInfoTest)
		{
			// Perform operation(s) to be tested.
			const CPlayerInfo obj;
			obj.LogRankInfo();

			ASSERT_TRUE(obj.GetRankId() == CPlayerInfo::INVALID_RANK_ID) <<
				_T("CPlayerInfo - m_rankId was not properly setup by the constructor");
		}

		/// <summary>
		/// Functional test for:
		///     CMemoryStorage::SetSprinkles()
		///     CMemoryStorage::IntegrityCheck()
		///     CMemoryStorage::UpgradeNVRAM()
		/// </summary>
		/// <remarks>
		/// Verify that upgrading NVRAM has no effect on the service window open configuration.
		/// </remarks>
		TEST_F(PlayerInfoUnitTests, CPlayerInfo_UpgradeNvRamTest)
		{
			const DWORD OLD_SIZE(256);
			const DWORD NEW_SIZE(512);

			typedef struct
			{
				// Input(s).
				const DWORD m_fromNvramVersion; // Upgrade NVRAM from this version to the current version.
				const bool m_oldUniverseEnabledXXX; // Use universe enabled to verify upgrades.
				const BYTE* const m_oldNvramBuffer;
				const BYTE* const m_newNvramBuffer;

				// Expected result(s).
				const bool m_universeEnabledExpectedXXX;
				const BYTE* const m_oldNvramBufferExpected;
				const BYTE* const m_newNvramBufferExpected;
			} TestDataType;

			BYTE old1[OLD_SIZE] = {
				'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4', '5', 0, // m_name.
				'r', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', // m_responseText.
				's', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2',
				't', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2',
				'u', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', 0,
				5, 0, 0, 0, // m_cardNumberLen.
				1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, // m_cardNumber.
				10, 20, 30, 40, // m_multiplier.
				11, 21, 31, 41, // m_divider.
				12, 22, 32, 42, // m_lastPoints.
				13, 23, 33, 43, // m_lastCountdown.
				14, 24, 34, 44, // m_compValue.
				0, // m_bBirthdayToday.
				1, // m_bAnniversaryToday.
				0, // m_bAllowPBT.
				15, 25, 35, 45, // m_dailyPoints.
				1, // m_bMaxCardLimitIndicator.
				16, 26, 36, 46, // m_pointsTillNextTier.
				17, 27, 37, 47, // m_tierMultiplier.
				0, // m_bBannedFromPlay.
				0xdd, 0xde, 0xdf, // Unused.
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
			};

			BYTE old1Expected[OLD_SIZE] = {
				'G', 'u', 'e', 's', 't', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // m_name.
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // m_responseText.
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, // m_cardNumberLen.
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // m_cardNumber.
				1, 0, 0, 0, // m_multiplier.
				0x30, 0x75, 0, 0, // m_divider.
				0, 0, 0, 0, // m_lastPoints.
				0x30, 0x75, 0, 0, // m_lastCountdown.
				0, 0, 0, 0, // m_compValue.
				0, // m_bBirthdayToday.
				0, // m_bAnniversaryToday.
				1, // m_bAllowPBT.
				0, 0, 0, 0, // m_dailyPoints.
				0, // m_bMaxCardLimitIndicator.
				0, 0, 0, 0, // m_pointsTillNextTier.
				0, 0, 0, 0, // m_tierMultiplier.
				0, // m_bBannedFromPlay.
				0xdd, 0xde, 0xdf, // Unused.
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
			};

			BYTE new1[NEW_SIZE] = {
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
				0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
				0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
				0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
				0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
				0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
				0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
				0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
				0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
				0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
				0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
				0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
			};

			BYTE new1Expected[NEW_SIZE] = {
				'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4', '5', 0, // m_name.
				'r', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', // m_responseText.
				's', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2',
				't', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2',
				'u', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '1', 0,
				5, 0, 0, 0, // m_cardNumberLen.
				1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, // m_cardNumber.
				10, 20, 30, 40, // m_multiplier.
				11, 21, 31, 41, // m_divider.
				12, 22, 32, 42, // m_lastPoints.
				13, 23, 33, 43, // m_lastCountdown.
				14, 24, 34, 44, // m_compValue.
				0, // m_bBirthdayToday.
				1, // m_bAnniversaryToday.
				0, // m_bAllowPBT.
				15, 25, 35, 45, // m_dailyPoints.
				1, // m_bMaxCardLimitIndicator.
				16, 26, 36, 46, // m_pointsTillNextTier.
				17, 27, 37, 47, // m_tierMultiplier.
				0, // m_bBannedFromPlay.
				0xff, 0xff, 0xff, 0xff, // m_rankId.
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // m_rankName.
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,// m_playerId, m_tierPoints
				0xff, 0xff, 0xff, 0xff, // m_countdownHighPrecision
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			};

			const TestDataType TEST_DATA[] =
			{
				{ NVRAM_ORIGINAL_VERSION, true, old1, new1, false, old1Expected, new1Expected },
				{ NVRAM_UNIVERSAL_VERSION, true, old1, new1, false, old1Expected, new1Expected },
				{ NVRAM_125_COMPATIBLE_VERSION, true, old1, new1, false, old1Expected, new1Expected },
				{ NVRAM_UNIVERSAL_POINTS_VERSION, true, old1, new1, false, old1Expected, new1Expected },
				{ NVRAM_NCL_CARD_TRACK_VERSION, true, old1, new1, false, old1Expected, new1Expected },
				{ NVRAM_REBOOT_DIAGNOSTIC_VERSION, true, old1, new1, false, old1Expected, new1Expected },
				{ NVRAM_PLAYER_RANK_VERSION, true, old1, new1, false, old1, new1 },	
				{ NVRAM_EGM_AWARD_VERSION, true, old1, new1, false, old1, new1 },
				{ NVRAM_CONFIG_EXT_VERSION, true, old1, new1, false, old1, new1 },
				{ NVRAM_CONFIG_VERSION_2, true, old1, new1, false, old1, new1 },
			};

			ASSERT_TRUE(OLD_SIZE == CPlayerInfo::NVRAM_OLD_PLAYER_INFO_SIZE) << _T("OLD_SIZE");
			ASSERT_TRUE(NEW_SIZE == NVRAM_PLAYER_INFO_SIZE) << _T("NEW_SIZE");

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Setup NVRAM to non-reset state.
				CMemoryStorage::SetSprinkles();
				CMemoryStorage::UpgradeNVRAM();
				CMemoryStorage::UpgradeNVRAM(); // Set memory storage to already upgraded.
				CNVRAMConfig nvramConfig;

				// Setup old NVRAM player information.
				BYTE oldBuffer[OLD_SIZE]; // Need read/write buffer for CNVRAMConfig::Set().
				memcpy(oldBuffer, TEST_DATA[testIndex].m_oldNvramBuffer, sizeof(oldBuffer));
				nvramConfig.Set(NVRAM_OLD_PLAYER_INFO_OFFSET, oldBuffer, sizeof(oldBuffer));
				memset(oldBuffer, 127 * (testIndex + 1), sizeof(oldBuffer));
				nvramConfig.Get(NVRAM_OLD_PLAYER_INFO_OFFSET, oldBuffer, sizeof(oldBuffer));
				CUnitTestingUtils::AssertEqual(oldBuffer, sizeof(oldBuffer), TEST_DATA[testIndex].m_oldNvramBuffer, OLD_SIZE,
					_T("oldBuffer init"), testIndex, __TFUNCTION__, __LINE__, NULL);

				// Setup new NVRAM player information.
				BYTE newBuffer[NEW_SIZE]; // Need read/write buffer for CNVRAMConfig::Set().
				memcpy(newBuffer, TEST_DATA[testIndex].m_newNvramBuffer, sizeof(newBuffer));
				nvramConfig.Set(NVRAM_PLAYER_INFO_OFFSET, newBuffer, sizeof(newBuffer));
				memset(newBuffer, 127 * (testIndex + 1), sizeof(newBuffer));
				nvramConfig.Get(NVRAM_PLAYER_INFO_OFFSET, newBuffer, sizeof(newBuffer));
				CUnitTestingUtils::AssertEqual(newBuffer, sizeof(newBuffer), TEST_DATA[testIndex].m_newNvramBuffer, NEW_SIZE,
					_T("newBuffer init"), testIndex, __TFUNCTION__, __LINE__, NULL);

				// Perform operation(s) to be tested.
				// Set NVRAM version and then upgrade NVRAM as appropriate.
				if (TEST_DATA[testIndex].m_fromNvramVersion <= NVRAM_ORIGINAL_VERSION)
				{
					CUnitTestingUtils::SetOldSprinkles();
				}
				else
				{
					CMemoryStorage::SetSprinkles();
				}
				CUnitTestingUtils::SetNvramVersion(TEST_DATA[testIndex].m_fromNvramVersion);
				CMemoryStorage::UpgradeNVRAM();

				// Test contents of old NVRAM player information.
				memset(oldBuffer, 127 * (testIndex + 5), sizeof(oldBuffer));
				nvramConfig.Get(NVRAM_OLD_PLAYER_INFO_OFFSET, oldBuffer, sizeof(oldBuffer));
				CUnitTestingUtils::AssertEqual(oldBuffer, sizeof(oldBuffer), TEST_DATA[testIndex].m_oldNvramBufferExpected, OLD_SIZE,
					_T("oldBuffer expected"), testIndex, __TFUNCTION__, __LINE__, NULL);

				// Test contents of new NVRAM player information.
				memset(newBuffer, 127 * (testIndex + 1), sizeof(newBuffer));
				nvramConfig.Get(NVRAM_PLAYER_INFO_OFFSET, newBuffer, sizeof(newBuffer));
				const BYTE* temp(TEST_DATA[testIndex].m_newNvramBufferExpected);
				CUnitTestingUtils::AssertEqual(newBuffer, sizeof(newBuffer), TEST_DATA[testIndex].m_newNvramBufferExpected, NEW_SIZE,
					_T("newBuffer expected"), testIndex, __TFUNCTION__, __LINE__, NULL);
			}
		}

		// Default expected values.
		static const WORD m_lossLimitBalanceExpected = 0;
		static const BYTE m_lossLimitStatusExpected = 0;
		static const DWORD m_dailyPointsExpected = (DWORD)-1;
		static const byte m_maxCardLimitIndicatorExpected = 0;
		static const DWORD m_pointsTillNextTierExpected = (DWORD)-1;
		static const DWORD m_tierMultiplierExpected = 0;
		static const DWORD m_rankIdExpected = (DWORD)-1;
		static const byte m_rankNameLengthExpected = 0;
		static const LPCTSTR m_rankNameExpected(_T(""));

		void AssertEqual(const CPlayerInfo &obj,
			LPCTSTR nameExpected,
			LPCTSTR responseTextExpected,
			const byte* cardNumberExpected,
			int cardNumberLengthExpected,
			DWORD multiplierExpected,
			DWORD dividerExpected,
			DWORD pointsExpected,
			DWORD countdownExpected,
			bool isBirthdayExpected,
			bool isAnniversaryExpected,
			bool bannedFromPlayExpected,
			DWORD compValueExpected,
			bool pbtAllowedExpected,
			DWORD dailyPointsExpected,
			bool maxCardLimitReachedExpected,
			DWORD pointsTillNextTierExpected,
			DWORD tierMultiplierExpected,
			DWORD rankIdExpected,
			LPCTSTR rankNameExpected,
			DWORD playerIdExpected,
			DWORD tierPointsExpected,
			DWORD countdownHighPrecisionExpected,
			AccountingType accountingTypeExpected,
			bool ebAvailableExpected,
			std::string ebJSONDataExpected,
			DWORD ebPointsIdExpected,
			DWORD ebPromosIdExpected,
			DWORD ebCompsIdExpected,
			DWORD ebTierPointsIdExpected,
			std::vector<ExtendedBucketsCountdownData> ebCountdownDataExpected,
			bool enrolledInWallet_2_0_Expected,
			int testIndex,
			LPCTSTR functionName,
			int lineNumber,
			const int* extraIndex = NULL)
		{
			ASSERT_EQ(obj.GetName(), nameExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetResponseText(), responseTextExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);

			int cardNumberLength(-1234567890);
			const  byte* cardNumber(obj.GetCardNumber(cardNumberLength));
			ASSERT_EQ(cardNumberLength, cardNumberLengthExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			CUnitTestingUtils::AssertEqual(cardNumber, cardNumberLength, cardNumberExpected, cardNumberLengthExpected, _T("cardNumber"), testIndex, __TFUNCTION__, __LINE__, NULL);

			ASSERT_EQ(obj.GetMultiplier(), multiplierExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetDivider(), dividerExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetPoints(), pointsExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetCountdown(), countdownExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.IsBirthdayToday(), isBirthdayExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.IsAnniversaryToday(), isAnniversaryExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.IsBannedFromPlay(), bannedFromPlayExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetCompValue(), compValueExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.IsPBTAllowed(), pbtAllowedExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetDailyPoints(), dailyPointsExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.IsMaxCardLimitReached(), maxCardLimitReachedExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetPointsTillNextTier(), pointsTillNextTierExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetTierMultiplier(), tierMultiplierExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetRankId(), rankIdExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetRankName(), rankNameExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetPlayerId(), playerIdExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetTierPoints(), tierPointsExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetCountdownHighPrecision(), countdownExpected * CPlayerInfo::COUNTDOWN_PRECISION_MULTIPLIER) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetAccountingType(), accountingTypeExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.IsExtendedBucketsAvailable(), ebAvailableExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetExtendedBucketsJSONData(), ebJSONDataExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetEBPointsId(), ebPointsIdExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetEBPromosId(), ebPromosIdExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetEBCompsId(), ebCompsIdExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
			ASSERT_EQ(obj.GetEBTierPointsId(), ebTierPointsIdExpected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);

			const std::vector<ExtendedBucketsCountdownData*> ebCountdownData = obj.GetExtendedBucketsCountdownData();
			ASSERT_EQ(ebCountdownData.size(), ebCountdownDataExpected.size());
			for (int i = 0; i < ebCountdownData.size(); i++)
			{
				ASSERT_EQ(ebCountdownData[i]->id, ebCountdownDataExpected[i].id) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);;
				ASSERT_EQ(ebCountdownData[i]->displayName, ebCountdownDataExpected[i].displayName) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);;
				ASSERT_EQ(ebCountdownData[i]->bucketCurrency, ebCountdownDataExpected[i].bucketCurrency) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);;
				ASSERT_EQ(ebCountdownData[i]->countdown, ebCountdownDataExpected[i].countdown) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);;
				ASSERT_EQ(ebCountdownData[i]->multiplier, ebCountdownDataExpected[i].multiplier) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);;
				ASSERT_EQ(ebCountdownData[i]->divider, ebCountdownDataExpected[i].divider) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);;
			}

			ASSERT_EQ(obj.IsEnrolledInWallet_2_0(), enrolledInWallet_2_0_Expected) << AssertMsg::FormatOnly(" testIndex=%d", (int)testIndex);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerInfo::CPlayerInfo(const PlayerResponse& playerResponse)
		TEST_F(PlayerInfoUnitTests, CPlayerInfo_PlayerResponseTest)
		{
			static const DWORD RESPONSE_PADDING_SIZE(4); // Bytes of padding on test input.

			typedef struct
			{
				// Input(s).
				const byte m_card_id[CARD_ID_LEN];
				const WORD m_multiplier;
				const WORD m_divider;
				const bool m_birthdayIndicator;
				const bool m_bannedFromPlayIndicator;
				const bool m_anniversaryIndicator;
				const DWORD m_totalPoints;
				const WORD m_countdown;
				const DWORD m_comps; // in cents
				const byte m_nameLength;
				const byte* const m_name;
				const byte m_respLength;
				const byte* const m_response;
				const WORD m_lossLimitBalance;
				const BYTE m_lossLimitStatus;
				const DWORD m_dailyPoints; //Tracker 026925
				const byte m_maxCardLimitIndicator; //Tracker 026984
				const DWORD m_pointsTillNextTier; //Tracker 026928
				const DWORD m_tierMultiplier; //SPS 7/14/2010 TR #26584
				const DWORD m_rankId;
				const byte m_rankNameLength;
				const byte* const m_rankName;
				const AccountingType m_accountingType;
				const bool m_bEBAvailable;
				const std::string m_ebJSONData;
				const DWORD m_ebPointsId;
				const DWORD m_ebPromosId;
				const DWORD m_ebCompsId;
				const DWORD m_ebTierPointsId;
				const std::vector<ExtendedBucketsCountdownData> m_ebCountdownData;
				const bool m_bEnrolledInWallet_2_0;

				// Expected result(s).
				LPCTSTR const m_nameExpected;
				LPCTSTR const m_responseTextExpected;
				const byte* const m_cardNumberExpected;
				const int m_cardNumberLengthExpected;
				const DWORD m_multiplierExpected;
				const DWORD m_dividerExpected;
				const DWORD m_pointsExpected;
				const DWORD m_countdownExpected;
				const bool m_isBirthdayExpected;
				const bool m_isAnniversaryExpected;
				const bool m_bannedFromPlayExpected;
				const DWORD m_compValueExpected;
				const bool m_pbtAllowedExpected;
				const DWORD m_dailyPointsExpected;
				const bool m_maxCardLimitReachedExpected;
				const DWORD m_pointsTillNextTierExpected;
				const DWORD m_tierMultiplierExpected;
				const DWORD m_rankIdExpected;
				LPCTSTR const m_rankNameExpected;
				const DWORD m_playerIdExpected;
				const DWORD m_tierPointsExpected;
                const DWORD m_countdownHighPrecisionExpected;
				const AccountingType m_accountingTypeExpected;
				const bool m_bEBAvailableExpected;
				const std::string m_ebJSONDataExpected;
				const DWORD m_ebPointsIdExpected;
				const DWORD m_ebPromosIdExpected;
				const DWORD m_ebCompsIdExpected;
				const DWORD m_ebTierPointsIdExpected;
				const std::vector<ExtendedBucketsCountdownData> m_ebCountdownDataExpected;
				const bool m_bEnrolledInWallet_2_0_Expected;
			} TestDataType;

			const byte card0[CARD_ID_LEN] = { 1, 2, 3, 4, 5 };

			// Test each permuation of boolean results.  Test other values with unique values.
			// Test both NULL and non-NULL string pointers.
			const TestDataType TEST_DATA[] =
			{
				{ { 1, 2, 3, 4, 5 }, 6, 7, false, false, false, 8, 9, 10, 0, (byte*)NULL, 0, (byte*)NULL, 13, 14, 15, 0, 17, 18, 19, 0, (byte*)NULL, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, false,
				_T(""), _T(""), card0, sizeof(card0), 6, 7, 8, 9, false, false, false, 10, true, 15, false, 17, 18, 19, _T(""), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, false
				},
				{ { 1, 2, 3, 4, 5 }, 6, 7, false, false, false, 8, 9, 10, 11, (byte*)"abcdefghijk", 12, (byte*)"123456789abc", 13, 14, 15, 16, 17, 18, 19, 20, (byte*)"ghijkLmnopqrstuvwxyz", AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, false,
				_T("abcdefghijk"), _T("123456789abc"), card0, sizeof(card0), 6, 7, 8, 9, false, false, false, 10, true, 15, true, 17, 18, 19, _T("ghijkLmnopqrstuvwxyz"), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, false
				},
				{ { 1, 2, 3, 4, 5 }, 6, 7, false, false, true, 8, 9, 10, 11, (byte*)"abcdefghijk", 12, (byte*)"123456789abc", 13, 14, 15, 0, 17, 18, 19, 20, (byte*)"ghijkLmnopqrstuvwxyz", AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, false,
				_T("abcdefghijk"), _T("123456789abc"), card0, sizeof(card0), 6, 7, 8, 9, false, true, false, 10, true, 15, false, 17, 18, 19, _T("ghijkLmnopqrstuvwxyz"), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, false
				},
				{ { 1, 2, 3, 4, 5 }, 6, 7, false, false, true, 8, 9, 10, 11, (byte*)"abcdefghijk", 12, (byte*)"123456789abc", 13, 14, 15, 1, 17, 18, 19, 20, (byte*)"ghijkLmnopqrstuvwxyz", AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, false,
				_T("abcdefghijk"), _T("123456789abc"), card0, sizeof(card0), 6, 7, 8, 9, false, true, false, 10, true, 15, true, 17, 18, 19, _T("ghijkLmnopqrstuvwxyz"), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, false
				},

				{ { 1, 2, 3, 4, 5 }, 6, 7, false, true, false, 8, 9, 10, 0, (byte*)NULL, 0, (byte*)NULL, 13, 14, 15, 0, 17, 18, 19, 0, (byte*)NULL, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, true,
				_T(""), _T(""), card0, sizeof(card0), 6, 7, 8, 9, false, false, true, 10, true, 15, false, 17, 18, 19, _T(""), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, true
				},
				{ { 1, 2, 3, 4, 5 }, 6, 7, false, true, false, 8, 9, 10, 0, (byte*)NULL, 12, (byte*)"123456789abc", 13, 14, 15, 254, 17, 18, 19, 20, (byte*)"ghijkLmnopqrstuvwxyz", AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, true,
				_T(""), _T("123456789abc"), card0, sizeof(card0), 6, 7, 8, 9, false, false, true, 10, true, 15, true, 17, 18, 19, _T("ghijkLmnopqrstuvwxyz"), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, true
				},
				{ { 1, 2, 3, 4, 5 }, 6, 7, false, true, true, 8, 9, 10, 11, (byte*)"abcdefghijk", 0, (byte*)NULL, 13, 14, 15, 0, 17, 18, 19, 20, (byte*)"ghijkLmnopqrstuvwxyz", AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, true,
				_T("abcdefghijk"), _T(""), card0, sizeof(card0), 6, 7, 8, 9, false, true, true, 10, true, 15, false, 17, 18, 19, _T("ghijkLmnopqrstuvwxyz"), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, true
				},
				{ { 1, 2, 3, 4, 5 }, 6, 7, false, true, true, 8, 9, 10, 11, (byte*)"abcdefghijk", 12, (byte*)"123456789abc", 13, 14, 15, 255, 17, 18, 19, 0, (byte*)NULL, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, true,
				_T("abcdefghijk"), _T("123456789abc"), card0, sizeof(card0), 6, 7, 8, 9, false, true, true, 10, true, 15, true, 17, 18, 19, _T(""), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, true
				},

				{ { 1, 2, 3, 4, 5 }, 6, 7, true, false, false, 8, 9, 10, 0, (byte*)NULL, 0, (byte*)NULL, 13, 14, 15, 0, 17, 18, 19, 0, (byte*)NULL, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, false,
				_T(""), _T(""), card0, sizeof(card0), 6, 7, 8, 9, true, false, false, 10, true, 15, false, 17, 18, 19, _T(""), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, false
				},
				{ { 1, 2, 3, 4, 5 }, 6, 7, true, false, false, 8, 9, 10, 11, (byte*)"abcdefghijk", 12, (byte*)"123456789abc", 13, 14, 15, 16, 17, 18, 19, 20, (byte*)"ghijkLmnopqrstuvwxyz", AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, false,
				_T("abcdefghijk"), _T("123456789abc"), card0, sizeof(card0), 6, 7, 8, 9, true, false, false, 10, true, 15, true, 17, 18, 19, _T("ghijkLmnopqrstuvwxyz"), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, false
				},
				{ { 1, 2, 3, 4, 5 }, 6, 7, true, false, true, 8, 9, 10, 11, (byte*)"abcdefghijk", 12, (byte*)"123456789abc", 13, 14, 15, 0, 17, 18, 19, 20, (byte*)"ghijkLmnopqrstuvwxyz", AccountingType::ACCOUNTING_COINS_IN, 0, "", 11, 22, 33, 44, {{101, "bucket1", "$", 100, 10, 100}}, false,
				_T("abcdefghijk"), _T("123456789abc"), card0, sizeof(card0), 6, 7, 8, 9, true, true, false, 10, true, 15, false, 17, 18, 19, _T("ghijkLmnopqrstuvwxyz"), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_COINS_IN, 0, "", 11, 22, 33, 44, {{101, "bucket1", "$", 100, 10, 100}}, false
				},
				{ { 1, 2, 3, 4, 5 }, 6, 7, true, false, true, 8, 9, 10, 11, (byte*)"abcdefghijk", 12, (byte*)"123456789abc", 13, 14, 15, 1, 17, 18, 19, 20, (byte*)"ghijkLmnopqrstuvwxyz", AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, true,
				_T("abcdefghijk"), _T("123456789abc"), card0, sizeof(card0), 6, 7, 8, 9, true, true, false, 10, true, 15, true, 17, 18, 19, _T("ghijkLmnopqrstuvwxyz"), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, true
				},

				{ { 1, 2, 3, 4, 5 }, 6, 7, true, true, false, 8, 9, 10, 0, (byte*)NULL, 0, (byte*)NULL, 13, 14, 15, 0, 17, 18, 19, 0, (byte*)NULL, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, true,
				_T(""), _T(""), card0, sizeof(card0), 6, 7, 8, 9, true, false, true, 10, true, 15, false, 17, 18, 19, _T(""), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_COINS_IN, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, true
				},
				{ { 1, 2, 3, 4, 5 }, 6, 7, true, true, false, 8, 9, 10, 11, (byte*)"abcdefghijk", 12, (byte*)"123456789abc", 13, 14, 15, 254, 17, 18, 19, 20, (byte*)"ghijkLmnopqrstuvwxyz", AccountingType::ACCOUNTING_CASHABLE_PLAYED, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, true,
				_T("abcdefghijk"), _T("123456789abc"), card0, sizeof(card0), 6, 7, 8, 9, true, false, true, 10, true, 15, true, 17, 18, 19, _T("ghijkLmnopqrstuvwxyz"), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_CASHABLE_PLAYED, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, true
				},
				{ { 1, 2, 3, 4, 5 }, 6, 7, true, true, true, 8, 9, 10, 11, (byte*)"abcdefghijk", 12, (byte*)"123456789abc", 13, 14, 15, 0, 17, 18, 19, 20, (byte*)"ghijkLmnopqrstuvwxyz", AccountingType::ACCOUNTING_COINS_OUT, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100},{102, "bucket2", "", 200, 20, 200},{103, "", "", 150, 15, 150}}, true,
				_T("abcdefghijk"), _T("123456789abc"), card0, sizeof(card0), 6, 7, 8, 9, true, true, true, 10, true, 15, false, 17, 18, 19, _T("ghijkLmnopqrstuvwxyz"), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_COINS_OUT, 0, "", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100},{102, "bucket2", "", 200, 20, 200},{103, "", "", 150, 15, 150}}, true
				},
				{ { 1, 2, 3, 4, 5 }, 6, 7, true, true, true, 8, 9, 10, 11, (byte*)"abcdefghijk", 12, (byte*)"123456789abc", 13, 14, 15, 255, 17, 18, 19, 20, (byte*)"ghijkLmnopqrstuvwxyz", AccountingType::ACCOUNTING_COINS_IN, 1, "some JSON data", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, false,
				_T("abcdefghijk"), _T("123456789abc"), card0, sizeof(card0), 6, 7, 8, 9, true, true, true, 10, true, 15, true, 17, 18, 19, _T("ghijkLmnopqrstuvwxyz"), 20, 21, (DWORD)-1, AccountingType::ACCOUNTING_COINS_IN, 1, "some JSON data", 10, 20, 30, 40, {{101, "bucket1", "$", 100, 10, 100}}, false 
				},
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				PlayerResponse playerResponse;

				memcpy_s(playerResponse.card_id, sizeof(playerResponse.card_id), TEST_DATA[testIndex].m_card_id, sizeof(TEST_DATA[testIndex].m_card_id));
				playerResponse.multiplier = TEST_DATA[testIndex].m_multiplier;
				playerResponse.divider = TEST_DATA[testIndex].m_divider;
				playerResponse.birthdayIndicator = TEST_DATA[testIndex].m_birthdayIndicator;
				playerResponse.bannedFromPlayIndicator = TEST_DATA[testIndex].m_bannedFromPlayIndicator;
				playerResponse.anniversaryIndicator = TEST_DATA[testIndex].m_anniversaryIndicator;
				playerResponse.totalPoints = TEST_DATA[testIndex].m_totalPoints;
				playerResponse.countdown = TEST_DATA[testIndex].m_countdown;
				playerResponse.comps = TEST_DATA[testIndex].m_comps;
				playerResponse.nameLength = TEST_DATA[testIndex].m_nameLength;
				playerResponse.name = (byte*)CUnitTestingUtils::NewMemDup(TEST_DATA[testIndex].m_name, TEST_DATA[testIndex].m_nameLength + 1);
				playerResponse.respLength = TEST_DATA[testIndex].m_respLength;
				playerResponse.response = (byte*)CUnitTestingUtils::NewMemDup(TEST_DATA[testIndex].m_response, TEST_DATA[testIndex].m_respLength + 1);
				playerResponse.lossLimitBalance = TEST_DATA[testIndex].m_lossLimitBalance;
				playerResponse.lossLimitStatus = TEST_DATA[testIndex].m_lossLimitStatus;
				playerResponse.dailyPoints = TEST_DATA[testIndex].m_dailyPoints;
				playerResponse.maxCardLimitIndicator = TEST_DATA[testIndex].m_maxCardLimitIndicator;
				playerResponse.pointsTillNextTier = TEST_DATA[testIndex].m_pointsTillNextTier;
				playerResponse.tierMultiplier = TEST_DATA[testIndex].m_tierMultiplier;
				playerResponse.m_rankId = TEST_DATA[testIndex].m_rankId;
				playerResponse.m_rankNameLength = TEST_DATA[testIndex].m_rankNameLength;
				playerResponse.m_rankName = (byte*)CUnitTestingUtils::NewMemDup(TEST_DATA[testIndex].m_rankName, TEST_DATA[testIndex].m_rankNameLength + 1);
				playerResponse.m_playerId = TEST_DATA[testIndex].m_playerIdExpected;
				playerResponse.m_tierPoints = TEST_DATA[testIndex].m_tierPointsExpected;
                playerResponse.countdownHighPrecision = TEST_DATA[testIndex].m_countdownHighPrecisionExpected;
                playerResponse.accountingType = TEST_DATA[testIndex].m_accountingType;
                playerResponse.ebIndicator = TEST_DATA[testIndex].m_bEBAvailable;
                playerResponse.ebJSONData = TEST_DATA[testIndex].m_ebJSONData;
                playerResponse.ebPointsId = TEST_DATA[testIndex].m_ebPointsId;
                playerResponse.ebPromosId = TEST_DATA[testIndex].m_ebPromosId;
                playerResponse.ebCompsId = TEST_DATA[testIndex].m_ebCompsId;
                playerResponse.ebTierPointsId = TEST_DATA[testIndex].m_ebTierPointsId;
				for (int i = 0; i < TEST_DATA[testIndex].m_ebCountdownData.size(); i++)
				{
					ExtendedBucketsCountdownData *countdownData = new ExtendedBucketsCountdownData(TEST_DATA[testIndex].m_ebCountdownData[i]);
					playerResponse.ebCountdowns.push_back(countdownData);
				}
				playerResponse.enrolledInWallet_2_0 = TEST_DATA[testIndex].m_bEnrolledInWallet_2_0;

				
				// Perform operation(s) to be tested.
				const CPlayerInfo obj(playerResponse);
				AssertEqual(obj,
					TEST_DATA[testIndex].m_nameExpected,
					TEST_DATA[testIndex].m_responseTextExpected,
					TEST_DATA[testIndex].m_cardNumberExpected,
					TEST_DATA[testIndex].m_cardNumberLengthExpected,
					TEST_DATA[testIndex].m_multiplierExpected,
					TEST_DATA[testIndex].m_dividerExpected,
					TEST_DATA[testIndex].m_pointsExpected,
					TEST_DATA[testIndex].m_countdownExpected,
					TEST_DATA[testIndex].m_isBirthdayExpected,
					TEST_DATA[testIndex].m_isAnniversaryExpected,
					TEST_DATA[testIndex].m_bannedFromPlayExpected,
					TEST_DATA[testIndex].m_compValueExpected,
					TEST_DATA[testIndex].m_pbtAllowedExpected,
					TEST_DATA[testIndex].m_dailyPointsExpected,
					TEST_DATA[testIndex].m_maxCardLimitReachedExpected,
					TEST_DATA[testIndex].m_pointsTillNextTierExpected,
					TEST_DATA[testIndex].m_tierMultiplierExpected,
					TEST_DATA[testIndex].m_rankIdExpected,
					TEST_DATA[testIndex].m_rankNameExpected,
					TEST_DATA[testIndex].m_playerIdExpected,
					TEST_DATA[testIndex].m_tierPointsExpected,
                    TEST_DATA[testIndex].m_countdownHighPrecisionExpected,
                    TEST_DATA[testIndex].m_accountingTypeExpected,
                    TEST_DATA[testIndex].m_bEBAvailableExpected,
                    TEST_DATA[testIndex].m_ebJSONDataExpected,
                    TEST_DATA[testIndex].m_ebPointsIdExpected,
                    TEST_DATA[testIndex].m_ebPromosIdExpected,
                    TEST_DATA[testIndex].m_ebCompsIdExpected,
                    TEST_DATA[testIndex].m_ebTierPointsIdExpected,
                    TEST_DATA[testIndex].m_ebCountdownDataExpected,
                    TEST_DATA[testIndex].m_bEnrolledInWallet_2_0_Expected,
					testIndex, __TFUNCTION__, __LINE__, NULL);

				// Cleanup.
				delete[] playerResponse.name;
				delete[] playerResponse.response;
				delete[] playerResponse.m_rankName;
				for (int i = 0; i < playerResponse.ebCountdowns.size(); i++)
				{
					delete playerResponse.ebCountdowns[i];
				}
			}
		}
}