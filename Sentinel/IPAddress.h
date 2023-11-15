#pragma once

#include "Utilities.h"
#include "GlobalDefs.h"

class CIPAddress
{
public:
	/// <summary>
	/// Constructor. Initializes a new instance of the <see cref="CIPAddress"/> class.
	/// </summary>
	CIPAddress(void);

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CIPAddress"/> class.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	CIPAddress(CIPAddress const &RHS);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CIPAddressArray"/> class.
	/// </summary>
	~CIPAddress(void);

	/// <summary>
	/// Sets the IP address.
	/// </summary>
	/// <param name="strAddress">IN - The IP address in dotted-decimal notation string format.</param>
	void SetIP(const std::string& strAddress);

	/// <summary>
	/// Sets the IP address.
	/// </summary>
	/// <param name="lpAddress">IN - The IP address in dot notation character buffer format.</param>
	void SetIP(const BYTE* lpAddress);

	/// <summary>
	/// Sets the port.
	/// </summary>
	/// <param name="port">IN - The port.</param>
	void SetPort(int port);

	/// <summary>
	/// Gets the IP address.
	/// </summary>
	/// <param name="strAddress">OUT - The IP address in dot notation string format.</param>
	void GetIP(std::string &strAddress) const;

	/// <summary>
	/// Gets the IP address.
	/// </summary>
	/// <param name="lpAddress">OUT - The IP address in dot notation character buffer format.</param>
	/// <param name="length">IN - The length of the IP address character buffer.</param>
	/// <remarks>
	/// If the length of the IP address character buffer is too small for the text being printed then the buffer is set to an empty string.
	/// </remarks>
	void GetIP(char* lpAddress, size_t length) const;

	/// <summary>
	/// Gets the IP address.
	/// </summary>
	/// <returns>A string containing the IP address in dot notation format.</returns>
	std::string GetIP() const;

	/// <summary>
	/// Gets the IP address.
	/// </summary>
	/// <param name="lpAddress">IN/OUT - The IP address buffer.</param>
	/// <param name="length">IN - The length of the IP address buffer.</param>
	/// <remarks>
	/// The IP address buffer is unchanged if length < IP_ADDRESS_SIZE (4 bytes).
	/// </remarks>
	void GetIP(BYTE* lpAddress, int length) const;

	/// <summary>
	/// Gets the IP address prefix.
	/// </summary>
	/// <returns>The IP address prefix.</returns>
	BYTE GetIPAddressPrefix(void) const;

	/// <summary>
	/// Gets the port.
	/// </summary>
	/// <returns>The port.</returns>
	long GetPort() const;

	/// <summary>
	/// Determines whether this IP address is is empty (0.0.0.0).
	/// </summary>
	/// <returns>true if address is empty</returns>
	bool IsEmpty() const;

	/// <summary>
	/// Assigment operator.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	/// <returns>A reference to the target of the assignment.</returns>
	CIPAddress &operator=(CIPAddress const &RHS);

	/// <summary>
	/// Equality operator. Compares two IP addresses.
	/// </summary>
	/// <param name="RHS">IN - The CIPAddress to be compared.</param>
	/// <returns>returns true if the IP adresses have the same value, false otherwise.</returns>
	bool operator==(CIPAddress const &RHS) const;

	/// <summary>
	/// Equality operator. Compares two IP addresses.
	/// </summary>
	/// <param name="RHS">IN - The CIPAddress to be compared.</param>
	/// <returns>returns true if the IP adresses have different values, false otherwise..</returns>
	bool operator!=(CIPAddress const &RHS) const;

private:
	static const int MSB_ADDRESS_INDEX = 0; // Address index for the most significant byte.
	static const int NMSB_ADDRESS_INDEX = 1; // Address index for the next most significant byte.
	static const int NLSB_ADDRESS_INDEX = 2; // Address index for the next least significant byte.
	static const int LSB_ADDRESS_INDEX = 3; // Address index for the least significant byte.

	BYTE m_Address[IP_ADDRESS_SIZE];
	int m_port;

};

