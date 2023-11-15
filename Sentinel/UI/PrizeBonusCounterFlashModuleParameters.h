#pragma once

#include "UI/FlashModuleParameters.h"

class CPrizeBonusCounterFlashModuleParameters : public CFlashModuleParameters
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CPrizeBonusCounterFlashModuleParameters"/> class.
	/// </summary>
	/// <param name="slotmast">The slotmast.</param>
	/// <param name="slotNum">The slot number.</param>
	/// <param name="player">The player.</param>
	/// <param name="levelName">Level Name</param>	
	/// <param name="prizeDescription">Prize Description</param>	
	CPrizeBonusCounterFlashModuleParameters(long slotmast, long slotNum, CPlayer *player, 
		const std::string& levelName, const std::string& prizeDescription);

	/// <summary>
	/// Finalizes an instance of the <see cref="CPrizeBonusCounterFlashModuleParameters"/> class.
	/// </summary>
	virtual ~CPrizeBonusCounterFlashModuleParameters();
};
