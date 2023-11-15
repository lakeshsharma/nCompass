#include "stdafx.h"

#include "ProgressiveKeys.h"


const char* pKeyAccepted			    = "Accepted";
const char* pKeyAccountingDenomination  = "AccountingDenomination";
const char* pKeyAddress					= "Address";
const char* pKeyAftBonusEnabled			= "AFTBonusEnabled";
const char* pKeyAftEnabled				= "AFTEnabled";
const char* pKeyAftInHouseEnabled		= "AFTInHouseEnabled";
const char* pKeyAmount					= "Amount";
const char* pKeyAnnouncements			= "Announcements";
const char* pKeyAnnouncementContent		= "AnnouncementContent";
const char* pKeyAssetNumber				= "AssetNumber";
const char* pKeyAwardType				= "AwardType";
const char* pKeyBank					= "Bank";
const char* pKeyBonusLevelType			= "BonusLevelType";
const char* pKeyBonusLevelData			= "BonusLevelData";
const char* pKeyCampaignId				= "CampaignId"; 
const char* pKeyCampaignRevision		= "CampaignRevision";
const char* pKeyCampaigns				= "Campaigns";
const char* pKeyCardNumber				= "CardNumber";
const char* pKeyCelebrationContent		= "CelebrationContent";
const char* pKeyChannel					= "Channel";
const char* pKeyClaimableAcceptanceUrl	= "ClaimableAcceptanceUrl";
const char* pKeyClaimableExpirationMillis = "ClaimableExpirationMillis";
const char* pKeyClaimableSamePlayer		= "ClaimableSamePlayer";
const char* pKeyCompactContent			= "CompactContent";
const char* pKeyContent					= "Content";
const char* pKeyContentHttpUrl			= "ContentHttpUrl";
const char* pKeyCountdownStartTime		= "CountdownStartTime";
const char* pKeyCountdownEndTime		= "CountdownEndTime";
const char* pKeyDenomination			= "Denomination";
const char* pKeyDescription				= "Description";
const char* pKeyDeviceAddress			= "DeviceAddress";
const char* pKeyDeviceConfigVersion		= "DeviceConfigVersion";
const char* pKeyDeviceId				= "DeviceId";
const char* pKeyDeviceMAC				= "DeviceMAC";
const char* pKeyDiscoveredLevelIds		= "DiscoveredLevelIds";
const char* pKeyDownloadableFirmware	= "DownloadableFirmware";
const char* pKeyDownloadableConfigFiles = "DownloadableConfigFiles";
const char* pKeyDuration				= "Duration";
const char* pKeyEftTransferFromEnabled	= "EFTTransferFromEnabled";
const char* pKeyEftTransferToEnabled	= "EFTTransferToEnabled";
const char* pKeyEgmEventCode			= "EGMEventCode";
const char* pKeyEgmProtocolType			= "EGMProtocolType";
const char* pKeyEgmStateCode			= "EgmStateCode";
const char* pKeyEligible				= "Eligible";
const char* pKeyEligibleLevels			= "EligibleLevels";
const char* pKeyEligibilityDetermined	= "EligibilityDetermined";
const char* pKeyEnabled					= "Enabled";
const char* pKeyEnable					= "Enable";
const char* pKeyEndTimeUtc				= "EndTimeUtc";
const char* pKeyEventDescription		= "EventDescription";
const char* pKeyExpected				= "Expected";
const char* pKeyExtendedMetersEnabled	= "ExtendedMetersEnabled";
const char* pKeyExternalAwardCurrency	= "ExternalAwardCurrencySymbol";
const char* pKeyExternalAwardId			= "ExternalAwardId";
const char* pKeyExternalAwardName		= "ExternalAwardName";
const char* pKeyExternalAwardValue		= "ExternalAwardValue";
const char* pKeyFeatures				= "Features";
const char* pKeyFileName				= "FileName";
const char* pKeyFirstName				= "FirstName";
const char* pKeyFromOneLink				= "FromOneLink";
const char* pKeyGameId					= "GameId";
const char* pKeyGameName				= "GameName";
const char* pKeyGameNumber				= "GameNumber";
const char* pKeyGames					= "Games";
const char* pKeyHitValue				= "HitValue";
const char* pKeyId						= "Id";
const char* pKeyImageUrl				= "ImageUrl";
const char* pKeyImm						= "IMM";
const char* pKeyImplementationVersion	= "ImplementationVersion";
const char* pKeyInfo					= "Info";
const char* pKeyIpAddress				= "IPAddress";
const char* pKeyItems					= "Items";
const char* pKeyJackpotId				= "JackpotId";
const char* pKeyJackpotTriggerType		= "JackpotTriggerType";
const char* pKeyJackpotType				= "JackpotType";
const char* pKeyLastName				= "LastName";
const char* pKeyLeaderboardUrl			= "LeaderboardUrl";
const char* pKeyLcdContent				= "LcdContent";
const char* pKeyLegacyBonusEnabled		= "LegacyBonusEnabled";
const char* pKeyLevelId					= "LevelId";
const char* pKeyLevelName				= "LevelName";
const char* pKeyLevels					= "Levels";
const char* pKeyLevelSequence			= "LevelSequence";
const char* pKeyLevelValues				= "LevelValues";
const char* pKeyMax						= "Max";
const char* pKeyMD5						= "MD5";
const char* pKeyManagementURL			= "ManagementURL";
const char* pKeyMaxBetCredits			= "MaxBetCredits";
const char* pKeyMediaMaxSeconds			= "MediaMaxSeconds";
const char* pKeyMediaMinSeconds			= "MediaMinSeconds";
const char* pKeyMediaWindowBottomContent = "MediaWindowBottomContent";
const char* pKeyMediaWindowSideContent  = "MediaWindowSideContent";
const char* pKeyMessageId				= "MessageId";
const char* pKeyMessageRefId			= "MessageRefId";
const char* pKeyMethodOfPayment			= "MethodOfPayment";
const char* pKeyMetricName				= "MetricName";
const char* pKeyMetricValue				= "MetricValue";
const char* pKeyMin						= "Min";
const char* pKeyMinorAwardId			= "MinorAwardId";
const char* pKeyMinorAwards				= "MinorAwards";
const char* pKeyMinors					= "Minors";
const char* pKeyMultiDenomEnabled		= "MultiDenomEnabled";
const char* pKeyMulticastActive			= "MulticastActive";
const char* pKeyMulticastDestination	= "MulticastDestination";
const char* pKeyMulticastCurrentSeed	= "CurrMulticastEncryptionSeed";
const char* pKeyMulticastPreviousSeed	= "PrevMulticastEncryptionSeed";
const char* pKeyName					= "Name";
const char* pKeyOfferId				    = "OfferId";
const char* pKeyOfflineCoinInData		= "Data";
const char* pKeyOfflineCoinInType		= "Type";
const char* pKeyPaytable				= "Paytable";
const char* pKeyPaytableItemTag			= "PaytableItemTag";
const char* pKeyPaytableItemType		= "PaytableItemType";
const char* pKeyPaytableItemValue		= "PaytableItemValue";
const char* pKeyPercentEligible			= "PercentEligible";
const char* pKeyPlayerId				= "PlayerId";
const char* pKeyPlayerQualified			= "PlayerQualified";
const char* pKeyPlaylist				= "Playlist";
const char* pKeyPlaylists				= "Playlists";
const char* pKeyPokerHand				= "PokerHand";
const char* pKeyPollingActive			= "PollingActive";
const char* pKeyPrizeDescription		= "PrizeDescription";
const char* pKeyPrizeJackpot			= "PrizeJackpot";
const char* pKeyProgressiveGroup		= "ProgressiveGroup";
const char* pKeyProgressiveGroupId		= "ProgressiveGroupId";
const char* pKeyPte						= "Pte";
const char* pKeyQuarantined 			= "Quarantined";
const char* pKeyQueryCards				= "QueryCards";
const char* pKeyRebootInformation		= "RebootInformation";
const char* pKeyRemoteJackpotId			= "RemoteJackpotId";
const char* pKeyRemoteReboot			= "RemoteReboot";
const char* pKeyRestricted				= "Restricted";
const char* pKeySasDiscovery			= "SASDiscovery";
const char* pKeySasVersion				= "SASVersion";
const char* pKeySequence				= "Sequence";
const char* pKeySerialNumber			= "SerialNumber";
const char* pKeySessionActive			= "SessionActive";
const char* pKeySessionId				= "SessionId";
const char* pKeySessionPts				= "SessionPts";
const char* pKeyShowInfo				= "ShowInfo";
const char* pKeyShowOdometer			= "ShowOdometer";
const char* pKeyPromoGames				= "PromoGames";
const char* pKeyPromoGameContent		= "PromoGameContent";
const char* pKeySlotsContent			= "SlotsContent";
const char* pKeySlotsContentUrl			= "SlotsContentUrl";
const char* pKeySourceAsset				= "SourceAsset";
const char* pKeySourceDevice			= "SourceDevice";
const char* pKeySourceDeviceConfigVersion = "SourceDeviceConfigVersion";
const char* pKeySourceDeviceId			= "SourceDeviceId";
const char* pKeyStartTimeUtc			= "StartTimeUtc";
const char* pKeyStatus					= "Status";
const char* pKeyStatusInfo				= "StatusInfo";
const char* pKeySuccess					= "Success";
const char* pKeyTheoPercentage			= "TheoPercentage";
const char* pKeyTier					= "Tier";
const char* pKeyTimestamp				= "Timestamp";
const char* pKeyTransactionId			= "TransactionId";
const char* pKeyTriggerType				= "TriggerType";
const char* pKeyType					= "Type";
const char* pKeyUrl						= "Url";
const char* pKeyUseAFTLockOnTransfers	= "UseAFTLockOnTransfers";
const char* pKeyValidationExtensionsSupported = "ValidationExtensionsSupported";
const char* pKeyValidationStyle			= "ValidationStyle";
const char* pKeyValue					= "Value";
const char* pKeyVenue					= "Venue";
const char* pKeyVersion					= "Version";
const char* pKeyWager					= "Wager";
const char* pKeyWin						= "Win";
const char* pKeyWinAmount				= "WinAmount";
const char* pKeyWinnerAssetNumber		= "WinnerAssetNumber";
const char* pKeyWinnerBank				= "WinnerBank";
const char* pKeyWinnerDeviceId			= "WinnerDeviceId";
const char* pKeyWinnerLocation			= "WinnerLocation";
const char* pKeyWinnerVenue				= "WinnerVenue";
const char* pKeyWinningAssetNumber		= "WinningAssetNumber";
const char* pKeyWinningHand				= "WinningHand";
const char* pKeyPatronInformation		= "PatronInformation";
const char* pKeyEgmCommReconnect		= "EGMCommReconnect";
const char* pKeyBasePointsEarned		= "BasePointsEarned";
const char* pKeyReconnect				= "Reconnect";
const char* pKeyOSVersion				= "OSVersion";
const char* pKeyPlatform				= "Platform";
const char* pKeyJackpotAmount			= "JackpotAmount";
const char* pKeyAssignedFirmware        = "AssignedFirmware";
const char* pKeyPlatformFirmware		= "PlatformFirmware";
const char* pKeyFirmwareFiles			= "FirmwareFile";
const char* pKeyFirmwareFileName		= "FileName";
const char* pKeyFirmwareFileDownloadURL = "FileUrl";
const char* pKeyFirmwareFileVersion		= "Version";
const char* pKeyFirmwareFileChecksum	= "Signature";
const char* pKeyConfigFilename			= "FileName";
const char* pKeyConfigURL				= "FileUrl";
const char* pKeyConfigChecksum			= "MD5";
const char* pKeyCampaign								= "Campaign";
const char* pKeyCampaignVersion							= "Version";
const char* pKeyCampaignBaseURL							= "BaseDownloadUrl";
const char* pKeyCampaignMediaPackages					= "CampaignMediaPackages";
const char* pKeyCampaignEventAssignments				= "CampaignEventAssignments";
const char* pKeyCampaignEvent							= "Event";
const char* pKeyCampaignPlaylist						= "Playlist";
const char* pKeyCampaignPlaylistItems					= "Items";
const char* pKeyCampaignPlaylistItem_ActiveSunday		= "ActiveSunday";
const char* pKeyCampaignPlaylistItem_ActiveMonday		= "ActiveMonday";
const char* pKeyCampaignPlaylistItem_ActiveTuesday		= "ActiveTuesday";
const char* pKeyCampaignPlaylistItem_ActiveWednesday	= "ActiveWednesday";
const char* pKeyCampaignPlaylistItem_ActiveThursday		= "ActiveThursday";
const char* pKeyCampaignPlaylistItem_ActiveFriday		= "ActiveFriday";
const char* pKeyCampaignPlaylistItem_ActiveSaturday		= "ActiveSaturday";
const char* pKeyCampaignPlaylistItem_BeginDate			= "BeginDate";
const char* pKeyCampaignPlaylistItem_BeginTime			= "BeginTime";
const char* pKeyCampaignPlaylistItem_EndDate			= "EndDate";
const char* pKeyCampaignPlaylistItem_EndTime			= "EndTime";
const char* pKeyCampaignPlaylistItem_DurationMilliSec	= "Duration";
const char* pKeyCampaignPlaylistItem_Enabled			= "Enabled";
const char* pKeyCampaignPlaylistItem_Image				= "Image";
const char* pKeyCampaignPlaylistItem_Mute				= "Mute";
const char* pKeyCampaignPlaylistItem_SequenceId			= "SequenceId";
const char* pKeyCampaignPlaylistItem_Content			= "Content";
const char* pKeyCampaignPlaylistItem_Filename			= "FileName";
const char* pKeyCampaignPlaylistItem_Checksum			= "MD5";
const char* pKeySkinId									= "SkinId";
const char* pKeySkin									= "Skin";
const char* pKeySkinRevision							= "SkinRevision";
const char* pKeySkins									= "Skins";
const char* pKeySkinVersion								= "Version";
const char* pKeySkinChecksum							= "MD5";
const char* pKeySkinBaseURL								= "BaseDownloadUrl";
const char* pKeySkinLanguagePacks						= "LanguagePacks";
const char* pKeySkinLanguagePack_Code					= "Code";
const char* pKeySkinLanguagePack_Checksum				= "MD5";
const char* pKeySkinFiles								= "Files";
const char* pKeySkinFile_Filename						= "FileName";
const char* pKeySkinFile_Checksum						= "MD5";
const char* pKeyDynamicContentPath						= "DynamicContentPath";
const char* pKeyResolution								= "Resolution";
const char* pKeyResolutionWidth							= "Width";
const char* pKeyResolutionHeight						= "Height";
const char* pKeyResolutionRotation						= "Rotation";
const char* pKeyResolutionRefreshRate					= "RefreshRate";

const char* pValOfflineCoinInType		= "Slots.ICard.sas.OfflineCoinIn";
const char* pValSAS						= "sas";

namespace OneLinkHeader
{
	const char* pOneLinkSystemId = "OneLinkSystemId";
	const char* pType = "type";

	std::string destination("destination");
	std::string type("type");

	std::string userName("ICard");
	std::string password("");

	std::string persistentType("persistent");
	std::string nonPersistent("false");

	std::string producerDiscoveryQueueName("/queue/@discovery");
	std::string producerQueueName("/queue/@");
	std::string consumerPartialQueueName("/queue/@device.ICARD.");

	std::string trustStore("decaf.net.ssl.trustStore");

	std::string subscribeDstPrefix("/queue/@device.ICARD.");
	std::string discoveryDestination("/queue/@discovery");
	std::string sendQueueName("/queue/@");
}

namespace ContentType
{
	const char* pJson = "application/vnd.onelink.v1+json";
	const char* pJsonEnc = "application/vnd.onelink.v1+enc-json";
	const char* pXml = "soap";
}

namespace FromOneLink
{
	const char* pBonusingAvailable = "BonusingAvailable";
	const char* pClaimableAward = "ClaimableAward";
	const char* pDeviceRestartRequest = "DeviceRestartRequest";
	const char* pDiscoveryEvent = "DiscoveryEvent";
	const char* pEgmStateRequest = "EgmStateRequest";
	const char* pICardConfiguration5 = "ICardConfiguration5";
	const char* pJackpotHit = "JackpotHit";
	const char* pOfferRequest = "OfferRequest";
	const char* pPromoGameRequest = "PromoGameRequest";
	const char* pMulticastEncryptionConfiguration = "MulticastEncryptionConfiguration";
	const char* pFirmwareDownload = "FirmwareDownload";
	const char* pICardJackpotAnnouncement = "ICardJackpotAnnouncement";
	const char* pICardTotalizerAnnouncement = "ICardTotalizerAnnouncement";
	const char* pConfigFileDownload = "ConfigFileDownload";
	const char* pCampaignDownload = "CampaignDownload";
	const char* pSkinDownload = "SkinDownload";
	const char* pScheduleEnd = "ScheduleEnd";

	static EnumMap<Type>::EnumPair types[] =
	{
		{ BonusingAvailable, pBonusingAvailable },
		{ ClaimableAward, pClaimableAward },
		{ DeviceRestartRequest, pDeviceRestartRequest },
		{ DiscoveryEvent, pDiscoveryEvent },
		{ EgmStateRequest, pEgmStateRequest },
		{ ICardConfiguration5, pICardConfiguration5 },
		{ JackpotHit, pJackpotHit },
		{ OfferRequest, pOfferRequest },		
		{ PromoGameRequest, pPromoGameRequest},
		{ MulticastEncryptionConfiguration, pMulticastEncryptionConfiguration},
		{ FirmwareDownload, pFirmwareDownload},
		{ ICardJackpotAnnouncement, pICardJackpotAnnouncement},
		{ ICardTotalizerAnnouncement, pICardTotalizerAnnouncement},
		{ ConfigFileDownload, pConfigFileDownload},
		{ CampaignDownload, pCampaignDownload},
		{ SkinDownload, pSkinDownload},
		{ ScheduleEnd, pScheduleEnd},
		
	};

	EnumMap<Type> emap(types);
}

namespace ToOneLink
{
	const char* pCoinIn5 = "CoinIn5";
	const char* pDeviceAcknowledgement = "DeviceAcknowledgement";
	const char* pEgmStateResponse = "EgmStateResponse";
	const char* pEventNotification = "Event";
	const char* pGameEnd5 = "GameEnd5";
	const char* pICardConfigurationRequest5 = "ICardConfigurationRequest5";
	const char* pICardEventNotification = "ICardEvent";
	const char* pICardStatus = "ICardStatus";
	const char* pJackpotPaid = "JackpotPaid";
	const char* pOfferAcceptanceResponse = "OfferAcceptanceResponse";
	const char* pProgressiveLevelHit = "ProgressiveLevelHit";
	const char* pPromoGameAcceptanceStatus = "PromoGameAcceptanceStatus";
	const char* pPayout = "ExternalJackpotPayout";
	const char* pConfigFileDownloadAck = "ConfigFileDownloadAcknowledgement";
	const char* pCampaignDownloadRequest = "CampaignDownloadRequest";
	const char* pSkinDownloadRequest = "SkinDownloadRequest";

	static EnumMap<Type>::EnumPair types[] =
	{
		{ CoinIn5, pCoinIn5 },
		{ DeviceAcknowledgement, pDeviceAcknowledgement },
		{ EgmStateResponse, pEgmStateResponse },
		{ EventNotification, pEventNotification },
		{ GameEnd5, pGameEnd5 },
		{ ICardConfigurationRequest5, pICardConfigurationRequest5 },
		{ ICardEventNotification, pICardEventNotification },
		{ ICardStatus, pICardStatus },
		{ JackpotPaid, pJackpotPaid },
		{ OfferAcceptanceResponse, pOfferAcceptanceResponse },
		{ ProgressiveLevelHit, pProgressiveLevelHit },
		{ PromoGameAcceptanceStatus, pPromoGameAcceptanceStatus },
		{ Payout, pPayout },
		{ ConfigFileDownloadAck, pConfigFileDownloadAck},
		{ CampaignDownloadRequest, pCampaignDownloadRequest},
		{ SkinDownloadRequest, pSkinDownloadRequest }
	};

	//**************DO NOT CREATE AN ENTRY for NumberOfToOneLinkMessages**************//
	static_assert(NumberOfToOneLinkMessages == _countof(types),
		"ToOneLink::types has to be updated to match all the entries in ToOneLink::Type");

	EnumMap<Type> emap(types);
}

namespace FromOneLinkUdp
{
	const char* pLevelBroadcast = "LevelBroadcast";
	const char* pWinnerPick = "WinnerPick";
	const char* pWinnerPicked = "WinnerPicked";

	static EnumMap<Type>::EnumPair types[] =
	{
		{ LevelBroadcast, pLevelBroadcast },
		{ WinnerPick, pWinnerPick },
		{ WinnerPicked, pWinnerPicked }
	};

	EnumMap<Type> emap(types);

}

namespace JackpotTypes
{
	const char* pStandardProgressive = "StandardProgressive";
	const char* pBonus = "Bonus";
}

namespace JackpotTrigger
{
	const char* pPayLine = "PayLineTrigger";
	const char* pCoinIn = "CoinInTrigger";
	const char* pCoinInHit = "CoinInHitTrigger";
	const char* pValue = "ValueTrigger";
	const char* pFrequency = "FrequencyTrigger";
	const char* pScheduledRange = "ScheduledRangeTrigger";
	const char* pPokerFrenzy = "PokerFrenzyTrigger";
	const char* pPokerFrenzyMinor = "PokerFrenzyTriggerMinor";
	const char* pCriteria = "CriteriaTrigger";
	const char* pTimeBased = "TimeBasedTrigger";
	const char* pPromoGame = "PromoGameProgressive";
	const char* pScatter = "ScatterTrigger";
	const char* pPlayXGetYTrigger = "PlayXGetYTrigger";
	const char* pJackpotOnDemandTrigger = "JackpotOnDemandTrigger";	

	static EnumMap<Type>::EnumPair types[] =
	{
		{ PayLine, pPayLine },
		{ CoinIn, pCoinIn },
		{ CoinInHit, pCoinInHit },
		{ Value, pValue },
		{ Frequency, pFrequency },
		{ ScheduledRange, pScheduledRange },
		{ PokerFrenzy, pPokerFrenzy },
		{ PokerFrenzyMinor, pPokerFrenzyMinor },
		{ Criteria, pCriteria },
		{ TimeBased, pTimeBased },
		{ PromoGame, pPromoGame },
		{ Scatter, pScatter },
		{ PlayXGetYTrigger, pPlayXGetYTrigger },
		{ JackpotOnDemandTrigger, pJackpotOnDemandTrigger },
	};

	EnumMap<Type> emap(types);
}

namespace JackpotPaidStatus
{
	const char* pSignaled = "Signaled";
	const char* pPending = "Pending";
	const char* pIneligible = "Ineligible";
	const char* pSessionMismatch = "SessionMismatch";
	const char* pPaid = "Paid";
	const char* pFailed = "Failed";
	// Type enumerated in EGMAward.h as different values
}

namespace MethodOfPayment
{
	const char* pPayToGame = "PayToGame";
	// static const char* pPayToGameWithGameLock = "PayToGameWithGameLock"; // Obsolete with One Link version 5.5 and greater.
	const char* pPayToGameWithHandPay = "PayToGameWithHandPay";
	const char* pPayToPlayerAccount = "PayToPlayerAccount";
	const char* pTriggerOnly = "TriggerOnly";
	const char* pPayToPromoGameDevice = "PayToPromoGameDevice";
	const char* pClaimable = "Claimable";

	static EnumMap<Type>::EnumPair types[] =
	{
		{ PayToGame, pPayToGame },
		{ PayToGameWithHandPay, pPayToGameWithHandPay },
		{ PayToPlayerAccount, pPayToPlayerAccount },
		{ TriggerOnly, pTriggerOnly },
		{ PayToPromoGameDevice, pPayToPromoGameDevice },
		{ Claimable, pClaimable },
	};

	EnumMap<Type> emap(types);
}

namespace AwardType
{
	const char *pCashableCredits("CashableCredits");
	const char *pRestrictedCredits("RestrictedCredits");
	const char *pNonRestrictedCredits("NonRestrictedCredits");
	const char *pPoints("Points");
	const char *pTriggerOnly("TriggerOnly");
	const char *pFreeplay("Freeplay");
	const char *pComps("Comps");
	const char *pPrize("Prize");
	const char *pOffer("Offer");
	const char* pExtendedBucket("ExtendedBucket");

	static EnumMap<Type>::EnumPair types[] =
	{
		{ CashableCredits, pCashableCredits },
		{ RestrictedCredits, pRestrictedCredits },
		{ NonRestrictedCredits, pNonRestrictedCredits },
		{ Points, pPoints },
		{ TriggerOnly, pTriggerOnly },
		{ Freeplay, pFreeplay },
		{ Comps, pComps },
		{ Prize, pPrize },
		{ Offer, pOffer },
		{ ExtendedBucket, pExtendedBucket },
	};

	EnumMap<Type> emap(types);
}

namespace ICardAnnouncementType
{
	const char* pJackpotAnnouncement("JackpotAnnouncement");
	const char* pTotalizerAnnouncement("TotalizerAnnouncement");

	static EnumMap<Type>::EnumPair types[] =
	{
		{ JackpotAnnouncement, pJackpotAnnouncement },
		{ TotalizerAnnouncement, pTotalizerAnnouncement },
	};

	EnumMap<Type> emap(types);
}

namespace ProgressiveLevelType
{
	const char* pStandard __attribute__((weak)) = "Standard";
	const char* pBonus __attribute__((weak)) = "Bonus";
	const char* pAccountingOnly __attribute__((weak)) = "AccountingOnly";
	const char* pPalcomX __attribute__((weak)) = "PalcomX";

	static EnumMap<Type>::EnumPair types[] =
	{
		{ Standard, pStandard },
		{ Bonus, pBonus },
		{ AccountingOnly, pAccountingOnly }
	};

	EnumMap<Type> emap(types);
}

namespace BonusPaymentMethod
{
	const char* pSasLegacyBonus = "SasLegacyBonus";
	const char* pSasAFTBonus = "SasAFTBonus";
	const char* pSasAFTInHouseBonus = "SasAFTInHouseBonus";
	const char* pSasEFTBonus = "SasEFTBonus";
	const char* pMS25Mystery = "MS25Mystery";
	const char* pASPBonus = "ASPBonus";
}

namespace PteStatus
{
	const char* pInactive = "Inactive";
	const char* pStartCountdown = "StartCountdown";
	const char* pActive = "Active";
	const char* pEndCountdown = "EndCountdown";
	const char* pResults = "Results";

	static EnumMap<Type>::EnumPair types[] =
	{
		{ Inactive, pInactive },
		{ StartCountdown, pStartCountdown },
		{ Active, pActive },
		{ EndCountdown, pEndCountdown },
		{ Results, pResults }
	};

	EnumMap<Type> emap(types);
}



namespace SlotsContentType
{
	const char *pAnticipationContent("AnticipationContent");
	const char *pCelebrationContent("CelebrationContent");
	const char *pOdometerContent("OdometerContent");
	const char *pLevelUnlockedContent("LevelUnlockedContent");
	const char *pEndingSoonContent("EndingSoonContent");
	const char *pLeaderboardContent("LeaderboardContent");
	const char *pFinalResultsContent("FinalResultsContent");
	const char *pPokerMinorCelebrationContent("PokerMinorCelebrationContent");
	const char *pPokerMinorPromoGameContent("PokerMinorPromoGameContent");
	const char *pPromoGameContent("PromoGameContent");
	const char* pJackpotAnnouncementContent("JackpotAnnouncementContent");
	const char *pTotalizerAnnouncementContent("TotalizerAnnouncement");
	const char* pScheduleEndContent("ScheduleEndContent");
	
	const char *pUnknown("Unknown");

	static EnumMap<Type>::EnumPair types[] =
	{
		{ Anticipation, pAnticipationContent },
		{ Celebration, pCelebrationContent },
		{ Odometer, pOdometerContent },
		{ PTE_LevelUnlocked, pLevelUnlockedContent },
		{ PTE_EndingSoon, pEndingSoonContent },
		{ PTE_Leaderboard, pLeaderboardContent },
		{ PTE_FinalResults, pFinalResultsContent },
		{ PokerMinorCelebration, pPokerMinorCelebrationContent },
		{ PokerMinorPromoGame, pPokerMinorPromoGameContent },
		{ PromoGame, pPromoGameContent },
		{ JackpotAnnouncement, pJackpotAnnouncementContent },
		{ TotalizerAnnouncement, pTotalizerAnnouncementContent },
		{ ScheduleEnd, pScheduleEndContent },
		{ Unknown, pUnknown },
	};

	EnumMap<Type> emap(types);
}

namespace PromoGameStatus
{
	const char* pAccepted = "Accepted";
	const char* pDeclined = "Declined";
	const char* pFailed = "Failed";

	static EnumMap<Type>::EnumPair types[] =
	{
		{ Accepted, pAccepted },
		{ Declined, pDeclined },
		{ Failed, pFailed }
	};

	EnumMap<Type> emap(types);
}

namespace OneLinkAwardType
{
	const char *pPayToGameAward("PayToGameAward");
	const char *pPrizeAward("PrizeAward");
	const char *pPlayerAcountAward("PlayerAcountAward");

	static EnumMap<Type>::EnumPair types[] =
	{
		{ PayToGameAward, pPayToGameAward },
		{ PrizeAward, pPrizeAward },
		{ PlayerAcountAward, pPlayerAcountAward },
	};

	EnumMap<Type> emap(types);
}


namespace OneLinkAwardStatus
{
	const char *pPayToGameAwardFailed("PayToGameAwardFailed");
	const char *pPayToGameAwardSuccessful("PayToGameAwardSuccessful");
	const char *pPrizeAwardClearedByEmployee("PrizeAwardClearedByEmployee");
	const char *pPrizeAwardClearedByOneLink("PrizeAwardClearedByOneLink");
	const char *pPlayerAcountAwardFailed("PlayerAcountAwardFailed");
	const char *pPlayerAcountAwardSuccessful("PlayerAcountAwardSuccessful");
	const char *pInvalid("Invalid");
	

	static EnumMap<Type>::EnumPair types[] =
	{
		{ PayToGameAwardFailed, pPayToGameAwardFailed },
		{ PayToGameAwardSuccessful, pPayToGameAwardSuccessful },
		{ PrizeAwardClearedByEmployee, pPrizeAwardClearedByEmployee },
		{ PrizeAwardClearedByOneLink, pPrizeAwardClearedByOneLink },
		{ PlayerAcountAwardFailed, pPlayerAcountAwardFailed },
		{ PlayerAcountAwardSuccessful, pPlayerAcountAwardSuccessful },
		{ Invalid, pInvalid },
	};

	EnumMap<Type> emap(types);
}