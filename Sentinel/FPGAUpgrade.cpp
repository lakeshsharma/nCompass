#include "stdafx.h"
#include "FPGAUpgrade.h"

CFPGAUpgrade::CFPGAUpgrade( )
{
}

CFPGAUpgrade::CFPGAUpgrade( const std::string& drive, byte *xmlData, FPGAUpgradeLocationType type )
{
    UNREFERENCED_PARAMETER(&drive); 
    UNREFERENCED_PARAMETER(&xmlData); 
    UNREFERENCED_PARAMETER(&type); 
}

CFPGAUpgrade::~CFPGAUpgrade(void)
{
}

FPGAUpgradeLocationType CFPGAUpgrade::GetLocationType()
{
	return m_locationType;
}
