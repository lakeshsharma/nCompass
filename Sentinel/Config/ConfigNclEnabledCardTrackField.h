#pragma once

#include "ConfigField.h"
#include "GlobalDefs.h"
#include "Utilities.h"

class CNclEnabledCardTrackField :
	public CConfigField
{
public:
	CNclEnabledCardTrackField(void);
	~CNclEnabledCardTrackField(void);

	void SetField(const byte buffer[], int length);
	bool GetNclEnabled();
	byte GetCardTrack();

private:
	byte m_nclEnabled;
	byte m_cardTrack;
};

