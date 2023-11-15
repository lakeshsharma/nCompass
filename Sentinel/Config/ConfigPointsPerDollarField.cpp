#include "stdafx.h"

#include "ConfigPointsPerDollarField.h"

CConfigPointsPerDollarField::CConfigPointsPerDollarField(void) :
	CConfigField(POINTS_PER_DOLLAR),
	m_pointsPerDollar(0)
{
}

CConfigPointsPerDollarField::~CConfigPointsPerDollarField(void)
{
}

void CConfigPointsPerDollarField::SetPointsPerDollar(const byte * pointsPerDollar, int length)
{
	CUtilities::MemCopyZero(&m_pointsPerDollar, sizeof(m_pointsPerDollar), pointsPerDollar, length);
}

WORD CConfigPointsPerDollarField::GetPointsPerDollar() const
{
	return m_pointsPerDollar;
}


