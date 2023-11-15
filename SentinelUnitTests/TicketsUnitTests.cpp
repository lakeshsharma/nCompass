#include "stdafx.h"

#include "GlobalDefs.h"
#include "Ticket.h"
#include "Tickets.h"
#include "UI/TicketLog.h"
#include "UnitTestingUtils.h"

#include "gtest/gtest.h"

namespace SentinelNativeUnitTests
{

		TEST(TicketsUnitTests, TicketIsDuplicate)
		{
			
			CConfig config(false, "");
			CTickets *tickets = new CTickets(config);
			//Setup

			CTicket ticket;
			ticket.SetActionCode(IncorrectMachineId);
			const __int64 LargeAmount = 9223372036854775806;
			ticket.SetAmount( LargeAmount );
			const byte Duration = 255;
			ticket.SetExpirationDuration( Duration );
			const TicketExpirationType ExpirationType = EXPIRES_MINUTES;
			ticket.SetExpirationType( ExpirationType );
			tm GameTm;
            GameTm.tm_sec = 3;
            GameTm.tm_min = 2;
            GameTm.tm_hour = 1;
            GameTm.tm_mday = 5;
            GameTm.tm_mon = 11;
            GameTm.tm_year = 121;

            time_t GameDate = mktime(&GameTm);
            ticket.SetGameDateTime( GameDate );
			ticket.SetID( LargeAmount );
			const int MachineNumber = 10999;
			ticket.SetMachineNumber( MachineNumber );
			const TicketResultCode ResultCode = ActionCodeNotSupported;
			ticket.SetResultCode( ResultCode );
			const WORD SequenceNumber = 1234;
			ticket.SetSequenceNumber( SequenceNumber );
			const int TicketNumber = 12345678;
			ticket.SetTicketNumber( TicketNumber );
			const TicketType Type = CouponPromo;
			ticket.SetType( Type, false ); // ticket out
			byte *validationNumber = new byte[VALIDATION_NUMBER_LENGTH];
			memset(validationNumber, 255, VALIDATION_NUMBER_LENGTH);
			ticket.SetValidationNumber( validationNumber, VALIDATION_NUMBER_LENGTH );

			CTicket *ticket2 = new CTicket();
			ticket2->SetActionCode(IncorrectMachineId);
			ticket2->SetAmount( LargeAmount );
			ticket2->SetExpirationDuration( Duration );
			ticket2->SetExpirationType( ExpirationType );
			ticket2->SetGameDateTime( GameDate );
			ticket2->SetID( LargeAmount );
			ticket2->SetMachineNumber( MachineNumber );
			ticket2->SetResultCode( ResultCode );
			ticket2->SetSequenceNumber( SequenceNumber );
			ticket2->SetTicketNumber( TicketNumber );
			ticket2->SetType( Type, false ); // ticket out
			ticket2->SetValidationNumber( validationNumber, VALIDATION_NUMBER_LENGTH );

			ASSERT_FALSE(tickets->IsDuplicate(*ticket2));
			tickets->StoreTicket(ticket);
			ASSERT_TRUE(tickets->IsDuplicate(*ticket2));

            GameTm.tm_sec = 3;
            GameTm.tm_min = 2;
            GameTm.tm_hour = 1;
            GameTm.tm_mday = 5;
            GameTm.tm_mon = 8;
            GameTm.tm_year = 113;
			ticket2->SetGameDateTime( mktime(&GameTm));
			ASSERT_FALSE(tickets->IsDuplicate(*ticket2));
			ticket2->SetGameDateTime(GameDate);

			ticket2->SetAmount( 10 );
			ASSERT_FALSE(tickets->IsDuplicate(*ticket2));
			ticket2->SetAmount( LargeAmount );

			validationNumber[1] = 5;
			ticket2->SetValidationNumber( validationNumber, VALIDATION_NUMBER_LENGTH );
			ASSERT_FALSE(tickets->IsDuplicate(*ticket2));

			delete [] validationNumber;
			delete ticket2;
			delete tickets;

		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::IsDuplicate()
		/// Test with ticket in, no stored ticket in.
		/// </summary>
		TEST(TicketsUnitTests, TicketIsDuplicate1)
		{
			const bool ticketInTrue(true);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist()";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);

			// Test.
			ASSERT_FALSE(tickets.IsDuplicate(ticket)) << "Test with ticket in, no stored ticket in.";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::IsDuplicate()
		/// Test with ticket in, stored ticket in, not duplicate.
		/// </summary>
		TEST(TicketsUnitTests, TicketIsDuplicate2)
		{
			const bool ticketInTrue(true);
			const byte validationNumber1[] = { 0x12, 0x34 };
			const byte validationNumber2[] = { 0x12, 0x34, 0x56 };

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist()";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- setup";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist()";


			// Test.
			ticket.SetValidationNumber(validationNumber2, sizeof(validationNumber2));
			ASSERT_FALSE(tickets.IsDuplicate(ticket)) << "Test with ticket in, stored ticket in, not duplicate.";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::IsDuplicate()
		/// Test with ticket in, stored ticket in, duplicate.
		/// </summary>
		TEST(TicketsUnitTests, TicketIsDuplicate3)
		{
			const bool ticketInTrue(true);
			const byte validationNumber1[] = { 0x12, 0x34 };

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist()";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- setup";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist()";


			// Test.
			ASSERT_TRUE(tickets.IsDuplicate(ticket)) << "Test with ticket in, stored ticket in, duplicate.";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::IsDuplicate()
		/// Test with ticket out, no stored ticket out.
		/// </summary>
		TEST(TicketsUnitTests, TicketIsDuplicate4)
		{
			const bool ticketInFalse(false);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketOut();

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInFalse);

			// Test.
			ASSERT_FALSE(tickets.IsDuplicate(ticket)) << "Test with ticket out, no stored ticket out.";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::IsDuplicate()
		/// Test with ticket out, stored ticket out, not duplicate.
		/// </summary>
		TEST(TicketsUnitTests, TicketIsDuplicate5)
		{
			const bool ticketInFalse(false);
			const byte validationNumber1[] = { 0x12, 0x34 };
			const byte validationNumber2[] = { 0x12, 0x34, 0x56 };

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketOut();

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInFalse);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- setup";

			// Test.
			ticket.SetValidationNumber(validationNumber2, sizeof(validationNumber2));
			ASSERT_FALSE(tickets.IsDuplicate(ticket)) << "Test with ticket out, stored ticket out, not duplicate.";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::IsDuplicate()
		/// Test with ticket out, stored ticket out, duplicate.
		/// </summary>
		TEST(TicketsUnitTests, TicketIsDuplicate6)
		{
			const bool ticketInFalse(false);
			const byte validationNumber1[] = { 0x12, 0x34 };

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketOut();

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInFalse);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- setup";

			// Test.
			ASSERT_TRUE(tickets.IsDuplicate(ticket)) << "Test with ticket out, stored ticket out, duplicate.";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::FinalizeTicket()
		/// Test with ticket in, no stored ticket in.
		/// </summary>
		TEST(TicketsUnitTests, TicketFinalizeTicket1)
		{
			const bool ticketInTrue(true);
			const bool additionalLogRecordsExpected(0);
			const int testIndex(0);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist()";
			CLogs &logs = *tickets.GetTicketLogs();
			CUnitTestingUtils::LogIndexInfoExpected logIndexInfoExpected;
			CUnitTestingUtils::GetLogIndexInfoExpected(&logs, logIndexInfoExpected, additionalLogRecordsExpected);

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);

			// Test.
			tickets.FinalizeTicket(ticket);
			CUnitTestingUtils::AssertLogInfoExpected(&logs, logIndexInfoExpected, 	testIndex, 	__TFUNCTION__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::FinalizeTicket()
		/// Test with ticket in, and stored ticket in.
		/// </summary>
		TEST(TicketsUnitTests, TicketFinalizeTicket2)
		{
			const bool ticketInTrue(true);
			const bool additionalLogRecordsExpected(1);
			const TicketActionCode actionCode1(TicketActionCodeNoValue);
			const TicketActionCode actionCode2(PrinterFailure);
			const int testIndex(0);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist()";
			CLogs &logs = *tickets.GetTicketLogs();
			CUnitTestingUtils::LogIndexInfoExpected logIndexInfoExpected;
			CUnitTestingUtils::GetLogIndexInfoExpected(&logs, logIndexInfoExpected, additionalLogRecordsExpected);

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ticket.SetActionCode(actionCode1);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- setup";

			// Test.
			ticket.SetActionCode(actionCode2);
			tickets.FinalizeTicket(ticket);
			CUnitTestingUtils::AssertLogInfoExpected(&logs, logIndexInfoExpected, 	testIndex, 	__TFUNCTION__);

			CTicketLog *ticketLog = (CTicketLog *)logs.GetFirst();
			ASSERT_TRUE(ticketLog != NULL) << "IsNotNull ticketLog";
			ASSERT_TRUE(ticketLog->GetTicketActionCode() == actionCode2) << "actionCode";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::FinalizeTicket()
		/// Test with ticket out.
		/// </summary>
		TEST(TicketsUnitTests, TicketFinalizeTicket3)
		{
			const bool ticketInFalse(false);
			const bool additionalLogRecordsExpected(1);
			const __int64 amount(12345678901234567);
			const int testIndex(0);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist()";
			CLogs &logs = *tickets.GetTicketLogs();
			CUnitTestingUtils::LogIndexInfoExpected logIndexInfoExpected;
			CUnitTestingUtils::GetLogIndexInfoExpected(&logs, logIndexInfoExpected, additionalLogRecordsExpected);

			CTicket ticket;
			ticket.SetType(SingleWinNoReceipt, ticketInFalse);
			ticket.SetAmount(amount);
			tickets.FinalizeTicket(ticket);
			CUnitTestingUtils::AssertLogInfoExpected(&logs, logIndexInfoExpected, 	testIndex, 	__TFUNCTION__);

			CTicketLog *ticketLog = (CTicketLog *)logs.GetFirst();
			ASSERT_TRUE(ticketLog != NULL) << "IsNotNull ticketLog";
			ASSERT_TRUE(ticketLog->GetAmount() == amount) << "amount";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::StoreTicket()
		/// Test with ticket in, and stored ticket in, is duplicate.
		/// </summary>
		TEST(TicketsUnitTests, TicketStoreTicket1)
		{
			const bool ticketInTrue(true);
			const WORD sequenceNumber1(0x1234);
			const WORD sequenceNumber2(0x2345);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ticket.SetSequenceNumber(sequenceNumber1);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- setup";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- setup";
			tickets.SetFinalStatusReceived();
			ASSERT_TRUE(tickets.WasFinalStatusReceived()) << "IsTrue tickets.WasFinalStatusReceived() -- setup";

			// Test.
			ticket.SetSequenceNumber(sequenceNumber2);
			ASSERT_TRUE(tickets.StoreTicket(ticket)) << "IsTrue tickets.StoreTicket -- test";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == sequenceNumber1) << "IsTrue tickets.GetTicketInSequenceNumber() -- test";
			ASSERT_TRUE(tickets.WasFinalStatusReceived()) << "IsTrue tickets.WasFinalStatusReceived() -- test";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::StoreTicket()
		/// Test with ticket in, no stored ticket in, not duplicate.
		/// </summary>
		TEST(TicketsUnitTests, TicketStoreTicket2)
		{
			const bool ticketInTrue(true);
			const WORD sequenceNumber2(0x2345);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";
			tickets.SetFinalStatusReceived();
			ASSERT_TRUE(tickets.WasFinalStatusReceived()) << "IsTrue tickets.WasFinalStatusReceived() -- setup";

			// Test.
			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ticket.SetSequenceNumber(sequenceNumber2);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- test";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- test";
			ASSERT_FALSE(tickets.WasFinalStatusReceived()) << "IsFalse tickets.WasFinalStatusReceived() -- test";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == sequenceNumber2) << "IsTrue tickets.GetTicketInSequenceNumber() -- test";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::StoreTicket()
		/// Test with ticket in, stored ticket in, not duplicate.
		/// </summary>
		TEST(TicketsUnitTests, TicketStoreTicket3)
		{
			const bool ticketInTrue(true);
			const WORD sequenceNumber1(0x1234);
			const WORD sequenceNumber2(0x2345);
			const byte validationNumber1[] = { 0x12, 0x34, 0x56 };
			const byte validationNumber2[] = { 0x12, 0x34, 0x56, 0x67 };

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ticket.SetSequenceNumber(sequenceNumber1);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- setup";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- setup";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == sequenceNumber1) << "IsTrue tickets.GetTicketInSequenceNumber() -- setup";
			tickets.SetFinalStatusReceived();
			ASSERT_TRUE(tickets.WasFinalStatusReceived()) << "IsTrue tickets.WasFinalStatusReceived() -- setup";

			// Test.
			ticket.SetSequenceNumber(sequenceNumber2);
			ticket.SetValidationNumber(validationNumber2, sizeof(validationNumber2));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- test";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- test";
			ASSERT_FALSE(tickets.WasFinalStatusReceived()) << "IsFalse tickets.WasFinalStatusReceived() -- test";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == sequenceNumber2) << "IsTrue tickets.GetTicketInSequenceNumber() -- test";
			ASSERT_TRUE(tickets.StoreTicket(ticket)) << "IsTrue tickets.StoreTicket -- test";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::StoreTicket()
		/// Test with ticket out, and stored ticket out, is duplicate.
		/// </summary>
		TEST(TicketsUnitTests, TicketStoreTicket4)
		{
			const bool ticketInFalse(false);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketOut();

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInFalse);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- setup";
			tickets.SetFinalStatusReceived();
			ASSERT_TRUE(tickets.WasFinalStatusReceived()) << "IsTrue tickets.WasFinalStatusReceived() -- setup";

			// Test.
			ASSERT_TRUE(tickets.StoreTicket(ticket)) << "IsTrue tickets.StoreTicket -- test";
			ASSERT_TRUE(tickets.WasFinalStatusReceived()) << "IsTrue tickets.WasFinalStatusReceived() -- test";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::StoreTicket()
		/// Test with ticket out, no stored ticket out, not duplicate.
		/// </summary>
		TEST(TicketsUnitTests, TicketStoreTicket5)
		{
			const bool ticketInFalse(false);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketOut();
			tickets.SetFinalStatusReceived();
			ASSERT_TRUE(tickets.WasFinalStatusReceived()) << "IsTrue tickets.WasFinalStatusReceived() -- setup";

			// Test.
			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInFalse);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- test";
			ASSERT_FALSE(tickets.WasFinalStatusReceived()) << "IsFalse tickets.WasFinalStatusReceived() -- test";
			ASSERT_TRUE(tickets.StoreTicket(ticket)) << "IsTrue tickets.StoreTicket -- test";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::StoreTicket()
		/// Test with ticket out, stored ticket out, not duplicate.
		/// </summary>
		TEST(TicketsUnitTests, TicketStoreTicket6)
		{
			const bool ticketInFalse(false);
			const byte validationNumber1[] = { 0x12, 0x34, 0x56 };
			const byte validationNumber2[] = { 0x12, 0x34, 0x56, 0x67 };

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInFalse);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- setup";
			tickets.SetFinalStatusReceived();
			ASSERT_TRUE(tickets.WasFinalStatusReceived()) << "IsTrue tickets.WasFinalStatusReceived() -- setup";

			// Test.
			ticket.SetValidationNumber(validationNumber2, sizeof(validationNumber2));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- test";
			ASSERT_FALSE(tickets.WasFinalStatusReceived()) << "IsFalse tickets.WasFinalStatusReceived() -- test";
			ASSERT_TRUE(tickets.StoreTicket(ticket)) << "IsTrue tickets.StoreTicket -- test";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::UpdateTicket()
		/// Test with ticket in, and no stored ticket in.
		/// </summary>
		TEST(TicketsUnitTests, TicketUpdateTicket1)
		{
			const bool ticketInTrue(true);
			const bool ticketInFalse(false);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			tickets.RemoveTicketOut();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);

			// Test.
			ASSERT_FALSE(tickets.UpdateTicket(ticket)) << "IsFalse tickets.UpdateTicket -- test";
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- test";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == 0) << "IsTrue tickets.GetTicketInSequenceNumber() -- test";

			// Verify by making additional state changes.
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket in -- verify";
			ticket.SetType(VoucherTicket, ticketInFalse);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket out -- verify";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::UpdateTicket()
		/// Test with ticket in, stored ticket in, and different ticket.
		/// </summary>
		TEST(TicketsUnitTests, TicketUpdateTicket2)
		{
			const bool ticketInTrue(true);
			const bool ticketInFalse(false);
			const WORD sequenceNumber1(0x1230);
			const WORD sequenceNumber2(0x2340);
			const int ticketNumber1(1);
			const int ticketNumber2(1);
			const byte validationNumber1[] = { 0x12, 0x34, 0x56 };
			const byte validationNumber2[] = { 0x12, 0x34, 0x56, 0x67 };

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			tickets.RemoveTicketOut();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ticket.SetSequenceNumber(sequenceNumber1);
			ticket.SetTicketNumber(ticketNumber1);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket in -- setup";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- setup";

			// Test.
			ticket.SetSequenceNumber(sequenceNumber2);
			ticket.SetTicketNumber(ticketNumber2);
			ticket.SetValidationNumber(validationNumber2, sizeof(validationNumber2));
			ASSERT_FALSE(tickets.UpdateTicket(ticket)) << "IsFalse tickets.UpdateTicket -- test";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == sequenceNumber1) << "IsTrue tickets.GetTicketInSequenceNumber() -- test";

			// Verify by attempting/making additional state changes.
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_TRUE(tickets.StoreTicket(ticket)) << "IsTrue tickets.StoreTicket in -- verify";
			ticket.SetType(VoucherTicket, ticketInFalse);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket out -- verify";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::UpdateTicket()
		/// Test with ticket in, stored ticket in, and same ticket
		/// 		ticket.ResultCode() == 0
		///         ticket.ID() == 0
		/// </summary>
		TEST(TicketsUnitTests, TicketUpdateTicket3)
		{
			const bool ticketInTrue(true);
			const bool ticketInFalse(false);
			const WORD sequenceNumber1(0x1230);
			const WORD sequenceNumber2(0x2340);
			const int ticketNumber1(1);
			const int ticketNumber2(1);
			const byte validationNumber1[] = { 0x12, 0x34, 0x56 };
			const byte validationNumber2[] = { 0x12, 0x34, 0x56, 0x67 };
			const TicketResultCode resultCode(Acknowledged);
			const __int64 id(0);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			tickets.RemoveTicketOut();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ticket.SetSequenceNumber(sequenceNumber1);
			ticket.SetTicketNumber(ticketNumber1);
			ticket.SetResultCode(resultCode);
			ticket.SetID(id);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket in -- setup";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- setup";

			// Test.
			ASSERT_FALSE(tickets.UpdateTicket(ticket)) << "IsFalse tickets.UpdateTicket -- test";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == sequenceNumber1) << "IsTrue tickets.GetTicketInSequenceNumber() -- test";

			// Verify by attempting/making additional state changes.
			ASSERT_TRUE(tickets.StoreTicket(ticket)) << "IsTrue tickets.StoreTicket in -- verify";
			ticket.SetType(VoucherTicket, ticketInFalse);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket out -- verify";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::UpdateTicket()
		/// Test with ticket in, stored ticket in, and same ticket
		/// 		ticket.ResultCode() != 0
		///         ticket.ID() == 0
		///         ticket.Number() != 0
		/// </summary>
		TEST(TicketsUnitTests, TicketUpdateTicket4n)
		{
			const bool ticketInTrue(true);
			const bool ticketInFalse(false);
			const WORD sequenceNumber1(0x1230);
			const WORD sequenceNumber2(0x2340);
			const int ticketNumber1(0);
			const int ticketNumber2(1);
			const byte validationNumber1[] = { 0x12, 0x34, 0x56 };
			const byte validationNumber2[] = { 0x12, 0x34, 0x56, 0x67 };
			const TicketResultCode resultCode(TicketResultCodeNoValue);
			const __int64 id(0);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			tickets.RemoveTicketOut();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ticket.SetSequenceNumber(sequenceNumber1);
			ticket.SetTicketNumber(ticketNumber1);
			ticket.SetResultCode(resultCode);
			ticket.SetID(id);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket in -- setup";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- setup";

			// Test.
			ticket.SetSequenceNumber(sequenceNumber2);
			ticket.SetTicketNumber(ticketNumber2);
			ASSERT_TRUE(tickets.UpdateTicket(ticket)) << "IsTrue tickets.UpdateTicket -- test";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == sequenceNumber2) << "IsTrue tickets.GetTicketInSequenceNumber() -- test";

			// Verify by attempting/making additional state changes.
			ASSERT_TRUE(tickets.StoreTicket(ticket)) << "IsTrue tickets.StoreTicket in -- verify";
			ticket.SetType(VoucherTicket, ticketInFalse);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket out -- verify";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::UpdateTicket()
		/// Test with ticket in, stored ticket in, and same ticket
		/// 		ticket.ResultCode() != 0
		///         ticket.ID() == 0
		///         ticket.Number() != 0
		/// </summary>
		TEST(TicketsUnitTests, TicketUpdateTicket4z)
		{
			const bool ticketInTrue(true);
			const bool ticketInFalse(false);
			const WORD sequenceNumber1(0x1230);
			const WORD sequenceNumber2(0x2340);
			const int ticketNumber1(1);
			const int ticketNumber2(0);
			const byte validationNumber1[] = { 0x12, 0x34, 0x56 };
			const byte validationNumber2[] = { 0x12, 0x34, 0x56, 0x67 };
			const TicketResultCode resultCode(TicketResultCodeNoValue);
			const __int64 id(0);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			tickets.RemoveTicketOut();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ticket.SetSequenceNumber(sequenceNumber1);
			ticket.SetTicketNumber(ticketNumber1);
			ticket.SetResultCode(resultCode);
			ticket.SetID(id);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket in -- setup";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- setup";

			// Test.
			ticket.SetSequenceNumber(sequenceNumber2);
			ticket.SetTicketNumber(ticketNumber2);
			ASSERT_TRUE(tickets.UpdateTicket(ticket)) << "IsTrue tickets.UpdateTicket -- test";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == sequenceNumber2) << "IsTrue tickets.GetTicketInSequenceNumber() -- test";

			// Verify by attempting/making additional state changes.
			ASSERT_TRUE(tickets.StoreTicket(ticket)) << "IsTrue tickets.StoreTicket in -- verify";
			ticket.SetType(VoucherTicket, ticketInFalse);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket out -- verify";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::UpdateTicket()
		/// Test with ticket in, stored ticket in, and same ticket
		/// 		ticket.ResultCode() == 0
		///         ticket.ID() != 0
		/// </summary>
		TEST(TicketsUnitTests, TicketUpdateTicket5n)
		{
			const bool ticketInTrue(true);
			const bool ticketInFalse(false);
			const WORD sequenceNumber1(0x1230);
			const WORD sequenceNumber2(0x2340);
			const int ticketNumber1(0);
			const int ticketNumber2(1);
			const byte validationNumber1[] = { 0x12, 0x34, 0x56 };
			const byte validationNumber2[] = { 0x12, 0x34, 0x56, 0x67 };
			const TicketResultCode resultCode(Acknowledged);
			const __int64 id(1);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			tickets.RemoveTicketOut();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ticket.SetSequenceNumber(sequenceNumber1);
			ticket.SetTicketNumber(ticketNumber1);
			ticket.SetResultCode(resultCode);
			ticket.SetID(id);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket in -- setup";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- setup";

			// Test.
			ticket.SetSequenceNumber(sequenceNumber2);
			ticket.SetTicketNumber(ticketNumber2);
			ASSERT_TRUE(tickets.UpdateTicket(ticket)) << "IsTrue tickets.UpdateTicket -- test";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == sequenceNumber2) << "IsTrue tickets.GetTicketInSequenceNumber() -- test";

			// Verify by attempting/making additional state changes.
			ASSERT_TRUE(tickets.StoreTicket(ticket)) << "IsTrue tickets.StoreTicket in -- verify";
			ticket.SetType(VoucherTicket, ticketInFalse);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket out -- verify";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::UpdateTicket()
		/// Test with ticket in, stored ticket in, and same ticket
		/// 		ticket.ResultCode() == 0
		///         ticket.ID() != 0
		/// </summary>
		TEST(TicketsUnitTests, TicketUpdateTicket5z)
		{
			const bool ticketInTrue(true);
			const bool ticketInFalse(false);
			const WORD sequenceNumber1(0x1230);
			const WORD sequenceNumber2(0x2340);
			const int ticketNumber1(1);
			const int ticketNumber2(0);
			const byte validationNumber1[] = { 0x12, 0x34, 0x56 };
			const byte validationNumber2[] = { 0x12, 0x34, 0x56, 0x67 };
			const TicketResultCode resultCode(Acknowledged);
			const __int64 id(1);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			tickets.RemoveTicketOut();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ticket.SetSequenceNumber(sequenceNumber1);
			ticket.SetTicketNumber(ticketNumber1);
			ticket.SetResultCode(resultCode);
			ticket.SetID(id);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket in -- setup";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- setup";

			// Test.
			ticket.SetSequenceNumber(sequenceNumber2);
			ticket.SetTicketNumber(ticketNumber2);
			ASSERT_TRUE(tickets.UpdateTicket(ticket)) << "IsTrue tickets.UpdateTicket -- test";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == sequenceNumber2) << "IsTrue tickets.GetTicketInSequenceNumber() -- test";

			// Verify by attempting/making additional state changes.
			ASSERT_TRUE(tickets.StoreTicket(ticket)) << "IsTrue tickets.StoreTicket in -- verify";
			ticket.SetType(VoucherTicket, ticketInFalse);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket out -- verify";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::UpdateTicket()
		/// Test with ticket out, and no stored ticket out.
		/// </summary>
		TEST(TicketsUnitTests, TicketUpdateTicket6)
		{
			const bool ticketInTrue(true);
			const bool ticketInFalse(false);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			tickets.RemoveTicketOut();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInFalse);

			// Test.
			ASSERT_FALSE(tickets.UpdateTicket(ticket)) << "IsFalse tickets.UpdateTicket -- test";
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- test";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == 0) << "IsTrue tickets.GetTicketInSequenceNumber() -- test";

			// Verify by making additional state changes.
			ticket.SetType(VoucherTicket, ticketInTrue);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket in -- verify";
			ticket.SetType(VoucherTicket, ticketInFalse);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket out -- verify";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::UpdateTicket()
		/// Test with ticket out, stored ticket out, and different ticket.
		/// </summary>
		TEST(TicketsUnitTests, TicketUpdateTicket7)
		{
			const bool ticketInTrue(true);
			const bool ticketInFalse(false);
			const WORD sequenceNumber1(0x1230);
			const WORD sequenceNumber2(0x2340);
			const int ticketNumber1(1);
			const int ticketNumber2(1);
			const __int64 amount1(12345);
			const __int64 amount2(2345);
			const byte validationNumber1[] = { 0x12, 0x34, 0x56 };
			const byte validationNumber2[] = { 0x12, 0x34, 0x56, 0x67 };

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			tickets.RemoveTicketOut();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInFalse);
			ticket.SetSequenceNumber(sequenceNumber1);
			ticket.SetTicketNumber(ticketNumber1);
			ticket.SetAmount(amount1);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket in -- setup";
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- setup";

			// Test.
			ticket.SetSequenceNumber(sequenceNumber2);
			ticket.SetTicketNumber(ticketNumber2);
			ticket.SetAmount(amount2);
			ticket.SetValidationNumber(validationNumber2, sizeof(validationNumber2));
			ASSERT_FALSE(tickets.UpdateTicket(ticket)) << "IsFalse tickets.UpdateTicket -- test";
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- test";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == 0) << "IsTrue tickets.GetTicketInSequenceNumber() -- test";

			// Verify by attempting/making additional state changes.
			ticket.SetType(VoucherTicket, ticketInTrue);
			ticket.SetAmount(amount1);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket in -- verify";
			ticket.SetType(VoucherTicket, ticketInFalse);
			ASSERT_TRUE(tickets.StoreTicket(ticket)) << "IsTrue tickets.StoreTicket out -- verify";
			ticket.SetAmount(amount2);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket out -- verify";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::UpdateTicket()
		/// Test with ticket out, stored ticket out, and same ticket.
		/// </summary>
		TEST(TicketsUnitTests, TicketUpdateTicket8)
		{
			const bool ticketInTrue(true);
			const bool ticketInFalse(false);
			const WORD sequenceNumber1(0x1230);
			const WORD sequenceNumber2(0x2340);
			const int ticketNumber1(1);
			const int ticketNumber2(1);
			const __int64 amount1(12345);
			const __int64 amount2(2345);
			const byte validationNumber1[] = { 0x12, 0x34, 0x56 };
			const byte validationNumber2[] = { 0x12, 0x34, 0x56, 0x67 };

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			tickets.RemoveTicketOut();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInFalse);
			ticket.SetSequenceNumber(sequenceNumber1);
			ticket.SetTicketNumber(ticketNumber1);
			ticket.SetAmount(amount1);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket in -- setup";
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- setup";

			// Test.
			ticket.SetSequenceNumber(sequenceNumber2);
			ticket.SetTicketNumber(ticketNumber2);
			ASSERT_TRUE(tickets.UpdateTicket(ticket)) << "IsTrue tickets.UpdateTicket -- test";
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- test";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == 0) << "IsTrue tickets.GetTicketInSequenceNumber() -- test";

			// Verify by attempting/making additional state changes.
			ticket.SetType(VoucherTicket, ticketInTrue);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket in -- verify";
			ticket.SetType(VoucherTicket, ticketInFalse);
			ASSERT_TRUE(tickets.StoreTicket(ticket)) << "IsTrue tickets.StoreTicket out -- verify";
			ticket.SetAmount(amount2);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket out -- verify";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::GetTicketInSequenceNumber()
		/// </summary>
		TEST(TicketsUnitTests, TicketGetTicketInSequenceNumber)
		{
			const bool ticketInTrue(true);
			const WORD sequenceNumber1(0x1234);
			const WORD sequenceNumber2(0x2345);
			const byte validationNumber1[] = { 0x12, 0x34, 0x56 };
			const byte validationNumber2[] = { 0x12, 0x34, 0x56, 0x67 };

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";

			// Test.
			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ticket.SetSequenceNumber(sequenceNumber1);
			ticket.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- setup";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == sequenceNumber1) << "IsTrue tickets.GetTicketInSequenceNumber() -- setup";

			ticket.SetSequenceNumber(sequenceNumber2);
			ticket.SetValidationNumber(validationNumber2, sizeof(validationNumber2));
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- test";
			ASSERT_TRUE(tickets.GetTicketInSequenceNumber() == sequenceNumber2) << "IsTrue tickets.GetTicketInSequenceNumber() -- test";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::GetTicketingInfo()
		/// Verify that a non-NULL pointer is returned.
		/// </summary>
		TEST(TicketsUnitTests, TicketGetTicketingInfo)
		{
			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);

			// Test.
			ASSERT_TRUE(tickets.GetTicketingInfo() != NULL) << "IsNotNull tickets.GetTicketingInfo() -- test";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::CTickets()
		///     CTickets::GetTicketingInfo()
		/// </summary>
		TEST(TicketsUnitTests, TicketGetTicketingInfo_TicketingEnabled1)
		{
			// Setup.
			CConfig varConfig(false, "");
			const CConfig &config(varConfig);

			// Test.
			{
				varConfig.SetTicketingEnabled(false);
				CTickets tickets(config);
				CTicketingInfo *ticketingInfo = tickets.GetTicketingInfo();
				ASSERT_TRUE(ticketingInfo != NULL) << "IsNotNull ticketingInfo";
				ASSERT_FALSE(ticketingInfo->IsTicketingEnabled()) << "IsFalse ticketingInfo->IsTicketingEnabled()";
			}

			{
				varConfig.SetTicketingEnabled(true);
				CTickets tickets(config);
				CTicketingInfo *ticketingInfo = tickets.GetTicketingInfo();
				ASSERT_TRUE(ticketingInfo != NULL) << "IsNotNull ticketingInfo";
				ASSERT_TRUE(ticketingInfo->IsTicketingEnabled()) << "IsTrue ticketingInfo->IsTicketingEnabled()";
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::ConfigSettings()
		///     CTickets::GetTicketingInfo()
		/// </summary>
		TEST(TicketsUnitTests, TicketGetTicketingInfo_TicketingEnabled2)
		{
			// Setup.
			CConfig varConfig(false, "");
			const CConfig &config(varConfig);
			CTickets tickets(config);
			CTicketingInfo *ticketingInfo = tickets.GetTicketingInfo();
			ASSERT_TRUE(ticketingInfo != NULL) << "IsNotNull ticketingInfo";

			// Test.
			varConfig.SetTicketingEnabled(false);
			tickets.ConfigSettings(config);
			ASSERT_FALSE(ticketingInfo->IsTicketingEnabled()) << "IsFalse ticketingInfo->IsTicketingEnabled()";

			varConfig.SetTicketingEnabled(true);
			tickets.ConfigSettings(config);
			ASSERT_TRUE(ticketingInfo->IsTicketingEnabled()) << "IsTrue ticketingInfo->IsTicketingEnabled()";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::GetTicketLogs()
		/// Verify that a non-NULL pointer is returned.
		/// </summary>
		TEST(TicketsUnitTests, TicketGetTicketLogs1)
		{
			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);

			// Test.
			ASSERT_TRUE(tickets.GetTicketLogs() != NULL) << "IsNotNull tickets.GetTicketLogs() -- test";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::GetTicketLogs()
		/// Verify that a record is added to the log.
		/// </summary>
		TEST(TicketsUnitTests, TicketGetTicketLogs2)
		{
			const bool ticketInTrue(true);
			const bool additionalLogRecordsExpected(1);
			const int testIndex(0);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist()";

			// Test.
			CLogs &logs = *tickets.GetTicketLogs();
			CUnitTestingUtils::LogIndexInfoExpected logIndexInfoExpected;
			CUnitTestingUtils::GetLogIndexInfoExpected(&logs, logIndexInfoExpected, additionalLogRecordsExpected);

			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- setup";

			tickets.FinalizeTicket(ticket);
			CUnitTestingUtils::AssertLogInfoExpected(&logs, logIndexInfoExpected, 	testIndex, 	__TFUNCTION__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::CTickets()
		///     CTickets::HasTicketInExpired()
		/// Test behavior after construction with no stored ticket in.
		/// </summary>
		TEST(TicketsUnitTests, TicketHasTicketInExpired1)
		{
			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);

			// Test.
			ASSERT_FALSE(tickets.HasTicketInExpired()) << "IsFalse tickets.HasTicketInExpired()";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::CTickets()
		///     CTickets::HasTicketInExpired()
		/// Test behavior after construction with stored ticket in and long Sleep().
		/// </summary>
		TEST(TicketsUnitTests, TicketHasTicketInExpired2)
		{
			const bool ticketInTrue(true);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- test";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- test";

			// Test.
			ASSERT_FALSE(tickets.HasTicketInExpired()) << "IsFalse tickets.HasTicketInExpired()";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::CTickets()
		///     CTickets::HasTicketInExpired()
		///     CTickets::ResetTicketInExpire()
		/// Test behavior after construction with stored ticket in and long Sleep().
		/// </summary>
		TEST(TicketsUnitTests, TicketHasTicketInExpired3)
		{
			const bool ticketInTrue(true);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- test";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- test";

			// Test.
			ASSERT_FALSE(tickets.HasTicketInExpired()) << "IsFalse tickets.HasTicketInExpired() -- first";
			sleep(31);
			ASSERT_TRUE(tickets.HasTicketInExpired()) << "IsTrue tickets.HasTicketInExpired()";
			tickets.ResetTicketInExpire();
			ASSERT_FALSE(tickets.HasTicketInExpired()) << "IsFalse tickets.HasTicketInExpired() -- again";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::DoesTicketInExist()
		///     CTickets::RemoveTicketIn()
		/// </summary>
		TEST(TicketsUnitTests, TicketRemoveTicketIn)
		{
			const bool ticketInTrue(true);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);

			// Test.
			tickets.RemoveTicketIn();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";
			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- test";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- test";
			tickets.RemoveTicketIn();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::RemoveTicketOut()
		/// </summary>
		TEST(TicketsUnitTests, TicketRemoveTicketOut)
		{
			const bool ticketInFalse(false);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);

			// Test.
			tickets.RemoveTicketOut();
			tickets.SetFinalStatusReceived();
			ASSERT_TRUE(tickets.WasFinalStatusReceived()) << "IsTrue tickets.WasFinalStatusReceived() -- setup";
			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInFalse);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- test";
			ASSERT_FALSE(tickets.WasFinalStatusReceived()) << "IsFalse tickets.WasFinalStatusReceived() -- test";
			ASSERT_TRUE(tickets.StoreTicket(ticket)) << "IsTrue tickets.StoreTicket -- test";
			tickets.RemoveTicketOut();
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- test";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::SetNumberOfTicketsPending()
		///     CTickets::GetNumberOfTicketsPending()
		/// </summary>
		TEST(TicketsUnitTests, TicketGetNumberOfTicketsPending)
		{
			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);

			// Test.
			tickets.SetNumberOfTicketsPending(INT_MIN);
			ASSERT_TRUE(tickets.GetNumberOfTicketsPending() == INT_MIN) << "IsTrue tickets.GetNumberOfTicketsPending() == INT_MIN";
			tickets.SetNumberOfTicketsPending(-1);
			ASSERT_TRUE(tickets.GetNumberOfTicketsPending() == -1) << "IsTrue tickets.GetNumberOfTicketsPending() == -1";
			tickets.SetNumberOfTicketsPending(0);
			ASSERT_TRUE(tickets.GetNumberOfTicketsPending() == 0) << "IsTrue tickets.GetNumberOfTicketsPending() == 0";
			tickets.SetNumberOfTicketsPending(1);
			ASSERT_TRUE(tickets.GetNumberOfTicketsPending() == 1) << "IsTrue tickets.GetNumberOfTicketsPending() == 1";
			tickets.SetNumberOfTicketsPending(INT_MAX);
			ASSERT_TRUE(tickets.GetNumberOfTicketsPending() == INT_MAX) << "IsTrue tickets.GetNumberOfTicketsPending() == INT_MAX";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::ConfigSettings()
		///     CTickets::GetMaxTicketOutPending()
		/// Test with m_maxTicketsOutPending == 0.
		/// </summary>
		TEST(TicketsUnitTests, TicketConfigSettings0)
		{
			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetTicketingEnabled(true);
			varConfig.SetVoucherOutEnabled(true);
			varConfig.SetTicketBufferingEnabled(false);
			const CConfig &config(varConfig);
			CTickets tickets(config);
			ASSERT_TRUE(tickets.GetMaxTicketOutPending() == 1) << "IsTrue tickets.GetMaxTicketOutPending() == 1";

			// Test.
			{
				CConfig varConfig(false, "");
				varConfig.SetTicketingEnabled(false);
				const CConfig &config(varConfig);
				tickets.ConfigSettings(config);
			}
			ASSERT_TRUE(tickets.GetMaxTicketOutPending() == 0) << "IsTrue tickets.GetMaxTicketOutPending() == 0";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::ConfigSettings()
		///     CTickets::GetMaxTicketOutPending()
		/// Test with m_maxTicketsOutPending == 1.
		/// </summary>
		TEST(TicketsUnitTests, TicketConfigSettings1)
		{
			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetTicketingEnabled(false);
			const CConfig &config(varConfig);
			CTickets tickets(config);
			ASSERT_TRUE(tickets.GetMaxTicketOutPending() == 0) << "IsTrue tickets.GetMaxTicketOutPending() == 0";

			// Test.
			{
				CConfig varConfig(false, "");
				varConfig.SetTicketingEnabled(true);
				varConfig.SetVoucherOutEnabled(true);
				varConfig.SetTicketBufferingEnabled(false);
				const CConfig &config(varConfig);
				tickets.ConfigSettings(config);
			}
			ASSERT_TRUE(tickets.GetMaxTicketOutPending() == 1) << "IsTrue tickets.GetMaxTicketOutPending() == 1";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::ConfigSettings()
		///     CTickets::GetMaxTicketOutPending()
		/// Test with m_maxTicketsOutPending == 30.
		/// </summary>
		TEST(TicketsUnitTests, TicketConfigSettingsMax)
		{
			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetTicketingEnabled(false);
			const CConfig &config(varConfig);
			CTickets tickets(config);
			ASSERT_TRUE(tickets.GetMaxTicketOutPending() == 0) << "IsTrue tickets.GetMaxTicketOutPending() == 0";

			// Test.
			{
				CConfig varConfig(false, "");
				varConfig.SetTicketingEnabled(true);
				varConfig.SetVoucherOutEnabled(true);
				varConfig.SetTicketBufferingEnabled(true);
				const CConfig &config(varConfig);
				tickets.ConfigSettings(config);
			}
			ASSERT_TRUE(tickets.GetMaxTicketOutPending() == 30) << "IsTrue tickets.GetMaxTicketOutPending() == 30";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::GetMaxTicketOutPending()
		/// Test with m_maxTicketsOutPending == 0.
		/// </summary>
		TEST(TicketsUnitTests, TicketGetMaxTicketOutPending0)
		{
			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetTicketingEnabled(false);
			const CConfig &config(varConfig);
			CTickets tickets(config);

			// Test.
			ASSERT_TRUE(tickets.GetMaxTicketOutPending() == 0) << "IsTrue tickets.GetMaxTicketOutPending() == 0";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::GetMaxTicketOutPending()
		/// Test with m_maxTicketsOutPending == 1.
		/// </summary>
		TEST(TicketsUnitTests, TicketGetMaxTicketOutPending1)
		{
			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetTicketingEnabled(true);
			varConfig.SetVoucherOutEnabled(true);
			varConfig.SetTicketBufferingEnabled(false);
			const CConfig &config(varConfig);
			CTickets tickets(config);

			// Test.
			ASSERT_TRUE(tickets.GetMaxTicketOutPending() == 1) << "IsTrue tickets.GetMaxTicketOutPending() == 1";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::GetMaxTicketOutPending()
		/// Test with m_maxTicketsOutPending == 30.
		/// </summary>
		TEST(TicketsUnitTests, TicketGetMaxTicketOutPendingMax)
		{
			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetTicketingEnabled(true);
			varConfig.SetVoucherOutEnabled(true);
			varConfig.SetTicketBufferingEnabled(true);
			const CConfig &config(varConfig);
			CTickets tickets(config);

			// Test.
			ASSERT_TRUE(tickets.GetMaxTicketOutPending() == 30) << "IsTrue tickets.GetMaxTicketOutPending() == 30";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::HasMaxTicketOutPending()
		/// Test with m_maxTicketsOutPending == 0.
		/// </summary>
		TEST(TicketsUnitTests, TicketHasMaxTicketOutPending0)
		{
			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetTicketingEnabled(false);
			const CConfig &config(varConfig);
			CTickets tickets(config);

			// Test.
			tickets.SetNumberOfTicketsPending(INT_MIN);
			ASSERT_FALSE(tickets.HasMaxTicketOutPending()) << "IsFalse tickets.HasMaxTicketOutPending() -- INT_MIN";
			tickets.SetNumberOfTicketsPending(-1);
			ASSERT_FALSE(tickets.HasMaxTicketOutPending()) << "IsFalse tickets.HasMaxTicketOutPending() -- -1";
			tickets.SetNumberOfTicketsPending(0);
			ASSERT_TRUE(tickets.HasMaxTicketOutPending()) << "IsTrue tickets.HasMaxTicketOutPending() -- 0";
			tickets.SetNumberOfTicketsPending(1);
			ASSERT_TRUE(tickets.HasMaxTicketOutPending()) << "IsTrue tickets.HasMaxTicketOutPending() -- 1";
			tickets.SetNumberOfTicketsPending(INT_MAX);
			ASSERT_TRUE(tickets.HasMaxTicketOutPending()) << "IsTrue tickets.HasMaxTicketOutPending() -- INT_MAX";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::HasMaxTicketOutPending()
		/// Test with m_maxTicketsOutPending == 1.
		/// </summary>
		TEST(TicketsUnitTests, TicketHasMaxTicketOutPending1)
		{
			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetTicketingEnabled(true);
			varConfig.SetVoucherOutEnabled(true);
			varConfig.SetTicketBufferingEnabled(false);
			const CConfig &config(varConfig);
			CTickets tickets(config);

			// Test.
			tickets.SetNumberOfTicketsPending(INT_MIN);
			ASSERT_FALSE(tickets.HasMaxTicketOutPending()) << "IsFalse tickets.HasMaxTicketOutPending() -- INT_MIN";
			tickets.SetNumberOfTicketsPending(0);
			ASSERT_FALSE(tickets.HasMaxTicketOutPending()) << "IsFalse tickets.HasMaxTicketOutPending() -- 0";
			tickets.SetNumberOfTicketsPending(1);
			ASSERT_TRUE(tickets.HasMaxTicketOutPending()) << "IsTrue tickets.HasMaxTicketOutPending() -- 1";
			tickets.SetNumberOfTicketsPending(2);
			ASSERT_TRUE(tickets.HasMaxTicketOutPending()) << "IsTrue tickets.HasMaxTicketOutPending() -- 2";
			tickets.SetNumberOfTicketsPending(INT_MAX);
			ASSERT_TRUE(tickets.HasMaxTicketOutPending()) << "IsTrue tickets.HasMaxTicketOutPending() -- INT_MAX";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::HasMaxTicketOutPending()
		/// Test with m_maxTicketsOutPending == 30.
		/// </summary>
		TEST(TicketsUnitTests, TicketHasMaxTicketOutPendingMax)
		{
			// Setup.
			CConfig varConfig(false, "");
			varConfig.SetTicketingEnabled(true);
			varConfig.SetVoucherOutEnabled(true);
			varConfig.SetTicketBufferingEnabled(true);
			const CConfig &config(varConfig);
			CTickets tickets(config);

			// Test.
			tickets.SetNumberOfTicketsPending(INT_MIN);
			ASSERT_FALSE(tickets.HasMaxTicketOutPending()) << "IsFalse tickets.HasMaxTicketOutPending() -- INT_MIN";
			tickets.SetNumberOfTicketsPending(29);
			ASSERT_FALSE(tickets.HasMaxTicketOutPending()) << "IsFalse tickets.HasMaxTicketOutPending() -- 29";
			tickets.SetNumberOfTicketsPending(30);
			ASSERT_TRUE(tickets.HasMaxTicketOutPending()) << "IsTrue tickets.HasMaxTicketOutPending() -- 30";
			tickets.SetNumberOfTicketsPending(31);
			ASSERT_TRUE(tickets.HasMaxTicketOutPending()) << "IsTrue tickets.HasMaxTicketOutPending() -- 31";
			tickets.SetNumberOfTicketsPending(INT_MAX);
			ASSERT_TRUE(tickets.HasMaxTicketOutPending()) << "IsTrue tickets.HasMaxTicketOutPending() -- INT_MAX";
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CTickets::SetFinalStatusReceived()
		///     CTickets::WasFinalStatusReceived()
		/// </summary>
		TEST(TicketsUnitTests, TicketWasFinalStatusReceived)
		{
			const bool ticketInTrue(true);

			// Setup.
			const CConfig config(false, "");
			CTickets tickets(config);
			tickets.RemoveTicketIn();
			ASSERT_FALSE(tickets.DoesTicketInExist()) << "IsFalse tickets.DoesTicketInExist() -- setup";

			// Test.
			tickets.SetFinalStatusReceived();
			ASSERT_TRUE(tickets.WasFinalStatusReceived()) << "IsTrue tickets.WasFinalStatusReceived() -- test1";
			CTicket ticket;
			ticket.SetType(VoucherTicket, ticketInTrue);
			ASSERT_FALSE(tickets.StoreTicket(ticket)) << "IsFalse tickets.StoreTicket -- test";
			ASSERT_TRUE(tickets.DoesTicketInExist()) << "IsTrue tickets.DoesTicketInExist() -- test";
			ASSERT_FALSE(tickets.WasFinalStatusReceived()) << "IsFalse tickets.WasFinalStatusReceived() -- test2";
			tickets.SetFinalStatusReceived();
			ASSERT_TRUE(tickets.WasFinalStatusReceived()) << "IsTrue tickets.WasFinalStatusReceived() -- test3";
		}

		TEST(TicketsUnitTests, TicketsCompareTicketAmount)
		{
			CConfig config(false,"");
			CTickets *tickets = new CTickets(config);
			//Setup

			CTicket ticket;
			ticket.SetActionCode(IncorrectMachineId);
			const long long amount = 10000;
			ticket.SetAmount(amount);
			const byte Duration = 255;
			ticket.SetExpirationDuration(Duration);
			const TicketExpirationType ExpirationType = EXPIRES_MINUTES;
			ticket.SetExpirationType(ExpirationType);
			tm GameTm;
			GameTm.tm_sec = 2;
			GameTm.tm_min = 2;
			GameTm.tm_hour = 1;
			GameTm.tm_mday = 5;
			GameTm.tm_mon = 11;
			GameTm.tm_year = 2021;

			time_t time = mktime(&GameTm);
			ticket.SetGameDateTime(time);
			ticket.SetID(10010);
			const int MachineNumber = 10999;
			ticket.SetMachineNumber(MachineNumber);
			const TicketResultCode ResultCode = ActionCodeNotSupported;
			ticket.SetResultCode(ResultCode);
			const WORD SequenceNumber = 1234;
			ticket.SetSequenceNumber(SequenceNumber);
			const int TicketNumber = 12345678;
			ticket.SetTicketNumber(TicketNumber);
			const TicketType Type = VoucherTicket;
			ticket.SetType(Type, true); // ticket in
			byte *validationNumber = new byte[VALIDATION_NUMBER_LENGTH];
			memset(validationNumber, 255, VALIDATION_NUMBER_LENGTH);
			ticket.SetValidationNumber(validationNumber, VALIDATION_NUMBER_LENGTH);

			tickets->StoreTicket(ticket);

			const long long amount2 = amount;
			CTicket *ticket2 = new CTicket();
			ticket2->SetActionCode(IncorrectMachineId);
			ticket2->SetAmount(amount2);
			ticket2->SetExpirationDuration(Duration);
			ticket2->SetExpirationType(ExpirationType);
			ticket2->SetGameDateTime(time);
			ticket2->SetID(10010);
			ticket2->SetMachineNumber(MachineNumber);
			ticket2->SetResultCode(ResultCode);
			ticket2->SetSequenceNumber(SequenceNumber);
			ticket2->SetTicketNumber(TicketNumber);
			ticket2->SetType(Type, true); // ticket in
			ticket2->SetValidationNumber(validationNumber, VALIDATION_NUMBER_LENGTH);

			// Same amount
			ASSERT_TRUE(tickets->CompareTicketInAmount(ticket2));

			ticket2->SetAmount(amount2 + 1);
			ASSERT_FALSE(tickets->CompareTicketInAmount(ticket2));

			delete ticket2;
			delete tickets;
			delete validationNumber;


		}
}
