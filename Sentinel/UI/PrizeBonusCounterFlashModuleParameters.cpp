#include "stdafx.h"
#include "Utilities.h"
#include "PrizeBonusCounterFlashModuleParameters.h"

CPrizeBonusCounterFlashModuleParameters::CPrizeBonusCounterFlashModuleParameters(long slotmast, long slotNum, 
	CPlayer *player, const string& levelName, const string& prizeDescription) : CFlashModuleParameters(slotmast, slotNum, player)
{
	SetContentSpecificParameters(FormatString("LevelName=%s&PrizeDescription=%s", levelName.c_str(), prizeDescription.c_str()));
}

CPrizeBonusCounterFlashModuleParameters::~CPrizeBonusCounterFlashModuleParameters()
{
}
