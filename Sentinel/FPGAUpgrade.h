#pragma once
#include "VideoMixerFPGAVersion.h"


typedef enum
{
	LocationNone,
	OSLocation,
	OSDownloadLocation,
	DownloadLocation,
	ExternalLocation,
} FPGAUpgradeLocationType;

typedef enum
{
	TypeNone,
	VideoMixerType,
	CarrierType,
} FPGAUpgradeType;

class CFPGAUpgrade
{
public:
	CFPGAUpgrade( );
	CFPGAUpgrade( const std::string& drive, byte *xmlData, FPGAUpgradeLocationType type );
	virtual ~CFPGAUpgrade(void);

protected:
	WORD m_fpgaFileCRC;
	std::string m_fpgaFilePath;
	std::string m_drive;
	FPGAUpgradeLocationType m_locationType;
	FPGAUpgradeType m_type;
	

public:
	virtual std::string GetCurrentVersionString() = 0;
	virtual std::string GetUpgradeVersionString() = 0;
	virtual bool IsCurrent() = 0;
	virtual bool Upgrade() = 0;
	FPGAUpgradeLocationType GetLocationType();
	
};
