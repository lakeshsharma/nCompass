
#include "stdafx.h"
#include "VideoMixerConfigFile.h"
#include "DirectoryManager.h"

#include "Logging/LogString.h"
#include "Utilities.h"

using namespace std;


const std::string VideoMixerConfigFilename = "/VideoMixerConfig.txt";
const std::string VideoInputFormatName = "VideoInputFormat";
const std::string VideoInputAutoName = "VideoInputAutoConfig";
const std::string VideoInputModeName = "VideoInputMode";
const std::string VideoOutputAutoName = "VideoOutputAutoConfig";
const std::string VideoOutputModeName = "VideoOutputMode";
const std::string TouchScreenProtocolName = "TouchScreenProtocolName";

VideoMixerConfigFile::VideoMixerConfigFile(DWORD startupTickcount) : CConfigFile(startupTickcount)
{
    SetDefaults();
}

VideoMixerConfigFile::~VideoMixerConfigFile()
{
}

bool VideoMixerConfigFile::CheckExists()
{
    bool fileFound = false;

    // cycle thru the USB paths
    int usbIndex = 0;
    bool moreUSBPaths = true;
    while (!fileFound && moreUSBPaths)
    {
        std::string filePath = CDirectoryManager::Instance().USBPath(usbIndex);

        if (!filePath.empty())
        {
            fileFound = CUtilities::FileExists(filePath + VideoMixerConfigFilename);
        }
        else
        {
            moreUSBPaths = false;
        }
        usbIndex++;
    }

    return fileFound;
}


bool VideoMixerConfigFile::GetVideoInput(std::string &inputFormatType, bool &isInputAutoConfig, int &inputMode)
{
    inputFormatType = m_inputFormatType;
    isInputAutoConfig = m_inputAutoConfig;
    inputMode = m_inputMode;

    return m_inputModeSpecified;
}

bool VideoMixerConfigFile::GetVideoOutput(bool &isOutputAutoConfig, int &outputMode)
{
    isOutputAutoConfig = m_outputAutoConfig;
    outputMode = m_outputMode;

    return m_outputModeSpecified;
}

bool VideoMixerConfigFile::GetTouchProtocol(std::string &touchProtocolName)
{
    touchProtocolName = m_touchProtocolName;

    return !touchProtocolName.empty();
}

bool VideoMixerConfigFile::Start()
{
	return LoadConfigFile(VideoMixerConfigFilename, true);
}

void VideoMixerConfigFile::SetDefaults()
{
    m_inputModeSpecified = false;
    m_inputFormatType = "";
    m_inputAutoConfig = false;
    m_inputMode = 0;

    m_outputModeSpecified = false;
    m_outputAutoConfig = false;
    m_outputMode = 0;

    m_touchProtocolName = "";
}

bool VideoMixerConfigFile::InterpretNameValue(const std::string& name, const std::string& value)
{
	bool retVal = false;

	if (VideoInputFormatName == name)
	{
        m_inputModeSpecified = true;
		m_inputFormatType = value;
        retVal = true;
	}
    else if (VideoInputAutoName == name) 
    {
        m_inputModeSpecified = true;
        retVal = InterpretBool(value, m_inputAutoConfig);
    }
    else if (VideoInputModeName == name) 
    {
        m_inputModeSpecified = true;
		m_inputMode = stoi(value.c_str());
        retVal = true;
	}

    else if (VideoOutputAutoName == name) 
    {
        m_outputModeSpecified = true;
        retVal = InterpretBool(value, m_outputAutoConfig);
    }
    else if (VideoOutputModeName == name) 
    {
        m_outputModeSpecified = true;
		m_outputMode = stoi(value.c_str());
        retVal = true;
	}

    else if (TouchScreenProtocolName == name) 
    {
		m_touchProtocolName = value;
        retVal = true;
	}

	return retVal;
}

void VideoMixerConfigFile::LogConfigValues(LPCTSTR sourceDesc, bool valid)
{
	LPCTSTR statusDesc = valid ? _T("valid") : _T("invalid");
	LogString(_T("GENERAL"), _T("%s=%s %s=%s %s=%d %s=%s %s=%d %s=%s %s from %s"),
		static_cast<LPCTSTR>(VideoInputFormatName.c_str()),
		static_cast<LPCTSTR>(m_inputFormatType.c_str()),
		static_cast<LPCTSTR>(VideoInputAutoName.c_str()),
		m_inputAutoConfig ? "true" : "false",
        static_cast<LPCTSTR>(VideoInputModeName.c_str()),
        m_inputMode,
        static_cast<LPCTSTR>(VideoOutputAutoName.c_str()),
        m_outputAutoConfig ? "true" : "false",
        static_cast<LPCTSTR>(VideoOutputModeName.c_str()),
        m_outputMode,		
        static_cast<LPCTSTR>(TouchScreenProtocolName.c_str()),
		static_cast<LPCTSTR>(m_touchProtocolName.c_str()),
		static_cast<LPCTSTR>(statusDesc),
		static_cast<LPCTSTR>(sourceDesc));
}
