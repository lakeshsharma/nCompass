#include "stdafx.h"
#include "PollerAuthorizationMessage.h"

CPollerAuthorizationMessage::CPollerAuthorizationMessage(const byte *response)
{
	if (response != nullptr)
	{
		int offset = 0;

		//length
		byte tempByte;
		memcpy(&tempByte, response + offset, sizeof(tempByte));
		offset += sizeof(tempByte);

		int idLength = tempByte > MAX_PBID_LENGTH ? MAX_PBID_LENGTH : tempByte;
		SetTransactionID(reinterpret_cast <const char*>(response + offset), idLength);
		offset += m_pbtIDLength;

		memcpy(&m_authorizationGranted, response + offset, sizeof(byte));
	}
	else
	{
		m_authorizationGranted = 0;
		m_pbtIDLength = 0;
		memset(m_pbtID, 0, MAX_PBID_LENGTH);
	}
}

CPollerAuthorizationMessage::~CPollerAuthorizationMessage(void)
{
}

bool CPollerAuthorizationMessage::IsAuthorized(void)
{
	if (m_authorizationGranted > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CPollerAuthorizationMessage::SetTransactionID(const char* pbID, int length)
{
	memcpy(m_pbtID, pbID, length > MAX_PBID_LENGTH ? MAX_PBID_LENGTH : length);
	m_pbtIDLength = length;
}

std::string CPollerAuthorizationMessage::GetTransactionID(void)
{
	std::string result(m_pbtID, (size_t)m_pbtIDLength);
	return result;
}
