#include "stdafx.h"
#include "TiltManager.h"
#include "UI/TiltLog.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{

	class TiltManagerUnitTests : public ::testing::Test
	{

	public:
	};

		TEST(TiltManagerUnitTests, TiltManagerConstruction)
		{
			CTiltManager *tiltManager = new CTiltManager(MEMORY_NONE, 1);

			CLogs *tiltLog = tiltManager->GetTiltLogs();

			ASSERT_TRUE(tiltLog != NULL) << L"tilt log should exists int he tilt manager";
			ASSERT_TRUE(tiltLog->GetNumItems() == 0) << L"tilt log should be empty after construction";

			delete tiltManager;
		}

		TEST(TiltManagerUnitTests, TiltManagerSend)
		{
			CTiltManager *tiltManager = new CTiltManager(MEMORY_NONE, 1);

			InternalMsgType additionalMsg = INT_MSG_NONE;
			bool sendTilt = tiltManager->ProcessTilt(tltBillJam, additionalMsg);
			ASSERT_TRUE(sendTilt) << L"tltBillJam should be sent";
			ASSERT_TRUE(additionalMsg == INT_MSG_NONE) << L"tltBillJam should not have an additional message";

			additionalMsg = INT_MSG_NONE;
			sendTilt = tiltManager->ProcessTilt(tltGameRamRecover, additionalMsg);
			ASSERT_TRUE(sendTilt) << L"tltGameRamRecover should be sent";
			ASSERT_TRUE(additionalMsg == INT_MSG_NONE) << L"tltGameRamRecover should not have an additional message";

			additionalMsg = INT_MSG_NONE;
			sendTilt = tiltManager->ProcessTilt(tltRebootButton, additionalMsg);
			ASSERT_TRUE(sendTilt) << L"tltRebootButton should be sent";
			ASSERT_TRUE(additionalMsg == INT_MSG_NONE) << L"tltRebootButton should not have an additional message";

			additionalMsg = INT_MSG_NONE;
			sendTilt = tiltManager->ProcessTilt(tltCashoutBtnPressed, additionalMsg);
			ASSERT_TRUE(sendTilt) << L"tltCashoutBtnPressed should be sent";
			ASSERT_TRUE(additionalMsg == INT_MSG_NONE) << L"tltCashoutBtnPressed should not have an additional message";

			delete tiltManager;
		}

		TEST(TiltManagerUnitTests, TiltManagerTiltLogs)
		{
			CTiltManager *tiltManager = new CTiltManager(MEMORY_NONE, 1);
			CLogs *tiltLog = tiltManager->GetTiltLogs();

			tiltLog->SetMaxLogs(10);	// Have to set a max if not using NVRAM

			ASSERT_TRUE(tiltLog->GetLogType() == LOG_TILT) << L"log type should be tilt";

			InternalMsgType additionalMsg = INT_MSG_NONE;
			tiltManager->ProcessTilt(tltBillFull, additionalMsg);
			ASSERT_TRUE(tiltLog->GetNumItems() == 1) << L"tilt log should have one item";

			tiltManager->ProcessTilt(tltRebootAsap, additionalMsg);
			ASSERT_TRUE(tiltLog->GetNumItems() == 2) << L"tilt log should have 2 items";

			tiltManager->ProcessTilt(tltCashoutBtnPressed, additionalMsg);
			ASSERT_TRUE(tiltLog->GetNumItems() == 3) << L"tilt log should have 3 items";

			// Iterate Tilt log, logs are iterated with latest one first

			CTiltLog *log = (CTiltLog *)tiltLog->GetFirst();
			ASSERT_TRUE(log->GetTiltCode() == tltCashoutBtnPressed) << L"First tilt log should be tltCashoutBtnPressed";

			log = (CTiltLog *)tiltLog->GetNext();
			ASSERT_TRUE(log->GetTiltCode() == tltRebootAsap) << L"Second tilt log should be tltRebootAsap";

			log = (CTiltLog *)tiltLog->GetNext();
			ASSERT_TRUE(log->GetTiltCode() == tltBillFull) << L"Third tilt log should be tltBillFull";

			delete tiltManager;
		}

		TEST(TiltManagerUnitTests, TiltManagerDuplicateTest)
		{
			CTiltManager *tiltManager = new CTiltManager(MEMORY_NONE, 10);

			int sendCount = 0;

			InternalMsgType additionalMsg = INT_MSG_NONE;

			for (int si = 0; si < 10; si++)
			{
				if (tiltManager->ProcessTilt(tltBillJam, additionalMsg))
					sendCount++;
			}

			ASSERT_TRUE(sendCount == 3) << L"Sending 10 duplicate tilts in a row should only send first 3";

			delete tiltManager;
		}


		TEST(TiltManagerUnitTests, TiltManagerTimeoutTest)
		{
			CTiltManager *tiltManager = new CTiltManager(MEMORY_NONE, 1);

			int sendCount = 0;

			InternalMsgType additionalMsg = INT_MSG_NONE;

			for (int si = 0; si < 4; si++)
			{
				if (tiltManager->ProcessTilt(tltBillJam, additionalMsg))
					sendCount++;

				CUtilities::Sleep(4000);
			}

			ASSERT_TRUE(sendCount == 4) << L"All paused duplicate tilts should have been sent";

			delete tiltManager;
		}

		TEST(TiltManagerUnitTests, TiltManagerAdditionalTransactions1)
		{
			CTiltManager *tiltManager = new CTiltManager(MEMORY_NONE, 1);

			InternalMsgType additionalMsg = INT_MSG_NONE;
			bool sendTilt = tiltManager->ProcessTilt(tltGamePowerOn, additionalMsg);
			ASSERT_TRUE(sendTilt) << L"tltGamePowerOn should be sent";
			ASSERT_TRUE(additionalMsg == INT_MSG_TRANSACTION_SLOT_POWER_ON) << L"tltGamePowerOn should have INT_MSG_TRANSACTION_SLOT_POWER_ON message";

			additionalMsg = INT_MSG_NONE;
			sendTilt = tiltManager->ProcessTilt(tltGamePowerOff, additionalMsg);
			ASSERT_TRUE(sendTilt) << L"tltGamePowerOff should be sent";
			ASSERT_TRUE(additionalMsg == INT_MSG_TRANSACTION_SLOT_POWER_OFF) << L"tltGamePowerOff should not have INT_MSG_TRANSACTION_SLOT_POWER_OFF message";

			delete tiltManager;
		}

		TEST(TiltManagerUnitTests, TiltManagerAdditionalTransactions2)
		{
			CTiltManager *tiltManager = new CTiltManager(MEMORY_NONE, 1);

			InternalMsgType additionalMsg = INT_MSG_NONE;
			bool sendTilt = tiltManager->ProcessTilt(tltBillStackerOut, additionalMsg);
			ASSERT_FALSE(sendTilt) << L"tltBillStackerOut should not be sent";
			ASSERT_TRUE(additionalMsg == INT_MSG_SEND_STACKER_OUT) << L"tltBillStackerOut should have INT_MSG_SEND_STACKER_OUT message";

			additionalMsg = INT_MSG_NONE;
			sendTilt = tiltManager->ProcessTilt(tltBillStackerIn, additionalMsg);
			ASSERT_FALSE(sendTilt) << L"tltBillStackerIn should not be sent";
			ASSERT_TRUE(additionalMsg == INT_MSG_SEND_STACKER_IN) << L"tltBillStackerIn should not have INT_MSG_SEND_STACKER_IN message";

			delete tiltManager;
		}
}
