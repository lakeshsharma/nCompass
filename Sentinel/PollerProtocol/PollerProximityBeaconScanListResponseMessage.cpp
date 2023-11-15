#include "stdafx.h"
#include "PollerProximityBeaconScanListResponseMessage.h"
#include "../Utilities.h"
#include "Logging/LogString.h"

CPollerProximityBeaconScanListResponseMessage::CPollerProximityBeaconScanListResponseMessage(byte *response)
{

}

CPollerProximityBeaconScanListResponseMessage::~CPollerProximityBeaconScanListResponseMessage()
{
}

std::vector<std::string> CPollerProximityBeaconScanListResponseMessage::GetUUIDList() const
{
	return m_UUIDList;
}
