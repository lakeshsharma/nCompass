#include "stdafx.h"
#include "ButtonInfo.h"

using namespace std;

CButtonInfo::CButtonInfo(void)
{
	m_btnID = -1;
}

CButtonInfo::CButtonInfo(CButtonInfo &bRHS)
{
	*this = bRHS;
}

CButtonInfo &CButtonInfo::operator=( CButtonInfo &bRHS )
{
	m_btnID = bRHS.GetButtonID();
	m_rLocation = bRHS.GetLocation();
	m_strUpButton = bRHS.GetUpButtonFilename();
	m_strDownButton = bRHS.GetDownButtonFilename();

	return *this;
}

CButtonInfo::~CButtonInfo(void)
{
}

void CButtonInfo::SetButtonID(int btnID)
{
	m_btnID = btnID;
}
	
int	CButtonInfo::GetButtonID()
{
	return (m_btnID);
}
	
void CButtonInfo::SetLocation(const RECT& rLocation)
{
	m_rLocation = rLocation;
}
	
RECT CButtonInfo::GetLocation() const
{
	return (m_rLocation);
}
	
string CButtonInfo::GetUpButtonFilename() const
{
	return (m_strUpButton);
}
	
string CButtonInfo::GetDownButtonFilename() const
{
	return (m_strDownButton);
}
	
void CButtonInfo::SetImages(const string& lpszButtonUp, const string& lpszButtonDown)
{
	m_strUpButton = lpszButtonUp;
	m_strDownButton = lpszButtonDown;
}
