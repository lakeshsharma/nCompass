#ifndef _CREDIT_TOTALS_REQUEST_H
#define _CREDIT_TOTALS_REQUEST_H

#include "InternalMsgContent.h"
#include "RapidJsonHelper.h"
#include "MobileConnectProtocolKeys.h"

class CFundsTransferBalanceRequest  :
        public CInternalMsgContent<INT_MSG_CONTENT_FUNDS_TRANSFER_BALANCE_REQUEST>
{
public:
    CFundsTransferBalanceRequest(const rapidjson::Document& jsonDoc);
	
	/// <summary>
	/// Get correlation id.
	/// </summary>
	/// <returns>string</returns>
	std::string GetCorrelationId() const;
		
	/// <summary>
	/// Get Device type.
	/// </summary>
	/// <returns>string</returns>
    std::string GetDeviceType() const;
			
	/// <summary>
	/// Get device id.
	/// </summary>
	/// <returns>string</returns>
    std::string GetDeviceId() const;
	
	/// <summary>
	/// Returns Pin.
	/// </summary>
	/// <returns>string</returns>
	std::string GetPin() const;

	/// <summary>
	/// Returns Payload Version.
	/// </summary>
	/// <returns>string</returns>
	std::string GetPayloadVersion() const;

private:
	std::string m_correlationId;
		
    std::string m_sDeviceType;
    std::string m_sDeviceId;
	std::string m_pin;
	std::string m_payloadVersion;
};


#endif //_CREDIT_TOTALS_REQUEST_H