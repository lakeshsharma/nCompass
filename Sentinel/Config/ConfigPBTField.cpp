#include "stdafx.h"

#include "ConfigPBTField.h"

CConfigPBTField::CConfigPBTField(void) :
	CConfigField(PBT),
	m_PromoEnabled(false),
	m_PointsEnabled(false),
	m_CashEnabled(false)
{
}

CConfigPBTField::~CConfigPBTField(void)
{
}

void CConfigPBTField::SetPBTOptions(const byte * buffer, int length)
{
	if (buffer == NULL)
	{
		length = 0;
	}
	length = max(0, length);

	int offset = 0;
	if (length >= (int)(sizeof(m_PromoEnabled) + sizeof(m_PointsEnabled) + sizeof(m_CashEnabled)))
	{
		m_PromoEnabled = buffer[offset++] == TRUE;
		m_PointsEnabled = buffer[offset++] == TRUE;
		m_CashEnabled = buffer[offset++] == TRUE;
	}
}

void CConfigPBTField::SetPBTOptionsBuffer( byte *buffer, int length, bool bPBTEnabled, bool bPointsEnabled, bool bCashEnabled )
{
	if (buffer == NULL)
	{
		length = 0;
	}
	length = max(0, length);

	int offset = 0;
	if (length >= (int)(sizeof(bPBTEnabled) + sizeof(bPointsEnabled) + sizeof(bCashEnabled)))
	{
		buffer[offset++] = bPBTEnabled ? 1 : 0;
		buffer[offset++] = bPointsEnabled ? 1 : 0;
		buffer[offset++] = bCashEnabled ? 1 :0;
	}
}

bool CConfigPBTField::PromoEnabled() const
{
	return m_PromoEnabled;
}

bool CConfigPBTField::PointsEnabled() const
{
	return m_PointsEnabled;
}

bool CConfigPBTField::CashEnabled() const
{
	return m_CashEnabled;
}


