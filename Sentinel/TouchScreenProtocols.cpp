
#include "stdafx.h"
#include "TouchScreenProtocols.h"


using namespace std;

TouchScreenProtocols::TouchScreenProtocols(bool usbDetected, uint32_t currentVendorID, uint32_t currentBaudRate)
{
    m_usbDetected = usbDetected;
    m_currentVendorID = currentVendorID;
    m_currentBaudRate = currentBaudRate;

    if (usbDetected) 
    {
        m_protocols.push_back(new TouchScreenProtocol());
    }
}

TouchScreenProtocols::~TouchScreenProtocols()
{
	for (std::vector<TouchScreenProtocol *>::iterator it = m_protocols.begin(); it != m_protocols.end(); ++it)
	{
		delete (*it);
	}
}

bool TouchScreenProtocols::USBDetected()
{
    return m_usbDetected;
}

void TouchScreenProtocols::GetCurrentInfo(std::string &currentName, std::string &currentVendorIDString, std::string &currentBaudRateString)
{
    const string UnknownCurrentSetting = "Unknown";

    TouchScreenProtocol *currentProtocol = GetCurrentProtocol();
    if (nullptr != currentProtocol) 
    {
        currentName = currentProtocol->GetName();
        currentVendorIDString = currentProtocol->GetVendorIDString();
        currentBaudRateString = currentProtocol->GetBaudString();
    }
    else
    {
        currentName = UnknownCurrentSetting;
        currentVendorIDString = UnknownCurrentSetting;
        currentBaudRateString = UnknownCurrentSetting;
    }
}

void TouchScreenProtocols::AddVendor(uint32_t vendorID, std::string &vendorName)
{
    const uint32_t SupportedBaud1 = 2400;
    const uint32_t SupportedBaud2 = 4800;
    const uint32_t SupportedBaud3 = 9600;

    if (!m_usbDetected) 
    {
        m_protocols.push_back(new TouchScreenProtocol(vendorID, vendorName, SupportedBaud1));
        m_protocols.push_back(new TouchScreenProtocol(vendorID, vendorName, SupportedBaud2));
        m_protocols.push_back(new TouchScreenProtocol(vendorID, vendorName, SupportedBaud3));
    }
}


std::vector<TouchScreenProtocol *> TouchScreenProtocols::GetAllProtocols()
{
    return m_protocols;
}


TouchScreenProtocol *TouchScreenProtocols::GetProtocol(std::string &protocolName)
{
    TouchScreenProtocol *protocol = nullptr;

	for (std::vector<TouchScreenProtocol *>::iterator it = m_protocols.begin(); it != m_protocols.end(); ++it)
	{
		if ((*it)->GetName() == protocolName)
        {
            protocol = *it;
            break;
        }
	}

    return protocol;
}

TouchScreenProtocol *TouchScreenProtocols::GetCurrentProtocol()
{
    TouchScreenProtocol *protocol = nullptr;

    if (m_usbDetected) 
    {
        protocol = GetUSBProtocol();
    }
    else
    {
        protocol = GetSerialProtocol(m_currentVendorID, m_currentBaudRate);
    }
    
    return protocol;
}

TouchScreenProtocol *TouchScreenProtocols::GetUSBProtocol()
{
    TouchScreenProtocol *protocol = nullptr;

	for (std::vector<TouchScreenProtocol *>::iterator it = m_protocols.begin(); it != m_protocols.end(); ++it)
	{
		if ((*it)->IsUSB())
        {
            protocol = *it;
            break;
        }
	}

    return protocol;
}

TouchScreenProtocol *TouchScreenProtocols::GetSerialProtocol(uint32_t vendorID, uint32_t baudRate)
{
    TouchScreenProtocol *protocol = nullptr;

	for (std::vector<TouchScreenProtocol *>::iterator it = m_protocols.begin(); it != m_protocols.end(); ++it)
	{
		if (!(*it)->IsUSB() && (*it)->GetVendorID() == vendorID && (*it)->GetBaud() == baudRate)
        {
            protocol = *it;
            break;
        }
	}

    return protocol;
}

