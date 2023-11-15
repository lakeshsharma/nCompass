
#include "CreditTotalsRequest.h"
#include "Logging/LogString.h"

CFundsTransferBalanceRequest::CFundsTransferBalanceRequest(const rapidjson::Document& jsonDoc)
{
	std::string dummyStr; // to avoid crash, if key not found
	m_correlationId = JsonGetString(jsonDoc, pFKeyCorrelationId, dummyStr.c_str());
	try
	{
		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];
	
		m_sDeviceType		= JsonGetString(payload, pFKeyDeviceType, dummyStr.c_str());
		m_sDeviceId 		= JsonGetString(payload, pFKeyDeviceId, dummyStr.c_str());
		m_pin				= JsonGetString(payload, pFKeyPin, dummyStr.c_str());
		m_payloadVersion	= JsonGetString(jsonDoc, pFKeyPayloadVer, dummyStr.c_str());

		LogString(POLLER_LOG, "%s %d: Data received from queue : DeviceType: %s, DeviceId: %s, PayloadVersion: %s",
					   __FUNCTION__, __LINE__, m_sDeviceType.c_str(), m_sDeviceId.c_str(), m_payloadVersion.c_str());
	}
	catch(...)
	{
		 LogString(ERROR_LOG,"Exception while reading response in CCreditTotalsRequest");
	}	
}

std::string CFundsTransferBalanceRequest::GetCorrelationId() const
{
	return m_correlationId;
}

std::string CFundsTransferBalanceRequest::GetDeviceType() const
{
	return m_sDeviceType;
}
	
std::string CFundsTransferBalanceRequest::GetDeviceId() const
{
	return m_sDeviceId;
}

std::string CFundsTransferBalanceRequest::GetPin() const
{
	return m_pin;
}

std::string CFundsTransferBalanceRequest::GetPayloadVersion() const
{
	return m_payloadVersion;
}


