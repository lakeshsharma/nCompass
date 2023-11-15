#include "stdafx.h"
#include "PlayerSession.h"

// Sentinel includes.
#include "Config/ConfigExtData.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingUtils.h"

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, TestIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, #result)

// AssertResult macro for calling CUnitTestingUtils (with an extra index) from a test method.
#define AssertResult3x(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, TestIndex, __TFUNCTION__, __LINE__, &extraIndex);
#define AssertResult2x(result, expected) AssertResult3x(result, expected, #result)


#include "gtest/gtest.h"

static const time_t  CTIME_DEFAULT_VALUE(0); // Arbitrary value when any valid value is okay.

const DWORD NVRAM_DATA_OFFSET = CPlayerSession::NVRAM_DATA_OFFSET;
const DWORD NVRAM_DATA_SIZE = CPlayerSession::NVRAM_DATA_SIZE;
const DWORD NVRAM_ACTIVE_OFFSET = CPlayerSession::NVRAM_ACTIVE_OFFSET;
const BYTE NVRAM_ACTIVE_VALUE = CPlayerSession::NVRAM_ACTIVE_VALUE;
const bool EGM_SENDS_GAME_END_DEFAULT = CPlayerSession::EGM_SENDS_GAME_END_DEFAULT;
const bool EGM_SENDS_GAME_START_DEFAULT = CPlayerSession::EGM_SENDS_GAME_START_DEFAULT;

namespace SentinelUnitTests
{
		// Default expected result(s).
		static const int m_sessionIdDefault = SESSION_ID_DEFAULT;
		static const bool m_gameInProgressExpected = false;
        static const SYSTEMTIME m_lastGameStartedExpected = CUnitTestingUtils::JAN1970_SYSTEMTIME;
		static const bool m_egmSendsGameEndDefault = false;
        static const bool m_egmSendsGameStartDefault = false;
		static const DWORD m_negligibleCreditsThreshold = 0;

		static const int m_lastGameNumberDefault = -1;
		static const long m_lastCoinsPlayedDefault = -1;
		static const int m_lastGameSessionGameNumberDefault = -1;
		static const int m_lastDenomDefault = -1;
		static const int m_lastGameSessionGameDenomDefault = -1;

		// Class member variable(s).
		int TestIndex;
        class PlayerSessionUnitTests : public ::testing::Test
        {
        protected:
        	virtual void SetUp()
        	{
                // Test method initialization code.
                TestIndex = 0;
        	}

        	virtual void TearDown()
        	{
        	}
        public:
             bool GetminiVirtualSessionEnabled(CPlayerSession& obj)
             {
                 return obj.m_miniVirtualSessionEnabled;
             }
             bool GetofflineCoinIn(CPlayerSession& obj)
             {
                 return obj.m_offlineCoinIn;
             }
             string GetPlayerTypeTest(CPlayerSession& obj)
             {
                 return obj.GetPlayerType();
             }
			 CSentinelDlgState* GetSentinelDlgState(CPlayerSession& obj)
			 {
				 return obj.m_uiState;
			 }
			 bool GetJackpotPending(CPlayerSession& obj)
			 {
				 return obj.m_jackpotPending;
			 }
		};

		TEST_F(PlayerSessionUnitTests, PlayerSession_StaticConst)
		{
			// Test NVRAM memory map for backwards and forwards memory compatibility.
			ASSERT_EQ((DWORD)713482, NVRAM_PLAYER_SESSION2_CHECKBYTE_OFFSET) << "NVRAM_PLAYER_SESSION2_CHECKBYTE_OFFSET";
			ASSERT_EQ((DWORD)1, NVRAM_PLAYER_SESSION2_CHECKBYTE_SIZE) << "NVRAM_PLAYER_SESSION2_CHECKBYTE_SIZE";
			ASSERT_EQ((BYTE)0x1d, NVRAM_PLAYER_SESSION2_CHECKBYTE_VALUE) << "NVRAM_PLAYER_SESSION2_CHECKBYTE_VERSION";
			ASSERT_EQ((DWORD)713483, NVRAM_PLAYER_SESSION2_DATA_OFFSET) << "NVRAM_PLAYER_SESSION2_OFFSET";
			ASSERT_EQ((DWORD)256, NVRAM_PLAYER_SESSION2_DATA_SIZE) << "NVRAM_PLAYER_SESSION2_DATA_SIZE";

			// Test NVRAM-related class members.
			ASSERT_EQ(NVRAM_PLAYER_SESSION2_DATA_OFFSET, NVRAM_DATA_OFFSET) << "NVRAM_DATA_OFFSET";
			ASSERT_EQ(NVRAM_PLAYER_SESSION2_DATA_SIZE, NVRAM_DATA_SIZE) << "NVRAM_DATA_SIZE";
			ASSERT_EQ(NVRAM_PLAYER_SESSION2_CHECKBYTE_OFFSET, NVRAM_ACTIVE_OFFSET) << "NVRAM_ACTIVE_OFFSET";
			ASSERT_EQ(NVRAM_PLAYER_SESSION2_CHECKBYTE_VALUE, NVRAM_ACTIVE_VALUE) << "NVRAM_ACTIVE_VALUE";

			ASSERT_EQ((int)0, SESSION_ID_DEFAULT) << "SESSION_ID_DEFAULT";
		}

		TEST_F(PlayerSessionUnitTests, SetUiStateObject)
		{
			// setup
			CPlayerSession obj;
			CSentinelDlgState uiState;

			// perform operation & test
			obj.SetUiStateObject(&uiState);
			CSentinelDlgState *uiState1 = GetSentinelDlgState(obj);

			ASSERT_TRUE(uiState1 == &uiState);
		}

		TEST_F(PlayerSessionUnitTests, PlayerSessionSetCredits)
		{
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			obj.SetCreditsInPennies(0, sessionActiveInfoChanged, creditsIncreased);

			// Set to non-zero
			ASSERT_TRUE(obj.SetCreditsInPennies(10, sessionActiveInfoChanged, creditsIncreased));
			ASSERT_TRUE(creditsIncreased);

			// Set to non-zero again
			creditsIncreased = false;
			ASSERT_FALSE(obj.SetCreditsInPennies(11, sessionActiveInfoChanged, creditsIncreased));
			ASSERT_TRUE(creditsIncreased);

			// Set to zero
			creditsIncreased = false;
			ASSERT_TRUE(obj.SetCreditsInPennies(0, sessionActiveInfoChanged, creditsIncreased));
			ASSERT_FALSE(creditsIncreased);

			// Set to zero again
			creditsIncreased = false;
			ASSERT_FALSE(obj.SetCreditsInPennies(0, sessionActiveInfoChanged, creditsIncreased));
			ASSERT_FALSE(creditsIncreased);

			// Set to nonzero
			creditsIncreased = false;
			ASSERT_TRUE(obj.SetCreditsInPennies(10, sessionActiveInfoChanged, creditsIncreased));
			ASSERT_TRUE(creditsIncreased);
		}

		TEST_F(PlayerSessionUnitTests, PlayerSessionHasSignificantCredits)
		{
			CPlayerSession obj;
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			obj.SetCreditsInPennies(0, sessionActiveInfoChanged, creditsIncreased);

			ASSERT_FALSE(obj.HasSignificantCredits());

			// set non zero credits
			obj.SetCreditsInPennies(10, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_TRUE(obj.HasSignificantCredits());
		}

		TEST_F(PlayerSessionUnitTests, PlayerSessionTranslateInsertPlayer)
		{
			CPlayerSession obj;
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			// Setup.
			// global card
			CCard *gcard(new CCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE));
			gcard->SetCardID("6909FF0001");
			gcard->SetCardType(CARD_GLOBAL);

			// player card
			CCard *pcard(new CCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE));
			pcard->SetCardID("245B22968F");
			pcard->SetCardType(CARD_PLAYER);

			CPlayerInfo *playerInfo(new CPlayerInfo);
            CGames *games = new CGames();
			CPlayer *player = new CPlayer(pcard, playerInfo, config, egmConfig, CTIME_DEFAULT_VALUE, false, games);
			CEmployee *employee = new CEmployee(gcard, config, false, false, 0, 0);

			// second player card
			CCard *scard(new CCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE));
			scard->SetCardID("A5740008E0");
			scard->SetCardType(CARD_PLAYER);

			// No player, no employee
			obj.SetCreditsInPennies(0, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_TRUE(obj.TransformInsertedCard(true, NULL, NULL, pcard) == Inserted);
			ASSERT_TRUE(obj.TransformInsertedCard(false, NULL, NULL, pcard) == Inserted);
			obj.SetCreditsInPennies(10, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_TRUE(obj.TransformInsertedCard(true, NULL, NULL, pcard) == Inserted);
			ASSERT_TRUE(obj.TransformInsertedCard(false, NULL, NULL, pcard) == Inserted);

			// No player, with employee
			obj.SetCreditsInPennies(0, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_TRUE(obj.TransformInsertedCard(true, employee, NULL, pcard) == Ignored);
			ASSERT_TRUE(obj.TransformInsertedCard(false, employee, NULL, pcard) == Ignored);
			obj.SetCreditsInPennies(10, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_TRUE(obj.TransformInsertedCard(true, employee, NULL, pcard) == Ignored);
			ASSERT_TRUE(obj.TransformInsertedCard(false, employee, NULL, pcard) == Ignored);

			// With non matching player
			obj.SetCreditsInPennies(10, sessionActiveInfoChanged, creditsIncreased); // With credits
			ASSERT_TRUE(obj.TransformInsertedCard(true, NULL, player, scard) == Ignored);
			ASSERT_TRUE(obj.TransformInsertedCard(false, NULL, player, scard) == Ignored);
			obj.SetCreditsInPennies(0, sessionActiveInfoChanged, creditsIncreased); // No credits
			ASSERT_TRUE(obj.TransformInsertedCard(true, NULL, player, scard) == RemovedInserted);
			ASSERT_TRUE(obj.TransformInsertedCard(false, NULL, player, scard) == RemovedInserted);

			// With matching player
			obj.SetCreditsInPennies(10, sessionActiveInfoChanged, creditsIncreased); // With credits
			ASSERT_TRUE(obj.TransformInsertedCard(true, NULL, player, pcard) == StayCarded);
			ASSERT_TRUE(obj.TransformInsertedCard(false, NULL, player, pcard) == Removed);
			obj.SetCreditsInPennies(0, sessionActiveInfoChanged, creditsIncreased); // No credits
			ASSERT_TRUE(obj.TransformInsertedCard(true, NULL, player, pcard) == StayCarded);
			ASSERT_TRUE(obj.TransformInsertedCard(false, NULL, player, pcard) == Removed);

			delete employee;
			delete player;
			delete scard;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::TransformInsertedCard()
		/// Test card inserted with a different player already present and no employee present.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, PlayerSessionTranslateInsertPlayer_DifferentPlayer)
		{
			const bool rfid(true);

			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			// Setup.
			// player card
			CCard *pcard(new CCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE));
			pcard->SetCardID("245B22968F");
			pcard->SetCardType(CARD_PLAYER);

			CPlayerInfo *playerInfo(new CPlayerInfo);
            CGames *games = new CGames();
			CPlayer *player = new CPlayer(pcard, playerInfo, config, egmConfig, CTIME_DEFAULT_VALUE, false, games);
			CEmployee *employee(nullptr);

			// second player card
			CCard *scard(new CCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE));
			scard->SetCardID("A5740008E0");
			scard->SetCardType(CARD_PLAYER);

			// Perform operation(s) and test(s).
			// RFID, Carded (non-virtual) player, insignificant credits, virtual sessions disabled, mini-virtual sessions disabled, no game in progress. 
			obj.SetCreditsInPennies(0, sessionActiveInfoChanged, creditsIncreased);
			config.SetNegligibleCreditsThreshold(5);
			config.SetVirtualSessionEnabled(false);
			config.SetMiniVirtualSessionEnabled(false);
			config.SetGameInProgress(false);
			obj.UpdateConfigItems(config);
			ASSERT_EQ(RemovedInserted, obj.TransformInsertedCard(rfid, employee, player, scard));

			// RFID, Carded (non-virtual) player, insignificant credits, virtual sessions disabled, mini-virtual sessions enabled, no game in progress. 
			obj.SetCreditsInPennies(0, sessionActiveInfoChanged, creditsIncreased);
			config.SetNegligibleCreditsThreshold(5);
			config.SetVirtualSessionEnabled(false);
			config.SetMiniVirtualSessionEnabled(true);
			config.SetGameInProgress(false);
			obj.UpdateConfigItems(config);
			ASSERT_EQ(RemovedInserted, obj.TransformInsertedCard(rfid, employee, player, scard));

			// RFID, Carded (non-virtual) player, insignificant credits, virtual sessions disabled, mini-virtual sessions disabled, game in progress. 
			obj.SetCreditsInPennies(0, sessionActiveInfoChanged, creditsIncreased);
			config.SetNegligibleCreditsThreshold(5);
			config.SetVirtualSessionEnabled(false);
			config.SetMiniVirtualSessionEnabled(false);
			config.SetGameInProgress(true);
			obj.UpdateConfigItems(config);
			ASSERT_EQ(RemovedInserted, obj.TransformInsertedCard(rfid, employee, player, scard));

			// RFID, Carded (non-virtual) player, insignificant credits, virtual sessions disabled, mini-virtual sessions enabled, game in progress. 
			obj.SetCreditsInPennies(0, sessionActiveInfoChanged, creditsIncreased);
			config.SetNegligibleCreditsThreshold(5);
			config.SetVirtualSessionEnabled(false);
			config.SetMiniVirtualSessionEnabled(true);
			config.SetGameInProgress(true);
			obj.UpdateConfigItems(config);
			ASSERT_EQ(Ignored, obj.TransformInsertedCard(rfid, employee, player, scard));

			// Perform operation(s) and test(s).
			// RFID, Carded (non-virtual) player, insignificant credits, virtual sessions enabled, no game in progress. 
			obj.SetCreditsInPennies(0, sessionActiveInfoChanged, creditsIncreased);
			config.SetNegligibleCreditsThreshold(5);
			config.SetVirtualSessionEnabled(true);
			config.SetGameInProgress(false);
			obj.UpdateConfigItems(config);
			ASSERT_EQ(RemovedInserted, obj.TransformInsertedCard(rfid, employee, player, scard));

			// RFID, Carded (non-virtual) player, insignificant credits, virtual sessions enabled, game in progress. 
			obj.SetCreditsInPennies(0, sessionActiveInfoChanged, creditsIncreased);
			config.SetNegligibleCreditsThreshold(5);
			config.SetVirtualSessionEnabled(true);
			config.SetGameInProgress(true);
			obj.UpdateConfigItems(config);
			ASSERT_EQ(RemovedInserted, obj.TransformInsertedCard(rfid, employee, player, scard));

			// Perform operation(s) and test(s).
			// RFID, Carded (non-virtual) player, significant credits, virtual sessions disabled, mini-virtual sessions disabled, no game in progress. 
			obj.SetCreditsInPennies(10, sessionActiveInfoChanged, creditsIncreased);
			config.SetNegligibleCreditsThreshold(5);
			config.SetVirtualSessionEnabled(false);
			config.SetMiniVirtualSessionEnabled(false);
			config.SetGameInProgress(false);
			obj.UpdateConfigItems(config);
			ASSERT_EQ(Ignored, obj.TransformInsertedCard(rfid, employee, player, scard));

			// Perform operation(s) and test(s).
			// RFID, Carded (non-virtual) player, significant credits, virtual sessions disabled, mini-virtual sessions enabled, no game in progress. 
			obj.SetCreditsInPennies(10, sessionActiveInfoChanged, creditsIncreased);
			config.SetNegligibleCreditsThreshold(5);
			config.SetVirtualSessionEnabled(false);
			config.SetMiniVirtualSessionEnabled(true);
			config.SetGameInProgress(false);
			obj.UpdateConfigItems(config);
			ASSERT_EQ(Ignored, obj.TransformInsertedCard(rfid, employee, player, scard));

			// RFID, Carded (non-virtual) player, significant credits, virtual sessions disabled, mini-virtual sessions disabled, game in progress. 
			obj.SetCreditsInPennies(10, sessionActiveInfoChanged, creditsIncreased);
			config.SetNegligibleCreditsThreshold(5);
			config.SetVirtualSessionEnabled(false);
			config.SetMiniVirtualSessionEnabled(false);
			config.SetGameInProgress(true);
			obj.UpdateConfigItems(config);
			ASSERT_EQ(Ignored, obj.TransformInsertedCard(rfid, employee, player, scard));

			// RFID, Carded (non-virtual) player, significant credits, virtual sessions disabled, mini-virtual sessions enabled, game in progress. 
			obj.SetCreditsInPennies(10, sessionActiveInfoChanged, creditsIncreased);
			config.SetNegligibleCreditsThreshold(5);
			config.SetVirtualSessionEnabled(false);
			config.SetMiniVirtualSessionEnabled(true);
			config.SetGameInProgress(true);
			obj.UpdateConfigItems(config);
			ASSERT_EQ(Ignored, obj.TransformInsertedCard(rfid, employee, player, scard));

			// Perform operation(s) and test(s).
			// RFID, Carded (non-virtual) player, significant credits, virtual sessions enabled, no game in progress. 
			obj.SetCreditsInPennies(10, sessionActiveInfoChanged, creditsIncreased);
			config.SetNegligibleCreditsThreshold(5);
			config.SetVirtualSessionEnabled(true);
			config.SetGameInProgress(false);
			obj.UpdateConfigItems(config);
			ASSERT_EQ(Ignored, obj.TransformInsertedCard(rfid, employee, player, scard));

			// RFID, Carded (non-virtual) player, significant credits, virtual sessions enabled, game in progress. 
			obj.SetCreditsInPennies(10, sessionActiveInfoChanged, creditsIncreased);
			config.SetNegligibleCreditsThreshold(5);
			config.SetVirtualSessionEnabled(true);
			config.SetGameInProgress(true);
			obj.UpdateConfigItems(config);
			ASSERT_EQ(Ignored, obj.TransformInsertedCard(rfid, employee, player, scard));

			delete employee;
			delete player;
			delete scard;
		}

		TEST_F(PlayerSessionUnitTests, PlayerSessionTranslateInsertEmployee)
		{
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			// Setup.
			// global card
			CCard *gcard(new CCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE));
			gcard->SetCardID("6909FF0001");
			gcard->SetCardType(CARD_GLOBAL);

			// player card
			CCard *pcard(new CCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE));
			pcard->SetCardID("245B22968F");
			pcard->SetCardType(CARD_PLAYER);

			CPlayerInfo *playerInfo(new CPlayerInfo);
            CGames *games = new CGames();
			CPlayer *player = new CPlayer(pcard, playerInfo, config, egmConfig, CTIME_DEFAULT_VALUE, false, games);
			CEmployee *employee = new CEmployee(gcard, config, false, false, 0, 0);

			// second employee card
			CCard *scard(new CCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE));
			scard->SetCardID("A5740008E0");
			scard->SetCardType(CARD_MECHANIC);

			// No player, no employee
			obj.SetCreditsInPennies(0, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_TRUE(obj.TransformInsertedCard(true, NULL, NULL, gcard) == Inserted);
			ASSERT_TRUE(obj.TransformInsertedCard(false, NULL, NULL, gcard) == Inserted);

			// With player
			ASSERT_TRUE(obj.TransformInsertedCard(true, NULL, player, gcard) == RemovedInserted);
			ASSERT_TRUE(obj.TransformInsertedCard(false, NULL, player, gcard) == RemovedInserted);

			// With non matching employee
			ASSERT_TRUE(obj.TransformInsertedCard(true, employee, NULL, scard) == RemovedInserted);
			ASSERT_TRUE(obj.TransformInsertedCard(false, employee, NULL, scard) == RemovedInserted);

			// With matching employee
			ASSERT_TRUE(obj.TransformInsertedCard(true, employee, NULL, gcard) == RemovedInserted);
			ASSERT_TRUE(obj.TransformInsertedCard(false, employee, NULL, gcard) == Removed);


			delete employee;
			delete player;
			delete scard;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::TransformInsertedCard()
		/// Test different player card inserted with a virtual player session.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, VirtualPlayerSessionTranslateInsertPlayerDifferentCard)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CCard pcard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
			pcard.SetCardID("1234567890");
			pcard.SetCardType(CARD_PLAYER);
            CGames games;
			CPlayer player(new CCard(pcard), NULL, config, egmConfig, CTIME_DEFAULT_VALUE, false, &games);
			CCard scard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
			scard.SetCardType(CARD_PLAYER);

			// Test.
			player.SetVirtualFlag(true);
			ASSERT_TRUE(obj.TransformInsertedCard(true, NULL, &player, &scard) == Inserted);
			ASSERT_TRUE(obj.TransformInsertedCard(false, NULL, &player, &scard) == Ignored);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::TransformInsertedCard()
		/// Test with the same player card inserted with a virtual player session.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, VirtualPlayerSessionTranslateInsertPlayerSameCard)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CCard pcard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
			pcard.SetCardID("1234567890");
			pcard.SetCardType(CARD_PLAYER);
            CGames games;
			CPlayer player(new CCard(pcard), NULL, config, egmConfig, CTIME_DEFAULT_VALUE, false, &games);
			CCard scard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
			scard.SetCardID("1234567890");
			scard.SetCardType(CARD_TEMP);

			// Test.
			player.SetVirtualFlag(true);
			ASSERT_TRUE(obj.TransformInsertedCard(true, NULL, &player, &scard) == Inserted);
			ASSERT_TRUE(obj.TransformInsertedCard(false, NULL, &player, &scard) == Inserted);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::TransformInsertedCard()
		/// Test with the same player card inserted with a virtual player session and an employee session in progress.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, VirtualPlayerSessionEmployeeSessionTranslateInsertPlayerSameCard)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CCard pcard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
			pcard.SetCardID("1234567890");
			pcard.SetCardType(CARD_PLAYER);
            CGames games;
			CPlayer player(new CCard(pcard), NULL, config, egmConfig, CTIME_DEFAULT_VALUE, false, &games);

			CCard *gcard(new CCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE));
			gcard->SetCardID("6909FF0001");
			gcard->SetCardType(CARD_GLOBAL);
			CEmployee *employee = new CEmployee(gcard, config, false, false, 0, 0);

			CCard scard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
			scard.SetCardID("1234567890");
			scard.SetCardType(CARD_TEMP);

			// Test.
			player.SetVirtualFlag(true);
			ASSERT_TRUE(obj.TransformInsertedCard(true, employee, &player, &scard) == Ignored);
			ASSERT_TRUE(obj.TransformInsertedCard(false, employee, &player, &scard) == Ignored);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::TransformInsertedCard()
		///     CPlayer::SetCardedOutOnEmployeeCardIn()
		///     CPlayer::WasCardedOutOnEmployeeCardIn()
		/// Test employee card inserted with a non-virtual player session.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, NonVirtualPlayerSessionTranslateInsertEmployee)
		{
			// Test data.
			const bool virtualFlag(false);
			const bool cardedOutOnEmployeeCardInSetup(false);
			const bool cardedOutOnEmployeeCardIn(true);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			CCard gcard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
			gcard.SetCardType(CARD_FLOOR);
			CPlayer player;

			// Test.
			player.SetCardedOutOnEmployeeCardIn(cardedOutOnEmployeeCardInSetup);
			player.SetVirtualFlag(virtualFlag);
			ASSERT_TRUE(obj.TransformInsertedCard(true, NULL, &player, &gcard) == RemovedInserted);
			ASSERT_TRUE(player.WasCardedOutOnEmployeeCardIn() == cardedOutOnEmployeeCardIn);

			player.SetCardedOutOnEmployeeCardIn(cardedOutOnEmployeeCardInSetup);
			player.SetVirtualFlag(virtualFlag);
			ASSERT_TRUE(obj.TransformInsertedCard(false, NULL, &player, &gcard) == RemovedInserted);
			ASSERT_TRUE(player.WasCardedOutOnEmployeeCardIn() == cardedOutOnEmployeeCardIn);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::TransformInsertedCard()
		///     CPlayer::SetCardedOutOnEmployeeCardIn()
		///     CPlayer::WasCardedOutOnEmployeeCardIn()
		/// Test employee card inserted with a virtual player session.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, VirtualPlayerSessionTranslateInsertEmployee)
		{
			// Test data.
			const bool virtualFlag(true);
			const bool cardedOutOnEmployeeCardIn(false);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			CCard gcard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
			gcard.SetCardType(CARD_FLOOR);
			CPlayer player;

			// Test.
			player.SetCardedOutOnEmployeeCardIn(cardedOutOnEmployeeCardIn);
			player.SetVirtualFlag(virtualFlag);
			ASSERT_TRUE(obj.TransformInsertedCard(true, NULL, &player, &gcard) == Inserted);
			ASSERT_TRUE(player.WasCardedOutOnEmployeeCardIn() == cardedOutOnEmployeeCardIn);

			player.SetCardedOutOnEmployeeCardIn(cardedOutOnEmployeeCardIn);
			player.SetVirtualFlag(virtualFlag);
			ASSERT_TRUE(obj.TransformInsertedCard(false, NULL, &player, &gcard) == Inserted);
			ASSERT_TRUE(player.WasCardedOutOnEmployeeCardIn() == cardedOutOnEmployeeCardIn);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::IsVirtualBadCardRead()
		/// Test for default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, IsVirtualBadCardRead_Default)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			//Test(s).
			ASSERT_TRUE(obj.IsVirtualBadCardRead() == false);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::IsVirtualBadCardRead()
		/// Test when a virtual bad card read is in progress.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, IsVirtualBadCardRead_True)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetVirtualBadCardRead(false);

			//Test(s).
			ASSERT_TRUE(obj.IsVirtualBadCardRead() == true);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::IsVirtualBadCardRead()
		/// Test when a virtual bad card read is not in progress.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, IsVirtualBadCardRead_False)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetVirtualBadCardRead(true);

			//Test(s).
			ASSERT_TRUE(obj.IsVirtualBadCardRead() == false);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetEmployeeOverPlayerIdleCardOut()
		/// Test for default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEmployeeOverPlayerIdleCardOutDefault)
		{
			// Test data.
			const bool employeeOverPlayerIdleCardOut(false);

			// Test.
			CPlayerSession obj; // Need writeable instance for getter.
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_TRUE(obj.GetEmployeeOverPlayerIdleCardOut() == employeeOverPlayerIdleCardOut);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::GetEmployeeOverPlayerIdleCardOut()
		/// Test for minimum value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEmployeeOverPlayerIdleCardOutMin)
		{
			// Test data.
			const bool employeeOverPlayerIdleCardOut(false);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			CConfig config(false, "");
			config.SetEmployeeOverPlayerIdleCardOut(employeeOverPlayerIdleCardOut);

			// Test.
			obj.UpdateConfigItems(config);
			ASSERT_TRUE(obj.GetEmployeeOverPlayerIdleCardOut() == employeeOverPlayerIdleCardOut);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::GetEmployeeOverPlayerIdleCardOut()
		/// Test for maximum value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEmployeeOverPlayerIdleCardOutMax)
		{
			// Test data.
			const bool employeeOverPlayerIdleCardOut(true);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			CConfig config(false, "");
			config.SetEmployeeOverPlayerIdleCardOut(employeeOverPlayerIdleCardOut);

			// Test.
			obj.UpdateConfigItems(config);
			ASSERT_TRUE(obj.GetEmployeeOverPlayerIdleCardOut() == employeeOverPlayerIdleCardOut);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetEmployeeIdleCardOutSeconds()
		/// Test for default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEmployeeIdleCardOutSecondsDefault)
		{
			// Test data.
			const WORD employeeIdleCardOutSeconds(120);

			// Test.
			CPlayerSession obj; // Need writeable instance for getter.
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_TRUE(obj.GetEmployeeIdleCardOutSeconds() == employeeIdleCardOutSeconds);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::GetEmployeeIdleCardOutSeconds()
		/// Test for minimum value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEmployeeIdleCardOutSecondsMin)
		{
			// Test data.
			const WORD employeeIdleCardOutSeconds(0);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			CConfig config(false, "");
			config.SetEmployeeIdleCardOutSeconds(employeeIdleCardOutSeconds);

			// Test.
			obj.UpdateConfigItems(config);
			ASSERT_TRUE(obj.GetEmployeeIdleCardOutSeconds() == employeeIdleCardOutSeconds);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetEmployeeIdleCardOutSeconds()
		/// Test for maximum value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEmployeeIdleCardOutSecondsMax)
		{
			// Test data.
			const WORD employeeIdleCardOutSeconds(WORD_MAX);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			CConfig config(false, "");
			config.SetEmployeeIdleCardOutSeconds(employeeIdleCardOutSeconds);

			// Test.
			obj.UpdateConfigItems(config);
			ASSERT_TRUE(obj.GetEmployeeIdleCardOutSeconds() == employeeIdleCardOutSeconds);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetPlayerRecardEmployeeCardOut()
		/// Test for default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetPlayerRecardEmployeeCardOutDefault)
		{
			// Test data.
			const bool playerRecardEmployeeCardOut(false);

			// Test.
			CPlayerSession obj; // Need writeable instance for getter.
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_TRUE(obj.GetPlayerRecardEmployeeCardOut() == playerRecardEmployeeCardOut);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetPlayerRecardEmployeeCardOut()
		/// Test for minimum value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetPlayerRecardEmployeeCardOutMin)
		{
			// Test data.
			const WORD playerRecardEmployeeCardOut(false);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			CConfig config(false, "");
			config.SetEmployeeIdleCardOutSeconds(playerRecardEmployeeCardOut);

			// Test.
			obj.UpdateConfigItems(config);
			ASSERT_TRUE(obj.GetEmployeeIdleCardOutSeconds() == playerRecardEmployeeCardOut);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetPlayerRecardEmployeeCardOut()
		/// Test for maximum value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetPlayerRecardEmployeeCardOutMax)
		{
			// Test data.
			const WORD playerRecardEmployeeCardOut(true);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			CConfig config(false, "");
			config.SetEmployeeIdleCardOutSeconds(playerRecardEmployeeCardOut);

			// Test.
			obj.UpdateConfigItems(config);
			ASSERT_TRUE(obj.GetEmployeeIdleCardOutSeconds() == playerRecardEmployeeCardOut);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetGameInProgress()
		/// Test for default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetGameInProgress_Default)
		{
			// Perform operation(s) and test(s).
			const CPlayerSession obj;
			ASSERT_EQ(m_gameInProgressExpected, obj.GetGameInProgress());
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::UpdateConfigItems()
		///     CPlayerSession::GetGameInProgress()
		/// Test for true value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetGameInProgress_UpdateConfigItems_False)
		{
			// Setup.
			CPlayerSession varPS;
			varPS.GameStarted();
			ASSERT_EQ(true, varPS.GetGameInProgress()); // Verify setup.

			CConfig varConfig(false, "");
			varConfig.SetGameInProgress(false);
			const CConfig &config(varConfig);

			// Perform operation(s) and test(s).
			varPS.UpdateConfigItems(config);
			const CPlayerSession &ps(varPS);
			ASSERT_FALSE(ps.GetGameInProgress());
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::UpdateConfigItems()
		///     CPlayerSession::GetGameInProgress()
		/// Test for true value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetGameInProgress_UpdateConfigItems_True)
		{
			// Setup.
			CPlayerSession varPS;
			varPS.GameEnded();
			ASSERT_FALSE(varPS.GetGameInProgress()); // Verify setup.

			CConfig varConfig(false, "");
			varConfig.SetGameInProgress(true);
			const CConfig &config(varConfig);

			// Perform operation(s) and test(s).
			varPS.UpdateConfigItems(config);
			const CPlayerSession &ps(varPS);
			ASSERT_EQ(true, ps.GetGameInProgress());
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::GameEnded()
		///     CPlayerSession::GetGameInProgress()
		/// Test for true value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetGameInProgress_GameEnded)
		{
			// Setup.
			CPlayerSession varPS;
			varPS.GameStarted();
			ASSERT_EQ(true, varPS.GetGameInProgress()); // Verify setup.

			// Perform operation(s) and test(s).
			varPS.GameEnded();
			const CPlayerSession &ps(varPS);
			ASSERT_FALSE(ps.GetGameInProgress());
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GetGameInProgress()
		/// Test for true value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetGameInProgress_GameStarted)
		{
			// Setup.
			CPlayerSession varPS;
			varPS.GameEnded();
			ASSERT_FALSE(varPS.GetGameInProgress()); // Verify setup.

			// Perform operation(s) and test(s).
			varPS.GameStarted();
			const CPlayerSession &ps(varPS);
			ASSERT_EQ(true, ps.GetGameInProgress());
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetLastGameStarted()
		/// Test for default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameStarted_Default)
		{
			// Perform operation(s) and test(s).
			const CPlayerSession obj;
			CUnitTestingUtils::AssertEqual(obj.GetLastGameStarted(),
				m_lastGameStartedExpected,
				"obj.GetLastGameStarted()",
				0,
				__TFUNCTION__,
				__LINE__,
				nullptr);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GetLastGameStarted()
		/// Test for default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameStarted_NonDefault)
		{
			// Setup.
			CPlayerSession varPS;
			varPS.GameStarted();

			// Perform operation(s) and test(s).
			const CPlayerSession &ps(varPS);
			CUnitTestingUtils::AssertNotEqual(ps.GetLastGameStarted(),
				m_lastGameStartedExpected,
				"ps.GetLastGameStarted()",
				0,
				__TFUNCTION__,
				__LINE__,
				nullptr);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetLastGameEnd()
		/// Test for default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameEnd_Default)
		{
			// Perform operation(s) and test(s).
			const CPlayerSession obj;
			CUnitTestingUtils::AssertEqual(obj.GetLastGameEnd(),
				m_lastGameStartedExpected,
				"obj.GetLastGameStarted()",
				0,
				__TFUNCTION__,
				__LINE__,
				nullptr);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayerSession::GameEnded()
		///     CPlayerSession::GetLastGameEnd()
		/// Test for default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameEnd_NonDefault)
		{
			// Setup.
			CPlayerSession varPS;
			varPS.GameEnded();

			// Perform operation(s) and test(s).
			const CPlayerSession &ps(varPS);
			CUnitTestingUtils::AssertNotEqual(ps.GetLastGameEnd(),
				m_lastGameStartedExpected,
				"ps.GetLastGameEnd()",
				0,
				__TFUNCTION__,
				__LINE__,
				nullptr);
		}

// #pragma region SessionActiveHelpers
		/// <summary>
		/// Gets the session active status by calling obj.GetSessionActiveInfo().
		/// </summary>
		/// <param name="obj">IN - The CPlayerSession object.</param>
		/// <returns></returns>
		static bool GetSessionActive(const CPlayerSession &obj)
		{
			bool sessionActive;
			bool cardedPlayer;
			int sessionId;
			obj.GetSessionActiveInfo(sessionActive, cardedPlayer, sessionId);
			return sessionActive;
		}

		/// <summary>
		/// Gets the session identifier by calling obj.GetSessionActiveInfo().
		/// </summary>
		/// <param name="obj">IN - The CPlayerSession object.</param>
		/// <returns></returns>
		static int GetSessionIdFromInfo(const CPlayerSession &obj)
		{
			bool sessionActive;
			bool cardedPlayer;
			int sessionId;
			obj.GetSessionActiveInfo(sessionActive, cardedPlayer, sessionId);
			return sessionId;
		}
// #pragma endregion SessionActiveHelpers

// #pragma region NVRAM
		void PersistNonDefaultSessionIdPlus1()
		{
			// Data.
			// SessionId is incremented each time it is recovered from NVRAM, because corresponding info is not persisted.
			const int sessionIdExpected(m_sessionIdDefault + 1);

			// Get default SessionId with NVRAM good.
			CUnitTestingUtils::SetNvramInvalid(); // For default SessionId.
			CPlayerSession obj(MEMORY_NVRAM);
			ASSERT_EQ(m_sessionIdDefault, obj.GetSessionId()) << "GetSessionId default";

			// Increment and persist SessionId.
			CUnitTestingUtils::SetNvramValidAndUpgraded();
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			obj.SetCreditsInPennies(2000, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_EQ(sessionIdExpected, obj.GetSessionId()) << "GetSessionId default";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		/// Test with default memory type -- expect MEMORY_NONE.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, CPlayerSession_MemoryDefault)
		{
			// Setup.
			PersistNonDefaultSessionIdPlus1();

			// Perform operation(s) and test(s).
			const CPlayerSession obj;
			ASSERT_EQ(m_sessionIdDefault, obj.GetSessionId()) << "GetSessionId";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		/// Test with memoryType = MEMORY_NONE.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, CPlayerSession_MemoryNone)
		{
			// Setup.
			PersistNonDefaultSessionIdPlus1();

			// Perform operation(s) and test(s).
			const CPlayerSession obj(MEMORY_NONE);
			ASSERT_EQ(m_sessionIdDefault, obj.GetSessionId()) << "GetSessionId";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		/// Test with memoryType = MEMORY_NVRAM.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, CPlayerSession_NvramValid)
		{
			// Data.
			// SessionId is incremented each time it is recovered from NVRAM, because corresponding info is not persisted.
			const int sessionIdExpected(m_sessionIdDefault + 1);

			// Setup.
			PersistNonDefaultSessionIdPlus1();

			// Perform operation(s) and test(s).
			const CPlayerSession obj(MEMORY_NVRAM);
			ASSERT_EQ(sessionIdExpected , obj.GetSessionId()) << "GetSessionId";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		/// Test with memoryType = MEMORY_NVRAM, but with NVRAM invalid (lost).
		/// This also tests for NVRAM cleared from the Global Card menu.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, CPlayerSession_NvramInvalid)
		{
			// Setup.
			PersistNonDefaultSessionIdPlus1();
			CUnitTestingUtils::SetNvramInvalid();

			// Perform operation(s) and test(s).
			const CPlayerSession obj(MEMORY_NVRAM);
			ASSERT_EQ(m_sessionIdDefault, obj.GetSessionId()) << "GetSessionId";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		/// Test with memoryType = MEMORY_NVRAM, but with NVRAM invalid (lost) on the 1st restart, but valid on the 2nd restart.
		/// This also tests for NVRAM cleared from the Global Card menu.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, CPlayerSession_NvramInvalidValid)
		{
			// Setup.
			PersistNonDefaultSessionIdPlus1();
			CUnitTestingUtils::SetNvramInvalid();

			// Perform operation(s) and test(s).
			const CPlayerSession obj1(MEMORY_NVRAM);
			ASSERT_EQ(m_sessionIdDefault, obj1.GetSessionId()) << "GetSessionId 1";

			CUnitTestingUtils::SetNvramValidAndUpgraded();
			const CPlayerSession obj2(MEMORY_NVRAM);
			ASSERT_EQ(m_sessionIdDefault, obj2.GetSessionId()) << "GetSessionId 2";
		}

		TEST_F(PlayerSessionUnitTests, GetNvramBuffer)
		{
			// Data.
			// SessionId is incremented each time it is recovered from NVRAM, because corresponding info is not persisted.
			// So, a SessionId of 2 is expected.
			bool activeExpected(true);
			UINT bufferSizeUsedExpected(46);
			BYTE bufferExpected[CPlayerSession::NVRAM_DATA_SIZE] =
			{
				0x02, 0x00, 0x00, 0x00,
			};
			// Setup. Get non-default values.
			PersistNonDefaultSessionIdPlus1();
			const CPlayerSession obj(MEMORY_NVRAM);

			// Perform operation(s) and test(s).
			bool active;
			BYTE buffer[CPlayerSession::NVRAM_DATA_SIZE];
			ASSERT_EQ(bufferSizeUsedExpected, obj.GetNvramBuffer(buffer, sizeof(buffer), active)) << "GetNvramBuffer";
			ASSERT_EQ(activeExpected, active) << "active";
		}

		TEST_F(PlayerSessionUnitTests, SetFromNvramBuffer)
		{
			// Data.
			const int sessionIdExpected(0x12345678);
			UINT bufferSizeUsed(4);
			BYTE buffer[CPlayerSession::NVRAM_DATA_SIZE] =
			{
				0x78, 0x56, 0x34, 0x12,
			};
			memset(buffer + bufferSizeUsed, ~CPlayerSession::NVRAM_ACTIVE_VALUE, sizeof(buffer) - bufferSizeUsed);

			// Perform operation(s) and test(s).
			CPlayerSession obj(MEMORY_NVRAM);
			obj.SetFromNvramBuffer(buffer, sizeof(buffer));
			ASSERT_EQ(sessionIdExpected, obj.GetSessionId()) << "GetSessionId";
		}
// #pragma endregion NVRAM

// #pragma region GetSessionId
		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetSessionId()
		///     CPlayerSession::GetSessionActiveInfo()
		/// Test constructor for default value(s).
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetSessionId_Default)
		{
			// Data.
			ASSERT_EQ(0, SESSION_ID_DEFAULT) << "SESSION_ID_DEFAULT";
			const int sessionIdDefault(SESSION_ID_DEFAULT);

			// Perform operation(s) and test(s).
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_EQ(sessionIdDefault, obj.GetSessionId()) << "GetSessionId";
			ASSERT_EQ(sessionIdDefault, GetSessionIdFromInfo(obj)) << "GetSessionActiveInfo";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::SetCreditsInPennies()
		///     CPlayerSession::GetSessionId()
		///     CPlayerSession::GetSessionActiveInfo()
		/// Test for non-default value(s).
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetSessionId_Increased)
		{
			// Data.
			const int sessionIdDefault(SESSION_ID_DEFAULT);
			const int sessionIdExpected(sessionIdDefault + 1);
			DWORD creditsInPennies(2000);

			// Setup.
			CPlayerSession obj;			
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_EQ(sessionIdDefault, obj.GetSessionId()) << "GetSessionId setup";

			// Perform operation(s) and test(s).
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			obj.SetCreditsInPennies(creditsInPennies, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_EQ(sessionIdExpected, obj.GetSessionId()) << "GetSessionId";
			ASSERT_EQ(sessionIdExpected, GetSessionIdFromInfo(obj)) << "GetSessionActiveInfo";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession(const CPlayerSession &)
		///     CPlayerSession::GetSessionId()
		///     CPlayerSession::GetSessionActiveInfo()
		/// Test copy constructor with non-default value(s).
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetSessionId_Copy)
		{
			// Data.
			const int sessionIdDefault(SESSION_ID_DEFAULT);
			const int sessionIdExpected(sessionIdDefault + 1);
			DWORD creditsInPennies(2000);

			// Setup.
			CPlayerSession sourceObj;
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			sourceObj.SetCreditsInPennies(creditsInPennies, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_EQ(sessionIdExpected, sourceObj.GetSessionId()) << "GetSessionId setup"; // Verify setup.

			// Perform operation(s) and test(s).
			const CPlayerSession obj(sourceObj);
			ASSERT_EQ(sessionIdExpected, obj.GetSessionId()) << "GetSessionId";
			ASSERT_EQ(sessionIdExpected, GetSessionIdFromInfo(obj)) << "GetSessionActiveInfo";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::operator=()
		///     CPlayerSession::GetSessionId()
		///     CPlayerSession::GetSessionActiveInfo()
		/// Test assignment operator with non-default value(s).
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetSessionId_Assignment)
		{
			// Data.
			const int sessionIdDefault(SESSION_ID_DEFAULT);
			const int sessionIdExpected(sessionIdDefault + 1);
			DWORD creditsInPennies(2000);

			// Setup.
			CPlayerSession sourceObj;
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			sourceObj.SetCreditsInPennies(creditsInPennies, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_EQ(sessionIdExpected, sourceObj.GetSessionId()) << "GetSessionId setup"; // Verify setup.

			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_EQ(sessionIdDefault, obj.GetSessionId()) << "GetSessionId default"; // Verify setup.

			// Perform operation(s) and test(s).
			obj = sourceObj;
			ASSERT_EQ(sessionIdExpected, obj.GetSessionId()) << "GetSessionId";
			ASSERT_EQ(sessionIdExpected, GetSessionIdFromInfo(obj)) << "GetSessionActiveInfo";
		}
// #pragma endregion GetSessionId

// #pragma region GetLastSessionIdIncrementTime
		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetLastSessionIdIncrementTime()
		/// Test constructor for default value(s).
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastSessionIdIncrementTime_Default)
		{
			// Data.
			tm gameTm;
            gameTm.tm_year=2014;
            gameTm.tm_mon= 05;
            gameTm.tm_mday= 15;
            gameTm.tm_hour= 12;
            gameTm.tm_min= 34;
            gameTm.tm_sec= 00;
            time_t GameDate = mktime(&gameTm);

			// Setup.
			COperatingSystemTest os;
			os.m_currentTime = GameDate;

			// Perform operation(s) and test(s).
			CPlayerSession obj(MEMORY_NONE, os);
			ASSERT_EQ(GameDate, obj.GetLastSessionIdIncrementTime()) << "GetLastSessionIdIncrementTime";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::SetCreditsInPennies()
		///     CPlayerSession::GetLastSessionIdIncrementTime()
		/// Test for non-default value(s).
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastSessionIdIncrementTime_Increased)
		{
			// Data.
			tm gameTm;
            gameTm.tm_year=2014;
            gameTm.tm_mon= 05;
            gameTm.tm_mday= 15;
            gameTm.tm_hour= 12;
            gameTm.tm_min= 34;
            gameTm.tm_sec= 00;
            time_t GameDate = mktime(&gameTm);
			const time_t  incrementTime(GameDate + 10);
			DWORD creditsInPennies(2000);

			// Setup.
			COperatingSystemTest os;
			os.m_currentTime = GameDate;
			CPlayerSession obj(MEMORY_NONE, os);
			ASSERT_EQ(GameDate, obj.GetLastSessionIdIncrementTime()) << "GetLastSessionIdIncrementTime setup"; // Verify setup.

			// Perform operation(s) and test(s).
			os.m_currentTime = incrementTime;
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			obj.SetCreditsInPennies(creditsInPennies, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_EQ(incrementTime, obj.GetLastSessionIdIncrementTime()) << "GetLastSessionIdIncrementTime";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetLastSessionIdIncrementTime()
		/// Test copy constructor with non-default value(s).
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastSessionIdIncrementTime_Copy)
		{
			// Data.
			tm gameTm;
            gameTm.tm_year=2014;
            gameTm.tm_mon= 05;
            gameTm.tm_mday= 15;
            gameTm.tm_hour= 12;
            gameTm.tm_min= 34;
            gameTm.tm_sec= 00;
            time_t incrementTime = mktime(&gameTm);
			DWORD creditsInPennies(2000);

			// Setup.
			COperatingSystemTest os;
			os.m_currentTime = incrementTime;
			CPlayerSession sourceObj(MEMORY_NONE, os);
			ASSERT_EQ(incrementTime, sourceObj.GetLastSessionIdIncrementTime()) << "GetLastSessionIdIncrementTime setup"; // Verify setup.

			// Perform operation(s) and test(s).
			const CPlayerSession obj(sourceObj);
			ASSERT_EQ(incrementTime, obj.GetLastSessionIdIncrementTime()) << "GetLastSessionIdIncrementTime";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetLastSessionIdIncrementTime()
		/// Test assignment operator with non-default value(s).
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastSessionIdIncrementTime_Assignment)
		{
			// Data.
			tm gameTm;
            gameTm.tm_year=2014;
            gameTm.tm_mon= 05;
            gameTm.tm_mday= 15;
            gameTm.tm_hour= 12;
            gameTm.tm_min= 34;
            gameTm.tm_sec= 00;
            time_t incrementTime = mktime(&gameTm);
			const time_t  initialTime(incrementTime  + 10);
			DWORD creditsInPennies(2000);

			// Setup.
			COperatingSystemTest os;
			os.m_currentTime = incrementTime;
			CPlayerSession sourceObj(MEMORY_NONE, os);
			ASSERT_EQ(incrementTime, sourceObj.GetLastSessionIdIncrementTime()) << "GetLastSessionIdIncrementTime setup"; // Verify setup.

			os.m_currentTime = initialTime;
			CPlayerSession obj(MEMORY_NONE, os);
			ASSERT_EQ(initialTime, obj.GetLastSessionIdIncrementTime()) << "GetLastSessionIdIncrementTime obj setup"; // Verify setup.

			// Perform operation(s) and test(s).
			obj = sourceObj;
			ASSERT_EQ(incrementTime, obj.GetLastSessionIdIncrementTime()) << "GetLastSessionIdIncrementTime"; // Verify setup.
		}
// #pragma endregion GetLastSessionIdIncrementTime

// #pragma region CreditMeterIncreased
		/// <summary>
		/// Setup for CreditMeterIncreased tests.
		/// Setup is completes at m_tickCountSetup with m_playerSession.m_sessionActive = false.
		/// </summary>
		class CCreditMeterIncreasedSetup
		{
		public:
			// Data - constants.
			static const WORD m_negligibleCreditsThreshold = 100; // Pennies.
			static const WORD m_negligibleTimeout = 10; // Seconds.
			static const WORD m_nonNegligibleTimeout = 20; // Seconds.
			static const DWORD m_tickCountInitial = 10000;
			static const DWORD m_tickCountSetup = m_tickCountInitial + m_nonNegligibleTimeout * 1000; // When setup completes.

			// Data - instance member variables.
			COperatingSystemTest m_operatingSystem; // So we can mock/control the tick count.
			CConfig m_config;
			CPlayerSession m_playerSession;

			CCreditMeterIncreasedSetup(DWORD creditsInPenniesInitial)
				:
                m_config(false,""),
				m_playerSession(MEMORY_NONE, m_operatingSystem)
			{
				m_operatingSystem.m_tickCount32 = m_tickCountInitial;
				
				m_config.SetNegligibleCreditsThreshold(m_negligibleCreditsThreshold);
				m_config.SetSessionActiveNegligibleCreditsTimeoutSeconds(m_negligibleTimeout);
				m_config.SetSessionActiveNonNegligibleCreditsTimeoutSeconds(m_nonNegligibleTimeout);
				m_config.SetGameInProgress(false);
				m_playerSession.UpdateConfigItems(m_config);

				bool sessionActiveInfoChanged(false), creditsIncreased(false);
				m_playerSession.SetCreditsInPennies(creditsInPenniesInitial, sessionActiveInfoChanged, creditsIncreased);

				// Verify setup.
				m_operatingSystem.m_tickCount32 = m_tickCountSetup;
				m_playerSession.PerformSessionActiveTimedEvents();
				//ASSERT_FALSE(GetSessionActive(m_playerSession)) << "GetSessionActive setup";
			}
		};

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::SetCreditsInPennies()
		///     CPlayerSession::GetSessionActiveInfo()
		/// Test a credit meter update with no change.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, CreditMeterIncreased_NoChange)
		{
			// Data.
			DWORD creditsInPenniesInitial(CCreditMeterIncreasedSetup::m_negligibleCreditsThreshold * 2);
			DWORD creditsInPenniesTest(creditsInPenniesInitial);

			// Setup.
			CCreditMeterIncreasedSetup setup(creditsInPenniesInitial);

			// Perform operation(s) and test(s).
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			setup.m_playerSession.SetCreditsInPennies(creditsInPenniesTest, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_FALSE(sessionActiveInfoChanged) << "sessionActiveInfoChanged";
			ASSERT_FALSE(creditsIncreased) << "sessionActiveInfoChanged";
			ASSERT_FALSE(GetSessionActive(setup.m_playerSession)) << "GetSessionActive";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::SetCreditsInPennies()
		///     CPlayerSession::GetSessionActiveInfo()
		/// Test a credit meter decrease (from a greater non-negligible value to a lesser non-negligible value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, CreditMeterIncreased_Decrease)
		{
			// Data.
			DWORD creditsInPenniesInitial(CCreditMeterIncreasedSetup::m_negligibleCreditsThreshold * 3);
			DWORD creditsInPenniesTest(CCreditMeterIncreasedSetup::m_negligibleCreditsThreshold * 2);

			// Setup.
			CCreditMeterIncreasedSetup setup(creditsInPenniesInitial);

			// Perform operation(s) and test(s).
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			setup.m_playerSession.SetCreditsInPennies(creditsInPenniesTest, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_FALSE(sessionActiveInfoChanged) << "sessionActiveInfoChanged";
			ASSERT_FALSE(GetSessionActive(setup.m_playerSession)) << "GetSessionActive";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::SetCreditsInPennies()
		///     CPlayerSession::GetSessionActiveInfo()
		/// Test a credit meter increase from a negligible credits value to a greater negligible credits value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, CreditMeterIncreased_NegligibleNegligible)
		{
			// Data.
			DWORD creditsInPenniesInitial(0);
			DWORD creditsInPenniesTest(CCreditMeterIncreasedSetup::m_negligibleCreditsThreshold / 2);

			// Setup.
			CCreditMeterIncreasedSetup setup(creditsInPenniesInitial);

			// Perform operation(s) and test(s).
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			setup.m_playerSession.SetCreditsInPennies(creditsInPenniesTest, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_EQ(true, sessionActiveInfoChanged) << "sessionActiveInfoChanged";
			ASSERT_EQ(true, GetSessionActive(setup.m_playerSession)) << "GetSessionActive";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::SetCreditsInPennies()
		///     CPlayerSession::GetSessionActiveInfo()
		/// Test a credit meter increase from a negligible credits value to a non-negligible credits value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, CreditMeterIncreased_NegligibleNonNegligible)
		{
			// Data.
			DWORD creditsInPenniesInitial(0);
			DWORD creditsInPenniesTest(CCreditMeterIncreasedSetup::m_negligibleCreditsThreshold * 2);

			// Setup.
			CCreditMeterIncreasedSetup setup(creditsInPenniesInitial);

			// Perform operation(s) and test(s).
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			setup.m_playerSession.SetCreditsInPennies(creditsInPenniesTest, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_EQ(true, sessionActiveInfoChanged) << "sessionActiveInfoChanged";
			ASSERT_EQ(true, GetSessionActive(setup.m_playerSession)) << "GetSessionActive";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::SetCreditsInPennies()
		///     CPlayerSession::GetSessionActiveInfo()
		/// Test a credit meter increase from a non-negligible credits value to a greater non-negligible credits value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, CreditMeterIncreased_NonNegligibleNonNegligible)
		{
			// Data.
			DWORD creditsInPenniesInitial(CCreditMeterIncreasedSetup::m_negligibleCreditsThreshold * 2);
			DWORD creditsInPenniesTest(CCreditMeterIncreasedSetup::m_negligibleCreditsThreshold * 3);

			// Setup.
			CCreditMeterIncreasedSetup setup(creditsInPenniesInitial);

			// Perform operation(s) and test(s).
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			setup.m_playerSession.SetCreditsInPennies(creditsInPenniesTest, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_EQ(true, sessionActiveInfoChanged) << "sessionActiveInfoChanged";
			ASSERT_EQ(true, GetSessionActive(setup.m_playerSession)) << "GetSessionActive";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::SetCreditsInPennies()
		///     CPlayerSession::GetSessionActiveInfo()
		/// Test an additional credit meter increase extending the session active duration.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, CreditMeterIncreased_Extend)
		{
			// Data.
			DWORD creditsInPenniesInitial(CCreditMeterIncreasedSetup::m_negligibleCreditsThreshold * 2);
			DWORD creditsInPenniesTest(CCreditMeterIncreasedSetup::m_negligibleCreditsThreshold * 3);
			DWORD tickCountExtend(CCreditMeterIncreasedSetup::m_tickCountSetup + CCreditMeterIncreasedSetup::m_nonNegligibleTimeout * 1000 / 2);
			DWORD creditsInPenniesExtend(CCreditMeterIncreasedSetup::m_negligibleCreditsThreshold * 4);
			DWORD tickCountTimeout(CCreditMeterIncreasedSetup::m_tickCountSetup + CCreditMeterIncreasedSetup::m_nonNegligibleTimeout * 1000 * 3 / 2);

			// Setup.
			CCreditMeterIncreasedSetup setup(creditsInPenniesInitial);
			bool sessionActiveInfoChanged(false), creditsIncreased(false);
			setup.m_playerSession.SetCreditsInPennies(creditsInPenniesTest, sessionActiveInfoChanged, creditsIncreased);

			// Perform operation(s) and test(s).
			setup.m_operatingSystem.m_tickCount32 = tickCountExtend;
			setup.m_playerSession.SetCreditsInPennies(creditsInPenniesExtend, sessionActiveInfoChanged, creditsIncreased);
			ASSERT_EQ(true, GetSessionActive(setup.m_playerSession)) << "GetSessionActive Extend";

			// Test for session active still active after the extended period is almost timed out.
			setup.m_operatingSystem.m_tickCount32 = tickCountTimeout - 1;
			ASSERT_FALSE(setup.m_playerSession.PerformSessionActiveTimedEvents()) << "PerformTimedEvents Timeout-1";
			ASSERT_TRUE(GetSessionActive(setup.m_playerSession)) << "GetSessionActive Timeout-1";

			// Test for extended session active duration being timed out.
			setup.m_operatingSystem.m_tickCount32 = tickCountTimeout;
			ASSERT_TRUE(setup.m_playerSession.PerformSessionActiveTimedEvents()) << "PerformTimedEvents Timeout";
			ASSERT_FALSE(GetSessionActive(setup.m_playerSession)) << "GetSessionActive Timeout";
		}
// #pragma endregion CreditMeterIncreased

// #pragma region MiniVirtualSessionsEnabled
		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		/// Test default m_miniVirtualSessionsEnabled value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, MiniVirtualSessionsEnabled_Default)
		{
			ASSERT_FALSE(CConfigExtData::DEFAULT_MINI_VIRTUAL_SESSION_ENABLED);

			// Perform operation(s) and test(s).
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			//ASSERT_EQ(CConfigExtData::DEFAULT_MINI_VIRTUAL_SESSION_ENABLED, obj.m_miniVirtualSessionEnabled);
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::UpdateConfigItems()
		///     CPlayerSession::GetLastCoinsPlayed()
		/// Test with different m_miniVirtualSessionsEnabled data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, MiniVirtualSessionsEnabled_UpdateConfigItems)
		{
			// Setup.
			CPlayerSession obj;
			CConfig config(false,"");

			// Perform operation(s) and test(s).
			config.SetMiniVirtualSessionEnabled(true);
			obj.UpdateConfigItems(config);
			ASSERT_TRUE(GetminiVirtualSessionEnabled(obj)) << "m_miniVirtualSessionEnabled 1";

			config.SetMiniVirtualSessionEnabled(false);
			obj.UpdateConfigItems(config);
			ASSERT_FALSE(GetminiVirtualSessionEnabled(obj)) << "m_miniVirtualSessionEnabled 2";

			config.SetMiniVirtualSessionEnabled(true);
			obj.UpdateConfigItems(config);
			ASSERT_TRUE(GetminiVirtualSessionEnabled(obj)) << "m_miniVirtualSessionEnabled 3";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession(const CPlayerSession &)
		/// Test copy constructor with a false m_miniVirtualSessionEnabled value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, MiniVirtualSessionsEnabled_CopyFalse)
		{
			// Setup
			CPlayerSession objA;
			CConfig configA(false,"");
			configA.SetMiniVirtualSessionEnabled(false);
			objA.UpdateConfigItems(configA);
            ASSERT_FALSE(GetminiVirtualSessionEnabled(objA))<< "m_miniVirtualSessionEnabled setup";

			// Perform operation(s) and test(s).
			CPlayerSession obj(objA);
			ASSERT_FALSE(GetminiVirtualSessionEnabled(objA)) << "m_miniVirtualSessionEnabled";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession(const CPlayerSession &)
		/// Test copy constructor with a true m_miniVirtualSessionEnabled value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, MiniVirtualSessionsEnabled_CopyTrue)
		{
			// Setup
			CPlayerSession objA;
			CConfig configA(false,"");
			configA.SetMiniVirtualSessionEnabled(true);
			objA.UpdateConfigItems(configA);
			ASSERT_TRUE(GetminiVirtualSessionEnabled(objA)) << "m_miniVirtualSessionEnabled setup";

			// Perform operation(s) and test(s).
			CPlayerSession obj(objA);
			ASSERT_TRUE(GetminiVirtualSessionEnabled(objA)) << "m_miniVirtualSessionEnabled";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::operator=()
		/// Test assignment operator with a false m_miniVirtualSessionEnabled value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, MiniVirtualSessionsEnabled_AssignmentFalse)
		{
			// Setup
			CPlayerSession objA;
			CConfig configA(false,"");
			configA.SetMiniVirtualSessionEnabled(false);
			objA.UpdateConfigItems(configA);
			ASSERT_FALSE(GetminiVirtualSessionEnabled(objA)) << "m_miniVirtualSessionEnabled setup A";

			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			CConfig config(false,"");
			config.SetMiniVirtualSessionEnabled(true);
			obj.UpdateConfigItems(config);
			ASSERT_TRUE(GetminiVirtualSessionEnabled(obj)) << "m_miniVirtualSessionEnabled setup";

			// Perform operation(s) and test(s).
			obj = objA;
			ASSERT_FALSE(GetminiVirtualSessionEnabled(obj)) << "m_miniVirtualSessionEnabled";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::operator=()
		/// Test assignment operator with a true m_miniVirtualSessionEnabled value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, MiniVirtualSessionsEnabled_AssigmentTrue)
		{
			// Setup
			CPlayerSession objA;
			CConfig configA(false,"");
			configA.SetMiniVirtualSessionEnabled(true);
			objA.UpdateConfigItems(configA);
			ASSERT_TRUE(GetminiVirtualSessionEnabled(objA)) << "m_miniVirtualSessionEnabled setup A";

			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			CConfig config(false,"");
			config.SetMiniVirtualSessionEnabled(false);
			obj.UpdateConfigItems(config);
			ASSERT_FALSE(GetminiVirtualSessionEnabled(obj)) << "m_miniVirtualSessionEnabled setup";

			// Perform operation(s) and test(s).
			obj = objA;
			ASSERT_TRUE(GetminiVirtualSessionEnabled(obj)) << "m_miniVirtualSessionEnabled";
		}
// #pragma endregion MiniVirtualSessionsEnabled

// #pragma region CPlayerSession::GetEgmSendsGameEnd
		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetEgmSendsGameEnd()
		/// Test default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameEnd_Default)
		{
			ASSERT_EQ(m_egmSendsGameEndDefault, EGM_SENDS_GAME_END_DEFAULT);

			// Perform operation(s) and test(s).
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_EQ(m_egmSendsGameEndDefault, obj.GetEgmSendsGameEnd());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession(const CPlayerSession &)
		///     CPlayerSession::GetEgmSendsGameEnd()
		/// Test copy constructor with a false value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameEnd_CopyFalse)
		{
			// Setup
			CPlayerSession objA;
			objA.GameStarted();
			objA.GameStarted();
			AssertResult2(objA.GetEgmSendsGameEnd(), false); // Verify setup.

			// Perform operation(s) and test(s).
			CPlayerSession obj(objA);
			AssertResult2(obj.GetEgmSendsGameEnd(), false);
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession(const CPlayerSession &)
		///     CPlayerSession::GetEgmSendsGameEnd()
		/// Test copy constructor with a true value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameEnd_CopyTrue)
		{
			// Setup
			CPlayerSession objA;
			objA.GameStarted();
			objA.GameEnded();
			AssertResult2(objA.GetEgmSendsGameEnd(), true); // Verify setup.

			// Perform operation(s) and test(s).
			CPlayerSession obj(objA);
			AssertResult2(obj.GetEgmSendsGameEnd(), true);
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::operator=()
		///     CPlayerSession::GetEgmSendsGameEnd()
		/// Test assignment operator with a false value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameEnd_AssignmentFalse)
		{
			// Setup
			CPlayerSession objA;
			objA.GameStarted();
			objA.GameStarted();
			AssertResult2(objA.GetEgmSendsGameEnd(), false); // Verify setup.

			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.GameStarted();
			obj.GameEnded();
			AssertResult2(obj.GetEgmSendsGameEnd(), true); // Verify setup.

			// Perform operation(s) and test(s).
			obj = objA;
			AssertResult2(obj.GetEgmSendsGameEnd(), false);
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::operator=()
		///     CPlayerSession::GetEgmSendsGameEnd()
		/// Test assignment operator with a true value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameEnd_AssigmentTrue)
		{
			// Setup
			CPlayerSession objA;
			objA.GameStarted();
			objA.GameEnded();
			AssertResult2(objA.GetEgmSendsGameEnd(), true); // Verify setup.

			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.GameStarted();
			obj.GameStarted();
			AssertResult2(obj.GetEgmSendsGameEnd(), false); // Verify setup.

			// Perform operation(s) and test(s).
			obj = objA;
			AssertResult2(obj.GetEgmSendsGameEnd(), true);
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GetEgmSendsGameEnd()
		/// Test with consecutive game starts.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameEnd_False)
		{
			// Setup
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.GameStarted();
			obj.GameStarted();
			AssertResult2(obj.GetEgmSendsGameEnd(), false);

			for (; TestIndex < 10; ++TestIndex)
			{
				obj.GameStarted();
				AssertResult2(obj.GetEgmSendsGameEnd(), false);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameEnded()
		///     CPlayerSession::GetEgmSendsGameEnd()
		/// Test with consecutive game ends.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameEnd_True1)
		{
			// Setup
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.GameEnded();
			AssertResult2(obj.GetEgmSendsGameEnd(), true);

			for (; TestIndex < 10; ++TestIndex)
			{
				obj.GameEnded();
				AssertResult2(obj.GetEgmSendsGameEnd(), true);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GameEnded()
		///     CPlayerSession::GetEgmSendsGameEnd()
		/// Test with game starts followed by game ends.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameEnd_True2)
		{
			// Setup
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.GameStarted();
			obj.GameEnded();
			AssertResult2(obj.GetEgmSendsGameEnd(), true);

			for (; TestIndex < 10; ++TestIndex)
			{
				obj.GameStarted();
				AssertResult2(obj.GetEgmSendsGameEnd(), true);
				obj.GameEnded();
				AssertResult2(obj.GetEgmSendsGameEnd(), true);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GameEnded()
		///     CPlayerSession::GetEgmSendsGameEnd()
		/// Test with consecutive game starts followed by a game end.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameEnd_Mixed1)
		{
			// Setup
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.GameStarted();
			obj.GameEnded();
			AssertResult2(obj.GetEgmSendsGameEnd(), true);

			for (; TestIndex < 10; ++TestIndex)
			{
				obj.GameStarted();
				AssertResult2(obj.GetEgmSendsGameEnd(), true);
				obj.GameStarted();
				AssertResult2(obj.GetEgmSendsGameEnd(), false);
				obj.GameEnded();
				AssertResult2(obj.GetEgmSendsGameEnd(), true);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GameEnded()
		///     CPlayerSession::GetEgmSendsGameEnd()
		/// Test with consecutive game starts and consecutive game ends.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameEnd_Mixed2)
		{
			// Setup
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.GameStarted();
			obj.GameEnded();
			AssertResult2(obj.GetEgmSendsGameEnd(), true);

			for (; TestIndex < 10; ++TestIndex)
			{
				obj.GameStarted();
				AssertResult2(obj.GetEgmSendsGameEnd(), true);
				obj.GameStarted();
				AssertResult2(obj.GetEgmSendsGameEnd(), false);
				obj.GameEnded();
				AssertResult2(obj.GetEgmSendsGameEnd(), true);
				obj.GameEnded();
				AssertResult2(obj.GetEgmSendsGameEnd(), true);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GameEnded()
		///     CPlayerSession::GetEgmSendsGameEnd()
		/// Test with varying consecutive game starts and varying consecutive game ends.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameEnd_Mixed3)
		{
			// Setup
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.GameStarted();
			obj.GameEnded();
			AssertResult2(obj.GetEgmSendsGameEnd(), true);

			for (; TestIndex < 10; ++TestIndex)
			{
				obj.GameStarted();
				AssertResult2(obj.GetEgmSendsGameEnd(), true);

				for (int extraIndex(0); extraIndex <= TestIndex; ++extraIndex)
				{
					obj.GameStarted();
					AssertResult2x(obj.GetEgmSendsGameEnd(), false);
				}

				obj.GameEnded();
				AssertResult2(obj.GetEgmSendsGameEnd(), true);

				for (int extraIndex(0); extraIndex <= TestIndex; ++extraIndex)
				{
					obj.GameEnded();
					AssertResult2x(obj.GetEgmSendsGameEnd(), true);
				}
			}
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GameEnded()
		///     CPlayerSession::GetEgmSendsGameEnd()
		/// Test with varying consecutive game starts and varying game starts followed by game ends.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameEnd_Mixed4)
		{
			// Setup
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.GameStarted();
			obj.GameEnded();
			AssertResult2(obj.GetEgmSendsGameEnd(), true);

			for (; TestIndex < 10; ++TestIndex)
			{
				obj.GameStarted();
				AssertResult2(obj.GetEgmSendsGameEnd(), true);

				for (int extraIndex(0); extraIndex <= TestIndex; ++extraIndex)
				{
					obj.GameStarted();
					AssertResult2x(obj.GetEgmSendsGameEnd(), false);
				}

				obj.GameEnded();
				AssertResult2(obj.GetEgmSendsGameEnd(), true);

				for (int extraIndex(0); extraIndex <= TestIndex; ++extraIndex)
				{
					obj.GameStarted();
					AssertResult2x(obj.GetEgmSendsGameEnd(), true);
					obj.GameEnded();
					AssertResult2x(obj.GetEgmSendsGameEnd(), true);
				}
			}
		}
// #pragma endregion CPlayerSession::GetEgmSupportsGameEnd

// #pragma region SetCardInSentToProgressiveProtocol
		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetCardInSentToProgressiveProtocol()
		/// Test default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, SetCardInSentToProgressiveProtocol_Default)
		{
			// Perform operation(s) and test(s).
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_FALSE(CPlayerSession::DEFAULT_CARD_IN_SENT_TO_PROGRESSIVE_PROTOCOL) <<
				L"DEFAULT_CARD_IN_SENT_TO_PROGRESSIVE_PROTOCOL";
			ASSERT_FALSE(obj.GetCardInSentToProgressiveProtocol()) << "GetCardInSentToProgressiveProtocol";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession(const CPlayerSession &)
		///     CPlayerSession::GetCardInSentToProgressiveProtocol()
		/// Test copy constructor for correct value(s).
		/// </summary>
		TEST_F(PlayerSessionUnitTests, SetCardInSentToProgressiveProtocol_Copy)
		{
			// Data.
			bool cardInSent(true);

			// Setup.
			CPlayerSession sourceObj;
			sourceObj.SetCardInSentToProgressiveProtocol(cardInSent);

			// Perform operation(s) and test(s).
			CPlayerSession obj(sourceObj);
			ASSERT_EQ(cardInSent, obj.GetCardInSentToProgressiveProtocol()) << "GetCardInSentToProgressiveProtocol";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession(const CPlayerSession &)
		///     CPlayerSession::GetCardInSentToProgressiveProtocol()
		/// Test assignment operator for correct value(s).
		/// </summary>
		TEST_F(PlayerSessionUnitTests, SetCardInSentToProgressiveProtocol_Assignment)
		{
			// Setup.
			CPlayerSession trueObj;
			trueObj.SetCardInSentToProgressiveProtocol(true);
			CPlayerSession falseObj;
			falseObj.SetCardInSentToProgressiveProtocol(false);

			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_FALSE(obj.GetCardInSentToProgressiveProtocol()) << "GetCardInSentToProgressiveProtocol setup";

			// Perform operation(s) and test(s).
			obj = trueObj;
			ASSERT_TRUE(obj.GetCardInSentToProgressiveProtocol()) << "GetCardInSentToProgressiveProtocol";
			obj = falseObj;
			ASSERT_FALSE(obj.GetCardInSentToProgressiveProtocol()) << "GetCardInSentToProgressiveProtocol";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::SetCardInSentToProgressiveProtocol()
		///     CPlayerSession::GetCardInSentToProgressiveProtocol()
		/// Test with various values.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, SetCardInSentToProgressiveProtocol)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.SetCardInSentToProgressiveProtocol(true);
			ASSERT_TRUE(obj.GetCardInSentToProgressiveProtocol()) << "GetCardInSentToProgressiveProtocol";

			obj.SetCardInSentToProgressiveProtocol(false);
			ASSERT_FALSE(obj.GetCardInSentToProgressiveProtocol()) << "GetCardInSentToProgressiveProtocol";

			obj.SetCardInSentToProgressiveProtocol(true);
			ASSERT_TRUE(obj.GetCardInSentToProgressiveProtocol()) << "GetCardInSentToProgressiveProtocol";
		}
// #pragma endregion SetCardInSentToProgressiveProtocol

// #pragma region OfflineCoinIn
		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::IsSasStartupQueueEmpty()
		/// Test default value for IsSasStartupQueueEmpty().
		/// </summary>
		TEST_F(PlayerSessionUnitTests, IsSasStartupQueueEmpty_Default)
		{
			// Perform operation(s) and test(s).
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_FALSE(CPlayerSession::DEFAULT_SAS_STARTUP_QUEUE_EMPTY) << "DEFAULT_SAS_STARTUP_QUEUE_EMPTY";
			ASSERT_FALSE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		/// Test default value for m_offlineCoinIn.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, IsOfflineCoinIn_Default)
		{
			// Perform operation(s) and test(s).
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_TRUE(CPlayerSession::DEFAULT_OFFLINE_COIN_IN) << "DEFAULT_OFFLINE_COIN_IN";
			ASSERT_TRUE(GetofflineCoinIn(obj)) << "m_offlineCoinIn";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::UpdateSasStartupQueueEmpty()
		///     CPlayerSession::IsSasStartupQueueEmpty()
		/// Test with m_sasStartupQueueEmpty false and m_offlineCoinIndefault true, updating to false.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, UpdateSasStartupQueueEmpty_FalseTrue_UpdateToFalse)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_FALSE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty"; // Verify setup.
			ASSERT_TRUE(GetofflineCoinIn(obj)) << "m_offlineCoinIn";

			// Perform operation(s) and test(s).
			bool initializeLastCoinsPlayedAndWon;
			bool sendOfflineCoinIn;
			obj.UpdateSasStartupQueueEmpty(false, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_FALSE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty";
			ASSERT_FALSE(initializeLastCoinsPlayedAndWon) << "initializeLastCoinsPlayedAndWon";
			ASSERT_FALSE(sendOfflineCoinIn) << "sendOfflineCoinIn";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::UpdateSasStartupQueueEmpty()
		///     CPlayerSession::IsSasStartupQueueEmpty()
		/// Test with m_sasStartupQueueEmpty false and m_offlineCoinIndefault true, updating to true.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, UpdateSasStartupQueueEmpty_FalseTrue_UpdateToTrue)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_FALSE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty"; // Verify setup.
			ASSERT_TRUE(GetofflineCoinIn(obj)) << "m_offlineCoinIn";

			// Perform operation(s) and test(s).
			bool initializeLastCoinsPlayedAndWon;
			bool sendOfflineCoinIn;
			obj.UpdateSasStartupQueueEmpty(true, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_TRUE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty";
			ASSERT_TRUE(initializeLastCoinsPlayedAndWon) << "initializeLastCoinsPlayedAndWon";
			ASSERT_TRUE(sendOfflineCoinIn) << "sendOfflineCoinIn";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::UpdateSasStartupQueueEmpty()
		///     CPlayerSession::IsSasStartupQueueEmpty()
		/// Test with m_sasStartupQueueEmpty true and m_offlineCoinIndefault false, updating to false.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, UpdateSasStartupQueueEmpty_TrueFalse_UpdateToFalse)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			bool initializeLastCoinsPlayedAndWon;
			bool sendOfflineCoinIn;
			obj.UpdateSasStartupQueueEmpty(true, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_TRUE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty"; // Verify setup.
			ASSERT_FALSE(GetofflineCoinIn(obj)) << "m_offlineCoinIn";

			// Perform operation(s) and test(s).
			obj.UpdateSasStartupQueueEmpty(false, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_FALSE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty";
			ASSERT_FALSE(initializeLastCoinsPlayedAndWon) << "initializeLastCoinsPlayedAndWon";
			ASSERT_FALSE(sendOfflineCoinIn) << "sendOfflineCoinIn";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::UpdateSasStartupQueueEmpty()
		///     CPlayerSession::IsSasStartupQueueEmpty()
		/// Test with m_sasStartupQueueEmpty true and m_offlineCoinIndefault false, updating to true.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, UpdateSasStartupQueueEmpty_TrueFalse_UpdateToTrue)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			bool initializeLastCoinsPlayedAndWon;
			bool sendOfflineCoinIn;
			obj.UpdateSasStartupQueueEmpty(true, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_TRUE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty"; // Verify setup.
			ASSERT_FALSE(GetofflineCoinIn(obj)) << "m_offlineCoinIn";

			// Perform operation(s) and test(s).
			obj.UpdateSasStartupQueueEmpty(true, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_TRUE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty";
			ASSERT_FALSE(initializeLastCoinsPlayedAndWon) << "initializeLastCoinsPlayedAndWon";
			ASSERT_FALSE(sendOfflineCoinIn) << "sendOfflineCoinIn";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::UpdateSasStartupQueueEmpty()
		///     CPlayerSession::IsSasStartupQueueEmpty()
		/// Test with m_sasStartupQueueEmpty false and m_offlineCoinIndefault false, updating to false.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, UpdateSasStartupQueueEmpty_FalseFalse_UpdateToFalse)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			bool initializeLastCoinsPlayedAndWon;
			bool sendOfflineCoinIn;
			obj.UpdateSasStartupQueueEmpty(true, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			obj.UpdateSasStartupQueueEmpty(false, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_FALSE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty"; // Verify setup.
			ASSERT_FALSE(GetofflineCoinIn(obj)) << "m_offlineCoinIn";

			// Perform operation(s) and test(s).
			obj.UpdateSasStartupQueueEmpty(false, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_FALSE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty";
			ASSERT_FALSE(initializeLastCoinsPlayedAndWon) << "initializeLastCoinsPlayedAndWon";
			ASSERT_FALSE(sendOfflineCoinIn) << "sendOfflineCoinIn";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::UpdateSasStartupQueueEmpty()
		///     CPlayerSession::IsSasStartupQueueEmpty()
		/// Test with m_sasStartupQueueEmpty false and m_offlineCoinIndefault false, updating to true.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, UpdateSasStartupQueueEmpty_FalseFalse_UpdateToTrue)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			bool initializeLastCoinsPlayedAndWon;
			bool sendOfflineCoinIn;
			obj.UpdateSasStartupQueueEmpty(true, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			obj.UpdateSasStartupQueueEmpty(false, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_FALSE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty"; // Verify setup.
			ASSERT_FALSE(GetofflineCoinIn(obj)) << "m_offlineCoinIn";

			// Perform operation(s) and test(s).
			obj.UpdateSasStartupQueueEmpty(true, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_TRUE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty";
			ASSERT_TRUE(initializeLastCoinsPlayedAndWon) << "initializeLastCoinsPlayedAndWon";
			ASSERT_FALSE(sendOfflineCoinIn) << "sendOfflineCoinIn";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::UpdateSasStartupQueueEmpty()
		///     CPlayerSession::IsSasStartupQueueEmpty()
		/// Test with m_sasStartupQueueEmpty false and m_offlineCoinIndefault true, game started and updating to false.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, UpdateSasStartupQueueEmpty_FalseTrue_GameStarted_UpdateToFalse)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_FALSE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty"; // Verify setup.
			ASSERT_TRUE(GetofflineCoinIn(obj)) << "m_offlineCoinIn";

			// Perform operation(s) and test(s).
			obj.GameStarted();
			bool initializeLastCoinsPlayedAndWon;
			bool sendOfflineCoinIn;
			obj.UpdateSasStartupQueueEmpty(false, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_FALSE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty";
			ASSERT_FALSE(initializeLastCoinsPlayedAndWon) << "initializeLastCoinsPlayedAndWon";
			ASSERT_FALSE(sendOfflineCoinIn) << "sendOfflineCoinIn";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::UpdateSasStartupQueueEmpty()
		///     CPlayerSession::IsSasStartupQueueEmpty()
		/// Test with m_sasStartupQueueEmpty false and m_offlineCoinIndefault true, game started and updating to true.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, UpdateSasStartupQueueEmpty_FalseTrue_GameStarted_UpdateToTrue)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_FALSE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty"; // Verify setup.
			ASSERT_TRUE(GetofflineCoinIn(obj)) << "m_offlineCoinIn";

			// Perform operation(s) and test(s).
			obj.GameStarted();
			bool initializeLastCoinsPlayedAndWon;
			bool sendOfflineCoinIn;
			obj.UpdateSasStartupQueueEmpty(true, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_TRUE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty";
			ASSERT_TRUE(initializeLastCoinsPlayedAndWon) << "initializeLastCoinsPlayedAndWon";
			ASSERT_TRUE(sendOfflineCoinIn) << "sendOfflineCoinIn";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::UpdateSasStartupQueueEmpty()
		///     CPlayerSession::IsSasStartupQueueEmpty()
		/// Test with m_sasStartupQueueEmpty true and m_offlineCoinIndefault false, game started and updating to false
		/// and then updating to true.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, UpdateSasStartupQueueEmpty_TrueFalse_GameStarted_UpdateToFalseTrue)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			bool initializeLastCoinsPlayedAndWon;
			bool sendOfflineCoinIn;
			obj.UpdateSasStartupQueueEmpty(true, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_TRUE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty"; // Verify setup.
			ASSERT_FALSE(GetofflineCoinIn(obj)) << "m_offlineCoinIn";

			// Perform operation(s) and test(s).
			obj.GameStarted();
			obj.UpdateSasStartupQueueEmpty(false, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_FALSE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty";
			ASSERT_FALSE(initializeLastCoinsPlayedAndWon) << "initializeLastCoinsPlayedAndWon";
			ASSERT_FALSE(sendOfflineCoinIn) << "sendOfflineCoinIn";

			// Verify that offline coin is still false.
			obj.UpdateSasStartupQueueEmpty(true, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_TRUE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty";
			ASSERT_TRUE(initializeLastCoinsPlayedAndWon) << "initializeLastCoinsPlayedAndWon";
			ASSERT_FALSE(sendOfflineCoinIn) << "sendOfflineCoinIn";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::UpdateSasStartupQueueEmpty()
		///     CPlayerSession::IsSasStartupQueueEmpty()
		/// Test with m_sasStartupQueueEmpty true and m_offlineCoinIndefault false, game started and updating to true.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, UpdateSasStartupQueueEmpty_TrueFalse_GameStarted_UpdateToTrue)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			bool initializeLastCoinsPlayedAndWon;
			bool sendOfflineCoinIn;
			obj.UpdateSasStartupQueueEmpty(true, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_TRUE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty"; // Verify setup.
			ASSERT_FALSE(GetofflineCoinIn(obj)) << "m_offlineCoinIn";

			// Perform operation(s) and test(s).
			obj.GameStarted();
			obj.UpdateSasStartupQueueEmpty(true, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_TRUE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty";
			ASSERT_FALSE(initializeLastCoinsPlayedAndWon) << "initializeLastCoinsPlayedAndWon";
			ASSERT_FALSE(sendOfflineCoinIn) << "sendOfflineCoinIn";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::UpdateSasStartupQueueEmpty()
		///     CPlayerSession::IsSasStartupQueueEmpty()
		/// Test with m_sasStartupQueueEmpty false and m_offlineCoinIndefault false, game started and updating to false.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, UpdateSasStartupQueueEmpty_FalseFalse_GameStarted_UpdateToFalse)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			bool initializeLastCoinsPlayedAndWon;
			bool sendOfflineCoinIn;
			obj.UpdateSasStartupQueueEmpty(true, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			obj.UpdateSasStartupQueueEmpty(false, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_FALSE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty"; // Verify setup.
			ASSERT_FALSE(GetofflineCoinIn(obj)) << "m_offlineCoinIn";

			// Perform operation(s) and test(s).
			obj.GameStarted();
			obj.UpdateSasStartupQueueEmpty(false, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_FALSE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty";
			ASSERT_FALSE(initializeLastCoinsPlayedAndWon) << "initializeLastCoinsPlayedAndWon";
			ASSERT_FALSE(sendOfflineCoinIn) << "sendOfflineCoinIn";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::UpdateSasStartupQueueEmpty()
		///     CPlayerSession::IsSasStartupQueueEmpty()
		/// Test with m_sasStartupQueueEmpty false and m_offlineCoinIndefault false, game started and updating to true.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, UpdateSasStartupQueueEmpty_FalseFalse_GameStarted_UpdateToTrue)
		{
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			bool initializeLastCoinsPlayedAndWon;
			bool sendOfflineCoinIn;
			obj.UpdateSasStartupQueueEmpty(true, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			obj.UpdateSasStartupQueueEmpty(false, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_FALSE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty"; // Verify setup.
			ASSERT_FALSE(GetofflineCoinIn(obj)) << "m_offlineCoinIn";

			// Perform operation(s) and test(s).
			obj.GameStarted();
			obj.UpdateSasStartupQueueEmpty(true, initializeLastCoinsPlayedAndWon, sendOfflineCoinIn);
			ASSERT_TRUE(obj.IsSasStartupQueueEmpty()) << "IsSasStartupQueueEmpty";
			ASSERT_TRUE(initializeLastCoinsPlayedAndWon) << "initializeLastCoinsPlayedAndWon";
			ASSERT_TRUE(sendOfflineCoinIn) << "sendOfflineCoinIn";
		}
// #pragma endregion OfflineCoinIn

// #pragma region GetLastGameNumber
		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetLastGameNumber()
		/// Test constructor for default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameNumber_Default)
		{
			// Perform operation(s) and test(s).
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_EQ(m_lastGameNumberDefault, obj.GetLastGameNumber()) << "GetLastGameNumber";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession(const CPlayerSession &)
		///     CPlayerSession::GetLastGameNumber()
		/// Test copy constructor with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameNumber_Copy)
		{
			// Data.
			int gameNumber(43);

			// Setup.
			CPlayerSession sourceObj;
			sourceObj.SetLastGameNumber(gameNumber);

			// Perform operation(s) and test(s).
			CPlayerSession obj(sourceObj);
			ASSERT_EQ(gameNumber, obj.GetLastGameNumber()) << "GetLastGameNumber";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::operator=()
		///     CPlayerSession::GetLastGameNumber()
		/// Test assignment operator with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameNumber_Assignment)
		{
			// Data.
			int gameNumberSetup(121);
			int gameNumber(43);

			// Setup.
			CPlayerSession sourceObj;
			sourceObj.SetLastGameNumber(gameNumber);

			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetLastGameNumber(gameNumberSetup);
			ASSERT_EQ(gameNumberSetup, obj.GetLastGameNumber()) << "GetLastGameNumber setup"; // Verify Setup.

			// Perform operation(s) and test(s).
			obj = sourceObj;
			ASSERT_EQ(gameNumber, obj.GetLastGameNumber()) << "GetLastGameNumber";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::SetLastGameNumber()
		///     CPlayerSession::GetLastGameNumber()
		/// Test with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameNumber_Set)
		{
			// Data.
			int gameNumber(43);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.SetLastGameNumber(gameNumber);
			ASSERT_EQ(gameNumber, obj.GetLastGameNumber()) << "GetLastGameNumber";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GetLastGameNumber()
		/// Test that non-default value is reset to default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameNumber_GameStarted)
		{
			// Data.
			int gameNumber(43);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetLastGameNumber(gameNumber);

			// Perform operation(s) and test(s).
			obj.GameStarted();
			ASSERT_EQ(m_lastGameNumberDefault, obj.GetLastGameNumber()) << "GetLastGameNumber";
		}
// #pragma endregion GetLastGameNumber

// #pragma region GetLastCoinsPlayed
		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetLastCoinsPlayed()
		/// Test constructor for default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastCoinsPlayed_Default)
		{
			// Perform operation(s) and test(s).
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_EQ(m_lastCoinsPlayedDefault, obj.GetLastCoinsPlayed()) << "GetLastCoinsPlayed";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::SetLastCoinsPlayed()
		///     CPlayerSession::GetLastCoinsPlayed()
		/// Test with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastCoinsPlayed_Set)
		{
			// Data.
			long coinsPlayed(20);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.SetLastCoinsPlayed(coinsPlayed);
			ASSERT_EQ(coinsPlayed, obj.GetLastCoinsPlayed()) << "GetLastCoinsPlayed";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::UpdateConfigItems()
		///     CPlayerSession::GetLastCoinsPlayed()
		/// Test with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastCoinsPlayed_UpdateConfigItems)
		{
			// Data.
			long coinsPlayed(20);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			CConfig config(false,"");
			config.SetLastCoinsPlayed(coinsPlayed);

			// Perform operation(s) and test(s).
			obj.UpdateConfigItems(config);
			ASSERT_EQ(coinsPlayed, obj.GetLastCoinsPlayed()) << "GetLastCoinsPlayed";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession(const CPlayerSession &)
		///     CPlayerSession::GetLastCoinsPlayed()
		/// Test copy constructor with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastCoinsPlayed_Copy)
		{
			// Data.
			long coinsPlayed(20);

			// Setup.
			CPlayerSession sourceObj;
			sourceObj.SetLastCoinsPlayed(coinsPlayed);

			// Perform operation(s) and test(s).
			CPlayerSession obj(sourceObj);
			ASSERT_EQ(coinsPlayed, obj.GetLastCoinsPlayed()) << "GetLastCoinsPlayed";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::operator=()
		///     CPlayerSession::GetLastCoinsPlayed()
		/// Test assignment operator with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastCoinsPlayed_Assignment)
		{
			// Data.
			long coinsPlayedSetup(100);
			long coinsPlayed(20);

			// Setup.
			CPlayerSession sourceObj;
			sourceObj.SetLastCoinsPlayed(coinsPlayed);

			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetLastCoinsPlayed(coinsPlayedSetup);
			ASSERT_EQ(coinsPlayedSetup, obj.GetLastCoinsPlayed()) << "GetLastCoinsPlayed setup"; // Verify Setup.

			// Perform operation(s) and test(s).
			obj = sourceObj;
			ASSERT_EQ(coinsPlayed, obj.GetLastCoinsPlayed()) << "GetLastCoinsPlayed";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GetLastCoinsPlayed()
		/// Test that non-default value is reset to default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastCoinsPlayed_GameStarted)
		{
			// Data.
			long coinsPlayed(20);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetLastCoinsPlayed(coinsPlayed);

			// Perform operation(s) and test(s).
			obj.GameStarted();
			ASSERT_EQ(m_lastCoinsPlayedDefault, obj.GetLastCoinsPlayed()) << "GetLastCoinsPlayed";
		}
// #pragma endregion GetLastCoinsPlayed

// #pragma region GetLastDenom
		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetLastDenom()
		/// Test constructor for default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastDenom_Default)
		{
			// Perform operation(s) and test(s).
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_EQ(m_lastDenomDefault, obj.GetLastDenom()) << "GetLastDenom";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession(const CPlayerSession &)
		///     CPlayerSession::GetLastDenom()
		/// Test copy constructor with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastDenom_Copy)
		{
			// Data.
			int denom(43);

			// Setup.
			CPlayerSession sourceObj;
			sourceObj.SetLastDenom(denom);

			// Perform operation(s) and test(s).
			CPlayerSession obj(sourceObj);
			ASSERT_EQ(denom, obj.GetLastDenom()) << "GetLastDenom";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::operator=()
		///     CPlayerSession::GetLastDenom()
		/// Test assignment operator with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastDenom_Assignment)
		{
			// Data.
			int denomSetup(121);
			int denom(43);

			// Setup.
			CPlayerSession sourceObj;
			sourceObj.SetLastDenom(denom);

			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetLastDenom(denomSetup);
			ASSERT_EQ(denomSetup, obj.GetLastDenom()) << "GetLastDenom setup"; // Verify Setup.

			// Perform operation(s) and test(s).
			obj = sourceObj;
			ASSERT_EQ(denom, obj.GetLastDenom()) << "GetLastDenom";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::SetLastDenom()
		///     CPlayerSession::GetLastDenom()
		/// Test with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastDenom_Set)
		{
			// Data.
			int denom(43);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.SetLastDenom(denom);
			ASSERT_EQ(denom, obj.GetLastDenom()) << "GetLastDenom";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GetLastDenom()
		/// Test that non-default value is reset to default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastDenom_GameStarted)
		{
			// Data.
			int denom(43);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetLastDenom(denom);

			// Perform operation(s) and test(s).
			obj.GameStarted();
			ASSERT_EQ(m_lastDenomDefault, obj.GetLastDenom()) << "GetLastDenom";
		}
// #pragma endregion GetLastDenom

        /// <summary>
		/// Functional test for:
		///     CPlayerSession::GetSubscriptionDataResponse() 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetSubscriptionDataResponse_CorrectTopic)
		{
            // Setup			
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CCard pcard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
			pcard.SetCardID("1234567890");
			pcard.SetCardType(CARD_PLAYER);
            CGames games;
			CPlayer player(new CCard(pcard), NULL, config, egmConfig, CTIME_DEFAULT_VALUE, false, &games);
            CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
            obj.SetPlayerState(&player, true);

			string expected = string("{\"PlaySession\":{\"PlayerType\":\"Carded\",\"PlayerName\":\"Guest\",\"PlayerSessionID\":1,") +
				string("\"IsVirtualSession\":false,\"IsPhysicalCardSession\":true,\"SessionInactive\":false}}");
            string result = obj.GetSubscriptionDataResponse("PlaySession", "");
            ASSERT_EQ(0, result.compare(expected));
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GetSubscriptionDataResponse() 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetSubscriptionDataResponse_CorrectTopic_MobileSession)
		{
			// Setup			
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CCard pcard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
			pcard.SetCardID("1234567890");
			pcard.SetCardType(CARD_PLAYER);
			CGames games;
			CPlayer player(new CCard(pcard), NULL, config, egmConfig, CTIME_DEFAULT_VALUE, false, &games);
			player.SetMobilePlayer();
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetPlayerState(&player, false);

			string expected = string("{\"PlaySession\":{\"PlayerType\":\"Mobile\",\"PlayerName\":\"Guest\",\"PlayerSessionID\":1,") +
				string("\"IsVirtualSession\":false,\"IsPhysicalCardSession\":false,\"SessionInactive\":false}}");
			string result = obj.GetSubscriptionDataResponse("PlaySession", "");
			printf("result : '%s'\n", result.c_str());
			printf("expected : '%s'\n", expected.c_str());
			ASSERT_EQ(0, result.compare(expected));
		}

        /// <summary>
		/// Functional test for:
		///     CPlayerSession::GetSubscriptionDataResponse() 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetSubscriptionDataResponse_InCorrectTopic)
		{						
            CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

            string expected = obj.GetSubscriptionDataResponse("PlayerSession", "");
            ASSERT_TRUE(expected.empty());
		}

        /// <summary>
		/// Functional test for:
		///     CPlayerSession::GetPlayerType() 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetPlayerType_Carded)
		{						
            // Setup			
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CCard pcard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
			pcard.SetCardID("1234567890");
			pcard.SetCardType(CARD_PLAYER);
            CGames games;
			CPlayer player(new CCard(pcard), NULL, config, egmConfig, CTIME_DEFAULT_VALUE, false, &games);
            CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
            obj.SetPlayerState(&player);

			string result = "Carded";
            string expected = GetPlayerTypeTest(obj);
            ASSERT_EQ(0, result.compare(expected));
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GetPlayerType() 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetPlayerType_Mobile)
		{
			// Setup			
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CCard pcard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
			pcard.SetCardID("1234567890");
			pcard.SetCardType(CARD_PLAYER);
			CGames games;
			CPlayer player(new CCard(pcard), NULL, config, egmConfig, CTIME_DEFAULT_VALUE, false, &games);
			player.SetMobilePlayer();
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetPlayerState(&player);

			string actual = obj.GetPlayerType();
			ASSERT_STREQ(actual.c_str(), "Mobile") << "GetPlayerType";
		}

        /// <summary>
		/// Functional test for:
		///     CPlayerSession::GetPlayerType() 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetPlayerType_Uncarded)
		{						
            CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
            bool sessionActiveInfoChanged(false), creditsIncreased(false);
            obj.SetCreditsInPennies(5, sessionActiveInfoChanged, creditsIncreased);
			string result = "Uncarded";
            string expected = GetPlayerTypeTest(obj);
            ASSERT_EQ(0, result.compare(expected));
		}

        /// <summary>
		/// Functional test for:
		///     CPlayerSession::GetPlayerType() 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetPlayerType_Idle)
		{						
            CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
            string result = "Idle";
            string expected = GetPlayerTypeTest(obj);
            ASSERT_EQ(0, result.compare(expected));
		}
		
        /// <summary>
        /// Functional test for:
        ///     CPlayerSession:: IsGameSessionIdIncremented() 
        ///  GameSessionId should increment on no game
        /// </summary>
		TEST_F(PlayerSessionUnitTests, NoGameSessionIdIncrement)
		{						
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			ASSERT_EQ(obj.GetGameSessionId(), GAME_SESSION_ID_DEFAULT);

            CGames games;
			obj.IncrementGameSessionIdIfRequired(games);
			
			ASSERT_EQ(obj.GetGameSessionId(), GAME_SESSION_ID_DEFAULT);
		}

        /// <summary>
        /// Functional test for:
        ///     CPlayerSession:: IsGameSessionIdIncremented() 
        ///  GameSessionId should increment on game change
        /// </summary>
		TEST_F(PlayerSessionUnitTests, IsGameSessionIdIncremented_ChangeGame)
		{						
            CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			ASSERT_EQ(obj.GetGameSessionId(), GAME_SESSION_ID_DEFAULT);

			const DWORD gameId1(0);

			// Add a new game and set it as current
			CGames games;
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetGameNumber(gameId1);
			egmGame->ChangeCurrentGameStatus(true);
			games.AddGame(egmGame);

			obj.IncrementGameSessionIdIfRequired(games);
			
			ASSERT_EQ(obj.GetGameSessionId(), GAME_SESSION_ID_DEFAULT + 1);
		}

		
        /// <summary>
        /// Functional test for:
        ///     CPlayerSession:: IsGameSessionIdIncremented()
        ///  GameSessionId should increment on denom change
        /// </summary>
		TEST_F(PlayerSessionUnitTests, IsGameSessionIdIncremented_ChangeDenom)
		{						
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			ASSERT_EQ(obj.GetGameSessionId(), GAME_SESSION_ID_DEFAULT);

			const DWORD gameId1(0);

			// Add a new game and set it as current
			CGames games;
			games.SetCurrentDenom(12);
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetGameNumber(gameId1);
			egmGame->ChangeCurrentGameStatus(true);
			games.AddGame(egmGame);

			obj.IncrementGameSessionIdIfRequired(games);
			
			ASSERT_EQ(obj.GetGameSessionId(), GAME_SESSION_ID_DEFAULT + 1);
		}

		/// <summary>
		/// Functional test for:
		/// 	CPlayerSession:: IncrementPlayTransactionSequenceNumber()
		///  Increment Play Transaction Sequence Number by one
		/// </summary>
		TEST_F(PlayerSessionUnitTests, IncrementPlayTransactionSequenceNumber)
		{	
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.IncrementPlayTransactionSequenceNumber();

			ASSERT_EQ(obj.GetPlayTransactionSequenceNumber(), 1);
		}

// #pragma region CPlayerSession::GetEgmSendsGameStart
		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetEgmSendsGameStart()
		/// Test default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameStart_Default)
		{
			ASSERT_EQ(m_egmSendsGameStartDefault, EGM_SENDS_GAME_START_DEFAULT);

			// Perform operation(s) and test(s).
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_EQ(m_egmSendsGameStartDefault, obj.GetEgmSendsGameStart());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession(const CPlayerSession &)
		///     CPlayerSession::GetEgmSendsGameStart()
		/// Test copy constructor with a false value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameStart_CopyFalse)
		{
			// Setup
			CPlayerSession objA;
			objA.GameEnded();
			AssertResult2(objA.GetEgmSendsGameStart(), false); // Verify setup.

			// Perform operation(s) and test(s).
			CPlayerSession obj(objA);
			AssertResult2(obj.GetEgmSendsGameStart(), false);
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession(const CPlayerSession &)
		///     CPlayerSession::GetEgmSendsGameStart()
		/// Test copy constructor with a true value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameStart_CopyTrue)
		{
			// Setup
			CPlayerSession objA;
			objA.GameStarted();
			objA.GameEnded();
			AssertResult2(objA.GetEgmSendsGameStart(), true); // Verify setup.

			// Perform operation(s) and test(s).
			CPlayerSession obj(objA);
			AssertResult2(obj.GetEgmSendsGameStart(), true);
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::operator=()
		///     CPlayerSession::GetEgmSendsGameStart()
		/// Test assignment operator with a false value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameStart_AssignmentFalse)
		{
			// Setup
			CPlayerSession objA;
			objA.GameEnded();
			AssertResult2(objA.GetEgmSendsGameStart(), false); // Verify setup.

			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.GameStarted();
			AssertResult2(obj.GetEgmSendsGameStart(), true); // Verify setup.

			// Perform operation(s) and test(s).
			obj = objA;
			AssertResult2(obj.GetEgmSendsGameStart(), false);
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::operator=()
		///     CPlayerSession::GetEgmSendsGameStart()
		/// Test assignment operator with a true value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameStart_AssigmentTrue)
		{
			// Setup
			CPlayerSession objA;
			objA.GameStarted();
			AssertResult2(objA.GetEgmSendsGameStart(), true); // Verify setup.

			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.GameEnded();
			AssertResult2(obj.GetEgmSendsGameStart(), false); // Verify setup.

			// Perform operation(s) and test(s).
			obj = objA;
			AssertResult2(obj.GetEgmSendsGameStart(), true);
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameEnded()
		///     CPlayerSession::GetEgmSendsGameStart()
		/// Test with consecutive game starts.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameStart_False)
		{
			// Setup
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.GameEnded();
			AssertResult2(obj.GetEgmSendsGameStart(), false);

			for (; TestIndex < 10; ++TestIndex)
			{
				obj.GameEnded();
				AssertResult2(obj.GetEgmSendsGameStart(), false);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GetEgmSendsGameStart()
		/// Test with consecutive game ends.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameStart_True1)
		{
			// Setup
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.GameStarted();
			AssertResult2(obj.GetEgmSendsGameStart(), true);

			for (; TestIndex < 10; ++TestIndex)
			{
				obj.GameStarted();
				AssertResult2(obj.GetEgmSendsGameStart(), true);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GameEnded()
		///     CPlayerSession::GetEgmSendsGameStart()
		/// Test with game starts followed by game ends.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetEgmSendsGameStart_True2)
		{
			// Setup
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.GameStarted();
			obj.GameEnded();
			AssertResult2(obj.GetEgmSendsGameStart(), true);

			for (; TestIndex < 10; ++TestIndex)
			{
				obj.GameStarted();
				AssertResult2(obj.GetEgmSendsGameStart(), true);
				obj.GameEnded();
				AssertResult2(obj.GetEgmSendsGameStart(), true);
			}
		}
// #pragma endregion CPlayerSession::GetEgmSendsGameStart

// #pragma region GetLastGameSessionGameNumber
		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetLastGameSessionGameNumber()
		/// Test constructor for default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameSessionGameNumber_Default)
		{
			// Perform operation(s) and test(s).
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_EQ(m_lastGameSessionGameNumberDefault, obj.GetLastGameSessionGameNumber()) << "GetLastGameSessionGameNumber";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession(const CPlayerSession &)
		///     CPlayerSession::GetLastGameSessionGameNumber()
		/// Test copy constructor with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameSessionGameNumber_Copy)
		{
			// Data.
			int gameNumber(43);

			// Setup.
			CPlayerSession sourceObj;
			sourceObj.SetLastGameSessionGameNumber(gameNumber);

			// Perform operation(s) and test(s).
			CPlayerSession obj(sourceObj);
			ASSERT_EQ(gameNumber, obj.GetLastGameSessionGameNumber()) << "GetLastGameSessionGameNumber";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::operator=()
		///     CPlayerSession::GetLastGameSessionGameNumber()
		/// Test assignment operator with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameSessionGameNumber_Assignment)
		{
			// Data.
			int gameNumberSetup(121);
			int gameNumber(43);

			// Setup.
			CPlayerSession sourceObj;
			sourceObj.SetLastGameSessionGameNumber(gameNumber);

			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetLastGameSessionGameNumber(gameNumberSetup);
			ASSERT_EQ(gameNumberSetup, obj.GetLastGameSessionGameNumber()) << "GetLastGameSessionGameNumber setup"; // Verify Setup.

			// Perform operation(s) and test(s).
			obj = sourceObj;
			ASSERT_EQ(gameNumber, obj.GetLastGameSessionGameNumber()) << "GetLastGameSessionGameNumber";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::SetLastGameSessionGameNumber()
		///     CPlayerSession::GetLastGameSessionGameNumber()
		/// Test with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameSessionGameNumber_Set)
		{
			// Data.
			int gameNumber(43);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.SetLastGameSessionGameNumber(gameNumber);
			ASSERT_EQ(gameNumber, obj.GetLastGameSessionGameNumber()) << "GetLastGameSessionGameNumber";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GetLastGameSessionGameNumber()
        /// Test that non-default value should not reset to default 
        /// value on game start. 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameSessionGameNumber_GameStarted)
		{
			// Data.
			int gameNumber(43);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetLastGameSessionGameNumber(gameNumber);

			// Perform operation(s) and test(s).
			obj.GameStarted();
			ASSERT_EQ(gameNumber, obj.GetLastGameSessionGameNumber()) << "GetLastGameSessionGameNumber";
		}
// #pragma endregion GetLastGameSessionGameNumber

// #pragma region GetLastGameSessionGameDenom
		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession()
		///     CPlayerSession::GetLastGameSessionGameDenom()
		/// Test constructor for default value.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameSessionGameDenom_Default)
		{
			// Perform operation(s) and test(s).
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			ASSERT_EQ(m_lastGameSessionGameDenomDefault, obj.GetLastGameSessionGameDenom()) << "GetLastGameSessionGameDenom";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::CPlayerSession(const CPlayerSession &)
		///     CPlayerSession::GetLastGameSessionGameDenom()
		/// Test copy constructor with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameSessionGameDenom_Copy)
		{
			// Data.
			int denom(43);

			// Setup.
			CPlayerSession sourceObj;
			sourceObj.SetLastGameSessionGameDenom(denom);

			// Perform operation(s) and test(s).
			CPlayerSession obj(sourceObj);
			ASSERT_EQ(denom, obj.GetLastGameSessionGameDenom()) << "GetLastGameSessionGameDenom";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::operator=()
		///     CPlayerSession::GetLastGameSessionGameDenom()
		/// Test assignment operator with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameSessionGameDenom_Assignment)
		{
			// Data.
			int denomSetup(121);
			int denom(43);

			// Setup.
			CPlayerSession sourceObj;
			sourceObj.SetLastGameSessionGameDenom(denom);

			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetLastGameSessionGameDenom(denomSetup);
			ASSERT_EQ(denomSetup, obj.GetLastGameSessionGameDenom()) << "GetLastGameSessionGameDenom setup"; // Verify Setup.

			// Perform operation(s) and test(s).
			obj = sourceObj;
			ASSERT_EQ(denom, obj.GetLastGameSessionGameDenom()) << "GetLastGameSessionGameDenom";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::SetLastGameSessionGameDenom()
		///     CPlayerSession::GetLastGameSessionGameDenom()
		/// Test with non-default data.
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameSessionGameDenom_Set)
		{
			// Data.
			int denom(43);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);

			// Perform operation(s) and test(s).
			obj.SetLastGameSessionGameDenom(denom);
			ASSERT_EQ(denom, obj.GetLastGameSessionGameDenom()) << "GetLastGameSessionGameDenom";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GetLastGameSessionGameDenom()
        /// Test that non-default value should not reset to default 
        /// value on game start. 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, GetLastGameSessionGameDenom_GameStarted)
		{
			// Data.
			int denom(43);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetLastGameSessionGameDenom(denom);

			// Perform operation(s) and test(s).
			obj.GameStarted();
			ASSERT_EQ(denom, obj.GetLastGameSessionGameDenom()) << "GetLastGameSessionGameDenom";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GetLastPlayedGameSessionGameDenom()
		///     CPlayerSession::SetLastPlayedGameSessionGameNumberAndDenom()
        /// Test that Played game number and denom returns zeros if no game exists. 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, SetLastPlayedGameSessionGameNumberAndDenom_NoGame)
		{
			// Data.			
			CGames games;
			
			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetLastPlayedGameSessionGameNumberAndDenom(games);

			ASSERT_EQ(0, obj.GetLastPlayedGameSessionGameNumber()) << "GetLastPlayedGameSessionGameNumber";
			ASSERT_EQ(1, obj.GetLastPlayedGameSessionGameDenom()) << "GetLastPlayedGameSessionGameDenom";
		}
		
		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GameStarted()
		///     CPlayerSession::GetLastPlayedGameSessionGameDenom()
		///     CPlayerSession::SetLastPlayedGameSessionGameNumberAndDenom()
        /// Test that Played game number and denom updated if exists. 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, SetLastPlayedGameSessionGameNumberAndDenom_OneGame)
		{
			// Data.
			int denom(43);
			int gameId1(1);
			
			CGames games;
			games.SetCurrentDenom(denom);
			CGame *egmGame(new CGame(MEMORY_NONE));
			egmGame->SetGameNumber(gameId1);
			egmGame->ChangeCurrentGameStatus(true);
			games.AddGame(egmGame);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetLastPlayedGameSessionGameNumberAndDenom(games);

			ASSERT_EQ(gameId1, obj.GetLastPlayedGameSessionGameNumber()) << "GetLastPlayedGameSessionGameNumber";
			ASSERT_EQ(denom, obj.GetLastPlayedGameSessionGameDenom()) << "GetLastPlayedGameSessionGameDenom";
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::GetLastPlayedGameSessionGameDenom()
		///     CPlayerSession::SetLastPlayedGameSessionGameNumberAndDenom()
		/// Test that last played game number and denom are set for EGM_GAME in case there is no other subgame. 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, SetLastPlayedGameSessionGameNumberAndDenom_EGMGame)
		{
			// Data.
			int denom(43);

			CGames games;
			games.SetCurrentDenom(denom);
			CGame *egmGame(new CGame(MEMORY_NONE, true));
			games.AddGame(egmGame);

			// Setup.
			CPlayerSession obj;
			CSentinelDlgState uiState;
			obj.SetUiStateObject(&uiState);
			obj.SetLastPlayedGameSessionGameNumberAndDenom(games);

			ASSERT_EQ(EGM_GAME_NUMBER, obj.GetLastPlayedGameSessionGameNumber()) << "GetLastPlayedGameSessionGameNumber";
			ASSERT_EQ(denom, obj.GetLastPlayedGameSessionGameDenom()) << "GetLastPlayedGameSessionGameDenom";
		}
// #pragma endregion GetLastGameSessionGameDenom

// #pragma region SetJackpotPending
		/// <summary>
		/// Functional test for:
		///     CPlayerSession::SetJackpotPending()
		/// Test SetJackpotPending method. 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, SetJackpotPending_True)
		{
			// Setup.
			CPlayerSession obj;

			// Perform operation and test.
			obj.SetJackpotPending(true);
			ASSERT_TRUE(GetJackpotPending(obj));
			ASSERT_TRUE(obj.CheckIfHandPayResetProcessingIsNeededAfterGameStart());
		}

		/// <summary>
		/// Functional test for:
		///     CPlayerSession::SetJackpotPending()
		/// Test SetJackpotPending method. 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, SetJackpotPending_False)
		{
			// Setup.
			CPlayerSession obj;

			// Perform operation and test.
			obj.SetJackpotPending(false);
			ASSERT_FALSE(GetJackpotPending(obj));
			ASSERT_FALSE(obj.CheckIfHandPayResetProcessingIsNeededAfterGameStart());
		}
// #pragma endregion SetJackpotPending

// #pragma region PlayerActivityOccurred
		/// <summary>
		/// Functional test for:
		///     CardoutBeforeGameEnd_VerifyNoCardlessSession
		/// Verify that no cardless session exists when game is started in carded play but 
		/// no card is inserted at the time when game ended (cardout already done) 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, CardoutBeforeGameEnd_VerifyNoCardlessSession)
		{
			// Setup			
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CCard pcard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
			pcard.SetCardID("1234567890");
			pcard.SetCardType(CARD_PLAYER);
			CGames games;
			CPlayer player(new CCard(pcard), NULL, config, egmConfig, CTIME_DEFAULT_VALUE, false, &games);
			CPlayerSession ps;
			CSentinelDlgState uiState;
			ps.SetUiStateObject(&uiState);
			
			// Simulate player card-in
			ps.SetPlayerState(&player);

			// Simulate a game play within carded session 
			ps.GameStarted();
			
			// Simulate player card-out before game ends
			ps.SetPlayerState(nullptr);

			// Simulate GameEnd happenning after cardout
			ps.GameEnded();
			
			// Ensure that no session exists at this time, cardless or carded
			ASSERT_EQ(PLAYERTYPEIDLE, ps.GetPlayerTypeEnum()) << "GetPlayerTypeEnum";
		}
		
		/// <summary>
		/// Functional test for:
		///     CardInCardoutDuringGamePlay_VerifyNoCardlessSession()
		/// Verify that no cardless session exists when game is started without player 
		///	card-in but cardin/cardout was done during the game in progress 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, CardInCardoutDuringGamePlay_VerifyNoCardlessSession)
		{
			// Setup			
			CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CCard pcard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
			pcard.SetCardID("1234567890");
			pcard.SetCardType(CARD_PLAYER);
			CGames games;
			CPlayer player(new CCard(pcard), NULL, config, egmConfig, CTIME_DEFAULT_VALUE, false, &games);
			CPlayerSession ps;
			CSentinelDlgState uiState;
			ps.SetUiStateObject(&uiState);
			
			// Simulate a game play without player card-in so that cardless session starts 
			ps.SetPlayerState(nullptr);
			ps.GameStarted();
			
			// Simulate player card-in before the game ends
			ps.SetPlayerState(&player);
			
			// Simulate player card-out also before game ends
			ps.SetPlayerState(nullptr);
			
			// Ensure that no session exists at this time, cardless or carded
			ASSERT_EQ(PLAYERTYPEIDLE, ps.GetPlayerTypeEnum()) << "GetPlayerTypeEnum";
		}
		
		/// <summary>
		/// Functional test for:
		///     CPlayerSession::TransformInsertedPlayerCard()
		/// Test TransformInsertedPlayerCard method. 
		/// </summary>
		TEST_F(PlayerSessionUnitTests, TransformInsertedPlayerCardTests)
		{
			typedef struct
			{
				// Inputs.
				const bool isCardInserted;
				const bool isVirtual;
				const bool isMobile;
				const bool activePlayer;
				const bool activeEmployee;
				const bool hasCredits;
				const bool gameInProgress;
				const string currentCard;
				const string newCard;

				// Expected result.
				const InsertTranslation insertTranslationExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				// isCardInserted  isVirtual	isMobile	activePlayer	activeEmployee	hasCredits	gameInProgress	currentCard		newCard			insertTranslationExpected	
				{  true,		   false,		false,		true,			false,			false,		false,			"1234567890",	"1234567890",	StayCarded		},//INDEX 0
				{  true,		   false,		true,		true,			false,			false,		false,			"1234567890",	"1234567890",	Ignored			},//INDEX 1
				{  false,		   false,		false,		true,			false,			false,		false,			"1234567890",	"1234567890",	Removed			},//INDEX 2
				{  false,		   false,		false,		true,			false,			true,		false,			"1234567890",	"1234567891",	Ignored			},//INDEX 3
				{  false,		   false,		true,		true,			false,			false,		false,			"1234567890",	"1234567891",	Ignored			},//INDEX 4
				{ false,		   false,		false,		true,			false,			false,		true,			"1234567890",	"1234567891",	Ignored			},//INDEX 5
				{ false,		   false,		false,		true,			false,			false,		false,			"1234567890",	"1234567891",	RemovedInserted },//INDEX 6
				{ false,		   false,		true,		true,			false,			false,		false,			"1234567890",	"1234567891",	Ignored			},//INDEX 7
				{ false,		   true,		true,		true,			true,			false,		false,			"1234567890",	"1234567891",	Ignored			},//INDEX 8
				{ false,		   true,		true,		true,			false,			false,		false,			"1234567890",	"1234567891",	Ignored			},//INDEX 9
				{ true,			   true,		false,		true,			false,			false,		false,			"1234567890",	"1234567891",	Inserted		},//INDEX 10
				{ true,			   true,		true,		false,			false,			false,		false,			"1234567890",	"1234567891",	Inserted		},//INDEX 11
			};

			//InsertTranslation CPlayerSession::TransformInsertedPlayerCard(bool isCardInserted, CEmployee *employee, CPlayer *player, CCard *card)
			//{
			//	// Default action "Inserted" so unknown card types will get the bad card read treatment.
			//	InsertTranslation ret = Inserted;

			//	if (player != NULL && !player->IsVirtual())
			//	{
			//		if (player->CompareCard(card))
			//		{
			//			// Cards match
			//			if (isCardInserted && !m_mobilePlayer)
			//			{
			//				ret = StayCarded;//INDEX 0
			//			}
			//			else if (m_mobilePlayer)
			//			{
			//				ret = Ignored;//INDEX 1
			//			}
			//			else
			//			{// RFID will remove
			//				ret = Removed;//INDEX 2
			//			}
			//		}
			//		else
			//		{
			//			// Cards are different, ignore the new card if we think there are still significant credits on the EGM.
			//			// Also, ignore the new card if carding out the old card would create a virtual session for the old card.
			//			if (HasSignificantCredits() || (!m_virtualSessionEnabled && m_miniVirtualSessionEnabled && m_gameInProgress) || m_mobilePlayer)//INDEX 3,4,5
			//			{
			//				ret = Ignored;
			//			}
			//			else
			//			{
			//				ret = RemovedInserted;//INDEX 6
			//			}
			//		}
			//	}
			//	else
			//	{
			//		// No player already inserted
			//		if (employee != NULL)
			//		{
			//			ret = Ignored;//INDEX 7
			//		}
			//		else if (player != NULL && player->IsVirtual() && !player->CompareCard(card))
			//		{
			//			// New card does not match existing virtual session.
			//			if (//INDEX 8 !isCardInserted || //INDEX 9 m_mobilePlayer)
			//			{
			//				// Player Session - RFID.
			//				// During a player virtual session, a swipe by another player will be ignored. The virtual session with the original player shall continue.
			//				ret = Ignored;
			//			}
			//			else
			//			{
			//				ret = Inserted;//INDEX 10
			//			}
			//		}
			//		else
			//		{
			//			ret = Inserted;
			//		}
			//	}

			//	return ret;

			//}


			for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				CGames games;
				CSentinelDlgState uiState;
				CCard currentCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
				currentCard.SetCardID(TEST_DATA[testIndex].currentCard);
				currentCard.SetCardType(CARD_PLAYER);
				CConfig config(false, "");
				config.SetVirtualSessionEnabled(false);
				config.SetMiniVirtualSessionEnabled(true);
				config.SetNegligibleCreditsThreshold(0);
				config.SetGameInProgress(TEST_DATA[testIndex].gameInProgress);
				const CEGMConfig egmConfig(MEMORY_NONE);
				CPlayer player(new CCard(currentCard), NULL, config, egmConfig, CTIME_DEFAULT_VALUE, false, &games);
				CPlayerSession obj;
				obj.SetUiStateObject(&uiState);				

				CCard newCard(CTIME_DEFAULT_VALUE, CTIME_DEFAULT_VALUE);
				newCard.SetCardID(TEST_DATA[testIndex].newCard);
				newCard.SetCardType(CARD_PLAYER);

				CPlayer* playerParam(nullptr);
				CEmployee* employeeParam(nullptr);
				if (TEST_DATA[testIndex].activePlayer)
				{
					player.SetVirtualFlag(TEST_DATA[testIndex].isVirtual);
					if (TEST_DATA[testIndex].isMobile)
					{
						player.SetMobilePlayer();
					}
					playerParam = &player;
					obj.SetPlayerState(playerParam);
				}
				obj.UpdateConfigItems(config);

				if (TEST_DATA[testIndex].activeEmployee)
				{
					currentCard.SetCardType(CARD_MECHANIC);
					employeeParam = new CEmployee(new CCard(currentCard), config, false, false, 0, 0);
				}
				
				if (TEST_DATA[testIndex].hasCredits)
				{
					bool sessionActiveInfoChanged(false), creditsIncreased(false);
					obj.SetCreditsInPennies(10, sessionActiveInfoChanged, creditsIncreased);
				}

				// Perform operation(s) to be tested.
				InsertTranslation insertTranslationResult(obj.TransformInsertedPlayerCard(TEST_DATA[testIndex].isCardInserted, employeeParam, playerParam, &newCard));
				{
					// Report
					bool areEqual(TEST_DATA[testIndex].insertTranslationExpected == insertTranslationResult);
					string failedMsg;
					failedMsg = FormatString(_T("FAILED InsertTranslationResult=%d expected=%d testIndex=%d in %s"),
						(int)insertTranslationResult,
						(int)TEST_DATA[testIndex].insertTranslationExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Cleanup.
				delete employeeParam;
			}
		}
// #pragma endregion TransformInsertedPlayerCard
}

