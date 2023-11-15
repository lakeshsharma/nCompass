#pragma once

#include "Connection.h"
#include "Player.h"
#include "Games.h"
#include "GameEndData.h"
#include "RapidJsonHelper.h"
#include "SASProtocol/SASGeneralException.h"
#include "PairingRequest.h"
#include "PairingStatusEvent.h"
#include "CardChangeEvent.h"
#include "PlayerChangeEvent.h"
#include "FundsTransferEvent.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class MobileConnectMessageFactoryUnitTests; }

class CMobileConnectMessageFactory
{
	friend class SentinelNativeUnitTests::MobileConnectMessageFactoryUnitTests;

public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CMobileConnectMessageFactory"/> class.
	/// </summary>
	/// <param name="connection">The connection data.</param>
	CMobileConnectMessageFactory(long slotMastID);

	/// <summary>
	/// Finalizes an instance of the <see cref="CMobileConnectMessageFactory"/> class.
	/// </summary>
	virtual ~CMobileConnectMessageFactory();

	/// <summary>
	/// Get device id.
	/// </summary>
	/// <returns>string</returns>
	string GetDeviceId() const;

	/// <summary>
	/// Update SlotMastID Info.
	/// </summary>
	/// <param name="newDeviceId">IN - new slot mast ID info.</param>
	void UpdateDeviceId(long newDeviceId);

	/// <summary>
	/// Create Pairing Settings Request.
	/// </summary>
	/// <returns>string</returns>
	std::string CreatePairingSettingsRequest();

	/// <summary>
	/// Create Pairing Request.
	/// </summary>
	/// <param name="pairingRequestData">IN - pairing Request Data.</param>
	/// <returns>string</returns>
	std::string CreatePairingRequest(CPairingRequest *pairingRequestData);

	/// <summary>
	/// Create Pairing Status Event.
	/// </summary>
	/// <param name="power">IN - event data.</param>
	/// <returns>string</returns>
	std::string CreatePairingStatusEvent(CPairingStatusEvent *pPairingStatusEventData);

	/// <summary>
	/// Create Player Change Event.
	/// </summary>
	/// <param name="power">IN - event data.</param>
	/// <returns>string</returns>
	std::string CreatePlayerChangeEvent(CPlayerChangeEvent *pPlayerChangeEventData);

	/// <summary>
	/// Create Card Change Event.
	/// </summary>
	/// <param name="power">IN - event data.</param>
	/// <returns>string</returns>
	std::string CreateCardChangeEvent(CCardChangeEvent *pCardChangeEventData);

	/// <summary>
	/// Create Player Session Change Event.
	/// </summary>
	/// <param name="power">IN - event data.</param>
	/// <returns>string</returns>
	std::string CreatePlayerSessionChangeEvent(CPlayerSessionChangeEvent *pPlayerSessionChangeEventData);

	/// <summary>
	/// Create Credit Totals Event.
	/// </summary>
	/// <param name="power">IN - event data.</param>
	/// <returns>string</returns>
	std::string CreateFundsTransferBalanceEvent(CFundsTransferBalanceEvent *pFundsTransferBalanceEventData);

	/// <summary>
	/// Create Funds Transfer Event.
	/// </summary>
	/// <param name="power">IN - event data.</param>
	/// <returns>string</returns>
	std::string CreateFundsTransferEvent(CFundsTransferEvent *pFundsTranferEventData);

	/// <summary>
	/// Create End PBT Session Event.
	/// </summary>
	/// <returns>string</returns>
	std::string CreateEndPBTSessionEvent();

	/// Proximity-specific message factory functionality

	/// <summary>
	/// Create Proximity Settings Request.
	/// This is a proximity request, not subject to the Mobile Connect Communication Flag.
	/// </summary>
	/// <returns>string</returns>
	std::string CreateProximitySettingsRequest();

	/// <summary>
	/// Create Proximity Status Event.
	/// This is a proximity request, not subject to the Mobile Connect Communication Flag.
	/// </summary>
	/// <param name="power">IN - event data.</param>
	/// <returns>string</returns>
	std::string CreateProximityStatusEvent(CProximityStatusEvent *pProximityStatusEventData);

private:

	std::string m_deviceId;
	std::string m_deviceType;

};



