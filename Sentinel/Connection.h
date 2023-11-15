#pragma once
#include "IPAddress.h"
#include "GlobalDefs.h"
#include "Utilities.h"
#include "InternalMsgContent.h"

class CConnection :
	public CInternalMsgContent<INT_MSG_CONTENT_CONNECTION>
{
public:
	CConnection(void);
public:
	~CConnection(void);
public:
	/// <summary>
	/// Sets the slot number.
	/// </summary>
	/// <param name="slotNumber">IN - The new value for the slot number.</param>
	void SetSlotNumber(long slotNumber);

	/// <summary>
	/// Gets the slot number.
	/// </summary>
	/// <returns> The slot number.</returns>
	long GetSlotNumber() const;

	///	<summary>
	/// Sets the slotmast ID.
	///	</summary>
	///	<param name="slotmastID">The slotmast ID.</param>
	void SetSlotmastID(long slotmastID);

	///	<summary>
	/// Returns the slotmast ID.
	///	</summary>
	/// <returns>The slotmast ID.</returns>
	long GetSlotmastID();

	///	<summary>
	/// Sets the host.
	///	</summary>
	///	<param name="strHost">The host.</param>
	void SetHost(const std::string& host);

	///	<summary>
	/// Sets the host.
	///	</summary>
	///	<param name="lpHost">The host.</param>
	void SetHost(char* lpHost);

	///	<summary>
	/// Sets the port.
	///	</summary>
	///	<param name="strPort">The port.</param>
	void SetPort(const std::string& port);

	///	<summary>
	/// Sets the port.
	///	</summary>
	///	<param name="lPort">The host.</param>
	void SetPort(long lPort);

	///	<summary>
	/// Returns the host.
	///	</summary>
	///	<returns">The host.</returns>
	std::string GetHost();

	///	<summary>
	/// Returns the port.
	///	</summary>
	///	<returns">The port.</returns>
	long GetPort();

	///	<summary>
	/// Sets my IP.
	///	</summary>
	///	<param name="myIP">My IP.</param>
	void SetMyIP(CIPAddress myIP);

	///	<summary>
	/// Returns my IP.
	///	</summary>
	///	<returns>My IP.</returns>
	CIPAddress GetMyIP(void);

	void SetSubnetMask(CIPAddress subnetMask);
	CIPAddress GetSubnetMask(void);

	void SetDefaultGateway(CIPAddress defaultGateway);
	CIPAddress GetDefaultGateway(void);

	void SetPreferredDNS(CIPAddress preferredDNS);
	CIPAddress GetPreferredDNS(void);

	void SetAlternateDNS(CIPAddress alternateDNS);
	CIPAddress GetAlternateDNS(void);

	void SetActualIP(CIPAddress actualIP);

	CIPAddress GetActualIP();

	void SetMacAddress(const std::string &macAddress);
	std::string GetMacAddress();

	void SetDHCP(bool autoDHCP);
	bool AutoDHCP(void);

	

private:
	long m_slotNumber;
	long m_slotmastID;
	std::string m_strHost;
	long m_lPort;
	CIPAddress m_myIP;
	CIPAddress m_subnetMask;
	CIPAddress m_defaultGateway;
	CIPAddress m_preferredDNS;
	CIPAddress m_alternateDNS;
	CIPAddress m_actualIP;
	bool m_autoDHCP;
	std::string m_macAddress;

};
