#include "stdafx.h"

#include "GlobalDefs.h"
#include "Ticket.h"
#include "Tickets.h"
#include "DirectoryManager.h"
#include "TicketPBTMaxAmount.h"

#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	const byte MEMSET_DEFAULT_VALUE = 0xcd;


		TEST(TicketUnitTests, TicketDefaultConstructor)
		{
			//Setup
			CTicket *ticket = new CTicket();

			//Tests
			ASSERT_TRUE( ticket->ActionCode() == TicketActionCodeNoValue) << "ActionCode incorrect" ;
			ASSERT_TRUE( ticket->Amount() == -1) << "Amount incorrect" ;
			ASSERT_TRUE( ticket->ExpirationDuration() == 1) << "ExpirationDuration incorrect" ;
			ASSERT_TRUE( ticket->ExpirationType() == EXPIRES_DAYS) << "ExpirationType incorrect" ;
			time_t DefaultDateTime;
			//ASSERT_TRUE( ticket->GameDateTime() == DefaultDateTime) << "GameDate incorrect" << ticket->GameDateTime() << " " << DefaultDateTime << " " << CUtilities::GetCurrentTime();
			ASSERT_TRUE( ticket->ID() == 0) << "Ticket ID incorrect" ;
			ASSERT_TRUE( ticket->MachineNumber() == -1) << "MachineNumber incorrect" ;
			ASSERT_TRUE( ticket->ResultCode() == TicketResultCodeNoValue) << "ResultCode incorrect" ;
			ASSERT_TRUE( ticket->SequenceNumber() == 0) << "SequenceNumber incorrect" ;
			ASSERT_TRUE( ticket->Number() == -1) << "TicketNumber incorrect" ;
			ASSERT_TRUE( ticket->Type() == TicketTypeNoValue) << "TicketType incorrect" ;
			ASSERT_TRUE( ticket->IsTicketIn()) << "Ticket-in incorrect" ;
			ASSERT_TRUE( ticket->ValidationLength() == 0) << "validation length should be 0" ;

			//Cleanup
			delete ticket;
		}

		TEST(TicketUnitTests, TicketSetGet)
		{
			//Setup
			CTicket *ticket = new CTicket();
			ticket->SetActionCode(IncorrectMachineId);
			const long long LargeAmount = 9223372036854775806;
			ticket->SetAmount( LargeAmount );
			const byte Duration = 255;
			ticket->SetExpirationDuration( Duration );
			const TicketExpirationType ExpirationType = EXPIRES_MINUTES;
			ticket->SetExpirationType( ExpirationType );
			tm gameTm;
			gameTm.tm_year=2021;
			gameTm.tm_mon= 11;
			gameTm.tm_mday= 5;
			gameTm.tm_hour= 1;
			gameTm.tm_min= 2;
			gameTm.tm_sec= 3 ;
			time_t GameDate = mktime(&gameTm);
			ticket->SetGameDateTime( GameDate );
			ticket->SetID( LargeAmount );
			const int MachineNumber = 10999;
			ticket->SetMachineNumber( MachineNumber );
			const TicketResultCode ResultCode = ActionCodeNotSupported;
			ticket->SetResultCode( ResultCode );
			const WORD SequenceNumber = 1234;
			ticket->SetSequenceNumber( SequenceNumber );
			const int TicketNumber = 12345678;
			ticket->SetTicketNumber( TicketNumber );
			const TicketType Type = CouponPromo;
			ticket->SetType( Type, false );
			byte *validationNumber = new byte[VALIDATION_NUMBER_LENGTH];
			memset(validationNumber, 255, VALIDATION_NUMBER_LENGTH);
			ticket->SetValidationNumber( validationNumber, VALIDATION_NUMBER_LENGTH );

			//Tests
			ASSERT_TRUE( ticket->ActionCode() == IncorrectMachineId) << "ActionCode incorrect" ;
			ASSERT_TRUE( ticket->Amount() == LargeAmount) << "Amount incorrect" ;
			ASSERT_TRUE( ticket->ExpirationDuration() == Duration) << "ExpirationDuration incorrect" ;
			ASSERT_TRUE( ticket->ExpirationType() == ExpirationType) << "ExpirationType incorrect" ;
			ASSERT_TRUE( ticket->GameDateTime() == GameDate) << "GameDate incorrect" ;
			ASSERT_TRUE( ticket->ID() == LargeAmount) << "Ticket ID incorrect" ;
			ASSERT_TRUE( ticket->MachineNumber() == MachineNumber) << "MachineNumber incorrect" ;
			ASSERT_TRUE( ticket->ResultCode() == ResultCode) << "ResultCode incorrect" ;
			ASSERT_TRUE( ticket->SequenceNumber() == SequenceNumber) << "SequenceNumber incorrect" ;
			ASSERT_TRUE( ticket->Number() == TicketNumber) << "TicketNumber incorrect" ;
			ASSERT_TRUE( ticket->Type() == Type) << "TicketType incorrect" ;
			ASSERT_FALSE( ticket->IsTicketIn()) << "Ticket-in incorrect" ;
			byte validationLength;
			ASSERT_TRUE( memcmp( validationNumber, ticket->ValidationNumber( validationLength ), VALIDATION_NUMBER_LENGTH ) == 0) << "ValidationNumber incorrect" ;
			ASSERT_TRUE( validationLength == VALIDATION_NUMBER_LENGTH) << "ValidationLength incorrect" ;

			//Cleanup
			delete ticket;
		}

		TEST(TicketUnitTests, TicketCopyConstructor)
		{
			//Setup
			CTicket *originalTicket = new CTicket();
			originalTicket->SetActionCode(IncorrectMachineId);
			const long long LargeAmount = 9223372036854775806;
			originalTicket->SetAmount( LargeAmount );
			const byte Duration = 255;
			originalTicket->SetExpirationDuration( Duration );
			const TicketExpirationType ExpirationType = EXPIRES_MINUTES;
			originalTicket->SetExpirationType( ExpirationType );
			tm gameTm;
			gameTm.tm_year=2021;
			gameTm.tm_mon= 11;
			gameTm.tm_mday= 5;
			gameTm.tm_hour= 1;
			gameTm.tm_min= 2;
			gameTm.tm_sec= 3 ;
			time_t GameDate = mktime(&gameTm);
			originalTicket->SetGameDateTime( GameDate );
			originalTicket->SetID( LargeAmount );
			const int MachineNumber = 10999;
			originalTicket->SetMachineNumber( MachineNumber );
			const TicketResultCode ResultCode = ActionCodeNotSupported;
			originalTicket->SetResultCode( ResultCode );
			const WORD SequenceNumber = 1234;
			originalTicket->SetSequenceNumber( SequenceNumber );
			const int TicketNumber = 12345678;
			originalTicket->SetTicketNumber( TicketNumber );
			const TicketType Type = CouponPromo;
			originalTicket->SetType( Type, false );
			byte *validationNumber = new byte[VALIDATION_NUMBER_LENGTH];
			memset(validationNumber, 255, VALIDATION_NUMBER_LENGTH);
			originalTicket->SetValidationNumber( validationNumber, VALIDATION_NUMBER_LENGTH );

			CTicket *ticket = new CTicket(*originalTicket);

			//Tests
			ASSERT_TRUE( ticket->ActionCode() == IncorrectMachineId) << "ActionCode incorrect" ;
			ASSERT_TRUE( ticket->Amount() == LargeAmount) << "Amount incorrect" ;
			ASSERT_TRUE( ticket->ExpirationDuration() == Duration) << "ExpirationDuration incorrect" ;
			ASSERT_TRUE( ticket->ExpirationType() == ExpirationType) << "ExpirationType incorrect" ;
			ASSERT_TRUE( ticket->GameDateTime() == GameDate) << "GameDate incorrect" ;
			ASSERT_TRUE( ticket->ID() == LargeAmount) << "Ticket ID incorrect" ;
			ASSERT_TRUE( ticket->MachineNumber() == MachineNumber) << "MachineNumber incorrect" ;
			ASSERT_TRUE( ticket->ResultCode() == ResultCode) << "ResultCode incorrect" ;
			ASSERT_TRUE( ticket->SequenceNumber() == SequenceNumber) << "SequenceNumber incorrect" ;
			ASSERT_TRUE( ticket->Number() == TicketNumber) << "TicketNumber incorrect" ;
			ASSERT_TRUE( ticket->Type() == Type) << "TicketType incorrect" ;
			ASSERT_FALSE( ticket->IsTicketIn()) << "Ticket-in incorrect" ;
			byte validationLength;
			ASSERT_TRUE( memcmp( validationNumber, ticket->ValidationNumber( validationLength ), VALIDATION_NUMBER_LENGTH ) == 0) << "ValidationNumber incorrect" ;
			ASSERT_TRUE( validationLength == VALIDATION_NUMBER_LENGTH) << "ValidationLength incorrect" ;

			//Cleanup
			delete ticket;
			delete originalTicket;
		}

		TEST(TicketUnitTests, TicketTooLongValidationNumber)
		{
			//Setup
			CTicket *ticket = new CTicket();
			byte *validationNumber = new byte[VALIDATION_NUMBER_LENGTH+10];
			memset(validationNumber, 255, VALIDATION_NUMBER_LENGTH+10);
			ticket->SetValidationNumber(validationNumber, VALIDATION_NUMBER_LENGTH+10);

			//Tests
			byte validationLength;
			ASSERT_TRUE( memcmp( validationNumber, ticket->ValidationNumber( validationLength ), VALIDATION_NUMBER_LENGTH ) == 0) << "ValidationNumber incorrect" ;
			ASSERT_TRUE( validationLength == VALIDATION_NUMBER_LENGTH) << "ValidationLength incorrect" ;

			//Cleanup
			delete ticket;
		}

		TEST(TicketUnitTests, SetValidationNumberNull)
		{
			const byte validationNumber1[] = { 0x12, 0x34 };
			const byte validationLength1((byte)sizeof(validationNumber1));
			const byte validationLength1Expected(validationLength1);

			const byte *validationNumber2(NULL);
			const byte validationLength2(33);
			const byte validationLength2Expected(0);

			// Setup.
			CTicket ticket;
			ticket.SetValidationNumber(validationNumber1, validationLength1);
			{
				byte validationLengthResult;
				const byte *validationNumberResult = ticket.ValidationNumber(validationLengthResult);
				ASSERT_TRUE(validationLengthResult == validationLength1Expected && 0 == memcmp(validationNumberResult, validationNumber1, validationLength1Expected)) << "Setup 1.";
			}

			ticket.SetValidationNumber(validationNumber2, validationLength2);
			{
				byte validationLengthResult;
				const byte *validationNumberResult = ticket.ValidationNumber(validationLengthResult);
				ASSERT_TRUE(validationLengthResult == validationLength2Expected && 0 == memcmp(validationNumberResult, validationNumber2, validationLength2Expected)) << "Test 1.";
			}
		}

		TEST(TicketUnitTests, TicketCopy)
		{
			const byte validationNumber[] = { 0x12, 0x34 };
			const bool ticketInTrue(true);
			const bool ticketInFalse(false);

			// Setup.
			CTicket ticket1;
			ticket1.SetType(VoucherTicket, ticketInFalse);
			ticket1.SetValidationNumber(validationNumber, sizeof(validationNumber));
			ticket1.SetAmount(1);
			ticket1.SetGameDateTime(CUtilities::GetCurrentTime());
			CTicket ticket2(ticket1);

			// Test.
			ASSERT_TRUE(ticket1 == ticket2) << "Test 1.";
			ASSERT_FALSE(ticket1 != ticket2) << "Test 2.";
		}

		TEST(TicketUnitTests, TicketAssign)
		{
			const byte validationNumber[] = { 0x12, 0x34 };
			const bool ticketInTrue(true);
			const bool ticketInFalse(false);

			// Setup.
			CTicket ticket1;
			ticket1.SetType(VoucherTicket, ticketInFalse);
			ticket1.SetValidationNumber(validationNumber, sizeof(validationNumber));
			ticket1.SetAmount(1);
			ticket1.SetGameDateTime(CUtilities::GetCurrentTime());
			CTicket ticket2;
			ticket2 = ticket1;

			// Test.
			ASSERT_TRUE(ticket1 == ticket2) << "Test 1.";
			ASSERT_FALSE(ticket1 != ticket2) << "Test 2.";
		}

		TEST(TicketUnitTests, TicketCompareNotEqualAmount)
		{
			const __int64 amount1(123);
			const __int64 amount2(1234);

			// Setup.
			CTicket ticket1;
			ticket1.SetAmount(amount1);
			CTicket ticket2(ticket1);

			// Test.
			ASSERT_FALSE(ticket1 != ticket2) << "Test 1.";

			ticket2.SetAmount(amount2); // Change.
			ASSERT_TRUE(ticket1 != ticket2) << "Test 2.";

			ticket2.SetAmount(amount1); // Restore.
			ASSERT_FALSE(ticket1 != ticket2) << "Test 3.";
		}

		// this.IsTicketIn() does affect operator==() for amount.
		TEST(TicketUnitTests, TicketCompareEqualAmount)
		{
			const bool ticketInTrue(true);
			const bool ticketInFalse(false);
			const __int64 amount1(123);
			const __int64 amount2(1234);

			// Setup.
			CTicket ticket1;
			ticket1.SetType(VoucherTicket, ticketInFalse);
			ticket1.SetAmount(amount1);
			CTicket ticket2(ticket1);

			// Test.
			ASSERT_TRUE(ticket1 == ticket2) << "Test 1.";

			ticket1.SetType(CouponPromo, ticketInTrue); // Change Type. No effect.
			ASSERT_TRUE(ticket1 == ticket2) << "Test 2.";

			ticket2.SetAmount(amount2); // Change amount. No effect.
			ASSERT_TRUE(ticket1 == ticket2) << "Test 3.";

			ticket1.SetType(VoucherTicket, ticketInFalse); // Restore Type. Amount still changed.
			ASSERT_FALSE(ticket1 == ticket2) << "Test 4.";

			ticket2.SetAmount(amount1); // Restore.
			ASSERT_TRUE(ticket1 == ticket2) << "Test 5.";
		}

		TEST(TicketUnitTests, TicketCompareNotEqualGameDateTime)
		{
			time_t gameDateTime1 = CUtilities::GetCurrentTime();
			time_t gameDateTime2 = CUtilities::GetCurrentTime()+5;

			// Setup.
			CTicket ticket1;
			ticket1.SetGameDateTime(gameDateTime1);
			CTicket ticket2(ticket1);

			// Test.
			ASSERT_FALSE(ticket1 != ticket2) << "Test 1.";

			ticket2.SetGameDateTime(gameDateTime2); // Change.
			ASSERT_TRUE(ticket1 != ticket2) << "Test 2.";

			ticket2.SetGameDateTime(gameDateTime1); // Restore.
			ASSERT_FALSE(ticket1 != ticket2) << "Test 3.";
		}

		// this.IsTicketIn() does affect operator==() for game date time.
		TEST(TicketUnitTests, TicketCompareEqualGameDateTime)
		{
			const bool ticketInTrue(true);
			const bool ticketInFalse(false);
            time_t gameDateTime1 = CUtilities::GetCurrentTime();
            time_t gameDateTime2 = CUtilities::GetCurrentTime()+5;


			// Setup.
			CTicket ticket1;
			ticket1.SetType(VoucherTicket, ticketInFalse);
			ticket1.SetGameDateTime(gameDateTime1);
			CTicket ticket2(ticket1);

			// Test.
			ASSERT_TRUE(ticket1 == ticket2) << "Test 1.";

			ticket1.SetType(CouponPromo, ticketInTrue); // Change Type. No effect.
			ASSERT_TRUE(ticket1 == ticket2) << "Test 2.";

			ticket2.SetGameDateTime(gameDateTime2); // Change game date time. No effect.
			ASSERT_TRUE(ticket1 == ticket2) << "Test 3.";

			ticket1.SetType(VoucherTicket, ticketInFalse); // Restore Type. Game date time still changed.
			ASSERT_FALSE(ticket1 == ticket2) << "Test 4.";

			ticket2.SetGameDateTime(gameDateTime1); // Restore.
			ASSERT_TRUE(ticket1 == ticket2) << "Test 5.";
		}

		TEST(TicketUnitTests, TicketCompareNotEqualValidationNumber)
		{
			const byte validationNumber1[] = { 0x12, 0x34 };
			const byte validationNumber2[] = { 0x56, 0x78 };
			const byte validationNumber3[] = { 0x12, 0x34, 0x56 }; // Different lengths have different comparision logic.

			// Setup.
			CTicket ticket1;
			ticket1.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			CTicket ticket2(ticket1);

			// Test.
			ASSERT_FALSE(ticket1 != ticket2) << "Test 1.";

			ticket2.SetValidationNumber(validationNumber2, sizeof(validationNumber2)); // Change.
			ASSERT_TRUE(ticket1 != ticket2) << "Test 2.";

			ticket2.SetValidationNumber(validationNumber3, sizeof(validationNumber3)); // Change.
			ASSERT_TRUE(ticket1 != ticket2) << "Test 3.";

			ticket2.SetValidationNumber(validationNumber1, sizeof(validationNumber1)); // Restore.
			ASSERT_FALSE(ticket1 != ticket2) << "Test 4.";
		}

		// IsTicketIn() does not affect operator==() for validation number.
		TEST(TicketUnitTests, TicketCompareEqualValidationNumber)
		{
			const byte validationNumber1[] = { 0x12, 0x34 };
			const byte validationNumber2[] = { 0x56, 0x78 };
			const byte validationNumber3[] = { 0x12, 0x34, 0x56 }; // Different lengths have different comparision logic.

			// Setup.
			CTicket ticket1;
			ticket1.SetValidationNumber(validationNumber1, sizeof(validationNumber1));
			CTicket ticket2(ticket1);

			// Test.
			ASSERT_TRUE(ticket1 == ticket2) << "Test 1.";

			ticket2.SetValidationNumber(validationNumber2, sizeof(validationNumber2)); // Change.
			ASSERT_FALSE(ticket1 == ticket2) << "Test 2.";

			ticket2.SetValidationNumber(validationNumber3, sizeof(validationNumber3)); // Change.
			ASSERT_FALSE(ticket1 == ticket2) << "Test 3.";

			ticket2.SetValidationNumber(validationNumber1, sizeof(validationNumber1)); // Restore.
			ASSERT_TRUE(ticket1 == ticket2) << "Test 4.";
		}

		/// <summary>
		/// Code coverage test for:
		///     CTicket::CTicket()
		///     CTicket::GetNvramBuffer()
		/// Code coverage:
		///     CTicket::~CTicket()
		/// Test with a NULL buffer.
		/// </summary>
		TEST(TicketUnitTests, GetNvramBuffer_Null)
		{
			const int nvramBufferLength = 128;

			typedef struct
			{
				// Inputs.
				const int m_nvramBufferLengthInput;

				// Expected results.
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				-33,
				0,
				1,
				2,
				nvramBufferLength - 2,
				nvramBufferLength - 1,
				nvramBufferLength,
				nvramBufferLength + 1,
			};

			// Outputs -- same for all iterations.
			const int nvramBufferLengthExpected(0);
			byte * const nvramBuffer(NULL);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup ticket.
				const CTicket ticket;

				// Perform operation(s) to be tested.
				const int nvramBufferLengthResult = ticket.GetNvramBuffer(nvramBuffer, TEST_DATA[testIndex].m_nvramBufferLengthInput);

				// Test #1.
				{
					const bool success(nvramBufferLengthResult == nvramBufferLengthExpected);
					if (!success)
					{
						std::stringstream failedMsg;
						failedMsg << "nvramBufferLengthResult=" << (int)nvramBufferLengthResult
                                  << " expected=" << (int)nvramBufferLengthExpected
                                  << " testIndex=" << (int)testIndex
                                  << " in " << (__TFUNCTION__)
                                  << "(" << (int)__LINE__ << ")";
						ASSERT_TRUE(success) << failedMsg.str();
					}
				}
			}
		}

		/// <summary>
		/// Code coverage test for:
		///     CTicket::CTicket()
		///     CTicket::GetNvramBuffer()
		/// Code coverage:
		///     CTicket::~CTicket()
		/// Test that buffer is not changed.
		/// </summary>
		TEST(TicketUnitTests, GetNvramBuffer_NoChange)
		{
			// Allocate extra memory before and after target buffer to check for unwanted overwrites.
			const UINT64 HeaderValue = 0xcdcdcdcdcdcdcdcd; // Each byte the same as MEMSET_DEFAULT_VALUE; 
			const size_t HeaderSize = sizeof(HeaderValue);
			const UINT64 TrailerValue = HeaderValue;
			const size_t TrailerSize = sizeof(TrailerValue);
			const int nvramBufferLength = 128;

			typedef struct
			{
				// Inputs.
				const int m_nvramBufferLengthInput;

				// Expected results.
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				-33,
				0,
				1,
				2,
				nvramBufferLength - 2,
				nvramBufferLength - 1,
			};

			// Outputs -- same for all iterations.
			const int nvramBufferLengthExpected(0);
			const byte nvramBufferExpected[nvramBufferLength] = {
				0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
				0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
				0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
				0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
				0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
				0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
				0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
				0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd };

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup ticket.
				const CTicket ticket;

				// Setup nvramBuffer.
				size_t bufferRawSize = HeaderSize + (unsigned)nvramBufferLength + TrailerSize;
				byte *bufferRaw = new byte[bufferRawSize];
				memset(bufferRaw, MEMSET_DEFAULT_VALUE, bufferRawSize);
				byte *nvramBuffer = bufferRaw + HeaderSize; // Target buffer.

				// Perform operation(s) to be tested.
				const int nvramBufferLengthResult = ticket.GetNvramBuffer(nvramBuffer, TEST_DATA[testIndex].m_nvramBufferLengthInput);

				// Test #1.
				{
					const bool success(nvramBufferLengthResult == nvramBufferLengthExpected);
					if (!success)
					{
            			std::stringstream failedMsg;
            			failedMsg << "nvramBufferLengthResult=" << (int)nvramBufferLengthResult
                                              << " expected=" << (int)nvramBufferLengthExpected
                                              << " testIndex=" << (int)testIndex
                                              << " in " << (__TFUNCTION__)
                                              << "(" << (int)__LINE__ << ")";
						ASSERT_TRUE(success) << failedMsg.str();
					}
				}

				// Test #2.
				{
					 // Verify that the header was not overwritten.
					const int headerMemcmpResult = memcmp(bufferRaw, &HeaderValue, HeaderSize);

					bool areEqual(headerMemcmpResult == MEMCMP_EQUAL);
					std::stringstream failedMsg;
					const string headerStr = CUtilities::ToHexString(bufferRaw, HeaderSize);
					const string expectedStr = CUtilities::ToHexString((const byte *)&HeaderValue, HeaderSize);
					failedMsg << "FAILED headerMemcmpResult=" << (int)headerMemcmpResult
                        << " expected=" << (int)MEMCMP_EQUAL
                        << "header=[" << (headerStr) << "]"
                        << " expected=[" << (expectedStr) << "]"
                        << " testIndex=" << (int)testIndex
                        << "in "<< (__TFUNCTION__) ;
					ASSERT_TRUE(areEqual) << failedMsg.str();
				}

				// Test #3.
				{
					 // Verify that the trailer was not overwritten.
					const int trailerMemcmpResult = memcmp(bufferRaw + bufferRawSize - TrailerSize, &HeaderValue, HeaderSize);

					bool areEqual(trailerMemcmpResult == MEMCMP_EQUAL);
					std::stringstream failedMsg;
					const string trailerStr = CUtilities::ToHexString(bufferRaw + bufferRawSize - TrailerSize, HeaderSize);
					const string expectedStr = CUtilities::ToHexString((const byte *)&HeaderValue, HeaderSize);
                    failedMsg << "FAILED trailerMemcmpResult=" << (int)trailerMemcmpResult
                        << " expected=" << (int)MEMCMP_EQUAL
                        << "trailer=[" << (trailerStr) << "]"
                        << " expected=[" << (expectedStr) << "]"
                        << " testIndex=" << (int)testIndex
                        << "in "<< (__TFUNCTION__) ;
                    ASSERT_TRUE(areEqual) << failedMsg.str();
 				}

				// Test #4.
				{
					 // Verify that the target buffer is as expected.
					const int bufferMemcmpResult = memcmp(nvramBuffer, nvramBufferExpected, nvramBufferLength);

					bool areEqual(bufferMemcmpResult == MEMCMP_EQUAL);
					std::stringstream failedMsg;
					const string bufferStr = CUtilities::ToHexString(nvramBuffer, nvramBufferLength);
					const string expectedStr = CUtilities::ToHexString(nvramBufferExpected, sizeof(nvramBufferExpected));
                    failedMsg << "FAILED bufferMemcmpResult=" << (int)bufferMemcmpResult
                        << " expected=" << (int)MEMCMP_EQUAL
                        << "nvramBuffer=[" << (bufferStr) << "]"
                        << " expected=[" << (expectedStr) << "]"
                        << " testIndex=" << (int)testIndex
                        << "in "<< (__TFUNCTION__) ;
                    ASSERT_TRUE(areEqual) << failedMsg.str();
				}

				// Cleanup.
				delete bufferRaw;
			}
		}

		/// <summary>
		/// Code coverage test for:
		///     CTicketLog::CTicketLog(const CTicket)
		///     CTicket::CTicket()
		///     CTicketLog::GetNvramBuffer()
		/// Code coverage:
		///     CTicketLog::~CTicketLog()
		/// Test for correct values when initialized with a CTicket instance.
		/// </summary>
		TEST(TicketUnitTests, GetNvramBuffer)
		{
			// Allocate extra memory before and after target buffer to check for unwanted overwrites.
			const UINT64 HeaderValue = 0xcdcdcdcdcdcdcdcd; // Each byte the same as MEMSET_DEFAULT_VALUE; 
			const size_t HeaderSize = sizeof(HeaderValue);
			const UINT64 TrailerValue = HeaderValue;
			const size_t TrailerSize = sizeof(TrailerValue);
			const int nvramBufferLength = 128;
			const int nvramBufferLengthGot = 87; // GetNvramBuffer() only sets 87 bytes, but we initialize all 128, so it is okay to verify all 128.

			const byte nvramBufferExpected[nvramBufferLength] = {
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb1, 0x07, 0x00, 0x00, 
				0x0c, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
				0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 
				0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd };

			// Outputs -- same for all iterations.
			const int testIndex(0);
			const int nvramBufferLengthExpected = nvramBufferLengthGot;

			// Setup ticket.
			const CTicket ticket;

			// Setup nvramBuffer.
			size_t bufferRawSize = HeaderSize + (unsigned)nvramBufferLength + TrailerSize;
			byte *bufferRaw = new byte[bufferRawSize];
			memset(bufferRaw, MEMSET_DEFAULT_VALUE, bufferRawSize);
			byte *nvramBuffer = bufferRaw + HeaderSize; // Target buffer.

			// Perform operation(s) to be tested.
			const int nvramBufferLengthResult = ticket.GetNvramBuffer(nvramBuffer, nvramBufferLength);

			// Test #1.
			{
				const bool success(nvramBufferLengthResult == nvramBufferLengthExpected);
				if (!success)
				{
        			std::stringstream failedMsg;
        			failedMsg << "nvramBufferLengthResult=" << (int)nvramBufferLengthResult
                                          << " expected=" << (int)nvramBufferLengthExpected
                                          << " testIndex=" << (int)testIndex
                                          << " in " << (__TFUNCTION__)
                                          << "(" << (int)__LINE__ << ")";
                    ASSERT_TRUE(success) << failedMsg.str();
				}
			}

			// Test #2.
			{
					// Verify that the header was not overwritten.
				const int headerMemcmpResult = memcmp(bufferRaw, &HeaderValue, HeaderSize);

				bool areEqual(headerMemcmpResult == MEMCMP_EQUAL);
				std::stringstream failedMsg;
				const string headerStr = CUtilities::ToHexString(bufferRaw, HeaderSize);
				const string expectedStr = CUtilities::ToHexString((const byte *)&HeaderValue, HeaderSize);

    		   failedMsg << "FAILED headerMemcmpResult=" << (int)headerMemcmpResult
                            << " expected=" << (int)MEMCMP_EQUAL
                            << "header=[" << (headerStr) << "]"
                            << " expected=[" << (expectedStr) << "]"
                            << " testIndex=" << (int)testIndex
                            << "in "<< (__TFUNCTION__) ;
    		   ASSERT_TRUE(areEqual) << failedMsg.str();

			}

			// Test #3.
			{
					// Verify that the trailer was not overwritten.
				const int trailerMemcmpResult = memcmp(bufferRaw + bufferRawSize - TrailerSize, &HeaderValue, HeaderSize);

				bool areEqual(trailerMemcmpResult == MEMCMP_EQUAL);
				std::stringstream failedMsg;
				const string trailerStr = CUtilities::ToHexString(bufferRaw + bufferRawSize - TrailerSize, HeaderSize);
				const string expectedStr = CUtilities::ToHexString((const byte *)&HeaderValue, HeaderSize);
                failedMsg << "FAILED trailerMemcmpResult=" << (int)trailerMemcmpResult
                    << " expected=" << (int)MEMCMP_EQUAL
                    << "trailer=[" << (trailerStr) << "]"
                    << " expected=[" << (expectedStr) << "]"
                    << " testIndex=" << (int)testIndex
                    << "in "<< (__TFUNCTION__) ;
                ASSERT_TRUE(areEqual) << failedMsg.str();
			}

#if 0 // NVRAM buffer is in local time (i.e., it depends on the operating system(?) time zone) so skip this test.
			// Test #4.
			{
					// Verify that the target buffer is as expected.
				const int bufferMemcmpResult = memcmp(nvramBuffer, nvramBufferExpected, nvramBufferLength);

				bool areEqual(bufferMemcmpResult == MEMCMP_EQUAL);
				string failedMsg;
				const string bufferStr = CUtilities::ToHexString(nvramBuffer, nvramBufferLength);
				const string expectedStr = CUtilities::ToHexString(nvramBufferExpected, sizeof(nvramBufferExpected));
				failedMsg.Format(_T("FAILED bufferMemcmpResult=%d expected=%d nvramBuffer=[%s] expected=[%s] testIndex=%d in %s"),
					(int)bufferMemcmpResult,
					(int)MEMCMP_EQUAL,
					(bufferStr),
					(expectedStr),
					(int)testIndex,
					(__TFUNCTION__));
				ASSERT_TRUE(areEqual, failedMsg);
			}
#endif

			// Test NVRAM buffer, but skip the CTime memory.
			const size_t nvramBufferLengthBefore = sizeof(int) + sizeof(__int64); // sizeof(m_number) + sizeof(m_amount).
 
			// Test #5.  Test NVRAM buffer portion that is before the CTime memory.
			{
				// Verify that the target buffer is as expected.
				const int bufferMemcmpResult = memcmp(nvramBuffer, nvramBufferExpected, nvramBufferLengthBefore);

				bool areEqual(bufferMemcmpResult == MEMCMP_EQUAL);
				std::stringstream  failedMsg;
				const string bufferStr = CUtilities::ToHexString(nvramBuffer, nvramBufferLengthBefore);
				const string expectedStr = CUtilities::ToHexString(nvramBufferExpected, nvramBufferLengthBefore);
                failedMsg << "FAILED bufferMemcmpResult=" << (int)bufferMemcmpResult
                    << " expected=" << (int)MEMCMP_EQUAL
                    << "nvramBuffer=[" << (bufferStr) << "]"
                    << " expected=[" << (expectedStr) << "]"
                    << " testIndex=" << (int)testIndex
                    << "in "<< (__TFUNCTION__) ;
                ASSERT_TRUE(areEqual) << failedMsg.str();
			}

			// Test #6.  Test NVRAM buffer portion that is after the CTime memory.
			{
				// Verify that the target buffer is as expected.
                const int bufferMemcmpResult = memcmp(nvramBuffer + nvramBufferLengthBefore + NVRAM_TIME_SIZE,
                                                      nvramBufferExpected + nvramBufferLengthBefore + NVRAM_TIME_SIZE,
                                                      nvramBufferLength - nvramBufferLengthBefore - NVRAM_TIME_SIZE);

                bool areEqual(bufferMemcmpResult == MEMCMP_EQUAL);
                std::stringstream  failedMsg;
                const string bufferStr = CUtilities::ToHexString(nvramBuffer + nvramBufferLengthBefore + NVRAM_TIME_SIZE,
                                                                 nvramBufferLength - nvramBufferLengthBefore - NVRAM_TIME_SIZE);
                const string expectedStr = CUtilities::ToHexString(nvramBufferExpected + nvramBufferLengthBefore + NVRAM_TIME_SIZE,
                                                                   sizeof(nvramBufferExpected) - nvramBufferLengthBefore - NVRAM_TIME_SIZE);
                failedMsg << "FAILED bufferMemcmpResult=" <<(int)bufferMemcmpResult
                << " expected=" <<(int)MEMCMP_EQUAL
                << "nvramBuffer+NVRAM_CTIME_SIZE=[" <<(bufferStr)<< "]"
                << " expected=[" <<(expectedStr)<< "]"
                << " testIndex=" <<(int)testIndex
                << "in "<<(__TFUNCTION__);
                ASSERT_TRUE(areEqual)<< failedMsg.str();
			}

			// Cleanup.
			delete bufferRaw;
		}

		/// <summary>
		/// Code coverage test for:
		///     CTicket::CTicket(const byte *)
		/// Code coverage:
		///     CTicket::~CTicket()
		/// Test for correct values when initialized from an NVRAM buffer.
		/// </summary>
		TEST(TicketUnitTests, CTicketLog_BufferTest)
		{
			// Inputs -- same for all loop iterations.
			const int nvramBufferLength = 128;
			const int nvramBufferLengthGot = 87; // 	CTicket( const byte *, ...) only reads 87 bytes, but it expects 128 bytes, so we provide 128.

			const byte nvramBuffer[nvramBufferLength] = {
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb1, 0x07, 0x00, 0x00, 
				0x0c, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
				0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 
				0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd };

			// Setup.

			// Perform operation(s) to be tested.
			const CTicket ticket(nvramBuffer);

			// Test.
			ASSERT_TRUE(ticket.ActionCode() == TicketActionCodeNoValue) << "ActionCode incorrect";
			ASSERT_TRUE(ticket.Amount() == -1) << "Amount incorrect" ;
			ASSERT_TRUE(ticket.ExpirationDuration() == 1) << "ExpirationDuration incorrect";
			ASSERT_TRUE(ticket.ExpirationType() == EXPIRES_DAYS) << "ExpirationType incorrect";
#if 0 // NVRAM buffer is in local time (i.e., it depends on the operating system(?) time zone), so skip this test.
			CTime gameDateTime(2 * 365 * 24 * 60 * 60);
			ASSERT_TRUE(ticket.GameDateTime() == gameDateTime) << "GameDate incorrect";
#endif
			ASSERT_TRUE(ticket.ID() == 0) << "Ticket ID incorrect";
			ASSERT_TRUE(ticket.MachineNumber() == -1) << "MachineNumber incorrect";
			ASSERT_TRUE(ticket.ResultCode() == TicketResultCodeNoValue) << "ResultCode incorrect";
			ASSERT_TRUE(ticket.SequenceNumber() == 0) << "SequenceNumber incorrect";
			ASSERT_TRUE(ticket.Number() == -1) << "TicketNumber incorrect";
			ASSERT_TRUE(ticket.Type() == TicketTypeNoValue) << "TicketType incorrect";
#if 0 // 2013-11-21 CTicket::CTicket(const byte *, int) leaves m_bTicketIn uninitialized, so skip this test.
			ASSERT_TRUE(ticket.IsTicketIn()) << "Ticket-in incorrect" ;
#endif
			ASSERT_TRUE(ticket.ValidationLength() == 0) << "validation length should be 0";
		}

		TEST(TicketUnitTests, TicketIsAmountLessThanMax)
		{
			CDirectoryManager::MockInstance();
			const long MAXTICKITAMOUNT = 100000;
			CTicket ticket;
			ticket.SetActionCode(IncorrectMachineId);
			const long long amount = 10000;
			ticket.SetAmount(amount);
			const byte Duration = 255;
			ticket.SetExpirationDuration(Duration);
			const TicketExpirationType ExpirationType = EXPIRES_MINUTES;
			ticket.SetExpirationType(ExpirationType);
			tm gameTm;
			gameTm.tm_year=2021;
			gameTm.tm_mon= 11;
			gameTm.tm_mday= 5;
			gameTm.tm_hour= 1;
			gameTm.tm_min= 2;
			gameTm.tm_sec= 3;
			time_t GameDate = mktime(&gameTm);
			ticket.SetGameDateTime(GameDate);
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

			ASSERT_TRUE(ticket.IsAmountLessThanMax());

			ticket.SetAmount(MAXTICKITAMOUNT);
			ASSERT_TRUE(ticket.IsAmountLessThanMax());

			// When CDirectoryManager is refactored enough to allow configuration files to be opened and parsed the following test can be activated
			//ticket.SetAmount(MAXTICKITAMOUNT + 1);
			//ASSERT_FALSE(ticket.IsAmountLessThanMax());

		}
}
