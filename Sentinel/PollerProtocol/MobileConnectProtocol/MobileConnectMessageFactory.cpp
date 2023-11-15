#include "stdafx.h"

#include "MobileConnectMessageFactory.h"
#include "RapidJsonHelper.h"
#include "ProgressiveProtocol/ProgressiveKeys.h"
#include "Logging/LogString.h"
#include "Util/Convert.h"
#include "MobileConnectProtocolKeys.h"

using namespace std;
using namespace rapidjson;

static const int MessageIdBufferLen = 64;
static const char MacAddressDash = '-';
static const char MacAddressColon = ':';
static const std::string DEVICE_TYPE("Slot");
static const std::string CASH("Cash");
static const std::string NON_CASHABLE_PROMO("NonCashablePromo");
static const std::string CASHABLE_PROMO("CashablePromo");
static const std::string PAYLOAD_VERSION("2.0");

static const std::string PLAYER_SESSION_EVENT_TYPE_GAMEPLAY("GamePlay");
static const std::string PLAYER_SESSION_EVENT_TYPE_JACKPOT_LOCKUP("JackpotLockup");
static const std::string PLAYER_SESSION_EVENT_TYPE_JACKPOT_RESET("JackpotReset");
static const std::string PLAYER_SESSION_EVENT_TYPE_CASHOUT_ERROR("CashoutError");

CMobileConnectMessageFactory::CMobileConnectMessageFactory(long slotMastID):
		m_deviceId(std::to_string(slotMastID)),
		m_deviceType(DEVICE_TYPE)
{
}

CMobileConnectMessageFactory::~CMobileConnectMessageFactory()
{
}

string CMobileConnectMessageFactory::GetDeviceId() const
{
	return m_deviceId;
}

void CMobileConnectMessageFactory::UpdateDeviceId(long newDeviceId)
{
	m_deviceId = std::to_string(newDeviceId);
}

string CMobileConnectMessageFactory::CreatePairingRequest(CPairingRequest *pairingRequestData)
{
	JsonWriteStreamString writeStream;

	if (pairingRequestData)
	{
		LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);

		JsonWriter writer(writeStream);

		writer.StartObject();
		writer.Key(pFKeyMessageType);	writer.String(pairingRequestData->GetMsgType());
		writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
		writer.Key(pFKeyMessageId); 	writer.String(CUtilities::GetGUID());
		writer.Key(pFKeyTimeStamp); 	writer.String(CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), "%Y-%m-%dT%I:%M:%S.%z"));
		writer.Key(pFKeyPayloadVer);	writer.String(PAYLOAD_VERSION);
		writer.Key(pFKeyCorrelationId); writer.String(CUtilities::GetGUID());
		writer.Key(pFKeyIdempotencyId);	writer.String(CUtilities::GetGUID());

		writer.Key(pFKeyPayload);
			writer.StartObject();
			writer.Key(pFKeyMessageType);	writer.String(pairingRequestData->GetMsgType());
			writer.Key(pFKeyDeviceType);	writer.String(m_deviceType);
			writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
			writer.EndObject();

		writer.EndObject();
	}
	else
	{
		LogString(POLLER_LOG, "%s %d: Invalid pairingRequestData", __FUNCTION__, __LINE__);
	}
	return writeStream.m_str;
}

string CMobileConnectMessageFactory::CreatePairingSettingsRequest()
{
	LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);

	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key(pFKeyMessageType);	writer.String(ToPlayerDeviceService::pRequestPairingSettings);
	writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
	writer.Key(pFKeyMessageId); 	writer.String(CUtilities::GetGUID());
	writer.Key(pFKeyTimeStamp); 	writer.String(CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), "%Y-%m-%dT%I:%M:%S.%z"));
	writer.Key(pFKeyPayloadVer);	writer.String(PAYLOAD_VERSION);
	writer.Key(pFKeyCorrelationId); writer.String(CUtilities::GetGUID());
	writer.Key(pFKeyIdempotencyId);	writer.String(CUtilities::GetGUID());

	writer.Key(pFKeyPayload);
		writer.StartObject();
		writer.Key(pFKeyMessageType);	writer.String(ToPlayerDeviceService::pRequestPairingSettings);
		writer.Key(pFKeyDeviceType);	writer.String(m_deviceType);
		writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
		writer.EndObject();

	writer.EndObject();

	return writeStream.m_str;
}

// The following are commented until features implemented in future changesets
std::string CMobileConnectMessageFactory::CreatePairingStatusEvent(CPairingStatusEvent *pPairingStatusEventData)
{
	JsonWriteStreamString writeStream;

	if (pPairingStatusEventData)
	{
		LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);

		JsonWriter writer(writeStream);

		writer.StartObject();
		writer.Key(pFKeyMessageType);	writer.String(pPairingStatusEventData->GetMsgType());
		writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
		writer.Key(pFKeyMessageId); 	writer.String(CUtilities::GetGUID());
		writer.Key(pFKeyTimeStamp); 	writer.String(CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), "%Y-%m-%dT%I:%M:%S.%z"));
		writer.Key(pFKeyPayloadVer);	writer.String(PAYLOAD_VERSION);
		writer.Key(pFKeyCorrelationId); writer.String(CUtilities::GetGUID());
		writer.Key(pFKeyIdempotencyId);	writer.String(CUtilities::GetGUID());

		writer.Key(pFKeyPayload);
			writer.StartObject();
			writer.Key(pFKeyMessageType);		writer.String(pPairingStatusEventData->GetMsgType());
			writer.Key(pFKeyDeviceType);		writer.String(m_deviceType);
			writer.Key(pFKeyDeviceId);			writer.String(m_deviceId);
			writer.Key(pFKeySuccess);			writer.Bool(pPairingStatusEventData->IsSuccess());
			writer.Key(pFKeyStatus);			writer.String(pPairingStatusEventData->GetStatus());
			writer.EndObject();

		writer.EndObject();
	}
	else
	{
		LogString(POLLER_LOG, "%s %d: Invalid PairingStatusEvent Data", __FUNCTION__, __LINE__);
	}
	return writeStream.m_str;
}

string CMobileConnectMessageFactory::CreatePlayerChangeEvent(CPlayerChangeEvent *pPlayerChangeEventData)
{
	JsonWriteStreamString writeStream;

	if (pPlayerChangeEventData)
	{
		LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);

		JsonWriter writer(writeStream);

		writer.StartObject();
		writer.Key(pFKeyMessageType);	writer.String(pPlayerChangeEventData->GetMessageType());
		writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
		writer.Key(pFKeyMessageId); 	writer.String(CUtilities::GetGUID());
		writer.Key(pFKeyTimeStamp); 	writer.String(CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), "%Y-%m-%dT%I:%M:%S.%z"));
		writer.Key(pFKeyPayloadVer);	writer.String(PAYLOAD_VERSION);
		writer.Key(pFKeyCorrelationId); writer.String(CUtilities::GetGUID());
		writer.Key(pFKeyIdempotencyId);	writer.String(CUtilities::GetGUID());

		writer.Key(pFKeyPayload);
			writer.StartObject();
			writer.Key(pFKeyMessageType);		writer.String(pPlayerChangeEventData->GetMessageType());
			writer.Key(pFKeyPlayerId);			writer.Uint64(pPlayerChangeEventData->GetPlayerId());
			writer.Key(pFKeyCardNumber);		writer.String(pPlayerChangeEventData->GetCardId());
			writer.Key(pFKeyDeviceId);			writer.String(m_deviceId);
			writer.Key(pFKeyDeviceType);		writer.String(m_deviceType);
			writer.Key(pFKeyNewPlayerSessionId);writer.Int(pPlayerChangeEventData->GetNewPlayerSessionId());
			writer.Key(pFKeyPrevPlayerType);	writer.String(pPlayerChangeEventData->GetPrevPlayerType());
			writer.Key(pFKeyNewPlayerType);		writer.String(pPlayerChangeEventData->GetNewPlayerType());
			writer.EndObject();

		writer.EndObject();
	}
	else
	{
		LogString(POLLER_LOG, "%s %d: Invalid PlayerChangeEventData", __FUNCTION__, __LINE__);
	}
	return writeStream.m_str;
}

string CMobileConnectMessageFactory::CreateCardChangeEvent(CCardChangeEvent *pCardChangeEventData)
{
	JsonWriteStreamString writeStream;

	if (pCardChangeEventData)
	{
		LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);

		JsonWriter writer(writeStream);

		writer.StartObject();
		writer.Key(pFKeyMessageType);	writer.String(pCardChangeEventData->GetMessageType());
		writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
		writer.Key(pFKeyMessageId); 	writer.String(CUtilities::GetGUID());
		writer.Key(pFKeyTimeStamp); 	writer.String(CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), "%Y-%m-%dT%I:%M:%S.%z"));
		writer.Key(pFKeyPayloadVer);	writer.String(PAYLOAD_VERSION);
		writer.Key(pFKeyCorrelationId); writer.String(pCardChangeEventData->GetCorrelationId());
		writer.Key(pFKeyIdempotencyId);	writer.String(CUtilities::GetGUID());

		writer.Key(pFKeyPayload);
			writer.StartObject();
			writer.Key(pFKeyMessageType);	writer.String(pCardChangeEventData->GetMessageType());
			writer.Key(pFKeyDeviceType);	writer.String(m_deviceType);
			writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
			writer.Key(pFKeySuccess);		writer.Bool(pCardChangeEventData->GetIsSuccess());
			writer.Key(pFKeyStatus);		writer.String(pCardChangeEventData->GetStatusText());
			writer.EndObject();

		writer.EndObject();
	}
	else
	{
		LogString(POLLER_LOG, "%s %d: Invalid CardChangeEventData", __FUNCTION__, __LINE__);
	}
	return writeStream.m_str;
}

string CMobileConnectMessageFactory::CreatePlayerSessionChangeEvent(CPlayerSessionChangeEvent *pPlayerSessionChangeEventData)
{
	JsonWriteStreamString writeStream;

	if (pPlayerSessionChangeEventData)
	{
		string sessionEventType;
		switch (pPlayerSessionChangeEventData->GetSessionChangeEventType())
		{
		case SESSION_CHANGE_EVENT_JACKPOT_LOCKUP:
			sessionEventType = PLAYER_SESSION_EVENT_TYPE_JACKPOT_LOCKUP;
			break;
		case SESSION_CHANGE_EVENT_JACKPOT_RESET:
			sessionEventType = PLAYER_SESSION_EVENT_TYPE_JACKPOT_RESET;
			break;
		case SESSION_CHANGE_EVENT_CASHOUT_ERROR:
			sessionEventType = PLAYER_SESSION_EVENT_TYPE_CASHOUT_ERROR;
			break;
		default:
			sessionEventType = PLAYER_SESSION_EVENT_TYPE_GAMEPLAY;
		};

		LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);

		JsonWriter writer(writeStream);

		writer.StartObject();
		writer.Key(pFKeyMessageType);	writer.String(pPlayerSessionChangeEventData->GetMessageType());
		writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
		writer.Key(pFKeyMessageId); 	writer.String(CUtilities::GetGUID());
		writer.Key(pFKeyTimeStamp); 	writer.String(CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), "%Y-%m-%dT%I:%M:%S.%z"));
		writer.Key(pFKeyPayloadVer);	writer.String(PAYLOAD_VERSION);
		writer.Key(pFKeyCorrelationId); writer.String(CUtilities::GetGUID());
		writer.Key(pFKeyIdempotencyId);	writer.String(CUtilities::GetGUID());
				
		writer.Key(pFKeyPayload);
			writer.StartObject();
			writer.Key(pFKeyMessageType);		writer.String(pPlayerSessionChangeEventData->GetMessageType());
			writer.Key(pFKeySessionEventType);	writer.String(sessionEventType);
			writer.Key(pFKeyPlayerId);			writer.Uint64(pPlayerSessionChangeEventData->GetPlayerId());
			writer.Key(pFKeyCardNumber);		writer.String(pPlayerSessionChangeEventData->GetCardId());
			writer.Key(pFKeyDeviceId);			writer.String(m_deviceId);
			writer.Key(pFKeyDeviceType);		writer.String(m_deviceType);
			writer.Key(pFKeyPlayerSessionId);	writer.Int(pPlayerSessionChangeEventData->GetPlayerSessionId());
			writer.Key(pFKeyTotalPoints);		writer.Uint64(pPlayerSessionChangeEventData->GetTotalPoints());
			writer.Key(pFKeySessionPoints);		writer.Uint64(pPlayerSessionChangeEventData->GetSessionPoints());
			writer.Key(pFKeySessionCountdown);	writer.Uint64(pPlayerSessionChangeEventData->GetSessionCountdown());
			writer.Key(pFKeyPointsTillNextTier);writer.Uint64(pPlayerSessionChangeEventData->GetPointsToNextTier());
			writer.Key(pFKeyCreditInPennies);	writer.Uint64(pPlayerSessionChangeEventData->GetCreditInPennies());
			writer.Key(pFKeyJackpotAmount);     writer.Uint64(pPlayerSessionChangeEventData->GetJackpotAmount());
			writer.Key(pFKeyPbtEnabled);		writer.Bool(pPlayerSessionChangeEventData->GetPbtEnabled());

			if (pPlayerSessionChangeEventData->GetSessionChangeEventType() == SESSION_CHANGE_EVENT_CASHOUT_ERROR)
			{
				writer.Key(pFKeyCashoutErrorFlags);	writer.Uint64(pPlayerSessionChangeEventData->GetCashoutErrorFlags());
				if (!pPlayerSessionChangeEventData->GetSASErrorText().empty())
				{
					writer.Key(pFKeySASErrorText);		writer.String(pPlayerSessionChangeEventData->GetSASErrorText());
				}
			}
			writer.EndObject();

		writer.EndObject();
	}
	else
	{
		LogString(POLLER_LOG, "%s %d: Invalid PlayerSessionChangeEvent", __FUNCTION__, __LINE__);
	}
	return writeStream.m_str;
}

string CMobileConnectMessageFactory::CreateFundsTransferBalanceEvent(CFundsTransferBalanceEvent *pFundsTransferBalanceEventData)
{
	JsonWriteStreamString writeStream;

	if (pFundsTransferBalanceEventData)
	{
		LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);

		JsonWriter writer(writeStream);

		writer.StartObject();
		writer.Key(pFKeyMessageType);	writer.String(pFundsTransferBalanceEventData->GetMessageType());
		writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
		writer.Key(pFKeyMessageId); 	writer.String(CUtilities::GetGUID());
		writer.Key(pFKeyTimeStamp); 	writer.String(CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), "%Y-%m-%dT%I:%M:%S.%z"));
		writer.Key(pFKeyPayloadVer);	writer.String(PAYLOAD_VERSION);
		writer.Key(pFKeyCorrelationId); writer.String(pFundsTransferBalanceEventData->GetCorrelationId());
		writer.Key(pFKeyIdempotencyId);	writer.String(CUtilities::GetGUID());
				
		writer.Key(pFKeyPayload);
			writer.StartObject();
			writer.Key(pFKeyMessageType);		writer.String(pFundsTransferBalanceEventData->GetMessageType());
			writer.Key(pFKeyDeviceId);			writer.String(m_deviceId);
			writer.Key(pFKeyDeviceType);		writer.String(m_deviceType);
			writer.Key(pFKeySuccess);			writer.Bool(pFundsTransferBalanceEventData->IsSuccess());
			writer.Key(pFKeyStatus);			writer.String(pFundsTransferBalanceEventData->GetStatus());
			writer.Key(pFKeyAreSomePropertiesOffline);			writer.Bool(pFundsTransferBalanceEventData->AreSomePropertiesOffline());
			writer.Key(pFKeyIsBannedFromRedemption);			writer.Bool(pFundsTransferBalanceEventData->IsBannedFromRedemption());
			writer.Key(pFKeyIsPartialTransferCashFromGameEnabled);			writer.Bool(pFundsTransferBalanceEventData->IsPartialTransferCashFromGameEnabled());
			writer.Key(pFKeyCashBalance);		writer.Uint64(pFundsTransferBalanceEventData->GetCashBalance());
			writer.Key(pFKeyPromoBalance);		writer.Uint64(pFundsTransferBalanceEventData->GetPromoBalance());
			writer.Key(pFKeyPointsAvailable);	writer.Uint64(pFundsTransferBalanceEventData->GetPointsAvailable());
			writer.Key(pFKeyPointsPerDollar);	writer.Uint64(pFundsTransferBalanceEventData->GetPointsPerDollar());
			writer.Key(pFKeyMinimumPoints);		writer.Uint64(pFundsTransferBalanceEventData->GetMinimumPoints());
			writer.Key(pFKeyMaximumPoints);		writer.Uint64(pFundsTransferBalanceEventData->GetMaximumPoints());
			writer.Key(pFKeyMinimumCashDown);	writer.Uint64(pFundsTransferBalanceEventData->GetMinimumCashDown());
			writer.Key(pFKeyMaximumCashDown);	writer.Uint64(pFundsTransferBalanceEventData->GetMaximumCashDown());
			writer.Key(pFKeyMinimumPromoDown);	writer.Uint64(pFundsTransferBalanceEventData->GetMinimumPromoDown());
			writer.Key(pFKeyMaximumPromoDown);	writer.Uint64(pFundsTransferBalanceEventData->GetMaximumPromoDown());

			writer.Key(pFKeyCredit);
				writer.StartArray();
					writer.StartObject();
					writer.Key(pFKeyType);				writer.String(CASH);
					writer.Key(pFKeyAmountInPennies);	writer.Uint64(pFundsTransferBalanceEventData->GetCashable());
					writer.EndObject();
					
					writer.StartObject();
					writer.Key(pFKeyType);				writer.String(CASHABLE_PROMO);
					writer.Key(pFKeyAmountInPennies);	writer.Uint64(pFundsTransferBalanceEventData->GetCashablePromo());
					writer.EndObject();
					
					writer.StartObject();
					writer.Key(pFKeyType);				writer.String(NON_CASHABLE_PROMO);
					writer.Key(pFKeyAmountInPennies);	writer.Uint64(pFundsTransferBalanceEventData->GetNonCashablePromo());
					writer.EndObject();
				writer.EndArray();

			writer.Key(pFKeyInactiveAccounts);
				writer.StartArray();
					if (pFundsTransferBalanceEventData->IsCashInactive())
					{
						writer.String(CASH);
					}
				writer.EndArray();

			writer.Key(pFKeyIsCashFunctionalityUnavailable);	writer.Bool(pFundsTransferBalanceEventData->IsCashFunctionalityUnavailable());

			writer.EndObject();
		writer.EndObject();
	}
	else
	{
		LogString(POLLER_LOG, "%s %d: Invalid CreditTotalsEventData", __FUNCTION__, __LINE__);
	}
	return writeStream.m_str;
}

string CMobileConnectMessageFactory::CreateFundsTransferEvent(CFundsTransferEvent *pFundsTranferEventData)
{
	JsonWriteStreamString writeStream;

	if (pFundsTranferEventData)
	{
		LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);

		JsonWriter writer(writeStream);

		writer.StartObject();
		writer.Key(pFKeyMessageType);	writer.String(pFundsTranferEventData->GetMessageType());
		writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
		writer.Key(pFKeyMessageId); 	writer.String(CUtilities::GetGUID());
		writer.Key(pFKeyTimeStamp); 	writer.String(CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), "%Y-%m-%dT%I:%M:%S.%z"));
		writer.Key(pFKeyPayloadVer);	writer.String(PAYLOAD_VERSION);
		writer.Key(pFKeyCorrelationId); writer.String(pFundsTranferEventData->GetCorrelationId());
		writer.Key(pFKeyIdempotencyId);	writer.String(CUtilities::GetGUID());
				
		writer.Key(pFKeyPayload);
			writer.StartObject();
			writer.Key(pFKeyMessageType);		writer.String(pFundsTranferEventData->GetMessageType());
			writer.Key(pFKeyDeviceType);		writer.String(m_deviceType);
			writer.Key(pFKeyDeviceId);			writer.String(m_deviceId);
			writer.Key(pFKeyPin);				writer.String(pFundsTranferEventData->GetPin());
			writer.Key(pFKeyFundType);			writer.String(pFundsTranferEventData->GetFundType());
			writer.Key(pFKeyTransferDirection);	writer.String(pFundsTranferEventData->GetTransferDirection());
			writer.Key(pFKeySuccess);			writer.Bool(pFundsTranferEventData->IsSuccess());
			writer.Key(pFKeyStatus);			writer.String(pFundsTranferEventData->GetStatus());

			// transfer amount detials for success transaction only
			if (pFundsTranferEventData->IsSuccess())
			{
				writer.Key(pFKeyTransfer);
				writer.StartArray();

				if(pFundsTranferEventData->GetCashable())
				{
					writer.StartObject();
					writer.Key(pFKeyType);				writer.String(CASH);
					writer.Key(pFKeyAmountInPennies);	writer.Uint64(pFundsTranferEventData->GetCashable());
					writer.EndObject();
				}
				
				if(pFundsTranferEventData->GetCashablePromo())
				{
					writer.StartObject();
					writer.Key(pFKeyType);				writer.String(CASHABLE_PROMO);
					writer.Key(pFKeyAmountInPennies);	writer.Uint64(pFundsTranferEventData->GetCashablePromo());
					writer.EndObject();
				}
				
				if(pFundsTranferEventData->GetNonCashablePromo())
				{
					writer.StartObject();
					writer.Key(pFKeyType);				writer.String(NON_CASHABLE_PROMO);
					writer.Key(pFKeyAmountInPennies);	writer.Uint64(pFundsTranferEventData->GetNonCashablePromo());
					writer.EndObject();
				}
				
				writer.EndArray();
			}
			writer.EndObject();
		writer.EndObject();
	}
	else
	{
		LogString(POLLER_LOG, "%s %d: Invalid CFundsTransferEvent", __FUNCTION__, __LINE__);
	}

	return writeStream.m_str;
}

std::string CMobileConnectMessageFactory::CreateEndPBTSessionEvent()
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key(pFKeyMessageType);	writer.String(ToPlayerDeviceService::pEventEndPBTSession);
	writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
	writer.Key(pFKeyMessageId);		writer.String(CUtilities::GetGUID());
	writer.Key(pFKeyTimeStamp);		writer.String(CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), "%Y-%m-%dT%I:%M:%S.%z"));
	writer.Key(pFKeyPayloadVer);	writer.String(PAYLOAD_VERSION);
	writer.Key(pFKeyCorrelationId);	writer.String(CUtilities::GetGUID());
	writer.Key(pFKeyIdempotencyId);	writer.String(CUtilities::GetGUID());

	writer.Key(pFKeyPayload);
		writer.StartObject();
		writer.Key(pFKeyMessageType); 	writer.String(ToPlayerDeviceService::pEventEndPBTSession);
		writer.Key(pFKeyDeviceType);	writer.String(m_deviceType);
		writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
		writer.EndObject();

	writer.EndObject();

	return writeStream.m_str;
}

/// Proximity-specific functionality
string CMobileConnectMessageFactory::CreateProximitySettingsRequest()
{
	LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);

	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key(pFKeyMessageType);	writer.String(ToPlayerDeviceService::pRequestProximitySettings);
	writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
	writer.Key(pFKeyMessageId); 	writer.String(CUtilities::GetGUID());
	writer.Key(pFKeyTimeStamp); 	writer.String(CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), "%Y-%m-%dT%I:%M:%S.%z"));
	writer.Key(pFKeyPayloadVer);	writer.String(PAYLOAD_VERSION);
	writer.Key(pFKeyCorrelationId); writer.String(CUtilities::GetGUID());
	writer.Key(pFKeyIdempotencyId);	writer.String(CUtilities::GetGUID());

	writer.Key(pFKeyPayload);
		writer.StartObject();
		writer.Key(pFKeyMessageType);	writer.String(ToPlayerDeviceService::pRequestProximitySettings);
		writer.Key(pFKeyDeviceType);	writer.String(m_deviceType);
		writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
		writer.EndObject();

	writer.EndObject();

	return writeStream.m_str;
}

/// Proximity-specific functionality
std::string CMobileConnectMessageFactory::CreateProximityStatusEvent(CProximityStatusEvent *pProximityStatusEventData)
{
	JsonWriteStreamString writeStream;

	if (pProximityStatusEventData)
	{
		LogString(POLLER_LOG, "%s %d", __FUNCTION__, __LINE__);

		JsonWriter writer(writeStream);

		writer.StartObject();
		writer.Key(pFKeyMessageType);	writer.String(pProximityStatusEventData->GetMsgType());
		writer.Key(pFKeyDeviceId);		writer.String(m_deviceId);
		writer.Key(pFKeyMessageId); 	writer.String(CUtilities::GetGUID());
		writer.Key(pFKeyTimeStamp); 	writer.String(CUtilities::FormatDateTimeString(CUtilities::GetCurrentTime(), "%Y-%m-%dT%I:%M:%S.%z"));
		writer.Key(pFKeyPayloadVer);	writer.String(PAYLOAD_VERSION);
		writer.Key(pFKeyCorrelationId); writer.String(CUtilities::GetGUID());
		writer.Key(pFKeyIdempotencyId);	writer.String(CUtilities::GetGUID());

		writer.Key(pFKeyPayload);
			writer.StartObject();
			writer.Key(pFKeyMessageType);		writer.String(pProximityStatusEventData->GetMsgType());
			writer.Key(pFKeyDeviceType);		writer.String(m_deviceType);
			writer.Key(pFKeyDeviceId);			writer.String(m_deviceId);
			writer.Key(pFKeySuccess);			writer.Bool(pProximityStatusEventData->IsSuccess());
			writer.Key(pFKeyStatus);			writer.String(pProximityStatusEventData->GetStatus());
			writer.EndObject();

		writer.EndObject();
	}
	else
	{
		LogString(POLLER_LOG, "%s %d: Invalid ProximityStatusEvent Data", __FUNCTION__, __LINE__);
	}
	return writeStream.m_str;
}