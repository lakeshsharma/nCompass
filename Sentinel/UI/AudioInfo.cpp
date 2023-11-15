#include "stdafx.h"
#include "AudioInfo.h"

using namespace std;

CAudioInfo::CAudioInfo(void)
{
}

CAudioInfo::CAudioInfo(CAudioInfo &iRHS)
{
	*this = iRHS;
}

CAudioInfo &CAudioInfo::operator=( CAudioInfo &iRHS )
{
	m_AudioName = iRHS.GetAudioFilename();
	return *this;
}

CAudioInfo::~CAudioInfo(void)
{
}

string CAudioInfo::GetAudioFilename() const
{
	return (m_AudioName);
}
	
void CAudioInfo::SetAudioFilename(const string& lpszAudio)
{
	m_AudioName = lpszAudio;
}
