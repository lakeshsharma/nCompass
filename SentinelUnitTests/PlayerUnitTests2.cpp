#include "stdafx.h"
#include "Player.h"

#include "MultiplierBonusEvent.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "UnitTestingUtils.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	static const time_t TIME_DEFAULT_VALUE(0); // Arbitrary value when any valid value is okay.
	static const byte MEMSET_DEFAULT_VALUE = 0xcd; // Value unlikely to be used for other purposes.

	class Player2 : public ::testing::Test
	{
	public:
		// Default expected result(s).
		static const bool m_isGoodbyeEnabled;
		static const bool m_cardedOutOnEmployeeCardIn;
	};

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::CPlayer()
		/// </summary>
		TEST_F(Player2, CPlayerDefaultTest)
		{
			// Test data -- expected results.
			const MEMORY_TYPE memoryTypeExpected(MEMORY_NONE);

			// Setup.

			// Perform operation(s) to be tested.
			const CPlayer player;

			// Test #1.
			{
				const MEMORY_TYPE memoryTypeResult(player.GetMemoryType());

				bool areEqual(memoryTypeResult == memoryTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%d expected=%d in %s"),
					(int)memoryTypeResult,
					(int)memoryTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::CPlayer()
		///     CPlayer::IsGoodbyeEnabled()
		/// Test for IsGoodbyeEnabled() default value.
		/// </summary>
		TEST_F(Player2, CPlayerDefault_GoodbyeEnabledTest)
		{
			// Perform operation(s) to be tested.
			const CPlayer obj;

			// Test(s).
			ASSERT_EQ(obj.IsGoodbyeEnabled(), m_isGoodbyeEnabled);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::CPlayer()
		///     CPlayer::WasCardedOutOnEmployeeCardIn()
		/// Test for WasCardedOutOnEmployeeCardIn() default value.
		/// </summary>
		TEST_F(Player2, CPlayerDefault_CardedOutOnEmployeeCardInTest)
		{
			// Perform operation(s) to be tested.
			const CPlayer obj;

			// Test(s).
			ASSERT_EQ(obj.WasCardedOutOnEmployeeCardIn(), m_cardedOutOnEmployeeCardIn);
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::CPlayer(CConfig &)
		/// </summary>
		TEST_F(Player2, CPlayerCConfigTest)
		{
			// Test data -- expected results.
			const MEMORY_TYPE memoryTypeExpected = MEMORY_NVRAM;

			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) to be tested.
			const CPlayer player(config, egmConfig);

			// Test #1.
			{
				const MEMORY_TYPE memoryTypeResult(player.GetMemoryType());

				bool areEqual(memoryTypeResult == memoryTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%d expected=%d in %s"),
					(int)memoryTypeResult,
					(int)memoryTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::CPlayer(CConfig &)
		///     CPlayer::IsGoodbyeEnabled()
		/// Test for IsGoodbyeEnabled() default value.
		/// </summary>
		TEST_F(Player2, CPlayerCConfig_GoodbyeEnabledTest)
		{
			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) to be tested.
			const CPlayer obj(config, egmConfig);

			// Test(s).
			ASSERT_EQ(obj.IsGoodbyeEnabled(), m_isGoodbyeEnabled);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::CPlayer(CConfig &)
		///     CPlayer::WasCardedOutOnEmployeeCardIn()
		/// Test for WasCardedOutOnEmployeeCardIn() default value.
		/// </summary>
		TEST_F(Player2, CPlayerCConfig_CardedOutOnEmployeeCardInTest)
		{
			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) to be tested.
			const CPlayer obj(config, egmConfig);

			// Test(s).
			ASSERT_EQ(obj.WasCardedOutOnEmployeeCardIn(), m_cardedOutOnEmployeeCardIn);
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::CPlayer(CCard *, ...)
		/// </summary>
		TEST_F(Player2, CPlayerCCard_NullCardNullPlayerInfoTest)
		{
			// Test data -- expected results.
			const MEMORY_TYPE memoryTypeExpected = MEMORY_NVRAM;

			// Setup.
			CCard *card = NULL;
			CPlayerInfo *playerInfo = NULL;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

            CGames games;
			// Perform operation(s) to be tested.
			const CPlayer player(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, m_isGoodbyeEnabled, &games);

			// Test #1.
			{
				const MEMORY_TYPE memoryTypeResult(player.GetMemoryType());

				bool areEqual(memoryTypeResult == memoryTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%d expected=%d in %s"),
					(int)memoryTypeResult,
					(int)memoryTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::CPlayer(CCard *, ...)
		///     CPlayer::IsGoodbyeEnabled()
		/// Test for IsGoodbyeEnabled() minimum value.
		/// </summary>
		TEST_F(Player2, CPlayerCCard_GoodbyeEnabledMinTest)
		{
			// Test data.
			const bool isGoodbyeEnabled(false);

			// Setup.
			CCard *card = NULL;
			CPlayerInfo *playerInfo = NULL;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

            CGames games;
			// Perform operation(s) to be tested.
			const CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, isGoodbyeEnabled, &games);

			// Test(s).
			ASSERT_EQ(obj.IsGoodbyeEnabled(), isGoodbyeEnabled);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::CPlayer(CCard *, ...)
		///     CPlayer::IsGoodbyeEnabled()
		/// Test for IsGoodbyeEnabled() maximum value.
		/// </summary>
		TEST_F(Player2, CPlayerCCard_GoodbyeEnabledMaxTest)
		{
			// Test data.
			const bool isGoodbyeEnabled(true);

			// Setup.
			CCard *card = NULL;
			CPlayerInfo *playerInfo = NULL;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

            CGames games;
			// Perform operation(s) to be tested.
			const CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, isGoodbyeEnabled, &games);

			// Test(s).
			ASSERT_EQ(obj.IsGoodbyeEnabled(), isGoodbyeEnabled);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::CPlayer(CCard *, ...)
		///     CPlayer::WasCardedOutOnEmployeeCardIn()
		/// Test for WasCardedOutOnEmployeeCardIn() default value.
		/// </summary>
		TEST_F(Player2, CPlayerCCard_CardedOutOnEmployeeCardInTest)
		{
			// Setup.
			CCard *card = NULL;
			CPlayerInfo *playerInfo = NULL;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

            CGames games;
			// Perform operation(s) to be tested.
			const CPlayer obj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, m_isGoodbyeEnabled, &games);

			// Test(s).
			ASSERT_EQ(obj.WasCardedOutOnEmployeeCardIn(), m_cardedOutOnEmployeeCardIn);
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::CPlayer(CCard *, ...)
		/// </summary>
		TEST_F(Player2, CPlayerCCard_NullCardTest)
		{
			// Test data -- expected results.
			const MEMORY_TYPE memoryTypeExpected = MEMORY_NVRAM;

			// Setup.
			CCard *card = NULL;
			CPlayerInfo *playerInfo = new CPlayerInfo();
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

            CGames games;
			// Perform operation(s) to be tested.
			const CPlayer player(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, m_isGoodbyeEnabled, &games);

			// Test #1.
			{
				const MEMORY_TYPE memoryTypeResult(player.GetMemoryType());

				bool areEqual(memoryTypeResult == memoryTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%d expected=%d in %s"),
					(int)memoryTypeResult,
					(int)memoryTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}


		/// <summary>
		/// Code coverage test for:
		///     CPlayer::CPlayer(const CPlayer &)
		/// </summary>
		TEST_F(Player2, CPlayerCopyTest)
		{
			// Test data -- expected results.
			const MEMORY_TYPE memoryTypeExpected = MEMORY_NONE;

			// Setup.
			const CPlayer player1;

			// Perform operation(s) to be tested.
			const CPlayer player(player1);

			// Test #1.
			{
				const MEMORY_TYPE memoryTypeResult(player.GetMemoryType());

				bool areEqual(memoryTypeResult == memoryTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%d expected=%d in %s"),
					(int)memoryTypeResult,
					(int)memoryTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::CPlayer(const CPlayer &)
		///     CPlayer::IsGoodbyeEnabled()
		/// Test for IsGoodbyeEnabled() minimum value.
		/// </summary>
		TEST_F(Player2, CPlayerCopy_GoodbyeEnabledMinTest)
		{
			// Test data.
			const bool isGoodbyeEnabled(false);

			// Setup.
			CCard *card = NULL;
			CPlayerInfo *playerInfo = NULL;
            CGames games;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CPlayer objA(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, isGoodbyeEnabled, &games);

			// Perform operation(s) to be tested.
			const CPlayer obj(objA);

			// Test(s).
			ASSERT_EQ(obj.IsGoodbyeEnabled(), isGoodbyeEnabled);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::CPlayer(const CPlayer &)
		///     CPlayer::IsGoodbyeEnabled()
		/// Test for IsGoodbyeEnabled() maximum value.
		/// </summary>
		TEST_F(Player2, CPlayerCopy_GoodbyeEnabledMaxTest)
		{
			// Test data.
			const bool isGoodbyeEnabled(true);

			// Setup.
			CCard *card = NULL;
			CPlayerInfo *playerInfo = NULL;
            CGames games;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CPlayer objA(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, isGoodbyeEnabled, &games);

			// Perform operation(s) to be tested.
			const CPlayer obj(objA);

			// Test(s).
			ASSERT_EQ(obj.IsGoodbyeEnabled(), isGoodbyeEnabled);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::CPlayer(const CPlayer &)
		///     CPlayer::SetCardedOutOnEmployeeCardIn()
		///     CPlayer::WasCardedOutOnEmployeeCardIn()
		/// Test for WasCardedOutOnEmployeeCardIn() minimum value.
		/// </summary>
		TEST_F(Player2, CPlayerCopy_CardedOutOnEmployeeCardInMinTest)
		{
			// Test data.
			const bool cardedOutOnEmployeeCardIn(false);

			// Setup.
			CPlayer varObjA;
			varObjA.SetCardedOutOnEmployeeCardIn(cardedOutOnEmployeeCardIn);
			const CPlayer &objA(varObjA);

			// Perform operation(s) to be tested.
			const CPlayer obj(objA);

			// Test(s).
			ASSERT_EQ(obj.WasCardedOutOnEmployeeCardIn(), cardedOutOnEmployeeCardIn);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::CPlayer(const CPlayer &)
		///     CPlayer::SetCardedOutOnEmployeeCardIn()
		///     CPlayer::WasCardedOutOnEmployeeCardIn()
		/// Test for WasCardedOutOnEmployeeCardIn() maximum value.
		/// </summary>
		TEST_F(Player2, CPlayerCopy_CardedOutOnEmployeeCardInMaxTest)
		{
			// Test data.
			const bool cardedOutOnEmployeeCardIn(true);

			// Setup.
			CPlayer varObjA;
			varObjA.SetCardedOutOnEmployeeCardIn(cardedOutOnEmployeeCardIn);
			const CPlayer &objA(varObjA);

			// Perform operation(s) to be tested.
			const CPlayer obj(objA);

			// Test(s).
			ASSERT_EQ(obj.WasCardedOutOnEmployeeCardIn(), cardedOutOnEmployeeCardIn);
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::~CPlayer()
		/// </summary>
		TEST_F(Player2, CPlayerDestructor_NullMetersTest)
		{
			// Test data -- expected results.
			const MEMORY_TYPE memoryTypeExpected = MEMORY_NONE;

			// Setup.

			// Perform operation(s) to be tested.
			const CPlayer player;

			// Test #1.
			{
				const MEMORY_TYPE memoryTypeResult(player.GetMemoryType());

				bool areEqual(memoryTypeResult == memoryTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%d expected=%d in %s"),
					(int)memoryTypeResult,
					(int)memoryTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.
			{
				const CMeters *sessionStartMetersResult(player.GetSessionStartMetersCopy());

				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%p expected=NULL in %s"),
					(const void *)sessionStartMetersResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_EQ((void *)nullptr, (void *)sessionStartMetersResult) << failedMsg;

				delete sessionStartMetersResult;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::~CPlayer()
		/// </summary>
		TEST_F(Player2, CPlayerDestructor_MemNoneMetersTest)
		{
			// Test data -- expected results.
			const MEMORY_TYPE memoryTypeExpected = MEMORY_NONE;

			// Setup.
			CMeters *meters = new CMeters(MEMORY_NONE);

			// Perform operation(s) to be tested.
			CPlayer varPlayer;
			varPlayer.SetSessionStartMeters(meters);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const MEMORY_TYPE memoryTypeResult(player.GetMemoryType());

				bool areEqual(memoryTypeResult == memoryTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%d expected=%d in %s"),
					(int)memoryTypeResult,
					(int)memoryTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.
			{
				const CMeters *sessionStartMetersResult(player.GetSessionStartMetersCopy());

				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%p expected=NOT NULL in %s"),
					(const void *)sessionStartMetersResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)sessionStartMetersResult) << failedMsg;

				delete sessionStartMetersResult;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::~CPlayer()
		/// </summary>
		TEST_F(Player2, CPlayerDestructor_MemNvramMetersTest)
		{
			// Test data -- expected results.
			const MEMORY_TYPE memoryTypeExpected = MEMORY_NVRAM;

			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CMeters *meters = new CMeters(MEMORY_NONE);

			// Perform operation(s) to be tested.
			CPlayer varPlayer(config, egmConfig);
			varPlayer.SetSessionStartMeters(meters);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const MEMORY_TYPE memoryTypeResult(player.GetMemoryType());

				bool areEqual(memoryTypeResult == memoryTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%d expected=%d in %s"),
					(int)memoryTypeResult,
					(int)memoryTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.
			{
				const CMeters *sessionStartMetersResult(player.GetSessionStartMetersCopy());

				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%p expected=NOT NULL in %s"),
					(const void *)sessionStartMetersResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)sessionStartMetersResult) << failedMsg;

				delete sessionStartMetersResult;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::operator=(const CPlayer &)
		/// </summary>
		TEST_F(Player2, CPlayerAssignSelfTest)
		{
			// Test data -- expected results.
			const MEMORY_TYPE memoryTypeExpected = MEMORY_NVRAM;

			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CPlayer varPlayer(config, egmConfig);

			// Perform operation(s) to be tested.
			varPlayer = varPlayer;
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const MEMORY_TYPE memoryTypeResult(player.GetMemoryType());

				bool areEqual(memoryTypeResult == memoryTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%d expected=%d in %s"),
					(int)memoryTypeResult,
					(int)memoryTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path(s) coverage test for:
		///     CPlayer::operator=(const CPlayer &)
		///     CPlayer::CopyObj()
		/// Test with:
		///      player2.m_card == NULL
		///      player2.m_playerInfo == NULL
		///      player1.m_bIsVirtual == false
		///      player2.m_bonusEvents == NULL
		/// </summary>
		TEST_F(Player2, CPlayerAssignOther0Test)
		{
			// Test data -- expected results.
			const MEMORY_TYPE memoryTypeExpected = MEMORY_NONE;

			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CPlayer varPlayer2(config, egmConfig);
			CPlayer varPlayer(config, egmConfig);

			// Perform operation(s) to be tested.
			varPlayer = varPlayer2;
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const MEMORY_TYPE memoryTypeResult(player.GetMemoryType());

				bool areEqual(memoryTypeResult == memoryTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%d expected=%d in %s"),
					(int)memoryTypeResult,
					(int)memoryTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path(s) coverage test for:
		///     CPlayer::operator=(const CPlayer &)
		///     CPlayer::CopyObj()
		/// Test with:
		///      player2.m_card == NULL
		///      player2.m_playerInfo == NULL
		///      player1.m_bIsVirtual == true
		///      player2.m_bonusEvents == NULL
		/// </summary>
		TEST_F(Player2, CPlayerAssignOther1Test)
		{
			// Test data -- inputs.
			const time_t removalTime(123);
			const time_t utcRemovalTime(12345);
			const bool bMakeVirtual(true);
			CMeters * const virtualSessionStartMeters = NULL;

			// Test data -- expected results.
			const MEMORY_TYPE memoryTypeExpected = MEMORY_NONE;

			// Setup.
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CPlayer varPlayer2(config, egmConfig);
			CPlayer varPlayer1(config, egmConfig);
			varPlayer1.CardRemoved(removalTime, utcRemovalTime, bMakeVirtual);

			// Perform operation(s) to be tested.
			varPlayer1 = varPlayer2;
			const CPlayer &player(varPlayer1);

			// Test #1.
			{
				const MEMORY_TYPE memoryTypeResult(player.GetMemoryType());

				bool areEqual(memoryTypeResult == memoryTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%d expected=%d in %s"),
					(int)memoryTypeResult,
					(int)memoryTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code path(s) coverage test for:
		///     CPlayer::operator=(const CPlayer &)
		///     CPlayer::CopyObj()
		/// Test with:
		///      player2.m_card != NULL
		///      player2.m_playerInfo != NULL
		///      player1.m_bIsVirtual == true
		///      player2.m_bonusEvents != NULL
		/// </summary>
		TEST_F(Player2, CPlayerAssignOther2Test)
		{
			// Test data -- inputs.
			const time_t removalTime(123);
			const time_t utcRemovalTime(12345);
			const bool bMakeVirtual(true);
			CMeters * const virtualSessionStartMeters = NULL;

			// Test data -- expected results.
			const MEMORY_TYPE memoryTypeExpected = MEMORY_NONE;

			// Setup.
			CCard *card(new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE));
			CPlayerInfo *playerInfo(new CPlayerInfo);
            CGames games;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CBonusEvents bonusEvents;

			CPlayer varPlayer2(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, false, &games);
			varPlayer2.SetBonusEvents(&bonusEvents);
			const CPlayer &player2(varPlayer2);

			CPlayer varPlayer1(config, egmConfig);
			varPlayer1.CardRemoved(removalTime, utcRemovalTime, bMakeVirtual);

			// Perform operation(s) to be tested.
			varPlayer1 = player2;
			const CPlayer &player(varPlayer1);

			// Test #1.
			{
				const MEMORY_TYPE memoryTypeResult(player.GetMemoryType());

				bool areEqual(memoryTypeResult == memoryTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%d expected=%d in %s"),
					(int)memoryTypeResult,
					(int)memoryTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::operator=(const CPlayer &)
		///     CPlayer::CopyObj()
		///     CPlayer::IsGoodbyeEnabled()
		/// Test for IsGoodbyeEnabled() minimum value.
		/// </summary>
		TEST_F(Player2, CPlayerAssign_GoodbyeEnabledMinTest)
		{
			// Test data.
			const bool isGoodbyeEnabled(false);

			// Setup.
			CCard *card = NULL;
			CPlayerInfo *playerInfo = NULL;
            CGames games;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CPlayer objA(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, isGoodbyeEnabled, &games);
			CPlayer varObj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, !isGoodbyeEnabled, &games);

			// Verify setup.
			ASSERT_EQ(varObj.IsGoodbyeEnabled(), !isGoodbyeEnabled);

			// Perform operation(s) to be tested.
			varObj = objA;
			const CPlayer &obj(varObj);

			// Test(s).
			ASSERT_EQ(obj.IsGoodbyeEnabled(), isGoodbyeEnabled);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::operator=(const CPlayer &)
		///     CPlayer::CopyObj()
		///     CPlayer::IsGoodbyeEnabled()
		/// Test for IsGoodbyeEnabled() maximum value.
		/// </summary>
		TEST_F(Player2, CPlayerAssign_GoodbyeEnabledMaxTest)
		{
			// Test data.
			const bool isGoodbyeEnabled(true);

			// Setup.
			CCard *card = NULL;
			CPlayerInfo *playerInfo = NULL;
            CGames games;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CPlayer objA(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, isGoodbyeEnabled, &games);
			CPlayer varObj(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, !isGoodbyeEnabled, &games);

			// Verify setup.
			ASSERT_EQ(varObj.IsGoodbyeEnabled(), !isGoodbyeEnabled);

			// Perform operation(s) to be tested.
			varObj = objA;
			const CPlayer &obj(varObj);

			// Test(s).
			ASSERT_EQ(obj.IsGoodbyeEnabled(), isGoodbyeEnabled);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::operator=(const CPlayer &)
		///     CPlayer::SetCardedOutOnEmployeeCardIn()
		///     CPlayer::WasCardedOutOnEmployeeCardIn()
		/// Test for WasCardedOutOnEmployeeCardIn() minimum value.
		/// </summary>
		TEST_F(Player2, CPlayerAssign_CardedOutOnEmployeeCardInMinTest)
		{
			// Test data.
			const bool cardedOutOnEmployeeCardIn(false);

			// Setup.
			CPlayer varObjA;
			varObjA.SetCardedOutOnEmployeeCardIn(cardedOutOnEmployeeCardIn);
			const CPlayer &objA(varObjA);
			CPlayer varObj;
			varObj.SetCardedOutOnEmployeeCardIn(!cardedOutOnEmployeeCardIn);

			// Verify setup.
			ASSERT_EQ(varObj.WasCardedOutOnEmployeeCardIn(), !cardedOutOnEmployeeCardIn);

			// Perform operation(s) to be tested.
			varObj = objA;
			const CPlayer &obj(varObj);

			// Test(s).
			ASSERT_EQ(obj.WasCardedOutOnEmployeeCardIn(), cardedOutOnEmployeeCardIn);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::operator=(const CPlayer &)
		///     CPlayer::SetCardedOutOnEmployeeCardIn()
		///     CPlayer::WasCardedOutOnEmployeeCardIn()
		/// Test for WasCardedOutOnEmployeeCardIn() maximum value.
		/// </summary>
		TEST_F(Player2, CPlayerAssign_CardedOutOnEmployeeCardInMaxTest)
		{
			// Test data.
			const bool cardedOutOnEmployeeCardIn(true);

			// Setup.
			CPlayer varObjA;
			varObjA.SetCardedOutOnEmployeeCardIn(cardedOutOnEmployeeCardIn);
			const CPlayer &objA(varObjA);
			CPlayer varObj;
			varObj.SetCardedOutOnEmployeeCardIn(!cardedOutOnEmployeeCardIn);

			// Verify setup.
			ASSERT_EQ(varObj.WasCardedOutOnEmployeeCardIn(), !cardedOutOnEmployeeCardIn);

			// Perform operation(s) to be tested.
			varObj = objA;
			const CPlayer &obj(varObj);

			// Test(s).
			ASSERT_EQ(obj.WasCardedOutOnEmployeeCardIn(), cardedOutOnEmployeeCardIn);
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::DoesPlayerExistInNVRAM()
		/// </summary>
		TEST_F(Player2, DoesPlayerExistInNVRAM_TrueTest)
		{
			// Test data -- expected results.
			const bool inNvramExpected = true;

			// Setup.
			CMemoryStorage::SetSprinkles();
			CCard *card = NULL;
			CPlayerInfo *playerInfo = NULL;
            CGames games;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CPlayer player(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, false, &games);

			// Perform operation(s) to be tested.
			const bool inNvramResult = CPlayer::DoesPlayerExistInNVRAM();

			// Test #1.
			{
				bool areEqual(inNvramResult == inNvramExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED inNvramResult=%d expected=%d in %s"),
					(int)inNvramResult,
					(int)inNvramExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::DoesPlayerExistInNVRAM()
		/// Make CMemoryStorage::IntegrityCheck() return true.
		/// </summary>
		TEST_F(Player2, DoesPlayerExistInNVRAM_FalseTest)
		{
			// Test data -- expected results.
			const bool inNvram1Expected = true;
			const bool inNvram2Expected = false;

			// Setup.
			CMemoryStorage::SetSprinkles();
			CCard *card = NULL;
			CPlayerInfo *playerInfo = NULL;
            CGames games;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);

			// Perform operation(s) to be tested.
			bool inNvram1Result = !inNvram1Expected;
			{
				const CPlayer player(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, false, &games);
				inNvram1Result = CPlayer::DoesPlayerExistInNVRAM();
			}
			const bool inNvram2Result = CPlayer::DoesPlayerExistInNVRAM();

			// Test #1.
			{
				bool areEqual(inNvram1Result == inNvram1Expected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED inNvram1Result=%d expected=%d in %s"),
					(int)inNvram1Result,
					(int)inNvram1Expected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #2.
			{
				bool areEqual(inNvram2Result == inNvram2Expected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED inNvram2Result=%d expected=%d in %s"),
					(int)inNvram2Result,
					(int)inNvram2Expected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::DoesPlayerExistInNVRAM()
		/// Make CMemoryStorage::IntegrityCheck() return false.
		/// </summary>
		TEST_F(Player2, DoesPlayerExistInNVRAM_InvalidTest)
		{
			// Test data -- expected results.
			const bool inNvramExpected = false;

			// Setup.
			CMemoryStorage::SetSprinkles();
			CMemoryStorage::SetInactiveLocation(SPRINKLE1_OFFSET, ~SPRINKLE_VALUE);
			CCard *card = NULL;
			CPlayerInfo *playerInfo = NULL;
            CGames games;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			const CPlayer player(card, playerInfo, config, egmConfig, TIME_DEFAULT_VALUE, false, &games);

			// Perform operation(s) to be tested.
			const bool inNvramResult = CPlayer::DoesPlayerExistInNVRAM();

			// Test #1.
			{
				bool areEqual(inNvramResult == inNvramExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED inNvramResult=%d expected=%d in %s"),
					(int)inNvramResult,
					(int)inNvramExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::CardRemoved()
		/// </summary>
		TEST_F(Player2, CardRemoved_NullCardFalseTest)
		{
			// Test data -- inputs.
			const time_t removalTime(123);
			const time_t utcRemovalTime(12345);
			const bool bMakeVirtual(false);
			CMeters * const virtualSessionStartMeters = NULL;

			// Test data -- expected results.
			const time_t virtualStartTimeExpected(0);

			// Setup.
			CPlayer varPlayer;

			// Perform operation(s) to be tested.
			varPlayer.CardRemoved(removalTime, utcRemovalTime, bMakeVirtual);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const CCard * const cardResult(player.GetCard());

				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=%p expected=NULL in %s"),
					(const void *)cardResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_EQ((void *)nullptr, (void *)cardResult) << failedMsg;
			}

			// Test #2.
			{
				const time_t virtualStartTimeResult(player.GetVirtualStartTime());

				bool areEqual(virtualStartTimeResult == virtualStartTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED virtualStartTimeResult=%lld expected=%lld in %s"),
					(long long)virtualStartTimeResult,
					(long long)virtualStartTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::CardRemoved()
		/// </summary>
		TEST_F(Player2, CardRemoved_CardFalseTest)
		{
			// Test data -- inputs.
			const time_t utcTime(1);
			const time_t removalTime(123);
			const time_t utcRemovalTime(12345);
			const bool bMakeVirtual(false);
			CMeters * const virtualSessionStartMeters(NULL);

			// Test data -- expected results.
			const time_t cardOutTimeExpected(removalTime);
			const time_t virtualStartTimeExpected(0);

			// Setup.
			CCard *card = new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE);
			CPlayerInfo *playerInfo(NULL);
            CGames games;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CPlayer varPlayer(card, playerInfo, config, egmConfig, utcTime, false, &games);

			// Perform operation(s) to be tested.
			varPlayer.CardRemoved(removalTime, utcRemovalTime, bMakeVirtual);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CCard * const cardResult(player.GetCard());
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=%p expected=NOT NULL in %s"),
					(const void *)cardResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)cardResult) << failedMsg;
			}

			// Test #2.
			{
				const time_t cardOutTimeResult(cardResult->GetCardOutTime());

				bool areEqual(cardOutTimeResult == cardOutTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardOutTimeResult=%lld expected=%lld in %s"),
					(long long)cardOutTimeResult,
					(long long)cardOutTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			{
				const time_t virtualStartTimeResult(player.GetVirtualStartTime());

				bool areEqual(virtualStartTimeResult == virtualStartTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED virtualStartTimeResult=%lld expected=%lld in %s"),
					(long long)virtualStartTimeResult,
					(long long)virtualStartTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::CardRemoved()
		///     CPlayer::MakeVirtual()
		/// Test MakeVirtual() with:
		///     m_virtualCard == NULL
		///      m_card == NULL
		/// </summary>
		TEST_F(Player2, CardRemoved_NullCardTrueTest)
		{
			// Test data -- inputs.
			const time_t utcRemovalTime(12345);
			const bool bMakeVirtual(true);
			CMeters * const virtualSessionStartMeters = NULL;

			// Test data -- expected results.
			const time_t virtualStartTimeExpected(utcRemovalTime);

			// Setup.
			CPlayer varPlayer;

			// Perform operation(s) to be tested.
			varPlayer.CardRemoved(utcRemovalTime, utcRemovalTime, bMakeVirtual);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const CCard * const cardResult(player.GetCard());

				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=%p expected=NULL in %s"),
					(const void *)cardResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_EQ((void *)nullptr, (void *)cardResult) << failedMsg;
			}

			// Test #2.
			{
				const time_t virtualStartTimeResult(player.GetVirtualStartTime());

				bool areEqual(virtualStartTimeResult == virtualStartTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED virtualStartTimeResult=%lld expected=%lld in %s"),
					(long long)virtualStartTimeResult,
					(long long)virtualStartTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::CardRemoved()
		///     CPlayer::MakeVirtual()
		/// Test MakeVirtual() with:
		///     m_virtualCard == NULL
		///      m_card != NULL
		/// </summary>
		TEST_F(Player2, CardRemoved_CardTrueTest)
		{
			// Test data -- inputs.
			const time_t utcTime(1);
			const time_t utcRemovalTime(12345);
			const bool bMakeVirtual(true);
			CMeters * const virtualSessionStartMeters(NULL);

			// Test data -- expected results.
			const time_t cardOutTimeExpected(utcRemovalTime);
			const time_t virtualStartTimeExpected(utcRemovalTime);

			// Setup.
			CCard *card = new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE);
			CPlayerInfo *playerInfo(NULL);
            CGames games;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CPlayer varPlayer(card, playerInfo, config, egmConfig, utcTime, false, &games);

			// Perform operation(s) to be tested.
			varPlayer.CardRemoved(utcRemovalTime, utcRemovalTime, bMakeVirtual);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CCard * const cardResult(player.GetCard());
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=%p expected=NOT NULL in %s"),
					(const void *)cardResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)cardResult) << failedMsg;
			}

			// Test #2.
			{
				const time_t cardOutTimeResult(cardResult->GetCardOutTime());

				bool areEqual(cardOutTimeResult == cardOutTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardOutTimeResult=%lld expected=%lld in %s"),
					(long long)cardOutTimeResult,
					(long long)cardOutTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			{
				const time_t virtualStartTimeResult(player.GetVirtualStartTime());

				bool areEqual(virtualStartTimeResult == virtualStartTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED virtualStartTimeResult=%lld expected=%lld in %s"),
					(long long)virtualStartTimeResult,
					(long long)virtualStartTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::CardRemoved()
		///     CPlayer::MakeVirtual()
		/// Test MakeVirtual() with:
		///     m_virtualCard != NULL
		///      m_card != NULL
		/// </summary>
		TEST_F(Player2, CardRemoved_CardTrue2Test)
		{
			// Test data -- inputs.
			const time_t utcTime(1);
			const time_t utcRemovalTime(12345);
			const bool bMakeVirtual(true);
			CMeters * const virtualSessionStartMeters(NULL);

			// Test data -- expected results.
			const time_t cardOutTimeExpected(utcRemovalTime);
			const time_t virtualStartTimeExpected(utcRemovalTime);

			// Setup.
			CCard *card = new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE);
			CPlayerInfo *playerInfo(NULL);
            CGames games;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CPlayer varPlayer(card, playerInfo, config, egmConfig, utcTime, false, &games);

			// Perform operation(s) to be tested.
			varPlayer.CardRemoved(utcRemovalTime, utcRemovalTime, bMakeVirtual);
			varPlayer.CardRemoved(utcRemovalTime, utcRemovalTime, bMakeVirtual);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CCard * const cardResult(player.GetCard());
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardResult=%p expected=NOT NULL in %s"),
					(const void *)cardResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)cardResult) << failedMsg;
			}

			// Test #2.
			{
				const time_t cardOutTimeResult(cardResult->GetCardOutTime());

				bool areEqual(cardOutTimeResult == cardOutTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED cardOutTimeResult=%lld expected=%lld in %s"),
					(long long)cardOutTimeResult,
					(long long)cardOutTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Test #3.
			{
				const time_t virtualStartTimeResult(player.GetVirtualStartTime());

				bool areEqual(virtualStartTimeResult == virtualStartTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED virtualStartTimeResult=%lld expected=%lld in %s"),
					(long long)virtualStartTimeResult,
					(long long)virtualStartTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::VirtualSessionEnd()
		/// </summary>
		TEST_F(Player2, VirtualSessionEnd_NullCardTest)
		{
			// Test data -- inputs.
			const time_t virtualEndTime(1234567);

			// Test data -- expected results.

			// Setup.
			CPlayer varPlayer;

			// Perform operation(s) to be tested.
			varPlayer.VirtualSessionEnd(virtualEndTime);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const CCard * const virtualCardResult(player.GetVirtualCard());

				string failedMsg;
				failedMsg = FormatString(_T("FAILED virtualCardResult=%p expected=NULL in %s"),
					(const void *)virtualCardResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_EQ((void *)nullptr, (void *)virtualCardResult) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::VirtualSessionEnd()
		/// </summary>
		TEST_F(Player2, VirtualSessionEnd_CardTest)
		{
			// Test data -- inputs.
			const time_t utcTime(1);
			const time_t removalTime(123);
			const time_t utcRemovalTime(12345);
			const bool bMakeVirtual(true);
			CMeters * const virtualSessionStartMeters(NULL);
			const time_t virtualEndTime(123456);

			// Test data -- expected results.
			const time_t virtualCardOutTimeExpected(virtualEndTime);

			// Setup.
			CCard *card = new CCard(TIME_DEFAULT_VALUE, TIME_DEFAULT_VALUE);
			CPlayerInfo *playerInfo(NULL);
            CGames games;
			const CConfig config(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			CPlayer varPlayer(card, playerInfo, config, egmConfig, utcTime, false, &games);
			varPlayer.CardRemoved(removalTime, utcRemovalTime, bMakeVirtual);

			// Perform operation(s) to be tested.
			varPlayer.VirtualSessionEnd(virtualEndTime);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CCard * const virtualCardResult(player.GetVirtualCard());
			{

				string failedMsg;
				failedMsg = FormatString(_T("FAILED virtualCardResult=%p expected=NOT NULL in %s"),
					(const void *)virtualCardResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)virtualCardResult) << failedMsg;
			}

			// Test #2.
			{
				const time_t virtualCardOutTimeResult(virtualCardResult->GetCardOutTime());

				bool areEqual(virtualCardOutTimeResult == virtualCardOutTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED virtualCardOutTimeResult=%lld expected=%lld in %s"),
					(long long)virtualCardOutTimeResult,
					(long long)virtualCardOutTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetLastPlayTime()
		///     CPlayer::GetLastPlayTime()
		/// </summary>
		TEST_F(Player2, SetLastPlayTimeTest)
		{
			// Test data -- inputs.
			const time_t playTime(234);

			// Test data -- expected results.
			const time_t lastplayTimeExpected(playTime);

			// Setup.
			CPlayer varPlayer;

			// Perform operation(s) to be tested.
			varPlayer.SetLastPlayTime(playTime);
			const CPlayer &player(varPlayer);
			const time_t lastPlayTimeResult = player.GetLastPlayTime();

			// Test #1.
			{
				bool areEqual(lastPlayTimeResult == lastplayTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED lastPlayTimeResult=%lld expected=%lld in %s"),
					(long long)lastPlayTimeResult,
					(long long)lastplayTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerInfo()
		/// Test with:
		///     old playerInfo NULL
		///     new playerInfo NULL
		/// </summary>
		TEST_F(Player2, SetPlayerInfo00Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const DWORD currentCountdownExpected(0);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo(NULL);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_EQ((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD currentCountdownResult(player.GetCurrentCountdown());

				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerInfo()
		/// Test with:
		///     old playerInfo not NULL
		///     old playerInfo multiplier zero
		///     new playerInfo NULL
		/// </summary>
		TEST_F(Player2, SetPlayerInfo01Test)
		{
			// Test data -- inputs.
			const DWORD multiplier1(0);
			const DWORD divider1(2);
			const DWORD points1(3);
			const DWORD countdown1(4);

			// Test data -- expected results.
			const DWORD currentCountdownExpected(countdown1);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo1 = new CPlayerInfo();
			playerInfo1->SetMultiplierDivider(multiplier1, divider1);
			playerInfo1->SetPointsAndCountdown(points1, countdown1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			varPlayer.SetPlayerInfo(playerInfo1);

			CPlayerInfo *playerInfo2(NULL);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo2);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_EQ((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD currentCountdownResult(player.GetCurrentCountdown());

				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerInfo()
		/// Test with:
		///     old playerInfo not NULL
		///     old playerInfo multiplier non-zero
		///     countdown unchanged
		///     new playerInfo NULL
		/// </summary>
		TEST_F(Player2, SetPlayerInfo02Test)
		{
			// Test data -- inputs.
			const DWORD multiplier1(1);
			const DWORD divider1(2);
			const DWORD points1(3);
			const DWORD countdown1(4);

			// Test data -- expected results.
			const DWORD currentCountdownExpected(countdown1);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo1 = new CPlayerInfo();
			playerInfo1->SetMultiplierDivider(multiplier1, divider1);
			playerInfo1->SetPointsAndCountdown(points1, countdown1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			varPlayer.SetPlayerInfo(playerInfo1);

			CPlayerInfo *playerInfo2(NULL);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo2);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_EQ((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD currentCountdownResult(player.GetCurrentCountdown());

				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerInfo()
		/// Test with:
		///     old playerInfo not NULL
		///     old playerInfo multiplier non-zero
		///     countdown less
		///     new playerInfo NULL
		/// </summary>
		TEST_F(Player2, SetPlayerInfo03Test)
		{
			// Test data -- inputs.
			const DWORD multiplier1(11);
			const DWORD divider1(22);
			const DWORD points1(33);
			const DWORD countdown1(44);
			const DWORD currentCountdown(countdown1 - 1);

			// Test data -- expected results.
			const DWORD currentCountdownExpected(currentCountdown);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo1 = new CPlayerInfo();
			playerInfo1->SetMultiplierDivider(multiplier1, divider1);
			playerInfo1->SetPointsAndCountdown(points1, countdown1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			varPlayer.SetPlayerInfo(playerInfo1);
			varPlayer.SetCurrentCountdown(currentCountdown);

			CPlayerInfo *playerInfo2(NULL);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo2);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_EQ((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD currentCountdownResult(player.GetCurrentCountdown());

				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerInfo()
		/// Test with:
		///     old playerInfo not NULL
		///     old playerInfo multiplier non-zero
		///     countdown greater
		///     new playerInfo NULL
		/// </summary>
		TEST_F(Player2, SetPlayerInfo04Test)
		{
			// Test data -- inputs.
			const DWORD multiplier1(11);
			const DWORD divider1(22);
			const DWORD points1(33);
			const DWORD countdown1(44);
			const DWORD currentCountdown(countdown1 + 1);

			// Test data -- expected results.
			const DWORD currentCountdownExpected(currentCountdown);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo1 = new CPlayerInfo();
			playerInfo1->SetMultiplierDivider(multiplier1, divider1);
			playerInfo1->SetPointsAndCountdown(points1, countdown1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			varPlayer.SetPlayerInfo(playerInfo1);
			varPlayer.SetCurrentCountdown(currentCountdown);

			CPlayerInfo *playerInfo2(NULL);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo2);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_EQ((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD currentCountdownResult(player.GetCurrentCountdown());

				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerInfo()
		/// Test with:
		///     old playerInfo NULL
		///     new playerInfo countdown zero
		/// </summary>
		TEST_F(Player2, SetPlayerInfo10Test)
		{
			// Test data -- inputs.
			const DWORD multiplier2(1);
			const DWORD divider2(2);
			const DWORD points2(3);
			const DWORD countdown2(0);

			// Test data -- expected results.
			const DWORD currentCountdownExpected(divider2);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo2 = new CPlayerInfo();
			playerInfo2->SetMultiplierDivider(multiplier2, divider2);
			playerInfo2->SetPointsAndCountdown(points2, countdown2, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo2);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NOT NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD currentCountdownResult(player.GetCurrentCountdown());

				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerInfo()
		/// Test with:
		///     old playerInfo not NULL
		///     old playerInfo multiplier zero
		///     new playerInfo countdown zero
		/// </summary>
		TEST_F(Player2, SetPlayerInfo11Test)
		{
			// Test data -- inputs.
			const DWORD multiplier1(0);
			const DWORD divider1(22);
			const DWORD points1(33);
			const DWORD countdown1(44);
			const DWORD multiplier2(1);
			const DWORD divider2(2);
			const DWORD points2(3);
			const DWORD countdown2(0);

			// Test data -- expected results.
			const DWORD currentCountdownExpected(divider2);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo1 = new CPlayerInfo();
			playerInfo1->SetMultiplierDivider(multiplier1, divider1);
			playerInfo1->SetPointsAndCountdown(points1, countdown1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			varPlayer.SetPlayerInfo(playerInfo1);

			CPlayerInfo *playerInfo2 = new CPlayerInfo();
			playerInfo2->SetMultiplierDivider(multiplier2, divider2);
			playerInfo2->SetPointsAndCountdown(points2, countdown2, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo2);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NOT NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD currentCountdownResult(player.GetCurrentCountdown());

				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerInfo()
		/// Test with:
		///     old playerInfo not NULL
		///     old playerInfo multiplier non-zero
		///     countdown unchanged
		///     new playerInfo countdown zero
		/// </summary>
		TEST_F(Player2, SetPlayerInfo12Test)
		{
			// Test data -- inputs.
			const DWORD multiplier1(11);
			const DWORD divider1(22);
			const DWORD points1(33);
			const DWORD countdown1(44);
			const DWORD multiplier2(1);
			const DWORD divider2(2);
			const DWORD points2(3);
			const DWORD countdown2(0);

			// Test data -- expected results.
			const DWORD currentCountdownExpected(divider2);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo1 = new CPlayerInfo();
			playerInfo1->SetMultiplierDivider(multiplier1, divider1);
			playerInfo1->SetPointsAndCountdown(points1, countdown1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			varPlayer.SetPlayerInfo(playerInfo1);

			CPlayerInfo *playerInfo2 = new CPlayerInfo();
			playerInfo2->SetMultiplierDivider(multiplier2, divider2);
			playerInfo2->SetPointsAndCountdown(points2, countdown2, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo2);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NOT NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD currentCountdownResult(player.GetCurrentCountdown());

				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerInfo()
		/// Test with:
		///     old playerInfo not NULL
		///     old playerInfo multiplier non-zero
		///     countdown less
		///     new playerInfo countdown zero
		/// </summary>
		TEST_F(Player2, SetPlayerInfo13Test)
		{
			// Test data -- inputs.
			const DWORD multiplier1(11);
			const DWORD divider1(22);
			const DWORD points1(33);
			const DWORD countdown1(44);
			const DWORD currentCountdown(countdown1 - 1);
			const DWORD multiplier2(7);
			const DWORD divider2(8);
			const DWORD points2(9);
			const DWORD countdown2(0);

			// Test data -- expected results.
			const DWORD currentCountdownExpected(divider2);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo1 = new CPlayerInfo();
			playerInfo1->SetMultiplierDivider(multiplier1, divider1);
			playerInfo1->SetPointsAndCountdown(points1, countdown1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			varPlayer.SetPlayerInfo(playerInfo1);
			varPlayer.SetCurrentCountdown(currentCountdown);

			CPlayerInfo *playerInfo2 = new CPlayerInfo();
			playerInfo2->SetMultiplierDivider(multiplier2, divider2);
			playerInfo2->SetPointsAndCountdown(points2, countdown2, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo2);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NOT NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD currentCountdownResult(player.GetCurrentCountdown());

				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerInfo()
		/// Test with:
		///     old playerInfo not NULL
		///     old playerInfo multiplier non-zero
		///     countdown greater
		///     new playerInfo countdown zero
		/// </summary>
		TEST_F(Player2, SetPlayerInfo14Test)
		{
			// Test data -- inputs.
			const DWORD multiplier1(11);
			const DWORD divider1(22);
			const DWORD points1(33);
			const DWORD countdown1(44);
			const DWORD currentCountdown(countdown1 + 1);
			const DWORD multiplier2(7);
			const DWORD divider2(8);
			const DWORD points2(9);
			const DWORD countdown2(0);

			// Test data -- expected results.
			// Countdown "wrap" causes excessive point calc, so use divider2 because unadjusted countdown2 is zero.
			const DWORD currentCountdownExpected(divider2);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo1 = new CPlayerInfo();
			playerInfo1->SetMultiplierDivider(multiplier1, divider1);
			playerInfo1->SetPointsAndCountdown(points1, countdown1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			varPlayer.SetPlayerInfo(playerInfo1);
			varPlayer.SetCurrentCountdown(currentCountdown);

			CPlayerInfo *playerInfo2 = new CPlayerInfo();
			playerInfo2->SetMultiplierDivider(multiplier2, divider2);
			playerInfo2->SetPointsAndCountdown(points2, countdown2, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo2);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NOT NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD currentCountdownResult(player.GetCurrentCountdown());

				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerInfo()
		/// Test with:
		///     old playerInfo NULL
		///     new playerInfo countdown non-zero
		/// </summary>
		TEST_F(Player2, SetPlayerInfo20Test)
		{
			// Test data -- inputs.
			const DWORD multiplier(1);
			const DWORD divider(2);
			const DWORD points(3);
			const DWORD countdown(4);

			// Test data -- expected results.
			const DWORD currentCountdownExpected(countdown);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NOT NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD currentCountdownResult(player.GetCurrentCountdown());

				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerInfo()
		/// Test with:
		///     old playerInfo not NULL
		///     old playerInfo multiplier zero
		///     new playerInfo countdown non-zero
		/// </summary>
		TEST_F(Player2, SetPlayerInfo21Test)
		{
			// Test data -- inputs.
			const DWORD multiplier1(0);
			const DWORD divider1(22);
			const DWORD points1(33);
			const DWORD countdown1(44);
			const DWORD multiplier2(1);
			const DWORD divider2(2);
			const DWORD points2(3);
			const DWORD countdown2(4);

			// Test data -- expected results.
			const DWORD currentCountdownExpected(countdown2);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo1 = new CPlayerInfo();
			playerInfo1->SetMultiplierDivider(multiplier1, divider1);
			playerInfo1->SetPointsAndCountdown(points1, countdown1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			varPlayer.SetPlayerInfo(playerInfo1);

			CPlayerInfo *playerInfo2 = new CPlayerInfo();
			playerInfo2->SetMultiplierDivider(multiplier2, divider2);
			playerInfo2->SetPointsAndCountdown(points2, countdown2, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo2);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NOT NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD currentCountdownResult(player.GetCurrentCountdown());

				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerInfo()
		/// Test with:
		///     old playerInfo not NULL
		///     old playerInfo multiplier non-zero
		///     countdown unchanged
		///     new playerInfo countdown non-zero
		/// </summary>
		TEST_F(Player2, SetPlayerInfo22Test)
		{
			// Test data -- inputs.
			const DWORD multiplier1(11);
			const DWORD divider1(22);
			const DWORD points1(33);
			const DWORD countdown1(44);
			const DWORD multiplier2(1);
			const DWORD divider2(2);
			const DWORD points2(3);
			const DWORD countdown2(4);

			// Test data -- expected results.
			const DWORD currentCountdownExpected(countdown2);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo1 = new CPlayerInfo();
			playerInfo1->SetMultiplierDivider(multiplier1, divider1);
			playerInfo1->SetPointsAndCountdown(points1, countdown1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			varPlayer.SetPlayerInfo(playerInfo1);

			CPlayerInfo *playerInfo2 = new CPlayerInfo();
			playerInfo2->SetMultiplierDivider(multiplier2, divider2);
			playerInfo2->SetPointsAndCountdown(points2, countdown2, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo2);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NOT NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD currentCountdownResult(player.GetCurrentCountdown());

				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerInfo()
		/// Test with:
		///     old playerInfo not NULL
		///     old playerInfo multiplier non-zero
		///     countdown less
		///     new playerInfo countdown non-zero
		/// </summary>
		TEST_F(Player2, SetPlayerInfo23Test)
		{
			// Test data -- inputs.
			const DWORD multiplier1(11);
			const DWORD divider1(22);
			const DWORD points1(33);
			const DWORD countdown1(44);
			const DWORD currentCountdown(countdown1 - 1);
			const DWORD multiplier2(7);
			const DWORD divider2(8);
			const DWORD points2(9);
			const DWORD countdown2(4);

			// Test data -- expected results.
			const DWORD currentCountdownExpected((divider2 + countdown2) % divider2);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo1 = new CPlayerInfo();
			playerInfo1->SetMultiplierDivider(multiplier1, divider1);
			playerInfo1->SetPointsAndCountdown(points1, countdown1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			varPlayer.SetPlayerInfo(playerInfo1);
			varPlayer.SetCurrentCountdown(currentCountdown);

			CPlayerInfo *playerInfo2 = new CPlayerInfo();
			playerInfo2->SetMultiplierDivider(multiplier2, divider2);
			playerInfo2->SetPointsAndCountdown(points2, countdown2, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo2);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NOT NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD currentCountdownResult(player.GetCurrentCountdown());

				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerInfo()
		/// Test with:
		///     old playerInfo not NULL
		///     old playerInfo multiplier non-zero
		///     countdown greater
		///     new playerInfo countdown non-zero
		/// </summary>
		TEST_F(Player2, SetPlayerInfo24Test)
		{
			// Test data -- inputs.
			const DWORD multiplier1(11);
			const DWORD divider1(22);
			const DWORD points1(33);
			const DWORD countdown1(44);
			const DWORD currentCountdown(countdown1 + 1);
			const DWORD multiplier2(7);
			const DWORD divider2(8);
			const DWORD points2(9);
			const DWORD countdown2(4);

			// Test data -- expected results.
			// Countdown "wrap" causes excessive point calc, so use countdown2 without adjustment.
			const DWORD currentCountdownExpected(countdown2);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo1 = new CPlayerInfo();
			playerInfo1->SetMultiplierDivider(multiplier1, divider1);
			playerInfo1->SetPointsAndCountdown(points1, countdown1, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			varPlayer.SetPlayerInfo(playerInfo1);
			varPlayer.SetCurrentCountdown(currentCountdown);

			CPlayerInfo *playerInfo2 = new CPlayerInfo();
			playerInfo2->SetMultiplierDivider(multiplier2, divider2);
			playerInfo2->SetPointsAndCountdown(points2, countdown2, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo2);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NOT NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD currentCountdownResult(player.GetCurrentCountdown());

				bool areEqual(currentCountdownResult == currentCountdownExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED currentCountdownResult=%u expected=%u in %s"),
					(unsigned)currentCountdownResult,
					(unsigned)currentCountdownExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerGlobalBalance()
		/// Test with:
		///     playerInfo NULL
		///     globalBalance NULL
		/// </summary>
		TEST_F(Player2, SetPlayerGlobalBalance0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const DWORD totalPointsExpected(0);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo(NULL);
			const CPlayerGlobalBalance *globalBalance(NULL);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo);
			varPlayer.SetPlayerGlobalBalance(globalBalance);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const DWORD totalPointsResult = player.GetTotalPoints();

				bool areEqual(totalPointsResult == totalPointsExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED totalPointsResult=%u expected=%u in %s"),
					(unsigned)totalPointsResult,
					(unsigned)totalPointsExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerGlobalBalance()
		/// Test with:
		///     playerInfo non-NULL
		///     globalBalance NULL
		/// </summary>
		TEST_F(Player2, SetPlayerGlobalBalance1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const DWORD totalPointsExpected(0);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo = new CPlayerInfo();
			const CPlayerGlobalBalance *globalBalance(NULL);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo);
			varPlayer.SetPlayerGlobalBalance(globalBalance);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const DWORD totalPointsResult = player.GetTotalPoints();

				bool areEqual(totalPointsResult == totalPointsExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED totalPointsResult=%u expected=%u in %s"),
					(unsigned)totalPointsResult,
					(unsigned)totalPointsExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerGlobalBalance()
		/// Test with:
		///     playerInfo non-NULL
		///     globalBalance non-NULL
		///     globalBalance totalPoints zero.
		///     player.GetTotalPoints() zero.
		/// </summary>
		TEST_F(Player2, SetPlayerGlobalBalance2Test)
		{
			// Test data -- inputs.
			const LONGLONG aTotalPoints(0);

			// Test data -- expected results.
			const DWORD totalPointsExpected(0);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo = new CPlayerInfo();
			CPlayerGlobalBalance *varGlobalBalance = new CPlayerGlobalBalance();
			varGlobalBalance->PutTotalPoints(aTotalPoints);
			const CPlayerGlobalBalance *globalBalance(varGlobalBalance);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo);
			varPlayer.SetPlayerGlobalBalance(globalBalance);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const DWORD totalPointsResult = player.GetTotalPoints();

				bool areEqual(totalPointsResult == totalPointsExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED totalPointsResult=%u expected=%u in %s"),
					(unsigned)totalPointsResult,
					(unsigned)totalPointsExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerGlobalBalance()
		/// Test with:
		///     playerInfo non-NULL
		///     globalBalance non-NULL
		///     globalBalance totalPoints non-zero.
		///     player.GetTotalPoints() zero.
		/// </summary>
		TEST_F(Player2, SetPlayerGlobalBalance3Test)
		{
			// Test data -- inputs.
			const LONGLONG aTotalPoints((LONGLONG)DWORD_MAX + 1);

			// Test data -- expected results.
			const DWORD totalPointsExpected((DWORD)aTotalPoints);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo = new CPlayerInfo();
			CPlayerGlobalBalance *varGlobalBalance = new CPlayerGlobalBalance();
			varGlobalBalance->PutTotalPoints(aTotalPoints);
			const CPlayerGlobalBalance *globalBalance(varGlobalBalance);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo);
			varPlayer.SetPlayerGlobalBalance(globalBalance);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const DWORD totalPointsResult = player.GetTotalPoints();

				bool areEqual(totalPointsResult == totalPointsExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED totalPointsResult=%u expected=%u in %s"),
					(unsigned)totalPointsResult,
					(unsigned)totalPointsExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetPlayerGlobalBalance()
		/// Test with:
		///     playerInfo non-NULL
		///     globalBalance non-NULL
		///     globalBalance totalPoints non-zero.
		///     player.GetTotalPoints() zero.
		/// </summary>
		TEST_F(Player2, SetPlayerGlobalBalance4Test)
		{
			// Test data -- inputs.
			const LONGLONG aTotalPoints((LONGLONG)DWORD_MAX + 2);

			// Test data -- expected results.
			const DWORD totalPointsExpected((DWORD)aTotalPoints);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo = new CPlayerInfo();
			CPlayerGlobalBalance *varGlobalBalance = new CPlayerGlobalBalance();
			varGlobalBalance->PutTotalPoints(aTotalPoints);
			const CPlayerGlobalBalance *globalBalance(varGlobalBalance);

			// Perform operation(s) to be tested.
			varPlayer.SetPlayerInfo(playerInfo);
			varPlayer.SetPlayerGlobalBalance(globalBalance);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const DWORD totalPointsResult = player.GetTotalPoints();

				bool areEqual(totalPointsResult == totalPointsExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED totalPointsResult=%u expected=%u in %s"),
					(unsigned)totalPointsResult,
					(unsigned)totalPointsExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::CheckMultiplierDivider()
		/// Test with:
		///     playerInfo NULL
		/// </summary>
		TEST_F(Player2, CheckMultiplierDivider_NullTest)
		{
			// Test data -- inputs.
			const DWORD fallbackMultiplier(DEFAULT_MULTIPLIER + 1);
			const DWORD fallbackDivider(DEFAULT_COUNTDOWN + 1);

			// Test data -- expected results.

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo(NULL);
			varPlayer.SetPlayerInfo(playerInfo);

			// Perform operation(s) to be tested.
			varPlayer.CheckMultiplierDivider(fallbackMultiplier, fallbackDivider);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_EQ((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::CheckMultiplierDivider()
		/// Test with:
		///     playerInfo non-NULL
		///     multiplier and divider unchanged.
		/// </summary>
		TEST_F(Player2, CheckMultiplierDivider_NoChangeTest)
		{
			// Test data -- inputs.
			const DWORD fallbackMultiplier(DEFAULT_MULTIPLIER + 1);
			const DWORD fallbackDivider(DEFAULT_COUNTDOWN + 1);

			// Test data -- expected results.
			const DWORD multiplierExpected(DEFAULT_MULTIPLIER);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo(new CPlayerInfo());
			varPlayer.SetPlayerInfo(playerInfo);

			// Perform operation(s) to be tested.
			varPlayer.CheckMultiplierDivider(fallbackMultiplier, fallbackDivider);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NOT NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD multiplierResult(playerInfoResult->GetMultiplier());

				bool areEqual(multiplierResult == multiplierExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED multiplierResult=%u expected=%u in %s"),
					(unsigned)multiplierResult,
					(unsigned)multiplierExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::CheckMultiplierDivider()
		/// Test with:
		///     playerInfo non-NULL
		///     multiplier or divider changed.
		/// </summary>
		TEST_F(Player2, CheckMultiplierDivider_ChangeTest)
		{
			// Test data -- inputs.
			const DWORD multiplier(0);
			const DWORD divider(DEFAULT_COUNTDOWN);
			const DWORD fallbackMultiplier(DEFAULT_MULTIPLIER + 1);
			const DWORD fallbackDivider(DEFAULT_COUNTDOWN + 1);

			// Test data -- expected results.
			const DWORD multiplierExpected(fallbackMultiplier);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo(new CPlayerInfo());
			playerInfo->SetMultiplierDivider(multiplier, divider);
			varPlayer.SetPlayerInfo(playerInfo);

			// Perform operation(s) to be tested.
			varPlayer.CheckMultiplierDivider(fallbackMultiplier, fallbackDivider);
			const CPlayer &player(varPlayer);

			// Test #1.
			const CPlayerInfo * const playerInfoResult = varPlayer.GetPlayerInfo();
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED playerInfoResult=%p expected=NOT NULL in %s"),
					(const void *)playerInfoResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_NE((void *)nullptr, (void *)playerInfoResult) << failedMsg;
			}

			// Test #2.
			{
				DWORD multiplierResult(playerInfoResult->GetMultiplier());

				bool areEqual(multiplierResult == multiplierExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED multiplierResult=%u expected=%u in %s"),
					(unsigned)multiplierResult,
					(unsigned)multiplierExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetBonusEvents()
		/// Test with:
		///     old bonusEvents NULL
		///     new bonusEvents NULL
		/// </summary>
		TEST_F(Player2, SetBonusEvents_NullNullTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const MEMORY_TYPE memoryTypeExpected = MEMORY_NONE;

			// Setup.
			CPlayer varPlayer;
			const CBonusEvents *bonusEvents(NULL);

			// Perform operation(s) to be tested.
			varPlayer.SetBonusEvents(bonusEvents);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const MEMORY_TYPE memoryTypeResult(player.GetMemoryType());

				bool areEqual(memoryTypeResult == memoryTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%d expected=%d in %s"),
					(int)memoryTypeResult,
					(int)memoryTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetBonusEvents()
		/// Test with:
		///     old bonusEvents NULL
		///     new bonusEvents non-NULL
		/// </summary>
		TEST_F(Player2, SetBonusEvents_NullNonNullTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const MEMORY_TYPE memoryTypeExpected = MEMORY_NONE;

			// Setup.
			CPlayer varPlayer;
			const CBonusEvents bonusEvents;

			// Perform operation(s) to be tested.
			varPlayer.SetBonusEvents(&bonusEvents);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const MEMORY_TYPE memoryTypeResult(player.GetMemoryType());

				bool areEqual(memoryTypeResult == memoryTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%d expected=%d in %s"),
					(int)memoryTypeResult,
					(int)memoryTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetBonusEvents()
		/// Test with:
		///     old bonusEvents non-NULL
		///     new bonusEvents NULL
		/// </summary>
		TEST_F(Player2, SetBonusEvents_NonNullNullTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const MEMORY_TYPE memoryTypeExpected = MEMORY_NONE;

			// Setup.
			CPlayer varPlayer;
			const CBonusEvents bonusEvents0;
			varPlayer.SetBonusEvents(&bonusEvents0);
			const CBonusEvents *bonusEvents(NULL);

			// Perform operation(s) to be tested.
			varPlayer.SetBonusEvents(bonusEvents);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const MEMORY_TYPE memoryTypeResult(player.GetMemoryType());

				bool areEqual(memoryTypeResult == memoryTypeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED memoryTypeResult=%d expected=%d in %s"),
					(int)memoryTypeResult,
					(int)memoryTypeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CPlayer::SetCardedOutOnEmployeeCardIn()
		///     CPlayer::WasCardedOutOnEmployeeCardIn()
		/// </summary>
		TEST_F(Player2, SetCardedOutOnEmployeeCardInTest)
		{
			// Test data.
			const bool cardedOutOnEmployeeCardIn0(false);
			const bool cardedOutOnEmployeeCardIn1(true);
			const bool cardedOutOnEmployeeCardIn2(true);
			const bool cardedOutOnEmployeeCardIn3(false);
			const bool cardedOutOnEmployeeCardIn4(false);

			// Setup.
			CPlayer player;

			// Perform operation(s) to be tested and test.
			player.SetCardedOutOnEmployeeCardIn(cardedOutOnEmployeeCardIn0);
			ASSERT_EQ(player.WasCardedOutOnEmployeeCardIn(), cardedOutOnEmployeeCardIn0);

			player.SetCardedOutOnEmployeeCardIn(cardedOutOnEmployeeCardIn1);
			ASSERT_EQ(player.WasCardedOutOnEmployeeCardIn(), cardedOutOnEmployeeCardIn1);

			player.SetCardedOutOnEmployeeCardIn(cardedOutOnEmployeeCardIn2);
			ASSERT_EQ(player.WasCardedOutOnEmployeeCardIn(), cardedOutOnEmployeeCardIn2);

			player.SetCardedOutOnEmployeeCardIn(cardedOutOnEmployeeCardIn3);
			ASSERT_EQ(player.WasCardedOutOnEmployeeCardIn(), cardedOutOnEmployeeCardIn3);

			player.SetCardedOutOnEmployeeCardIn(cardedOutOnEmployeeCardIn4);
			ASSERT_EQ(player.WasCardedOutOnEmployeeCardIn(), cardedOutOnEmployeeCardIn4);
		}
	const bool Player2::m_isGoodbyeEnabled = false;
	const bool Player2::m_cardedOutOnEmployeeCardIn = false;
}
