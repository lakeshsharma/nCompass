#pragma once
#include "PollerMessage.h"

class CPollerProximityBeaconScanListResponseMessage :
	public CPollerMessage
{
public:
	CPollerProximityBeaconScanListResponseMessage(byte *response);
	~CPollerProximityBeaconScanListResponseMessage();
    std::vector<std::string> GetUUIDList() const;

private:
    std::vector<std::string> m_UUIDList;
};