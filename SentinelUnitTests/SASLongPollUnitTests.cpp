#include "stdafx.h"
#include "SASProtocol/LongPoll6FMessage.h"
#include "SASProtocol/LongPoll1CMessage.h"
#include "SASProtocol/LongPollUMessage.h"
#include "SASProtocol/LongPollDMessage.h"
#include "Meters.h"
#include "gtest/gtest.h"

namespace SentinelNativeUnitTests
{

		TEST(SASLongPollUnitTests, SASPoll_6F_MessageBuffersTest)
		{

			int gameAddress = 1; // all games are address 1
			int meterCount = 0;
			CMeters *meters = new CMeters(MEMORY_NONE);
			CSASPollMessage *poll = new CLongPoll6FMessage(gameAddress, meters);

			// 1 metercode
			meterCount = 1;
			meters->SetMeter(gCI, INVALID_METER_VALUE);
			poll->Fill(false);
			ASSERT_TRUE(poll->GetSendBuffer() != NULL) << "Send buffer size is NULL";
			ASSERT_TRUE(poll->GetResponseBuffer() != NULL) << "Receive buffer size is NULL";
			ASSERT_TRUE(poll->GetSendBufferSize() ==  X6F_EXTRADATASIZE + meterCount * 2) << "6F long poll send buffer size is incorrect";
			ASSERT_TRUE(poll->GetResponseBufferSize() ==  X6F_DEFMETERSIZE + X6F_RESPONSE_EXTRADATASIZE + CRCSIZE) << "6F long poll response buffer size is incorrect";

			delete poll;
			// 3 meter codes, gCI = 1 meter code, gCO = 2 meter codes
			meterCount = 3;
			meters->SetMeter(gCO, INVALID_METER_VALUE);
			poll = new CLongPoll6FMessage(gameAddress, meters);
			poll->Fill();
			ASSERT_TRUE(poll->GetSendBuffer() != NULL) << "Multiple meter send buffer size is NULL";
			ASSERT_TRUE(poll->GetSendBufferSize() ==  X6F_EXTRADATASIZE + meterCount * 2) << "6F long poll send buffer size is incorrect";
			ASSERT_TRUE(poll->GetResponseBuffer() != NULL) << "Multiple meter receive buffer size is NULL";
			ASSERT_TRUE(poll->GetResponseBufferSize() ==  3 * X6F_DEFMETERSIZE + X6F_RESPONSE_EXTRADATASIZE + CRCSIZE) << "6F long poll response buffer size is incorrect";

			delete poll;
			delete meters;

		}

		// fixed lengthBuffers
		TEST(SASLongPollUnitTests, SASPoll_1C_MessageBuffersTest)
		{

			int gameAddress = 1; // all games are address 1
			CMeters *meters = new CMeters(MEMORY_NONE);
			CSASPollMessage *poll = new CLongPoll1CMessage(gameAddress, meters);
			poll->Fill();
			ASSERT_TRUE(poll->GetSendBuffer() != NULL) << "Send buffer size is NULL";
			ASSERT_TRUE(poll->GetResponseBuffer() != NULL) << "Receive buffer size is NULL";
			ASSERT_TRUE(poll->GetSendBufferSize() ==  2) << "1C long poll send buffer size is incorrect";
			ASSERT_TRUE(poll->GetResponseBufferSize() ==  36) << "1C long poll response buffer size is incorrect";

			delete poll;
			delete meters;

		}

		TEST(SASLongPollUnitTests, SASPollSwapPollValueTest)
		{
			int gameAddress = 1; // all games are address 1
			CMeters *meters = new CMeters(MEMORY_NONE);
			CSASPollMessage *poll = new CLongPoll1CMessage(gameAddress, meters);
			ASSERT_FALSE(poll->SwapPollValue()) << "Swap Poll Value didn't return false";
			ASSERT_TRUE(poll->GetPollValue() == 0x1C) << "Get Poll Value set incorrect poll value";

			delete poll;
			poll = new CLongPoll6FMessage(gameAddress, meters);
			meters->SetMeter(gCI);
			poll->Fill();
			ASSERT_TRUE(poll->SwapPollValue()) << "Swap Poll Value didn't return true";
			ASSERT_TRUE(poll->GetPollValue() == 0xaf) << "Get Poll Value didn't return expected 0xaf value";
			ASSERT_TRUE(poll->SwapPollValue()) << "Swap Poll Value didn't return true";
			ASSERT_TRUE(poll->GetPollValue() == 0x6f) << "Get Poll Value didn't return expected 0x6f value";

			delete meters;
			delete poll;


		}

		TEST(SASLongPollUnitTests, SASPollMessageGetMeterCodeForMeterNumberTest)
		{
			byte primary = INVALID_SAS_METER_CODE;
			byte secondary = INVALID_SAS_METER_CODE;
			int meterNumber;
			int gameNumber = 0;
			// EGM meters
			meterNumber = gCI;
			CSASPollMessage::GetMeterCodeForMeterNumber(meterNumber, gameNumber, primary, secondary);
			ASSERT_TRUE(primary == 0x00);
			ASSERT_TRUE(secondary == INVALID_SAS_METER_CODE);

			meterNumber = gCO;
			CSASPollMessage::GetMeterCodeForMeterNumber(meterNumber, gameNumber, primary, secondary);
			ASSERT_TRUE(primary == 0x1c);
			ASSERT_TRUE(secondary == 0x01);

			meterNumber = mBDr;
			CSASPollMessage::GetMeterCodeForMeterNumber(meterNumber, gameNumber, primary, secondary);
			ASSERT_TRUE(primary == INVALID_SAS_METER_CODE);
			ASSERT_TRUE(secondary == INVALID_SAS_METER_CODE);

			// Subgame Meters
			gameNumber = 1;
			CSASPollMessage::GetMeterCodeForMeterNumber(meterNumber, gameNumber, primary, secondary);
			ASSERT_TRUE(primary == INVALID_SAS_METER_CODE);
			ASSERT_TRUE(secondary == INVALID_SAS_METER_CODE);

			meterNumber = SUBGAME_METER_WTPP;
			CSASPollMessage::GetMeterCodeForMeterNumber(meterNumber, gameNumber, primary, secondary);
			ASSERT_TRUE(primary == 0x7f);
			ASSERT_TRUE(secondary == INVALID_SAS_METER_CODE);
			
		}

		TEST(SASLongPollUnitTests, SASPollMessageDefaultIncOnNoResponse)
		{
			CMeters *meters = new CMeters(MEMORY_NONE);
			
			// Test base class default
			CSASPollMessage *lp = new CLongPoll1CMessage(1, meters);
			ASSERT_TRUE( lp->IncRetryOnNoResponse() );
			delete lp;

			// Test EFT class override
			lp = new CLongPollDMessage(1);
			ASSERT_FALSE( lp->IncRetryOnNoResponse() );
			delete lp;

			lp = new CLongPollUMessage(1);
			ASSERT_FALSE( lp->IncRetryOnNoResponse() );
			delete lp;

			delete meters;


		}

}
