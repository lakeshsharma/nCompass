#pragma once

#include "EnumMap.h"

// This was done with __declspec(selectany) since it's critical that the variables and strings
// correlate. This has already proved invaluable when working on the JSON support.

extern const char* pKeyAccepted;
extern const char* pKeyAccountingDenomination;
extern const char* pKeyAddress;
extern const char* pKeyAftBonusEnabled;
extern const char* pKeyAftEnabled;
extern const char* pKeyAftInHouseEnabled;
extern const char* pKeyAmount;
extern const char* pKeyAnnouncements;
extern const char* pKeyAnnouncementContent;
extern const char* pKeyAssetNumber;
extern const char* pKeyAwardType;
extern const char* pKeyBank;
extern const char* pKeyBonusLevelType;
extern const char* pKeyBonusLevelData;
extern const char* pKeyCampaignId;
extern const char* pKeyCampaignRevision;
extern const char* pKeyCampaigns;
extern const char* pKeyCardNumber;
extern const char* pKeyCelebrationContent;
extern const char* pKeyChannel;
extern const char* pKeyClaimableAcceptanceUrl;
extern const char* pKeyClaimableExpirationMillis;
extern const char* pKeyClaimableSamePlayer;
extern const char* pKeyCompactContent;
extern const char* pKeyContent;
extern const char* pKeyContentHttpUrl;
extern const char* pKeyCountdownStartTime;
extern const char* pKeyCountdownEndTime;
extern const char* pKeyDenomination;
extern const char* pKeyDescription;
extern const char* pKeyDeviceAddress;
extern const char* pKeyDeviceConfigVersion;
extern const char* pKeyDeviceId;
extern const char* pKeyDeviceMAC;
extern const char* pKeyDiscoveredLevelIds;
extern const char* pKeyDownloadableFirmware;
extern const char* pKeyDownloadableConfigFiles;
extern const char* pKeyDuration;
extern const char* pKeyEftTransferFromEnabled;
extern const char* pKeyEftTransferToEnabled;
extern const char* pKeyEgmEventCode;
extern const char* pKeyEgmProtocolType;
extern const char* pKeyEgmStateCode;
extern const char* pKeyEligible;
extern const char* pKeyEligibleLevels;
extern const char* pKeyEligibilityDetermined;
extern const char* pKeyEnabled;
extern const char* pKeyEnable;
extern const char* pKeyEndTimeUtc;
extern const char* pKeyEventDescription;
extern const char* pKeyExpected;
extern const char* pKeyExtendedMetersEnabled;
extern const char* pKeyExternalAwardCurrency;
extern const char* pKeyExternalAwardId;
extern const char* pKeyExternalAwardName;
extern const char* pKeyExternalAwardValue;
extern const char* pKeyFeatures;
extern const char* pKeyFileName;
extern const char* pKeyFirstName;
extern const char* pKeyFromOneLink;
extern const char* pKeyGameId;
extern const char* pKeyGameName;
extern const char* pKeyGameNumber;
extern const char* pKeyGames;
extern const char* pKeyHitValue;
extern const char* pKeyId;
extern const char* pKeyImageUrl;
extern const char* pKeyImm;
extern const char* pKeyImplementationVersion;
extern const char* pKeyInfo;
extern const char* pKeyIpAddress;
extern const char* pKeyItems;
extern const char* pKeyJackpotId;
extern const char* pKeyJackpotTriggerType;
extern const char* pKeyJackpotType;
extern const char* pKeyLastName;
extern const char* pKeyLeaderboardUrl;
extern const char* pKeyLcdContent;
extern const char* pKeyLegacyBonusEnabled;
extern const char* pKeyLevelId;
extern const char* pKeyLevelName;
extern const char* pKeyLevels;
extern const char* pKeyLevelSequence;
extern const char* pKeyLevelValues;
extern const char* pKeyMax;
extern const char* pKeyMD5;
extern const char* pKeyManagementURL;
extern const char* pKeyMaxBetCredits;
extern const char* pKeyMediaMaxSeconds;
extern const char* pKeyMediaMinSeconds;
extern const char* pKeyMediaWindowBottomContent;
extern const char* pKeyMediaWindowSideContent;
extern const char* pKeyMessageId;
extern const char* pKeyMessageRefId;
extern const char* pKeyMethodOfPayment;
extern const char* pKeyMetricName;
extern const char* pKeyMetricValue;
extern const char* pKeyMin;
extern const char* pKeyMinorAwardId;
extern const char* pKeyMinorAwards;
extern const char* pKeyMinors;
extern const char* pKeyMultiDenomEnabled;
extern const char* pKeyMulticastActive;
extern const char* pKeyMulticastDestination;
extern const char* pKeyMulticastCurrentSeed;
extern const char* pKeyMulticastPreviousSeed;
extern const char* pKeyName;
extern const char* pKeyOfferId;
extern const char* pKeyOfflineCoinInData;
extern const char* pKeyOfflineCoinInType;
extern const char* pKeyPatronInformation;
extern const char* pKeyPaytable;
extern const char* pKeyPaytableItemTag;
extern const char* pKeyPaytableItemType;
extern const char* pKeyPaytableItemValue;
extern const char* pKeyPercentEligible;
extern const char* pKeyPlayerId;
extern const char* pKeyPlayerQualified;
extern const char* pKeyPlaylist;
extern const char* pKeyPlaylists;
extern const char* pKeyPokerHand;
extern const char* pKeyPollingActive;
extern const char* pKeyPrizeDescription;
extern const char* pKeyPrizeJackpot;
extern const char* pKeyProgressiveGroup;
extern const char* pKeyProgressiveGroupId;
extern const char* pKeyPte;
extern const char* pKeyQueryCards;
extern const char* pKeyRebootInformation;
extern const char* pKeyRemoteJackpotId;
extern const char* pKeyRemoteReboot;
extern const char* pKeyRestricted;
extern const char* pKeySasDiscovery;
extern const char* pKeySasVersion;
extern const char* pKeySequence;
extern const char* pKeySerialNumber;
extern const char* pKeySessionActive;
extern const char* pKeySessionId;
extern const char* pKeySessionPts;
extern const char* pKeyShowInfo;
extern const char* pKeyShowOdometer;
extern const char* pKeyPromoGames;
extern const char* pKeyPromoGameContent;
extern const char* pKeyQuarantined;
extern const char* pKeySlotsContent;
extern const char* pKeySlotsContentUrl;
extern const char* pKeySourceAsset;
extern const char* pKeySourceDevice;
extern const char* pKeySourceDeviceConfigVersion;
extern const char* pKeySourceDeviceId;
extern const char* pKeyStartTimeUtc;
extern const char* pKeyStatus;
extern const char* pKeyStatusInfo;
extern const char* pKeySuccess;
extern const char* pKeyTheoPercentage;
extern const char* pKeyTier;
extern const char* pKeyTimestamp;
extern const char* pKeyTransactionId;
extern const char* pKeyTriggerType;
extern const char* pKeyType;
extern const char* pKeyUrl;
extern const char* pKeyUseAFTLockOnTransfers;
extern const char* pKeyValidationExtensionsSupported;
extern const char* pKeyValidationStyle;
extern const char* pKeyValue;
extern const char* pKeyVenue;
extern const char* pKeyVersion;
extern const char* pKeyWager;
extern const char* pKeyWin;
extern const char* pKeyWinAmount;
extern const char* pKeyWinnerAssetNumber;
extern const char* pKeyWinnerBank;
extern const char* pKeyWinnerDeviceId;
extern const char* pKeyWinnerLocation;
extern const char* pKeyWinnerVenue;
extern const char* pKeyWinningAssetNumber;
extern const char* pKeyWinningHand;
extern const char* pValOfflineCoinInType;
extern const char* pValSAS;
extern const char* pKeyEgmCommReconnect;
extern const char* pKeyBasePointsEarned;
extern const char* pKeyReconnect;
extern const char* pKeyOSVersion;
extern const char* pKeyPlatform;
extern const char* pKeyJackpotAmount;
extern const char* pKeyAssignedFirmware;
extern const char* pKeyPlatformFirmware;
extern const char* pKeyFirmwareFiles;
extern const char* pKeyFirmwareFileName;
extern const char* pKeyFirmwareFileDownloadURL;
extern const char* pKeyFirmwareFileVersion;
extern const char* pKeyFirmwareFileChecksum;
extern const char* pKeyConfigFilename;
extern const char* pKeyConfigURL;
extern const char* pKeyConfigChecksum;
extern const char* pKeyCampaign;
extern const char* pKeyCampaignVersion;
extern const char* pKeyCampaignBaseURL;
extern const char* pKeyCampaignMediaPackages;
extern const char* pKeyCampaignEventAssignments;
extern const char* pKeyCampaignEvent;
extern const char* pKeyCampaignPlaylist;
extern const char* pKeyCampaignPlaylistItems;
extern const char* pKeyCampaignPlaylistItem_ActiveSunday;
extern const char* pKeyCampaignPlaylistItem_ActiveMonday;
extern const char* pKeyCampaignPlaylistItem_ActiveTuesday;
extern const char* pKeyCampaignPlaylistItem_ActiveWednesday;
extern const char* pKeyCampaignPlaylistItem_ActiveThursday;
extern const char* pKeyCampaignPlaylistItem_ActiveFriday;
extern const char* pKeyCampaignPlaylistItem_ActiveSaturday;
extern const char* pKeyCampaignPlaylistItem_BeginDate;
extern const char* pKeyCampaignPlaylistItem_BeginTime;
extern const char* pKeyCampaignPlaylistItem_EndDate;
extern const char* pKeyCampaignPlaylistItem_EndTime;
extern const char* pKeyCampaignPlaylistItem_DurationMilliSec;
extern const char* pKeyCampaignPlaylistItem_Enabled;
extern const char* pKeyCampaignPlaylistItem_Image;
extern const char* pKeyCampaignPlaylistItem_Mute;
extern const char* pKeyCampaignPlaylistItem_SequenceId;
extern const char* pKeyCampaignPlaylistItem_Content;
extern const char* pKeyCampaignPlaylistItem_Filename;
extern const char* pKeyCampaignPlaylistItem_Checksum;
extern const char* pKeySkinId;
extern const char* pKeySkin;
extern const char* pKeySkinRevision;
extern const char* pKeySkins;
extern const char* pKeySkinVersion;
extern const char* pKeySkinChecksum;
extern const char* pKeySkinBaseURL;
extern const char* pKeySkinLanguagePacks;
extern const char* pKeySkinLanguagePack_Code;
extern const char* pKeySkinLanguagePack_Checksum;
extern const char* pKeySkinFiles;
extern const char* pKeySkinFile_Filename;
extern const char* pKeySkinFile_Checksum;
extern const char* pKeyDynamicContentPath;
extern const char* pKeyResolution;
extern const char* pKeyResolutionWidth;
extern const char* pKeyResolutionHeight;
extern const char* pKeyResolutionRotation;
extern const char* pKeyResolutionRefreshRate;

namespace OneLinkHeader
{
	extern const char* pOneLinkSystemId;
	extern const char* pType;

	extern std::string destination;
	extern std::string type;

	extern std::string userName;
	extern std::string password;

	extern std::string persistentType;
	extern std::string nonPersistent;

	extern std::string producerDiscoveryQueueName;
	extern std::string producerQueueName;
	extern std::string consumerPartialQueueName;

	extern std::string trustStore;

	extern std::string subscribeDstPrefix;
	extern std::string discoveryDestination;
	extern std::string sendQueueName;
}

namespace ContentType
{
	extern const char* pJson;
	extern const char* pJsonEnc;
	extern const char* pXml;
}

namespace FromOneLink
{
	extern const char* pBonusingAvailable;
	extern const char* pClaimableAward;
	extern const char* pDeviceRestartRequest;
	extern const char* pDiscoveryEvent;
	extern const char* pEgmStateRequest;
	extern const char* pICardConfiguration5;
	extern const char* pJackpotHit;
	extern const char* pOfferRequest;	
	extern const char* pPromoGameRequest;
	extern const char* pMulticastEncryptionConfiguration;
	extern const char* pFirmwareDownload;
	extern const char* pICardJackpotAnnouncement;
	extern const char* pICardTotalizerAnnouncement;
	extern const char* pConfigFileDownload;
	extern const char* pCampaignDownload;
	extern const char* pSkinDownload;
	extern const char* pScheduleEnd;
	
	enum Type
	{
		BonusingAvailable,
		ClaimableAward,
		DeviceRestartRequest,
		DiscoveryEvent,
		EgmStateRequest,
		ICardConfiguration5,
		JackpotHit,
		OfferRequest,
		PromoGameRequest,
		MulticastEncryptionConfiguration,
		FirmwareDownload,
		ICardJackpotAnnouncement,
		ICardTotalizerAnnouncement,
		ConfigFileDownload,
		CampaignDownload,
		SkinDownload,
		ScheduleEnd
	};

	extern EnumMap<Type> emap;
}

namespace ToOneLink
{
	extern const char* pCoinIn5;
	extern const char* pDeviceAcknowledgement;
	extern const char* pEgmStateResponse;
	extern const char* pEventNotification;
	extern const char* pGameEnd5;
	extern const char* pICardConfigurationRequest5;
	extern const char* pICardEventNotification;
	extern const char* pICardStatus;
	extern const char* pJackpotPaid;
	extern const char* pOfferAcceptanceResponse;
	extern const char* pProgressiveLevelHit;
	extern const char* pPromoGameAcceptanceStatus;
	extern const char* pConfigFileDownloadAck;


	enum Type
	{
		CoinIn5 = 0,
		DeviceAcknowledgement,
		EgmStateResponse,
		EventNotification,
		GameEnd5,
		ICardConfigurationRequest5,
		ICardEventNotification,
		ICardStatus,
		JackpotPaid,
		OfferAcceptanceResponse,
		ProgressiveLevelHit,
		PromoGameAcceptanceStatus,
		Payout,
		ConfigFileDownloadAck,
		CampaignDownloadRequest,
		SkinDownloadRequest,
		NumberOfToOneLinkMessages//always make this last
	};

	extern EnumMap<Type> emap;

}

namespace FromOneLinkUdp
{
	extern const char* pLevelBroadcast;
	extern const char* pWinnerPick;
	extern const char* pWinnerPicked;

	enum Type
	{
		LevelBroadcast = 0,
		WinnerPick,
		WinnerPicked,
		Invalid = -1
	};

	extern EnumMap<Type> emap;
}

namespace JackpotTypes
{
	extern const char* pStandardProgressive;
	extern const char* pBonus;
}

namespace JackpotTrigger
{
	extern const char* pPayLine;
	extern const char* pCoinIn;
	extern const char* pCoinInHit;
	extern const char* pValue;
	extern const char* pFrequency;
	extern const char* pScheduledRange;
	extern const char* pPokerFrenzy;
	extern const char* pPokerFrenzyMinor;
	extern const char* pCriteria;
	extern const char* pTimeBased;
	extern const char* pPromoGame;
	extern const char* pScatter;
	extern const char* pPlayXGetYTrigger;
	extern const char* pJackpotOnDemandTrigger;	

	enum Type
	{
		PayLine = 0,
		CoinIn,
		CoinInHit,
		Value,
		Frequency,
		ScheduledRange,
		PokerFrenzy,
		PokerFrenzyMinor,
		Criteria,
		TimeBased,
		PromoGame,
		Scatter,
		PlayXGetYTrigger,
		JackpotOnDemandTrigger,
		Invalid = -1
	};

	extern EnumMap<Type> emap;
}

namespace JackpotPaidStatus
{
	extern const char* pSignaled;
	extern const char* pPending ;
	extern const char* pIneligible;
	extern const char* pSessionMismatch;
	extern const char* pPaid ;
	extern const char* pFailed;

	// Type enumerated in EGMAward.h as different values
}

namespace MethodOfPayment
{
	extern const char* pPayToGame;
	// static const char* pPayToGameWithGameLock = "PayToGameWithGameLock"; // Obsolete with One Link version 5.5 and greater.
	extern const char* pPayToGameWithHandPay;
	extern const char* pPayToPlayerAccount;
	extern const char* pTriggerOnly;
	extern const char* pPayToPromoGameDevice;
	extern const char* pClaimable;

	enum Type
	{
		PayToGame = 0,
		PayToGameWithGameLock_Deprecated,
		PayToGameWithHandPay,
		PayToPlayerAccount,
		TriggerOnly,
		PayToPromoGameDevice,
		Claimable,

		Invalid = -1
	};

	extern EnumMap<Type> emap;
}

namespace AwardType
{
	extern const char *pCashableCredits;
	extern const char *pRestrictedCredits;
	extern const char *pNonRestrictedCredits;
	extern const char *pPoints;
	extern const char *pTriggerOnly;
	extern const char *pFreeplay;
	extern const char *pComps;
	extern const char *pPrize;
	extern const char *pOffer;
	extern const char* pExtendedBucket;

	enum Type
	{
		CashableCredits=0,
		RestrictedCredits,
		NonRestrictedCredits,
		Points,
		TriggerOnly,
		Freeplay,
		Comps,
		Prize,
		Offer,
		ExtendedBucket,
		Invalid = -1
	};

	extern EnumMap<Type> emap;
}

namespace ICardAnnouncementType
{
	extern const char* pJackpotAnnouncement;
	extern const char* pTotalizerAnnouncement;

	enum Type
	{
		JackpotAnnouncement,
		TotalizerAnnouncement,
		Invalid = -1
	};

	extern EnumMap<Type> emap;
}

namespace ProgressiveLevelType
{
	extern const char* pStandard __attribute__((weak));
	extern const char* pBonus __attribute__((weak));
	extern const char* pAccountingOnly __attribute__((weak));
	extern const char* pPalcomX __attribute__((weak));

	enum Type
	{
		Standard,
		Bonus,
		AccountingOnly,
		PalcomX,
		Invalid = -1
	};

	extern EnumMap<Type> emap;
}

namespace BonusPaymentMethod
{
	extern const char* pSasLegacyBonus;
	extern const char* pSasAFTBonus;
	extern const char* pSasAFTInHouseBonus;
	extern const char* pSasEFTBonus;
	extern const char* pMS25Mystery;
	extern const char* pASPBonus;

	enum Type
	{
		SasLegacyBonus=0,
		SasAFTBonus,
		SasAFTInHouseBonus,
		SasEFTBonus,
		MS25Mystery,
		ASPBonus,
		Invalid = -1
	};
	extern EnumMap<Type> emap;
}

namespace PteStatus
{
	extern const char* pInactive;
	extern const char* pStartCountdown;
	extern const char* pActive;
	extern const char* pEndCountdown;
	extern const char* pResults;

	enum Type
	{
		Inactive=0,
		StartCountdown,
		Active,
		EndCountdown,
		Results,
		Invalid = -1
	};

	extern EnumMap<Type> emap;
}

namespace SlotsContentType
{
	extern const char *pAnticipationContent;
	extern const char *pCelebrationContent;
	extern const char *pOdometerContent;
	extern const char *pLevelUnlockedContent;
	extern const char *pEndingSoonContent;
	extern const char *pLeaderboardContent;
	extern const char *pFinalResultsContent;
	extern const char *pPokerMinorCelebrationContent;
	extern const char *pPokerMinorPromoGameContent;
	extern const char *pPromoGameContent;
	extern const char* pJackpotAnnouncementContent;
	extern const char *pTotalizerAnnouncementContent;
	extern const char* pScheduleEndContent;
	extern const char *pUnknown;

	enum Type
	{
		Anticipation = 0,
		Celebration,
		Odometer,
		PTE_LevelUnlocked,
		PTE_EndingSoon,
		PTE_Leaderboard,
		PTE_FinalResults,
		PokerMinorCelebration,
		PokerMinorPromoGame,
		PromoGame,
		JackpotAnnouncement,
		TotalizerAnnouncement,
		ScheduleEnd,
		Unknown,//Always keep Unknown and Invalid as the last 2 items
		Invalid = -1
	};

	extern EnumMap<Type> emap;
}

namespace PromoGameStatus
{
	extern const char* pAccepted;
	extern const char* pDeclined;
	extern const char* pFailed;
	enum Type
	{
		Accepted,
		Declined,
		Failed,
		Invalid = -1
	};

	extern EnumMap<Type> emap;
}

namespace OneLinkAwardType
{
	extern const char *pPayToGameAward;
	extern const char *pPrizeAward;
	extern const char *pPlayerAcountAward;

	enum Type
	{
		PayToGameAward,
		PrizeAward,
		PlayerAcountAward,
		Invalid = -1
	};
	extern EnumMap<Type> emap;
}

namespace OneLinkAwardStatus
{
	extern const char *pPayToGameAwardFailed;
	extern const char *pPayToGameAwardSuccessful;
	extern const char *pPrizeAwardClearedByEmployee;
	extern const char *pPrizeAwardClearedByOneLink;
	extern const char *pPlayerAcountAwardFailed;
	extern const char *pPlayerAcountAwardSuccessful;
	extern const char *pInvalid;

	enum Type
	{
		PayToGameAwardFailed,
		PayToGameAwardSuccessful,
		PrizeAwardClearedByEmployee,
		PrizeAwardClearedByOneLink,
		PlayerAcountAwardFailed,
		PlayerAcountAwardSuccessful,
		Invalid = -1,
	};
	extern EnumMap<Type> emap;
}