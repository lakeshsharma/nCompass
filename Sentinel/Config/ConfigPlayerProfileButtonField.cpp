#include "stdafx.h"
#include "ConfigPlayerProfileButtonField.h"

CConfigPlayerProfileButtonField::CConfigPlayerProfileButtonField(void) : 
CConfigField(PLAYER_PROFILE_BUTTON),
m_displayPlayerProfileButton(false)
{	
}

CConfigPlayerProfileButtonField::~CConfigPlayerProfileButtonField(void)
{
}

void CConfigPlayerProfileButtonField::SetDisplayPlayerProfileButton(bool displayProfileButton)
{
	m_displayPlayerProfileButton = displayProfileButton;
}

bool CConfigPlayerProfileButtonField::DisplayPlayerProfileButton() const
{
	return m_displayPlayerProfileButton;
}


