#include "stdafx.h"
#include "Player.h"

#include "MultiplierBonusEvent.h"
#include "Config.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "UnitTestingUtils.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	static const time_t TIME_DEFAULT_VALUE(0); // Arbitrary value when any valid value is okay.
	static const byte MEMSET_DEFAULT_VALUE = 0xcd; // Value unlikely to be used for other purposes.
    static const DWORD fallbackMultiplier(DEFAULT_MULTIPLIER + 1);
    static const DWORD fallbackDivider(DEFAULT_COUNTDOWN + 1);


	class Player4 : public ::testing::Test
	{
	};

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     config.PBTCashEnabled() true
		/// </summary>
		TEST(Player4, ConfigSettings_PBEnabled0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPBTCashEnabled_local(true);
			varConfig.SetPBTCashEnabled_Global(true);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     config.PBTCashEnabled() false
		///     config.PBTPointsEnabled() true
		/// </summary>
		TEST(Player4, ConfigSettings_PBEnabled1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPBTCashEnabled_local(false);
			varConfig.SetPBTPointsEnabled_local(true);
			varConfig.SetPBTPointsEnabled_Global(true);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     config.PBTCashEnabled() false
		///     config.PBTPointsEnabled() false
		///     config.PBTPointsAsPromo() true
		/// </summary>
		TEST(Player4, ConfigSettings_PBEnabled2Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPBTCashEnabled_local(false);
			varConfig.SetPBTPointsEnabled_local(false);
			varConfig.SetPBTPointsAsPromo(true);
			varConfig.SetPBTPointsEnabled_Global(true);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     config.PBTCashEnabled() false
		///     config.PBTPointsEnabled() false
		///     config.PBTPointsAsPromo() false
		///     config.PBTPromoEnabled() true
		/// </summary>
		TEST(Player4, ConfigSettings_PBEnabled3Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPBTCashEnabled_local(false);
			varConfig.SetPBTPointsEnabled_local(false);
			varConfig.SetPBTPointsAsPromo(false);
			varConfig.SetPBTPromoEnabled_local(true);
			varConfig.SetPBTPromoEnabled_Global(true);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     config.PBTCashEnabled() false
		///     config.PBTPointsEnabled() false
		///     config.PBTPointsAsPromo() false
		///     config.PBTPromoEnabled() false
		///     config.PBFromGameEnabled() true
		/// </summary>
		TEST(Player4, ConfigSettings_PBEnabled4Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPBTCashEnabled_local(false);
			varConfig.SetPBTPointsEnabled_local(false);
			varConfig.SetPBTPointsAsPromo(false);
			varConfig.SetPBTPromoEnabled_local(false);
			varConfig.SetPBFromGameEnabled(true);
			varConfig.SetPBTCashEnabled_Global(true);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     config.PBTCashEnabled() false
		///     config.PBTPointsEnabled() false
		///     config.PBTPointsAsPromo() false
		///     config.PBTPromoEnabled() false
		///     config.PBFromGameEnabled() false
		/// </summary>
		TEST(Player4, ConfigSettings_PBEnabled5Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPBTCashEnabled_local(false);
			varConfig.SetPBTPointsEnabled_local(false);
			varConfig.SetPBTPointsAsPromo(false);
			varConfig.SetPBTPromoEnabled_local(false);
			varConfig.SetPBFromGameEnabled(false);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     config.GetPointsPerDollar() zero
		/// </summary>
		TEST(Player4, ConfigSettings_PointsPerDollar0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPointsPerDollar(0);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     config.GetPointsPerDollar() non-zero
		/// </summary>
		TEST(Player4, ConfigSettings_PointsPerDollar1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPointsPerDollar(1);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     m_SessionPointsSingleLineText.IsEmpty() == true
		///     config.DisplayPlayerPtsTotal() true
		/// </summary>
		TEST(Player4, ConfigSettings_InPlayDisplayType0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetSessionPointsMessage(_T(""));
			varConfig.SetDisplayPlayerPtsTotal(true);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     m_SessionPointsSingleLineText.IsEmpty() == true
		///     config.DisplayPlayerPtsTotal() false
		///     config.DisplayCountdownAndSession() true
		/// </summary>
		TEST(Player4, ConfigSettings_InPlayDisplayType1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetSessionPointsMessage(_T(""));
			varConfig.SetDisplayPlayerPtsTotal(false);
			varConfig.SetDisplayCountdownAndSession(true);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     m_SessionPointsSingleLineText.IsEmpty() == true
		///     config.DisplayPlayerPtsTotal() false
		///     config.DisplayCountdownAndSession() false
		///     config.DisplayGoodLuckAndSession() true
		/// </summary>
		TEST(Player4, ConfigSettings_InPlayDisplayType2Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetSessionPointsMessage(_T(""));
			varConfig.SetDisplayPlayerPtsTotal(false);
			varConfig.SetDisplayCountdownAndSession(false);
			varConfig.SetDisplayGoodLuckAndSession(true);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     m_SessionPointsSingleLineText.IsEmpty() == true
		///     config.DisplayPlayerPtsTotal() false
		///     config.DisplayCountdownAndSession() false
		///     config.DisplayGoodLuckAndSession() false
		///     config.DisplayTotalAndSessionPtsNum() true
		/// </summary>
		TEST(Player4, ConfigSettings_InPlayDisplayType3Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetSessionPointsMessage(_T(""));
			varConfig.SetDisplayPlayerPtsTotal(false);
			varConfig.SetDisplayCountdownAndSession(false);
			varConfig.SetDisplayGoodLuckAndSession(false);
			varConfig.SetDisplayTotalAndSessionPtsNum(true);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     m_SessionPointsSingleLineText.IsEmpty() == true
		///     config.DisplayPlayerPtsTotal() false
		///     config.DisplayCountdownAndSession() false
		///     config.DisplayGoodLuckAndSession() false
		///     config.DisplayTotalAndSessionPtsNum() false
		///     config.DisplayTotalPtsSessionPtsAsCash() true
		/// </summary>
		TEST(Player4, ConfigSettings_InPlayDisplayType4Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetSessionPointsMessage(_T(""));
			varConfig.SetDisplayPlayerPtsTotal(false);
			varConfig.SetDisplayCountdownAndSession(false);
			varConfig.SetDisplayGoodLuckAndSession(false);
			varConfig.SetDisplayTotalAndSessionPtsNum(false);
			varConfig.SetDisplayTotalPtsSessionPtsAsCash(true);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     m_SessionPointsSingleLineText.IsEmpty() == true
		///     config.DisplayPlayerPtsTotal() false
		///     config.DisplayCountdownAndSession() false
		///     config.DisplayGoodLuckAndSession() false
		///     config.DisplayTotalAndSessionPtsNum() false
		///     config.DisplayTotalPtsSessionPtsAsCash() false
		///     config.DisplayGoodLuckAndSessionPts() true
		/// </summary>
		TEST(Player4, ConfigSettings_InPlayDisplayType5Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetSessionPointsMessage(_T(""));
			varConfig.SetDisplayPlayerPtsTotal(false);
			varConfig.SetDisplayCountdownAndSession(false);
			varConfig.SetDisplayGoodLuckAndSession(false);
			varConfig.SetDisplayTotalAndSessionPtsNum(false);
			varConfig.SetDisplayTotalPtsSessionPtsAsCash(false);
			varConfig.SetDisplayGoodLuckAndSessionPts(true);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     m_SessionPointsSingleLineText.IsEmpty() == true
		///     config.DisplayPlayerPtsTotal() false
		///     config.DisplayCountdownAndSession() false
		///     config.DisplayGoodLuckAndSession() false
		///     config.DisplayTotalAndSessionPtsNum() false
		///     config.DisplayTotalPtsSessionPtsAsCash() false
		///     config.DisplayGoodLuckAndSessionPts() false
		///     config.DisplayPtsTotalAsCashSession() true
		/// </summary>
		TEST(Player4, ConfigSettings_InPlayDisplayType6Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetSessionPointsMessage(_T(""));
			varConfig.SetDisplayPlayerPtsTotal(false);
			varConfig.SetDisplayCountdownAndSession(false);
			varConfig.SetDisplayGoodLuckAndSession(false);
			varConfig.SetDisplayTotalAndSessionPtsNum(false);
			varConfig.SetDisplayTotalPtsSessionPtsAsCash(false);
			varConfig.SetDisplayGoodLuckAndSessionPts(false);
			varConfig.SetDisplayPtsTotalAsCashSession(true);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     m_SessionPointsSingleLineText.IsEmpty() == true
		///     config.DisplayPlayerPtsTotal() false
		///     config.DisplayCountdownAndSession() false
		///     config.DisplayGoodLuckAndSession() false
		///     config.DisplayTotalAndSessionPtsNum() false
		///     config.DisplayTotalPtsSessionPtsAsCash() false
		///     config.DisplayGoodLuckAndSessionPts() false
		///     config.DisplayPtsTotalAsCashSession() false
		/// </summary>
		TEST(Player4, ConfigSettings_InPlayDisplayType7Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetSessionPointsMessage(_T(""));
			varConfig.SetDisplayPlayerPtsTotal(false);
			varConfig.SetDisplayCountdownAndSession(false);
			varConfig.SetDisplayGoodLuckAndSession(false);
			varConfig.SetDisplayTotalAndSessionPtsNum(false);
			varConfig.SetDisplayTotalPtsSessionPtsAsCash(false);
			varConfig.SetDisplayGoodLuckAndSessionPts(false);
			varConfig.SetDisplayPtsTotalAsCashSession(false);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     config.DisplayPtsTotalAsCash() true
		/// </summary>
		TEST(Player4, ConfigSettings_CardInPointsDisplayType0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(true);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     config.DisplayPtsTotalAsCash() false
		///     config.DisplayNumericPointTotal() true
		/// </summary>
		TEST(Player4, ConfigSettings_CardInPointsDisplayType1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(false);
			varConfig.SetDisplayNumericPointTotal(true);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     config.DisplayPtsTotalAsCash() false
		///     config.DisplayNumericPointTotal() false
		/// </summary>
		TEST(Player4, ConfigSettings_CardInPointsDisplayType2Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPtsTotalAsCash(false);
			varConfig.SetDisplayNumericPointTotal(false);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     config.DisplayComps() true
		/// </summary>
		TEST(Player4, ConfigSettings_ShowCardInCompDisplay0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayComps(true);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     config.DisplayComps() false
		/// </summary>
		TEST(Player4, ConfigSettings_ShowCardInCompDisplay1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const bool pbEnabledExpected(false);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayComps(false);
			const CConfig &config(varConfig);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const bool pbEnabledResult(player.IsPBEnabled());

				bool areEqual(pbEnabledResult == pbEnabledExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED pbEnabledResult=%d expected=%d in %s"),
					(int)pbEnabledResult,
					(int)pbEnabledExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     config.DisplayComps() false
		///     m_playerInfo != NULL
		/// </summary>
		TEST(Player4, ConfigSettings_DisplayComps0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string compsAsCashStringExpected(_T(""));

			// Setup.
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayComps(false);
			const CConfig &config(varConfig);

			CPlayerInfo *playerInfo(new CPlayerInfo());

			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(playerInfo);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const string compsAsCashStringResult(player.GetCompsAsCashString());

				bool areEqual(compsAsCashStringResult == compsAsCashStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED compsAsCashStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(compsAsCashStringResult.c_str()),
					static_cast<LPCTSTR>(compsAsCashStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ConfigSettings()
		/// Test with:
		///     default configuration
		///     config.DisplayComps() true
		///     m_playerInfo != NULL
		/// </summary>
		TEST(Player4, ConfigSettings_DisplayComps1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string compsAsCashStringExpected(_T("$0.00"));

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayComps(true);
			const CConfig &config(varConfig);

			CPlayerInfo *playerInfo(new CPlayerInfo());

			CPlayer varPlayer;
			varPlayer.SetPlayerInfo(playerInfo);

			// Perform operation(s) to be tested.
			varPlayer.ConfigSettings(config, egmConfig);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const string compsAsCashStringResult(player.GetCompsAsCashString());

				bool areEqual(compsAsCashStringResult == compsAsCashStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED compsAsCashStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(compsAsCashStringResult.c_str()),
					static_cast<LPCTSTR>(compsAsCashStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::SetCurrentCountdown()
		/// </summary>
		TEST(Player4, SetCurrentCountdownTest)
		{
			// Test data -- inputs.
			const DWORD currentCountdown(34);

			// Test data -- expected results.
			const DWORD currentCountdownExpected(currentCountdown);

			// Setup.
			CPlayer varPlayer;

			// Perform operation(s) to be tested.
			varPlayer.SetCurrentCountdown(currentCountdown);
			const CPlayer &player(varPlayer);

			// Test #1.
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
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo NULL
		/// </summary>
		TEST(Player4, DecrementCountdown00Test)
		{
			// Test data -- inputs.
			const DWORD decrement(0);

			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo(NULL);
			varPlayer.SetPlayerInfo(playerInfo);

			// Perform operation(s) to be tested.
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 2, true, false);
			meters->SetMeter(gCI, 2, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			const CPlayer &player(varPlayer);
			delete meters;
			delete orig;
			// Test #1.
			{

				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo non-NULL
		///	     decrement zero
		/// </summary>
		TEST(Player4, DecrementCountdown01Test)
		{
			// Test data -- inputs.
			const DWORD decrement(0);

			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo = new CPlayerInfo();
			varPlayer.SetPlayerInfo(playerInfo);

			// Perform operation(s) to be tested.
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 2, true, false);
			meters->SetMeter(gCI, 2, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			const CPlayer &player(varPlayer);
			delete meters;
			delete orig;

			// Test #1.
			{
				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo non-NULL
		///	     decrement non-zero
		///	     decrement less than current countdown
		/// </summary>
		TEST(Player4, DecrementCountdown02Test)
		{
			// Test data -- inputs.
			const DWORD decrement(1);
			const DWORD points(0);
			const DWORD countdown(decrement + 1);
 
			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);
			const DWORD currentCountdownExpected(countdown - decrement);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
 			varPlayer.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);

			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			ASSERT_TRUE (varPlayer.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE (varPlayer.ApplySubgameSelectionResponse(&subgameSelectionResponse));
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 2, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			const CPlayer &player(varPlayer);
			delete meters;
			delete orig;

			// Test #1.
			{
				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
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
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo non-NULL
		///	     decrement non-zero
		///	     decrement equal to countdown
		/// </summary>
		TEST(Player4, DecrementCountdown03Test)
		{
			// Test data -- inputs.
            const DWORD multiplier(1);
			const DWORD divider(10);
			const DWORD points(0);
			const DWORD countdown(2);
			const DWORD decrement(countdown);

			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);
			const DWORD currentCountdownExpected(divider);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
 			varPlayer.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);

			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			subgameSelectionResponse.m_subgameSessionParams.m_multiplier = multiplier;
			subgameSelectionResponse.m_subgameSessionParams.m_divider = divider;
			ASSERT_TRUE(varPlayer.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE(varPlayer.ApplySubgameSelectionResponse(&subgameSelectionResponse));
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + decrement, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
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
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo non-NULL
		///	     decrement non-zero
		///	     decrement greater than countdown
		///	     decrement less than countdown + divider
		/// </summary>
		TEST(Player4, DecrementCountdown04Test)
		{
			// Test data -- inputs.
			const DWORD multiplier(1);
			const DWORD divider(10);
			const DWORD points(0);
			const DWORD countdown(2);
			const DWORD decrement(countdown + 1);

			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);
			const DWORD currentCountdownExpected(divider + countdown - decrement);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			varPlayer.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);

			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			subgameSelectionResponse.m_subgameSessionParams.m_multiplier = multiplier;
			subgameSelectionResponse.m_subgameSessionParams.m_divider = divider;
			ASSERT_TRUE(varPlayer.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE(varPlayer.ApplySubgameSelectionResponse(&subgameSelectionResponse));
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + decrement, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
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
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo non-NULL
		///	     decrement non-zero
		///	     decrement equal to countdown + divider
		/// </summary>
		TEST(Player4, DecrementCountdown05Test)
		{
			// Test data -- inputs.
			const DWORD multiplier(1);
			const DWORD divider(10);
			const DWORD points(0);
			const DWORD countdown(2);
			const DWORD decrement(countdown + divider);

			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);
			const DWORD currentCountdownExpected(divider);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
 			varPlayer.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);

			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			subgameSelectionResponse.m_subgameSessionParams.m_multiplier = multiplier;
			subgameSelectionResponse.m_subgameSessionParams.m_divider = divider;
			ASSERT_TRUE(varPlayer.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE(varPlayer.ApplySubgameSelectionResponse(&subgameSelectionResponse));
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + decrement, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
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
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo non-NULL
		///	     decrement non-zero
		///	     decrement greater than countdown + divider
		/// </summary>
		TEST(Player4, DecrementCountdown06Test)
		{
			// Test data -- inputs.
			const DWORD multiplier(1);
			const DWORD divider(10);
			const DWORD points(0);
			const DWORD countdown(2);
			const DWORD decrement(countdown + divider + 1);

			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);
			const DWORD currentCountdownExpected((divider + countdown - decrement % divider) % divider);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
 			varPlayer.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);

			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			subgameSelectionResponse.m_subgameSessionParams.m_multiplier = multiplier;
			subgameSelectionResponse.m_subgameSessionParams.m_divider = divider;
			ASSERT_TRUE(varPlayer.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE(varPlayer.ApplySubgameSelectionResponse(&subgameSelectionResponse));
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + decrement, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
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
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo non-NULL
		///	     decrement non-zero
		///	     newSessionPoints zero
		/// </summary>
		TEST(Player4, DecrementCountdown07Test)
		{
			// Test data -- inputs.
			const DWORD multiplier(1);
			const DWORD divider(10);
			const DWORD points(0);
			const DWORD countdown(2);
			const DWORD decrement(1);

			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);
			const DWORD currentCountdownExpected((divider + countdown - decrement % divider) % divider);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
			varPlayer.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);

			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			subgameSelectionResponse.m_subgameSessionParams.m_multiplier = multiplier;
			subgameSelectionResponse.m_subgameSessionParams.m_divider = divider;
			ASSERT_TRUE(varPlayer.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE(varPlayer.ApplySubgameSelectionResponse(&subgameSelectionResponse));
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + decrement, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
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
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo non-NULL
		///	     decrement non-zero
		///	     newSessionPoints greater than zero
		///     m_bPartialBonusing true
		/// </summary>
		TEST(Player4, DecrementCountdown08Test)
		{
			// Test data -- inputs.
			const DWORD multiplier(1);
			const DWORD divider(10);
			const DWORD points(0);
			const DWORD countdown(2);
			const DWORD decrement(countdown);

			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);
			const DWORD currentCountdownExpected(divider);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPartialBonusAward(true);
			varPlayer.ConfigSettings(varConfig, egmConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
 			varPlayer.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);

			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			subgameSelectionResponse.m_subgameSessionParams.m_multiplier = multiplier;
			subgameSelectionResponse.m_subgameSessionParams.m_divider = divider;
			ASSERT_TRUE(varPlayer.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE(varPlayer.ApplySubgameSelectionResponse(&subgameSelectionResponse));
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + decrement, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
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
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo non-NULL
		///	     decrement non-zero
		///	     newSessionPoints greater than zero
		///     m_bPartialBonusing false
		/// </summary>
		TEST(Player4, DecrementCountdown09Test)
		{
			// Test data -- inputs.
			const DWORD multiplier(1);
			const DWORD divider(10);
			const DWORD points(0);
			const DWORD countdown(2);
			const DWORD decrement(countdown);

			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);
			const DWORD currentCountdownExpected(divider);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPartialBonusAward(false);
			varPlayer.ConfigSettings(varConfig, egmConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
 			varPlayer.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);

			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			subgameSelectionResponse.m_subgameSessionParams.m_multiplier = multiplier;
			subgameSelectionResponse.m_subgameSessionParams.m_divider = divider;
			ASSERT_TRUE(varPlayer.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE(varPlayer.ApplySubgameSelectionResponse(&subgameSelectionResponse));
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + decrement, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
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
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo non-NULL
		///	     decrement non-zero
		///     m_bPartialBonusing false
		/// </summary>
		TEST(Player4, DecrementCountdown10Test)
		{
			// Test data -- inputs.
			const DWORD multiplier(1);
			const DWORD divider(10);
			const DWORD points(0);
			const DWORD countdown(2);
			const DWORD decrement(countdown);

			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);
			const DWORD currentCountdownExpected(divider);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPartialBonusAward(false);
			varPlayer.ConfigSettings(varConfig, egmConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
 			varPlayer.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);

			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			subgameSelectionResponse.m_subgameSessionParams.m_multiplier = multiplier;
			subgameSelectionResponse.m_subgameSessionParams.m_divider = divider;
			ASSERT_TRUE(varPlayer.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE(varPlayer.ApplySubgameSelectionResponse(&subgameSelectionResponse));
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + decrement, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
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
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo non-NULL
		///	     decrement non-zero
		///     m_bPartialBonusing true
		///     remaining decrement equal to zero
		/// </summary>
		TEST(Player4, DecrementCountdown11Test)
		{
			// Test data -- inputs.
			const DWORD multiplier(1);
			const DWORD divider(10);
			const DWORD points(0);
			const DWORD countdown(2);
			const DWORD decrement(countdown);

			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);
			const DWORD currentCountdownExpected(divider);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPartialBonusAward(true);
			varPlayer.ConfigSettings(varConfig, egmConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
 			varPlayer.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);

			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			subgameSelectionResponse.m_subgameSessionParams.m_multiplier = multiplier;
			subgameSelectionResponse.m_subgameSessionParams.m_divider = divider;
			ASSERT_TRUE(varPlayer.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE(varPlayer.ApplySubgameSelectionResponse(&subgameSelectionResponse));
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + decrement, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
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
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo non-NULL
		///	     decrement non-zero
		///     m_bPartialBonusing true
		///     remaining decrement greater than zero
		///     multiplier equal to 1
		/// </summary>
		TEST(Player4, DecrementCountdown12Test)
		{
			// Test data -- inputs.
			const DWORD multiplier(1);
			const DWORD divider(10);
			const DWORD points(0);
			const DWORD countdown(2);
			const DWORD decrement(1);

			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);
			const DWORD currentCountdownExpected(countdown - decrement);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPartialBonusAward(true);
			varPlayer.ConfigSettings(varConfig, egmConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
 			varPlayer.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);

			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			subgameSelectionResponse.m_subgameSessionParams.m_multiplier = multiplier;
			subgameSelectionResponse.m_subgameSessionParams.m_divider = divider;
			ASSERT_TRUE(varPlayer.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE(varPlayer.ApplySubgameSelectionResponse(&subgameSelectionResponse));
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + decrement, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
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
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo non-NULL
		///	     decrement non-zero
		///     m_bPartialBonusing true
		///     remaining decrement greater than zero
		///     multiplier greater than 1
		///     new session points zero
		/// </summary>
		TEST(Player4, DecrementCountdown13Test)
		{
			// Test data -- inputs.
			const DWORD multiplier(2);
			const DWORD divider(10);
			const DWORD points(0);
			const DWORD countdown(2);
			const DWORD decrement(1);

			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);
			const DWORD currentCountdownExpected(countdown - decrement);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPartialBonusAward(true);
			varPlayer.ConfigSettings(varConfig, egmConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
 			varPlayer.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);

			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			subgameSelectionResponse.m_subgameSessionParams.m_multiplier = multiplier;
			subgameSelectionResponse.m_subgameSessionParams.m_divider = divider;
			ASSERT_TRUE(varPlayer.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE(varPlayer.ApplySubgameSelectionResponse(&subgameSelectionResponse));
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + decrement, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
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
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo non-NULL
		///	     decrement non-zero
		///     m_bPartialBonusing true
		///     remaining decrement greater than zero
		///     multiplier greater than 1
		///     new session points greater than zero
		//      AwardBonusPoints() false
		/// </summary>
		TEST(Player4, DecrementCountdown14Test)
		{
			// Test data -- inputs.
			const DWORD multiplier(2);
			const DWORD divider(10);
			const DWORD points(0);
			const DWORD countdown(divider);
			const DWORD decrement(countdown - 1);

			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(false);
			const DWORD currentCountdownExpected(countdown - decrement);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPartialBonusAward(true);
			varPlayer.ConfigSettings(varConfig, egmConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
 			varPlayer.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);

			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			subgameSelectionResponse.m_subgameSessionParams.m_multiplier = multiplier;
			subgameSelectionResponse.m_subgameSessionParams.m_divider = divider;
			ASSERT_TRUE(varPlayer.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE(varPlayer.ApplySubgameSelectionResponse(&subgameSelectionResponse));
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + decrement, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
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
		///     CPlayer::DecrementCountdown()
		/// Test with:
		///	     playerInfo non-NULL
		///	     decrement non-zero
		///     m_bPartialBonusing true
		///     remaining decrement greater than zero
		///     multiplier greater than 1
		///     new session points greater than zero
		//      AwardBonusPoints() true
		/// </summary>
		TEST(Player4, DecrementCountdown15Test)
		{
			// Test data -- inputs.
			const DWORD multiplier(2);
			const DWORD divider(10);
			const DWORD points(0);
			const DWORD countdown(divider);
			const DWORD decrement(countdown - 1);

			// Test data -- expected results.
			const bool bonusPointsAwardedExpected(true);
			const DWORD currentCountdownExpected(countdown - decrement);

			// Setup.
			CPlayer varPlayer;
			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetPartialBonusAward(true);
			varPlayer.ConfigSettings(varConfig, egmConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			playerInfo->SetMultiplierDivider(multiplier, divider);
			playerInfo->SetPointsAndCountdown(points, countdown, CPollerPlayerResponseMessage::DEFAULT_COUNTDOWN_HIGH_PRECISION);
 			varPlayer.SetPlayerInfoAndMultiplierDividerCountdown(playerInfo, fallbackMultiplier, fallbackDivider, true, 1, 1, AccountingType::ACCOUNTING_COINS_IN, false);

			// Perform operation(s) to be tested.
			SubgameSelectionResponse subgameSelectionResponse;
			subgameSelectionResponse.m_subgameSessionParams.m_multiplier = multiplier;
			subgameSelectionResponse.m_subgameSessionParams.m_divider = divider;
			ASSERT_TRUE(varPlayer.SetGameSession(subgameSelectionResponse.m_subgameSessionParams));
			ASSERT_TRUE(varPlayer.ApplySubgameSelectionResponse(&subgameSelectionResponse));

			CBonusEvents varBonusEvents;
			CMultiplierBonusEvent *bonusEvent = new CMultiplierBonusEvent(0, (time_t)0, (time_t)INT32_MAX, 1);
			varBonusEvents.AddEvent(bonusEvent );
			const CBonusEvents &bonusEvents(varBonusEvents);
			varPlayer.SetBonusEvents(&bonusEvents);

			// Perform operation(s) to be tested.
			CMeters *meters = new CMeters(MEMORY_NONE);
			CMeters *orig = new CMeters(MEMORY_NONE);
			orig->SetMeter(gCI, 1, true, false);
			meters->SetMeter(gCI, 1 + decrement, true, false);
			const bool bonusPointsAwardedResult = varPlayer.DecrementCountdown(meters, orig, true);
			delete meters;
			delete orig;
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				bool areEqual(bonusPointsAwardedResult == bonusPointsAwardedExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED bonusPointsAwardedResult=%d expected=%d in %s"),
					(int)bonusPointsAwardedResult,
					(int)bonusPointsAwardedExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
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
		///     CPlayer::GetOutstandingBonusEvent()
		/// Test with:
		///	     bonusEvents NULL
		/// </summary>
		TEST(Player4, GetOutstandingBonusEvent0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.

			// Setup.
			CPlayer varPlayer;

			// Perform operation(s) to be tested.
			const CBonusEvent *outstandingBonusEventResult = varPlayer.GetOutstandingBonusEvent(true);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED outstandingBonusEventResult=%p expected=NULL in %s"),
					(const void *)outstandingBonusEventResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_EQ((void *)nullptr, (void *)outstandingBonusEventResult) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetOutstandingBonusEvent()
		/// Test with:
		///	     bonusEvents non-NULL
		/// </summary>
		TEST(Player4, GetOutstandingBonusEvent1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.

			// Setup.
			CPlayer varPlayer;
			const CBonusEvents bonusEvents;
			varPlayer.SetBonusEvents(&bonusEvents);

			// Perform operation(s) to be tested.
			const CBonusEvent *outstandingBonusEventResult = varPlayer.GetOutstandingBonusEvent(true);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				string failedMsg;
				failedMsg = FormatString(_T("FAILED outstandingBonusEventResult=%p expected=NULL in %s"),
					(const void *)outstandingBonusEventResult,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_EQ((void *)nullptr, (void *)outstandingBonusEventResult) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ResetSession()
		/// Test with:
		///     playerInfo NULL
		///     bonusEvents NULL
		/// </summary>
		TEST(Player4, ResetSession0Test)
		{
			// Test data -- inputs.
			const time_t utcTime(123);

			// Test data -- expected results.
			const time_t lastUpdateSentTimeExpected(0);

			// Setup.
			CPlayer varPlayer;

			// Perform operation(s) to be tested.
			varPlayer.ResetSession(utcTime);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const time_t lastUpdateSentTimeResult(player.GetLastUpdateSentTime());

				bool areEqual(lastUpdateSentTimeResult == lastUpdateSentTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED lastUpdateSentTimeResult=%lld expected=%lld in %s"),
					(long long)lastUpdateSentTimeResult,
					(long long)lastUpdateSentTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ResetSession()
		/// Test with:
		///     playerInfo non-NULL
		///     bonusEvents NULL
		/// </summary>
		TEST(Player4, ResetSession1Test)
		{
			// Test data -- inputs.
			const time_t utcTime(123);

			// Test data -- expected results.
			const time_t lastUpdateSentTimeExpected(utcTime);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo = new CPlayerInfo();
			varPlayer.SetPlayerInfo(playerInfo);

			// Perform operation(s) to be tested.
			varPlayer.ResetSession(utcTime);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const time_t lastUpdateSentTimeResult(player.GetLastUpdateSentTime());

				bool areEqual(lastUpdateSentTimeResult == lastUpdateSentTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED lastUpdateSentTimeResult=%lld expected=%lld in %s"),
					(long long)lastUpdateSentTimeResult,
					(long long)lastUpdateSentTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::ResetSession()
		/// Test with:
		///     playerInfo NULL
		///     bonusEvents non-NULL
		/// </summary>
		TEST(Player4, ResetSession2Test)
		{
			// Test data -- inputs.
			const time_t utcTime(123);

			// Test data -- expected results.
			const time_t lastUpdateSentTimeExpected(0);

			// Setup.
			CPlayer varPlayer;
			const CBonusEvents bonusEvents;
			varPlayer.SetBonusEvents(&bonusEvents);

			// Perform operation(s) to be tested.
			varPlayer.ResetSession(utcTime);
			const CPlayer &player(varPlayer);

			// Test #1.
			{
				const time_t lastUpdateSentTimeResult(player.GetLastUpdateSentTime());

				bool areEqual(lastUpdateSentTimeResult == lastUpdateSentTimeExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED lastUpdateSentTimeResult=%lld expected=%lld in %s"),
					(long long)lastUpdateSentTimeResult,
					(long long)lastUpdateSentTimeExpected,
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetTotalPoints()
		/// Test with:
		///     playerInfo NULL
		/// </summary>
		TEST(Player4, GetTotalPoints0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const DWORD totalPointsExpected(0);

			// Setup.

			// Perform operation(s) to be tested.
			const CPlayer player;

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
		///     CPlayer::GetTotalPoints()
		/// Test with:
		///     playerInfo non-NULL
		/// </summary>
		TEST(Player4, GetTotalPoints1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const DWORD totalPointsExpected(0);

			// Setup.
			CPlayer varPlayer;
			CPlayerInfo *playerInfo = new CPlayerInfo();
			varPlayer.SetPlayerInfo(playerInfo);

			// Perform operation(s) to be tested.
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
		///     CPlayer::GetTotalPointsAsCashString()
		/// </summary>
		TEST(Player4, GetTotalPointsAsCashStringTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string totalPointsAsCashStringExpected(_T("$0.00"));

			// Setup.
			const CPlayer player;

			// Perform operation(s) to be tested.
			const string totalPointsAsCashStringResult = player.GetTotalPointsAsCashString();

			// Test #1.
			{
				bool areEqual(totalPointsAsCashStringResult == totalPointsAsCashStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED totalPointsAsCashStringResult=%s expected=%s in %s"),
					static_cast<LPCTSTR>(totalPointsAsCashStringResult.c_str()),
					static_cast<LPCTSTR>(totalPointsAsCashStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetSessionPointsAsCashString()
		/// </summary>
		TEST(Player4, GetSessionPointsAsCashStringTest)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string sessionPointsAsCashStringExpected(_T("$0.00"));

			// Setup.
			const CPlayer player;

			// Perform operation(s) to be tested.
			const string sessionPointsAsCashStringResult = player.GetSessionPointsAsCashString();

			// Test #1.
			{
				bool areEqual(sessionPointsAsCashStringResult == sessionPointsAsCashStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED sessionPointsAsCashStringResult=%s expected=%s in %s"),
					static_cast<LPCTSTR>(sessionPointsAsCashStringResult.c_str()),
					static_cast<LPCTSTR>(sessionPointsAsCashStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCompsAsCashString()
		/// Test with:
		///     playerInfo NULL
		/// </summary>
		TEST(Player4, GetCompsAsCashString0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string compsPointsAsCashStringExpected(_T(""));

			// Setup.
			const CPlayer player;

			// Perform operation(s) to be tested.
			const string compsPointsAsCashStringResult = player.GetCompsAsCashString();

			// Test #1.
			{
				bool areEqual(compsPointsAsCashStringResult == compsPointsAsCashStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED compsPointsAsCashStringResult=%s expected=%s in %s"),
					static_cast<LPCTSTR>(compsPointsAsCashStringResult.c_str()),
					static_cast<LPCTSTR>(compsPointsAsCashStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCompsAsCashString()
		/// Test with:
		///     playerInfo non-NULL
		/// m_showCardInCompDisplay false
		/// </summary>
		TEST(Player4, GetCompsAsCashString1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string compsPointsAsCashStringExpected(_T(""));

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayComps(false);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const string compsPointsAsCashStringResult = player.GetCompsAsCashString();

			// Test #1.
			{
				bool areEqual(compsPointsAsCashStringResult == compsPointsAsCashStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED compsPointsAsCashStringResult=%s expected=%s in %s"),
					static_cast<LPCTSTR>(compsPointsAsCashStringResult.c_str()),
					static_cast<LPCTSTR>(compsPointsAsCashStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetCompsAsCashString()
		/// Test with:
		///     playerInfo non-NULL
		/// m_showCardInCompDisplay true
		/// </summary>
		TEST(Player4, GetCompsAsCashString2Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string compsPointsAsCashStringExpected(_T("$0.00"));

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayComps(true);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			const string compsPointsAsCashStringResult = player.GetCompsAsCashString();

			// Test #1.
			{
				bool areEqual(compsPointsAsCashStringResult == compsPointsAsCashStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED compsPointsAsCashStringResult=%s expected=%s in %s"),
					static_cast<LPCTSTR>(compsPointsAsCashStringResult.c_str()),
					static_cast<LPCTSTR>(compsPointsAsCashStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetInPlayText()
		/// Test with:
		///     m_inPlayDisplayType != IPDT_TOTAL    -- AKA config.DisplayPlayerPtsTotal() = false
		///     m_inPlayDisplayType != IPDT_TOTALCASH   -- AKA config.DisplayPtsTotalAsCashSession() = false
		///     m_inPlayDisplayType != IPDT_GOOD_CARD   -- AKA config.DisplayGoodLuckAndSession() = true
		/// </summary>
		TEST(Player4, GetInPlayText0Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string inPlaySingleLineStringExpected;

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPlayerPtsTotal(false);
			varConfig.SetDisplayCountdownAndSession(false);
			varConfig.SetDisplayGoodLuckAndSession(true);
			const CConfig &config(varConfig);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string inPlaySingleLineStringResult;
			vector<string> args;
			player.GetInPlayText(inPlaySingleLineStringResult, args);

			// Test #1.
			{
				bool areEqual(inPlaySingleLineStringResult == inPlaySingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED inPlaySingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(inPlaySingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(inPlaySingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_TRUE(args.empty()) << "Invalid Argument Count";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetInPlayText()
		/// Test with:
		///     m_inPlayDisplayType != IPDT_TOTAL    -- AKA config.DisplayPlayerPtsTotal() = false
		///     m_inPlayDisplayType != IPDT_TOTALCASH   -- AKA config.DisplayPtsTotalAsCashSession() = false
		///     m_inPlayDisplayType == IPDT_GOOD_CARD   -- AKA ...
		///         config.DisplayPlayerPtsTotal() = false
		///         config.DisplayCountdownAndSession() = false
		///         config.DisplayGoodLuckAndSession() = false
		///         config.DisplayTotalAndSessionPtsNum() = false
		///         config.DisplayTotalPtsSessionPtsAsCash() = false
		///         config.DisplayGoodLuckAndSessionPts() = false
		///         config.DisplayPtsTotalAsCashSession() = false
		/// </summary>
		TEST(Player4, GetInPlayText1Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string inPlaySingleLineStringExpected("good_card_read_inplay");

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			const CEGMConfig egmConfig(MEMORY_NONE);
			varConfig.SetDisplayPlayerPtsTotal(false);
			varConfig.SetDisplayCountdownAndSession(false);
			varConfig.SetDisplayGoodLuckAndSession(false);
			varConfig.SetDisplayTotalAndSessionPtsNum(false);
			varConfig.SetDisplayTotalPtsSessionPtsAsCash(false);
			varConfig.SetDisplayGoodLuckAndSessionPts(false);
			varConfig.SetDisplayPtsTotalAsCashSession(false);
			const CConfig &config(varConfig);

			CNotificationTextConfig varNotificationTextConfig;

			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string inPlaySingleLineStringResult;
			vector<string> args;
			player.GetInPlayText(inPlaySingleLineStringResult, args);

			// Test #1.
			{
				bool areEqual(inPlaySingleLineStringResult == inPlaySingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED inPlaySingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(inPlaySingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(inPlaySingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_TRUE(args.empty()) << "Invalid Argument Count";
			}

			// Cleanup.
		}

		/// <summary>
		/// Code coverage test for:
		///     CPlayer::GetInPlayText()
		/// Test with:
		///     m_inPlayDisplayType != IPDT_TOTAL    -- AKA config.DisplayPlayerPtsTotal() = false
		///     m_inPlayDisplayType == IPDT_TOTALCASH   -- AKA ...
		///         config.DisplayPlayerPtsTotal() = false
		///         config.DisplayCountdownAndSession() = false
		///         config.DisplayGoodLuckAndSession() = false
		///         config.DisplayTotalAndSessionPtsNum() = false
		///         config.DisplayTotalPtsSessionPtsAsCash() = false
		///         config.DisplayGoodLuckAndSessionPts() = false
		///         config.DisplayPtsTotalAsCashSession() = true
		///         config.DisplayPlayerName() == false
		///         config.GetSessionPointsMessage().IsEmpty() == true
		/// </summary>
		TEST(Player4, GetInPlayText2Test)
		{
			// Test data -- inputs.

			// Test data -- expected results.
			const string inPlaySingleLineStringExpected("points_as_cash");

			// Setup.
			CPlayer varPlayer;

			CConfig varConfig(false, "");
			varConfig.SetDisplayPlayerPtsTotal(false);
			varConfig.SetDisplayCountdownAndSession(false);
			varConfig.SetDisplayGoodLuckAndSession(false);
			varConfig.SetDisplayTotalAndSessionPtsNum(false);
			varConfig.SetDisplayTotalPtsSessionPtsAsCash(false);
			varConfig.SetDisplayGoodLuckAndSessionPts(false);
			varConfig.SetDisplayPtsTotalAsCashSession(true);
			varConfig.SetDisplayPlayerName(false);
			const CConfig &config(varConfig);
			const CEGMConfig egmConfig(MEMORY_NONE);
			varPlayer.ConfigSettings(config, egmConfig);

			CPlayerInfo *playerInfo = new CPlayerInfo();
			varPlayer.SetPlayerInfo(playerInfo);
			const CPlayer &player(varPlayer);

			// Perform operation(s) to be tested.
			string inPlaySingleLineStringResult;
			vector<string> args;
			player.GetInPlayText(inPlaySingleLineStringResult, args);

			// Test #1.
			{
				bool areEqual(inPlaySingleLineStringResult == inPlaySingleLineStringExpected);
				string failedMsg;
				failedMsg = FormatString(_T("FAILED inPlaySingleLineStringResult=\"%s\" expected=\"%s\" in %s"),
					static_cast<LPCTSTR>(inPlaySingleLineStringResult.c_str()),
					static_cast<LPCTSTR>(inPlaySingleLineStringExpected.c_str()),
					static_cast<LPCTSTR>(__TFUNCTION__));
				ASSERT_TRUE(areEqual) << failedMsg;

				ASSERT_EQ(args.size(), 1) << "Invalid Argument Count";
				ASSERT_EQ(args[0], "$0.00") << "Invalid Argument Value";
			}

			// Cleanup.
		}
}
