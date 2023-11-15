#include "stdafx.h"
#include "SASProtocol/SASProtocolState.h"
#include "Ticket.h"
#include "gtest/gtest.h"

namespace SentinelUnitTests
{
		TEST(SASProtocolStateUnitTests, SASProtocolStateEGMConfig)
		{
			CEGMConfig config(MEMORY_NONE);
			ASSERT_TRUE(config.GetEGMType() == EEPROM_EGM_TYPE_DEFAULT);

			CSASProtocolState state;
			state.SetEGMConfig(config);
			ASSERT_TRUE(state.GetEGMConfig().GetEGMType() == EEPROM_EGM_TYPE_DEFAULT);

			config.SetEGMType(IGTS2000);
			state.SetEGMConfig(config);
			ASSERT_TRUE(state.GetEGMConfig().GetEGMType() == IGTS2000);

		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateIgnoreTicketPrinted)
		{
			CSASProtocolState state;
			ASSERT_TRUE(state.GetIgnoreTicketPrinted());

			state.SetIgnoreTicketPrinted(false);
			ASSERT_FALSE(state.GetIgnoreTicketPrinted());

			state.SetIgnoreTicketPrinted(true);
			ASSERT_TRUE(state.GetIgnoreTicketPrinted());
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateSnapshotType)
		{
			CSASProtocolState state;
			ASSERT_TRUE(state.GetSnapshotType() == SnapshotNone);

			state.SetSnapshotType(MaintenancePlayEnd);
			ASSERT_TRUE(state.GetSnapshotType() == MaintenancePlayEnd);
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateSlotmastID)
		{
			CSASProtocolState state;
			ASSERT_TRUE(state.GetSlotmastID() == 0);

			state.SetSlotmastID(1111);
			ASSERT_TRUE(state.GetSlotmastID() == 1111);
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateCasinoNumber)
		{
			CSASProtocolState state;
			ASSERT_TRUE(state.GetCasinoNumber() == 0);

			state.SetCasinoNumber(11);
			ASSERT_TRUE(state.GetCasinoNumber() == 11);
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateEnableEGM)
		{
			CSASProtocolState state;
			ASSERT_TRUE(state.GetEnableEGM());

			state.SetEnableEGM(false, false);
			ASSERT_FALSE(state.GetEnableEGM());

			state.SetEnableEGM(false, true);
			ASSERT_TRUE(state.GetEnableEGM());
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateAllawableEnableDisable)
		{
			CSASProtocolState state;
			ASSERT_TRUE(state.GetAllowableEnableDisable());

			state.SetAllowableEnableDisable(false);
			ASSERT_FALSE(state.GetAllowableEnableDisable());

			state.SetAllowableEnableDisable(true);
			ASSERT_TRUE(state.GetAllowableEnableDisable());
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateAllowableGameDelayPoll)
		{
			CSASProtocolState state;
			ASSERT_TRUE(state.GetAllowableGameDelayPoll());

			state.SetAllowableGameDelayPoll(false);
			ASSERT_FALSE(state.GetAllowableGameDelayPoll());

			state.SetAllowableGameDelayPoll(true);
			ASSERT_TRUE(state.GetAllowableGameDelayPoll());
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateAllowStepdownResetForWMS)
		{
			CSASProtocolState state;
			ASSERT_FALSE(state.GetAllowStepdownResetForWMS());

			state.SetAllowStepdownResetForWMS(true);
			ASSERT_TRUE(state.GetAllowStepdownResetForWMS());

			state.SetAllowStepdownResetForWMS(false);
			ASSERT_FALSE(state.GetAllowStepdownResetForWMS());
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateIsEnhancedTicketing)
		{
			CSASProtocolState state;
			ASSERT_FALSE(state.GetIsEnhancedTicketing());

			state.SetIsEnhancedTicketing(true);
			ASSERT_TRUE(state.GetIsEnhancedTicketing());

			state.SetIsEnhancedTicketing(false);
			ASSERT_FALSE(state.GetIsEnhancedTicketing());
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateTicketingEnabled)
		{
			CSASProtocolState state;
			ASSERT_FALSE(state.GetTicketingEnabled());

			state.SetTicketingEnabled(true);
			ASSERT_TRUE(state.GetTicketingEnabled());

			state.SetTicketingEnabled(false);
			ASSERT_FALSE(state.GetTicketingEnabled());
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateTicketinIngEnabled)
		{
			CSASProtocolState state;
			ASSERT_FALSE(state.GetTicketingInEnabled());

			state.SetTicketingInEnabled(true);
			ASSERT_TRUE(state.GetTicketingInEnabled());

			state.SetTicketingInEnabled(false);
			ASSERT_FALSE(state.GetTicketingInEnabled());
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateTicketingOutEnabled)
		{
			CSASProtocolState state;
			ASSERT_FALSE(state.GetTicketingOutEnabled());

			state.SetTicketingOutEnabled(true);
			ASSERT_TRUE(state.GetTicketingOutEnabled());

			state.SetTicketingOutEnabled(false);
			ASSERT_FALSE(state.GetTicketingOutEnabled());
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateLastTicketPrinted)
		{
			CSASProtocolState state;
			ASSERT_TRUE(state.GetLastPrintedTicket() == nullptr);

			CTicket *ticket = new CTicket();
			state.SetLastPrintedTicket(ticket);
			ASSERT_TRUE(state.GetLastPrintedTicket() == ticket);

			state.SetLastPrintedTicket(nullptr);
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateSupportMultiDenomExtensions)
		{
			CSASProtocolState state;
			ASSERT_FALSE(state.GetSupportMultiDenomExtensions());

			state.SetSupportMultiDenomExtensions(true);
			ASSERT_TRUE(state.GetSupportMultiDenomExtensions());

			state.SetSupportMultiDenomExtensions(false);
			ASSERT_FALSE(state.GetSupportMultiDenomExtensions());
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateLockPending)
		{
			CSASProtocolState state;
			ASSERT_FALSE(state.GetLockPending());

			state.SetLockPending(true);
			ASSERT_TRUE(state.GetLockPending());

			state.SetLockPending(false);
			ASSERT_FALSE(state.GetLockPending());
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateValidationExtensions)
		{
			CSASProtocolState state;
			ASSERT_FALSE(state.GetValidationExtensions());

			state.SetValidationExtensions(true);
			ASSERT_TRUE(state.GetValidationExtensions());

			state.SetValidationExtensions(false);
			ASSERT_FALSE(state.GetValidationExtensions());
		}

		TEST(SASProtocolStateUnitTests, SASProtocolStateLastLockRequestTicks)
		{
			CSASProtocolState state;
			ASSERT_TRUE(state.GetLastLockRequest() == 0);

			state.SetLastLockRequest(100);
			ASSERT_TRUE(state.GetLastLockRequest() == 100);

		}

		TEST(SASProtocolStateUnitTests, CheckIfTransactionNeedsToBeSentForLockUnlockEgmTest)
		{
			//Test#1 enableGame = true, ackedSuccessfully = true
			CSASProtocolState state1;
			TransactionType transactionType(NONE_SPECIFIED);
			ASSERT_FALSE(state1.CheckIfTransactionNeedsToBeSentForLockUnlockEgm(true, true, transactionType));
			ASSERT_TRUE(EGM_UNLOCK_ACK == transactionType);

			//Test#2 enableGame = true, ackedSuccessfully = false
			CSASProtocolState state2;
			ASSERT_FALSE(state2.CheckIfTransactionNeedsToBeSentForLockUnlockEgm(true, false, transactionType));
			ASSERT_TRUE(EGM_UNLOCK_NACK == transactionType);

			//Test#3 enableGame = false, ackedSuccessfully = true
			CSASProtocolState state3;
			ASSERT_FALSE(state3.CheckIfTransactionNeedsToBeSentForLockUnlockEgm(false, true, transactionType));
			ASSERT_TRUE(EGM_LOCK_ACK == transactionType);

			//Test#4 enableGame = false, ackedSuccessfully = false
			CSASProtocolState state4;
			ASSERT_FALSE(state4.CheckIfTransactionNeedsToBeSentForLockUnlockEgm(false, false, transactionType));
			ASSERT_TRUE(EGM_LOCK_NACK == transactionType);

			//Test#5a enableGame = false, ackedSuccessfully = false
			CSASProtocolState state5;
			state5.SetEnableEGM(false, false);
			ASSERT_TRUE(state5.CheckIfTransactionNeedsToBeSentForLockUnlockEgm(false, false, transactionType));
			ASSERT_TRUE(EGM_LOCK_NACK == transactionType);

			//Test#5b enableGame = false, ackedSuccessfully = true
			ASSERT_TRUE(state5.CheckIfTransactionNeedsToBeSentForLockUnlockEgm(false, true, transactionType));
			ASSERT_TRUE(EGM_LOCK_ACK == transactionType);

			//Test#5c enableGame = true, ackedSuccessfully = false
			state5.SetEnableEGM(false, true);
			ASSERT_TRUE(state5.CheckIfTransactionNeedsToBeSentForLockUnlockEgm(true, false, transactionType));
			ASSERT_TRUE(EGM_UNLOCK_NACK == transactionType);

			//Test#5d enableGame = true, ackedSuccessfully = true
			ASSERT_TRUE(state5.CheckIfTransactionNeedsToBeSentForLockUnlockEgm(true, true, transactionType));
			ASSERT_TRUE(EGM_UNLOCK_ACK == transactionType);

			//Test#6 enableGame = false, ackedSuccessfully = false
			CSASProtocolState state6;
			state6.SetEnableEGM(true, false);
			ASSERT_FALSE(state6.CheckIfTransactionNeedsToBeSentForLockUnlockEgm(false, false, transactionType));
			ASSERT_TRUE(EGM_LOCK_NACK == transactionType);

			//Test#7 enableGame = false, ackedSuccessfully = false
			CSASProtocolState state7;
			state7.SetEnableEGM(true, false);
			ASSERT_FALSE(state7.CheckIfTransactionNeedsToBeSentForLockUnlockEgm(false, true, transactionType));
			ASSERT_TRUE(EGM_LOCK_ACK == transactionType);
		}
}
