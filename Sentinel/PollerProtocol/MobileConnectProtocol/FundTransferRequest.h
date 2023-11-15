#ifndef _FUNDS_TRANSFER_REQUEST_H
#define _FUNDS_TRANSFER_REQUEST_H

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"
#include "MobileConnectProtocolKeys.h"

class CFundsTransferRequest  :
        public CInternalMsgContent<INT_MSG_CONTENT_FUNDS_TRANSFER_REQUEST>
{
public:
    CFundsTransferRequest(const rapidjson::Document& jsonDoc);
	
	/// <summary>
	/// Returns CorrelationId.
	/// </summary>
	/// <returns>string</returns>
	std::string GetCorrelationId() const;
	
	/// <summary>
	/// Returns Device Type.
	/// </summary>
	/// <returns>string</returns>
    std::string GetDeviceType() const;
	
	/// <summary>
	/// Returns DeviceId.
	/// </summary>
	/// <returns>string</returns>
    std::string GetDeviceId() const;
	
	/// <summary>
	/// Returns Pin.
	/// </summary>
	/// <returns>string</returns>
	std::string GetPin() const;
	
	/// <summary>
	/// Returns FundType.
	/// </summary>
	/// <returns>string</returns>
	std::string GetFundType() const;
	
	/// <summary>
	/// Returns Transfer Direction.
	/// </summary>
	/// <returns>string</returns>
	std::string GetTransferDirection() const;
	
	/// <summary>
	/// Returns AmountInPennies.
	/// </summary>
	/// <returns>string</returns>
	DWORD GetAmountInPennies() const;

private:
	std::string m_correlationId;
		
    std::string m_deviceType;
    std::string m_deviceId;
	std::string m_pin;
    std::string m_fundType;
	std::string m_transferDirection;
    DWORD m_amountInPennies;
};


#endif //_FUND_TRANSFER_REQUEST_H