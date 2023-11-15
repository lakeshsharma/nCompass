#include "stdafx.h"
#include "VideoModeConfig.h"

// Sentinel includes.
#include "Utilities.h"

using namespace std;

const string CVideoModeConfig::AutoTypeString = "Auto";
const string CVideoModeConfig::AnalogTypeString = "Analog";
const string CVideoModeConfig::DVITypeString = "DVI";
const string CVideoModeConfig::DisplayPortTypeString = "DisplayPort";
const string CVideoModeConfig::NotConnectedTypeString = "NotConnected";
const string CVideoModeConfig::ErrorTypeString = "Error";
const string CVideoModeConfig::UnknownTypeString = "Unknown";

const LPCTSTR VideoMixerConfigFilename = "VideoMixerConfig.txt";

// Constructor
CVideoModeConfig::CVideoModeConfig(void)
{
	SetDefaults();
}

// Input config specific constructor
CVideoModeConfig::CVideoModeConfig(const std::string& signalTypeString,  bool inputModeIsAuto, int selectedInputModeIndex)
{
	SetDefaults();

    m_signalType = ConvertStringToSignalType(signalTypeString);
	
	m_inputModeIsAuto = inputModeIsAuto;
	m_selectedInputModeIndex = selectedInputModeIndex;
	m_changeStatus = VIDEO_CHANGE_INPUT_ENTERED;
}


// Output config specific constructor
CVideoModeConfig::CVideoModeConfig(bool outputModeIsAuto, int selectedOutputModeIndex)
{
	SetDefaults();
	m_outputModeIsAuto = outputModeIsAuto;
	m_selectedOutputModeIndex = selectedOutputModeIndex;
	m_changeStatus = VIDEO_CHANGE_OUTPUT_ENTERED;
}

// Revert Config specific constructor
CVideoModeConfig::CVideoModeConfig(VideoModeSignalType signalType, bool inputModeIsAuto, int selectedInputModeIndex, bool outputModeIsAuto, int selectedOutputModeIndex)
{
	SetDefaults();
	
	m_signalType = signalType;
	m_inputModeIsAuto = inputModeIsAuto;
	m_selectedInputModeIndex = selectedInputModeIndex;
	m_outputModeIsAuto = outputModeIsAuto;
	m_selectedOutputModeIndex = selectedOutputModeIndex;
	m_changeStatus = VIDEO_CHANGE_RESTARTING_UI;
}

// Offsets config specific constructor
CVideoModeConfig::CVideoModeConfig(int egmHorizontalOffset, int egmVerticalOffset, int monitorHorizontalOffset, int monitorVerticalOffset)
{
	SetDefaults();
	m_egmHorizontalOffset = egmHorizontalOffset;
	m_egmVerticalOffset = egmVerticalOffset;
	m_monitorHorizontalOffset = monitorHorizontalOffset;
	m_monitorVerticalOffset = monitorVerticalOffset;
	m_changeStatus = VIDEO_CHANGE_OFFSETS_ENTERED;
}

// Copy constructor
CVideoModeConfig::CVideoModeConfig(const CVideoModeConfig &rhs)
{
	*this = rhs;
}

// CVideoModeConfig class assignment operator
CVideoModeConfig & CVideoModeConfig::operator=(const CVideoModeConfig &rhs)
{
	m_signalType = rhs.m_signalType;
	m_inputModeIsAuto = rhs.m_inputModeIsAuto;
	m_outputModeIsAuto = rhs.m_outputModeIsAuto;
	m_selectedInputModeIndex = rhs.m_selectedInputModeIndex;
	m_selectedOutputModeIndex = rhs.m_selectedOutputModeIndex;
	m_egmHorizontalOffset = rhs.m_egmHorizontalOffset;
	m_egmVerticalOffset = rhs.m_egmVerticalOffset;
	m_monitorHorizontalOffset = rhs.m_monitorHorizontalOffset;
	m_monitorVerticalOffset = rhs.m_monitorVerticalOffset;
	m_changeStatus = rhs.m_changeStatus;

	return *this;
}

// Destructor
CVideoModeConfig::~CVideoModeConfig(void)
{
}

// Called by cosntructors to set the defualt values
void CVideoModeConfig::SetDefaults()
{
	m_signalType = VIDEO_MODE_SIGNAL_AUTO;

	m_inputModeIsAuto = true;
	m_outputModeIsAuto = true;;

	m_selectedInputModeIndex = INVALID_VIDEO_MODE_INDEX;
	m_selectedOutputModeIndex = INVALID_VIDEO_MODE_INDEX;

	m_egmHorizontalOffset = 0;
	m_egmVerticalOffset = 0;
	m_monitorHorizontalOffset = 0;
	m_monitorVerticalOffset = 0;

	m_changeStatus = VIDEO_CHANGE_NONE;
}
	
bool CVideoModeConfig::IsInputConfig()
{
	return (m_changeStatus == VIDEO_CHANGE_INPUT_ENTERED);
}

bool CVideoModeConfig::IsOutputConfig()
{
	return (m_changeStatus == VIDEO_CHANGE_OUTPUT_ENTERED);
}

bool CVideoModeConfig::IsOffsetConfig()
{
	return (m_changeStatus == VIDEO_CHANGE_OFFSETS_ENTERED);
}

VideoModeSignalType CVideoModeConfig::GetInputSignalType()
{
	return m_signalType;
}

bool CVideoModeConfig::IsAutoInputMode()
{
	return m_inputModeIsAuto;
}

int CVideoModeConfig::GetInputMode()
{
	return m_selectedInputModeIndex;
}

bool CVideoModeConfig::IsAutoOutputMode()
{
	return m_outputModeIsAuto;
}

int CVideoModeConfig::GetOutputMode()
{
	return m_selectedOutputModeIndex;
}

void CVideoModeConfig::GetOffsets(int &egmHorizontalOffset, int &egmVerticalOffset, int &monitorHorizontalOffset, int &monitorVerticalOffset)
{
	egmHorizontalOffset = m_egmHorizontalOffset;
	egmVerticalOffset = m_egmVerticalOffset;
	monitorHorizontalOffset = m_monitorHorizontalOffset;
	monitorVerticalOffset = m_monitorVerticalOffset;
}


VideoConfigChangeStatus CVideoModeConfig::UIRestarted()
{
	if (m_changeStatus == VIDEO_CHANGE_RESTARTING_UI)
	{
		m_revertTick = CUtilities::GetTickCount();
		m_changeStatus = VIDEO_CHANGE_REVERT_TIMER;
	}
	
	return m_changeStatus;
}

bool CVideoModeConfig::HasRevertTimeExpired()
{
	bool revertTimeExpired = false;
	
	const DWORD RevertTime = 90000;
	
	if (m_changeStatus == VIDEO_CHANGE_REVERT_TIMER && 
		CUtilities::GetTickCount() - m_revertTick > RevertTime)
	{
		revertTimeExpired = true;
	}
	
	return revertTimeExpired;
}

VideoModeSignalType CVideoModeConfig::ConvertStringToSignalType(const std::string& signalTypeString)
{
    VideoModeSignalType signalType;
	
	if (signalTypeString == AutoTypeString)
	{
		signalType = VIDEO_MODE_SIGNAL_AUTO;
	}
	else if (signalTypeString == AnalogTypeString)
	{
		signalType = VIDEO_MODE_SIGNAL_ANALOG;
	}
	else if (signalTypeString == DVITypeString)
	{
		signalType = VIDEO_MODE_SIGNAL_DVI;
	}
	else if (signalTypeString == DisplayPortTypeString)
	{
		signalType = VIDEO_MODE_SIGNAL_DP;
	}
	else
	{
		signalType = VIDEO_MODE_SIGNAL_AUTO;
	}

    return signalType;
}
