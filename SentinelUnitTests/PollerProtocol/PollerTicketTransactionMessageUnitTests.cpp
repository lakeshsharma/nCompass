#include "stdafx.h"
#include "PollerProtocol/PollerTicketTransactionMessage.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
    /// <summary>
    /// Functional test for:
    ///     CPollerTicketTransactionMessage::CPollerTicketTransactionMessage()
    ///     CPollerTicketTransactionMessage::GetPlaySessionID()
    /// Test Player session ID.
    TEST(PollerTicketTransactionMessage, GetPlaySessionID_ValueUpdate)
    {
        // Setup for CPollerTicketTransactionMessage.
        CMeters *meters = (new CMeters(MEMORY_NONE));
        CTicket  *ticket = (new CTicket());
        byte transType = TICKET_CODE;
        const byte * cardID = reinterpret_cast<const byte*>("1234567890");
        
        CPollerTicketTransactionMessage pollerMsg(meters, ticket, transType, 0, cardID);


        DWORD psid = 12;
        pollerMsg.SetPlaySessionID(psid);

        ASSERT_EQ(pollerMsg.GetPlaySessionID(), psid);
    }

    /// <summary>
    /// Functional test for:
    ///     CPollerTicketTransactionMessage::CPollerTicketTransactionMessage()
    ///     CPollerTicketTransactionMessage::GetReadWriteBuffer()
    /// Test Player session ID in the PollerTicketTransactionMessage buffer.
    TEST(PollerTicketTransactionMessage, GetReadWriteBuffer_PlaySessionID)
    {
        // Setup for CPollerTicketTransactionMessage.
        CMeters *meters = (new CMeters(MEMORY_NONE));
        CTicket  *ticket = (new CTicket());
        byte transType = TICKET_CODE;
        const byte * cardID = reinterpret_cast<const byte*>("1234567890");
        
        CPollerTicketTransactionMessage pollerMsg(meters, ticket, transType, 0, cardID);

        DWORD psid = 12;
        pollerMsg.SetPlaySessionID(psid);

        DWORD transLength(0);
        pollerMsg.SetReadWriteBuffer();
        const byte* transBuffer(pollerMsg.GetReadWriteBuffer(transLength));

        uint8_t typeOffset(112);
        DWORD recvdPsid(0);
        byte recvdType(0), recvdDataSize(0), actualDataSize(4);

        ASSERT_TRUE(transLength >= typeOffset + sizeof(byte));
        memcpy(&recvdType, transBuffer + typeOffset, sizeof(byte));
        ASSERT_EQ (recvdType, DATA_TYPE_PLAY_SESSION_ID);

        uint8_t sizeOffset = typeOffset + sizeof(byte);
        ASSERT_TRUE(transLength >= sizeOffset + sizeof(byte));
        memcpy (&recvdDataSize, transBuffer + sizeOffset, sizeof(byte));
        ASSERT_EQ (recvdDataSize, actualDataSize );

        uint8_t dataOffset = sizeOffset + sizeof(byte);	
        ASSERT_TRUE(transLength >= dataOffset + recvdDataSize );
        memcpy (&recvdPsid, transBuffer + dataOffset, recvdDataSize);
        ASSERT_EQ (recvdPsid, psid);
    }


}

