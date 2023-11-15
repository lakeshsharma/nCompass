#pragma once

class CIPItem
{

public:

	CIPItem()
    : m_IpAddress(""),
      m_failCount(0)
	{
	}

	CIPItem(const std::string &IPAddress)
    : m_IpAddress(IPAddress),
      m_failCount(0)
	{
	}

	~CIPItem()
	{
	}

	CIPItem (const std::string &IPAddress, int failCount)
    : m_IpAddress(IPAddress),
      m_failCount(failCount)
	{
	}

	std::string GetIP() const
	{
		return m_IpAddress;
	}

	int GetFailCount() const
	{
		return m_failCount;
	}

	void IncrementFailCount()
	{
		m_failCount++;
	}

	void ResetFailCount()
	{
		m_failCount = 0;
	}

private:
	std::string m_IpAddress;
	int m_failCount;

};	
