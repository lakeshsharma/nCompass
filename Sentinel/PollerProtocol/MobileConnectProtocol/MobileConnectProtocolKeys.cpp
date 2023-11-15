
#include "MobileConnectProtocolKeys.h"

const char* pFKeyMessageType 		= "messageType";
const char* pFKeyDeviceType 		= "deviceType";
const char* pFKeyDeviceId 			= "deviceId";
const char* pFKeyPairingType 		= "pairingType";
const char* pFKeyPairingEnabled 	= "pairingEnabled";
const char* pFKeyPairingTimeOutMs 	= "pairingTimeOutMs";
const char* pFKeyIdlePairingId 		= "idlePairingId";
const char* pFKeyPairingId 			= "pairingId";
const char* pFKeyIdlePairingChecksum= "idlePairingChecksum";
const char* pFKeyPairingChecksum	= "pairingChecksum";
const char* pFKeySuccess 			= "success";
const char* pFKeyStatus 			= "status";
const char* pFKeyCardNumber			= "cardNumber";
const char* pFKeyChangeType			= "changeType";
const char* pFKeyPlayerId			= "playerId";
const char* pFKeyNewPlayerSessionId	= "newPlayerSessionId";
const char* pFKeyPrevPlayerType		= "previousPlayerType";
const char* pFKeyNewPlayerType		= "newPlayerType";
const char* pFKeyPlayerSessionId	= "playerSessionId";
const char* pFKeyTotalPoints		= "totalPoints";
const char* pFKeySessionPoints		= "sessionPoints";
const char* pFKeySessionCountdown	= "sessionCountdown";
const char* pFKeyPointsTillNextTier	= "pointsTillNextTier";
const char* pFKeyCreditInPennies	= "creditInPennies";
const char* pFKeyCredit				= "credit";
const char* pFKeyInactiveAccounts	= "inactiveAccounts";
const char* pFKeyType				= "type";
const char* pFKeyAmountInPennies	= "amountInPennies";
const char* pFKeyPin				= "pin";
const char* pFKeyFundType			= "fundType";
const char* pFKeyTransferDirection	= "transferDirection";
const char* pFKeyTransfer			= "transfer";
const char* pFKeyMessageId			= "messageId";
const char* pFKeyTimeStamp			= "timestamp";
const char* pFKeyPayloadVer			= "payloadVersion";
const char* pFKeyPayload			= "payload";
const char* pFKeyCorrelationId		= "correlationId";
const char* pFKeyIdempotencyId		= "idempotencyId";
const char* pFKeyTransmitPower		= "transmitPower";
const char* pFKeyTransmitFrequency	= "transmitFrequency";
const char* pFKeyMeasuredPower		= "measuredPower";
const char* pFKeyAreSomePropertiesOffline	= "areSomePropertiesOffline";
const char* pFKeyIsBannedFromRedemption		= "isBannedFromRedemption";
const char* pFKeyIsPartialTransferCashFromGameEnabled = "isPartialTransferCashFromGameEnabled";
const char* pFKeyCashBalance		= "cashBalance";
const char* pFKeyPromoBalance		= "promoBalance";
const char* pFKeyPointsAvailable	= "pointsAvailable";
const char* pFKeyPointsPerDollar	= "pointsPerDollar";
const char* pFKeyMinimumPoints		= "minimumPoints";
const char* pFKeyMaximumPoints		= "maximumPoints";
const char* pFKeyMinimumCashDown	= "minimumCashDown";
const char* pFKeyMaximumCashDown	= "maximumCashDown";
const char* pFKeyMinimumPromoDown	= "minimumPromoDown";
const char* pFKeyMaximumPromoDown	= "maximumPromoDown";
const char* pFKeyErrorText			= "errorText";
const char* pFKeySessionEventType   = "sessionEventType";
const char* pFKeyJackpotAmount		= "jackpotAmount";
const char* pFKeyPbtEnabled			= "isPBTEnabled";
const char* pFKeyCashoutErrorFlags	= "cashoutErrorFlags";
const char* pFKeySASErrorText		= "sasErrorText";
const char* pFKeyIsCashFunctionalityUnavailable	= "isCashFunctionalityUnavailable";
const char* pFKeyUUID				= "uuid";
const char* pFKeyBeaconId			= "beaconId";
const char* pFKeyPointOfInterest	= "pointOfInterest";
const char* pFKeyActive				= "active";

namespace MobileConnectProtocolHeader
{
	std::string userNameFS("FS");
	std::string passwordFS("FS");

	std::string destination("destination");
	std::string type("type");
	std::string persistentType("persistent");
	std::string nonPersistent("false");

	std::string producerQueueNameFS("/queue/@FSproducer");

	std::string trustStore("decaf.net.ssl.trustStore");
}

namespace ContentTypeFS
{
	const char* pJson = "application/json";
}

namespace FromPlayerDeviceService
{
	// Mobile Connect-specific types
	const char* pEventPairingSettings 	= "mobile.event.pairingSettings";
	const char* pEventPairing 			= "mobile.event.pairing";
	const char* pRequestCardChange		= "session.request.cardChange";
	const char* pRequestFundsTransferBalance	= "session.request.getBalance";
	const char* pRequestFundsTransfer	= "session.request.fundsTransfer";
	const char* pRequestEndPBTSession	= "session.request.endPbtSession";

	// Proximity-specific types
	const char* pEventProximitySettings		= "mobile.event.proximitySettings";

	static EnumMap<Type>::EnumPair types[] =
	{
		// Mobile Connect-specific types
		{ EventPairingSettings, pEventPairingSettings },
		{ EventPairing, pEventPairing },
		{ RequestCardChange, pRequestCardChange },
		{ RequestFundsTransferBalance, pRequestFundsTransferBalance },
		{ RequestFundsTransfer, pRequestFundsTransfer },
		{ RequestEndPBTSession, pRequestEndPBTSession },

		// Proximity-specific types
		{ EventProximitySettings, pEventProximitySettings }
	};

	EnumMap<Type> emap(types);
}

namespace ToPlayerDeviceService
{
	// Mobile Connect-specific types
	const char* pRequestPairingSettings = "mobile.request.pairingSettings";
	const char* pRequestPairing 		= "mobile.request.pairing";
	const char* pEventPairingStatus		= "mobile.event.pairingStatus";
	const char* pEventCardChange 		= "session.event.cardChange";
	const char* pEventPlayerChange 		= "session.event.playerChange";
	const char* pEventPlayerSessionChange 		= "session.event.playerSessionChange";
	const char* pEventCreditTotals		= "session.response.getBalance";
	const char* pEventFundsTransfer		= "session.event.fundsTransfer";
	const char* pEventEndPBTSession		= "session.event.endPbtSession";

	// Proximity-specific types
	const char* pRequestProximitySettings	    = "mobile.request.proximitySettings";
	const char* pEventProximityStatus	= "mobile.event.proximityStatus";

	static EnumMap<Type>::EnumPair types[] =
	{
		// Mobile Connect-specific types
		{ RequestPairingSettings, pRequestPairingSettings },
		{ RequestPairing, pRequestPairing },
		{ EventPairingStatus, pEventPairingStatus },
		{ EventCardChange, pEventCardChange },
		{ EventPlayerChange, pEventPlayerChange },
		{ EventPlayerSessionChange, pEventPlayerSessionChange },
		{ EventFundsTransferBalance, pEventCreditTotals },
		{ EventFundsTransfer, pEventFundsTransfer },
		{ EventEndPBTSession, pEventEndPBTSession },

		// Proximity-specific types
		{ RequestProximitySettings, pRequestProximitySettings },
		{ EventProximityStatus, pEventProximityStatus }
	};

	EnumMap<Type> emap(types);
}


