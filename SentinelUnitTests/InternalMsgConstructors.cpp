#include "stdafx.h"
#include "../Sentinel/InternalMessage.h"
#include "../Sentinel/Meters.h"
#include "../Sentinel/Ticket.h"
#include "../Sentinel/Games.h"
#include "../Sentinel/TicketingInfo.h"
#include "../Sentinel/ProgressiveProtocol/EGMAward.h"
#include "gtest/gtest.h"

using namespace std;

class InternalMessageConstructorsUnitTests : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

};

TEST_F(InternalMessageConstructorsUnitTests, InternalMessageMetersConstructor)
{
    CMeters *meters = new CMeters(MEMORY_NONE);
    meters->SetMeter(1, 100);

    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, meters, EGM_GAME_NUMBER, false);

    WORD gameNumber = EGM_GAME_NUMBER;
    CMeters *msgMeters = msg->GetMetersData(gameNumber);
    ASSERT_TRUE(msgMeters == meters);
    ASSERT_TRUE(msgMeters->GetMeterByNumber(1)->GetValue() == meters->GetMeterByNumber(1)->GetValue());

    delete msg;
}

TEST_F(InternalMessageConstructorsUnitTests, InternalMessageTicketConstructor)
{
    CTicket *ticket = new CTicket();
    ticket->SetAmount(1234);

    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, ticket);

    CTicket *msgTicket = msg->GetTicketData();
    ASSERT_TRUE(msgTicket == ticket);
    ASSERT_TRUE(msgTicket->Amount() == ticket->Amount());

    delete msg;

}
TEST_F(InternalMessageConstructorsUnitTests, InternalMessageGamesConstructor)
{

    CGames *games = new CGames(MEMORY_NONE);
    CGame *game = new CGame(MEMORY_NONE);
    game->SetMaxBet(1000);
    games->AddGame(game);

    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, games);

    CGames *msgGames = msg->GetGamesData();

    ASSERT_TRUE(msgGames == games);
    ASSERT_TRUE(msgGames->GetGameByIndex(0) == games->GetGameByIndex(0));

    delete msg;

}

TEST_F(InternalMessageConstructorsUnitTests, InternalMessageConfigFieldsConstructor)
{

    CConfigFields *fields = new CConfigFields();
    CConfigField *field = new CConfigField(COMM_ID);
    fields->AddField(field);
    
    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, fields);

    CConfigFields *msgFields = msg->GetConfigData();

    ASSERT_TRUE(msgFields == fields);

    delete msg;

}

TEST_F(InternalMessageConstructorsUnitTests, InternalMessageStringConstructor)
{
    string str("TestString");

    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, str);

    ASSERT_TRUE(msg->GetStringData() == str);

    delete msg;
}

TEST_F(InternalMessageConstructorsUnitTests, InternalMessageDWordConstructor)
{

    DWORD primary = 1234;
    DWORD secondary = 5678;

    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, primary, secondary);

    DWORD msgPrimary = msg->GetDWordData(false);
    DWORD msgSecondary = msg->GetDWordData(true);

    ASSERT_TRUE(msgPrimary == primary);
    ASSERT_TRUE(msgSecondary == secondary);

    delete msg;

}

TEST_F(InternalMessageConstructorsUnitTests, InternalMessageInt64Constructor)
{

    __int64 primary = 1234;
    __int64 secondary = 5678;

    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, primary, secondary);

    __int64 msgPrimary = msg->GetInt64Data(false);
    __int64 msgSecondary = msg->GetInt64Data(true);

    ASSERT_TRUE(msgPrimary == primary);
    ASSERT_TRUE(msgSecondary == secondary);

    delete msg;

}

TEST_F(InternalMessageConstructorsUnitTests, InternalMessageBytesConstructor)
{

    size_t sizeBytes = 5;
    BYTE bytes[5] = { 01, 02, 03, 04, 05 };

    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, bytes, sizeBytes);

    size_t msgSize = 0;
    const BYTE *msgBytes = msg->GetBytesData(msgSize);
    msg->SetData((const BYTE *)NULL, 0);

    ASSERT_TRUE(msgSize == sizeBytes);
    ASSERT_TRUE(memcmp(bytes, msgBytes, msgSize) == 0);

    delete msg;

}

TEST_F(InternalMessageConstructorsUnitTests, InternalMessageConnectionConstructor)
{

    CConnection *conn = new CConnection();
    string host = "test.com";
    conn->SetHost(host);
    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, conn);

    CConnection *msgConn = msg->GetConnectionData();

    ASSERT_TRUE(msgConn == conn);
    ASSERT_TRUE(msgConn->GetHost() == conn->GetHost());

    delete msg;

}

TEST_F(InternalMessageConstructorsUnitTests, InternalMessageTicketingInfoConstructor)
{
    CConfig config(false, "");
    CTicketingInfo *tkt = new CTicketingInfo(config);
    tkt->SetCasinoNameAddress(_T("Casino Name"), _T("Address Line 1"), _T("Address Line 2"));

    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, tkt);

    CTicketingInfo *msgTkt = msg->GetTicketingInfoData();

    ASSERT_TRUE(msgTkt == tkt);
    ASSERT_TRUE(msgTkt->GetCasinoName() == tkt->GetCasinoName());
    ASSERT_TRUE(msgTkt->GetCasinoAddress1() == tkt->GetCasinoAddress1());
    ASSERT_TRUE(msgTkt->GetCasinoAddress2() == tkt->GetCasinoAddress2());

    delete msg;

}

TEST_F(InternalMessageConstructorsUnitTests, InternalMessagePersonalBankerInfoConstructor)
{
    CPersonalBankerInfo *info = new CPersonalBankerInfo(MEMORY_NONE, false);
    info->SetMaximumCashDown(12345);
    info->SetMaximumCashUp(67890);
    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, info);

    CPersonalBankerInfo *msgInfo = msg->GetPBInfoData();

    ASSERT_TRUE(msgInfo == info);

    delete msg;

}

TEST_F(InternalMessageConstructorsUnitTests, InternalMessagePersonalBankerTransferConstructor)
{
    CPersonalBankerTransfer *transfer = new CPersonalBankerTransfer(0, MEMORY_NONE, false);
    transfer->SetAmount(1234);
    transfer->SetDenomination(12);
    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, transfer, PBStateUnknown, false);

    CPersonalBankerTransfer *msgTransfer = msg->GetPBTransferData();

    ASSERT_TRUE(msgTransfer == transfer);

    delete msg;
}

TEST_F(InternalMessageConstructorsUnitTests, InternalMessageLogsConstructor)
{
    CLogs *logs = new CLogs(MEMORY_NONE, LOG_TICKET, "");
    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, logs);

    CLogs *msgLogs = msg->GetLogData();

    ASSERT_TRUE(msgLogs == logs);

    delete msg;
}

TEST_F(InternalMessageConstructorsUnitTests, InternalMessageEgmConfigConstructor)
{
    CEGMConfig *config = new CEGMConfig(MEMORY_NONE);

    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, config);

    CEGMConfig *msgConfig = msg->GetEGMConfigData();

    ASSERT_TRUE(msgConfig == config);

    delete msg;
}

TEST_F(InternalMessageConstructorsUnitTests, InternalMessageJackpotConstructor)
{
    CJackpot *data = new CJackpot(MEMORY_NONE, false);
    data->SetJackpotAmount(12345);

    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, data);

    CJackpot *msgData = msg->GetJackpotData();

    ASSERT_TRUE(msgData == data);
    ASSERT_TRUE(msgData->GetJackpotAmount() == data->GetJackpotAmount());

    delete msg;
}

TEST_F(InternalMessageConstructorsUnitTests, InternalMessageFlashStatusConstructor)
{
    CFlashStatus *data = new CFlashStatus(FLASH_SYSTEM, 99, "", "", "", "");

    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, data);
    CFlashStatus *msgData = msg->GetFlashStatus();

    ASSERT_TRUE(msgData == data);
    ASSERT_TRUE(msgData->GetPercentComplete() == data->GetPercentComplete());

    delete msg;
}

TEST_F(InternalMessageConstructorsUnitTests, InternalMessageEGMAwardConstructor)
{
    CEGMAward *data = new CEGMAward();

    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, data);

    CEGMAward *msgData = msg->GetEGMAward();

    ASSERT_TRUE(msgData == data);

    delete msg;
}
TEST_F(InternalMessageConstructorsUnitTests, InternalMessageGlobalBalanceConstructor)
{
    CPlayerGlobalBalance *data = new CPlayerGlobalBalance();

    CInternalMessage *msg = new CInternalMessage(INT_MSG_NONE, data);

    CPlayerGlobalBalance *msgData = msg->GetPlayerGlobalBalanceData();

    ASSERT_TRUE(msgData == data);

    delete msg;
}