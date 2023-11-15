#include "stdafx.h"

// Project includes.
#include "TestModuleInitialize.h"

// Unit testing includes.
#include "gtest/gtest.h"

int main (int argc, char *argv[])
{
	//::testing::GTEST_FLAG(filter) = "BonusCounterItemUnitTests.*:BonusCounterStateUnitTests.*:InternalMessageQueueListUnitTests.*";
	//::testing::GTEST_FLAG(filter) = "ProgressiveMessageFactoryUnitTests.*";
	//::testing::GTEST_FLAG(filter) = "PayoutTests.*";
	//::testing::GTEST_FLAG(filter) = "MobileConnectMessageFactoryUnitTests.*";

	// All tests filtered after this line are skipped.
	::testing::GTEST_FLAG(filter) += "-";

#define SKIP_SLOW_TESTS
#ifdef SKIP_SLOW_TESTS
	::testing::GTEST_FLAG(filter) += ":CardInfoStateUnitTests.CardInfoStateFunctionsTest";

	::testing::GTEST_FLAG(filter) += ":PlayerUpdateManager.PlayerUpdateTimeTest";
	::testing::GTEST_FLAG(filter) += ":PlayerUpdateManager.PlayerUpdateIntervalTest";

	::testing::GTEST_FLAG(filter) += ":RestartManagerUnitTests.PerformControlledRebootAsapCheckRebootOverrideFalseUnitTest";
	::testing::GTEST_FLAG(filter) += ":RestartManagerUnitTests.PerformControlledRebootAsapRebootCheckOverrideTrueUnitTest";
	::testing::GTEST_FLAG(filter) += ":RestartManagerUnitTests.PerformControlledRebootAsapCheckForOsUpgradeOverrideFalseUnitTest";
	::testing::GTEST_FLAG(filter) += ":RestartManagerUnitTests.PerformControlledRebootAsapForOsUpgradeCheckOverrideTrueUnitTest";
	::testing::GTEST_FLAG(filter) += ":RestartManagerUnitTests.PerformControlledSwfRestartCheckOverrideFalseUnitTest";
	::testing::GTEST_FLAG(filter) += ":RestartManagerUnitTests.PerformControlledSwfRestartCheckOverrideTrueUnitTest";
	::testing::GTEST_FLAG(filter) += ":RestartManagerUnitTests.PerformControlledRemoteRebootCheckOverrideTrueUnitTest";
	::testing::GTEST_FLAG(filter) += ":RestartManagerUnitTests.PerformControlledRemoteRebootCheckOverrideFalseUnitTest";

	::testing::GTEST_FLAG(filter) += ":SASInterfaceUnitTests.SASInterfacePollingSpeedFailedPolls";

	::testing::GTEST_FLAG(filter) += ":TiltManagerUnitTests.TiltManagerTimeoutTest";

	::testing::GTEST_FLAG(filter) += ":WatchdogThreadInfoUnitTests.WatchdogThreadInfoTimeoutTest";
	::testing::GTEST_FLAG(filter) += ":WatchdogThreadInfoUnitTests.WatchdogThreadInfoCheckinTest";
	::testing::GTEST_FLAG(filter) += ":WatchdogThreadInfoUnitTests.WatchdogThreadInfoSuspendResumeTest";

	::testing::GTEST_FLAG(filter) += ":WatchDogUnitTests.ShutdownTimedOut_True_DefaultOSTest";
	::testing::GTEST_FLAG(filter) += ":WatchDogUnitTests.WatchDogSafeSleepTest";

	// unstable UTs, to be investigated.
	::testing::GTEST_FLAG(filter) += ":SASInterfaceUnitTests.SASInterfaceSamePollBackToBack";
	::testing::GTEST_FLAG(filter) += ":InternalsProcessingForUiUnitTests.ProcessUJPCheckPinFromUI_ValidInputs";
	::testing::GTEST_FLAG(filter) += ":InternalsProcessingForUiUnitTests.ProcessUJPCheckPinFromUI_InvalidJackpotState";
	::testing::GTEST_FLAG(filter) += ":InternalsProcessingForUiUnitTests.ProcessUJPCheckPinFromUI_InvalidPinLength";
	::testing::GTEST_FLAG(filter) += ":InternalsProcessingForGCFUnitTests.ProcessPBBalancesFromGCF_PBInfoWithError";
	::testing::GTEST_FLAG(filter) += ":InternalsProcessingForGCFUnitTests.SendMCFundsTransferBalanceToPollerProtocol_CEPOutEnabled";
	::testing::GTEST_FLAG(filter) += ":InternalsProcessingForGCFUnitTests.SendMCFundsTransferBalanceToPollerProtocol_CEPOutDisabled";
#endif // SKIP_SLOW_TESTS

#if 1//TODO-PORT - Watchdog tests for checkin time after logging callstacks.
#define SKIP_WATCHDOG_RESUME_TESTS
#ifdef SKIP_WATCHDOG_RESUME_TESTS
	// Temporarily skip watchdog tests that rely on all threads having their checkin time updated via CWatchDog::ResumeAll() after logging callstacks.
	::testing::GTEST_FLAG(filter) += ":WatchDogUnitTests.CheckForTimeout_2Test";
	::testing::GTEST_FLAG(filter) += ":WatchDogUnitTests.CheckForTimeout_12Test";
	::testing::GTEST_FLAG(filter) += ":WatchDogUnitTests.CheckForTimeout_Checkin1Test";
	::testing::GTEST_FLAG(filter) += ":WatchDogUnitTests.CheckForTimeout_Checkin12Test";
	::testing::GTEST_FLAG(filter) += ":WatchDogUnitTests.CheckForTimeout_Checkin2Test";
#endif // SKIP_WATCHDOG_RESUME_TESTS.
#endif//TODO-PORT - Watchdog tests for checkin time after logging callstacks.

	::testing::InitGoogleTest(&argc, argv);
	::testing::AddGlobalTestEnvironment(new SentinelNativeUnitTests::CTestModuleEnvironment());
    return RUN_ALL_TESTS();
}

