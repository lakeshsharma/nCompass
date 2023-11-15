#pragma once
#include "TouchScreenProtocol.h"

class TouchScreenProtocols
{
public:

	/// <summary>
	/// Constructor for container of available touch screen protocols	
	/// </summary>
	/// <param name="usbDetected">Indicates if USB was detected</param>	
	/// <param name="currentVendorID">The vendor ID for the currect touch screen protocol.</param>
	/// <param name="currentBaudRate">The baud rate for the currect touch screen protocol..</param>
	TouchScreenProtocols(bool usbDetected, uint32_t currentVendorID, uint32_t currentBaudRate);
    virtual ~TouchScreenProtocols();

    /// <summary> Is USB touch screen detected. 
    /// </summary>
    /// <returns>true if USB touch screen is detected</returns>
    bool USBDetected();

    /// <summary>
    /// Gets the current protocol name, vendor string and baud rate 
    /// string 
    /// </summary>
    /// <param name="currentName">OUT - the current protocol 
    /// name. Can be unknown</param> 
    /// <param name="currentVendorIDString">OUT - the current 
    /// vendor ID string. Can be unknown</param> 
    /// <param name="GetCurrentBaudRateString">OUT - the current 
    /// baud rate string. Can be unknown</param> 
    void GetCurrentInfo(std::string &currentName, std::string &currentVendorIDString, std::string &currentBaudRateString);

	/// <summary>
	/// Add a vendor to the protocols. This will add additional protocols.
	/// </summary>
	/// <param name="VendorID">The ID of the vendor to be added.</param>
	/// <param name="vendorName">The name of the vendor to be added.</param>
	void AddVendor(uint32_t vendorID, std::string &vendorName);

	/// <summary>
	/// Get the list of all the protocols.
	/// </summary>
	/// <returns>the vector list of the protocols.</returns>
	std::vector<TouchScreenProtocol *> GetAllProtocols();

	/// <summary>
	/// Get the protocol for the specified name.
	/// </summary>	
	/// <param name="protocolName">The name of the protocol to get.</param>
	/// <returns>pointer to the protocol. nullptr if not found</returns>
	TouchScreenProtocol *GetProtocol(std::string &protocolName);

	/// <summary>
	/// Get the current protocol.
	/// </summary>	
	/// <returns>pointer to the protocol. nullptr if not found</returns>
	TouchScreenProtocol *GetCurrentProtocol();

	/// <summary>
	/// Get the USB protocol.
	/// </summary>	
	/// <returns>pointer to the USB protocol. nullptr if not found</returns>
	TouchScreenProtocol *GetUSBProtocol();

	/// <summary>
	/// Get the serial protocol given the vendor ID and baud rate.
	/// </summary>	
	/// <param name="vendorID">The vendor ID of the protocol to get.</param>
	/// <param name="vendorID">The baud Rate of the protocol to get.</param>
	/// <returns>pointer to the serial protocol. nullptr if not found</returns>
	TouchScreenProtocol *GetSerialProtocol(uint32_t vendorID, uint32_t baudRate);

private:

    bool m_usbDetected;
    uint32_t m_currentVendorID;
    uint32_t m_currentBaudRate;
    std::vector<TouchScreenProtocol *> m_protocols;

};
