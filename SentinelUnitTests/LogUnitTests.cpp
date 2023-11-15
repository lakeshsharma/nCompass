#include "stdafx.h"

#include "GlobalDefs.h"
#include "UI/AttendantPaidPayoutLog.h"
#include "UI/MediaWindowLog.h"
#include "UI/RepairLog.h"
#include "UI/TiltLog.h"
#include "UI/TransactionLog.h"
#include "UI/PersonalBankerLog.h"
#include "UI/TicketLog.h"


#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{
	class LogUnitTests : public ::testing::Test
	{

		protected:
			__int64 m_amount;
			JackpotTransactionType m_type;
			time_t m_time;
			WORD m_code;
			TransactionType m_msgType;
		public:
			virtual void SetUp()
			{
				m_amount = 969;
				m_type = JTRANS_PROGRESSIVE_HOPPER_PAID;
				m_code = 123;
				m_time = 123427200;
				m_msgType = CONTROLLED_REBOOT_CODE;
			}
	};
	TEST_F(LogUnitTests, AttendantPaidPayoutLogConstructorTest)
	{
		//Setup
		CAttendantPaidPayoutLog* log(new CAttendantPaidPayoutLog(m_amount, m_type));

		//Tests
		ASSERT_TRUE(m_amount == log->GetAmount()) 
			<< "CAttendantPaidPayoutLog - m_amount was not properly initialized in the constructor";
		ASSERT_TRUE(m_type == log->GetJackpotType())
			<< "CAttendantPaidPayoutLog - m_type was not properly initialized in the constructor";

		//Cleanup
		delete log;
	}

	TEST_F(LogUnitTests, AttendantPaidPayoutLogCopyConstructorTest)
	{
		//Setup
		CAttendantPaidPayoutLog* log1(new CAttendantPaidPayoutLog(m_amount, m_type));
		CAttendantPaidPayoutLog* log2(new CAttendantPaidPayoutLog(*log1));

		//Tests
		ASSERT_TRUE(log1->GetAmount() == log2->GetAmount()) 
			<< "AttendantPaidPayoutLogCopyConstructorTest - m_amount was not properly copied in the constructor";
		ASSERT_TRUE(log1->GetJackpotType() == log2->GetJackpotType()) 
			<< "AttendantPaidPayoutLogCopyConstructorTest - m_type was not properly copied in the constructor";

		//Cleanup
		delete log1;
		delete log2;
	}

	TEST_F(LogUnitTests, AttendantPaidPayoutLogConstructorFromBufferTest)
	{
		//Setup
		CAttendantPaidPayoutLog* log(new CAttendantPaidPayoutLog(m_amount, m_type));
		byte* buffer(new byte[NVRAM_ATT_PAID_LOG_DATA_SIZE]);
		log->GetNvramBuffer(buffer, NVRAM_ATT_PAID_LOG_DATA_SIZE);
		CAttendantPaidPayoutLog* logFromBuffer(new CAttendantPaidPayoutLog(buffer));

		//Tests
		ASSERT_TRUE(log->GetAmount() == logFromBuffer->GetAmount()) 
			<< "CAttendantPaidPayoutLog - m_amount was not properly initialized in the constructor from NVRAM";
		ASSERT_TRUE(log->GetJackpotType() == logFromBuffer->GetJackpotType()) 
			<< "CAttendantPaidPayoutLog - m_type was not properly initialized in the constructor from NVRAM";

		//Cleanup
		delete log;
		delete logFromBuffer;
		delete [] buffer;
	}

	TEST_F(LogUnitTests, AttendantPaidPayoutLogConstructorFromNvRamTest)
	{
		byte NVRAM_DATA[NVRAM_ATT_PAID_LOG_DATA_SIZE] = 
		{ 
			0xdd,0x07,0x00,0x00,0x06,0x00,0x00,0x00,0x14,0x00,
			0x00,0x00,0x09,0x00,0x00,0x00,0x2b,0x00,0x00,0x00,
			0x19,0x00,0x00,0x00,0xc9,0x03,0x00,0x00,0x00,0x00,
			0x00,0x00,0x01,0x00,0x00,0x00,0xcd,0xcd,0xcd,0xcd
		};

		//Setup
		CAttendantPaidPayoutLog* logFromNvram(new CAttendantPaidPayoutLog(NVRAM_DATA));

		//Tests
		ASSERT_TRUE(m_amount == logFromNvram->GetAmount()) 
			<< "CAttendantPaidPayoutLog - m_amount was not properly initialized in the constructor from NVRAM";
		ASSERT_TRUE(m_type == logFromNvram->GetJackpotType()) 
			<< "CAttendantPaidPayoutLog - m_type was not properly initialized in the constructor from NVRAM";

		//Cleanup
		delete logFromNvram;
	}

	TEST_F(LogUnitTests, AttendantPaidPayoutLogFunctionsTest)
	{
		//Setup
		__int64 amount(1);
		JackpotTransactionType type(JTRANS_NONE);
		CAttendantPaidPayoutLog* log(new CAttendantPaidPayoutLog(amount, type));

		//Tests
		ASSERT_TRUE(amount == log->GetAmount()) 
			<< "CAttendantPaidPayoutLog - m_amount was not properly initialized in the constructor" ;
		ASSERT_TRUE(type == log->GetJackpotType()) 
			<< "CAttendantPaidPayoutLog - m_type was not properly initialized in the constructor" ;

		log->SetAmount(m_amount);
		ASSERT_TRUE(m_amount == log->GetAmount()) 
			<< "CAttendantPaidPayoutLog - SetAmount did not work";

		log->SetJackpotType(m_type);
		ASSERT_TRUE(m_type == log->GetJackpotType()) 
			<< "CAttendantPaidPayoutLog - SetJackpotType did not work";

		//Cleanup
		delete log;
	}

	TEST_F(LogUnitTests, MediaWindowLogConstructorTest)
	{
		//Setup
		byte cardIDExpected[] = {0x01, 0x23, 0x45, 0x67, 0x89};
		CMediaWindowLog* log(new CMediaWindowLog(m_code, cardIDExpected));
		int memcmpResult = memcmp(log->GetCardID(), cardIDExpected, CARD_ID_LEN);
		bool areEqual = 0 == memcmpResult;

		//Tests
		ASSERT_TRUE(m_code == log->GetMediaWindowCode()) 
			<< "CMediaWindowLog - m_code was not properly initialized in the constructor";
		ASSERT_TRUE(areEqual) 
			<< "CMediaWindowLog - m_cardID was not properly initialized in the constructor";

		//Cleanup
		delete log;
	}

	TEST_F(LogUnitTests, MediaWindowLogConstructorFromBufferTest)
	{
		//Setup
		byte cardIDExpected[] = {0x01, 0x23, 0x45, 0x67, 0x89};
		CMediaWindowLog* log(new CMediaWindowLog(m_code, cardIDExpected));
		byte* buffer(new byte[NVRAM_MEDIA_WINDOW_CODES_LOG_DATA_SIZE]);
		log->GetNvramBuffer(buffer, NVRAM_MEDIA_WINDOW_CODES_LOG_DATA_SIZE);
		CMediaWindowLog* logFromBuffer(new CMediaWindowLog(buffer));

		int memcmpResult = memcmp(log->GetCardID(), logFromBuffer->GetCardID(), CARD_ID_LEN);
		bool areEqual = 0 == memcmpResult;

		//Tests
		ASSERT_TRUE(m_code == log->GetMediaWindowCode()) 
			<< "CMediaWindowLog - m_code was not properly initialized in the constructor";
		ASSERT_TRUE(areEqual) 
			<< "CMediaWindowLog - m_cardID was not properly initialized in the constructor";

		//Cleanup
		delete log;
		delete logFromBuffer;
	}

	TEST_F(LogUnitTests, MediaWindowLogConstructorFromNvRamTest)
	{
		//Setup
		byte NVRAM_DATA[NVRAM_ATT_PAID_LOG_DATA_SIZE] = 
		{
			0xdd,0x07,0x00,0x00,0x06,0x00,0x00,0x00,0x14,
			0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x3a,0x00,
			0x00,0x00,0x3b,0x00,0x00,0x00,0x01,0x23,0x45,
			0x67,0x89,0x7b,0x00,0x00,0x00,0xcd,0xcd,0xcd
		};
		byte cardIDExpected[] = {0x01, 0x23, 0x45, 0x67, 0x89};
		CMediaWindowLog* logFromNvram(new CMediaWindowLog(NVRAM_DATA));

		int memcmpResult = memcmp(logFromNvram->GetCardID(), cardIDExpected, CARD_ID_LEN);
		bool areEqual = 0 == memcmpResult;

		//Tests
		ASSERT_TRUE(m_code == logFromNvram->GetMediaWindowCode()) 
			<< "CMediaWindowLog - m_code was not properly initialized in the constructor";
		ASSERT_TRUE(areEqual) 
			<< "CMediaWindowLog - m_cardID was not properly initialized in the constructor";

		//Cleanup
		delete logFromNvram;
	}

	TEST_F(LogUnitTests, RepairLogConstructorTest)
	{
		//Setup
		CRepairLog* log(new CRepairLog(m_code));

		//Tests
		ASSERT_TRUE(m_code == log->GetRepairCode()) 
			<< "CRepairLog - m_code was not properly initialized in the constructor";

		//Cleanup
		delete log;
	}

	TEST_F(LogUnitTests, RepairLogConstructorFromBufferTest)
	{
		//Setup
		CRepairLog* log(new CRepairLog(m_code));
		byte* buffer(new byte[NVRAM_REPAIR_CODES_LOG_DATA_SIZE]);
		log->GetNvramBuffer(buffer, NVRAM_REPAIR_CODES_LOG_DATA_SIZE);
		CRepairLog* logFromBuffer(new CRepairLog(buffer));

		//Tests
		ASSERT_TRUE(logFromBuffer->GetRepairCode() == log->GetRepairCode()) 
			<< "CRepairLog - m_code was not properly initialized in the constructor";

		//Cleanup
		delete log;
		delete logFromBuffer;
	}

	TEST_F(LogUnitTests, RepairLogConstructorFromNvramTest)
	{
		//Setup
		byte NVRAM_DATA[NVRAM_REPAIR_CODES_LOG_DATA_SIZE] = 
		{
			0xdd,0x07,0x00,0x00,0x06,0x00,0x00,0x00,0x14,0x00,
			0x00,0x00,0x0a,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
			0x26,0x00,0x00,0x00,0x7b,0x00,0x00,0x00,0xcd,0xcd 
		};

		CRepairLog* logFromNvram(new CRepairLog(NVRAM_DATA));

		//Tests
		ASSERT_TRUE(m_code == logFromNvram->GetRepairCode()) 
			<< "CRepairLog - m_code was not properly initialized in the constructor";

		//Cleanup
		delete logFromNvram;
	}

	TEST_F(LogUnitTests, TiltLogConstructorTest)
	{
		//Setup
		CTiltLog* log(new CTiltLog(m_code, m_time));

		//Tests
		ASSERT_TRUE(m_code == log->GetTiltCode()) 
			<< "CTiltLog - m_code was not properly initialized in the constructor";

		ASSERT_TRUE(m_time == log->GetDateTime()) 
			<< "CTiltLog - m_time was not properly initialized in the constructor";

		//Cleanup
		delete log;
	}

	TEST_F(LogUnitTests, TiltLogConstructorFromBufferTest)
	{
		//Setup
		CTiltLog* log(new CTiltLog(m_code, m_time));
		byte* buffer(new byte[NVRAM_COMPOSITE_TILTS_LOG_DATA_SIZE]);
		log->GetNvramBuffer(buffer, NVRAM_COMPOSITE_TILTS_LOG_DATA_SIZE);
		CTiltLog* logFromBuffer(new CTiltLog(buffer));

		//Tests
		ASSERT_TRUE(logFromBuffer->GetTiltCode() == log->GetTiltCode()) 
			<< "CTiltLog - m_code was not properly initialized in the constructor";

		ASSERT_TRUE(logFromBuffer->GetDateTime() == log->GetDateTime()) 
			<< "CTiltLog - m_time was not properly initialized in the constructor";

		//Cleanup
		delete log;
		delete logFromBuffer;
	}

	TEST_F(LogUnitTests, TiltLogConstructorFromNvramTest)
	{
		//Setup
		byte NVRAM_DATA[NVRAM_REPAIR_CODES_LOG_DATA_SIZE] = 
		{
			0xb5,0x07,0x00,0x00,0x0b,0x00,0x00,0x00,0x1d,0x00,
			0x00,0x00,0x0d,0x00,0x00,0x00,0x14,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x7b,0x00,0xcd,0xcd,0xcd,0xcd
		};
		CTiltLog* logFromNvram(new CTiltLog(NVRAM_DATA));

		//Tests
		ASSERT_TRUE(m_code == logFromNvram->GetTiltCode()) 
			<< "CTiltLog - m_code was not properly initialized in the constructor";

		ASSERT_TRUE(m_time == logFromNvram->GetDateTime()) 
			<< "CTiltLog - m_time was not properly initialized in the constructor";

		//Cleanup
		delete logFromNvram;
	}

	TEST_F(LogUnitTests, TransactionLogConstructorTest)
	{
		//Setup
		CTransactionLog* log(new CTransactionLog(m_msgType, m_amount, m_time));

		//Tests
		ASSERT_TRUE(m_msgType == log->GetTransactionType()) 
			<< "CTransactionLog - m_transactionType was not properly initialized in the constructor";

		ASSERT_TRUE(m_amount == log->GetAmount()) 
			<< "CTransactionLog - m_amount was not properly initialized in the constructor";

		ASSERT_TRUE(m_time == log->GetDateTime()) 
			<< "CTransactionLog - m_dateTime was not properly initialized in the constructor";

		//Cleanup
		delete log;
	}

	TEST_F(LogUnitTests, TransactionLogConstructorFromBufferTest)
	{
		//Setup
		CTransactionLog* log(new CTransactionLog(m_msgType, m_amount, m_time));
		byte* buffer(new byte[NVRAM_TRANSACTION_LOG_DATA_SIZE]);
		log->GetNvramBuffer(buffer, NVRAM_TRANSACTION_LOG_DATA_SIZE);
		CTransactionLog* logFromBuffer(new CTransactionLog(buffer));

		//Tests
		ASSERT_TRUE(logFromBuffer->GetTransactionType() == log->GetTransactionType()) 
			<< "CTransactionLog - m_transactionType was not properly initialized in the constructor";

		ASSERT_TRUE(logFromBuffer->GetAmount() == log->GetAmount()) 
			<< "CTransactionLog - m_amount was not properly initialized in the constructor";

		ASSERT_TRUE(logFromBuffer->GetDateTime() == log->GetDateTime()) 
			<< "CTransactionLog - m_dateTime was not properly initialized in the constructor";

		//Cleanup
		delete log;
		delete logFromBuffer;
	}

	TEST_F(LogUnitTests, TransactionLogConstructorFromNvramTest)
	{
		//Setup
		byte NVRAM_DATA[NVRAM_TRANSACTION_LOG_DATA_SIZE] = 
		{
			0xb5,0x07,0x00,0x00,0x0b,0x00,0x00,0x00,0x1d,0x00,
			0x00,0x00,0x0d,0x00,0x00,0x00,0x14,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0xc9,0x03,0x00,0x00,0x00,0x00,
			0x00,0x00,0x2d,0x00,0x00,0x00,0xcd,0xcd,0xcd,0xcd
		};
		CTransactionLog* logFromNvram(new CTransactionLog(NVRAM_DATA));

		//Tests
		ASSERT_TRUE(m_msgType == logFromNvram->GetTransactionType()) 
			<< "CTransactionLog - m_transactionType was not properly initialized in the constructor";

		ASSERT_TRUE(m_amount == logFromNvram->GetAmount()) 
			<< "CTransactionLog - m_amount was not properly initialized in the constructor";

		ASSERT_TRUE(m_time == logFromNvram->GetDateTime()) 
			<< "CTransactionLog - m_dateTime was not properly initialized in the constructor";

		//Cleanup
		delete logFromNvram;
	}

	TEST_F(LogUnitTests, PersonalBankerLogConstructorTest)
	{
		//Setup
		CPersonalBankerTransfer *pbTransfer = new CPersonalBankerTransfer(CUtilities::GetTickCount(), MEMORY_NONE);
		PersonalBankerTransferType transferType(PromoDollarsFromGame);
		time_t cardTime = CUtilities::GetTimeFromFields(2013,03,07,12,12,12);
		CCard *card = new CCard(cardTime,cardTime);
		card->SetCardID("A531000002");
		// copy constructor
		int cardIdLength = 0;
		const int PBTAMOUNT = 1000;
		const byte *cardID = card->GetCardID(cardIdLength);			
		pbTransfer->SetAmount(PBTAMOUNT);
		pbTransfer->SetRestrictedAmount(PBTAMOUNT);
		pbTransfer->SetNonrestrictedAmount(PBTAMOUNT);
		pbTransfer->SetPersonalBankerType(CashDownload);
		pbTransfer->SetPromoPersonalBankerTransferType(CashToGame);
		pbTransfer->SetCardNumber(cardID, cardIdLength);
		__int64 amount(23);
		time_t time = 123456000;
		CPersonalBankerLog* log(new CPersonalBankerLog(pbTransfer, transferType, amount, time));

		//Tests
		ASSERT_TRUE(transferType == log->GetSource()) 
			<< "CPersonalBankerLog - m_source was not properly initialized in the constructor";

		ASSERT_TRUE(amount == log->GetAmount()) 
			<< "CPersonalBankerLog - m_amount was not properly initialized in the constructor";

		ASSERT_TRUE(time == log->GetDateTime()) 
			<< "CPersonalBankerLog - m_dateTime was not properly initialized in the constructor";

		int memcmpResult = memcmp(log->GetCardID(cardIdLength), cardID, cardIdLength);
		bool areEqual = 0 == memcmpResult;
		ASSERT_TRUE(areEqual) 
			<< "CPersonalBankerLog - m_cardID was not properly initialized in the constructor";

		ASSERT_TRUE(!pbTransfer->IsTransferRequestCanceled() == log->IsSuccessful()) 
			<< "CPersonalBankerLog - m_bSuccessful was not properly initialized in the constructor";

		//Cleanup
		delete log;
		delete pbTransfer;
	}

	TEST_F(LogUnitTests, PersonalBankerLogConstructorPartialUploadTest)
	{
		//Setup
		CPersonalBankerTransfer *pbTransfer = new CPersonalBankerTransfer(CUtilities::GetTickCount(), MEMORY_NONE);
		PersonalBankerTransferType transferType(CashFromGame);
		const int PBTAMOUNT = 1000;

		// Simulate partial cash upload log 
		pbTransfer->SetAmount(PBTAMOUNT);
		pbTransfer->SetPersonalBankerType(CashUpload);
		time_t time = 123456000;
		bool isPartial = true;
		CPersonalBankerLog* log(new CPersonalBankerLog(pbTransfer, transferType, PBTAMOUNT, time, isPartial));

		//Tests
		ASSERT_TRUE(transferType == log->GetSource())
			<< "CPersonalBankerLog - m_source was not properly initialized in the constructor";

		ASSERT_TRUE(PBTAMOUNT == log->GetAmount())
			<< "CPersonalBankerLog - m_amount was not properly initialized in the constructor";

		ASSERT_TRUE(isPartial == log->IsPartial())
			<< "CPersonalBankerLog - m_bPartial was not properly initialized in the constructor";

		ASSERT_STREQ(log->GetSourceString().c_str(), "Partial Cash from game")
			<< "CPersonalBankerLog - GetSourceString() returned unexpected string";

		ASSERT_TRUE(!pbTransfer->IsTransferRequestCanceled() == log->IsSuccessful())
			<< "CPersonalBankerLog - m_bSuccessful was not properly initialized in the constructor";

		//Cleanup
		delete log;
		delete pbTransfer;
	}

	TEST_F(LogUnitTests, PersonalBankerLogConstructorCashoutUploadTest)
	{
		//Setup
		CPersonalBankerTransfer *pbTransfer = new CPersonalBankerTransfer(CUtilities::GetTickCount(), MEMORY_NONE);
		PersonalBankerTransferType transferType(PointsFromGame);
		const int PBTAMOUNT = 1000;

		//// Simulate canceled partial cash upload log 
		pbTransfer->SetPersonalBankerType(CashUpload);
		pbTransfer->SetCashoutToHostTransfer(true);

		time_t time = 123456000;
		CPersonalBankerLog* log(new CPersonalBankerLog(pbTransfer, transferType, PBTAMOUNT, time));

		//Tests
		ASSERT_TRUE(transferType == log->GetSource())
			<< "CPersonalBankerLog - m_source was not properly initialized in the constructor";

		ASSERT_TRUE(PBTAMOUNT == log->GetAmount())
			<< "CPersonalBankerLog - m_amount was not properly initialized in the constructor";

		ASSERT_TRUE(time == log->GetDateTime())
			<< "CPersonalBankerLog - m_dateTime was not properly initialized in the constructor";

		ASSERT_TRUE(log->IsCashoutToHost())
			<< "CPersonalBankerLog - m_bCashoutToHost was not properly initialized in the constructor";

		ASSERT_STREQ(log->GetSourceString().c_str(), "Cashout Points from game")
			<< "CPersonalBankerLog - GetSourceString() returned unexpected string";

		ASSERT_TRUE(!pbTransfer->IsTransferRequestCanceled() == log->IsSuccessful())
			<< "CPersonalBankerLog - m_bSuccessful was not properly initialized in the constructor";

		//Cleanup
		delete log;
		delete pbTransfer;
	}

	TEST_F(LogUnitTests, PersonalBankerLogConstructorFromBufferTest)
	{
		//Setup
		CPersonalBankerTransfer *pbTransfer = new CPersonalBankerTransfer(CUtilities::GetTickCount(), MEMORY_NONE);
		PersonalBankerTransferType transferType(PromoDollarsFromGame);
		time_t cardTime = CUtilities::GetTimeFromFields(2013,03,07,12,12,12);
		CCard *card = new CCard(cardTime,cardTime);
		card->SetCardID("A531000002");
		// copy constructor
		int cardIdLength = 0;
		const int PBTAMOUNT = 1000;
		const byte *cardID = card->GetCardID(cardIdLength);			
		pbTransfer->SetAmount(PBTAMOUNT);
		pbTransfer->SetRestrictedAmount(PBTAMOUNT);
		pbTransfer->SetNonrestrictedAmount(PBTAMOUNT);
		pbTransfer->SetPersonalBankerType(CashDownload);
		pbTransfer->SetPromoPersonalBankerTransferType(CashToGame);
		pbTransfer->SetCardNumber(cardID, cardIdLength);
		pbTransfer->SetCashoutToHostTransfer(true);
		CPersonalBankerLog* log(new CPersonalBankerLog(pbTransfer, transferType, m_amount, m_time));
		byte* buffer(new byte[NVRAM_PBT_LOG_LOG_DATA_SIZE]);
		log->GetNvramBuffer(buffer, NVRAM_PBT_LOG_LOG_DATA_SIZE);
		CPersonalBankerLog* logFromBuffer(new CPersonalBankerLog(buffer));

		//Tests
		ASSERT_TRUE(logFromBuffer->GetSource() == log->GetSource()) 
			<< "CPersonalBankerLog - m_source was not properly initialized in the constructor";

		ASSERT_TRUE(logFromBuffer->GetAmount() == log->GetAmount()) 
			<< "CPersonalBankerLog - m_amount was not properly initialized in the constructor";

		ASSERT_TRUE(logFromBuffer->GetDateTime() == log->GetDateTime()) 
			<< "CPersonalBankerLog - m_dateTime was not properly initialized in the constructor";

		int memcmpResult = memcmp(logFromBuffer->GetCardID(cardIdLength), log->GetCardID(cardIdLength), cardIdLength);
		bool areEqual = 0 == memcmpResult;
		ASSERT_TRUE(areEqual) 
			<< "CPersonalBankerLog - m_cardID was not properly initialized in the constructor";

		ASSERT_TRUE(logFromBuffer->IsSuccessful() == log->IsSuccessful()) 
			<< "CPersonalBankerLog - m_bSuccessful was not properly initialized in the constructor";

		ASSERT_TRUE(log->IsCashoutToHost())
			<< "CPersonalBankerLog - m_bCashoutToHost was not properly initialized in the constructor";

		ASSERT_TRUE(logFromBuffer->IsCashoutToHost() == log->IsCashoutToHost())
			<< "CPersonalBankerLog - m_bCashoutToHost was not properly initialized when object is created from buffer";

		ASSERT_STREQ(logFromBuffer->GetSourceString().c_str(), "Cashout Promos from game")
			<< "CPersonalBankerLog - GetSourceString() returned unexpected string";

		//Cleanup
		delete log;
		delete logFromBuffer;
		delete pbTransfer;
	}

	TEST_F(LogUnitTests, PersonalBankerLogConstructorFromNvramTest)
	{
		//Setup
		byte NVRAM_DATA[NVRAM_PBT_LOG_LOG_DATA_SIZE] = 
		{
			0xb5,0x07,0x00,0x00,0x0b,0x00,0x00,0x00,0x1d,0x00,0x00,0x00,0x0d,0x00,0x00,0x00,0x14,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0xc9,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x05,0x00,0x00,0x00,0xa5,0x31,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0xcd,
			0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,
			0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd
		};
		int cardIdLength = 0;
		time_t cardTime = CUtilities::GetTimeFromFields(2013,03,07,12,12,12);
		CCard *card = new CCard(cardTime,cardTime);
		card->SetCardID("A531000002");		
		CPersonalBankerLog* logFromNvram(new CPersonalBankerLog(NVRAM_DATA));

		//Tests
		ASSERT_TRUE(PromoDollarsFromGame == logFromNvram->GetSource()) 
			<< "CPersonalBankerLog - m_source was not properly initialized in the constructor";

		ASSERT_TRUE(m_amount == logFromNvram->GetAmount()) 
			<< "CPersonalBankerLog - m_amount was not properly initialized in the constructor";

		ASSERT_TRUE(m_time == logFromNvram->GetDateTime()) 
			<< "CPersonalBankerLog - m_dateTime was not properly initialized in the constructor";

		int memcmpResult = memcmp(card->GetCardID(cardIdLength), logFromNvram->GetCardID(cardIdLength), cardIdLength);
		bool areEqual = 0 == memcmpResult;
		ASSERT_TRUE(areEqual) 
			<< "CPersonalBankerLog - m_cardID was not properly initialized in the constructor";

		ASSERT_TRUE(true == logFromNvram->IsSuccessful()) 
			<< "CPersonalBankerLog - m_bSuccessful was not properly initialized in the constructor";

		//Cleanup
		delete logFromNvram;
		delete card;
	}

	/// <summary>
	/// Test the implementation of:
	///     CPersonalBankerLog::CPersonalBankerLog(CPersonalBankerTransfer * ...)
	/// Test with canceled transfer.
	/// </summary>
	TEST_F(LogUnitTests, PersonalBankerLogConstructorTransferCanceledTest)
	{
		//Setup
		CPersonalBankerTransfer *pbTransfer = new CPersonalBankerTransfer(CUtilities::GetTickCount(), MEMORY_NONE);
		PersonalBankerTransferType transferType(CashFromGame);
		time_t cardTime = CUtilities::GetTimeFromFields(2013,03,07,12,12,12);
		CCard *card = new CCard(cardTime,cardTime);
		card->SetCardID("A531000002");
		// copy constructor
		int cardIdLength = 0;
		const int PBTAMOUNT = 1000;
		const byte *cardID = card->GetCardID(cardIdLength);			
		pbTransfer->SetAmount(PBTAMOUNT);
		pbTransfer->SetRestrictedAmount(PBTAMOUNT);
		pbTransfer->SetNonrestrictedAmount(PBTAMOUNT);
		pbTransfer->SetPersonalBankerType(CanceledSessionRequest);
		pbTransfer->SetPromoPersonalBankerTransferType(CashToGame);
		pbTransfer->SetCardNumber(cardID, cardIdLength);
		pbTransfer->SetCashoutToHostTransfer(true);

		__int64 amount(23);
		time_t time = 123456000;
		CPersonalBankerLog* log(new CPersonalBankerLog(pbTransfer, transferType, amount, time));

		//Tests
		ASSERT_TRUE(transferType == log->GetSource()) 
			<< "CPersonalBankerLog - m_source was not properly initialized in the constructor";

		ASSERT_TRUE(amount == log->GetAmount()) 
			<< "CPersonalBankerLog - m_amount was not properly initialized in the constructor";

		ASSERT_TRUE(time == log->GetDateTime()) 
			<< "CPersonalBankerLog - m_dateTime was not properly initialized in the constructor";

		int memcmpResult = memcmp(log->GetCardID(cardIdLength), cardID, cardIdLength);
		bool areEqual = 0 == memcmpResult;
		ASSERT_TRUE(areEqual) 
			<< "CPersonalBankerLog - m_cardID was not properly initialized in the constructor";

		ASSERT_TRUE(!pbTransfer->IsTransferRequestCanceled() == log->IsSuccessful()) 
			<< "CPersonalBankerLog - m_bSuccessful was not properly initialized in the constructor";

		ASSERT_TRUE(log->IsCashoutToHost())
			<< "CPersonalBankerLog - m_bCashoutToHost was not properly initialized in the constructor";

		ASSERT_STREQ(log->GetSourceString().c_str(), "Cashout Cash from game")
			<< "CPersonalBankerLog - GetSourceString() returned unexpected string";

		//Cleanup
		delete log;
		delete pbTransfer;
	}

	TEST_F(LogUnitTests, PersonalBankerLogFunctionsTest)
	{
		//Setup
		CPersonalBankerTransfer *pbTransfer = new CPersonalBankerTransfer(CUtilities::GetTickCount(), MEMORY_NONE);
		PersonalBankerTransferType transferType(PromoDollarsFromGame);
		__int64 amount(23);
		time_t  time = 123456000;
		CPersonalBankerLog* log(new CPersonalBankerLog(pbTransfer, transferType, amount, time));

		//Tests
		log->SetIsPending(true);
		ASSERT_TRUE(log->IsPending()) << "CPersonalBankerLog - SetIsPending did not work.";

		log->SetIsPending(false);
		ASSERT_FALSE(log->IsPending()) << "CPersonalBankerLog - SetIsPending did not work.";

		//Cleanup
		delete log;
		delete pbTransfer;
	}

	TEST_F(LogUnitTests, TicketLogConstructorTest)
	{
		//Setup
		CTicket ticket;
		TicketActionCode ticketActionCode(VenReplyTimeout);
		__int64 amount(23);
		WORD sequenceNumber(69);
		bool isTicketIn(false);
		ticket.SetType(TicketTypeNoValue, isTicketIn);
		ticket.SetAmount(amount);
		ticket.SetActionCode(ticketActionCode);
		ticket.SetTicketNumber(sequenceNumber);
		byte validationNumber[VALIDATION_NUMBER_LENGTH];
		for(int index = 0; index < VALIDATION_NUMBER_LENGTH; ++index)
		{
			validationNumber[index] = index + 1;
		}
		ticket.SetValidationNumber(validationNumber, VALIDATION_NUMBER_LENGTH);
		CTicketLog* log(new CTicketLog(ticket));

		//Tests
		ASSERT_TRUE(amount == log->GetAmount()) 
			<< "CTicketLog - m_amount was not properly initialized in the constructor";

		ASSERT_TRUE(isTicketIn == log->GetTicketIn()) 
			<< "CTicketLog - m_isTicketIn was not properly initialized in the constructor";

		ASSERT_TRUE(sequenceNumber == log->GetSequenceNum()) 
			<< "CTicketLog - m_sequenceNum was not properly initialized in the constructor";

		ASSERT_TRUE(ticketActionCode == log->GetTicketActionCode()) 
			<< "CTicketLog - m_ticketAction was not properly initialized in the constructor";

		//Cleanup
		delete log;
	}
}
