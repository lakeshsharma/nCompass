#include "stdafx.h"
#include "IPAddressArray.h"

const CIPAddress CIPAddressArray::m_defaultIPAddress;

CIPAddressArray::CIPAddressArray()
{
}

CIPAddressArray::~CIPAddressArray()
{
	RemoveAll();
}

CIPAddressArray::CIPAddressArray(const CIPAddressArray &rhs)
{
	*this = rhs;
}

CIPAddressArray& CIPAddressArray::operator=(const CIPAddressArray &rhs)
{
	if (this != &rhs)
	{
		RemoveAll();

		for (size_t index = 0; index < rhs.GetCount(); ++index)
		{
			CIPAddress* ipAddress(new CIPAddress(rhs.GetAt(index)));
			m_addresses.push_back(ipAddress);
		}
	}

	return *this;
}

size_t CIPAddressArray::GetCount() const
{
	return m_addresses.size();
}

const CIPAddress& CIPAddressArray::GetAt(size_t nIndex) const
{
	const CIPAddress* ipAddress(&m_defaultIPAddress);
	if (nIndex < m_addresses.size())
	{
		ipAddress = m_addresses.at(nIndex);
	}

	return *ipAddress;
}


bool CIPAddressArray::operator==(CIPAddressArray const &RHS) const
{
	bool isSame = (m_addresses.size() == RHS.GetCount());

	// Check if all elements in A are also in B.
	for (size_t index = 0; isSame && index < m_addresses.size(); index++)
	{
		bool found = false;

		for (size_t rhsIndex = 0; !found && rhsIndex < RHS.GetCount(); rhsIndex++)
		{
			if (GetAt(index) == RHS.GetAt(rhsIndex))
			{
				found = true;
			}
		}

		isSame = found;
	}

	// Check if all elements in B are also in A.
	for (size_t rhsIndex = 0; isSame && rhsIndex < RHS.GetCount(); rhsIndex++)
	{
		bool found = false;

		for (size_t index = 0; !found && index < m_addresses.size(); index++)
		{
			if (GetAt(index) == RHS.GetAt(rhsIndex))
			{
				found = true;
			}
		}

		isSame = found;
	}

	return isSame;
}

bool CIPAddressArray::operator!=(CIPAddressArray const &RHS) const
{
	return !(*this == RHS);
}


void CIPAddressArray::Add(const BYTE* address, int port)
{
	CIPAddress* pIP = new CIPAddress();
	pIP->SetIP(address);
	pIP->SetPort(port);
	m_addresses.push_back(pIP);
}

void CIPAddressArray::Add(LPCTSTR address, int port)
{
	CIPAddress* pIP = new CIPAddress();
	pIP->SetIP(address);
	pIP->SetPort(port);
	m_addresses.push_back(pIP);
}

void CIPAddressArray::Add(const CIPAddress &address)
{
	CIPAddress* pIP = new CIPAddress();
	*pIP = address;
	m_addresses.push_back(pIP);
}

void CIPAddressArray::SetPortAll(int port)
{
    for (auto item: m_addresses) 
    {
        item->SetPort(port);
    }
}

void CIPAddressArray::RemoveAt(size_t index)
{
	CIPAddress *address = m_addresses.at(index);

	m_addresses.erase(m_addresses.begin() + index);

	delete address;
}

void CIPAddressArray::RemoveAll()
{
	for (size_t index = 0; index < m_addresses.size(); index++)
	{
		delete m_addresses.at(index);
	}
	m_addresses.clear();
}
