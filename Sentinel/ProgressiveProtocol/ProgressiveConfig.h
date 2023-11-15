#pragma once
#include <stdafx.h>

// Sentinel headers.
#include "InternalMsgContent.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class ProgressiveConfigFile; }

class CProgressiveConfig : 	public CInternalMsgContent<INT_MSG_CONTENT_PROGRESSIVE_CONFIG>
{
	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::ProgressiveConfigFile;

public:
	static const DWORD DEFAULT_PING_INTERVAL = 15000; // In milliseconds -- i.e., tick counts.  For example, 5000 == 5 seconds.
	static const DWORD MINIMUM_PING_INTERVAL = 5000; // In milliseconds -- i.e., tick counts.  For example, 1000 == 1 second.
	static const DWORD MAXIMUM_PING_INTERVAL = DWORD_MAX; // In milliseconds -- i.e., tick counts.  For example, 3600*1000 == 1 hour.

	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CProgressiveConfig"/> class.
	/// </summary>
	CProgressiveConfig();

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CProgressiveConfig"/> class.
	/// </summary>
	~CProgressiveConfig();

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CProgressiveConfig"/> class.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	CProgressiveConfig(const CProgressiveConfig &rhs);

	/// <summary>
	/// Assigment operator.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	/// <returns>A reference to the target of the assignment.</returns>
	CProgressiveConfig& operator=(const CProgressiveConfig &rhs);

	/// <summary>
	/// Sets member variables to default values.
	/// </summary>
	void SetDefaults();

	/// <summary>
	/// Sets whether settings were received from a configuration file.
	/// </summary>
	/// <param name="hasConfigFile">IN - true if settings were received from a configuration file, false otherwise.</param>
	void SetHasConfigFile(bool hasConfigFile);

	/// <summary>
	/// Returns true if settings were received from a configuration file, false otherwise.
	/// </summary>
	/// <returns>true if settings were received from a configuration file, false otherwise.</returns>
	bool HasConfigFile() const;

	/// <summary>
	/// Sets whether progressive functionality (communications, etc.) is enabled.
	/// </summary>
	/// <param name="active">IN - true to enable progressive functionality (communications, etc.), false to disable.</param>
	void SetActive(bool active);

	/// <summary>
	/// Returns true if progressive functionality (communications, etc.) is enabled, false otherwise.
	/// </summary>
	/// <returns>true if progressive functionality (communications, etc.) is enabled, false otherwise.</returns>
	bool GetActive() const;

	/// <summary>
	/// Sets whether DNS lookup is enabled.
	/// </summary>
	/// <param name="dnsLookup">IN - true to enable DNS lookup, false to disable.</param>
	void SetDNSLookup(bool dnsLookup);

	/// <summary>
	/// Returns true if DNS lookup is enabled, false otherwise.
	/// </summary>
	/// <returns>true if DNS lookup is enabled, false otherwise.</returns>
	bool GetDNSLookup() const;

	/// <summary>
	/// Adds an endpoint IP address.
	/// </summary>
	/// <param name="endpoint">IN - The endpoint.</param>
	void AddEndpoint(const std::string &endpoint);

	/// <summary>
	/// Returns the number of endpoints.
	/// </summary>
	/// <returns>The number of endpoints.</returns>
	int GetEndpointCount() const;

	/// <summary>
	/// Gets an endpoint.
	/// </summary>
	/// <param name="index">IN - The index.</param>
	/// <returns>The endpoint for the index.</returns>
	std::string GetEndpoint(int index) const;

	/// <summary>
	/// Sets the ping interval in milliseconds -- i.e., tick counts.
	/// </summary>
	/// <param name="pingInterval">IN - the ping interval in milliseconds.</param>
	void SetPingInterval(DWORD pingInterval);

	/// <summary>
	/// Returns the ping interval in milliseconds -- i.e., tick counts.
	/// </summary>
	/// <returns>The ping interval in milliseconds.</returns>
	DWORD GetPingInterval() const;

	/// <summary>
	/// Returns the patron interval.
	/// </summary>
	/// <returns>The patron interval in milliseconds.</returns>
	DWORD GetPatronInterval() const;

private:
	static const bool DEFAULT_HAS_CONFIG_FILE = false;
	static const bool DEFAULT_ACTIVE = false;
	static const bool DEFAULT_DNS_LOOKUP = true;
	static const LPCTSTR DEFAULT_ENDPOINT1;
	static const LPCTSTR DEFAULT_ENDPOINT2;

	static const DWORD PATRON_PING_INTERVAL_DELTA = 5000;	// 5 second diff between patron and ping intervals

	bool m_hasConfigFile;
	bool m_active;
	bool m_dnsLookup;
	std::vector<std::string> m_endpoints;
	bool m_haveDefaultEndpoints;
	DWORD m_pingInterval;
	DWORD m_patronInterval;

	void SetPatronInterval();
};
