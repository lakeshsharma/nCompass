
#include "stdafx.h"
#include "Util/Convert.h"
#include "RapidJsonTestHelper.h"
#include "TestUtil.h"
#include "PollerProtocol/MobileConnectProtocol/MobileConnectMessageFactory.h"
#include "PollerProtocol/MobileConnectProtocol/MobileConnectProtocolKeys.h"
#include "PollerProtocol/MobileConnectProtocol/PairingInformation.h"
#include "PollerProtocol/MobileConnectProtocol/MobilePBTSessionInfo.h"

#include "gtest/gtest.h"
using namespace std;

namespace SentinelNativeUnitTests
{
	class MobileConnectMessageFactoryUnitTests : public ::testing::Test
	{
	public:
        void f_MobileConnectMessageFactoryConstructorUnitTest(void)
        {
        	long slotMastID = 2185;
			CMobileConnectMessageFactory factory(slotMastID);

			ASSERT_EQ("2185", factory.m_deviceId) << "DeviceId";
			ASSERT_STREQ("Slot", factory.m_deviceType.c_str()) << "DeviceType";
        }
	};

	TEST_F(MobileConnectMessageFactoryUnitTests, MobileConnectMessageFactoryConstructorUnitTest)
	{
        f_MobileConnectMessageFactoryConstructorUnitTest();
		
	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreatePairingSettingsRequestUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);

		string jsonStr = factory.CreatePairingSettingsRequest();

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payload, pFKeyMessageType, "mobile.request.pairingSettings");
		JsonGetStringTest(payload, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payload, pFKeyDeviceId, "2185");
	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreatePairingRequestUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);
		CPairingRequest pairingRequestData;
		string jsonStr = factory.CreatePairingRequest(&pairingRequestData);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payload, pFKeyMessageType, "mobile.request.pairing");
		JsonGetStringTest(payload, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payload, pFKeyDeviceId, "2185");
	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreateCardChangeEventUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);
		string guid = CUtilities::GetGUID();
		CCardChangeEvent cardChangeEventData(UNKNOWN_CARD, false, guid);
		string jsonStr = factory.CreateCardChangeEvent(&cardChangeEventData);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payload, pFKeyMessageType, "session.event.cardChange");
		JsonGetStringTest(payload, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payload, pFKeyDeviceId, "2185");
		JsonGetBoolTest(payload, pFKeySuccess, false);
		JsonGetStringTest(payload, pFKeyStatus, "UnknownCard");

		CCardChangeEvent cardChangeEventData2(string(), true, guid);
		jsonStr = factory.CreateCardChangeEvent(&cardChangeEventData2);

		result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payloadNew = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payloadNew, pFKeyMessageType, "session.event.cardChange");
		JsonGetStringTest(payloadNew, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payloadNew, pFKeyDeviceId, "2185");
		JsonGetBoolTest(payloadNew, pFKeySuccess, true);
		JsonGetStringTest(payloadNew, pFKeyStatus, string().c_str());

	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreatePairingStatusEventUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);
		CPairingStatusEvent pairingStatusEventData(true, PAIRING_ID_SET);
		string jsonStr = factory.CreatePairingStatusEvent(&pairingStatusEventData);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payload, pFKeyMessageType, "mobile.event.pairingStatus");
		JsonGetStringTest(payload, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payload, pFKeyDeviceId, "2185");
		JsonGetBoolTest(payload, pFKeySuccess, true);
		JsonGetStringTest(payload, pFKeyStatus, "PairingIDSet");

		CPairingStatusEvent pairingStatusEventData2(false, ERROR_STATUS);
		jsonStr = factory.CreatePairingStatusEvent(&pairingStatusEventData2);

		result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payloadNew = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payloadNew, pFKeyMessageType, "mobile.event.pairingStatus");
		JsonGetStringTest(payloadNew, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payloadNew, pFKeyDeviceId, "2185");
		JsonGetBoolTest(payloadNew, pFKeySuccess, false);
		JsonGetStringTest(payloadNew, pFKeyStatus, "Error");
	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreatePlayerChangeEventUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);

		CPlayerChangeEvent playerChangeEventData(nullptr, 1234, "Idle", "Uncarded" );
		string jsonStr = factory.CreatePlayerChangeEvent(&playerChangeEventData);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payload, pFKeyMessageType, "session.event.playerChange");
		JsonGetStringTest(payload, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payload, pFKeyDeviceId, "2185");
		JsonGetUintTest(payload, pFKeyPlayerId, 0);
		JsonGetStringTest(payload, pFKeyCardNumber, string().c_str());
		JsonGetIntTest(payload, pFKeyNewPlayerSessionId, 1234);
		JsonGetStringTest(payload, pFKeyPrevPlayerType, "Idle");
		JsonGetStringTest(payload, pFKeyNewPlayerType, "Uncarded");

		CCard *card = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime());
		CCardConfig *cardConfig = new CCardConfig();
		card->SetCardID("A5CA006EBA", cardConfig);
		CPlayerInfo *playerInfo = new CPlayerInfo();
		playerInfo->SetPlayerId(12345);
		CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);
        CGames games;
        CPlayer *player = new CPlayer(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);

		CPlayerChangeEvent *playerChangeEventData2 = new CPlayerChangeEvent(player, 1235, "Uncarded", "Carded" );
		jsonStr = factory.CreatePlayerChangeEvent(playerChangeEventData2);

		result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payload2 = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payload2, pFKeyMessageType, "session.event.playerChange");
		JsonGetStringTest(payload2, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payload2, pFKeyDeviceId, "2185");
		JsonGetUintTest(payload2, pFKeyPlayerId, 12345);
		JsonGetStringTest(payload2, pFKeyCardNumber, "A5CA006EBA");
		JsonGetIntTest(payload2, pFKeyNewPlayerSessionId, 1235);
		JsonGetStringTest(payload2, pFKeyPrevPlayerType, "Uncarded");
		JsonGetStringTest(payload2, pFKeyNewPlayerType, "Carded"); 
	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreatePlayerSessionChangeEventGamePlayUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);
		CCard *card = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime());
		CCardConfig *cardConfig = new CCardConfig();
		card->SetCardID("A5CA006EBA", cardConfig);
		CPlayerInfo *playerInfo = new CPlayerInfo();
		playerInfo->SetPlayerId(12345);
		CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);
        CGames games;
        CPlayer *player = new CPlayer(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);
		CPlayerSessionChangeEvent *pPlayerSessionChangeEventData =  new CPlayerSessionChangeEvent(player, 
			SESSION_CHANGE_EVENT_GAMEPLAY, 1234, 98765);
		string jsonStr = factory.CreatePlayerSessionChangeEvent(pPlayerSessionChangeEventData);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payload, pFKeyMessageType, "session.event.playerSessionChange");
		JsonGetStringTest(payload, pFKeySessionEventType, "GamePlay");
		JsonGetStringTest(payload, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payload, pFKeyDeviceId, "2185");
		JsonGetUintTest(payload, pFKeyPlayerId, 12345);
		JsonGetStringTest(payload, pFKeyCardNumber, "A5CA006EBA");
		JsonGetIntTest(payload, pFKeyPlayerSessionId, 1234);
		JsonGetUint64Test(payload, pFKeyTotalPoints, (DWORD)0);
		JsonGetUint64Test(payload, pFKeySessionPoints, (DWORD)0);
		JsonGetUint64Test(payload, pFKeySessionCountdown, player->GetCurrentCountdown());
		JsonGetUint64Test(payload, pFKeyPointsTillNextTier, (DWORD)0);
		JsonGetUint64Test(payload, pFKeyCreditInPennies, (DWORD)98765);
		JsonGetUint64Test(payload, pFKeyJackpotAmount, 0);
		JsonGetBoolTest(payload, pFKeyPbtEnabled, true);
	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreatePlayerSessionChangeEventJackpotLockupUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);
		CCard *card = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime());
		CCardConfig *cardConfig = new CCardConfig();
		card->SetCardID("A5CA006EBA", cardConfig);
		CPlayerInfo *playerInfo = new CPlayerInfo();
		playerInfo->SetPlayerId(12345);
		CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);
        CGames games;
        CPlayer *player = new CPlayer(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);
		CPlayerSessionChangeEvent *pPlayerSessionChangeEventData =  new CPlayerSessionChangeEvent(player, 
			SESSION_CHANGE_EVENT_JACKPOT_LOCKUP,1234, 98765, 56567, false);
		string jsonStr = factory.CreatePlayerSessionChangeEvent(pPlayerSessionChangeEventData);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payload, pFKeyMessageType, "session.event.playerSessionChange");
		JsonGetStringTest(payload, pFKeySessionEventType, "JackpotLockup");
		JsonGetStringTest(payload, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payload, pFKeyDeviceId, "2185");
		JsonGetUintTest(payload, pFKeyPlayerId, 12345);
		JsonGetStringTest(payload, pFKeyCardNumber, "A5CA006EBA");
		JsonGetIntTest(payload, pFKeyPlayerSessionId, 1234);
		JsonGetUint64Test(payload, pFKeyTotalPoints, (DWORD)0);
		JsonGetUint64Test(payload, pFKeySessionPoints, (DWORD)0);
		JsonGetUint64Test(payload, pFKeySessionCountdown, player->GetCurrentCountdown());
		JsonGetUint64Test(payload, pFKeyPointsTillNextTier, (DWORD)0);
		JsonGetUint64Test(payload, pFKeyCreditInPennies, (DWORD)98765);
		JsonGetUint64Test(payload, pFKeyJackpotAmount, (DWORD)56567);
		JsonGetBoolTest(payload, pFKeyPbtEnabled, false);
	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreatePlayerSessionChangeEventJackpotResetUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);
		CCard *card = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime());
		CCardConfig *cardConfig = new CCardConfig();
		card->SetCardID("A5CA006EBA", cardConfig);
		CPlayerInfo *playerInfo = new CPlayerInfo();
		playerInfo->SetPlayerId(12345);
		CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);
        CGames games;
        CPlayer *player = new CPlayer(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);
		CPlayerSessionChangeEvent *pPlayerSessionChangeEventData =  new CPlayerSessionChangeEvent(player, 
			SESSION_CHANGE_EVENT_JACKPOT_RESET, 1234, 98768, 56568, true);
		string jsonStr = factory.CreatePlayerSessionChangeEvent(pPlayerSessionChangeEventData);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payload, pFKeyMessageType, "session.event.playerSessionChange");
		JsonGetStringTest(payload, pFKeySessionEventType, "JackpotReset");
		JsonGetStringTest(payload, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payload, pFKeyDeviceId, "2185");
		JsonGetUintTest(payload, pFKeyPlayerId, 12345);
		JsonGetStringTest(payload, pFKeyCardNumber, "A5CA006EBA");
		JsonGetIntTest(payload, pFKeyPlayerSessionId, 1234);
		JsonGetUint64Test(payload, pFKeyTotalPoints, (DWORD)0);
		JsonGetUint64Test(payload, pFKeySessionPoints, (DWORD)0);
		JsonGetUint64Test(payload, pFKeySessionCountdown, player->GetCurrentCountdown());
		JsonGetUint64Test(payload, pFKeyPointsTillNextTier, (DWORD)0);
		JsonGetUint64Test(payload, pFKeyCreditInPennies, (DWORD)98768);
		JsonGetUint64Test(payload, pFKeyJackpotAmount, (DWORD)56568);
		JsonGetBoolTest(payload, pFKeyPbtEnabled, true);
	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreatePlayerSessionChangeEventCashoutErrorUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);
		CCard *card = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetCurrentTime());
		CCardConfig *cardConfig = new CCardConfig();
		card->SetCardID("A5CA006EBA", cardConfig);
		CPlayerInfo *playerInfo = new CPlayerInfo();
		playerInfo->SetPlayerId(12345);
		CConfig config(false, "");
		const CEGMConfig egmConfig(MEMORY_NONE);
		CGames games;
		CPlayer *player = new CPlayer(card, playerInfo, config, egmConfig, CUtilities::GetCurrentTime(), false, &games);
		WORD cashoutErrorFlag = CASHOUT_ERROR_CASH_FUNCTIONALITY_UNAVAILABLE | CASHOUT_ERROR_SAS_ERROR;
		std::string sasErrorText = "Over Transfer Limit";
		CPlayerSessionChangeEvent *pPlayerSessionChangeEventData = new CPlayerSessionChangeEvent(player,
			SESSION_CHANGE_EVENT_CASHOUT_ERROR, 1234, 98765, 0, true, cashoutErrorFlag, sasErrorText);
		string jsonStr = factory.CreatePlayerSessionChangeEvent(pPlayerSessionChangeEventData);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payload, pFKeyMessageType, "session.event.playerSessionChange");
		JsonGetStringTest(payload, pFKeySessionEventType, "CashoutError");
		JsonGetStringTest(payload, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payload, pFKeyDeviceId, "2185");
		JsonGetUintTest(payload, pFKeyPlayerId, 12345);
		JsonGetStringTest(payload, pFKeyCardNumber, "A5CA006EBA");
		JsonGetIntTest(payload, pFKeyPlayerSessionId, 1234);
		JsonGetUint64Test(payload, pFKeyTotalPoints, (DWORD)0);
		JsonGetUint64Test(payload, pFKeySessionPoints, (DWORD)0);
		JsonGetUint64Test(payload, pFKeySessionCountdown, player->GetCurrentCountdown());
		JsonGetUint64Test(payload, pFKeyPointsTillNextTier, (DWORD)0);
		JsonGetUint64Test(payload, pFKeyCreditInPennies, (DWORD)98765);
		JsonGetUint64Test(payload, pFKeyJackpotAmount, 0);
		JsonGetBoolTest(payload, pFKeyPbtEnabled, true);
		JsonGetUint64Test(payload, pFKeyCashoutErrorFlags, (DWORD)cashoutErrorFlag);
		JsonGetStringTest(payload, pFKeySASErrorText, sasErrorText.c_str());
	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreateFundsTransferBalanceEventUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);

		CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
		CFundsTransferBalanceEvent *pFundsTransferBalanceEventData = new CFundsTransferBalanceEvent(0,0,0, pbInfo, PBT_ALREADY_INPROGRESS, CUtilities::GetGUID(), false);
		string jsonStr = factory.CreateFundsTransferBalanceEvent(pFundsTransferBalanceEventData);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payload, pFKeyMessageType, "session.response.getBalance");
		JsonGetStringTest(payload, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payload, pFKeyDeviceId, "2185");
		JsonGetBoolTest(payload, pFKeySuccess, false);
		JsonGetStringTest(payload, pFKeyStatus, "PbtAlreadyInProgress");
		JsonGetBoolTest(payload, pFKeyAreSomePropertiesOffline, false);
		JsonGetBoolTest(payload, pFKeyIsBannedFromRedemption, false);
		JsonGetBoolTest(payload, pFKeyIsPartialTransferCashFromGameEnabled, false);
		JsonGetUint64Test(payload, pFKeyCashBalance, (DWORD)0);
		JsonGetUint64Test(payload, pFKeyPromoBalance, (DWORD)0);
		JsonGetUint64Test(payload, pFKeyPointsAvailable,  (DWORD)0);
		JsonGetUint64Test(payload, pFKeyPointsPerDollar, (DWORD)1);
		JsonGetUint64Test(payload, pFKeyMinimumPoints, (DWORD)0);
		JsonGetUint64Test(payload, pFKeyMaximumPoints, (DWORD)0);
		JsonGetUint64Test(payload, pFKeyMinimumCashDown,  (DWORD)0);
		JsonGetUint64Test(payload, pFKeyMaximumCashDown, (DWORD)0);
		JsonGetUint64Test(payload, pFKeyMinimumPromoDown,  (DWORD)0);
		JsonGetUint64Test(payload, pFKeyMaximumPromoDown, (DWORD)0);

		const rapidjson::Value& creditArray = payload[pFKeyCredit];
		if (creditArray.IsArray())
		{
			for (auto it = creditArray.Begin(); it != creditArray.End(); ++it)
			{
					
				JsonGetInt64Test(*it, pFKeyAmountInPennies, (DWORD)0);
			}
		}

		JsonGetBoolTest(payload, pFKeyIsCashFunctionalityUnavailable, false);
	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreateFundsTransferBalanceEventCashActiveUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);
		CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
		CFundsTransferBalanceEvent *pFundsTransferBalanceEventData = new CFundsTransferBalanceEvent(0, 0, 0, pbInfo, PBT_ALREADY_INPROGRESS, CUtilities::GetGUID(), true);
		string jsonStr = factory.CreateFundsTransferBalanceEvent(pFundsTransferBalanceEventData);
		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));
		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		const rapidjson::Value& inactiveAccounts = payload[pFKeyInactiveAccounts];
		if (inactiveAccounts.IsArray())
		{
			ASSERT_TRUE(inactiveAccounts.Begin() == inactiveAccounts.End());
		}

		JsonGetBoolTest(payload, pFKeyIsCashFunctionalityUnavailable, true);
	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreateFundsTransferBalanceEventCashInactiveUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);
		u_PBFlags pbFlags;
		pbFlags.flags.CashInactive = true;
		CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
		pbInfo->SetPBFlags(pbFlags.uValue);
		CFundsTransferBalanceEvent *pFundsTransferBalanceEventData = new CFundsTransferBalanceEvent(0, 0, 0, pbInfo, PBT_ALREADY_INPROGRESS, CUtilities::GetGUID(), false);
		string jsonStr = factory.CreateFundsTransferBalanceEvent(pFundsTransferBalanceEventData);
		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));
		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		const rapidjson::Value& inactiveAccounts = payload[pFKeyInactiveAccounts];
		if (inactiveAccounts.IsArray())
		{
			const std::string CASH("Cash");
			ASSERT_TRUE(CASH == inactiveAccounts[0].GetString());
		}

		JsonGetBoolTest(payload, pFKeyIsCashFunctionalityUnavailable, false);
	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreateFundsTransferEventUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);
		
		CFundsTransferEvent *pFundsTranferEventData = new CFundsTransferEvent("1357", "Cash", "ToGame", true, string(), 1234, 0, 0, CUtilities::GetGUID());
		string jsonStr = factory.CreateFundsTransferEvent(pFundsTranferEventData);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payload, pFKeyMessageType, "session.event.fundsTransfer");
		JsonGetStringTest(payload, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payload, pFKeyDeviceId, "2185");
		JsonGetStringTest(payload, pFKeyPin, "1357");
		JsonGetStringTest(payload, pFKeyFundType, "Cash");
		JsonGetStringTest(payload, pFKeyTransferDirection, "ToGame");
		JsonGetBoolTest(payload, pFKeySuccess, true);
		JsonGetStringTest(payload, pFKeyStatus, string().c_str());

		const rapidjson::Value& transferArray = payload[pFKeyTransfer];
		if (transferArray.IsArray())
		{
			for (auto it = transferArray.Begin(); it != transferArray.End(); ++it)
			{
				if (!strcmp( JsonGetString(*it, pFKeyType, nullptr), CASH.c_str()))
					JsonGetInt64Test(*it, pFKeyAmountInPennies, (DWORD)1234);
			}
		}
	
	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreateEndPBTSessionEventUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);

		string jsonStr = factory.CreateEndPBTSessionEvent();

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payload, pFKeyMessageType, "session.event.endPbtSession");
		JsonGetStringTest(payload, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payload, pFKeyDeviceId, "2185");
	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreateProximitySettingsRequestUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);

		string jsonStr = factory.CreateProximitySettingsRequest();

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payload, pFKeyMessageType, "mobile.request.proximitySettings");
		JsonGetStringTest(payload, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payload, pFKeyDeviceId, "2185");
	}

	TEST_F(MobileConnectMessageFactoryUnitTests, CreateProximityStatusEventUnitTest)
	{
		long slotMastID = 2185;
		CMobileConnectMessageFactory factory(slotMastID);
		CProximityStatusEvent proximityStatusEventData(true, PROXIMITY_ID_SET);
		string jsonStr = factory.CreateProximityStatusEvent(&proximityStatusEventData);

		rapidjson::Document jsonDoc;
		bool result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payload, pFKeyMessageType, "mobile.event.proximityStatus");
		JsonGetStringTest(payload, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payload, pFKeyDeviceId, "2185");
		JsonGetBoolTest(payload, pFKeySuccess, true);
		JsonGetStringTest(payload, pFKeyStatus, "ProximityIdSet");

		CProximityStatusEvent proximityStatusEventData2(false, BEACON_ERROR);
		jsonStr = factory.CreateProximityStatusEvent(&proximityStatusEventData2);

		result = jsonDoc.Parse(jsonStr.c_str()).HasParseError();
		ASSERT_FALSE(result) << AssertMsg::Format(0, L"Parse", L"%S", rapidjson::GetParseError_En(jsonDoc.GetParseError()));

		const rapidjson::Value& payloadNew = jsonDoc[pFKeyPayload];
		JsonGetStringTest(payloadNew, pFKeyMessageType, "mobile.event.proximityStatus");
		JsonGetStringTest(payloadNew, pFKeyDeviceType, "Slot");
		JsonGetStringTest(payloadNew, pFKeyDeviceId, "2185");
		JsonGetBoolTest(payloadNew, pFKeySuccess, false);
		JsonGetStringTest(payloadNew, pFKeyStatus, "BeaconError");
	}
}
