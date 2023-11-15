#include "stdafx.h"

#include "ProgressiveMessageFactory.h"
#include "RapidJsonHelper.h"
#include "ProgressiveKeys.h"
#include "Logging/LogString.h"
#include "Util/Convert.h"
#include "Platform.h"
#include "Payout.h"

using namespace std;
using namespace rapidjson;

static const int MessageIdBufferLen = 64;
static const char MacAddressDash = '-';
static const char MacAddressColon = ':';

const char* CProgressiveMessageFactory::pWebDiagnosticsProtocol = "http://";
const char* CProgressiveMessageFactory::pWebDiagnosticsPage = "/nCompass.html";

CProgressiveMessageFactory::CProgressiveMessageFactory(CConnection& connection, const std::string &firmwareVersion)
	: m_isPopulatedFromDiscovery(false)
	, m_actualIP(connection.GetActualIP().GetIP())
	, m_macAddress(connection.GetMacAddress())
	, m_sourceDeviceID(0)
	, m_sourceDeviceConfigVersion(0)
	, m_sentinelVersion(CUtilities::GetEXEVersion())
	, m_messageCounter(0)
	, m_slotNumber(connection.GetSlotNumber())
	, m_firmwareVersion(firmwareVersion)
	, m_osVersion(CPlatform::Instance().GetOSImageVersionNumber())
{
}

CProgressiveMessageFactory::~CProgressiveMessageFactory()
{
}

string CProgressiveMessageFactory::GetMacAddressWithColon() const
{
	string macAddressWithColon(m_macAddress);
	replace(macAddressWithColon.begin(), macAddressWithColon.end(), MacAddressDash, MacAddressColon);
	return macAddressWithColon;
}

string CProgressiveMessageFactory::GetMacAddressWithoutColon() const
{
	string macAddressWithoutColon(m_macAddress);
	macAddressWithoutColon.erase(remove(macAddressWithoutColon.begin(), macAddressWithoutColon.end(), MacAddressDash), macAddressWithoutColon.end());
	return macAddressWithoutColon;
}

LONG64 CProgressiveMessageFactory::GetSourceDeviceID() const
{
	return m_sourceDeviceID;
}

bool CProgressiveMessageFactory::IsPopulatedFromDiscovery()
{
	return m_isPopulatedFromDiscovery;
}

void CProgressiveMessageFactory::SetQueryPokerCardsSet(set<DWORD>& queryPokerCardsSet)
{
	m_queryPokerCardsSet = queryPokerCardsSet;
}

const string& CProgressiveMessageFactory::GetActualIP() const
{
	return m_actualIP;
}

bool CProgressiveMessageFactory::SetActualIP(const string& actualIP)
{
	bool changed = m_actualIP != actualIP;
	if (changed)
	{
		LogString(PROGRESSIVE, "IP changed from %s to %s.", m_actualIP.c_str(), actualIP.c_str());
		m_actualIP = actualIP;
	}
	return changed;
}

std::string CProgressiveMessageFactory::GetDeviceAttributesString()
{
	// "SourceDeviceConfigVersion":145,"SourceDeviceId":63,"SourceDevice":"@device.ICARD.0060E06C8764","SourceAsset":"88884444","Timestamp",1341234342,
	stringstream ss;
	ss << "\"" << pKeySourceDeviceConfigVersion << "\":" << m_sourceDeviceConfigVersion << ",";
	ss << "\"" << pKeySourceDeviceId << "\":" << m_sourceDeviceID << ",";
	ss << "\"" << pKeySourceDevice << "\":\"" << m_sourceDevice << "\",";
	ss << "\"" << pKeySourceAsset << "\":\"" << m_sourceAsset << "\",";
	ss << "\"" << pKeyTimestamp << "\":" << CUtilities::MillisecondsSince1970() << ",";
	return ss.str();

}

void CProgressiveMessageFactory::ClearDiscoveryData()
{
	m_isPopulatedFromDiscovery = false;
	m_sourceDeviceID = 0;
	m_sourceDevice.clear();
	m_sourceAsset.clear();
}

void CProgressiveMessageFactory::WriteGameData(const CGame* pGame, JsonWriter& writer, unsigned gameNumber)
{
	writer.StartObject();

	writer.Key(pKeyGameNumber);		writer.Uint(gameNumber);
	writer.Key(pKeyDenomination);	writer.Double((double) pGame->GetDenomMultiplier() / ONE_DOLLAR_IN_CENTS);
	writer.Key(pKeyMaxBetCredits);	writer.Int64(pGame->GetMaxBet());
	writer.Key(pKeyTheoPercentage);	writer.Double((double) pGame->GetTheoreticalPayback() / ONE_DOLLAR_IN_CENTS);
	writer.Key(pKeyGameName);		writer.String(pGame->GetMfgID());
	writer.Key(pKeyPaytable);		writer.String(pGame->GetPaytableID());
	writer.Key(pKeyDiscoveredLevelIds);	writer.String(pGame->GetLevelIdsString());

	writer.EndObject();
}

string CProgressiveMessageFactory::CreateICardConfigurationRequest5(const CGames* pGames)
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();

	writer.Key(pKeyGames);
	writer.StartArray();

	if (pGames->GetGameCount() > 1)
	{
		for (int i = 0; i < pGames->GetGameCount(); i++)
		{
			const CGame* pGame = pGames->GetGameByIndex(i);
			if (pGame->GetGameNumber() != EGM_GAME_NUMBER)
				WriteGameData(pGame, writer, pGame->GetGameNumber());
		}
	}
	else if (pGames->GetGameCount() == 1)
	{
		// OneLink ignores game number 0.
		unsigned gameNumber = pGames->GetGameByIndex(0)->GetGameNumber();
		WriteGameData(pGames->GetGameByIndex(0), writer, gameNumber == 0 ? SINGLE_GAME_NUMBER : gameNumber);
	}

	writer.EndArray();

	writer.Key(pKeySasDiscovery);
	writer.StartObject();
	writer.Key(pKeyAddress);						writer.Uint(pGames->GetAddress());
	writer.Key(pKeyGameId);							writer.String(pGames->GetGameId());
	writer.Key(pKeySasVersion);						writer.String(pGames->GetSASVersion());
	writer.Key(pKeySerialNumber);					writer.String(pGames->GetSerialNumber());
	writer.Key(pKeyAssetNumber);					writer.String(to_string((int64_t) m_slotNumber));
	writer.Key(pKeyAccountingDenomination);			writer.Currency(pGames->GetAccountingDenom());
	writer.Key(pKeyMultiDenomEnabled);				writer.Bool(pGames->GetMultiDenomEnabled());
	writer.Key(pKeyExtendedMetersEnabled);			writer.Bool(pGames->GetExtendedMetersEnabled());
	writer.Key(pKeyAftEnabled);						writer.Bool(pGames->GetAftEnabled());
	writer.Key(pKeyAftBonusEnabled);				writer.Bool(pGames->GetAftBonusEnabled());
	writer.Key(pKeyAftInHouseEnabled);				writer.Bool(pGames->GetAftInHouseEnabled());
	writer.Key(pKeyLegacyBonusEnabled);				writer.Bool(pGames->GetLegacyBonusEnabled());
	writer.Key(pKeyEftTransferToEnabled);			writer.Bool(pGames->GetEftTransferFromEnabled());
	writer.Key(pKeyEftTransferFromEnabled);			writer.Bool(pGames->GetEftTransferToEnabled());
	writer.Key(pKeyValidationExtensionsSupported);	writer.Bool(pGames->GetValidationExtensionsEnabled());
	writer.Key(pKeyProgressiveGroup);				writer.Uint(pGames->GetProgressiveGroup());
	writer.EndObject();

	LogString(PROGRESSIVE, "ICardConfigurationRequest5.SASDiscovery->ProgressiveGroup=%u", pGames->GetProgressiveGroup());

	writer.Key(pKeyFeatures);
	writer.StartObject();
	writer.Key(pKeyPlaylists);					writer.Bool(true);
	writer.Key(pKeyRemoteReboot);				writer.Bool(true);
	writer.Key(pKeyPromoGames);					writer.Bool(true);	
	writer.Key(pKeyPrizeJackpot);				writer.Bool(true);
	writer.Key(pKeySlotsContent);				writer.Bool(true);
	writer.Key(pKeyDownloadableFirmware);		writer.Bool(true);
	writer.Key(pKeyDownloadableConfigFiles);	writer.Bool(true);
	writer.Key(pKeyCampaigns);					writer.Bool(true);
	writer.Key(pKeySkins);						writer.Bool(true);
	writer.EndObject();

	writer.Key(pKeyDeviceMAC);				writer.String(GetMacAddressWithColon());
	writer.Key(pKeyImplementationVersion); 	writer.String(m_sentinelVersion);
	writer.Key(pKeyOSVersion);				writer.String(m_osVersion);
	writer.Key(pKeyPlatform);				writer.String(CPlatform::Instance().GetPlatformName());

	writer.Key(pKeyPlatformFirmware);
	writer.StartArray();

	writer.StartObject();
	writer.Key(pKeyName); 				writer.String(FIRMWARE_BUNDLE_OS);
	writer.Key(pKeyVersion); 			writer.String(m_osVersion);
	writer.EndObject();

	writer.StartObject();
	writer.Key(pKeyName); 				writer.String(FIRMWARE_BUNDLE_EXE);
	writer.Key(pKeyVersion); 			writer.String(m_sentinelVersion);
	writer.EndObject();

	writer.StartObject();
	writer.Key(pKeyName); 				writer.String(FIRMWARE_BUNDLE_FPGA);
	writer.Key(pKeyVersion); 			writer.String(m_firmwareVersion);
	writer.EndObject();

	writer.EndArray();

	writer.Key(pKeyIpAddress); 				writer.String(m_actualIP);
	writer.Key(pKeyManagementURL); 			writer.String(string(pWebDiagnosticsProtocol) + m_actualIP);

	PopulateDeviceAttributes(writer);

	writer.EndObject();

	return writeStream.m_str;
}

string CProgressiveMessageFactory::CreateOfflineCoinInEvent(__int64 coinIn)
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key(pKeyOfflineCoinInType);	writer.String(pValOfflineCoinInType);
	writer.Key(pKeyOfflineCoinInData); 	writer.String(to_string(coinIn));
	PopulateDeviceAttributes(writer);
	writer.EndObject();

	return writeStream.m_str;
}

string CProgressiveMessageFactory::CreateICardEvent(GeneralPollException generalPollException)
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key(pKeyEgmProtocolType);	writer.String(pValSAS);
	writer.Key(pKeyEgmEventCode); 		writer.Int64((int64_t) generalPollException);
	writer.Key(pKeyEventDescription); 	writer.String(m_sasGeneralException.GetDescriptionFor(generalPollException));
	PopulateDeviceAttributes(writer);
	writer.EndObject();

	return writeStream.m_str;
}

string CProgressiveMessageFactory::CreateDeviceAcknowledgement(const char* pMessageRefId, bool success, const char* pDescription)
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();

	if (pMessageRefId != nullptr)
	{
		writer.Key(pKeyMessageRefId);
		writer.String(pMessageRefId);
	}

	writer.Key(pKeySuccess);
	writer.Bool(success);

	if (pDescription != nullptr)
	{
		writer.Key(pKeyDescription);
		writer.String(pDescription);
	}

	PopulateDeviceAttributes(writer);

	writer.EndObject();

	return writeStream.m_str;
}

string CProgressiveMessageFactory::CreateJackpotPaid(const CEGMAward& egmAward)
{
	JsonWriteStreamString writeStream;

	const char* pJackpotPaidStatus = egmAward.GetJackpotPaidStatus();
	if (pJackpotPaidStatus != nullptr)
	{
		JsonWriter writer(writeStream);

		writer.StartObject();
		writer.Key(pKeyLevelId);			writer.Int(egmAward.GetLevelId());
		writer.Key(pKeyJackpotId);			writer.Int64(egmAward.GetJackpotId());
		writer.Key(pKeyRemoteJackpotId);	writer.Int64(egmAward.GetRemoteJackpotId());
		writer.Key(pKeyStatus);				writer.String(pJackpotPaidStatus);
		writer.Key(pKeyAmount);				writer.Currency(egmAward.GetAwardAmount());
		string statusInfo = egmAward.GetStatusInfo();
		if (statusInfo.length() > 0)
		{
			writer.Key(pKeyStatusInfo);
			writer.String(statusInfo);
		}
		writer.Key(pKeyWinningAssetNumber);	writer.Int(m_slotNumber);
		PopulateDeviceAttributes(writer);
		writer.EndObject();
	}

	return writeStream.m_str;
}

string CProgressiveMessageFactory::CreateProgressiveLevelHit(const CEGMAward& egmAward, bool reconnect)
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	writer.StartObject();
	writer.Key(pKeyLevelId);	writer.Int64(egmAward.GetLevelId());
	writer.Key(pKeyHitValue);	writer.Currency(egmAward.GetAwardAmountHit());
	writer.Key(pKeyReconnect);	writer.Bool(reconnect);
	PopulateDeviceAttributes(writer);
	writer.EndObject();

	LogString(PROGRESSIVE, "CProgressiveMessageFactory::CreateProgressiveLevelHit: SourceDeviceID:%lld - SourceDevice:%s - SourceAsset:%s - Level:%d - Amount:%lld, Reconnect:%d",
		m_sourceDeviceID, m_sourceDevice.c_str(), m_sourceAsset.c_str(), egmAward.GetLevelId(), egmAward.GetAwardAmountHit(), reconnect);

	return writeStream.m_str;
}

string CProgressiveMessageFactory::CreateEgmStateResponse(bool enabled)
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	writer.StartObject();
	writer.Key(pKeySuccess); writer.Bool(TRUE);
	writer.Key(pKeyEgmStateCode); writer.String(pKeyEnabled);
	writer.EndObject();
	LogString(PROGRESSIVE, "CProgressiveMessageFactory::CreateEgmStateResponse: Enabled:%d",
		enabled);

	return writeStream.m_str;
}

string CProgressiveMessageFactory::CreateOfferAcceptanceResponse(const CProgressiveOfferRequest& progressiveOfferRequest)
{
	LogString(PROGRESSIVE, "CProgressiveMessageFactory::CreateOfferAcceptanceResponse: JackpotId:%lld - PlayerId:%lld - OfferId:%s - GetTransactionId:%s", 
		progressiveOfferRequest.GetJackpotId(), progressiveOfferRequest.GetPlayerId(), progressiveOfferRequest.GetOfferId().c_str(), progressiveOfferRequest.GetTransactionId().c_str());
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	writer.StartObject();
	writer.Key(pKeyJackpotId);	  writer.Int64(progressiveOfferRequest.GetJackpotId());
	writer.Key(pKeyPlayerId);	  writer.Int64(progressiveOfferRequest.GetPlayerId());
	writer.Key(pKeyOfferId);	  writer.String(progressiveOfferRequest.GetOfferId());
	writer.Key(pKeyAccepted);	  writer.Bool(true);
	writer.Key(pKeyTransactionId); writer.String(progressiveOfferRequest.GetTransactionId());
	PopulateDeviceAttributes(writer);
	writer.EndObject();
	return writeStream.m_str;
}

void CProgressiveMessageFactory::WritePatronInformation(JsonWriter& writer, const CPlayer* pPlayer)
{
	if (pPlayer != nullptr)
	{
		JsonConditionalObject conditionalObject(writer, pKeyPatronInformation);

		// Either player->GetCard() or player->GetVirtualCard() is expected to return non-nullptr.
		const CCard* pCard = pPlayer->GetCard();
		if (pCard == nullptr)
			pCard = pPlayer->GetVirtualCard();

		if (pCard != nullptr)
		{
			int cardLen = 0;
			const byte* pCardNum = pCard->GetCardID(cardLen);
			if (pCardNum != nullptr && cardLen > 0)
			{
				conditionalObject.Start();
				writer.Key(pKeyCardNumber); writer.String(CUtilities::MakeStringFromBCD(pCardNum, cardLen));
			}
		}

		const CPlayerInfo* pPlayerInfo = pPlayer->GetPlayerInfo();
		if (pPlayerInfo != nullptr)
		{
			conditionalObject.Start();

			writer.Key(pKeyName); writer.String(pPlayerInfo->GetName());
			writer.Key(pKeyTier); writer.String(pPlayerInfo->GetRankName());

			if (pPlayerInfo->GetPlayerId() != CPlayerInfo::INVALID_PLAYER_ID)
			{
				writer.Key(pKeyPlayerId);
				writer.Uint(pPlayerInfo->GetPlayerId());
			}
		}
	}
}
void CProgressiveMessageFactory::WriteResolution(JsonWriter& writer)
{
	DWORD width = 0;
	DWORD height = 0;
	CUtilities::GetResolution(width, height);

	writer.Key(pKeyResolution);

	writer.StartObject();
	writer.Key(pKeyName);					writer.String("nCompass LCD");
	writer.Key(pKeyResolutionWidth);		writer.Uint64(width);
	writer.Key(pKeyResolutionHeight);		writer.Uint64(height);
	writer.Key(pKeyResolutionRotation);		writer.Uint64(0);
	writer.Key(pKeyResolutionRefreshRate);	writer.Uint64(60);
	writer.EndObject();
}

string CProgressiveMessageFactory::CreateICardStatus(
	const CPlayer* pPlayer,
	int sessionId,
	bool isPollingActive,
	bool isSessionActive,
	bool isMulticastActive,
	int *gameNumber,
	__int64 *denomination,
	long campaignId,
	long campaignRevision,
	long skinId,
	long skinRevision,
	std::string status)
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key(pKeyPollingActive);	 writer.Bool(isPollingActive);
	writer.Key(pKeySessionActive);	 writer.Bool(isSessionActive);
	writer.Key(pKeyMulticastActive); writer.Bool(isMulticastActive);
	writer.Key(pKeySessionId);		 writer.Int(sessionId);

	if (gameNumber != nullptr)
	{
		writer.Key(pKeyGameNumber);
		writer.Int(*gameNumber);
	}

	if (denomination != nullptr)
	{
		writer.Key(pKeyDenomination);
		writer.Currency(*denomination);
	}

	writer.Key(pKeyCampaignId);			writer.Int(campaignId);
	writer.Key(pKeyCampaignRevision);	writer.Int(campaignRevision);
	writer.Key(pKeySkinId);				writer.Int(skinId);
	writer.Key(pKeySkinRevision);		writer.Int(skinRevision);

	writer.Key(pKeyStatus);					writer.String(status);
	
	WriteResolution(writer);
	WritePatronInformation(writer, pPlayer);
	PopulateDeviceAttributes(writer);
	writer.EndObject();

	return writeStream.m_str;
}

void CProgressiveMessageFactory::WriteCommonGameEndData(const CGameEndData& gameData, const CPlayer* pPlayer, int sessionId, JsonWriter& writer)
{
	const CGame* pGame = gameData.GetCurrentGame();
	DWORD gameNumber = pGame->GetGameNumber();
	int currentGameDenom = gameData.GetCurrentDenom();
	if (currentGameDenom <= 0)
	{
		// No current denom, use game denom
		currentGameDenom = pGame->GetDenomMultiplier();
	}

	writer.Key(pKeyGameNumber);		writer.Uint(gameNumber);
	writer.Key(pKeyDenomination);	writer.Currency(currentGameDenom);
	writer.Key(pKeySessionId);		writer.Int(sessionId);
	if (gameData.GetPokerHand().IsValid() && m_queryPokerCardsSet.find(gameNumber) != m_queryPokerCardsSet.end())
	{
		writer.Key(pKeyPokerHand);
		writer.String(gameData.GetPokerHand().ToOneLinkString());
	}
	WritePatronInformation(writer, pPlayer);
	PopulateDeviceAttributes(writer);
}

string CProgressiveMessageFactory::CreateCoinIn5(const CGameEndData& gameData, const CPlayer* pPlayer, int sessionId, bool egmCommReconnect, bool reconnect)
{
	JsonWriteStreamString writeStream;

	const CGame* pGame = gameData.GetCurrentGame();
	if (pGame != nullptr)
	{
		JsonWriter writer(writeStream);

		writer.StartObject();
		writer.Key(pKeyWager);
		writer.Currency(gameData.GetLastPlayedCashableAmount() * pGame->GetDenomMultiplier());
		writer.Key(pKeyRestricted);
		writer.Currency(gameData.GetLastPlayedRestrictedAmount() * pGame->GetDenomMultiplier());
		WriteCommonGameEndData(gameData, pPlayer, sessionId, writer);
		writer.Key(pKeyBasePointsEarned);
		writer.Double(gameData.GetBasePointsEarned());
		writer.Key(pKeyEgmCommReconnect);
		writer.Bool(egmCommReconnect);
		writer.Key(pKeyReconnect);
		writer.Bool(reconnect);
		writer.EndObject();
	}
	return writeStream;
}

string CProgressiveMessageFactory::CreateGameEnd5(const CGameEndData& gameData, const CPlayer* pPlayer, int sessionId, bool reconnect)
{
	JsonWriteStreamString writeStream;

	const CGame* pGame = gameData.GetCurrentGame();
	if (pGame != nullptr)
	{
		JsonWriter writer(writeStream);

		writer.StartObject();
		writer.Key(pKeyWin);	writer.Currency(gameData.GetLastWinAmount() * pGame->GetDenomMultiplier());
		WriteCommonGameEndData(gameData, pPlayer, sessionId, writer);
		writer.Key(pKeyReconnect);
		writer.Bool(reconnect);
		writer.EndObject();
	}
	return writeStream;
}

string CProgressiveMessageFactory::CreatePayout(const CPayout* payout, const CPlayer* pPlayer, int sessionId)
{
	JsonWriteStreamString writeStream;

	if (payout != nullptr)
	{
		JsonWriter writer(writeStream);
		
		writer.StartObject();
		WritePatronInformation(writer, pPlayer);
		writer.Key(pKeySessionId);		
		writer.Int(sessionId);
		writer.Key(pKeyJackpotAmount);
		writer.Currency(payout->GetJackpotAmount());
		writer.Key(pKeyJackpotType);
		switch (payout->GetJackpotType())
		{
		case AttendantPaidJackpot:
			writer.String("AttendantPaidJackpot");
			break;
		case AttendantPaidProgressiveJackpot:
			writer.String("AttendantPaidProgressiveJackpot");
			break;
		default:
			writer.String("MiscJackpot");
			break;
		}
		PopulateDeviceAttributes(writer);
		writer.EndObject();
	}
	return writeStream.m_str;
}

string CProgressiveMessageFactory::CreateCampaignDownloadRequest(const DWORD campaignId)
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key(pKeyCampaignId);			writer.Uint(campaignId);
	PopulateDeviceAttributes(writer);
	writer.EndObject();
	
	return writeStream.m_str;
}

string CProgressiveMessageFactory::CreateSkinDownloadRequest(const DWORD skinId)
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key(pKeySkinId);				writer.Uint(skinId);
	PopulateDeviceAttributes(writer);
	writer.EndObject();

	return writeStream.m_str;
}

string CProgressiveMessageFactory::CreatePromoGameAcceptanceStatus(const CPromoGameAcceptanceStatus& promoGameAcceptanceStatus)
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	writer.StartObject();
	writer.Key(pKeyAccepted);			writer.String(promoGameAcceptanceStatus.GetPromoGameStatus());
	writer.Key(pKeyJackpotId);			writer.Int64(promoGameAcceptanceStatus.GetJackpotId());
	PopulateDeviceAttributes(writer);
	writer.EndObject();
	return writeStream.m_str;
}

std::string CProgressiveMessageFactory::CreateConfigFileDownloadAck(const std::string &messageRefId, const std::string &filename)
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);
	writer.StartObject();
	writer.Key(pKeySourceAsset);		writer.String(m_sourceAsset);
	writer.Key(pKeyConfigFilename);		writer.String(filename);
	writer.Key(pKeyMessageRefId);		writer.String(messageRefId);
	writer.Key(pKeyTimestamp);			writer.Int64(CUtilities::MillisecondsSince1970());
	PopulateDeviceAttributes(writer);
	writer.EndObject();
	return writeStream.m_str;
}

void CProgressiveMessageFactory::UpdateCardConfiguration(const Document& jsonDoc)
{
	m_isPopulatedFromDiscovery	= true;
	m_sourceDeviceConfigVersion = JsonGetInt64Ex (jsonDoc, pKeyDeviceConfigVersion);
	m_sourceDeviceID			= JsonGetInt64Ex (jsonDoc, pKeyDeviceId);
	m_sourceDevice				= JsonGetStringEx(jsonDoc, pKeyDeviceAddress);
	m_sourceAsset				= JsonGetStringEx(jsonDoc, pKeyAssetNumber);
}

void CProgressiveMessageFactory::WriteMessageId(JsonWriter& writer)
{
	int64_t milliseconds = CUtilities::MillisecondsSince1970();

	char messageId[MessageIdBufferLen];
	sprintf(messageId, "%lld-%u", milliseconds, m_messageCounter++);

	writer.Key(pKeyTimestamp);	writer.Int64(milliseconds);
	writer.Key(pKeyMessageId);	writer.String(messageId);
}

void CProgressiveMessageFactory::PopulateDeviceAttributes(JsonWriter& writer)
{
	writer.Key(pKeySourceDeviceConfigVersion);	writer.Int64(m_sourceDeviceConfigVersion);
	writer.Key(pKeySourceDeviceId);	writer.Int64(m_sourceDeviceID);
	writer.Key(pKeySourceDevice);	writer.String(m_sourceDevice);
	writer.Key(pKeySourceAsset);	writer.String(m_sourceAsset);
	WriteMessageId(writer);
}

