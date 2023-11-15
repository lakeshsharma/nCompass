#pragma once

#include "ProgressiveHost.h"

class CProgressiveHostManager
{
public:

	/// <summary>
	/// Initializes a new instance of the <see cref="CProgressiveHostManager"/> class.
	/// </summary>
	/// <param name="hostNameList">The list of host names.</param>
	/// <param name="namesAreDNS">The host names in the list are DNS names.</param>
	CProgressiveHostManager(std::vector<std::string> &hostNameList, bool namesAreDNS);

	/// <summary>
	/// Finalizes an instance of the <see cref="CProgressiveHostManager"/> class.
	/// </summary>
	virtual ~CProgressiveHostManager();

	/// <summary>
	/// Gets the URL we are currently using.
	/// </summary>
	/// <returns>the url. If no URL is available, an empty string is returned.</returns>
	std::string GetUrl();

	/// <summary>
	/// Call this function upon a connection failure. 
	/// This will attempt to go to a new address.
	/// </summary>
	void ConnectionFailure();

	/// <summary>
	/// Call this function to check to see if we are currently using or preferred address.
	/// This will also do a DNS look-up if we are configured
	/// </summary>
	/// <returns>true if the current address has changed</returns>
	bool AddressCheck();

	/// <summary>
	/// Gets the current host object. 
	/// </summary>
	/// <returns>Returns NULL if we exhausted our list.</returns>
	CProgressiveHost *GetCurrentHost();

	/// <summary>
	/// Gets the current address.
	/// </summary>
	/// <returns>The current IP address. This address will be empty if the address does not exist</returns>
	CIPAddress GetCurrentAddress();

private:

	std::recursive_mutex m_criticalSection;

	// An array of progressive host objects.
	std::vector<CProgressiveHost*> m_hostArray;

	// The index of the host object currently in use from the host array.
	// If we are not doing a DNS look-up this will always be 0.
	int m_currentHostIndex;

	/// <summary>
	/// Setups the host manager for DNS lookup.
	/// </summary>
	/// <param name="hostNameList">The host name list.</param>
	void SetupWithDNSLookup(std::vector<std::string> &hostNameList);

	/// <summary>
	/// Setups the host manager with address list.
	/// </summary>
	/// <param name="addressList">The list of addresses and ports.</param>
	void SetupWithAddressList(std::vector<std::string> &addressList);

	/// <summary>
	/// Extracts the port from hostname.
	/// </summary>
	/// <param name="hostNameWithPort">The host name with port.</param>
	/// <param name="hostName">Host name returned without the port.</param>
	/// <returns>the extracted port number</returns>
	DWORD ExtractPortFromHostname(const std::string &hostNameWithPort, std::string &hostName);

};

