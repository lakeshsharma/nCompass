#include "stdafx.h"

#include "IPAddress.h"

CIPAddress::CIPAddress(void)
{
	for (int i = 0; i < IP_ADDRESS_SIZE; i++)
	{
		m_Address[i] = 0;
	}

	m_port = 0;
}

CIPAddress::~CIPAddress(void)
{
}

CIPAddress::CIPAddress( CIPAddress const &RHS )
{
	*this = RHS;
}

CIPAddress & CIPAddress::operator=( CIPAddress const &RHS )
{
	RHS.GetIP(m_Address, IP_ADDRESS_SIZE);
	m_port = RHS.GetPort();
	return *this;
}

bool CIPAddress::operator==( CIPAddress const &RHS ) const
{
	bool isSame = true;

	for (int i = 0; i < IP_ADDRESS_SIZE && isSame; i++)
	{
		if (m_Address[i] != RHS.m_Address[i])
		{
			isSame = false;
		}
	}

	if (m_port != RHS.GetPort())
	{
		isSame = false;
	}

	return isSame;
}

bool CIPAddress::operator!=( CIPAddress const &RHS ) const
{
	if (*this == RHS)
		return false;
	return true;
}

void CIPAddress::SetIP(const std::string& strAddress)
{
	unsigned int uintAddress[IP_ADDRESS_SIZE];
	int matchCount;

	ZeroMemory(m_Address, sizeof(m_Address));

	if (!strAddress.empty())
	{
		matchCount = sscanf(strAddress.c_str(), "%u.%u.%u.%u",
			uintAddress + MSB_ADDRESS_INDEX,
			uintAddress + NMSB_ADDRESS_INDEX,
			uintAddress + NLSB_ADDRESS_INDEX,
			uintAddress + LSB_ADDRESS_INDEX);

		if (matchCount == IP_ADDRESS_SIZE)
		{
			// Verify that all of the parsed values are in range.
			bool addressBytesOk(true);
			for (int addressIndex(0); addressBytesOk && addressIndex < (int)_countof(uintAddress); ++addressIndex)
			{
				if (uintAddress[addressIndex] > BYTE_MAX)
				{
					addressBytesOk = false;
				}
			}

			// If all of the parsed values are in range then update the member address.
			if (addressBytesOk)
			{
				for (int addressIndex(0); addressIndex < (int)_countof(m_Address) && addressIndex < (int)_countof(uintAddress); ++addressIndex)
				{
					m_Address[addressIndex] = (BYTE)uintAddress[addressIndex];
				}
			}
		}
	}

}
void CIPAddress::SetIP(const BYTE* lpAddress)
{
	for (int i = 0; i < IP_ADDRESS_SIZE; i++)
	{
		m_Address[i] = lpAddress[i];
	}
}

/// <summary>
/// Gets the ip.
/// </summary>
/// <param name="strAddress">The string address.</param>
void CIPAddress::GetIP(std::string &strAddress) const
{
	strAddress = FormatString(_T("%d.%d.%d.%d"),m_Address[0], m_Address[1], m_Address[2], m_Address[3]); 
}

// "length" is the number of characters (that can be overwritten) in the "lpAddress" buffer.
void CIPAddress::GetIP(char* lpAddress, size_t length) const
{
	snprintf(lpAddress, length, "%d.%d.%d.%d", m_Address[0], m_Address[1], m_Address[2], m_Address[3]);
}

std::string CIPAddress::GetIP() const
{
	char buffer[IPADDRESS_STR_LENGTH];
	GetIP(buffer, _countof(buffer));
	return std::string(buffer);
}

void CIPAddress::GetIP(BYTE* lpAddress, int length) const
{
	if (length >= IP_ADDRESS_SIZE)
	{
		for (int i = 0; i < IP_ADDRESS_SIZE; i++)
		{
			 lpAddress[i] = m_Address[i];
		}
	}
}

BYTE CIPAddress::GetIPAddressPrefix(void) const
{
	return m_Address[0];
}

void CIPAddress::SetPort(int port)
{
	m_port = port;
}

long CIPAddress::GetPort() const
{
	return m_port;
}

bool CIPAddress::IsEmpty() const
{
	CIPAddress emptyAddress;

	return (*this == emptyAddress);
}
