#include "stdafx.h"
#include "SASProtocol/LongPoll71Message.h"

// Unit testing includes.
#include "UnitTestingUtils.h"

#include "gtest/gtest.h"
using namespace std;

namespace SentinelUnitTests
{
    class LongPoll71Message : public ::testing::Test
    {
    public: 

        bool compareBCD(byte *bcd1, byte *bcd2, int bcdLen)
        {
            bool same = true;
            for (int bcdi = 0; same && bcdi < bcdLen; bcdi++) 
            {
                same = (*bcd1++ == *bcd2++);
            }

            return same;
        }

    protected:
        virtual void SetUp() 
        {
        }

        virtual void TearDown() 
        {
        }
            
    };

    TEST_F(LongPoll71Message, CLongPoll71Message_FormatExpDate)
    {

        time_t currentDate = CUtilities::GetTimeFromFields(2018, 2, 12, 0, 0, 0);
        time_t ticketValidationDate = CUtilities::GetTimeFromFields(2018, 2, 11, 0, 0, 0);
		const int ExpireBCDSize = 4; 
        byte expireDateBCD[ExpireBCDSize];

        WORD expireDays = 60;
        CLongPoll71Message::FormatTicketExpire(ticketValidationDate, currentDate, expireDays, expireDateBCD, ExpireBCDSize);
        byte expectedBCD1[ExpireBCDSize] = {0x04, 0x12, 0x20, 0x18};
        ASSERT_TRUE(compareBCD(expireDateBCD, expectedBCD1, ExpireBCDSize)) << "Expected BCD 04122018";

        expireDays = 10;
        CLongPoll71Message::FormatTicketExpire(ticketValidationDate, currentDate, expireDays, expireDateBCD, ExpireBCDSize);
        byte expectedBCD2[ExpireBCDSize] = {0x02, 0x21, 0x20, 0x18};
        ASSERT_TRUE(compareBCD(expireDateBCD, expectedBCD2, ExpireBCDSize)) << "Expected BCD 02212018";

        expireDays = 10;
        ticketValidationDate = CUtilities::GetTimeFromFields(2018, 1, 11, 0, 0, 0);
        CLongPoll71Message::FormatTicketExpire(ticketValidationDate, currentDate, expireDays, expireDateBCD, ExpireBCDSize);
        byte expectedBCD3[ExpireBCDSize] = {0x01, 0x21, 0x20, 0x18};
        ASSERT_TRUE(compareBCD(expireDateBCD, expectedBCD3, ExpireBCDSize)) << "Expected BCD 01212018";

        expireDays = 9999;
        ticketValidationDate = currentDate;
        CLongPoll71Message::FormatTicketExpire(ticketValidationDate, currentDate, expireDays, expireDateBCD, ExpireBCDSize);
        byte expectedBCD4[ExpireBCDSize] = {0x00, 0x00, 0x99, 0x99};
        ASSERT_TRUE(compareBCD(expireDateBCD, expectedBCD4, ExpireBCDSize)) << "Expected BCD 00009999";

        expireDays = 366;
        ticketValidationDate = currentDate;
        CLongPoll71Message::FormatTicketExpire(ticketValidationDate, currentDate, expireDays, expireDateBCD, ExpireBCDSize);
        byte expectedBCD5[ExpireBCDSize] = {0x00, 0x00, 0x03, 0x66};
        ASSERT_TRUE(compareBCD(expireDateBCD, expectedBCD5, ExpireBCDSize)) << "Expected BCD 00000366";
    }
}

