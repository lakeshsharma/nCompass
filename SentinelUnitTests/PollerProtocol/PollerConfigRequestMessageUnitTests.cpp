#include "stdafx.h"

#include "PollerProtocol/PollerConfigRequestMessage.h"

// Sentinel includes.
#include "Utilities.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
    TEST(PollerConfigRequestMessage, FeaturesFlag)
    {
		CPollerMessage *pollerMsg = new CPollerConfigRequestMessage();
        long bufferSize = 0;

        byte *buffer = pollerMsg->GetReadWriteBuffer(bufferSize);

        byte flagOffset = 22;
        DWORD featuresFlag = 0;
        memcpy(&featuresFlag, buffer + flagOffset, sizeof( featuresFlag ));

        ASSERT_EQ( featuresFlag & FF_ATOMIC_REDEMPTIONS, FF_ATOMIC_REDEMPTIONS );
        ASSERT_EQ( featuresFlag & FF_JACKPOT_EXTERNAL_BONUS, FF_JACKPOT_EXTERNAL_BONUS );
        ASSERT_EQ( featuresFlag & FF_EXTENDED_TICKET_EXPIRATION, FF_EXTENDED_TICKET_EXPIRATION );
        ASSERT_EQ( featuresFlag & FF_VOUCHER_SEPARATION, FF_VOUCHER_SEPARATION );
        ASSERT_EQ( featuresFlag & FF_MULTI_GAME_MULTI_DENOM, FF_MULTI_GAME_MULTI_DENOM );
        ASSERT_EQ( featuresFlag & FF_CHUNKED_GAMES_LIST, FF_CHUNKED_GAMES_LIST );
        ASSERT_EQ( featuresFlag & FF_WALLET_2_0_SUPPORTED, FF_WALLET_2_0_SUPPORTED);

    }
};
