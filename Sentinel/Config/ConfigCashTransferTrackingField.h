#pragma once
#include "ConfigField.h"

class CConfigCashTransferTrackingField :
	public CConfigField
{
public:
	CConfigCashTransferTrackingField(void);
public:
	~CConfigCashTransferTrackingField(void);

	void SetCashTransferTracking(bool cashTransferTracking)
	{
		m_cashTransferTracking = cashTransferTracking;
	}


	bool CashTransferTracking()
	{
		return m_cashTransferTracking;
	}

private:
	bool m_cashTransferTracking;
};


