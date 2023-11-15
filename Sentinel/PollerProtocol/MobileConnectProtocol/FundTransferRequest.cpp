
#include "FundTransferRequest.h"
#include "Logging/LogString.h"

CFundsTransferRequest::CFundsTransferRequest(const rapidjson::Document& jsonDoc)
{
	std::string dummyStr; // to avoid crash, if key not found
	m_correlationId = JsonGetString(jsonDoc, pFKeyCorrelationId, dummyStr.c_str());
	try
	{
		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];

		m_deviceType		= JsonGetString(payload, pFKeyDeviceType, dummyStr.c_str());
		m_deviceId	 		= JsonGetString(payload, pFKeyDeviceId, dummyStr.c_str());
		m_pin				= JsonGetString(payload, pFKeyPin, dummyStr.c_str());
		m_fundType	 		= JsonGetString(payload, pFKeyFundType, dummyStr.c_str());
		m_transferDirection = JsonGetString(payload, pFKeyTransferDirection, dummyStr.c_str());
		// only consider the amount if its ToGame transfer request or FromGame cash transfer request
		if (!m_transferDirection.compare("ToGame") || !m_fundType.compare("Cash"))
			m_amountInPennies = JsonGetInt64(payload, pFKeyAmountInPennies);
		else
			m_amountInPennies = 0;

		LogString(POLLER_LOG, "%s %d: Data received from queue : DeviceType: %s, DeviceId: %s FundType: %s, TransferDirection: %s Amount: %d",
					   __FUNCTION__, __LINE__, m_deviceType.c_str(), m_deviceId.c_str(), m_fundType.c_str(), m_transferDirection.c_str(), m_amountInPennies);
	}
	catch(...)
	{
		LogString(ERROR_LOG,"Exception while reading response in CFundsTransferRequest");
	}
}

std::string CFundsTransferRequest::GetCorrelationId() const
{
	return m_correlationId;
}

std::string CFundsTransferRequest::GetDeviceType() const
{
	return m_deviceType;
}

std::string CFundsTransferRequest::GetDeviceId() const
{
	return m_deviceId;
}

std::string CFundsTransferRequest::GetPin() const
{
	return m_pin;
}

std::string CFundsTransferRequest::GetFundType() const
{
	return m_fundType;
}

std::string CFundsTransferRequest::GetTransferDirection() const
{
	return m_transferDirection;
}

DWORD CFundsTransferRequest::GetAmountInPennies() const
{
	return m_amountInPennies;
}

