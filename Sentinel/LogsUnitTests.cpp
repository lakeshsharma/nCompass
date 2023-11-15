#include "stdafx.h"

#include "GlobalDefs.h"
#include "UI/Log.h"
#include "UI/Logs.h"
#include "UI/TransactionLog.h"
#include "UI/RepairLog.h"
#include "UI/TiltLog.h"
#include "UI/AttendantPaidPayoutLog.h"
#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{
	class LogsUnitTests : public ::testing::Test
	{

	protected:
	public:
		virtual void SetUp()
		{
		}
	};

	TEST_F(LogsUnitTests, TransactionLogSubscriptionDataTest)
	{
		CLogs& logs = *new CLogs(MEMORY_NVRAM, LOG_TRANS, "Transactions");

		CTransactionLog* log = new CTransactionLog(TransactionType::AUTHORIZE_PIN, 0, 0);
		
		logs.Add(log);

		string expectedResp = "{\"TransactionLog\":[{\"TimeStamp\":\"(1) 01/01/1970 12:00AM\",\"Amount\":\"N/A\",\"Type\":\"125\"}]}";
		string resp = logs.GetSubscriptionDataResponse(TRANSACTION_LOG, "");

		ASSERT_TRUE(resp == expectedResp) << "GetSubscriptionDataResponse()";

		CLogs logs2 = logs; // test copy constructor

		string resp2 = logs2.GetSubscriptionDataResponse(TRANSACTION_LOG, "");

		ASSERT_TRUE(resp2 == expectedResp) << "GetSubscriptionDataResponse()";
	}

	TEST_F(LogsUnitTests, FailureLogSubscriptionDataTest)
	{
		CLogs& logs = *new CLogs(MEMORY_NVRAM, LOG_FAILURE, REPAIR_LOG);

		logs.Add(new CRepairLog(123));

		string expectedResp = "{\"RepairLog\":[{\"TimeStamp\":\"(1) 07/19/2023 11:16AM\",\"Code\":\"123\"}]}";
		string resp = logs.GetSubscriptionDataResponse(REPAIR_LOG, "");

		ASSERT_TRUE(resp.length() == expectedResp.length()) << "GetSubscriptionDataResponse()";

		CLogs logs2 = logs; // test copy constructor

		string resp2 = logs2.GetSubscriptionDataResponse(REPAIR_LOG, "");

		ASSERT_TRUE(resp2.length() == expectedResp.length()) << "GetSubscriptionDataResponse()";
	}

	TEST_F(LogsUnitTests, TiltLogSubscriptionDataTest)
	{
		CLogs& logs = *new CLogs(MEMORY_NVRAM, LOG_TILT, TILT_LOGS);

		CTiltLog* tiltLog = new CTiltLog(123, CUtilities::GetCurrentTime());
		logs.Add(tiltLog);

		string expectedResp = "";
		string resp = logs.GetSubscriptionDataResponse(TILT_LOGS, "");

		ASSERT_TRUE(resp == expectedResp) << "GetSubscriptionDataResponse()";

		CLogs logs2 = logs; // test copy constructor

		string resp2 = logs2.GetSubscriptionDataResponse(TILT_LOGS, "");

		ASSERT_TRUE(resp2 == expectedResp) << "GetSubscriptionDataResponse()";
	}

	TEST_F(LogsUnitTests, AttPaidPayoutLogSubscriptionDataTest)
	{
		CLogs& logs = *new CLogs(MEMORY_NVRAM, LOG_ATT_PAID_PAYOUT, ATTENDANT_PAID_LOG);

		CAttendantPaidPayoutLog* appLog = new CAttendantPaidPayoutLog(123, JTRANS_COMPOSITE);
		logs.Add(appLog);

		string expectedResp = "{\"TransactionLog\":[{\"TimeStamp\":\"(1) 07/19/2023 01:10PM\",\"Amount\":\"$1.23\",\"Type\":\"Handpay\"}]}";
		string resp = logs.GetSubscriptionDataResponse(ATTENDANT_PAID_LOG, "");
		ASSERT_TRUE(resp.length() == expectedResp.length()) << "GetSubscriptionDataResponse()";

		CLogs logs2 = logs; // test copy constructor
		string resp2 = logs2.GetSubscriptionDataResponse(ATTENDANT_PAID_LOG, "");
		ASSERT_TRUE(resp2.length() == expectedResp.length()) << "GetSubscriptionDataResponse()";
	}
	
}
