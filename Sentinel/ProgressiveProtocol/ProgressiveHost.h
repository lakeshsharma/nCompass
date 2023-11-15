#pragma once

#include "InternalMsgContent.h"
#include "IPAddressArray.h"

/// <summary>
/// This is the progressive host and the its IP address list.
/// </summary>
class CProgressiveHost : public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_HOST>
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CProgressiveHost"/> class.
	/// This will assume there will be a DNS lookup to resolve the list of addresses
	/// </summary>
	/// <param name="hostName">Name of the host.</param>
	/// <param name="hostPort">The host port number.</param>
	CProgressiveHost(const std::string& hostName, DWORD hostPort);

	/// <summary>
	/// Initializes a new instance of the <see cref="CProgressiveHost"/> class.
	/// This will assume there will NOT be a DNS lookup to resolve address list
	/// </summary>
	/// <param name="addressArray">IN/OUT - The list of addresses. The list will be empty upon return.</param>
	CProgressiveHost(CIPAddressArray &addressArray);
	
	/// <summary>
	/// Finalizes an instance of the <see cref="CProgressiveHost"/> class.
	/// </summary>
	virtual ~CProgressiveHost();

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CProgressiveHost"/> class.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	CProgressiveHost(const CProgressiveHost &rhs);

	/// <summary>
	/// Assignment operator.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	/// <returns>A reference to the target of the assignment.</returns>
	CProgressiveHost& operator=(const CProgressiveHost &rhs);

	/// <summary>
	/// Get the host name.
	/// </summary>
	/// <returns>The host name.</returns>
	std::string GetName() const;

	/// <summary>
	/// Determines whether this host has an address.
	/// </summary>
	/// <returns>true if this host has an address, false otherwise.</returns>
	/// <remarks>
	/// GetURL() returns an empty string when this method returns false.
	/// </remarks>
	bool HasAddress() const;

	/// <summary>
	/// Gets the currently used IP address for this host.
	/// </summary>
	/// <returns>returns IP address</returns>
	CIPAddress GetIPAddress() const;

	/// <summary>
	/// Sets to next address.
	/// </summary>
	/// <returns>true if there was a address to set it to.</returns>
	bool SetToNextAddress();

	/// <summary>
	/// Returns the current IP address index (for diagnostics).
	/// </summary>
	/// <returns>The current IP address index.</returns>
	size_t GetCurrentAddressIndex() const;

	/// <summary>
	/// Returns the number of IP addresses.
	/// </summary>
	/// <returns>The number of IP addresses.</returns>
	size_t GetIPAddressCount() const;

	/// <summary>
	/// Gets an IP address.
	/// </summary>
	/// <param name="index">IN - The index.</param>
	/// <returns>The IP address for the index.</returns>
	CIPAddress GetIPAddress(size_t index) const;

	/// <summary>
	/// Gets the port for this host
	/// </summary>
	/// <returns>the port for this host</returns>
	DWORD GetHostPort() const;

	/// <summary>
	/// Gets the current URL.
	/// </summary>
	/// <returns>the current URL</returns>
	/// <remarks>
	/// Returns an empty string when HasAddress() returns false.
	/// </remarks>
	std::string GetURL() const;

	/// <summary>
	/// Performs a DNS lookup on the host name and gets a list of IP addresses.
	/// If the list has changed it will re-sort the list and pick a new preferred address.
	/// If the currently used address is not the preferred address, 
	///   the current address will be set to the preferred.
	/// </summary>
	/// <returns><b>true</b> if the current address is not empty, else <b>false</b>.</returns>
	bool UpdateAddress();

private:
	
	/// <summary>
	/// The host name
	/// </summary>
	std::string m_name;

	/// <summary>
	/// The host port
	/// </summary>
	DWORD m_port;

	/// <summary>
	/// The list of addresses for this host after a DNS lookup.
	/// This list is to be randomly sorted.
	/// The preferred address is the top address
	/// </summary>
	CIPAddressArray m_addresses;

	/// <summary>
	/// The index of the IP address that is currently in use
	/// </summary>
	size_t m_currentAddressIndex;

	void RandomAddressListMove(CIPAddressArray &newAddresses);
};

