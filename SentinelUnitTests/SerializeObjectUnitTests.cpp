#include "stdafx.h"
#include "PersonalBankerTransfer.h"
#include "Ticket.h"



#include "gtest/gtest.h"


namespace SentinelUnitTests
{
		TEST(SerializeUnitTests, SerializePersonalBankerTransferTest)
		{
			BYTE OCR[CARD_ID_LEN] = { 0xaa, 0x09, 0x15, 0xc0, 0xf8 };
			const char id[14] = "ATI1234567890";
            time_t cardInTime(CUtilities::GetTimeFromFields(2014, 01, 23, 14, 8, 0));

			CPersonalBankerTransfer *pbt = new CPersonalBankerTransfer(1500);

			pbt->SetID(id, 14);
			pbt->SetCardNumber(OCR, CARD_ID_LEN);
			pbt->SetEFTNumber(123);
			pbt->SetRestrictedSequenceNumber(1234);
			pbt->SetNonrestrictedSequenceNumber(5678);
			pbt->SetSequenceNumber(3456);
			pbt->SetPersonalBankerType(Unspecified);
			pbt->SetPromoPersonalBankerType(PromoCreditsToCash);
			pbt->SetPersonalBankerTransferType(Undeclared);
			pbt->SetPromoPersonalBankerTransferType(PromoDollarsToGame);
			pbt->SetRestrictedPointRedemption(false);
			pbt->SetCEPOutEnabled(false);
			pbt->SetDenomination(1);
			pbt->SetOriginalCreditMeter(1000);
			pbt->SetAmount(1200);
			pbt->SetRestrictedAmount(2400);
			pbt->SetNonrestrictedAmount(3600);
			pbt->SetCancelledType(NoGameDenomination);
			pbt->SetPromoCancelledType(OverTransferLimit);
			pbt->SetUTCCardInTime(cardInTime);
			pbt->SetMinCashUp(1);
			pbt->SetMaxCashUp(3000);
			pbt->SetMinPromoUp(1);
			pbt->SetMaxPromoUp(3000);
			pbt->SetPBTSent(true);

			string result = pbt->ToStringForLog();
			string expected = "PBTId=ATI1234567890; CardNumber=AA0915C0F8; EFTNumber=123; RestrictedSequenceNumber=1234; NonRestrictedSequenceNumber=5678; \
SequenceNumber=3456; PBTSessionStartTicks=1500; PBType=Unspecified; PromoPBType=Unspecified; PBTransferType=Undeclared; PromoPBTransferType=PromoDollarsToGame; \
PointRedemptionRestricted=0; CEPOutEnabled=0; Denomination=1; OriginalCreditmeter=1000; AmountInPennies=1200; RestrictedAmountInPennies=2400; \
NonRestrictedAmountInPennies=3600; PollerCancellationType=OverTransferLimit; PromoCancelledType=OverTransferLimit; UTCCardInTime=01/23/2014 02:08:00 PM; \
MinimumCashUp=1; MaximumCashUp=3000; MinimumPromoUp=1, MaximumPromoUp=3000; PBTSent=1;";

			ASSERT_TRUE(result == expected);

			pbt->SetPersonalBankerType(UnexpectedCashFromGameErr);
			pbt->SetPromoPersonalBankerType(UnknownError);
			pbt->SetPersonalBankerTransferType(AllFromGame);
			pbt->SetPromoPersonalBankerTransferType(ExternalCredit);
			pbt->SetCancelledType(CreditTransferErrorInternalToPoller2);
			pbt->SetPromoCancelledType(CreditTransferErrorTimeout);
			pbt->SetPBTSent(false);

			expected = "PBTId=ATI1234567890; CardNumber=AA0915C0F8; EFTNumber=123; RestrictedSequenceNumber=1234; NonRestrictedSequenceNumber=5678; \
SequenceNumber=3456; PBTSessionStartTicks=1500; PBType=CanceledCashUpload; PromoPBType=Unspecified; PBTransferType=AllFromGame; PromoPBTransferType=ExternalCredit; \
PointRedemptionRestricted=0; CEPOutEnabled=0; Denomination=1; OriginalCreditmeter=1000; AmountInPennies=1200; RestrictedAmountInPennies=2400; \
NonRestrictedAmountInPennies=3600; PollerCancellationType=CreditTransferErrorTimeout; PromoCancelledType=CreditTransferErrorTimeout; UTCCardInTime=01/23/2014 02:08:00 PM; \
MinimumCashUp=1; MaximumCashUp=3000; MinimumPromoUp=1, MaximumPromoUp=3000; PBTSent=0;";

			result = pbt->ToStringForLog();

			delete pbt;

			ASSERT_TRUE(result == expected);

		}

		TEST(SerializeUnitTests, SerializeTicketTest)
		{
			// ticket with minimum enum values
			CTicket *ticket = new CTicket();
			ticket->SetTicketNumber(1234);
			ticket->SetAmount(57920);
			ticket->SetActionCode(TicketActionCodeNoValue);
			ticket->SetResultCode(Acknowledged);
			ticket->SetType(TicketTypeNoValue, true);
			ticket->SetMachineNumber(567);
			ticket->SetExpirationType(UNUSED);
			ticket->SetExpirationDuration(10);
			ticket->SetSequenceNumber(99);
			ticket->SetID(123456789);

			//Set validationNumber
			byte validationNumber[] = { 0x00, 0x71, 0x37, 0x17, 0x08, 0x50, 0x80, 0x96, 0x49 };
			ticket->SetValidationNumber(validationNumber, 9);

			// Set date
			ticket->SetGameDateTime(CUtilities::GetTimeFromFields(2014, 01, 28, 14, 30, 00));

			string result = ticket->ToStringForLog();
			string expected = "Number=1234; Amount=57920; TicketActionCode=TicketActionCodeNoValue; ResultCode=Acknowledged; TicketType=TicketTypeNoValue; TicketInOut=In; ValidationNumber=007137170850809649; \
ValidationNumberLength=9; MachineNumber=567; ExpirationType=UNUSED; expirationDuration=10; gameDateTime=01/28/2014 02:30:00 PM; SequenceNumber=99; ID=123456789";
			ASSERT_TRUE(result == expected);


			// ticket with maximum enum values
			ticket->SetActionCode(ActionCode192DeniedByDPU);
			ticket->SetResultCode(TicketResultCodeNoValue);
			ticket->SetType(SingleWinNoReceipt, false);
			result = ticket->ToStringForLog();
			expected = "Number=1234; Amount=57920; TicketActionCode=ActionCode192DeniedByDPU; ResultCode=TicketResultCodeNoValue; TicketType=SingleWinNoReceipt; TicketInOut=Out; ValidationNumber=007137170850809649; \
ValidationNumberLength=9; MachineNumber=567; ExpirationType=UNUSED; expirationDuration=10; gameDateTime=01/28/2014 02:30:00 PM; SequenceNumber=99; ID=123456789";
			ASSERT_TRUE(result == expected);

			delete ticket;

		}
}



