#include "stdafx.h"
#include "Utilities.h"
#include "TouchScreenProtocol.h"

using namespace std;

TouchScreenProtocol::TouchScreenProtocol()
    :   m_isUSB(true)
    ,   m_vendorID(0)
    ,   m_baudRate(0)
{
}

TouchScreenProtocol::TouchScreenProtocol(uint32_t vendorID, const std::string &vendorName, uint32_t baudRate)
    :   m_isUSB(false)
    ,   m_vendorID(vendorID)
    ,   m_vendorName(vendorName)
    ,   m_baudRate(baudRate)
{
}


bool TouchScreenProtocol::IsUSB()
{
    return m_isUSB;
}

std::string TouchScreenProtocol::GetName()
{
    const string USBProtocol = "USB";
    const string SerialProtocol = "Serial:";

    string protocolName;

    if (m_isUSB) 
    {
        protocolName = USBProtocol;
    }
    else
    {
        protocolName = SerialProtocol + m_vendorName + " " + GetBaudString();
    }

    return protocolName;
}

uint32_t TouchScreenProtocol::GetVendorID()
{
    return m_vendorID;
}

std::string TouchScreenProtocol::GetVendorIDString()
{
    string vendorID;

    if (m_isUSB) 
    {
        vendorID = "N/A";
    }
    else
    {
        vendorID = FormatString("0x%04x", m_vendorID);
    }

    return vendorID;
}

uint32_t TouchScreenProtocol::GetBaud()
{
    return m_baudRate;
}

std::string TouchScreenProtocol::GetBaudString()
{
    string baudString;

    if (m_isUSB) 
    {
        baudString = "N/A";
    }
    else
    {
        baudString = FormatString("%d", m_baudRate);
    }

    return baudString;
}
