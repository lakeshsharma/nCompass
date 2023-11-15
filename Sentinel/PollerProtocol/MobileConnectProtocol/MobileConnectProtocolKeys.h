#ifndef _MOBILECONNECTPROTOCOLKEYS_H
#define _MOBILECONNECTPROTOCOLKEYS_H

#include "EnumMap.h"

extern const char* pFKeyMessageType;
extern const char* pFKeyDeviceType;
extern const char* pFKeyDeviceId;
extern const char* pFKeyMessageType;
extern const char* pFKeyDeviceType;
extern const char* pFKeyDeviceId;
extern const char* pFKeyPairingType;
extern const char* pFKeyPairingEnabled;
extern const char* pFKeyPairingTimeOutMs;
extern const char* pFKeyIdlePairingId;
extern const char* pFKeyPairingId;
extern const char* pFKeyIdlePairingChecksum;
extern const char* pFKeyPairingChecksum;
extern const char* pFKeySuccess;
extern const char* pFKeyStatus;
extern const char* pFKeyCardNumber;
extern const char* pFKeyChangeType;
extern const char* pFKeyPlayerId;
extern const char* pFKeyNewPlayerSessionId;
extern const char* pFKeyPrevPlayerType;
extern const char* pFKeyNewPlayerType;
extern const char* pFKeyPlayerSessionId;
extern const char* pFKeyTotalPoints;
extern const char* pFKeySessionPoints;
extern const char* pFKeySessionCountdown;
extern const char* pFKeyPointsTillNextTier;
extern const char* pFKeyCreditInPennies;
extern const char* pFKeyCredit;
extern const char* pFKeyInactiveAccounts;
extern const char* pFKeyType;
extern const char* pFKeyAmountInPennies;
extern const char* pFKeyPin;
extern const char* pFKeyFundType;
extern const char* pFKeyTransferDirection;
extern const char* pFKeyTransfer;
extern const char* pFKeyMessageId;
extern const char* pFKeyTimeStamp;
extern const char* pFKeyPayloadVer;
extern const char* pFKeyPayload;
extern const char* pFKeyCorrelationId;
extern const char* pFKeyIdempotencyId;
extern const char* pFKeyTransmitPower;
extern const char* pFKeyTransmitFrequency;
extern const char* pFKeyMeasuredPower;
extern const char* pFKeyAreSomePropertiesOffline;
extern const char* pFKeyIsBannedFromRedemption;
extern const char* pFKeyIsPartialTransferCashFromGameEnabled;
extern const char* pFKeyCashBalance;
extern const char* pFKeyPromoBalance;
extern const char* pFKeyPointsAvailable;
extern const char* pFKeyPointsPerDollar;
extern const char* pFKeyMinimumPoints;
extern const char* pFKeyMaximumPoints;
extern const char* pFKeyMinimumCashDown;
extern const char* pFKeyMaximumCashDown;
extern const char* pFKeyMinimumPromoDown;
extern const char* pFKeyMaximumPromoDown;
extern const char* pFKeyErrorText;
extern const char* pFKeySessionEventType;
extern const char* pFKeyJackpotAmount;
extern const char* pFKeyPbtEnabled;
extern const char* pFKeyCashoutErrorFlags;
extern const char* pFKeySASErrorText;
extern const char* pFKeyIsCashFunctionalityUnavailable;
extern const char* pFKeyUUID;
extern const char* pFKeyBeaconId;
extern const char* pFKeyPointOfInterest;
extern const char* pFKeyActive;

namespace FromPlayerDeviceService
{
	extern const char* pEventPairingSettings;
	extern const char* pEventPairing;
	extern const char* pRequestCardChange;
	extern const char* pRequestFundsTransferBalance;
	extern const char* pRequestFundsTransfer;
	extern const char* pRequestEndPBTSession;
	extern const char* pEventProximitySettings;

	enum Type
	{
		// Mobile Connect-specific types
		EventPairingSettings,
		EventPairing,
		RequestCardChange,
		RequestFundsTransferBalance,
		RequestFundsTransfer,
		RequestEndPBTSession,

		// Proximity-specific types
		EventProximitySettings
	};

	extern EnumMap<Type> emap;
}

namespace ToPlayerDeviceService
{
	// Mobile Connect-specific types
	extern const char* pRequestPairingSettings;
	extern const char* pRequestPairing;
	extern const char* pEventPairingStatus;
	extern const char* pEventCardChange;
	extern const char* pEventPlayerChange;
	extern const char* pEventPlayerSessionChange;
	extern const char* pEventCreditTotals;
	extern const char* pEventFundsTransfer;
	extern const char* pEventEndPBTSession;

	// Proximity-specific types
	extern const char* pRequestProximitySettings;
	extern const char* pEventProximityStatus;

	enum Type
	{
		// Mobile Connect-specific types
		RequestPairingSettings = 0,
		RequestPairing,
		EventPairingStatus,
		EventCardChange,
		EventPlayerChange,
		EventPlayerSessionChange,
		EventFundsTransferBalance,
		EventFundsTransfer,
		EventEndPBTSession,

		// Proximity-specific types
		RequestProximitySettings,
		EventProximityStatus
	};

	extern EnumMap<Type> emap;

}

#endif //_MOBILECONNECTPROTOCOLKEYS_H
