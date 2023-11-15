#pragma once
#include "Hardware/MemoryStorage.h"
#include "Config/EEPROMConfig.h"

class CDisplayManagerConfig:
	public CMemoryStorage
{
public:
	CDisplayManagerConfig(void);
public:
	~CDisplayManagerConfig(void);

	bool GetMonitorResolution(SIZE& resolution) const;
	void SetMonitorResolution(SIZE const &resolution);

private:
	bool m_monitorResolutionValid;
	SIZE m_monitorResolution;

	void BuildYourself();
};

