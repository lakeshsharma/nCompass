#include "stdafx.h"
#include "ConfigPointsForPromoField.h"
#include "Utilities.h"

CConfigPointsForPromoField::CConfigPointsForPromoField(void) :
	CConfigField(POINTS_FOR_PROMO),
	m_accruePointsForNCEP(false),
	m_accrueCompForNCEP(false)
{
}

CConfigPointsForPromoField::~CConfigPointsForPromoField(void)
{
}

void CConfigPointsForPromoField::SetAccruePointsForPromo(const byte * buffer, int length)
{
	size_t bufferSize = 0 <= length ? (size_t)length : 0;
	size_t bufferOffset = 0;
	bool ok = true;

	CUtilities::MemCopyZero_FromOffset(&ok, &m_accruePointsForNCEP, sizeof(m_accruePointsForNCEP),
		buffer, bufferSize, &bufferOffset);

	CUtilities::MemCopyZero_FromOffset(&ok, &m_accrueCompForNCEP, sizeof(m_accrueCompForNCEP),
		buffer, bufferSize, &bufferOffset);
}

BOOL CConfigPointsForPromoField::GetAccruePointsForNCEP() const
{
	return m_accruePointsForNCEP == TRUE;
}

BOOL CConfigPointsForPromoField::GetAccrueCompForNCEP() const
{
	return m_accrueCompForNCEP == TRUE;
}


