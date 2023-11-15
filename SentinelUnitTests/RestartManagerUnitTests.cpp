#include "stdafx.h"
#include "RestartManager.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;
/*
	class RestartManagerUnitTests : public ::testing::Test
	{
	public:
	};

		TEST(RestartManagerUnitTests, RestartManagerConstructorUnitTest)
		{
			//Setup
			CRestartManager restartManager;
			DWORD lastTickCount(CUtilities::GetTickCount());

			//Tests
			ASSERT_FALSE(restartManager.NeedToPerformControlledRebootAfterNextDrop());
			ASSERT_FALSE(restartManager.NeedToPerformControlledRebootAsap());
			ASSERT_FALSE(restartManager.NeedToPerformControlledRebootForOsUpgrade());
			ASSERT_TRUE(restartManager.NeedToSendRebootWarningTransaction());
			ASSERT_FALSE(restartManager.IsShutdownInProcess());
			ASSERT_FALSE(restartManager.m_heartbeatReceivedFromUI);
			ASSERT_FALSE(restartManager.m_processingUiHeartbeats);
			ASSERT_FALSE(restartManager.m_restartUiAsap);
			ASSERT_TRUE(restartManager.m_lastUiRestartTime < lastTickCount + 1);
			ASSERT_TRUE(restartManager.m_uiHeartbeatReceived < lastTickCount + 1);
		}

		TEST(RestartManagerUnitTests, PerformRebootAfterNextDropUnitTest)
		{
			//Setup
			CRestartManager restartManager;

			//Tests
			ASSERT_FALSE(restartManager.NeedToPerformControlledRebootAfterNextDrop());
			ASSERT_TRUE(restartManager.NeedToSendRebootWarningTransaction());
			restartManager.SetToPerformRebootAfterNextDrop();
			ASSERT_TRUE(restartManager.NeedToPerformControlledRebootAfterNextDrop());
			ASSERT_FALSE(restartManager.NeedToSendRebootWarningTransaction());
		}

		TEST(RestartManagerUnitTests, PerformControlledRebootAsapUnitTest)
		{
			//Setup
			CRestartManager restartManager;

			//Tests
			ASSERT_FALSE(restartManager.NeedToPerformControlledRebootAsap());
			ASSERT_TRUE(restartManager.NeedToSendRebootWarningTransaction());
			restartManager.SetToPerformControlledRebootAsap();
			ASSERT_TRUE(restartManager.NeedToPerformControlledRebootAsap());
			ASSERT_FALSE(restartManager.NeedToSendRebootWarningTransaction());
		}

		TEST(RestartManagerUnitTests, PerformControlledRebootForOsUpgradeUnitTest)
		{
			//Setup
			CRestartManager restartManager;

			//Tests
			ASSERT_FALSE(restartManager.NeedToPerformControlledRebootForOsUpgrade());
			restartManager.SetToPerformControlledRebootForOsUpgrade();
			ASSERT_TRUE(restartManager.NeedToPerformControlledRebootForOsUpgrade());
		}
		
		TEST(RestartManagerUnitTests, ShutdownInProcessUnitTest)
		{
			//Setup
			CRestartManager restartManager;

			//Tests
			ASSERT_FALSE(restartManager.IsShutdownInProcess());
			restartManager.m_shutdownInProcess = true;
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
		}

// #pragma region IsOkayToDoRebootOrUiRestart
		TEST(RestartManagerUnitTests, IsOkayToDoSwfRestartUnitTest)
		{
			//Setup
			CRestartManager restartManager;

			//Tests
			ASSERT_FALSE(restartManager.GetPerformUiRestartAsapFlag());
			restartManager.SetPerformUiRestartAsapFlag(true);
			ASSERT_TRUE(restartManager.GetPerformUiRestartAsapFlag());
		}

		TEST(RestartManagerUnitTests, IsOkayToDoRebootOrUiRestartUnitTest)
		{
			//Setup
			CRestartManager restartManager;
			time_t currentUtcTime(CUtilities::GetUTCTime());
			time_t lastCreditMeterZeroTime(currentUtcTime);

			//Tests
			ASSERT_FALSE(restartManager.IsOkayToDoRebootOrUiRestart(lastCreditMeterZeroTime, true, currentUtcTime));
			ASSERT_FALSE(restartManager.IsOkayToDoRebootOrUiRestart(lastCreditMeterZeroTime, false, currentUtcTime));
			currentUtcTime += FOURMINUTESTIMESPAN + 1;
			ASSERT_TRUE(restartManager.IsOkayToDoRebootOrUiRestart(lastCreditMeterZeroTime, false, currentUtcTime));
		}

		/// <summary>
		/// Functional test for constants and/or defines.
		/// </summary>
		TEST(RestartManagerUnitTests, IsOkayToDoRebootOrUiRestartUnitTest_Const)
		{
			// Perform operation(s) and/or test(s).
			ASSERT_EQ((LONGLONG)240, FOURMINUTESTIMESPAN);
			ASSERT_EQ((LONGLONG)-60, MINUSONEMINUTETIMESPAN);
		}

		/// <summary>
		/// Functional test for default values.
		/// </summary>
		TEST(RestartManagerUnitTests, IsOkayToDoRebootOrUiRestartUnitTest_Default)
		{
			// Setup.
			CRestartManager restartManager;

			// Perform operation(s) and/or test(s).
			ASSERT_EQ((LONGLONG)240, restartManager.m_creditZeroTimeSpan);
			ASSERT_EQ((LONGLONG)-60, restartManager.m_minCreditZeroTimeSpan);
		}

		/// <summary>
		/// Functional test for:
		///     CRestartManager::IsOkayToDoRebootOrUiRestart()
		/// Test min limit for no timeout
		/// </summary>
		TEST(RestartManagerUnitTests, IsOkayToDoRebootOrUiRestartUnitTest_Min)
		{
			// Data.
			bool hasSignificantCredits(false);

			// Setup.
			CRestartManager restartManager;
			time_t lastCreditMeterZeroTime(CUtilities::GetUTCTime());
			lastCreditMeterZeroTime += 3600; // Make lastCreditMeterZeroTime from initialization or last change to be one hour greater than current time.

			// Perform operation(s) and/or test(s).
			ASSERT_TRUE(restartManager.IsOkayToDoRebootOrUiRestart(lastCreditMeterZeroTime, hasSignificantCredits,
				lastCreditMeterZeroTime + restartManager.m_minCreditZeroTimeSpan + -1));
			ASSERT_FALSE(restartManager.IsOkayToDoRebootOrUiRestart(lastCreditMeterZeroTime, hasSignificantCredits,
				lastCreditMeterZeroTime + restartManager.m_minCreditZeroTimeSpan + 0));
			ASSERT_FALSE(restartManager.IsOkayToDoRebootOrUiRestart(lastCreditMeterZeroTime, hasSignificantCredits,
				lastCreditMeterZeroTime + restartManager.m_minCreditZeroTimeSpan + 1));
		}

		/// <summary>
		/// Functional test for:
		///     CRestartManager::IsOkayToDoRebootOrUiRestart()
		/// Test max limit for no timeout.
		/// </summary>
		TEST(RestartManagerUnitTests, IsOkayToDoRebootOrUiRestartUnitTest_Max)
		{
			// Data.
			bool hasSignificantCredits(false);

			// Setup.
			CRestartManager restartManager;
			time_t utcTime(CUtilities::GetUTCTime());
			time_t lastCreditMeterZeroTime(utcTime);

			// Perform operation(s) and/or test(s).
			ASSERT_FALSE(restartManager.IsOkayToDoRebootOrUiRestart(lastCreditMeterZeroTime, hasSignificantCredits,
				lastCreditMeterZeroTime + restartManager.m_creditZeroTimeSpan + -1));
			ASSERT_FALSE(restartManager.IsOkayToDoRebootOrUiRestart(lastCreditMeterZeroTime, hasSignificantCredits,
				lastCreditMeterZeroTime + restartManager.m_creditZeroTimeSpan + 0));
			ASSERT_TRUE(restartManager.IsOkayToDoRebootOrUiRestart(lastCreditMeterZeroTime, hasSignificantCredits,
				lastCreditMeterZeroTime + restartManager.m_creditZeroTimeSpan + 1));
		}

		/// <summary>
		/// Functional test for:
		///     CRestartManager::IsOkayToDoRebootOrUiRestart()
		/// Test that timeout occurs immediately when there are no signficant credits and the time has gone backwards
		/// since lastCreditMeterZeroTime was initialized or set -- and there are no credits on the credit meter.
		/// </summary>
		TEST(RestartManagerUnitTests, IsOkayToDoRebootOrUiRestartUnitTest_BackwardsTimeNoCredits)
		{
			// Data.
			bool hasSignificantCredits(false);

			// Setup.
			CRestartManager restartManager;
			time_t utcTime(CUtilities::GetUTCTime());
			time_t lastCreditMeterZeroTime(utcTime + 3600); // Make lastCreditMeterZeroTime one hour greater than current time.

			// Perform operation(s) and test(s).
			ASSERT_TRUE(restartManager.IsOkayToDoRebootOrUiRestart(lastCreditMeterZeroTime, hasSignificantCredits, utcTime));
		}

		/// <summary>
		/// Functional test for:
		///     CRestartManager::IsOkayToDoRebootOrUiRestart()
		/// Test that timeout has no effect when there are signficant credits and the time has gone backwards
		/// since lastCreditMeterZeroTime was initialized or set -- and there are significant credits on the credit meter.
		/// </summary>
		TEST(RestartManagerUnitTests, IsOkayToDoRebootOrUiRestartUnitTest_BackwardsTimeCredits)
		{
			// Data.
			bool hasSignificantCredits(true);

			// Setup.
			CRestartManager restartManager;
			time_t utcTime(CUtilities::GetUTCTime());
			time_t lastCreditMeterZeroTime(utcTime + 3600); // Make lastCreditMeterZeroTime one hour greater than current time.

			// Perform operation(s) and test(s).
			ASSERT_FALSE(restartManager.IsOkayToDoRebootOrUiRestart(lastCreditMeterZeroTime, hasSignificantCredits, utcTime));
		}
// #pragma endregion IsOkayToDoRebootOrUiRestart

		TEST(RestartManagerUnitTests, PerformPostDropRebootUnitTest)
		{
			//Setup
			CRestartManager restartManager;

			//Tests
			restartManager.PerformPostDropTasks();
			ASSERT_FALSE(restartManager.NeedToPerformControlledRebootAfterNextDrop());
			ASSERT_FALSE(restartManager.NeedToPerformControlledRebootAsap());			
			restartManager.SetToPerformRebootAfterNextDrop();
			restartManager.PerformPostDropTasks();
			ASSERT_TRUE(restartManager.NeedToPerformControlledRebootAfterNextDrop());
			ASSERT_TRUE(restartManager.NeedToPerformControlledRebootAsap());
		}

		TEST(RestartManagerUnitTests, PerformPostDropSwfRestartUnitTest)
		{
			//Setup
			CRestartManager restartManager;

			//Tests
			restartManager.PerformPostDropTasks();
			ASSERT_FALSE(restartManager.NeedToPerformControlledRebootAfterNextDrop());
			ASSERT_FALSE(restartManager.NeedToPerformControlledRebootAsap());
			ASSERT_FALSE(restartManager.m_restartUiAsap);
			restartManager.PerformPostDropTasks();
			ASSERT_FALSE(restartManager.m_restartUiAsap);
		}

		TEST(RestartManagerUnitTests, PerformControlledRebootAsapCheckRebootOverrideFalseUnitTest)
		{
			//Setup
			CRestartManager restartManager;
			bool isOsUpgrade = false;

			CPlayer* player(new CPlayer());
			CConfig config(false, "");
			CEmployee *employee = new CEmployee(
				new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() + (10 * SECONDS_PER_MINUTE)),
				config, false, false, 0, 0);
			time_t currentUtcTime(CUtilities::GetUTCTime());

			//Tests
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(player, employee, currentUtcTime, false, false, true, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());			
			restartManager.SetToPerformControlledRebootAsap();

			//Active player
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(player, NULL, currentUtcTime, false, false, false, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());

			//Active employee
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, employee, currentUtcTime, false, false, false, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());

			//Meter capture is in progress
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, true, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());

			//EGM door is open
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, true, false, false, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());

			//EGM has credits in the game
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, true, false, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());

			//Credit zero time span check is not met
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());

			//Credit zero time span check is met
			restartManager.m_creditZeroTimeSpan = (1);
			CUtilities::Sleep(2000);
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, false, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Shutdown already in process
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, false, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Cleanup
			delete player;
			delete employee;
		}

		TEST(RestartManagerUnitTests, PerformControlledRebootAsapRebootCheckOverrideTrueUnitTest)
		{
			//Setup
			CRestartManager restartManager;
			bool isOsUpgrade = false;
			CPlayer* player(new CPlayer());
			CConfig config(false, "");
			CEmployee *employee = new CEmployee(
				new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() + (10 * SECONDS_PER_MINUTE)),
				config, false, false, 0, 0);
			time_t currentUtcTime(CUtilities::GetUTCTime());

			//Tests
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(player, employee, currentUtcTime, false, false, true, true, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());
			restartManager.SetToPerformControlledRebootAsap();

			//Active player
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(player, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Active employee
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, employee, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Meter capture is in progress
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, true, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//EGM door is open
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, true, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//EGM has credits in the game
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, true, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Credit zero time span check is not met
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Credit zero time span check is met
			restartManager.m_creditZeroTimeSpan = (1);
			CUtilities::Sleep(2000);
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(player, employee, currentUtcTime, false, false, true, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
			restartManager.SetToPerformControlledRebootAsap();

			//Active player
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(player, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Active employee
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, employee, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Meter capture is in progress
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, true, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//EGM door is open
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, true, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//EGM has credits in the game
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, true, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Shutdown already in process
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Cleanup
			delete player;
			delete employee;
		}

		TEST(RestartManagerUnitTests, PerformControlledRemoteRebootCheckOverrideFalseUnitTest)
		{
			//Setup
			CRestartManager restartManager;
			bool isOsUpgrade = false;
		
			CPlayer* player(new CPlayer());
			CConfig config(false, "");
			CEmployee *employee = new CEmployee(
				new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() + (10 * SECONDS_PER_MINUTE)),
				config, false, false, 0, 0);
			time_t currentUtcTime(CUtilities::GetUTCTime());
		
			//Tests
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(player, employee, currentUtcTime, false, false, true, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess()); 		
			restartManager.SetToPerformControlledRemoteReboot();
		
			//Active player
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(player, NULL, currentUtcTime, false, false, false, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());
		
			//Active employee
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, employee, currentUtcTime, false, false, false, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());
		
			//Meter capture is in progress
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, true, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());
		
			//EGM door is open
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, true, false, false, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());

			//EGM has credits in the game
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, true, false, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());
		
			//Credit zero time span check is not met
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());
		
			//Credit zero time span check is met
			restartManager.m_creditZeroTimeSpan = (1);
			CUtilities::Sleep(2000);
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, false, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
		
			//Shutdown already in process
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, false, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
					
			//Remote reboot pending tilt
			restartManager.m_shutdownInProcess = false;
			restartManager.SetFirstTimeAfterRemoteRebootRequest(true);
			restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, false, isOsUpgrade);
			ASSERT_TRUE(restartManager.IsSendRemoteRebootPendingTilt());

			restartManager.SetSendRemoteRebootPendingTilt(false);
			restartManager.SetFirstTimeAfterRemoteRebootRequest(false);
			restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, false, isOsUpgrade);
			ASSERT_FALSE(restartManager.IsSendRemoteRebootPendingTilt());

			//Cleanup
			delete player;
			delete employee;
		}

		TEST(RestartManagerUnitTests, PerformControlledRemoteRebootCheckOverrideTrueUnitTest)
		{
			//Setup
			CRestartManager restartManager;
			bool isOsUpgrade = false;
			CPlayer* player(new CPlayer());
			CConfig config(false, "");
			CEmployee *employee = new CEmployee(
				new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() + (10 * SECONDS_PER_MINUTE)),
				config, false, false, 0, 0);
			time_t currentUtcTime(CUtilities::GetUTCTime());
		
			//Tests
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(player, employee, currentUtcTime, false, false, true, true, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());
			restartManager.SetToPerformControlledRemoteReboot();
		
			//Active player
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(player, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
		
			//Active employee
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, employee, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
		
			//Meter capture is in progress
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, true, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
		
			//EGM door is open
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, true, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
		
			//EGM has credits in the game
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, true, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
		
			//Credit zero time span check is not met
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
		
			//Credit zero time span check is met
			restartManager.m_creditZeroTimeSpan = (1);
			CUtilities::Sleep(2000);
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
		
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(player, employee, currentUtcTime, false, false, true, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
			restartManager.SetToPerformControlledRebootAsap();
		
			//Active player
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(player, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
		
			//Active employee
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, employee, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
		
			//Meter capture is in progress
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, true, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
		
			//EGM door is open
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, true, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
		
			//EGM has credits in the game
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, true, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
		
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
		
			//Shutdown already in process
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Remote reboot pending tilt
			restartManager.m_shutdownInProcess = false;
			restartManager.SetFirstTimeAfterRemoteRebootRequest(true);
			restartManager.PerformControlledRebootCheck(NULL, employee, currentUtcTime, false, false, false, true, isOsUpgrade);
			ASSERT_FALSE(restartManager.IsSendRemoteRebootPendingTilt());
			
			restartManager.m_shutdownInProcess = false;
			restartManager.SetFirstTimeAfterRemoteRebootRequest(false);
			restartManager.PerformControlledRebootCheck(NULL, employee, currentUtcTime, false, false, false, true, isOsUpgrade);
			ASSERT_FALSE(restartManager.IsSendRemoteRebootPendingTilt());

			//Cleanup
			delete player;
			delete employee;
		}

		TEST(RestartManagerUnitTests, PerformControlledRebootAsapCheckForOsUpgradeOverrideFalseUnitTest)
		{
			//Setup
			CRestartManager restartManager;
			bool isOsUpgrade = false;

			CPlayer* player(new CPlayer());
			CConfig config(false, "");
			CEmployee *employee = new CEmployee(
				new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() + (10 * SECONDS_PER_MINUTE)), 
				config, false, false, 0, 0);
			time_t currentUtcTime(CUtilities::GetUTCTime());

			//Tests
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(player, employee, currentUtcTime, false, false, true, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());
			restartManager.SetToPerformControlledRebootForOsUpgrade();

			//Active player
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(player, NULL, currentUtcTime, false, false, false, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());

			//Active employee
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, employee, currentUtcTime, false, false, false, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());

			//Meter capture is in progress
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, true, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());

			//EGM door is open
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, true, false, false, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());

			//EGM has credits in the game
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, true, false, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());

			//Credit zero time span check is not met
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, false, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());

			//Credit zero time span check is met
			restartManager.m_creditZeroTimeSpan = (1);
			CUtilities::Sleep(2000);
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, false, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Shutdown already in process
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, false, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Cleanup
			delete player;
			delete employee;
		}

		TEST(RestartManagerUnitTests, PerformControlledRebootAsapForOsUpgradeCheckOverrideTrueUnitTest)
		{
			//Setup
			CRestartManager restartManager;
			bool isOsUpgrade = false;
			CPlayer* player(new CPlayer());
			CConfig config(false, "");
			CEmployee *employee = new CEmployee(
				new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() + (10 * SECONDS_PER_MINUTE)),
				config, false, false, 0, 0);
			time_t currentUtcTime(CUtilities::GetUTCTime());

			//Tests
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(player, employee, currentUtcTime, false, false, true, true, isOsUpgrade));
			ASSERT_FALSE(restartManager.IsShutdownInProcess());
			restartManager.SetToPerformControlledRebootForOsUpgrade();

			//Active player
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(player, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Active employee
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, employee, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Meter capture is in progress
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, true, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//EGM door is open
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, true, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//EGM has credits in the game
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, true, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Credit zero time span check is not met
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Credit zero time span check is met
			restartManager.m_creditZeroTimeSpan = (1);
			CUtilities::Sleep(2000);
			restartManager.m_shutdownInProcess = false;
			ASSERT_TRUE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());
			
			//Shutdown already in process
			ASSERT_FALSE(restartManager.PerformControlledRebootCheck(NULL, NULL, currentUtcTime, false, false, false, true, isOsUpgrade));
			ASSERT_TRUE(restartManager.IsShutdownInProcess());

			//Cleanup
			delete player;
			delete employee;
		}

		TEST(RestartManagerUnitTests, PerformControlledUIRestartCheckOverrideFalseUnitTest)
		{
			//Setup
			CRestartManager restartManager;
			bool restartOnly(false);
			CPlayer* player(new CPlayer());
			CConfig config(false, "");
			CEmployee *employee = new CEmployee(
				new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() + (10 * SECONDS_PER_MINUTE)),
				config, false, false, 0, 0);
			time_t currentUtcTime(CUtilities::GetUTCTime());

			//Tests
			ASSERT_FALSE(restartManager.PerformControlledUIRestartCheck(player, employee, currentUtcTime, false, false, false));
			restartManager.SetPerformUiRestartAsapFlag(true);

			//Active player
			ASSERT_FALSE(restartManager.PerformControlledUIRestartCheck(player, NULL, currentUtcTime, false, false, false));

			//Active employee
			ASSERT_FALSE(restartManager.PerformControlledUIRestartCheck(NULL, employee, currentUtcTime, false, false, false));

			//Meter capture is in progress
			ASSERT_FALSE(restartManager.PerformControlledUIRestartCheck(NULL, NULL, currentUtcTime, false, true, false));

			//EGM has credits in the game
			ASSERT_FALSE(restartManager.PerformControlledUIRestartCheck(NULL, NULL, currentUtcTime, true, false, false));

			//Credit zero time span check is not met
			ASSERT_FALSE(restartManager.PerformControlledUIRestartCheck(NULL, NULL, currentUtcTime, false, false, false));

			//Credit zero time span check is met
			restartManager.m_creditZeroTimeSpan = (1);
			CUtilities::Sleep(2000);
			
			restartManager.m_heartbeatReceivedFromUI = true;
			restartManager.m_processingUiHeartbeats = true;
			restartManager.m_uiHeartbeatReceived -= UINOHEARTBEATRESTART;
			restartManager.m_lastUiRestartTime -= UINOHEARTBEATRESTART;

			restartManager.SetUiHeartbeatReceived();
			restartManager.SetProcessingUiHeartbeats(true);
			ASSERT_TRUE(restartManager.m_heartbeatReceivedFromUI);
			ASSERT_TRUE(restartManager.PerformControlledUIRestartCheck(NULL, NULL, currentUtcTime, false, false, false));
			ASSERT_FALSE(restartManager.m_restartUiAsap);
			ASSERT_FALSE(restartManager.m_heartbeatReceivedFromUI);

			//Cleanup
			delete player;
			delete employee;
		}

		TEST(RestartManagerUnitTests, PerformControlledUIRestartCheckOverrideTrueUnitTest)
		{
			//Setup
			CRestartManager restartManager;
			bool restartOnly(false);
			CPlayer* player(new CPlayer());
			CConfig config(false, "");
			CEmployee *employee = new CEmployee(
				new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime() + (10 * SECONDS_PER_MINUTE)),
				config, false, false, 0, 0);
			time_t currentUtcTime(CUtilities::GetUTCTime());

			//Tests
			ASSERT_FALSE(restartManager.PerformControlledUIRestartCheck(player, employee, currentUtcTime, false, false, true));
			
			//Active player
			restartManager.SetPerformUiRestartAsapFlag(true);
			ASSERT_TRUE(restartManager.PerformControlledUIRestartCheck(player, NULL, currentUtcTime, false, false, true));

			//Active employee
			restartManager.SetPerformUiRestartAsapFlag(true);
			ASSERT_TRUE(restartManager.PerformControlledUIRestartCheck(NULL, employee, currentUtcTime, false, false, true));

			//Meter capture is in progress
			restartManager.SetPerformUiRestartAsapFlag(true);
			ASSERT_TRUE(restartManager.PerformControlledUIRestartCheck(NULL, NULL, currentUtcTime, false, true, true));

			//EGM has credits in the game
			restartManager.SetPerformUiRestartAsapFlag(true);
			ASSERT_TRUE(restartManager.PerformControlledUIRestartCheck(NULL, NULL, currentUtcTime, true, false, true));

			//Credit zero time span check is not met
			restartManager.SetPerformUiRestartAsapFlag(true);
			ASSERT_TRUE(restartManager.PerformControlledUIRestartCheck(NULL, NULL, currentUtcTime, false, false, true));

			//Credit zero time span check is met
			restartManager.SetUiHeartbeatReceived();
			restartManager.SetProcessingUiHeartbeats(true);
			ASSERT_TRUE(restartManager.m_heartbeatReceivedFromUI);
			restartManager.m_creditZeroTimeSpan = (1);
			CUtilities::Sleep(2000);
			restartManager.m_heartbeatReceivedFromUI = true;
			restartManager.m_processingUiHeartbeats = true;
			restartManager.m_uiHeartbeatReceived -= UINOHEARTBEATRESTART;
			restartManager.m_lastUiRestartTime -= UINOHEARTBEATRESTART;
			restartManager.SetPerformUiRestartAsapFlag(true);
			ASSERT_TRUE(restartManager.PerformControlledUIRestartCheck(NULL, NULL, currentUtcTime, false, false, true));

			//Cleanup
			delete player;
			delete employee;
		}
		*/