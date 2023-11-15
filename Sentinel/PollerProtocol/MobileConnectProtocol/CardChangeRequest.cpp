//
// Created by singhsukh on 7/27/2018.
//

#include "CardChangeRequest.h"
#include "MobileConnectProtocolKeys.h"
#include "Logging/LogString.h"

CCardChangeRequest::CCardChangeRequest(const rapidjson::Document &jsonDoc)
{
	std::string dummyStr; // to avoid crash, if key not found

	m_correlationId = JsonGetString(jsonDoc, pFKeyCorrelationId, dummyStr.c_str());
	try
	{
		const rapidjson::Value& payload = jsonDoc[pFKeyPayload];

		m_sDeviceType       = JsonGetString(payload, pFKeyDeviceType, dummyStr.c_str());
		m_sDeviceId         = JsonGetString(payload, pFKeyDeviceId, dummyStr.c_str());
		m_sCardNumber       = JsonGetString(payload, pFKeyCardNumber, dummyStr.c_str());
		m_sChangeType       = JsonGetString(payload, pFKeyChangeType, dummyStr.c_str());

			LogString(POLLER_LOG, "%s %d: Parsed payload received from queue : DeviceType: %s, DeviceId: %s, CardNumber: %s, ChangeType: %s",
					__FUNCTION__, __LINE__, m_sDeviceType.c_str(), m_sDeviceId.c_str(), m_sCardNumber.c_str(), m_sChangeType.c_str());
	}
	catch(...)
	{
		LogString(ERROR_LOG,"Exception while reading response in CCardChangeRequest");
	}
}

std::string CCardChangeRequest::GetCorrelationId() const
{
	return m_correlationId;
}

std::string CCardChangeRequest::GetDeviceType() const
{
    return m_sDeviceType;
}

std::string CCardChangeRequest::GetDeviceId() const
{
    return m_sDeviceId;
}

std::string CCardChangeRequest::GetCardNumber() const
{
    return m_sCardNumber;
}

std::string CCardChangeRequest::GetChangeType() const
{
    return m_sChangeType;
}
