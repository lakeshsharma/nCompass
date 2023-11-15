#pragma once

class TouchScreenProtocol
{
public:

	/// <summary>
	/// Constructor for USB protocol	
	/// </summary>
    TouchScreenProtocol();

	/// <summary>
	/// Constructor for serial protocol	
	/// </summary>
	/// <param name="vendorID">The vendor ID.</param>	
	/// <param name="vendorName">The vendor name.</param>
	/// <param name="baudRate">The baud rate.</param>
	TouchScreenProtocol(uint32_t vendorID, const std::string &vendorName, uint32_t baudRate);

	/// <summary>
	/// Determines if protocol is for USB.
	/// </summary>
	/// <returns>true if USB protocol. False if serial.</returns>
	bool IsUSB();

	/// <summary>
	/// Get name of touch screen protocol.
	/// </summary>
	/// <returns>touch screen name.</returns>
	std::string GetName();

	/// <summary>
	/// Get vendor ID of touch screen protocol.
	/// </summary>
	/// <returns>touch screen protocol vendor ID.</returns>
	uint32_t GetVendorID();

	/// <summary>
	/// Get vendor ID of touch screen protocol as a string.
	/// </summary>
	/// <returns>touch screen protocol vendor ID string.</returns>
	std::string GetVendorIDString();

	/// <summary>
	/// Get boad rate of touch screen protocol.
	/// </summary>
	/// <returns>touch screen protocol baud rate.</returns>
    uint32_t GetBaud();

	/// <summary>
	/// Get boad rate of touch screen protocol as a string.
	/// </summary>
	/// <returns>touch screen protocol baud rate string.</returns>
    std::string GetBaudString();

private:

    bool m_isUSB;
    uint32_t m_vendorID;
    std::string m_vendorName;
    uint32_t m_baudRate;

};

