#pragma once

#include "IPAddress.h"

class CIPAddressArray
{
public:
	/// <summary>
	/// Constructor. Initializes a new instance of the <see cref="CIPAddressArray"/> class.
	/// </summary>
	CIPAddressArray();

	/// <summary>
	/// Deconstructor. Finalizes an instance of the <see cref="CIPAddressArray"/> class.
	/// </summary>
	virtual ~CIPAddressArray();

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CIPAddressArray"/> class.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	CIPAddressArray(const CIPAddressArray &rhs);

	/// <summary>
	/// Assigment operator.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	/// <returns>A reference to the target of the assignment.</returns>
	CIPAddressArray& operator=(const CIPAddressArray &rhs);

	/// <summary>
	/// Returns the number of IP addresses.
	/// </summary>
	/// <returns>Number of IP addresses.</returns>
	size_t GetCount() const;

	/// <summary>
	/// Return an IP address for the given index.
	/// </summary>
	/// <param name="nIndex">IN - Array index of the IP address</param>
	/// <returns>A read-only reference to the IP address for the specified index.</returns>
	/// <remarks>
	/// Returns a reference to a default CIPAddress instance if nIndex is out of range.
	/// </remarks>
	const CIPAddress& GetAt(size_t nIndex) const;

	/// <summary>
	/// Equality operator. Compares two address arrays
	/// </summary>
	/// <param name="RHS">IN - The CIPAddressArray to be compared</param>
	/// <returns>returns true if both lists have the same items. They can be sorted differently and still be equal.</returns>
	bool operator==(CIPAddressArray const &RHS) const;

	/// <summary>
	/// Inequality operator. Compares two address arrays
	/// </summary>
	/// <param name="RHS">IN - The CIPAddressArray to be compared</param>
	/// <returns>true if both lists do not have the same items. They can be sorted differently and still be equal.</returns>
	bool operator!=(CIPAddressArray const &RHS) const;

	/// <summary>
	/// Adds the specified IP address to the instance.
	/// </summary>
	/// <param name="address">IN - IP address in the form of a 4 byte array.</param>
	/// <param name="port">IN - The port.</param>
	void Add(const BYTE* address, int port = 0);

	/// <summary>
	/// Add a given IP address.
	/// </summary>
	/// <param name="address">IN - IP address in string form.</param>
	/// <param name="port">IN - The port.</param>
	void Add(LPCTSTR address, int port = 0);

	/// <summary>
	/// Add a given ip address object.
	/// </summary>
	/// <param name="address">IN - IP address object.</param>
	void Add(const CIPAddress &address);
		
	/// <summary>
	/// Sets the port for all addresses.
	/// </summary>
	/// <param name="port">The port.</param>
	void SetPortAll(int port);

	/// <summary>
	/// Remove the IP addresses at the given index.
	/// </summary>
	/// <param name="nIndex">IN - Array index of the IP address.</param>
	void RemoveAt(size_t nIndex);

	/// <summary>
	/// Remove all IP addresses.
	/// </summary>
	void RemoveAll();

private:
	static const CIPAddress m_defaultIPAddress;

	std::vector<CIPAddress *> m_addresses;
};

