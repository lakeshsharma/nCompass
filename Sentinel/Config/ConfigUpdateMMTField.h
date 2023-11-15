#pragma once
#include "ConfigField.h"

class CConfigUpdateMMTField :
	public CConfigField
{
public:
	CConfigUpdateMMTField(void);
	~CConfigUpdateMMTField(void);

	void SetUpdateMMT(bool updateMMT);
	bool GetUpdateMMT() const;

private:
	bool m_updateMMT;
};


