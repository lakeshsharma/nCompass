#pragma once
#include "ConfigField.h"
#include "Utilities.h"

class CConfigLossLimitField :
	public CConfigField
{
public:
	CConfigLossLimitField(void);
public:
	~CConfigLossLimitField(void);

public:

	void SetLossLimitOptions(byte* buffer, int length)
	{
		//_ASSERT(0 <= length);
		size_t bufferSize = 0 <= length ? (size_t)length : 0;
		size_t bufferOffset = 0;
		bool ok = true;
	
		byte active;
		CUtilities::MemCopyZero_FromOffset(&ok, &active, sizeof active,
			buffer, bufferSize, &bufferOffset);
		m_lossLimitActive = (bool)active;

		CUtilities::MemCopyZero_FromOffset(&ok, &m_lossLimitSessionNumber, sizeof(m_lossLimitSessionNumber),
			buffer, bufferSize, &bufferOffset);
		CUtilities::MemCopyZero_FromOffset(&ok, &m_lossLimitSessionAmount, sizeof(m_lossLimitSessionAmount),
			buffer, bufferSize, &bufferOffset);
		CUtilities::MemCopyZero_FromOffset(&ok, &m_lossLimitBillValidatorOffAmount, sizeof(m_lossLimitBillValidatorOffAmount),
			buffer, bufferSize, &bufferOffset);
	}

	bool GetLossLimitActive()
	{
		return m_lossLimitActive;
	}

	DWORD GetLossLimitSessionNumber()
	{
		return m_lossLimitSessionNumber;
	}

	WORD GetLossLimitSessionAmount()
	{
		return m_lossLimitSessionAmount;
	}

	WORD GetLossLimitBillValidatorOffAmount()
	{
		return m_lossLimitBillValidatorOffAmount;
	}

private:

	bool m_lossLimitActive;
	DWORD m_lossLimitSessionNumber;
	WORD m_lossLimitSessionAmount;
	WORD m_lossLimitBillValidatorOffAmount;
};


