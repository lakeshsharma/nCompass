#include "stdafx.h"
#include "ConfigPlayerTrackingField.h"
#include "Utilities.h"

CConfigPlayerTrackingField::CConfigPlayerTrackingField(void) :
	CConfigField(PLAYER_TRACKING),
	m_playerTrackingMultiplier(0),
	m_playerTrackingDivisor(0)
{
}

CConfigPlayerTrackingField::~CConfigPlayerTrackingField(void)
{
}

void CConfigPlayerTrackingField::SetPlayerTrackingOptions(const byte * buffer, int length)
{
	size_t bufferSize = 0 <= length ? (size_t)length : 0;
	size_t bufferOffset = 0;
	bool ok = true;

	CUtilities::MemCopyZero_FromOffset(&ok, &m_playerTrackingMultiplier, sizeof(m_playerTrackingMultiplier),
		buffer, bufferSize, &bufferOffset);
	CUtilities::MemCopyZero_FromOffset(&ok, &m_playerTrackingDivisor, sizeof(m_playerTrackingDivisor),
		buffer, bufferSize, &bufferOffset);
}

WORD CConfigPlayerTrackingField::GetPlayerTrackingMultiplier() const
{
	return m_playerTrackingMultiplier;
}

WORD CConfigPlayerTrackingField::GetPlayerTrackingDivisor() const
{
	return m_playerTrackingDivisor;
}


